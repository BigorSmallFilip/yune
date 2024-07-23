#ifndef __Yu_PARSE_H__
#define __Yu_PARSE_H__

#include "yu_utility.h"
#include "yu_state.h"
#include "yu_code.h"



enum
{
	Yu_TT_KEYWORD,
	Yu_TT_OPERATOR,
	Yu_TT_SEPARATOR,
	Yu_TT_NUMBER,
	Yu_TT_STRING,
	Yu_TT_IDENTIFIER,
	Yu_NUM_TOKENTYPES,
	Yu_TT_NULL = -1
};
typedef char Yu_TokenType;

typedef struct Yu_Token
{
	Yu_TokenType   type;
	Yu_KeywordID   keywordid;
	Yu_OperatorID  operatorid;
	Yu_SeparatorID separatorid;
	union
	{
		char* string;
		Yu_Float number;
	};
	int linenum;
	short indentlevel;          /* The indentation of the line of this token if it is the first on the line. -1 if not the first token on the line */
	Yu_Bool lastonline;         /* If this token is the last on a line. For expressions ending without semicolons */
	Yu_Bool firstonline;        /* For indentation syntax */
} Yu_Token;

void Yu_PrintToken(const Yu_Token* token);

void Yu_FreeToken(Yu_Token* token);



typedef struct
{
	Yu_State* main_state;
	const char* filename;
	const char* sourcecode;
	Yu_Size max_sourcecode_length;
	int tabsize;            /* Number of spaces equivalent to a tab. For indentation */
	int charpos;            /* Lexer character index */
	int linenum;            /* The current line being read by the lexer */
	int errorcount;
	Yu_Vector tokens;       /* Vector of Yu_Tokens. This is set by the Yu_TokenizeSourceCode function */
	int curtoken;           /* Parser token index */
} Yu_ParserState;

Yu_Bool Yu_TokenizeSourceCode(Yu_ParserState* state);

void Yu_PrintSourceCodeTokens(Yu_ParserState* parser);

Yu_CodeBlock* Yu_ParseSourceCode(Yu_State* state, const char* sourcecode);

#endif
