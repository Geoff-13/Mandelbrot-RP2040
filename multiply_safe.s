
@   Multiply used by C routines (saves and restores registers used)

@   By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
@   Date : Nov 2025



.section  .text                     @  Can be in flash since only runs outside the iterate loop
.syntax	  unified
.cpu      cortex-m0plus
.align    4
.thumb_func


.global   mul_asm_safe


@        _________________
@  _____/ Multiply (safe) \________________________________________________________________________
@  Input   : R0 = a, R1 = b
@  Output  : R0 = a * b  or  overflow
@  Clobber : N/A (_safe saves and restores all reigisters)

mul_asm_safe:
    PUSH    {R2-R6, LR}             @  Save the registers

    LSRS    R2, R0, #31             @  R2 = sign of a
    CMP     R2, #0
    BEQ     a_pos

    NEGS    R0, R0                  @  R0 = abs(a)
a_pos:
    LSRS    R6, R1, #31             @  R6 = sign of b
    CMP     R6, #0
    BEQ     b_pos

    NEGS    R1, R1                  @  R1 = abs(b)
b_pos:
    EORS    R6, R6, R2              @  R6 is the sign for the result

    LSRS    R3, R1, #16             @  R3  = b upper
    UXTH    R4, R1                  @  R4  = b lower

    LSRS    R1, R0, #16             @  R1  = a upper (OK to overwrite incoming b (R1) as it's now split into R3 and R4)
    UXTH    R2, R0                  @  R2  = a lower

    MOV     R0, R4                  @  R0  = R4
    MULS    R0, R2                  @  R0 *= R2     b lower * a lower (R0 will be the returned result)
    LSRS    R0, R0, #17             @               discard lower 16 bits + 1 bit for carryover

    MOV     R5, R4                  @  R5  = R4
    MULS    R5, R1                  @  R5 *= R1     b lower * a upper
    LSRS    R5, R5, #1              @               discard 1 bit for carryover
    ADD     R0, R5                  @  R0 += R5     add to result

    MOV     R5, R3                  @  R5  = R3
    MULS    R5, R2                  @  R5 *= R2     b upper * a lower
    LSRS    R5, R5, #1              @               discard 1 bit for carryover
    ADD     R0, R5                  @  R0 += R5     add to result
    LSRS    R0, R0, #11             @               discard lower 12 bit - 1 bit for carryover

    MOV     R5, R3                  @  R5  = R3
    MULS    R5, R1                  @  R5 *= R1     b upper * a upper
    LSRS    R4, R5, #27             @  If non-zero there's an overflow (R4 can be used as scratch space)
    BEQ     cont

    LDR     R0, overflow            @  Return an overflow
    B       exit

cont:
    LSLS    R5, R5, #4
    ADD     R0, R5                  @  R0 += R5     final result

    CMP     R6, #0                  @  Check if result should be negative
    BEQ     exit

    NEGS    R0, R0                  @  R0 = -result
exit:
    POP     {R2-R6, PC}             @  Restore registers, and return




.align 4
overflow:   .word   0x7FFFFFFF      @  Special value to indicate an overflow in math subroutines

