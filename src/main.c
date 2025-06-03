#include "chip.h"
#include "instructions.h"
#include <stdio.h>

int main(void) {
    init_instruction_lookups();

    byte kb[0x10000] = {0xA9, 11, 0x69, 5};
    chip_t chip;
    chip_init(&chip, CHIP_QUOTA_POLICY_FLAT, 0xFFFF, kb);
    
    chip_step(&chip);
    chip_step(&chip);

    chip_dump_registers_stdout(&chip);

    return 0;
}
