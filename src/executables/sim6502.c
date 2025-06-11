#include <stdio.h>
#include <stdlib.h>

#include "../6502/chip.h"
#include "../6502/ops.h"

#define MEMORY_SIZE 0x10000

void chip_show_registers(chip_t *self) {
  printf("S PC=%04X A=%02X X=%02X Y=%02X SP=%02X\n", self->pc, self->ac,
         self->x, self->y, self->sp);
}

int main(int argc, const char *argv[]) {
  // Disable for better reading from other programs
  setvbuf(stdout, NULL, _IOLBF, 0);

  byte *memory = (byte *)malloc(MEMORY_SIZE);
  memset(memory, 0, MEMORY_SIZE);

  chip_t chip = {};
  chip_init(&chip, memory, 0);

  byte *rom = (byte *)malloc(MEMORY_SIZE);
  FILE *file = fopen(argv[1], "rb");
  fseek(file, 0, SEEK_END);
  u16 filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fread(rom, 1, filesize, file);
  fclose(file);

  chip_load_rom(&chip, rom, filesize, 0xF800);

  while (!chip.halted) {
    chip_show_registers(&chip);
    printf("O %02X %s\n", chip.memory[chip.pc],
           opcode_to_str(chip.memory[chip.pc]));
    chip_step(&chip);
  }

  chip_show_registers(&chip);

  file = fopen("./dump.bin", "wb");
  fwrite(chip.memory, 1, MEMORY_SIZE, file);

  return 0;
}
