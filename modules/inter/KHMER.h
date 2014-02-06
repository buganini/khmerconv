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

#define C_SHIFTER2	4096	// for use in KHMER-BONUS
#define C_COENG2	8192	// for use in KHMER-BONUS

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

static inline int khmerType(uint32_t ucs){
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
