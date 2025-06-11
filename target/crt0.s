.export   _init, _exit
.import   _main

.export   __STARTUP__ : absolute = 1
.import   __RAM_START__, __RAM_SIZE__       ; Provided by the linker

.import copydata, zerobss, initlib, donelib

.include "zeropage.inc"

.segment "STARTUP"

_init:  LDX     #$FF
        TXS
        CLD

        LDA     #<(__RAM_START__ + __RAM_SIZE__)
        STA     sp
        LDA     #>(__RAM_START__ + __RAM_SIZE__)
        STA     sp+1


        JSR     zerobss              ; Clear BSS segment
        JSR     copydata             ; Initialize DATA segment
        JSR     initlib              ; Run constructors

        JSR     _main

_exit:  JSR     donelib
        .byte $80
