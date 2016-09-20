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
	.globl	outchar
	.globl	_program_vectors
	.globl	_need_resched
	.globl	_ramsize
	.globl	_procmem
	.globl	_trap_monitor
	.globl	_trap_reboot
	.globl	_di
	.globl	_ei
	.globl	_irqrestore
	.globl	_timercheck
	.globl	_keycheck
	.globl	__outchar
	.globl	__inchar

	; imported
	.globl	unix_syscall_entry
	.globl	null_handler
	.globl	nmi_handler
	.globl	_timer_interrupt
	.globl	_tty_inproc
	.globl	_tty_caninsert
#if defined(CONFIG_SOFT_CURSOR) && CONFIG_BLINK_SPEED > 0
	.globl	_tty_cursor
#endif

#define INCLUDE_LIB_BOOT
#include "boot.h"
#include "config.h"
#include "kernel.def"
#include "../kernel09.def"
#ifdef CONFIG_SPI_DISPLAY
	.area	.text
	lib	<io2/spi.i09>
	lib	<lcd/SSD1306_devs.i09>
	lib	<lcd/SSD1306_pic_4x8.i09>
	.area	.bss
	lib	<lcd/SSD1306_rmb.i09>
#endif

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
#ifdef CONFIG_SPI_DISPLAY
	ldd	#(SSD1306.F_FAST|SSD1306.F_HW)*$100|1
	jsr	SSD1306.INIT
	ldx	#SSD1306.DISPCHR
	stx	BOOT.TF_OUTCHAR
#endif
	ldx	#null_handler
	stx	1
	lda	#$7E
	sta	0
	rts

init_hardware:
	ldd	#64
	std	_ramsize
	ldd	#(U_DATA-PROGBASE)/$400 ;32
	std	_procmem
	; Init PTM for timer interrupt
	lda	#%00000011	; PTM interrupt disabled
	sta	BOOT.PTM_CR2
	clra
	sta	BOOT.PTM_CR13	; PTM set CR1 to 0
	sta	BOOT.PTM_CR2
	lda	#%01000011	; PTM timer 3,
				; interrupt enabled, /8 prescaler,
				; 16-bit continuous mode.
	sta	BOOT.PTM_CR13
	;	######		; adjust for system speed, 68B09 run at 2.5MHz, 63C09 run at 5MHz
	lda	BOOT.PORTA
	anda	#BOOT.B_FAST
	sta	,-s
	ldd	#5000000/8/TICKSPERSEC
	tst	,s+
	bne	fast
	lsra
	rorb
	;	######
fast:	std	BOOT.PTM_MBR3
	rts


;
; COMMON MEMORY PROCEDURES FOLLOW
;

	.area	.common


_trap_reboot:
	orcc	#$10
	ldb	#-2
	jmp	BOOT.F_ERROR	; throw an error for now, TODO

_trap_monitor:
	orcc	#$10
	ldb	#-3
	jmp	BOOT.F_ERROR	; throw an error for now, TODO


_di:
	tfr	cc,b		; return the old irq state
	orcc	#$10
	rts

_ei:
	andcc	#$EF
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


_timercheck:
	ldb	BOOT.PTM_STA
	bpl	tcend
	ldb	BOOT.PTM_TC3
	jsr	_timer_interrupt
#if defined(CONFIG_SOFT_CURSOR) && CONFIG_BLINK_SPEED > 0
	jsr	_tty_cursor
#endif
tcend:	rts

;extern bool tty_caninsert(uint8_t minor);
;extern int tty_inproc(uint8_t minor, unsigned char c);
_keycheck:
kckey:	ldb	#1
	jsr	_tty_caninsert
	tstb
	beq	kcend
	jsr	BOOT.F_INCHARNB
	bcc	kcend
	pshs	b
	ldb	#1
	jsr	_tty_inproc
	leas	1,s
	bra	kckey
kcend:	rts


;
;	FIXME:
;
firq_handler:
	ldb	#-4
	jmp	BOOT.F_ERROR

;
;	FIXME:
;
badswi_handler:
	ldb	#-5
	jmp	BOOT.F_ERROR

;
;	debug output port, output to main tty for now
;
outchar:
;	ldb	#-7
;	jmp	BOOT.F_ERROR
	pshs	b,a
	ldb	,s
	jsr	BOOT.F_OUTCHAR
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
	bitb	#BOOT.ACIA_SR_RDRF
	beq	__inchar
	ldb	BOOT.ACIA_RDR
	rts

aciaerr:
	ldb	#-6
	jmp	BOOT.F_ERROR


	.area	.common

_need_resched:
	.db	0

; make sure the kernel space finish before user space
	.area	.end_of_kernel
kernend	.blkb	(PROGBASE)-kernend

; make sure the user space finish before the bootloader
	.area	.end_of_user
userend	.blkb	$FC00-userend

