#pragma once

#include <string>

/* 
	Use name of scopes to search for the correct one - slightly more expensive than ids but also more readable.
	Names use the following pattern - 

	int main()
	{
		int add(int a, int b)
		{
			if(a != b)
			{
				return a + b;
			}
		}
	}

	The scope for the add function is global_main_add
	And the scope for the if block is global_main_add_if1

	 This helps prune searching for scopes within the symbol table.
*/

struct ScopeData
{
	// Used to create dynamic scope names.
	int ifCount = 0;
	int whileCount = 0;
	bool isGlobal = false;

	std::string name;
	int depth;

	bool operator==(ScopeData const& right) const
	{
		// If they are both global the name is irrelevant.
		if (isGlobal && right.isGlobal)
		{
			return true;
		}
		else if (isGlobal != right.isGlobal)
		{
			return false;
		}
		else if (name == right.name)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};