#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../arena.h"
#include "../defs.h"
#include "../physics/sat.h"

#define EPSILON 1e-4

int _test_count = 6;
const char *_tests[] = {
    "_test_two_triangles_intersecting",  "_test_two_triangles_non_intersecting",
    "_test_two_squares_partial_overlap", "_test_corner_touching_squares",
    "_test_triangles_sharing_edge",      "_test_identical_triangles",
};

void _test_setup() { arena_init_default(&ARENA_GLOBAL); }

void _test_cleanup() { arena_cleanup(&ARENA_GLOBAL); }

static bool almost_equal(float a, float b) {
  return (a + EPSILON >= b) && (a - EPSILON <= b);
}

void _test_two_triangles_intersecting() {
  vec2 tri1[3] = {{1, 1}, {4, 1}, {2, 4}};
  vec2 tri2[3] = {{2, 2}, {5, 2}, {3, 5}};
  vec2 axis;
  bool ix = sat_intersect(tri1, 3, tri2, 3, &axis);

  ASSERT_(ix, "Expected triangles to intersect");
}

void _test_two_triangles_non_intersecting() {
  vec2 tri1[3] = {{-1, 0}, {0, 0}, {0, -1}};
  vec2 tri2[3] = {{1, 2}, {1, 1}, {2, 1}};
  vec2 axis;

  bool ix = sat_intersect(tri1, 3, tri2, 3, &axis);

  ASSERT_(!ix, "Expected triangles to not intersect");
}

void _test_two_squares_partial_overlap() {
  vec2 sq1[4] = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};
  vec2 sq2[4] = {{1, 1}, {3, 1}, {3, 3}, {1, 3}};
  vec2 axis;
  bool ix = sat_intersect(sq1, 4, sq2, 4, &axis);
  ASSERT_(ix, "Expected squares to intersect");

  ASSERT(almost_equal(axis.x, 0.00f), "axis.x = %.2f", axis.x);
  ASSERT(almost_equal(axis.y, 2.00f), "axis.y = %.2f", axis.y);
}

void _test_corner_touching_squares() {
  vec2 sq1[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  vec2 sq2[4] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}};
  vec2 axis;
  bool ix = sat_intersect(sq1, 4, sq2, 4, &axis);
  ASSERT_(!ix, "Expected corner-touching squares to not intersect");
}

void _test_triangles_sharing_edge() {
  vec2 tri1[3] = {{0, 0}, {2, 0}, {1, 2}};
  vec2 tri2[3] = {{2, 0}, {4, 0}, {3, 2}};
  vec2 axis;
  bool ix = sat_intersect(tri1, 3, tri2, 3, &axis);
  ASSERT_(!ix, "Expected edge-sharing triangles to not intersect");
}

void _test_identical_triangles() {
  vec2 tri[3] = {{0, 0}, {2, 0}, {1, 2}};
  vec2 axis;
  bool ix = sat_intersect(tri, 3, tri, 3, &axis);
  ASSERT_(ix, "Expected identical triangles to intersect");
  ASSERT(almost_equal(axis.x, -1.60f), "axis.x = %.2f", axis.x);
  ASSERT(almost_equal(axis.y, -0.80f), "axis.y = %.2f", axis.y);
}
