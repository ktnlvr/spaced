#include "../physics/sat.h"

int main(void) {
  vec2 a[] = {{1, 1}, {4, 1}, {2, 4}};
  vec2 b[] = {{2, 2}, {5, 2}, {3, 5}};

  bool intersect = sat_polygons(a, 3, b, 3);
  printf("%d intersect\n", intersect);

  return 0;
}
