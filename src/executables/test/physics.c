#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../arena.h"
#include "../../physics/sat.h"

int sat_test(int shapes, bool calculate_minimal) {
  int *vertex_count = arena_alloc_ty(int, shapes);
  vec2 **vertices = arena_alloc_ty(vec2 *, shapes);
  vec2 minimal_axis;

  for (int i = 0; i < shapes; i++) {
    scanf("%d", &vertex_count[i]);
    vertices[i] = arena_alloc_ty(vec2, vertex_count[i]);
    for (int j = 0; j < vertex_count[i]; j++)
      scanf("%f %f", &vertices[i][j].x, &vertices[i][j].y);
  }

  for (int i = 0; i < shapes; i++) {
    for (int j = 0; j < i; j++) {
      bool ix = sat_intersect(vertices[i], vertex_count[i], vertices[j],
                              vertex_count[j], &minimal_axis);
      printf("%d\n", ix);
      if (ix && calculate_minimal) {
        printf("%.2f %.2f\n", minimal_axis.x, minimal_axis.y);
      }
    }
  }

  return 0;
}

int main(void) {
  arena_init_default();

  int ret, shapes;
  if (scanf("sat-test %d", &shapes)) {
    ret = sat_test(shapes, true);
  } else {
    fprintf(stderr, "Failed to match the test type to a mode");
  }

  arena_free();

  return ret;
}
