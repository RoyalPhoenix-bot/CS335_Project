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
	varTypes type;
	vector<string> params;
	
	bool operator=(attr const& obj){
		if(nodeno!=obj.nodeno) return false;
		if(type!=obj.type) return false;

		if(params.size()!=(obj.params).size()) return false;
		
		for(int i=0;i<params.size();i++){
			if(params[i]!=(obj.params)[i]) return false;
		}
		return true;
	} 
};
#endif