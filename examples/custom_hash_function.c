#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//
// CDict instantiation
//

#define CDICT_INST
#define CDICT_HASH_FN(pkey) my_hash(pkey)
#include "../cdict.h"

unsigned long my_hash(char **pkey) {
	char *key = *pkey;
	unsigned long hash = 5381;

    for (int c = *key; c; c = *key++) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

//
// The code
//

int main(int argc, char **argv) {
	CDict_CStr_CStr d;
	cdict_CStr_CStr_init(&d);
	cdict_CStr_CStr_add_vv(&d, "key_a", "val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_b", "another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_c", "yet_another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "zzz", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "akjdhaw", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esddaad", CDICT_REPLACE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esdd", CDICT_LEAVE_EXIST);

	if (d.error_code != CDICT_ERR_SUCCESS) {
		printf("Error #%d from CDict\n", d.error_code);
		return 1;
	}

	printf("[key_a] = %s\n", cdict_CStr_CStr_get_v(&d, "key_a"));
	printf("[key_b] = %s\n", cdict_CStr_CStr_get_v(&d, "key_b"));
	printf("[key_c] = %s\n", cdict_CStr_CStr_get_v(&d, "key_c"));
	printf("[key] = %s\n", cdict_CStr_CStr_get_v(&d, "key"));
	return 0;
}
