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

#include "modulehandler.h"

#include "modulelinker.h"
#include "astnode.h"

#include <cassert>
#include <sstream>

#include <llvm/Module.h>
#include <llvm/Support/StandardPasses.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Constants.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Target/TargetData.h>

namespace shine
{
ModuleHandler::ModuleHandler(llvm::Module *module,
                             llvm::ExecutionEngine *execution_engine,
                             llvm::PassManager *pass_manager,
                             llvm::FunctionPassManager *func_pass_manager)
{
    assert(module && "No module provided !");
    assert(execution_engine && "No Execution Engine provided !");
    assert(pass_manager && "No Pass Manager provided !");
    assert(func_pass_manager && "No Function Pass Manager provided !");

    mInternalModule = module;
    mExecutionEngine = execution_engine;
    mPassManager = pass_manager;
    mFunctionPassManager = func_pass_manager;
}

ModuleHandler* ModuleHandler::create(llvm::Module *module,
                                     std::string &error_string,
                                     llvm::PassManager *pass_manager,
                                     llvm::FunctionPassManager *func_pass_manager)
{
    assert(module && "No module provided !");

    std::string i_error_string;

    llvm::ExecutionEngine *execution_engine =
        llvm::EngineBuilder(module)
            .setErrorStr(&i_error_string)
            .setEngineKind(llvm::EngineKind::JIT)
            .setOptLevel(llvm::CodeGenOpt::Default)
            .create();

    if(!execution_engine)
    {
        error_string = "Error while creating Execution Engine (JIT): [ " + i_error_string + " ]";
        return NULL;
    }

    llvm::PassManager *created_pass_manager = pass_manager;

    if(!pass_manager)
    {
        created_pass_manager = new llvm::PassManager();
        llvm::Pass *inlining_pass = llvm::createFunctionInliningPass();
        llvm::createStandardModulePasses(created_pass_manager,
                                         3, false, true, true, true,
                                         true, inlining_pass);
        llvm::createStandardLTOPasses(created_pass_manager, true,
                                      true, false);
    }

    llvm::FunctionPassManager *created_func_pass_manager = func_pass_manager;

    if(!func_pass_manager)
    {
        // FunctionPassManager doesn't takes the ownership of the Module.
        created_func_pass_manager = new llvm::FunctionPassManager(module);
        const llvm::TargetData *exec_engine_td = execution_engine->getTargetData();
        llvm::TargetData *target_data = new llvm::TargetData(*exec_engine_td);

        created_func_pass_manager->add(target_data);
        llvm::createStandardFunctionPasses(created_func_pass_manager, 3);

        created_func_pass_manager->add(llvm::createPromoteMemoryToRegisterPass());
        created_func_pass_manager->add(llvm::createInstructionCombiningPass());
        created_func_pass_manager->add(llvm::createDeadCodeEliminationPass());
        created_func_pass_manager->add(llvm::createReassociatePass());
        created_func_pass_manager->add(llvm::createGVNPass());
        created_func_pass_manager->add(llvm::createLICMPass());
        created_func_pass_manager->add(llvm::createDeadStoreEliminationPass());
    }

    return new ModuleHandler(module, execution_engine,
                             created_pass_manager,
                             created_func_pass_manager);
}

ModuleHandler::~ModuleHandler()
{
    // We already created an execution engine who has taken
    // the ownership of the module, so we just need to delete
    // the Execution Engine
    delete mExecutionEngine;
    delete mPassManager;
}


bool ModuleHandler::run_module_passes()
{
    const bool ret = mPassManager->run(*mInternalModule);
    return ret;
}

bool ModuleHandler::run_function_passes(const std::string &func_name)
{
    llvm::Function *func = mExecutionEngine->FindFunctionNamed(func_name.c_str());
    assert(func!=NULL && "Function not found !");
    if(!func) return false;
    return mFunctionPassManager->run(*func);
}

std::string ModuleHandler::get_function_ir(const std::string &func_name)
{
    llvm::Function *func = mExecutionEngine->FindFunctionNamed(func_name.c_str());
    assert(func!=NULL && "Function not found !");
    if(!func) return std::string();

    std::stringstream ss;
    llvm::raw_os_ostream raw_stream(ss);
    func->print(raw_stream);
    return ss.str();
}

void ModuleHandler::print_module()
{
    mInternalModule->dump();
}

void ModuleHandler::print_module(std::ostream &stream)
{
    llvm::raw_os_ostream raw_stream(stream);
    mInternalModule->print(raw_stream, NULL);
}

llvm::Function* ModuleHandler::declare_function(const std::string &function_name,
                                                std::map<std::string, llvm::Value*> &named_values)
{
    std::vector<const llvm::Type*> func_proto(mVariableList.size(),
                                              llvm::Type::getDoubleTy(llvm::getGlobalContext()));

    llvm::FunctionType *func_type =
        llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()),
                                func_proto, false);

    assert(func_type!=NULL);

    llvm::Function *func =
        llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                               function_name, mInternalModule);

    assert(func!=NULL);

    unsigned int var_index = 0;
    for (llvm::Function::arg_iterator arg_it = func->arg_begin();
         var_index != mVariableList.size(); ++arg_it, ++var_index)
    {
        const std::string var_name = mVariableList[var_index];
        arg_it->setName(var_name);
        named_values[var_name] = arg_it;
    }

    return func;
}

void ModuleHandler::codegen_ast(const std::vector<ASTNode*> *ast_nodes,
                                const std::string &func_name)
{
    std::vector<llvm::Value*> ast_codegen;
    std::map<std::string, llvm::Value*> named_values;

    llvm::Function *func = declare_function(func_name, named_values);

    llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", func);

    llvm::IRBuilder<> builder(llvm::getGlobalContext());
    builder.SetInsertPoint(basic_block);

    std::vector<ASTNode*>::const_iterator it_ast_nodes = ast_nodes->end()-1;

    while(it_ast_nodes != ast_nodes->begin()-1)
    {
        const ASTNode *node = *it_ast_nodes;
        it_ast_nodes--;

        switch(node->get_id())
        {

        // Handles the ASTConstant node type
        case ASTNode::AST_CONSTANT:
        {
            const ASTConstant *constant =
                static_cast<const ASTConstant*>(node);

            llvm::Value *val =
                llvm::ConstantFP::get(llvm::getGlobalContext(),
                                      llvm::APFloat(constant->get_value()));
            assert(val!=NULL);
            ast_codegen.push_back(val);
            break;
        }

        // Handles the ASTVariable node type
        case ASTNode::AST_VARIABLE:
        {
            const ASTVariable *variable =
                static_cast<const ASTVariable*>(node);

            llvm::Value *variable_codegen = named_values[variable->get_name()];
            assert(variable_codegen!=NULL);

            ast_codegen.push_back(variable_codegen);
            break;
        }

        // Handles the ASTFunction node type
        case ASTNode::AST_FUNCTION:
        {
            const ASTFunction *func_codegen =
                static_cast<const ASTFunction*>(node);

            llvm::Function *find_func =
                mInternalModule->getFunction(func_codegen->get_name());
            assert(find_func!=NULL);

            const int arg_size =
                find_func->arg_size();

            std::vector<llvm::Value*> argument_list;
            for(int i=0; i < arg_size; i++)
            {
                argument_list.push_back(ast_codegen.back());
                ast_codegen.pop_back();
            }

            llvm::CallInst *call_inst =
                    builder.CreateCall(find_func, argument_list.begin(),
                                       argument_list.end(), "tmp_call");
            ast_codegen.push_back(call_inst);
            break;
        }

        default:
            break;
        }
    }
    assert(ast_codegen.size()==1);
    builder.CreateRet(ast_codegen.back());
}

void* ModuleHandler::jit_function(const std::string &func_name)
{
    llvm::Function *func = mExecutionEngine->FindFunctionNamed(func_name.c_str());
    if(!func) return NULL;

    void *jit_func = mExecutionEngine->recompileAndRelinkFunction(func);

    if(jit_func)
        mJITFunctions.insert(std::make_pair(func_name, func));

    return jit_func;
}

bool ModuleHandler::free_jit_memory(const std::string &func_name)
{
    JITFunctionMap::iterator func_it = mJITFunctions.find(func_name);

    if(func_it==mJITFunctions.end())
        return false;

    llvm::Function *function = func_it->second;
    mJITFunctions.erase(func_it);
    mExecutionEngine->freeMachineCodeForFunction(function);
    return true;
}

bool ModuleHandler::free_jit_memory(void)
{
    bool ret_free = false;
    for(JITFunctionMap::const_iterator it = mJITFunctions.begin();
        it!=mJITFunctions.end(); it++)
    {
        llvm::Function *item = it->second;
        mExecutionEngine->freeMachineCodeForFunction(item);
        ret_free = true;
    }
    mJITFunctions.clear();
    return ret_free;
}

}


