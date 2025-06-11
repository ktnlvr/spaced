

; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.import   _stop
.export   _irq_int, _nmi_int

.segment  "CODE"

.PC02                             ; Force 65C02 assembly mode

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:  RTI                    ; Return from all NMI interrupts

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

_irq_int:   PHA         ; Save A register
            TXA         ; Transfer X to A
            PHA         ; Save X register
            TSX         ; Save Stack pointer to X
            INX 
            INX
            INX         ; Point to the Status register on the stack
            LDA $0100,X ; Load the Status register from the stack
            AND #$10    ; Isolate the B flag
            BNE break   ; If B = 1 give up


; ---------------------------------------------------------------------------
; IRQ detected, return

irq:       PLA      ; Pull stack X into A
           TAX      ; Restore X register
           PLA      ; Restore A register
           RTI      ; Return from all IRQ interrupts

; ---------------------------------------------------------------------------
; BRK detected, stop

break:     .byte $80        ; If BRK is detected, something very bad
                            ;   has happened, so stop running
