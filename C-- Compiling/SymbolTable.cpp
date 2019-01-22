#include "SymbolTable.h"
#include "NodeTypes.h"
#include <stack>
#include <iostream>
#include "FunctionData.h"
#include "IRContainer.h"

shared_ptr<SymbolTable> SymbolTable::GLOBAL_SYMBOL_TABLE = make_shared<SymbolTable>();
ScopeData SymbolTable::GLOBAL_SCOPE;
int SymbolTable::globalVars = 0;
int SymbolTable::localVars = 0;

void SymbolTable::getVariableNames(NODE* node, vector<string>& names)
{
	int type = node->type;
	switch (type)
	{
	case NodeTypes::ASSIGNMENT:
	{
		getVariableNames(node->left, names);
		break;
	}
	case NodeTypes::IDENTIFIER:
	{
		TOKEN* identifier = (TOKEN*)node;
		names.push_back(identifier->lexeme);
		break;
	}
	case NodeTypes::LIST:
	{
		getVariableNames(node->left, names);
		getVariableNames(node->right, names);
		break;
	}
	case NodeTypes::LEAF:
	{
		getVariableNames(node->left, names);
		break;
	}
	default:
		break;
	}
}

void SymbolTable::getParams(NODE* node, vector<pair<int, string>>& params)
{
	int type = node->type;
	switch (type)
	{
	case NodeTypes::FUNCTION_PARAMS:
	{
		getParams(node->right, params);
		break;
	}
	case NodeTypes::DECLARATION:
	{
		
		int type = node->left->left->type;
		string name = ((TOKEN*)(node->right->left))->lexeme;
		params.push_back(pair<int, string>(type, name));
		break;
	}
	case NodeTypes::LIST:
	{
		getParams(node->left, params);
		getParams(node->right, params);
	}
	default:
		break;
	}
}

void SymbolTable::addChild(shared_ptr<SymbolTable> child)
{
	// Need special case for global scope due to weird smart pointer bug
	if (scope == GLOBAL_SCOPE)
	{
		child->parent = GLOBAL_SYMBOL_TABLE;
		children.push_back(child);
	}
	else
	{
		// Create weak_ptr to avoid circular reference.
		child->parent = shared_from_this();
		children.push_back(child);
	}
}

shared_ptr<SymbolTable> SymbolTable::getSymbolTableForScope(ScopeData scope)
{
	if (scope.name == this->scope.name)
	{
		return shared_from_this();
	}

	string name = scope.name;

	for (shared_ptr<SymbolTable> child : children)
	{
		string childScopeName = child->scope.name;
		size_t found = name.find(childScopeName);
		if (found == 0)
		{
			if (childScopeName == name)
			{
				return child;
			}
			else
			{
				return child->getSymbolTableForScope(scope);
			}
		}
	}
	
	throw exception("Could not find valid scope.");
}

void SymbolTable::buildGlobalTable(NODE* root)
{
	ScopeData globalScope;
	globalScope.isGlobal = true;
	globalScope.name = "global";
	globalScope.depth = 0;

	SymbolTable::GLOBAL_SCOPE = globalScope;

	// TODO Predefine global functions for printing and reading integers/strings.
	Symbol read_int;
	read_int.type = NodeTypes::FUNCTION;
	read_int.name = "read_int";
	read_int.scope = globalScope;
	FunctionData read_int_func;
	read_int_func.name = "read_int";
	read_int_func.returnType = NodeTypes::INT;
	read_int_func.scope = globalScope;
	read_int.functionData = make_shared<FunctionData>(read_int_func);

	Symbol print_int;
	print_int.type = NodeTypes::FUNCTION;
	print_int.name = "print_int";
	print_int.scope = globalScope;
	FunctionData print_int_func;
	print_int_func.name = "print_int";
	print_int_func.returnType = NodeTypes::VOID;
	print_int_func.scope = globalScope;
	print_int.functionData = make_shared<FunctionData>(print_int_func);

	Symbol print_string;
	print_string.type = NodeTypes::FUNCTION;
	print_string.name = "print_string";
	print_string.scope = globalScope;
	FunctionData print_string_func;
	print_string_func.name = "print_string";
	print_string_func.returnType = NodeTypes::VOID;
	print_string_func.scope = globalScope;
	print_string.functionData = make_shared<FunctionData>(print_string_func);

	GLOBAL_SYMBOL_TABLE->table["read_int"] = read_int;
	GLOBAL_SYMBOL_TABLE->table["print_int"] = print_int;
	GLOBAL_SYMBOL_TABLE->table["print_string"] = print_string;

	GLOBAL_SYMBOL_TABLE->scope = globalScope;
	GLOBAL_SYMBOL_TABLE->buildSymbolTable(root);
}

void SymbolTable::buildSymbolTable(NODE* node)
{
	if (node == NULL)
	{
		return;
	}

	int type = node->type;
	switch (type)
	{
	case NodeTypes::DECLARATION:
	{
		int leftType = node->left->type;
		int rightType = node->right->type;

		//If the left node is a not a typed leaf we know we have several declarations and should recurse.
		if (leftType != NodeTypes::LEAF)
		{
			buildSymbolTable(node->left);
			buildSymbolTable(node->right);
		}

		// If the left node is a leaf representing type we know that the right will give us a variable
		// Or a sequence of variables of that type.
		if (leftType == NodeTypes::LEAF)
		{
			Symbol newSym;
			newSym.scope = this->scope;

			int varType = node->left->left->type;
			if (varType == NodeTypes::FUNCTION)
			{
				newSym.type = varType;
			}
			else if (varType == NodeTypes::INT)
			{
				newSym.type = varType;
			}
			else if (varType == NodeTypes::VOID)
			{
				throw std::exception("Can't assign void type to variable");
			}
			else
			{
				throw std::exception("Non typed leaf for variable declaration");
			}

			// Get name of variable/variables.
			if (node->right->type == NodeTypes::ASSIGNMENT)
			{
				TOKEN* identifier_token = (TOKEN*) (node->right->left->left);
				newSym.name = identifier_token->lexeme;
				add_symbol(newSym);
			}
			else if (node->right->type == NodeTypes::LEAF)
			{
				TOKEN* identifier_token = (TOKEN*)(node->right->left);
				newSym.name = identifier_token->lexeme;
				add_symbol(newSym);
			}
			else if (node->right->type == NodeTypes::LIST)
			{
				vector<string> varNames;
				getVariableNames(node->right, varNames);
				for (string name : varNames)
				{
					newSym.name = name;
					add_symbol(newSym);
				}
			}
		}
		break;
	}

	// Declaration of a function 
	case NodeTypes::FUNCTION_DECLARATION:
	{
		Symbol functionSymbol;
		FunctionData funcData;

		functionSymbol.type = NodeTypes::FUNCTION;
		functionSymbol.scope = this->scope;
		
		int functionReturnType = node->left->left->left->type;
		switch (functionReturnType)
		{
		case NodeTypes::INT:
			funcData.returnType = functionReturnType;
			break;

		case NodeTypes::FUNCTION:
			funcData.returnType = functionReturnType;
			break;

		case NodeTypes::VOID:
			funcData.returnType = functionReturnType;
			break;
		
		default:
			throw std::exception("No return type for function.");
			break;
		}

		TOKEN* func_identifier = (TOKEN*) node->left->right->left->left;
		functionSymbol.name = func_identifier->lexeme;
		funcData.name = func_identifier->lexeme;

		funcData.funcDefinition = node->right;

		vector<pair<int, string>> params;
		// Make sure there are params to get!
		if (node->left->right->right != NULL)
		{
			getParams(node->left->right, params);
		}

		funcData.params = params;

		// Create new scope from function.
		ScopeData newScope = this->scope;
		newScope.name = this->scope.name + "_" + funcData.name;
		newScope.isGlobal = false;
		newScope.depth = this->scope.depth + 1;

		funcData.scope = newScope;

		shared_ptr<FunctionData> pFuncData = make_shared<FunctionData>(funcData);
		functionSymbol.functionData = pFuncData;
		add_symbol(functionSymbol);

		shared_ptr<SymbolTable> child = make_shared<SymbolTable>();
		child->scope = newScope;
		if (node->left->right->right != NULL)
		{
			child->buildSymbolTable(node->left->right->right);
		}
		if (node->right != NULL)
		{
			child->buildSymbolTable(node->right);
		}
		addChild(child);

		break;
	}
	// Create new scopes for if and while loops.
	case NodeTypes::IF:
	{
		++scope.ifCount;
		string currentScopeName = scope.name;
		string newScopeName = currentScopeName + "_IF" + std::to_string(scope.ifCount);
		
		ScopeData newScope;
		newScope.name = newScopeName;
		newScope.depth = scope.depth + 1;
		newScope.isGlobal = false;

		shared_ptr<SymbolTable> newTable = make_shared<SymbolTable>();
		newTable->scope = newScope;
		addChild(newTable);

		if (node->right == NULL)
		{
			return;
		}
		
		// Check for else statement to create another scope.
		if (node->right->type == NodeTypes::ELSE )
		{
			// If we evaluate the expression to be true the block carried out will be the left sub node of the else.
			// So build a subtree out of this.
			newTable->buildSymbolTable(node->right->left);
			buildSymbolTable(node->right);
		}
		else
		{
			newTable->buildSymbolTable(node->right);
		}
		break;
	}
	case NodeTypes::ELSE:
	{
		if (node->right == NULL)
		{
			return;
		}
		int rightType = node->right->type;
		if (rightType == NodeTypes::IF)
		{
			buildSymbolTable(node->right);
		}
		else
		{
			++scope.ifCount;
			string currentScopeName = scope.name;
			string newScopeName = currentScopeName + "_IF" + std::to_string(scope.ifCount);

			ScopeData newScope;
			newScope.name = newScopeName;
			newScope.depth = scope.depth + 1;
			newScope.isGlobal = false;

			shared_ptr<SymbolTable> newTable = make_shared<SymbolTable>();
			newTable->scope = newScope;

			newTable->buildSymbolTable(node->right);
			addChild(newTable);
		}
		break;
	}
	case NodeTypes::WHILE:
	{
		++scope.whileCount;
		string currentScopeName = scope.name;
		string newScopeName = currentScopeName + "_WHILE" + to_string(scope.whileCount);

		ScopeData newScope;
		newScope.name = newScopeName;
		newScope.depth = scope.depth + 1;
		newScope.isGlobal = false;

		shared_ptr<SymbolTable> newTable = make_shared<SymbolTable>();
		newTable->scope = newScope;
		newTable->buildSymbolTable(node->right);
		addChild(newTable);
		break;
	}
	case NodeTypes::SEQUENCE:
	{
		buildSymbolTable(node->left);
		buildSymbolTable(node->right);
		break;
	}
	case NodeTypes::LIST:
	{
		buildSymbolTable(node->left);
		buildSymbolTable(node->right);
		break;
	}
	default:
		break;
	}
}

bool SymbolTable::add_symbol(Symbol symbol)
{
	if (symbol.type == NodeTypes::VOID)
	{
		throw std::exception("Attempted to assign void return type to variable.");
	}

	if (symbol.scope == this->scope)
	{
		if (table.find(symbol.name) != table.end())
		{
			cout << "Multiple declarations of variable." << endl;
			throw std::exception("Multiple declarations of variable");
		}

		table[symbol.name] = symbol;
	}
	else
	{
		for (shared_ptr<SymbolTable> child : children)
		{
			if (child->add_symbol(symbol))
			{
				return true;
			}
		}
	}
	return false;
}

Symbol& SymbolTable::lookup_symbol(string name)
{
	bool isGlobalTable = scope.isGlobal;

	bool symPresent = table.find(name) != table.end();
	if (!symPresent)
	{	
		auto shared = parent.lock();
		if (shared)
		{
			return shared->lookup_symbol(name);
		}
		else
		{
			throw exception("Could not find given symbol.");
		}
	}
	else
	{
		return table.at(name);
	}
}

bool SymbolTable::local_lookup(string name)
{
	return table.find(name) != table.end();
}

void SymbolTable::bind_registers(vector<TACReg>& globals, vector<TACReg>& locals)
{
	for (auto& k : table)
	{
		Symbol& sym = k.second;

		if (!sym.type == NodeTypes::INT)
		{
			continue;
		}

		TACReg reg;
		if (scope.isGlobal)
		{
			reg.type = RegType::R_GLOBAL;
			reg.regValue = globals.size();
			sym.regValue = reg.regValue;
			
			globals.push_back(reg);
		}
		else
		{
			reg.type = RegType::R_LOCAL;
			reg.regValue = locals.size();
			sym.regValue = reg.regValue;
			
			locals.push_back(reg);
		}
	}

	for (shared_ptr<SymbolTable> child : children)
	{
		child->bind_registers(globals, locals);
	}
}

TACReg SymbolTable::get_register(string name, IRContainer* ir)
{
	if (table.find(name) != table.end())
	{
		Symbol const& sym = table.at(name);
		if (sym.scope.isGlobal)
		{
			return ir->globalRegisters.at(sym.regValue);
		}
		else
		{
			return ir->localRegisters.at(sym.regValue);
		}
	}
	else
	{
		for (auto symTable : children)
		{
			return symTable->get_register(name, ir);
		}
	}
}

shared_ptr<int> SymbolTable::get_integer_symbol_value(string name, ScopeData const& scope)
{
	Symbol const& sym = get_symbol(name, scope);
	if (sym.value == nullptr)
	{
		throw exception("Undefined variable.");
	}
	return sym.value;
}

shared_ptr<FunctionData> SymbolTable::get_function_symbol_data(string name, ScopeData const& scope)
{
	Symbol const& sym = get_symbol(name, scope);
	if (sym.functionData == nullptr)
	{
		throw exception("Undefined variable.");
	}
	return sym.functionData;
}

void SymbolTable::bind_integer_value(string name, ScopeData const& scope, int value)
{
	shared_ptr<SymbolTable> table = getSymbolTableForScope(scope);
	Symbol& sym = table->lookup_symbol(name);
	if (sym.type != NodeTypes::INT)
	{
		throw exception("Attempted to bind integer value to non int.");
	}
	sym.value = make_shared<int>(value);
}

void SymbolTable::bind_function_value(string name, ScopeData const& scope, FunctionData data)
{
	shared_ptr<SymbolTable> table = getSymbolTableForScope(scope);
	Symbol& sym = table->lookup_symbol(name);
	if (sym.type != NodeTypes::FUNCTION)
	{
		throw exception("Attempted to bind function value to non function.");
	}
	sym.functionData = make_shared<FunctionData>(data);
}

Symbol const& SymbolTable::get_symbol(string name, ScopeData const& scope)
{
	shared_ptr<SymbolTable> table = getSymbolTableForScope(scope);
	return table->lookup_symbol(name);
}




