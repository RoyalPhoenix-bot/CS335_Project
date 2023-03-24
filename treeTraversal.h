#include <bits/stdc++.h>
#include "attr.h"

using namespace std;


typedef struct localtableparams{
	string name="";
	string type="";
    vector<int> arraySize;
    int line;
	pair<int,int> scope;
	pair<int,int> parentScope;
	int offset;
	vector<localtableparams>* functionTablePointer;
	vector<string> functionParams;
	string functionReturnType;
} localTableParams ;

map<int, pair<string,vector<int>>> arrayInfo;
map<int,int> trueLabel;
map<int,int> falseLabel;
map<int,int> nextLabel;
int countNodes=0;
int tempNum=0;
map<string,int> typeSize;
int labelNum=0;
vector<string> nodeType;
map<int, vector<int>> adj;
map<int,int> prodNum;

vector<attr> attrSymTab;
vector<attr> attr3AC;

// pair<string,vector<int>> getArrayInfo(string _arrayName){
//     // returns the type of array and the size vector 
// }

string getLabel(int nodeNum, int type){
    switch(type){
        case 1: //True label
        {
            if(trueLabel.find(nodeNum)!=trueLabel.end()){
                string r = "L" + to_string(trueLabel[nodeNum]);
                return r;
            }else{
                labelNum++;
                string r = "L" + to_string(labelNum);
                trueLabel[nodeNum]=labelNum;
                return r;
            }
        }
        break;
        case 2: //False label
        {
            if(falseLabel.find(nodeNum)!=falseLabel.end()){
                string r = "L" + to_string(falseLabel[nodeNum]);
                return r;
            }else{
                labelNum++;
                string r = "L" + to_string(labelNum);
                falseLabel[nodeNum]=labelNum;
                return r;
            }
        }
        break;
        case 3: //Next label
        {
            if(nextLabel.find(nodeNum)!=nextLabel.end()){
                string r = "L" + to_string(nextLabel[nodeNum]);
                return r;
            }else{
                labelNum++;
                string r = "L" + to_string(labelNum);
                nextLabel[nodeNum]=labelNum;
                return r;
            }
        }
        break;
        default:
        {
            labelNum++;
            string r = "L" + to_string(labelNum);
            return r;
        }
    }
}

void pushLabelUp(int par,int chld){
    if(trueLabel.find(par)!=trueLabel.end() && trueLabel.find(chld)!=trueLabel.end())trueLabel[par]=trueLabel[chld];
    if(falseLabel.find(par)!=falseLabel.end() && falseLabel.find(chld)!=falseLabel.end())falseLabel[par]=falseLabel[chld];
    if(nextLabel.find(par)!=nextLabel.end() && nextLabel.find(chld)!=nextLabel.end())nextLabel[par]=nextLabel[chld];
    return;
}

int getLabelNumber(string label){
    string r="";
    for(int i=1;i<label.size();i++)r+=label[i];
    return stoi(r);
}

void initializeAttributeVectors(){
    for(int i=0;i<nodeType.size();i++){
        attr temp;
        attrSymTab.push_back(temp);
        attr3AC.push_back(temp);
    }
    typeSize["int"]=4;
    typeSize["long"]=8;
    typeSize["float"]=4;
    typeSize["bool"]=1;
    typeSize["double"]=8;
    typeSize["char"]=2;
    typeSize["short"]=2;
    return;
}



void execWhileStatementNoShortIf(int nodeNum){
    string beg = getLabel(-1,0);
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    cout << "whilestatementnoshortif " << attr3AC[c3].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    cout << "B true: " << c3true << endl;
    nextLabel[c5]= getLabelNumber(beg);
    string temp = beg + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
    temp = c3true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
    temp = "goto " + beg;
    attr3AC[nodeNum].threeAC.push_back(temp);

    falseLabel[c3] = nextLabel[nodeNum];

    return;
}

void execVariableInitializers(int nodeNum){
    return;
}

void execVariableDeclarators(int nodeNum){
    return;
}

void execVariableDeclaratorId(int nodeNum){
    return;
}

void execVariableDeclarator(int nodeNum){
    return;
}

void execTypeImportOnDemandDeclaration(int nodeNum){
    return;
}

void execType(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    return;
}

void execTryStatement(int nodeNum){
    return;
}

void execThrowStatement(int nodeNum){
    return;
}

void execThrows(int nodeNum){
    return;
}

void execSynchronizedStatement(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    attr3AC[nodeNum] = attr3AC[c3] + attr3AC[c5];
    return;
}

void execSwitchStatement(int nodeNum){
    cout << "switch statemnt karna he" << endl;
    return;
}

void execSwitchLabels(int nodeNum){
    return;
}

void execSwitchLabel(int nodeNum){
    return;
}

void execSwitchBlockStatementGroups(int nodeNum){
    return;
}

void execSwitchBlockStatementGroup(int nodeNum){

    return;
}

void execSwitchBlock(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
        break;
        case 2:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            pushLabelUp(nodeNum,c2);
        }
        break;
        case 3:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            pushLabelUp(nodeNum,c2);
        }
        break;
        case 4:{
            int c2 = adj[nodeNum][1];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c2] + attr3AC[c3];
        }
        break;
    }
    
    return;
}

void execSuper(int nodeNum){
    return;
}

void execStringLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[c];
    return;
}

void execStaticInitializer(int nodeNum){
    int c2 = adj[nodeNum][1];
    attr3AC[nodeNum] = attr3AC[c2];
    pushLabelUp(nodeNum,c2);
    return;
}

void execStatementExpressionList(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
        }
        break;
    }
    return;
}

void execSingleTypeImportDeclaration(int nodeNum){
    return;
}

void execReturnStatement(int nodeNum){
    switch (prodNum[nodeNum]){
        case 1:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            string temp = "RETURN " + attr3AC[c2].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c2);
        }
        break;
        case 2:{
            string temp = "RETURN";
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
    }
}

void execReferenceType(int nodeNum){
    //Types not required to write 3AC
    return;
}

void execQualifiedName(int nodeNum){
    //to be tried out
    return;
}

void execPrimitiveType(int nodeNum){
    //Types not required to write 3AC
    return;
}

void execPreIncrementExpression(int nodeNum){
    int c = adj[nodeNum][1~];
    tempNum++;
    attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
    string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + " + 1";
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = attr3AC[c].addrName + " = " + attr3AC[nodeNum].addrName;
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum]+ attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execPreDecrementExpression(int nodeNum){
    int c = adj[nodeNum][1];
    tempNum++;
    attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
    string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + " - 1";
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = attr3AC[c].addrName + " = " + attr3AC[nodeNum].addrName;
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execPostIncrementExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    tempNum++;
    attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
    string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + " + 1";
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = attr3AC[c].addrName + " = " + attr3AC[nodeNum].addrName;
    attr3AC[nodeNum].threeAC.push_back(temp);
    pushLabelUp(nodeNum,c);
    return;
}

void execPostDecrementExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    tempNum++;
    attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
    string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + " - 1";
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = attr3AC[c].addrName + " = " + attr3AC[nodeNum].addrName;
    attr3AC[nodeNum].threeAC.push_back(temp);
    pushLabelUp(nodeNum,c);
    return;
}

void execPackageDeclaration(int nodeNum){
    return;
}

void execNumericType(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
}

void execNullLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[c];
    return;
}

void execModifiers(int nodeNum){
    return;
}

void execModifier(int nodeNum){
    return;
}

void execMethodHeader(int nodeNum){
    return;
}

void execMethodDeclarator(int nodeNum){
    return;
}

void execLocalVariableDeclarationStatement(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execLocalVariableDeclaration(int nodeNum){
    int c = adj[nodeNum][0];
    int c2 = adj[nodeNum][1];
    attr3AC[nodeNum] = attr3AC[c2];
    attr3AC[nodeNum].type = attr3AC[c].type;
    return;
}

void execLiteral(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execLabeledStatementNoShortIf(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c = adj[nodeNum][0];
    string ctrue = getLabel(c,1);
    string temp = ctrue + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
    pushLabelUp(nodeNum,c3);
    return;
}

void execLabeledStatement(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c = adj[nodeNum][0];
    string ctrue = getLabel(c,1);
    string temp = ctrue + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
    pushLabelUp(nodeNum,c3);
    return;
}

void execInterfaceTypeList(int nodeNum){
    return;
}

void execInterfaceType(int nodeNum){
    return;
}

void execInterfaces(int nodeNum){
    int c2 = adj[nodeNum][1];
    attr3AC[nodeNum] = attr3AC[c2];
    pushLabelUp(nodeNum,c2);
    return;
}

void execInterfaceMemberDeclarations(int nodeNum){
    return;
}

void execInterfaceMemberDeclaration(int nodeNum){
    return;
}

void execInterfaceDeclaration(int nodeNum){
    return;
}

void execInterfaceBody(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:
        break;
    }
    return;
}

void execIntegerLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[c];
    return;
}

void execImportDeclaration(int nodeNum){
    return;
}

void execImportDeclarations(int nodeNum){
    return;
}

void execIfThenElseStatementNoShortIf(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    int c7 = adj[nodeNum][6];
    cout << "ifthen else " << attr3AC[c3].threeAC.size() << " " << attr3AC[c5].threeAC.size() << " " << attr3AC[c7].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    string c3false = getLabel(c3,2);
    attr3AC[nodeNum] = attr3AC[c3];
    // cout << " vasf " << attr3AC[c3].threeAC[0] << endl;
    string temp = c3true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
    string pnext = getLabel(nodeNum,3);
    temp = "goto " + pnext;
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = c3false + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];

    return;
}

void execForUpdate(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execForStatementNoShortIf(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            int c7 = adj[nodeNum][6];
            int c9 = adj[nodeNum][8];
            string beg = getLabel(-1,0);
            nextLabel[c9]= getLabelNumber(beg);
            string c5true = getLabel(c5,1);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = c5true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c9];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c5] = nextLabel[nodeNum];
        }
        break;
        case 2:{
            int c4 = adj[nodeNum][3];
            int c6 = adj[nodeNum][5];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c4];
            temp = c4true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c4] = nextLabel[nodeNum];
        }
        break;
        case 3:{
            int c3 = adj[nodeNum][2];
            int c6 = adj[nodeNum][6];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 4:{
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            string c5true = getLabel(c5,1);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = c5true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c5] = nextLabel[nodeNum];
        }
        break;
        case 5:{
            int c5 = adj[nodeNum][4];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 6:{
            int c4 = adj[nodeNum][3];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c4];
            temp = c4true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c4] = nextLabel[nodeNum];
        }
        break;
        case 7:{
            int c3 = adj[nodeNum][2];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 8:{
            int c6 = adj[nodeNum][5];
            string beg = getLabel(-1,0);
            nextLabel[c6]= getLabelNumber(beg);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp); 
            // pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execFormalParameter(int nodeNum){
    return;
}

void execFormalParameterList(int nodeNum){
    return;
}

void execForInit(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
}

void execFloatingPointType(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum].type = nodeType[c]; 
        }
        break;
        case 2:{
            attr3AC[nodeNum].type= "double";
        }
        break;

    }
    return;
}

void execFinally(int nodeNum){
    int c = adj[nodeNum][1];
    attr3AC[nodeNum] = attr3AC[c];
    return;
}

void execFloatingPointLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[c];
    return;
}

void execFieldDeclaration(int nodeNum){
    //declaration, not required for 3ac
    return;
}

void execFieldAccess(int nodeNum){
    
    return;
}

void execExtendsInterfaces(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            // pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execExplicitConstructorInvocation(int nodeNum){
    
    return;
}

void execDoublePointLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[c];
    return;
}

void execDoStatement(int nodeNum){

}

void execContinueStatement(int nodeNum){

}

void execConstructorDeclarator(int nodeNum){
    //Declaration: not necessarily required for 3AC
    return;
}

void execConstructorDeclaration(int nodeNum){
    //Just a declaration
    return;
}

void execConstructorBody(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c2 = adj[nodeNum][1];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c2]+attr3AC[c3];
        }
        break;
        case 2:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
        case 3:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
        case 4:
        break;
    }
    return;
}

void execConstantExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execConstantDeclaration(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
}

void execClassTypeList(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
        }
        break;
    }
    return;
}

void execClassOrInterfaceType(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execClassInstanceCreationExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            pushLabelUp(nodeNum,c2);
        }
        break;
        case 2:{
            int c2 = adj[nodeNum][1];
            int c4 = adj[nodeNum][3];
            attr3AC[nodeNum] = attr3AC[c2] + attr3AC[c4];
        }
        break;
    }
    return;
}

void execCharacterLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].addrName = nodeType[nodeNum];
    return;
}

void execCatches(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
        }
        break;
    }
    return;
}

void execCatchClause(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][3];
    attr3AC[nodeNum] = attr3AC[c3];
    string c3true = getLabel(c3,1);
    string c3false = getLabel(c3,2);
    string temp = "if " + attr3AC[c3].addrName + " goto " + c3true;
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = "goto " + c3false;
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = c3true + ": ";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
    temp = c3false+":";
    return;
}

void execBreakStatement(int nodeNum){

    return;
}

void execArrayType(int nodeNum){
    return;
}

void execBooleanLiteral(int nodeNum){
    attr3AC[nodeNum].addrName = nodeType[nodeNum];
    string ptrue = getLabel(nodeNum,1);
    string pfalse = getLabel(nodeNum,2);
    if(nodeType[nodeNum]=="true"){
        string temp = "goto " + ptrue;
        attr3AC[nodeNum].threeAC.push_back(temp);
    }else{
        string temp = "goto " + pfalse;
        attr3AC[nodeNum].threeAC.push_back(temp);
    }
    return;
}

void execAssignmentOperator(int nodeNum){
    return;
}

void execArrayInitializer(int nodeNum){
    //This kindof declaration 
    return;
}

void execAbstractMethodDeclaration(int nodeNum){
    int c = adj[nodeNum][2];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execCompilationUnit(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
        }
        break;
    }
    return;
}

void execTypeDeclarations(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
    }
    return;
}

void execTypeDeclaration(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:
        break;
    }
    return;
}

void execClassDeclaration(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][5];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][4];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][3];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][3];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][4];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][4];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 8:{
            int c = adj[nodeNum][3];
            attr3AC[nodeNum] = attr3AC[c];
            // attr3AC[nodeNum].nodeno = nodeNum;
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execClassBody(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:
        break;
    }
    return;
}

void execClassBodyDeclarations(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
    }
    return;
}

void execClassBodyDeclaration(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execClassMemberDeclaration(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execMethodDeclaration(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
    }
    return;
}

void execMethodBody(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:
        break;
    }
    return;
}

void execBlock(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
        break;
        case 2:{
            int c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execBlockStatements(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
    }
    return;
}

void execBlockStatement(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execStatement(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            cout << "statement " << attr3AC[nodeNum].threeAC.size() << endl;
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execStatementNoShortIf(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            cout << "statementNoshort if " << attr3AC[nodeNum].threeAC.size() << endl;;
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            string cNext = getLabel(c,3);
            string temp = cNext +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execIfThenElseStatement(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    int c7 = adj[nodeNum][6];
    cout << "ifthen else " << attr3AC[c3].threeAC.size() << " " << attr3AC[c5].threeAC.size() << " " << attr3AC[c7].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    string c3false = getLabel(c3,2);
    attr3AC[nodeNum] = attr3AC[c3];
    // cout << " vasf " << attr3AC[c3].threeAC[0] << endl;
    string temp = c3true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
    string pnext = getLabel(nodeNum,3);
    temp = "goto " + pnext;
    attr3AC[nodeNum].threeAC.push_back(temp);
    temp = c3false + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];

    return;
}

void execIfThenStatement(int nodeNum){
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    string c3true = getLabel(c3,1);
    attr3AC[nodeNum] = attr3AC[c3];
    string temp = c3true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];

    return;
}

void execForStatement(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            int c7 = adj[nodeNum][6];
            int c9 = adj[nodeNum][8];
            string beg = getLabel(-1,0);
            nextLabel[c9]= getLabelNumber(beg);
            string c5true = getLabel(c5,1);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = c5true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c9];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c5] = nextLabel[nodeNum];
        }
        break;
        case 2:{
            int c4 = adj[nodeNum][3];
            int c6 = adj[nodeNum][5];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c4];
            temp = c4true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c4] = nextLabel[nodeNum];
        }
        break;
        case 3:{
            int c3 = adj[nodeNum][2];
            int c6 = adj[nodeNum][6];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 4:{
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            int c8 = adj[nodeNum][7];
            string beg = getLabel(-1,0);
            nextLabel[c8]= getLabelNumber(beg);
            string c5true = getLabel(c5,1);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = c5true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c8];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c5] = nextLabel[nodeNum];
        }
        break;
        case 5:{
            int c5 = adj[nodeNum][4];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 6:{
            int c4 = adj[nodeNum][3];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c4];
            temp = c4true + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);

            falseLabel[c4] = nextLabel[nodeNum];
        }
        break;
        case 7:{
            int c3 = adj[nodeNum][2];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            nextLabel[c7]= getLabelNumber(beg);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 8:{
            int c6 = adj[nodeNum][5];
            string beg = getLabel(-1,0);
            nextLabel[c6]= getLabelNumber(beg);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c6];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp); 
            // pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execWhileStatement(int nodeNum){
    string beg = getLabel(-1,0);
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    cout << "while " << attr3AC[c3].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    cout << "B true: " << c3true << endl;
    nextLabel[c5]= getLabelNumber(beg);
    string temp = beg + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
    temp = c3true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
    temp = "goto " + beg;
    attr3AC[nodeNum].threeAC.push_back(temp);

    falseLabel[c3] = nextLabel[nodeNum];

    return;
}

void execStatementWithoutTrailingSubstatement(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            cout << "swts " << attr3AC[nodeNum].threeAC.size() << endl;
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 8:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 9:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 10:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 11:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execExpressionStatement(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execStatementExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            cout << "statementexpression " << attr3AC[nodeNum].threeAC.size() << endl;;
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execPrimary(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execArrayCreationExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            // int c2 = adj[nodeNum][1];
            // int c3 = adj[nodeNum][2];
            // attr3AC[nodeNum] = attr3AC[c3];
            // attr3AC[nodeNum].type = attr3AC[c2].type;
            // tempNum++;
            // attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            // string temp = attr3AC[nodeNum].addrName + " = NEW " + attr3AC[c2].type;
            // for(int i=0;i<attr3AC[nodeNum].arrDims.size();i++){
            //     temp = temp + to_string((attr3AC[nodeNum].arrDims)[i]) + ", ";
            // }
            // attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 2:{
            // int c2 = adj[nodeNum][1];
            // int c3 = adj[nodeNum][2];
            // int c4 = adj[nodeNum][3];
            // attr3AC[nodeNum] = attr3AC[c3] + attr3AC[c4];
            // attr3AC[nodeNum].type = attr3AC[c2].type;
            // tempNum++;
            // attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            // string temp = attr3AC[nodeNum].addrName + " = NEW " + attr3AC[c2].type;
            // for(int i=0;i<attr3AC[nodeNum].arrDims.size();i++){
            //     temp = temp + to_string((attr3AC[nodeNum].arrDims)[i]) + ", ";
            // }
            // attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 3:
        break;
        case 4:
        break;
    }
    return;
}

void execArrayAccess(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            auto mdata = arrayInfo[attr3AC[c].nodeno];// lowestnode
            string t = mdata.first;
            vector<int> d = mdata.second;
            int mult=typeSize[t];
            for(int i=0;i<d.size();i++){
                if(i)mult*=d[i];
                attr3AC[nodeNum].arrDims.push_back(d[i]);
            }
            attr3AC[nodeNum] = attr3AC[c3];
            attr3AC[nodeNum].type = t;
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            attr3AC[nodeNum].dimsDone++;
            attr3AC[nodeNum].nameAtNode = attr3AC[c].nameAtNode;
            string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c3].addrName + " * " + to_string(mult);
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 2:{
            int c =adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c3];
            int mult = typeSize[attr3AC[c].type];
            for(int i=attr3AC[c].dimsDone+1;i<attr3AC[c].arrDims.size();i++){
                mult*=(attr3AC[c].arrDims)[i];
            }
            tempNum++;
            string temp = to_string(tempNum) + " = " + attr3AC[c3].addrName + " * " + to_string(mult);
            attr3AC[nodeNum].threeAC.push_back(temp);
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + to_string(tempNum-1) ;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
    }
    return;
}

void execDimExprs(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
        }   
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
        }
        break;
    }
    return;
}

void execDimExpr(int nodeNum){
    int c = adj[nodeNum][1];
    attr3AC[nodeNum]=attr3AC[c];
    attr3AC[nodeNum].arrDims.push_back(stoi(attr3AC[c].nameAtNode));
    return;
}

void execDims(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            attr3AC[nodeNum].arrDims.push_back(-1);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum].arrDims.push_back(-1);
        }
        break;
    }
    return;
}

void execPrimaryNoNewArray(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            attr3AC[nodeNum].nameAtNode = "THIS";
        }
        break;
        case 3:{
            int c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 4:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 5:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 6:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].nameAtNode + " [ " + attr3AC[c].addrName + " ] ";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 7:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execMethodInvocation(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = attr3AC[nodeNum].addrName + " = " "call " + attr3AC[c].addrName + ", 0";
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
            break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            for(int fcall=0; fcall<(attr3AC[c3].params).size();fcall++){
                string temp = "param " + (attr3AC[c3].params)[fcall];
                attr3AC[nodeNum].threeAC.push_back(temp);
            }
            string temp = attr3AC[nodeNum].addrName + " = " "call " + attr3AC[c].addrName + ", " + to_string(attr3AC[c3].params.size());
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c);
        }
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
            pushLabelUp(nodeNum,c);
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
    }
}

void execArgumentList(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum].params.push_back(attr3AC[c].addrName);
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum].params.push_back(attr3AC[c3].addrName);
            pushLabelUp(nodeNum,c);
        }

        break;
    }
    return;
}

void execExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execAssignment(int nodeNum){
    int c = adj[nodeNum][0];
    int c2 = adj[nodeNum][1];
    int c3 = adj[nodeNum][2];
    switch(prodNum[c2]){
        case 1:{

            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " = " + attr3AC[c3].addrName;
            cout << "assignment " << temp << endl;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 2:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " *= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 3:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " /= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 4:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " += " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 5:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " -= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 6:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " <<= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 7:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " >>= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 8:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " >>>= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 9:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " &= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 10:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " ^= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 11:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " |= " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
    }

    return;
}

void execAssignmentExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
    }
}

void execConditionalExpression(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            int c5 = adj[nodeNum][4];
            string ltrue = getLabel(c,1);
            string lfalse = getLabel(c,2);
            string nextStmt = getLabel(nodeNum,3);

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
            pushLabelUp(nodeNum,c);
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            string cFalse = getLabel(c,2);
            string temp = cFalse + ":";
            (attr3AC[nodeNum].threeAC).push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];

            trueLabel[c] = trueLabel[nodeNum];
            trueLabel[c3] = trueLabel[nodeNum];
            falseLabel[c3] = falseLabel[nodeNum];
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
            pushLabelUp(nodeNum,c);
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            string cTrue = getLabel(c,1);
            string temp = cTrue + ":";
            (attr3AC[nodeNum].threeAC).push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];

            falseLabel[c] = falseLabel[nodeNum];
            trueLabel[c3]= trueLabel[nodeNum];
            falseLabel[c3] = trueLabel[nodeNum];
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
            break;
        case 2:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string pTrue = getLabel(nodeNum,1);
            string pFalse = getLabel(nodeNum,2);
            cout << nodeNum << " " << nodeType[nodeNum] << " " << pTrue << " " << pFalse << endl;
            string temp = "if " + attr3AC[c].addrName+ " < " + attr3AC[c3].addrName + " goto " + pTrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + pFalse;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string pTrue = getLabel(nodeNum,1);
            string pFalse = getLabel(nodeNum,2);
            string temp = "if " + attr3AC[c].addrName+ " > " + attr3AC[c3].addrName + " goto " + pTrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + pFalse;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:{
            {
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string pTrue = getLabel(nodeNum,1);
            string pFalse = getLabel(nodeNum,2);
            string temp = "if " + attr3AC[c].addrName+ " <= " + attr3AC[c3].addrName + " goto " + pTrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + pFalse;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
        }
            break;
        case 5:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string pTrue = getLabel(nodeNum,1);
            string pFalse = getLabel(nodeNum,2);
            string temp = "if " + attr3AC[c].addrName+ " >= " + attr3AC[c3].addrName + " goto " + pTrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + pFalse;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 6:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string pTrue = getLabel(nodeNum,1);
            string pFalse = getLabel(nodeNum,2);
            string temp = "if " + attr3AC[c].addrName+ " < " + attr3AC[c3].addrName + " instanceof " + pTrue;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            temp = "goto " + pFalse;
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
        }
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
        case 3:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
            break;
        case 2:{
            c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =~ " + attr3AC[c].addrName;
            (attr3AC[nodeNum].threeAC).push_back(temp);
            pushLabelUp(nodeNum,c);
        }
            break;
        case 3:{
            c = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c];
            trueLabel[c] = falseLabel[nodeNum];
            falseLabel[c] = trueLabel[nodeNum];
            pushLabelUp(nodeNum,c);
        }
            break;
        case 4:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
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
            pushLabelUp(nodeNum,c);
            break;
        case 2:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
        case 3:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
        case 4:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
    }
}

void execLeftHandSide(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];//Get child node number
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            cout << "LeftHandSide " << attr3AC[nodeNum].addrName << " " << attr3AC[c].addrName << endl;

        }
        break;
        case 2:{
            int c = adj[nodeNum][0];//Get child node number
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
        case 3:{
            int c = adj[nodeNum][0];//Get child node number
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execName(int nodeNum){
    int c;
    switch(prodNum[nodeNum]){
        case 1:
            c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            break;
    }
    return;
}

void execSimpleName(int nodeNum){
    int c = adj[nodeNum][0];//Get child node number
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    cout << "Simplename " << attr3AC[nodeNum].addrName << " " << attr3AC[c].addrName << endl;
    return;
}

void execIdentifier(int nodeNum){

    int c = adj[nodeNum][0];//Get child node number
    attr3AC[nodeNum].addrName = nodeType[c];//Add addrname of identifier 
    attr3AC[nodeNum].nodeno = c;
    return;
}

void labelStatement(int nodeNum){
    getLabel(adj[nodeNum][0],3);
    return;
}

void labelStatementNoShortIf(int nodeNum){
    getLabel(adj[nodeNum][0],3);
    return;
}

void labelConditionalAndExpression(int nodeNum){
    getLabel(nodeNum,1);
    getLabel(nodeNum,2);
    return;
}

void labelConditionalOrExpression(int nodeNum){
    getLabel(nodeNum,1);
    getLabel(nodeNum,2);
    return;
}

void labelIfElse(int nodeNum){
    getLabel(adj[nodeNum][2],1);
    getLabel(adj[nodeNum][2],2);
    nextLabel[adj[nodeNum][4]] = nextLabel[nodeNum];
    nextLabel[adj[nodeNum][6]] = nextLabel[nodeNum];
    return;
}

void labelIf(int nodeNum){
    getLabel(adj[nodeNum][2],1);
    falseLabel[adj[nodeNum][2]]=nextLabel[nodeNum];
    nextLabel[adj[nodeNum][4]] = nextLabel[nodeNum];
    return;
}

void labelWhile(int nodeNum){
    getLabel(adj[nodeNum][2],1);
    falseLabel[adj[nodeNum][2]]=nextLabel[nodeNum];
    return;
}

void generateLabels(int nodeNum){
    string s = nodeType[nodeNum];
    if("Statement"==s && (prodNum[nodeNum]==3 || prodNum[nodeNum]==4 || prodNum[nodeNum]==5 || prodNum[nodeNum]==6)){
        labelStatement(nodeNum);
    }else if("StatementNoShortIf"==s && (prodNum[nodeNum]==3 || prodNum[nodeNum]==4 || prodNum[nodeNum]==5)){
        labelStatementNoShortIf(nodeNum);
    }else if("ConditionalAndExpression"==s){
        labelConditionalAndExpression(nodeNum);
    }else if("ConditionalOrExpression"==s){
        labelConditionalOrExpression(nodeNum);
    }else if("IfThenElseStatement"==s || "IfThenElseStatementNoShortIf"==s){
        labelIfElse(nodeNum);
    }else if("IfThenStatement" == s){
        labelIf(nodeNum);
    }else if("WhileStatement"==s || "WhileStatementNoShortIf"==s || "ForStatement"==s || "ForStatementNoShortIf"==s){
        labelWhile(nodeNum);
    }
    // if(nodeNum==127)cout << "ihfadf " << nodeType[nodeNum] << endl;
    if(adj[nodeNum].size()==1){
        // if(nodeNum==127)cout << "here " << trueLabel[nodeNum] << endl;
        // if(adj[nodeNum][0]==118)cout<< " asdfsadfasdf " << adj[nodeNum][0] << " " << trueLabel[nodeNum] << endl;
        if(trueLabel.find(nodeNum)!=trueLabel.end())trueLabel[adj[nodeNum][0]] = trueLabel[nodeNum];
        if(falseLabel.find(nodeNum)!=falseLabel.end())falseLabel[adj[nodeNum][0]] = falseLabel[nodeNum];
        if(nextLabel.find(nodeNum)!=nextLabel.end())nextLabel[adj[nodeNum][0]] = nextLabel[nodeNum];
    }
    for(int i=0;i<adj[nodeNum].size();i++){
        generateLabels(adj[nodeNum][i]);
    }
    return;
}

void postOrderTraversal3AC(int nodeNum){

    for(int i=0;i<adj[nodeNum].size();i++){
        postOrderTraversal3AC(adj[nodeNum][i]);
    }

    string s = nodeType[nodeNum];
    if(adj[nodeNum].size()==0)return;
    if("CompilationUnit" == s){
        execCompilationUnit(nodeNum);
    }else if("TypeDeclarations"==s){
        execTypeDeclarations(nodeNum);
    }else if("TypeDeclaration"==s){
        execTypeDeclaration(nodeNum);
    }else if("ClassDeclaration" == s){
        execClassDeclaration(nodeNum);
    }else if("ClassBody" == s){
        execClassBody(nodeNum);
    }else if("ClassBodyDeclarations" == s){
        execClassBodyDeclarations(nodeNum);
    }else if("ClassBodyDeclaration" == s){
        execClassBodyDeclaration(nodeNum);
    }else if("ClassMemberDeclaration" == s){
        execClassMemberDeclaration(nodeNum);
    }else if("MethodDeclaration" == s){
        execMethodDeclaration(nodeNum);
    }else if("MethodBody" == s){
        execMethodBody(nodeNum);
    }else if("Block" == s){
        execBlock(nodeNum);
    }else if("BlockStatements" == s){
        execBlockStatements(nodeNum);
    }else if("BlockStatement" == s){
        execBlockStatement(nodeNum);
    }else if("Statement" == s){
        execStatement(nodeNum);
    }else if("IfThenElseStatement" == s){
        execIfThenElseStatement(nodeNum);
    }else if("IfThenStatement" == s){
        execIfThenStatement(nodeNum);
    }else if("StatementWithoutTrailingSubstatement" == s){
        execStatementWithoutTrailingSubstatement(nodeNum);
    }else if("ExpressionStatement" == s){
        execExpressionStatement(nodeNum);
    }else if("StatementExpression" == s){
        execStatementExpression(nodeNum);
    }else if("Primary" == s){
        execPrimary(nodeNum);
    }else if("ArrayAccess" == s){
        execArrayAccess(nodeNum);
    }else if("DimExprs" == s){
        execDimExprs(nodeNum);
    }else if("DimExpr" == s){
        execDimExpr(nodeNum);
    }else if("Dims" == s){
        execDims(nodeNum);
    }else if("PrimaryNoNewArray" == s){
        execPrimaryNoNewArray(nodeNum);
    }else if("MethodInvocation" == s){
        execMethodInvocation(nodeNum);
    }else if("VariableInitializer" == s){
        execVariableInitializer(nodeNum);
    }else if("ArgumentList" == s){
        execArgumentList(nodeNum);
    }else if("Expression" == s){
        execExpression(nodeNum);
    }else if("Assignment" == s){
        execAssignment(nodeNum);
    }else if("AssignmentExpression" == s){
        execAssignmentExpression(nodeNum);
    }else if("ConditionalExpression" == s){
        execConditionalExpression(nodeNum);
    }else if("ConditionalOrExpression" == s){
        execConditionalOrExpression(nodeNum);
    }else if("ConditionalAndExpression" == s){
        execConditionalAndExpression(nodeNum);
    }else if("InclusiveOrExpression" == s){
        execInclusiveOrExpression(nodeNum);
    }else if("ExclusiveOrExpression" == s){
        execExclusiveOrExpression(nodeNum);
    }else if("AndExpression" == s){
        execAndExpression(nodeNum);
    }else if("EqualityExpression" == s){
        execEqualityExpression(nodeNum);
    }else if("RelationalExpression" == s){
        execRelationalExpression(nodeNum);
    }else if("ShiftExpression" == s){
        execShiftExpression(nodeNum);
    }else if("AdditiveExpression" == s){
        execAdditiveExpression(nodeNum);
    }else if("MultiplicativeExpression" == s){
        execMultiplicativeExpression(nodeNum);
    }else if("UnaryExpression" == s){
        execUnaryExpression(nodeNum);
    }else if("PostfixExpression" == s){
        execPostfixExpression(nodeNum);
    }else if("Name" == s){
        execName(nodeNum);
    }else if("SimpleName" == s){
        execSimpleName(nodeNum);
    }else if("Identifier" == s){
        execIdentifier(nodeNum);
    }else if("LeftHandSide" == s){
        execLeftHandSide(nodeNum);
    }else if("UnaryExpressionNotPlusMinus" == s){
        execUnaryExpressionNotPlusMinus(nodeNum);
    }else if("StatementNoShortIf" == s){
        execStatementNoShortIf(nodeNum);
    }else if("WhileStatement" == s){
        execWhileStatement(nodeNum);
    }else if("ForStatement" == s){
        execForStatement(nodeNum);
    }else if("AbstractMethodDeclaration" == s){
        execAbstractMethodDeclaration(nodeNum);
    }else if("ArrayCreationExpression" == s){
        execArrayCreationExpression(nodeNum);
    }else if("ArrayInitializer" == s){
        execArrayInitializer(nodeNum);
    }else if("ArrayType" == s){
        execArrayType(nodeNum);
    }else if("AssignmentOperator" == s){
        execAssignmentOperator(nodeNum);
    }else if("BooleanLiteral" == s){
        execBooleanLiteral(nodeNum);
    }else if("BreakStatement" == s){
        execBreakStatement(nodeNum);
    }else if("CatchClause" == s){
        execCatchClause(nodeNum);
    }else if("Catches"== s){
        execCatches(nodeNum);
    }else if("CharacterLiteral" == s){
        execCharacterLiteral(nodeNum);
    }else if("ClassInstanceCreationExpression" == s){
        execClassInstanceCreationExpression(nodeNum);
    }else if("ClassOrInterfaceType" == s){
        execClassOrInterfaceType(nodeNum);
    }else if("ClassTypeList"==s){
        execClassTypeList(nodeNum);
    }else if("ConstantDeclaration" == s){
        execConstantDeclaration(nodeNum);
    }else if("ConstantExpression" == s){
        execConstantExpression(nodeNum);
    }else if("ConstructorBody" == s){
        execConstructorBody(nodeNum);
    }else if("ConstructorDeclaration" == s){
        execConstructorDeclaration(nodeNum);
    }else if("ConstructorDeclarator" == s){
        execConstructorDeclarator(nodeNum);
    }else if("ContinueStatement" == s){
        execContinueStatement(nodeNum);
    }else if("DoStatement" == s){
        execDoStatement(nodeNum);
    }else if("DoublePointLiteral" == s){
        execDoublePointLiteral(nodeNum);
    }else if("EmptyStatement" == s){

    }else if("ExplicitConstructorInvocation"==s){
        execExplicitConstructorInvocation(nodeNum);
    }else if("ExtendsInterfaces" == s){
        execExtendsInterfaces(nodeNum);
    }else if("FieldAccess"==s){
        execFieldAccess(nodeNum);
    }else if("FieldDeclaration"==s){
        execFieldDeclaration(nodeNum);
    }else if("Finally"==s){
        execFinally(nodeNum);
    }else if("FloatingPointLiteral" == s){
        execFloatingPointLiteral(nodeNum);
    }else if("FloatingPointType" == s){
        execFloatingPointType(nodeNum);
    }else if("ForInit"==s){
        execForInit(nodeNum);
    }else if("FormalParameterList"== s){
        execFormalParameterList(nodeNum);
    }else if("FormalParameter" == s){
        execFormalParameter(nodeNum);
    }else if("ForStatementNoShortIf"==s){
        execForStatementNoShortIf(nodeNum);
    }else if("ForUpdate"==s){
        execForUpdate(nodeNum);
    }else if("IfThenElseStatementNoShortIf" == s){
        execIfThenElseStatementNoShortIf(nodeNum);
    }else if("ImportDeclarations"==s){
        execImportDeclarations(nodeNum);
    }else if("ImportDeclaration"==s){
        execImportDeclaration(nodeNum);
    }else if("IntegerLiteral" == s){
        execIntegerLiteral(nodeNum);
    }else if("InterfaceBody"==s){
        execInterfaceBody(nodeNum);
    }else if("InterfaceDeclaration" == s){
        execInterfaceDeclaration(nodeNum);
    }else if("InterfaceMemberDeclaration"==s){
        execInterfaceMemberDeclaration(nodeNum);
    }else if("InterfaceMemberDeclarations"==s){
        execInterfaceMemberDeclarations(nodeNum);
    }else if("Interfaces"==s){
        execInterfaces(nodeNum);
    }else if("InterfaceType"==s){
        execInterfaceType(nodeNum);
    }else if("InterfaceTypeList"==s){
        execInterfaceTypeList(nodeNum);
    }else if("LabeledStatement" == s){
        execLabeledStatement(nodeNum);
    }else if("LabeledStatementNoShortIf" == s){
        execLabeledStatementNoShortIf(nodeNum);
    }else if("Literal"==s){
        execLiteral(nodeNum);
    }else if("LocalVariableDeclaration"==s){
        execLocalVariableDeclaration(nodeNum);
    }else if("LocalVariableDeclarationStatement"==s){
        execLocalVariableDeclarationStatement(nodeNum);
    }else if("MethodDeclarator"==s){
        execMethodDeclarator(nodeNum);
    }else if("MethodHeader" == s){
        execMethodHeader(nodeNum);
    }else if("Modifier" == s){
        execModifier(nodeNum);
    }else if("Modifiers" == s){
        execModifiers(nodeNum);
    }else if("NullLiteral" == s){
        execNullLiteral(nodeNum);
    }else if("NumericType" == s){
        execNumericType(nodeNum);
    }else if("PackageDeclaration" == s){
        execPackageDeclaration(nodeNum);
    }else if("PostDecrementExpression"==s){
        execPostDecrementExpression(nodeNum);
    }else if("PostIncrementExpression"==s){
        execPostIncrementExpression(nodeNum);
    }else if("PreDecrementExpression"==s){
        execPreDecrementExpression(nodeNum);
    }else if("PreIncrementExpression"==s){
        execPreIncrementExpression(nodeNum);
    }else if("PrimitiveType"==s){
        execPrimitiveType(nodeNum);
    }else if("QualifiedName"==s){
        execQualifiedName(nodeNum);
    }else if("ReferenceType"==s){
        execReferenceType(nodeNum);
    }else if("ReturnStatement"==s){
        execReturnStatement(nodeNum);
    }else if("SingleTypeImportDeclaration" == s){
        execSingleTypeImportDeclaration(nodeNum);
    }else if("StatementExpressionList" == s){
        execStatementExpressionList(nodeNum);
    }else if("StaticInitializer" == s){
        execStaticInitializer(nodeNum);
    }else if("StringLiteral" == s){
        execStringLiteral(nodeNum);
    }else if("Super"==s){
        execSuper(nodeNum);
    }else if("SwitchBlock"==s){
        execSwitchBlock(nodeNum);
    }else if("SwitchBlockStatementGroup"==s){
        execSwitchBlockStatementGroup(nodeNum);
    }else if("SwitchBlockStatementGroups"==s){
        execSwitchBlockStatementGroups(nodeNum);
    }else if("SwitchLabel"==s){
        execSwitchLabel(nodeNum);
    }else if("SwitchLabels"==s){
        execSwitchLabels(nodeNum);
    }else if("SwitchStatement"==s){
        execSwitchStatement(nodeNum);
    }else if("SynchronizedStatement"==s){
        execSynchronizedStatement(nodeNum);
    }else if("Throws"==s){
        execThrows(nodeNum);
    }else if("ThrowStatement"==s){
        execThrowStatement(nodeNum);
    }else if("TryStatement"==s){
        execTryStatement(nodeNum);
    }else if("Type"==s){
        execType(nodeNum);
    }else if("TypeImportOnDemandDeclaration"==s){
        execTypeImportOnDemandDeclaration(nodeNum);
    }else if("VariableDeclarator"==s){
        execVariableDeclarator(nodeNum);
    }else if("VariableDeclaratorId"==s){
        execVariableDeclaratorId(nodeNum);
    }else if("VariableDeclarators"==s){
        execVariableDeclarators(nodeNum);
    }else if("VariableInitializers"==s){
        execVariableInitializers(nodeNum);
    }else if("WhileStatementNoShortIf"==s){
        execWhileStatementNoShortIf(nodeNum);
    }
    else{
        cout << "function not written " << s << endl;
    }

    return;
}

void print3AC(int nodeNum){
    FILE* fp = freopen("hojayaar.txt","w",stdout);
    for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
        cout << (attr3AC[nodeNum].threeAC)[i] << endl;
    }
    fclose(fp);
}