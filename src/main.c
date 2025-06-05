#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "ops.h"

int main(void) {
  byte* kb = (byte*)malloc(0x10000);

  FILE* file = fopen("./o.bin", "rb");
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  fread(kb, 1, size, file);
  fclose(file);

  chip_t chip;

  chip_init(&chip, CHIP_QUOTA_POLICY_FLAT, 100, kb);

  for (int i = 0; i < 50; i++) {
    chip_dbg_dump(&chip);
    chip_step(&chip);
  }

  chip_dbg_dump(&chip);
  chip_dbg_dump_stack(&chip);

  return 0;
}
