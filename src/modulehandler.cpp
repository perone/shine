#include "modulehandler.h"

#include "modulelinker.h"
#include "astnode.h"

#include <cassert>

#include <llvm/Module.h>
#include <llvm/Support/StandardPasses.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Constants.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Target/TargetSelect.h>

namespace shine
{
ModuleHandler::ModuleHandler(llvm::Module *module,
                             llvm::ExecutionEngine *execution_engine,
                             llvm::PassManager *pass_manager)
{
    assert(module && "No module provided !");
    assert(execution_engine && "No Execution Engine provided !");
    assert(pass_manager && "No Pass Manager provided !");

    mInternalModule = module;
    mExecutionEngine = execution_engine;
    mPassManager = pass_manager;
}

ModuleHandler* ModuleHandler::create(llvm::Module *module,
                                     std::string &error_string,
                                     llvm::PassManager *pass_manager)
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

    return new ModuleHandler(module, execution_engine, created_pass_manager);
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

void ModuleHandler::print_module()
{
    mInternalModule->dump();
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

        case ASTNode::AST_VARIABLE:
        {
            const ASTVariable *variable =
                static_cast<const ASTVariable*>(node);

            llvm::Value *variable_codegen = named_values[variable->get_name()];
            assert(variable_codegen!=NULL);

            ast_codegen.push_back(variable_codegen);
            break;
        }

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
    if(!func) return func;
    return mExecutionEngine->recompileAndRelinkFunction(func);
}

}

