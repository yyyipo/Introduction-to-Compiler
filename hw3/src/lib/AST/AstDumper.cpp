#include "AST/AstDumper.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/decl.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"

#include <cstdio>

// FIXME: remove this line if you choose to use visitor pattern with this template
//#ifdef I_WANT_TO_USE_VISITOR_PATTERN

void AstDumper::incrementIndentation() {
    m_indentation += m_indentation_stride;
}

void AstDumper::decrementIndentation() {
    m_indentation -= m_indentation_stride;
}

static void outputIndentationSpace(const uint32_t indentation) {
    std::printf("%*s", indentation, "");
}

void AstDumper::visit(ProgramNode &p_program) {
    outputIndentationSpace(m_indentation);

    // std::printf("program <line: %u, col: %u> %s %s\n",
    //             p_program.getLocation().line, p_program.getLocation().col,
    //             p_program.getNameCString(), "void");
    std::printf("program <line: %u, col: %u> %s %s\n",
                p_program.getLocation().line, p_program.getLocation().col,
                p_program.name.c_str(), "void");

    incrementIndentation();
    p_program.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(DeclNode &p_decl) {
    outputIndentationSpace(m_indentation);

    // std::printf("declaration <line: %u, col: %u>\n", p_decl.getLocation().line,
    //             p_decl.getLocation().col);
    std::printf("declaration <line: %u, col: %u>\n",
                p_decl.getLocation().line, p_decl.getLocation().col);

    incrementIndentation();
    p_decl.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(VariableNode &p_variable) {
    outputIndentationSpace(m_indentation);

    // TODO: name, type
    // std::printf("variable <line: %u, col: %u> %s %s\n",
    //             p_variable.getLocation().line, p_variable.getLocation().col,
    //             "TODO", "TODO");
    std::string var_type;
    // normal type and array
    if(p_variable.type->type == Type::TYPE_INTEGER){
        var_type = "integer";
    }
    else if(p_variable.type->type == Type::TYPE_REAL){
        var_type = "real";
    }
    else if(p_variable.type->type == Type::TYPE_STRING){
        var_type = "string";
    }
    else if(p_variable.type->type == Type::TYPE_BOOLEAN){
        var_type = "boolean";
    }

    //array
    if(p_variable.type->array_num.size() > 0){
        var_type += " ";
        for(uint i = 0; i < p_variable.type->array_num.size(); i++){
            var_type += "[";
            var_type += std::to_string(p_variable.type->array_num[i]);
            var_type += "]";
        }        
    }
    
    

    std::printf("variable <line: %u, col: %u> %s %s\n",
                p_variable.getLocation().line, p_variable.getLocation().col, p_variable.name, var_type.c_str());

    incrementIndentation();
    p_variable.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(ConstantValueNode &p_constant_value) {
    outputIndentationSpace(m_indentation);

    // TODO: string of constant value
    // std::printf("constant <line: %u, col: %u> %s\n",
    //             p_constant_value.getLocation().line,
    //             p_constant_value.getLocation().col,
    //             "TODO");
    if(p_constant_value.const_value->type == Type::TYPE_INTEGER){
        std::printf("constant <line: %u, col: %u> %d\n",
                    p_constant_value.getLocation().line,
                    p_constant_value.getLocation().col, p_constant_value.const_value->int_literal
                    );
    }
    else if(p_constant_value.const_value->type == Type::TYPE_REAL){
        std::printf("constant <line: %u, col: %u> %f\n",
                    p_constant_value.getLocation().line,
                    p_constant_value.getLocation().col, p_constant_value.const_value->real_literal
                    );
    }
    else if(p_constant_value.const_value->type == Type::TYPE_STRING){
        std::printf("constant <line: %u, col: %u> %s\n",
                p_constant_value.getLocation().line,
                p_constant_value.getLocation().col, p_constant_value.const_value->string_literal
                );
    }
    else if(p_constant_value.const_value->type == Type::TYPE_BOOLEAN){
        std::string bool_val;
        if(p_constant_value.const_value->boolean_literal == Boolean_type::booltype_True){
            bool_val = "true";
        }
        else if(p_constant_value.const_value->boolean_literal == Boolean_type::booltype_False){
            bool_val = "false";
        }
        std::printf("constant <line: %u, col: %u> %s\n",
                p_constant_value.getLocation().line,
                p_constant_value.getLocation().col, bool_val.c_str());
    }
}

void AstDumper::visit(FunctionNode &p_function) {
    outputIndentationSpace(m_indentation);

    // TODO: name, prototype string
    // std::printf("function declaration <line: %u, col: %u> %s %s\n",
    //             p_function.getLocation().line, p_function.getLocation().col,
    //             "TODO", "TODO");
    std::string func_type; 
    if(p_function.return_type->type == Type::TYPE_INTEGER){
        func_type = "integer";
    }
    else if(p_function.return_type->type == Type::TYPE_REAL){
        func_type = "real";
    }
    else if(p_function.return_type->type == Type::TYPE_STRING){
        func_type = "string";
    }
    else if(p_function.return_type->type == Type::TYPE_BOOLEAN){
        func_type = "boolean";
    }
    else if(p_function.return_type->type == Type::TYPE_VOID){
        func_type = "void";
    }

    //function parameter
    std::string func_parameter;
    func_parameter = "(";
    if(p_function.parameter_list->size() > 0){
        for(uint i = 0; i < p_function.parameter_list->size(); i++){
            
            if(p_function.parameter_list->at(i)->type == Type::TYPE_INTEGER){
                func_parameter += "integer";
            }
            else if(p_function.parameter_list->at(i)->type == Type::TYPE_REAL){
                func_parameter += "real";
            }
            else if(p_function.parameter_list->at(i)->type == Type::TYPE_STRING){
                func_parameter += "string";
            }
            else if(p_function.parameter_list->at(i)->type == Type::TYPE_BOOLEAN){
                func_parameter += "boolean";
            }

            if(p_function.parameter_list->at(i)->array_num.size() > 0){
            func_parameter += " ";
            for(uint j = 0; j < p_function.parameter_list->at(i)->array_num.size(); j++){
                //std::cout << "num:" << parameter_list->at(i)->array_num[i] << std::endl;
                func_parameter += "[";
                func_parameter += std::to_string(p_function.parameter_list->at(i)->array_num[j]);
                func_parameter += "]";
            }        
            }

            if(i != p_function.parameter_list->size() - 1){
                func_parameter += ", ";
            }
            
        }
        
    }
    func_parameter += ")";

    std::printf("function declaration <line: %u, col: %u> %s %s %s\n",
                p_function.getLocation().line, p_function.getLocation().col, p_function.name, func_type.c_str(), func_parameter.c_str());

    incrementIndentation();
    p_function.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(CompoundStatementNode &p_compound_statement) {
    outputIndentationSpace(m_indentation);

    // std::printf("compound statement <line: %u, col: %u>\n",
    //             p_compound_statement.getLocation().line,
    //             p_compound_statement.getLocation().col);
    std::printf("compound statement <line: %u, col: %u>\n",
                p_compound_statement.getLocation().line,
                p_compound_statement.getLocation().col);

    incrementIndentation();
    p_compound_statement.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(PrintNode &p_print) {
    outputIndentationSpace(m_indentation);

    // std::printf("print statement <line: %u, col: %u>\n",
    //             p_print.getLocation().line, p_print.getLocation().col);
    std::printf("print statement <line: %u, col: %u>\n",
                p_print.getLocation().line, p_print.getLocation().col);

    incrementIndentation();
    p_print.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(BinaryOperatorNode &p_bin_op) {
    outputIndentationSpace(m_indentation);

    // TODO: operator string
    // std::printf("binary operator <line: %u, col: %u> %s\n",
    //             p_bin_op.getLocation().line, p_bin_op.getLocation().col,
    //             "TODO");
    std::string op_type;

    if(p_bin_op.op->operator_type == Operator::OP_OR){
        op_type = "or";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_AND){
        op_type = "and";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_LESS){
        op_type = "<";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_LESS_OR_EQUAL){
        op_type = "<=";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_EQUAL){
        op_type = "=";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_GREATER){
        op_type = ">";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_GREATER_OR_EQUAL){
        op_type = ">=";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_NOT_EQUAL){
        op_type = "<>";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_PLUS){
        op_type = "+";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_MINUS){
        op_type = "-";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_MULTIPLY){
        op_type = "*";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_DEVIDE){
        op_type = "/";
    }
    else if(p_bin_op.op->operator_type == Operator::OP_MOD){
        op_type = "mod";
    }

    std::printf("binary operator <line: %u, col: %u> %s\n",
                p_bin_op.getLocation().line, p_bin_op.getLocation().col, op_type.c_str());


    incrementIndentation();
    p_bin_op.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(UnaryOperatorNode &p_un_op) {
    outputIndentationSpace(m_indentation);

    // TODO: operator string
    // std::printf("unary operator <line: %u, col: %u> %s\n",
    //             p_un_op.getLocation().line, p_un_op.getLocation().col,
    //             "TODO");
    std::string op_type;
    if(p_un_op.op->operator_type == Operator::OP_MINUS){
        op_type = "neg";
    }
    else if(p_un_op.op->operator_type == Operator::OP_NOT){
        op_type = "not";
    }

    std::printf("unary operator <line: %u, col: %u> %s\n",
                p_un_op.getLocation().line, p_un_op.getLocation().col, op_type.c_str());

    incrementIndentation();
    p_un_op.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(FunctionInvocationNode &p_func_invocation) {
    outputIndentationSpace(m_indentation);

    // TODO: function name
    // std::printf("function invocation <line: %u, col: %u> %s\n",
    //             p_func_invocation.getLocation().line,
    //             p_func_invocation.getLocation().col,
    //             "TODO");
    std::printf("function invocation <line: %u, col: %u> %s\n",
                p_func_invocation.getLocation().line,
                p_func_invocation.getLocation().col, p_func_invocation.name);

    incrementIndentation();
    p_func_invocation.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(VariableReferenceNode &p_variable_ref) {
    outputIndentationSpace(m_indentation);

    // TODO: variable name
    // std::printf("variable reference <line: %u, col: %u> %s\n",
    //             p_variable_ref.getLocation().line,
    //             p_variable_ref.getLocation().col,
    //             "TODO");
    std::printf("variable reference <line: %u, col: %u> %s\n",
                p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, p_variable_ref.name);

    incrementIndentation();
    p_variable_ref.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(AssignmentNode &p_assignment) {
    outputIndentationSpace(m_indentation);

    // std::printf("assignment statement <line: %u, col: %u>\n",
    //             p_assignment.getLocation().line,
    //             p_assignment.getLocation().col);
    std::printf("assignment statement <line: %u, col: %u>\n",
                p_assignment.getLocation().line, p_assignment.getLocation().col);

    incrementIndentation();
    p_assignment.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(ReadNode &p_read) {
    outputIndentationSpace(m_indentation);

    // std::printf("read statement <line: %u, col: %u>\n",
    //             p_read.getLocation().line, p_read.getLocation().col);
    std::printf("read statement <line: %u, col: %u>\n",
                p_read.getLocation().line, p_read.getLocation().col);

    incrementIndentation();
    p_read.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(IfNode &p_if) {
    outputIndentationSpace(m_indentation);

    // std::printf("if statement <line: %u, col: %u>\n", p_if.getLocation().line,
    //             p_if.getLocation().col);
    std::printf("if statement <line: %u, col: %u>\n",
                p_if.getLocation().line, p_if.getLocation().col);

    incrementIndentation();
    p_if.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(WhileNode &p_while) {
    outputIndentationSpace(m_indentation);

    // std::printf("while statement <line: %u, col: %u>\n",
    //             p_while.getLocation().line, p_while.getLocation().col);
    std::printf("while statement <line: %u, col: %u>\n",
                p_while.getLocation().line, p_while.getLocation().col);

    incrementIndentation();
    p_while.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(ForNode &p_for) {
    outputIndentationSpace(m_indentation);

    // std::printf("for statement <line: %u, col: %u>\n", p_for.getLocation().line,
    //             p_for.getLocation().col);
    std::printf("for statement <line: %u, col: %u>\n",
                p_for.getLocation().line, p_for.getLocation().col);

    incrementIndentation();
    p_for.visitChildNodes(*this);
    decrementIndentation();
}

void AstDumper::visit(ReturnNode &p_return) {
    outputIndentationSpace(m_indentation);

    // std::printf("return statement <line: %u, col: %u>\n",
    //             p_return.getLocation().line, p_return.getLocation().col);
    std::printf("return statement <line: %u, col: %u>\n",
                p_return.getLocation().line, p_return.getLocation().col);

    incrementIndentation();
    p_return.visitChildNodes(*this);
    decrementIndentation();
}

// FIXME: remove this line if you choose to use visitor pattern with this template
//#endif
