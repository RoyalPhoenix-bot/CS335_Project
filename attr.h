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

	bool operator==(attr const& other){
		if(nodeno!=other.nodeno) return false;
		if(type!=other.type) return false;
		if(addrName!=other.addrName) return false;
		if(addrConst!=other.addrConst) return false;

		if(params.size()!=(other.params).size()) return false;
		if(threeAC.size()!=(other.threeAC).size()) return false;
		
		for(int i=0;i<other.params.size();i++){
			if(params[i]!=(other.params)[i]) return false;
		}
		for(int i=0;i<other.threeAC.size();i++){
			if(threeAC[i]!=(other.threeAC)[i]) return false;
		}
		return true;
	} 

	void operator=(attr const& other){

		type = other.type;
		addrName = other.addrName;
		addrConst = other.addrConst;

		params.clear();
		
		for(int i=0;i<other.params.size();i++){
			params.push_back((other.params)[i]);
		}

		threeAC.clear();

		for(int i=0;i<other.threeAC.size();i++){
			threeAC.push_back((other.threeAC)[i]);
		}
	} 


	attr operator+(attr const& other){
		attr res;

		res.type = other.type;//should be ensured by type checking
		
		res.addrName = other.addrName;//not useful for +
		res.addrConst = other.addrConst;//not useful for +

		for(int i=0;i<params.size();i++){
			res.params.push_back(params[i]);
		}

		for(int i=0;i<other.params.size();i++){
			res.params.push_back((other.params)[i]);
		}

		for(int i=0;i<threeAC.size();i++){
			res.threeAC.push_back(threeAC[i]);
		}

		for(int i=0;i<other.threeAC.size();i++){
			res.threeAC.push_back((other.threeAC)[i]);
		}

		return res;
	} 
	
};

class attr2{
	
public:
	int nodeno;
	string type;
	string funcname;
	string classname;
	string nameId;//name of identifier
	vector<string> funcParams;
	vector<string> Idused;
	vector<int> dimFuncParams;
	int nDims;

	attr2(){
		nodeno=0;
		type="";
		classname="";
		funcname="";
		nDims=0;
	}
};

inline ostream& operator<<(ostream &os, const attr& other){
		
	for(int i=0;i<other.threeAC.size();i++){
		os<<(other.threeAC)[i]<<endl;
	}

	return os;
}

#endif