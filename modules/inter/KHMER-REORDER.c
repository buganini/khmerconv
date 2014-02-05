#include <bsdconv.h>

/*
 * This program takes input as unordered khmer unicode string and produce
 * an organized khmer unicode string based on the rule:
 * baseCharacter [+ [Robat/Shifter] + [Coeng*] + [Shifter] + [Vowel] + [Sign]]
 */

#define C_BASE		1
#define C_VOWEL		2
#define C_SHIFTER	4		// is shifter (muusekatoan or triisap) characer
#define C_COENG		8
#define C_SIGN		16
#define C_LEFT		32		// vowel appear on left side of base
#define C_WITHE		64		// vowel can be combined with SRA-E
#define C_WITHU		128		// vowel can be combined with SRA-U
#define C_POSRAA	256		// can be under PO SraA
#define C_MUUS		512		// shifter place on specific character
#define C_TRII		1024	// shifter place on specific character
#define C_ROBAT		2048	// is robat character

// important character to test in order to form a cluster
#define K_RO			0x179A
#define K_PO			0x1796
#define K_SRAAA			0x17B6
#define K_SRAE			0x17C1
#define K_SRAOE			0x17BE
#define K_SRAOO			0x17C4
#define K_SRAYA			0x17BF
#define K_SRAIE			0x17C0
#define K_SRAAU			0x17C5
#define K_SRAII			0x17B8
#define K_SRAU			0x17BB
#define K_TRIISAP		0x17CA
#define K_MUUSIKATOAN	0x17C9
#define K_SA			0x179F
#define K_SAMYOKSANNYA	0x17D0
#define K_NYO			0x1789
#define K_ZWSP			0x200B

// list of khmer character in unicode table (start from 1780)
int KHMERCHAR[] = {
	C_BASE,	// ក     0x1780
	C_BASE,	// ខ
	C_BASE,	// គ
	C_BASE,	// ឃ
	C_BASE,	// ង
	C_BASE,	// ច
	C_BASE,	// ឆ
	C_BASE,	// ជ
	C_BASE,	// ឈ
	C_BASE + C_MUUS,	// ញ
	C_BASE,	// ដ
	C_BASE,	// ឋ
	C_BASE,	// ឌ
	C_BASE,	// ឍ
	C_BASE,	// ណ
	C_BASE + C_POSRAA,	// ត
	C_BASE,	// ថ     0x1790
	C_BASE,	// ទ
	C_BASE,	// ធ
	C_BASE + C_POSRAA,	// ន
	C_BASE + C_MUUS,	// ប
	C_BASE,	// ផ
	C_BASE,	// ព
	C_BASE + C_POSRAA,	// ភ
	C_BASE,	// ម
	C_BASE + C_POSRAA,	// យ
	C_BASE + C_POSRAA,	// រ
	C_BASE + C_POSRAA,	// ល
	C_BASE + C_POSRAA,	// វ
	C_BASE,
	C_BASE,
	C_BASE + C_TRII,	// ស
	C_BASE + C_TRII,	// ហ     0x17A0
	C_BASE,	// ឡ
	C_BASE + C_TRII,	// អ
	C_BASE,	// អ
	C_BASE,	// អា
	C_BASE,	// ឥ
	C_BASE,	// ឦ
	C_BASE,	// ឧ
	C_BASE,
	C_BASE,	// ឩ
	C_BASE,	// ឪ
	C_BASE,	// ឫ
	C_BASE,	// ឬ
	C_BASE,	// ឭ
	C_BASE,	// ឮ
	C_BASE,	// ឯ
	C_BASE,	//       0x17B0
	C_BASE,
	C_BASE,	// ឲ
	C_BASE,
	0, 0,
	C_VOWEL + C_WITHE + C_WITHU,	// ា
	C_VOWEL + C_WITHU,	// ិ
	C_VOWEL + C_WITHE + C_WITHU,	// ី
	C_VOWEL + C_WITHU,	// ឹ
	C_VOWEL + C_WITHU,	// ឺ
	C_VOWEL,	// ុ
	C_VOWEL,	// ូ
	C_VOWEL,	// ួ
	C_VOWEL + C_WITHU,	// ើ
	C_VOWEL + C_WITHE,	// ឿ
	C_VOWEL + C_WITHE,	// ៀ     0x17C0
	C_VOWEL + C_LEFT,	// េ
	C_VOWEL + C_LEFT,	// ែ
	C_VOWEL + C_LEFT,	// ៃ
	C_VOWEL,	// ោ
	C_VOWEL + C_WITHE,	// ៅ
	C_SIGN + C_WITHU,	// ំ
	C_SIGN,	// ះ
	C_SIGN,	// ៈ
	C_SHIFTER,	// ៉
	C_SHIFTER,	// ៊
	C_SIGN,	// ់
	C_ROBAT,	// ៌
	C_SIGN,	// ៍
	C_SIGN,
	C_SIGN,	// ៏
	C_SIGN + C_WITHU,	// ័​​   0x17D0
	C_SIGN,
	C_COENG,	// ្
	C_SIGN
};

static int khmerType(uint32_t ucs){
	/*
	 * input one unicode character;
	 * output an integer which is the Khmer type of the character or 0
	 */
	if(ucs >= 0x1780){
		ucs -= 0x1780;
		if(ucs < sizeof(KHMERCHAR)/sizeof(int))
			return KHMERCHAR[ucs];
	}
	return 0;
}

struct ll_s{
	struct data_rt *p;
	int ccc;
	struct ll_s *prev;
	struct ll_s *next;
};

struct my_s{
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

static struct data_rt D_MUUSIKATOAN;
static struct data_rt D_TRIISAP;
static struct data_rt D_NYO;
static struct data_rt D_SRAOE;
static struct data_rt D_SRAYA;
static struct data_rt D_SRAIE;
static struct data_rt D_SRAOO;
static struct data_rt D_SRAAU;

int cbcreate(struct bsdconv_instance *ins, struct bsdconv_hash_entry *arg){
	struct my_s *r=malloc(sizeof(struct my_s));
	THIS_CODEC(ins)->priv=r;
	str2datum("0117C9", &D_MUUSIKATOAN);
	str2datum("0117CA", &D_TRIISAP);
	str2datum("011789", &D_NYO);
	str2datum("0117BE", &D_SRAOE);
	str2datum("0117BF", &D_SRAYA);
	str2datum("0117C0", &D_SRAIE);
	str2datum("0117C4", &D_SRAOO);
	str2datum("0117C5", &D_SRAAU);

	D_MUUSIKATOAN.flags=0;
	D_TRIISAP.flags=0;
	D_NYO.flags=0;
	D_SRAOO.flags=0;
	D_SRAOE.flags=0;
	D_SRAYA.flags=0;
	D_SRAIE.flags=0;
	D_SRAOO.flags=0;
	D_SRAAU.flags=0;

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
	DATUM_FREE(X); \
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
			DATUM_FREE(r->vowel);
			CLEAR(r->vowel);
			r->vowel_ucs = 0;
			r->shifter1 = dup_data_rt(ins, &D_MUUSIKATOAN);
		}
	}
	// examine ifshifter1 should move shifter2 (base on coeng)
	if(r->shifter1 && r->coeng1){
		int c = khmerType(r->coeng1_follower_ucs);
		if(c & C_TRII){
			r->shifter2=dup_data_rt(ins, &D_TRIISAP);
			DATUM_FREE(r->shifter1);
			r->shifter1 = NULL;
		}else if(c & C_MUUS){
			r->shifter2 = dup_data_rt(ins, &D_MUUSIKATOAN);
			DATUM_FREE(r->shifter1);
			r->shifter1 = NULL;
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
		if((r->poSraA && underPoSraA==NULL && r->vowel) || ((r->baseChar_ucs == K_PO) && (r->vowel_ucs == K_SRAAA) && underPoSraA==NULL)){
			// change baseChar to letter NYO
			r->baseChar=dup_data_rt(ins, &D_NYO);
			if(r->vowel_ucs == K_SRAAA && !r->poSraA){
				r->vowel = NULL;
				r->vowel_ucs = 0;
			}
		}
	}

	// PO + SraA + SraE
	if(r->poSraA && r->vowel_ucs == K_SRAE){
		// PO + sraA is not NYO and there is leading sraE they should be recombined
		DATUM_FREE(r->vowel);
		r->vowel=dup_data_rt(ins, &D_SRAOO);
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
			r->coeng_bak=NULL;
			CLEAR(r->keep);
			CONTINUE();

		// coeng1 is coeng RO, the cluster can have two coeng, dump coeng to coeng2
		}else if(r->coeng1_follower_ucs == K_RO){
			r->coeng2 = dup_data_rt(ins, r->coeng_bak);
			r->coeng2_follower = dup_data_rt(ins, curr);
			r->coeng2_follower_ucs = ucs;
			r->coeng_bak=NULL;
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
							DATUM_FREE(r->vowel);
							r->vowel = dup_data_rt(ins, &D_SRAOE);
							r->vowel_ucs = K_SRAOE;
							break;
						case K_SRAYA:
							DATUM_FREE(r->vowel);
							r->vowel = dup_data_rt(ins, &D_SRAYA);
							r->vowel_ucs = K_SRAYA;
							break;
						case K_SRAIE:
							DATUM_FREE(r->vowel);
							r->vowel = dup_data_rt(ins, &D_SRAIE);
							r->vowel_ucs = K_SRAIE;
							break;
						case K_SRAAA:
							DATUM_FREE(r->vowel);
							r->vowel = dup_data_rt(ins, &D_SRAOO);
							r->vowel_ucs = K_SRAOO;
							break;
						case K_SRAAU:
							DATUM_FREE(r->vowel);
							r->vowel = dup_data_rt(ins, &D_SRAAU);
							r->vowel_ucs = K_SRAAU;
							break;
					}

					CLEAR(r->keep);

				// test if vowel can be combine with sin[i] (e.g. sra U and sra I or vice versa)
				}else if((r->vowel_ucs == K_SRAU && (sinType & C_WITHU)) || ((khmerType(r->vowel_ucs) & C_WITHU) && ucs == K_SRAU)){
					// vowel is not Sra I, II, Y, YY, transfer value from sin[i] to vowel
					if(!(khmerType(r->vowel_ucs) & C_WITHU)){
						DATUM_FREE(r->vowel);
						r->vowel = dup_data_rt(ins, curr);
						r->vowel_ucs = ucs;
					}
					// select shifter1 base on specific consonants
					if(r->baseChar && (khmerType(r->baseChar_ucs) & C_TRII)){
						r->shifter1=dup_data_rt(ins, &D_TRIISAP);
					}else{
						r->shifter1=dup_data_rt(ins, &D_MUUSIKATOAN);
					}
					// examine if shifter1 should move shifter2 (base on coeng SA)
				}else if((r->vowel_ucs == K_SRAE) && (ucs == K_SRAU)){
					if(r->baseChar && (khmerType(r->baseChar_ucs) & C_TRII)){
						r->shifter1=dup_data_rt(ins, &D_TRIISAP);
					}else{
						r->shifter1=dup_data_rt(ins, &D_MUUSIKATOAN);
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
				r->keep = dup_data_rt(ins, curr);
				CONTINUE();
			}else{
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

	free(D_MUUSIKATOAN.data);
	free(D_TRIISAP.data);
	free(D_NYO.data);
	free(D_SRAOE.data);
	free(D_SRAYA.data);
	free(D_SRAIE.data);
	free(D_SRAOO.data);
	free(D_SRAAU.data);

	cbinit(ins);
	free(r);
}
