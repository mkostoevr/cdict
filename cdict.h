// Copyright (c) 2020 Magomed Kostoev
//
// You may use, distribute and modify this code under the terms of the MIT license.
//
// You should have received a copy of the MIT license with this file. If not, please visit
// https://opensource.org/licenses/MIT or boppan.org/MIT for full license details.

// cdict.h - simple dictionaty implementation in C.
//
// Uses hash table with fixed (but configurable) size. Functions named using given type names:
// cdict_<CDICT_KEY_T>_<CDICT_VAL_T>_<name>.
//
// See configuration below (in public "Input macros" section), also:
// CDICT_INST: Instantiate the functions if defined.
//
// Minimal definitions for declaration: CDICT_KEY_T, CDICT_VAL_T
// Minimal definitions for instantiation: CDICT_INST, CDICT_KEY_T, CDICT_VAL_T, CDICT_HASH_FN,
// CDICT_CMP_FN
//
// WARNING: All used definitions will be undefined on header exit.
//
// Dependencies:
//  <stddef.h> or another source of size_t
//  <stdlib.h> or another source of malloc, and calloc
//  <assert.h> or another source of assert

//
// Internal macros for external declarations
//

#define CDICT_CAT2_IMPL(x, y) x ## _ ## y
#define CDICT_CAT2(x, y) CDICT_CAT2_IMPL(x, y)

/// Creates method name according to CDICT_KEY_T and CDICT_VAL_T
#define CDICT_FUN(name) CDICT_CAT2(cdict, CDICT_CAT2(CDICT_KEY_T, CDICT_CAT2(CDICT_VAL_T, name)))

#define cdict_init CDICT_FUN(init)
#define cdict_init_ud CDICT_FUN(init_ud)
#define cdict_init_pud CDICT_FUN(init_pud)
#define cdict_add_pp CDICT_FUN(add_pp)
#define cdict_add_vv CDICT_FUN(add_vv)
#define cdict_get_p CDICT_FUN(get_p)
#define cdict_get_v CDICT_FUN(get_v)
#define cdict_error_message CDICT_FUN(error_message)
#define CDictItem CDICT_CAT2(CDictItem, CDICT_CAT2(CDICT_KEY_T, CDICT_VAL_T))
#define CDictItem_s CDICT_CAT2(CDictItem_s, CDICT_CAT2(CDICT_KEY_T, CDICT_VAL_T))
#define CDict CDICT_CAT2(CDict, CDICT_CAT2(CDICT_KEY_T, CDICT_VAL_T))

//
// Input macros
//

/// Type of the dictionary's keys, named type only - used in functions names, default: CStr
#ifndef CDICT_KEY_T
typedef char *CStr;
#define CDICT_CSTR_IS_THERE
#define CDICT_KEY_T CStr
#endif

/// Type of the dictionary's values, named type only - used in functions names, default: CStr
#ifndef CDICT_VAL_T
#ifndef CDICT_CSTR_IS_THERE
typedef char *CStr;
#endif
#define CDICT_VAL_T CStr
#endif

/// Type of user data
#ifndef CDICT_USER_DATA_T
#define CDICT_USER_DATA_T void *
#endif

//
// Public interface
//

#define CDICT_NO_CHECK 0
#define CDICT_REPLACE_EXIST 1
#define CDICT_LEAVE_EXIST 2

#define CDICT_ERR_SUCCESS 0
#define CDICT_ERR_OUT_OF_MEMORY 1

typedef struct CDictItem_s {
    struct CDictItem_s *next_collision;
    CDICT_KEY_T key;
    CDICT_VAL_T val;
} CDictItem;

typedef struct {
    CDictItem **hash_table;
    CDICT_USER_DATA_T user_data;
    int error_code;
} CDict;

/// Initializes dictionary structure
int cdict_init(CDict *s);

/// Initializes dictionary structure with non-standard user data
int cdict_init_ud(CDict *s, CDICT_USER_DATA_T user_data);

/// Initializes dictionary structure with non-standard user data
int cdict_init_pud(CDict *s, CDICT_USER_DATA_T *user_data);

/// Inserts a value by key (receives pointers to val and key)
CDictItem *cdict_add_pp(CDict *s, CDICT_KEY_T *pkey, CDICT_VAL_T *pval, int if_exists);

/// Inserts a value by key (receives values of val and key)
CDictItem *cdict_add_vv(CDict *s, CDICT_KEY_T key, CDICT_VAL_T val, int if_exists);

/// Gives a value by key (receives a pointer to key)
CDICT_VAL_T cdict_get_p(CDict *s, CDICT_KEY_T *pkey);

/// Gives a vaule by key (receives a value of key)
CDICT_VAL_T cdict_get_v(CDict *s, CDICT_KEY_T key);

#ifdef CDICT_INST

//
// Input macros (instantiation edition)
//

/// Hashing function for the key type
#ifndef CDICT_HASH_FN
#include <string.h>
#define CDICT_HASH_FN(pkey) strlen(*pkey) ^ (*pkey)[0]
#endif

/// Ammount of pointers to elements in hash table
#ifndef CDICT_HASHTAB_SZ
#define CDICT_HASHTAB_SZ 1024
#endif

/// Hash table allocator MUST return zeroed buffer
#ifndef CDICT_HASHTAB_ALLOC_FN
#include <stdlib.h>
#define CDICT_HASHTAB_ALLOC_FN(cdict, size) calloc(1, size)
#else
#define CDICT_HASHTAB_ALLOCATORS_OVERRIDDEN
#endif

/// New hash table items allocator (should be just a function call)
#ifndef CDICT_HASHTAB_ITEM_ALLOC_FN
#include <stdlib.h>
#define CDICT_HASHTAB_ITEM_ALLOC_FN(cdict, size) malloc(size)
#define CDICT_HASHTAB_ITEM_FREE_FN(cdict, ptr) free(ptr)
#else
#ifndef CDICT_HASHTAB_ITEM_FREE_FN
#error "CDICT_HASHTAB_ITEM_FREE_FN should be defiend along with CDICT_HASHTAB_ITEM_ALLOC_FN"
#endif
#define CDICT_HASHTAB_ITEM_ALLOCATORS_OVERRIDDEN
#endif

/// Replacement for assert from <assert.h>
#ifndef CDICT_ASSERT_FN
#include <assert.h>
#define CDICT_ASSERT_FN(x) if (!x) { printf(__FILE__":%d: Disasserted", __LINE__); } assert(x);
#endif

/// Function for comparsion of keys, return should be the same as memcmp
#ifndef CDICT_CMP_FN
#include <string.h>
#define CDICT_CMP_FN(pkey0, pkey1) strcmp(*pkey0, *pkey1)
#endif

//
// Internal macros (instantiation edition)
//

#define CDICT_ASSERT(x) ({ CDICT_ASSERT_FN(x); x; })

//
// Predeclarations
//

#ifdef CDICT_HASHTAB_ITEM_ALLOCATORS_OVERRIDDEN
void *CDICT_HASHTAB_ITEM_ALLOC_FN(CDict *s, size_t size);
void CDICT_HASHTAB_ITEM_FREE_FN(CDict *s, CDictItem *ptr);
#endif

#ifdef CDICT_HASHTAB_ALLOCATORS_OVERRIDDEN
void *CDICT_HASHTAB_ALLOC_FN(CDict *s, size_t size);
#endif

//
// The code
//

static size_t cdict_hash(CDICT_KEY_T *pkey) {
    return CDICT_HASH_FN(CDICT_ASSERT(pkey)) & CDICT_HASHTAB_SZ;
}

static int cdict_keycmp(CDICT_KEY_T *pkey0, CDICT_KEY_T *pkey1) {
    return CDICT_CMP_FN(CDICT_ASSERT(pkey0), CDICT_ASSERT(pkey1));
}

static CDictItem **cdict_chain_begin(CDict *s, CDICT_KEY_T *pkey) {
    size_t hash = cdict_hash(CDICT_ASSERT(pkey));
    return &CDICT_ASSERT(s)->hash_table[hash];
}

static CDictItem **cdict_chain_next(CDictItem **ppit) {
    return &CDICT_ASSERT(*CDICT_ASSERT(ppit))->next_collision;  
}

int cdict_init(CDict *s) {
    return cdict_init_pud(s, NULL);
}

int cdict_init_ud(CDict *s, CDICT_USER_DATA_T user_data) {
    return cdict_init_pud(s, &user_data);
}

int cdict_init_pud(CDict *s, CDICT_USER_DATA_T *user_data) {
    CDICT_ASSERT(s);
    s->user_data = user_data ? *user_data : (CDICT_USER_DATA_T){ 0 };
    s->error_code = CDICT_ERR_SUCCESS;
    if (!(s->hash_table = CDICT_HASHTAB_ALLOC_FN(s, sizeof(*s->hash_table) * CDICT_HASHTAB_SZ))) {
        s->error_code = CDICT_ERR_OUT_OF_MEMORY;
        return 0;
    }
    return 1;
}

CDictItem *cdict_add_pp(CDict *s, CDICT_KEY_T *pkey, CDICT_VAL_T *pval, int if_exists) {
    CDICT_ASSERT(s);
    CDICT_ASSERT(pval);
    CDICT_ASSERT(pkey);
    CDictItem **ppit = cdict_chain_begin(s, pkey);
    while (*ppit) {
        int exists = if_exists == CDICT_NO_CHECK ? 0 : !cdict_keycmp(pkey, &(*ppit)->key);
        if (exists) {
            if (if_exists == CDICT_LEAVE_EXIST) {
                return *ppit;
            } else if (if_exists == CDICT_REPLACE_EXIST) {
                CDICT_HASHTAB_ITEM_FREE_FN(s, *ppit);
                break;
            }
        }
        ppit = cdict_chain_next(ppit);
    }
    *ppit = CDICT_HASHTAB_ITEM_ALLOC_FN(s, sizeof(**ppit));
    CDictItem *pit = *ppit;
    pit->key = *pkey;
    pit->val = *pval;
    pit->next_collision = 0;
    return pit;
}

CDictItem *cdict_add_vv(CDict *s, CDICT_KEY_T key, CDICT_VAL_T val, int if_exists) {
    CDICT_ASSERT(s);
    return cdict_add_pp(s, &key, &val, if_exists);
}

CDICT_VAL_T cdict_get_p(CDict *s, CDICT_KEY_T *pkey) {
    CDICT_ASSERT(s);
    CDICT_ASSERT(pkey);
    for (CDictItem **ppit = cdict_chain_begin(s, pkey); *ppit; ppit = cdict_chain_next(ppit)) {
        if (!cdict_keycmp(pkey, &(*ppit)->key)) {
            return (*ppit)->val;
        }
    }
    return NULL;
}

CDICT_VAL_T cdict_get_v(CDict *s, CDICT_KEY_T key) {
    CDICT_ASSERT(s);
    return cdict_get_p(s, &key);
}

#endif

#undef CDICT_CAT2_IMPL
#undef CDICT_CAT2
#undef CDICT_FUN

#undef cdict_init
#undef cdict_init_ud
#undef cdict_init_pud
#undef cdict_add_pp
#undef cdict_add_vv
#undef cdict_get_p
#undef cdict_get_v

#undef CDICT_KEY_T
#undef CDICT_VAL_T
#undef CDICT_USER_DATA_T

#ifdef CDICT_INST
#undef CDICT_INST
#undef CDICT_HASH_FN
#undef CDICT_HASHTAB_SZ
#undef CDICT_ASSERT_FN
#undef CDICT_CMP_FN
#undef CDICT_ASSERT
#endif
