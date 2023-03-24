%{
#include<bits/stdc++.h>
#include "attr.h"
#include "treeTraversal.h"

#define YYDEBUG 1
using namespace std;

extern int yylineno;
int yylex();
int yyerror(char *s);

vector<string> threeAC;

int rootNodenum=0;

%}

%token <name> SUPER IF ELSE SWITCH CASE DEFAULT WHILE DO FOR BREAK THROW TRY FINALLY CATCH VOID THROWS EXTENDS IMPLEMENTS CLASS INTERFACE BOOLEAN SHORT INT LONG CHAR FLOAT DOUBLE INSTANCEOF THIS NEW 
%token <name> EOL SEPARATOR  LITERAL IDENTIFIER WHITESPACE OTHER EQUALTO STAREQUALTO BYEQUALTO PLUSEQUALTO MINUSEQUALTO TWOLEFTSHIFTEQUALTO TWORIGHTSHIFTEQUALTO THREERIGHTSHIFTEQUALTO ANDEQUALTO XOREQUALTO BAREQUALTO
%token <name> OPROUND CLROUND OPSQR CLSQR DOT PLUS MINUS COLON SEMICOLON TILDA EX QUES ASTERIX FSLASH MOD LSHIFT RSHIFT URSHIFT LT GT LTE GTE DOUBLEEQ NOTEQ AND XOR OR DAND DOR DPLUS DMINUS
%token <name> PACKAGE STRINGLITERAL NULLLITERAL IMPORT BYTE OPCURLY CLCURLY COMMA CONTINUE RETURN PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE

%token<name> INTEGERLITERAL
%token<name> CHARACTERLITERAL
%token<name> FLOATINGPOINTLITERAL
%token<name> DOUBLEPOINTLITERAL
%token<name> BOOLEANLITERAL
%type<intval> MethodHeader_ IfThenElseStatementNoShortIf VariableDeclaratorId ExtendsInterfaces Modifiers Throws PrimaryNoNewArray Identifier SingleTypeImportDeclaration SwitchBlock StatementExpressionList Block ForStatement FieldAccess ArrayType Expression ClassOrInterfaceType LabeledStatementNoShortIf Name NullLiteral LabeledStatement InterfaceTypeList AssignmentExpression IfThenStatement VariableDeclarator Literal QualifiedName PreIncrementExpression ReturnStatement EqualityExpression VariableDeclarators PrimitiveType ConditionalOrExpression LeftHandSide Primary Type UnaryExpressionNotPlusMinus PostfixExpression BlockStatements ConstantDeclaration ExpressionStatement TypeImportOnDemandDeclaration ClassDeclaration InclusiveOrExpression ForStatementNoShortIf NumericType StatementNoShortIf Super PostIncrementExpression ContinueStatement InterfaceMemberDeclaration StaticInitializer ShiftExpression ClassInstanceCreationExpression FloatingPointType ArgumentList PostDecrementExpression ClassBodyDeclaration SimpleName IntegralType VariableInitializer UnaryExpression FormalParameterList MethodBody InterfaceBody Finally AssignmentOperator RelationalExpression WhileStatement ClassTypeList ConstructorDeclarator StatementExpression IfThenElseStatement BreakStatement TypeDeclaration FloatingPointLiteral DoublePointLiteral SwitchBlockStatementGroups ClassType LocalVariableDeclaration MethodInvocation ConditionalAndExpression ClassBody FieldDeclaration AdditiveExpression DoStatement Catches Assignment AndExpression SwitchLabel MultiplicativeExpression ForInit ForUpdate FormalParameter ConstructorBody BooleanLiteral Dims Statement SwitchBlockStatementGroup WhileStatementNoShortIf TypeDeclarations ImportDeclaration BlockStatement StatementWithoutTrailingSubstatement ArrayCreationExpression ExplicitConstructorInvocation CastExpression ThrowStatement InterfaceMemberDeclarations ClassBodyDeclarations VariableInitializers SynchronizedStatement DimExprs ConditionalExpression ArrayAccess Interfaces SwitchLabels MethodHeader ReferenceType DimExpr CatchClause CharacterLiteral ConstantExpression Modifier ArrayInitializer MethodDeclaration SwitchStatement ConstructorDeclaration StringLiteral CompilationUnit ImportDeclarations ClassMemberDeclaration EmptyStatement IntegerLiteral AbstractMethodDeclaration TryStatement InterfaceType PackageDeclaration ExclusiveOrExpression InterfaceDeclaration MethodDeclarator LocalVariableDeclarationStatement PreDecrementExpression 

%union{
	char* name;
	int intval;
}

%debug

%%

Goal:
	CompilationUnit {rootNodenum=$1;}
;

Literal:
	IntegerLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| FloatingPointLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=2;
		}
	| DoublePointLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=3;
	}
	| BooleanLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=4;
		}
	| CharacterLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=5;
	}
	| StringLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=6;
	}
	| NullLiteral {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Literal");
		adj[$$].push_back($1);
		prodNum[$$]=7;
		}
;

IntegerLiteral: INTEGERLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("IntegerLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

FloatingPointLiteral: FLOATINGPOINTLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("FloatingPointLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

DoublePointLiteral: DOUBLEPOINTLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("DoublePointLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

BooleanLiteral: BOOLEANLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("BooleanLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

CharacterLiteral: CHARACTERLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("CharacterLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

StringLiteral: STRINGLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("StringLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

NullLiteral: NULLLITERAL {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("NullLiteral");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

Identifier: IDENTIFIER {
	nodeType.push_back($1);
	countNodes++;
	$$=countNodes;
	countNodes++;
	nodeType.push_back("Identifier");
	adj[$$].push_back(countNodes-2);
	prodNum[$$]=1;
}
;

Type:
	PrimitiveType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Type");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ReferenceType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Type");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

PrimitiveType:
	NumericType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimitiveType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| BOOLEAN {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("PrimitiveType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
;

NumericType:
	IntegralType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("NumericType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| FloatingPointType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("NumericType");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

IntegralType:
	BYTE {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("IntegralType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=1;
	}
	| SHORT {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("IntegralType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
	| INT {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("IntegralType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=3;
	}
	| LONG {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("IntegralType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=4;
	}
	| CHAR {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("IntegralType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=5;
	}
;

FloatingPointType:
	FLOAT {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("FloatingPointType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=1;
	}
	| DOUBLE {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("FloatingPointType");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
;

ReferenceType:
	ClassOrInterfaceType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ReferenceType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ArrayType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ReferenceType");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

ClassOrInterfaceType:
	Name {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassOrInterfaceType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
;

ClassType:
	ClassOrInterfaceType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
;

InterfaceType:
	ClassOrInterfaceType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("InterfaceType");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
;

ArrayType:
	PrimitiveType OPSQR CLSQR  {
		nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=1;
	}
	| Name OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=2;
	}
	| ArrayType OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("ArrayType"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=3;
	}
;


Name:
	SimpleName {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Name");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
  	| QualifiedName {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Name");
		adj[$$].push_back($1);
		prodNum[$$]=2;
  }
;

SimpleName: Identifier {
	$$ =countNodes;
		countNodes++;
		nodeType.push_back("SimpleName");
		adj[$$].push_back($1);
		prodNum[$$]=1;
}
;

QualifiedName: Name DOT Identifier {nodeType.push_back($2); nodeType.push_back("QualifiedName"); $$=countNodes+1; adj[$$].push_back($1);adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;
	prodNum[$$]=1;
}
;

CompilationUnit: PackageDeclaration ImportDeclarations TypeDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back($3);countNodes++;}
	| ImportDeclarations TypeDeclarations {
			nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
			prodNum[$$]=1;
		}
	| PackageDeclaration TypeDeclarations {
			nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
			prodNum[$$]=2;
		}
	| PackageDeclaration ImportDeclarations {
			nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
			prodNum[$$]=3;
		}
	| PackageDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("CompilationUnit");
		adj[$$].push_back($1);
		prodNum[$$]=4;
	}
	| ImportDeclarations {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("CompilationUnit");
		adj[$$].push_back($1);
		prodNum[$$]=5;
	}
	| TypeDeclarations {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("CompilationUnit");
		adj[$$].push_back($1);
		prodNum[$$]=6;
	}
;

ImportDeclarations:
	ImportDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ImportDeclarations");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ImportDeclarations ImportDeclaration {nodeType.push_back("ImportDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
		prodNum[$$]=2;
	}
;

TypeDeclarations:
	TypeDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("TypeDeclarations");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| TypeDeclarations TypeDeclaration {nodeType.push_back("TypeDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
		prodNum[$$]=2;
	}
;

PackageDeclaration:
	PACKAGE Name SEMICOLON {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("PackageDeclaration"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;prodNum[$$]=1;}
;

ImportDeclaration:
	SingleTypeImportDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ImportDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| TypeImportOnDemandDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ImportDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

SingleTypeImportDeclaration:
	IMPORT Name SEMICOLON {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("SingleTypeImportDeclaration"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=1;
	}
;

TypeImportOnDemandDeclaration:
	IMPORT Name DOT ASTERIX SEMICOLON {nodeType.push_back($1); nodeType.push_back($3);nodeType.push_back($4); nodeType.push_back($5); nodeType.push_back("TypeImportOnDemandDeclaration"); $$=countNodes+4; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back(countNodes+2); adj[$$].push_back(countNodes+3);countNodes+=5;
		prodNum[$$]=1;
	}
;

TypeDeclaration: 
	ClassDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("TypeDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| InterfaceDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("TypeDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
	| SEMICOLON {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("TypeDeclaration");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=3;
	}
;

Modifiers:
	Modifier {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Modifiers");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| Modifiers Modifier {nodeType.push_back("Modifiers");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
		prodNum[$$]=1;
	}
;

Modifier:
	PUBLIC {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=1;
	}
	| PROTECTED {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
	| PRIVATE {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=3;
	}
	| STATIC {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=4;
	}
	| ABSTRACT {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=5;
	}
	| FINAL {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=6;
	}
	| NATIVE {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=7;
	}
	| SYNCHRONIZED {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=8;
	}
	| TRANSIENT {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=9;
	}
	| VOLATILE {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("Modifier");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=10;
	}
;


ClassDeclaration: 
	Modifiers CLASS Identifier Super Interfaces ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);adj[$$].push_back($6);countNodes+=2;
		prodNum[$$]=1;
	}
	| CLASS Identifier Super Interfaces ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;
		prodNum[$$]=2;
	}
	| CLASS Identifier Interfaces ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); countNodes+=2;
		prodNum[$$]=3;
	}
	| CLASS Identifier Super ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4); countNodes+=2;
		prodNum[$$]=4;
	}
	| CLASS Identifier ClassBody {nodeType.push_back($1); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=5;
	}
	| Modifiers CLASS Identifier Interfaces ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;
		prodNum[$$]=6;
	}
	| Modifiers CLASS Identifier Super ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4); adj[$$].push_back($5);countNodes+=2;
		prodNum[$$]=7;
	}
	| Modifiers CLASS Identifier ClassBody {nodeType.push_back($2); nodeType.push_back("ClassDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;
		prodNum[$$]=8;
	}
;

Super: EXTENDS ClassType {
	nodeType.push_back($1); nodeType.push_back("Super"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2); countNodes+=2; prodNum[$$]=1;
	}
;

Interfaces: IMPLEMENTS InterfaceTypeList {
		nodeType.push_back($1); nodeType.push_back("Interfaces"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2); countNodes+=2;
		prodNum[$$]=1;
	}
;

InterfaceTypeList: 
	InterfaceType {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("InterfaceTypeList");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| InterfaceTypeList COMMA InterfaceType {
		nodeType.push_back($2); nodeType.push_back("InterfaceTypeList"); $$=countNodes+1; adj[$$].push_back($1);adj[$$].push_back(countNodes); adj[$$].push_back($3); countNodes+=2;
		prodNum[$$]=2;
	}
;

ClassBody: 
	OPCURLY ClassBodyDeclarations CLCURLY {
		nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("ClassBody"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=1;	
	}
	| OPCURLY CLCURLY {
		nodeType.push_back($1); nodeType.push_back($2); nodeType.push_back("ClassBody"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=2;	
	}
;

ClassBodyDeclarations: 
	ClassBodyDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassBodyDeclarations");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ClassBodyDeclarations ClassBodyDeclaration {
		nodeType.push_back("ClassBodyDeclarations");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
		prodNum[$$]=2;	
	}
;

ClassBodyDeclaration: 
	ClassMemberDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassBodyDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| StaticInitializer {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassBodyDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
	| ConstructorDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassBodyDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=3;
	}
;

ClassMemberDeclaration: 
	FieldDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassMemberDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| MethodDeclaration {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassMemberDeclaration");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

FieldDeclaration: 
	Modifiers Type VariableDeclarators SEMICOLON { 
												   nodeType.push_back($4); nodeType.push_back("FieldDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back(countNodes); countNodes+=2;
													prodNum[$$]=1;
													lineNum[$$]=yylineno;
												}
	| Type VariableDeclarators SEMICOLON {	
											nodeType.push_back($3); nodeType.push_back("FieldDeclaration"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back(countNodes); countNodes+=2;
										 	prodNum[$$]=2;
											lineNum[$$]=yylineno;
										 }
;

VariableDeclarators: 
	VariableDeclarator {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("VariableDeclarators");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| VariableDeclarators COMMA VariableDeclarator {
		nodeType.push_back($2); nodeType.push_back("VariableDeclarators"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=2;
		}
;

VariableDeclarator: 
	VariableDeclaratorId {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("VariableDeclarator");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| VariableDeclaratorId EQUALTO VariableInitializer {
		nodeType.push_back($2); nodeType.push_back("VariableDeclarator"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=2;
		}
;

VariableDeclaratorId: 
	Identifier {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("VariableDeclaratorId");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| VariableDeclaratorId OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3);nodeType.push_back("VariableDeclaratorId"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=2;
	}
;

Primary:
	PrimaryNoNewArray {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Primary");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ArrayCreationExpression {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Primary");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

PrimaryNoNewArray:
	Literal {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| THIS {
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
	| OPROUND Expression CLROUND {nodeType.push_back($1); nodeType.push_back($3);nodeType.push_back("PrimaryNoNewArray"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=3;
	}
	| ClassInstanceCreationExpression {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back($1);
		prodNum[$$]=4;
	}
	| FieldAccess {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back($1);
		prodNum[$$]=5;
	}
	| ArrayAccess {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back($1);
		prodNum[$$]=6;
	}
	| MethodInvocation {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PrimaryNoNewArray");
		adj[$$].push_back($1);
		prodNum[$$]=7;
	}
;

ClassInstanceCreationExpression:
	NEW ClassType OPROUND CLROUND {
		nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($4); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back(countNodes+2);countNodes+=4;
		prodNum[$$]=1;
		}
	| NEW ClassType OPROUND ArgumentList CLROUND {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($5); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back($4);adj[$$].push_back(countNodes+2);countNodes+=4;
		prodNum[$$]=2;
	}
;

ArgumentList:
	Expression {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ArgumentList");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ArgumentList COMMA Expression {nodeType.push_back($2); nodeType.push_back("ArgumentList"); $$=countNodes+1; adj[$$].push_back($1); adj[$$].push_back(countNodes);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=2;
	}
;

ArrayCreationExpression:
	NEW PrimitiveType DimExprs {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=1;
	}
	| NEW PrimitiveType DimExprs Dims {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;
		prodNum[$$]=2;
	}
	| NEW ClassOrInterfaceType DimExprs {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);countNodes+=2;
		prodNum[$$]=3;
	}
	| NEW ClassOrInterfaceType DimExprs Dims {nodeType.push_back($1); nodeType.push_back("ArrayCreationExpression"); $$=countNodes+1; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back($3);adj[$$].push_back($4);countNodes+=2;
		prodNum[$$]=4;
	}
;

DimExprs:
	DimExpr {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("DimExprs");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}	
	| DimExprs DimExpr {nodeType.push_back("DimExprs");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);countNodes++;
		prodNum[$$]=2;
	}
;

DimExpr:
	OPSQR Expression CLSQR {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back("DimExpr"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=1;
	}
;

Dims:
	OPSQR CLSQR {nodeType.push_back($1); nodeType.push_back($2); nodeType.push_back("Dims"); $$=countNodes+2; adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=1;
	}
	| Dims OPSQR CLSQR {nodeType.push_back($2); nodeType.push_back($3); nodeType.push_back("Dims"); $$=countNodes+2; adj[$$].push_back($1); adj[$$].push_back(countNodes); adj[$$].push_back(countNodes+1);countNodes+=3;
		prodNum[$$]=2;
	}
;

FieldAccess:
	Primary DOT Identifier{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("FieldAccess");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=1;
	}
	| SUPER DOT Identifier{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("FieldAccess");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;	
	}
;

MethodInvocation:
	Name OPROUND CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		prodNum[$$]=1;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		countNodes++;
	}
	| Name OPROUND ArgumentList CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		prodNum[$$]=2;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;	

	}
	| Primary DOT Identifier OPROUND CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		prodNum[$$]=3;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		countNodes++;	
	}
	| Primary DOT Identifier OPROUND ArgumentList CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		countNodes++;	

	}
	| SUPER DOT Identifier OPROUND CLROUND {
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		countNodes++;	

		prodNum[$$]=5;
	}
	| SUPER DOT Identifier OPROUND ArgumentList CLROUND{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($6); int n6 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		adj[countNodes].push_back(n6);
		countNodes++;	

		prodNum[$$]=6;
	}
;

ArrayAccess:
	Name OPSQR Expression CLSQR{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ArrayAccess");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;

		prodNum[$$]=1;	
	}
	| PrimaryNoNewArray OPSQR Expression CLSQR{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ArrayAccess");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;	

		prodNum[$$]=2;
	}
;

PostfixExpression:
	Primary{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PostfixExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| Name{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PostfixExpression");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
	| PostIncrementExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PostfixExpression");
		adj[$$].push_back($1);
		prodNum[$$]=3;
	}
	| PostDecrementExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("PostfixExpression");
		adj[$$].push_back($1);
		prodNum[$$]=4;
	}
;

PostIncrementExpression:
	PostfixExpression DPLUS{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("PostIncrementExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		countNodes++;	
		prodNum[$$]=1;
	}
;

PostDecrementExpression:
	PostfixExpression DMINUS{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("PostDecrementExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		countNodes++;	

		prodNum[$$]=1;
	}
;

UnaryExpression:
	PreIncrementExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("UnaryExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| PreDecrementExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("UnaryExpression");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
	| PLUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=3;
	}
	| MINUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=4;
	}
	| UnaryExpressionNotPlusMinus{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("UnaryExpression");
		adj[$$].push_back($1);
		prodNum[$$]=5;
	}
;

PreIncrementExpression:
	DPLUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("PreIncrementExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=1;
	}
;

PreDecrementExpression:
	DMINUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("PreDecrementExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=1;
	}
;

UnaryExpressionNotPlusMinus:
	PostfixExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| TILDA UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=2;	
	}
	| EX UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=3;	
	}
	| CastExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[$$].push_back($1);
		prodNum[$$]=4;
	}
;

CastExpression:
	OPROUND PrimitiveType CLROUND UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("CastExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		countNodes++;
		prodNum[$$]=1;	
	}
	| OPROUND PrimitiveType Dims CLROUND UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("CastExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++;	
		prodNum[$$]=2;
	}
	| OPROUND Expression CLROUND UnaryExpressionNotPlusMinus{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("CastExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($4);
		countNodes++;	
		prodNum[$$]=3;
	}
	| OPROUND Name Dims CLROUND UnaryExpressionNotPlusMinus{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("CastExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++;	
		prodNum[$$]=4;
	}
;

MultiplicativeExpression:
	UnaryExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("MultiplicativeExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| MultiplicativeExpression ASTERIX UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;	
	}
	| MultiplicativeExpression FSLASH UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=3;	
	}
	| MultiplicativeExpression MOD UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=4;
	}
;

AdditiveExpression:
	MultiplicativeExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("AdditiveExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| AdditiveExpression PLUS MultiplicativeExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AdditiveExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
	| AdditiveExpression MINUS MultiplicativeExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AdditiveExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=3;
	}
;

ShiftExpression:
	AdditiveExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ShiftExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ShiftExpression LSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
	| ShiftExpression RSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=3;
	}
	| ShiftExpression URSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=4;
	}
;

RelationalExpression:
	ShiftExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("RelationalExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| RelationalExpression LT ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
	| RelationalExpression GT ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=3;
	}
	| RelationalExpression LTE ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=4;
	}
	| RelationalExpression GTE ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=5;
	}
	| RelationalExpression INSTANCEOF ReferenceType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=6;	
	}
;

EqualityExpression:
	RelationalExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("EqualityExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| EqualityExpression DOUBLEEQ RelationalExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("EqualityExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
	| EqualityExpression NOTEQ RelationalExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("EqualityExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=3;
	}
;

AndExpression:
	EqualityExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("AndExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| AndExpression AND EqualityExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AndExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;	
	}
;

ExclusiveOrExpression:
	AndExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ExclusiveOrExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ExclusiveOrExpression XOR AndExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExclusiveOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
;

InclusiveOrExpression:
	ExclusiveOrExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("InclusiveOrExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| InclusiveOrExpression OR ExclusiveOrExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InclusiveOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
;

ConditionalAndExpression:
	InclusiveOrExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ConditionalAndExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ConditionalAndExpression DAND InclusiveOrExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConditionalAndExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
;

ConditionalOrExpression:
	ConditionalAndExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ConditionalOrExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ConditionalOrExpression DOR ConditionalAndExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConditionalOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;	
	}
;

ConditionalExpression:
	ConditionalOrExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ConditionalExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ConditionalOrExpression QUES Expression COLON ConditionalExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConditionalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back($5);
		countNodes++;	
		prodNum[$$]=2;
	}
;

AssignmentExpression:
	ConditionalExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("AssignmentExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| Assignment{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("AssignmentExpression");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

Assignment:
	LeftHandSide AssignmentOperator AssignmentExpression{
		$$ = countNodes;
		nodeType.push_back("Assignment");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=1;	
	}
;

AssignmentOperator:
	EQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=1;
	}
	| STAREQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
	| BYEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=3; 
	}
	| PLUSEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=4;
	}
	| MINUSEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=5;
	}
	| TWOLEFTSHIFTEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=6;
	}
	| TWORIGHTSHIFTEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=7; 
	}
	| THREERIGHTSHIFTEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=8;
	}
	| ANDEQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=9;  
	}
	| XOREQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=10;  
	}
	| BAREQUALTO{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("AssignmentOperator");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=11;  
	}

LeftHandSide:
	Name{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("LeftHandSide");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| FieldAccess{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("LeftHandSide");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
	| ArrayAccess{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("LeftHandSide");
		adj[$$].push_back($1);
		prodNum[$$]=3;
	}
;

Expression:
	AssignmentExpression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("Expression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
;

ConstantExpression: 
	Expression {
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ConstantExpression");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
;

VariableInitializer: 
	Expression{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("VariableInitializer");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| ArrayInitializer{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("VariableInitializer");
		adj[$$].push_back($1);
		prodNum[$$]=2;
	}
;

MethodDeclaration: MethodHeader_ MethodBody {
	$$ = countNodes;
	nodeType.push_back("MethodDeclaration");
	adj[countNodes].push_back($1);
	adj[countNodes].push_back($2);
	countNodes++;	
	prodNum[$$]=1;
	lineNum[$$]=lineNum[$1];
}
;

MethodHeader_: MethodHeader {
	$$ =countNodes;
	countNodes++;
	nodeType.push_back("MethodHeader_");
	adj[$$].push_back($1);
	prodNum[$$]=1;
	lineNum[$$]=yylineno;
}

MethodHeader: 
	Modifiers Type MethodDeclarator Throws{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;	
		prodNum[$$]=1;
	}
	| Type MethodDeclarator Throws{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
	| Modifiers Type MethodDeclarator {
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=3;
	}
	| Type MethodDeclarator{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=4;
	}
	| Modifiers VOID MethodDeclarator Throws{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;	
		prodNum[$$]=5;
	}
	| Modifiers VOID MethodDeclarator {
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=6;
	}
	| VOID MethodDeclarator Throws{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=7;
	}
	| VOID MethodDeclarator{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=8;
	}
;

MethodDeclarator: 
	Identifier OPROUND FormalParameterList CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodDeclarator");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;	
		prodNum[$$]=1;
	}
	| Identifier OPROUND CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodDeclarator");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		countNodes++;	
		prodNum[$$]=2;
	}

;

FormalParameterList: 
	FormalParameter{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("FormalParameterList");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| FormalParameterList COMMA FormalParameter{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("FormalParameterList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
;

FormalParameter: Type VariableDeclaratorId{
		$$ = countNodes;
		nodeType.push_back("FormalParameter");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
}
;

Throws: THROWS ClassTypeList{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Throws");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
		prodNum[$$]=1;
}
;

ClassTypeList: 
	ClassType{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("ClassTypeList");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
    | ClassTypeList COMMA ClassType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ClassTypeList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
		prodNum[$$]=2;
	}
;

MethodBody: 
	Block{
		$$ =countNodes;
		countNodes++;
		nodeType.push_back("MethodBody");
		adj[$$].push_back($1);
		prodNum[$$]=1;
	}
	| SEMICOLON{
		nodeType.push_back($1);
		countNodes++;
		$$=countNodes;
		countNodes++;
		nodeType.push_back("MethodBody");
		adj[$$].push_back(countNodes-2);
		prodNum[$$]=2;
	}
;

StaticInitializer: STATIC Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("StaticInitializer");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
}
;

ConstructorDeclaration:	
	Modifiers ConstructorDeclarator Throws ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;
		prodNum[$$]=1;
	}
	| ConstructorDeclarator Throws ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;
	}
	| ConstructorDeclarator ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=3;
	}
	| Modifiers ConstructorDeclarator ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=4;
	}
;

ConstructorDeclarator: 
	SimpleName OPROUND FormalParameterList CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclarator");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;
		prodNum[$$]=1;
	}
	| SimpleName OPROUND CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclarator");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=2;
	}
;

ConstructorBody: 
	OPCURLY ExplicitConstructorInvocation BlockStatements CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		countNodes++;
		prodNum[$$]=1;
	}
	| OPCURLY BlockStatements CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=2;
	}
	| OPCURLY ExplicitConstructorInvocation CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=3;
	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++;
		prodNum[$$]=4;
	}
;

ExplicitConstructorInvocation: 
	THIS OPROUND ArgumentList CLROUND SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExplicitConstructorInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		countNodes++;
		prodNum[$$]=1;	
	}
	| THIS OPROUND CLROUND SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExplicitConstructorInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		countNodes++;
		prodNum[$$]=2;	
	}
	| SUPER OPROUND ArgumentList CLROUND SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		nodeType.push_back($5); int n5 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExplicitConstructorInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n4);
		adj[countNodes].push_back(n5);
		countNodes++;
		prodNum[$$]=3;	
	}
	| SUPER OPROUND CLROUND SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		nodeType.push_back($4); int n4 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExplicitConstructorInvocation");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back(n4);
		countNodes++;
		prodNum[$$]=4;	
	}
;


InterfaceDeclaration: 
	Modifiers INTERFACE Identifier ExtendsInterfaces InterfaceBody{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		adj[countNodes].push_back($5);
		countNodes++;
		prodNum[$$]=1;
	}
	| INTERFACE Identifier ExtendsInterfaces InterfaceBody{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceDeclaration");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;
		prodNum[$$]=2;
	}
	| Modifiers INTERFACE Identifier InterfaceBody{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;
		prodNum[$$]=3;
	}
	| INTERFACE Identifier InterfaceBody{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceDeclaration");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=4;
	}
;

ExtendsInterfaces: 
	EXTENDS InterfaceType{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExtendsInterfaces");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
	}
	| ExtendsInterfaces COMMA InterfaceType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExtendsInterfaces");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;
	}
;

InterfaceBody: 
	OPCURLY InterfaceMemberDeclarations CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=1;
	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++;
		prodNum[$$]=2;
	}
;


InterfaceMemberDeclarations: 
	InterfaceMemberDeclaration	{
		$$ = countNodes;
		nodeType.push_back("InterfaceMemberDeclarations");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| InterfaceMemberDeclarations InterfaceMemberDeclaration {$$ = countNodes; nodeType.push_back("InterfaceMemberDeclarations"); adj[countNodes].push_back($1); adj[countNodes].push_back($2) ; countNodes++; prodNum[$$]=2; }
;

InterfaceMemberDeclaration: 
	ConstantDeclaration {
		$$ = countNodes;
		nodeType.push_back("InterfaceMemberDeclaration");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| AbstractMethodDeclaration {
		$$ = countNodes;
		nodeType.push_back("InterfaceMemberDeclaration");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
;

ConstantDeclaration: 
	FieldDeclaration {
		$$ = countNodes;
		nodeType.push_back("ConstantDeclaration");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
;

AbstractMethodDeclaration: MethodHeader SEMICOLON {
		nodeType.push_back($2); 
		int n2 = countNodes;
		countNodes++; 
		$$ = countNodes; 
		nodeType.push_back("AbstractMethodDeclaration"); 
		adj[countNodes].push_back($1); 
		adj[countNodes].push_back(n2); 
		countNodes++;
		prodNum[$$]=1;
	}
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
													adj[countNodes].push_back($2);
													adj[countNodes].push_back(comma);
													adj[countNodes].push_back(clcurl);
													countNodes++;
													prodNum[$$]=1;
												}
	| OPCURLY COMMA CLCURLY {nodeType.push_back($1); int opcurl = countNodes; countNodes++; nodeType.push_back($2); int comma = countNodes; countNodes++; nodeType.push_back($3); int clcurl = countNodes; countNodes++;
								$$ = countNodes;
								nodeType.push_back("ArrayInitializer");
								adj[countNodes].push_back(opcurl);
								adj[countNodes].push_back(comma);
								adj[countNodes].push_back(clcurl);
								countNodes++;
								prodNum[$$]=2;
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
		prodNum[$$]=3;

	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int opcurl = countNodes;countNodes++;
		nodeType.push_back($2); int clcurl = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ArrayInitializer");
		adj[countNodes].push_back(opcurl);
		adj[countNodes].push_back(clcurl);
		countNodes++;
		prodNum[$$]=4;
	}
;

VariableInitializers: 
	VariableInitializer {
		$$ = countNodes;
		nodeType.push_back("VariableInitializers");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| VariableInitializers COMMA VariableInitializer{
		nodeType.push_back($2); int comma = countNodes; countNodes++;
		$$ =countNodes;
		nodeType.push_back("VariableInitializers");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(comma);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
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
		prodNum[$$]=2;
	}
;

BlockStatements: 
	BlockStatement {		
		$$ = countNodes;
		nodeType.push_back("BlockStatements");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| BlockStatements BlockStatement{
		$$ = countNodes;
		nodeType.push_back("BlockStatements");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=2;
	}
;

BlockStatement:
	LocalVariableDeclarationStatement {		
		$$ = countNodes;
		nodeType.push_back("BlockStatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
		lineNum[$$]=yylineno;
	}
	| Statement {		
		$$ = countNodes;
		nodeType.push_back("BlockStatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
;

LocalVariableDeclarationStatement:
	LocalVariableDeclaration SEMICOLON {
		nodeType.push_back($2); int semicolon = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("LocalVariableDeclarationStatement");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(semicolon);
		countNodes++;
		prodNum[$$]=1;
	}
;

LocalVariableDeclaration:
	Type VariableDeclarators{
		$$ = countNodes;
		nodeType.push_back("LocalVariableDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
	}
;

Statement:
	StatementWithoutTrailingSubstatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| LabeledStatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
	| IfThenStatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=3; 
	}
	| IfThenElseStatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=4; 
	}
	| WhileStatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=5; 
	}
	| ForStatement {		
		$$ = countNodes;
		nodeType.push_back("Statement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=6; 
	}
;

StatementNoShortIf:
	StatementWithoutTrailingSubstatement {
		$$ = countNodes;
		nodeType.push_back("StatementNoShortIf");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| LabeledStatementNoShortIf {
		$$ = countNodes;
		nodeType.push_back("StatementNoShortIf");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
	| IfThenElseStatementNoShortIf {
		$$ = countNodes;
		nodeType.push_back("StatementNoShortIf");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=3;
	}
	| WhileStatementNoShortIf {
		$$ = countNodes;
		nodeType.push_back("StatementNoShortIf");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=4; 
	}
	| ForStatementNoShortIf {
		$$ = countNodes;
		nodeType.push_back("StatementNoShortIf");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=5; 
	}
;

StatementWithoutTrailingSubstatement:
	Block {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| EmptyStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
	| ExpressionStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=3; 
	}
	| SwitchStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=4; 
	}
	| DoStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=5; 
	}
	| BreakStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=6; 
	}
	| ContinueStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=7; 
	}
	| ReturnStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=8; 
	}
	| SynchronizedStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=9; 
	}
	| ThrowStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=10; 
	}
	| TryStatement {
		$$ = countNodes;
		nodeType.push_back("StatementWithoutTrailingSubstatement");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=11; 
	}

EmptyStatement:
	SEMICOLON {
		nodeType.push_back($1);
		$$ = countNodes;
		countNodes++;
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
	}
;

StatementExpression:
	Assignment {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| PreIncrementExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
	}
	| PreDecrementExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=3; 
	}
	| PostIncrementExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=4; 
	}
	| PostDecrementExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=5; 
	}
	| MethodInvocation {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=6; 
	}
	| ClassInstanceCreationExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpression");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=7; 
	}
;

IfThenStatement:
	IF OPROUND Expression CLROUND Statement {
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($4); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("IfThenStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($5);
		countNodes++;
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=2;
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
		prodNum[$$]=3;
	}
	| OPCURLY SwitchBlockStatementGroups SwitchLabels CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($4); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchBlock");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n2);
		countNodes++; 
		prodNum[$$]=4;
	}
;

SwitchBlockStatementGroups:
	SwitchBlockStatementGroup {
		$$ = countNodes;
		nodeType.push_back("SwitchBlockStatementGroups");
		adj[countNodes].push_back($1);
		countNodes++;
		prodNum[$$]=1;
	}
	| SwitchBlockStatementGroups SwitchBlockStatementGroup{
		$$ = countNodes;
		nodeType.push_back("SwitchBlockStatementGroups");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
	}
;

SwitchBlockStatementGroup:
	SwitchLabels BlockStatements{
		$$ = countNodes;
		nodeType.push_back("SwitchBlockStatementGroup");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
	}
;

SwitchLabels:
	SwitchLabel {
		$$ = countNodes;
		nodeType.push_back("SwitchLabels");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
	| SwitchLabels SwitchLabel{
		$$ = countNodes;
		nodeType.push_back("SwitchLabels");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
	}
	| DEFAULT COLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("SwitchLabel");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++; 
		prodNum[$$]=2;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=2;
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
		prodNum[$$]=3;
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
		prodNum[$$]=4;
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
		prodNum[$$]=5;
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
		prodNum[$$]=6;
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
		prodNum[$$]=7;
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
		prodNum[$$]=8;
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
		prodNum[$$]=1;
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
		prodNum[$$]=2;
	}
	| FOR OPROUND ForInit SEMICOLON            SEMICOLON ForUpdate CLROUND StatementNoShortIf{
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
		prodNum[$$]=3;
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
		prodNum[$$]=4;
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
		prodNum[$$]=5;
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
		prodNum[$$]=6;
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
		prodNum[$$]=7;
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
		prodNum[$$]=8;
	}
;

ForInit:
	StatementExpressionList {
		$$ = countNodes;
		nodeType.push_back("ForInit");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
		lineNum[$$]=yylineno;
	}
	| LocalVariableDeclaration {
		$$ = countNodes;
		nodeType.push_back("ForInit");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=2; 
		lineNum[$$]=yylineno;
	}
;

ForUpdate:
	StatementExpressionList {
		$$ = countNodes;
		nodeType.push_back("ForUpdate");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1; 
	}
;

StatementExpressionList:
	StatementExpression {
		$$ = countNodes;
		nodeType.push_back("StatementExpressionList");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1;
	}
	| StatementExpressionList COMMA StatementExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("StatementExpressionList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
	}
	| BREAK SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("BreakStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
	}
	| CONTINUE SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ContinueStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
	}
	| RETURN SEMICOLON{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ReturnStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n3);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=1;
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
		prodNum[$$]=2;
	}
	| TRY Block Finally{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("TryStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
		prodNum[$$]=3;
	}
;

Catches:
	CatchClause {
		$$ = countNodes;
		nodeType.push_back("Catches");
		adj[countNodes].push_back($1);
		countNodes++; 
		prodNum[$$]=1;
	}
	| Catches CatchClause{
		$$ = countNodes;
		nodeType.push_back("Catches");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=2;
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
		prodNum[$$]=1;
	}
;

Finally: FINALLY Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Finally");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
		prodNum[$$]=1;
	}
;


%%

int yyerror(char* s){
    printf("Error: %s on line %d\n",s,yylineno);
    exit(0);
}

int main(int argc, char* argv[])
{
	int flag=0;
	for(int i=0;i<argc;i++){
		string s = argv[i];
		if(s.substr(0, 7) == "--input"){
			string filename="";
			for(int i=8;i<s.size();i++){
				filename+=s[i];
			}
			freopen(filename.c_str(),"r",stdin);
		}
		if(s.substr(0, 8) == "--output"){
			string filename="";
			for(int i=9;i<s.size();i++){
				filename+=s[i];
			}
			freopen(filename.c_str(),"w",stdout);
			flag=1;
		}
		if(s.substr(0, 9) == "--verbose"){
			yydebug = 1;
		}
		if(s.substr(0, 6) == "--help"){
			printf("You can use the following flags : \n");
			printf("1. --input=[FILENAME]: To specify from where to take the input. \n");
			printf("2. --output=[FILENAME] : To specify where to redirect the output. \n");
			printf("3. --verbose : To print detailed error messages.\n");
			printf("4. --help : To display help regarding the flags. \n");
		}
	}

    yyparse(); 

	initializeAttributeVectors();
	
	preOrderTraversal(rootNodenum);

	printTables();

	if(!flag)freopen("output.dot","w",stdout);
	cout << "// dot -Tps output.dot -o out.ps\n\n"
		<< "graph \"Tree\"\n"
		<< "{\n"
		<< "\tfontname=\"Helvetica,Arial,sans-serif\"\n"
    	 << "\tnode [fontsize=10, width=\".2\", height=\".2\", margin=0]\n"
		 << "\tedge [fontsize=6]\n"
    	 << "\tgraph[fontsize=8];\n\n"
    	 << "\tlabel=\"Abstract Syntax Tree\"\n\n";

	for(int i=0;i<nodeType.size();i++){
		cout << "\tn" << i << ";\n";
		cout << "\tn" << i << "[label=\"" ;
		for(int t=0;t<nodeType[i].length();t++){
			if(nodeType[i][t]=='"'){
				if(t>0){
					if(nodeType[i][t]!='\\'){
						cout << "\\" << nodeType[i][t];
					}else cout << nodeType[i][t];
				}else{
					cout << "\\" << nodeType[i][t];
				}
			}else cout << nodeType[i][t];
		}
		cout <<"\"];\n";
		auto child = adj[i];
		for(int j=0;j<child.size();j++){
			cout << "\tn" << i << "--" << 'n' << child[j] << ";\n";
		}
		cout << endl;
	}
	cout << "}" << endl;

    return 0;
}