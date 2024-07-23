#include "yu_parse.h"



void Yu_PrintToken(const Yu_Token* token)
{
	if (!token)
	{
		printf("(null token)");
		return;
	}
	switch (token->type)
	{
	case Yu_TT_KEYWORD:
		if (token->keywordid >= 0 && token->keywordid < Yu_NUM_KEYWORDS)
			printf("%s", yu_keyword_strings[token->keywordid]);
		else
			printf("(invalid keywordid: %d)", token->keywordid);
		break;
	case Yu_TT_OPERATOR:
		if (token->operatorid >= 0 && token->operatorid < Yu_NUM_OPERATORS)
			printf("%s", yu_operator_strings[token->operatorid]);
		else
			printf("(invalid operatorid: %d)", token->operatorid);
		break;
	case Yu_TT_SEPARATOR:
		if (token->separatorid >= 0 && token->separatorid < Yu_NUM_SEPARATORS)
			printf("%c", yu_separator_chars[token->separatorid]);
		else
			printf("(invalid separatorid: %d)", token->separatorid);
		break;
	case Yu_TT_NUMBER: printf("%llf", token->number); break;
	case Yu_TT_IDENTIFIER: printf("%s", token->string); break;
	case Yu_TT_STRING: printf("\"%s\"", token->string); break;
	default:
		break;
	}
}

void Yu_FreeToken(Yu_Token* token)
{
	if (!token) return;
	if (token->type == Yu_TT_STRING || token->type == Yu_TT_IDENTIFIER)
		Yu_Free(token->string);
	Yu_Free(token);
}



Yu_CodeBlock* Yu_ParseSourceCode(Yu_State* state, const char* sourcecode)
{
	Yu_ParserState parser = { 0 };
	parser.main_state = state;
	parser.tabsize = state->tabsize;
	parser.sourcecode = sourcecode;
	parser.filename = NULL;
	parser.max_sourcecode_length = 100000;

	if (!Yu_TokenizeSourceCode(&parser, sourcecode)) return NULL;

	Yu_PrintSourceCodeTokens(&parser);
	
	return NULL;
}
