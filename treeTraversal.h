#include <bits/stdc++.h>
#include "attr.h"

using namespace std;

int countNodes=0;
vector<string> nodeType;
map<int,int> lineNum;
map<int, vector<int>> adj;
map<int,int> prodNum;

int classCount=0;
int funcInClass=0;
vector<int> scopeHelper;
// scopeHelper[0] is the number of 4 level stuffs
// scopeHelper[1] is the number of 5 level stuffs ...

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


typedef struct globaltableparams{
	string name="";
	string type="";
    int classNum;
    int parentClassNum=0;
	vector<localTableParams>* localTablePointer; 
} globalTableParams;

vector<globalTableParams> globalTable;
vector<localTableParams>* currSymTab;

vector<attr1> attrSymTab;
vector<attr2> attr3AC;

stack<pair<int,int>> currScope;
stack<pair<int,int>> parentScope;

map<vector<localTableParams>*,vector<localTableParams>*> parentTable;

// map<int,pair<string,vector<int>>> arrayInfo;
// nodeNum -> {type,sizes[]}


// Given the array_name and nodeNumber, need to provide the array type and sizes
//    1. When the array is declared, store the sizes in the table [done]
//    2. Make a map nodeNum -> {Scope,tablePointer} [done]
//    3. Check for array_name in *tablePointer and it's parent scope tables to send out {type,sizes[]} stored in the table

// map nodeNum -> {Scope,tablePointer}
map< int, pair< pair<int,int>,vector<localTableParams>* > > scopeAndTable;

string getArrayType(string _type){
    
    string lType_rev;
    for (int i=_type.length()-1; ;i--){

        if (_type[i]!=')'){
            lType_rev="";
            while(_type[i]!='('){
                lType_rev+=_type[i];
                i--;
            }
            break;
        }

    }

    string lType="";

    for (int i=lType_rev.length()-1;i>=0;i--)
        lType+=lType_rev[i];
    
    return lType;
}

pair<string,vector<int>> getArrayInfo(string _arrayName, int _nodeNum, vector<localTableParams>* _startTablePointer){
    
    //return {type,arraySizes}
    vector<localTableParams>* primaryTable=scopeAndTable[_nodeNum].second;
    pair<int,int> arrayScope=scopeAndTable[_nodeNum].first;

    // check in current table and it's parent table (class table's parent is mapped to itself so no worries)
    pair<string,vector<int>> retObj;
    
    for (auto locRow:*primaryTable){
        if (locRow.arraySize.size()!=0 && locRow.scope.first==arrayScope.first && locRow.scope.second==arrayScope.second ){
            
            retObj.first=getArrayType(locRow.type);
            retObj.second=locRow.arraySize;

            return retObj;
        }

    }

    for (auto locRow:*(parentTable[primaryTable])){
        if (locRow.arraySize.size()!=0 && locRow.scope.first==arrayScope.first && locRow.scope.second==arrayScope.second ){
            
            retObj.first=getArrayType(locRow.type);
            retObj.second=locRow.arraySize;

            return retObj;
        }
    }

    return retObj;   
}

int checkIfValidSuperClass(string _superClass, int _line){

    for (auto globRow:globalTable){

        if (globRow.type=="class" && globRow.name==_superClass){
            return globRow.classNum;
        }
    }

    // Super class not found
    cout<<"[Compilation Error]: Undeclared class on line "<<_line<<"\nSuper Class '"<<_superClass<<"' must be declared before inheriting!\nAborting...\n";
    exit(0);

    return 0;
}

bool checkIfValidArrayDeclaration(string _typeLHS, string _typeRHS, int _rDims,  int _line){
    
    int lDims=0;
    string lType_rev;
    for (int i=_typeLHS.length()-1; ;i--){

        if (_typeLHS[i]!=')'){
            lType_rev="";
            while(_typeLHS[i]!='('){
                lType_rev+=_typeLHS[i];
                i--;
            }
            break;
        }

        lDims++;
    }

    string lType="";

    for (int i=lType_rev.length()-1;i>=0;i--)
        lType+=lType_rev[i];

    if (lDims!=_rDims){
        cout<<"[Compilation Error]: Dimension mismatch on line "<<_line<<"\nLHS dimensions "<<lDims<<" do not match "<<_rDims<<"!\nAborting...\n";
        exit(0);
    }


    if (lType==_typeRHS){
        return true;
    }
    else{
        cout<<"[Compilation Error]: Type mismatch on line "<<_line<<"\nArray Type "<<lType<<" does not match "<<_typeRHS<<"!\nAborting...\n";
        exit(0);
    }
    return false;
}

void preOrderTraversal(int nodeNum){

    if (nodeType[nodeNum]=="ClassDeclaration"){ // not checking if this class is inherited or not, so this new class will have it's own scope
        
        
        globalTableParams globRow ;
        
        // new class being formed (subclass is also a new class)
        classCount++;
        funcInClass=0;
        currSymTab= new vector<localTableParams>();
        globRow.localTablePointer = currSymTab;
        parentTable[currSymTab]=currSymTab;
        // update scopes before going inside the class 
        parentScope.push(currScope.top());
        currScope.push(make_pair(2,classCount));
        globRow.classNum=classCount;
        // cout<<currSymTab<<" From ClassDeclaration\n";

        //call the children from left to right
        
        // ClassDeclaration: Modifiersopt class Identifier Superopt Interfacesopt ClassBody
        // We are only interested in ClassBody which is always at the last and compulsory and Identifier
        
        // call Identifier
        switch (prodNum[nodeNum])
        {
            case 1:
            case 6:
            case 7:
            case 8:
                preOrderTraversal(adj[nodeNum][2]);
                globRow.name=attrSymTab[adj[nodeNum][2]].name;
                break;
            default:
                preOrderTraversal(adj[nodeNum][1]);
                globRow.name=attrSymTab[adj[nodeNum][1]].name;
                
        }

        // call Super 
        switch (prodNum[nodeNum])
        {   
            case 1:
            case 7:{ //Super at 4th
                int c4=adj[nodeNum][3];
                preOrderTraversal(c4);
                globRow.parentClassNum=checkIfValidSuperClass(attrSymTab[c4].name,lineNum[nodeNum]);
                // valid inheritence
                break;

            }
            case 2:
            case 4:{ //Super at 3rd
                int c3=adj[nodeNum][2];
                preOrderTraversal(c3);
                globRow.parentClassNum=checkIfValidSuperClass(attrSymTab[c3].name,lineNum[nodeNum]);
                // valid inheritence
                break;
            }
            default:{ // no Super
                
                break;
            }
        }

        // call ClassBody
        preOrderTraversal(adj[nodeNum][adj[nodeNum].size()-1]);
        
        // class ended, change scopes and complete the global table entry
        currScope.pop();
        parentScope.pop();
        
        globRow.type="class";
        globalTable.push_back(globRow);
        return;

    }
    else if (nodeType[nodeNum]=="Identifier"){

        // push the lexeme into Identifier's attributes
        attrSymTab[nodeNum].name=(nodeType[adj[nodeNum][0]]);
        // cout<<"From Identifier:"<<attrSymTab[nodeNum].name<<"\n";
        // cout<<(nodeType[adj[nodeNum][0]])<<"\n";
        attrSymTab[nodeNum].type=(nodeType[adj[nodeNum][0]]);
        scopeAndTable[nodeNum].first=currScope.top();
        scopeAndTable[nodeNum].second=currSymTab;
        return;
    }
    else if (nodeType[nodeNum]=="Super"){
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);
        attrSymTab[nodeNum].name=attrSymTab[c2].name;
        return;
    }
    else if (nodeType[nodeNum]=="ClassType" || nodeType[nodeNum]=="ClassOrInterfaceType"){
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        attrSymTab[nodeNum].name=attrSymTab[c1].name;
        attrSymTab[nodeNum].type=attrSymTab[c1].type; 
        return;
    }
    else if (nodeType[nodeNum]=="ClassBody"){

        // ClassBody:{ ClassBodyDeclarationsopt }
        // check if there is ClassBodyDeclarations and call it

        if (adj[nodeNum].size()==3){
            preOrderTraversal(adj[nodeNum][1]);
        }

        // else
        return;
    }
    else if (nodeType[nodeNum]=="FieldDeclaration"){
        // FieldDeclaration: Modifiersopt Type VariableDeclarators ;

        // new variable(s) being declared
        // localTableParams localRow ;

        // call Type (always 3rd from the back)
        int cb3=adj[nodeNum][adj[nodeNum].size()-3];
        preOrderTraversal(cb3);

        // call VariableDeclarators (always 2nd from the back)
        int cb2=adj[nodeNum][adj[nodeNum].size()-2];
        preOrderTraversal(cb2);
        // cout<<"hey"<<endl;
        // cout<<attrSymTab[cb2].intParams.size()<<endl;

        //check if array is being declared
        if (attrSymTab[cb2].intParams.size()!=0){
            // cout<<"Hi bhai yaha hu\n";
            //array it is

            // check if initialized with correct type
            bool flag=checkIfValidArrayDeclaration(attrSymTab[cb3].type,attrSymTab[cb2].type,attrSymTab[cb2].intParams.size(),lineNum[nodeNum]);
            
            localTableParams locRow ;
            
            locRow.type=attrSymTab[cb3].type;

            locRow.name=attrSymTab[cb2].otherParams[0];
            locRow.line=lineNum[nodeNum];
            locRow.scope=currScope.top();
            locRow.parentScope=parentScope.top();
            locRow.arraySize=attrSymTab[cb2].intParams;

            // cout<<"From FieldDeclaration:"<<locRow.arraySize.size()<<endl;
            (*currSymTab).push_back(locRow);
            return;

        }

        // save variable names in localRow along with their common type
        // add localRow(s) to currTable
        for (int i=0;i<attrSymTab[cb2].otherParams.size();i++){
            localTableParams locRow ;
            
            locRow.type=attrSymTab[cb3].type;

            locRow.name=attrSymTab[cb2].otherParams[i];
            locRow.line=lineNum[nodeNum];
            locRow.scope=currScope.top();
            locRow.parentScope=parentScope.top();
            (*currSymTab).push_back(locRow);
        }

        return;

    }
    else if (nodeType[nodeNum]=="Type"){
        // Type: PrimitiveType | ReferenceType
        // call the child
        preOrderTraversal(adj[nodeNum][0]);
        
        // type is filled
        attrSymTab[nodeNum].type=attrSymTab[adj[nodeNum][0]].type;
        return;
    }
    else if (nodeType[nodeNum]=="PrimitiveType"){

        if (nodeType[adj[nodeNum][0]]=="boolean"){
            attrSymTab[nodeNum].type="boolean";
        }
        else{
            preOrderTraversal(adj[nodeNum][0]);
            attrSymTab[nodeNum].type=attrSymTab[adj[nodeNum][0]].type;
        }
        return;
    }
    else if (nodeType[nodeNum]=="NumericType" || nodeType[nodeNum]=="ReferenceType" || nodeType[nodeNum]=="ClassOrInterfaceType" || nodeType[nodeNum]=="Name" || nodeType[nodeNum]=="SimpleName"){

        preOrderTraversal(adj[nodeNum][0]);
        attrSymTab[nodeNum].type=attrSymTab[adj[nodeNum][0]].type;
        attrSymTab[nodeNum].name=attrSymTab[adj[nodeNum][0]].name;
        return;
    }
    else if (nodeType[nodeNum]=="IntegralType" || nodeType[nodeNum]=="FloatingPointType"){
        
        attrSymTab[nodeNum].type=nodeType[adj[nodeNum][0]];
        return;
    }
    else if (nodeType[nodeNum]=="QualifiedName"){
        //QualifiedName: Name . Identifier
        // saving type as the attribute 'type' in Identifier:
        int c1=adj[nodeNum][0];
        int c3=adj[nodeNum][2];
        preOrderTraversal(c1);
        preOrderTraversal(c3);
        attrSymTab[nodeNum].type=attrSymTab[c3].type ;
        attrSymTab[nodeNum].name=attrSymTab[c1].name+"."+attrSymTab[c3].name;
        // cout<<attrSymTab[nodeNum].name;
        return;
    }
    else if (nodeType[nodeNum]=="ArrayType"){
        
        for (auto child: adj[nodeNum]){
            preOrderTraversal(child);
        }

        int c1=adj[nodeNum][0];

        attrSymTab[nodeNum].type="array(" + attrSymTab[c1].type + ")";
        
    }
    else if (nodeType[nodeNum]=="VariableDeclarators"){
        
        for (auto child: adj[nodeNum]){
            preOrderTraversal(child);
        }

        int c1=adj[nodeNum][0];
        
        if (prodNum[nodeNum]==1){
            attrSymTab[nodeNum].otherParams.push_back(attrSymTab[c1].name);
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;

            // cout<<attrSymTab[nodeNum].intParams.size()<<endl;
        }
        else{
            int c3=adj[nodeNum][2];
            
            for (int i=0;i<attrSymTab[c1].otherParams.size();i++){
                attrSymTab[nodeNum].otherParams.push_back(attrSymTab[c1].otherParams[i]);
            }
            attrSymTab[nodeNum].otherParams.push_back(attrSymTab[c3].name);
        }
        return;
    }
    else if (nodeType[nodeNum]=="VariableDeclarator"){

        for(auto child:adj[nodeNum]){
            preOrderTraversal(child);
        }
        
        int c1=adj[nodeNum][0];

        attrSymTab[nodeNum].name=attrSymTab[c1].name;

        if (prodNum[nodeNum]==2){
            int c3=adj[nodeNum][2];
            attrSymTab[nodeNum].type=attrSymTab[c3].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c3].intParams;
        }
        return;
    }
    else if (nodeType[nodeNum]=="VariableDeclaratorId"){
        
        for (auto child: adj[nodeNum]){
            preOrderTraversal(child);
        }

        int c1=adj[nodeNum][0];
        attrSymTab[nodeNum].name=attrSymTab[c1].name;
        
        if (prodNum[nodeNum]==2){
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
        }
        return;
    }
    else if (nodeType[nodeNum]=="PackageDeclaration"){
        //PackageDeclaration: package Name ;
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);

        globalTableParams globRow ;
        globRow.type="package";
        globRow.name=attrSymTab[c2].name;
        globalTable.push_back(globRow);
        return;

    }
    else if (nodeType[nodeNum]=="TypeImportOnDemandDeclaration"){
        // TypeImportOnDemandDeclaration:import Name . * ;
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);
        globalTableParams globRow;
        globRow.name=attrSymTab[c2].name;
        globRow.type="import_on_demand";
        globalTable.push_back(globRow);
        return;
    }
    else if (nodeType[nodeNum]=="SingleTypeImportDeclaration"){
        // SingleTypeImportDeclaration:import Name ;
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);
        globalTableParams globRow;
        globRow.name=attrSymTab[c2].name;
        globRow.type="import";
        globalTable.push_back(globRow);
        return;
    }
    else if (nodeType[nodeNum]=="MethodDeclaration"){

            funcInClass++;

            localTableParams locRow;
            locRow.scope=currScope.top();
            locRow.parentScope=parentScope.top();
            
            parentScope.push(currScope.top());
            currScope.push(make_pair(3,funcInClass));
            
            locRow.functionTablePointer=new vector<localTableParams>();
            locRow.type="method";
            vector<localTableParams>* saveClassSymTab=currSymTab;
            currSymTab=locRow.functionTablePointer;

            parentTable[currSymTab]=saveClassSymTab;

            int c1=adj[nodeNum][0];
            int c2=adj[nodeNum][1];

            preOrderTraversal(c1);
            preOrderTraversal(c2);
            
            parentScope.pop();
            currScope.pop();
            
            currSymTab=saveClassSymTab;

            locRow.name=attrSymTab[c1].name;
            // cout<<locRow.name<<" From MethodDeclaration\n";
            locRow.functionReturnType=attrSymTab[c1].type;
            locRow.functionParams=attrSymTab[c1].otherParams;
            locRow.line=lineNum[nodeNum];

            (*currSymTab).push_back(locRow);
            return;
      
    }
    else if (nodeType[nodeNum]=="MethodHeader_"){
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        attrSymTab[nodeNum].type=attrSymTab[c1].type;
        attrSymTab[nodeNum].name=attrSymTab[c1].name;
        attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;
        return;
    }
    else if (nodeType[nodeNum]=="MethodHeader"){
            // MethodHeader: Modifiersopt Type/void MethodDeclarator Throwsopt
            
            // cout<<currSymTab<<" From MethodHeader\n";

            // get name,type and otherParams
            switch (prodNum[nodeNum])
            {
            case 1:
            case 3:{
    
                int c2=adj[nodeNum][1];
                int c3=adj[nodeNum][2];
                preOrderTraversal(c2);
                attrSymTab[nodeNum].type=attrSymTab[c2].type;
                preOrderTraversal(c3);// will give name and function params
                attrSymTab[nodeNum].name=attrSymTab[c3].name;
                for (auto fparam:attrSymTab[c3].otherParams){
                    attrSymTab[nodeNum].otherParams.push_back(fparam);
                }
                break;
            }
            case 2:
            case 4:{

                int c1=adj[nodeNum][0];
                int c2=adj[nodeNum][1];
                preOrderTraversal(c1);
                attrSymTab[nodeNum].type=attrSymTab[c1].type;
                preOrderTraversal(c2);
                attrSymTab[nodeNum].name=attrSymTab[c2].name;
                for (auto fparam:attrSymTab[c2].otherParams){
                    attrSymTab[nodeNum].otherParams.push_back(fparam);
                }
                break;
            }
            case 5:
            case 6:{
                
                int c3=adj[nodeNum][2];
                attrSymTab[nodeNum].type="void";
                preOrderTraversal(c3);
                attrSymTab[nodeNum].name=attrSymTab[c3].name;
                for (auto fparam:attrSymTab[c3].otherParams){
                    attrSymTab[nodeNum].otherParams.push_back(fparam);
                }
                break;
            }
            case 7:
            case 8:{
                int c2=adj[nodeNum][1];
                attrSymTab[nodeNum].type="void";
                preOrderTraversal(c2);
                attrSymTab[nodeNum].name=attrSymTab[c2].name;
                for (auto fparam:attrSymTab[c2].otherParams){
                    attrSymTab[nodeNum].otherParams.push_back(fparam);
                }
                break;
            }
            default:
                break;
            }
            
            
    }
    else if (nodeType[nodeNum]=="MethodDeclarator"){
        // MethodDeclarator: Identifier ( FormalParameterListopt ) 
        
        switch (prodNum[nodeNum])
        {
        case 1:{
            //call Identifier
            int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
            attrSymTab[nodeNum].name=attrSymTab[c1].name;
            //call FormalParameterList
            int c3=adj[nodeNum][2];
            preOrderTraversal(c3);
            // update Formal Parameters' type
            for (auto paramType:attrSymTab[c3].otherParams){
                attrSymTab[nodeNum].otherParams.push_back(paramType);
            }
            break;
        }
        case 2:{
            //call Identifier
            int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
            attrSymTab[nodeNum].name=attrSymTab[c1].name;
            //no FormalParameterList
            break;
        }
        default:
            break;
        }
    }
    else if (nodeType[nodeNum]=="FormalParameterList"){

        switch (prodNum[nodeNum])
        {
        case 1:{
            int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
            attrSymTab[nodeNum].otherParams.push_back(attrSymTab[c1].type);
            break;
        }
        case 2:{
            int c1=adj[nodeNum][0];
            int c3=adj[nodeNum][2];
            preOrderTraversal(c1);
            preOrderTraversal(c3);
            for (auto param:attrSymTab[c1].otherParams){
                attrSymTab[nodeNum].otherParams.push_back(param);
            }
            attrSymTab[nodeNum].otherParams.push_back(attrSymTab[c3].type);
        }
        default:
            break;
        }
        return;
    }
    else if (nodeType[nodeNum]=="FormalParameter"){
        // only need the Type
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        attrSymTab[nodeNum].type=attrSymTab[c1].type ;
        return;
    }
    else if (nodeType[nodeNum]=="BlockStatement"){

        if (prodNum[nodeNum]==1){

            int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
            // attrSymTab[nodeNum].type=attrSymTab[c1].type;
            // attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;

            for (auto varName:attrSymTab[c1].otherParams){

                localTableParams locRow ;
                locRow.type=attrSymTab[c1].type;
                // cout<<locRow.type<<" From BlockStatement\n";
                locRow.name=varName;
                locRow.line=lineNum[nodeNum];
                locRow.scope=currScope.top();
                locRow.parentScope=parentScope.top();
                // cout<<"From BlockStatement: "<<varName<<endl; //works
                // cout<<locRow.type<<" "<<locRow.name<<endl; //works
                (*currSymTab).push_back(locRow);
                // cout<<currSymTab<<" From BlockStatement\n";
            }
        }
        else{
             int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
        }

        return;
    }
    else if (nodeType[nodeNum]=="LocalVariableDeclarationStatement"){
        
            int c1=adj[nodeNum][0];
            preOrderTraversal(c1);
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;
 
    }
    else if (nodeType[nodeNum]=="LocalVariableDeclaration"){
        int c1=adj[nodeNum][0];
        int c2=adj[nodeNum][1];
        
        preOrderTraversal(c1);
        preOrderTraversal(c2);

        attrSymTab[nodeNum].type=attrSymTab[c1].type;
        attrSymTab[nodeNum].otherParams=attrSymTab[c2].otherParams;
        return;
    }
    else if (nodeType[nodeNum]=="ForStatement" || nodeType[nodeNum]=="IfThenStatement" || nodeType[nodeNum]=="WhileStatement"){
        //change scopes
        if (scopeHelper.size()==(currScope.top().first-3)){
            scopeHelper.push_back(0);
        }

        // currScope.top().first={3,4,5} 
        scopeHelper[currScope.top().first-3]++;
        parentScope.push(currScope.top());
        currScope.push(make_pair(currScope.top().first+1,scopeHelper[currScope.top().first-3]));
        
        for (auto child:adj[nodeNum]){
            preOrderTraversal(child);
        }
        parentScope.pop();
        currScope.pop();
        return;
    }
    else if (nodeType[nodeNum]=="IfThenElseStatement"){
        
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

        return;
    }
    else if (nodeType[nodeNum]=="ForInit"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        for (auto var:attrSymTab[c1].otherParams){
            localTableParams locRow ;
            locRow.name=var;
            locRow.type=attrSymTab[c1].type;
            locRow.scope=currScope.top();
            locRow.parentScope=parentScope.top();
            locRow.line=lineNum[nodeNum];
            currSymTab->push_back(locRow);

        }
        return;
        
    }
    else if (nodeType[nodeNum]=="ArrayCreationExpression"){

        for(auto child:adj[nodeNum])
            preOrderTraversal(child);

        int c2=adj[nodeNum][1];
        int c3=adj[nodeNum][2];

        attrSymTab[nodeNum].type=attrSymTab[c2].type;
        attrSymTab[nodeNum].intParams=attrSymTab[c3].intParams;
        return;
    }
    else if (nodeType[nodeNum]=="DimExprs"){

        for (auto child:adj[nodeNum])
            preOrderTraversal(child);
        
        if (prodNum[nodeNum]==1){
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].intParams.push_back(attrSymTab[c1].num);
        }
        else{
            int c1=adj[nodeNum][0];
            int c2=adj[nodeNum][1];

            for (auto intParam:attrSymTab[c1].intParams)
                attrSymTab[nodeNum].intParams.push_back(intParam);
            
            attrSymTab[nodeNum].intParams.push_back(attrSymTab[c2].num);

        }
        return;
        
    }
    else if (nodeType[nodeNum]=="DimExpr"){
        
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);

        attrSymTab[nodeNum].num=attrSymTab[c2].num;    
        
        return;
    }
    else if (nodeType[nodeNum]=="Expression"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        attrSymTab[nodeNum].num=attrSymTab[c1].num;    
        attrSymTab[nodeNum].type=attrSymTab[c1].type;
        attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
        
        return;
    }
    else if (nodeType[nodeNum]=="UnaryExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==5){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num;  
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
        }
        return;
    }
    else if (nodeType[nodeNum]=="PostfixExpression"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        if (prodNum[nodeNum]==1){
            
            attrSymTab[nodeNum].num=attrSymTab[c1].num;   
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
        }
        return;
    }
    else if (nodeType[nodeNum]=="Primary"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        if (prodNum[nodeNum]==1){
            attrSymTab[nodeNum].num=attrSymTab[c1].num;    
        }
        else{

            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;

        }
        return;
    }
    else if (nodeType[nodeNum]=="PrimaryNoNewArray" || nodeType[nodeNum]=="UnaryExpressionNotPlusMinus" || nodeType[nodeNum]=="MultiplicativeExpression" || nodeType[nodeNum]=="AdditiveExpression" || nodeType[nodeNum]=="ShiftExpression" || nodeType[nodeNum]=="RelationalExpression" || nodeType[nodeNum]=="EqualityExpression" || nodeType[nodeNum]=="AndExpression" || nodeType[nodeNum]=="ExclusiveOrExpression" || nodeType[nodeNum]=="InclusiveOrExpression"|| nodeType[nodeNum]=="ConditionalAndExpression" || nodeType[nodeNum]=="ConditionalOrExpression" || nodeType[nodeNum]=="ConditionalExpression" || nodeType[nodeNum]=="AssignmentExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==1){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;   
        }
        return;
    }
    else if (nodeType[nodeNum]=="Literal"){
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        attrSymTab[nodeNum].num=attrSymTab[c1].num;
        return;
    }
    else if (nodeType[nodeNum]=="IntegerLiteral"  ){
        int c1=adj[nodeNum][0];
        attrSymTab[nodeNum].num=stoi(nodeType[c1]);
        return;
    }
    else if (nodeType[nodeNum]=="VariableInitializer"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        if (prodNum[nodeNum]==1){
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
        }

    }
    
    else {

        // simple visiting without any action
        for (auto child: adj[nodeNum]){
            preOrderTraversal(child);
        }
        return;
    }
    
}

void initializeAttributeVectors(){
    for(int i=0;i<nodeType.size();i++){
        attr1 temp;
        attr2 temp2;
        attrSymTab.push_back(temp);
        attr3AC.push_back(temp2);
    }
    return;
}

void printTables(){

    freopen("global_symbol_table.csv","w",stdout);

    // print global table
    cout<<"Name,Type,Class Number,Parent Class Number,Local Table Pointer\n";
    for (auto row: globalTable){

        if (row.type!="class"){
            cout<<row.name<<","<<row.type<<",-,-,null\n";
        }
        else{
            cout<<row.name<<","<<row.type<<","<<row.classNum<<","<<row.parentClassNum<<","<<row.localTablePointer<<"\n";
        }
 
    }

    //print class tables
    int cc=0;
    for (auto row: globalTable){
        
        if (row.type=="class"){
            int fc=0;
            cc++;
            string cCount=to_string(cc);
            string fname="class_table_";
            fname+=cCount;
            fname+=".csv";
            freopen(fname.c_str(),"w",stdout);

            cout<<"Name,Type,Array Size,Return Type,Function Param Types,Scope,Parent Scope,Line Number,Function Table Pointer\n";
            for(int j=0;j<(*(row.localTablePointer)).size();j++){
                
                cout<<(*(row.localTablePointer))[j].name<<",";
                cout<<(*(row.localTablePointer))[j].type;
                cout<<",";
                if ((*(row.localTablePointer))[j].arraySize.size()!=0){
                    
                    for (auto siz:(*(row.localTablePointer))[j].arraySize)
                        cout<<siz<<" ";
                    cout<<",";
                }
                else{
                    cout<<"-,";
                }
                    
                cout<<(*(row.localTablePointer))[j].functionReturnType<<",";

                for (auto param:(*(row.localTablePointer))[j].functionParams)
                    cout<<param<<" ";

                cout<<","<<(*(row.localTablePointer))[j].scope.first<<" "<<(*(row.localTablePointer))[j].scope.second ;

                cout<<","<<(*(row.localTablePointer))[j].parentScope.first<<" "<<(*(row.localTablePointer))[j].parentScope.second ;

                cout<<","<<(*(row.localTablePointer))[j].line ;
                if ((*(row.localTablePointer))[j].type=="method")
                    cout<<","<<(*(row.localTablePointer))[j].functionTablePointer<<"\n";
                else
                    cout<<",null\n";

		    }

        }
    }

    // print function tables

    cc=0;
    for (auto row: globalTable){
        
        if (row.type=="class"){
            int fc=0;
            cc++;

            for(int j=0;j<(*(row.localTablePointer)).size();j++){
                
                    if ((*(row.localTablePointer))[j].type=="method"){
                        fc++;
                    
                        vector<localTableParams> funcTable=*((*(row.localTablePointer))[j].functionTablePointer);
                        string ffname="class_"+to_string(cc)+"_function_"+to_string(fc)+"_table.csv";
                        freopen(ffname.c_str(),"w",stdout);

                        cout<<"Name,Type,Array Size,Scope,Parent Scope,Line Number\n";

                        for (auto fRow:funcTable){
                            cout<<fRow.name<<",";
                            cout<<fRow.type<<",";
                            if ((*(row.localTablePointer))[j].arraySize.size()!=0){
                    
                                for (auto siz:(*(row.localTablePointer))[j].arraySize)
                                    cout<<siz<<" ";
                                cout<<",";
                            }
                            else{
                                cout<<"-,";
                            }
                            cout<<fRow.scope.first<<" "<<fRow.scope.second<<",";
                            cout<<fRow.parentScope.first<<" "<<fRow.parentScope.second<<",";
                            cout<<fRow.line<<"\n";
                        }

                    }
		    }

        }
    }

    return;
}

