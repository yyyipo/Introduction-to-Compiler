#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char* f_name, std::vector<AstNode*> *f_expression
                           /* TODO: function name, expressions */);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor);

  public:
    // TODO: function name, expressions
    const char* name;
    std::vector<AstNode*> *expression;
};

#endif
