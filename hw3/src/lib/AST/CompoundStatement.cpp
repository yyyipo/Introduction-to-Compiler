#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector <AstNode*> *cs_declaration_list,
                                             std::vector <AstNode*> *cs_statement_list)
    : AstNode{line, col}, declaration_list(cs_declaration_list), statement_list(cs_statement_list) {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::printf("compound statement <line: %u, col: %u>\n",
                location.line, location.col);
                
    // AstNode::incrementIndentation();

    // if(declaration_list != NULL){
    //     for(int i = 0; i < declaration_list->size(); i++){
    //     declaration_list->at(i)->print();
    //     }
    // }

    // if(statement_list != NULL){
    //     for(int i = 0; i < statement_list->size(); i++){
    //     statement_list->at(i)->print();
    //     }
    // }

    // AstNode::decrementIndentation();
}

void CompoundStatementNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO

    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(statement_list != NULL){
        for (auto &statement_list : *statement_list) {
            statement_list->accept(p_visitor);
        }
    }
}
