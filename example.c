#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CDICT_INST
#include "cdict.h"

int main(int argc, char **argv) {
	CDict_CStr_CStr d;
	cdict_CStr_CStr_init(&d);
	cdict_CStr_CStr_add_vv(&d, "key_a", "val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_b", "another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_c", "yet_another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "zzz", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "akjdhaw", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esdd", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esddaad", CDICT_REPLACE_EXIST);
	printf("[key] = %s", cdict_CStr_CStr_get_v(&d, "key"));
	return 0;
}