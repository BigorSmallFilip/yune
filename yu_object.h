#ifndef __Yu_OBJECT_H__
#define __Yu_OBJECT_H__

#include "yu_utility.h"
#include "yu_code.h"
#include "yu_state.h"

enum
{
	Yu_VT_NULL,
	Yu_VT_BOOL,
	Yu_VT_NUMBER,
	Yu_VT_VECTOR,
	Yu_VT_STRING,
};
typedef unsigned char Yu_ValueType;

#ifdef Yu_32BIT_NUMBERS
typedef float Yu_Number;
#else
typedef long double Yu_Number;
#endif

typedef struct
{
	Yu_ValueType type;
	union
	{
		Yu_Bool u_bool;
		Yu_Number u_number;
		struct
		{
			unsigned char component_count;
			Yu_Number x;
			Yu_Number y;
			Yu_Number z;
			Yu_Number w;
		} u_vector;
	};
} Yu_Value;

Yu_Bool Yu_IsTruthy(const Yu_Value* v);

void Yu_CastBoolToNumber(Yu_Value* v);

/*
** Performs a binary operation on two values and stores the result in left
** This function mutates the left operand!
*/
Yu_Bool Yu_ValueBinOp(Yu_State* state, Yu_Value* left, const Yu_Value* right, Yu_OperatorID op);

void Yu_PrintValue(const Yu_Value* value);

#endif