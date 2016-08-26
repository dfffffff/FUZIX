#include <kernel.h>
#include <timer.h>
#include <kdata.h>
#include <printf.h>
#include <blkdev.h>
#include "device.h"
#include "devtty.h"

void platform_idle(void)
{
}

void platform_discard(void)
{
}

#ifndef BOOTDEVICE
uint8_t platform_param(unsigned char *p)
{
    return 0;
}
#endif
