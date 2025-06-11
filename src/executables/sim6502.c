#include <stdio.h>
#include <stdlib.h>

#include "../6502/chip.h"
#include "../6502/ops.h"

#define MEMORY_SIZE 0x10000

typedef struct chip_monitor_t {
  u16 write_ptr;
  u16 read_ptr;
  u16 write_addresses[16];
  u16 read_addresses[16];
} chip_monitor_t;

byte memory_write_callback(chip_monitor_t *monitor, u16 addr, byte value) {
  monitor->write_addresses[monitor->write_ptr++] = addr;
  return value;
}

byte memory_read_callback(chip_monitor_t *monitor, u16 addr, byte value) {
  monitor->read_addresses[monitor->read_ptr++] = addr;
  return value;
}

void chip_show_registers(chip_t *self) {
  printf("S PC=%04X A=%02X X=%02X Y=%02X SP=%02X\n", self->pc, self->ac, self->x,
         self->y, self->sp);
}

void chip_monitor_show(chip_t* chip, chip_monitor_t *self) {
  for (int i = 0; i < self->read_ptr; i++) {
    u16 addr = self->read_addresses[i];
    printf("R %04X: %02X\n", addr, chip->memory[addr]);
  }

  for (int i = 0; i < self->write_ptr; i++) {
    u16 addr = self->write_addresses[i];
    printf("W %04X: %02X\n", addr, chip->memory[addr]);
  }

  self->read_ptr = 0;
  self->write_ptr = 0;
}

int main(int argc, const char *argv[]) {
  // Disable for better reading from other programs
  setvbuf(stdout, NULL, _IOLBF, 0);

  byte *memory = (byte *)malloc(MEMORY_SIZE);
  memset(memory, 0, MEMORY_SIZE);

  chip_t chip = {};
  chip_init(&chip, memory, 0);

  chip_monitor_t monitor = {};

  chip.userdata = &monitor;

  chip_memory_write_callback_set(&chip,
                                 (chip_memory_callback_t)memory_write_callback);
  chip_memory_read_callback_set(&chip,
                                (chip_memory_callback_t)memory_read_callback);

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
    printf("O %02X\n", chip.memory[chip.pc]);
    chip_step(&chip);
    chip_monitor_show(&chip, &monitor);
  }

  chip_show_registers(&chip);

  file = fopen("./dump.bin", "wb");
  fwrite(chip.memory, 1, MEMORY_SIZE, file);

  return 0;
}
