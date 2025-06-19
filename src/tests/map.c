#include "../map.h"
#include "../arena.h"

#include <stdio.h>

int main(void) {
  allocator_t alloc = allocator_new_malloc();

  map_t map;
  map_init_ty(int, &map, alloc);

  int iters;
  scanf("%d", &iters);

  for (int i = 0; i < iters; i++) {
    int insert, delete, retrieve;
    scanf("%d %d %d", &insert, &delete, &retrieve);
    for (int i = 0; i < insert; i++) {
      int k, v;
      scanf("%d %d", &k, &v);
      map_insert_ty(int, &map, k, &v);
    }

    for (int i = 0; i < delete; i++) {
      int k;
      scanf("%d", &k);
      PANIC_("Deletion not implemented yet");
    }

    for (int i = 0; i < retrieve; i++) {
      int k;
      scanf("%d", &k);
      int *ptr = map_get_ty(int, &map, k);

      if (ptr == 0)
        printf("NA\n");
      else
        printf("%d\n", *ptr);
    }
  }

  map_cleanup(&map);

  return 0;
}
