#include <bits/stdc++.h>
#include "attr.h"

using namespace std;
map<string,string> typeOfNode; // to_string(nodeNum) -> Type of that node
// map<int, pair<string,vector<int>>> arrayInfo;
map<int,int> trueLabel;
map<int,int> falseLabel;
map<int,int> nextLabel;
int countNodes=0;
int tempNum=0;
map<string,int> typeSize;
int labelNum=0;
vector<string> nodeType;
map<int,int> lineNum;
map<int, vector<int>> adj;      
map<int,int> prodNum;

vector<int> helpScope;
stack<pair<int,int>> scopeCurr;
stack<pair<int,int>> scopeParent;

map<pair<int,int>, pair<int,int>> mapParentScope;
map<string, int> typeWiden = {{"double",0}, {"float",1}, {"long",2}, {"int",3}, {"byte",4}, {"short",4}, {"char",4}};//minimum value->widest
vector<string> indexType = {"double", "float", "long", "int", "byte"};

vector<string> integerTypes = {"double", "float", "long", "int", "byte", "short"};

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
    int line;
	vector<localTableParams>* localTablePointer; 
} globalTableParams;

vector<globalTableParams> globalTable;
vector<localTableParams>* currSymTab;
map< int, pair< pair<int,int>,vector<localTableParams>* > > scopeAndTable;

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


localTableParams* checkInScope(string _varName, pair<int,int> _scope, vector<localTableParams>* _tablePointer){
    // cout<<"Hi\n";
    for (auto &locRow:*(_tablePointer)){
        // cout<<locRow.name<<_varName<<" "<<locRow.scope.first<<_scope.first<<" "<<locRow.scope.second<<_scope.second<<endl;
        if (locRow.name==_varName && locRow.scope.first==_scope.first && locRow.scope.second==_scope.second)
            return &locRow;
    }

    return NULL;
}

string getType(string _varName, int _nodeNum){

    //return type

    vector<localTableParams>* primaryTable=scopeAndTable[_nodeNum].second;
    // cout<<_nodeNum<<" "<<scopeAndTable[_nodeNum].first.first<<" "<<scopeAndTable[_nodeNum].first.second<<endl;

    pair<int,int> startScope=scopeAndTable[_nodeNum].first;

    // cout<<startScope.first<<" "<<startScope.second<<endl;

    // check in current table and it's parent table (class table's parent is mapped to itself so no worries)
    
    while(startScope.first>1){

        auto rowPtr=checkInScope(_varName,startScope,primaryTable);

        if (rowPtr!=NULL){

            // got the row

            return rowPtr->type;

        }

        startScope=mapParentScope[startScope];
    }

    startScope=scopeAndTable[_nodeNum].first;

    while(startScope.first>1){

        auto rowPtr=checkInScope(_varName,startScope,(parentTable[primaryTable]));

        if (rowPtr!=NULL){

            // got the row

            return rowPtr->type;
        }

        startScope=mapParentScope[startScope];
    }

    return "notfound";   

}

string fillHelper(string _nodeNum){
        
        string ret;
        int node_number=atoi(_nodeNum.c_str());
        ret=getType(nodeType[node_number],node_number);
        
        if (ret[ret.size()-1]==')')
            ret=getArrayType(ret);
        
        typeOfNode[_nodeNum]=ret;
        return ret;
}

void filltypeOfNode(){

    for (auto &elem:typeOfNode){

        if (elem.second=="fillMe"){

            elem.second=fillHelper(elem.first);
        }
    }
}

void printfilltypeOfNode(){
    for (auto elem:typeOfNode)
        cout<<nodeType[stoi(elem.first)]<<" "<<elem.first<<"->"<<elem.second<<endl;
}

void storeParseTree(int flag){

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
	
}

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

int getLabelNumber(string s){
    string r = "";
    for(int i=1;i<s.length();i++)r+=s[i];
    return stoi(r);
}

void pushLabelUp(int par,int chld){
    if(trueLabel.find(par)!=trueLabel.end() && trueLabel.find(chld)!=trueLabel.end())trueLabel[par]=trueLabel[chld];
    if(falseLabel.find(par)!=falseLabel.end() && falseLabel.find(chld)!=falseLabel.end())falseLabel[par]=falseLabel[chld];
    if(nextLabel.find(par)!=nextLabel.end() && nextLabel.find(chld)!=nextLabel.end())nextLabel[par]=nextLabel[chld];
    return;
}

void initializeAttributeVectors(){
    for(int i=0;i<nodeType.size();i++){
        attr temp;
        attr1 temp2;
        attrSymTab.push_back(temp2);
        attr3AC.push_back(temp);
    }
    currScope.push(make_pair(1,1));
    typeSize["int"]=4;
    typeSize["long"]=8;
    typeSize["float"]=4;
    typeSize["bool"]=1;
    typeSize["double"]=8;
    typeSize["char"]=2;
    typeSize["short"]=2;
    return;
}

// map<int,pair<string,vector<int>>> arrayInfo;
// nodeNum -> {type,sizes[]}


// Given the array_name and nodeNumber, need to provide the array type and sizes
//    1. When the array is declared, store the sizes in the table [done]
//    2. Make a map nodeNum -> {Scope,tablePointer} [done]
//    3. Check for array_name in *tablePointer and it's parent scope tables to send out {type,sizes[]} stored in the table

// map nodeNum -> {Scope,tablePointer}

pair<string,vector<int>> getArrayInfo(string _arrayName, int _nodeNum){
    
    //return {type,arraySizes}

    vector<localTableParams>* primaryTable=scopeAndTable[_nodeNum].second;
    pair<int,int> startScope=scopeAndTable[_nodeNum].first;

    // check in current table and it's parent table (class table's parent is mapped to itself so no worries)
    pair<string,vector<int>> retObj;
    
    while(startScope.first>1){

        auto rowPtr=checkInScope(_arrayName,startScope,primaryTable);

        if (rowPtr!=NULL){

            // got the row

            retObj.first=getArrayType(rowPtr->type);
            retObj.second=rowPtr->arraySize;
            // cout<<"GOT IT: "<<retObj.first<<" "<<retObj.second.size()<<endl;
            return retObj;
        }

        startScope=mapParentScope[startScope];
    }

    startScope=scopeAndTable[_nodeNum].first;

    while(startScope.first>1){

        auto rowPtr=checkInScope(_arrayName,startScope,(parentTable[primaryTable]));

        if (rowPtr!=NULL){

            // got the row

            retObj.first=getArrayType(rowPtr->type);
            retObj.second=rowPtr->arraySize;

            // cout<<"GOT IT: "<<retObj.first<<" "<<retObj.second.size()<<endl;
            return retObj;
        }

        startScope=mapParentScope[startScope];
    }

    // cout<<"DIDN'T GET\n";
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

bool checkIfTypeOkay(string _t1, string _t2){

    bool t1isInt=find(integerTypes.begin(),integerTypes.end(),_t1)!=integerTypes.end();
    bool t2isInt=find(integerTypes.begin(),integerTypes.end(),_t2)!=integerTypes.end();

    if (t1isInt && t2isInt){
        return true;
    }
    else if (!t1isInt && !t2isInt){
        return _t1==_t2;
    }
    else{
        return false;
    }

}

void preOrderTraversal(int nodeNum){

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
        mapParentScope[currScope.top()]=parentScope.top();
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
                globRow.line=lineNum[adj[nodeNum][2]];
                break;
            default:
                preOrderTraversal(adj[nodeNum][1]);
                globRow.name=attrSymTab[adj[nodeNum][1]].name;
                globRow.line=lineNum[adj[nodeNum][1]];
                
        }

        // call Super 
        switch (prodNum[nodeNum])
        {   
            case 1:
            case 7:{ //Super at 4th
                int c3=adj[nodeNum][2];
                int c4=adj[nodeNum][3];
                preOrderTraversal(c4);
                globRow.parentClassNum=checkIfValidSuperClass(attrSymTab[c4].name,lineNum[c3]);
                // valid inheritence
                break;

            }
            case 2:
            case 4:{ //Super at 3rd
                int c2=adj[nodeNum][1];
                int c3=adj[nodeNum][2];
                preOrderTraversal(c3);
                globRow.parentClassNum=checkIfValidSuperClass(attrSymTab[c3].name,lineNum[c2]);
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
        // attrSymTab[nodeNum].decLine=lineNum[nodeNum];
        int c1=adj[nodeNum][0];
        scopeAndTable[c1].first=currScope.top();
        // cout<<c1<<" "<<scopeAndTable[c1].first.first<<" "<<scopeAndTable[c1].first.second<<endl;
        scopeAndTable[c1].second=currSymTab;
        attrSymTab[nodeNum].leafNodeNum=c1;
        typeOfNode[to_string(c1)]="fillMe";

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
            // cout<<"Hi bhai array declare hua\n";
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
            // cout<<attrSymTab[cb2].intParams.size()<<endl;

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
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
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
        globRow.line=lineNum[nodeNum];
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
        globRow.line=lineNum[nodeNum];
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
        globRow.line=lineNum[nodeNum];
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
            mapParentScope[currScope.top()]=parentScope.top();
            
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

        // if (prodNum[nodeNum]==1){

        //     int c1=adj[nodeNum][0];
        //     preOrderTraversal(c1);
        //     // attrSymTab[nodeNum].type=attrSymTab[c1].type;
        //     // attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;

        //     for (auto varName:attrSymTab[c1].otherParams){

        //         localTableParams locRow ;
        //         locRow.type=attrSymTab[c1].type;
        //         // cout<<locRow.type<<" From BlockStatement\n";
        //         locRow.name=varName;
        //         locRow.line=lineNum[nodeNum];
        //         locRow.scope=currScope.top();
        //         locRow.parentScope=parentScope.top();
        //         // cout<<"From BlockStatement: "<<varName<<endl; //works
        //         // cout<<locRow.type<<" "<<locRow.name<<endl; //works
        //         (*currSymTab).push_back(locRow);
        //         cout<<currSymTab<<" in blockstat "<<(*currSymTab).size()<<"\n";
        //         // cout<<currSymTab<<" From BlockStatement\n";
        //     }
        // }
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        

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

        //check if array is being declared
        if (attrSymTab[c2].intParams.size()!=0){
            // cout<<"Hi bhai array declare hua\n";
            //array it is

            // check if initialized with correct type
            bool flag=checkIfValidArrayDeclaration(attrSymTab[c1].type,attrSymTab[c2].type,attrSymTab[c2].intParams.size(),lineNum[nodeNum]);
            localTableParams locRow ;
            
            locRow.type=attrSymTab[c1].type;

            locRow.name=attrSymTab[c2].otherParams[0];
            locRow.line=lineNum[nodeNum];
            locRow.scope=currScope.top();
            locRow.parentScope=parentScope.top();
            locRow.arraySize=attrSymTab[c2].intParams;

            // cout<<"From FieldDeclaration:"<<locRow.arraySize.size()<<endl;
            (*currSymTab).push_back(locRow);
            // cout<<currSymTab<<" in locvaldec "<<(*currSymTab).size()<<"\n";
            
            // for (auto ro:*(currSymTab))
            //     cout<<ro.name<<" "<<ro.arraySize.size()<<endl;
            return;

        }
        else{

            int c1=adj[nodeNum][0];
            int c2=adj[nodeNum][1];
            preOrderTraversal(c1);
            preOrderTraversal(c2);
            // attrSymTab[nodeNum].type=attrSymTab[c1].type;
            // attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;

            for (auto varName:attrSymTab[c2].otherParams){

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
                // cout<<currSymTab<<" in locvardec "<<(*currSymTab).size()<<"\n";
                // cout<<currSymTab<<" From BlockStatement\n";
            }
        }
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
        mapParentScope[currScope.top()]=parentScope.top();
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
        mapParentScope[currScope.top()]=parentScope.top();
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
        mapParentScope[currScope.top()]=parentScope.top();
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
        // cout<<"[PreOrderTraversal] From ArrayCreationExpression: "<<attrSymTab[nodeNum].intParams.size()<<endl;
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
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
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
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
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
    else if (nodeType[nodeNum]=="AdditiveExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==1){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        }
        else {

            int c1=adj[nodeNum][0];
            int c3=adj[nodeNum][2];
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            string t1=fillHelper(to_string(attrSymTab[c1].leafNodeNum));
            string t3=fillHelper(to_string(attrSymTab[c3].leafNodeNum));
            
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            
            if (checkIfTypeOkay(t1,t3))
                attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
            else{

                string var1=nodeType[attrSymTab[c1].leafNodeNum];
                string var2=nodeType[attrSymTab[c3].leafNodeNum];

                cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<"' of '"<<var1<<"' does not match type '"<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<"' of '"<<var2<<"'!\nAborting...\n";
                exit(0);
            }
        }
        return;


    }
    else if (nodeType[nodeNum]=="Assignment"){
        
        for (auto child: adj[nodeNum])
            preOrderTraversal(child);
        
        int c1=adj[nodeNum][0];
        int c3=adj[nodeNum][2];
        // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
        string t1=fillHelper(to_string(attrSymTab[c1].leafNodeNum));
        string t3=fillHelper(to_string(attrSymTab[c3].leafNodeNum));
            
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            
        if (checkIfTypeOkay(t1,t3))
                attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
        else{

            string var1=nodeType[attrSymTab[c1].leafNodeNum];
            string var2=nodeType[attrSymTab[c3].leafNodeNum];

            cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<"' of '"<<var1<<"' does not match type '"<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<"' of '"<<var2<<"'!\nAborting...\n";
            exit(0);
        }

    }
    else if (nodeType[nodeNum]=="LeftHandSide"){

        preOrderTraversal(adj[nodeNum][0]);

        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        
        return;
    }
    else if (nodeType[nodeNum]=="PrimaryNoNewArray" || nodeType[nodeNum]=="UnaryExpressionNotPlusMinus" || nodeType[nodeNum]=="MultiplicativeExpression" || nodeType[nodeNum]=="ShiftExpression" || nodeType[nodeNum]=="RelationalExpression" || nodeType[nodeNum]=="EqualityExpression" || nodeType[nodeNum]=="AndExpression" || nodeType[nodeNum]=="ExclusiveOrExpression" || nodeType[nodeNum]=="InclusiveOrExpression"|| nodeType[nodeNum]=="ConditionalAndExpression" || nodeType[nodeNum]=="ConditionalOrExpression" || nodeType[nodeNum]=="ConditionalExpression" || nodeType[nodeNum]=="AssignmentExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==1){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
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
            // cout<<"[PreOrderTraversal] From VariableInitializer: "<<attrSymTab[nodeNum].intParams.size()<<endl;

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



void printTables(){

    freopen("global_symbol_table.csv","w",stdout);

    // print global table
    cout<<"Name,Type,Class Number,Parent Class Number,Line Number,Local Table Pointer\n";
    for (auto row: globalTable){

        if (row.type!="class"){
            cout<<row.name<<","<<row.type<<",-,-,"<<row.line<<",null\n";
        }
        else{
            cout<<row.name<<","<<row.type<<","<<row.classNum<<","<<row.parentClassNum<<","<<row.line<<","<<row.localTablePointer<<"\n";
        }
 
    }
    std::fclose(stdout);

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

            std::fclose(stdout);

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
                            if (fRow.arraySize.size()!=0){
                    
                                for (auto siz:(fRow.arraySize))
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

                        std::fclose(stdout);

                    }
		    }

        }
    }

    freopen("log.log","w",stdout);

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
    int c = adj[nodeNum][1];
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

void execMethodHeader_(int nodeNum){
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
            // cout << "statement " << attr3AC[nodeNum].threeAC.size() << endl;
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
            // cout << "statementexpression " << attr3AC[nodeNum].threeAC.size() << endl;;
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
            
            // auto mdata = arrayInfo[attr3AC[c].nodeno];// lowestnode
            // cout << "over here " << nodeNum << " " << attr3AC[c].nodeno << "? " << nodeType[attr3AC[c].nodeno] << endl;
            auto mdata=getArrayInfo(attr3AC[c].addrName,attr3AC[c].nodeno);
            // cout << "after calling  getArrayInfo" << nodeNum << " " << attr3AC[c].nodeno << " " << attr3AC[c].addrName << endl;
            string t = mdata.first;
            vector<int> d = mdata.second;
            // for(auto i : d)cout << i << endl;
            // cout << "idhar " << t << " " << d.size() << endl;
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
            attr3AC[nodeNum].nameAtNode = nodeType[attr3AC[c].nodeno];
            // cout << "yaha pe hu aray1 " << attr3AC[nodeNum].nameAtNode << " " << attr3AC[c].nameAtNode << endl;
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
            for(int i=attr3AC[c].dimsDone;i<attr3AC[c].arrDims.size();i++){
                mult*=(attr3AC[c].arrDims)[i];
            }
            tempNum++;
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c3].addrName + " * " + to_string(mult);
            attr3AC[nodeNum].threeAC.push_back(temp);
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            // attr3AC[nodeNum].dimsDone++;
            attr3AC[nodeNum].dimsDone = attr3AC[c].dimsDone+1;
            attr3AC[nodeNum].nameAtNode = attr3AC[c].nameAtNode;
            // cout << "yaha pe hu aray " << attr3AC[nodeNum].nameAtNode << " " << attr3AC[c].nameAtNode << endl;
            temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].addrName + " + t" + to_string(tempNum-1) ;
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
    // cout << "Dimexpr " << attr3AC[c].nameAtNode << endl;
    if(attr3AC[c].nameAtNode.size()!=0)attr3AC[nodeNum].arrDims.push_back(stoi(attr3AC[c].nameAtNode));
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
            // cout << "idahr dekhra hu " << attr3AC[nodeNum].nameAtNode << " " << attr3AC[c].nameAtNode << endl;
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
            // cout << "assignment " << temp << endl;
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
            // cout << "LeftHandSide " << attr3AC[nodeNum].addrName << " " << attr3AC[c].addrName << endl;

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
    // cout << "Simplename " << attr3AC[nodeNum].addrName << " " << attr3AC[c].addrName << endl;
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
    }else if("MethodHeader_"==s){
        execMethodHeader_(nodeNum);
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