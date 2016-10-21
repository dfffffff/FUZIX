/* Enable to make ^Z dump the inode table for debug */
#undef CONFIG_IDUMP
/* Enable to make ^A drop back into the monitor */
#undef CONFIG_MONITOR
/* Profil syscall support (not yet complete) */
#undef CONFIG_PROFIL
/* Multiple processes in memory at once */
#undef CONFIG_MULTI
/* Single tasking - for now while we get it booting */
#undef CONFIG_SINGLETASK

/* The tty is a spi display */
//#define CONFIG_SPI_DISPLAY
/* The tty software cursor character */
//#define CONFIG_SOFT_CURSOR 0x1F
/* Soft cursor blink speed (1-7, 0 always on) */
//#define CONFIG_BLINK_SPEED 3

/* The IDE is on the kernel TTY */
#define CONFIG_IDE_ON_TTY 1

/* Show processor speed */
//#define CONFIG_SHOW_MHZ 1

/* Banks as reported to user space */
#define CONFIG_BANKS 1

/* Pure swap */
#define CONFIG_SWAP_ONLY
#define CONFIG_SPLIT_UDATA
#define CONFIG_USERMEM_DIRECT

#ifdef CONFIG_SPI_DISPLAY
#define PTABSIZE    10         /* Max processes */
#else
#define PTABSIZE    20         /* Max processes */
#endif

/* And swapping */
#define SWAPDEV     1          /* Uses IDE slice 1 */
#define SWAP_SIZE   0x40       /* 32K in 512 byte blocks */
#define SWAPBASE    0x8000     /* We swap the lot, excluding stashed uarea */
#define SWAPTOP     0xFA00     /* so it's a round number of 512 byte sectors */
#define UDATA_SIZE  0x0200     /* one block */
#define UDATA_BLKS  1
#define MAX_SWAPS   PTABSIZE
#define swap_map(x) ((uint8_t *)(x))

/* Permit large I/O requests to bypass cache and go direct to userspace */
#define CONFIG_LARGE_IO_DIRECT

/* A serial tty */
#undef CONFIG_VT
#undef CONFIG_VT_SIMPLE

#define TICKSPERSEC 50         /* Ticks per second */
#define PROGBASE    SWAPBASE   /* also data base */
#define PROGLOAD    SWAPBASE   /* also data base */
#define PROGTOP     SWAPTOP    /* Top of program */

/* We need a tidier way to do this from the loader */
#define CMDLINE NULL           /* Location of root dev name */

/* Device parameters */
#define BOOT_TTY    (512+1) /* Set this to default device for stdio/stderr */
#define NUM_DEV_TTY 2
#define NDEVS       2     /* Devices 0..NDEVS-1 are capable of being mounted */
                          /*  (add new mountable devices to beginning area.) */
#define TTYDEV   BOOT_TTY /* Device used by kernel for messages, panics */
#define NBUFS    6        /* Number of block buffers */
#define NMOUNTS  2        /* Number of mounts at a time */

/* Boot device */
#define BOOTDEVICE 0