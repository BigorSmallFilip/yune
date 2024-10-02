#ifndef __Yu_STATE_H__
#define __Yu_STATE_H__

#include "yu_utility.h"

typedef struct
{
	Yu_Size current_value_memory; /* Amount of memory being used to store values */
	Yu_Size max_value_memory; /* The maximum amount of memory allowed for storing values */
	int tabsize; /* The number of spaces which make up a tab */
	Yu_Vector messages;
} Yu_State;

Yu_State* Yu_CreateState();

void Yu_PrintStateMessages(Yu_State* state);

void Yu_RuntimeError(Yu_State* state, const char* const format, ...);

#endif
