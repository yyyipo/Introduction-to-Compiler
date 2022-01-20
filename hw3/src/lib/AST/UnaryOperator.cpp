#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     Op_information *unary_operator, AstNode *unary_operand)
    : ExpressionNode{line, col}, op(unary_operator), operand(unary_operand) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {
    //AstNode::outputIndentationSpace(m_indentation);

    std::string op_type;
    if(op->operator_type == Operator::OP_MINUS){
        op_type = "neg";
    }
    else if(op->operator_type == Operator::OP_NOT){
        op_type = "not";
    }

    std::printf("unary operator <line: %u, col: %u> %s\n",
                location.line, location.col, op_type.c_str());

    // AstNode::incrementIndentation();

    // if(operand != NULL){
    //     operand->print();
    // }

    
    // AstNode::decrementIndentation();

}

void UnaryOperatorNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(operand != NULL){
        operand->accept(p_visitor);
    }
}
