/*
 *  dfboard boot loader definitions
 */
#ifndef _BOOT_H_
#define _BOOT_H_
#if __ASSEMBLER__
#ifdef INCLUDE_LIB_BOOT
	lib	<boot/boot.i09>
_F_ERROR	.gblequ	BOOT.F_ERROR
_F_OUTCHAR	.gblequ	BOOT.F_OUTCHAR
_F_INCHAR	.gblequ	BOOT.F_INCHAR
#endif
	; exported
	.globl	PTM.CR2
	.globl	PTM.CR13
	.globl	PTM.MBR3
	.globl	PTM.STA
	.globl	PTM.TC3
	.globl	BOOT.PORTA
	.globl	BOOT.B_FAST
	.globl	F_ERROR
	.globl	F_OUTCHAR
	.globl	F_INCHAR
	.globl	F_OUTCHARNB
	.globl	F_INCHARNB
#else
extern void F_OUTCHAR(unsigned char c);
extern unsigned char F_INCHAR(void);
extern void F_ERROR(unsigned char e);
#endif
#endif
