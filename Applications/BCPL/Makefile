#
#	IntCode engine
#

FCC = fcc
FCCOPTS = -O2 #--nostdio
PLATFORM =

SRC = icint.c blib.c
OBJ = $(SRC:.c=.rel) blibv.o icintv.o

.SUFFIXES: .c .rel

APPS = icint icintv run-time.i codegen.i b.i

all: $(APPS) sizes

.c.rel:
	$(FCC) $(PLATFORM) $(FCCOPTS) -c $<

icintv.rel: icint.v
	$(FCC) $(PLATFORM) $(FCCOPTS) -c icint.c

blibv.rel: blib.c
	$(FCC) $(PLATFORM) $(FCCOPTS) -c blib.c

icint:  icint.rel blib.rel
	$(FCC) $(PLATFORM) $(FCCOPTS) -o $@ icint.rel  blib.rel

icintv: icintv.rel blibv.rel
	$(FCC) $(PLATFORM) $(FCCOPTS) -o $@ icintv.rel blibv.rel

sizes: $(APPS)
	ls -l $(APPS) > size.report

run-time.i: blib16.i
	cat blib16.i iclib.i > run-time.i

codegen.i:  cg.i run-time.i
	cat cg.i run-time.i > codegen.i

b.i:    syn.i trn.i run-time.i
	cat syn.i trn.i run-time.i > b.i

clean:
	rm -f $(OBJS) $(APPS) $(SRCS:.c=) core *~ *.rel *.asm *.lst *.sym *.map *.noi *.lk *.ihx *.tmp *.bin size.report

rmbak:
	rm -f *~ core
