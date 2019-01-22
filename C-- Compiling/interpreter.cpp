#include "TACReg.h"
#include "interpreter.h"
#include "SymbolTable.h"
#include "NodeTypes.h"
#include <iostream>


const set<int> Interpreter::boolean_ops = { NodeTypes::EQ_OP, NodeTypes::NE_OP, NodeTypes::LESS_OP, NodeTypes::LE_OP, NodeTypes::GREATER_OP, NodeTypes::GE_OP };
const set<int> Interpreter::integer_ops = { NodeTypes::PLUS, NodeTypes::DIVIDE, NodeTypes::MOD, NodeTypes::MULTIPLY, NodeTypes::MINUS };

bool Interpreter::eval_bool(NODE * node)
{
	int leftValue;
	int rightValue;

	int type = node->type;

	if (node->left == NULL || node->right == NULL)
	{
		throw exception("Subchilds for boolean expression do not exist.");
	}

	leftValue = eval_int(node->left);
	rightValue = eval_int(node->right);

	switch (type)
	{
	case NodeTypes::EQ_OP:
	{
		return leftValue == rightValue;
	}
	case NodeTypes::NE_OP:
	{
		return leftValue != rightValue;
	}
	case NodeTypes::LESS_OP:
	{
		return leftValue < rightValue;
	}
	case NodeTypes::LE_OP:
	{
		return leftValue <= rightValue;
	}
	case NodeTypes::GREATER_OP:
	{
		return leftValue > rightValue;
	}
	case NodeTypes::GE_OP:
	{
		return leftValue >= rightValue;
	}

	default:
		throw std::exception("Invalid value in boolean comparison.");
	}
	return false;
}

FunctionData Interpreter::eval_func(NODE * node)
{
	// Can only get a function from an identifier or another function call.
	int type = node->type;

	if (type == NodeTypes::APPLY)
	{
		Value funcValue = runFunction(node);
		if (funcValue.valueType != NodeTypes::FUNCTION || funcValue.func == nullptr)
		{
			throw exception("Invalid function value.");
		}
		else
		{
			return *funcValue.func;
		}
	}
	else if (type == NodeTypes::IDENTIFIER)
	{
		string funcName = ((TOKEN*)node)->lexeme;
		Symbol const& sym = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(funcName, scopes.top());
		if (sym.type != NodeTypes::FUNCTION)
		{
			throw exception("Invalid type - should be function.");
		}
		return *SymbolTable::GLOBAL_SYMBOL_TABLE->get_function_symbol_data(funcName, scopes.top());
	}
	else if (type == NodeTypes::LEAF)
	{
		return eval_func(node->left);
	}
	else
	{
		throw exception("Attempted to evaluate non function as function.");
	}
}

// Runs function and returns a value from a given APPLY node.
Value Interpreter::runFunction(NODE * node)
{
	if (node->left == NULL)
	{
		throw exception("No function to apply");
	}

	string funcName = ((TOKEN*)(node->left->left))->lexeme;

	// Check if it is an input/output function.
	if (funcName == "print_string")
	{
		if (node->right->left->type != NodeTypes::STRING_LITERAL)
		{
			throw exception("Tried to print non string type.");
		}

		string toPrint = ((TOKEN*)node->right->left)->lexeme;
		cout << toPrint << endl;

		Value value;
		value.valueType = NodeTypes::VOID;
		return value;
	}
	else if (funcName == "read_int")
	{
		int i;
		cin >> i;

		Value intValue;
		intValue.valueType = NodeTypes::INT;
		intValue.integer = make_shared<int>(i);
		return intValue;
	}
	else if (funcName == "print_int")
	{
		int toPrint = eval_int(node->right);
		cout << toPrint << endl;
		Value value;
		value.valueType = NodeTypes::VOID;
		return value;
	}

	shared_ptr<FunctionData> funcData = SymbolTable::GLOBAL_SYMBOL_TABLE->get_function_symbol_data(funcName, scopes.top());

	ScopeData funcScope = funcData->scope;
	vector<Value> paramValues;
	if (node->right != NULL)
	{
		getFunctionParamValues(node->right, paramValues);
	}

	if (paramValues.size() != funcData->params.size())
	{
		throw exception("Invalid parameters");
	}

	vector<pair<int, string>> params = funcData->params;

	scopes.push(funcData->scope);

	// Do type checking on params and assign parameter values.
	for (int i = 0;  i<(int)params.size();++i)
	{
		int valueType = paramValues.at(i).valueType;
		if (params.at(i).first != paramValues.at(i).valueType)
		{
			throw exception("Invalid type used for parameter");
		}

		int type = params.at(i).first;
		if (type == NodeTypes::INT)
		{
			if (paramValues.at(i).integer == nullptr)
			{
				throw exception("Invalid parameter");
			}
			SymbolTable::GLOBAL_SYMBOL_TABLE->bind_integer_value(params.at(i).second, scopes.top(), *paramValues.at(i).integer);
		}
		else if (type == NodeTypes::FUNCTION)
		{
			if (paramValues.at(i).func == nullptr)
			{
				throw exception("Invalid parameter");
			}
			SymbolTable::GLOBAL_SYMBOL_TABLE->bind_function_value(params.at(i).second, scopes.top(), *paramValues.at(i).func);
		}
	}

	Value returnValue;
	interpret(funcData->funcDefinition, returnValue);
	scopes.pop();

	if (funcData->returnType == NodeTypes::VOID)
	{
		Value voidValue;
		voidValue.valueType = NodeTypes::VOID;
		return voidValue;
	}
	else if (!returnValue.returningValue)
	{
		throw exception("Did not return value from function!");
	}
	else if(returnValue.valueType != funcData->returnType)
	{
		throw exception("Wrong type returned.");
	}
	else
	{
		return returnValue;
	}
}

int Interpreter::eval_int(NODE* node)
{
	int type = node->type;

	// For the following we return direct values
	if (type == NodeTypes::LEAF)
	{
		return eval_int(node->left);
	}
	if (type == NodeTypes::APPLY)
	{
		// do type check on function to make sure it returns an int
		// then carry out function
		Value val = runFunction(node);
		if (val.valueType == NodeTypes::INT && val.integer != nullptr)
		{
			return *val.integer;
		}	
	}
	if (type == NodeTypes::IDENTIFIER)
	{
		string name = ((TOKEN*)node)->lexeme;
		Symbol const& sym = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(name, scopes.top());
		if (sym.type != NodeTypes::INT)
		{
			throw exception("Invalid type - should be integer.");
		}
		if (shared_ptr<int> value = SymbolTable::GLOBAL_SYMBOL_TABLE->get_integer_symbol_value(name, scopes.top()))
		{
			return *value;
		}
		else
		{
			throw exception("No value for variable found.");
		}
	}
	if (type == NodeTypes::CONSTANT)
	{
		return ((TOKEN*)node)->value;
	}

	// If we are not returning a direct value, make sure the sub nodes are not null pointers.
	if (node->left == NULL || node->right == NULL)
	{
		throw exception("Subchilds missing for integer expression.");
	}

	int leftValue = eval_int(node->left);
	int rightValue = eval_int(node->right);

	switch (type)
	{
	case NodeTypes::PLUS:
		return leftValue + rightValue;

	case NodeTypes::DIVIDE:
		return (int)(leftValue / rightValue);

	case NodeTypes::MINUS:
		return leftValue - rightValue;

	case NodeTypes::MULTIPLY:
		return leftValue * rightValue;

	case NodeTypes::MOD:
		return leftValue % rightValue;

	default:
		break;
	}

	// If we get here we are not evaluating an integer expression and should throw an exception.
	throw exception("Attempting to evaluate non integer expression as integer.");
}

void Interpreter::getFunctionParamValues(NODE * node, vector<Value>& paramValues)
{
	if (node == NULL)
	{
		return;
	}

	int type = node->type;
	switch (type)
	{
	case NodeTypes::APPLY:
	{
		paramValues.push_back(runFunction(node));
		break;
	}
	case NodeTypes::LIST:
	{
		getFunctionParamValues(node->left, paramValues);
		getFunctionParamValues(node->right, paramValues);
		break;
	}
	case NodeTypes::LEAF:
	{
		getFunctionParamValues(node->left, paramValues);
		break;
	}
	case NodeTypes::IDENTIFIER:
	{
		string varName = ((TOKEN*)node)->lexeme;
		Symbol const& sym = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(varName, scopes.top());
		int type = sym.type;
		if (type == NodeTypes::FUNCTION)
		{
			shared_ptr<FunctionData> funcData = SymbolTable::GLOBAL_SYMBOL_TABLE->get_function_symbol_data(varName, scopes.top());
			Value val;
			val.valueType = type;
			val.func = funcData;
			paramValues.push_back(val);
		}
		else if (type == NodeTypes::INT)
		{
			shared_ptr<int> integer = SymbolTable::GLOBAL_SYMBOL_TABLE->get_integer_symbol_value(varName, scopes.top());
			Value val;
			val.valueType = type;
			val.integer = integer;
			paramValues.push_back(val);
		}
		break;
	}
	default:
	{
		break;
	}
	}

	if(type == NodeTypes::PLUS || 
		type == NodeTypes::DIVIDE ||
		type == NodeTypes::MINUS || 
		type == NodeTypes::MULTIPLY || 
		type == NodeTypes::MOD || 
		type == NodeTypes::CONSTANT)
	{
		int value = eval_int(node);
		Value val;
		val.valueType = NodeTypes::INT;
		val.integer = make_shared<int>(value);
		paramValues.push_back(val);
	}
}

Interpreter::Interpreter()
{
	scopes.push(SymbolTable::GLOBAL_SCOPE);
}

void Interpreter::interpret(NODE* node, Value& returnValue)
{
	if (node == NULL)
	{
		return;
	}

	if (returnValue.returningValue)
	{
		//Return to top
		return;
	}

	int type = node->type;
	switch (type)
	{
	case(NodeTypes::APPLY):
	{
		runFunction(node);
		break;
	}
	case(NodeTypes::DECLARATION):
	{
		// We would need to interpret declarations in case there is an assignment within them.
		interpret(node->left, returnValue);
		interpret(node->right, returnValue);
		break;
	}
	case(NodeTypes::FUNCTION_DECLARATION):
	{
		// We need this to run main when we first declare it.
		if (node->left->left->left->type == NodeTypes::INT)
		{
			NODE* fNode = node->left->right;
			string funcName = ((TOKEN*)(fNode->left->left))->lexeme;
			if (funcName == "main")
			{
				if (node->right != NULL)
				{
					ScopeData mainScope;
					mainScope.depth = 1;
					mainScope.isGlobal = false;
					mainScope.name = "global_main";

					scopes.push(mainScope);
					
					interpret(node->right, returnValue);

					scopes.pop();
				}
			}
		}
		break;
	}
	case(NodeTypes::ASSIGNMENT):
	{
		string varName = ((TOKEN*)node->left->left)->lexeme;
		Symbol const& var = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(varName, scopes.top());
		int type = var.type;

		NODE* right = node->right;
		if (right == NULL)
		{
			throw exception("No assignment made.");
		}

		if (type == NodeTypes::INT)
		{
			int value = eval_int(node->right);
			SymbolTable::GLOBAL_SYMBOL_TABLE->bind_integer_value(varName, scopes.top(), value);
		}
		else if (type == NodeTypes::FUNCTION)
		{
			// Can either be an identifier or an applied function that returns a function value.
			FunctionData value = eval_func(node->right);
			SymbolTable::GLOBAL_SYMBOL_TABLE->bind_function_value(varName, scopes.top(), value);
		}
		else
		{
			throw exception("Invalid type.");
		}
		break;
	}
	case(NodeTypes::IF):
	{
		if (node->right == NULL)
		{
			break;
		}

		++scopes.top().ifCount;

		if (eval_bool(node->left))
		{
			ScopeData& currentScope = scopes.top();

			// Create new scopes
			ScopeData newScope;
			newScope.depth = currentScope.depth + 1;
			newScope.ifCount = 0;
			newScope.whileCount = 0;
			newScope.name = currentScope.name + "_IF" + to_string(currentScope.ifCount);
			scopes.push(newScope);

			if (node->right->type == NodeTypes::ELSE)
			{
				interpret(node->right->left, returnValue);
			}
			else
			{
				interpret(node->right, returnValue);
			}

			scopes.pop();
		}
		else
		{
			if (node->right->type == NodeTypes::ELSE)
			{
				interpret(node->right, returnValue);
			}
		};
		break;
	}
	case(NodeTypes::ELSE):
	{
		if (node->right == NULL)
		{
			break;
		}

		if (node->right->type == NodeTypes::IF)
		{
			interpret(node->right, returnValue);
		}
		else
		{
			ScopeData& currentScope = scopes.top();
			++currentScope.ifCount;

			// Create new scopes
			ScopeData newScope;
			newScope.depth = currentScope.depth + 1;
			newScope.ifCount = 0;
			newScope.whileCount = 0;
			newScope.name = currentScope.name + "_IF" + to_string(currentScope.ifCount);
			scopes.push(newScope);

			interpret(node->right, returnValue);

			scopes.pop();		
		}
		break;
	}
	case(NodeTypes::WHILE):
	{
		ScopeData& currentScope = scopes.top();
		++currentScope.whileCount;

		if (node->right == NULL)
		{
			break;
		}

		ScopeData newScope;
		newScope.depth = currentScope.depth + 1;
		newScope.ifCount = 0;
		newScope.whileCount = 0;
		newScope.name = currentScope.name + "_WHILE" + to_string(currentScope.whileCount);
		scopes.push(newScope);

		while (eval_bool(node->left))
		{
			interpret(node->right, returnValue);
		}

		scopes.pop();
	}
	case (NodeTypes::SEQUENCE):
	{
		interpret(node->left, returnValue);
		interpret(node->right, returnValue);
		break;
	}
	case (NodeTypes::LIST):
	{
		interpret(node->left, returnValue);
		interpret(node->right, returnValue);
		break;
	}
	case (NodeTypes::RETURN):
	{
		// If there is no left sub tree return void
		if (node->left == NULL)
		{
			Value voidValue;
			voidValue.valueType = NodeTypes::VOID;
			returnValue = voidValue;
			returnValue.returningValue = true;
			break;
		}

		int leftType = node->left->type;

		if (leftType == NodeTypes::LEAF)
		{
			int leafType = node->left->left->type;
			if (leafType == NodeTypes::CONSTANT)
			{
				returnValue.valueType = NodeTypes::INT;
				returnValue.integer = make_shared<int>(eval_int(node->left->left));
				returnValue.returningValue = true;
				break;
			}
			if (leafType == NodeTypes::IDENTIFIER)
			{
				string name = ((TOKEN*)node->left->left)->lexeme;
				Symbol const& sym = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(name, scopes.top());
				if (sym.type == NodeTypes::INT)
				{
					returnValue.valueType = NodeTypes::INT;
					returnValue.integer = make_shared<int>(eval_int(node->left->left));
					returnValue.returningValue = true;
					break;
				}
				else if (sym.type == NodeTypes::FUNCTION)
				{
					returnValue.valueType = NodeTypes::FUNCTION;
					returnValue.func = make_shared<FunctionData>(eval_func(node->left->left));
					returnValue.returningValue = true;
					break;
				}
			}
		}
		if (integer_ops.find(leftType) != integer_ops.end())
		{
			returnValue.valueType = NodeTypes::INT;
			returnValue.integer = make_shared<int>(eval_int(node->left));
			returnValue.returningValue = true;
			break;
		}
		if (leftType == NodeTypes::APPLY)
		{
			returnValue = runFunction(node->left);
			returnValue.returningValue = true;
			break;
		}
		break;
	}
	default:
		break;
	}
}