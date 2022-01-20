%{
#include <stdint.h>

#define LIST                strCat(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static int32_t opt_src = 1;
static int32_t opt_tok = 1;
static char *buffer_ptr = buffer;

static void strCat(const char *Text);

%}

%x STRING
%x COMMENT_ONE
%x COMMENT_TWO

integer ([1-9][0-9]*)|[0]
floating_point ([1-9][0-9]*|0)[\.]([0-9]*[1-9]|0)

%%
    /*delimeters*/
"," { TOKEN_CHAR(','); }
";" { TOKEN_CHAR(';'); }
":" { TOKEN_CHAR(':'); }
"(" { TOKEN_CHAR('('); }
")" { TOKEN_CHAR(')'); }
"[" { TOKEN_CHAR('['); }
"]" { TOKEN_CHAR(']'); }

    /*operators*/
"+" { TOKEN_CHAR('+'); }
"-" { TOKEN_CHAR('-'); }
"*" { TOKEN_CHAR('*'); }
"/" { TOKEN_CHAR('/'); }
"mod" { TOKEN(mod); }
":=" { TOKEN(:=); }
"<" { TOKEN_CHAR('<'); }
"<=" { TOKEN(<=); }
"<>" { TOKEN(<>); }
">=" { TOKEN(>=); }
">" { TOKEN_CHAR('>'); }
"=" { TOKEN_CHAR('='); }
"and" { TOKEN(and); }
"or" { TOKEN(or); }
"not" { TOKEN(not); }

    /*keywords*/
"array" { TOKEN(KWarray); }
"begin" { TOKEN(KWbegin); }
"boolean" { TOKEN(KWboolean); }
"def" { TOKEN(KWdef); }
"do" { TOKEN(KWdo); }
"else" { TOKEN(KWelse); }
"end" { TOKEN(KWend); }
"false" { TOKEN(KWfalse); }
"for" { TOKEN(KWfor); }
"integer" { TOKEN(KWinteger); }
"if" { TOKEN(KWif); }
"of" { TOKEN(KWof); }
"print" { TOKEN(KWprint); }
"read" { TOKEN(KWread); }
"real" { TOKEN(KWreal); }
"string" { TOKEN(KWstring); }
"then" { TOKEN(KWthen); }
"to" { TOKEN(KWto); }
"true" { TOKEN(KWtrue); }
"return" { TOKEN(KWreturn); }
"var" { TOKEN(KWvar); }
"while" { TOKEN(KWwhile); }

    /*identifiers*/
[a-zA-Z]([a-zA-Z]|[0-9])*  { TOKEN_STRING(id, yytext); }

    /*integer constants*/
([1-9][0-9]*)|[0] { TOKEN_STRING(integer, yytext); }
[0][0-7][0-7]* { TOKEN_STRING(oct_integer, yytext); }

    /*floating-point constants*/
([1-9][0-9]*|0)[\.]([0-9]*[1-9]|0) { TOKEN_STRING(float, yytext); }

    /*scientific notations*/
({integer}|{floating_point})?[Ee][\+\-]?{integer} { TOKEN_STRING(scientific, yytext); }

    /*string constants*/
\" { LIST; if (opt_tok) printf("<%s: ", "string"); BEGIN (STRING); }
<STRING>\" { LIST; if(opt_tok) printf(">\n"); BEGIN (INITIAL); }
<STRING>\"\" { LIST; if(opt_tok) printf("%s", "\""); }
<STRING>. { LIST; if(opt_tok) printf("%s", yytext); }

    /*whilespace*/
[ \t] { LIST; } 

    /*comments*/
\/\/ { LIST; BEGIN (COMMENT_ONE); }
<COMMENT_ONE>\n { if (opt_src) {
                  printf("%d: %s\n", line_num, buffer); }
                  ++line_num;
                  buffer[0] = '\0';
                  buffer_ptr = buffer; 
                  BEGIN (INITIAL); }
<COMMENT_ONE>. { LIST; }

\/\* { LIST; BEGIN (COMMENT_TWO); }
<COMMENT_TWO>\*\/ { LIST; BEGIN (INITIAL); }
<COMMENT_TWO>\n { if (opt_src) {
                  printf("%d: %s\n", line_num, buffer); }
                  ++line_num;
                  buffer[0] = '\0';
                  buffer_ptr = buffer; }
<COMMENT_TWO>. { LIST;}

    /*pseudocomments*/
"\/\/\&T\+".* {LIST; opt_tok = 1;}
"\/\/\&T\-".* {LIST; opt_tok = 0;}
"\/\/\&S\+".* {LIST; opt_src = 1;}
"\/\/\&S\-".* {LIST; opt_src = 0;}

\n {
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

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}

