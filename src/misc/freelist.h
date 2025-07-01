#ifndef __H__MISC_FREELIST__
#define __H__MISC_FREELIST__

#include "../list.h"

#define FREELIST_LEDGER_INVALID_BLOCK (~(u32)0)

typedef u32 freelist_ledger_ptr_t;

/// @private @memberof freelist_ledger_t
typedef struct {
  freelist_ledger_ptr_t start;
  freelist_ledger_ptr_t end;
} _freelist_ledger_block_t;

/// @brief An object that implements malloc-like
/// behaviour without specifying the exact internal
/// object being managed.
///
/// Functionallity equivalent to a slab allocator
/// without any actual memory control.
typedef struct {
  /// @brief The blocks that were reserved. Contrary to the name,
  /// this list stores the *busy* blocks, not the *free* ones.
  list_t _blocks;
  /// @brief The maximum amount of slots managed by the ledger.
  /// Errors out if reached.
  u32 capacity;
} freelist_ledger_t;

/// @memberof freelist_ledger_t
static void freelist_ledger_init(freelist_ledger_t *ledger,
                                 allocator_t allocator, u32 capacity) {
  ledger->capacity = capacity;
  list_init_ty(_freelist_ledger_block_t, &ledger->_blocks, allocator);
}

/// @brief Reserves the next block from the ledger. Returns @ref FREELIST_LEDGER_INVALID_BLOCK
/// if the requested block does not fit.
/// @memberof freelist_ledger_t
static freelist_ledger_ptr_t freelist_ledger_reserve(freelist_ledger_t *ledger,
                                                     u32 size) {
  ASSERT__(ledger->capacity <= size);

  freelist_ledger_ptr_t last_block_end = 0;
  for (sz i = 0; i < ledger->_blocks.size; i++) {
    _freelist_ledger_block_t *blk =
        list_get_ty_ptr(_freelist_ledger_block_t, &ledger->_blocks, i);

    if (blk->start - last_block_end <= size) {
      _freelist_ledger_block_t new_blk;
      new_blk.start = last_block_end;
      new_blk.end = blk->start;
      list_insert(&ledger->_blocks, i, &new_blk);
      return new_blk.start;
    }
  }

  if (ledger->capacity - last_block_end <= size) {
    _freelist_ledger_block_t new_blk;
    new_blk.start = last_block_end;
    new_blk.end = last_block_end + size;
    list_push_var(&ledger->_blocks, new_blk);
    return new_blk.start;
  }

  return FREELIST_LEDGER_INVALID_BLOCK;
}

/// @brief Returns the block back into the pool.
/// @memberof freelist_ledger_t
static void freelist_ledger_release(freelist_ledger_t *ledger,
                                    freelist_ledger_ptr_t at) {
  for (sz i = 0; i < ledger->_blocks.size; i++) {
    _freelist_ledger_block_t *blk =
        list_get_ty_ptr(_freelist_ledger_block_t, &ledger->_blocks, i);

    if (blk->start == at) {
      list_remove(&ledger->_blocks, i);
      return;
    }

    ASSERT(at < blk->start,
           "Attempt to release block at %d, possibly a member of block %d", at,
           blk->start);
  }
}

/// @memberof freelist_ledger_t
static void freelist_ledger_cleanup(freelist_ledger_t *ledger) {
  list_cleanup(&ledger->_blocks);
}

#endif
