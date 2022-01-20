#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              AstNode* w_expression_node, AstNode* w_compound_statement_node
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: expression, compound statement
    AstNode* expression_node;
    AstNode* compound_statement_node;
};

#endif
