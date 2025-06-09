.setcpu     "6502"

.forceimport    __STARTUP__
.export     _main

; Puts "Hello, world!" at $0200

.segment	"CODE"

.proc   _main: near

.segment "RODATA"

hello:
    .asciiz "Hello, world!"

.segment  "CODE"

loop:
    ldx #$00
    lda hello,x
    beq done
    sta $0800,x
    inx
    jmp loop
    
done:
    brk
    rts

.endproc
