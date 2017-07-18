#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ono/flexb.h"

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

void int_tests() {
    static uint8_t small_int_bytes[]   = {1,4,1}; // 0x01
    static uint8_t short_int_bytes[]   = {1,0,4,2}; // 0x0100
    static uint8_t regular_int_bytes[] = {1,0,0,0,4,4}; // 0x01000000
    static uint8_t big_int_bytes[]     = {1,0,0,0,0,0,0,0,4,8}; // 0x0100000000000000
    int64_t num = 0;
    ONO_flexb_ref root;
    IS_OK(ono_flexb_set_root(small_int_bytes, 3, &root) == 0);
    IS_OK(ono_flexb_as_int64(&root, &num) == 0);

}

int main() {
    int results = 0;

    if (tests_failed) {
        results = 1;
    }
    printf("Tests succeeded %d\n",  tests_passed);
    printf("Tests failed %d\n",  tests_failed);

    return results;
}
