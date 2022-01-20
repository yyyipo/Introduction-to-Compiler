#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     AstNode* w_expression_node, AstNode* w_compound_statement_node)
    : AstNode{line, col}, expression_node(w_expression_node), compound_statement_node(w_compound_statement_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("while statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(expression_node != NULL){
    //     expression_node->print();
    // }

    // if(compound_statement_node != NULL){
    //     compound_statement_node->print();
    // }
    
    // AstNode::decrementIndentation();
}

void WhileNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
    if(compound_statement_node != NULL){
        compound_statement_node->accept(p_visitor);
    }
}
