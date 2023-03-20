%{
#include<bits/stdc++.h>
#define YYDEBUG 1
using namespace std;

extern int yylineno;
int yylex();
int yyerror(char *s);

int countNodes=0;
vector<string> nodeType;
map<int, vector<int>> adj;

typedef struct localtableparams{
	string name;
	string type;
	pair<int,int> scope;
	pair<int,int> parentScope;
	int offset;
	vector<localtableparams>* functionTablePointer;
	vector<string> functionParams;
	string functionReturnType;
} localTableParams ;

typedef struct globaltableparams{
	string name;
	string type;
	vector<localTableParams>* localTablePointer; 
} globalTableParams;

vector<globalTableParams> globalTable;

vector<string> threeAC;

%}

%token <name> SUPER IF ELSE SWITCH CASE DEFAULT WHILE DO FOR BREAK THROW TRY FINALLY CATCH VOID THROWS EXTENDS IMPLEMENTS CLASS INTERFACE BOOLEAN SHORT INT LONG CHAR FLOAT DOUBLE INSTANCEOF THIS NEW 
%token <name> EOL SEPARATOR  LITERAL IDENTIFIER WHITESPACE OTHER EQUALTO STAREQUALTO BYEQUALTO PLUSEQUALTO MINUSEQUALTO TWOLEFTSHIFTEQUALTO TWORIGHTSHIFTEQUALTO THREERIGHTSHIFTEQUALTO ANDEQUALTO XOREQUALTO BAREQUALTO
%token <name> OPROUND CLROUND OPSQR CLSQR DOT PLUS MINUS COLON SEMICOLON TILDA EX QUES ASTERIX FSLASH MOD LSHIFT RSHIFT URSHIFT LT GT LTE GTE DOUBLEEQ NOTEQ AND XOR OR DAND DOR DPLUS DMINUS
%token <name> PACKAGE STRINGLITERAL NULLLITERAL IMPORT BYTE OPCURLY CLCURLY COMMA CONTINUE RETURN PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE

%token<name> INTEGERLITERAL
%token<name> CHARACTERLITERAL
%token<name> FLOATINGPOINTLITERAL
%token<name> BOOLEANLITERAL
%type<intval> IfThenElseStatementNoShortIf VariableDeclaratorId ExtendsInterfaces Modifiers Throws PrimaryNoNewArray Identifier SingleTypeImportDeclaration SwitchBlock StatementExpressionList Block ForStatement FieldAccess ArrayType Expression ClassOrInterfaceType LabeledStatementNoShortIf Name NullLiteral LabeledStatement InterfaceTypeList AssignmentExpression IfThenStatement VariableDeclarator Literal QualifiedName PreIncrementExpression ReturnStatement EqualityExpression VariableDeclarators PrimitiveType ConditionalOrExpression LeftHandSide Primary Type UnaryExpressionNotPlusMinus PostfixExpression BlockStatements ConstantDeclaration ExpressionStatement TypeImportOnDemandDeclaration ClassDeclaration InclusiveOrExpression ForStatementNoShortIf NumericType StatementNoShortIf Super PostIncrementExpression ContinueStatement InterfaceMemberDeclaration StaticInitializer ShiftExpression ClassInstanceCreationExpression FloatingPointType ArgumentList PostDecrementExpression ClassBodyDeclaration SimpleName IntegralType VariableInitializer UnaryExpression FormalParameterList MethodBody InterfaceBody Finally AssignmentOperator RelationalExpression WhileStatement ClassTypeList ConstructorDeclarator StatementExpression IfThenElseStatement BreakStatement TypeDeclaration FloatingPointLiteral SwitchBlockStatementGroups ClassType LocalVariableDeclaration MethodInvocation ConditionalAndExpression ClassBody FieldDeclaration AdditiveExpression DoStatement Catches Assignment AndExpression SwitchLabel MultiplicativeExpression ForInit ForUpdate FormalParameter ConstructorBody BooleanLiteral Dims Statement SwitchBlockStatementGroup WhileStatementNoShortIf TypeDeclarations ImportDeclaration BlockStatement StatementWithoutTrailingSubstatement ArrayCreationExpression ExplicitConstructorInvocation CastExpression ThrowStatement InterfaceMemberDeclarations ClassBodyDeclarations VariableInitializers SynchronizedStatement DimExprs ConditionalExpression ArrayAccess Interfaces SwitchLabels MethodHeader ReferenceType DimExpr CatchClause CharacterLiteral ConstantExpression Modifier ArrayInitializer MethodDeclaration SwitchStatement ConstructorDeclaration StringLiteral CompilationUnit ImportDeclarations ClassMemberDeclaration EmptyStatement IntegerLiteral AbstractMethodDeclaration TryStatement InterfaceType PackageDeclaration ExclusiveOrExpression InterfaceDeclaration MethodDeclarator LocalVariableDeclarationStatement PreDecrementExpression 

%union{
	char* name;
	int intval;
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

CompilationUnit: PackageDeclaration ImportDeclarations TypeDeclarations {nodeType.push_back("CompilationUnit");$$=countNodes; adj[$$].push_back($1); adj[$$].push_back($2);adj[$$].push_back($3);countNodes++;}
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
	NEW ClassType OPROUND CLROUND {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($4); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back(countNodes+2);countNodes+=4;}
	| NEW ClassType OPROUND ArgumentList CLROUND {nodeType.push_back($1); nodeType.push_back($3); nodeType.push_back($5); nodeType.push_back("ClassInstanceCreationExpression"); $$=countNodes+3; adj[$$].push_back(countNodes); adj[$$].push_back($2);adj[$$].push_back(countNodes+1);adj[$$].push_back($4);adj[$$].push_back(countNodes+2);countNodes+=4;}
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
	Primary DOT Identifier{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("FieldAccess");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
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
	}
;

MethodInvocation:
	Name OPROUND CLROUND{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
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
	}
;

PostfixExpression:
	Primary{
		$$ = $1;
	}
	| Name{
		$$ = $1;
	}
	| PostIncrementExpression{
		$$ = $1;
	}
	| PostDecrementExpression{
		$$ = $1;
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
	}
;

UnaryExpression:
	PreIncrementExpression{
		$$ = $1;
	}
	| PreDecrementExpression{
		$$ = $1;
	}
	| PLUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
	}
	| MINUS UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpression");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
	}
	| UnaryExpressionNotPlusMinus{
		$$ = $1;
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
	}
;

UnaryExpressionNotPlusMinus:
	PostfixExpression{
		$$ = $1;
	}
	| TILDA UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
	}
	| EX UnaryExpression{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("UnaryExpressionNotPlusMinus");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
	}
	| CastExpression{
		$$ = $1;
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
	}
;

MultiplicativeExpression:
	UnaryExpression{
		$$ = $1;
	}
	| MultiplicativeExpression ASTERIX UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| MultiplicativeExpression FSLASH UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| MultiplicativeExpression MOD UnaryExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MultiplicativeExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

AdditiveExpression:
	MultiplicativeExpression{
		$$ = $1;
	}
	| AdditiveExpression PLUS MultiplicativeExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AdditiveExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| AdditiveExpression MINUS MultiplicativeExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AdditiveExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

ShiftExpression:
	AdditiveExpression{
		$$ = $1;
	}
	| ShiftExpression LSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| ShiftExpression RSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| ShiftExpression URSHIFT AdditiveExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ShiftExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

RelationalExpression:
	ShiftExpression{
		$$ = $1;
	}
	| RelationalExpression LT ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| RelationalExpression GT ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| RelationalExpression LTE ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| RelationalExpression GTE ShiftExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| RelationalExpression INSTANCEOF ReferenceType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("RelationalExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

EqualityExpression:
	RelationalExpression{
		$$ = $1;
	}
	| EqualityExpression DOUBLEEQ RelationalExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("EqualityExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| EqualityExpression NOTEQ RelationalExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("EqualityExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

AndExpression:
	EqualityExpression{
		$$ = $1;
	}
	| AndExpression AND EqualityExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("AndExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

ExclusiveOrExpression:
	AndExpression{
		$$ = $1;
	}
	| ExclusiveOrExpression XOR AndExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExclusiveOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

InclusiveOrExpression:
	ExclusiveOrExpression{
		$$ = $1;
	}
	| InclusiveOrExpression OR ExclusiveOrExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InclusiveOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

ConditionalAndExpression:
	InclusiveOrExpression{
		$$ = $1;
	}
	| ConditionalAndExpression DAND InclusiveOrExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConditionalAndExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

ConditionalOrExpression:
	ConditionalAndExpression{
		$$ = $1;
	}
	| ConditionalOrExpression DOR ConditionalAndExpression{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConditionalOrExpression");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

ConditionalExpression:
	ConditionalOrExpression{
		$$ = $1;
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
	}
;

AssignmentExpression:
	ConditionalExpression{
		$$ = $1;
	}
	| Assignment{
		$$ = $1;
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
	}
;

AssignmentOperator:
	EQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| STAREQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| BYEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| PLUSEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| MINUSEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| TWOLEFTSHIFTEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| TWORIGHTSHIFTEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| THREERIGHTSHIFTEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| ANDEQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| XOREQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}
	| BAREQUALTO{
		$$=countNodes;
		countNodes++;  
		nodeType.push_back($1);  
	}

LeftHandSide:
	Name{
		$$ = $1;
	}
	| FieldAccess{
		$$ = $1;
	}
	| ArrayAccess{
		$$ = $1;
	}
;

Expression:
	AssignmentExpression{
		$$ = $1;
	}
;

ConstantExpression: 
	Expression {
		$$ = $1;
	}
;

VariableInitializer: 
	Expression{
		$$ = $1;
	}
	| ArrayInitializer{
		$$ = $1;
	}
;

MethodDeclaration: MethodHeader MethodBody{
	$$ = countNodes;
	nodeType.push_back("MethodDeclaration");
	adj[countNodes].push_back($1);
	adj[countNodes].push_back($2);
	countNodes++;	
}
;

MethodHeader: 
	Modifiers Type MethodDeclarator Throws{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back($4);
		countNodes++;	
	}
	| Type MethodDeclarator Throws{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
	| Modifiers Type MethodDeclarator {
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	

	}
	| Type MethodDeclarator{
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;	

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

	}
	| Modifiers VOID MethodDeclarator {
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	

	}
	| VOID MethodDeclarator Throws{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;	

	}
	| VOID MethodDeclarator{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodHeader");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	

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
	}
	| MethodDeclarator OPSQR CLSQR{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		nodeType.push_back($3); int n3 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("MethodDeclarator");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back(n3);
		countNodes++;	
	}
;

FormalParameterList: 
	FormalParameter{
		$$ = $1;
	}
	| FormalParameterList COMMA FormalParameter{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("FormalParameterList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

FormalParameter: Type VariableDeclaratorId{
		$$ = countNodes;
		nodeType.push_back("FormalParameter");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
}
;

Throws: THROWS ClassTypeList{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("Throws");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;	
}
;

ClassTypeList: 
	ClassType{
		$$ = $1;
	}
    | ClassTypeList COMMA ClassType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ClassTypeList");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;	
	}
;

MethodBody: 
	Block{
		$$ = $1;
	}
	| SEMICOLON{
		$$ = countNodes;
		countNodes++;
		nodeType.push_back($1);
	}
;

StaticInitializer: STATIC Block{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("StaticInitializer");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		countNodes++;
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
	}
	| ConstructorDeclarator Throws ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
	}
	| ConstructorDeclarator ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		countNodes++;
	}
	| Modifiers ConstructorDeclarator ConstructorBody{
		$$ = countNodes;
		nodeType.push_back("ConstructorDeclaration");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
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
	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ConstructorBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++;
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
	}
	| INTERFACE Identifier InterfaceBody{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceDeclaration");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		countNodes++;
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
	}
	| ExtendsInterfaces COMMA InterfaceType{
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("ExtendsInterfaces");
		adj[countNodes].push_back($1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		countNodes++;
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
	}
	| OPCURLY CLCURLY{
		nodeType.push_back($1); int n1 = countNodes; countNodes++;
		nodeType.push_back($2); int n2 = countNodes; countNodes++;
		$$ = countNodes;
		nodeType.push_back("InterfaceBody");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		countNodes++;
	}
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

AbstractMethodDeclaration: MethodHeader SEMICOLON {
		nodeType.push_back($2); 
		int n2 = countNodes;
		countNodes++; 
		$$ = countNodes; 
		nodeType.push_back("AbstractMethodDeclaration"); 
		adj[countNodes].push_back($1); 
		adj[countNodes].push_back(n2); 
		countNodes++;
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
		$$ = countNodes;
		nodeType.push_back("IfThenStatement");
		adj[countNodes].push_back(n1);
		adj[countNodes].push_back(n2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n3);
		adj[countNodes].push_back($5);
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
		adj[countNodes].push_back($2);
		adj[countNodes].push_back($3);
		adj[countNodes].push_back(n2);
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
		adj[countNodes].push_back($1);
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