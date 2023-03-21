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
	int nodeno;
	varTypes type;
	vector<string> params;
} attr;
#endif