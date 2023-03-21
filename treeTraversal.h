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
    if(prodNum[nodeNum]==1);
    return;
}