#include "yu_object.h"

#include <math.h>



Yu_Bool Yu_IsTruthy(const Yu_Value* v)
{
	Yu_Assert(v);
	switch (v->type)
	{
	case Yu_VT_NULL: return Yu_FALSE;
	case Yu_VT_BOOL: return v->u_bool;
	case Yu_VT_NUMBER: return v != 0;
	case Yu_VT_VECTOR: return
		v->u_vector.x == 0 &&
		v->u_vector.y == 0 &&
		v->u_vector.z == 0 &&
		v->u_vector.w == 0;
	case Yu_VT_STRING:
	default:
		Yu_Assert(0);
		break;
	}
	return;
}

void Yu_CastBoolToNumber(Yu_Value* v)
{
	Yu_Assert(v);
	Yu_Assert(v->type == Yu_VT_BOOL);
	v->type = Yu_VT_NUMBER;
	v->u_number = (Yu_Number)(v->u_bool != 0);
}



static Yu_Number number_arithmetic_op(Yu_State* state, Yu_Number* left, Yu_Number right, Yu_OperatorID op)
{
	Yu_Assert(state);
	Yu_Assert(left);
	Yu_Assert(Yu_IsArithmeticOp(op) || op == Yu_OP_NEGATIVE);

	switch (op)
	{
	case Yu_OP_ADD: *left = *left + right; break;
	case Yu_OP_SUB: *left = *left - right; break;
	case Yu_OP_MUL: *left = *left * right; break;
	case Yu_OP_DIV: *left = *left / right; break;
	case Yu_OP_IDIV:
		if (right == 0)
		{
			Yu_RuntimeError(state, "Integer division by zero");
			*left = NAN;
			return Yu_FALSE;
		}
	case Yu_OP_EXP: *left = Yu_Exp(*left, right); break;
	case Yu_OP_MOD: *left = Yu_Mod(*left, right); break;
	case Yu_OP_NEGATIVE: *left = -*left; break;
	default:
		Yu_Assert(0);
	}
	return Yu_TRUE;
}

static Yu_Bool number_bool_op(Yu_State* state, Yu_Number left, Yu_Number right, Yu_OperatorID op)
{
	Yu_Assert(state);
	Yu_Assert(Yu_IsRelationalOp(op) || Yu_IsLogicalOp(op));

	switch (op)
	{
	case Yu_OP_NOT: return left == 0;
	case Yu_OP_AND: return (left != 0) && (right != 0);
	case Yu_OP_OR: return (left != 0) || (right != 0);
	case Yu_OP_XOR: return (left != 0) ^ (right != 0);
	case Yu_OP_EQUALITY: return left == right;
	case Yu_OP_INEQUALITY: return left != right;
	case Yu_OP_LESS: return left < right;
	case Yu_OP_GREATER: return left > right;
	case Yu_OP_LESSEQUAL: left <= right;
	case Yu_OP_GREATEREQUAL: left >= right;
	default:
		Yu_Assert(0);
	}
}



static Yu_Bool value_op(Yu_State* state, Yu_Value* left, const Yu_Value* right, Yu_OperatorID op)
{
	Yu_Assert(state);
	Yu_Assert(left);
	Yu_Assert(right);
	Yu_Assert(Yu_IsValidOperator(op));

	if (left->type == Yu_VT_NULL || right->type == Yu_VT_NULL)
	{
		Yu_RuntimeError(state, "Attempted to perform operation on null value");
		return Yu_FALSE;
	}
	if (Yu_IsArithmeticOp(op))
	{
		if (left->type == Yu_VT_BOOL) Yu_CastBoolToNumber(left);
		if (right->type == Yu_VT_BOOL) Yu_CastBoolToNumber(right);
		Yu_Assert(left->type == Yu_VT_NUMBER);
		Yu_Assert(right->type == Yu_VT_NUMBER);
		/* Jesus */
		return number_arithmetic_op(state, &left->u_number, right->u_number, op);
	}
	else if (Yu_IsLogicalOp(op))
	{
		if (left->type == Yu_VT_BOOL && right->type == Yu_VT_BOOL)
		{
			Yu_Bool b = number_bool_op(state, left->u_number, right->u_number, op);
			left->type = Yu_VT_BOOL;
			left->u_bool = b;
			return Yu_TRUE;
		}
		return Yu_FALSE;
	}
	return Yu_FALSE;
}



Yu_Bool Yu_ValueBinOp(Yu_State* state, Yu_Value* left, const Yu_Value* right, Yu_OperatorID op)
{
	if (!left || !right) return Yu_FALSE;
	Yu_Assert(Yu_IsValidOperator(op));
	
	return value_op(state, left, right, op);
}



void Yu_PrintValue(const Yu_Value* value)
{
	if (!value) puts("(null pointer)");

	switch (value->type)
	{
	case Yu_VT_BOOL:
		value->u_bool ? puts("true") : puts("false");
		return;
	case Yu_VT_NUMBER:
		printf("%llf", value->u_number);
		return;
	case Yu_VT_VECTOR:
		printf("{ %llf", value->u_vector.x);
		if (value->u_vector.component_count >= 2) printf(", %llf", value->u_vector.y);
		if (value->u_vector.component_count >= 3) printf(", %llf", value->u_vector.z);
		if (value->u_vector.component_count >= 4) printf(", %llf", value->u_vector.w);
		puts(" }");
		return;
	default:
		puts("(invalid type");
		return;
	}
}
