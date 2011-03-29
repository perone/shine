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
#include <string>

#include <glib.h>
#include <llvm/Support/ManagedStatic.h>

using namespace shine;

gboolean stack_traversal(GNode *node, gpointer stack)
{
    std::vector<ASTNode*> *ast_stack =
        static_cast<std::vector<ASTNode*>*>(stack);

    ASTNode *ast_node = static_cast<ASTNode*>(node->data);
    ast_stack->push_back(ast_node);
    return FALSE;
}

gboolean destroy_traversal(GNode *node, gpointer data)
{
    ASTNode *ast_node = static_cast<ASTNode*>(node->data);
    delete ast_node;
    return FALSE;
}

int main(void)
{
    std::string error_string;

    shine_initialize();

    ModuleLoader *loader1 =
            ModuleLoader::create_from_file("mod1.o", error_string);

    if(!loader1)
    {
        std::cout << "Error: " << error_string << std::endl;
        return -1;
    }

    const bool closure_check = loader1->check_closure(error_string);
    std::cout << "Closure check: " << std::boolalpha << closure_check << std::endl;
    if(!closure_check)
        std::cout << "Res: " << std::endl << error_string << std::endl;

    ModuleLinker *link = new ModuleLinker("lala", "lero");

    bool link_ret = link->link_module_loader(loader1, error_string);
    delete loader1;

    if(!link_ret)
    {
        std::cout << "Error: " << error_string << std::endl;
        return -1;
    }

    ModuleHandler *mod_handler =
            ModuleHandler::create(link->release_module(), error_string);

    if(!mod_handler)
    {
        std::cout << "Error: " << error_string << std::endl;
        return -1;
    }

    delete link;

    //mod_handler->run_module_passes();

    /************************************************************
     *                         NODES
     ************************************************************/
    GNode *n_f = g_node_new(new ASTFunction("F"));
        GNode *n_x = g_node_append_data(n_f, new ASTVariable("x"));
        GNode *n_g = g_node_append_data(n_f, new ASTFunction("G"));
            GNode *n_h = g_node_append_data(n_g, new ASTFunction("H"));
                GNode *n_h1 = g_node_append_data(n_h, new ASTConstant(1));
                GNode *n_h2 = g_node_append_data(n_h, new ASTConstant(2));
            GNode *n_g2 = g_node_append_data(n_g, new ASTConstant(2));
            GNode *n_i = g_node_append_data(n_g, new ASTFunction("I"));
                GNode *n_i0 = g_node_append_data(n_i, new ASTConstant(0));

    std::vector<ASTNode*> ast_nodes;
    g_node_traverse(n_f, G_PRE_ORDER, G_TRAVERSE_ALL, -1,
                    stack_traversal, &ast_nodes);

    std::vector<std::string> vars;
    vars.push_back("x");

    mod_handler->set_variable_list(vars);
    assert(mod_handler->get_variable_list()==vars);

    mod_handler->codegen_ast(&ast_nodes, "my_func");

    std::cout << mod_handler->get_function_ir("my_func") << std::endl;
    mod_handler->run_function_passes("my_func");

    void *func_ptr = mod_handler->jit_function("my_func");
    mod_handler->free_jit_memory();
    func_ptr = mod_handler->jit_function("my_func");

    if(!func_ptr)
    {
        std::cout << "Error: function not found !" << std::endl;
        return -1;
    }

    double (*FP)(double) = (double (*)(double))(intptr_t)func_ptr;
    double answer = FP(10.2);
    std::cout << "JIT Run Func: " << answer << std::endl;
    assert(answer==12.7);

    delete mod_handler;

    g_node_traverse(n_f, G_IN_ORDER, G_TRAVERSE_ALL, -1,
                    destroy_traversal, NULL);
    g_node_destroy(n_f);

    shine_shutdown();

    return 0;
}
