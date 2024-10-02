#include "yu_state.h"

#include <stdarg.h>

Yu_State* Yu_CreateState()
{
	Yu_State* state = Yu_AllocType(Yu_State);
	if (!state) return NULL;
	state->current_value_memory = 0;
	state->max_value_memory = 1;
	state->tabsize = 4;
	Yu_InitVector(&state->messages);
	return state;
}

void Yu_PrintStateMessages(Yu_State* state)
{
	Yu_Assert(state);
	for (int i = 0; i < state->messages.count; i++)
	{
		const char* message = state->messages.buffer[i];
		printf("State message: %s\n", message);
	}
}

void Yu_RuntimeError(Yu_State* state, const char* const format, ...)
{
	Yu_Assert(state);
	Yu_Assert(format);

	char* str = Yu_Malloc(256);
	if (!str) return;
	va_list args;
	va_start(args, format);
	int len = vsprintf_s(str, 255, format, args);
	va_end(args);
	str = Yu_Realloc(str, len + 1);
	str[len] = '\0';

	Yu_PushBackVector(&state->messages, str);
}
