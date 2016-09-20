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
#ifdef CONFIG_SOFT_CURSOR
static unsigned char count, disable;
#endif

struct s_queue ttyinq[NUM_DEV_TTY + 1] = {	/* ttyinq[0] is never used */
	{NULL, NULL, NULL, 0, 0, 0},
	{tbuf1, tbuf1, tbuf1, TTYSIZ, 0, TTYSIZ / 2},
	{tbuf2, tbuf2, tbuf2, TTYSIZ, 0, TTYSIZ / 2}
};

/* Output for the system console (kprintf etc) */
void kputchar(char c)
{
	tty_putc(1, c);
}

ttyready_t tty_writeready(uint8_t minor)
{
	if (minor != 1)
		panic("tty_writeready: minor != 1");
	return TTY_READY_NOW;
}

void tty_putc(uint8_t minor, unsigned char c)
{
	if (minor != 1)
		panic("tty_putc: minor != 1");
#ifdef CONFIG_SOFT_CURSOR
	tty_outchar(c);
#else
	BOOT_OUTCHAR(c);
#endif
}

void tty_sleeping(uint8_t minor)
{
	if (minor != 1)
		panic("tty_sleeping: minor != 1");
	used(minor);
}

void tty_setup(uint8_t minor)
{
	struct termios *termios_p;
	if (minor != 1)
		panic("tty_setup: minor != 1");
	termios_p = &ttydata[minor].termios;
	/* no Map NL to CR-NL on output */
	termios_p->c_oflag &= ~ONLCR;
	/* erase character is the rubout character */
	termios_p->c_cc[VERASE] = 0x7F;
}

int tty_carrier(uint8_t minor)
{
	if (minor != 1)
		panic("tty_carrier: minor != 1");
	return 1;
}

bool tty_caninsert(uint8_t minor)
{
	if (minor != 1)
		panic("tty_caninsert: minor != 1");
	return ttyinq[minor].q_count < ttyinq[minor].q_size;
}

#ifdef CONFIG_SOFT_CURSOR
void tty_cursor(void)
{
	if (!disable) {
		count++;
		if (!(count & ((CONFIG_BLINK_SPEED)-1))) {
			if (count & (CONFIG_BLINK_SPEED))
				BOOT_OUTCHAR(CONFIG_SOFT_CURSOR);
			else
				BOOT_OUTCHAR(' ');
			BOOT_OUTCHAR('\b');
		}
	}
}

void tty_outchar(unsigned char chr)
{
	uint8_t cc = di();
	if (!disable) {
		if (count & (CONFIG_BLINK_SPEED)) {
			count = 0;
			BOOT_OUTCHAR(' ');
			BOOT_OUTCHAR('\b');
		}
		if (chr == '\r')
			disable = 1;
	}
	else {
		if (chr == '\n')
			disable = 0;
	}
	BOOT_OUTCHAR(chr);
	irqrestore(cc);
}
#endif
