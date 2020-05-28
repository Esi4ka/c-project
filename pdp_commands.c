#include <stdio.h>
#include "pdp.h"
#include <stdlib.h>

int N, Z, C;
void set_NZ(word w){
	if(bw == 0)
		N = (w >> 15) & 1;
	else
		N = (w >> 7) & 1;
	Z = (w == 0);
}

void set_C(word w){
	if (bw == 0)
		C = (w >> 15) & 1;
	else 
		C = (w >> 7) & 1;
}

void do_mov(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (bw == 0)
		w_write(dd.adr, ss.val);
	else {
		b_write(dd.adr, ss.val);
		if (dd.adr == 0177566)
			printf("%c", display_val);	
	}
	set_NZ(ss.val);
};


void do_add(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (dd.adr < 8 && ss.adr < 8) {
		reg[dd.adr] = reg[dd.adr] + reg[ss.adr];
		set_NZ(reg[dd.adr]);
		set_C((reg[dd.adr] + reg[ss.adr]) >> 1);
	}
	else if (dd.adr < 8 && ss.adr > 8){
		reg[dd.adr] = reg[dd.adr] + ss.val;
		set_NZ(reg[dd.adr]);
		set_C((reg[dd.adr] + ss.val) >> 1);
	}
	else {
		word w = w_read(dd.adr) + w_read(ss.adr);
		w_write(dd.adr, w);
		set_NZ(w);
		set_C((w_read(dd.adr) + w_read(ss.adr)) >> 1);
	}
};


void print_reg() {
	trace("R0:%o R1:%o R2:%o R3:%o \nR4:%o R5:%o R6:%o R7:%o\n", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
};

void do_halt() {
	trace("\n------------ halted -------------\n");
	print_reg();
	exit(0);
};


void do_inc(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (dd.adr < 8)
		reg[dd.adr] += 1;
	else if(bw == 0)
		w_write(dd.adr, w_read(dd.adr) + 1);
	else
		b_write(dd.adr, b_read(dd.adr) + 1);
	set_NZ(w_read(dd.adr) + 1);
};

void do_sob(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	reg[r] -= 1;
	if (reg[r] != 0)
		pc -= nn;
};

void do_ccc(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	N = 0;
	Z = 0;
	C = 0;
};

void do_cln(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	N = 0;
};

void do_clz(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	Z = 0;
};

void do_clc(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	C = 0;
};

void do_scc(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	N = 1;
	Z = 1;
	C = 1;
};

void do_sec(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	C = 1;
};

void do_sen(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	N = 1;
};

void do_sez(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	Z = 1;
};

void do_nop(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
};

void do_clr(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (dd.adr < 8)
		reg[dd.adr] = 0;
	else
		w_write(dd.adr, 0);
	do_clc(dd, ss, nn, r, xx);
	do_sez(dd, ss, nn, r, xx);
	do_cln(dd, ss, nn, r, xx);
};

void do_tst(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (dd.adr < 8){
		set_NZ(reg[dd.adr]);
	}
	else{
		set_NZ(w_read(dd.adr));
	}
	do_clc(dd, ss, nn, r, xx);
};

void do_cmp(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (dd.adr < 8 && ss.adr < 8){
		word w = reg[ss.adr] - reg[dd.adr];
		set_NZ(w);
		set_C((reg[ss.adr] - reg[dd.adr]) >> 1);
	}
	else{
		word w = w_read(ss.adr) - w_read(dd.adr);
		set_NZ(w);
		set_C((w_read(ss.adr) - w_read(dd.adr)) >> 1);
	}
};

void do_br(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	pc = pc + 2*xx;
};

void do_bcc(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (C == 0)
		do_br(dd, ss, nn, r, xx);
};

void do_beq(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (Z == 1)
		do_br(dd, ss, nn, r, xx);
};

void do_bpl(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (N == 0)
		do_br(dd, ss, nn, r, xx);
};

void do_jsr(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	sp -= 2;
	w_write(sp, reg[r]);
	reg[r] = pc;
	pc = dd.adr;

};

void do_rts(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	pc = reg[r];
	reg[r] = w_read(sp);
	sp += 2;
};

void do_asl(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (bw == 0){
		word w = w_read(dd.adr);
		w_write(dd.adr, w * 2);	
		set_NZ(w_read(dd.adr));
		set_C((w * 2) >> 1);
	}
	else{
		byte w = b_read(dd.adr);
		b_write(dd.adr, w * 2);	
		set_NZ(b_read(dd.adr));
		set_C((w * 2) >> 1);
	}
};

void do_rol(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (bw == 0){
		word w = w_read(dd.adr);
		word w1 = w;
		w = (w << 1) | C;
		set_C(((w1 << 1) | C)>>1);
		w_write(dd.adr, w);
		set_NZ(w);
	}
	else{
		byte w = b_read(dd.adr);
		byte w1 = w;
		w = (w << 1) | C;
		set_C(((w1 << 1) | C)>>1);
		b_write(dd.adr, w);
		set_NZ(w);
	}
};

void do_bic(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	if (bw == 0){
		word w = w_read(dd.adr);
		w_write(dd.adr, w & (~ss.val));
		set_NZ(w_read(dd.adr));
	}
	else{
		byte w = b_read(dd.adr);
		b_write(dd.adr, w & (~ss.val));
		set_NZ(b_read(dd.adr));	
	}
};


void do_unkn(struct Argument dd, struct Argument ss, unsigned int nn, unsigned int r, unsigned int xx) {
	trace("UNKNOWN FUNCTION\n");
	exit(0);
}

Command cmd[] = {
	{0070000, 0010000, "mov", do_mov, HAS_SS + HAS_DD},
	{0170000, 0060000, "add", do_add, HAS_SS + HAS_DD},
	{0xFFFF, 0000000, "halt", do_halt, NO_PARAMS},
	{0077700, 0005200, "inc", do_inc, HAS_DD},
	{0177000, 0077000, "sob", do_sob, HAS_R + HAS_NN},
	{0177700, 0005000, "clr", do_clr, HAS_DD},
	{0077700, 0005700, "tst", do_tst, HAS_DD},
	{0070000, 0020000, "cmp", do_cmp, HAS_DD + HAS_SS},
	{0177777, 0000257, "ccc", do_ccc, NO_PARAMS},
	{0177777, 0000241, "clc", do_clc, NO_PARAMS},
	{0177777, 0000250, "cln", do_cln, NO_PARAMS},
	{0177777, 0000244, "clz", do_clz, NO_PARAMS},
	{0177777, 0000277, "scc", do_scc, NO_PARAMS},
	{0177777, 0000261, "sec", do_sec, NO_PARAMS},
	{0177777, 0000270, "sen", do_sen, NO_PARAMS},
	{0177777, 0000264, "sez", do_sez, NO_PARAMS},
	{0177777, 0000240, "nop", do_nop, NO_PARAMS},
	{0xFF00, 0x0100, "br", do_br, HAS_XX},
	{0xFF00, 0x8600, "bcc",do_bcc, HAS_XX},
	{0xFF00, 0x0300, "beq", do_beq, HAS_XX},
	{0xFF00, 0x8000, "bpl", do_bpl, HAS_XX},
	{0177000, 0004000, "jsr", do_jsr, HAS_DD + HAS_R},
	{0177770, 0000200, "rts", do_rts, HAS_R1},
	{0007700, 0006300, "asl", do_asl, HAS_DD},
	{0007700, 0006100, "rol", do_rol, HAS_DD},
	{0070000, 0040000, "bic", do_bic, HAS_DD + HAS_SS},
	{0000000, 0000000,"UNKNOWN", do_unkn, NO_PARAMS}
};
