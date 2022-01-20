#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *p_name,const char *p_return,
                         std::vector <AstNode*> *p_declaration_list,
                         std::vector <AstNode*> *p_function_list,
                         AstNode *p_body)
    : AstNode{line, col}, name(p_name), return_type(p_return),
      declaration_list(p_declaration_list), function_list(p_function_list), body(p_body) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
    // AstNode::incrementIndentation();

    // if(declaration_list != NULL){
    //     for(int i = 0; i < declaration_list->size(); i++){
    //     declaration_list->at(i)->print();
    //     }
    // }

    // if(function_list != NULL){
    //     for(int i = 0; i < function_list->size(); i++){
    //     function_list->at(i)->print();
    //     }
    // }

    // if(body != NULL){
    //     body->print();
    // }
    
    // AstNode::decrementIndentation();

}

void ProgramNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    //TODO
    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(function_list != NULL){
        for (auto &function_list : *function_list) {
            function_list->accept(p_visitor);
        }
    }

    body->accept(p_visitor);
    
}
