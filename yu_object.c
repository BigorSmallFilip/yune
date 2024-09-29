#include "yu_object.h"



Yu_Bool Yu_ValueAdd()

Yu_Bool Yu_ValueBinOp(Yu_Value* left, const Yu_Value* right, Yu_OperatorID op)
{
	if (left || right == NULL) return Yu_FALSE;
	Yu_Assert(Yu_IsValidOperator(op));

	switch (op)
	{
	case Yu_OP_ADD:

		break;
	default:
		puts("Not implemented!\n");
		break;
	}

	return Yu_TRUE;
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
