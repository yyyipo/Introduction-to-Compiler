#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col,
                                               const char* f_name, std::vector<AstNode*> *f_expression)
    : ExpressionNode{line, col}, name(f_name), expression(f_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("function invocation <line: %u, col: %u> %s\n",
                location.line, location.col, name);

    // AstNode::incrementIndentation();

    // if(expression != NULL){
    //     for(int i = 0; i < expression->size(); i++){
    //         expression->at(i)->print();
    //     }
    // }
    
    // AstNode::decrementIndentation();
}

void FunctionInvocationNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression != NULL){
        for (auto &expression : *expression) {
            expression->accept(p_visitor);
        }
    }
}
