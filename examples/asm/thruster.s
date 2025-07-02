.setcpu     "6502"

.forceimport    __STARTUP__
.export         _main

.segment    "CODE"

.proc   _main: near

ldx #16
lda #$80
pha
ldy #$0
.byte $89
.byte $80

.endproc
