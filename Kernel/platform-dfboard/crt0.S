;;;
;;; The Kernel C run-time / start routine
;;;

	.module	ctr0

	; exported
	.globl	start

	; imported symbols
	.globl	_fuzix_main
	.globl	_system_id
	.globl	init_early
	.globl	init_hardware
	.globl	kstack_top

#include "boot.h"

	; startup code

	.area	.discard
start:
	orcc	#0x10		; interrupts definitely off

	fcb	$1F,$C8		; TFR 0,A
	tsta
	bne	no_6309		; MPU is 6309 ?
	fcb	$11,$3D,$00	; LDMD #0  (6809 emulation mode)
no_6309:
	lds	#kstack_top
	ldx	#s_.bss		; __sectionbase_.bss__
	ldy	#l_.bss		; __sectionlen_.bss__
	clra
bss_wipe:
	sta	,x+
	leay	-1,y
	bne	bss_wipe
	; This might be in BSS so don't save until we've wiped!
	lda	#0
	sta	_system_id	; what sort of a box are we ?

	jsr	init_early
	jsr	init_hardware

	jmp	main


	.area	.text
main:
	jsr	_fuzix_main
	orcc	#0x10
	ldb	#-1
	jmp	F_ERROR
