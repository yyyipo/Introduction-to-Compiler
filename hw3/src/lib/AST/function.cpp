#include "AST/function.hpp"
#include <iostream>

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                           const char *f_name, std::vector <AstNode*> *f_declaration_list, std::vector <Variable*> *f_parameter_list,
                           Variable *f_return_type, AstNode *f_compuound_statement)
    : AstNode{line, col}, name(f_name), declaration_list(f_declaration_list), parameter_list(f_parameter_list),
      return_type(f_return_type), compuound_statement(f_compuound_statement) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    //fuction type
    std::string func_type; 
    if(return_type->type == Type::TYPE_INTEGER){
        func_type = "integer";
    }
    else if(return_type->type == Type::TYPE_REAL){
        func_type = "real";
    }
    else if(return_type->type == Type::TYPE_STRING){
        func_type = "string";
    }
    else if(return_type->type == Type::TYPE_BOOLEAN){
        func_type = "boolean";
    }
    else if(return_type->type == Type::TYPE_VOID){
        func_type = "void";
    }

    //function parameter
    std::string func_parameter;
    func_parameter = "(";
    if(parameter_list->size() > 0){
        for(uint i = 0; i < parameter_list->size(); i++){
            
            if(parameter_list->at(i)->type == Type::TYPE_INTEGER){
                func_parameter += "integer";
            }
            else if(parameter_list->at(i)->type == Type::TYPE_REAL){
                func_parameter += "real";
            }
            else if(parameter_list->at(i)->type == Type::TYPE_STRING){
                func_parameter += "string";
            }
            else if(parameter_list->at(i)->type == Type::TYPE_BOOLEAN){
                func_parameter += "boolean";
            }

            if(parameter_list->at(i)->array_num.size() > 0){
            func_parameter += " ";
            for(uint j = 0; j < parameter_list->at(i)->array_num.size(); j++){
                //std::cout << "num:" << parameter_list->at(i)->array_num[i] << std::endl;
                func_parameter += "[";
                func_parameter += std::to_string(parameter_list->at(i)->array_num[j]);
                func_parameter += "]";
            }        
            }

            if(i != parameter_list->size() - 1){
                func_parameter += ", ";
            }
            
        }
        
    }
    func_parameter += ")";

    std::printf("function declaration <line: %u, col: %u> %s %s %s\n",
                location.line, location.col, name, func_type.c_str(), func_parameter.c_str());

    // AstNode::incrementIndentation();

    // if(declaration_list != NULL){
    //     for(int i = 0; i < declaration_list->size(); i++){
    //         declaration_list->at(i)->print();
    //     }
    // }

    // if(compuound_statement != NULL){
    //     compuound_statement->print();
    // }
    
    // AstNode::decrementIndentation();

}

void FunctionNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(compuound_statement != NULL){
        compuound_statement->accept(p_visitor);
    }
}
