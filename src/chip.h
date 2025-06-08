#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <string.h>

#include "addressing.h"
#include "defs.h"

#define CHIP_STACK_BOTTOM_ADDR 0x0100

typedef struct chip_t {
  byte *memory;
  u32 quota;

  u16 pc;
  byte ac, x, y, sr, sp;
} chip_t;

typedef enum register_flags_t {
  FLAG_CARRY = 0,
  FLAG_ZERO = 1,
  FLAG_OVERFLOW = 6,
  FLAG_NEGATIVE = 7,
} register_flags_t;

typedef enum register_flags_mask_t {
  FLAG_MASK_CARRY = 0b00000001,
  FLAG_MASK_ZERO = 0b00000010,
  FLAG_MASK_OVERFLOW = 0b01000000,
  FLAG_MASK_NEGATIVE = 0b10000000,
} register_flags_mask_t;

static void chip_dbg_dump(chip_t *self) {
  printf("PC=0x%04X AC=0x%02X X=0x%02X Y=0x%02X SR=0x%02X SP=0x%02X\n",
         self->pc, self->ac, self->x, self->y, self->sr, self->sp);
}

static void chip_init(chip_t *self, byte *memory, u32 quota) {
  memset(self, 0, sizeof(chip_t));
  self->memory = memory;
  self->quota = quota;
  self->pc = 0;
  self->sp = 0xFF;
}

static void chip_load_rom(chip_t* self, byte* rom, size_t len, u16 rom_start) {
  memcpy(self->memory + rom_start, rom, len);
  self->pc = self->memory[0xFFFC] | (self->memory[0xFFFD] << 8);
  printf("PC=%04X\n", self->pc);
}

static byte chip_memory_read_direct(chip_t *self, u16 at) {
  return self->memory[at & 0xFFFF];
}

static void chip_memory_write_direct(chip_t *self, u16 at, byte value) {
  self->memory[at] = value;
}

static byte chip_pc_inc(chip_t *self) { return self->memory[self->pc++]; }

static u32 chip_memory_perform_read(chip_t *self, addressing_mode_t mode) {
  u16 value = 0;
  u16 addr = 0;

  switch (mode) {
  case ADDR_MODE_ABSOLUTE:
    addr |= chip_pc_inc(self);
    addr |= (u16)chip_pc_inc(self) << 8;
    break;
  case ADDR_MODE_ABSOLUTE_X:
    addr |= chip_pc_inc(self);
    addr |= (u16)chip_pc_inc(self) << 8;
    addr = (addr + self->x) & 0xFFFF;
    break;
  case ADDR_MODE_ABSOLUTE_Y:
    addr |= chip_pc_inc(self);
    addr |= (u16)chip_pc_inc(self) << 8;
    addr = (addr + self->y) & 0xFFFF;
    break;
  case ADDR_MODE_IMMEDIATE:
    addr = self->pc;
    chip_pc_inc(self);
    break;
  case ADDR_MODE_ZERO_PAGE:
    addr = chip_pc_inc(self);
    break;
  case ADDR_MODE_INDIRECT_Y: {
    u16 zp_addr = chip_pc_inc(self);
    addr |= (u16)chip_memory_read_direct(self, zp_addr);
    addr |= (u16)chip_memory_read_direct(self, (zp_addr + 1) & 0xFFFF) << 8;
    addr += self->y;
    break;
  }
  case ADDR_MODE_ACCUMULATOR:
    PANIC_("The ACCUMULATOR addressing read mode is handled on per-instruction basis");
  default:
    PANIC("Unhandled addressing mode read %d", mode);
  }

  value |= (u32)chip_memory_read_direct(self, addr);
  value |= (u32)chip_memory_read_direct(self, (addr + 1) & 0xFF) << 8;

  return ((u32)addr << 16) | value;
}

static u16 chip_memory_perform_write(chip_t *self, addressing_mode_t mode,
                                     byte value) {
  u16 addr = 0;

  switch (mode) {
  case ADDR_MODE_ABSOLUTE: {
    addr |= chip_pc_inc(self);
    addr |= (u16)chip_pc_inc(self) << 8;
    break;
  }
  case ADDR_MODE_ZERO_PAGE: {
    addr = chip_pc_inc(self);
    break;
  }
  case ADDR_MODE_ZERO_PAGE_Y: {
    // NOTE(Artur): can this method leave the zero page?
    // that would be a cool bug/feature
    // in the original 6502 it will just ignore the higher
    // byte, but I think its an interesting feature
    // https://www.masswerk.at/6502/6502_instruction_set.html#modes
    addr = chip_pc_inc(self);
    addr += self->y;
    addr &= 0xFF;
    break;
  }
  case ADDR_MODE_INDIRECT_Y: {
    u16 zp_addr = chip_pc_inc(self);
    addr |= (u16)chip_memory_read_direct(self, zp_addr);
    addr |= (u16)chip_memory_read_direct(self, (zp_addr + 1) & 0xFFFF) << 8;
    addr += self->y;
    break;
  }
  case ADDR_MODE_ACCUMULATOR:
    PANIC_("The ACCUMULATOR addressing read mode is handled on per-instruction basis");
  default:
    PANIC("Unhandled addressing mode write %d", mode);
  }

  return self->memory[addr];
}

static u16 get_memory_addr(u32 read) { return (read >> 16) & 0xFFFF; }

static byte get_memory_word(u32 read) { return read & 0xFF; }

static u16 chip_memory_read_addr(chip_t *self, addressing_mode_t mode) {
  return get_memory_addr(chip_memory_perform_read(self, mode));
}

static byte chip_memory_read_word(chip_t *self, addressing_mode_t mode) {
  return chip_memory_perform_read(self, mode) & 0xFF;
}

static u16 chip_memory_read_dword(chip_t *self, addressing_mode_t mode) {
  return chip_memory_perform_read(self, mode) & 0xFFFF;
}

static void chip_stack_push(chip_t *self, byte value) {
  if (self->sp == 0x00) {
    PANIC_("Stack overflow");
  }

  chip_memory_write_direct(self, CHIP_STACK_BOTTOM_ADDR + self->sp, value);
  self->sp--;
  self->sp &= 0xFF;
}

static byte chip_stack_pull(chip_t *self) {
  if (self->sp == 0xFF) {
    PANIC_("Stack underflow");
  }

  self->sp++;
  self->sp &= 0xFF;
  byte value = chip_memory_read_direct(self, CHIP_STACK_BOTTOM_ADDR + self->sp);
  return value;
}

static byte chip_flags_get(chip_t *self, register_flags_t flag) {
  return (self->sr >> flag) & 1;
}

static void chip_flags_set(chip_t *self, register_flags_t flag, byte value) {
  self->sr &= ~(1 << flag);
  self->sr |= !!value << flag;
}

static void chip_flags_update_carry(chip_t *self, byte value) {
  chip_flags_set(self, FLAG_CARRY, value);
}

static void chip_flags_update_zero_negative(chip_t *self, byte value) {
  register_flags_mask_t mask =
      (register_flags_mask_t)(FLAG_MASK_ZERO | FLAG_MASK_NEGATIVE);
  self->sr &= ~mask;
  chip_flags_set(self, FLAG_ZERO, value == 0);
  chip_flags_set(self, FLAG_NEGATIVE, value & 0x80);
}

static void chip_flags_update_overflow(chip_t* self, byte value) {
  chip_flags_set(self, FLAG_OVERFLOW, value);
}

#endif
