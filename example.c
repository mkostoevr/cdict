#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CDICT_INST
#include "cdict.h"

int main(int argc, char **argv) {
	CDict_CStr_CStr d;
	cdict_CStr_CStr_init(&d);
	cdict_CStr_CStr_add_vv(&d, "val", "key_a", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "another_val", "key_b", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "yet_another_val", "key_c", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "zzz", "key", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "akjdhaw", "key", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "esdd", "key", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "esddaad", "key", CDICT_REPLACE_EXIST);
	printf("[key] = %s", cdict_CStr_CStr_get_v(&d, "key"));
	return 0;
}
