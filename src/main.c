#include <stdio.h>

#include "chip.h"

int main(void) {
    byte kb[0x10000] = {0xA9, 11, 0x69, 5};
    chip_t chip;

    chip_init(&chip, CHIP_QUOTA_POLICY_FLAT, -1, kb);
    
    chip_dump_registers_stdout(&chip);

    return 0;
}
