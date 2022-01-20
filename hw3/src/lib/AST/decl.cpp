#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector <AstNode*> *d_varialble_list)
    : AstNode{line, col}, variable_list(d_varialble_list) {}

// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("declaration <line: %u, col: %u>\n",
                location.line, location.col);

    // AstNode::incrementIndentation();

    // if(variable_list != NULL){
    //     for(int i = 0; i < variable_list->size(); i++){
    //         variable_list->at(i)->print();
    //     }
    // }
    
    // AstNode::decrementIndentation();
}

void DeclNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(variable_list != NULL){
        for (auto &variable_list : *variable_list) {
            variable_list->accept(p_visitor);
        }
    }
}
