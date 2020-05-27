CC = gcc -pipe -Wno-all -Iinc 
#CFLAGS = -g -O2 -v
CFLAGS = -g -O2

USERDRIVER = clk.o # rk11.o
STDDRIVER = reg.o cons.o ltc.o
DRIVERO = $(STDDRIVER) $(USERDRIVER)

DISASO = disas11.o heapsort.o odt.o
ASO = as11.o asm-parse.o htable.o copyof.o heapsort.o
#ODT = odt.o
PDPO = pdp11.o heapsort.o instrs.o finstrs.o instr-aux.o drivers.o $(DRIVERO) $(ODT)

SHARSRC =\
	README\
	*.doc\
	KDJ11-A KDJ11-A.instrs.byname KDJ11-A.instrs.byvalue\
	Makefile\
	*.fsm\
	*.c\
	*.h\
	*.s11\
	inc

.SUFFIXES: .s11 .l11 .o11

all: as11 pdp11 test.o11

ALWAYS:

trydist:
	rm -f *.o
	( unset LD_LIBRARY_PATH ; make $(MFLAGS) CC=/bin/cc CFLAGS=-Iinc )

pdp11.static: $(PDPO)
	$(CC) $(CFLAGS) -static -o $@ $(PDPO) $(TRACE)

pdp11: $(PDPO)
	$(CC) $(CFLAGS) -o $@ $(PDPO) $(TRACE)

as11.static: $(ASO)
	$(CC) $(CFLAGS) -static -o $@ $(ASO) $(TRACE)

as11: $(ASO)
	$(CC) $(CFLAGS) -o $@ $(ASO) $(TRACE)

disas11: $(DISASO)
	$(CC) $(CFLAGS) -o disas11 $(DISASO) $(TRACE)

asm-parse.c: asm-parse.fsm
	fsm < asm-parse.fsm > asm-parse.c

pdp11.shar: ALWAYS
	shar -o pdp11.shar -r $(SHARSRC)

aux.shar: ALWAYS
	shar -o aux.shar -C aux -r .

pdp11.shar.Z: pdp11.shar
	compress < pdp11.shar > pdp11.shar.Z

pdp11.shar.Z.btoa: pdp11.shar.Z
	btoa < pdp11.shar.Z > pdp11.shar.Z.btoa

fcvt-exptab: fcvt-exptab-gen
	fcvt-exptab-gen > fcvt-exptab

fcvt-exptab-gen: fcvt-exptab-gen.o
	$(CC) -o fcvt-exptab-gen fcvt-exptab-gen.o $(TRACE)

fcvt-mantbits: fcvt-mantbits-gen
	fcvt-mantbits-gen > fcvt-mantbits

fcvt-mantbits-gen: fcvt-mantbits-gen.o
	$(CC) -o fcvt-mantbits-gen fcvt-mantbits-gen.o $(TRACE)

# -M output from some ccs includes a leading ./ on local includes....

./fcvt-exptab: fcvt-exptab

./fcvt-mantbits: fcvt-mantbits

# grrr, some (most? all?) makes don't listen to the dependency on as11 on these next two lines....
# not much to do about it!

.s11.o11: as11
	as11 -o $*.o11 -l $*.l11 $*.s11

.s11.l11: as11
	as11 -o $*.o11 -l $*.l11 $*.s11

depend:
	( echo '/^# DO NOT DELETE/+2,$$c' ; \
	  $(CC) -MM $(CFLAGS) *.c | sort | uniq ; \
	  echo . ; \
	  echo w ; \
	  echo q ) | ed - Makefile

# DO NOT DELETE THIS LINE

asm-parse.o: asm-parse.c instrtbl.h pdp11types.h
clk.o: clk.c driver.h pdp11.h pdp11types.h
cons.o: cons.c driver.h pdp11.h pdp11types.h
copyof.o: copyof.c
drivers.o: drivers.c driver.h pdp11.h pdp11types.h
fcvt-exptab-gen.o: fcvt-exptab-gen.c
fcvt-mantbits-gen.o: fcvt-mantbits-gen.c
finstrs.o: finstrs.c pdp11.h pdp11types.h
heapsort.o: heapsort.c
htable.o: htable.c
instr-aux.o: instr-aux.c pdp11.h pdp11types.h driverint.h driver.h
instrs.o: instrs.c pdp11.h pdp11types.h driver.h instrtbl.h
lp11.o: lp11.c pdp11.h pdp11types.h driver.h
ltc.o: ltc.c driver.h pdp11.h pdp11types.h
odt.o: odt.c pdp11.h pdp11types.h driverint.h driver.h instrtbl.h
pdp11.o: pdp11.c pdp11.h pdp11types.h driverint.h driver.h instrs.h
reg.o: reg.c pdp11.h pdp11types.h driver.h
rk11.o: rk11.c pdp11.h pdp11types.h driver.h
