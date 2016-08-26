;;;
;;; The dfboard platform
;;;

	.module	dfboard

	; exported
	.globl	map_kernel
	.globl	map_process
	.globl	map_process_a
	.globl	map_process_always
	.globl	map_save
	.globl	map_restore
	.globl	init_early
	.globl	init_hardware
	.globl	_program_vectors
	.globl	_need_resched

	.globl	_ramsize
	.globl	_procmem

	; imported
	.globl	unix_syscall_entry
	.globl	null_handler
	.globl	_vtoutput
	.globl	_timer_interrupt
	.globl	nmi_handler

	; exported debugging tools
	.globl	_trap_monitor
	.globl	_trap_reboot
	.globl	outchar
	.globl	_di
	.globl	_ei
	.globl	_irqrestore
	.globl	_timercheck
	.globl	__outchar
	.globl	__inchar

#define INCLUDE_LIB_BOOT
#include "boot.h"
#include "kernel.def"
#include "../kernel09.def"

	.area	.vectors
	.blkb	2			; $FFF0 RESERVED  lets the bootloader handle this for now
	.dw	badswi_handler		; $FFF2 SWI3
	.dw	badswi_handler		; $FFF4 SWI2
	.dw	firq_handler		; $FFF6 FIRQ
	.dw	interrupt_handler	; $FFF8 IRQ
	.dw	unix_syscall_entry	; $FFFA SWI
	.dw	nmi_handler		; $FFFC NMI
	.dw	badswi_handler		; $FFFE RESET


	.area	.discard

init_early:
	ldx	#null_handler
	stx	1
	lda	#0x7E
	sta	0
	rts

init_hardware:
	ldd	#64
	std	_ramsize
	ldd	#(U_DATA-PROGBASE)/$400 ;32
	std	_procmem
	; Init PTM for 50Hz interrupt
	lda	#%00000011	; PTM interrupt disabled
	sta	PTM.CR2
	clra
	sta	PTM.CR13	; PTM set CR1 to 0
	sta	PTM.CR2
	lda	#%01000011	; PTM timer 3,
				; interrupt enabled, /8 prescaler,
				; 16-bit continuous mode.
	sta	PTM.CR13
	;	######		; adjust for system speed, 68B09 run at 2.5MHz, 63C09 run at 5MHz
	lda	BOOT.PORTA
	anda	#BOOT.B_FAST
	sta	,-s
	ldd	#12500		; = 5000000/8/50
	tst	,s+
	bne	fast
	lsra
	rorb
	;	######
fast:	std	PTM.MBR3
	rts


;
; COMMON MEMORY PROCEDURES FOLLOW
;

	.area	.common


_trap_reboot:
	orcc	#0x10
	ldb	#-2
	jmp	F_ERROR		; throw an error for now, TODO

_trap_monitor:
	orcc	#0x10
	ldb	#-3
	jmp	F_ERROR		; throw an error for now, TODO


_di:
	tfr	cc,b		; return the old irq state
	orcc	#0x10
	rts

_ei:
	andcc	#0xef
	rts

_irqrestore:			; B holds the data
	tfr	b,cc
	rts

;
;	Our vectors live in a fixed block and are not banked out. Likewise
;	No memory management for now
;
_program_vectors:
map_kernel:
map_restore:
map_process:
map_process_always:
map_process_a:
map_save:
	rts


;extern int tty_inproc(uint8_t minor, unsigned char c);
_timercheck
	tst	PTM.STA
	bpl	tcend
	tst	PTM.TC3
	jsr	_timer_interrupt
	jsr	F_INCHARNB
	bcc	tcend
	pshs	b
	ldb	#1
	jsr	_tty_inproc
	puls	b,pc
tcend:	rts


;
;	FIXME:
;
firq_handler:
	ldb	#-4
	jmp	F_ERROR

;
;	FIXME:
;
badswi_handler:
	ldb	#-5
	jmp	F_ERROR

;
;	debug output port, output to main tty for now
;
outchar:
;	ldb	#-7
;	jmp	F_ERROR
	pshs	b,a
	ldb	,s
	jsr	F_OUTCHAR
	puls	a,b,pc


;
;	__outchar and __inchar are used for external storage access
;
__outchar:
	lda	BOOT.ACIA_SR
	bita	#BOOT.ACIA_SR_FE|BOOT.ACIA_SR_OVRN|BOOT.ACIA_SR_PE
	bne	aciaerr
	bita	#BOOT.ACIA_SR_TDRE
	beq	__outchar
	stb	BOOT.ACIA_TDR
	rts

__inchar:
	ldb	BOOT.ACIA_SR
	bitb	#BOOT.ACIA_SR_FE|BOOT.ACIA_SR_OVRN|BOOT.ACIA_SR_PE
	bne	aciaerr
	lsrb
	bcc	__inchar
	ldb	BOOT.ACIA_RDR
	rts

aciaerr:
	ldb	#-6
	jmp	F_ERROR


	.area	.common

_need_resched:
	.db	0

; make sure the kernel finish before user space
	.area	.endofkernel
kernend	.blkb	PROGBASE-kernend

