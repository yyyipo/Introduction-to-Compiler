#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const char *v_name, Variable *v_type,
                           AstNode *v_constant_value
                           /* TODO: variable name, type, constant value */)
    : AstNode{line, col}, name(v_name), type(v_type), constant_value(v_constant_value) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::string var_type;
    // normal type and array
    if(type->type == Type::TYPE_INTEGER){
        var_type = "integer";
    }
    else if(type->type == Type::TYPE_REAL){
        var_type = "real";
    }
    else if(type->type == Type::TYPE_STRING){
        var_type = "string";
    }
    else if(type->type == Type::TYPE_BOOLEAN){
        var_type = "boolean";
    }

    //array
    if(type->array_num.size() > 0){
        var_type += " ";
        for(uint i = 0; i < type->array_num.size(); i++){
            var_type += "[";
            var_type += std::to_string(type->array_num[i]);
            var_type += "]";
        }        
    }
    
    

    std::printf("variable <line: %u, col: %u> %s %s\n",
                location.line, location.col, name, var_type.c_str());

    // AstNode::incrementIndentation();

    // if(constant_value != NULL){
    //     constant_value->print();
    // }
    
    // AstNode::decrementIndentation();

}

void VariableNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO

    if(constant_value != NULL){
        constant_value->accept(p_visitor);
    }
}
