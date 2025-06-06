#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <string.h>

#include "addressing.h"
#include "defs.h"

typedef struct chip_t {
  byte *memory;
  u32 quota;

  u16 pc;
  byte ac, x, y, sr, sp;
} chip_t;

static void chip_dbg_dump(chip_t *self) {
  printf("PC=0x%04X AC=0x%02X X=0x%02X Y=0x%02X SR=0x%02X SP=0x%02X\n", self->pc,
         self->ac, self->x, self->y, self->sr, self->sp);
}

static void chip_init(chip_t *self, byte *memory, u32 quota, u32 start_at) {
  memset(self, 0, sizeof(chip_t));
  self->memory = memory;
  self->quota = quota;
  self->pc = start_at;
}

static byte chip_memory_read_direct(chip_t *self, u16 at) {
  return self->memory[at % 0xFFFF];
}

static void chip_memory_write_direct(chip_t *self, u16 at, byte value) {
  self->memory[at] = value;
}

static byte chip_pc_inc(chip_t *self) { return self->memory[self->pc++]; }

static u32 chip_memory_read(chip_t *self, addressing_mode_t mode) {
  u16 value = 0;
  u16 addr = 0;

  switch (mode) {
  case ADDR_MODE_ABSOLUTE: {
    addr |= chip_pc_inc(self);
    addr |= (u16)chip_pc_inc(self) << 8;
    break;
  }
  case ADDR_MODE_IMMEDIATE: {
    addr = self->pc;
    chip_pc_inc(self);
    break;
  }
  }

  value |= (u32)chip_memory_read_direct(self, addr);
  value |= (u32)chip_memory_read_direct(self, addr + 1) << 8;

  return ((u32)addr << 16) | value;
}

static u16 get_memory_addr(u32 read) { return (read >> 16) & 0xFFFF; }

static u16 chip_memory_read_addr(chip_t *self, addressing_mode_t mode) {
  return get_memory_addr(chip_memory_read(self, mode));
}

static u16 chip_memory_read_word(chip_t *self, addressing_mode_t mode) {
  return chip_memory_read(self, mode) & 0xFFFF;
}

static u16 chip_memory_read_dword(chip_t *self, addressing_mode_t mode) {
  return chip_memory_read(self, mode) & 0xFF;
}

static u16 chip_memory_write(chip_t *self, addressing_mode_t mode, byte value) {
  return 0;
}

#endif
