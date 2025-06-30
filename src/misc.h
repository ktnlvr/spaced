#ifndef __H__MISC__
#define __H__MISC__

#include "defs.h"
#include "memory.h"

/// @brief Read a binary file `file` of into memory.
/// @param[out] size The final size of the buffer read
static void *read_binary_file(allocator_t allocator, const char *filename, sz* size) {
  ASSERT__(filename);
  ASSERT__(size);

  // TODO: do error handling
  FILE *file = fopen(filename, "rb");
  fseek(file, 0, SEEK_END);

  *size = ftell(file);
  rewind(file);

  byte *buffer = allocator_alloc_ty(byte, allocator, *size);
  fread(buffer, sizeof(byte), *size, file);
  fclose(file);

  return buffer;
}

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
