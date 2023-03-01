flex lexer.l
bison -dv parser.y
g++ -o runnable parser.tab.c lex.yy.c -lfl
./runnable < programs/sample10.java