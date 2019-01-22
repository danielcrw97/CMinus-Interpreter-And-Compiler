#pragma once
#include <string>
#include <vector>
#include <utility>
#include "Symbol.h"

using namespace std;

struct NODE;

// Gives us a pointer to the function definition
// But also parameter information for type checking.
// Also gives the scope of the function to build closures from.
struct FunctionData
{
	string name;
	int returnType;
	ScopeData scope;
	// List of type, name pairs for params
	vector<pair<int, string>> params;
	NODE* funcDefinition;	
};