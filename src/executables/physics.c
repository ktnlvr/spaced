#include <stdio.h>
#include <string.h>

#include "../arena.h"
#include "../physics/sat.h"

int sat_poly() {
  int shapes;
  scanf("%d", &shapes);

  int *vertex_count = arena_alloc_ty(int, shapes);
  vec2 **vertices = arena_alloc_ty(vec2 *, shapes);

  for (int i = 0; i < shapes; i++) {
    scanf("%d", &vertex_count[i]);
    vertices[i] = arena_alloc_ty(vec2, vertex_count[i]);
    for (int j = 0; j < vertex_count[i]; j++)
      scanf("%f %f", &vertices[i][j].x, &vertices[i][j].y);
  }

  for (int i = 0; i < shapes; i++) {
    for (int j = 0; j < i; j++) {
      bool ix = sat_intersect(vertices[i], vertex_count[i], vertices[j],
                              vertex_count[j], 0);
      printf("%d\n", ix);
    }
  }

  return 0;
}

int main(void) {
  arena_init_default();

  // TODO: switch based on the mode
  int ret = sat_poly();

  arena_free();

  return ret;
}
