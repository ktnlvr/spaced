; ------------------------------
; Puts "Hello, world!" at OFFSET

OFFSET = $C800

.setcpu     "6502"

.forceimport    __STARTUP__
.export     _main

.segment    "CODE"

.proc   _main: near

.segment "RODATA"

hello:
    .asciiz "Hello, world!"

.segment  "CODE"

start:
    ldx #$00
loop:
    lda hello,x
    beq done
    sta OFFSET,x
    inx
    jmp loop
    
done:
    brk
    rts

.endproc
; Puts "Hello, world!" at OFFSET
OFFSET = $C800

.setcpu     "6502"

.forceimport    __STARTUP__
.export     _main

.segment    "CODE"

.proc   _main: near

.segment "RODATA"

hello:
    .asciiz "Hello, world!"

.segment  "CODE"

start:
    ldx #$00
loop:
    lda hello,x
    beq done
    sta OFFSET,x
    inx
    jmp loop
    
done:
    brk
    rts

.endproc
