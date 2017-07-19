#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "snf/flexb.h"

int tests_failed = 0;
int tests_passed = 0;

void test_failed (const char* test, const char* file, int line) {
    printf("FAILED: %s:%d %s\n", file, line, test);
    tests_failed++;
}

void test_passed (const char* test, const char* file, int line) {
    printf("PASSED: %s:%d %s\n", file, line, test);
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

static char byte_uint_bytes[]  = {0xff,8,1}; // 0xff
static char short_uint_bytes[] = {1,0xff,9,2}; // 0xff01
static char uint_bytes[]       = {1,2,3,0xff,10,4}; // 0x04030201
static char long_uint_bytes[]  = {1,2,3,4,5,6,7,0xff,11,8}; // 0x0100000000000000

void int_tests() {
    int64_t num = 0;
    SNF_flexb_ref root;
    IS_OK(snf_flexb_set_root(byte_int_bytes, 3, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 1);

    IS_OK(snf_flexb_set_root(short_int_bytes, 4, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0x0201);

    IS_OK(snf_flexb_set_root(int_bytes, 6, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0x04030201);

    IS_OK(snf_flexb_set_root(long_int_bytes, 10, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0x0807060504030201);

    IS_OK(snf_flexb_set_root(byte_neg_int_bytes, 3, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == -1);

    IS_OK(snf_flexb_set_root(byte_uint_bytes, 3, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0xff);


}

void uint_tests() {
    int64_t num = 0;
    SNF_flexb_ref root;
    IS_OK(snf_flexb_set_root(byte_uint_bytes, 3, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);

    IS_OK(snf_flexb_set_root(short_uint_bytes, 4, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0xff01);

    IS_OK(snf_flexb_set_root(uint_bytes, 6, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0xff030201);

    IS_OK(snf_flexb_set_root(long_uint_bytes, 10, &root) == 0);
    IS_OK(snf_flexb_as_int64(&root, &num) == 0);
    IS_OK(num == 0xff07060504030201);

}

int main() {
    int results = 0;
    int_tests();
    uint_tests();

    if (tests_failed) {
        results = 1;
    }
    printf("Tests succeeded %d\n",  tests_passed);
    printf("Tests failed %d\n",  tests_failed);

    return results;
}
