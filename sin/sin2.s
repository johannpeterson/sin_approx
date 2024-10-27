// sin2.s
// Approximate Sin(x) by Chebyshev approximation

.global         _sin_2
.p2align        2			// Make sure everything is aligned properly

        .macro  LOADVAL reg, name
        adrp    x0, \name@GOTPAGE
        ldr     x0, [x0, \name@GOTPAGEOFF]
        ldr     \reg, [x0]
        .endm

.text

_sin_2:
        adrp    x0, a3@GOTPAGE
        ldr     x0, [x0, a3@GOTPAGEOFF]
        ldr     d1, [x0]
        LOADVAL d2, a3
        fmov    d0, d1
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
