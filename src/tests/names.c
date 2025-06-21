#include "../arena.h"
#include "../names.h"

#include <stdio.h>

void _test_setup() {
  names_init();
}

void _test_cleanup() {
  names_cleanup();
}

int _test_count = 1;
const char *_tests[] = {"_test_simple_name",};

void _test_simple_name() {
    ASSERT__(_name_list.size == 0);

    name_t a1 = as_name("a");
    name_t b = as_name("b");
    name_t a2 = as_name("a");

    ASSERT__(_name_list.size == 2);

    ASSERT__(a1 == a2);
    ASSERT__(a1 != b);
    ASSERT__(a2 != b);
    ASSERT__(b == b);
}
