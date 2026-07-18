/* newlib-ABI setjmp/longjmp.
 *
 * Code compiled against newlib's <setjmp.h> stack-allocates a jmp_buf sized to
 * newlib's ARM _JBLEN = 23 (92 bytes). Sony's SceLibc setjmp/longjmp use a
 * different jmp_buf layout, so binding those to a newlib-sized buffer overruns
 * it and smashes the saved return address. SceLibcPosix therefore exports
 * newlib's own setjmp/longjmp so every newlib-compiled caller (and pthread's
 * cancellation path) gets a matching pair, and SceLibcExt no longer exports the
 * Sony versions. Transcribed from newlib's lib_a-setjmp.o: saves r4-r11, sp, lr.
 */

	.syntax unified
	.thumb
	.text

	.global setjmp
	.thumb_func
	.type setjmp, %function
setjmp:
	mov	ip, sp
	stmia.w	r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	mov.w	r0, #0
	bx	lr
	.size setjmp, .-setjmp

	.global longjmp
	.thumb_func
	.type longjmp, %function
longjmp:
	ldmia.w	r0!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	mov	sp, ip
	movs	r0, r1
	it	eq
	moveq	r0, #1
	bx	lr
	.size longjmp, .-longjmp
