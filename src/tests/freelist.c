#include "../misc/freelist.h"

#define LEDGER_CAPACITY 0x10000

static freelist_ledger_t ledger;

void _test_setup() {
  freelist_ledger_init(&ledger, allocator_new_malloc(), LEDGER_CAPACITY);
}

void _test_cleanup() { freelist_ledger_cleanup(&ledger); }

int _test_count = 1;
const char *_tests[] = {"_test_max_out"};

void _test_max_out() {
  freelist_ledger_ptr_t ptr = freelist_ledger_reserve(&ledger, LEDGER_CAPACITY);
  ASSERT__(ptr != FREELIST_LEDGER_INVALID_BLOCK);
  ASSERT__(ptr == 0);
}
