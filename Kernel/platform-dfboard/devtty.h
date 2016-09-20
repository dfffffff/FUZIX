#ifndef __DEVTTY_DOT_H__
#define __DEVTTY_DOT_H__

bool tty_caninsert(uint8_t minor);
#ifdef CONFIG_SOFT_CURSOR
void tty_cursor(void);
void tty_outchar(unsigned char chr);
#endif

#endif
