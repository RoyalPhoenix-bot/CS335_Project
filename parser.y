%{
#include<bits/stdc++.h>
// #define YYDEBUG 1
using namespace std;

int yylex();
int yyerror(char *s);
%}

%token <name> SUPER IF ELSE SWITCH CASE DEFAULT WHILE DO FOR BREAK THROW SYNCHRONIZED TRY FINALLY CATCH VOID STATIC THROWS EXTENDS IMPLEMENTS CLASS INTERFACE BOOLEAN SHORT INT LONG CHAR FLOAT DOUBLE INSTANCEOF THIS NEW 
%token <name> EOL SEPARATOR ASSIGNMENTOPERATOR LITERAL IDENTIFIER WHITESPACE OTHER
%token <name> OPROUND CLROUND OPSQR CLSQR DOT PLUS MINUS COLON SEMICOLON TILDA EX QUES ASTERIX FSLASH MOD LSHIFT RSHIFT URSHIFT LT GT LTE GTE DOUBLEEQ NOTEQ AND XOR OR DAND DOR DPLUS DMINUS
%token <name> PACKAGE MODIFIER_ INTEGERLITERAL FLOATINGPOINTLITERAL BOOLEANLITERAL CHARACTERLITERAL STRINGLITERAL NULLLITERAL IMPORT BYTE OPCURLY CLCURLY COMMA EQUALTO CONTINUE RETURN

%union{
	char* name;
}

%%

Goal:
	CompilationUnit
;

Literal:
	IntegerLiteral 
	| FloatingPointLiteral 
	| BooleanLiteral 
	| CharacterLiteral 
	| StringLiteral 
	| NullLiteral
;

IntegerLiteral: INTEGERLITERAL
;

FloatingPointLiteral: FLOATINGPOINTLITERAL
;

BooleanLiteral: BOOLEANLITERAL
;

CharacterLiteral: CHARACTERLITERAL
;

StringLiteral: STRINGLITERAL
;

NullLiteral: NULLLITERAL
;

Identifier: IDENTIFIER
;

Type:
	PrimitiveType 
	| ReferenceType
;

PrimitiveType:
	NumericType 
	| BOOLEAN
;

NumericType:
	IntegralType 
	| FloatingPointType
;

IntegralType:
	BYTE 
	| SHORT 
	| INT 
	| LONG 
	| CHAR
;

FloatingPointType:
	FLOAT 
	| DOUBLE
;

ReferenceType:
	ClassOrInterfaceType 
	| ArrayType
;

ClassOrInterfaceType:
	Name
;

ClassType:
	ClassOrInterfaceType
;

InterfaceType:
	ClassOrInterfaceType 
;

ArrayType:
	PrimitiveType OPSQR CLSQR 
	| Name OPSQR CLSQR 
	| ArrayType OPSQR CLSQR
;


Name:
	SimpleName
  | QualifiedName
;

SimpleName: Identifier
;

QualifiedName: Name DOT Identifier
;

CompilationUnit:
	| PackageDeclaration ImportDeclarations TypeDeclarations
	| ImportDeclarations TypeDeclarations
	| PackageDeclaration TypeDeclarations
	| PackageDeclaration ImportDeclarations
	| PackageDeclaration
	| ImportDeclarations
	| TypeDeclarations
;

ImportDeclarations:
	ImportDeclaration
	| ImportDeclarations ImportDeclaration
;

TypeDeclarations:
	TypeDeclaration
	| TypeDeclarations TypeDeclaration
;

PackageDeclaration:
	PACKAGE Name SEMICOLON;

ImportDeclaration:
	SingleTypeImportDeclaration
	| TypeImportOnDemandDeclaration
;

SingleTypeImportDeclaration:
	IMPORT Name SEMICOLON;

TypeImportOnDemandDeclaration:
	IMPORT Name DOT ASTERIX SEMICOLON;

TypeDeclaration: 
	ClassDeclaration
	| InterfaceDeclaration
	| SEMICOLON
;

Modifiers:
	Modifier_
	| Modifiers Modifier_
	| SYNCHRONIZED
	| Modifiers SYNCHRONIZED
	| STATIC
	| Modifiers STATIC
;

Modifier_:
	MODIFIER_;


ClassDeclaration: 
	Modifiers CLASS Identifier Super Interfaces ClassBody
	| CLASS Identifier Super Interfaces ClassBody
	| CLASS Identifier Interfaces ClassBody
	| CLASS Identifier Super ClassBody
	| CLASS Identifier ClassBody
	| Modifiers CLASS Identifier Interfaces ClassBody
	| Modifiers CLASS Identifier Super ClassBody
	| Modifiers CLASS Identifier ClassBody
;

Super: EXTENDS ClassType
;

Interfaces: IMPLEMENTS InterfaceTypeList
;

InterfaceTypeList: 
	InterfaceType
	| InterfaceTypeList COMMA InterfaceType
;

ClassBody: 
	OPCURLY ClassBodyDeclarations CLCURLY
	| OPCURLY CLCURLY
;

ClassBodyDeclarations: 
	ClassBodyDeclaration
	| ClassBodyDeclarations ClassBodyDeclaration
;

ClassBodyDeclaration: 
	ClassMemberDeclaration
	| StaticInitializer
	| ConstructorDeclaration
;

ClassMemberDeclaration: 
	FieldDeclaration
	| MethodDeclaration
;

FieldDeclaration: 
	Modifiers Type VariableDeclarators SEMICOLON
	| Type VariableDeclarators SEMICOLON
;

VariableDeclarators: 
	VariableDeclarator
	| VariableDeclarators COMMA VariableDeclarator
;

VariableDeclarator: 
	VariableDeclaratorId
	| VariableDeclaratorId EQUALTO VariableInitializer

VariableDeclaratorId: 
	Identifier
	| VariableDeclaratorId OPSQR CLSQR
;

Primary:
	PrimaryNoNewArray
	| ArrayCreationExpression
;

PrimaryNoNewArray:
	Literal
	| THIS
	| OPROUND Expression CLROUND
	| ClassInstanceCreationExpression
	| FieldAccess
	| ArrayAccess
	| MethodInvocation
;

ClassInstanceCreationExpression:
	NEW ClassType OPROUND CLROUND
	| NEW ClassType OPROUND ArgumentList CLROUND
;

ArgumentList:
	Expression
	| ArgumentList COMMA Expression
;

ArrayCreationExpression:
	NEW PrimitiveType DimExprs
	| NEW PrimitiveType DimExprs Dims
	| NEW ClassOrInterfaceType DimExprs
	| NEW ClassOrInterfaceType DimExprs Dims
;

DimExprs:
	DimExpr
	| DimExprs DimExpr
;

DimExpr:
	OPSQR Expression CLSQR
;

Dims:
	OPSQR CLSQR
	| Dims OPSQR CLSQR
;

FieldAccess:
	Primary DOT Identifier
	| SUPER DOT Identifier
;

MethodInvocation:
	Name OPROUND CLROUND
	| Name OPROUND ArgumentList CLROUND
	| Primary DOT Identifier OPROUND CLROUND
	| Primary DOT Identifier OPROUND ArgumentList CLROUND
	| SUPER DOT Identifier OPROUND CLROUND 
	| SUPER DOT Identifier OPROUND ArgumentList CLROUND
;

ArrayAccess:
	Name OPSQR Expression CLSQR
	| PrimaryNoNewArray OPSQR Expression CLSQR
;

PostfixExpression:
	Primary
	| Name
	| PostIncrementExpression
	| PostDecrementExpression
;

PostIncrementExpression:
	PostfixExpression PLUS PLUS
;

PostDecrementExpression:
	PostfixExpression MINUS MINUS
;

UnaryExpression:
	PreIncrementExpression
	| PreDecrementExpression
	| PLUS UnaryExpression
	| MINUS UnaryExpression
	| UnaryExpressionNotPlusMinus
;

PreIncrementExpression:
	DPLUS UnaryExpression
;

PreDecrementExpression:
	DMINUS UnaryExpression
;

UnaryExpressionNotPlusMinus:
	PostfixExpression
	| TILDA UnaryExpression
	| EX UnaryExpression
	| CastExpression
;

CastExpression:
	OPROUND PrimitiveType CLROUND UnaryExpression
	| OPROUND PrimitiveType Dims CLROUND UnaryExpression
	| OPROUND Expression CLROUND UnaryExpressionNotPlusMinus
	| OPROUND Name Dims CLROUND UnaryExpressionNotPlusMinus
;

MultiplicativeExpression:
	UnaryExpression
	| MultiplicativeExpression ASTERIX UnaryExpression
	| MultiplicativeExpression FSLASH UnaryExpression
	| MultiplicativeExpression MOD UnaryExpression
;

AdditiveExpression:
	MultiplicativeExpression
	| AdditiveExpression PLUS MultiplicativeExpression
	| AdditiveExpression MINUS MultiplicativeExpression
;

ShiftExpression:
	AdditiveExpression
	| ShiftExpression LSHIFT AdditiveExpression
	| ShiftExpression RSHIFT AdditiveExpression
	| ShiftExpression URSHIFT AdditiveExpression
;

RelationalExpression:
	ShiftExpression
	| RelationalExpression LT ShiftExpression
	| RelationalExpression GT ShiftExpression
	| RelationalExpression LTE ShiftExpression
	| RelationalExpression GTE ShiftExpression
	| RelationalExpression INSTANCEOF ReferenceType
;

EqualityExpression:
	RelationalExpression
	| EqualityExpression DOUBLEEQ RelationalExpression
	| EqualityExpression NOTEQ RelationalExpression
;

AndExpression:
	EqualityExpression
	| AndExpression AND EqualityExpression
;

ExclusiveOrExpression:
	AndExpression
	| ExclusiveOrExpression XOR AndExpression
;

InclusiveOrExpression:
	ExclusiveOrExpression
	| InclusiveOrExpression OR ExclusiveOrExpression
;

ConditionalAndExpression:
	InclusiveOrExpression
	| ConditionalAndExpression DAND InclusiveOrExpression
;

ConditionalOrExpression:
	ConditionalAndExpression
	| ConditionalOrExpression DOR ConditionalAndExpression
;

ConditionalExpression:
	ConditionalOrExpression
	| ConditionalOrExpression QUES Expression COLON ConditionalExpression
;

AssignmentExpression:
	ConditionalExpression
	| Assignment
;

Assignment:
	LeftHandSide ASSIGNMENTOPERATOR AssignmentExpression
;

LeftHandSide:
	Name
	| FieldAccess
	| ArrayAccess
;

Expression:
	AssignmentExpression
;

ConstantExpression: Expression;

VariableInitializer: 
	Expression
	| ArrayInitializer
;

MethodDeclaration: MethodHeader MethodBody
;

MethodHeader: 
	Modifiers Type MethodDeclarator Throws
	| Type MethodDeclarator Throws
	| Modifiers Type MethodDeclarator 
	| Type MethodDeclarator
	| Modifiers VOID MethodDeclarator Throws
	| Modifiers VOID MethodDeclarator 
	| VOID MethodDeclarator Throws
	| VOID MethodDeclarator
;

MethodDeclarator: 
	Identifier OPROUND FormalParameterList CLROUND
	| Identifier OPROUND CLROUND
	| MethodDeclarator OPSQR CLSQR
;

FormalParameterList: 
	FormalParameter
	| FormalParameterList COMMA FormalParameter
;

FormalParameter: Type VariableDeclaratorId
;

Throws: THROWS ClassTypeList
;

ClassTypeList: 
	ClassType
    | ClassTypeList COMMA ClassType
;

MethodBody: 
	Block 
	| SEMICOLON
;

StaticInitializer: STATIC Block
;

ConstructorDeclaration:	
	Modifiers ConstructorDeclarator Throws ConstructorBody
	| ConstructorDeclarator Throws ConstructorBody
	| ConstructorDeclarator ConstructorBody
	| Modifiers ConstructorDeclarator ConstructorBody
;

ConstructorDeclarator: 
	SimpleName OPROUND FormalParameterList CLROUND
	| SimpleName OPROUND CLROUND
;

ConstructorBody: 
	OPCURLY ExplicitConstructorInvocation BlockStatements CLCURLY
	| OPCURLY BlockStatements CLCURLY
	| OPCURLY ExplicitConstructorInvocation CLCURLY
	| OPCURLY CLCURLY
;

ExplicitConstructorInvocation: 
	THIS OPROUND ArgumentList CLROUND SEMICOLON
	| THIS OPROUND CLROUND SEMICOLON
	| SUPER OPROUND ArgumentList CLROUND SEMICOLON
	| SUPER OPROUND CLROUND SEMICOLON
;


InterfaceDeclaration: 
	Modifiers INTERFACE Identifier ExtendsInterfaces InterfaceBody
	| INTERFACE Identifier ExtendsInterfaces InterfaceBody
	| Modifiers INTERFACE Identifier InterfaceBody
	| INTERFACE Identifier InterfaceBody
;

ExtendsInterfaces: 
	EXTENDS InterfaceType
	| ExtendsInterfaces COMMA InterfaceType
;

InterfaceBody: 
	OPCURLY InterfaceMemberDeclarations CLCURLY
	| OPCURLY CLCURLY
;

InterfaceMemberDeclarations: 
	InterfaceMemberDeclaration
	| InterfaceMemberDeclarations InterfaceMemberDeclaration
;

InterfaceMemberDeclaration: 
	ConstantDeclaration 
	| AbstractMethodDeclaration
;

ConstantDeclaration: FieldDeclaration
;

AbstractMethodDeclaration: MethodHeader SEMICOLON
;

ArrayInitializer: 
	OPCURLY VariableInitializers COMMA CLCURLY 
	| OPCURLY COMMA CLCURLY
	| OPCURLY VariableInitializers CLCURLY
	| OPCURLY CLCURLY
;

VariableInitializers: 
	VariableInitializer
	| VariableInitializers COMMA VariableInitializer
;

Block:
	OPCURLY CLCURLY 
	| OPCURLY BlockStatements CLCURLY 
;

BlockStatements: 
	BlockStatement 
	| BlockStatements BlockStatement
;

BlockStatement:
	LocalVariableDeclarationStatement
	| Statement
;

LocalVariableDeclarationStatement:
	LocalVariableDeclaration SEMICOLON
;

LocalVariableDeclaration:
	Type VariableDeclarators
;

Statement:
	StatementWithoutTrailingSubstatement
	| LabeledStatement
	| IfThenStatement
	| IfThenElseStatement
	| WhileStatement
	| ForStatement
;

StatementNoShortIf:
	StatementWithoutTrailingSubstatement
	| LabeledStatementNoShortIf
	| IfThenElseStatementNoShortIf
	| WhileStatementNoShortIf
	| ForStatementNoShortIf
;

StatementWithoutTrailingSubstatement:
	Block
	| EmptyStatement
	| ExpressionStatement
	| SwitchStatement
	| DoStatement
	| BreakStatement
	| ContinueStatement
	| ReturnStatement
	| SynchronizedStatement
	| ThrowStatement
	| TryStatement

EmptyStatement:
	SEMICOLON
;

LabeledStatement:
	Identifier COLON Statement
;

LabeledStatementNoShortIf:
	Identifier COLON StatementNoShortIf
;

ExpressionStatement:
	StatementExpression SEMICOLON
;

StatementExpression:
	Assignment
	| PreIncrementExpression
	| PreDecrementExpression
	| PostIncrementExpression
	| PostDecrementExpression
	| MethodInvocation
	| ClassInstanceCreationExpression
;

IfThenStatement:
	IF OPROUND Expression CLROUND Statement
;

IfThenElseStatement:
	IF OPROUND Expression CLROUND StatementNoShortIf ELSE Statement
;

IfThenElseStatementNoShortIf:
	IF OPROUND Expression CLROUND StatementNoShortIf ELSE StatementNoShortIf
;

SwitchStatement:
	SWITCH OPROUND Expression CLROUND SwitchBlock
;

SwitchBlock:
	OPCURLY CLCURLY
	| OPCURLY SwitchLabels CLCURLY
	| OPCURLY SwitchBlockStatementGroups CLCURLY
	| OPCURLY SwitchBlockStatementGroups SwitchLabels CLCURLY
;

SwitchBlockStatementGroups:
	SwitchBlockStatementGroup
	| SwitchBlockStatementGroups SwitchBlockStatementGroup
;

SwitchBlockStatementGroup:
	SwitchLabels BlockStatements
;

SwitchLabels:
	SwitchLabel
	| SwitchLabels SwitchLabel
;

SwitchLabel:
	CASE ConstantExpression COLON
	| DEFAULT COLON
;

WhileStatement:
	WHILE OPROUND Expression CLROUND Statement
;

WhileStatementNoShortIf:
	WHILE OPROUND Expression CLROUND StatementNoShortIf
;

DoStatement:
	DO Statement WHILE OPROUND Expression CLROUND SEMICOLON
;

ForStatement:
	  FOR OPROUND ForInit SEMICOLON Expression SEMICOLON ForUpdate CLROUND Statement
	| FOR OPROUND         SEMICOLON Expression SEMICOLON ForUpdate CLROUND Statement
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON ForUpdate CLROUND Statement
	| FOR OPROUND ForInit SEMICOLON Expression SEMICOLON           CLROUND Statement
	| FOR OPROUND         SEMICOLON            SEMICOLON ForUpdate CLROUND Statement
	| FOR OPROUND         SEMICOLON Expression SEMICOLON           CLROUND Statement
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON           CLROUND Statement
	| FOR OPROUND         SEMICOLON            SEMICOLON           CLROUND Statement
;

ForStatementNoShortIf:
	  FOR OPROUND ForInit SEMICOLON Expression SEMICOLON ForUpdate CLROUND StatementNoShortIf
	| FOR OPROUND         SEMICOLON Expression SEMICOLON ForUpdate CLROUND StatementNoShortIf
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON ForUpdate CLROUND StatementNoShortIf
	| FOR OPROUND ForInit SEMICOLON Expression SEMICOLON           CLROUND StatementNoShortIf
	| FOR OPROUND         SEMICOLON            SEMICOLON ForUpdate CLROUND StatementNoShortIf
	| FOR OPROUND         SEMICOLON Expression SEMICOLON           CLROUND StatementNoShortIf
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON           CLROUND StatementNoShortIf
	| FOR OPROUND         SEMICOLON            SEMICOLON           CLROUND StatementNoShortIf
;

ForInit:
	StatementExpressionList 
	| LocalVariableDeclaration
;

ForUpdate:
	StatementExpressionList
;

StatementExpressionList:
	StatementExpression 
	| StatementExpressionList COMMA StatementExpression
;

BreakStatement:
	BREAK Identifier SEMICOLON
	| BREAK SEMICOLON
;

ContinueStatement:
	CONTINUE Identifier SEMICOLON
	| CONTINUE SEMICOLON
;

ReturnStatement:
	RETURN Expression SEMICOLON
	| RETURN SEMICOLON
;

ThrowStatement:
	THROW Expression SEMICOLON
;

SynchronizedStatement:
	SYNCHRONIZED OPROUND Expression CLROUND Block
;

TryStatement: 
	TRY Block Catches
	| TRY Block Catches Finally
	| TRY Block Finally
;

Catches:
	CatchClause
	| Catches CatchClause
;

CatchClause:
	CATCH OPROUND FormalParameter CLROUND Block
;

Finally: FINALLY Block
;


%%

int yyerror(char *s)
{
	printf("%s\n", s);
	return 0;
}

int main()
{
	// YYDEBUG = 1;
    yyparse(); 
    return 0;
}