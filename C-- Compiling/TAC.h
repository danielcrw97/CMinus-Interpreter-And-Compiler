#pragma once

#include <string>
#include "TACReg.h"

enum class TACOperator
{
	ADD,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MOD,
	ASSIGN,
	FUNCTIONDEC,
	BEGINFUNC,
	CALL,
	RETURNINT,
	ENDFUNC,
	PARAM,
	NONE
};

// A representation of a quadruple TAC.
struct TAC {
	TACOperator op;
	TACReg assignedReg;
	TACReg var1;
	TACReg var2;
	
	// Used for functions and jump tos
	std::string name;

	void print(std::ostream const& stream)
	{
		switch (op)
		{
		}
	}
};
