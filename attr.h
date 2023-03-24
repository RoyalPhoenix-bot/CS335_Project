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

typedef struct Attributes{
	string type;
	string name;
	vector<string> otherParams;
	vector<int> intParams;
	int num; // to store a numbered attribute (Ex.to get size of array, to store how many vars declared at once i.e int a,b,c)
} attr1;

typedef struct Attributes2{
	int nodeno;
	varTypes type;
	vector<string> params;
} attr2;

#endif