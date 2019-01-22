#pragma once

enum class RegType {
	R_REG,
	R_GLOBAL,
	R_LOCAL,
	R_CONST,
	R_NONE
};

using namespace std;

// A register that is nothing by default.
struct TACReg {
	RegType type = RegType::R_NONE;
	int regValue;

	bool operator==(TACReg const& right) const
	{
		return (type == right.type) && (regValue == right.regValue);
	}

	bool static equals(TACReg const& left, TACReg const& right)
	{
		return left == right;
	}
};

struct RegCompare {
	bool operator() (const TACReg& lhs, const TACReg& rhs) const {
		return lhs == rhs;
	}
};