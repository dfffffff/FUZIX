#ifndef __DEVICE_DOT_H__
#define __DEVICE_DOT_H__

#include "boot.h"

extern void _outchar(uint8_t d); /* dfboard.S */
extern uint8_t _inchar(void); /* dfboard.S */
extern void timercheck(void); /* dfboard.S */

#endif /* __DEVICE_DOT_H__ */
