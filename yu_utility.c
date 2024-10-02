#include "yu_utility.h"



Yu_Bool Yu_InitVector(Yu_Vector* vector)
{
	Yu_Assert(vector);
	vector->buffer = Yu_Calloc(Yu_MIN_VECTOR_CAPACITY, sizeof(void*));
	vector->capacity = Yu_MIN_VECTOR_CAPACITY;
	vector->count = 0;
	return vector;
}

Yu_Bool Yu_InitVectorSize(Yu_Vector* vector, Yu_Size capacity)
{
	Yu_Assert(vector);
	Yu_Assert(capacity >= Yu_MIN_VECTOR_CAPACITY);
	vector->buffer = Yu_Calloc(capacity, sizeof(void*));
	vector->capacity = capacity;
	vector->count = 0;
	return vector;
}

Yu_Bool Yu_ResizeVector(Yu_Vector* vector, Yu_Size newcapacity)
{
	Yu_Assert(vector);
	Yu_Assert(vector->buffer);
	Yu_Assert(vector->capacity >= Yu_MIN_VECTOR_CAPACITY);
	if (newcapacity < vector->count)
	{
		return Yu_FALSE;
	}
	if (newcapacity < Yu_MIN_VECTOR_CAPACITY) newcapacity = Yu_MIN_VECTOR_CAPACITY;
	vector->buffer = Yu_Realloc(vector->buffer, newcapacity * sizeof(void*));
	vector->capacity = newcapacity;
	return Yu_TRUE;
}

Yu_Bool Yu_PushBackVector(Yu_Vector* vector, void* item)
{
	Yu_Assert(vector);
	Yu_Assert(vector->buffer && vector->capacity >= Yu_MIN_VECTOR_CAPACITY);
	Yu_Assert(item);
	if (vector->count + 1 > vector->capacity)
	{
		if (!Yu_DoubleVectorSize(vector)) return Yu_FALSE;
	}
	vector->buffer[vector->count] = item;
	vector->count++;
	return Yu_TRUE;
}

void* Yu_PopBackVector(Yu_Vector* vector)
{
	Yu_Assert(vector);
	Yu_Assert(vector->buffer);
	if (vector->count <= 0) return NULL;
	void* item = vector->buffer[vector->count - 1];
	vector->count--;
	if (vector->count * 2 <= vector->capacity)
	{
		if (!Yu_HalfVectorSize(vector)) return NULL;
	}
	return item;
}



char* Yu_CopyCutString(const char* srcstring, Yu_Size start, Yu_Size length)
{
	char* str = Yu_Malloc(length + 1); /* Plus 1 to include null terminator */
	if (!str) return NULL;
	for (int i = 0; i < length; i++)
		str[i] = srcstring[i + start];
	str[length] = '\0';
	return str;
}
