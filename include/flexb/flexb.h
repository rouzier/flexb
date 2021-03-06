#ifndef __FLEXB_FLEXB__
#define __FLEXB_FLEXB__

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


#define FLEXB_NULL  0
#define FLEXB_INT   1
#define FLEXB_UINT  2
#define FLEXB_FLOAT 3
#define FLEXB_KEY  4
#define FLEXB_STRING        5
#define FLEXB_INDIRECT_INT  6
#define FLEXB_INDIRECT_UINT 7
#define FLEXB_INDIRECT_FLOAT 8
#define FLEXB_MAP 9
#define FLEXB_VECTOR 10
#define FLEXB_VECTOR_INT  11    // Typed any size (stores no type table).
#define FLEXB_VECTOR_UINT  12
#define FLEXB_VECTOR_FLOAT  13
#define FLEXB_VECTOR_KEY  14
#define FLEXB_VECTOR_STRING  15
#define FLEXB_VECTOR_INT2  16   // Typed tuple (no type table, no size field).
#define FLEXB_VECTOR_UINT2  17
#define FLEXB_VECTOR_FLOAT2  18
#define FLEXB_VECTOR_INT3  19   // Typed triple (no type table, no size field).
#define FLEXB_VECTOR_UINT3  20
#define FLEXB_VECTOR_FLOAT3  21
#define FLEXB_VECTOR_INT4  22   // Typed quad (no type table, no size field).
#define FLEXB_VECTOR_UINT4  23
#define FLEXB_VECTOR_FLOAT4  24
#define FLEXB_BLOB 25
#define FLEXB_BOOL 26
#define FLEXB_VECTOR_BOOL 36

#define FLEXB_SUCCESS 0

#define FLEXB_INVALID_CONVERSION -10000
#define FLEXB_CORRUPTED -10001
#define FLEXB_NOT_FOUND -10002

#define SET_REF(ref, DATA, WIDTH, PACK_TYPE) do { \
    uint8_t type = (PACK_TYPE) >> 2;\
    if (!((FLEXB_NULL <= type && type <= FLEXB_BOOL) || type == FLEXB_VECTOR_BOOL )) {\
        return FLEXB_CORRUPTED;\
    }\
    ref->data = (DATA);\
    ref->parent_width = (WIDTH);\
    ref->type = type;\
    ref->byte_width = 1 << ((PACK_TYPE) & 0x3);\
} while(0)

typedef struct FLEXB_ref {
    const void * data;
    uint8_t parent_width;
    uint8_t byte_width;
    uint8_t type;
} FLEXB_ref;

typedef struct FLEXB_vec {
    const void * data;
    size_t length;
    uint8_t byte_width;
    uint8_t type;
} FLEXB_vec;

typedef struct FLEXB_map {
    FLEXB_vec values;
    FLEXB_vec keys;
} FLEXB_map;

typedef struct FLEXB_root {
    const void * start;
    const void * end;
} FLEXB_root;

typedef struct _FLEXB_key_cmp {
    const char* key;
    uint8_t width;
} _FLEXB_key_cmp;

inline const void * _flexb_indirect(const void* data, int width);

inline int flexb_set_root(const void* data, size_t length, FLEXB_root *root, FLEXB_ref* ref) {
    if (data == NULL || ref == NULL || length < 3) {
        return EINVAL;
    }
    const uint8_t* end = (const uint8_t*)((const uint8_t*)data + length);
    if (root != NULL) {
        root->start = data;
        root->end = end;
    }
    const uint8_t byte_width = (uint8_t) *--end;
    if (byte_width != 1 && byte_width != 2 && byte_width != 4 && byte_width != 8) {
        return FLEXB_CORRUPTED;
    }
    const uint8_t packed_byte = (uint8_t)*--end;
    SET_REF(ref, end - byte_width, byte_width, packed_byte);
    return FLEXB_SUCCESS;
}

inline int64_t _flexb_get_int64(const void* data, int width) {
    int64_t num;
    switch (width) {
    case 1:
        {
           int8_t tmp;
           memcpy(&tmp, data, 1);
           num = tmp;
        }
       break;
    case 2:
        {
           int16_t tmp;
           memcpy(&tmp, data, 2);
           num = tmp;
        }
       break;
    case 4:
        {
           int32_t tmp;
           memcpy(&tmp, data, 4);
           num = tmp;
        }
       break;
    case 8:
        {
           memcpy(&num, data, 8);
        }
       break;
    default:
        assert(0);
    }
    return num;
}

inline uint64_t _flexb_get_uint64(const void* data, int width) {
    uint64_t num;
    switch (width) {
    case 1:
        {
           uint8_t tmp;
           memcpy(&tmp, data, 1);
           num = tmp;
        }
       break;
    case 2:
        {
           uint16_t tmp;
           memcpy(&tmp, data, 2);
           num = tmp;
        }
       break;
    case 4:
        {
           uint32_t tmp;
           memcpy(&tmp, data, 4);
           num = tmp;
        }
       break;
    case 8:
        {
           memcpy(&num, data, 8);
        }
       break;
    default:
        assert(0);
    }
    return num;
}

inline double flexb_get_float(const void* data, int width) {
    double num;
    switch (width) {
    /*
    Look into support for half and quarter float
    case 1:
        {
           uint8_t tmp;
           memcpy(&tmp, data, 1);
           num = tmp;
        }
       break;
    case 2:
        {
           uint16_t tmp;
           memcpy(&tmp, data, 2);
           num = tmp;
        }
       break;
    */
    case 4:
        {
           float tmp;
           memcpy(&tmp, data, 4);
           num = tmp;
        }
       break;
    case 8:
        {
           memcpy(&num, data, 8);
        }
       break;
    default:
        assert(0);
    }
    return num;
}

inline const void * _flexb_indirect(const void* data, int width) {
    return data - _flexb_get_uint64(data, width);
}

inline int flexb_as_float(void *root, FLEXB_ref* ref, double *num) {
    if (num == NULL || ref == NULL) {
        return EINVAL;
    }
    switch(ref->type) {
    case FLEXB_FLOAT:
        *num = flexb_get_float(ref->data, ref->parent_width);
        return FLEXB_SUCCESS;
    case FLEXB_INDIRECT_FLOAT:
        {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        *num = flexb_get_float(data, ref->byte_width);
        return FLEXB_SUCCESS;
        }
    }
    return FLEXB_INVALID_CONVERSION;
}

inline int flexb_as_int64(const FLEXB_ref* ref, int64_t *num) {
    if (num == NULL || ref == NULL) {
        return EINVAL;
    }
    switch(ref->type) {
    case FLEXB_INT:
        *num = _flexb_get_int64(ref->data, ref->parent_width);
        return FLEXB_SUCCESS;
    case FLEXB_UINT:
        *num = (int64_t)_flexb_get_uint64(ref->data, ref->parent_width);
        return FLEXB_SUCCESS;
    case FLEXB_INDIRECT_INT:
        {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        *num = _flexb_get_int64(data, ref->byte_width);
        return FLEXB_SUCCESS;
        }
    case FLEXB_INDIRECT_UINT:
        {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        *num = (int64_t)_flexb_get_uint64(data, ref->byte_width);
        return FLEXB_SUCCESS;
        }
    case FLEXB_NULL:
        *num = 0;
        return FLEXB_SUCCESS;
    }
    return FLEXB_INVALID_CONVERSION;
}

inline int flexb_as_uint64(const FLEXB_ref* ref, uint64_t *num) {
    if (num == NULL || ref == NULL) {
        return EINVAL;
    }
    switch(ref->type) {
    case FLEXB_UINT:
        *num = _flexb_get_uint64(ref->data, ref->parent_width);
        return FLEXB_SUCCESS;
    case FLEXB_INT:
        *num = (uint64_t)_flexb_get_int64(ref->data, ref->parent_width);
        return FLEXB_SUCCESS;
    case FLEXB_INDIRECT_UINT:
        {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        *num = _flexb_get_uint64(data, ref->byte_width);
        return FLEXB_SUCCESS;
        }
    case FLEXB_INDIRECT_INT:
        {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        *num = (uint64_t)_flexb_get_int64(data, ref->byte_width);
        return FLEXB_SUCCESS;
        }
    case FLEXB_NULL:
        *num = 0;
        return FLEXB_SUCCESS;
    }
    return FLEXB_INVALID_CONVERSION;
}

inline int flexb_as_bool(const void* root, const FLEXB_ref* ref, char *boolean) {
    if (boolean == NULL || ref == NULL) {
        return EINVAL;
    }
    if (ref->type == FLEXB_BOOL) {
        *boolean = _flexb_get_uint64(ref->data, ref->parent_width) != 0;
    }
    uint64_t num = 0;
    flexb_as_uint64(ref, &num);
    *boolean = num != 0;
    return FLEXB_SUCCESS;
}

inline int flexb_as_str(const void* root, const FLEXB_ref* ref, const char **str) {
    if (str == NULL || ref == NULL) {
        return EINVAL;
    }
    if (ref->type == FLEXB_STRING) {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        if (root != NULL && data < root) {
            return FLEXB_CORRUPTED;
        }
        *str = data;
        return FLEXB_SUCCESS;
    }
    return FLEXB_INVALID_CONVERSION;
}

inline int flexb_as_blob(const void* root, const FLEXB_ref* ref, const char **blob, size_t * length) {
    if (length == NULL || blob == NULL || ref == NULL) {
        return EINVAL;
    }
    if (ref->type == FLEXB_STRING || ref->type == FLEXB_BLOB) {
        const void* data = _flexb_indirect(ref->data, ref->parent_width);
        if (root != NULL && data < root) {
            return FLEXB_CORRUPTED;
        }
        const void* size_offset = data - ref->byte_width;
        if (root != NULL && size_offset < root) {
            return FLEXB_CORRUPTED;
        }
        *blob = (const char *)data;
        *length = _flexb_get_uint64(size_offset, ref->byte_width);
        return FLEXB_SUCCESS;
    }
    return FLEXB_INVALID_CONVERSION;
}

inline int flexb_as_vec(const void* root, const FLEXB_ref* ref, FLEXB_vec *vec) {
    if (ref == NULL || vec == NULL) {
            return EINVAL;
    }
    if (ref->type < FLEXB_MAP || ref->type > FLEXB_VECTOR_FLOAT4) {
        return FLEXB_INVALID_CONVERSION;
    }
    const void* data = _flexb_indirect(ref->data, ref->parent_width);
    if (root != NULL && data < root) {
        return FLEXB_CORRUPTED;
    }
    size_t length = 0;
    uint8_t type = 0;// No sub type
    // Getting the size
    if (FLEXB_VECTOR_INT2 <= ref->type) {
        length = (ref->type - FLEXB_VECTOR_INT2) / 3 + 2;
        type = (ref->type - FLEXB_VECTOR_INT2) % 3 + FLEXB_INT;
    } else {
        const void* size_offset = data - ref->byte_width;
        if (root != NULL && size_offset < root) {
            return FLEXB_CORRUPTED;
        }
        length = _flexb_get_uint64(size_offset, ref->byte_width);
    }
    // Getting the sub-type if any of the vector
    if (FLEXB_VECTOR_INT <= ref->type && ref->type <= FLEXB_VECTOR_STRING) {
        type = ref->type - FLEXB_VECTOR;
    }
    if (type) {
        type <<= 2;
        switch(ref->byte_width) {
            case 1:
                break;
            case 2:
                type |= 1;
                break;
            case 4:
                type |= 2;
                break;
            case 8:
                type |= 3;
                break;
            default: return FLEXB_CORRUPTED;
        }
    }
    vec->data = data;
    vec->byte_width = ref->byte_width;
    vec->type = type;
    vec->length = length;

    return FLEXB_SUCCESS;
}

inline int flexb_as_map(const void* root, const FLEXB_ref* ref, FLEXB_map *map) {
    if (map == NULL || ref == NULL) {
        return EINVAL;
    }
    int rc = 0;
    FLEXB_vec vec;
    if (ref->type !=  FLEXB_MAP) {
        return FLEXB_INVALID_CONVERSION;
    }
    rc = flexb_as_vec(root, ref, &vec);
    if (rc != FLEXB_SUCCESS) {
        return rc;
    }
    const void* data = _flexb_indirect(ref->data, ref->parent_width);
    const void* keys_offset = data - (vec.byte_width * 3);
    if (root != NULL && keys_offset < root) {
        return FLEXB_CORRUPTED;
    }
    map->values = vec;
    map->keys.data = _flexb_indirect(keys_offset, map->values.byte_width);
    const void* keys_width_offset = keys_offset + map->values.byte_width;
    map->keys.byte_width = _flexb_get_uint64(keys_width_offset, map->values.byte_width);
    map->keys.type = FLEXB_KEY;
    map->keys.length =  vec.length;
    return FLEXB_SUCCESS;
}

int _key_compare(const void *a, const void* b) {
    _FLEXB_key_cmp *key = (_FLEXB_key_cmp*) a;
    return strcmp((const char* )key->key, (const char*)_flexb_indirect(b, key->width));
}

inline int flexb_map_get_ref(const void* root, FLEXB_map *map, const char* key, FLEXB_ref* ref) {
    if (map == NULL || key == NULL || ref == NULL) {
        return EINVAL;
    }
    _FLEXB_key_cmp key_cmp = {key, map->keys.byte_width};
    const void* item = bsearch(&key_cmp, map->keys.data, map->keys.length, map->keys.byte_width, _key_compare);
    if (item == NULL) {
        return FLEXB_NOT_FOUND;
    }
    size_t index = (size_t)(item - map->keys.data) / map->keys.byte_width;
    uint8_t packed_byte = *(const uint8_t*)(map->values.data + (map->values.byte_width * map->values.length) + index);
    SET_REF(ref, map->values.data + (map->values.byte_width * index), map->values.byte_width, packed_byte);
    return FLEXB_SUCCESS;
}

inline int flexb_vec_get_ref(const void* root, const FLEXB_vec *vec, size_t index, FLEXB_ref* ref) {
    if (vec == NULL || ref == NULL) {
        return EINVAL;
    }
    if (index >= vec->length) {
        return FLEXB_NOT_FOUND;
    }
    uint8_t packed_byte = 0;
    if (vec->type == 0) {
        packed_byte = *(const uint8_t*)(vec->data + (vec->byte_width * vec->length) + index);
    } else {
        packed_byte = vec->type;
    }
    SET_REF(ref, vec->data + (vec->byte_width * index), vec->byte_width, packed_byte);
    return FLEXB_SUCCESS;
}

inline int flexb_mapsize(const void* root, const FLEXB_map* map, uint64_t* num) {
    *num = map->values.length;
    return FLEXB_SUCCESS;
}

inline int flexb_is_null(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_NULL;
}

inline int flexb_is_int(const FLEXB_ref* ref) {
    return ref != NULL && (ref->type <= FLEXB_INT || ref->type == FLEXB_INDIRECT_INT);
}

inline int flexb_is_uint(const FLEXB_ref* ref) {
    return ref != NULL && (ref->type <= FLEXB_UINT || ref->type == FLEXB_INDIRECT_UINT);
}

inline int flexb_is_float(const FLEXB_ref* ref) {
    return ref != NULL && (ref->type <= FLEXB_FLOAT || ref->type == FLEXB_INDIRECT_FLOAT);
}

inline int flexb_is_numeric(const FLEXB_ref* ref) {
    return ref != NULL && (
        (FLEXB_INT <= ref->type && ref->type <= FLEXB_FLOAT) ||
        (FLEXB_INDIRECT_INT <= ref->type && ref->type <= FLEXB_INDIRECT_FLOAT)
    );
}

inline int flexb_is_key(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_KEY;
}

inline int flexb_is_string(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_STRING;
}

inline int flexb_is_map(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_MAP;
}

inline int flexb_is_vector(const FLEXB_ref* ref) {
    return ref != NULL && ((FLEXB_MAP <= ref->type && ref->type <= FLEXB_VECTOR_FLOAT4) || ref->type == FLEXB_VECTOR_BOOL);
}

inline int flexb_is_typed_vector(const FLEXB_ref* ref) {
    return ref != NULL && ((FLEXB_VECTOR_INT <= ref->type && ref->type <= FLEXB_VECTOR_FLOAT4) || ref->type == FLEXB_VECTOR_BOOL);
}

inline int flexb_is_fixed_typed_vector(const FLEXB_ref* ref) {
    return ref != NULL && (FLEXB_VECTOR_INT2 <= ref->type && ref->type <= FLEXB_VECTOR_FLOAT4);
}

inline int flexb_is_blob(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_BLOB;
}

inline int flexb_is_bool(const FLEXB_ref* ref) {
    return ref != NULL && ref->type == FLEXB_BOOL;
}

#undef SET_REF

#endif
