%{
#define YYSTYPE double
#include <math.h>
double d;
%}

/* BISON Declarations */
%token NUM
%left '-' '+'
%left '*' '/'
%right '^'    /* exponentiation        */

/* Grammar follows */
%%
/*
line:     exp                { d = $1;          }
;
*/

exp:      NUM                { $$ = $1;         }
        | exp '+' exp        { $$ = $1 + $3;    }
        | exp '-' exp        { $$ = $1 - $3;    }
        | exp '*' exp        { $$ = $1 * $3;    }
        | exp '/' exp        { $$ = $1 / $3;    }
        | exp '^' exp        { $$ = pow ($1, $3); }
        | '(' exp ')'        { $$ = $2;         }
;
%%

