#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       AstNode* r_expression_node)
    : AstNode{line, col}, expression_node(r_expression_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("return statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(expression_node != NULL){
    //     expression_node->print();
    // }
    
    // AstNode::decrementIndentation();
}

void ReturnNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
}
