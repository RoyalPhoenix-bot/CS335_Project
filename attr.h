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
	string nameAtNode;
	string addrName;
	string type;
	int addrConst;
	vector<string> params;
	vector<int> arrDims;
	int dimsDone;
	vector<string> threeAC;
	
	attr(){
		nodeno=-1;
		dimsDone=0;
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
		nodeno = other.nodeno;
		type = other.type;
		addrName = other.addrName;
		addrConst = other.addrConst;
		nameAtNode = other.nameAtNode;

		// if(trueLabel.find())

		arrDims.clear();
		
		for(int i=0;i<other.arrDims.size();i++){
			arrDims.push_back((other.arrDims)[i]);
		}

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

		// res.type = other.type;//should be ensured by type checking
		
		// res.addrName = other.addrName;//not useful for +
		// res.addrConst = other.addrConst;//not useful for +

		for(int i=0;i<arrDims.size();i++){
			res.arrDims.push_back(arrDims[i]);
		}

		for(int i=0;i<other.arrDims.size();i++){
			res.arrDims.push_back((other.arrDims)[i]);
		}

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

inline ostream& operator<<(ostream &os, const attr& other){
		
	for(int i=0;i<other.threeAC.size();i++){
		os<<(other.threeAC)[i]<<endl;
	}

	return os;
}

typedef struct Attributes{
	string type;
	string name;
	vector<string> otherParams;
	vector<int> intParams;
	int num; // to store a numbered attribute (Ex.to get size of array, to store how many vars declared at once i.e int a,b,c)
} attr1;

#endif