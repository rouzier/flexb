#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "flexb/flexb.h"

int tests_failed = 0;
int tests_passed = 0;

void test_failed (const char* test, const char* file, int line) {
    printf("!FAILED: %s:%03d %s\n", file, line, test);
    tests_failed++;
}

void test_passed (const char* test, const char* file, int line) {
    printf(" PASSED: %s:%03d %s\n", file, line, test);
    tests_passed++;
}

void test_ok(int passed, const char* test, const char* file, int line) {
    if (!passed) {
        test_failed(test, file, line);
    } else {
        test_passed(test, file, line);
    }
}

#define IS_OK(exp) do{ test_ok((exp), #exp, __FILE__, __LINE__); } while(0)
static char byte_int_bytes[]  = {1,4,1}; // 0x01
static char short_int_bytes[] = {1,2,5,2}; // 0x0201
static char int_bytes[]       = {1,2,3,4,6,4}; // 0x04030201
static char long_int_bytes[]  = {1,2,3,4,5,6,7,8,7,8}; // 0x0100000000000000
static char byte_neg_int_bytes[]  = {0xff,4,1}; // 0xff
static char byte_int_indirect_bytes[]  = {1,1,24,1}; // 0x01

static char byte_uint_bytes[]  = {0xff,8,1}; // 0xff
static char short_uint_bytes[] = {1,0xff,9,2}; // 0xff01
static char uint_bytes[]       = {1,2,3,0xff,10,4}; // 0x04030201
static char long_uint_bytes[]  = {1,2,3,4,5,6,7,0xff,11,8}; // 0x0100000000000000

static char bad_byte_width[]  = {1,4,3}; // Corrupted byte width
static char bad_type[]  = {1,108,1}; // Corrupted byte width

static char typed_int_vector[]  = {3,1,2,3,3,44,1}; // [1, 2, 3]
static char typed_int3_vector[]  = {1,2,3,3,80,1}; // [1, 2, 3]

// { vec: [ -100, "Fred", 4.0, false ], bar: [ 1, 2, 3 ], bar3: [ 1, 2, 3 ], foo: 100, bool: true, mymap: { foo: "Fred", sbool1 : "true", sbool2: "false", sbool3: "0", sbool3: "1" } }
static char map_bytes[]={ 118, 101, 99, 0, 4, 70, 114, 101, 100, 0, 0, 0, 0, 0, 128, 64, 4, 156, 13, 7, 0, 4, 20, 34, 104, 98, 97, 114, 0, 3, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 98, 97, 114, 51, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 98, 111, 111, 108, 0, 102, 111, 111, 0, 109, 121, 109, 97, 112, 0, 115, 98, 111, 111, 108, 49, 0, 4, 116, 114, 117, 101, 0, 115, 98, 111, 111, 108, 50, 0, 5, 102, 97, 108, 115, 101, 0, 115, 98, 111, 111, 108, 51, 0, 1, 49, 0, 115, 98, 111, 111, 108, 52, 0, 1, 48, 0, 5, 58, 49, 37, 24, 15, 5, 1, 5, 128, 49, 37, 24, 15, 20, 20, 20, 20, 20, 6, 119, 100, 84, 80, 77, 149, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 6, 0, 0, 0, 131, 0, 0, 0, 118, 0, 0, 0, 1, 0, 0, 0, 0, 0, 200, 66, 47, 0, 0, 0, 167, 0, 0, 0, 46, 78, 106, 14, 36, 40, 30, 38, 1 };

void int_tests() {
    int64_t num = 0;
    FLEXB_ref ref = {};

    IS_OK(flexb_set_root(byte_int_bytes, 3, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 1);

    IS_OK(flexb_set_root(short_int_bytes, 4, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 0x0201);

    IS_OK(flexb_set_root(int_bytes, 6, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 0x04030201);

    IS_OK(flexb_set_root(long_int_bytes, 10, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 0x0807060504030201);

    IS_OK(flexb_set_root(byte_neg_int_bytes, 3, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == -1);

    IS_OK(flexb_set_root(byte_uint_bytes, 3, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 0xff);

    IS_OK(flexb_set_root(byte_int_indirect_bytes, 4, &ref) == 0);
    IS_OK(flexb_as_int64(&ref, &num) == 0);
    IS_OK(num == 1);
}

void uint_tests() {
    uint64_t num = 0;
    FLEXB_ref ref = {};
    IS_OK(flexb_set_root(byte_uint_bytes, 3, &ref) == 0);
    IS_OK(flexb_as_uint64(&ref, &num) == 0);

    IS_OK(flexb_set_root(short_uint_bytes, 4, &ref) == 0);
    IS_OK(flexb_as_uint64(&ref, &num) == 0);
    IS_OK(num == 0xff01);

    IS_OK(flexb_set_root(uint_bytes, 6, &ref) == 0);
    IS_OK(flexb_as_uint64(&ref, &num) == 0);
    IS_OK(num == 0xff030201);

    IS_OK(flexb_set_root(long_uint_bytes, 10, &ref) == 0);
    IS_OK(flexb_as_uint64(&ref, &num) == 0);
    IS_OK(num == 0xff07060504030201);
}

void map_tests() {
    uint64_t num = 0;
    int64_t num2 = 0;
    double num3 = 0.0;
    const char* str;
    FLEXB_ref ref  = {};
    FLEXB_ref ref2 = {};
    FLEXB_ref ref3 = {};
    FLEXB_map map  = {};
    FLEXB_vec vec  = {};

    IS_OK(flexb_set_root(map_bytes, sizeof(map_bytes), &ref) == 0);
    IS_OK(ref.type == FLEXB_MAP);
    IS_OK(flexb_as_map(map_bytes, &ref, &map) == 0);
    IS_OK((ref.data - map.values.data) == 30);
    IS_OK(map.values.byte_width == 4);
    IS_OK(flexb_mapsize(map_bytes, &map, &num) == 0);
    IS_OK(num == 6);
    IS_OK(flexb_map_get_ref(map_bytes, &map, "vec", &ref2) == 0);
    IS_OK(ref2.type == FLEXB_VECTOR);
    IS_OK(ref2.parent_width == map.values.byte_width);
    IS_OK(ref2.byte_width == 1);
    IS_OK(flexb_as_vec(map_bytes, &ref2, &vec) == 0);
    IS_OK(vec.length == 4);

    IS_OK(flexb_vec_get_ref(map_bytes, &vec, 4, &ref3) != 0);

    IS_OK(flexb_vec_get_ref(map_bytes, &vec, 0, &ref3) == 0);
    IS_OK(ref3.type == FLEXB_INT);
    IS_OK(flexb_as_int64(&ref3, &num2) == 0);
    IS_OK(num2 == -100);

    IS_OK(flexb_vec_get_ref(map_bytes, &vec, 1, &ref3) == 0);
    IS_OK(ref3.type == FLEXB_STRING);
    IS_OK(flexb_as_str(map_bytes, &ref3, &str) == 0);
    IS_OK(strcmp(str, "Fred") == 0);

    IS_OK(flexb_as_blob(map_bytes, &ref3, &str, &num) == 0);
    IS_OK(strcmp(str, "Fred") == 0);
    IS_OK(num == 4);

    IS_OK(flexb_vec_get_ref(map_bytes, &vec, 2, &ref3) == 0);
    IS_OK(ref3.type == FLEXB_INDIRECT_FLOAT);
    IS_OK(flexb_as_float(map_bytes, &ref3, &num3) == 0);
    IS_OK(num3 == 4.0);
}


void vec_tests() {
    uint64_t num = 0;
    FLEXB_ref ref = {};
    FLEXB_ref ref2 = {};
    FLEXB_vec vec = {};

    IS_OK(flexb_set_root(typed_int_vector, 7, &ref) == 0);
    IS_OK(flexb_as_vec(typed_int_vector, &ref, &vec) == 0);
    IS_OK(ref.type == FLEXB_VECTOR_INT);
    IS_OK(ref.byte_width == 1);
    IS_OK(ref.parent_width == 1);
    IS_OK(ref.data == (4 + typed_int_vector));

    IS_OK(flexb_vec_get_ref(typed_int_vector, &vec, 0, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 1);

    IS_OK(flexb_vec_get_ref(typed_int_vector, &vec, 1, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 2);

    IS_OK(flexb_vec_get_ref(typed_int_vector, &vec, 2, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 3);

    IS_OK(flexb_set_root(typed_int3_vector, 6, &ref) == 0);
    IS_OK(flexb_as_vec(typed_int3_vector, &ref, &vec) == 0);
    IS_OK(ref.type == FLEXB_VECTOR_UINT3);
    IS_OK(ref.byte_width == 1);
    IS_OK(ref.parent_width == 1);
    IS_OK(ref.data == (3 + typed_int3_vector));

    IS_OK(flexb_vec_get_ref(typed_int3_vector, &vec, 0, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 1);

    IS_OK(flexb_vec_get_ref(typed_int3_vector, &vec, 1, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 2);

    IS_OK(flexb_vec_get_ref(typed_int3_vector, &vec, 2, &ref2) == 0);
    IS_OK(flexb_as_uint64(&ref2, &num) == 0);
    IS_OK(num == 3);
}

void bad_data () {
    FLEXB_ref ref = {};
    IS_OK(flexb_set_root(bad_byte_width, 3, &ref) == FLEXB_CORRUPTED);
    IS_OK(flexb_set_root(bad_type, 3, &ref) == FLEXB_CORRUPTED);
}


int main() {
    int results = 0;
    int_tests();
    uint_tests();
    map_tests();
    vec_tests();
    bad_data();

    if (tests_failed) {
        results = 1;
    }
    printf("Tests succeeded %d\n",  tests_passed);
    printf("Tests failed %d\n",  tests_failed);

    return results;
}
