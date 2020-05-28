#include <stdio.h>
#include <stdlib.h>
#include "pdp.h"


struct Argument get_mr(word w) {
	struct Argument res;
	unsigned int r = w & 7;  			  //номер регистра
	unsigned int mode = (w >> 3) & 7;     //номер моды
	int x;
	switch(mode) {
		case 0:
			res.adr = r;
			res.val = reg[r];
			trace("R%o ", r);
			break;
		case 1:
			res.adr = reg[r];
			if (bw == 0 || r == 7 || r == 6)
				res.val = w_read(res.adr); 				
			else
				res.val = b_read(res.adr); 
			trace("(R%o) ", r);
			break;
		case 2:
			res.adr = reg[r];
			if (r == 6) {
				res.val = w_read(res.adr); 			
				reg[r] += 2;
				trace("(R%o)+ ", r);
			}
			else if (bw == 0 || r == 7) {
				res.val = w_read(res.adr); 			
				reg[r] += 2;
				if (r == 7)
					trace("#%o ", res.val);
				else	
					trace("(R%o)+ ", r);
			}
			else {
				res.val = b_read(res.adr);
				reg[r] += 1;
				trace("(R%o)+ ", r);
			}
			break;
		case 3:
			res.adr = w_read(reg[r]);
			res.val = w_read(res.adr);
			reg[r] += 2; 
			if (r == 7)
				trace("@#%o ", reg[r] - 2);
			else
				trace("@(R%o)+ ", r);
			break;
		case 4:
			if (r == 6) {
				reg[r] -= 2;
				res.adr = reg[r];
				res.val = w_read(res.adr);
			}
			else if (bw == 0 || r == 7) {
				reg[r] -= 2;
				res.adr = reg[r];
				res.val = w_read(res.adr);
			}
			else {
				reg[r] -= 1;
				res.adr = reg[r];
				res.val = b_read(res.adr);
			}
			trace("-(R%o) ", r);
			break;
		case 5:
			if(r == 6){
				res.adr = w_read(reg[r]);
				res.val = w_read(res.adr);
				reg[r] -= 2;
			}
			else{
				reg[r] -= 2;
				res.adr = w_read(reg[r]);
				res.val = w_read(res.adr);
			}	
			trace("@-(R%o) ", r);
			break;
		case 6:
			x = w_read(pc);
			pc += 2;
			res.adr = reg[r] + x;
			res.val = w_read(res.adr);
			if (r == 7)
				trace("%o ", res.adr);
			else
				trace("%o(R%o) ", x, r); 
			break;
		case 7:
			x = w_read(pc);
			pc += 2;
			res.adr = reg[r] + x;
			res.adr = w_read(res.adr);
			res.val = w_read(res.adr);
			if (r == 7)
				trace("@%o ", res.adr);
			else
				trace("@%o(R%o) ", x, r);
			break;
	}
	return res;
}

extern Command cmd[];


void run() {
	trace("------------ running ------------\n");
	pc = 01000;
	sp = 0;
	struct Argument ss, dd;
	unsigned int nn, r, xx;
	while(1) {
		word w = w_read(pc);
		trace("%06o: %06o\n", pc, w);
		pc += 2;
		int i = 0;
		while(1){
			if ((w & cmd[i].mask) == cmd[i].opcode){
				bw = (w >> 15);
				trace("%s ", cmd[i].name);
				if (cmd[i].params & HAS_SS)
					ss = get_mr(w >> 6);
				if (cmd[i].params & HAS_DD)
					dd = get_mr(w);
				if (cmd[i].params & HAS_R){
					r = (w >> 6) & 7;
					trace("R%o ", r);
				}
				if (cmd[i].params & HAS_R1){
					r = w & 7;
					trace("R%o ", r);	
				}
				if (cmd[i].params & HAS_NN) {
					nn = (w & 077) << 1;
					trace("LOOP ");
				}
				if (cmd[i].params & HAS_XX) {
					if ((w & 0x00FF) >> 7 == 1)
						xx = (w & 0x00FF) - 0400;
					else
						xx = w & 0x00FF;
					trace("%d", xx);
				}				
				cmd[i].do_func(dd, ss, nn, r, xx);
				trace("\n");
				break;
			}
			i++;
		}
	}
}
