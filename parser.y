%{
#include<bits/stdc++.h>
// #define YYDEBUG 1
using namespace std;

extern int yylineno;
int yylex();
int yyerror(char *s);

int countNodes=0;
vector<string> nodeType;
map<int, vector<int>> adj;

%}

%token <name> SUPER IF ELSE SWITCH CASE DEFAULT WHILE DO FOR BREAK THROW TRY FINALLY CATCH VOID THROWS EXTENDS IMPLEMENTS CLASS INTERFACE BOOLEAN SHORT INT LONG CHAR FLOAT DOUBLE INSTANCEOF THIS NEW 
%token <name> EOL SEPARATOR  LITERAL IDENTIFIER WHITESPACE OTHER EQUALTO STAREQUALTO BYEQUALTO PLUSEQUALTO MINUSEQUALTO TWOLEFTSHIFTEQUALTO TWORIGHTSHIFTEQUALTO THREERIGHTSHIFTEQUALTO ANDEQUALTO XOREQUALTO BAREQUALTO
%token <name> OPROUND CLROUND OPSQR CLSQR DOT PLUS MINUS COLON SEMICOLON TILDA EX QUES ASTERIX FSLASH MOD LSHIFT RSHIFT URSHIFT LT GT LTE GTE DOUBLEEQ NOTEQ AND XOR OR DAND DOR DPLUS DMINUS
%token <name> PACKAGE STRINGLITERAL NULLLITERAL IMPORT BYTE OPCURLY CLCURLY COMMA CONTINUE RETURN PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE

%token<nane> INTEGERLITERAL
%token<name> CHARACTERLITERAL 
%token<name> FLOATINGPOINTLITERAL
%token<name> BOOLEANLITERAL

%union{
	char* name;
}
%debug

%%

Goal:
	CompilationUnit
;

Literal:
	IntegerLiteral {$$ =$1;}
	| FloatingPointLiteral {$$ =$1;}
	| BooleanLiteral {$$ =$1;}
	| CharacterLiteral {$$ =$1;}
	| StringLiteral {$$ =$1;}
	| NullLiteral {$$ =$1;}
;

IntegerLiteral: INTEGERLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

FloatingPointLiteral: FLOATINGPOINTLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

BooleanLiteral: BOOLEANLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

CharacterLiteral: CHARACTERLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

StringLiteral: STRINGLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

NullLiteral: NULLLITERAL {$$=countNodes; countNodes++;  nodeType.push_back($1);  }
;

Identifier: IDENTIFIER {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;

Type:
	PrimitiveType {$$ =$1;}
	| ReferenceType {$$ =$1;}
;

PrimitiveType:
	NumericType {$$ =$1;}
	| BOOLEAN {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;

NumericType:
	IntegralType {$$=$1;}
	| FloatingPointType {$$=$1;}
;

IntegralType:
	BYTE {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| SHORT {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| INT {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| LONG {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| CHAR {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;

FloatingPointType:
	FLOAT {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| DOUBLE {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;

ReferenceType:
	ClassOrInterfaceType {$$=$1;}
	| ArrayType {$$=$1;}
;

ClassOrInterfaceType:
	Name {$$=$1;}
;

ClassType:
	ClassOrInterfaceType {$$=$1;}
;

InterfaceType:
	ClassOrInterfaceType {$$=$1;}
;

ArrayType:
	PrimitiveType OPSQR CLSQR  {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;}
	| Name OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;}
	| ArrayType OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;}
;


Name:
	SimpleName {$$=$1;}
  | QualifiedName {$$=$1;}
;

SimpleName: Identifier {$$=$1;}
;

QualifiedName: Name DOT Identifier {nodeType.push_back($2); nodeType.push_back("QualifiedName"); $$=countNodes+1; adj[$$].push_back($1);adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;}
;

CompilationUnit:
	| PackageDeclaration ImportDeclarations TypeDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back($3);countNodes++;}
	| ImportDeclarations TypeDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
	| PackageDeclaration TypeDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
	| PackageDeclaration ImportDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
	| PackageDeclaration {$$=$1;}
	| ImportDeclarations {$$=$1;}
	| TypeDeclarations {$$=$1;}
;

ImportDeclarations:
	ImportDeclaration {$$=$1;}
	| ImportDeclarations ImportDeclaration {nodeType.push_back("ImportDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
;

TypeDeclarations:
	TypeDeclaration {$$=$1;}
	| TypeDeclarations TypeDeclaration {nodeType.push_back("TypeDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
;

PackageDeclaration:
	PACKAGE Name SEMICOLON {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("PackageDeclaration"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;}
;

ImportDeclaration:
	SingleTypeImportDeclaration {$$=$1;}
	| TypeImportOnDemandDeclaration {$$=$1;}
;

SingleTypeImportDeclaration:
	IMPORT Name SEMICOLON {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("SingleTypeImportDeclaration"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;}
;

TypeImportOnDemandDeclaration:
	IMPORT Name DOT ASTERIX SEMICOLON {nodeType.push_back($1); nodeType.push_back($3);nodeType.push_back($4); nodeType.push_back($5); nodeType.push_back("TypeImportOnDemandDeclaration"); $$=countNodes+4; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back(countNodes+2); adj[$$].push_back(countNodes+3);countNodes+=5;}
;

TypeDeclaration: 
	ClassDeclaration {$$=$1;}
	| InterfaceDeclaration {$$=$1;}
	| SEMICOLON {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;

Modifiers:
	Modifier {$$=$1;}
	| Modifiers Modifier {nodeType.push_back("Modifiers");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
;

Modifier:
	PUBLIC {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| PROTECTED {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| PRIVATE {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| STATIC {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| ABSTRACT {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| FINAL {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| NATIVE {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| SYNCHRONIZED {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| TRANSIENT {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| VOLATILE {$$=countNodes; countNodes++;  nodeType.push_back($1); }
;


ClassDeclaration: 
	Modifiers CLASS Identifier Super Interfaces ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);adj[$$].push_back($6);countNodes+=2;}
	| CLASS Identifier Super Interfaces ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;}
	| CLASS Identifier Interfaces ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); countNodes+=2;}
	| CLASS Identifier Super ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); countNodes+=2;}
	| CLASS Identifier ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;}
	| Modifiers CLASS Identifier Interfaces ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;}
	| Modifiers CLASS Identifier Super ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;}
	| Modifiers CLASS Identifier ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;}
;

Super: EXTENDS ClassType {nodeType.push_back($1); nodeType.push_back("Super"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2); countNodes+=2;}
;

Interfaces: IMPLEMENTS InterfaceTypeList {nodeType.push_back($1); nodeType.push_back("Interfaces"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2); countNodes+=2;}
;

InterfaceTypeList: 
	InterfaceType {$$=$1;}
	| InterfaceTypeList COMMA InterfaceType {nodeType.push_back($2); nodeType.push_back("InterfaceTypeList"); $$=countNodes+1; adj[$$].push_back($1);adj[$$].push_back(countNodes); adj[$$].push_back($3); countNodes+=2;}
;

ClassBody: 
	OPCURLY ClassBodyDeclarations CLCURLY {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("ClassBody"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;}
	| OPCURLY CLCURLY {nodeType.push_back($1); nodeType.push_back($2); nodeType.push_back("ClassBody"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;}
;

ClassBodyDeclarations: 
	ClassBodyDeclaration {$$=$1;}
	| ClassBodyDeclarations ClassBodyDeclaration {nodeType.push_back("ClassBodyDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
;

ClassBodyDeclaration: 
	ClassMemberDeclaration {$$=$1;}
	| StaticInitializer {$$=$1;}
	| ConstructorDeclaration {$$=$1;}
;

ClassMemberDeclaration: 
	FieldDeclaration {$$=$1;}
	| MethodDeclaration {$$=$1;}
;

FieldDeclaration: 
	Modifiers Type VariableDeclarators SEMICOLON {nodeType.push_back($4); nodeType.push_back("FieldDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back(countNodes); countNodes+=2;}
	| Type VariableDeclarators SEMICOLON {nodeType.push_back($3); nodeType.push_back("FieldDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back(countNodes); countNodes+=2;}
;

VariableDeclarators: 
	VariableDeclarator {$$=$1;}
	| VariableDeclarators COMMA VariableDeclarator {nodeType.push_back($2); nodeType.push_back("VariableDeclarators"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;}
;

VariableDeclarator: 
	VariableDeclaratorId {$$=$1;}
	| VariableDeclaratorId EQUALTO VariableInitializer {nodeType.push_back($2); nodeType.push_back("VariableDeclarator"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;}
;

VariableDeclaratorId: 
	Identifier {$$=$1;}
	| VariableDeclaratorId OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3);nodeType.push_back("VariableDeclaratorId"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;}
;

Primary:
	PrimaryNoNewArray {$$=$1;}
	| ArrayCreationExpression {$$=$1;}
;

PrimaryNoNewArray:
	Literal {$$=$1;}
	| THIS {$$=countNodes; countNodes++;  nodeType.push_back($1); }
	| OPROUND Expression CLROUND {nodeType.push_back($1); nodeType.push_back($3);nodeType.push_back("PrimaryNoNewArray"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;}
	| ClassInstanceCreationExpression {$$=$1;}
	| FieldAccess {$$=$1;}
	| ArrayAccess {$$=$1;}
	| MethodInvocation {$$=$1;}
;

ClassInstanceCreationExpression:
	NEW ClassType OPROUND CLROUND {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($4); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($1);adj[$$].push_back(countNodes+1);adj[$$].push_back(countNodes+2);countNodes+=4;}
	| NEW ClassType OPROUND ArgumentList CLROUND {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($5); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($1);adj[$$].push_back(countNodes+1);adj[$$].push_back($4);adj[$$].push_back(countNodes+2);countNodes+=4;}
;

ArgumentList:
	Expression {$$=$1;}
	| ArgumentList COMMA Expression {nodeType.push_back($2); nodeType.push_back("ArgumentList"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;}
;

ArrayCreationExpression:
	NEW PrimitiveType DimExprs {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;}
	| NEW PrimitiveType DimExprs Dims {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;}
	| NEW ClassOrInterfaceType DimExprs {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;}
	| NEW ClassOrInterfaceType DimExprs Dims {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;}
;

DimExprs:
	DimExpr {$$=$1;}
	| DimExprs DimExpr {nodeType.push_back("DimExprs");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;}
;

DimExpr:
	OPSQR Expression CLSQR {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("DimExpr"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;}
;

Dims:
	OPSQR CLSQR {nodeType.push_back($1); nodeType.push_back($2); nodeType.push_back("Dims"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;}
	| Dims OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("Dims"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;}
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
	PostfixExpression DPLUS
;

PostDecrementExpression:
	PostfixExpression DMINUS
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
	LeftHandSide AssignmentOperator AssignmentExpression
;

AssignmentOperator:
	EQUALTO
	| STAREQUALTO
	| BYEQUALTO
	| PLUSEQUALTO
	| MINUSEQUALTO
	| TWOLEFTSHIFTEQUALTO
	| TWORIGHTSHIFTEQUALTO
	| THREERIGHTSHIFTEQUALTO
	| ANDEQUALTO
	| XOREQUALTO
	| BAREQUALTO

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
	InterfaceMemberDeclaration	{$$ = $1;}
	| InterfaceMemberDeclarations InterfaceMemberDeclaration {$$ = countNodes; nodeType.push_back("InterfaceMemberDeclarations"); adj[countNodes].push_back($1); adj[countNodes].push_back($2) ; countNodes++;}
;

InterfaceMemberDeclaration: 
	ConstantDeclaration {$$ = $1;}
	| AbstractMethodDeclaration {$$ = $1;}
;

ConstantDeclaration: FieldDeclaration {$$ = $1;}
;

AbstractMethodDeclaration: MethodHeader SEMICOLON {$$ = countNodes; nodeType.push_back("AbstractMethodDeclaration"); adj[countNodes].push_back($1); adj[countNodes].push_back($2); countNodes++;}
;

ArrayInitializer: 
	OPCURLY VariableInitializers COMMA CLCURLY {
													nodeType.push_back($1);
													int opcurl = countNodes;
													countNodes++;
													nodeType.push_back($3);
													int comma = countNodes;
													countNodes++;
													nodeType.push_back($4);
													int clcurl = countNodes;
													countNodes++;
													$$ = countNodes;
													nodeType.push_back("ArrayInitializer");
													adj[countNodes].push_back(opcurl);
													adj[countNodes].push_back(comma);
													adj[countNodes].push_back(clcurl);
													countNodes++;
												}
	| OPCURLY COMMA CLCURLY {nodeType.push_back($1); int opcurl = countNodes; countNodes++; nodeType.push_back($2); int comma = countNodes; countNodes++; nodeType.push_back($3); int clcurl = countNodes; countNodes++;
								$$ = countNodes;
								nodeType.push_back("ArrayInitializer");
								adj[countNodes].push_back(opcurl);
								adj[countNodes].push_back(comma);
								adj[countNodes].push_back(clcurl);
								countNodes++;
							}
	| OPCURLY VariableInitializers CLCURLY{
		nodeType.push_back($1); int opcurl = countNodes; countNodes++;
		nodeType.push_back($3); int clcurl = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ArrayInitializer");
		adj[countNodes].push_back(opcurl);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(clcurl);
		countNodes++; 

	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int opcurl = countNodes;countNodes++;
		nodeType.push_back($2); int clcurl = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ArrayInitializer");
		adj[countNodes].push_back(opcurl);
		adj[countNodes].push_back(clcurl);
		countNodes++;
	}
;

VariableInitializers: 
	VariableInitializer {$$ = $1;}
	| VariableInitializers COMMA VariableInitializer{
		nodeType.push_back($2); int comma = countNodes; countNodes++;
		$$ =countNodes;
		nodeType.push_back("VariableInitializers");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(comma);
		adj[countNodes].push_back($3);
		countNodes++;
	}
;

Block:
	OPCURLY CLCURLY {
		nodeType.push_back($1); int opcurl = countNodes; countNodes++;
		nodeType.push_back($2); int clcurl = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Block");
		adj[countNodes].push_back(opcurl);
		adj[countNodes].push_back(clcurl);
		countNodes++;
	}
	| OPCURLY BlockStatements CLCURLY {
		nodeType.push_back($1); int opcurl = countNodes; countNodes++;
		nodeType.push_back($3); int clcurl = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Block");
		adj[countNodes].push_back(opcurl);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(clcurl);
		countNodes++;
	}
;

BlockStatements: 
	BlockStatement {$$ = $1;}
	| BlockStatements BlockStatement{
		$$ = countNodes;
		nodeType.push_back("BlockStatements");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

BlockStatement:
	LocalVariableDeclarationStatement {$$ = $1;}
	| Statement {$$ = $1;}
;

LocalVariableDeclarationStatement:
	LocalVariableDeclaration SEMICOLON {
		nodeType.push_back($2); int semicolon = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("LocalVariableDeclarationStatement");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(semicolon);
		countNodes++;
	}
;

LocalVariableDeclaration:
	Type VariableDeclarators{
		$$ = countNodes;
		nodeType.push_back("LocalVariableDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

Statement:
	StatementWithoutTrailingSubstatement {$$ = $1;}
	| LabeledStatement {$$=$1;}
	| IfThenStatement {$$=$1;}
	| IfThenElseStatement {$$=$1;}
	| WhileStatement {$$=$1;}
	| ForStatement {$$=$1;}
;

StatementNoShortIf:
	StatementWithoutTrailingSubstatement {$$=$1;}
	| LabeledStatementNoShortIf {$$=$1;}
	| IfThenElseStatementNoShortIf {$$=$1;}
	| WhileStatementNoShortIf {$$=$1;}
	| ForStatementNoShortIf {$$=$1;}
;

StatementWithoutTrailingSubstatement:
	Block {$$=$1;}
	| EmptyStatement {$$=$1;}
	| ExpressionStatement {$$=$1;}
	| SwitchStatement {$$=$1;}
	| DoStatement {$$=$1;}
	| BreakStatement {$$=$1;}
	| ContinueStatement {$$=$1;}
	| ReturnStatement {$$=$1;}
	| SynchronizedStatement {$$=$1;}
	| ThrowStatement {$$=$1;}
	| TryStatement {$$=$1;}

EmptyStatement:
	SEMICOLON {
		nodeType.push_back($1);
		$$ = countNodes;
		countNodes++;
	}
;

LabeledStatement:
	Identifier COLON Statement{
		nodeType.push_back($2); int colon = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("LabeledStatement");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(colon);
		adj[countNodes].push_back($3);
		countNodes++;
	}
;

LabeledStatementNoShortIf:
	Identifier COLON StatementNoShortIf{
		nodeType.push_back($2); int colon = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("LabeledStatementNoShortIf");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(colon);
		adj[countNodes].push_back($3);
		countNodes++;
	}
;

ExpressionStatement:
	StatementExpression SEMICOLON{
		nodeType.push_back($2); int semicolon = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExpressionStatement");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(semicolon);
		countNodes++;
	}
;

StatementExpression:
	Assignment {$$=$1;}
	| PreIncrementExpression {$$=$1;}
	| PreDecrementExpression {$$=$1;}
	| PostIncrementExpression {$$=$1;}
	| PostDecrementExpression {$$=$1;}
	| MethodInvocation {$$=$1;}
	| ClassInstanceCreationExpression {$$=$1;}
;

IfThenStatement:
	IF OPROUND Expression CLROUND Statement {
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n3 = countNodes; countNodes++;
		nodeType.push_back($5); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("IfThenStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		countNodes++;
	}
;

IfThenElseStatement:
	IF OPROUND Expression CLROUND StatementNoShortIf ELSE Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("IfThenElseStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++; 
	}
;

IfThenElseStatementNoShortIf:
	IF OPROUND Expression CLROUND StatementNoShortIf ELSE StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("IfThenElseStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++; 
	}
;

SwitchStatement:
	SWITCH OPROUND Expression CLROUND SwitchBlock{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++; 
	}
;

SwitchBlock:
	OPCURLY CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchBlock");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++; 
	}
	| OPCURLY SwitchLabels CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchBlock");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n2);
		countNodes++; 
	}
	| OPCURLY SwitchBlockStatementGroups CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchBlock");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n2);
		countNodes++; 
	}
	| OPCURLY SwitchBlockStatementGroups SwitchLabels CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($4); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchBlock");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++; 
	}
;

SwitchBlockStatementGroups:
	SwitchBlockStatementGroup {$$ = $1;}
	| SwitchBlockStatementGroups SwitchBlockStatementGroup{
		$$ = countNodes;
		nodeType.push_back("SwitchBlockStatementGroups");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

SwitchBlockStatementGroup:
	SwitchLabels BlockStatements{
		$$ = countNodes;
		nodeType.push_back("SwitchBlockStatementGroup");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

SwitchLabels:
	SwitchLabel {$$ = $1;}
	| SwitchLabels SwitchLabel{
		$$ = countNodes;
		nodeType.push_back("SwitchLabels");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

SwitchLabel:
	CASE ConstantExpression COLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchLabel");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n2);
		countNodes++; 
	}
	| DEFAULT COLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchLabel");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++; 
	}
;

WhileStatement:
	WHILE OPROUND Expression CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("WhileStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++; 
	}
;

WhileStatementNoShortIf:
	WHILE OPROUND Expression CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("WhileStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++; 
	}
;

DoStatement:
	DO Statement WHILE OPROUND Expression CLROUND SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("DoStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back(n7);
		countNodes++; 
	}
;

ForStatement:
	  FOR OPROUND ForInit SEMICOLON Expression SEMICOLON ForUpdate CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		nodeType.push_back($8); int n8 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		adj[countNodes].push_back(n8);
		adj[countNodes].push_back($9);
		countNodes++;
	  }
	| FOR OPROUND         SEMICOLON Expression SEMICOLON ForUpdate CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON ForUpdate CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON Expression SEMICOLON           CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON            SEMICOLON ForUpdate CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON Expression SEMICOLON           CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON           CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON            SEMICOLON           CLROUND Statement{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
	}
;

ForStatementNoShortIf:
	  FOR OPROUND ForInit SEMICOLON Expression SEMICOLON ForUpdate CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		nodeType.push_back($8); int n8 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		adj[countNodes].push_back(n8);
		adj[countNodes].push_back($9);
		countNodes++;
	  }
	| FOR OPROUND         SEMICOLON Expression SEMICOLON ForUpdate CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON ForUpdate CLROUND {
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON Expression SEMICOLON           CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		nodeType.push_back($7); int n7 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back(n7);
		adj[countNodes].push_back($8);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON            SEMICOLON ForUpdate CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON Expression SEMICOLON           CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON           CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back(n6);
		adj[countNodes].push_back($7);
		countNodes++;
	}
	| FOR OPROUND         SEMICOLON            SEMICOLON           CLROUND StatementNoShortIf{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ForStatementNoShortIf");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		adj[countNodes].push_back($6);
	}
;

ForInit:
	StatementExpressionList {$$ = $1;}
	| LocalVariableDeclaration {$$ = $1;}
;

ForUpdate:
	StatementExpressionList {$$ = $1;}
;

StatementExpressionList:
	StatementExpression {$$ = $1;}
	| StatementExpressionList COMMA StatementExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("StatementExpressionList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
	}
;

BreakStatement:
	BREAK Identifier SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("BreakStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
	| BREAK SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("BreakStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
;

ContinueStatement:
	CONTINUE Identifier SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ContinueStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
	| CONTINUE SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ContinueStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
;

ReturnStatement:
	RETURN Expression SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ReturnStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
	| RETURN SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ReturnStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
;

ThrowStatement:
	THROW Expression SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ThrowStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
;

SynchronizedStatement:
	SYNCHRONIZED OPROUND Expression CLROUND Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SynchronizedStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++;
	}
;

TryStatement: 
	TRY Block Catches{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("TryStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
	}
	| TRY Block Catches Finally{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("TryStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;
	}
	| TRY Block Finally{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("TryStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
	}
;

Catches:
	CatchClause {$$ = $1;}
	| Catches CatchClause{
		$$ = countNodes;
		nodeType.push_back("Catches");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;

CatchClause:
	CATCH OPROUND FormalParameter CLROUND Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("CatchClause");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++;
	}
;

Finally: FINALLY Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Finally");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
;


%%

int yyerror(char* s){
    printf("Error: %s on line %d\n",s,yylineno);
    exit(0);
}

int main()
{
	// YYDEBUG = 1;
    yyparse(); 
    return 0;
}