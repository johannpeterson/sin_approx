// sin2.s
// Approximate Sin(x) by Chebyshev approximation

.global         _sin_3
.p2align        2		// Make sure everything is aligned properly

        .macro  LOADVAL reg, name
        adrp    x0, \name@GOTPAGE
        ldr     x0, [x0, \name@GOTPAGEOFF]
        ldr     \reg, [x0]
        .endm

.text

_sin_3:
        mov     w1, wzr         // we're going to use w1 to keep some flags
        fmov    d4, #2          // we're going to divide by 2 later

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
        fcvtms  x2, d3          // convert to integer ("floor")
        fmov    d2, x2
        fmsub   d0, d0, d1, d2

inrange:                        // d0 < 2*Pi
        fdiv    d1, d1, d4      // d1 = Pi
        fcmp    d0, d1
        ble     first_half      // 0 <= x <= Pi
        fsub    d0, d0, d1      // d0 = d0 - Pi
        eor     w1, w1, 0x0001  // toggle 1st bit of w1, to negate the final answer

first_half:
        fdiv    d1, d1, d4      // d1 = Pi/2
        fcmp    d0, d1
        ble     approx          // 0 <= d0 <= Pi/2 - calculate the approximation
        // Pi/2 < d0 <= Pi - return Sin(Pi-x)
        fmul    d1, d1, d4      // d1 = Pi
        fsub    d0, d1, d0      // d0 = Pi - d0

approx:
        LOADVAL d1, a0
        LOADVAL d2, a1
        LOADVAL d3, a2
        LOADVAL d4, a3
        LOADVAL d5, a4
        LOADVAL d6, a5
        LOADVAL d7, a6
        LOADVAL d8, a7
        LOADVAL d9, a8
        LOADVAL d10, a9
        LOADVAL d11, a10
        LOADVAL d12, a11
        LOADVAL d13, a12
        LOADVAL d14, a13

        fmadd   d13, d14, d0, d13
        fmadd   d12, d13, d0, d12
        fmadd   d11, d12, d0, d11
        fmadd   d10, d11, d0, d10
        fmadd   d9, d10, d0, d9
        fmadd   d8, d9, d0, d8
        fmadd   d7, d8, d0, d7
        fmadd   d6, d7, d0, d6
        fmadd   d5, d6, d0, d5
        fmadd   d4, d5, d0, d4
        fmadd   d3, d4, d0, d3
        fmadd   d2, d3, d0, d2
        fmadd   d1, d2, d0, d1

        fmov    d0, d1
        and     w1, w1, 0x0001
        cmp     w1, #0
        beq     end
        fneg    d0, d0

end:
        ret

.p2align        2
.data
a0:	.double	+3.15159609307366933583264e-17
a1:	.double	+9.99999999999992137463981e-1
a2:	.double	+3.24848403977218879514298e-13
a3:	.double	-1.66666666671945646405887e-1
a4:	.double	+4.46929940061919965152147e-11
a5:	.double	+8.33333310712103651500584e-3
a6:	.double	+7.39903364746182917886826e-10
a7:	.double	-1.98414335571346275936300e-4
a8:	.double	+2.51241790401253696723421e-9
a9:	.double	+2.75303544969185019022074e-6
a10:	.double	+2.00650971121911487700779e-9
a11:	.double	-2.60546344930653900663444e-8
a12:	.double	+3.11243537080303902068867e-10
a13:	.double	+1.12392760716968552199773e-10

c2Pi:   .double +6.28318530717958647692529
cPi:    .double +3.14159265358979323846264
cPiD2:  .double +1.57079632679489661923132
