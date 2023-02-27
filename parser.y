%{

#include<bits/stdc++.h>
using namespace std;

int yylex();
int yyerror(char *s);


%}

%token  KEYWORD EOL SEPARATOR OPERATOR LITERAL IDENTIFIER WHITESPACE OTHER 

%type <name> KEYWORD
%type <name> SEPARATOR
%type <name> OPERATOR
%type <name> LITERAL
%type <name> IDENTIFIER

%union{
	char* name;
}

%%
input: javasyntax;

javasyntax: keywords | javasyntax keywords
	| identifier | javasyntax identifier
;

keywords: KEYWORD ;

identifier: IDENTIFIER ;

%%

int yyerror(char *s)
{
	printf("%s\n", s);
	return 0;
}

int main()
{
    yyparse();
    
    
    return 0;
}