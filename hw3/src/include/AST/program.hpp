#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"

class ProgramNode : public AstNode {
  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *p_return,
                std::vector <AstNode*> *p_declaration_list,
                std::vector <AstNode*> *p_function_list,
                AstNode *p_body
                );
                /* TODO: return type, declarations, functions,
                 *       compound statement */
    ~ProgramNode() = default;

    // visitor pattern version: const char *getNameCString() const; 

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    const std::string name;
    // TODO: return type, declarations, functions, compound statement
    const std::string return_type;
    std::vector <AstNode*> *declaration_list;
    std::vector <AstNode*> *function_list;
    AstNode *body;

};

#endif
