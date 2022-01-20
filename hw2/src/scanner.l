%{
#include <stdint.h>

#include "parser.h"

#define LIST                strCat(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static int32_t opt_src = 1;
static int32_t opt_tok = 1;
static char string_literal[MAX_LINE_LENG];
static char *buffer_ptr = buffer;

static void strCat(const char *Text);

%}

integer 0|[1-9][0-9]*
float {integer}\.(0|[0-9]*[1-9])

%x CCOMMENT

%%
    /* Delimiter */
"," { TOKEN_CHAR(','); return COMMA;}
";" { TOKEN_CHAR(';'); return SEMICOLON;}
":" { TOKEN_CHAR(':'); return COLON;}
"(" { TOKEN_CHAR('('); return LEFT_PARENTHESIS;}
")" { TOKEN_CHAR(')'); return RIGHT_PARENTHESIS;}
"[" { TOKEN_CHAR('['); return LEFT_SQUARE_BRACKET;}
"]" { TOKEN_CHAR(']'); return RIGHT_SQUARE_BRACKET;}

    /* Operator */
"+"   { TOKEN_CHAR('+'); return ADDITION;}
"-"   { TOKEN_CHAR('-'); return SUBTRACTION;}
"*"   { TOKEN_CHAR('*'); return MULTIPLICATION;}
"/"   { TOKEN_CHAR('/'); return DIVISION;}
"mod" { TOKEN(mod); return MODULO;}
":="  { TOKEN(:=); return ASSIGNMENT;}
"<"   { TOKEN_CHAR('<'); return LT;}
"<="  { TOKEN(<=); return LE;}
"<>"  { TOKEN(<>); return NE;}
">="  { TOKEN(>=); return GE;}
">"   { TOKEN_CHAR('>'); return GT;}
"="   { TOKEN_CHAR('='); return EQ;}
"and" { TOKEN(and); return AND;}
"or"  { TOKEN(or); return OR;}
"not" { TOKEN(not); return NOT;}

    /* Keyword */
"array"   { TOKEN(KWarray); return KW_ARRAY;}
"begin"   { TOKEN(KWbegin); return KW_BEGIN;}
"boolean" { TOKEN(KWboolean); return KW_BOOLEAN;}
"def"     { TOKEN(KWdef); return KW_DEF;}
"do"      { TOKEN(KWdo); return KW_DO;}
"else"    { TOKEN(KWelse); return KW_ELSE;}
"end"     { TOKEN(KWend); return KW_END;}
"false"   { TOKEN(KWfalse); return KW_FALSE;}
"for"     { TOKEN(KWfor); return KW_FOR;}
"integer" { TOKEN(KWinteger); return KW_INTEGER;}
"if"      { TOKEN(KWif); return KW_IF;}
"of"      { TOKEN(KWof); return KW_OF;}
"print"   { TOKEN(KWprint); return KW_PRINT;}
"read"    { TOKEN(KWread); return KW_READ;}
"real"    { TOKEN(KWreal); return KW_REAL;}
"string"  { TOKEN(KWstring); return KW_STRING;}
"then"    { TOKEN(KWthen); return KW_THEN;}
"to"      { TOKEN(KWto); return KW_TO;}
"true"    { TOKEN(KWtrue); return KW_TRUE;}
"return"  { TOKEN(KWreturn); return KW_RETURN;}
"var"     { TOKEN(KWvar); return KW_VAR;}
"while"   { TOKEN(KWwhile); return KW_WHILE;}

    /* Identifier */
[a-zA-Z][a-zA-Z0-9]* { TOKEN_STRING(id, yytext); return ID;}

    /* Integer (decimal/octal) */
{integer} { TOKEN_STRING(integer, yytext); return INTEGER;}
0[0-7]+   { TOKEN_STRING(oct_integer, yytext); return OCT_INTEGER;}

    /* Floating-Point */
{float} { TOKEN_STRING(float, yytext); return FLOAT;}

    /* Scientific Notation [Ee][+-]?[0-9]+ */
({integer}|{float})[Ee][+-]?({integer}) { TOKEN_STRING(scientific, yytext); return SCIENTIFIC_NOTATION;}

    /* String */
\"([^"\n]|\"\")*\" {
    char *yyt_ptr = yytext;
    char *str_ptr = string_literal;

    /* Condition guard of the end of string literal
       Check only when *yyt_ptr == '"' */
    while (*(yyt_ptr + 1) != '\0') {
        ++yyt_ptr;

        /* Handle the situation of double quotes */
        if (*yyt_ptr == '"' && *(yyt_ptr + 1) != '\0')
            *str_ptr++ = *yyt_ptr++;

        while (*yyt_ptr != '"')
            *str_ptr++ = *yyt_ptr++;
    }
    *str_ptr = '\0';
    TOKEN_STRING(string, string_literal);
    return STRING;
}
    /* Whitespace */
[ \t]+ { LIST; }

    /* Pseudocomment */
"//&"[ST][+-].* {
    LIST;
    char option = yytext[3];
    switch (option) {
    case 'S':
        opt_src = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'T':
        opt_tok = (yytext[4] == '+') ? 1 : 0;
        break;
    }
}

    /* C++ Style Comment */
"//".* { LIST; }

    /* C Style Comment */
"/*"           { LIST; BEGIN(CCOMMENT); }
<CCOMMENT>"*/" { LIST; BEGIN(INITIAL); }
<CCOMMENT>.    { LIST; }

    /* Newline */
<INITIAL,CCOMMENT>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void strCat(const char *Text) {
    while (*Text) {
        *buffer_ptr++ = *Text++;
    }
    *buffer_ptr = '\0';
}
