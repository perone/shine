/**
 * \file modulehandler.h
 * This file defines and implement the ModuleHandler related class and methods.
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

#ifndef MODULEHANDLER_H
#define MODULEHANDLER_H

#include <string>
#include <vector>
#include <map>
#include <tr1/unordered_map>

#include <llvm/PassManager.h>

namespace tr1impl = std::tr1;

// External forward declaration
namespace llvm
{
    class Linker;
    class ExecutionEngine;
    class Value;
}

namespace shine
{

class ModuleLinker;
class ASTNode;

/**
 * This class takes the ModuleLinker ownership and perform
 * optimizations, analysis, and some other utility operations.
 */
class ModuleHandler
{
// Ctor & Dtor
public:
    /**
     * This constructor *always* takes the ownership of the
     * module passed by referece, you shouldn't delete it by
     * yourself. Use create methods instead of this constructor.
     *
     * \param module The LLVM Module.
     * \param execution_engine The LLVM Execution Engine (JIT).
     * \param pass_manager The LLVM Pass Manager.
     */
    ModuleHandler(llvm::Module *module,
                  llvm::ExecutionEngine *execution_engine,
                  llvm::PassManager *pass_manager,
                  llvm::FunctionPassManager *func_pass_manager);
    virtual ~ModuleHandler();

// Not implemented copy/assign
private:
    ModuleHandler(const ModuleHandler&);
    ModuleHandler& operator=(const ModuleHandler&);

// Public interface
public:
    /**
     * This is the creator method for creating ModuleHandler instances,
     * use this method instead of the constructor.
     *
     * \param module The LLVM module (this is typically from ModuleLinker).
     * \param error_string Error message in case of error.
     * \param pass_manager This is optional, just if you want to provide
     *                     your customized LLVM Pass Manager.
     * \param func_pass_manager This is optional, if you do not want to provide a
     *                          Function Pass Manager, ModuleHandler will create
     *                          it for you.
     * \return A new ModuleHandler instance in case of success, otherwise
     *         NULL and the error message on the error_string parameter.
     */
    static ModuleHandler *create(llvm::Module *module,
                                 std::string &error_string,
                                 llvm::PassManager *pass_manager=NULL,
                                 llvm::FunctionPassManager *func_pass_manager=NULL);

    /**
     * Run the optimization passes into the composite
     * linker module.
     *
     * \return true if module was modified, false otherwise.
     */
    bool run_module_passes();

    /**
     * Run the function optimization passes into the specified
     * function.
     *
     * \param func_name The function name.
     * \return true if Function was modified, false otherwise.
     */
    bool run_function_passes(const std::string &func_name);

    /**
     * This method will dump the function LLVM IR code to a
     * string.
     *
     * \param func_name The function name.
     * \return The LLVM IR of the function.
     */
    std::string get_function_ir(const std::string &func_name);

    /**
     * This method will dump the internal composite module contents
     * to the console.
     */
    void print_module();

    /**
     * Dump the module IR into the specified stream.
     *
     * \param stream The output stream.
     */
    void print_module(std::ostream &stream);

    /**
     * This method will generate LLVM IR code for your AST tree.
     *
     * \param ast_nodes Your AST Tree.
     * \param func_name The function name.
     */
    void codegen_ast(const std::vector<ASTNode*> *ast_nodes,
                     const std::string &func_name);

    /**
     * JITs the function (func_name) and then return a function
     * pointer to that function.
     *
     * \param func_name The function name.
     * \return The function pointer of the JITed function.
     */
    void *jit_function(const std::string &func_name);

    /**
     * Sets the variable list used in your AST.
     *
     * \param var_list Variable list.
     */
    void set_variable_list(const std::vector<std::string> &var_list)
    {
        assert(var_list.size()>0);
        mVariableList = var_list;
    }

    /**
     * Returns the variable list used for the AST.
     *
     * \return Variable list.
     */
    std::vector<std::string> get_variable_list(void)
    { return mVariableList; }

    /**
     * This method will free memory from all JITed functions.
     *
     * \return true if at least one function was free'd, otherwise false.
     */
    bool free_jit_memory(void);

    /**
     * This method will free memory from the specified JIT function.
     *
     * \param func_name The name of the function JITed.
     * \return true if the function was found and free'd, false otherwise.
     */
    bool free_jit_memory(const std::string &func_name);

// Private interface
private:
    /**
     * This method is used to declare the function prototype inside
     * the module. Its used before creating an entry point.
     *
     * \param function_name The function name.
     * \param named_values The mapping of named values.
     * \return The new created function.
     */
    llvm::Function *declare_function(const std::string &function_name,
                                     std::map<std::string, llvm::Value*> &named_values);

private:
    /**
     * The internal Module Linker.
     */
    llvm::Module *mInternalModule;

    /**
     * The Pass manager.
     */
    llvm::PassManager *mPassManager;

    /**
     * The Function Pass Manager.
     */
    llvm::FunctionPassManager *mFunctionPassManager;

    /**
     * The AST Variable list.
     */
    std::vector<std::string> mVariableList;

    /**
     * The Execution Engine (JIT).
     */
    llvm::ExecutionEngine *mExecutionEngine;

    /**
     * This typedef declares a hash map from function name to LLVM Function pointer.
     */
    typedef tr1impl::unordered_map<std::string, llvm::Function*> JITFunctionMap;

    /**
     * The hash map from function name to LLVM Function pointer.
     */
    JITFunctionMap mJITFunctions;
};

}

#endif // MODULEHANDLER_H
