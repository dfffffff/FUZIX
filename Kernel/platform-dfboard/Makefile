
CSRCS = devtty.c devices.c main.c fake_ide.c

C3SRCS = libc.c

CDSRCS = discard.c

DSRCS =

ASRCS = crt0.S dfboard.S tricks.S commonmem.S

COBJS = $(CSRCS:.c=$(BINEXT))
C3OBJS = $(C3SRCS:.c=$(BINEXT))
CDOBJS = $(CDSRCS:.c=$(BINEXT))
AOBJS = $(ASRCS:.S=$(BINEXT))
DOBJS = $(patsubst ../dev/%.c,%.o, $(DSRCS))
OBJS  = $(COBJS) $(C3OBJS) $(CDOBJS) $(AOBJS) $(DOBJS)

CROSS_CCOPTS += -I../dev/

all: $(OBJS)

export CROSS_CC_SEGD=-mcode-section=.text3 -mfar-code-page=2

$(COBJS): %$(BINEXT): %.c
	$(CROSS_CC) $(CROSS_CCOPTS) $(CROSS_CC_SEG2) -c $<

$(C3OBJS): %$(BINEXT): %.c
	$(CROSS_CC) $(CROSS_CCOPTS) $(CROSS_CC_SEGD) -c $<

$(CDOBJS): %$(BINEXT): %.c
	$(CROSS_CC) $(CROSS_CCOPTS) $(CROSS_CC_SEGDISC) -c $<

$(DOBJS): %$(BINEXT): ../dev/%.c
	$(CROSS_CC) $(CROSS_CCOPTS) $(CROSS_CC_SEGD) -c $<

$(DDOBJS): %$(BINEXT): ../dev/%.c
	$(CROSS_CC) $(CROSS_CCOPTS) $(CROSS_CC_SEGDISC) -c $<

$(AOBJS): %$(BINEXT): %.S
	$(CROSS_CC) -c $(ASOPTS) $< -o $@

clean:
	rm -f *.o *~ *.s19

image:
	$(CROSS_LD) -o fuzix.s19 -Map=../fuzix.map --script=fuzix.link \
	crt0.o commonmem.o \
	dfboard.o discard.o ../simple.o \
	../start.o ../version.o ../lowlevel-6809.o \
	tricks.o main.o ../timer.o ../kdata.o devices.o fake_ide.o \
	../devio.o ../filesys.o ../process.o ../inode.o ../syscall_fs.o \
	../syscall_proc.o ../syscall_other.o ../mm.o ../swap.o \
	../tty.o ../devsys.o ../usermem.o ../syscall_fs2.o ../syscall_exec16.o \
	devtty.o libc.o ../vt.o ../syscall_fs3.o
	s19tobin <fuzix.s19 >../fuzix.bin
	rm fuzix.s19

.PHONY: all clean image
