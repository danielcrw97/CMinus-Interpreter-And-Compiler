#pragma once
#include<vector>
#include "TAC.h"
#include <stack>
#include <utility>
#include "TACReg.h"
#include "ScopeData.h"

struct NODE;
struct TACReg;

class IRContainer
{
private:
	std::stack<ScopeData> scopes;

	int tempCount;

	// Used to keep track of how muuch size will be needed on a stack for a function.
	int currentScopeRegCount;
	
	TACReg generateRegister();

	void getParams(NODE* node, std::vector<TACReg>& params);
		
public:
	std::vector<TACReg> globalRegisters;
	std::vector<TACReg> localRegisters;
	std::vector<TAC> codes;
	std::vector<pair<int, int>> basicBlocks;

	// Generate a three adress code for C--
	TACReg generateCode(NODE* node);

	void print();

	// Optmise the TAC using basic block optimisation.
	void optimise();

	IRContainer();
};

