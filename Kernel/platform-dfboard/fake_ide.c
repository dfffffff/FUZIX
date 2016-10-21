#include <kernel.h>
#include <kdata.h>
#include <printf.h>
#include "device.h"
#include "fake_ide.h"

#define MAX_BLOCK ((SWAPTOP-SWAPBASE)/0x200)

#ifdef CONFIG_IDE_ON_TTY
static uint8_t cc;
static void begin_io(void)
{
	cc = di();
	_outchar(0);
	while (_inchar());
}
static void end_io(void)
{
	_outchar(0);
	irqrestore(cc);
}
#endif

int ide_open(uint8_t minor, uint16_t flag)
{
	char c1, c2, c3;
	int i;
	if (minor > 1) {
		udata.u_error = ENODEV;
		return -1;
	}
#ifdef CONFIG_IDE_ON_TTY
	begin_io();
#endif
	_outchar('Z');
	i = -3;
	c1 = c2 = 1;
	do {
		c3 = c2;
		c2 = c1;
		c1 = _inchar();
		i++;
	} while (c1 != '\377' || c2 != '\0' || c3 != 'U');
#ifdef CONFIG_IDE_ON_TTY
	end_io();
#endif
	return 0;
}

static int ide_transfer(uint8_t minor, bool is_read, uint8_t rawflag)
{
	uint8_t cylh, cyll, sec, *dptr;
	uint16_t i, block, nblock;

	if (rawflag == 1 && d_blkoff(BLKSHIFT))
		return -1;

	if (!udata.u_nblock || udata.u_nblock>MAX_BLOCK)
		panic("ide_transfer: !udata.u_nblock || udata.u_nblock>MAX_BLOCK");

	dptr = udata.u_dptr;
	block = udata.u_block;
	nblock = udata.u_nblock;

	cylh = minor & 0x7F;	/* Slice number */
#ifdef CONFIG_IDE_ON_TTY
	begin_io();
#endif
	do {
		cyll = block >> 8;	/* Each slice is 32MB (256*256*512) */
		sec = block & 0xFF;
		if (is_read) {
			_outchar('R');
			_outchar(cylh);
			_outchar(cyll);
			_outchar(sec);
			for (i=0; i<512; i++)
				*dptr++ = _inchar();
		}
		else {
			_outchar('W');
			_outchar(cylh);
			_outchar(cyll);
			_outchar(sec);
			for (i=0; i<512; i++)
				_outchar(*dptr++);
		}
		block++;
	} while (--nblock);
#ifdef CONFIG_IDE_ON_TTY
	end_io();
#endif

	return 0;
}

int ide_read(uint8_t minor, uint8_t rawflag, uint8_t flag)
{
	return ide_transfer(minor, 1, rawflag);
}

int ide_write(uint8_t minor, uint8_t rawflag, uint8_t flag)
{
	return ide_transfer(minor, 0, rawflag);
}

int ide_ioctl(uint8_t minor, uarg_t request, char *unused)
{
   if (request != BLKFLSBUF)
        return -1;
    return 0;
}
