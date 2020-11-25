#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//
// CDict instantiation
//

void my_assert(char *file, int line);

#define CDICT_INST
#define CDICT_ASSERT_FN(x) (void)((x) ? 0 : my_assert(__FILE__, __LINE__))
#include "cdict.h"

void my_assert(char *file, int line) {
	printf("FATAL: Assertion failed at %s:%d", file, line);
	exit(-1);
}

//
// The code
//

int main(int argc, char **argv) {
	CDict_CStr_CStr d;
	// Give it the user data
	if (!cdict_CStr_CStr_init(&d)) {
		printf("Error #%d from CDict\n", d.error_code);
		return 1;
	}
	cdict_CStr_CStr_add_vv(&d, "key_a", "val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_b", "another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_c", "yet_another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "zzz", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "akjdhaw", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esddaad", CDICT_REPLACE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esdd", CDICT_LEAVE_EXIST);
	printf("[key_a] = %s\n", cdict_CStr_CStr_get_v(&d, "key_a"));
	d.hash_table = NULL;
	printf("[key_b] = %s\n", cdict_CStr_CStr_get_v(&d, "key_b"));
	printf("[key_c] = %s\n", cdict_CStr_CStr_get_v(&d, "key_c"));
	printf("[key] = %s\n", cdict_CStr_CStr_get_v(&d, "key"));
	return 0;
}
