#include <kernel.h>
#include <timer.h>
#include <kdata.h>
#include <printf.h>
#include <blkdev.h>
#include "device.h"
#include "devtty.h"

static const char *sysname[] = {"DFBOARD", "Unknown"};
uint8_t system_id;

void map_init(void)
{
	uint8_t i;

	kprintf("%s system.\n", sysname[system_id]);

	/* We put swap on the start of slice 1 */
	for (i = 0; i < MAX_SWAPS; i++)
		swapmap_add(i);
}
