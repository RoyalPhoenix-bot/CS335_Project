#include <bits/stdc++.h>
#include "attr.h"

using namespace std;


int countNodes=0;
int tempNum=0;
int countL=0;//counts the L in 'control flow statements' [as given in slides]
vector<string> nodeType;
map<int, vector<int>> adj;      
map<int,int> prodNum;

vector<int> helpScope;
stack<pair<int,int>> scopeCurr;
stack<pair<int,int>> scopeParent;

map<string, int> typeWiden = {{"double",0}, {"float",1}, {"long",2}, {"int",3}, {"byte",4}, {"short",4}, {"char",4}};//minimum value->widest
vector<string> indexType = {"double", "float", "long", "int", "byte"};

typedef struct localtableparams{
	string name;
	varTypes type;
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
vector<localTableParams> currSymTab;

vector<string> threeAC;


vector<localtableparams>* funcScope = nullptr; //to maintain function scope if any
vector<localtableparams>* classScope = nullptr; //to maintain class scope if any
int numFuncinClass=-1;

vector<attr> attrSymTab;
vector<attr> attr3AC;
vector<attr2> attrType;

string getNewLabel(){
    countL++;
    string r = "L" + to_string(countL);
    return r;
}

void initializeAttributeVectors(){
    for(int i=0;i<nodeType.size();i++){
        attr temp;
        attr2 temp2;
        attrSymTab.push_back(temp);
        attr3AC.push_back(temp);
        attrType.push_back(temp2);
    }
    return;
}

void execMethodInvocation(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:

            break;
        case 2:

            break;
    }

    return;
}

void execVariableInitializer(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
}

void execExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    return;
}

void execAssignmentExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
}

void execConditionalExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            string ltrue = getNewLabel();
            string lfalse = getNewLabel();
            string nextStmt = getNewLabel();

            attr3AC[nodeNum] = attr3AC[c];
            string temp = "if " + attr3AC[c].addrName + " goto " + ltrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + lfalse;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = ltrue + ":";
            (attr3AC[nodeNum].threeAC).push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c3];
            temp = "goto " + nextStmt;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = lfalse + ":";
            (attr3AC[nodeNum].threeAC).push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c5];
            temp = nextStmt + ":";
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
}

void execConditionalOrExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " || " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
}

void execConditionalAndExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " && " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        
    }
    return;
}

void execInclusiveOrExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " | " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
    return;
}

void execExclusiveOrExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " ^ " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
}

void execAndExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " & " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
}

void execEqualityExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " == " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " != " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
    return;
}

void execRelationalExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " < " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " > " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:{
            {
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " <= " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
        }
            break;
        case 5:{
            {
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " >= " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
        }
            break;
        case 6:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " instanceof " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
    return;
}

void execShiftExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " << " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " >> " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " >>> " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
}

void execAdditiveExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " + " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " - " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);

        }
            break;
    }
    return;
}

void execMultiplicativeExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " * " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " / " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " % " + attr3AC[c3].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
    }
    return;
}

void execUnaryExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:{
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
        }
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 3:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 4:{
            c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =- " + attr3AC[c].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 5:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
    return;
}

void execUnaryExpressionNotPlusMinus(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:{
            c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =~ " + attr3AC[c].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =! " + attr3AC[c].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }

    return;
}

void execPostfixExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 3:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 4:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
}

void execName(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
    return;
}

void execSimpleName(int nodeNum){
    int c = adj[nodeNum][0];//Get child node number
    attr3AC[nodeNum] = attr3AC[c];
    return;
}

void execIdentifier(int nodeNum){
    int c = adj[nodeNum][0];//Get child node number
    attr3AC[nodeNum].addrName = nodeType[c];//Add addrname of identifier 

    return;
}


bool isInt(string type1){
   if(type1=="byte") return 1;
   if(type1=="short") return 1;
   if(type1=="int") return 1;
   if(type1=="long") return 1;
   if(type1=="char") return 1;
   return 0;
}

bool isFloat (string type1){
   if(type1=="float") return 1;
   if(type1=="double") return 1;
   return 0;
}

bool isNumeric(string type1){
    return isInt(type1) || isFloat(type1);
}

bool isScalar(int a){
    return a==0;
}

//return 1 if found in globalScope, 2 if classScope, 3 if funcScope 
// NOTE: can return pointer to symbolTableEntry if required 
int checkIdentifierInScope(string nameIden, vector<localtableparams>* FuncScopePtr, vector<localtableparams>* classScopePtr){
    if(FuncScopePtr==nullptr && classScopePtr==nullptr){//in globalScope
        for(int i=0;i<globalTable.size();i++){
            if(globalTable[i].name == nameIden) return 1;
        }
        return 0;
    }
    else if(FuncScopePtr==nullptr){
        //check classScope and globalTable 
        for(int i=0;i<(*classScopePtr).size();i++){
            if((*classScopePtr)[i].name==nameIden) return 2;
        }

        for(int i=0;i<globalTable.size();i++){
            if(globalTable[i].name == nameIden) return 1;
        }
        return 0;
    }
    else{//check in funcSymTab, classSymTab, globalSymTab
        
        for(int i=0;i<(*FuncScopePtr).size();i++){
            if((*FuncScopePtr)[i].name==nameIden) return 3;
        }

        for(int i=0;i<(*classScope).size();i++){
            if((*classScope)[i].name==nameIden) return 2;
        }
        
        for(int i=0;i<globalTable.size();i++){
            if(globalTable[i].name == nameIden) return 1;
        }
        return 0;
    }
}

void preOrder(int nodenum){//root left right
    
    if(nodeType[nodenum]=="MethodDeclaration"){
        //update funcScope
        //no type checking in MethodHeader. need funcname;
        
        int i;//entry of classScope in globalTable

        for(i=0;i<globalTable.size();i++){
            if(globalTable[i].localTablePointer == classScope) break;
        }
        if(i==globalTable.size()){
            cout<<"symbol table entry of class not found"<<"\n";
            exit(0);
        }
        
        int c1 = adj[nodenum][0];
        string nameFunc = attrType[c1].funcname;

        for(i=0;i<(*classScope).size();i++){
            if((*classScope)[i].name==nameFunc){
                funcScope = (*classScope)[i].functionTablePointer;
                numFuncinClass=i;
                break;
            }
        }

        if(i==(*classScope).size()){
            cout<<"symbol table entry of function not found"<<"\n";
            exit(0);
        }

        int c2 = adj[nodenum][1];//body. type check        
        preOrder(c2);

        funcScope=nullptr;
    }

    else if(nodeType[nodenum]=="ClassDeclaration"){
        //update classScope. need the name of class
        int c1, c2;
        switch(prodNum[nodenum]){
            case 1:
            case 6:
            case 7:
            case 8:
                c1 = adj[nodenum][2];    
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                c1 = adj[nodenum][1];         
                break;
        }

        switch(prodNum[nodenum]){
            case 1:
                c2 = adj[nodenum][5];
            case 2:
            case 6:
            case 7:
                c2 = adj[nodenum][4];
            case 3:
            case 4:
            case 8:
                c2 = adj[nodenum][3];    
                break;
            case 5:
                c2 = adj[nodenum][2];
        }

        string nameClass = attrType[c1].classname;           
        //use nameClass to index into localtable from global table

        int i;//entry of classScope in globalTable

        for(i=0;i<globalTable.size();i++){
            if(globalTable[i].name == nameClass){
                classScope = globalTable[i].localTablePointer;
                break;
            }
        }
        if(i==globalTable.size()){
            cout<<"symbol table entry of class not found"<<"\n";
            exit(0);
        }
        preOrder(c2);

        classScope=nullptr;        
    }

    else if(nodeType[nodenum]=="IntegerLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "int"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="FloatingPointLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "float"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="DoublePointLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "double"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="BooleanLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "bool"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="CharacterLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "char"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="StringLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "string"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="NullLiteral"){
        int c = adj[nodenum][0]; attrType[c].type = "null"; attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="Literal"){
        int c = adj[nodenum][0]; 
        preOrder(c);
        attrType[nodenum].type = attrType[c].type;
        attrType[nodenum].nDims=attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="PrimaryNoNewArray"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims=attrType[c].nDims;
                break;
            }
            case 2:
                break;
            case 3:{
                int c = adj[nodenum][1];
                preOrder(c);
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims=attrType[c].nDims;
                break;
            }
            case 4:
                //TODO
                break;
            case 5:
                //TODO
                break;
            case 6:
                //TODO
                break;
            case 7:
                //TODO
                break;
        }
    }

    else if(nodeType[nodenum]=="Primary"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims=attrType[c].nDims;
                break;
            }
            case 2:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims=attrType[c].nDims;
                attrType[nodenum].classname=attrType[c].classname;
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="PostfixExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 2:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 3:{                
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 4:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
        }
    }

    else if(nodeType[nodenum]=="UnaryExpressionNotPlusMinus"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 2:{
                int c = adj[nodenum][1]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 3:{
                int c = adj[nodenum][1]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
            case 4:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; attrType[nodenum].nDims=attrType[c].nDims; break;
            }
        }
    }

    else if(nodeType[nodenum]=="UnaryExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;  attrType[nodenum].nDims = attrType[c].nDims;  break;

            }
            case 2:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;  attrType[nodenum].nDims = attrType[c].nDims;  break;

            }
            case 3:{
                int c = adj[nodenum][1]; preOrder(c); attrType[nodenum].type=attrType[c].type;  attrType[nodenum].nDims = attrType[c].nDims;  break;

            }
            case 4:{
                int c = adj[nodenum][1]; preOrder(c); attrType[nodenum].type=attrType[c].type;  attrType[nodenum].nDims = attrType[c].nDims;  break;

            }
            case 5:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;  attrType[nodenum].nDims = attrType[c].nDims;  break;

            }
        }
    }

    else if(nodeType[nodenum]=="MultiplicativeExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; 
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:
            case 3:
            case 4:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(isNumeric(attrType[c1].type) && isNumeric(attrType[c2].type)){
                    int a = typeWiden[attrType[c1].type];
                    a = min(a, typeWiden[attrType[c2].type]);
                    //CASTING DONE HERE
                    attrType[nodenum].type=indexType[a];
                }
                else{
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="AdditiveExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; 
                attrType[nodenum].nDims = attrType[c].nDims;
                
                break;
            }
            case 2:
            case 3:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;


                if(isNumeric(attrType[c1].type) && isNumeric(attrType[c2].type)){
                    int a = typeWiden[attrType[c1].type];
                    a = min(a, typeWiden[attrType[c2].type]);
                    //CASTING DONE HERE
                    attrType[nodenum].type=indexType[a];
                }
                else if(attrType[c1].type == "string" && attrType[c2].type == "string"){
                    attrType[nodenum].type="string";
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="ShiftExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; 
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:
            case 3:
            case 4:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(isInt(attrType[c1].type) && isInt(attrType[c2].type)){
                    attrType[nodenum].type="int";
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="RelationalExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; 
                preOrder(c); 
                attrType[nodenum].nDims = attrType[c].nDims;
                attrType[nodenum].type=attrType[c].type; break;

            }
            case 2:
            case 3:
            case 4:
            case 5:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;


                if(isNumeric(attrType[c1].type) && isNumeric(attrType[c2].type)){
                    attrType[nodenum].type="bool";  
                }
                else if(attrType[c1].type == "char" && attrType[c2].type == "char"){
                    attrType[nodenum].type="bool";
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
            }
            case 6:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                if(attrType[c1].classname == attrType[c2].classname){
                    attrType[nodenum].classname=attrType[c1].classname;
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }

            }

        }
    }

    else if(nodeType[nodenum]=="EqualityExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 2:
            case 3:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(attrType[c1].type == attrType[c2].type){
                    attrType[nodenum].type = "bool";
                }
                else{
                    cout<<"Type mismatch"<<"\n";
                    exit(0);
                }


            }
        }
    }

    else if(nodeType[nodenum]=="AndExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(isInt(attrType[c1].type) && isInt(attrType[c2].type)){

                    int a = typeWiden[attrType[c1].type];
                    a = min(a, typeWiden[attrType[c2].type]);
                    attrType[nodenum].type=indexType[a];
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }   
        }
    }

    else if(nodeType[nodenum]=="ExclusiveOrExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(isInt(attrType[c1].type) && isInt(attrType[c2].type)){
                    int a = typeWiden[attrType[c1].type];
                    a = min(a, typeWiden[attrType[c2].type]);
                    attrType[nodenum].type=indexType[a];
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }   
        }
    }    

    else if(nodeType[nodenum]=="InclusiveOrExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims;

                if(isInt(attrType[c1].type) && isInt(attrType[c2].type)){
                    int a = typeWiden[attrType[c1].type];
                    a = min(a, typeWiden[attrType[c2].type]);
                    attrType[nodenum].type=indexType[a];
                }
                else{
                    //TODO
                    cout<<"Type mismatch\n";
                    exit(0);
                }
                break;
            }   
        }
    }

    else if(nodeType[nodenum]=="ConditionalAndExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);
                //no type checking required
                attrType[nodenum].nDims = attrType[c1].nDims;
                attrType[nodenum].type = "bool";
                break;
            }   
        }
    }

    else if(nodeType[nodenum]=="ConditionalOrExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type; break;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);
                //no type checking required
                attrType[nodenum].nDims = attrType[c1].nDims;
                attrType[nodenum].type = "bool";
                break;
            }   
        }
    }

    else if(nodeType[nodenum]=="ConditionalExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                int c3 = adj[nodenum][4];
                preOrder(c1);
                preOrder(c2);
                preOrder(c3);
                if(attrType[c1].type != "bool"){
                    cout<<"Type mismatch"<<"\n";
                }

                attrType[nodenum].nDims = attrType[c2].nDims;
                attrType[nodenum].type = attrType[c2].type;
                break;
            }   
        }
    }

    else if(nodeType[nodenum]=="AssignmentExpression"){
        int c = adj[nodenum][0];
        preOrder(c);
        attrType[nodenum].type = attrType[nodenum].type;
        attrType[nodenum].nDims = attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="VariableInitializer"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].type = attrType[nodenum].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 2:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].type = attrType[nodenum].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                //check for array dims
            }
        }
    }

    else if(nodeType[nodenum]=="VariableDeclaratorId"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims +1;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="VariableDeclarator"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0]; 
                int c2 = adj[nodenum][2]; 
                preOrder(c1); 
                preOrder(c2); 
                
                //check if dims and type match

                attrType[nodenum].type=attrType[c1].type;
                attrType[nodenum].nDims = attrType[c1].nDims;

                // checkType(attrType[nodenum].type, attrType[nodenum].nDims, )

                break;
            }
        }
    }

    else if(nodeType[nodenum]=="VariableDeclarators"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; preOrder(c); attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
                break;
            }
            case 2:{
                int c1 = adj[nodenum][0]; 
                int c2 = adj[nodenum][2]; 
                preOrder(c1); 
                preOrder(c2); 

                //check if dims and type match for second wala

                attrType[nodenum].type=attrType[c1].type;
                attrType[nodenum].nDims = attrType[c1].nDims;
                break;
            }
        }
    }

    else if(nodeType[nodenum]=="LocalVariableDeclaration"){
        //check types;

    }  

    else if(nodeType[nodenum]=="LocalVariableDeclarationStatement"){
        //check types;

    }    

    else if(nodeType[nodenum]=="FloatingPointType" || nodeType[nodenum]=="IntegralType" || nodeType[nodenum]=="NumericType"){ 
        int c = adj[nodenum][0]; 
        preOrder(c); 
        attrType[nodenum].type=attrType[c].type;
        attrType[nodenum].nDims = attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="PrimitiveType"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; 
                preOrder(c); 
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 2:{
                int c = adj[nodenum][0]; 
                attrType[c].type = "bool";
                attrType[nodenum].type = attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;

            }
        }
    }

    else if(nodeType[nodenum]=="Type"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0]; 
                preOrder(c); 
                attrType[nodenum].type=attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 2:{
                int c = adj[nodenum][0]; 
                preOrder(c); 
                attrType[nodenum].classname=attrType[c].classname;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
        }
    }

    else if(nodeType[nodenum]=="Identifier"){
        int c = adj[nodenum][0];
        preOrder(c); 
        attrType[nodenum].type=attrType[c].type;
    }

    else if(nodeType[nodenum]=="ClassOrInterfaceType"){
        int c = adj[nodenum][0];
        preOrder(c); 
        attrType[nodenum].type=attrType[c].type;
        attrType[nodenum].classname=attrType[c].classname;
    }

    else if(nodeType[nodenum]=="SimpleName"){
        int c = adj[nodenum][0];
        preOrder(c); 
        attrType[nodenum].type=attrType[c].type;
        attrType[nodenum].classname=attrType[c].classname;
        attrType[nodenum].nameId=attrType[c].nameId;
    }

    else if(nodeType[nodenum]=="QualifiedName"){
        int c1 = adj[nodenum][0];
        int c2 = adj[nodenum][2];
        preOrder(c1); 
        preOrder(c2);

        attrType[nodenum].type=attrType[c1].type;
        attrType[nodenum].classname=attrType[c1].classname;
        attrType[nodenum].nameId=attrType[c2].nameId;
    }

    else if(nodeType[nodenum]=="Name"){
        int c = adj[nodenum][0];
        preOrder(c); 
        attrType[nodenum].type=attrType[c].type;
        attrType[nodenum].classname=attrType[c].classname;
        attrType[nodenum].funcname=attrType[c].funcname;
        attrType[nodenum].nameId=attrType[c].nameId;
    }
    
    else if(nodeType[nodenum]=="Dims"){
        switch(prodNum[nodenum]){
            case 1:{
                attrType[nodenum].nDims = 1;
            }
            case 2:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].nDims=attrType[c].nDims+1;
            }
        }
    }

    else if(nodeType[nodenum]=="DimExpr"){
        int c = adj[nodenum][1];
        preOrder(c);
        attrType[nodenum].nDims = attrType[c].nDims+1;
        attrType[nodenum].type = attrType[c].type;
    }
   
    else if(nodeType[nodenum]=="DimExprs"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][1];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].nDims = attrType[c1].nDims+1;
            }
        }
    }

    else if(nodeType[nodenum]=="ArrayCreationExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c1 = adj[nodenum][1];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].type = attrType[c1].type;
                attrType[nodenum].nDims = attrType[c2].nDims;
            }
            case 2:{
                int c1 = adj[nodenum][1];
                int c2 = adj[nodenum][2];
                int c3 = adj[nodenum][3];
                preOrder(c1);
                preOrder(c2);
                preOrder(c3);

                attrType[nodenum].type = attrType[c1].type;
                attrType[nodenum].nDims = attrType[c2].nDims + attrType[c3].nDims;

            }
            case 3:{
                int c1 = adj[nodenum][1];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].classname = attrType[c1].classname;
                attrType[nodenum].nDims = attrType[c2].nDims;
            }
            case 4:{
                int c1 = adj[nodenum][1];
                int c2 = adj[nodenum][2];
                int c3 = adj[nodenum][3];
                preOrder(c1);
                preOrder(c2);
                preOrder(c3);

                attrType[nodenum].classname = attrType[c1].classname;
                attrType[nodenum].nDims = attrType[c2].nDims + attrType[c3].nDims;
            }
        }
    }



    //function parameter type matching : to be done at MethodDeclarator
    else if(nodeType[nodenum]=="MethodDeclarator"){ 
        //match the symbol table with the type of attributes in FormalParameterList
        
        //funcScope assigned : check in  this symbol tabl
        switch(prodNum[nodenum]){
            
        }    
    }

    else if(nodeType[nodenum]=="FormalParameter"){
        int c1 = adj[nodenum][0];
        int c2 = adj[nodenum][1];
        preOrder(c1);
        preOrder(c2);

        attrType[nodenum].type = attrType[c1].type;
        attrType[nodenum].nDims = attrType[c2].nDims;
    }

    else if(nodeType[nodenum]=="FormalParameterList"){
        switch(prodNum[nodenum]){
            case 1:{
                int c1 = adj[nodenum][0];
                preOrder(c1);
                
                attrType[nodenum].funcParams.push_back(attrType[c1].type);
                attrType[nodenum].dimFuncParams.push_back(attrType[c1].nDims);
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                for(int i=0;i<attrType[c1].funcParams.size();i++){
                    attrType[nodenum].funcParams.push_back((attrType[c1].funcParams)[i]);
                }
                for(int i=0;i<attrType[c1].dimFuncParams.size();i++){
                    attrType[nodenum].dimFuncParams.push_back((attrType[c1].dimFuncParams)[i]);
                }

                attrType[nodenum].funcParams.push_back(attrType[c2].type);
                attrType[nodenum].dimFuncParams.push_back(attrType[c2].nDims);
            }
        }
    }

    else if(nodeType[nodenum]=="ForStatement"){
        //change scopes
        if (helpScope.size()==(scopeCurr.top().first-3)){
            helpScope.push_back(0);
        }

        // scopeCurr.top().first={3,4,5} 
        helpScope[scopeCurr.top().first-3]++;
        scopeParent.push(scopeCurr.top());
        scopeCurr.push(make_pair(scopeCurr.top().first+1,helpScope[scopeCurr.top().first-3]));
        
        for (auto child:adj[nodenum]){
            preOrder(child);
        }
        scopeParent.pop();
        scopeCurr.pop();
        return;
    }

    else if(nodeType[nodenum]=="ForInit"){
        
    }

    else if(nodeType[nodenum]=="Expression"){
        int c = adj[nodenum][0];
        preOrder(c);

        attrType[nodenum].type = attrType[c].type;
        attrType[nodenum].nDims = attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="ForUpdate"){
        
    }

    else if(nodeType[nodenum]=="Statement"){
        
    }

    else if(nodeType[nodenum]=="StatementWithoutTrailingSubstatement"){
        
    }
    
    else if(nodeType[nodenum]=="Block"){
        
    }

    else if(nodeType[nodenum]=="BlockStatements"){
        
    }

    else if(nodeType[nodenum]=="BlockStatement"){
        
    }

    else if(nodeType[nodenum]=="EmptyStatement"){
        
    }

    else if(nodeType[nodenum]=="ExpressionStatement"){
        
    }

    else if(nodeType[nodenum]=="SwitchStatement"){
        
    }

    else if(nodeType[nodenum]=="SwitchBlock"){
        
    }

    else if(nodeType[nodenum]=="SwitchLabels"){
        
    }

    else if(nodeType[nodenum]=="SwitchBlockStatementGroup"){
        
    }

    else if(nodeType[nodenum]=="SwitchBlockStatementGroups"){
        
    }

    else if(nodeType[nodenum]=="DoStatement"){
        
    }

    else if(nodeType[nodenum]=="BreakStatement"){
        
    }

    else if(nodeType[nodenum]=="ContinueStatement"){
        
    }

    else if(nodeType[nodenum]=="ReturnStatement"){
        
    }
    else if(nodeType[nodenum]=="SynchronizedStatement"){
        
    }
    else if(nodeType[nodenum]=="ThrowStatement"){
        
    }
    else if(nodeType[nodenum]=="TryStatement"){
        
    }

    else if(nodeType[nodenum]=="LabeledStatement"){
        
    }

    else if(nodeType[nodenum]=="IfThenStatement"){
        //change scopes
        if (helpScope.size()==(scopeCurr.top().first-3)){
            helpScope.push_back(0);
        }

        // scopeCurr.top().first={3,4,5} 
        helpScope[scopeCurr.top().first-3]++;
        scopeParent.push(scopeCurr.top());
        scopeCurr.push(make_pair(scopeCurr.top().first+1,helpScope[scopeCurr.top().first-3]));
        
        for (auto child:adj[nodenum]){
            preOrder(child);
        }

        scopeParent.pop();
        scopeCurr.pop();
        return;
    }

    else if(nodeType[nodenum]=="IfThenElseStatement"){
        int c3=adj[nodeNum][2];
        int c5=adj[nodeNum][4];
        int c7=adj[nodeNum][6];

        //change scopes for IF body
        if (scopeHelper.size()==(currScope.top().first-3)){
            scopeHelper.push_back(0);
        }
        
        // currScope.top().first={3,4,5} 
        scopeHelper[currScope.top().first-3]++;
        parentScope.push(currScope.top());
        currScope.push(make_pair(currScope.top().first+1,scopeHelper[currScope.top().first-3]));
        
        preOrderTraversal(c3);
        preOrderTraversal(c5);

        parentScope.pop();
        currScope.pop();

        //change scopes for ELSE body
        if (scopeHelper.size()==(currScope.top().first-3)){
            scopeHelper.push_back(0);
        }
        
        // currScope.top().first={3,4,5} 
        scopeHelper[currScope.top().first-3]++;
        parentScope.push(currScope.top());
        currScope.push(make_pair(currScope.top().first+1,scopeHelper[currScope.top().first-3]));
        
        preOrderTraversal(c7);

        parentScope.pop();
        currScope.pop();
    }

    else if(nodeType[nodenum]=="WhileStatement"){
        //change scopes
        if (helpScope.size()==(scopeCurr.top().first-3)){
            helpScope.push_back(0);
        }

        // scopeCurr.top().first={3,4,5} 
        helpScope[scopeCurr.top().first-3]++;
        scopeParent.push(scopeCurr.top());
        scopeCurr.push(make_pair(scopeCurr.top().first+1,helpScope[scopeCurr.top().first-3]));
        
        for (auto child:adj[nodenum]){
            preOrder(child);
        }
        scopeParent.pop();
        scopeCurr.pop();
        return;
    }

    else if(nodeType[nodenum]=="StatementExpressionList"){
        switch(prodNum[nodenum]){
            case 1:{
                int c1 = adj[nodenum][0];
            }   
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];

            }
        }
    }
    
    else if(nodeType[nodenum]=="StatementExpression"){
        switch(prodNum[nodenum]){
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:{
                int c = adj[nodenum][0];
                preOrder(c);

                attrType[nodenum].type = attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 6:{
                int c = adj[nodenum][0];
                preOrder(c);

                attrType[nodenum].type = attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
            case 7:{
                int c = adj[nodenum][0];
                preOrder(c);

                attrType[nodenum].type = attrType[c].type;
                attrType[nodenum].nDims = attrType[c].nDims;
            }
        }
    }

    else if(nodeType[nodenum]=="Assignment"){
        int c1 = adj[nodenum][0];
        int c2 = adj[nodenum][1];
        int c3 = adj[nodenum][2];

        preOrder(c1);
        preOrder(c2);
        preOrder(c3);
        //nDims and type

        if(attrType[c1].type==attrType[c3].type && attrType[c1].nDims==attrType[c3].nDims){
            attrType[nodenum].type = attrType[c3].type;
            attrType[nodenum].nDims = attrType[c3].nDims;
        }
        else{
            cout<<"Error : Tried to assign non-equivalent types"<<"\n";
        }
    }

    else if(nodeType[nodenum]=="LeftHandSide"){
        int c = adj[nodenum][0];
        preOrder(c);

        attrType[nodenum].type = attrType[c].type;
        attrType[nodenum].nDims = attrType[c].nDims;
        attrType[nodenum].nameId = attrType[c].nameId;
    }

    else if(nodeType[nodenum]=="FieldAccess"){
        switch(prodNum[nodenum]){
            case 1:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                attrType[nodenum].type = attrType[c2].type;
                attrType[nodenum].classname = attrType[c1].classname;
                attrType[nodenum].Idused.push_back(attrType[c1].nameId);

                //check if Identifier exists in scope of Primary
            }
            case 2:{
                int c = adj[nodenum][2];
                preOrder(c);

                attrType[nodenum].type = attrType[c].type;
                attrType[nodenum].Idused.push_back(attrType[c].nameId);
            }
        }
    }

    else if(nodeType[nodenum]=="ArrayAccess"){
        int c1 = adj[nodenum][0];
        int c2 = adj[nodenum][2];
        preOrder(c1);
        preOrder(c2);
        
        attrType[nodenum].nDims=attrType[c1].nDims-1;
        if(attrType[nodenum].nDims<0){
            cout<<"Error : tried to index a non array identifier"<<"\n";
        }
        attrType[nodenum].type=attrType[c1].type;
        attrType[nodenum].nameId=attrType[c1].nameId;
        
    }

    else if(nodeType[nodenum]=="AssignmentOperator"){
        //nothing here;
    }

    else if(nodeType[nodenum]=="AssignmentExpression"){
        int c = adj[nodenum][0];
        preOrder(c);

        attrType[nodenum].type = attrType[c].type;
        attrType[nodenum].nDims = attrType[c].nDims;
    }

    else if(nodeType[nodenum]=="PreIncrementExpression" || nodeType[nodenum]=="PreDecrementExpression" || nodeType[nodenum]=="PostIncrementExpression" || nodeType[nodenum]=="PostDecrementExpression"){
        int c = adj[nodenum][0];
        preOrder(c);

        if(isNumeric(attrType[c].type) && isScalar(attrType[c].nDims)){
            attrType[nodenum].type = attrType[c].type;
            attrType[nodenum].nDims = attrType[c].nDims;
        }
        else{
            cout<<"Error : tried to increment a non scalar numeric value"<<"\n";
            exit(0);
        }

    }

    else if(nodeType[nodenum]=="MethodInvocation"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);
                //check funcdef

            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);
                //check funcdef

            }
        }   
    }

    else if(nodeType[nodenum]=="ArgumentList"){
        switch(prodNum[nodenum]){
            case 1:{
                int c = adj[nodenum][0];
                preOrder(c);

                attrType[nodenum].funcParams.push_back(attrType[c].type);
                attrType[nodenum].dimFuncParams.push_back(attrType[c].nDims);
            }
            case 2:{
                int c1 = adj[nodenum][0];
                int c2 = adj[nodenum][2];
                preOrder(c1);
                preOrder(c2);

                for(int i=0;i<attrType[c1].funcParams.size();i++){
                    attrType[nodenum].funcParams.push_back((attrType[c1].funcParams)[i]);
                }

                for(int i=0;i<attrType[c1].dimFuncParams.size();i++){
                    attrType[nodenum].dimFuncParams.push_back((attrType[c1].dimFuncParams)[i]);           
                }

                attrType[nodenum].funcParams.push_back(attrType[c2].type);
                attrType[nodenum].dimFuncParams.push_back(attrType[c2].nDims);
            }   
        }
    }

    else if(nodeType[nodenum]=="ClassInstanceCreationExpression"){
        switch(prodNum[nodenum]){
            case 1:{
                int c1=adj[nodenum][1];
                preOrder(c1);

                attrType[nodenum].classname = attrType[c1].classname;
                //check argumenttypes
            }
            case 2:{
                int c1=adj[nodenum][1];
                int c2=adj[nodenum][3];
                preOrder(c1);
                preOrder(c2);
                //check argument types

            }
        }
    }

    else if(nodeType[nodenum]=="ClassType" || nodeType[nodenum]=="ClassOrInterfaceType" || nodeType[nodenum]=="InterfaceType"){
        int c1=adj[nodenum][1];
        preOrder(c1);   

        attrType[nodenum].classname = attrType[c1].classname;

    }

    else if(nodeType[nodenum]=="ForUpdate"){
        int c = adj[nodenum][0];
        preOrder(c);

        attrType[nodenum].type = attrType[c].type;
        attrType[nodenum].nDims = attrType[c].nDims;
    }

    else{
        
        for (auto child: adj[nodenum])
            preOrder(child);

        // attrType[nodenum].type = attrType[c1].type;
        // attrType[nodenum].nDims = attrType[c1].nDims;
        // attrType[nodenum].funcname = attrType[c1].funcname;
        // attrType[nodenum].classname = attrType[c1].classname;
        // attrType[nodenum].nameId = attrType[c1].nameId;
    }
}