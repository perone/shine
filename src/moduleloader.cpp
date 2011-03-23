#include "moduleloader.h"

#include <cassert>

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Module.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/system_error.h>
#include <llvm/Type.h>

#include <sstream>

namespace shine
{

ModuleLoader::ModuleLoader(llvm::Module *module)
: mInternalModule(module)
{
    assert(module!=NULL);
}

ModuleLoader::~ModuleLoader()
{
    delete mInternalModule;
}

ModuleLoader *ModuleLoader::create_from_memory_buffer(llvm::MemoryBuffer *memory_buffer,
                                                      std::string &error_string)
{

    if(!memory_buffer)
    {
        error_string = "Error while loading memory bufer: [ memory buffer is NULL ]";
        return NULL;
    }

    std::string i_error_string;

    llvm::Module *module =
        llvm::ParseBitcodeFile(memory_buffer, llvm::getGlobalContext(), &i_error_string);

    if(!module)
    {
        error_string = "Error while parsing bitcode: [" + i_error_string + "]";
        return NULL;
    }

    ModuleLoader *loader = new ModuleLoader(module);
    return loader;
}


ModuleLoader *ModuleLoader::create_from_file(const std::string &filename,
                                             std::string &error_string)
{
    llvm::OwningPtr<llvm::MemoryBuffer> buffer;

    if(filename.empty())
    {
        error_string = "Error while reading bitcode: [ No filename specified ]";
        return NULL;
    }

    if (llvm::error_code ec = llvm::MemoryBuffer::getFileOrSTDIN(filename.c_str(), buffer))
    {
        error_string = "Error while reading bitcode: [" + ec.message() + "]";
        return NULL;
    }

    ModuleLoader *loader =
        ModuleLoader::create_from_memory_buffer(buffer.get(), error_string);

    return loader;
}

bool ModuleLoader::check_closure(std::string &error_string)
{
    bool closure_checking = true;
    std::stringstream ss_error;

    const llvm::Module::FunctionListType &f_list =
        mInternalModule->getFunctionList();

    for(llvm::Module::FunctionListType::const_iterator it = f_list.begin();
        it != f_list.end(); it++)
    {
        const llvm::Function *func = it;
        const bool is_ret_double = func->getReturnType()->isDoubleTy();

        if(!is_ret_double)
        {
            ss_error << "Function " << func->getNameStr() << " isn't returning double !\n";
            closure_checking = false;
        }

        const llvm::Function::ArgumentListType &a_list = func->getArgumentList();
        for(llvm::Function::ArgumentListType::const_iterator it_arg = a_list.begin();
            it_arg != a_list.end(); it_arg++)
        {
            const llvm::Argument *arg = it_arg;
            const bool is_arg_double = arg->getType()->isDoubleTy();
            if(!is_arg_double)
            {
                ss_error << "Argument " << func->getNameStr() << "[" << arg->getNameStr() << "] isn't double !\n";
                closure_checking = false;
            }
        }
    }

    error_string = ss_error.str();

    return closure_checking;
}


}
