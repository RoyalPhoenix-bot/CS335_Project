%{

#include<bits/stdc++.h>
using namespace std;

int yylex();
int yyerror(char *s);


%}

%token <name> KEYWORD EOL SEPARATOR ASSIGNMENTOPERATOR LITERAL IDENTIFIER WHITESPACE OTHER
%token <name> OPROUND CLROUND OPSQR CLSQR THIS DOT SUPER NEW PLUS MINUS COLON SEMICOLON TILDA EX QUES ASTERIX FSLASH MOD LSHIFT RSHIFT URSHIFT LT GT LTE GTE INSTANCEOF DOUBLEEQ NOTEQ AND XOR OR DAND DOR 

%union{
	char* name;
}

%%
input: javasyntax;

javasyntax: keywords | javasyntax keywords
	| identifier | javasyntax identifier
;

Primary:
	PrimaryNoNewArray
	| ArrayCreationExpression
;

PrimaryNoNewArray:
	LITERAL
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
	| NEW ClassOrInterfaceTypes DimExprs
	| NEW ClassOrInterfaceTypes DimExprs Dims
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
	Primary DOT identifier
	| SUPER DOT identifier
;

MethodInvocation:
	Name OPROUND CLROUND
	| Name OPROUND ArgumentList CLROUND
	| Primary DOT identifier OPROUND CLROUND
	| Primary DOT identifier OPROUND ArgumentList CLROUND
	| SUPER DOT identifier OPROUND CLROUND 
	| SUPER DOT identifier OPROUND ArgumentList CLROUND
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
	PLUS PLUS UnaryExpression
;

PreDecrementExpression:
	MINUS MINUS UnaryExpression
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

LeftHandSide:
	Name
	| FieldAccess
	| ArrayAccess
;

Expression:
	AssignmentExpression
;

ConstantExpression: Expression;

keywords: KEYWORD ;

identifier: IDENTIFIER ;

AssignmentOperator: ASSIGNMENTOPERATOR;

ExpressionName: IDENTIFIER;

ArrayInitializer: OPCURLY VariableInitializers COMMA CLCURLY 
				| OPCURLY COMMA CLCURLY
				| OPCURLY VariableInitializers CLCURLY
				| OPCURLY CLCURLY
;

VariableInitializers: VariableInitializer
					| VariableInitializers COMMA VariableInitializer
;

InterfaceDeclaration: Modifiers interface Identifier ExtendsInterfaces InterfaceBody
					| interface Identifier ExtendsInterfaces InterfaceBody
					| Modifiers interface Identifier InterfaceBody
					| interface Identifier InterfaceBody
;

ExtendsInterfaces: extends InterfaceType
				 | ExtendsInterfaces COMMA InterfaceType
;

InterfaceBody: OPCURLY InterfaceMemberDeclarations CLCURLY
			 | OPCURLY CLCURLY
;

InterfaceMemberDeclarations: InterfaceMemberDeclaration
						   | InterfaceMemberDeclarations InterfaceMemberDeclaration
;

InterfaceMemberDeclaration: ConstantDeclaration 
						  |	AbstractMethodDeclaration
;

ConstantDeclaration: FieldDeclaration
;

AbstractMethodDeclaration: MethodHeader SEMICOLON
;

ClassDeclaration: Modifiers class Identifier Super Interfaces ClassBody
				| class Identifier Super Interfaces ClassBody
				| class Identifier Interfaces ClassBody
				| class Identifier Super ClassBody
				| class Identifier ClassBody
				| Modifiers class Identifier Interfaces ClassBody
				| Modifiers class Identifier Super ClassBody
				| Modifiers class Identifier ClassBody
;

Super: extends ClassType
;

Interfaces: implements InterfaceTypeList
;

InterfaceTypeList: InterfaceType
				 | InterfaceTypeList COMMA InterfaceType
;

ClassBody: OPCURLY ClassBodyDeclarationsopt CLCURLY
;

ClassBodyDeclarations: ClassBodyDeclaration
					 | ClassBodyDeclarations ClassBodyDeclaration
;

ClassBodyDeclaration: ClassMemberDeclaration
					| StaticInitializer
					| ConstructorDeclaration
;

ClassMemberDeclaration: FieldDeclaration
					  | MethodDeclaration
;

FieldDeclaration: Modifiers Type VariableDeclarators SEMICOLON
				| Type VariableDeclarators SEMICOLON
;

VariableDeclarators: VariableDeclarator
				   | VariableDeclarators COMMA VariableDeclarator
;

VariableDeclarator: VariableDeclaratorId
				  | VariableDeclaratorId EQUALTO VariableInitializer

VariableDeclaratorId: Identifier
					| VariableDeclaratorId OPSQR CLSQR
;

VariableInitializer: Expression
				   | ArrayInitializer
;

MethodDeclaration: MethodHeader MethodBody
;

MethodHeader: Modifiers Type MethodDeclarator Throws
			| Type MethodDeclarator Throws
			| Modifiers Type MethodDeclarator 
			| Type MethodDeclarator
			| Modifiers void MethodDeclarator Throws
			| Modifiers void MethodDeclarator 
			| void MethodDeclarator Throws
			| void MethodDeclarator
;

MethodDeclarator: Identifier OPROUND FormalParameterList CLROUND
				| Identifier OPROUND CLROUND
				| MethodDeclarator OPSQR CLSQR
;

FormalParameterList: FormalParameter
				   | FormalParameterList COMMA FormalParameter
;

FormalParameter: Type VariableDeclaratorId
;

Throws: throws ClassTypeList
;

ClassTypeList: ClassType
     		 | ClassTypeList COMMA ClassType
;

MethodBody: Block SEMICOLON
;

StaticInitializer: static Block
;

ConstructorDeclaration:	Modifiers ConstructorDeclarator Throws ConstructorBody
					  | ConstructorDeclarator Throws ConstructorBody
					  | ConstructorDeclarator ConstructorBody
					  | Modifiers ConstructorDeclarator ConstructorBody
;

ConstructorDeclarator: SimpleName OPROUND FormalParameterList CLROUND
					 | SimpleName OPROUND CLROUND
;

ConstructorBody: OPCURLY ExplicitConstructorInvocation BlockStatements CLCURLY
			   | OPCURLY BlockStatements CLCURLY
			   | OPCURLY ExplicitConstructorInvocation CLCURLY
			   | OPCURLY CLCURLY
;

ExplicitConstructorInvocation: this OPROUND ArgumentList CLROUND SEMICOLON
							 | this OPROUND CLROUND SEMICOLON
							 | super OPROUND ArgumentList CLROUND SEMICOLON
							 | super OPROUND CLROUND SEMICOLON
;

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