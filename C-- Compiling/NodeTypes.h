#pragma once
// C.tab.h does not give you all the node types, so put them all in here.
class NodeTypes
{
public:
	static constexpr int LEAF = 271;
	static constexpr int IDENTIFIER = 258;
	static constexpr int CONSTANT = 259;
	static constexpr int STRING_LITERAL = 260;

	// The following give type information about a variable.
	static constexpr int VOID = 268;
	static constexpr int FUNCTION = 269;
	static constexpr int INT = 267;

	static constexpr int FUNCTION_DECLARATION = 68;
	static constexpr int FUNCTION_DATA = 100;
	static constexpr int FUNCTION_PARAMS = 70;
	static constexpr int APPLY = 270;

	static constexpr int DECLARATION = 126;
	static constexpr int LIST = ',';
	static constexpr int SEQUENCE = 59;
	static constexpr int ASSIGNMENT = 61;

	static constexpr int PLUS = '+';
	static constexpr int MINUS = '-';
	static constexpr int MULTIPLY = '*';
	static constexpr int DIVIDE = '/';
	static constexpr int MOD = '%';
	static constexpr int LESS_OP = 60;
	static constexpr int GREATER_OP = 62;
	static constexpr int GE_OP = 262;
	static constexpr int LE_OP = 261;
	static constexpr int EQ_OP = 263;
	static constexpr int NE_OP = 264;

	static constexpr int IF = 272;
	static constexpr int ELSE = 273;
	static constexpr int WHILE = 274;
	static constexpr int RETURN = 277;
};

