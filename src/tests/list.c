#include "../list.h"
#include "../arena.h"

static list_t list;

void _test_setup() {
  arena_init_default(&ARENA_GLOBAL);
  list_init_ty(int, &list, arena_as_allocator(&ARENA_GLOBAL));
}

void _test_cleanup() {
  list_cleanup(&list);
  arena_cleanup(&ARENA_GLOBAL);
}

int _test_count = 4;
const char *_tests[] = {"_test_empty_insert", "_test_middle_insert",
                        "_test_tail_insert", "_test_remove_every_second"};

void _test_empty_insert() {
  list_push_int(&list, 1);

  ASSERT_(list_get_ty(int, &list, 0) == 1, "The first element of a list is 1");
}

void _test_middle_insert() {
  list_push_int(&list, 0);
  list_push_int(&list, 1);
  list_push_int(&list, 3);
  list_push_int(&list, 4);

  ASSERT__(list.size == 4);

  int value = 2;
  list_insert(&list, 2, &value);

  ASSERT__(list.size == 5);

  for (int i = 0; i < 5; i++) {
    int v = list_get_ty(int, &list, i);
    ASSERT(v == i, "%d == %d", v, i);
  }
}

void _test_tail_insert() {
  for (int i = 0; i <= 0xFF; i++)
    list_push_int(&list, i);

  int value = 0x100;
  list_insert(&list, 0x100, &value);

  for (int i = 0; i <= 0x100; i++) {
    int v = list_get_ty(int, &list, i);
    ASSERT(v == i, "%d == %d", v, i);
  }
}

void _test_remove_every_second() {
  for (int i = 0; i <= 0xFF; i++)
    list_push_int(&list, i);

  for (int i = 0; i < 0x80; i++)
    list_remove(&list, i + 1);

  for (int i = 0; i < 0x80; i++) {
    int x = list_get_ty(int, &list, i * 2);
  }
}
