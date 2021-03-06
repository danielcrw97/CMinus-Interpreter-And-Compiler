#include <iostream>
extern "C"
{
	#include <ctype.h>
	#include "C.tab.h"
}
#include <string>
#include <cstdio>
#include <fstream>
#include "SymbolTable.h"
#include "interpreter.h"
#include "IRContainer.h"

#ifdef _WIN32
	#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifdef _WIN64
	#define _CRT_SECURE_NO_DEPRECATE
#endif

std::string named(int t)
{
	static char b[100];
	if (t >= -1 && t <= 255)
	{
		if (isgraph(t) || t == ' ') {
			sprintf(b, "%c", t);
			return b;
		}
	}
	
	switch (t) {
	default: return "???";
	case IDENTIFIER:
		return "id";
	case CONSTANT:
		return "constant";
	case STRING_LITERAL:
		return "string";
	case LE_OP:
		return "<=";
	case GE_OP:
		return ">=";
	case EQ_OP:
		return "==";
	case NE_OP:
		return "!=";
	case EXTERN:
		return "extern";
	case AUTO:
		return "auto";
	case INT:
		return "int";
	case VOID:
		return "void";
	case APPLY:
		return "apply";
	case LEAF:
		return "leaf";
	case IF:
		return "if";
	case ELSE:
		return "else";
	case WHILE:
		return "while";
	case CONTINUE:
		return "continue";
	case BREAK:
		return "break";
	case RETURN:
		return "return";
	}
}

void print_leaf(NODE *tree, int level)
{
	TOKEN *t = (TOKEN *)tree;
	int i;
	for (i = 0; i < level; i++) putchar(' ');
	if (t->type == CONSTANT) printf("%d\n", t->value);
	else if (t->type == STRING_LITERAL) printf("\"%s\"\n", t->lexeme);
	else if (t) puts(t->lexeme);
}

void print_tree0(NODE *tree, int level)
{
	int i;
	if (tree == NULL) return;
	if (tree->type == LEAF) {
		print_leaf(tree->left, level);
	}
	else {
		for (i = 0; i < level; i++) putchar(' ');
		std::cout << named(tree->type) << std::endl;
		/*       if (tree->type=='~') { */
		/*         for(i=0; i<level+2; i++) putchar(' '); */
		/*         printf("%p\n", tree->left); */
		/*       } */
		/*       else */
		print_tree0(tree->left, level + 2);
		print_tree0(tree->right, level + 2);
	}
}

void print_tree(NODE *tree)
{
	print_tree0(tree, 0);
}


extern "C" {
	int yydebug;
	NODE* yyparse();
	NODE* ans;
	void init_symbtable();
	TOKEN** symbtable;
	TOKEN* lookup_token(const char* s);
	void yyrestart(FILE* input_file);
}

int main(int argc, char** argv)
{
	NODE* tree;
	if (argc > 1 && strcmp(argv[1], "-d") == 0) yydebug = 1;
	init_symbtable();
	printf("--C COMPILER\n");
	FILE *f = fopen("../input.c", "r");
	yyrestart(f);
	yyparse();
	tree = ans;

	printf("parse finished with %p\n", tree);
	print_tree(tree);

	// Now that we have the AST we can build a SymbolTable with a lot more information.
	// Though this does require an extra traversal through the AST, this is a more organised
	// solution which helps us get all the information.
	SymbolTable::buildGlobalTable(tree);
	
	bool COMPILE_TO_TAC = false;

	// Interpret or compile depending on the given option. 
	// Use true here to compile into a TAC rather than interpret.
	// Will change this to true to compile rather than interpret.
	if (COMPILE_TO_TAC)
	{
		std::unique_ptr<IRContainer> container = make_unique<IRContainer>();
		container->generateCode(tree);
	}
	else 
	{
		std::unique_ptr<Interpreter> interpreter = make_unique<Interpreter>();
		Value mainReturnValue;
		try
		{
			interpreter->interpret(tree, mainReturnValue);
		}
		catch (std::exception& e)
		{
			cout << "ERROR: " << endl;
			cout << e.what() << endl;
		}
	}
}
