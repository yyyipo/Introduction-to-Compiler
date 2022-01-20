#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

size_t level;
string parent_record;
int in_function = 0;
int main_ = 0;
int s0_offset = -8;
int assign_right_constant;
string assigned_value;
string variable_ref_name;
int variable_ref_level, variable_ref_offset;
int func_parameter;
int if_compound = 0;
int label = 1;
int is_for_loop_variable = 0;
string for_loop_bound;
int is_constant_after_for = 0;
vector <int> for_loop_v_offset;

int in_for = 0;
vector <int> for_label;

CodeGenerator::CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *symbol_manager) 
    : in_file_name(in_file_name), symbol_manager(symbol_manager) {
    this->out_fp = fopen(out_file_name, "w");
    assert(this->out_fp != NULL && "fopen() fails");
}

CodeGenerator::~CodeGenerator() {
    fclose(this->out_fp);
}

void CodeGenerator::dumpInstrs(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->out_fp, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    s0_offset = -8;
    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    symbol_manager->reconstructHashTableFromSymbolTable(p_program.getSymbolTable());

    // Generate RISC-V instructions for program header
    dumpInstrs("%s%s%s%s", 
        "   .file \"", this->in_file_name, "\"\n",
        "   .option nopic\n\n"
    );

    parent_record = "program";
    p_program.visitChildNodes(*this);

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    string parent = parent_record;

    func_parameter = 0;

    parent_record = "decl";
    p_decl.visitChildNodes(*this);
    parent_record = parent;
}

void CodeGenerator::visit(VariableNode &p_variable) {
    string parent = parent_record;

    // get the level of current entry
    level = symbol_manager->lookup(p_variable.getNameCString())->getLevel();
    string kind = symbol_manager->lookup(p_variable.getNameCString())->getKindCString();    

    //global variable
    if(level == 0){
        if(kind == "Variable"){
            // integer
            dumpInstrs("%s%s%s%s", 
                "# declare golbal variable\n",
                ".comm ", p_variable.getNameCString(), ", 4, 4\n\n"
            );
        }
        if(kind == "Constant"){            
            dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
                "# declare global constant\n",
                ".section    .rodata\n",
                "    .align 2\n",
                "    .globl ", p_variable.getNameCString(), "\n",
                "    .type ", p_variable.getNameCString(), ", @object\n",
                p_variable.getNameCString(), ": \n", 
                "    .word ", p_variable.getConstantPtr()->getConstantValueCString(), "\n\n",
                "    li t0, ", assigned_value.c_str(), "\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );
        }
    }
    else{
        // set the offset of the local variable
        s0_offset -= 4;
        int *entry_offset = const_cast<int*>(&(symbol_manager->lookup(p_variable.getNameCString())->offset));
        *entry_offset = s0_offset;

        if(kind == "Parameter"){
            if(func_parameter < 8){
                dumpInstrs("%s%s%s%s%s", 
                    "    sw a", to_string(func_parameter).c_str(), ", ", to_string(*entry_offset).c_str(), "(s0)\n"
                );
            }
            else{
                dumpInstrs("%s%s%s%s", 
                    "    # push the parameter to the stack\n",
                    "    sw t3, ",to_string(*entry_offset).c_str(), "(s0)\n"
                );
            }
            func_parameter++;
        }   

        if(kind == "Constant"){
            dumpInstrs("%s%s%s%s%s", 
                "    addi t0, s0, ", to_string(*entry_offset).c_str(), "\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"     
            );
        }
    }
    // visit child node 
    parent_record = "variable";
    p_variable.visitChildNodes(*this);
    parent_record = parent;

    //local variable
    if(level > 0){
        if(kind == "Constant"){   
            dumpInstrs("%s%s%s%s%s", 
                "    lw t0, 0(sp)\n",
                "    addi sp, sp, 4\n",
                "    lw t1, 0(sp)\n",
                "    addi sp, sp, 4\n",
                "    sw t0, 0(t1)\n\n"
            );
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    string parent = parent_record;

    if(parent == "bin_op"){
        dumpInstrs("%s%s%s%s%s%s",
            "    # push the constant to the stack\n",
            "    li t0, ", p_constant_value.getConstantValueCString(), "\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }
    else if(parent == "un_op"){
        dumpInstrs("%s%s%s%s%s%s",
            "    # push the constant to the stack\n",
            "    li t0, -", p_constant_value.getConstantValueCString(), "\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );

    }
    else{
        // global 
        if(level == 0){
            if(parent == "assignment"){
                dumpInstrs("%s%s%s%s%s%s",
                    "    # push the global constant to the stack\n",
                    "    li t0, ", p_constant_value.getConstantValueCString(), "\n",
                    "    addi sp, sp, -4\n",
                    "    sw t0, 0(sp)\n"
                );
            }
        }
        // local 
        else{
            dumpInstrs("%s%s%s%s%s%s",
                "    # push the local constant to the stack\n",
                "    li t0, ", p_constant_value.getConstantValueCString(), "\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );

            
            if(is_constant_after_for){
                int current_label = for_label.back();
                for_label.pop_back();
                
                dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s", 
                    "    # pop two values from stack\n",
                    "    lw t0, 0(sp)\n",
                    "    addi sp, sp, 4\n",
                    "    lw t1, 0(sp)\n",
                    "    addi sp, sp, 4\n",
                    "    # check whether to exit the loop\n",
                    "    bge t1, t0, L", to_string(current_label + 1).c_str(), "\n",
                    "L", to_string(current_label).c_str(), ":\n"
                );
                current_label++;
                for_label.push_back(current_label);
                is_constant_after_for = 0;
            }            
        }
    } 
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());
    s0_offset = -8;

    in_function = 1;
    string parent = parent_record;

    int *function_param = const_cast<int*>(&(symbol_manager->lookup(p_function.getNameCString())->function_parameter_num));
    *function_param = p_function.getParametersNum(p_function.getParameters());
    
    dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
        ".section    .text\n",
        "    .align 2\n",
        "    .globl ", p_function.getNameCString(), "\n",
        "    .type ", p_function.getNameCString(), ", @function\n",
        p_function.getNameCString(), ":\n",
        "    addi sp, sp, -128\n",
        "    sw ra, 124(sp)\n",
        "    sw s0, 120(sp)\n",
        "    addi s0, sp, 128\n\n"
    );

    /*if(*function_param > 8){
        for(int i = 8; i < *function_param; i++){
            dumpInstrs("%s%s%s",
                "    # pop a value from the stack\n",
                "    lw t3, 0(sp)\n",      
                "    addi sp, sp, 4\n"
            );
        }
    }*/

    parent_record = "function";
    p_function.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s%s%s%s%s%s%s", 
        "    lw ra, 124(sp)\n",
        "    lw s0, 120(sp)\n",
        "    addi sp, sp, 128\n",
        "    jr ra\n",
        "    .size ", p_function.getNameCString(), ", .-", p_function.getNameCString(), "\n\n"
    );

    in_function = 0;
    
    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_compound_statement.getSymbolTable());

    string parent = parent_record;
    int is_main = 0;

    // in if statement
    if(parent == "if"){
        
        dumpInstrs("%s%s%s", 
            "L", to_string(label).c_str(), ":\n" 
        );
        if_compound--;
        label++;
    }
    // not in if statement
    else if(in_function == 0 && main_ == 0){
        s0_offset = -8;
        dumpInstrs("%s%s%s%s%s%s%s%s%s%s", 
            ".section    .text\n",
            "    .align 2\n",
            "    .globl main\n",
            "    .type main, @function\n",
            "main:\n",
            "    # in the function prologue\n",
            "    addi sp, sp, -128\n",
            "    sw ra, 124(sp)\n",
            "    sw s0, 120(sp)\n",
            "    addi s0, sp, 128\n\n"
        );
        main_ = 1;
        is_main = 1;
    }    
    
    parent_record = "compound_statement";
    p_compound_statement.visitChildNodes(*this);
    parent_record = parent;


    if(is_main == 1){
        dumpInstrs("%s%s%s%s%s%s", 
            "    # in the function epilogue\n",
            "    lw ra, 124(sp)\n",
            "    lw s0, 120(sp)\n",
            "    addi sp, sp, 128\n",
            "    jr ra\n",
            "    .size main, .-main\n\n"
        );
    }

    if(parent == "if"){
        // L1
        if(if_compound != 0){
            dumpInstrs("%s%s%s", 
                "    j L", to_string(label + 1).c_str(), "\n"
            );
        }
    }
    else if(parent == "for"){
        int current_label = for_label.back();
        for_label.pop_back();
        int current_offset = for_loop_v_offset.back();

        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
            "    # for loop job\n",
            "    addi t0, s0, ", to_string(current_offset).c_str(), "\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n",
            "    lw t0, ", to_string(current_offset).c_str(), "(s0)\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n",
            "    li t0, 1\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n",
            "    lw t0, 0(sp)\n",
            "    addi sp, sp, 4\n",
            "    lw t1, 0(sp)\n",
            "    addi sp, sp, 4\n",
            "    add t0, t1, t0\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n",    
            "    lw t0, 0(sp)\n",
            "    addi sp, sp, 4\n",
            "    lw t1, 0(sp)\n",
            "    addi sp, sp, 4\n",
            "    sw t0, 0(t1)\n",
            "    j L", to_string(current_label - 2).c_str(), "\n",
            "L", to_string(current_label).c_str(), ":\n"
        );
        current_label++;
        for_label.push_back(current_label);
    }

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    string parent = parent_record;

    parent_record = "print";
    p_print.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s%s",
        "    # print\n",
        "    lw a0, 0(sp)\n",
        "    addi sp, sp, 4\n",
        "    jal ra, printInt\n\n"
    );      
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    string parent = parent_record;
    
    parent_record = "bin_op";
    p_bin_op.visitChildNodes(*this);
    parent_record = parent;

    string bin_operator = p_bin_op.getOpCString();

    //pop 2 values from the stack
    dumpInstrs("%s%s%s%s%s",
        "    # pop two values from the stack\n",
        "    lw t0, 0(sp)\n",      
        "    addi sp, sp, 4\n",
        "    lw t1, 0(sp)\n",     
        "    addi sp, sp, 4\n"
    );

    // do the operation
    if(bin_operator == "+"){
        dumpInstrs("%s%s",
            "    # do the operation\n",
            "    add t0, t1, t0\n"
        );
        dumpInstrs("%s%s%s",
            "    # push the value to the stack\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }
    else if(bin_operator == "-"){
        dumpInstrs("%s%s",
            "    # do the operation\n",
            "    sub t0, t1, t0\n"
        );
        dumpInstrs("%s%s%s",
            "    # push the value to the stack\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }
    else if(bin_operator == "*"){
        dumpInstrs("%s%s",
            "    # do the operation\n",
            "    mul t0, t1, t0\n"
        );
        dumpInstrs("%s%s%s",
            "    # push the value to the stack\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }
    else if(bin_operator == "/"){
        dumpInstrs("%s%s",
            "    # do the operation\n",
            "    div t0, t1, t0\n"
        );
        dumpInstrs("%s%s%s",
            "    # push the value to the stack\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }
    else if(bin_operator == "mod"){
        dumpInstrs("%s%s",
            "    # do the operation\n",
            "    rem t0, t1, t0\n"
        );
        dumpInstrs("%s%s%s",
            "    # push the value to the stack\n",
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
    }

    else if(bin_operator == "="){
        dumpInstrs("%s%s%s%s",
            "    # do the operation\n",
            "    bne t1, t0, L", to_string(label + 1).c_str(), "\n"
        );
    }
    else if(bin_operator == ">"){
        dumpInstrs("%s%s%s%s",
            "    # do the operation\n",
            "    ble t1, t0, L", to_string(label + 1).c_str(), "\n"
        );
    }
    else if(bin_operator == "<"){
        dumpInstrs("%s%s%s%s",
            "    # do the operation\n",
            "    bge t1, t0, L", to_string(label + 1).c_str(), "\n"
        );
    }
    else if(bin_operator == ">="){
        dumpInstrs("%s%s%s%s",
            "    # do the operation\n",
            "    blt t1, t0, L", to_string(label + 1).c_str(), "\n"
        );
    }
    else if(bin_operator == "<="){
        dumpInstrs("%s%s%s%s",
            "    # do the operation\n",
            "    bgt t1, t0, L", to_string(label + 1).c_str(), "\n"
        );
    }
    if(parent == "while"){
        dumpInstrs("%s%s%s", 
            "L", to_string(label).c_str(), ":\n" 
        );
        label++;
    }
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    string parent = parent_record;
    
    parent_record = "un_op";
    p_un_op.visitChildNodes(*this);
    parent_record = parent;    
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    string parent = parent_record;

    parent_record = "func_invocation";
    p_func_invocation.visitChildNodes(*this);
    parent_record = parent;


    int parameter_num = symbol_manager->lookup(p_func_invocation.getNameCString())->function_parameter_num;
    for(int i = parameter_num - 1; i >= 0; i--){
        if(i >= 8){
            dumpInstrs("%s%s%s", 
                "    # pop the parameter value from the stack\n",
                "    lw t3, 0(sp)\n",
                "    addi sp, sp, 4\n"
            );
        }
        else{
            dumpInstrs("%s%s%s%s%s",
                "    # pop the value from the stack to argument register\n",
                "    lw a", to_string(i).c_str(), ", 0(sp)\n",
                "    addi sp, sp, 4\n"
            );
        }
        
    }
    /*for(int i = 8; i < parameter_num; i++){
        dumpInstrs("%s%s%s",
            "    # push the parameter value to the stack\n", 
            "    addi sp, sp, -4\n",      
            "    sw t3, 0(sp)\n"
        );
    }*/

    dumpInstrs("%s%s%s%s%s%s%s",
        "    # call the function, and push the return value to the stack\n",
        "    jal ra, ", p_func_invocation.getNameCString(), "\n",
        "    mv t0, a0\n",
        "    addi sp, sp, -4\n",
        "    sw t0, 0(sp)\n"
    );
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    string parent = parent_record;

    parent_record = "variable_ref";
    p_variable_ref.visitChildNodes(*this);
    parent_record = parent;


    int level = symbol_manager->lookup(p_variable_ref.getNameCString())->getLevel();
    string kind = symbol_manager->lookup(p_variable_ref.getNameCString())->getKindCString();
    int variable_offset = symbol_manager->lookup(p_variable_ref.getNameCString())->offset;

    if(parent == "assignment"){
        // global
        if(level == 0){
            dumpInstrs("%s%s%s%s%s%s", 
                "    # assign value to gloabal variable\n",
                "    la t0, ", p_variable_ref.getNameCString(), "\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );

        }
        // local
        else{
            dumpInstrs("%s%s%s%s%s%s", 
                "    # assign value to local variable/constant\n",
                "    addi t0, s0, ", to_string(variable_offset).c_str(), "\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );
        }
    }
    else if(parent == "print" || parent == "bin_op" || parent == "return" || parent == "func_invocation"){
        // global
        if(level == 0){
             dumpInstrs("%s%s%s%s%s%s%s%s", 
                "    # push the global variable/constant value to the stack\n",
                "    la t0, ", p_variable_ref.getNameCString(), "\n",
                "    lw t1, 0(t0)\n",
                "    mv t0, t1\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            ); 
        }
        // local
        else{
            dumpInstrs("%s%s%s%s%s%s", 
                "    # push the local variable/constant value to the stack\n",
                "    lw t0, ", to_string(variable_offset).c_str(),"(s0)\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );
        }
    }
    else if(parent == "read"){
        // global
        if(level == 0){
            dumpInstrs("%s%s%s%s%s%s", 
                "    # load the address of the global variable to t0\n",
                "    la t0, ", p_variable_ref.getNameCString(),"\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );
        }
        // local
        else{
            dumpInstrs("%s%s%s%s%s%s", 
                "    # load the address of the local variable to t0\n",
                "    la t0, ", to_string(variable_offset).c_str(),"(s0)\n",
                "    addi sp, sp, -4\n",
                "    sw t0, 0(sp)\n"
            );
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    string parent = parent_record;

    level = symbol_manager->lookup(p_assignment.getLvalue()->getNameCString())->getLevel();

    parent_record = "assignment";
    p_assignment.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s%s%s%s", 
            "    # save the value into the variable\n",
            "    lw t0, 0(sp)       # pop the value from the stack\n",
            "    addi sp, sp, 4\n",
            "    lw t1, 0(sp)       # pop the address from the stack\n",
            "    addi sp, sp, 4\n",
            "    sw t0, 0(t1)\n\n"
        );
    
    if(parent == "for" && is_for_loop_variable == 1){

        int variable_offset = symbol_manager->lookup(p_assignment.getLvalue()->getNameCString())->offset;
        int current_label = for_label.back();
        for_label.pop_back();

        dumpInstrs("%s%s%s%s%s%s%s%s%s", 
            "L", to_string(current_label).c_str(), ":\n",
            "    # load the value of is_for_loop_variable, and push the value to the stack\n",
            "    lw t0, ", to_string(variable_offset).c_str(), "(s0)\n",        
            "    addi sp, sp, -4\n",
            "    sw t0, 0(sp)\n"
        );
        current_label++;
        for_label.push_back(current_label);

        is_for_loop_variable = 0;
        is_constant_after_for = 1;
        //for_loop_v_offset = variable_offset;
        for_loop_v_offset.push_back(variable_offset);
    }
}

void CodeGenerator::visit(ReadNode &p_read) {
    string parent = parent_record;

    parent_record = "read";
    p_read.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s%s%s%s", 
        "    # call function 'readInt'\n",
        "    jal ra, readInt\n",
        "    # pop the address from the stack, and store the return value to 'a'\n",
        "    lw t0, 0(sp)\n",   
        "    addi sp, sp, 4\n",
        "    sw a0, 0(t0)\n\n"
    );
}

void CodeGenerator::visit(IfNode &p_if) {
    string parent = parent_record;

    
    if(p_if.else_body != nullptr){
        if_compound = 2;
    }

    parent_record = "if";
    p_if.visitChildNodes(*this);
    parent_record = parent;

    
    dumpInstrs("%s%s%s", 
        "L", to_string(label).c_str(), ":\n"
    );
    label++;
}

void CodeGenerator::visit(WhileNode &p_while) {
    string parent = parent_record;

    dumpInstrs("%s%s%s", 
        "L", to_string(label).c_str(), ":\n"
    );
    label++;
    int while_condition_label;
    while_condition_label = label - 1;
    
    parent_record = "while";
    p_while.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s", 
        "    j L", to_string(while_condition_label).c_str(), "\n"
    );

    dumpInstrs("%s%s%s", 
        "L", to_string(label).c_str(), ":\n"
    );

    label++;    
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_for.getSymbolTable());
    for_label.push_back(label);

    string parent = parent_record;  

    label += 3;    

    is_for_loop_variable = 1;
    for_loop_bound = p_for.getUpperBoundNode()->getConstantValueCString();

    parent_record = "for";
    p_for.visitChildNodes(*this);
    parent_record = parent;

    for_label.pop_back();
    for_loop_v_offset.pop_back();

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
    string parent = parent_record;

    parent_record = "return";
    p_return.visitChildNodes(*this);
    parent_record = parent;

    dumpInstrs("%s%s%s%s%s", 
        "    # pop the value from the stack\n",
        "    lw t0, 0(sp)\n",      
        "    addi sp, sp, 4\n",
        "    # load the value of variable to the return value register 'a0'\n",
        "    mv a0, t0\n"
    );    
}
