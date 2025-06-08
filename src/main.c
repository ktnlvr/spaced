#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "ops.h"

#define MEMORY_SIZE 0x10000

int main(void) {
  byte *memory = (byte *)malloc(MEMORY_SIZE);

  chip_t chip;
  chip_init(&chip, memory, 100);

  byte* rom = (byte*)malloc(MEMORY_SIZE);
  FILE *file = fopen("./examples/c/hello-world/main.bin", "rb");
  fseek(file, 0, SEEK_END);
  u16 filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  printf("ROM Size: %d\n", filesize);
  fread(rom, 1, filesize, file);
  fclose(file);

  chip_load_rom(&chip, rom, filesize, 0xF800);

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
