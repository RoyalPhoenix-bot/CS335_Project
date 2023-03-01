flex lexer.l
bison -dv parser.y
g++ -o myASTGenerator parser.tab.c lex.yy.c -lfl
