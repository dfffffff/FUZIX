#include <kernel.h>
#include <timer.h>
#include <kdata.h>
#include <printf.h>
#include <blkdev.h>
#include "device.h"
#include "devtty.h"

static const char *sysname[] = {"DF's Board", "Unknown"};
#define SYSNAME_MAX (uint16_t)(sizeof(sysname)/sizeof(char*)-1)
uint8_t system_id = -1;
uint16_t mpu_id = 6809;

void map_init(void)
{
	uint8_t i;

	if (system_id > SYSNAME_MAX)
		system_id = SYSNAME_MAX;

	kprintf("%s system running on %u MPU.\n", sysname[system_id], mpu_id);

	/* We put swap on the start of slice 1 */
	for (i = 0; i < MAX_SWAPS; i++)
		swapmap_add(i);
}
