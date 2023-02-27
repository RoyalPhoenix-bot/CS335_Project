%{
    #include <stdio.h>
    #include <string.h>
    #include "parser.tab.h"

%}

%option yylineno

EndOfLineComment "//".*
Comment "/*"[^*]*"*"+([^*/][^*]*[*]+)*"/"|{EndOfLineComment}


LineTerminator \n|\r|\r\n
WhiteSpace [ \t\f]+|{LineTerminator}


JavaLetter [a-zA-Z$_]
JavaLetterOrDigit [a-zA-Z$_0-9]
Identifier {JavaLetter}{JavaLetterOrDigit}*
ReservedKeyword abstract|continue|for|new|switch|assert|default|if|package|synchronized|boolean|do|goto|private|this|break|double|implements|protected|throw|byte|else|import|public|throws|case|enum|instanceof|return|transient|catch|extends|int|short|try|char|final|interface|static|void|class|finally|long|strictfp|volatile|const|float|native|super|while|_ 
ContextualKeyword exports|opens|requires|uses|module|permits|sealed|var|non-sealed|provides|to|with|open|record|transitive|yield
Keyword {ReservedKeyword}|{ContextualKeyword}

Underscores _+
NonZeroDigit [1-9]
DigitOrUnderscore {Digit}|"_"
DigitsAndUnderscores {DigitOrUnderscore}{DigitOrUnderscore}*
Digit "0"|{NonZeroDigit}
Digits {Digit}|({Digit}{DigitsAndUnderscores}?{Digit})
DecimalNumeral "0"|({NonZeroDigit}{Digits}?)|({NonZeroDigit}{Underscores}{Digits})
IntegerTypeSuffix [lL]
DecimalIntegerLiteral {DecimalNumeral}{IntegerTypeSuffix}?


HexDigit [0-9a-fA-F]
HexDigitsAndUnderscore {HexDigit}|"_"
HexDigitsAndUnderscores {HexDigitsAndUnderscore}{HexDigitsAndUnderscore}*
HexDigits {HexDigit}|({HexDigit}{HexDigitsAndUnderscores}?{HexDigit})
HexNumeral ("0x"{HexDigits})|("0X"{HexDigits})
HexIntegerLiteral {HexNumeral}{IntegerTypeSuffix}?



OctalDigit [0-7]
OctalDigitOrUnderscore {OctalDigit}|"_"
OctalDigitsAndUnderscores {OctalDigitOrUnderscore}{OctalDigitOrUnderscore}*
OctalDigits {OctalDigit}|({OctalDigit}{OctalDigitsAndUnderscores}?{OctalDigit})
OctalNumeral ("0"{OctalDigits})|("0"{Underscores}{OctalDigits})
OctalIntegerLiteral {OctalNumeral}{IntegerTypeSuffix}?


BinaryDigit [0-1]
BinaryDigitOrUnderscore {BinaryDigit}|"_"
BinaryDigitsAndUnderscores {BinaryDigitOrUnderscore}{BinaryDigitOrUnderscore}*
BinaryDigits {BinaryDigit}|({BinaryDigit}{BinaryDigitsAndUnderscores}?{BinaryDigit})
BinaryNumeral ("0b"{BinaryDigits})|("0B"{BinaryDigits})
BinaryIntegerLiteral {BinaryNumeral}{IntegerTypeSuffix}?

IntegerLiteral {DecimalIntegerLiteral}|{HexIntegerLiteral}|{OctalIntegerLiteral}|{BinaryIntegerLiteral}

FloatTypeSuffix "f"|"F"|"D"|"d"
Sign [+-]
SignedInteger {Sign}?{Digits}
ExponentIndicator "e"|"E"
ExponentPart {ExponentIndicator}{SignedInteger}
DecimalFloatingPointLiteral ({Digits}\.{Digits}?{ExponentPart}?{FloatTypeSuffix}?)|(\.{Digits}?{ExponentPart}?{FloatTypeSuffix}?)|({Digits}{ExponentPart}{FloatTypeSuffix}?)|({Digits}{ExponentPart}?{FloatTypeSuffix})


BinaryExponentIndicator "p"|"P"
BinaryExponent {BinaryExponentIndicator}{SignedInteger}
HexSignificand ({HexNumeral}\.?)|("0x"{HexDigits}?\.{HexDigits})
HexadecimalFloatingPointLiteral {HexSignificand}{BinaryExponent}{FloatTypeSuffix}?

FloatingPointLiteral {DecimalFloatingPointLiteral}|{HexadecimalFloatingPointLiteral}


BooleanLiteral "true"|"false"


OctalEscape \{OctalDigit}|\{OctalDigit}{OctalDigit}|\[0-3]{OctalDigit}{OctalDigit}
EscapeSequence \b|\s|\t|\n|\f|\r|{LineTerminator}|\'|\\
/* OctalEscape is not included in EscapeSequence for now */
/* \" is not included in EscapeSequence for now */
InputCharacter [^\n\r]
SingleCharacter [^\n\r\'\\]
CharacterLiteral ("'"{SingleCharacter}"'")|\'\\n\'|(\'\\s\')|(\'\\t\')|(\'\\b\')|(\'\\f\')|(\'\\r\')|(\'\\'\')|(\'\\\\\')|(\'\\r\\n\')


StringCharacter [^\"\\]|{EscapeSequence}
StringLiteral \"{StringCharacter}*\"


TextBlockCharacter [^\"\n\r\\]|{EscapeSequence}|{LineTerminator}
TextBlockWhiteSpace [ \t\f]+
TextBlock1 \"\"\"([^\"]+\"{0,2}[^\"]+)*\"\"\"
TextBlock (\"\"\"{TextBlockWhiteSpace}*{LineTerminator}{TextBlockCharacter}*\"\"\")|{TextBlock1}


NullLiteral "null"

Literal {IntegerLiteral}|{FloatingPointLiteral}|{BooleanLiteral}|{CharacterLiteral}|{StringLiteral}|{TextBlock}|{NullLiteral}


Separator "::"|"..."|[(){};,.@]|"["|"]"


Operator "="|">"|"<"|"!"|"~"|"?"|":"|"->"|"=="|">="|"<="|"!="|"&&"|"||"|"++"|"--"|"+"|"-"|"*"|"/"|"&"|"|"|"^"|"%"|"<<"|">>"|">>>"|"+="|"-="|"*="|"/="|"&="|"|="|"^="|"%="|"<<="|">>="|">>>="

%%

{Comment} {printf("Comment\n");}
{LineTerminator} {continue;}
{Keyword} {yylval.name=strdup(yytext);  return (KEYWORD);}
{Separator} {yylval.name=strdup(yytext);  return (SEPARATOR);}
{Operator} {yylval.name=strdup(yytext);  return (OPERATOR);}

{Literal} {yylval.name=strdup(yytext);  return (LITERAL); }

{Identifier} {yylval.name=strdup(yytext);  return (IDENTIFIER);}

{WhiteSpace} { continue;}
. {printf("Other\n");return(OTHER);}

%%

void showError(){
    printf("Other input");
}
int yywrap(){
    return 1;
}