#ifndef __SEMA_SEMANTIC_ANALYZER_H
#define __SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "AST/PType.hpp"
#include "AST/constant.hpp"
#include <stack>


using namespace std;

enum class Kind {
  KIND_PROGRAM,
  KIND_FUNCTION,
  KIND_PARAMETER,
  KIND_VARIABLE,
  KIND_LOOP_VAR,
  KIND_CONSTANT
};

class SymbolEntry {
  public:
    string name;
    Kind kind;
    int level;
    string type;
    string attribute;
    int error = 0;
    int dimension = 0;
    SymbolEntry();
    SymbolEntry(string entry_name, Kind entry_kind, int entry_level, string entry_type, string attribute, int entry_error);

};

class SymbolTable {
  public:
    SymbolTable();
    void addSymbol(SymbolEntry *new_entry);
    vector<SymbolEntry*> entries;
};


class SemanticAnalyzer : public AstNodeVisitor {
  public:
    SemanticAnalyzer() = default;
    ~SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

  //private:

    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager

    void pushScope(SymbolTable *new_scope);
    void popScope();
    void print_entry(SymbolEntry *cur_entry);
    void print_table(SymbolTable *cur_table);
    void print_arrow(int len);

    stack <SymbolTable*> tables;
};

#endif
