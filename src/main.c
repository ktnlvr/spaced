#include <stdio.h>

#include "chip.h"
#include "ops.h"

int main(void) {
  byte kb[0x10000] = {
    _OP_NOOP(),
    _OP_CONST(5),
    _OP_COPY(R_AR_LO, R_ACC),

    _OP_CONST(1),
    _OP_COPY(R_X, R_ACC),
    _OP_ADD(R_X),
    _OP_SWAP(R_X, R_ACC),
    _OP_JMPZ(R_AR_HI),
  };

  chip_t chip;

  chip_init(&chip, CHIP_QUOTA_POLICY_FLAT, 100, kb);

  for (int i = 0; i < 50; i++) {
    chip_dbg_dump(&chip);
    chip_step(&chip);
  }

  chip_dbg_dump(&chip);

  return 0;
}
