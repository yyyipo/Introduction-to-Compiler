#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       Op_information *binary_operator, AstNode *binary_left_operand, AstNode *binary_right_operand
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: operator, expressions
    Op_information *op;
    AstNode *left_operand;
    AstNode *right_operand;
};

#endif
