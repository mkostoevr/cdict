#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//
// CDict instantiation
//

#define CDICT_INST
#define CDICT_KEY_T char
#define CDICT_HASH_FN(pkey) my_hash(pkey)
#define CDICT_CMP_FN(pkey0, pkey1) my_cmp(pkey0, pkey1)
#include "../cdict.h"

unsigned long my_hash(char *pkey) {
	return *pkey;
}

// ACHTUNG! This is NOT a function returning true or false
// It should return the difference between 1st and 2nd argument, or:
// if pkey0 == pkey1: return 0
// if pkey0 < pkey1: return whatever negative number
// if pkey0 > pkey1: return whatever positive number
int my_cmp(char *pkey0, char *pkey1) {
	return *pkey0 - *pkey1;
}

//
// The code
//

int main(int argc, char **argv) {
	CDict_char_CStr d;
	cdict_char_CStr_init(&d);
	cdict_char_CStr_add_vv(&d, 'a', "Letter 'A'", CDICT_NO_CHECK);
	cdict_char_CStr_add_vv(&d, 'b', "Letter 'B'", CDICT_NO_CHECK);
	cdict_char_CStr_add_vv(&d, 'c', "Letter 'C'", CDICT_NO_CHECK);

	if (d.error_code != CDICT_ERR_SUCCESS) {
		printf("Error #%d from CDict\n", d.error_code);
		return 1;
	}

	printf("[a] = %s\n", cdict_char_CStr_get_v(&d, 'a'));
	printf("[b] = %s\n", cdict_char_CStr_get_v(&d, 'b'));
	printf("[c] = %s\n", cdict_char_CStr_get_v(&d, 'c'));
	return 0;
}
