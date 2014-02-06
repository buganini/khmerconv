#include <bsdconv.h>

/*
 * This program takes input as unordered khmer unicode string and produce
 * an organized khmer unicode string based on the rule:
 * baseCharacter [+ [Robat/Shifter] + [Coeng*] + [Shifter] + [Vowel] + [Sign]]
 */

#include "KHMER.h"

struct ll_s{
	struct data_rt *p;
	int ccc;
	struct ll_s *prev;
	struct ll_s *next;
};

struct my_s{
	struct data_rt D_MUUSIKATOAN;
	struct data_rt D_TRIISAP;
	struct data_rt D_NYO;
	struct data_rt D_SRAOE;
	struct data_rt D_SRAYA;
	struct data_rt D_SRAIE;
	struct data_rt D_SRAOO;
	struct data_rt D_SRAAU;

	int status;

	struct data_rt *baseChar;
	uint32_t baseChar_ucs;
	struct data_rt *robat;
	struct data_rt *shifter1;
	struct data_rt *shifter2;
	struct data_rt *coeng_bak;
	struct data_rt *coeng1;
	struct data_rt *coeng1_follower;
	uint32_t coeng1_follower_ucs;
	struct data_rt *coeng2;
	struct data_rt *coeng2_follower;
	uint32_t coeng2_follower_ucs;
	struct data_rt *vowel;
	uint32_t vowel_ucs;
	struct data_rt *sign;
	uint32_t sign_ucs;
	struct data_rt *keep;
	struct data_rt *cluster;
	int poSraA;
};


int cbcreate(struct bsdconv_instance *ins, struct bsdconv_hash_entry *arg){
	struct my_s *r=malloc(sizeof(struct my_s));
	THIS_CODEC(ins)->priv=r;
	str2datum("0117C9", &r->D_MUUSIKATOAN);
	str2datum("0117CA", &r->D_TRIISAP);
	str2datum("011789", &r->D_NYO);
	str2datum("0117BE", &r->D_SRAOE);
	str2datum("0117BF", &r->D_SRAYA);
	str2datum("0117C0", &r->D_SRAIE);
	str2datum("0117C4", &r->D_SRAOO);
	str2datum("0117C5", &r->D_SRAAU);

	r->D_MUUSIKATOAN.flags=0;
	r->D_TRIISAP.flags=0;
	r->D_NYO.flags=0;
	r->D_SRAOO.flags=0;
	r->D_SRAOE.flags=0;
	r->D_SRAYA.flags=0;
	r->D_SRAIE.flags=0;
	r->D_SRAOO.flags=0;
	r->D_SRAAU.flags=0;

	r->baseChar=NULL;
	r->robat=NULL;
	r->shifter1=NULL;
	r->shifter2=NULL;
	r->coeng_bak=NULL;
	r->coeng1=NULL;
	r->coeng1_follower=NULL;
	r->coeng2=NULL;
	r->coeng2_follower=NULL;
	r->vowel=NULL;
	r->sign=NULL;
	r->keep=NULL;
	r->cluster=NULL;

	return 0;
}

#define CLEAR(X) if(X){ \
	DATUM_FREE(ins, X); \
	X=NULL; \
}

void cbinit(struct bsdconv_instance *ins){
	struct my_s *r=THIS_CODEC(ins)->priv;
	CLEAR(r->baseChar);
	r->baseChar_ucs=0;
	CLEAR(r->robat);
	CLEAR(r->shifter1);
	CLEAR(r->shifter2);
	r->coeng_bak=NULL;
	CLEAR(r->coeng1);
	CLEAR(r->coeng1_follower);
	r->coeng1_follower_ucs=0;
	CLEAR(r->coeng2);
	CLEAR(r->coeng2_follower);
	r->coeng2_follower_ucs=0;
	CLEAR(r->vowel);
	r->vowel_ucs=0;
	CLEAR(r->sign);
	r->sign_ucs=0;
	CLEAR(r->keep);
	CLEAR(r->cluster);
	r->poSraA=0;
}

#define CONCAT(d) do{ \
	this_phase->data_tail->next = (d); \
	this_phase->data_tail = this_phase->data_tail->next; \
	d = NULL; \
}while(0);

void cbflush(struct bsdconv_instance *ins){
	struct bsdconv_phase *this_phase=THIS_PHASE(ins);
	struct my_s *r=THIS_CODEC(ins)->priv;

	if(r->coeng_bak){
		CLEAR(r->coeng1);
		r->coeng1 = dup_data_rt(ins, r->coeng_bak);
		r->coeng_bak = NULL;
	}

	// Organization of a cluster{
	if((r->vowel_ucs == K_SRAU) && (r->sign) && (khmerType(r->sign_ucs) & C_WITHU)){
		// samyoksanha + sraU --> MUUS + samyoksanha
		if(r->sign_ucs == K_SAMYOKSANNYA){
			DATUM_FREE(ins, r->vowel);
			CLEAR(r->vowel);
			r->vowel_ucs = 0;
			CLEAR(r->shifter1);
			r->shifter1 = dup_data_rt(ins, &r->D_MUUSIKATOAN);
		}
	}
	// examine ifshifter1 should move shifter2 (base on coeng)
	if(r->shifter1 && r->coeng1){
		int c = khmerType(r->coeng1_follower_ucs);
		if(c & C_TRII){
			CLEAR(r->shifter2);
			r->shifter2 = dup_data_rt(ins, &r->D_TRIISAP);
			CLEAR(r->shifter1);
		}else if(c & C_MUUS){
			CLEAR(r->shifter2);
			r->shifter2 = dup_data_rt(ins, &r->D_MUUSIKATOAN);
			CLEAR(r->shifter1);
		}
	}
	// examine if PO + sraA > NYO, this case can only determin
	// here since it need all element
	// coeng2 is priority (if coeng2 exist, coeng1 is always coRO)
	struct data_rt *underPoSraA = NULL;
	struct data_rt *underPoSraA_follower = NULL;
	uint32_t underPoSraA_follower_ucs = 0;
	if(r->coeng2){
		underPoSraA=r->coeng2;
		underPoSraA_follower = r->coeng2_follower;
		underPoSraA_follower_ucs = r->coeng2_follower_ucs;
	}else if(r->coeng1){
		underPoSraA=r->coeng1;
		underPoSraA_follower = r->coeng1_follower;
		underPoSraA_follower_ucs = r->coeng1_follower_ucs;
	}
	if(underPoSraA && underPoSraA_follower){
		int underPoSraA = khmerType(underPoSraA_follower_ucs) & C_POSRAA;
		// test if coeng is allow under PO + SRAA
		if(
			(r->poSraA && !underPoSraA && r->vowel) || ((r->baseChar_ucs == K_PO) && (r->vowel_ucs == K_SRAAA) && !underPoSraA)){
			// change baseChar to letter NYO
			CLEAR(r->baseChar);
			r->baseChar = dup_data_rt(ins, &r->D_NYO);
			if(r->vowel_ucs == K_SRAAA && !r->poSraA){
				DATUM_FREE(ins, r->vowel);
				r->vowel_ucs = 0;
			}
		}
	}

	// PO + SraA + SraE
	if(r->poSraA && r->vowel_ucs == K_SRAE){
		// PO + sraA is not NYO and there is leading sraE they should be recombined
		DATUM_FREE(ins, r->vowel);
		r->vowel = dup_data_rt(ins, &r->D_SRAOO);
	}

	// Rule of cluster
	// if there are two coeng, ceong1 is always coRO so put it after coeng2

	//cluster = baseChar + robat + shifter1 + coeng2 + coeng1 + shifter2 + vowel + sign
	//result = cluster + keep

	if(r->baseChar)
		CONCAT(r->baseChar);

	if(r->robat)
		CONCAT(r->robat);

	if(r->shifter1)
		CONCAT(r->shifter1);

	if(r->coeng2)
		CONCAT(r->coeng2);

	if(r->coeng2_follower)
		CONCAT(r->coeng2_follower);

	if(r->coeng1)
		CONCAT(r->coeng1);

	if(r->coeng1_follower)
		CONCAT(r->coeng1_follower);

	if(r->shifter2)
		CONCAT(r->shifter2);

	if(r->vowel)
		CONCAT(r->vowel);

	if(r->sign)
		CONCAT(r->sign);

	if(r->keep)
		CONCAT(r->keep);

	this_phase->data_tail->next = NULL;

	cbinit(ins);
}

#define YIELD() do{ \
	cbflush(ins); \
	this_phase->state.status = YIELD; \
	return; \
}while(0);

#define CONTINUE() do{ \
	this_phase->state.status = CONTINUE; \
	return; \
}while(0);

void cbconv(struct bsdconv_instance *ins){
	/*
	 * This program takes input as unordered khmer unicode string and produce
	 * an organized khmer unicode string based on the rule:
	 * baseCharacter [+ [Robat/Shifter] + [Coeng*] + [Shifter] + [Vowel] + [Sign]]
	 */
	struct my_s *r=THIS_CODEC(ins)->priv;
	int i;
	uint32_t ucs=0;
	struct bsdconv_phase *this_phase=THIS_PHASE(ins);
	struct data_rt *curr=this_phase->curr;
	unsigned char *data=UCP(curr->data);

	if(curr->len==0){
		this_phase->state.status = NOOP;
		return;
	}
	if(data[0]!=0x1){
		this_phase->flags &= ~(F_MATCH | F_PENDING);
		this_phase->state.status = DEADEND;
		return;
	}

	this_phase->state.status = CONTINUE;

	for(i=1;i<curr->len;++i){
		ucs<<=8;
		ucs|=data[i];
	}

	if(r->coeng_bak){
		// if it is coeng RO (and consonent is not blank), it must belong to next cluster
		// so finish this cluster
		if(ucs == K_RO && r->baseChar){
			this_phase->curr = r->coeng_bak;
			r->coeng_bak = NULL;
			YIELD();
		}

		// no coeng yet so dump coeng to coeng1
		if(r->coeng1==NULL){
			r->coeng1 = dup_data_rt(ins, r->coeng_bak);
			r->coeng1_follower = dup_data_rt(ins, curr);
			r->coeng1_follower_ucs = ucs;
			r->coeng_bak = NULL;
			CLEAR(r->keep);
			CONTINUE();

		// coeng1 is coeng RO, the cluster can have two coeng, dump coeng to coeng2
		}else if(r->coeng1_follower_ucs == K_RO){
			r->coeng2 = dup_data_rt(ins, r->coeng_bak);
			r->coeng2_follower = dup_data_rt(ins, curr);
			r->coeng2_follower_ucs = ucs;
			r->coeng_bak = NULL;
			CLEAR(r->keep);
			CONTINUE();
		}else{
			YIELD();
		}
	}else{
		int sinType = khmerType(ucs);

		if(sinType & C_BASE){
			if(r->baseChar){
				// second baseChar -> end of cluster
				YIELD();
			}
			r->baseChar = dup_data_rt(ins, curr);
			r->baseChar_ucs = ucs;
			CLEAR(r->keep);
			CONTINUE();
		}else if(sinType & C_ROBAT){
			if(r->robat){
				// second robat -> end of cluster
				YIELD();
			}
			r->robat = dup_data_rt(ins, curr);
			CLEAR(r->keep);
			CONTINUE();
		}else if(sinType & C_SHIFTER){
			if(r->shifter1){
				// second shifter -> end of cluster
				YIELD();
			}
			r->shifter1 = dup_data_rt(ins, curr);
			CLEAR(r->keep);
			CONTINUE();
		}else if(sinType & C_SIGN){
			if(r->sign){
				// second sign -> end of cluster
				YIELD();
			}
			r->sign = dup_data_rt(ins, curr);
			r->sign_ucs = ucs;
			CLEAR(r->keep);
			CONTINUE();
		}else if(sinType & C_COENG){
			r->coeng_bak = curr;
			this_phase->state.data = NULL;
			this_phase->state.status = SUBMATCH;
			return;
		}else if(sinType & C_VOWEL){
			if(r->vowel == NULL){
				// if it is sra E ES AI (and consonent is not blank), it must belong to next cluster,
				// so finish this cluster
				if((sinType & C_LEFT) && (r->baseChar)){
					YIELD();
				}
				// give vowel a value found in the unorganized cluster
				r->vowel = dup_data_rt(ins, curr);
				r->vowel_ucs = ucs;
				CLEAR(r->keep);
			}else if((r->baseChar_ucs == K_PO) && (!r->poSraA) && ((ucs == K_SRAAA) || (r->vowel_ucs == K_SRAAA))){
				r->poSraA = 1;
				if(r->vowel_ucs == K_SRAAA){
					DATUM_FREE(ins, r->vowel);
					r->vowel = dup_data_rt(ins, curr);
					r->vowel_ucs = ucs;
					CLEAR(r->keep);
				}
			}else{
				// test if sra E is follow by sin which could combine with the following
				if((r->vowel_ucs == K_SRAE) && (sinType & C_WITHE)){
					// give vowel a real sra by eleminate leading sra E
					// possible combination for sra E
					switch(ucs){
						case K_SRAII:
							DATUM_FREE(ins, r->vowel);
							r->vowel = dup_data_rt(ins, &r->D_SRAOE);
							r->vowel_ucs = K_SRAOE;
							break;
						case K_SRAYA:
							DATUM_FREE(ins, r->vowel);
							r->vowel = dup_data_rt(ins, &r->D_SRAYA);
							r->vowel_ucs = K_SRAYA;
							break;
						case K_SRAIE:
							DATUM_FREE(ins, r->vowel);
							r->vowel = dup_data_rt(ins, &r->D_SRAIE);
							r->vowel_ucs = K_SRAIE;
							break;
						case K_SRAAA:
							DATUM_FREE(ins, r->vowel);
							r->vowel = dup_data_rt(ins, &r->D_SRAOO);
							r->vowel_ucs = K_SRAOO;
							break;
						case K_SRAAU:
							DATUM_FREE(ins, r->vowel);
							r->vowel = dup_data_rt(ins, &r->D_SRAAU);
							r->vowel_ucs = K_SRAAU;
							break;
					}

					CLEAR(r->keep);

				// test if vowel can be combine with sin[i] (e.g. sra U and sra I or vice versa)
				}else if((r->vowel_ucs == K_SRAU && (sinType & C_WITHU)) || ((khmerType(r->vowel_ucs) & C_WITHU) && ucs == K_SRAU)){
					// vowel is not Sra I, II, Y, YY, transfer value from sin[i] to vowel
					if(!(khmerType(r->vowel_ucs) & C_WITHU)){
						DATUM_FREE(ins, r->vowel);
						r->vowel = dup_data_rt(ins, curr);
						r->vowel_ucs = ucs;
					}
					// select shifter1 base on specific consonants
					if(r->baseChar && (khmerType(r->baseChar_ucs) & C_TRII)){
						CLEAR(r->shifter1);
						r->shifter1 = dup_data_rt(ins, &r->D_TRIISAP);
					}else{
						CLEAR(r->shifter1);
						r->shifter1 = dup_data_rt(ins, &r->D_MUUSIKATOAN);
					}
					// examine if shifter1 should move shifter2 (base on coeng SA)
				}else if((r->vowel_ucs == K_SRAE) && (ucs == K_SRAU)){
					if(r->baseChar && (khmerType(r->baseChar_ucs) & C_TRII)){
						CLEAR(r->shifter1);
						r->shifter1 = dup_data_rt(ins, &r->D_TRIISAP);
					}else{
						CLEAR(r->shifter1);
						r->shifter1 = dup_data_rt(ins, &r->D_MUUSIKATOAN);
					}
				}else{
					// sign can't be combine -> end of cluster
					YIELD();
				}
			}
		}else{
			// other than khmer -> end of cluster
			// continue with the next character
			if(ucs == K_ZWSP){
				// avoid breaking of cluster ifmeet zwsp
				// and move zwsp to end of cluster
				CLEAR(r->keep);
				r->keep = dup_data_rt(ins, curr);
				CONTINUE();
			}else{
				CLEAR(r->keep);
				r->keep = dup_data_rt(ins, curr);
				cbflush(ins);
				this_phase->state.status=NEXTPHASE;
				return;
			}
		}
		CONTINUE();
	}
}

void cbdestroy(struct bsdconv_instance *ins){
	struct my_s *r=THIS_CODEC(ins)->priv;

	free(r->D_MUUSIKATOAN.data);
	free(r->D_TRIISAP.data);
	free(r->D_NYO.data);
	free(r->D_SRAOE.data);
	free(r->D_SRAYA.data);
	free(r->D_SRAIE.data);
	free(r->D_SRAOO.data);
	free(r->D_SRAAU.data);

	cbinit(ins);
	free(r);
}
