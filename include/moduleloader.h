/**
 * \file moduleloader.h
 * This file defines and implement the ModuleLoader related class and methods.
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

#ifndef MODULELOADER_H
#define MODULELOADER_H

#include <string>

namespace llvm
{

class Module;
class MemoryBuffer;

}

namespace shine
{

/**
 * ModuleLoader is responsible to load and handle
 * LLVM modules. Please note that this class takes
 * the ownership over the module.
 */
class ModuleLoader
{
// Ctor & Dtor
public:
    /**
     * This constructor takes the ownership over
     * the module.
     *
     * \param module An already constructed llvm module.
     */
    ModuleLoader(llvm::Module *module);
    virtual ~ModuleLoader();

// Not implemented copy/assign
private:
    ModuleLoader(const ModuleLoader&);
    ModuleLoader& operator=(const ModuleLoader&);

// Private attributes
private:
    /**
     * The internal module handler
     */
    llvm::Module *mInternalModule;

// Public interface
public:
    /**
     * Returns the internal module created by the
     * ModuleLoader.
     *
     * \return The internal LLVM Module.
     */
    llvm::Module *get_internal_module()
    { return mInternalModule; }

    /**
     * Checks if the entire module is a valid double closure
     * type needed for the system. This method will check
     * if every function arguments and return types are returning
     * double or not.
     *
     * \param error_string Closure errors found.
     * \return true if problems were found, false otherwise.
     */
    bool check_closure(std::string &error_string);

// Public static interface
public:
    /**
     * This method creates a new ModuleLoader instance from the
     * specified bitcode file, or if it fail, it will return NULL as
     * well the error message.
     *
     * \param filename The bitcode file.
     * \param error_string The error message in case of problems.
     * \return A new ModuleLoader instance, or NULL if error.
     */
    static ModuleLoader* create_from_file(const std::string &filename,
                                          std::string &error_string);

    /**
     * This method creates a new ModuleLoader instance from the
     * specified memory buffer, or if it fail, it will return NULL as
     * well the error message.
     *
     * \param memory_buffer The memory buffer.
     * \param error_string The error message in case of problems.
     * \return A new ModuleLoader instance, or NULL if error.
     */
    static ModuleLoader* create_from_memory_buffer(llvm::MemoryBuffer *memory_buffer,
                                                   std::string &error_string);
};

} // namespace shine

#endif // MODULELOADER_H
