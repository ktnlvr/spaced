#include "../../list.h"

int main(void) {
  allocator_t alloc = allocator_new_malloc();

  list_t list;
  list_init_ty(int, &list, alloc);

  int iters;
  scanf("%d", &iters);

  for (int i = 0; i < iters; i++) {
    int push, insert, delete, retrieve;
    scanf("%d %d %d %d", &push, &insert, &delete, &retrieve);

    for (int i = 0; i < push; i++) {
      int v;
      scanf("%d", &v);
      list_push(&list, &v);
    }

    for (int i = 0; i < insert; i++) {
      int k, v;
      scanf("%d %d", &k, &v);
      list_insert(&list, k, &v);
    }

    for (int i = 0; i < delete; i++) {
      int k;
      scanf("%d", &k);
      PANIC_("Deletion not implemented yet");
    }

    for (int i = 0; i < retrieve; i++) {
      int k;
      scanf("%d", &k);
      printf("%d\n", list_get_ty(int, &list, k));
    }
  }

  list_cleanup(&list);

  return 0;
}
