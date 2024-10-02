#ifndef __Yu_UTILITY_H__
#define __Yu_UTILITY_H__

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#ifdef _DEBUG
#define Yu_DEBUG
#endif

#ifdef Yu_DEBUG
#define Yu_Assert(expr) assert(expr)
#else
#define Yu_Assert(expr) void
#endif



typedef bool Yu_Bool;
#define Yu_FALSE false
#define Yu_TRUE true

typedef uint32_t Yu_Size;

#ifndef Yu_USE_32BIT_NUMBERS
typedef double Yu_Float;
typedef int64_t Yu_Int;
#define Yu_StringToFloat strtod
#define Yu_StringToInt strtol
#define Yu_Exp pow
#define Yu_Mod fmod
#else
typedef float Yu_Float;
typedef int32_t Yu_Int;
#define Yu_StringToFloat strtof
#define Yu_StringToInt strtol
#define Yu_Exp powf
#define Yu_Mod fmodf
#endif



#define Yu_Malloc(size) malloc(size)
#define Yu_Calloc(count, size) calloc(count, size)
#define Yu_Realloc(block, size) realloc(block, size)
#define Yu_AllocType(type) (type*)malloc(sizeof(type))
#define Yu_Free(block) free(block)



#define Yu_Stringify2(str) #str
#define Yu_Stringify(str) Yu_Stringify2(str)

#define Yu_CharInvalid(c) ((char)c < 0)



typedef uint32_t Yu_Hash;
Yu_Hash Yu_HashString(const char* str);



/*
** Yune vector implementation
** This is separate from the Yune script type vector which are strictly 2-4 dimensional
*/
typedef struct
{
	void** buffer;
	Yu_Size count;
	Yu_Size capacity;
} Yu_Vector;

#define Yu_MIN_VECTOR_CAPACITY 4

Yu_Bool Yu_InitVector(Yu_Vector* vector);
Yu_Bool Yu_InitVectorSize(Yu_Vector* vector, Yu_Size capacity);

Yu_Bool Yu_ResizeVector(Yu_Vector* vector, Yu_Size newcapacity);
#define Yu_HalfVectorSize(vector) Yu_ResizeVector((vector), (vector)->capacity >> 1)
#define Yu_DoubleVectorSize(vector) Yu_ResizeVector((vector), (vector)->capacity << 1)

Yu_Bool Yu_PushBackVector(Yu_Vector* vector, void* item);
void* Yu_PopBackVector(Yu_Vector* vector);

#define Yu_VectorBack(vector, type) (vector.count > 0 ? (type)(vector.buffer[vector.count - 1]) : NULL)
#define Yu_VectorPBack(vector, type) (vector->count > 0 ? (type)(vector->buffer[vector->count - 1]) : NULL)




typedef struct
{
	void* buffer;
	Yu_Size size;
	Yu_Size capacity;
} Yu_Buffer;



typedef struct
{
	char* str;
	Yu_Size length;
	Yu_Hash hash;
} Yu_String;



/*
** @brief Copies part of a string and returns a new string of that.
** @param srcstring String to copy from.
** @param start Where in the srcstring to start.
** @param length The length of the part to copy.
** @return Pointer to the new string, remember to free!
*/
char* Yu_CopyCutString(
	const char* srcstring,
	Yu_Size start,
	Yu_Size length
);



#endif
