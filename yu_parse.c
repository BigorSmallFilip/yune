#include "yu_parse.h"



#define syntax_error(msg) printf(msg)



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



Yu_Bool parse_code_block(Yu_ParserState* state, Yu_CodeBlock** o_block);



const Yu_Token* get_current_token(const Yu_ParserState* state)
{
	if (state->curtoken >= state->tokens.count) return NULL;
	return state->tokens.buffer[state->curtoken];
}

const Yu_Token* get_next_token(Yu_ParserState* state)
{
	state->curtoken++;
	if (state->curtoken >= state->tokens.count) return NULL;
	return state->tokens.buffer[state->curtoken];
}



Yu_Bool parse_expression(Yu_ParserState* state, Yu_Expression** o_expression)
{
	printf("Expression :");
	Yu_PrintToken(get_current_token(state));
	printf(": {\n");

	state->curtoken += 3;
	Yu_Expression* expression = Yu_AllocType(Yu_Expression);
	*o_expression = expression;

	printf("} Expression\n");
	return Yu_TRUE;
}



/*
** Parses statements beginning with the 'if' keyword. It also parses the blocks
** and optional 'else' blocks. 'else if' chains are parsed recursively through
** the else blocks.
** 
** @param parser Parser state
** @param o_statement Output variable. Should be a pointer to a pointer to an
** allocated, uninitialized Yu_Statement struct
*/
Yu_Bool parse_if_statement(Yu_ParserState* state, Yu_Statement** o_statement)
{
	/* if <condition> : <block_ontrue> (else <block_onfalse>) */
	if (!get_next_token(state)) return Yu_FALSE; /* Go to token after if */

	Yu_Expression* condition = NULL;
	if (!parse_expression(state, &condition));

	Yu_Token* token = get_current_token(state);
	if (!token->firstonline) goto fail;

	Yu_CodeBlock* block_ontrue = NULL;
	if (!parse_code_block(state, &block_ontrue)) goto fail;
	
	if (!block_ontrue) return Yu_TRUE; /* No ontrue block so simply continue */
	

	return Yu_TRUE;

fail:
	/* Free everything */
	Yu_Free(NULL);
	return Yu_FALSE;
}



/*
** Parses statements beginning with the 'for' keyword and all their variants.
** 
** @param parser Parser state
** @param o_statement Output variable. Should be a pointer to a pointer to an
** allocated, uninitialized Yu_Statement struct
*/
Yu_Bool parse_for_statement(Yu_ParserState* parser, Yu_Statement** o_statement)
{

}



/*
** Parses statements beginning with the 'while' keyword. 'do while' loops are
** handled separately.
**
** @param parser Parser state
** @param o_statement Output variable. Should be a pointer to a pointer to an
** allocated, uninitialized Yu_Statement struct
*/
Yu_Bool parse_while_statement(Yu_ParserState* parser, Yu_Statement** o_statement)
{

}



/*
** Parses statements beginning with the 'do' keyword.
**
** @param parser Parser state
** @param o_statement Output variable. Should be a pointer to a pointer to an
** allocated, uninitialized Yu_Statement struct
*/
Yu_Bool parse_do_while_statement(Yu_ParserState* parser, Yu_Statement** o_statement)
{

}



/*
** Parses statements beginning with the 'while' keyword. 'do while' loops are
** handled separately.
**
** @param parser Parser state
** @param o_statement Output variable. Should be a pointer to a pointer to an
** allocated, uninitialized Yu_Statement struct
*/
Yu_Bool parse_return_statement(Yu_ParserState* parser, Yu_Statement** o_statement)
{

}




Yu_Bool parse_expression_statement(Yu_ParserState* state, Yu_Statement** o_statement)
{
	/* <expression> */
	parse_expression(state, &(*o_statement)->stmt_expr.expression);
}



Yu_Bool parse_statement(Yu_ParserState* state, Yu_Statement** o_statement)
{
	printf("Statement :");
	Yu_PrintToken(get_current_token(state));
	printf(": {\n");

	const Yu_Token* token = get_current_token(state);
	if (!token) return Yu_FALSE;

	Yu_Statement* statement = Yu_AllocType(Yu_Statement);
	if (!statement) return Yu_FALSE;
	statement->indentlevel = token->indentlevel;

	switch (token->keywordid)
	{
	case Yu_KW_IF:
		if (!parse_if_statement(state, &statement)) goto fail;
		break;
	default:
		if (!parse_expression_statement(state, &statement)) goto fail;
		break;
	}

	*o_statement = statement;

	printf("} Statement\n");
	return Yu_TRUE;

fail:

	return Yu_FALSE;
}


/*
** Parses statements beginning with the 'while' keyword. 'do while' loops are
** handled separately.
**
** @param parser Parser state
** @param parent_indentlevel All statements in this block must have indents
** that are higher than the parent. Inline statements ignore this.
** @param o_statement Output variable. This function is what allocates the
** Yu_CodeBlock struct so pass a pointer to a null pointer
*/
Yu_Bool parse_code_block(Yu_ParserState* state, short parent_indentlevel, Yu_CodeBlock** o_block)
{
	printf("Block :");
	Yu_PrintToken(get_current_token(state));
	printf(": {\n");

	Yu_CodeBlock* block = Yu_AllocType(Yu_CodeBlock);
	if (!block) return Yu_FALSE;
	
	const Yu_Token* token = get_current_token(state);
	short block_indentlevel = token->indentlevel;

	while (1) /* TODO Maybe add a maximum statements per block rule */
	{
		const Yu_Token* stmt_token = get_current_token(state);
		if (!stmt_token->inlinestatement)
		{
			/* Token isn't inline */
			if (stmt_token->indentlevel < block_indentlevel)
				break; /* Statement is outside of this block */
			if (stmt_token->indentlevel > block_indentlevel)
			{
				/* Unexpected indent */
				syntax_error("Unexpected indent");
				goto fail;
			}
		}
		Yu_Statement* statement = NULL;
		if (!parse_statement(state, &statement))
			goto fail;
		if (statement)
			if (!Yu_PushBackVector(&block->statements, statement)) goto fail;
		else
			break;
	}

	*o_block = block;

	printf("} Block\n");
	return Yu_TRUE;

fail:
	/* Free everything */
	return Yu_FALSE;
}



Yu_CodeBlock* parse_main_block(Yu_ParserState* state)
{
	state->curtoken = 0;
	Yu_CodeBlock* mainblock = NULL;
	parse_code_block(state, 0, &mainblock);
	return mainblock;
}



Yu_CodeBlock* Yu_ParseSourceCode(Yu_State* state, const char* sourcecode)
{
	Yu_ParserState parser = { 0 };
	parser.main_state = state;
	parser.tabsize = state->tabsize;
	parser.sourcecode = sourcecode;
	parser.filename = NULL;
	parser.max_sourcecode_length = 100000;

	if (!Yu_TokenizeSourceCode(&parser)) return NULL;

	Yu_PrintSourceCodeTokens(&parser);

	Yu_CodeBlock* mainblock = parse_main_block(&parser);
	
	return mainblock;
}
