#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   AstNode* a_variable_reference_node, AstNode* a_expression_node
                   /* TODO: variable reference, expression */);
    ~AssignmentNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: variable reference, expression
    AstNode* variable_reference_node;
    AstNode* expression_node;
};

#endif
