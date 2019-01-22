#pragma once
#include <string>
#include "ScopeData.h"

struct TACReg;
struct FunctionData;

using namespace std;

struct Symbol
{
	//  Key information about symbol
	std::string name;
	int type;
	ScopeData scope;

	// COMPILER
	// Index of local/global register.
	int regValue;

	// INTERPRETER
	// Interpreter uses these fields to get values of symbols
	shared_ptr<FunctionData> functionData;
	shared_ptr<int> value;


	bool operator==(Symbol const& right) const
	{
		return (name == right.name);
	}
};