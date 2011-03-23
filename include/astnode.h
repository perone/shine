/**
 * \file astnode.h
 * This file defines and implement all the AST related classes and methods. It contains
 * the implementation of the three AST node types: Variables, Constants and Functions.
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

#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <vector>
#include <iostream>

namespace shine
{

/**
 * This is the abstract AST class, all AST Nodes inherits
 * from this abstract class. This class also has some
 * utility static methods useful for handling the ASTs.
 */
class ASTNode
{
public:
    ASTNode() {};
    virtual ~ASTNode() {};

    /**
     * This method is used to send a textual representation
     * of the AST node to a stream.
     *
     * \param stream The stream, it could be \p std::cout for example.
     */
    virtual void print_stream(std::ostream &stream) const = 0;

// Public interface
public:
    /**
     * Node types, used to avoid C++ RTTI.
     */
    enum ASTNodeType { AST_VARIABLE, AST_CONSTANT, AST_FUNCTION};

    /**
     * This method returns the type of the node.
     *
     * \see ASTNodeType
     * \return The node type
     */
    virtual ASTNodeType get_id() const = 0;
};

/**
 * Pointer to the ASTNode, used by some bindings.
 */
typedef ASTNode* ASTNodePointer;

/**
 * This class represents a Genetic Programming variable, this is the
 * same as the parameter of the individual function representation.
 */
class ASTVariable : public ASTNode
{
public:
    /**
     * Constructor of the ASTVariable.
     *
     * \param name This is the name of the variable, and must
     *             match with the variable name of the individual.
     */
    ASTVariable(const std::string &name)
    : mName(name) { };
    virtual ~ASTVariable() {};

public:
    virtual ASTNodeType get_id() const { return ASTNode::AST_VARIABLE; }

    /**
     * Returns the name of the variable.
     * \see ASTVariable::ASTVariable
     * \see ASTVariable::set_name
     *
     * \return The name of the variable.
     */
    std::string get_name() const { return mName; }

    /**
     * Sets the name of the variable.
     * \see ASTVariable::ASTVariable
     * \see ASTVariable::get_name
     *
     * \param name The name of the variable.
     */
    void set_name(const std::string &name) { mName = name; }

    virtual void print_stream(std::ostream &stream) const
    { stream << "[ASTVariable " << mName << "]"; }

private:
    /**
     * The name of the variable.
     */
    std::string mName;
};

/**
 * This node represents a single constant.
 */
class ASTConstant : public ASTNode
{
public:
    /**
     * Constant constructor.
     *
     * \param value The constant value.
     */
    ASTConstant(double value)
    : mValue(value) {};
    virtual ~ASTConstant() {};

public:
    virtual ASTNodeType get_id() const { return ASTNode::AST_CONSTANT; }

    /**
     * Returns the constant value.
     * \see ASTConstant::set_value
     *
     * \return Constant value.
     */
    double get_value() const { return mValue; }

    /**
     * Sets the constant value.
     * \see ASTConstant::get_value
     *
     * \param value Constant value.
     */
    void set_value(const double &value) { mValue = value; }

    virtual void print_stream(std::ostream &stream) const
    { stream << "[ASTConstant " << mValue << "]"; }

private:
    /**
     * Constant value.
     */
    double mValue;
};

/**
 * This class represents a Genetic Programming non-terminal
 * node, a function.
 */
class ASTFunction : public ASTNode
{
public:
    /**
     * This is the name of the function, and must match with
     * the name of the function defined in the module loaded
     * by the ModuleLoader.
     *
     * \param name Function name.
     */
    ASTFunction(const std::string &name)
    : mName(name) {};
    virtual ~ASTFunction() {};

public:
    virtual ASTNodeType get_id() const { return ASTNode::AST_FUNCTION; }

    /**
     * Returns the name of the function.
     * \see ASTFunction::ASTFunction
     * \see ASTFunction::set_name
     *
     * \return The name of the function.
     */
    std::string get_name() const { return mName; }

    /**
     * Sets the name of the function.
     * \see ASTFunction::ASTFunction
     * \see ASTFunction::get_name
     *
     * \param name The name of the function.
     */
    void set_name(const std::string &name) { mName = name; }

    virtual void print_stream(std::ostream &stream) const
    { stream << "[ASTFunction " << mName << "]"; }

private:
    /**
     * Function name.
     */
    std::string mName;
};


}

#endif // ASTNODE_H
