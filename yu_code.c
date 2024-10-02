#include "yu_code.h"

const char* yu_keyword_strings[Yu_NUM_KEYWORDS] =
{
	"if",               "else",
	"for",              "in",
	"do",               "while",
	"function",         "return",
	"false",            "true"
};

Yu_KeywordID Yu_GetKeywordID(const char* string)
{
	if (!string) return Yu_KW_NULL;
	for (int i = 0; i < Yu_NUM_KEYWORDS; i++)
		if (strcmp(string, yu_keyword_strings[i]) == 0)
			return (Yu_KeywordID)i;
	return Yu_KW_NULL;
}



const char* yu_operator_strings[Yu_NUM_OPERATORS] =
{
	"+",
	"-",
	"*",
	"/",
	"//",
	"**",
	"%",

	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"//=",
	"**=",
	"%=",

	"-", /* Same char as OP_SUB but is placed later in array */
	"!",
	"&",
	"|",
	"^",

	"==",
	"!=",
	"<",
	">",
	"<=",
	">=",

	".",

	"[]", /* Array access has a special case using separators */
	"in",
};

const char* yu_operatorid_strings[Yu_NUM_OPERATORS] =
{
	"OP_ADD",
	"OP_SUB",
	"OP_MUL",
	"OP_DIV",
	"OP_IDIV",
	"OP_EXP",
	"OP_MOD",

	"OP_ASSIGN",
	"OP_ASSIGNADD",
	"OP_ASSIGNSUB",
	"OP_ASSIGNMUL",
	"OP_ASSIGNDIV",
	"OP_ASSIGNIDIV",
	"OP_ASSIGNMOD",

	"OP_NEGATIVE",
	"OP_NOT",
	"OP_AND",
	"OP_OR",
	"OP_XOR",

	"OP_EQUALITY",
	"OP_INEQUALITY",
	"OP_LESS",
	"OP_GREATER",
	"OP_LESSEQUAL",
	"OP_GREATEREQUAL",

	"OP_MEMBERACCESS",
	"OP_ARRAYACCESS",
	"OP_IN",
};



Yu_OperatorID Yu_GetOperatorID(const char* string)
{
	if (!string) return Yu_OP_NULL;
	for (int i = 0; i <= Yu_OP_MEMBERACCESS; i++)
		if (strcmp(string, yu_operator_strings[i]) == 0)
			return (Yu_OperatorID)i;
	return Yu_OP_NULL;
}



const char yu_separator_chars[Yu_NUM_SEPARATORS] =
{
	'(',
	')',
	'[',
	']',
	'{',
	'}',
	',',
};

Yu_SeparatorID Yu_GetSeparatorID(const char c)
{
	for (int i = 0; i < Yu_NUM_SEPARATORS; i++)
		if (c == yu_separator_chars[i])
			return (Yu_SeparatorID)i;
	return Yu_SP_NULL;
}



void Yu_PrintExprNode(const Yu_ExprNode* node)
{
	if (!node)
	{
		printf("(null)");
		return;
	}

	switch (node->type)
	{
	case Yu_ET_OPERATOR:
		if (node->u_op.id >= 0 && node->u_op.id < Yu_NUM_OPERATORS)
			printf("%s", yu_operatorid_strings[node->u_op.id]);
		else
			printf("(invalid operatorid: '%d'", (int)node->u_op.id);
		break;
	case Yu_ET_BOOL:         printf(node->u_bool ? "true" : "false"); break;
	case Yu_ET_NUMBER:       printf("%llf", node->u_number); break;
	case Yu_ET_STRING:       printf("\"%s\"", node->u_string.str); break;
	case Yu_ET_VECTOR:       printf("vector%i", node->u_vector.num_components); break;
	case Yu_ET_LIST:         printf("list[%i]", node->u_list.elements.count); break;
	case Yu_ET_OBJECT:       printf("{%i}", node->u_object.members.count); break;
	case Yu_ET_FUNCTION:     printf("<function>"); break;
	case Yu_ET_VARIABLE:     printf("%s", node->u_variable.str); break;
	case Yu_ET_FUNCTIONCALL: printf("function call"); break;
	default:                 printf("(invalid node type: '%d')", (int)node->type); break;
	}
}

void Yu_FreeExprNode(Yu_ExprNode* node)
{
	printf("unimplemented free");
}
