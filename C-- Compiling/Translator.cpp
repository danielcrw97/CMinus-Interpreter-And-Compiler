#include "Translator.h"
#include <fstream>
#include "IRContainer.h"
#include "TAC.h"

using namespace std;

// Used to make the code neater
int textDepth = 0;

// Used to be able to tell how many procedures deep we are in, as nested subroutines require extra work in MIPS assembly.
int procedureDepth = 0;

bool inMain = false;

string functionOffset = "    ";

/*
void translate(IRContainer & ir)
{
	std::ofstream stream;
	stream.open("./output.asm");

	stream << ".globl main" << endl;

	int codesIndex = 0;

	for (TAC code : ir.codes)
	{
		TACOperator op = code.op;
		switch (op)
		{
		case(TACOperator::FUNCTIONDEC):
		{
			if (code.name == "main")
			{
				inMain = true;
			}
			stream << code.name << endl;
			++textDepth;
			break;
		}
		case(TACOperator::ADD):
		{

		}
		case(TACOperator::ASSIGN):
		{

		}
		case(TACOperator::CALL):
		{

		}
		case(TACOperator::PARAM):
		{
			int argumentNumber = 0;

		}
		case(TACOperator::ENDFUNC):
		{
			stream << endl;
			--textDepth;
			if (inMain)
			{
				// End program.
				stream << functionOffset << "li $v0 10" << endl;
				stream << "syscall" << endl;
			}
			break;
		}
		++codesIndex;
		}
}
*/
