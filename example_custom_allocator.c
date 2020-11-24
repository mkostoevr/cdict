#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//
// Predeclared UserData structure
//

typedef struct {
	void *space;
	size_t size;
	size_t capacity;
} Allocator;

typedef struct {
	Allocator allocator;
} UserData;

//
// CDict instantiation
//

#define CDICT_INST
#define CDICT_USER_DATA_T UserData
#define CDICT_HASHTAB_ITEM_ALLOC_FN(cdict, size) item_alloc(cdict, size)
#define CDICT_HASHTAB_ITEM_FREE_FN(cdict, ptr) item_free(cdict, ptr)
#include "cdict.h"

//
// Sequential allocator for items (fixed sized and does not free)
//

void custom_item_allocator_init(Allocator *allocator) {
	allocator->size = 0;
	allocator->capacity = 1024;
	allocator->space = calloc(1, allocator->capacity);
	if (!allocator->space) {
		printf("Can't allocate space for items allocator");
		exit(-1);
	}
}

void *item_alloc(CDict_CStr_CStr *s, size_t size) {
	Allocator *self = &s->user_data.allocator;
	if (size + self->size >= self->capacity) {
		return NULL;
	}
	void *res = &((char *)self->space)[self->size];
	self->size += size;
	return res;
}

void item_free(CDict_CStr_CStr *s, CDictItem_CStr_CStr *ptr) {
	return;
}

//
// The code
//

int main(int argc, char **argv) {
	UserData ud = { 0 };
	custom_item_allocator_init(&ud.allocator);

	CDict_CStr_CStr d;
	cdict_CStr_CStr_init_pud(&d, &ud); // Give it the user data
	cdict_CStr_CStr_add_vv(&d, "key_a", "val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_b", "another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key_c", "yet_another_val", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "zzz", CDICT_NO_CHECK);
	cdict_CStr_CStr_add_vv(&d, "key", "akjdhaw", CDICT_LEAVE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esddaad", CDICT_REPLACE_EXIST);
	cdict_CStr_CStr_add_vv(&d, "key", "esdd", CDICT_LEAVE_EXIST);
	printf("[key_a] = %s\n", cdict_CStr_CStr_get_v(&d, "key_a"));
	printf("[key_b] = %s\n", cdict_CStr_CStr_get_v(&d, "key_b"));
	printf("[key_c] = %s\n", cdict_CStr_CStr_get_v(&d, "key_c"));
	printf("[key] = %s\n", cdict_CStr_CStr_get_v(&d, "key"));
	return 0;
}
