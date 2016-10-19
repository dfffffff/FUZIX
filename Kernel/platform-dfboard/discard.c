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
uint16_t mpu_id = -1;

void map_init(void)
{
	uint8_t i;

	if (system_id > SYSNAME_MAX)
		system_id = SYSNAME_MAX;

#ifdef CONFIG_SHOW_MHZ
	char buffer[16];
	extern char *compute_mhz(char *buffer);
	kprintf("%s system running on %u MPU at ", sysname[system_id], mpu_id, buffer);
	kprintf("%s MHz.\n", compute_mhz(buffer));
#else
	kprintf("%s system running on %u MPU.\n", sysname[system_id], mpu_id);
#endif

	/* We put swap on the start of slice 1 */
	for (i = 0; i < MAX_SWAPS; i++)
		swapmap_add(i);
}
