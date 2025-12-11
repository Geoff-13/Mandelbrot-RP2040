
@   Develop full iteration calc for Mandelbrot in assembly

@   By   : Geoff  (https://forum.clockworkpi.com/c/picocalc/31)
@   Date : Nov 2025



.section  .time_critical, "ax", %progbits     @  Force into SRAM for faster execution
.syntax	  unified
.cpu      cortex-m0plus
.align    4
.thumb_func

.equ  cr,          0                @  32-bits = 4 bytes
.equ  ci,          4
.equ  zr,          8
.equ  zi,         12
.equ  zr_sqr,     16
.equ  zi_sqr,     20
.equ  iter_limit, 24
.equ  stack_size, 28                @  Amount to move stack pointer


.global   iter_asm


@        _______________
@  _____/ Iterate pixel \__________________________________________________________________________
@  Input   : R0 = cr, R1 = ci, R2 = iter_limit
@  Output  : R0 = iteration count
@  Clobber : R3 thru R7 (including subroutines)

iter_asm:
    PUSH    {R3-R7, LR}             @  Save all registers and the return address.
    SUB     SP, #stack_size         @  Reserve xx bytes on the stack
    STR     R0, [SP, #cr]           @  Save cr
    STR     R1, [SP, #ci]           @  Save ci
    STR     R2, [SP, #iter_limit]   @  Save iter_limit

    LDR     R2, one                 @  Fudge to get rid of some incorrect black pixels
    LDR     R3, quarter             @  Gave up trying to chase down the issue
    ADDS    R2, R3
    NEGS    R2, R2                  @  R2 = -1.25
    CMP     R2, R0
    BPL     iterate                 @  If cr < -1.25 skip checks

cardiod_check:
    MOV     R0, R1                  @  R0 = ci
    BL      sqr_asm                 @  R0 = ci * ci
    MOV     R7, R0                  @  R7 = ci_sqr = ci * ci

    LDR     R0, [SP, #cr]           @  R0 = cr
    LDR     R1, quarter             @  R1 = 0.25
    SUBS    R0, R1                  @  R0 = cr - 0.25
    MOV     R5, R0                  @  R5 = cr_mq = cr - 0.25

    BL      sqr_asm                 @  R0 = cr_mq * cr_mq
    ADDS    R0, R7                  @  R0 = q = cr_mq * cr_mq + ci_sqr
    MOV     R4, R0                  @  R4 = q
    ADDS    R0, R5                  @  R0 = q + cr_mq
    MOV     R1, R4                  @  R1 = q
    BL      mul_asm                 @  R0 = q * (q + cr_mq)

    MOV     R1, R7                  @  R1 = ci_sqr
    LSRS    R1, #2                  @  R1 = 0.25 * ci_sqr (>>2 equal to /4.0)
    CMP     R0, R1
    BGT     period_2_check          @  If q > (0.25 * ci_sqr) then pixel outside cardiod

    LDR     R7, [SP, #iter_limit]   @  Inside cardiod 1
    ADDS    R7, #1                  @  Add 1 to differentiate from actual calc reaching iter_limit
    B       exit

period_2_check:
    LDR     R0, [SP, #cr]           @  R0 = cr
    LDR     R1, one                 @  R1 = 1.0
    ADDS    R0, R1                  @  R0 = p = cr + 1.0
    BL      sqr_asm                 @  R0 = p * p
    ADDS    R0, R7                  @  R0 = p * p + ci_sqr

    LDR     R1, sixteenth           @  R1 = 0.0625
    CMP     R0, R1
    BGT     iterate                 @  If p > 0.0625 then pixel is outside period 2 bulb

    LDR     R7, [SP, #iter_limit]   @  Inside cardiod 2
    ADDS    R7, #1                  @  Add 1 to differentiate from actual calc to iter_limit
    B       exit

iterate:
    MOVS    R7, #0                  @  R7 = iteration count
    STR     R7, [SP, #zr]
    STR     R7, [SP, #zi]
    STR     R7, [SP, #zr_sqr]
    STR     R7, [SP, #zi_sqr]

                                    @        ___________
                                    @  _____/ Main Loop \__________________________________________
loop:
    ADDS    R7, #1                  @  Increment the iteration count


                                    @        _______________________
                                    @  _____/ zi = 2 * zr * zi + ci \______________________________
    LDR     R0, [SP, #zr]           @  R0 = zr
    LDR     R1, [SP, #zr]           @  R1 = zr
    ADD     R0, R1                  @  R0 = zr + zr = 2 * zr  (can never overflow)
    LDR     R1, [SP, #zi]           @  R1 = zi
    BL      mul_asm                 @  R0 = 2 * zr * zi
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    LDR     R1, [SP, #ci]           @  R1 = ci
    BL      add_asm                 @  R0 = 2 * zr * zi + ci
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    STR     R0, [SP, #zi]           @  zi = 2 * zr * zi + ci


                                    @        ___________________________
                                    @  _____/ zr = zr_sqr - zi_sqr + cr \__________________________
    LDR     R0, [SP, #zr_sqr]       @  R0 = zr_sqr
    LDR     R1, [SP, #zi_sqr]       @  R1 = zi_sqr
    SUBS    R0, R1                  @  R0 = zr_sqr - zi_sqr  (can never overflow)
    LDR     R1, [SP, #cr]           @  R1 = cr
    BL      add_asm                 @  R0 = zr_sqr - zi_sqr + cr
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    STR     R0, [SP, #zr]           @  zr = zr_sqr - zi_sqr + cr


                                    @        __________________
                                    @  _____/ zr_sqr = zr * zr \___________________________________
    BL      sqr_asm                 @  r0 = zr * zr
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    STR     r0, [SP, #zr_sqr]       @  zr_sqr = zr * zr


                                    @        __________________
                                    @  _____/ zi_sqr = zi * zi \___________________________________
    LDR     R0, [SP, #zi]           @  R0 = zi
    BL      sqr_asm                 @  R0 = zi * zi
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    STR     R0, [SP, #zi_sqr]       @  zi_sqr = zi * zi


                                    @        ___________________________
                                    @  _____/ Check zi_sqr + zr_sqr > 4 \__________________________
    LDR     R1, [SP, zr_sqr]        @  R1 = zr_sqr
    BL      add_asm                 @  R0 = zi_sqr + zr_sqr
    LDR     R2, overflow            @  Check for overflow, and exit if true
    CMP     R0, R2
    BEQ     exit

    LDR     R1, four
    CMP     R0, R1
    BPL     exit                    @  Exit if zi_sqr + zr_sqr > 4.0


                                    @        __________________________
                                    @  _____/ Check iter != iter_limit \___________________________
    LDR     R0, [SP, #iter_limit]   @  R0 = iter_limit
    CMP     R7, R0                  @  R7 = iteration count
    BNE     loop                    @  Loop if iteration count != iter_limit


                                    @        _____________
                                    @  _____/ Return iter \________________________________________
exit:
    MOVS    R0, R7                  @  Return iteration count

    ADD     SP, #stack_size         @  Release the reserved xx bytes on the stack
    POP     {R3-R7, PC}             @  Restore all registers, and return




.align 4
overflow:   .word   0x7FFFFFFF      @  Special value to indicate an overflow in math subroutines
four:       .word   0x40000000      @  Coincidence !!
one:        .word   0x10000000
quarter:    .word   0x04000000
sixteenth:  .word   0x01000000
