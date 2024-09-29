#include <stdio.h>
#include "yune.h"
#include "yu_code.h"
#include "yu_parse.h"

#include "yu_object.h"



char* load_string_from_file(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (!file) return NULL;

	int length = 0;
	while (!feof(file))
	{
		fgetc(file);
		length++;
	}

	char* filebuffer = malloc(length + 1);
	if (!filebuffer) goto fail;
	rewind(file);
	int pos = 0;
	while (!feof(file) && pos < length)
	{
		char c = fgetc(file);
		filebuffer[pos] = c;
		pos++;
	}

	filebuffer[pos - 1] = '\0';

fail:
	fclose(file);
	return filebuffer;
}



int main(int argc, char** argv)
{
	printf("Yune programming language test but here??\n");

	Yu_State* state = Yu_CreateState();
	if (!state) return 1;
	
	char* filestring = load_string_from_file("test.yu");
	Yu_CodeBlock* block = Yu_ParseSourceCode(state, filestring);
	free(filestring);

	Yu_Value v1 = { Yu_VT_NUMBER, .u_number = 2.5 };
	Yu_Value v2 = { Yu_VT_NUMBER, .u_number = 2.5 };
	Yu_Value v3 = { Yu_VT_VECTOR, .u_vector = { 4, 0, 1, 2, 3 } };
	Yu_ValueBinOp(&v1, &v2, Yu_OP_ADD);
	Yu_PrintValue(&v3);

	return 0;
}
