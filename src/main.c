#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "defs.h"
#include "generated/lookup.h"
#include "ops.h"

#define MEMORY_SIZE 0x10000

int main(int argc, const char* argv[]) {
  byte *memory = (byte *)malloc(MEMORY_SIZE);
  memset(memory, 0, MEMORY_SIZE);

  chip_t chip = {};
  chip_init(&chip, memory, 0);

  byte* rom = (byte*)malloc(MEMORY_SIZE);
  FILE *file = fopen(argv[1], "rb");
  fseek(file, 0, SEEK_END);
  u16 filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  printf("ROM Size: %d\n", filesize);
  fread(rom, 1, filesize, file);
  fclose(file);

  chip_load_rom(&chip, rom, filesize, 0xF800);

  while (!chip.halted) {
    chip_dbg_dump(&chip);
    printf("Next: %s\n", opcode_to_str(chip.memory[chip.pc]));
    chip_step(&chip);
  }

  file = fopen("./dump.bin", "wb");
  fwrite(chip.memory, 1, MEMORY_SIZE, file);

  return 0;
}
