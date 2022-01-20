#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      Op_information *unary_operator, AstNode *unary_operand
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
     void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: operator, expression
    Op_information *op;
    AstNode *operand;
    
    
};

#endif
