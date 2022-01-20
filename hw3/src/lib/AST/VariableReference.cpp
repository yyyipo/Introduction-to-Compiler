#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                                             const char* v_name, std::vector<AstNode*> *v_expression_node)
    : ExpressionNode{line, col}, name(v_name), expression_node(v_expression_node) {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("variable reference <line: %u, col: %u> %s\n",
                location.line, location.col, name);

    // AstNode::incrementIndentation();

    // if(expression_node != NULL){
    //     for(int i = 0; i < expression_node->size(); i++){
    //         expression_node->at(i)->print();
    //     }        
    // }
    
    // AstNode::decrementIndentation();
}

void VariableReferenceNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression_node != NULL){
        for (auto &expression_node : *expression_node) {
            expression_node->accept(p_visitor);
        }
    }
}
