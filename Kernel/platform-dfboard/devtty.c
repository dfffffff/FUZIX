#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include <stdbool.h>
#include <vt.h>
#include <tty.h>
#include <graphics.h>
#include "devtty.h"
#include "device.h"

unsigned char tbuf1[TTYSIZ];
unsigned char tbuf2[TTYSIZ];

struct s_queue ttyinq[NUM_DEV_TTY + 1] = {	/* ttyinq[0] is never used */
	{NULL, NULL, NULL, 0, 0, 0},
	{tbuf1, tbuf1, tbuf1, TTYSIZ, 0, TTYSIZ / 2},
	{tbuf2, tbuf2, tbuf2, TTYSIZ, 0, TTYSIZ / 2}
};

/* Output for the system console (kprintf etc) */
void kputchar(char c)
{
	if (c == '\n')
		tty_putc(1, '\r');
	tty_putc(1, c);
}

ttyready_t tty_writeready(uint8_t minor)
{
	uint8_t c;
	if (minor == 1)
		return TTY_READY_NOW;
	else
		panic("tty_writeready: minor != 1");
/*	c = *uart_status;
	return (c & 16) ? TTY_READY_NOW : TTY_READY_SOON;*/ /* TX DATA empty */
}

void tty_putc(uint8_t minor, unsigned char c)
{
	if (minor == 1)
		F_OUTCHAR(c);
	else
		panic("tty_putc: minor != 1");
}

void tty_sleeping(uint8_t minor)
{
	if (minor != 1)
		panic("tty_sleeping: minor != 1");
    used(minor);
}

void tty_setup(uint8_t minor)
{
	if (minor != 1)
		panic("tty_setup: minor != 1");
}

int tty_carrier(uint8_t minor)
{
	if (minor != 1)
		panic("tty_carrier: minor != 1");
	return 1;
}

void platform_interrupt(void)
{
	timercheck();
}
