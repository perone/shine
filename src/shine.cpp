#include "shine.h"

#include <llvm/Target/TargetSelect.h>
#include <llvm/Support/ManagedStatic.h>

namespace shine
{

void shine_initialize(void)
{
    llvm::InitializeNativeTarget();
}

void shine_shutdown(void)
{
    llvm::llvm_shutdown();
}


}
