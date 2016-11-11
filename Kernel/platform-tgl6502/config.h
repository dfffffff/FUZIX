/* Enable to make ^Z dump the inode table for debug */
#undef CONFIG_IDUMP
/* Enable to make ^A drop back into the monitor */
#undef CONFIG_MONITOR
/* Profil syscall support (not yet complete) */
#undef CONFIG_PROFIL
/* Acct syscall support */
#undef CONFIG_ACCT
/* Multiple processes in memory at once */
#define CONFIG_MULTI
/* Single tasking - for now while we get it booting */
#undef CONFIG_SINGLETASK
#define CONFIG_BANKS	2

#define CONFIG_CALL_R2L		/* Runtime stacks arguments backwards */

/*
 *	The banking is really 8K banks but we run with them in pairs
 *	We've got 128K - 8 banks of 16K, of which the kernel eats one for data
 *	(Kernel code is in the ROM banks which are separate)
 */
#define CONFIG_BANK16_LOW
#define MAX_MAPS 7

#if 0
/* And swapping */
#define SWAPDEV 	257	/* FIXME */
#define SWAP_SIZE   	0x70 	/* 56K in blocks */
#define SWAPBASE    	0x0000	/* We swap the lot in one, include the */
#define SWAPTOP	    	0xE000	/* vectors so its a round number of sectors */
/* FIXME: we need to swap the udata separately */
#define MAX_SWAPS	32
/* TODO */
#endif

#define TICKSPERSEC 10	    /* Ticks per second */
#define MAPBASE	    0x0000  /* We map from 0 */
#define PROGBASE    0x2000  /* also data base */
#define PROGLOAD    0x2000
#define PROGTOP     0xC000  /* Top of program (for debug for now, can go to FFF9) */

#define BOOT_TTY 513        /* Set this to default device for stdio, stderr */

/* We need a tidier way to do this from the loader */
#define CMDLINE	NULL	  /* Location of root dev name */

/* Device parameters */
#define NUM_DEV_TTY 1
#define TTYDEV   BOOT_TTY /* Device used by kernel for messages, panics */
#define NBUFS    8        /* Number of block buffers */
#define NMOUNTS	 2	  /* Number of mounts at a time */

#define platform_discard()
