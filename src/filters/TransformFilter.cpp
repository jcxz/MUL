/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of MUL.
 *
 * MUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MUL. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TransformFilter.h"
#include "opencl/qclcontext.h"
#include "debug.h"



void TransformFilter::invertMatrix(float *mat)
{
  double D = mat[0] * mat[4] - mat[1] * mat[3];
  D = D != 0 ? 1./D : 0;

  double A11 = mat[4] * D, A22 = mat[0] * D;
  mat[0] = A11; mat[1] *= -D;
  mat[3] *= -D; mat[4] = A22;

  double b1 = -mat[0] * mat[2] - mat[1] * mat[5];
  double b2 = -mat[3] * mat[2] - mat[4] * mat[5];
  mat[2] = b1; mat[5] = b2;
}


bool TransformFilter::init(void)
{
  // skip OpenCL initialization in case context is not given
  if (m_ctx == nullptr) return (m_good = true);

  // compile program
  m_program = m_ctx->buildProgramFromSourceFile(CLSRCFILE("transform.cl"));
  if (m_program.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create kernel
  m_kernel = m_program.createKernel("transform");
  if (m_kernel.isNull())
  {
    ERRORM("Failed to create OpenCL program: " << m_program.log().toStdString());
    return false;
  }

  // create a memory object for the matrix
  m_matrix_ocl = m_ctx->createBufferCopy(m_matrix, sizeof(m_matrix), QCLMemoryObject::ReadOnly);
  if (m_matrix_ocl.isNull())
  {
    ERRORM("Failed to create matrix buffer");
    return false;
  }

  m_kernel.setArg(2, m_matrix_ocl);

  return (m_good = true);
}


bool TransformFilter::runCL(const QCLImage2D & src, int w, int h, QCLImage2D & dst)
{
  if (!isGood()) return false;

  m_kernel.setArg(0, src);
  m_kernel.setArg(1, dst);
  m_kernel.setGlobalWorkSize(w, h);

  if (m_matrix_changed)
  {
    m_matrix_ocl.write(m_matrix, sizeof(m_matrix));
    m_matrix_changed = false;
  }

  m_kernel.run();

  return true;
}
