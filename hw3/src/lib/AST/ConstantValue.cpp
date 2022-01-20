#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     Variable *c_value)
    : ExpressionNode{line, col}, const_value(c_value) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    if(const_value->type == Type::TYPE_INTEGER){
        std::printf("constant <line: %u, col: %u> %d\n",
                location.line, location.col, const_value->int_literal
                );
    }
    else if(const_value->type == Type::TYPE_REAL){
        std::printf("constant <line: %u, col: %u> %f\n",
                location.line, location.col, const_value->real_literal
                );
    }
    else if(const_value->type == Type::TYPE_STRING){
        std::printf("constant <line: %u, col: %u> %s\n",
                location.line, location.col, const_value->string_literal
                );
    }
    else if(const_value->type == Type::TYPE_BOOLEAN){
        std::string bool_val;
        if(const_value->boolean_literal == Boolean_type::booltype_True){
            bool_val = "true";
        }
        else if(const_value->boolean_literal == Boolean_type::booltype_False){
            bool_val = "false";
        }
        std::printf("constant <line: %u, col: %u> %s\n",
                location.line, location.col, bool_val.c_str());
    }

}

void ConstantValueNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}
