#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                   AstNode* r_variable_reference_node)
    : AstNode{line, col}, variable_reference_node(r_variable_reference_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("read statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(variable_reference_node != NULL){
    //     variable_reference_node->print();
    // }
    
    // AstNode::decrementIndentation();

}

void ReadNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(variable_reference_node != NULL){
        variable_reference_node->accept(p_visitor);
    }
}
