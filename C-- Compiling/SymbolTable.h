#pragma once
#include <memory>
#include <map>
#include "Symbol.h"
#include <vector>
#include "TACReg.h"
extern "C"
{
	#include "nodes.h"
}

class IRContainer;

using namespace std;

// A far more powerful SymbolTable with more information about scopes, types etc which we wil build from the AST.
// The SymbolTable can have SymbolTable children to implement lexical scoping (effectively we have a table per scope).
class SymbolTable : public enable_shared_from_this<SymbolTable>
{
private:
	map<std::string, Symbol> table;
	weak_ptr<SymbolTable> parent;
	vector<shared_ptr<SymbolTable>> children;

	// Keep track of global and local registers for compiler.
	static int globalVars;
	static int localVars;

	// For a list of variable declarations of a single type, get the names of all the variables.
	void getVariableNames(NODE* node, vector<string>& names);

	// Takes the F node and gets all the type and name param information from it. 
	void getParams(NODE* node, vector<pair<int, string>>& params);

	// Add child sym table
	void addChild(shared_ptr<SymbolTable> child);

	shared_ptr<SymbolTable> getSymbolTableForScope(ScopeData scope);
	
	// Looks for the next symbol upwards through the symbol table tree.
	Symbol& lookup_symbol(string name);
public:
	ScopeData scope;

	// Symbol Table we build from the AST
	static shared_ptr<SymbolTable> GLOBAL_SYMBOL_TABLE;
	static ScopeData GLOBAL_SCOPE;

	static void buildGlobalTable(NODE* root);

	void buildSymbolTable(NODE* node);

	bool add_symbol(Symbol symbol);

	// See if the variable has already been declared in the current scope.
	bool local_lookup(string name);

	// COMPILER
	void bind_registers(vector<TACReg>& globals, vector<TACReg>& locals);

	// Get register for a given variable.
	TACReg get_register(string name, IRContainer* ir);

	// Get the size of the function in bytes to alter the stack accordingly.
	int get_function_size(string name);

	// INTERPRETER 

	// Get integer value if it exists.
	shared_ptr<int> get_integer_symbol_value(string name, ScopeData const& scope);

	// Get function value if it exists
	shared_ptr<FunctionData> get_function_symbol_data(string name, ScopeData const& scope);

	// Bind a value to a symbol from a given scope - useful for the interpreter.
	void bind_integer_value(string name, ScopeData const& scope, int value);

	// Bind function data to a function symbol from a given scope - useful for interpreter.
	void bind_function_value(string name, ScopeData const& scope, FunctionData data);

	Symbol const& get_symbol(string name, ScopeData const& scope);
};


