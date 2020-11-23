// Copyright (c) 2020 Magomed Kostoev
//
// You may use, distribute and modify this code under the terms of the MIT license.
//
// You should have received a copy of the MIT license with this file. If not, please visit
// https://opensource.org/licenses/MIT for full license details.

// cdict.h - simple dictionaty implementation in C.
//
// Uses hash table with fixed (but configurable) size. Functions named using given type names:
// cdict_<CDICT_TYPE_KEY>_<CDICT_TYPE_VAL>_<name>.
//
// Configuration (definitions):
// CDICT_TYPE_KEY:  Type of the dictionary's keys, named type only - used in functions names,
//                  default: CStr (char *)
// CDICT_TYPE_VAL:  Type of the dictionary's values, named type only - used in functions names,
//                  default: CStr (char *)
// CDICT_INST:      Instantiate the functions if defined
// CDICT_HASH:      Hashing function for the key type, taking pointer to CDICT_TYPE_KEY as first
//                  parameter, default is very simple (and for C strings)
// CDICT_CMPF:      Function for comparsion of keys, return should be the same as memcmp, taking
//                  two pointers to CDICT_TYPE_KEY, default: strcmp
// CDICT_HTSZ:      Ammount of elements in hash table, default: 1024
// CDICT_ASSERT_FN: Replacement for assert from <assert.h>, by default also prints file and line
//
// Minimal definitions for declaration: CDICT_TYPE_KEY, CDICT_TYPE_VAL
// Minimal definitions for instantiation: CDICT_INST, CDICT_TYPE_KEY, CDICT_TYPE_VAL, CDICT_HASH,
// CDICT_CMPF
//
// WARNING: All used definitions will be undefined on header exit.
//
// Dependencies:
// <stddef.h> or another source of size_t
// <stdlib.h> or another source of malloc, calloc and realloc
// <assert.h> or another source of assert

//
// Input macros
//

#ifndef CDICT_TYPE_KEY
    typedef char *CStr;
#   define CDICT_CSTR_IS_THERE
#   define CDICT_TYPE_KEY CStr
#endif
#ifndef CDICT_TYPE_VAL
#   ifndef CDICT_CSTR_IS_THERE
        typedef char *CStr;
#   endif
#   define CDICT_TYPE_VAL CStr
#endif

//
// Internal macros
//

#define CDICT_CAT2_IMPL(x, y) x ## _ ## y
#define CDICT_CAT2(x, y) CDICT_CAT2_IMPL(x, y)

/// Creates method name according to CDICT_TYPE_KEY and CDICT_TYPE_VAL
#define CDICT_FUN(name) CDICT_CAT2(cdict, CDICT_CAT2(CDICT_TYPE_KEY, CDICT_CAT2(CDICT_TYPE_VAL, name)))

#define cdict_init CDICT_FUN(init)
#define cdict_add_pp CDICT_FUN(add_pp)
#define cdict_add_vv CDICT_FUN(add_vv)
#define cdict_get_p CDICT_FUN(get_p)
#define cdict_get_v CDICT_FUN(get_v)
#define CDictItem CDICT_CAT2(CDictItem, CDICT_CAT2(CDICT_TYPE_KEY, CDICT_TYPE_VAL))
#define CDict CDICT_CAT2(CDict, CDICT_CAT2(CDICT_TYPE_KEY, CDICT_TYPE_VAL))

//
// Public interface
//

#define CDICT_NO_CHECK 0
#define CDICT_REPLACE_EXIST 1
#define CDICT_LEAVE_EXIST 2

#define CDICT_ERR_SUCCESS 0
#define CDICT_ERR_OUT_OF_MEMORY 1

typedef struct CDictItem_s {
    struct CDictItem *next_collision;
    CDICT_TYPE_KEY key;
    CDICT_TYPE_VAL val;
} CDictItem;

typedef struct {
    CDictItem **hash_table;
    int error_code;
} CDict;

int cdict_init(CDict *s);
CDictItem *cdict_add_pp(CDict *s, CDICT_TYPE_KEY *pkey, CDICT_TYPE_VAL *pval, int if_exists);
CDictItem *cdict_add_vv(CDict *s, CDICT_TYPE_KEY key, CDICT_TYPE_VAL val, int if_exists);
CDICT_TYPE_VAL cdict_get_p(CDict *s, CDICT_TYPE_KEY *pkey);
CDICT_TYPE_VAL cdict_get_v(CDict *s, CDICT_TYPE_KEY key);

#ifdef CDICT_INST

//
// Input macros (instantiation edition)
//

#ifndef CDICT_HASH
#define CDICT_HASH(pkey) strlen(*pkey) ^ (*pkey)[0]
#endif
#ifndef CDICT_HTSZ
#define CDICT_HTSZ 1024
#endif
#ifndef CDICT_ASSERT_FN
#include <assert.h>
#define CDICT_ASSERT_FN(x) if (!x) { printf(__FILE__":%d: Disasserted", __LINE__); } assert(x);
#endif
#ifndef CDICT_CMPF
#include <string.h>
#define CDICT_CMPF(pkey0, pkey1) strcmp(*pkey0, *pkey1)
#endif

//
// Interna macros (instantiation edition)
//

#define CDICT_ASSERT(x) ({ CDICT_ASSERT_FN(x); x; })

//
// The code
//

static size_t cdict_hash(CDICT_TYPE_KEY *pkey) {
    return CDICT_HASH(CDICT_ASSERT(pkey)) & CDICT_HTSZ;
}

static int cdict_keycmp(CDICT_TYPE_KEY *pkey0, CDICT_TYPE_KEY *pkey1) {
    return CDICT_CMPF(CDICT_ASSERT(pkey0), CDICT_ASSERT(pkey1));
}

static CDictItem **cdict_chain_begin(CDict *s, CDICT_TYPE_KEY *pkey) {
    size_t hash = cdict_hash(CDICT_ASSERT(pkey));
    return &CDICT_ASSERT(s)->hash_table[hash];
}

static CDictItem **cdict_chain_next(CDictItem **ppit) {
    return &CDICT_ASSERT(*CDICT_ASSERT(ppit))->next_collision;  
}

int cdict_init(CDict *s) {
    CDICT_ASSERT(s);
    s->error_code = CDICT_ERR_SUCCESS;
    s->hash_table = calloc(sizeof(*s->hash_table), CDICT_HTSZ);
    if (!s->hash_table) {
        s->error_code = CDICT_ERR_OUT_OF_MEMORY;
    }
}

CDictItem *cdict_add_pp(CDict *s, CDICT_TYPE_KEY *pkey, CDICT_TYPE_VAL *pval, int if_exists) {
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
                free(*ppit);
                break;
            }
        }
        ppit = cdict_chain_next(ppit);
    }
    *ppit = malloc(sizeof(**ppit));
    CDictItem *pit = *ppit;
    pit->key = *pkey;
    pit->val = *pval;
    pit->next_collision = 0;
    return pit;
}

CDictItem *cdict_add_vv(CDict *s, CDICT_TYPE_KEY key, CDICT_TYPE_VAL val, int if_exists) {
    CDICT_ASSERT(s);
    return cdict_add_pp(s, &key, &val, if_exists);
}

CDICT_TYPE_VAL cdict_get_p(CDict *s, CDICT_TYPE_KEY *pkey) {
    CDICT_ASSERT(s);
    CDICT_ASSERT(pkey);
    for (CDictItem **ppit = cdict_chain_begin(s, pkey); *ppit; ppit = cdict_chain_next(ppit)) {
        if (!cdict_keycmp(pkey, &(*ppit)->key)) {
            return (*ppit)->val;
        }
    }
    return NULL;
}

CDICT_TYPE_VAL cdict_get_v(CDict *s, CDICT_TYPE_KEY key) {
    CDICT_ASSERT(s);
    return cdict_get_p(s, &key);
}

#endif

#undef CDICT_PREFIX

#undef CDICT_CAT2_IMPL
#undef CDICT_CAT2
#undef CDICT_FUN

#undef cdict_init
#undef cdict_add_pp
#undef cdict_add_vv
#undef cdict_get_p
#undef cdict_get_v

#ifdef CDICT_INST
#undef CDICT_INST
#undef CDICT_TYPE_KEY
#undef CDICT_TYPE_VAL
#undef CDICT_HASH
#undef CDICT_HTSZ
#undef CDICT_ASSERT_FN
#undef CDICT_CMPF
#undef CDICT_ASSERT
#endif
