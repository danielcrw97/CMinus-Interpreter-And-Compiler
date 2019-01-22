#pragma once
#include <functional>
#include "FunctionData.h"

// Either contains an int pointer or a function pointer
struct Value
{
	int valueType;

	std::shared_ptr<FunctionData> func;
	std::shared_ptr<int> integer;

	// Flag to check whether this is a value being returned.
	bool returningValue = false;
};