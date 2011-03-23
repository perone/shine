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

#include "shine.h"

#include <iostream>
#include <llvm/Support/ManagedStatic.h>

using namespace shine;

int main(void)
{
    std::string error_string;
    shine_initialize();

    ModuleLoader *loader =
            ModuleLoader::create_from_file("mod1.o", error_string);

    assert(loader!=NULL);
    assert(loader->get_internal_module()!=NULL);

    delete loader;

    shine_shutdown();
    return 0;
}
