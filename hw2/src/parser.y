%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;   /* declared in scanner.l */
extern char buffer[512];  /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token COMMA SEMICOLON COLON LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%token ADDITION SUBTRACTION MULTIPLICATION DIVISION MODULO ASSIGNMENT LT LE NE GE GT EQ AND OR NOT
%token KW_ARRAY KW_BEGIN KW_BOOLEAN KW_DEF KW_DO KW_ELSE KW_END KW_FALSE KW_FOR KW_INTEGER KW_IF KW_OF KW_PRINT 
%token KW_READ KW_REAL KW_STRING KW_THEN KW_TO KW_TRUE KW_RETURN KW_VAR KW_WHILE
%token ID INTEGER OCT_INTEGER FLOAT SCIENTIFIC_NOTATION STRING

%left AND OR NOT
%left LT LE NE GE GT EQ 
%left SUBTRACTION 
%left ADDITION
%left DIVISION MODULO
%left MULTIPLICATION

%%

program_name: ID SEMICOLON
              zero_more_variable_constant_declaration
              zero_more_funtion_declaration_definition
              compound_statement
              KW_END
            ;

zero_more_funtion_declaration_definition: zero_more_funtion_declaration_definition function_declaration_definition
                                        |
                                        ;

function_declaration_definition: function_declaration
                               | function_definition
                               ;

function_declaration: ID LEFT_PARENTHESIS zero_more_formal_arguments RIGHT_PARENTHESIS COLON scalar_type SEMICOLON
		    | ID LEFT_PARENTHESIS zero_more_formal_arguments RIGHT_PARENTHESIS SEMICOLON
                    ;

function_definition: ID LEFT_PARENTHESIS zero_more_formal_arguments RIGHT_PARENTHESIS COLON scalar_type
                     compound_statement
                     KW_END
                    |
                     ID LEFT_PARENTHESIS zero_more_formal_arguments RIGHT_PARENTHESIS
                     compound_statement
                     KW_END
                   ;

zero_more_formal_arguments: zero_more_formal_arguments formal_arguments
                          |
                          ;

formal_arguments: identifier_list COLON type
                ;

identifier_list: more_identifier ID
               ;

more_identifier: more_identifier ID COMMA
               |
               ;

scalar_type: KW_BOOLEAN
           | KW_INTEGER
           | KW_REAL
           | KW_STRING
           ;

type: array
    | scalar_type
    ;

array: KW_ARRAY INTEGER KW_OF type
     | 
     ;

statement: compound_statement
         | simple_statement
         | condition
         | while
         | for
         | return
         | procedure_call
         ;

compound_statement: KW_BEGIN
                    zero_more_variable_constant_declaration
                    zero_more_statement
                    KW_END
                  ;

zero_more_statement: zero_more_statement statement
                   |
                   ;

simple_statement: variable_reference ASSIGNMENT expression SEMICOLON
                | KW_PRINT expression SEMICOLON
                | KW_READ variable_reference SEMICOLON
                ;

condition: KW_IF expression KW_THEN
           compound_statement
           KW_ELSE
           compound_statement
           KW_END KW_IF
         | KW_IF expression KW_THEN
           compound_statement
           KW_END KW_IF
         ;

while: KW_WHILE expression KW_DO
       compound_statement
       KW_END KW_DO       
     ;

for: KW_FOR ID ASSIGNMENT INTEGER KW_TO INTEGER KW_DO
     compound_statement
     KW_END KW_DO
   ;

return: KW_RETURN expression SEMICOLON
      ;

procedure_call: ID LEFT_PARENTHESIS expression_seperated_by_comma expression RIGHT_PARENTHESIS SEMICOLON
              | ID LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON
              ;

variable_reference: ID zero_more_expression_w_brackets
                  ;

zero_more_expression_w_brackets: LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET zero_more_expression_w_brackets
                               |
                               ;

expression: literal_constant
          | variable_reference
          | function_invocation
          | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
          | SUBTRACTION expression  %prec MULTIPLICATION
          | expression MULTIPLICATION expression
          | expression DIVISION expression
          | expression MODULO expression
          | expression ADDITION expression
          | expression SUBTRACTION expression
          | expression LT expression
          | expression LE expression
          | expression NE expression
          | expression GE expression
          | expression GT expression
          | expression EQ expression
          | expression AND expression
          | expression OR expression
          | NOT expression
          ;

function_invocation: ID LEFT_PARENTHESIS expression_seperated_by_comma expression RIGHT_PARENTHESIS
                   | ID LEFT_PARENTHESIS RIGHT_PARENTHESIS
                   ;

expression_seperated_by_comma: expression_seperated_by_comma expression COMMA
                             |
                             ;


zero_more_variable_constant_declaration: zero_more_variable_constant_declaration variable_constant_declaration
                                       |
                                       ;

variable_constant_declaration: variable_declaration
                             | constant_declaration
                             ;

variable_declaration: KW_VAR identifier_list COLON scalar_type SEMICOLON
                    | KW_VAR identifier_list COLON KW_ARRAY INTEGER KW_OF type SEMICOLON
                    ;

constant_declaration: KW_VAR identifier_list COLON literal_constant SEMICOLON
                    ;

literal_constant: INTEGER
                | OCT_INTEGER 
                | FLOAT
                | SCIENTIFIC_NOTATION 
                | STRING
                | KW_TRUE
                | KW_FALSE
                ;


%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
