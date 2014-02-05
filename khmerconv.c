/*
 * Copyright (c) 2014 Kuan-Chung Chiu <buganini@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <bsdconv.h>

#define IBUFLEN 1024

static bsdconv_counter_t process(FILE *, FILE *);
static void usage(void);
static void finish(int r);

struct codec {
	char *name;
	struct bsdconv_instance *evl;
	struct bsdconv_instance *ins;
	char *evl_conv, *conv;
	double wv;
	char up;
};

static struct codec codecs1[6];
static struct codec codecs2[11];
static size_t bufsiz;
static char verbose;
static char linebreak;

int main(int argc, char *argv[]){
	int ch;
	int i;
	bsdconv_counter_t e;
	char inplace=0;
	char *tmp;
	int fd;
	FILE *fi, *fo;

	linebreak=0;
	bufsiz=8192;

	i=0;

	codecs1[i].name="CP1252/ISO-8859-1";
	codecs1[i].evl_conv="CP1252:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="CP1252:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	codecs1[i].name="UTF-8";
	codecs1[i].evl_conv="UTF-8:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="UTF-8:NOBOM:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	codecs1[i].name="UTF-16LE";
	codecs1[i].evl_conv="UTF-16le:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="UTF-16le:NOBOM:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	codecs1[i].name="UTF-16BE";
	codecs1[i].evl_conv="UTF-16be:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="UTF-16be:NOBOM:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	codecs1[i].name="UTF-32LE";
	codecs1[i].evl_conv="UTF-32le:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="UTF-32le:NOBOM:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	codecs1[i].name="UTF-32BE";
	codecs1[i].evl_conv="UTF-32be:SCORE#WITH=LATIN1:COUNT:NULL";
	codecs1[i].conv="UTF-32be:NOBOM:UTF-8";
	codecs1[i].ins=NULL;
	i+=1;

	i=0;

	codecs2[i].name="ABC";
	codecs2[i].evl_conv="UTF-8:KHMER-ABC:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-ABC:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="ABC2";
	codecs2[i].evl_conv="UTF-8:KHMER-ABC2:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-ABC2:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="ABC3";
	codecs2[i].evl_conv="UTF-8:KHMER-ABC3:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-ABC3:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="ABC-ZWSP";
	codecs2[i].evl_conv="UTF-8:KHMER-ABC-ZWSP:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-ABC-ZWSP:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="BAIDOK";
	codecs2[i].evl_conv="UTF-8:KHMER-BAIDOK:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-BAIDOK:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="FK";
	codecs2[i].evl_conv="UTF-8:KHMER-FK:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-FK:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="KAOHKONG";
	codecs2[i].evl_conv="UTF-8:KHMER-KAOHKONG:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-KAOHKONG:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="KHEK";
	codecs2[i].evl_conv="UTF-8:KHMER-KHEK:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-KHEK:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="LIMON";
	codecs2[i].evl_conv="UTF-8:KHMER-LIMON:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-LIMON:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="RASMEI";
	codecs2[i].evl_conv="UTF-8:KHMER-RASMEI:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-RASMEI:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;

	codecs2[i].name="TRUTH";
	codecs2[i].evl_conv="UTF-8:KHMER-TRUTH:KHMER-REORDER:NULL";
	codecs2[i].conv="UTF-8:KHMER-TRUTH:KHMER-REORDER:UTF-8";
	codecs2[i].ins=NULL;
	i+=1;


	while ((ch = getopt(argc, argv, "ifs:vwmx")) != -1)
		switch(ch) {
		case 'i':
			inplace=1;
			break;
		case 'f':
			inplace=2;
			break;
		case 's':
			if(sscanf(optarg, "%d", &i)!=1)
				usage();
			bufsiz=i;
			break;
		case 'v':
			verbose=1;
			break;
		case 'w':
		case 'm':
		case 'x':
			linebreak=ch;
			break;
		case '?':
		default:
			usage();
		}

#ifdef WIN32
	setmode(STDIN_FILENO, O_BINARY);
	setmode(STDOUT_FILENO, O_BINARY);
#endif

	for(i=0;i<sizeof(codecs1)/sizeof(struct codec);++i){
		codecs1[i].evl = bsdconv_create(codecs1[i].evl_conv);
		if(codecs1[i].evl == NULL){
			char *e = bsdconv_error();
			fprintf(stderr, "WARNING: Skipping encoding %s: %s\n", codecs1[i].name, e);
			bsdconv_free(e);
			continue;
		}
	}

	for(i=0;i<sizeof(codecs2)/sizeof(struct codec);++i){
		codecs2[i].evl = bsdconv_create(codecs2[i].evl_conv);
		if(codecs2[i].evl == NULL){
			char *e = bsdconv_error();
			fprintf(stderr, "WARNING: Skipping font %s: %s\n", codecs2[i].name, e);
			bsdconv_free(e);
			continue;
		}
	}

	if(optind<argc){
		for(;optind<argc;optind++){
			fi=fopen(argv[optind],"rb");
			if(fi==NULL){
				fprintf(stderr, "Failed opening file %s.\n", argv[optind]);
				continue;
			}
			if(inplace==0){
				process(fi, stdout);
				fclose(fi);
			}else{
				tmp=malloc(strlen(argv[optind])+8);
				strcpy(tmp, argv[optind]);
				strcat(tmp, ".XXXXXX");
				if((fd=mkstemp(tmp))==-1){
					free(tmp);
					fprintf(stderr, "Failed creating temp file.\n");
					fclose(fi);
					continue;
				}
				fo=fdopen(fd, "wb");
				if(!fo){
					fprintf(stderr, "Unable to open output file for %s.\n", argv[optind]);
					fclose(fi);
					continue;
				}
#ifndef WIN32
				struct stat stat;
				fstat(fileno(fi), &stat);
				fchown(fileno(fo), stat.st_uid, stat.st_gid);
				fchmod(fileno(fo), stat.st_mode);
#endif
				e=process(fi, fo);
				fclose(fi);
				fclose(fo);
				if(e==0 || inplace==2){
					unlink(argv[optind]);
					rename(tmp,argv[optind]);
				}else{
					fprintf(stderr, "Skipping %s (%zu error(s))\n", argv[optind], e);
					unlink(tmp);
				}
				free(tmp);
			}
		}
	}else{
		process(stdin, stdout);
	}

	finish(0);

	return 0;
}

static int input(FILE *fi, FILE *tmp, char **ib, size_t *len, int *rwnd){
	if(rwnd == NULL){
		*ib = malloc(bufsiz);
		*len = fread(*ib, 1, bufsiz, fi);
		if(tmp != NULL)
			fwrite(*ib, *len, 1, tmp);
		if(feof(fi))
			return 1;
		return 0;
	}else if(*rwnd != 0){
		*rwnd = 0;
		if(tmp!=NULL){
			free(*ib);
			fseek(tmp, 0L, SEEK_SET);
			*ib=malloc(bufsiz);
			*len=fread(*ib, 1, bufsiz, tmp);
		}
		return 0;
	}else if(tmp && !feof(tmp)){
		*ib=malloc(bufsiz);
		*len=fread(*ib, 1, bufsiz, tmp);
		return 0;
	}else{
		int ret = 0;
		*ib=malloc(bufsiz);
		*len=fread(*ib, 1, bufsiz, fi);
		if(feof(fi)){
			ret = 1;
		}
		return ret;
	}
}

static bsdconv_counter_t process(FILE *fi, FILE *fo){
	char *conv;
	struct bsdconv_instance *ins, *ins0;
	int i, max_i=-1;
	char *ctmp;
	int candidates;
	FILE *tmp;
	char *ib;
	size_t len;
	bsdconv_counter_t *e;
	bsdconv_counter_t r=0;

	tmp=tmpfile();

	for(i=0;i<sizeof(codecs1)/sizeof(struct codec);i+=1){
		if(codecs1[i].evl == NULL){
			codecs1[i].up = 0;
			continue;
		}
		ins = codecs1[i].evl;
		bsdconv_counter_reset(ins, NULL);
		bsdconv_init(ins);
		codecs1[i].up = 1;
	}

	for(i=0;i<sizeof(codecs2)/sizeof(struct codec);i+=1){
		if(codecs2[i].evl == NULL){
			codecs2[i].up = 0;
			continue;
		}
		ins = codecs2[i].evl;
		bsdconv_counter_reset(ins, NULL);
		bsdconv_init(ins);
		codecs2[i].up = 1;
	}

	int rnd=0;
	int flush=0;
 	candidates = sizeof(codecs1)/sizeof(struct codec);
	while(candidates > 1 && !flush){
		rnd += 1;
		if(verbose){
			fprintf(stderr, "Encoding detection round %d\n================================\n", rnd);
		}
		flush = input(fi, tmp, &ib, &len, NULL);
		for(i = 0;i < sizeof(codecs1)/sizeof(struct codec);++i){
			if(codecs1[i].up!=1)
				continue;
			ins=codecs1[i].evl;
			bsdconv_counter_t *_ierr=bsdconv_counter(ins, "IERR");
			bsdconv_counter_t *_score=bsdconv_counter(ins, "SCORE");
			bsdconv_counter_t *_count=bsdconv_counter(ins, "COUNT");
			ins->input.data=ib;
			ins->input.flags=0;
			ins->input.next=NULL;
			ins->input.len=len;
			ins->flush=flush;
			ins->output_mode=BSDCONV_NULL;
			bsdconv(ins);
			double ierr=(double)(*_ierr);
			double score=(double)(*_score);
			double count=(double)(*_count);
			codecs1[i].wv=(score - ierr*(count*0.01))/count;
			if(verbose){
				fprintf(stderr, "%s: %.6lf\n", codecs1[i].name, codecs1[i].wv);
				fprintf(stderr, "\tIERR: %.0lf\n", ierr);
				fprintf(stderr, "\tSCORE: %.0lf\n", score);
				fprintf(stderr, "\tCOUNT: %.0lf\n", count);
				fprintf(stderr, "\n");
			}
		}
		for(i=0;i<sizeof(codecs1)/sizeof(struct codec);++i){
			if(max_i==-1){
				max_i=i;
				continue;
			}
			if(codecs1[i].up!=1)
				continue;
			if(codecs1[i].wv > codecs1[max_i].wv){
				codecs1[max_i].up=0;
				max_i=i;
				candidates-=1;
			}else if(codecs1[i].wv < codecs1[max_i].wv){
				codecs1[i].up=0;
				candidates-=1;
			}
		}
		if(tmp==NULL){
			fprintf(stderr, "WARNING: Encoding detection is early finished because of temporary file creation failure\n");
			break;
		}
	}

	if(verbose){
		fprintf(stderr, "Detected encoding: %s\n", codecs1[max_i].name);
	}

	if(!codecs1[max_i].ins){
		codecs1[max_i].ins=bsdconv_create(codecs1[max_i].conv);
	}
	ins0=codecs1[max_i].ins;

	int rwnd=1;
	rnd=0;
	flush=0;
 	candidates = sizeof(codecs2)/sizeof(struct codec);
	while(candidates > 1 && !flush){
		rnd += 1;
		if(verbose){
			fprintf(stderr, "Font detection round %d\n================================\n", rnd);
		}
		flush = input(fi, tmp, &ib, &len, &rwnd);
		for(i = 0;i < sizeof(codecs2)/sizeof(struct codec);++i){
			if(codecs2[i].up!=1)
				continue;
			ins=codecs2[i].evl;
			bsdconv_counter_t *_ierr=bsdconv_counter(ins, "IERR");
			ins->input.data=ib;
			ins->input.flags=0;
			ins->input.next=NULL;
			ins->input.len=len;
			ins->flush=flush;
			ins->output_mode=BSDCONV_NULL;
			bsdconv(ins);
			double ierr=(double)(*_ierr);
			codecs2[i].wv=-ierr;
			if(verbose){
				fprintf(stderr, "%s: %.6lf\n", codecs2[i].name, codecs2[i].wv);
				fprintf(stderr, "\tIERR: %.0lf\n", ierr);
				fprintf(stderr, "\n");
			}
		}
		for(i=0;i<sizeof(codecs2)/sizeof(struct codec);++i){
			if(max_i==-1){
				max_i=i;
				continue;
			}
			if(codecs2[i].up!=1)
				continue;
			if(codecs2[i].wv > codecs2[max_i].wv){
				codecs1[max_i].up=0;
				max_i=i;
				candidates-=1;
			}else if(codecs2[i].wv < codecs2[max_i].wv){
				codecs2[i].up=0;
				candidates-=1;
			}
		}
		if(tmp==NULL){
			fprintf(stderr, "WARNING: Font detection is early finished because of temporary file creation failure\n");
			break;
		}
	}

	if(verbose){
		fprintf(stderr, "Detected font: %s\n", codecs2[max_i].name);
	}

	if(codecs2[max_i].ins){
		ins=codecs2[max_i].ins;
	}else{
		conv=strdup(codecs2[max_i].conv);
		switch(linebreak){
			case 'w':
				ctmp=conv;
				conv=bsdconv_insert_phase(conv, "WIN", INTER, -1);
				bsdconv_free(ctmp);
				break;
			case 'm':
				ctmp=conv;
				conv=bsdconv_insert_phase(conv, "MAC", INTER, -1);
				bsdconv_free(ctmp);
				break;
			case 'x':
				ctmp=conv;
				conv=bsdconv_insert_phase(conv, "UNIX", INTER, -1);
				bsdconv_free(ctmp);
				break;
		}
		codecs2[max_i].ins=ins=bsdconv_create(conv);
		bsdconv_free(conv);
	}

	bsdconv_counter_reset(ins0, NULL);
	bsdconv_init(ins0);
	bsdconv_counter_reset(ins, NULL);
	bsdconv_init(ins);

	rwnd=1;
	do{
		flush = input(fi, tmp, &ib, &len, &rwnd);
		ins0->input.data=ib;
		ins0->input.len=len;
		ins0->input.flags=F_FREE;
		ins0->input.next=NULL;
		ins0->flush=flush;

		ins0->output_mode=BSDCONV_PASS;
		ins0->output.data=ins;

		bsdconv(ins0);

		ins->flush=flush;

		ins->output_mode=BSDCONV_FILE;
		ins->output.data=fo;
		bsdconv(ins);
	}while(flush==0);
	e=bsdconv_counter(ins0, "IERR");
	r+=*e;
	e=bsdconv_counter(ins, "OERR");
	r+=*e;

	return r;
}

static void usage(void){
	(void)fprintf(stderr,
		"usage: chiconv [-bug] [-i bufsiz]\n"
		"\t -i\tSave in-place if no error\n"
		"\t -f\tSave in-place regardless of errors (implies -i)\n"
		"\t -s\tBuffer size used for encoding2 detection, default=8192\n"
		"\t -v\tVerbose\n"
		"\t -w\tUse Windows linebreak\n"
		"\t -m\tUse Mac linebreak\n"
		"\t -x\tUse Unix linebreak\n"
	);
	finish(1);
}

static void finish(int r){
	int i;
	for(i=0;i<sizeof(codecs1)/sizeof(struct codec);++i){
		if(codecs1[i].evl)
			bsdconv_destroy(codecs1[i].evl);
		if(codecs1[i].ins)
			bsdconv_destroy(codecs1[i].ins);
	}
	for(i=0;i<sizeof(codecs2)/sizeof(struct codec);++i){
		if(codecs2[i].evl)
			bsdconv_destroy(codecs2[i].evl);
		if(codecs2[i].ins)
			bsdconv_destroy(codecs2[i].ins);
	}
	exit(r);
}
