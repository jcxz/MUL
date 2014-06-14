/*
 * Copyright (C) 2014 Pavel Csoka <xcsoka00@stud.fit.vutbr.cz>
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

#include "glrenderer.h"

GLRenderer::GLRenderer(QObject *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers))
{
    setMinimumSize(320,240);
    setAttribute(Qt::WA_DeleteOnClose);
}

void GLRenderer::renderPixmap(const QPixmap &_pixmap)
{
    pixmap = _pixmap;
    resizeEvent(NULL);
    this->update();
}

void GLRenderer::resizeEvent(QResizeEvent *)
{
    //retain aspect ratio when changing size
    float srcAspectRt = pixmap.width() / (float)pixmap.height();
    float dstAspectRt = this->width() / (float)this->height();
    float width;

    if (srcAspectRt > dstAspectRt)
        width = this->width();
    else
        width = this->height() * srcAspectRt;

    double scale = width / pixmap.width();

    scaleMat = QTransform().scale(scale, scale);
    scaleMatI = scaleMat.inverted();
}

void GLRenderer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setWorldTransform(scaleMat);
    QRect scaled = scaleMatI.mapRect(e->rect());
    p.drawPixmap(scaled, pixmap, scaled);
}
