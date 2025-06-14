#include "../physics/sat.h"

int main(void) {
  vec2 a[] = {{1, 1}, {4, 1}, {2, 4}};
  vec2 b[] = {{2, 2}, {5, 2}, {3, 5}};

  float2 minimal_separation;
  bool intersect = sat_intersect(a, 3, b, 3, &minimal_separation);

  printf("%d intersect\n", intersect);
  printf("%f %f separation\n", minimal_separation.x, minimal_separation.y);

  return 0;
}
