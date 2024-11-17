// reduce.s
// Approximate Sin(x) by Chebyshev approximation

.global         _reduce
.p2align        2		// Make sure everything is aligned properly

        .macro  LOADVAL reg, name
        adrp    x0, \name@GOTPAGE
        ldr     x0, [x0, \name@GOTPAGEOFF]
        ldr     \reg, [x0]
        .endm

.text

_reduce:
        mov     w1, wzr         // we're going to use w1 to keep some flags
        fmov    d4, #2          // we're going to mulitply & divide by 2 later
        fcmp    d0, #0.0
        bge     pos             // x < 0, return -Sin(-x)
        mov     w1, 0x0001      // 1 in w1 bit 1 will mean negate the result at the end
        fneg    d0, d0
pos:
        LOADVAL d1, c2Pi        // d1 = 2*Pi
        fcmp    d0, d1
        ble     inrange
        fdiv    d3, d0, d1      // d0 > 2*Pi
        frintz  d3, d3          // floor
        fmsub   d0, d3, d1, d0
inrange:
        and     w1, w1, 0x0001
        cmp     w1, #0
        beq     end
        fneg    d0, d0
end:
        ret

.p2align        2
.data
c2Pi:   .double +6.28318530717958647692529
cPi:    .double +3.14159265358979323846264
cPiD2:  .double +1.57079632679489661923132
