#ifndef __Yu_CODE_H__
#define __Yu_CODE_H__

#include "yu_utility.h"
#include "yu_state.h"



typedef char Yu_KeywordID;
enum
{
	Yu_KW_IF,	    Yu_KW_ELSE,
	Yu_KW_FOR,          Yu_KW_IN,
	Yu_KW_DO,           Yu_KW_WHILE,
	Yu_KW_FUNCTION,     Yu_KW_RETURN,
	Yu_KW_FALSE,        Yu_KW_TRUE,
	Yu_NUM_KEYWORDS,
	Yu_KW_NULL = -1, /* Invalid or non keyword */
};
#define Yu_IsValidKeyword(id) ((id) >= 0)
#define Yu_AssertValidKeyword(id) Yu_Assert((id) >= 0 && (id) < Yu_NUM_KEYWORDS)
extern const char* yu_keyword_strings[Yu_NUM_KEYWORDS];

/*
** @brief Checks the keyword id of a string.
** @param string String to check.
** @return The ID of the keyword or Yu_KW_NULL if not a keyword.
*/
Yu_KeywordID Yu_GetKeywordID(const char* string);



typedef char Yu_OperatorID;
enum
{
	Yu_OP_ADD,          /* '+' */
	Yu_OP_SUB,          /* '-' */
	Yu_OP_MUL,          /* '*' */
	Yu_OP_DIV,          /* '/' */
	Yu_OP_IDIV,         /* '//' */
	Yu_OP_EXP,          /* '**' */
	Yu_OP_MOD,          /* '%' */
	
	Yu_OP_ASSIGN,       /* = */
	Yu_OP_ASSIGNADD,    /* += */
	Yu_OP_ASSIGNSUB,    /* -= */
	Yu_OP_ASSIGNMUL,    /* *= */
	Yu_OP_ASSIGNDIV,    /* /= */
	Yu_OP_ASSIGNIDIV,   /* //= */
	Yu_OP_ASSIGNEXP,    /* **= */
	Yu_OP_ASSIGNMOD,    /* %= */

	Yu_OP_NEGATIVE,     /* '-' Unary negative */
	Yu_OP_NOT,          /* '!' Logical not */
	Yu_OP_AND,          /* '&' Logical and */
	Yu_OP_OR,           /* '|' Logical or */

	Yu_OP_EQUALITY,     /* '==' */
	Yu_OP_INEQUALITY,   /* '!=' */
	Yu_OP_LESS,         /* '<'  */
	Yu_OP_GREATER,      /* '>'  */
	Yu_OP_LESSEQUAL,    /* '<=' */
	Yu_OP_GREATEREQUAL, /* '>=' */

	Yu_OP_MEMBERACCESS, /* '.' */

	/* The following operators are not used for lexing as they do not contain 
	   regular operator characters. They are here only for parsing and operatorIDs */

	/* Array access uses separators instead of operator chars */
	Yu_OP_ARRAYACCESS,  /* '[]' */

	/* 'in' is a keyword but is executed with an OP instruction */
	Yu_OP_IN,           /* in */

	Yu_NUM_OPERATORS,
	Yu_OP_NULL = -1,    /* Invalid or non operator */
};
#define Yu_IsValidOperator(id) ((id) >= 0)
#define Yu_AssertValidOperator(id) Yu_Assert((id) >= 0 && (id) < Yu_NUM_OPERATORS)
extern const char* yu_operator_strings[Yu_NUM_OPERATORS];
extern const char* yu_operatorid_strings[Yu_NUM_OPERATORS];
extern const unsigned char yu_operator_precedence[Yu_NUM_OPERATORS];

#define Yu_OpPrecedence(op)	(yu_operator_precedence[op])

/*
** @brief Checks the operator id of a string. This does not detect special keywords.
** They are unary negative '-', list access '[]' and the 'in' keyword.
** @param string String to check.
** @return The ID of the operator or Yu_OP_NULL if not operator.
*/
Yu_OperatorID Yu_GetOperatorID(const char* string);

#define Yu_IsAssignmentOp(op)   ((op) >= Yu_OP_ASSIGN   && (op) <= Yu_OP_ASSIGNDIV)
#define Yu_IsLogicalOp(op)      ((op) >= Yu_OP_NOT      && (op) <= Yu_OP_XOR)
#define Yu_IsRelationalOp(op)   ((op) >= Yu_OP_EQUALITY && (op) <= Yu_OP_GREATEREQUAL)
#define Yu_IsArithmeticOp(op)   ((op) >= Yu_OP_ADD      && (op) <= Yu_OP_MOD)
#define Yu_IsUnaryOp(op)        ((op) == Yu_OP_NOT      || (op) == Yu_OP_NEGATIVE)



typedef char Yu_SeparatorID;
enum
{
	Yu_SP_LPAREN,   /* '(' */
	Yu_SP_RPAREN,   /* ')' */
	Yu_SP_LBRACKET,	/* '[' */
	Yu_SP_RBRACKET,	/* ']' */
	Yu_SP_LBRACE,   /* '{' */
	Yu_SP_RBRACE,   /* '}' */
	Yu_SP_COMMA,    /* ',' */
	Yu_NUM_SEPARATORS,
	Yu_SP_NULL = -1,	/* Invalid or non separator */
};
#define Yu_IsValidSeparator(id) ((id) >= 0)
#define Yu_AssertValidSeparator(id) Yu_Assert((id) >= 0 && (id) < Yu_NUM_SEPARATORS)
extern const char yu_separator_chars[Yu_NUM_SEPARATORS];

/*
** @brief Checks the separator id of a char.
** @param c Character to check.
** @return The ID of the separator or Yu_SP_NULL if not separator char.
*/
Yu_SeparatorID Yu_GetSeparatorID(const char c);



typedef char Yu_ExprNodeType;
enum
{
	Yu_ET_OPERATOR,
	Yu_ET_BOOL,
	Yu_ET_NUMBER,
	Yu_ET_STRING,
	Yu_ET_VECTOR,
	Yu_ET_LIST,
	Yu_ET_OBJECT,
	Yu_ET_FUNCTION,
	Yu_ET_VARIABLE,
	Yu_ET_FUNCTIONCALL,
	Yu_NUM_EXPRNODETYPES,
	Yu_ET_NULL = -1,
};
extern const char* yu_exprnodetype_names[Yu_NUM_EXPRNODETYPES];

/* Nodes that make up expression trees */
typedef struct Yu_ExprNode
{
	Yu_ExprNodeType type;
	union
	{
		struct
		{
			Yu_OperatorID id;
			struct Yu_ExprNode* left; /* Left child */
			struct Yu_ExprNode* right; /* Right child */
		} u_op;
		Yu_Bool u_bool;
		Yu_Float u_number;
		struct
		{
			char* str;
			Yu_Hash hash;
			Yu_Vector interpolation; /* All subexpressions to be evaluated and inserted into the string literal upon loading */
			/* Example: str = "name is {person.name}" */
		} u_string;
		struct
		{
			char num_components; /* 2, 3 or 4 */
			struct Yu_ExprNode* comp0;
			struct Yu_ExprNode* comp1;
			struct Yu_ExprNode* comp2;
			struct Yu_ExprNode* comp3;
		} u_vector;
		struct
		{
			Yu_Vector elements; /* Vector of Yu_ExprNodes */
		} u_list;
		struct
		{
			Yu_Vector members; /* Vector of Yu_ExprObjectMembers */
		} u_object;
		struct
		{
			Yu_Vector parameters; /* Vector of Yu_ExprFunctionParameter */
			struct Yu_CodeBlock* functionblock;
		} u_function;
		struct
		{
			char* str;
			Yu_Hash hash;
		} u_variable;
		struct
		{
			char* identifier;
			int numargs;
			struct Yu_Expression* args; /* Array of arguments */
		} functioncall;
	};
} Yu_ExprNode;

/* Used for object literals by expression nodes */
typedef struct
{
	char* key;
	Yu_Hash hash;
	Yu_ExprNode* value;
} Yu_ExprObjectMember;

typedef struct
{
	char* name;
	Yu_Hash hash;
	Yu_ExprNode* defaultvalue;
	/* Type hmmm? */
} Yu_ExprFunctionParameter;

void Yu_PrintExprNode(const Yu_ExprNode* node);

void Yu_FreeExprNode(Yu_ExprNode* node);

typedef struct
{
	Yu_Vector nodes;
	Yu_ExprNode* topnode;
} Yu_Expression;

void Yu_PrintExpressionTree(const Yu_Expression* expr);

/*
** @brief Frees an expression and all its nodes recursively
** @param expr Expression to free.
*/
void Yu_FreeExpression(Yu_Expression* expr);



/*
** @brief Code blocks are containers for a list statements.
** These are the statements that are in the same scope depth and are executed in order.
** Scopes contain code blocks and are executed recursively from other code blocks.
*/
typedef struct Yu_CodeBlock
{
	Yu_Vector statements;
	struct Yu_CodeBlock* parent; /* Used to go up in the tree to the top node */
	char* filename; /* Only written in the global scope, top node */
} Yu_CodeBlock;

void Yu_PrintCodeTree(const Yu_CodeBlock* block);

/*
** @brief Frees a code block and all its child nodes recursively.
** @param block Block to free.
*/
void Yu_FreeCodeBlock(Yu_CodeBlock* block);



typedef enum
{
	Yu_ST_EXPRESSION,
	Yu_ST_IF,
	Yu_ST_FOR,
	Yu_ST_WHILE,
	Yu_ST_DOWHILE,
	Yu_ST_RETURN,
	Yu_NUM_STATEMENTTYPES
} Yu_StatementType;
extern const char* yu_statementtype_names[Yu_NUM_STATEMENTTYPES];

typedef struct Yu_Statement
{
	Yu_StatementType type;
	union
	{
		struct
		{
			Yu_Expression* expression;
		} stmt_expr;
		struct
		{
			Yu_Expression* expression;
		} stmt_return;
		struct
		{
			Yu_Expression* condition;
			Yu_CodeBlock* block_ontrue;
			Yu_CodeBlock* block_onfalse;
		} stmt_if;
		struct
		{
			Yu_Expression* init;
			Yu_Expression* condition;
			Yu_Expression* loop;
			Yu_CodeBlock* block;
		} stmt_for;
		struct
		{
			Yu_Expression* condition;
			Yu_CodeBlock* block;
		} stmt_while;
		struct
		{
			Yu_Expression* condition;
			Yu_CodeBlock* block;
		} stmt_dowhile;
		struct
		{
			Yu_CodeBlock* block;
		} stmt_scope;
	};
	int linenum;
} Yu_Statement;

const char* yu_statementtype_names[Yu_NUM_STATEMENTTYPES];

/*
** @brief Frees a statement and all its child nodes recursively.
** @param stmt Statement to free.
*/
void Yu_FreeStatement(Yu_Statement* stmt);



#endif