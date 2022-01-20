#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
               AstNode* i_expression_node, AstNode* i_if_compound_statement_node, AstNode* i_else_compound_statement_node)
    : AstNode{line, col}, expression_node(i_expression_node), 
      if_compound_statement_node(i_if_compound_statement_node), else_compound_statement_node(i_else_compound_statement_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("if statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(expression_node != NULL){
    //     expression_node->print();
    // }

    // if(if_compound_statement_node != NULL){
    //     if_compound_statement_node->print();
    // }

    // if(else_compound_statement_node != NULL){
    //     else_compound_statement_node->print();
    // }
    
    // AstNode::decrementIndentation();
}

void IfNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
    if(if_compound_statement_node != NULL){
        if_compound_statement_node->accept(p_visitor);
    }
    if(else_compound_statement_node != NULL){
        else_compound_statement_node->accept(p_visitor);
    }
}
