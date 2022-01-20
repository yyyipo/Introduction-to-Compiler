#include "AST/expression.hpp"

ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

const char *ExpressionNode::getTypeCString(){
    if(type == nullptr)
        return "null";
    else
        return type->getPTypeCString(); 
}

PTypeSharedPtr ExpressionNode::getType(){
    return type;
}

void ExpressionNode::setType(PTypeSharedPtr set){
    type = set;    
}


