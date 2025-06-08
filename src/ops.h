#ifndef __SPACED_H__OPS__
#define __SPACED_H__OPS__

#include "addressing.h"
#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "memory.h"

static void chip_op_beq(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_ZERO))
    self->pc += offset;
}

static void chip_op_bne(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_ZERO) == 0)
    self->pc += offset;
}

// STore X into memory
static void chip_op_stx(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->x);
}

// STore Y into memory
static void chip_op_sty(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->y);
}

// STore Accumulator into memory
static void chip_op_sta(chip_t *self, addressing_mode_t mode) {
  chip_memory_perform_write(self, mode, self->ac);
}

// INCrement memory
static void chip_op_inc(chip_t *self, addressing_mode_t mode) {
  u32 read = chip_memory_perform_read(self, mode);
  u16 addr = get_memory_addr(read);
  byte value = get_memory_word(read) + 1;

  chip_flags_update_zero_negative(self, value);
  chip_memory_write_direct(self, addr, value);
}

// DEcrement Y
static void chip_op_dey(chip_t *self) {
  self->y = (self->y - 1) & 0xFF;
  chip_flags_update_zero_negative(self, self->y);
}

// DEcrement X
static void chip_op_dex(chip_t *self) {
  self->x = (self->x - 1) & 0xFF;
  chip_flags_update_zero_negative(self, self->x);
}

// INcrement Y
static void chip_op_iny(chip_t *self) {
  self->y = (self->y + 1) & 0xFF;
  chip_flags_update_zero_negative(self, self->y);
}

// Jump with Saved Return
static void chip_op_jsr(chip_t *self) {
  u16 pc = (self->pc + 1) & 0xFFFF;

  chip_stack_push(self, (pc >> 8) & 0xFF);
  chip_stack_push(self, pc & 0xFF);

  self->pc = chip_memory_read_addr(self, ADDR_MODE_ABSOLUTE);
}

static void chip_op_pla(chip_t *self) {
  byte value = chip_stack_pull(self);
  self->ac = value;
  chip_flags_update_zero_negative(self, value);
}

static void chip_op_pha(chip_t *self) { chip_stack_push(self, self->ac); }

static void chip_op_cpy(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  byte result = self->y - value;

  chip_flags_update_zero_negative(self, result);
  chip_flags_update_carry(self, self->y >= value);
}

static void chip_op_rts(chip_t *self) {
  u16 addr = 0;
  addr |= (u16)chip_stack_pull(self);
  addr |= (u16)chip_stack_pull(self) << 8;
  self->pc = (addr + 1) & 0xFFFF;
}

static void chip_op_lda(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->ac = value;
  chip_flags_update_zero_negative(self, value);
}

static void chip_op_jmp(chip_t *self, addressing_mode_t mode) {
  u16 read = chip_memory_read_addr(self, mode);
  self->pc = read % 0xFFFF;
}

static void chip_op_ldy(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->y = value;
  chip_flags_update_zero_negative(self, value);
}

static void chip_op_ldx(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->x = value;
  chip_flags_update_zero_negative(self, value);
}

static void chip_op_tay(chip_t *self) {
  self->y = self->ac;
  // NOTE(Artur): Just philosophically, is it more beneficial to
  // use self->y or self->ac here?
  chip_flags_update_zero_negative(self, self->ac);
}

static void chip_op_tya(chip_t *self) {
  self->ac = self->y;
  chip_flags_update_zero_negative(self, self->y);
}

static void chip_op_txa(chip_t *self) {
  self->ac = self->x;
  chip_flags_update_zero_negative(self, self->x);
}

static void chip_op_tax(chip_t *self) {
  self->x = self->ac;
  chip_flags_update_zero_negative(self, self->ac);
}

static void chip_op_sec(chip_t *self) { chip_flags_set(self, FLAG_CARRY, 1); }

static void chip_op_sbc(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  byte carry = chip_flags_get(self, FLAG_CARRY);

  u16 result = (u16)self->ac - value - (1 - carry);

  chip_flags_update_carry(self, !(result & 0x100));
  chip_flags_update_zero_negative(self, result);

  // TODO: sanity check this, looks too complicated
  byte overflow =
      (self->ac ^ (byte)result) & ((~(self->ac ^ value) & 0x80) != 0);
  chip_flags_update_overflow(self, overflow);

  self->ac = result;
}

static void chip_op_adc(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  byte carry = chip_flags_get(self, FLAG_CARRY);

  u16 result = (u16)self->ac + value + carry;

  chip_flags_update_carry(self, result > 0xFF);
  chip_flags_update_zero_negative(self, result);

  byte overflow =
      (self->ac ^ (byte)result) & ((~(self->ac ^ value) & 0x80) != 0);
  chip_flags_update_overflow(self, overflow);

  self->ac = result;
}

// Branch on Carry Set
static void chip_op_bcs(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_CARRY) == 1)
    self->pc += offset;
}

// Branch on Carry Clear
static void chip_op_bcc(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_CARRY) == 0)
    self->pc += offset;
}

// DECrement memory
static void chip_op_dec(chip_t *self, addressing_mode_t mode) {
  u32 read = chip_memory_perform_read(self, mode);
  u16 addr = get_memory_addr(read);
  byte value = get_memory_word(read) - 1;

  chip_flags_update_zero_negative(self, value);
  chip_memory_write_direct(self, addr, value);
}

// CoMPare accumulator
static void chip_op_cmp(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  byte result = (self->ac - value) & 0xFF;

  chip_flags_update_carry(self, self->ac >= value);
  chip_flags_update_zero_negative(self, result);
}

static void chip_op_bvc(chip_t *self) {
  i8 offset = chip_memory_read_word(self, ADDR_MODE_RELATIVE);
  if (chip_flags_get(self, FLAG_OVERFLOW) == 0)
    self->pc += offset;
}

static void chip_op_eor(chip_t *self, addressing_mode_t mode) {
  byte value = chip_memory_read_word(self, mode);
  self->ac = self->ac ^ value;
  chip_flags_update_zero_negative(self, self->ac);
}

// Arithmetic Shift Left
static void chip_op_asl(chip_t *self, addressing_mode_t mode) {
  byte value;
  u16 addr;
  if (mode == ADDR_MODE_ACCUMULATOR)
    value = self->ac;
  else {
    byte read = chip_memory_perform_read(self, mode);
    value = get_memory_word(read);
    addr = get_memory_addr(read);
  }

  byte carry = (value >> 7) & 1;
  byte result = (value << 1) & 0xFF;

  chip_flags_update_carry(self, carry);
  chip_flags_update_zero_negative(self, result);

  if (mode == ADDR_MODE_ACCUMULATOR)
    self->ac = result;
  else
    chip_memory_write_direct(self, addr, result);
}

static void chip_op_rol(chip_t *self, addressing_mode_t mode) {
  byte value;
  u16 addr;
  if (mode == ADDR_MODE_ACCUMULATOR)
    value = self->ac;
  else {
    byte read = chip_memory_perform_read(self, mode);
    value = get_memory_word(read);
    addr = get_memory_addr(read);
  }

  byte carry = (value >> 7) & 1;
  byte result = ((value << 1) & 0xFF) | chip_flags_get(self, FLAG_CARRY);

  chip_flags_update_carry(self, carry);
  chip_flags_update_zero_negative(self, result);

  if (mode == ADDR_MODE_ACCUMULATOR)
    self->ac = result;
  else
    chip_memory_write_direct(self, addr, result);
}

static void chip_op_clc(chip_t *self) { chip_flags_set(self, FLAG_CARRY, 0); }

static void chip_step(chip_t *self) {
  byte opcode = chip_pc_inc(self);
  chip_decode_execute(self, opcode);
}

#endif
