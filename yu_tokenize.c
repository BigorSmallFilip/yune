#include "yu_parse.h"

#include <ctype.h>



typedef enum
{
	CT_NULL,		/* Default type or any char not recognised */
	CT_ALPHA,		/* Letter or underscore */
	CT_DIGIT,
	CT_POINT,
	CT_OPERATOR,
	CT_SEPARATOR,
	CT_SPACE,		/* Space or tab */
	CT_QUOTE,		/* Quotation marks for strings */
	CT_COMMENT,		/* Comments start with # and end with an endline */
	CT_ENDLINE,
	CT_TERMINATOR,          /* Char 0 */
} chartype;

#define USE_CHARTYPE_LUT
#ifdef USE_CHARTYPE_LUT

const unsigned char chartype_lut[256] =
{
	10,0, 0, 0, 0, 0, 0, 0, 0, 6, 9, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	6, 4, 7, 8, 0, 0, 4, 7, 5, 5, 4, 4, 5, 4, 3, 4,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 4, 4, 4, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 5, 0, 1,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 4, 5, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

#define check_chartype(c) (chartype_lut[(c)])

#define is_alpha(c) (chartype_lut[(c)] == CT_ALPHA)
#define is_digit(c) (chartype_lut[(c)] == CT_DIGIT)
#define is_point(c) (chartype_lut[(c)] == CT_POINT)
#define is_operator(c) (chartype_lut[(c)] == CT_OPERATOR)
#define is_separator(c) (chartype_lut[(c)] == CT_SEPARATOR)
#define is_quote(c) (chartype_lut[(c)] == CT_QUOTE)
#define is_endline(c) (chartype_lut[(c)] == CT_ENDLINE)
#define is_terminator(c) (chartype_lut[(c)] == CT_TERMINATOR)
#define is_invalid(c) (chartype_lut[(c)] == CT_NULL)

#else

#define is_alpha(c) (isalpha(c) || c == '_')
#define is_operator_char(c) (strchr("+-/*=<>!&|", c))
#define is_separator_char(c) (strchr("()[]{},", c))
#define is_quote_char(c) (c == '\"' || c == '\'')
#define is_invalid(c) (c < 0)

static chartype check_chartype(const char c)
{
	if (c < 0)                 return CT_NULL;
	if (is_alpha(c))           return CT_ALPHA;
	if (isdigit(c))            return CT_DIGIT;
	if (c == '.')              return CT_POINT;
	if (is_operator_char(c))   return CT_OPERATOR;
	if (is_separator_char(c))  return CT_SEPARATOR;
	if (isblank(c))            return CT_SPACE;
	if (c == '\n' || c == ';') return CT_ENDLINE; /* Semicolon acts the same as endline */
	if (is_quote_char(c))      return CT_QUOTE;
	if (c == '#')              return CT_COMMENT;
	return CT_NULL;
}

#endif



static Yu_Token* create_token()
{
	Yu_Token* token = Yu_Malloc(sizeof(Yu_Token)); /* Malloc instead of calloc since this will setup everything manually so no need to 0 out token */
	if (!token) return NULL;
	token->type = Yu_TT_NULL;
	token->keywordid = Yu_KW_NULL;
	token->operatorid = Yu_OP_NULL;
	token->separatorid = Yu_SP_NULL;
	token->string = NULL;
	token->lastonline = Yu_FALSE;
	token->firstonline = Yu_FALSE;
	token->inlinestatement = Yu_FALSE;
	token->linenum = -1;
	token->indentlevel = -1;
	return token;
}



#define syntax_error(format, ...)    \
	Yu_PushStateMessage(         \
	parser->main_state,          \
	Yu_MSGT_SYNTAXERROR,         \
	parser->linenum,             \
	parser->sourcecode_filename, \
	format,                      \
	__VA_ARGS__);                \
        parser->errorcount++
#define syntax_error(format, ...) (void)0

#define syntax_error(format, ...) printf(format ## " | Line: %i", __VA_ARGS__, parser->linenum)



static Yu_Token* read_alpha_token(Yu_ParserState* parser)
{
	int start = parser->charpos;
	int end = -1;
	for (int i = start + 1; i < parser->max_sourcecode_length; i++)
	{
		char c = parser->sourcecode[i];
		if (is_alpha(c) || is_digit(c))
			continue;
		else
		{
			end = i;
			break;
		}
		
	}
	Yu_Token* token = create_token();
	if (!token) return NULL;
	char* cutstring = Yu_CopyCutString(parser->sourcecode, start, end - start);
	if (!cutstring) goto fail;
	token->keywordid = Yu_GetKeywordID(cutstring);
	if (token->keywordid == Yu_KW_NULL)
	{
		token->type = Yu_TT_IDENTIFIER;
		token->string = cutstring;
	} else
	{
		token->type = Yu_TT_KEYWORD;
		token->string = NULL;
		Yu_Free(cutstring); /* Keyword doesn't need the string */
	}
	parser->charpos = end;
	return token;

fail:
	Yu_Free(token);
	return NULL;
}



static Yu_Token* read_number_token(Yu_ParserState* parser)
{
	int start = parser->charpos;
	int end = -1;
	Yu_Bool read_point = Yu_FALSE; /* If a decimal point has been reached */
	for (int i = start + 1; i < parser->max_sourcecode_length; i++)
	{
		char c = parser->sourcecode[i];
		if (is_digit(c))
			continue;
		else if (is_point(c))
		{
			if (read_point)
			{
				syntax_error("Multiple decimal points in one number");
				return NULL;
			}
			read_point = Yu_TRUE;
		} else
		{
			end = i;
			break;
		}
	}
	if (parser->sourcecode[end] == '.')
	{
		syntax_error("Number can't end with a decimal point");
		return NULL;
	}
	Yu_Token* token = create_token();
	if (!token) return NULL;
	char* cutstring = Yu_CopyCutString(parser->sourcecode, start, end - start);
	if (!cutstring) goto fail;
	token->type = Yu_TT_NUMBER;
	token->number = Yu_StringToFloat(cutstring, NULL);
	Yu_Free(cutstring);
	parser->charpos = end;
	return token;

fail:
	Yu_Free(token);
	return NULL;
}



static Yu_Token* read_operator_token(Yu_ParserState* parser)
{
	int start = parser->charpos;
	int end = -1;
	for (int i = start + 1; i < parser->max_sourcecode_length; i++)
	{
		char c = parser->sourcecode[i];
		if (is_operator(c))
			continue;
		else
		{
			end = i;
			break;
		}
	}
	Yu_Token* token = create_token();
	if (!token) return NULL;
	char* cutstring = Yu_CopyCutString(parser->sourcecode, start, end - start);
	if (!cutstring) goto fail_0;
	token->operatorid = Yu_GetOperatorID(cutstring);
	if (token->operatorid == Yu_OP_NULL) goto fail_1;
	token->type = Yu_TT_OPERATOR;
	parser->charpos = end;
	return token;

fail_1:
	Yu_Free(cutstring);
fail_0:
	Yu_Free(token);
	return NULL;
}



static Yu_Token* read_separator_token(Yu_ParserState* parser)
{
	Yu_SeparatorID id = Yu_GetSeparatorID(parser->sourcecode[parser->charpos]);
	if (id == Yu_OP_NULL) return NULL;
	Yu_Token* token = create_token();
	if (!token) return NULL;
	token->separatorid = Yu_GetSeparatorID(parser->sourcecode[parser->charpos]);
	token->type = Yu_TT_SEPARATOR;
	parser->charpos++;
	return token;
}



static Yu_Token* read_string_literal_token(Yu_ParserState* parser)
{
	int start = parser->charpos;
	int end = -1;
	for (int i = start + 1; i < parser->max_sourcecode_length; i++)
	{
		char c = parser->sourcecode[i];
		if (is_quote(c))
		{
			end = i + 1; /* Plus 1 to include quote */
			break;
		} else if (c == '\n' || is_terminator(c))
		{
			syntax_error("String does not have closing quotation marks");
			return NULL;
		} else
			continue;
	}
	Yu_Token* token = create_token();
	if (!token) return NULL;
	char* cutstring = Yu_CopyCutString(parser->sourcecode, start + 1, end - start - 2);
	if (!cutstring) goto fail;
	token->string = cutstring;
	token->type = Yu_TT_STRING;
	parser->charpos = end;
	return token;

fail:
	Yu_Free(token);
	return NULL;
}



void read_comment(Yu_ParserState* parser)
{
	int end = -1;
	for (int i = parser->charpos + 1; i < parser->max_sourcecode_length; i++)
	{
		char c = parser->sourcecode[i];
		if (is_endline(c) || is_terminator(c))
		{
			end = i;
			break;
		} else
			continue;
	}
	parser->charpos = end;
}



Yu_Bool Yu_TokenizeSourceCode(Yu_ParserState* parser)
{
	// This code generates the lut for the chartypes
	/*printf("The lookup table:\n");
	for (int i = 0; i < 128; i += 16)
	{
		for (int j = 0; j < 16; j++)
		{
			chartype t = check_chartype((char)(i + j));
			printf("%i, ", (int)t);
		}
		putchar('\n');
	}
	return Yu_FALSE;*/

	parser->charpos = 0;
	parser->linenum = 1;
	if (!Yu_InitVector(&parser->tokens)) return Yu_FALSE;
	short line_indentlevel = 0;
	Yu_Bool reading_indent = Yu_TRUE;
	Yu_Bool newstatement = Yu_TRUE;
	Yu_Bool continue_reading = Yu_TRUE;

	while (continue_reading)
	{
		//if ((unsigned int)parser->charpos >= parser->max_sourcecode_length)
		//{
		//	//syntax_error("Sourcecode exceeds max length of %i characters", parser->max_sourcecode_length);
		//	break;
		//}

		char c = parser->sourcecode[parser->charpos];

		Yu_Token* token = NULL;
		switch (check_chartype(c))
		{
		case CT_ALPHA:     token = read_alpha_token(parser); break;
		case CT_DIGIT:     token = read_number_token(parser); break;
		case CT_POINT:
		case CT_OPERATOR:  token = read_operator_token(parser); break;
		case CT_SEPARATOR: token = read_separator_token(parser); break;
		case CT_QUOTE:     token = read_string_literal_token(parser); break;
		case CT_COMMENT:   read_comment(parser); continue;
		case CT_ENDLINE:
		{
			/* If an endline char is found then the previous token gets the lastonline flag set */
			if (parser->tokens.count > 0)
			{
				Yu_Token* lasttoken = (Yu_Token*)parser->tokens.buffer[parser->tokens.count - 1];
				lasttoken->lastonline = Yu_TRUE;
			}
			/* Only real endlines reset the indentation, not semicolons which count as endlines */
			if (c == '\n')
			{
				parser->linenum++;
				line_indentlevel = 0;
				reading_indent = Yu_TRUE;
			}
			parser->charpos++;
			newstatement = Yu_TRUE;
			continue;
		}
		case CT_SPACE:
			if (reading_indent)
			{
				if (c == ' ') line_indentlevel++;
				else if (c == '\t') line_indentlevel += parser->tabsize;
			}
			parser->charpos++;
			continue;
		case CT_TERMINATOR:
			continue_reading = Yu_FALSE;
			goto exit_loop;

		default:
			syntax_error("Invalid character 0x%X", (unsigned char)c);
			parser->charpos = -1;
			break;
		}

		if (parser->charpos < 0) goto fail;

		if (token)
		{
			token->linenum = parser->linenum;
			if (reading_indent)
			{
				token->indentlevel = line_indentlevel;
				reading_indent = Yu_FALSE;
			} else
				if (newstatement)
					token->inlinestatement = Yu_TRUE;
			if (newstatement)
			{
				token->firstonline = Yu_TRUE;
				newstatement = Yu_FALSE;
			}
			Yu_PushBackVector(&parser->tokens, token);
		} else
			goto fail;
	}
exit_loop: {}

	/* Last token in file gets the lastonline flag set */
	Yu_Token* lasttoken = Yu_VectorBack(parser->tokens, Yu_Token*);
	if (lasttoken) lasttoken->lastonline = Yu_TRUE;

	return Yu_TRUE;

fail:
	for (int i = 0; i < parser->tokens.count; i++)
	{
		Yu_FreeToken(parser->tokens.buffer[i]);
	}
	Yu_Free(parser->tokens.buffer);
	return Yu_FALSE;
}



void Yu_PrintSourceCodeTokens(Yu_ParserState* parser)
{
	if (!parser) return;
	if (!parser->tokens.buffer || parser->tokens.count <= 0) return;

	int linenum = 1;
	printf(" === Tokens ===\n");
	for (int i = 0; i < (int)parser->tokens.count; i++)
	{
		Yu_Token* token = parser->tokens.buffer[i];
		if (token->firstonline)
		{
			printf("%5i | ", token->linenum);
		}
		for (int intent = 0; intent < token->indentlevel; intent++)
			putchar(' ');
		if (token->firstonline) printf("\x1B[1;32m");
		//if (token->firstonline) printf("\x1B[1;34m");
		Yu_PrintToken(token);
		printf("\x1B[0m");
		putchar(token->lastonline ? '\n' : ' ');
	}
	putchar('\n');
}
