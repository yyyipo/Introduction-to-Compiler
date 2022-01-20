#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            AstNode* f_declaration_node, AstNode* f_assignment_node,
            AstNode* f_expression_node, AstNode* f_compound_statement_node
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    
  public:
    // TODO: declaration, assignment, expression, compound statement
    AstNode* declaration_node;
    AstNode* assignment_node;
    AstNode* expression_node;
    AstNode* compound_statement_node;
};

#endif
