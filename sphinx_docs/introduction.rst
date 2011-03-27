Introduction
===============================================================================

.. _ASTs: http://en.wikipedia.org/wiki/Abstract_syntax_tree
.. _LLVM IR: http://llvm.org/docs/LangRef.html
.. _LLVM Passes: http://llvm.org/docs/Passes.html

What is Shine ?
-------------------------------------------------------------------------------
Shine is a library that automatically makes the code generation of the `LLVM IR`_
(Intermediate Representation) from the user Genetic Programming trees.
It also provides an easy API for optimizing and JITing Genetic Programming
`ASTs`_ into native code.

.. note:: Genetic Programming literature often call individuals as *trees*,
          *syntax trees* or *abstract syntax trees* (ASTs), we'll adopt the term
          AST (Abstract Syntax Tree) here and in the library itself.

Architecture Design
-------------------------------------------------------------------------------
The follow image shows an overview of the Shine architecture:

.. figure:: images/shine_process_overview.png
   :align: center

Shine takes as input the individuals (`ASTs`_) of your Genetic Programming system and
then creates the `LLVM IR` assembly. Shine uses the `LLVM Passes`_ to optimize this
generated assembly code; here is some examples showing how these transformation
passes work on your Genetic Programming individuals:

**Instruction Combine** - *Combine redundant instructions*

It combines instructions like:

.. code-block:: llvm

    %Y = add i32 %X, 1
    %Z = add i32 %Y, 1

into:

.. code-block:: llvm

    %Z = add i32 %X, 2

**Constant Propagation**

This transformation implements the constant propagation and merging, example:

.. code-block:: llvm

    add i32 1, 2

is transformed into:

.. code-block:: llvm

    i32 3

These are just two examples of how the LLVM's Transformations works, for more information see the
LLVM's Analysis and Transform Passes guide.

In the diagram below, you can see how Shine interacts with the user's Genetic Programming system:

.. figure:: images/shine_diagram.png
   :align: center

The first step is the preparation of the non-terminal (funcions) code, actually, the definition
of the functions that will be used in your ASTs can be done in any language that supports
the LLVM IR generation (you can use Clang, llvm-gcc, etc...) to create the functions, in simple
terms, you can define your functions using C/C++ for example.

.. seealso::
    
    `LLVM's Analysis and Transform Passes <http://llvm.org/docs/Passes.htm>`_

    `LLVM Assembly Language Reference <http://llvm.org/docs/LangRef.html>`_


Requirements
-------------------------------------------------------------------------------
.. todo:: Add requirements.
