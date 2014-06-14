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

#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <iostream>
#include <cassert>

// macros for printing messages of various types
#define ERRORM(x) std::cerr << "ERROR: " << x << std::endl
#define WARNM(x) std::cerr << "WARNING: " << x << std::endl
#define INFOM(x) //std::cerr << "INFO: " << x << std::endl
#define DBGM(x)// std::cerr << x << std::endl

#endif
