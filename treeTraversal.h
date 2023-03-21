#include <bits/stdc++.h>
#include "attr.h"

using namespace std;


int countNodes=0;
vector<string> nodeType;
map<int, vector<int>> adj;
map<int,int> prodNum;

vector<attr> attrSymTab;
vector<attr> attr3AC;

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
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            int c = adj[nodeNum][0];
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
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
}

void execConditionalExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
    }
}

void execConditionalOrExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
    }
}

void execConditionalAndExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        
    }
    return;
}

void execInclusiveOrExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
    }
    return;
}

void execExclusiveOrExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
    }
}

void execAndExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
    }
}

void execEqualityExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
    }
    return;
}

void execRelationalExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
    }
    return;
}

void execShiftExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
    }
}

void execAdditiveExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
    }
    return;
}

void execMultiplicativeExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
    }
    return;
}

void execUnaryExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
    return;
}

void execUnaryExpressionNotPlusMinus(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
    }

    return;
}

void execPostfixExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 2:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 3:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
        case 4:
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            break;
    }
}

void execName(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:
            int c = adj[nodeNum][0];
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