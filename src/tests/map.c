#include "../map.h"
#include "../arena.h"

#include <stdio.h>

static map_t map;

void _test_setup() {
  arena_init_default(&ARENA_GLOBAL);
  map_init_ty(int, &map, arena_as_allocator(&ARENA_GLOBAL));
}

void _test_cleanup() {
  map_cleanup(&map);
  arena_cleanup(&ARENA_GLOBAL);
}

int _test_count = 5;
const char *_tests[] = {"_test_insert_get", "_test_insert_overwrite_get",
                        "_test_get_missing", "_test_collision_chain",
                        "_test_map_grow"};

void _test_insert_get() {
  int value = 16;
  map_insert_ty(int, &map, 0, &value);

  ASSERT__(*map_get_ty(int, &map, 0) == 16);
}

void _test_insert_overwrite_get() {
  int v1 = 16, v2 = 32;
  map_insert_ty(int, &map, 0, &v1);

  ASSERT__(map.size == 1);
  ASSERT__(*map_get_ty(int, &map, 0) == v1);

  map_insert_ty(int, &map, 0, &v2);

  ASSERT__(map.size == 1);
  ASSERT__(*map_get_ty(int, &map, 0) == v2);
}

void _test_get_missing() {
  int *ptr = map_get_ty(int, &map, 0);
  ASSERT__(ptr == 0);
}

void _test_collision_chain() {
  int v1 = 1, v2 = 2;
  map_insert_ty(int, &map, 0, &v1);
  map_insert_ty(int, &map, 4, &v2);

  ASSERT__(map.size == 2);
  ASSERT__(*map_get_ty(int, &map, 0) == v1);
  ASSERT__(*map_get_ty(int, &map, 4) == v2);
}

void _test_map_grow() {
  for (int i = 0; i < 100; i++)
    map_insert_ty(int, &map, i, &i);

    ASSERT__(map.size == 100);

  for (int i = 0; i < 100; i++)
    ASSERT__(*map_get_ty(int, &map, i) == i);
  ASSERT__(map.capacity >= map.size);
}
