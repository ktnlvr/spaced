#ifndef __SPACED_H__CHIP__
#define __SPACED_H__CHIP__

#include <stdio.h>
#include <string.h>

#include "defs.h"

typedef enum chip_quota_policy_t {
  // Consume 1 per instruction
  CHIP_QUOTA_POLICY_FLAT,
} chip_quota_policy_t;

typedef struct chip_t {
  // How the prices for the instructions are calculated
  chip_quota_policy_t quota_policy;
  // How many instructions it can afford to execute
  u32 quota;
  // The actual memory to be modified
  byte *memory;

  byte a, x, y, p;
  u16 sp, pc;
} chip_t;

static void chip_init(chip_t* self, chip_quota_policy_t quota_policy, u32 quota, byte* memory) {
  memset((void*)self, 0, sizeof(chip_t));
  
  self->quota_policy = quota_policy;
  self->quota = quota;

  self->memory = memory;
}

static void chip_dump_registers_stdout(chip_t* self) {
  printf("A=%02X X=%02X Y=%02X\n", self->a, self->x, self->y);
} 

#endif
