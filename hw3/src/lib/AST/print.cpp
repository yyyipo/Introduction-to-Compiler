#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, 
                     AstNode* print_expression)
    : AstNode{line, col}, expression_node(print_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("print statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(expression_node != NULL){
    //     expression_node->print();
    // }
    
    // AstNode::decrementIndentation();
}

void PrintNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
}
