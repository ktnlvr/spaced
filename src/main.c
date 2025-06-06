#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "defs.h"
#include "ops.h"

#define MEMORY_SIZE 0x10000

int main(void) {
  byte* memory = (byte*)malloc(MEMORY_SIZE);
  
  chip_t chip;
  chip_init(&chip, memory, 100, 0x200);

  FILE* file = fopen("./examples/c/hello-world/main.bin", "rb");
  fread(memory + 0x200, 1, MEMORY_SIZE, file);
  fclose(file);

  while (chip.quota > 0) {
    chip_step(&chip);
    chip.quota--;
    chip_dbg_dump(&chip);
  }

  return 0;
}
