#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "ops.h"

#define MEMORY_SIZE 0x10000
#define PROGRAM_START 0x1000

int main(void) {
  byte *memory = (byte *)malloc(MEMORY_SIZE);

  chip_t chip;
  chip_init(&chip, memory, 44, PROGRAM_START);

  FILE *file = fopen("./examples/c/hello-world/main.bin", "rb");
  fread(memory + PROGRAM_START, 1, MEMORY_SIZE, file);
  fclose(file);

  while (chip.quota > 0) {
    chip.quota--;
    chip_step(&chip);
    chip_dbg_dump(&chip);
    printf("Anticipated Instruction: %s\n",
           opcode_to_str(chip.memory[chip.pc]));
  }

  file = fopen("./dump.bin", "wb");
  fwrite(chip.memory, 1, MEMORY_SIZE, file);

  return 0;
}
