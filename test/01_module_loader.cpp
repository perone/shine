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
