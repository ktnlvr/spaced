#ifndef __SPACED_H__MISC__
#define __SPACED_H__MISC__

#include "defs.h"

static u64 hash_combine_u64(u64 seed, u64 value) {
  return seed ^ (value + 0x9e3779b97f4a7c15 + (seed << 6) + (seed >> 2));
}

// Commutative combinational hash
static u64 hash_combine_com_u64(u64 a, u64 b) {
  // Combine the hashes
  u64 o = hash_combine_u64(a, b) ^ hash_combine_u64(b, a);
  // Mix up the bits
  o ^= 0x9e3779b97f4a7c15 + (a << 6) + (a >> 2);
  // Implemented using only commutative operations, so should be
  // commutative
  return o;
}

#endif
