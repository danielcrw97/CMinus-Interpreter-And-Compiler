#pragma once
#include "ScopeData.h"
#include <set>
#include <stack>
#include "Value.h"

using namespace std;

struct NODE;

class Interpreter
{

private:
	static const set<int> boolean_ops;
	static const set<int> integer_ops;

	// Used to keep track of where we are.
	stack<ScopeData> scopes;

	// Evaluate boolean expression.
	bool eval_bool(NODE* node);

	// Evaluate integer expression.
	int eval_int(NODE* node);

	// Evaluate function expression.
	FunctionData eval_func(NODE* node);

	Value runFunction(NODE* node);

	void getFunctionParamValues(NODE* node, vector<Value>& paramValues);

public:
	Interpreter();

	void interpret(NODE* root, Value& returnValue);
};

