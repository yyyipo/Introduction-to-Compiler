#ifndef __AST_H
#define __AST_H

#include <cstdint>
#include <string>
#include <vector>
#include <visitor/AstNodeVisitor.hpp>
#include <AST/AstDumper.hpp>

struct Location {
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}

    uint32_t line;
    uint32_t col;
};

class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;

    virtual void accept(AstNodeVisitor &visitor) = 0;

  public:
    const Location location;

  public:
    void incrementIndentation();
    void decrementIndentation();
    static void outputIndentationSpace(const uint32_t indentation);

  protected:
    static const uint32_t m_indentation_stride = 2u;
    static uint32_t m_indentation;
};

enum class Boolean_type{
  booltype_True,
  booltype_False
};

enum class Type{
  TYPE_INTEGER,
  TYPE_REAL,
  TYPE_STRING,
  TYPE_BOOLEAN,
  TYPE_VOID
};

enum class Operator{
  OP_ASSIGN,
  OP_OR,
  OP_AND,
  OP_NOT,
  OP_LESS,
  OP_LESS_OR_EQUAL,
  OP_EQUAL,
  OP_GREATER,
  OP_GREATER_OR_EQUAL,
  OP_NOT_EQUAL,
  OP_PLUS,
  OP_MINUS,
  OP_MULTIPLY,
  OP_DEVIDE,
  OP_MOD
};

struct Variable {
  uint32_t line_number;
  uint32_t col_number;
  Type type;
  int int_literal;
  double real_literal;
  char* string_literal;
  Boolean_type boolean_literal;
  std::vector<int> array_num;
};

struct Id_information{
  char* name;
  uint32_t line_number;
  uint32_t col_number;
};

struct Node_Type{
  Variable* type;
  AstNode* ast_node;
  int cnt;
};

struct Op_information {
  Operator operator_type;
};

#endif
