#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <iostream>

typedef std::vector<std::unique_ptr<ExpressionNode>> Exprs;

SymbolTable *current_table;
SymbolEntry *current_entry;
stack <string> previous_nodes;
int level = 0;
vector <string> for_loop_var;
vector <SymbolEntry*> for_loop_entry;
vector <SymbolEntry*> global_symbol;
vector <SymbolEntry*> outer_symbol;
vector <int> outer_symbol_num;
vector <string> for_loop_range;
vector <int> binary_op_dimension;
vector <int> unary_op_dimension;
string unary_operand, print_type;
string function_return_type = "not in function";
int function_return_dimension;
int return_type_wrong = 0;

vector <string> function_invocation_parameter_type;

int not_integer = 0;
int assign_const = 0;
int array_assignment = 0;
int assign_loopvar = 0;
int in_for_loop = 0;


extern char* code[512];
extern int error_found;
extern int32_t opt_dum;

SymbolEntry::SymbolEntry(){}

SymbolEntry::SymbolEntry(string entry_name, Kind entry_kind, int entry_level, string entry_type, string entry_attribute, int entry_error)
    :name(entry_name), kind(entry_kind), level(entry_level), type(entry_type), attribute(entry_attribute), error(entry_error) {}

SymbolTable::SymbolTable() {}


void SymbolTable ::addSymbol(SymbolEntry *new_entry){
    // push the entry into the symbol table
    this->entries.push_back(new_entry);

    // if it is a gloabal symbol, push it into the vector global_symbol
    if(new_entry->level == 0){

        global_symbol.push_back(new_entry);
       
    }
} 

void SemanticAnalyzer::pushScope(SymbolTable *new_scope){
    this->tables.push(new_scope);
}
void SemanticAnalyzer::popScope(){
    // pop the table
    this->tables.pop();
}

void SemanticAnalyzer::print_entry(SymbolEntry *cur_entry){
    string kind;
    if(cur_entry->kind == Kind::KIND_PROGRAM)   
        kind = "program";
    else if(cur_entry->kind == Kind::KIND_FUNCTION)
        kind = "function";
    else if(cur_entry->kind == Kind::KIND_PARAMETER)
        kind = "parameter";
    else if(cur_entry->kind == Kind::KIND_VARIABLE)
        kind = "variable";
    else if(cur_entry->kind == Kind::KIND_LOOP_VAR)
        kind = "loop_var";
    else if(cur_entry->kind == Kind::KIND_CONSTANT)
        kind = "constant";

    string global_local;
    if(level == 0)
        global_local = "(global)";
    else
        global_local = "(local)";


    printf("%-33s", cur_entry->name.c_str());
    printf("%-11s", kind.c_str());
    printf("%d%-10s", level, global_local.c_str());
    printf("%-17s", cur_entry->type.c_str());
    printf("%-11s", cur_entry->attribute.c_str());
    puts("");
}
void SemanticAnalyzer::print_table(SymbolTable *cur_table){
    if(opt_dum == 1){
        // print the table out
        printf("%s", "==============================================================================================================\n");
        printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
        printf("%s", "--------------------------------------------------------------------------------------------------------------\n");
        for(uint i = 0; i < cur_table->entries.size(); i++){
            print_entry(cur_table->entries[i]);        
        }
        printf("%s", "--------------------------------------------------------------------------------------------------------------\n");
    }

}

void SemanticAnalyzer::print_arrow(int len) {
    for(int i = 1; i < len + 4; ++i) {
        std::cerr << " ";
    }
    std::cerr << "^\n";
}


void SemanticAnalyzer::visit(ProgramNode &p_program) {
    //cout << opt_dum << endl;
    // create a new symbol table
    SymbolTable *new_table = new SymbolTable();
    // point the current table pointer to the new table
    current_table = new_table;    
    // push the new symbol into the scope
    pushScope(new_table);

    // create a symbol entry
    SymbolEntry *new_entry = new SymbolEntry(p_program.getNameCString(), Kind::KIND_PROGRAM, level, 
                                            p_program.getTypeCString(), "", 0);
    // point the current entry pointer to the new entry
    current_entry = new_entry;

    // insert the symbol entry into current table
    current_table->addSymbol(new_entry);         
    
    previous_nodes.push("program");
    //visit child node
    p_program.visitChildNodes(*this);
    previous_nodes.pop();

    // print the table out
    print_table(current_table);
    
    popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    //printf("decl\n");

    previous_nodes.push("decl");
    //visit child node
    p_decl.visitChildNodes(*this);
    previous_nodes.pop();

    if(previous_nodes.top() == "function" && current_entry->kind == Kind::KIND_VARIABLE){
        current_entry->kind = Kind::KIND_PARAMETER;
    }

    //printf("decl_end\n");
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    //printf("Variable\n");

    int redeclaration = 0;
    for(uint i = 0; i < current_table->entries.size(); i++){
        if(current_table->entries[i]->name == p_variable.getNameCString()){
            redeclaration = 1;
            error_found = 1;
            current_entry->error = 1;

            cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
                 << ": symbol \'"  << p_variable.getNameCString() << "\' is redeclared\n";
            cerr << "    " <<  code[p_variable.getLocation().line] << endl;
            print_arrow(p_variable.getLocation().col);

            break;
        }
    }
    for(uint i = 0; i < for_loop_var.size(); i++){
        if(for_loop_var[i] == p_variable.getNameCString()){
            redeclaration = 1;
            error_found = 1;
            current_entry->error = 1;

            cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
                 << ": symbol \'"  << p_variable.getNameCString() << "\' is redeclared\n";
            cerr << "    " <<  code[p_variable.getLocation().line] << endl;
            print_arrow(p_variable.getLocation().col);

            break;
        }
    }

    if(redeclaration == 0){
        // create a symbol entry
        SymbolEntry *new_entry = new SymbolEntry(p_variable.getNameCString(), Kind::KIND_VARIABLE, level, 
                                                p_variable.getTypeCString(), "", 0);
        // point the current entry pointer to the new entry
        current_entry = new_entry;

        // insert the symbol entry into current table
        current_table->addSymbol(new_entry);
    }


    // check dimension in array is greater than 0
    vector<uint64_t> array_dimensions = p_variable.getDimesions();
    current_entry->dimension = array_dimensions.size();

    for(uint i = 0; i < array_dimensions.size(); i++) {
        if(current_entry->error == 0 && array_dimensions[i] == 0){
            error_found = 1;
            current_entry->error = 1;

            cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
                 << ": \'"  << p_variable.getNameCString() << "\' declared as an array with an index that is not greater than 0\n";
            cerr << "    " <<  code[p_variable.getLocation().line] << endl;
            print_arrow(p_variable.getLocation().col);
        }
    }


    previous_nodes.push("variable");
    //visit child node
    p_variable.visitChildNodes(*this);
    previous_nodes.pop();

    //printf("Variable_end\n");
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    //printf("Constant_value\n");
   
    if(previous_nodes.top() == "variable" && current_entry->kind == Kind::KIND_VARIABLE){
        current_entry->kind = Kind::KIND_CONSTANT;
        current_entry->attribute = p_constant_value.getConstantValueCString();        
    }
    else if(previous_nodes.top() == "for"){
        current_entry->kind = Kind::KIND_LOOP_VAR;
        current_entry->attribute = "";

        for_loop_var.push_back(current_entry->name);
        for_loop_entry.push_back(current_entry);
    }
    else if(previous_nodes.top() == "variable_reference"){
        string type = p_constant_value.getTypeCString();

        if(current_entry->error == 0 && type != "integer"){
            error_found = 1;
            not_integer = 1;
            cerr << "<Error> Found in line " << p_constant_value.getLocation().line << ", column " << p_constant_value.getLocation().col 
            << ": index of array reference must be an integer\n";
            cerr << "    " <<  code[p_constant_value.getLocation().line] << endl;
            print_arrow(p_constant_value.getLocation().col);
            not_integer = 0;

        }
    }
    // for binary operator and unary operator
    else if(previous_nodes.top() == "binary_op" || previous_nodes.top() == "unary_op" || previous_nodes.top() == "assignment" || previous_nodes.top() == "if"){
        p_constant_value.setType(p_constant_value.getTypePtr());
        if(previous_nodes.top() == "binary_op")     
            binary_op_dimension.push_back(0);
        else if(previous_nodes.top() == "unary_op"){
            unary_op_dimension.push_back(0);
            unary_operand = p_constant_value.getTypeCString();
        }
    }
    else if(previous_nodes.top() == "function_invocation"){
        function_invocation_parameter_type.push_back(p_constant_value.getTypeCString());
    }

    if((signed) for_loop_range.size() < 2 * in_for_loop){
        //cout << p_constant_value.getConstantValueCString() << endl;
        for_loop_range.push_back(p_constant_value.getConstantValueCString());
    }


    //printf("Constant_value_end\n");
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    //printf("Function\n");

    

    int redeclaration = 0;
    for(uint i = 0; i < current_table->entries.size(); i++){
        if(current_entry->error == 0 && current_table->entries[i]->name == p_function.getNameCString()){
            redeclaration = 1;
            error_found = 1;
            cerr << "<Error> Found in line " << p_function.getLocation().line << ", column " << p_function.getLocation().col 
                 << ": symbol \'"  << p_function.getNameCString() << "\' is redeclared\n";
            cerr << "    " <<  code[p_function.getLocation().line] << endl;
            print_arrow(p_function.getLocation().col);

            current_entry->error = 1;

            break;
        }
    }
    for(uint i = 0; i < for_loop_var.size(); i++){
        if(current_entry->error == 0 && for_loop_var[i] == p_function.getNameCString()){
            redeclaration = 1;
            error_found = 1;
            cerr << "<Error> Found in line " << p_function.getLocation().line << ", column " << p_function.getLocation().col 
                 << ": symbol \'"  << p_function.getNameCString() << "\' is redeclared\n";
            cerr << "    " <<  code[p_function.getLocation().line] << endl;
            print_arrow(p_function.getLocation().col);

            current_entry->error = 1;

            break;
        }
    }

    if(redeclaration == 0){
        // create a symbol entry
        SymbolEntry *new_entry = new SymbolEntry(p_function.getNameCString(), Kind::KIND_FUNCTION, level, 
                                                p_function.getTypeCString(), p_function.getAttributeCString(), 0);
        
        // point the current entry pointer to the new entry
        current_entry = new_entry;

        // insert the symbol entry into current table
        current_table->addSymbol(new_entry);

        function_return_type = current_entry->type;
        function_return_dimension = current_entry->dimension;
    }


    // create a new symbol table
    SymbolTable *new_table = new SymbolTable();    
    // push the new symbol into the scope
    pushScope(new_table); 

    for(uint i = 0; i < current_table->entries.size(); i++){
        outer_symbol.push_back(current_table->entries[i]);
    }
    outer_symbol_num.push_back(current_table->entries.size());

    // point the current table pointer to the new table
    current_table = new_table;
    // add 1 to level
    level++;

    previous_nodes.push("function");
    //visit child node
    p_function.visitChildNodes(*this);
    previous_nodes.pop();

    if(redeclaration == 0){
        function_return_type = "not in function";
    }

    // print the table out
    print_table(current_table); 

    popScope();
    current_table = tables.top();
    level--;

    for(int i = 0; i < outer_symbol_num.back(); i++){
        outer_symbol.pop_back();
    }
    outer_symbol_num.pop_back();
    
    //printf("Function_end\n");
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    //printf("Compound\n");
    if(previous_nodes.top() == "function"){
        previous_nodes.push("compound");
        //visit child node
        p_compound_statement.visitChildNodes(*this);
        previous_nodes.pop();
    }
    else{     

        // create a new symbol table
        SymbolTable *new_table = new SymbolTable();    
        // push the new symbol into the scope
        pushScope(new_table);    

        for(uint i = 0; i < current_table->entries.size(); i++){
            outer_symbol.push_back(current_table->entries[i]);
        }
        outer_symbol_num.push_back(current_table->entries.size());

        // point the current table pointer to the new table
        current_table = new_table;
        // add 1 to level
        level++;
        previous_nodes.push("compound");
        //visit child node
        p_compound_statement.visitChildNodes(*this);
        previous_nodes.pop();

        // print the table out
        print_table(current_table);

        popScope();
        current_table = this->tables.top();
        level--;

        for(int i = 0; i < outer_symbol_num.back(); i++){
            outer_symbol.pop_back();
        }
        outer_symbol_num.pop_back(); 
    }

    //printf("Compound_end\n");
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    //printf("print\n");
    previous_nodes.push("print");
    //visit child node
    p_print.visitChildNodes(*this);
    previous_nodes.pop();

    //printf("print_end\n");
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    //printf("bin_op\n");


    previous_nodes.push("binary_op");
    //visit child node
    p_bin_op.visitChildNodes(*this);
    previous_nodes.pop();


    int find_wrong = 0;

    string op = p_bin_op.getOpCString();
    string left_type = p_bin_op.left->getTypeCString();
    string right_type = p_bin_op.right->getTypeCString();
    int right_dimension = binary_op_dimension.back();
    binary_op_dimension.pop_back();
    int left_dimension = binary_op_dimension.back();
    binary_op_dimension.pop_back();


    
    if(op == "+"){
        if(left_type == "string" && right_type == "string" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "integer" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "real" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "real" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "integer" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)            
            p_bin_op.setType(p_bin_op.right->getType());
        else if(left_type == "null" || right_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;            
        }        
    }
    else if(op == "-" || op == "*" || op == "/"){
        if(left_type == "integer" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "real" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "real" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "integer" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)            
            p_bin_op.setType(p_bin_op.right->getType());
        else if(left_type == "null" || right_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;
        }        
    }
    else if(op == "mod"){
        if(left_type == "integer" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "null" || right_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;
        }        
    }
    else if(op == "and" || op == "or" || op == "not"){
        if(left_type == "boolean" &&  right_type == "boolean" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(p_bin_op.left->getType());
        else if(left_type == "null" || right_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;
        }        
    }
    else if(op == "<" || op == "<=" || op == "=" || op == ">=" || op == ">" || op == "<>"){
        if(left_type == "integer" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(left_type == "real" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(left_type == "real" &&  right_type == "integer" && left_dimension == 0 && right_dimension == 0)
            p_bin_op.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(left_type == "integer" &&  right_type == "real" && left_dimension == 0 && right_dimension == 0)            
            p_bin_op.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(left_type == "null" || right_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;
        }              
    }

    if(find_wrong == 1){
        p_bin_op.setType(nullptr);

        left_type = "\'" + left_type;
        for(int i = 0; i < left_dimension; i++){
            if(i == 0) 
                left_type += " ";
            left_type += "[1]";
        }
        left_type += "\'";
        right_type = "\'" + right_type;
        for(int i = 0; i < right_dimension; i++){
            if(i == 0) 
                right_type += " ";
            right_type += "[1]";
        }
        right_type += "\'";

        error_found = 1;
        cerr << "<Error> Found in line " << p_bin_op.getLocation().line << ", column " << p_bin_op.getLocation().col 
                << ": invalid operands to binary operator \'" << op << "\' (" + left_type + " and " + right_type + ")\n";
        cerr << "    " <<  code[p_bin_op.getLocation().line] << endl;
        print_arrow(p_bin_op.getLocation().col);
    }



    if(previous_nodes.top() == "binary_op"){
        binary_op_dimension.push_back(0);
    }
    

    //printf("bin_op_end\n");
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    //printf("un_op\n");

    previous_nodes.push("unary_op");
    //visit child node    
    p_un_op.visitChildNodes(*this);
    previous_nodes.pop();

    int find_wrong = 0;

    string op = p_un_op.getOpCString();
    string operand_type = p_un_op.operand->getTypeCString();
    int operand_dimension = unary_op_dimension.back();
    unary_op_dimension.pop_back();


    
    if(op == "-"){
        if(operand_type == "integer"&& operand_dimension == 0)
            p_un_op.setType(p_un_op.operand->getType());
        else if(operand_type == "real" && operand_dimension == 0)
            p_un_op.setType(p_un_op.operand->getType());
        else if(operand_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;            
        }        
    }
    else if(op == "not"){
        if(operand_type == "boolean"&& operand_dimension == 0)
            p_un_op.setType(p_un_op.operand->getType());
        else if(operand_type == "null"){}
        else{
            error_found = 1;
            find_wrong = 1;            
        }
    }

    if(find_wrong == 1){
        p_un_op.setType(nullptr);

        operand_type = "\'" + operand_type;
        for(int i = 0; i < operand_dimension; i++){
            if(i == 0) 
                operand_type += " ";
            operand_type += "[1]";
        }
        operand_type += "\'";

        error_found = 1;
        cerr << "<Error> Found in line " << p_un_op.getLocation().line << ", column " << p_un_op.getLocation().col 
                << ": invalid operand to unary operator \'" << op << "\' (\'" + unary_operand + "\')\n";
        cerr << "    " <<  code[p_un_op.getLocation().line] << endl;
        print_arrow(p_un_op.getLocation().col);
    }
    
    if(previous_nodes.top() == "unary_op"){
        unary_op_dimension.push_back(0);
    }

    //printf("un_op_end\n");
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    //printf("func_invocation\n");
    function_invocation_parameter_type.clear();

    previous_nodes.push("function_invocation");
    //visit child node
    p_func_invocation.visitChildNodes(*this);
    previous_nodes.pop();

    

    int use_undeclared_symbol = 1;
    int use_non_function_symbol = 0;
    int argument_num_wrong = 0;
    int argument_type_wrong = 0;
    int type_wrong_col;

    string func_name = p_func_invocation.getNameCString(); 

    // for binary operator and unary operator
    if(previous_nodes.top() == "binary_op" || previous_nodes.top() == "unary_op" || previous_nodes.top() == "assignment" || previous_nodes.top() == "if"){
        string func_name = p_func_invocation.getNameCString();    
        for(uint i = 0; i < global_symbol.size(); i++){
            if(global_symbol[i]->name == func_name){
                current_entry = global_symbol[i];
                break;
            }
        }
        for(uint i = 0; i < current_table->entries.size(); i++){
            if(current_table->entries[i]->name == func_name){
                current_entry = current_table->entries[i];            
                break;
            }     
        }
        if(current_entry->type == "void")
            p_func_invocation.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kVoidType));
        else if(current_entry->type == "integer")
            p_func_invocation.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kIntegerType));
        else if(current_entry->type == "real")
            p_func_invocation.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kRealType));
        else if(current_entry->type == "bool")
            p_func_invocation.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(current_entry->type == "string")
            p_func_invocation.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kStringType));
        
        if(previous_nodes.top() == "binary_op")     
            binary_op_dimension.push_back(0);
        else if(previous_nodes.top() == "unary_op"){
            unary_op_dimension.push_back(0);
        }
    }
              
    for(uint i = 0; i < global_symbol.size(); i++){
        if(global_symbol[i]->name == func_name){
            current_entry = global_symbol[i];
            // print("change_entry");
            // print_entry(current_entry);

            if(current_entry->kind == Kind::KIND_FUNCTION){
                use_undeclared_symbol = 0;
            }
            else{
                use_non_function_symbol = 1;
                use_undeclared_symbol = 0;
                current_entry->error = 1;
            }
            break;
        }
    }
    for(uint i = 0; i < current_table->entries.size(); i++){
        if(current_table->entries[i]->name == func_name){
            current_entry = current_table->entries[i];
            if(current_entry->kind == Kind::KIND_FUNCTION){
                use_undeclared_symbol = 0;                    
            }
            else{
                use_non_function_symbol = 1;
                use_undeclared_symbol = 0;
                current_entry->error = 1;
            }
            break;
        }     
    }


    // previous_nodes.push("function_invocation");
    // //visit child node
    // p_func_invocation.visitChildNodes(*this);
    // previous_nodes.pop();

    

    string wrong_type, correct_type;
    if(use_undeclared_symbol == 0 && use_non_function_symbol == 0){
        int start = 0;
        int end;
        string type_tmp;
        string attribute = current_entry->attribute;
        vector <string> argument_type;
        
        while(attribute.find(", ", start) != string::npos){
            end = current_entry->attribute.find(", ", start);
            type_tmp = attribute.substr(start, end - start);
            argument_type.push_back(type_tmp);
            start = end + 2;
        }
        end = attribute.length();
        type_tmp = attribute.substr(start, end - start);
        if(start != end)
            argument_type.push_back(type_tmp);

        if(argument_type.size()!= p_func_invocation.arguments.size()){
            argument_num_wrong = 1;
        }
        else{
            //cout << function_invocation_parameter_type.size() << endl;
            for(int i = function_invocation_parameter_type.size() - 1; i >= 0; i--){
                //cout << argument_type[i]  << "\t" <<  function_invocation_parameter_type.back() << endl;
                if (argument_type[i] != function_invocation_parameter_type.back()){
                    argument_type_wrong = 1;
                    wrong_type = function_invocation_parameter_type.back();
                    correct_type = argument_type[i];
                    type_wrong_col = p_func_invocation.arguments[i]->getLocation().col;
                    
 
                    break;
                }
                function_invocation_parameter_type.pop_back();
                
            }
        }
    }

    if(previous_nodes.top() == "print"){
        //print_entry(current_entry);
        if(argument_num_wrong == 0 && use_undeclared_symbol == 0 && use_non_function_symbol == 0 && argument_type_wrong == 0 && current_entry->type == "void"){
            error_found = 1;
            cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
                 << ": expression of print statement must be scalar type\n";
            cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
            print_arrow(p_func_invocation.getLocation().col);
        }
    }


    if(use_undeclared_symbol == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
            << ": use of undeclared symbol \'"  << p_func_invocation.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(p_func_invocation.getLocation().col);
    }

    if(use_non_function_symbol == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
            << ": call of non-function symbol \'"  << p_func_invocation.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(p_func_invocation.getLocation().col);
    }

    if(argument_num_wrong == 1){
        error_found = 1;
        current_entry->error = 1;
        
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
            << ": too few/much arguments provided for function \'"  << p_func_invocation.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(p_func_invocation.getLocation().col);
    }

    if(argument_type_wrong == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << type_wrong_col 
            << ": incompatible type passing \'" << wrong_type << "\' to parameter of type \'" << correct_type << "\'\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(type_wrong_col);
    }
    

    //printf("func_invocation_end\n");
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    //printf("variable_reference\n");

    SymbolEntry *function_invocation_entry = nullptr;
    if (previous_nodes.top() == "function_invocation"){
        function_invocation_entry = current_entry;
    }

    
    int use_undeclared_symbol = 1;
    int use_non_variable_symbol = 0;
    int over_array_subscript = 0;
    int read_statement_const_loopvar = 0;
    int read_statement_scalar = 0;
    string variable_name = p_variable_ref.getNameCString();

    int current_entry_changed = 0;


    for(uint i = 0; i < global_symbol.size(); i++){
        if(global_symbol[i]->name == variable_name){
            current_entry = global_symbol[i];
            current_entry_changed = 1;

            if(global_symbol[i]->kind == Kind::KIND_VARIABLE){
                use_undeclared_symbol = 0;
                break;
            }
            else{
                use_non_variable_symbol = 1;
                use_undeclared_symbol = 0;
                current_entry->error = 1;
                break;
            }
        }
    }

    for(uint i = 0; i < current_table->entries.size(); i++){
        if(current_table->entries[i]->name == variable_name){
            current_entry = current_table->entries[i];          
            current_entry_changed = 1;

            use_undeclared_symbol = 0;           
            break;
        }     
    }
    for(uint i = 0; i < for_loop_entry.size(); i++){
        if(for_loop_entry[i]->name == variable_name){
            current_entry = for_loop_entry[i];        
            current_entry_changed = 1;

            use_undeclared_symbol = 0;           
            break;
        }     
    }
    if(current_entry_changed == 0){
        for(int i = outer_symbol.size() - 1; i >= 0; i--){
            if(outer_symbol[i]->name == variable_name){
                current_entry = outer_symbol[i];        
                current_entry_changed = 1;

                use_undeclared_symbol = 0;           
                break;
            }     
        }
    }
    

    if(current_entry->error == 0 && p_variable_ref.indices.empty() == 0){
        if((signed)p_variable_ref.indices.size() > current_entry->dimension){
            over_array_subscript = 1;
            current_entry->error = 1;
        }
    }

    // for binary operator and unary operator
    if(previous_nodes.top() == "binary_op" || previous_nodes.top() == "unary_op" || previous_nodes.top() == "assignment" || previous_nodes.top() == "if" || previous_nodes.top() == "return"){
        string func_name = p_variable_ref.getNameCString();    
        for(uint i = 0; i < global_symbol.size(); i++){
            if(global_symbol[i]->name == func_name){
                current_entry = global_symbol[i];
                current_entry_changed = 1;

                break;
            }
        }
        for(uint i = 0; i < current_table->entries.size(); i++){
            if(current_table->entries[i]->name == func_name){
                current_entry = current_table->entries[i];
                current_entry_changed = 1;

                break;
            }     
        }
        if(current_entry->type.find("void") != string::npos)
            p_variable_ref.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kVoidType));
        else if(current_entry->type.find("integer") != string::npos)
            p_variable_ref.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kIntegerType));
        else if(current_entry->type.find("real") != string::npos)
            p_variable_ref.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kRealType));
        else if(current_entry->type.find("boolean") != string::npos)
            p_variable_ref.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kBoolType));
        else if(current_entry->type.find("string") != string::npos)
            p_variable_ref.setType(std::make_shared<const PType>(PType::PrimitiveTypeEnum::kStringType));

        int real_dimension = current_entry->dimension - p_variable_ref.indices.size();
        if(previous_nodes.top() == "binary_op")     
            binary_op_dimension.push_back(real_dimension);
        else if(previous_nodes.top() == "unary_op"){
            unary_op_dimension.push_back(real_dimension);
            unary_operand = current_entry->type;
        }
        else if(previous_nodes.top() == "assignment"){        
            if(real_dimension != 0){
                array_assignment = p_variable_ref.getLocation().col;
            }
            else if(current_entry->kind == Kind::KIND_LOOP_VAR){
                assign_loopvar = 1;
            }
        }
        else if(previous_nodes.top() == "return"){
            if(function_return_type != "not in function" && over_array_subscript == 0){
                if(real_dimension != function_return_dimension || function_return_type.find(p_variable_ref.getTypeCString()) == string::npos){        
                    error_found = 1;

                    int first, second;
                    string original_type, real_type;
                    second = 0;
                    original_type = current_entry->type;
                    first = original_type.find("[");
                    for(uint i = 0; i < p_variable_ref.indices.size(); i++){
                        second = original_type.find("]");
                    }

                    if(p_variable_ref.indices.size() > 0)
                        real_type = original_type.substr(0, first) + original_type.substr(second + 1, original_type.length() - second);
                    else
                        real_type = original_type;

                    cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                        << ": return \'" << real_type << "\' from a function with return type \'" << function_return_type << "\'\n";
                    cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
                    print_arrow(p_variable_ref.getLocation().col);
                }
            }
        }
    }

    else if(previous_nodes.top() == "function_invocation"){
        function_invocation_parameter_type.push_back(current_entry->type);
    }

    else if(previous_nodes.top() == "print"){
        
        if((signed) p_variable_ref.indices.size() < current_entry->dimension){
            error_found = 1;
            cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                 << ": expression of print statement must be scalar type\n";
            cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
            print_arrow(p_variable_ref.getLocation().col);
        }
    }
    else if(previous_nodes.top() == "read"){        
        if(current_entry_changed == 1 && (current_entry->kind == Kind::KIND_CONSTANT || current_entry->kind == Kind::KIND_LOOP_VAR)){
            read_statement_const_loopvar = 1;            
        }
        else if(current_entry_changed == 1 && (signed) p_variable_ref.indices.size() < current_entry->dimension){
            read_statement_scalar = 1;
        }
    }

    
    previous_nodes.push("variable_reference");
    //visit child node
    p_variable_ref.visitChildNodes(*this);
    previous_nodes.pop();

    if(use_undeclared_symbol == 0 && use_non_variable_symbol == 0 && previous_nodes.top() == "assignment"){
        if(current_entry->kind == Kind::KIND_CONSTANT){
            assign_const = 1;
        }
    }

    if(use_undeclared_symbol == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
            << ": use of undeclared symbol \'"  << p_variable_ref.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    }
    if(use_non_variable_symbol == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
            << ": use of non-variable symbol \'"  << p_variable_ref.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    }

    if(over_array_subscript == 1){
        error_found = 1;
        
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
            << ": there is an over array subscript on \'"  << p_variable_ref.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    }

    if(previous_nodes.top() == "function_invocation"){
        current_entry = function_invocation_entry;
    }

    if(read_statement_const_loopvar == 1){       
        error_found = 1;     
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                << ": variable reference of read statement cannot be a constant or loop variable\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    }

    if(read_statement_scalar == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                << ": variable reference of read statement must be scalar type\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    }

    //printf("variable_reference_end\n");
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    //printf("assignment\n");

    assign_const = 0;
    array_assignment = 0;
    assign_loopvar = 0;

    previous_nodes.push("assignment");
    //visit child node
    p_assignment.visitChildNodes(*this);
    previous_nodes.pop();

    string left = p_assignment.lvalue->getTypeCString();
    string right = p_assignment.expr->getTypeCString();
    string real = "real";
    string integer = "integer";

    if(assign_const == 1){
        error_found = 1;
        cerr << "<Error> Found in line " << p_assignment.lvalue->getLocation().line << ", column " << p_assignment.lvalue->getLocation().col 
            << ": cannot assign to variable \'" << p_assignment.lvalue->getNameCString() << "\' which is a constant\n";
        cerr << "    " <<  code[p_assignment.lvalue->getLocation().line] << endl;
        print_arrow(p_assignment.lvalue->getLocation().col);
    }    
    else {
        if(array_assignment > 0){
            error_found = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << array_assignment 
                << ": array assignment is not allowed\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(array_assignment);
            
        }
        else if(assign_loopvar == 1 && previous_nodes.top() == "compound"){
            error_found = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.lvalue->getLocation().col 
                << ": the value of loop variable cannot be modified inside the loop body\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(p_assignment.lvalue->getLocation().col);
        }
        else if(left == right || (left == real && right == integer)){}
        else{
            error_found = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.getLocation().col 
                << ": assigning to \'" << p_assignment.lvalue->getTypeCString() << "\' from incompatible type \'" << p_assignment.expr->getTypeCString() << "\'\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(p_assignment.getLocation().col);
        }
        
    }
      
    //printf("assignment_end\n");
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    //printf("read\n");
    
    previous_nodes.push("read");
    //visit child node    
    p_read.visitChildNodes(*this);
    previous_nodes.pop();

    //printf("read_end\n");
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    //printf("if\n");

    previous_nodes.push("if");
    //visit child node    
    p_if.visitChildNodes(*this);
    previous_nodes.pop();

    string condition_type = p_if.condition->getTypeCString();
    if(condition_type != "null" && condition_type != "boolean" ){
        error_found = 1;
        cerr << "<Error> Found in line " << p_if.getLocation().line << ", column " << p_if.condition->getLocation().col 
            << ": the expression of condition must be boolean type\n";
        cerr << "    " <<  code[p_if.getLocation().line] << endl;
        print_arrow(p_if.condition->getLocation().col);
    }

    //printf("if_end\n");
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    //printf("while\n");
    previous_nodes.push("while");
    //visit child node    
    p_while.visitChildNodes(*this);
    previous_nodes.pop();

    //printf("while_end\n");
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    //printf("For\n");

    in_for_loop++;
    
    // create a new symbol table
    SymbolTable *new_table = new SymbolTable();    
    // push the new symbol into the scope
    pushScope(new_table);    

    // point the current table pointer to the new table
    current_table = new_table;
    // add 1 to level
    level++;
    
    previous_nodes.push("for");
    //visit child node    
    p_for.visitChildNodes(*this);
    previous_nodes.pop();

    // print the table out
    print_table(current_table); 

    popScope();
    current_table = this->tables.top();
    level--;
    
    for_loop_var.pop_back();
    for_loop_entry.pop_back();

    string for_begin = for_loop_range.back();
    for_loop_range.pop_back();
    string for_end = for_loop_range.back();
    for_loop_range.pop_back();

    if(for_begin < for_end){
        error_found = 1;
        cerr << "<Error> Found in line " << p_for.getLocation().line << ", column " << p_for.getLocation().col 
            << ": the lower bound and upper bound of iteration count must be in the incremental order\n";
        cerr << "    " <<  code[p_for.getLocation().line] << endl;
        print_arrow(p_for.getLocation().col);
    }

    in_for_loop--;

    //printf("For_end\n");
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    //printf("return\n");

    return_type_wrong = 0;

    if(function_return_type == "not in function"){
         error_found = 1;
        cerr << "<Error> Found in line " << p_return.getLocation().line << ", column " << p_return.getLocation().col 
            << ": program/procedure should not return a value\n";
        cerr << "    " <<  code[p_return.getLocation().line] << endl;
        print_arrow(p_return.getLocation().col);
    }
   
    previous_nodes.push("return");
    //visit child node
    p_return.visitChildNodes(*this);
    previous_nodes.pop();

    

    //printf("return_end\n");
}
