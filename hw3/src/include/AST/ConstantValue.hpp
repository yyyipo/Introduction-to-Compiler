#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      Variable *c_value                     
                      /* TODO: constant value */);
    ~ConstantValueNode() = default;

    void print() override;
    void accept(AstNodeVisitor &visitor) override;

  public:
    // TODO: constant value
    Variable *const_value;
};

#endif
