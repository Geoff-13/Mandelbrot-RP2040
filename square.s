
@   Square used by iterate.s

@   By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
@   Date : Nov 2025



.section  .time_critical, "ax", %progbits     @  Force into SRAM for faster execution
.syntax	  unified
.cpu      cortex-m0plus
.align    4
.thumb_func


.global   sqr_asm


@        ________
@  _____/ Square \_________________________________________________________________________________
@  Input   : R0 = a
@  Output  : R0 = a * a  or  overflow
@  Clobber : R1 thru R3

sqr_asm:
    CMP     R0, #0                  @  Check if a is negative
    BPL     a_pos

    NEGS    R0, R0                  @  R0 = abs(a)

a_pos:
    LSRS    R1, R0, #16             @  R1  = a upper
    UXTH    R2, R0                  @  R2  = a lower

    MOV     R0, R2                  @  R0  = R2     a lower
    MULS    R0, R2                  @  R0 *= R2     a lower * a lower (R0 will be the returned result)
    LSRS    R0, R0, #17             @               discard lower 16 bits + 1 bit for carryover

    MOV     R3, R2                  @  R3  = R2     a lower
    MULS    R3, R1                  @  R3 *= R1     a lower * a upper
    LSRS    R3, R3, #1              @               discard 1 bit for carryover
    ADD     R0, R3                  @  R0 += R3     add to result
    ADD     R0, R3                  @  R0 += R3     add to result
    LSRS    R0, R0, #11             @               discard lower 12 bit - 1 bit for carryover

    MOV     R3, R1                  @  R3  = R1     a upper
    MULS    R3, R1                  @  R3 *= R1     a upper * a upper
    LSRS    R4, R3, #27             @  If non-zero there's an overflow
    BEQ     cont

    LDR     R0, overflow            @  Return an overflow
    B       exit

cont:
    LSLS    R3, R3, #4
    ADD     R0, R3                  @  R0 += R3     add to result

exit:
    BX      LR




.align 4
overflow:   .word   0x7FFFFFFF      @  Special value to indicate an overflow in math subroutines
