#include <bits/stdc++.h>
#include "attr.h"

using namespace std;


int countNodes=0;
int tempNum=0;
int countL=0;//counts the L in 'control flow statements' [as given in slides]
vector<string> nodeType;
map<int, vector<int>> adj;
map<int,int> prodNum;

vector<attr> attrSymTab;
vector<attr> attr3AC;

string getNewLabel(){
    countL++;
    string r = "L" + to_string(countL);
    return r;
}


void initializeAttributeVectors(){
    for(int i=0;i<nodeType.size();i++){
        attr temp;
        attrSymTab.push_back(temp);
        attr3AC.push_back(temp);
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