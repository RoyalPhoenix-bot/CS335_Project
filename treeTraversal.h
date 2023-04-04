#include <bits/stdc++.h>
#include "attr.h"

using namespace std;

map<string,string> funcParamTemp;//maps function parameter list to their corresponding temp
// ClassBody_NodeNumber->Class Name
map<string,int> initVal;
map<int,string>  classNameMap;
string insideClassName;
map<string,string> typeOfNode; // to_string(nodeNum) -> Type of that node
// map<int, pair<string,vector<int>>> arrayInfo;
map<int,int> trueLabel;
map<int,int> falseLabel;
map<int,int> nextLabel;
int countNodes=0;
int tempNum=0;
int inStatement=0;
int inMethodInvocation=0,inMN=0;
map<string,int> typeSize;
int labelNum=0;
vector<string> nodeType;
map<int,int> lineNum;
map<int, vector<int>> adj;      
map<int,int> prodNum;
string switchExpAddr;
string someExpAddr;
int isWhile=0;string whileContinueLabel,whileBreakLabel;
int isFor=0;string forContinueLabel,forBreakLabel;
int isDo=0;string doContinueLabel,doBreakLabel;

map<string, string> functionParameterMap;
map<pair<int,int>, pair<int,int>> mapParentScope;

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

int classCount=0;
int funcInClass=0;
vector<int> scopeHelper;
// scopeHelper[0] is the number of 4 level stuffs
// scopeHelper[1] is the number of 5 level stuffs ...


vector<attr1> attrSymTab;
vector<attr> attr3AC;

stack<pair<int,int>> currScope;
stack<pair<int,int>> parentScope;

map<vector<localTableParams>*,vector<localTableParams>*> parentTable;

string getFuncRet(int calleNodeNum, string funcName, string className){
    for(int i=0;i<globalTable.size();i++){
        if(globalTable[i].type=="class" && globalTable[i].name==className){
            vector<localTableParams> *classtab = globalTable[i].localTablePointer;
            for(int j=0;j<(*classtab).size();j++){
                if((*classtab)[j].type=="method" && (*classtab)[j].name==funcName){
                    if(lineNum[calleNodeNum]>=(*classtab)[j].line){
                        // cout << "idhar  " << (*classtab)[j].functionReturnType << " " << funcName << " " << className << endl;
                        return (*classtab)[j].functionReturnType;
                    }
                }
            }
        }
    }
    cout<<"[Compilation Error]: Function not declared but called on line "<<lineNum[calleNodeNum]<<"\nFunction '"<<  funcName << "' in class '"<< className << "' !\nAborting...\n";
    exit(0);
}

string removeLastChar(string _type){
    string ret="";
    for (int i=0;i<_type.size()-1;i++)
        ret+=_type[i];
    
    return ret;
}
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


void fillFunctionOffsets(vector<localTableParams>* _funcTablePtr){
    int funcOffset=0;
    for(auto &funcRow: *_funcTablePtr){

            funcRow.offset=funcOffset;

            if (funcRow.arraySize.size()==0)
                funcOffset+=typeSize[funcRow.type];
            else{
                //it's an array
                int jump=1;
                for (int i=0;i<funcRow.arraySize.size();i++)
                    jump=jump*funcRow.arraySize[i];
                jump*=typeSize[getArrayType(funcRow.type)];
                funcOffset+=jump;
            }
    } 
}

void fillClassOffsets(vector<localTableParams>* _classTablePtr){
    int classOffset=0;
    for(auto &classRow: *_classTablePtr){

        if (classRow.type=="method"){
            fillFunctionOffsets(classRow.functionTablePointer);
        }
        else {
            classRow.offset=classOffset;

            if (classRow.arraySize.size()==0)
                classOffset+=typeSize[classRow.type];
            else{
                //it's an array
                int jump=1;
                for (int i=0;i<classRow.arraySize.size();i++)
                    jump=jump*classRow.arraySize[i];
                jump*=typeSize[getArrayType(classRow.type)];
                classOffset+=jump;
            }

        }
    }
}

void fillOffsets(){

    for (auto & globRow:globalTable){
        if (globRow.type=="class"){
            fillClassOffsets(globRow.localTablePointer);
        }
    }

}

int getOffset(int _nodeNum){
    
    vector<localTableParams>* funTabPtr = scopeAndTable[_nodeNum].second ;
    vector<localTableParams>* classTabPtr = parentTable[funTabPtr];
    string varName=nodeType[_nodeNum];

    for (auto cRow: *classTabPtr){

        if (cRow.name==varName){
            return cRow.offset;
        }
    }

    return 0;
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
        
        int node_number=atoi(_nodeNum.c_str());
        // cout<<"from FillHeper: "<<nodeType[node_number]<<endl;

        if (find(integerTypes.begin(),integerTypes.end(),typeOfNode[_nodeNum])!=integerTypes.end() || typeOfNode[_nodeNum]=="char" || typeOfNode[_nodeNum]=="String" || typeOfNode[_nodeNum]=="true" || typeOfNode[_nodeNum]=="false")
            return typeOfNode[_nodeNum];
        
        string ret;
        ret=getType(nodeType[node_number],node_number);
        // cout<<ret<<endl;
        if (ret[ret.size()-1]==')'){
            string og_type=ret;
            ret=getArrayType(ret);
            int i=og_type.size()-1;
            int dims=0;
            while(og_type[i]==')'){
                dims++ ;
                i--;
            }
            ret=ret+to_string(dims)+";";
        }
        
        typeOfNode[_nodeNum]=ret;
        return ret;
}

void filltypeOfNode(){

    for (auto &elem:typeOfNode){
        // cout << "filltypeofnode " << elem.first << " " << elem.second << endl;
        
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

string getTypeNode(int c){
    // cout << "GET TYPE " << attr3AC[c].nodeno << " " << typeOfNode[to_string(attr3AC[c].nodeno)] << endl;
    string tp;
    if(typeOfNode.find(to_string(attr3AC[c].nodeno))!=typeOfNode.end()){
        tp = typeOfNode[to_string(attr3AC[c].nodeno)];
    }
    if(attr3AC[c].addrName.size()>0 && typeOfNode.find(attr3AC[c].addrName)!=typeOfNode.end()){
        tp = typeOfNode[attr3AC[c].addrName];
    }
    return tp;
}

void widenNode(int a , int b){
    string p = getTypeNode(a);
    string q = getTypeNode(b);
    // cout << "choda karna he " << p << " " << q << endl;
    if(p=="int" && q=="float"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_float " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "float";
        typeOfNode[t] = "float";
        //call table update
    }else if(p=="int" && q=="double"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }else if(p=="float" && q=="double"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }else if(p=="float" && q=="int"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_float " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "float";
        typeOfNode[t] = "float";
        //call table update
    }else if(p=="double" && q=="float"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }else if(p=="double" && q=="int"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }else if(p=="int" && q=="long"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_long " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "long";
        typeOfNode[t] = "long";
        //call table update
    }else if(p=="long" && q=="float"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_float " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "float";
        typeOfNode[t] = "float";
        //call table update
    }else if(p=="long" && q=="double"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[a].addrName;
        attr3AC[a].threeAC.push_back(temp);
        attr3AC[a].addrName = t;
        attr3AC[a].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }else if(q == "int" && p=="long"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_long " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "long";
        typeOfNode[t] = "long";
        //call table update
    }else if(p=="float" && q=="long"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_float " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "float";
        typeOfNode[t] = "float";
        //call table update
    }else if(p == "double" && q=="long"){
        tempNum++;
        string t = "t" + to_string(tempNum);
        string temp = t + " = " + "cast_to_double " + attr3AC[b].addrName;
        attr3AC[b].threeAC.push_back(temp);
        attr3AC[b].addrName = t;
        attr3AC[b].type = "double";
        typeOfNode[t] = "double";
        //call table update
    }
    return;
}

void pushLabelUp(int par,int chld){
    if(trueLabel.find(chld)!=trueLabel.end())trueLabel[par]=trueLabel[chld];
    if(falseLabel.find(chld)!=falseLabel.end())falseLabel[par]=falseLabel[chld];
    if(nextLabel.find(chld)!=nextLabel.end())nextLabel[par]=nextLabel[chld];
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
    typeSize["boolean"]=1;
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

    // cout<<"inside getarrayinfo\n";

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

void checkRedeclaration(int n, string x){
    auto data = scopeAndTable[n];
    auto scope = data.first;
    auto table = data.second;
    auto partable = parentTable[table];
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,table);

        if (rowPtr!=NULL){

            // got the row
            cout<<"[Compilation Error]: Variable Redeclaration on line "<<lineNum[n]<<"\nVariable '"<<  x << "' !\nAborting...\n";
            exit(0);
        }

    scope=mapParentScope[scope];
    }
    scope=data.first;
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,partable);

        if (rowPtr!=NULL){

            // got the row
            cout<<"[Compilation Error]: Variable Redeclaration on line "<<lineNum[n]<<"\nVariable '"<<  x << "' !\nAborting...\n";
            exit(0);

        }

    scope=mapParentScope[scope];
    }

    return;
}

void checkIfDeclared(int n,string x){

    auto data = scopeAndTable[n];
    auto scope = data.first;
    auto table = data.second;
    auto partable = parentTable[table];
    int flag=0;
    int useLineNumber = lineNum[n];
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,table);

        if (rowPtr!=NULL){
            if((*rowPtr).line <= useLineNumber)
            flag=1;
        }

    scope=mapParentScope[scope];
    }
    scope=data.first;
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,partable);

        if (rowPtr!=NULL){

            // got the row
            if((*rowPtr).line <= useLineNumber)
            flag=1;

        }

    scope=mapParentScope[scope];
    }
    if(!flag){
        cout<<"[Compilation Error]: Variable Not Declared on line "<<lineNum[n]<<"\nVariable '"<<  x << "' !\nAborting...\n";
        exit(0);
    }
    return;
}

void checkFunctionParameterTypes(int n, vector<int> paramNodeNo){
    // cout << "inside check function " << n << " " << paramNodeNo.size() << endl;
    // for(int i=0;i<paramNodeNo.size();i++)cout << i << " " << nodeType[paramNodeNo[i]] << " " << typeOfNode[to_string(paramNodeNo[i])] << endl;
    auto data = scopeAndTable[n];
    auto scope = data.first;
    auto table = data.second;
    auto partable = parentTable[table];
    string x = nodeType[n];
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,table);

        if (rowPtr!=NULL){
            if((rowPtr->functionParams).size()!=paramNodeNo.size()){
                cout<<"[Compilation Error]: Incompatible function parameters on line "<<lineNum[n]<<"\nFunction'"<<  x << "' takes " <<
                (rowPtr->functionParams).size() <<" parameters, but you passed "<< paramNodeNo.size() << " parameters!\nAborting...\n";
                exit(0);
            }
            for(int i=0;i<(rowPtr->functionParams).size();i++){
                if((rowPtr->functionParams)[i]!=typeOfNode[to_string(paramNodeNo[i])]){
                    if(typeOfNode[to_string(paramNodeNo[i])]=="notfound"){
                        cout<<"[Compilation Error]: Function parameter not declared on line "<<lineNum[n]<<"\nVariable '"<<  nodeType[paramNodeNo[i]] << "' !\nAborting...\n";
                        exit(0);
                    }
                        cout<<"[Compilation Error]: Incompatibe function parameter types on line "<<lineNum[n]<<"\n Parameter No. '"<<  i
                         << "' Expected type: "<< (rowPtr->functionParams)[i] <<" but got type: "<< typeOfNode[to_string(paramNodeNo[i])] <<"!\nAborting...\n";
                        exit(0);
                }
            }
            // got the row
            // cout<<"[Compilation Error]: Variable Redeclaration on line "<<lineNum[n]<<"\nVariable '"<<  x << "' !\nAborting...\n";
            // exit(0);
        }

    scope=mapParentScope[scope];
    }
    scope=data.first;
    while(scope.first>1){

        auto rowPtr=checkInScope(x,scope,partable);

        if (rowPtr!=NULL){
            if((rowPtr->functionParams).size()!=paramNodeNo.size()){
                cout<<"[Compilation Error]: Incompatible function parameters on line "<<lineNum[n]<<"\nFunction'"<<  x << "' takes " <<
                (rowPtr->functionParams).size() <<" parameters, but you passed "<< paramNodeNo.size() << " parameters!\nAborting...\n";
                exit(0);
            }
            for(int i=0;i<(rowPtr->functionParams).size();i++){
                if((rowPtr->functionParams)[i]!=typeOfNode[to_string(paramNodeNo[i])]){
                    if(typeOfNode[to_string(paramNodeNo[i])]=="notfound"){
                        cout<<"[Compilation Error]: Function parameter not declared on line "<<lineNum[n]<<"\nVariable '"<<  nodeType[paramNodeNo[i]] << "' !\nAborting...\n";
                        exit(0);
                    }
                        cout<<"[Compilation Error]: Incompatibe function parameter types on line "<<lineNum[n]<<"\n Parameter No. '"<<  i
                         << "' Expected type: "<< (rowPtr->functionParams)[i] <<" but got type: "<< typeOfNode[to_string(paramNodeNo[i])] <<"!\nAborting...\n";
                        exit(0);
                }
            }

        }

    scope=mapParentScope[scope];
    }

    return;
}

void preOrderTraversal(int nodeNum){

    // cout<<"Visiting: "<<nodeType[nodeNum]<<endl;
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


        // after Identifier before uper in ClassDeclaration
        // ->
        //put the class name in the classBody map
        int cb=adj[nodeNum][adj[nodeNum].size()-1];
        classNameMap[cb]=globRow.name;
        // cout<<lineNum[cb]<<" "<<globRow.name<<endl;
        // cout << "hax hax " << globRow.name << " " << lineNum[cb] << endl;

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
        attrSymTab[nodeNum].funcParams.push_back(nodeType[adj[nodeNum][0]]);

        attrSymTab[nodeNum].name=(nodeType[adj[nodeNum][0]]);
        // cout<<(nodeType[adj[nodeNum][0]])<<"\n";
        attrSymTab[nodeNum].type=(nodeType[adj[nodeNum][0]]);
        // attrSymTab[nodeNum].decLine=lineNum[nodeNum];
        int c1=adj[nodeNum][0];
        scopeAndTable[c1].first=currScope.top();
        // cout<<c1<<" "<<scopeAndTable[c1].first.first<<" "<<scopeAndTable[c1].first.second<<endl;
        scopeAndTable[c1].second=currSymTab;
        attrSymTab[nodeNum].leafNodeNum=c1;
        // cout << "identiifer " << c1 << " " <<  << endl;
        // cout<<"[From Identifier]"<<endl;
        typeOfNode[to_string(c1)]="fillMe";
        // cout<<nodeType[adj[nodeNum][0]]<<endl;
        if (attrSymTab[nodeNum].name=="true" || attrSymTab[nodeNum].name=="false"){
            typeOfNode[to_string(c1)]=attrSymTab[nodeNum].name;
            // cout<<to_string(c1)<<" "<<attrSymTab[nodeNum].name<<endl;
        }
        if(functionParameterMap.size()!=0 && functionParameterMap.find(attrSymTab[nodeNum].name)!=functionParameterMap.end()){
            typeOfNode[to_string(c1)] = functionParameterMap[attrSymTab[nodeNum].name];
            // cout << "assign map " << nodeType[adj[nodeNum][0]] << " " << c1 << " " << typeOfNode[to_string(c1)] << endl;
        }
        // cout << "exit identifier " << typeOfNode[to_string(c1)] << endl;
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
        // cout<<"from Type: "<<attrSymTab[nodeNum].type<<endl;
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
        
        // cout<<nodeType[nodeNum]<<" from "<<attrSymTab[nodeNum].type<<endl;
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
            attrSymTab[nodeNum].name=attrSymTab[c1].name;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
            // cout<<"In vardecls: "<<attrSymTab[nodeNum].leafNodeNum<<endl;
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
        attrSymTab[nodeNum].leafNodeNum = attrSymTab[c1].leafNodeNum;
        if (prodNum[nodeNum]==2){
            int c3=adj[nodeNum][2];
            attrSymTab[nodeNum].type=attrSymTab[c3].type;
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][2]].leafNodeNum;
            initVal[attrSymTab[c1].name]=atoi(nodeType[attrSymTab[nodeNum].leafNodeNum].c_str());
            // cout<<"In vardec: "<< attrSymTab[nodeNum].leafNodeNum<<endl;
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
        attrSymTab[nodeNum].leafNodeNum = attrSymTab[c1].leafNodeNum;
        attrSymTab[nodeNum].funcParams.push_back(attrSymTab[c1].funcParams[0]);
        checkRedeclaration(attrSymTab[c1].leafNodeNum,attrSymTab[c1].name);
        attrSymTab[nodeNum].funcParams.push_back(attrSymTab[c1].funcParams[0]);
        
        // cout << "in vdid " << attrSymTab[nodeNum].leafNodeNum << " " << attrSymTab[nodeNum].name << endl;
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
            // functionParameterMap.clear();
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
            int i=0;
            for (auto paramType:attrSymTab[c3].otherParams){
                attrSymTab[nodeNum].otherParams.push_back(paramType);
                localTableParams locRow ;
            
                locRow.type=paramType;

                locRow.name=attrSymTab[c3].funcParams[i];
                locRow.line=lineNum[nodeNum];
                locRow.scope=currScope.top();
                locRow.parentScope=parentScope.top();
                // cout<<attrSymTab[cb2].intParams.size()<<endl;

                // cout<<"From FieldDeclaration:"<<locRow.arraySize.size()<<endl;
                (*currSymTab).push_back(locRow);
                i++;
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
            attrSymTab[nodeNum].funcParams.push_back(attrSymTab[c1].funcParams[0]);
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

            for (auto fp:attrSymTab[c1].funcParams){
                attrSymTab[nodeNum].funcParams.push_back(fp);

            }
            
            attrSymTab[nodeNum].funcParams.push_back(attrSymTab[c3].funcParams[0]);

        }
        default:
            break;
        }
        return;
    }
    else if (nodeType[nodeNum]=="FormalParameter"){
        // only need the Type
        int c1=adj[nodeNum][0];
        int c2 = adj[nodeNum][1];
        preOrderTraversal(c1);
        preOrderTraversal(c2);
        attrSymTab[nodeNum].funcParams.push_back(attrSymTab[c2].funcParams[0]);
        attrSymTab[nodeNum].type=attrSymTab[c1].type ;
        attrSymTab[nodeNum].leafNodeNum = attrSymTab[c2].leafNodeNum;
        // cout <<"inside formal parameter " << attrSymTab[c2].leafNodeNum << " " << attrSymTab[c2].name  << " " << attrSymTab[c1].type << endl;
        typeOfNode[to_string(attrSymTab[c2].leafNodeNum)] = attrSymTab[c1].type;
        // cout << "typeOfNode after " << typeOfNode[to_string(attrSymTab[c2].leafNodeNum)] << endl;
        functionParameterMap[attrSymTab[c2].name] = attrSymTab[nodeNum].type;
        // cout << functionParameterMap[attrSymTab[c2].name] << " " << attrSymTab[nodeNum].type << endl;
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
        // cout<<"In LocalVariableDeclaration\n";
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
            // cout<<attrSymTab[c2].intParams.size()<<endl;
            locRow.arraySize=attrSymTab[c2].intParams;
            if(locRow.type=="void"){
                cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nVariable '"<<  locRow.name << "' cannot have a type 'void'!\nAborting...\n";
                exit(0);
            }
            
            // cout<<"From FieldDeclaration:"<<locRow.arraySize.size()<<endl;
            (*currSymTab).push_back(locRow);
            // cout<<currSymTab<<" in locvaldec "<<(*currSymTab).size()<<"\n";
            
            // for (auto ro:*(currSymTab))
            //     cout<<ro.name<<" "<<ro.arraySize.size()<<endl;
            return;

        }
        else{

            if (initVal.find(attrSymTab[c2].name)!=initVal.end()){
                
                // var is initialized as well

                int c1=adj[nodeNum][0];
                int c2=adj[nodeNum][1];
                initVal[attrSymTab[c2].name]=atoi(nodeType[attrSymTab[c2].leafNodeNum].c_str());          
                // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
                
                string t3=fillHelper(to_string(attrSymTab[c2].leafNodeNum));
                
                // cout<<"[In LocVarDec]: "<<" "<<t3<<endl;
                string t1=attrSymTab[c1].type;
                t3=typeOfNode[to_string(attrSymTab[c2].leafNodeNum)];

                if (t1[t1.size()-1]>'0' && t1[t1.size()-1]<'9'){
                            t1=removeLastChar(t1);
                }

                if (t3[t3.size()-1]>'0' && t3[t3.size()-1]<'9'){
                            t3=removeLastChar(t3);
                            
                        }
                // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c2].leafNodeNum)]<<endl;                     

                if (checkIfTypeOkay(t1,t3))
                        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
                else{
                  
                    string var1=nodeType[attrSymTab[c1].leafNodeNum];
                    string var2=nodeType[attrSymTab[c2].leafNodeNum];

                    // cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<t1<<"' does not match type '"<<t3<<"' of '"<<var2<<"'!\nAborting...\n";
                    // exit(0);
                }
            }

            // attrSymTab[nodeNum].type=attrSymTab[c1].type;
            // attrSymTab[nodeNum].otherParams=attrSymTab[c1].otherParams;
            // cout<<attrSymTab[c2].otherParams.size()<<endl;
            for (auto varName:attrSymTab[c2].otherParams){

                localTableParams locRow ;
                locRow.type=attrSymTab[c1].type;

                // cout<<locRow.type<<" From BlockStatement\n";
                locRow.name=varName;
                locRow.line=lineNum[nodeNum];
                locRow.scope=currScope.top();
                locRow.parentScope=parentScope.top();
                if(locRow.type=="void"){
                cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nVariable '"<<  locRow.name << "' cannot have a type 'void'!\nAborting...\n";
                exit(0);
            }
                // cout<<"From BlockStatement: "<<varName<<endl; //works
                // cout<<locRow.type<<" "<<locRow.name<<endl; //works
                // checkRedeclaration(attrSymTab[c2].leafNodeNum,locRow.name);
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

        // for (auto var:attrSymTab[c1].otherParams){
        //     localTableParams locRow ;
        //     locRow.name=var;
        //     locRow.type=attrSymTab[c1].type;
        //     locRow.scope=currScope.top();
        //     locRow.parentScope=parentScope.top();
        //     locRow.line=lineNum[nodeNum];
        //     currSymTab->push_back(locRow);

        // }
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
        // for (auto siz:attrSymTab[nodeNum].intParams)
        //     cout<<siz<<" ";
        // cout<<endl;
        // cout<< attrSymTab[nodeNum].type<<endl;

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

        // for (auto siz:attrSymTab[nodeNum].intParams)
        //     cout<<siz<<" ";
        // cout<<endl;
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
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
        
        return;
    }
    else if (nodeType[nodeNum]=="UnaryExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==5){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num;  
            // cout<<"From Uexp: "<< attrSymTab[nodeNum].num<<endl;
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
            // cout<<"From Uexp: ";
            // for (auto siz:attrSymTab[nodeNum].intParams)
            //     cout<<siz<<" ";
            // cout<< attrSymTab[nodeNum].type<<endl;
            // cout<<endl;
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        }

        switch (prodNum[nodeNum])
        {
        case 1:
        case 2:{
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
            break;
        }
        case 3:
        case 4:{
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][1]].leafNodeNum;
            break;
        }
        default:
            break;
        }
        return;
    }
    else if (nodeType[nodeNum]=="PostIncrementExpression"|| nodeType[nodeNum]=="PostDecrementExpression"){
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
    }
    else if (nodeType[nodeNum]=="PostfixExpression" ){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        if (prodNum[nodeNum]==1){
            
            attrSymTab[nodeNum].num=attrSymTab[c1].num;   
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
            // for (auto siz:attrSymTab[nodeNum].intParams)
            //     cout<<siz<<" ";
            
            // cout<<endl;
        }
        else if (prodNum[nodeNum]==2){

            attrSymTab[nodeNum].num=initVal[attrSymTab[c1].name];
        }
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        return;
    }
    else if (nodeType[nodeNum]=="Primary"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);

        if (prodNum[nodeNum]==1){
            attrSymTab[nodeNum].num=attrSymTab[c1].num;    
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
            typeOfNode[to_string(attrSymTab[nodeNum].leafNodeNum)]=fillHelper(to_string(attrSymTab[nodeNum].leafNodeNum));
            // cout<<"From Primary: "<<nodeType[attrSymTab[nodeNum].leafNodeNum]<<" "<<attrSymTab[nodeNum].leafNodeNum<<endl;
            // cout<<typeOfNode[to_string(attrSymTab[nodeNum].leafNodeNum)]<<endl;
            // cout<<"From Primary: "<<attrSymTab[nodeNum].num<<endl;
        }
        else{

            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
            // cout<<"From Primary: "<<attrSymTab[nodeNum].intParams.size()<<endl;
        }
        return;
    }
    else if (nodeType[nodeNum]=="AdditiveExpression"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==1){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            // cout<<"FRom Additive Expression:"<<attrSymTab[nodeNum].num<<endl;
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;
            // cout<<"From  AddExpre"<<   attrSymTab[nodeNum].intParams.size()<<endl;
            // cout<<attrSymTab[nodeNum].type<<"\n";
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        }
        else {

            int c1=adj[nodeNum][0];
            int c3=adj[nodeNum][2];
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            // cout<<t1<<" "<<t3<<endl;
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            string t1=fillHelper(to_string(attrSymTab[c1].leafNodeNum));
            string t3=fillHelper(to_string(attrSymTab[c3].leafNodeNum));

            // cout<<t1<<" from additive expression "<<t3<<endl;
            t1=typeOfNode[to_string(attrSymTab[c1].leafNodeNum)];
            t3=typeOfNode[to_string(attrSymTab[c3].leafNodeNum)];

            if (t1[t1.size()-1]>'0' && t1[t1.size()-1]<'9'){
                    t1=removeLastChar(t1);
                }

            if (t3[t3.size()-1]>'0' && t3[t3.size()-1]<'9'){
                    t3=removeLastChar(t3);
                    
                }
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            
            if (checkIfTypeOkay(t1,t3))
                attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
            else{

                string var1=nodeType[attrSymTab[c1].leafNodeNum];
                string var2=nodeType[attrSymTab[c3].leafNodeNum];
                if(t1=="notfound"){
                    cout<<"[Compilation Error]: Variable Not Declared on line "<<lineNum[nodeNum]<<"\nVariable '"<<  var1 << "' !\nAborting...\n";
                    exit(0);
                }
                if(t3=="notfound"){
                    cout<<"[Compilation Error]: Variable Not Declared on line "<<lineNum[nodeNum]<<"\nVariable '"<<  var2 << "' !\nAborting...\n";
                    exit(0);
                }
                cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<t1<<"' of '"<<var1<<"' does not match type '"<<t3<<"' of '"<<var2<<"'!\nAborting...\n";
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
        // cout<<"[In Assignment]: "<<t1<<" "<<t3<<endl;
        t1=typeOfNode[to_string(attrSymTab[c1].leafNodeNum)];
        t3=typeOfNode[to_string(attrSymTab[c3].leafNodeNum)];

        if (t1[t1.size()-1]>'0' && t1[t1.size()-1]<'9'){
                    t1=removeLastChar(t1);
                }

        if (t3[t3.size()-1]>'0' && t3[t3.size()-1]<'9'){
                    t3=removeLastChar(t3);
                    
                }
            // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
            
        if (checkIfTypeOkay(t1,t3))
                attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
        else{

            string var1=nodeType[attrSymTab[c1].leafNodeNum];
            string var2=nodeType[attrSymTab[c3].leafNodeNum];

            // cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<t1<<"' of '"<<var1<<"' does not match type '"<<t3<<"' of '"<<var2<<"'!\nAborting...\n";
            // exit(0);
        }

    }
    else if (nodeType[nodeNum]=="LeftHandSide"){

        preOrderTraversal(adj[nodeNum][0]);

        attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        
        return;
    }
    else if (nodeType[nodeNum]=="MultiplicativeExpression"){
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);
        
        switch (prodNum[nodeNum])
        {   
            case 1:{
                attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
                attrSymTab[nodeNum].num=attrSymTab[adj[nodeNum][0]].num;
                attrSymTab[nodeNum].intParams=attrSymTab[adj[nodeNum][0]].intParams;
                attrSymTab[nodeNum].type=attrSymTab[adj[nodeNum][0]].type;

                break;
            }
            case 2:
            case 3:
            case 4:{
                int c1=adj[nodeNum][0];
                int c3=adj[nodeNum][2];

                string t1=fillHelper(to_string(attrSymTab[c1].leafNodeNum));
                string t3=fillHelper(to_string(attrSymTab[c3].leafNodeNum));
                    
                    // cout<<typeOfNode[to_string(attrSymTab[c1].leafNodeNum)]<<" "<<typeOfNode[to_string(attrSymTab[c3].leafNodeNum)]<<endl;
                t1=typeOfNode[to_string(attrSymTab[c1].leafNodeNum)];
                t3=typeOfNode[to_string(attrSymTab[c3].leafNodeNum)];

                if (t1[t1.size()-1]>'0' && t1[t1.size()-1]<'9'){
                    t1=removeLastChar(t1);
                }

                if (t3[t3.size()-1]>'0' && t3[t3.size()-1]<'9'){
                    t3=removeLastChar(t3);
                    
                }   
                if (checkIfTypeOkay(t1,t3))
                        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
                else{

                    string var1=nodeType[attrSymTab[c1].leafNodeNum];
                    string var2=nodeType[attrSymTab[c3].leafNodeNum];
                    if(t1=="notfound"){
                        cout<<"[Compilation Error]: Variable Not Declared on line "<<lineNum[nodeNum]<<"\nVariable '"<<  var1 << "' !\nAborting...\n";
                        exit(0);
                    }
                    if(t3=="notfound"){
                        cout<<"[Compilation Error]: Variable Not Declared on line "<<lineNum[nodeNum]<<"\nVariable '"<<  var2 << "' !\nAborting...\n";
                        exit(0);
                    }
                    cout<<"[Compilation Error]: Type mismatch on line "<<lineNum[nodeNum]<<"\nType '"<<t1<<"' of '"<<var1<<"' does not match type '"<<t3<<"' of '"<<var2<<"'!\nAborting...\n";
                    exit(0);
                }

                break;
            }
            default: 
                break;
        }
    }
    else if (nodeType[nodeNum]=="CastExpression"){

        for (auto child:adj[nodeNum])
            preOrderTraversal(child);
        
        switch (prodNum[nodeNum])
        {
        case 1:{
            int c2=adj[nodeNum][1];
            typeOfNode[to_string(nodeNum)]=attrSymTab[c2].type;
            attrSymTab[nodeNum].leafNodeNum=nodeNum;
            break;
        }
        case 2:{
            int c4=adj[nodeNum][3];
            typeOfNode[to_string(nodeNum)]=attrSymTab[c4].type;
            attrSymTab[nodeNum].leafNodeNum=nodeNum;
            break;
        }
        case 3:{
            int c2=adj[nodeNum][1];
            typeOfNode[to_string(nodeNum)]=attrSymTab[c2].type;
            attrSymTab[nodeNum].leafNodeNum=nodeNum;
            break;
        }
        default:
            break;
        }

    }
    else if (nodeType[nodeNum]=="UnaryExpressionNotPlusMinus"){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);

        if (prodNum[nodeNum]==1){
            
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
        }

        switch (prodNum[nodeNum])
        {
        case 1:{
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
            break;
        }
        case 2:
        case 3:{
            int c2=adj[nodeNum][1];
            // attrSymTab[nodeNum].num=attrSymTab[c2].num; 
            attrSymTab[nodeNum].type=attrSymTab[c2].type;
            // attrSymTab[nodeNum].intParams=attrSymTab[c2].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[c2].leafNodeNum;
            break;
        }
        case 4:{
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;

        }
        default:
            break;
        }

    }
    else if (nodeType[nodeNum]=="ArrayAccess"){
        
        int c1=adj[nodeNum][0];
        int c3=adj[nodeNum][2];
        preOrderTraversal(c1);
        preOrderTraversal(c3);
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c1].leafNodeNum;
    }
    else if (nodeType[nodeNum]=="AssignmentExpression"){
        
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            
            // cout<<nodeType[nodeNum]<<" "<<attrSymTab[nodeNum].num<<endl;
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            // cout<<"From ExclusiveOrExpression: "<<attrSymTab[c1].intParams.size()<<endl;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum; 
            // cout<<"assexrp: "<<attrSymTab[nodeNum].leafNodeNum<<endl;
    }
    else if (nodeType[nodeNum]=="PrimaryNoNewArray" || nodeType[nodeNum]=="ShiftExpression" || nodeType[nodeNum]=="RelationalExpression" || nodeType[nodeNum]=="EqualityExpression" || nodeType[nodeNum]=="AndExpression" || nodeType[nodeNum]=="ExclusiveOrExpression" || nodeType[nodeNum]=="InclusiveOrExpression"|| nodeType[nodeNum]=="ConditionalAndExpression" || nodeType[nodeNum]=="ConditionalOrExpression" || nodeType[nodeNum]=="ConditionalExpression" ){
        
        for (auto child:adj[nodeNum])
            preOrderTraversal(child);


        switch (prodNum[nodeNum])
        {
        case 1:{
            int c1=adj[nodeNum][0];
            attrSymTab[nodeNum].num=attrSymTab[c1].num; 
            
            // cout<<nodeType[nodeNum]<<" "<<attrSymTab[nodeNum].num<<endl;
            attrSymTab[nodeNum].type=attrSymTab[c1].type;
            // cout<<"From ExclusiveOrExpression: "<<attrSymTab[c1].intParams.size()<<endl;
            attrSymTab[nodeNum].intParams=attrSymTab[c1].intParams;  
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum; 
            if (nodeType[nodeNum]=="InclusiveOrExpression"){
                // cout<<"From IncOrEx:";
                // for (auto siz:attrSymTab[nodeNum].intParams)
                //     cout<<siz<<" ";
                // cout<<endl;
                // cout<<attrSymTab[nodeNum].type<<endl;
            }

            break;
        }
        case 3:{
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][1]].leafNodeNum; 
            // cout<<attrSymTab[nodeNum].leafNodeNum<<": from PNNA"<<endl;
            
            break;
        }
        case 6:{
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum; 

            // cout<<attrSymTab[nodeNum].leafNodeNum<<endl;
            break;
        }
        default:
            break;
        }
        return;
    }
    else if (nodeType[nodeNum]=="Literal"){
        int c1=adj[nodeNum][0];
        preOrderTraversal(c1);
        attrSymTab[nodeNum].num=attrSymTab[c1].num;

        scopeAndTable[c1].first=currScope.top();
        // cout<<c1<<" "<<scopeAndTable[c1].first.first<<" "<<scopeAndTable[c1].first.second<<endl;
        scopeAndTable[c1].second=currSymTab;
        attrSymTab[nodeNum].leafNodeNum=adj[c1][0];

        switch (prodNum[nodeNum])
        {
        case 1:{
            typeOfNode[to_string(adj[c1][0])]="int";
            break;
        }
        case 2:
            typeOfNode[to_string(adj[c1][0])]="float";
            break;
        case 3:
            typeOfNode[to_string(adj[c1][0])]="double";
            break;
        case 4:
            typeOfNode[to_string(adj[c1][0])]="boolean";
            // cout<<"Hi\n";
            // cout<<to_string(adj[c1][0])<<endl;
            break;
        case 5:
            typeOfNode[to_string(adj[c1][0])]="char";
            break;
        case 6:
            typeOfNode[to_string(adj[c1][0])]="String";
            break;
        case 7:
            typeOfNode[to_string(adj[c1][0])]="null";
            break;
        default:
            break;
        }
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
            attrSymTab[nodeNum].leafNodeNum=attrSymTab[adj[nodeNum][0]].leafNodeNum;
            // cout<<"[PreOrderTraversal] From VariableInitializer: "<<attrSymTab[nodeNum].leafNodeNum<<endl;
            // for (auto siz:attrSymTab[nodeNum].intParams)
            //     cout<<siz<<" ";
            // cout<<endl;

        }

    }
    else if (nodeType[nodeNum]=="PreIncrementExpression"){
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c2].leafNodeNum;
        
    }
    else if (nodeType[nodeNum]=="PreDecrementExpression"){
        int c2=adj[nodeNum][1];
        preOrderTraversal(c2);
        
        attrSymTab[nodeNum].leafNodeNum=attrSymTab[c2].leafNodeNum;
    
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

            cout<<"Name,Type,Array Size,Return Type,Function Param Types,Scope,Parent Scope,Offset,Line Number,Function Table Pointer\n";
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
                if ((*(row.localTablePointer))[j].type=="method")
                    cout<<",-";
                else{
                    cout<<","<<(*(row.localTablePointer))[j].offset;
                }

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

                        cout<<"Name,Type,Array Size,Scope,Parent Scope,Offset,Line Number\n";

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
                            cout<<fRow.offset<<",";
                            cout<<fRow.line<<"\n";
                        }

                        std::fclose(stdout);

                    }
		    }

        }
    }

    // freopen("log.log","w",stdout);

    return;
}

void execCastExpression(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c2 = adj[nodeNum][1];
            int c4 = adj[nodeNum][3];
            attr3AC[nodeNum] = attr3AC[c4];
            attr3AC[nodeNum].type = attr3AC[c2].type;
        }
        break;
    }
    return;
}

void execIntegralType(int nodeNum){
    attr3AC[nodeNum].type = nodeType[adj[nodeNum][0]];
    return;
}

void execWhileStatementNoShortIf(int nodeNum){
    string beg = getLabel(-1,0);
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    // cout << "whilestatementnoshortif " << attr3AC[c3].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    // cout << "B true: " << c3true << endl;
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
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            // pushLabelUp(nodeNum,c);
        }
        break;
    }
    return;
}

void execVariableDeclaratorId(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    pushLabelUp(nodeNum,c);
    return;
}

void execVariableDeclarator(int nodeNum){
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
            string tp = getTypeNode(c);
            // cout << "in vardec " << tp << endl;
            if(tp[tp.size()-1]==';'){
                string temp = attr3AC[c].addrName + " = popparam";
                attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
                attr3AC[nodeNum].threeAC.push_back(temp);
            }else{
                string temp = attr3AC[c].addrName + " = " + attr3AC[c3].addrName;
                attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
                // cout << "variabledeclarator " << temp << endl;
                typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
                attr3AC[nodeNum].threeAC.push_back(temp);
                // cout << "variabledeclarator me " << attr3AC[c3].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
                attr3AC[nodeNum].addrName = attr3AC[c].addrName;
            }
        }
        break;
    }
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
    // cout << "switch statemnt karna he" << endl;
    int c3 = adj[nodeNum][2];
    int c5 = adj[nodeNum][4];
    attr3AC[nodeNum] = attr3AC[c3] + attr3AC[c5];
    return;
}

void execSwitchLabels(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            // cout << "label switch labels " << getLabel(nodeNum,1) << " " << getLabel(nodeNum,2) << " " << getLabel(c,1) << " " << getLabel(c,2) << endl;
            // for(int i=0;i<attr3AC[c].threeAC.size();i++){
            //     cout << attr3AC[c].threeAC[i] << endl;
            // }
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c1 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c1];
        }
        break;
    }
    return;
}

void execSwitchLabel(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            string c2true = getLabel(c2,1);
            string c2false =getLabel(c2,2);
            string temp = "if " + switchExpAddr + " == " + attr3AC[c2].addrName + " goto " + c2true;
            // cout << "sdfadfsadf as temp " << temp << endl;
            attr3AC[nodeNum].threeAC.push_back(temp);
            temp = "goto " + c2false;
            attr3AC[nodeNum].threeAC.push_back(temp);
            pushLabelUp(nodeNum,c2);
            // cout << "over here  " << c2true << " " << c2false << endl;
        }
        break;
        case 2:{
            attr3AC[nodeNum].addrName = "default";
        }
        break;
    }
    return;
}

void execSwitchBlockStatementGroups(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
            pushLabelUp(nodeNum,c);
            // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
            //     cout << "asdfadsfasdfasdfasf1 " << attr3AC[nodeNum].threeAC[i] << endl;
            // }
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c] + attr3AC[c2];
            // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
            //     cout << "asdfadsfasdfasdfasf2 " << attr3AC[nodeNum].threeAC[i] << endl;
            // }
        }
        break;
    }
    return;
}

void execSwitchBlockStatementGroup(int nodeNum){
    int c = adj[nodeNum][0];
    int c2 = adj[nodeNum][1];
    string ctrue = getLabel(c,1);
    string cfalse = getLabel(c,2);
    string temp = ctrue + ":";
    attr3AC[nodeNum] = attr3AC[c];
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c2];
    temp = cfalse + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
    //     cout << "asdfasdf " << attr3AC[nodeNum].threeAC[i] << endl;
    // }
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
            // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
            //     cout << "asdfadsfasdfasdfasf " << attr3AC[nodeNum].threeAC[i] << endl;
            // }
        }
        break;
        case 3:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
            pushLabelUp(nodeNum,c2);
            // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
            //     cout << "asdfadsfasdfasdfasf " << attr3AC[nodeNum].threeAC[i] << endl;
            // }
        }
        break;
        case 4:{
            int c2 = adj[nodeNum][1];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c2] + attr3AC[c3];
            // for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
            //     cout << "asdfadsfasdfasdfasf " << attr3AC[nodeNum].threeAC[i] << endl;
            // }
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
    attr3AC[nodeNum].nodeno = c;
    typeOfNode[to_string(c)] = "string";
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
    int c = adj[nodeNum][1];
    attr3AC[nodeNum] = attr3AC[c];
    return;
}

void execPrimitiveType(int nodeNum){
    attr3AC[nodeNum] = attr3AC[adj[nodeNum][0]];
    // cout << "primtive " << attr3AC[nodeNum].type << endl;
    return;
}

void execPreIncrementExpression(int nodeNum){
    int c = adj[nodeNum][1];
    tempNum++;
    attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
    typeOfNode[attr3AC[nodeNum].addrName] = "int";
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
    typeOfNode[attr3AC[nodeNum].addrName]="int";
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
    typeOfNode[attr3AC[nodeNum].addrName] = "int";
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
    typeOfNode[attr3AC[nodeNum].addrName] = "int";
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
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    for(int i=0;i<attr3AC[nodeNum].params.size();i++){
        // cout << "methodheader____ " << attr3AC[nodeNum].params[i] << " " << attr3AC[nodeNum].paramsNodeNo[i] << endl;
    }
    for(int i=attr3AC[nodeNum].params.size()-1;i>=0;i--){
        tempNum++;
        string temp = "t" + to_string(tempNum);
        funcParamTemp[attr3AC[nodeNum].params[i]]=temp;
        string temp2 = temp + " = popparam";
        attr3AC[nodeNum].threeAC.push_back(temp2);
        // cout << "in lop " << i << " " << temp2 << endl;
    }
    attr3AC[nodeNum].params.clear();
    attr3AC[nodeNum].paramsNodeNo.clear();
    return;
}

void execMethodHeader(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c3];
        }
        break;
        case 2:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
        case 3:{
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c3];
        }
        break;
        case 4:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
        case 5:{
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c3];
        }
        break;
        case 6:{
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c3];
        }
        break;
        case 7:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
        case 8:{
            int c2 = adj[nodeNum][1];
            attr3AC[nodeNum] = attr3AC[c2];
        }
        break;
    }
    return;
}

void execMethodDeclarator(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            for(int i=0;i<attr3AC[c3].params.size();i++){
                attr3AC[nodeNum].params.push_back(attr3AC[c3].params[i]);
                attr3AC[nodeNum].paramsNodeNo.push_back(attr3AC[c3].paramsNodeNo[i]);
            }
            // for(int i=0;i<attr3AC[nodeNum].params.size();i++){
            //     cout << "methoddeclarator " << attr3AC[nodeNum].params[i] << " " << attr3AC[nodeNum].paramsNodeNo[i] << endl;
            // }
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum] = attr3AC[c];
        }
        break;
    }
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
    attr3AC[nodeNum].nodeno = c;
    typeOfNode[to_string(c)]="int";
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
    // cout << "ifthen else " << attr3AC[c3].threeAC.size() << " " << attr3AC[c5].threeAC.size() << " " << attr3AC[c7].threeAC.size() << endl;
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 6:{
            int c4 = adj[nodeNum][3];
            int c7 = adj[nodeNum][6];
            string beg = getLabel(-1,0);
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            nextLabel[c7]= getLabelNumber(beg);
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            nextLabel[c7]= getLabelNumber(beg);
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c7];
            temp = "goto " + beg;
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 8:{
            int c6 = adj[nodeNum][5];
            string beg = getLabel(-1,0);
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            nextLabel[c6]= getLabelNumber(beg);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
    int c = adj[nodeNum][1];
    attr3AC[nodeNum].params.push_back(attr3AC[c].addrName);
    attr3AC[nodeNum].paramsNodeNo.push_back(attr3AC[c].nodeno);
    // cout << "formal parameter " << attr3AC[nodeNum].params[0] << " " << attr3AC[nodeNum].paramsNodeNo[0] << endl;
    return;
}

void execFormalParameterList(int nodeNum){
    switch(prodNum[nodeNum]){
        case 1:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum]=attr3AC[c];
            for(int i=0;i<attr3AC[nodeNum].params.size();i++){
                // cout << "formalparamterlist1 " << attr3AC[nodeNum].params[i] << " " << attr3AC[nodeNum].paramsNodeNo[i] << endl;
            }
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum].params.push_back(attr3AC[c3].params[0]);
            attr3AC[nodeNum].paramsNodeNo.push_back(attr3AC[c3].paramsNodeNo[0]);
            for(int i=0;i<attr3AC[nodeNum].params.size();i++){
                // cout << "formalparamterlist " << attr3AC[nodeNum].params[i] << " " << attr3AC[nodeNum].paramsNodeNo[i] << endl;
            }
        }
        break;
    }
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
            attr3AC[nodeNum].nodeno = c;
            typeOfNode[to_string(c)] = "float";
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            attr3AC[nodeNum].type = nodeType[c]; 
            attr3AC[nodeNum].nodeno = c;
            typeOfNode[to_string(c)] = "double";
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
    attr3AC[nodeNum].nodeno = c;
    typeOfNode[to_string(c)] = "float";
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
    attr3AC[nodeNum].nodeno = c;
    typeOfNode[to_string(c)] = "double";
    return;
}

void execDoStatement(int nodeNum){
    // string beg = getLabel(-1,0);
    int c2 = adj[nodeNum][1];
    int c5 = adj[nodeNum][4];
    string c5true = getLabel(c5,1);
    string c5false = getLabel(c5,2);
    // nextLabel[c2] = getLabelNumber(beg);
    string temp = c5true + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c2];
    attr3AC[nodeNum].threeAC.push_back(doContinueLabel + ":");
    attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
    temp = c5false + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    return;
}

// string beg = getLabel(-1,0);
//     int c3 = adj[nodeNum][2];
//     int c5 = adj[nodeNum][4];
//     // cout << "while " << attr3AC[c3].threeAC.size() << endl;
//     string c3true = getLabel(c3,1);
//     // cout << "B true: " << c3true << endl;
//     nextLabel[c5]= getLabelNumber(beg);
//     string temp = beg + ":";
//     attr3AC[nodeNum].threeAC.push_back(temp);
//     attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
//     temp = c3true + ":";
//     attr3AC[nodeNum].threeAC.push_back(temp);
//     attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c5];
//     temp = "goto " + beg;
//     attr3AC[nodeNum].threeAC.push_back(temp);

//     falseLabel[c3] = nextLabel[nodeNum];

void execContinueStatement(int nodeNum){
    if(isFor){
        attr3AC[nodeNum].threeAC.push_back("goto " + forContinueLabel);
    }else if(isWhile){
        attr3AC[nodeNum].threeAC.push_back("goto " + whileContinueLabel);
    }else if(isDo){
        attr3AC[nodeNum].threeAC.push_back("goto " + doContinueLabel);
    }else{
        cout << "ERROR: Continue Statement must occur within a loop." << endl;
    }
    return;
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
            attr3AC[nodeNum].addrName = nodeType[c]; 
            attr3AC[nodeNum].nodeno = c;
            typeOfNode[to_string(c)] = "char";
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
    if(isFor){
        string temp = "goto " + forBreakLabel;
        attr3AC[nodeNum].threeAC.push_back(temp);
    }else if(isWhile){
        string temp = "goto " + whileBreakLabel;
        attr3AC[nodeNum].threeAC.push_back(temp);
    }else if(isDo){
        string temp = "goto " + doBreakLabel;
        attr3AC[nodeNum].threeAC.push_back(temp);
    }
    return;
}

void execArrayType(int nodeNum){
    return;
}

void execBooleanLiteral(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum].type = nodeType[c]; 
    attr3AC[nodeNum].nodeno = c;
    typeOfNode[to_string(c)] = "boolean";
    // string ptrue = getLabel(nodeNum,1);
    // string pfalse = getLabel(nodeNum,2);
    // if(nodeType[nodeNum]=="true"){
    //     string temp = "goto " + ptrue;
    //     attr3AC[nodeNum].threeAC.push_back(temp);
    // }else{
    //     string temp = "goto " + pfalse;
    //     attr3AC[nodeNum].threeAC.push_back(temp);
    // }
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
            // cout << "inside method declaration " << insideClassName << endl;
            string temp = nodeType[attr3AC[c].nodeno] + insideClassName +":";
            attr3AC[nodeNum].threeAC.push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c];
            attr3AC[nodeNum] =  attr3AC[nodeNum] + attr3AC[c2];
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
            // cout << "statementNoshort if " << attr3AC[nodeNum].threeAC.size() << endl;;
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
    // cout << "ifthen else " << attr3AC[c3].threeAC.size() << " " << attr3AC[c5].threeAC.size() << " " << attr3AC[c7].threeAC.size() << endl;
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            string c4true = getLabel(c4,1);
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            attr3AC[nodeNum] = attr3AC[c3];
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
            // attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
            string temp = beg + ":";
            attr3AC[nodeNum].threeAC.push_back(forContinueLabel + ":");
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
    // cout << "while " << attr3AC[c3].threeAC.size() << endl;
    string c3true = getLabel(c3,1);
    // cout << "B true: " << c3true << endl;
    nextLabel[c5]= getLabelNumber(beg);
    string temp = beg + ":";
    attr3AC[nodeNum].threeAC.push_back(temp);
    attr3AC[nodeNum].threeAC.push_back(whileContinueLabel + ":");
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
            // cout << "swts " << attr3AC[nodeNum].threeAC.size() << endl;
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
            string snext = getLabel(c,3);
            string temp = snext + ":";
            attr3AC[nodeNum].threeAC.push_back(temp);
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
            int c2 = adj[nodeNum][1];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum]=attr3AC[c2]+attr3AC[c3];
            int totsize = typeSize[attr3AC[c2].type];
            for(int i=0;i<attr3AC[c3].arrDims.size();i++){
                totsize*=attr3AC[c3].arrDims[i];
            }
            tempNum++;
            string temp = "t" + to_string(tempNum);
            string temp2 = temp + " = " + to_string(totsize);
            attr3AC[nodeNum].threeAC.push_back(temp2);
            temp2 = "pushparam " + temp;
            attr3AC[nodeNum].threeAC.push_back(temp2);
            temp2 = "stackpointer + 4";
            attr3AC[nodeNum].threeAC.push_back(temp2);
            temp2 = "call allocmem 1";
            attr3AC[nodeNum].threeAC.push_back(temp2);
            temp2 = "stackpointer - 4";
            attr3AC[nodeNum].threeAC.push_back(temp2);
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
            auto mdata= getArrayInfo(attr3AC[c].addrName,attr3AC[c].nodeno);
            // auto mdata = getArrayInfo(nodeType[adj[attr3AC[c].nodeno][0]],attr3AC[c].nodeno);// lowestnode
            // cout << "in array access wefasdfsf " << attr3AC[c].arrDims.size() << " " << attr3AC[c].dimsDone << endl;

            // cout << "over here " << nodeNum << " " << attr3AC[c].nodeno << " " << nodeType[adj[attr3AC[c].nodeno][0]] << endl;
            string t = mdata.first;
            vector<int> d = mdata.second;
            // cout << "idhar " << t << " " << d.size() << endl;
            int mult=typeSize[t];
            for(int i=0;i<d.size();i++){
                // cout << "dims " << i << " " << d[i] << endl;
                if(i)mult*=d[i];
                attr3AC[nodeNum].arrDims.push_back(d[i]);
            }
            attr3AC[nodeNum] = attr3AC[c3];
            attr3AC[nodeNum].type = t;
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            typeOfNode[attr3AC[nodeNum].addrName] = t;
            attr3AC[nodeNum].dimsDone++;//dimsdone=1
            for(int i=0;i<d.size();i++)attr3AC[nodeNum].arrDims.push_back(d[i]);
            attr3AC[nodeNum].nameAtNode = nodeType[attr3AC[c].nodeno];
            // cout << "yaha pe hu aray1 " << attr3AC[nodeNum].dimsDone << " " << attr3AC[c].dimsDone << endl;
            string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c3].addrName + " * " + to_string(mult);
            attr3AC[nodeNum].threeAC.push_back(temp);
        }
        break;
        case 2:{
            int c =adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            // cout << "first print " << attr3AC[c].dimsDone << endl;
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum] = attr3AC[nodeNum]+attr3AC[c3];
            int mult = typeSize[attr3AC[c].type];
            // cout << "in array access " << attr3AC[c].arrDims.size() << " " << attr3AC[c].dimsDone << " " << mult << endl;
            for(int i=attr3AC[c].dimsDone+1;i<attr3AC[c].arrDims.size();i++){
                // cout << "multipling " << attr3AC[c].dimsDone << " " << i << " " << attr3AC[c].arrDims[i] <<" " << mult << endl;
                mult*=(attr3AC[c].arrDims)[i];
            }
            tempNum++;
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c3].addrName + " * " + to_string(mult);
            typeOfNode["t"+to_string(tempNum)] = attr3AC[c].type;
            attr3AC[nodeNum].threeAC.push_back(temp);
            tempNum++;
            typeOfNode[attr3AC[nodeNum].addrName] = attr3AC[c].type;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            // cout << "dims 2 hojaye " << attr3AC[c].dimsDone << endl; 
            attr3AC[nodeNum].dimsDone = attr3AC[c].dimsDone +1;
            attr3AC[nodeNum].arrDims.clear();
            attr3AC[nodeNum].type = attr3AC[c].type;
            for(int i=0;i<attr3AC[c].arrDims.size();i++)attr3AC[nodeNum].arrDims.push_back(attr3AC[c].arrDims[i]);
            // cout << "dims 2 hojaye " << attr3AC[nodeNum].dimsDone << endl; 
            attr3AC[nodeNum].nameAtNode = attr3AC[c].nameAtNode;
            // cout << "yaha pe hu aray " << attr3AC[nodeNum].nameAtNode << " " << attr3AC[nodeNum].arrDims.size() << endl;
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
            // cout << "dimexprs" << endl;
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
    // cout << "Dimexpr1 " << attr3AC[c].nameAtNode << endl;
    if(attr3AC[c].nameAtNode.size()!=0)attr3AC[nodeNum].arrDims.push_back(stoi(attr3AC[c].nameAtNode));
    else{
        attr3AC[nodeNum].arrDims.push_back(stoi(attr3AC[c].addrName));
        // cout << "dimexpr " << attr3AC[nodeNum].arrDims[0] << endl;
    }
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
            // cout << "idahr dekhra hu " << attr3AC[nodeNum].dimsDone << " " << attr3AC[c].dimsDone << endl;
            // cout << "check dims " << attr3AC[nodeNum].dimsDone << " " << attr3AC[nodeNum].arrDims.size() << endl;
            attr3AC[nodeNum].dimsDone = attr3AC[c].dimsDone;
            if(attr3AC[nodeNum].dimsDone == attr3AC[nodeNum].arrDims.size()){
                tempNum++;
                attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
                typeOfNode[attr3AC[nodeNum].addrName] = attr3AC[c].type;
                string temp = attr3AC[nodeNum].addrName + " = " + attr3AC[c].nameAtNode + " [ " + attr3AC[c].addrName + " ] ";
                attr3AC[nodeNum].threeAC.push_back(temp);
            }
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
            if(prodNum[adj[nodeNum][0]]==1){
                int c = adj[nodeNum][0];
                attr3AC[nodeNum] = attr3AC[c];
                tempNum++;
                attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
                // cout << "inside methodinvocation " << endl;
                vector<int> p;
                checkFunctionParameterTypes(attr3AC[c].nodeno, p);
                // pushonstack oldstackpointer
                //stackpointer =8
                //oldsp = sp
                string oldsp = "pushonstack oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer + 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = stackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                string temp = "call " + attr3AC[c].addrName +insideClassName + ", 0";
                attr3AC[nodeNum].threeAC.push_back(temp);
                temp = attr3AC[nodeNum].addrName + " = popparam";
                attr3AC[nodeNum].threeAC.push_back(temp);
                //stackpointer = oldsp
                //sp-8
                //oldsp=popfromstack
                oldsp = "stackpointer = oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer - 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = popfromstack";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                typeOfNode[attr3AC[nodeNum].addrName] = getFuncRet(nodeNum, attr3AC[c].addrName, insideClassName);

                pushLabelUp(nodeNum,c);
            }else{
                int c = adj[nodeNum][0];
                attr3AC[nodeNum] = attr3AC[c];
                tempNum++;
                string fname = attr3AC[adj[adj[c][0]][2]].addrName;
                attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
                // cout << "inside methodinvocation " << endl;
                vector<int> p;
                checkFunctionParameterTypes(attr3AC[c].nodeno, p);
                string oldsp = "pushonstack oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer + 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = stackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                string temp = "call " + fname +insideClassName + ", 0";
                attr3AC[nodeNum].threeAC.push_back(temp);
                temp = attr3AC[nodeNum].addrName + " = popparam";
                attr3AC[nodeNum].threeAC.push_back(temp);
                oldsp = "stackpointer = oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer - 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = popfromstack";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                typeOfNode[attr3AC[nodeNum].addrName] = getFuncRet(nodeNum, fname, insideClassName);
                pushLabelUp(nodeNum,c);
            }
        }
            break;
        case 2:{
            if(prodNum[adj[nodeNum][0]]==1){
                int c = adj[nodeNum][0];
                int c3 = adj[nodeNum][2];
                attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
                tempNum++;
                attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
                string oldsp = "pushonstack oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer + 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = stackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                for(int fcall=0; fcall<(attr3AC[c3].params).size();fcall++){
                    string temp = "pushparam " + (attr3AC[c3].params)[fcall];
                    attr3AC[nodeNum].threeAC.push_back(temp);
                }
                checkFunctionParameterTypes(attr3AC[c].nodeno, attr3AC[c3].paramsNodeNo);
                int sizeOfParams=0;
                for(int i=0;i<attr3AC[c3].paramsNodeNo.size();i++){
                    sizeOfParams+=typeSize[typeOfNode[to_string(attr3AC[c3].paramsNodeNo[i])]];
                }
                string spointer = "stackpointer + " + to_string(sizeOfParams);
                attr3AC[nodeNum].threeAC.push_back(spointer);
                string temp = "call " + attr3AC[c].addrName + insideClassName + ", " + to_string(attr3AC[c3].params.size());
                attr3AC[nodeNum].threeAC.push_back(temp);
                temp = attr3AC[nodeNum].addrName + " = popparam";
                attr3AC[nodeNum].threeAC.push_back(temp);
                oldsp = "stackpointer = oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer - 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = popfromstack";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                typeOfNode[attr3AC[nodeNum].addrName] = getFuncRet(nodeNum, attr3AC[c].addrName, insideClassName);
                pushLabelUp(nodeNum,c);
            }else{
                int c = adj[nodeNum][0];
                int c3 = adj[nodeNum][2];
                string fname = attr3AC[adj[adj[c][0]][2]].addrName;
                // cout << "in methodinvocation " << fname << endl;
                attr3AC[nodeNum] = attr3AC[c] + attr3AC[c3];
                tempNum++;
                attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
                string oldsp = "pushonstack oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer + 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = stackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                for(int fcall=0; fcall<(attr3AC[c3].params).size();fcall++){
                    string temp = "pushparam " + (attr3AC[c3].params)[fcall];
                    attr3AC[nodeNum].threeAC.push_back(temp);
                }
                checkFunctionParameterTypes(attr3AC[c].nodeno, attr3AC[c3].paramsNodeNo);
                int sizeOfParams=0;
                for(int i=0;i<attr3AC[c3].paramsNodeNo.size();i++){
                    sizeOfParams+=typeSize[typeOfNode[to_string(attr3AC[c3].paramsNodeNo[i])]];
                }
                string spointer = "stackpointer + " + to_string(sizeOfParams);
                attr3AC[nodeNum].threeAC.push_back(spointer);
                string temp = "call " + fname + insideClassName + ", " + to_string(attr3AC[c3].params.size());
                attr3AC[nodeNum].threeAC.push_back(temp);
                temp = attr3AC[nodeNum].addrName + " = popparam";
                attr3AC[nodeNum].threeAC.push_back(temp);
                oldsp = "stackpointer = oldstackpointer";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "stackpointer - 8";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                oldsp = "oldstackpointer = popfromstack";
                attr3AC[nodeNum].threeAC.push_back(oldsp);
                typeOfNode[attr3AC[nodeNum].addrName] = getFuncRet(nodeNum, fname, insideClassName);
                pushLabelUp(nodeNum,c);
            }
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
            // cout << "var init  " << attr3AC[nodeNum].threeAC.size() << endl;
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
            attr3AC[nodeNum].paramsNodeNo.push_back(attr3AC[c].nodeno);
            // cout << "in argument list " << attr3AC[c].nodeno << endl;
            pushLabelUp(nodeNum,c);
        }
        break;
        case 2:{
            int c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            attr3AC[nodeNum] = attr3AC[c];
            attr3AC[nodeNum].params.push_back(attr3AC[c3].addrName);
            attr3AC[nodeNum].paramsNodeNo.push_back(attr3AC[c3].nodeno);
            pushLabelUp(nodeNum,c);
        }

        break;
    }
    return;
}

void execExpression(int nodeNum){
    int c = adj[nodeNum][0];
    attr3AC[nodeNum] = attr3AC[c];
    someExpAddr = attr3AC[nodeNum].addrName;
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
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 2:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " *= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 3:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " /= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 4:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " += " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 5:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " -= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 6:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " <<= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 7:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " >>= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 8:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " >>>= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 9:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " &= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 10:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " ^= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
        }
        break;
        case 11:{
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            string temp = attr3AC[c].addrName + " |= " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=getTypeNode(c3);
            attr3AC[nodeNum].threeAC.push_back(temp);
            // cout << "assignment me " << attr3AC[c].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
            attr3AC[nodeNum].addrName = attr3AC[c].addrName;
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
            // typeOfNode[attr3AC[nodeNum].addrName] = 
            // cout << "in assignment expression " << attr3AC[nodeNum].addrName << " " << typeOfNode[attr3AC[nodeNum].addrName] << endl;
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
            // cout << "condaitona and " << attr3AC[c].threeAC.size() << endl;
            // string cTrue = getLabel(c,1);
            // string temp = cTrue + ":";
            // (attr3AC[nodeNum].threeAC).push_back(temp);
            attr3AC[nodeNum] = attr3AC[nodeNum] + attr3AC[c3];
            string temp = attr3AC[c].addrName + " && " + attr3AC[c3].addrName;
            attr3AC[nodeNum].threeAC.push_back(temp);
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
            typeOfNode[attr3AC[nodeNum].addrName] = "boolean";
    

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
            typeOfNode[attr3AC[nodeNum].addrName]="boolean";

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
            typeOfNode[attr3AC[nodeNum].addrName] = "boolean";

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

            typeOfNode[attr3AC[nodeNum].addrName]="boolean";

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

            typeOfNode[attr3AC[nodeNum].addrName]="boolean";

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
            // cout << nodeNum << " " << nodeType[nodeNum] << " " << pTrue << " " << pFalse << endl;
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
            typeOfNode[attr3AC[nodeNum].addrName] = "int";
            
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
            typeOfNode[attr3AC[nodeNum].addrName] = "int";

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

            typeOfNode[attr3AC[nodeNum].addrName] = "int";

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
            //widen
            widenNode(c,c3);
            string tp;
            tp = getTypeNode(c);
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            tempNum++;
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " +" +tp + " " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=tp;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            widenNode(c,c3);
            tempNum++;
            string tp = getTypeNode(c);
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " -"+tp+" " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName] = tp;
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
            widenNode(c,c3);
            tempNum++;
            string tp = getTypeNode(c);
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " *" + tp + " " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=tp;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 3:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            widenNode(c,c3);
            tempNum++;
            string tp = getTypeNode(c);
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " /"+ tp + " " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName]=tp;
            (attr3AC[nodeNum].threeAC).push_back(temp);
        }
            break;
        case 4:{
            c = adj[nodeNum][0];
            int c3 = adj[nodeNum][2];
            widenNode(c,c3);
            tempNum++;
            string tp = getTypeNode(c);
            attr3AC[nodeNum] = attr3AC[c]+attr3AC[c3];
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " = " + attr3AC[c].addrName + " % " + attr3AC[c3].addrName;
            typeOfNode[attr3AC[nodeNum].addrName] = tp;
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
            string tp;
            if(typeOfNode.find(to_string(attr3AC[c].nodeno))!=typeOfNode.end()){
                tp = typeOfNode[to_string(attr3AC[c].nodeno)];
            }
            if(attr3AC[c].addrName.size()>0 && typeOfNode.find(attr3AC[c].addrName)!=typeOfNode.end()){
                tp = typeOfNode[attr3AC[c].addrName];
            }
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =- " + attr3AC[c].addrName;
            typeOfNode[attr3AC[nodeNum].addrName] = tp;
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
            string tp;
            if(typeOfNode.find(to_string(attr3AC[c].nodeno))!=typeOfNode.end()){
                tp = typeOfNode[to_string(attr3AC[c].nodeno)];
            }
            if(attr3AC[c].addrName.size()>0 && typeOfNode.find(attr3AC[c].addrName)!=typeOfNode.end()){
                tp = typeOfNode[attr3AC[c].addrName];
            }
            attr3AC[nodeNum].addrName = "t" + to_string(tempNum);
            string temp = "t" + to_string(tempNum) + " =~ " + attr3AC[c].addrName;
            typeOfNode[attr3AC[nodeNum].addrName] = tp;
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
    if(inStatement && !(inMethodInvocation && inMN))checkIfDeclared(c,nodeType[c]);
    if(funcParamTemp.find(nodeType[c])!=funcParamTemp.end())attr3AC[nodeNum].addrName = funcParamTemp[nodeType[c]];
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

void labelStatementWithoutTrailingSubstatement(int nodeNum){
    if(prodNum[nodeNum]==5){
        getLabel(adj[nodeNum][4],3);
    }
}

void labelConditionalAndExpression(int nodeNum){
    if(prodNum[nodeNum]==2){
        getLabel(nodeNum,1);
        getLabel(nodeNum,2);
        falseLabel[adj[nodeNum][0]] = falseLabel[nodeNum];
        trueLabel[adj[nodeNum][2]] = trueLabel[nodeNum];
        falseLabel[adj[nodeNum][2]] = falseLabel[nodeNum];
    }
    return;
}

void labelConditionalOrExpression(int nodeNum){
    if(prodNum[nodeNum]==2){
        getLabel(nodeNum,1);
        getLabel(nodeNum,2);
        trueLabel[adj[nodeNum][0]] = trueLabel[nodeNum];
        trueLabel[adj[nodeNum][2]] = trueLabel[nodeNum];
        falseLabel[adj[nodeNum][2]]= falseLabel[nodeNum];
    }
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
    whileContinueLabel = getLabel(-1,0);//get label for continue
    falseLabel[adj[nodeNum][2]]=nextLabel[nodeNum];
    // cout << "idhar while ke label banana he " << nextLabel[nodeNum];
    
    whileBreakLabel = "L" + to_string(nextLabel[nodeNum]);
    return;
}

void labelFor(int nodeNum){
    forContinueLabel = getLabel(-1,0);
    forBreakLabel = "L" + to_string(nextLabel[nodeNum]);
    switch(prodNum[nodeNum]){
        case 1:{
            getLabel(adj[nodeNum][4],1);
            falseLabel[adj[nodeNum][4]]=nextLabel[nodeNum];
        }
        break;
        case 2:{
            getLabel(adj[nodeNum][3],1);
            falseLabel[adj[nodeNum][3]]=nextLabel[nodeNum];
        }
        break;
        case 3:
        break;
        case 4:{
            getLabel(adj[nodeNum][4],1);
            falseLabel[adj[nodeNum][4]]=nextLabel[nodeNum];
        }
        break;
        case 5:
        break;
        case 6:{
            getLabel(adj[nodeNum][3],1);
            falseLabel[adj[nodeNum][3]]=nextLabel[nodeNum];
        }
        break;
        case 7:
        break;
    }
}

void labelDo(int nodeNum){
    getLabel(adj[nodeNum][4],1);
    doContinueLabel = getLabel(-1,0);
    falseLabel[adj[nodeNum][4]] = nextLabel[nodeNum];
    doBreakLabel = "L" + to_string(nextLabel[nodeNum]);
    return;
}

void labelNot(int nodeNum){
    trueLabel[adj[nodeNum][1]] = falseLabel[nodeNum];
    falseLabel[adj[nodeNum][1]] = trueLabel[nodeNum];
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
    }else if("WhileStatement"==s || "WhileStatementNoShortIf"==s){
        labelWhile(nodeNum);
    }else if("ForStatement"==s || "ForStatementNoShortIf"==s){
        labelFor(nodeNum);
    }else if("DoStatement"==s){
        labelDo(nodeNum);
    }else if("StatementWithoutTrailingSubstatement"==s){
        labelStatementWithoutTrailingSubstatement(nodeNum);
    }else if("UnaryExpressionNotPlusMinus"==s && prodNum[nodeNum]==3){
        labelNot(nodeNum);
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
    if(nodeType[nodeNum]=="ClassBody"){
        insideClassName = classNameMap[nodeNum];
        // cout << "initialized insideclassname " << insideClassName << " " << nodeType[nodeNum] << " " << nodeNum << endl;
    }

    if(nodeType[nodeNum]=="Statement"){
        inStatement=1;
    }
    if(nodeType[nodeNum]=="MethodInvocation"){
        inMethodInvocation=1;
    }
    if(nodeType[nodeNum]=="Name"){
        inMN=1;
    }

    if(nodeType[nodeNum]=="WhileStatement"){
        isWhile=1;
    }
    if(nodeType[nodeNum]=="DoStatement"){
        isDo=1;
    }
    if(nodeType[nodeNum]=="ForStatement" || nodeType[nodeNum]=="ForStatementNoShortIf"){
        isFor = prodNum[nodeNum];
    }
    // cout << "NODENUM " << nodeNum << " " << nodeType[nodeNum] << endl;
    for(int i=0;i<adj[nodeNum].size();i++){
        postOrderTraversal3AC(adj[nodeNum][i]);
        // if("SwitchStatement"==nodeType[nodeNum] && i==2){
        //     switchExpAddr = someExpAddr;
            // cout << "inside possafsdfas df " << switchExpAddr << endl;
        // }
    }
    if(nodeType[nodeNum]=="WhileStatement"){
        isWhile=0;
    }
    if(nodeType[nodeNum]=="DoStatement"){
        isDo=0;
    }
    if(nodeType[nodeNum]=="ForStatement" || nodeType[nodeNum]=="ForStatementNoShortIf"){
        isFor = 0;
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
        insideClassName.clear();
    }else if("ClassBodyDeclarations" == s){
        execClassBodyDeclarations(nodeNum);
    }else if("ClassBodyDeclaration" == s){
        execClassBodyDeclaration(nodeNum);
    }else if("ClassMemberDeclaration" == s){
        execClassMemberDeclaration(nodeNum);
    }else if("MethodDeclaration" == s){
        execMethodDeclaration(nodeNum);
        funcParamTemp.clear();
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
        inStatement=0;
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
        inMethodInvocation=0;
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
        inMN = 0;
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
    }else if("IntegralType"==s){
        execIntegralType(nodeNum);
    }else if("CastExpression"==s){
        execCastExpression(nodeNum);
    }
    else{
        // cout << "function not written " << s << endl;
    }

    return;
}

void print3AC(int nodeNum){
    FILE* fp = freopen("threeAC.txt","w",stdout);
    for(int i=0;i<attr3AC[nodeNum].threeAC.size();i++){
        cout << (attr3AC[nodeNum].threeAC)[i] << endl;
    }
    fclose(fp);
}

