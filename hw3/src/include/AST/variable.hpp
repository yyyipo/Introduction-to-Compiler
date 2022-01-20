#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 const char *v_name, Variable *v_type,
                 AstNode *v_constant_value
                 /* TODO: variable name, type, constant value */);

    ~VariableNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: variable name, type, constant value
    const char *name;
    Variable *type;
    AstNode *constant_value;
};

#endif
