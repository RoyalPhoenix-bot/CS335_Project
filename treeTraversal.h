#include <bits/stdc++.h>
#include "attr.h"

using namespace std;

int countNodes=0;
int tempNum=0;
int countL=0;//counts the L in 'control flow statements' [as given in slides]
vector<string> nodeType;
map<int,int> lineNum;
map<int, vector<int>> adj;      
map<int,int> prodNum;

vector<int> helpScope;
stack<pair<int,int>> scopeCurr;
stack<pair<int,int>> scopeParent;

map<string, int> typeWiden = {{"double",0}, {"float",1}, {"long",2}, {"int",3}, {"byte",4}, {"short",4}, {"char",4}};//minimum value->widest
vector<string> indexType = {"double", "float", "long", "int", "byte"};

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


vector<localtableparams>* funcScope = nullptr; //to maintain function scope if any
vector<localtableparams>* classScope = nullptr; //to maintain class scope if any
int numFuncinClass=-1;

int classCount=0;
int funcInClass=0;
vector<int> scopeHelper;
// scopeHelper[0] is the number of 4 level stuffs
// scopeHelper[1] is the number of 5 level stuffs ...


vector<attr1> attrSymTab;
vector<attr> attr3AC;
vector<attr2> attrType;

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

    // cout<<"visiting: "<<nodeType[nodeNum]<<"\n";

    if (nodeType[nodeNum]=="ClassDeclaration"){ 
        
        
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
        // attr3AC.push_back(temp2);
        attrType.push_back(temp2);
    }
    currScope.push(make_pair(1,1));
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

// void execConditionalExpression(int nodeNum){
//     int c;
//     switch(prodNum[nodeNum]){
//         case 1:
//             c = adj[nodeNum][0];
//             attr3AC[nodeNum] = attr3AC[c];
//             break;
//         case 2:{
//             c = adj[nodeNum][0];
//             int c3 = adj[nodeNum][2];
//             int c5 = adj[nodeNum][4];
//             string ltrue = getNewLabel();
//             string lfalse = getNewLabel();
//             string nextStmt = getNewLabel();

//             attr3AC[nodeNum] = attr3AC[c];
//             string temp = "if " + attr3AC[c].addrName + " goto " + ltrue;
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//             temp = "goto " + lfalse;
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//             temp = ltrue + ":";
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//             attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c3];
//             temp = "goto " + nextStmt;
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//             temp = lfalse + ":";
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//             attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c5];
//             temp = nextStmt + ":";
//             (attr3AC[nodeNum].threeAC).push_back(temp);
//         }
//             break;
//     }
// }

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
        // int c3=adj[nodenum][2];
        // int c5=adj[nodenum][4];
        // int c7=adj[nodenum][6];

        // //change scopes for IF body
        // if (scopeHelper.size()==(currScope.top().first-3)){
        //     scopeHelper.push_back(0);
        // }
        
        // // currScope.top().first={3,4,5} 
        // scopeHelper[currScope.top().first-3]++;
        // parentScope.push(currScope.top());
        // currScope.push(make_pair(currScope.top().first+1,scopeHelper[currScope.top().first-3]));
        
        // preOrderTraversal(c3);
        // preOrderTraversal(c5);

        // parentScope.pop();
        // currScope.pop();

        // //change scopes for ELSE body
        // if (scopeHelper.size()==(currScope.top().first-3)){
        //     scopeHelper.push_back(0);
        // }
        
        // // currScope.top().first={3,4,5} 
        // scopeHelper[currScope.top().first-3]++;
        // parentScope.push(currScope.top());
        // currScope.push(make_pair(currScope.top().first+1,scopeHelper[currScope.top().first-3]));
        
        // preOrderTraversal(c7);

        // parentScope.pop();
        // currScope.pop();
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