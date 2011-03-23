/**
 * \file modulelinker.h
 * This file defines and implement the ModuleLinker related class and methods.
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

#ifndef MODULELINKER_H
#define MODULELINKER_H

#include <string>

// External forward declaration
namespace llvm
{
    class Linker;
    class Module;
}

namespace shine
{

// Internal forward declaration
class ModuleLoader;

/**
 * This module is responsible by the linkage of multiple
 * llvm modules into a composite one, it uses the LLVM Linker.
 */
class ModuleLinker
{
// Ctor & Dtor
public:
    /**
     * Creates a new ModuleLinker.
     *
     * \param prog_name The program name.
     * \param module_name The composite module name.
     */
    ModuleLinker(const std::string &prog_name,
                 const std::string &module_name);

    virtual ~ModuleLinker();

// Not implemented copy/assign
private:
    ModuleLinker(const ModuleLinker&);
    ModuleLinker& operator=(const ModuleLinker&);

// Private attributes
private:
    /**
     * This is the internal LLVM Linker
     */
    llvm::Linker *mInternalLinker;

// Public interface
public:
    /**
     * Link a ModuleLoader into the composite module. Please note
     * that the ModuleLoader passed will have its internal llvm
     * module destroyed, so the ModuleLoader shouldn't be used
     * anymore.
     *
     * \param module_loader The module to link.
     * \param error_string The error message in case of error.
     * \return true for ok, false for error.
     */
    bool link_module_loader(ModuleLoader *module_loader,
                            std::string &error_string);

    /**
     * Returns the internal linker composite module. You can't
     * own this module, see /release_module()/ for reference.
     *
     * \return Return the composite module.
     */
    llvm::Module *get_composite_module();

    /**
     * This method releases the composite Module into which
     * linking is being done. Ownership of the composite Module
     * is transferred to the caller who must arrange for its destruct.
     *
     * \return Linker composite module.
     */
    llvm::Module *release_module();

};

} // namespace shine

#endif // MODULELINKER_H
