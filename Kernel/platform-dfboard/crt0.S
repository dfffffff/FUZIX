;;;
;;; The Kernel C run-time / start routine
;;;

	.module	ctr0

	; exported
	.globl	start

	; imported symbols
	.globl	_fuzix_main
	.globl	_system_id
	.globl	_mpu_id
	.globl	init_early
	.globl	init_hardware
	.globl	kstack_top

#include "boot.h"

	; startup code

	.area	.discard
start:
	orcc	#$10		; interrupts definitely off

	lds	#kstack_top
	ldx	#__sectionbase_.bss__
	ldy	#__sectionlen_.bss__
	clra
bss_wipe:
	sta	,x+
	leay	-1,y
	bne	bss_wipe

	ldb	#0		; humm well, zero as system id
	ldx	#6809		; default MPU

	fcb	$1F,$C8		; TFR 0,A
	tsta
	bne	no_6309		; MPU is 6309 ?
	fcb	$11,$3D,$00	; LDMD #0  (6809 emulation mode)
	ldx	#6309
no_6309:

	stb	_system_id	; what sort of a box are we ?
	stx	_mpu_id		; what mpu in that box ?

	jsr	init_early
	jsr	init_hardware
	jmp	main


	.area	.text
main:
	jsr	_fuzix_main
	orcc	#$10
	ldb	#-1
	jmp	F_ERROR
