#ifndef __DEVTTY_DOT_H__
#define __DEVTTY_DOT_H__

bool tty_caninsert(uint8_t minor);
#ifdef CONFIG_SOFT_CURSOR
#if CONFIG_BLINK_SPEED > 0
void tty_cursor(void);
#endif
void tty_outchar(unsigned char chr);
#endif

#endif
