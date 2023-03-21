#ifndef ATTRIBUTES
#define ATTRIBUTES
#include <bits/stdc++.h>
using namespace std;

enum varTypes{
	intType,
	floatType,
	boolType,
	stringType,
	arrayType,
	nullType
};


class attr{
	
public:
	int nodeno;
	string addrName;
	varTypes type;
	int addrConst;
	vector<string> params;
	vector<string> threeAC;
	
	attr(){
		nodeno=0;
		addrName="";
		type=intType;
		addrConst=0;
	}

	bool operator==(attr const& obj){
		if(nodeno!=obj.nodeno) return false;
		if(type!=obj.type) return false;
		if(addrName!=obj.addrName) return false;
		if(addrConst!=obj.addrConst) return false;

		if(params.size()!=(obj.params).size()) return false;
		if(threeAC.size()!=(obj.threeAC).size()) return false;
		
		for(int i=0;i<obj.params.size();i++){
			if(params[i]!=(obj.params)[i]) return false;
		}
		for(int i=0;i<obj.threeAC.size();i++){
			if(threeAC[i]!=(obj.threeAC)[i]) return false;
		}
		return true;
	} 

	void operator=(attr const& obj){

		type = obj.type;
		addrName = obj.addrName;
		addrConst = obj.addrConst;

		params.clear();
		
		for(int i=0;i<obj.params.size();i++){
			params.push_back((obj.params)[i]);
		}

		threeAC.clear();

		for(int i=0;i<obj.threeAC.size();i++){
			threeAC.push_back((obj.threeAC)[i]);
		}
	} 


	attr operator+(attr const& obj){
		attr res;

		res.type = obj.type;//should be ensured by type checking
		
		res.addrName = obj.addrName;//not useful for +
		res.addrConst = obj.addrConst;//not useful for +

		for(int i=0;i<params.size();i++){
			res.params.push_back(params[i]);
		}

		for(int i=0;i<obj.params.size();i++){
			res.params.push_back((obj.params)[i]);
		}

		for(int i=0;i<threeAC.size();i++){
			res.threeAC.push_back(threeAC[i]);
		}

		for(int i=0;i<obj.threeAC.size();i++){
			res.threeAC.push_back((obj.threeAC)[i]);
		}

		return res;
	} 
	
};

#endif