/*
 *  dfboard boot loader definitions
 */
#ifndef _BOOT_H_
#define _BOOT_H_
#if __ASSEMBLER__
#ifdef INCLUDE_LIB_BOOT
	lib	<boot/boot.i09>
_BOOT_ERROR	.gblequ	BOOT.F_ERROR
_BOOT_OUTCHAR	.gblequ	BOOT.TF_OUTCHAR
_BOOT_INCHAR	.gblequ	BOOT.TF_INCHAR
#endif
	; exported
	.globl	BOOT.PTM_CR2
	.globl	BOOT.PTM_CR13
	.globl	BOOT.PTM_MBR3
	.globl	BOOT.PTM_STA
	.globl	BOOT.PTM_TC3
	.globl	BOOT.ACIA_SR
	.globl	BOOT.ACIA_SR_FE
	.globl	BOOT.ACIA_SR_OVRN
	.globl	BOOT.ACIA_SR_PE
	.globl	BOOT.ACIA_SR_RDRF
	.globl	BOOT.ACIA_SR_TDRE
	.globl	BOOT.ACIA_RDR
	.globl	BOOT.ACIA_TDR
	.globl	BOOT.PORTA
	.globl	BOOT.B_FAST
	.globl	BOOT.F_ERROR
	.globl	BOOT.TF_OUTCHAR
	.globl	BOOT.TF_INCHAR
	.globl	BOOT.TF_OUTCHARNB
	.globl	BOOT.TF_INCHARNB
#else
extern void BOOT_ERROR(unsigned char e);
extern void (*BOOT_OUTCHAR)(unsigned char c);
extern unsigned char (*BOOT_INCHAR)(void);
#endif
#endif
