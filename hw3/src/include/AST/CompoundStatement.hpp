#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          std::vector <AstNode*> *cs_declaration_list,
                          std::vector <AstNode*> *cs_statement_list
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: declarations, statements
    std::vector <AstNode*> *declaration_list;
    std::vector <AstNode*> *statement_list;
};

#endif
