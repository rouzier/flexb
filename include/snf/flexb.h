#ifndef __SNF_FLEXBUFFERS__
#define __SNF_FLEXBUFFERS__

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define FLEXB_NULL  0
#define FLEXB_INT   1
#define FLEXB_UINT  2
#define FLEXB_FLOAT 3

typedef struct SNF_flexb_ref {
    const uint8_t * data;
    uint8_t parent_width;
    uint8_t byte_width;
    uint8_t type;
} SNF_flexb_ref;

inline int snf_flexb_set_root( const char* root, size_t length, SNF_flexb_ref* ref) {
    if (root == NULL || ref == NULL || length < 3) {
        return EINVAL;
    }

    const uint8_t* end = root + length;
    const uint8_t byte_width = *--end;
    const uint8_t packed_byte = *--end;
    ref->byte_width = byte_width;
    ref->type = (packed_byte >> 2);
    ref->parent_width  = 1 << (packed_byte & 0x03);
    ref->data = end - byte_width;
    return 0;
}

inline int64_t snf_flexb_get_int64(const char* data, int width) {
    int64_t num;
    switch (width) {
    case 1:
        {
           int8_t tmp;
           memcpy(&tmp, data, 1);
           num = (int64_t) tmp;
        }
        return ;
    case 2:
        {
           int16_t tmp;
           memcpy(&tmp, data, 2);
           num = (int64_t) tmp;
        }
        return 0;
    case 4:
        {
           int32_t tmp;
           memcpy(&tmp, data, 4);
           num = (int64_t) tmp;
        }
        return 0;
    case 8:
        {
           memcpy(&num, data, 8);
        }
        return 0;
    default:
        assert(0);
    }
    return num;
}

inline int snf_flexb_as_int64(SNF_flexb_ref* ref, int64_t *num) {
    switch(ref->type) {
    case FLEXB_INT:
        *num = snf_flexb_get_int64(ref->data, ref->parent_width);
        return 0;
    }
    return 1;
}

#endif
