
@   Add and subtract used by iterate.s

@   By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
@   Date : Nov 2025



.section  .time_critical, "ax", %progbits     @  Force into SRAM for faster execution
.syntax	  unified
.cpu      cortex-m0plus
.align    4
.thumb_func


.global   sub_asm
.global   add_asm


@        __________
@  _____/ Subtract \_______________________________________________________________________________
@  Input   : R0 = a, R1 = b
@  Output  : R0 = a, R1 = -b
@  Clobber : R2 thru R3

sub_asm:
    NEGS    R1, R1                  @  R1 = -b
                                    @  Continue into add_asm to do  a + (-b)


@        _____
@  _____/ Add \____________________________________________________________________________________
@  Input   : R0 = a, R1 = b
@  Output  : R0 = a + b  or  a - b from subtract  or  overflow
@  Clobber : R2 thru R3

add_asm:
    LSRS    R2, R0, #31             @  R2 = sign of a
    LSRS    R3, R1, #31             @  R3 = sign of b
    CMP     R2, R3                  @  same sign (possible overflow),  diff sign (no overflow)
    BEQ     same_sign

diff_sign:
    ADDS    R0, R1                  @  R0 = a + b
    BX      LR                      @  Return result

same_sign:
    CMP     R2, #0                  @  If R2 positive then a & b are positive
    BEQ     positives

negatives:
    ADDS    R0, R1
    BVS     report_error
    LSRS    R2, R0, #31             @  R2 = sign of result
    CMP     R2, #0                  @  If sign is positive, there's been overflow
    BEQ     report_error
    BX      LR                      @  Return result

positives:
    ADDS    R0, R1
    BVS     report_error
    BPL     exit                    @  If result is positive, no overflow

report_error:
    LDR     R0, overflow            @  Return an overflow
exit:
    BX      LR




.align 4
overflow:   .word   0x7FFFFFFF      @  Special value to indicate an overflow in math subroutines
