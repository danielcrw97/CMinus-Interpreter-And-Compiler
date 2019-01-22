#include "IRContainer.h"
#include "NodeTypes.h"
#include "SymbolTable.h"
#include "FunctionData.h"
#include <set>
#include <fstream>

using namespace std;

void IRContainer::getParams(NODE * node, vector<TACReg>& params)
{
	if (node == NULL)
	{
		return;
	}

	int type = node->type;

	if (type == NodeTypes::LIST)
	{
		getParams(node->left, params);
		getParams(node->right, params);
	}
	else if (type == NodeTypes::IDENTIFIER)
	{
		string name = ((TOKEN*)node)->lexeme;
		TACReg reg = SymbolTable::GLOBAL_SYMBOL_TABLE->get_register(name, this);
		params.push_back(reg);
	}
	else if (type == NodeTypes::CONSTANT)
	{
		TACReg reg = generateRegister();
		reg.type = RegType::R_CONST;
		int value = ((TOKEN*)node)->value;
		reg.regValue = value;
		params.push_back(reg);
	}
}

TACReg IRContainer::generateCode(NODE * node)
{
	TACReg noResult = { RegType::R_NONE, 1 };
	if (node == NULL)
	{
		return noResult;
	}

	int type = node->type;
	switch(type)
	{
	case NodeTypes::FUNCTION_DECLARATION:
	{
		TAC code;
		code.op = TACOperator::FUNCTIONDEC;
		string name = ((TOKEN*)node->left->right->left->left)->lexeme;
		code.name = name;
		codes.push_back(code);

		TAC beginFunc;
		beginFunc.op = TACOperator::BEGINFUNC;
		codes.push_back(beginFunc);
		int beginIndex = (int)(codes.size()) - 1;

		generateCode(node->right);

		TAC endFunc;
		endFunc.op = TACOperator::ENDFUNC;
		codes.push_back(endFunc);

		// Get number of bytes we need to allocate on stack between beginFunc and endFunc.
		bool flag = true;
		int index = beginIndex;
		std::set<TACReg, RegCompare> functionRegisters;
		
		while (flag)
		{
			++index;
			if (codes.at(index).op == TACOperator::ENDFUNC)
			{
				flag = false;
				break;
			}

			TAC code = codes.at(index);
			functionRegisters.insert(code.assignedReg).second;
		}
		
		int bytesAllocated = 4 * (int)(functionRegisters.size());

		Symbol const& sym = SymbolTable::GLOBAL_SYMBOL_TABLE->get_symbol(name, SymbolTable::GLOBAL_SCOPE);
		if (sym.functionData->returnType == NodeTypes::INT)
		{
			bytesAllocated += 4;
		}

		TAC& begin = codes.at(beginIndex);
		TACReg bytesReg;
		bytesReg.type = RegType::R_CONST;
		bytesReg.regValue = bytesAllocated;
		begin.var1 = bytesReg;

		pair<int, int> basicBlock;
		basicBlock.first = beginIndex;
		basicBlock.second = ((int)(codes.size())) - 1;
		basicBlocks.push_back(basicBlock);
		
		break;
	}
	case NodeTypes::LEAF:
	{
		generateCode(node->left);
		break;
	}
	case NodeTypes::CONSTANT:
	{
		TACReg reg = generateRegister();
		reg.type = RegType::R_CONST;
		int value = ((TOKEN*)node)->value;
		reg.regValue = value;
		return reg;
	}
	case NodeTypes::IDENTIFIER:
	{
		string name = ((TOKEN*)node)->lexeme;
		TACReg reg = SymbolTable::GLOBAL_SYMBOL_TABLE->get_register(name, this);
		return reg;                                                                                                                                                                                                                                                        
	}
	case NodeTypes::APPLY:
	{
		string funcName = ((TOKEN*)(node->left->left))->lexeme;

		std::vector<TACReg> params;
		getParams(node->right, params);	

		// Add params
		for (auto param : params)
		{
			TAC code;
			code.op = TACOperator::PARAM;
			code.var1 = param;
			codes.push_back(code);
		}

		TAC callFunc;
		callFunc.op = TACOperator::CALL;
		callFunc.name = funcName;

		codes.push_back(callFunc);
		break;
	}
	case NodeTypes::SEQUENCE:
	{
		generateCode(node->left);
		generateCode(node->right);
		break;
	}
	case NodeTypes::ASSIGNMENT:
	{
		TACReg assignedRegister = generateCode(node->left);
		TACReg assigneeRegister = generateCode(node->right);
		TAC code;
		code.op = TACOperator::ASSIGN;
		code.assignedReg = assignedRegister;
		code.var1 = assigneeRegister;
		break;
	}
	case NodeTypes::DECLARATION:
	{
		generateCode(node->left);
		generateCode(node->right);
		break;
	}
	case NodeTypes::RETURN:
	{
		TAC returnCode;
		returnCode.op = TACOperator::RETURNINT;
		TACReg reg = generateCode(node->left);
		returnCode.var1 = reg;
		codes.push_back(returnCode);
		break;
	}
	case NodeTypes::PLUS:
	case NodeTypes::MINUS:
	case NodeTypes::MULTIPLY:
	case NodeTypes::DIVIDE:
	case NodeTypes::MOD:
		NODE* left = node->left;
		NODE* right = node->right;

		TACReg t = generateRegister();

		TACReg t1 = generateCode(left);
		TACReg t2 = generateCode(right);

		TACOperator op;
		if (type == NodeTypes::PLUS)
		{
			op = TACOperator::ADD;
		}
		else if (type == NodeTypes::MINUS)
		{
			op = TACOperator::MINUS;
		}
		else if (type == NodeTypes::MULTIPLY)
		{
			op = TACOperator::MULTIPLY;
		}
		else if (type == NodeTypes::DIVIDE)
		{
			op = TACOperator::DIVIDE;
		}
		else if (type == NodeTypes::MOD)
		{
			op = TACOperator::MOD;
		}

		TAC tac = { op, t, t1, t2 };
		codes.push_back(tac);

		return t;
	}
}

void IRContainer::print()
{
	ofstream stream;
	stream.open("./TACOutput.txt");
	for (TAC tac : codes)
	{
		tac.print(stream);
	}
}

TACReg IRContainer::generateRegister()
{
	TACReg reg;
	reg.type = RegType::R_REG;
	reg.regValue = tempCount;
	++tempCount;
	++currentScopeRegCount;
	return reg;
}

void IRContainer::optimise()
{
	bool finishedOptimising = false;
	while (!finishedOptimising)
	{
		for (auto block : basicBlocks)
		{
		
		}
	}
}

IRContainer::IRContainer()
{
	scopes.push(SymbolTable::GLOBAL_SCOPE);
	SymbolTable::GLOBAL_SYMBOL_TABLE->bind_registers(globalRegisters, localRegisters);
}

