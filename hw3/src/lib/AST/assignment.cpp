#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               AstNode* a_variable_reference_node, AstNode* a_expression_node)
    : AstNode{line, col}, variable_reference_node(a_variable_reference_node), expression_node(a_expression_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("assignment statement <line: %u, col: %u>\n",
                location.line, location.col);
    

    // AstNode::incrementIndentation();

    // if(variable_reference_node != NULL){
    //     variable_reference_node->print();
    // }

    // if(expression_node != NULL){
    //     expression_node->print();
    // }
    
    // AstNode::decrementIndentation();
}

void AssignmentNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    
    if(variable_reference_node != NULL){
        variable_reference_node->accept(p_visitor);
    }

    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
}
