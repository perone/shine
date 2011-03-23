/**
 * \file shine.h
 * This file includes all the Shine framework related headers.
 */

/*
 * Shine - The Symbolic Regression Machine
 *
 * Copyright (C) 2011 Christian S. Perone
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SHINE_H
#define SHINE_H

#include "moduleloader.h"
#include "modulelinker.h"
#include "modulehandler.h"
#include "astnode.h"

namespace shine
{

/**
 * Perform the Shine library initialization as well LLVM
 * JIT initialization.
 */
void shine_initialize(void);

/**
 * Perform the Shine library shutdown as well LLVM shutdown.
 */
void shine_shutdown(void);

}

#endif // SHINE_H
