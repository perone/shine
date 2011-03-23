#include "modulelinker.h"

#include "moduleloader.h"

#include <cassert>

#include <llvm/Module.h>
#include <llvm/Linker.h>
#include <llvm/LLVMContext.h>

namespace shine
{

ModuleLinker::ModuleLinker(const std::string &prog_name,
                           const std::string &module_name)
{
    mInternalLinker =
        new llvm::Linker(prog_name.c_str(),
                         module_name.c_str(),
                         llvm::getGlobalContext());

    assert(mInternalLinker!=NULL);
}

ModuleLinker::~ModuleLinker()
{
    delete mInternalLinker;
}

bool ModuleLinker::link_module_loader(ModuleLoader *module_loader,
                                      std::string &error_string)
{
    assert(module_loader!=NULL);

    std::string i_error_string;

    llvm::Module *llvm_module =
        module_loader->get_internal_module();

    assert(llvm_module!=NULL);

    const bool ret_link =
        mInternalLinker->LinkInModule(llvm_module, &i_error_string);

    if(ret_link)
    {
        error_string = "Error while linking module: [" + i_error_string + "]";
        return false;
    }

    return true;
}

llvm::Module* ModuleLinker::get_composite_module()
{
    llvm::Module *composite_module =
        mInternalLinker->getModule();

    return composite_module;
}

llvm::Module* ModuleLinker::release_module()
{
    return mInternalLinker->releaseModule();
}

}
