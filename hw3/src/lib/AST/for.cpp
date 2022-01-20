#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 AstNode* f_declaration_node, AstNode* f_assignment_node,
                AstNode* f_expression_node, AstNode* f_compound_statement_node)
    : AstNode{line, col}, declaration_node(f_declaration_node), assignment_node(f_assignment_node),
      expression_node(f_expression_node), compound_statement_node(f_compound_statement_node) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {

    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("for statement <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(declaration_node != NULL){
    //     declaration_node->print();
    // }

    // if(assignment_node != NULL){
    //     assignment_node->print();
    // }

    // if(expression_node != NULL){
    //     expression_node->print();
    // }

    // if(compound_statement_node != NULL){
    //     compound_statement_node->print();
    // }
    
    // AstNode::decrementIndentation();


}

void ForNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration_node != NULL){
        declaration_node->accept(p_visitor);
    }
    if(assignment_node != NULL){
        assignment_node->accept(p_visitor);
    }
    if(expression_node != NULL){
        expression_node->accept(p_visitor);
    }
    if(compound_statement_node != NULL){
        compound_statement_node->accept(p_visitor);
    }
}
