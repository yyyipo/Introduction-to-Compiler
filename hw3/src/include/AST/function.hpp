#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 const char *f_name, std::vector <AstNode*> *f_declaration_list, std::vector <Variable*> *f_parameter_list,
                 Variable *f_return_type, AstNode *f_compuound_statement
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: name, declarations, return type, compound statement
    const char *name;
    std::vector <AstNode*> *declaration_list;
    std::vector <Variable*> *parameter_list;
    Variable *return_type;
    AstNode *compuound_statement;
};

#endif
