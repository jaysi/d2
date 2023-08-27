#include <ctype.h>
#include <stdio.h>
#include "lib13.h"
#include "token.h"
#include "d2.h"

#define dm_loc() fprintf(stderr, "loc: %s.%s().%i", __FILE__, __FUNC__, __LINE__)
#define dm(fmt, ...)	fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok1(fmt, ...) //fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok2(fmt, ...) //fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok3(fmt, ...) 

/*
	source code shape:
	block;
		exp;
		block;
			exp;
		label:
*/

struct d2_tok_form_s {

	char* form;
	int preced;

} d2_tok_form[] = {
					//EMPTY
					{"", 0},

					//100
					{"(", 100},
					{")", 100},
					{"[", 100},
					{"]", 100},

					//200
					{"++", 200},
					{"--", 200},
					{"!", 200},
					{"~", 200},

					//300
					{"*", 300},
					{"/", 300},
					{"%", 300},

					//400
					{"+", 400},
					{"-", 400},

					//500
					{"<<", 500},
					{">>", 500},

					//600
					{"<", 600},
					{"<=", 600},
					{">", 600},
					{">=", 600},

					//700
					{"==", 700},
					{"!=", 700},

					//800
					{"&", 800},

					//900
					{"^", 900},

					//1000
					{"|", 1000},

					//1100
					{"&&", 1100},

					//1200
					{"||", 1200},

					//1300
					{"?:", 1300},

					//1400
					{"=", 1400},
					{"+=", 1400},
					{"-=", 1400},
					{"*=", 1400},
					{"/=", 1400},
					{"%=", 1400},
					{"<<=", 1400},
					{">>=", 1400},
					{"&=", 1400},
					{"^=", 1400},
					{"|=", 1400},

					//1500
					{",", 1500},
					{";", 1500},

					//1600
					{"{", 1600},
					{"}", 1600},

					//access
					{"@", 1650},

					//1700 - flow control - c keywords
					{"if", 1700},
					{"else", 1700},
					{"else if", 1700},
					{"switch", 1700},
					{"case", 1700},
					{"default", 1700},
					{"do", 1700},
					{"while", 1700},
					{"break", 1700},
					{"continue", 1700},
					{"for", 1700},
					{"goto", 1700},
					{"return", 1700},

					//label
					{":", 1800},

					//scripting 
					{"thread", 1900},
					{"exit", 1900},
					{"sys", 1900},
					{"print", 1900},

					//operands

					//vars

					//fns

					//INVAL
					{NULL}
};

int __d2_tok_preced(d2_tok_enum code){
	return d2_tok_form[code].preced;
}

size_t __d2_estimate_ntokens(char* start,
						char delim[],
						char esc,
						char pack1,
						char pack2
						){
	char* end, *d = delim;
	char pack = 0;
	size_t cnt = 0UL;
	end = start;
	while(*end){
		dm_tok1("end is %c, cnt = %lu\n", *end, cnt);
		if(*end == esc) {dm_tok1("escape %c passed to %c\n", *(end+1),*(end+2));end+=2; continue;}
		if(!pack){
			d = delim;
			while(*d){
				if(*d == *end){
					if(*end == pack1 || *end == pack2){
						pack = *end;
						break;
					}
					cnt+=2;
				}
				if(pack) continue;
				d++;
			}
		} else { //if(!pack)
			if(*end == pack) cnt+=2;
		}
		end++;
	}
	return cnt;
}


char* __d2_token(   char* start,
					char delim[],
					char esc,
					char pack1,
					char pack2
				){
	char* end, *d = delim;
	char pack = 0;	
	end = start;    
	while(*end){		
		if(*end == esc) {end+=2; continue;}
			if(!pack){					
				d = delim;
				while(*d){
					if(*d == *end){
						if(*end == pack1 || *end == pack2){
							pack = *end;
							break;
						}
						//will take care of isspace() in the calling function
						//while(isspace(*end)) end++;//skip whitespaces
						return start==end?end:end-1;
					}
					if(pack) break;
					d++;
				}
			} else {
				if(*end == pack){
					return end;
				}
			}
			end++;
	}
	return end;
}

e13_t d2_lex(struct d2_tok* tok){
	d2_tok_enum tokenum;
	struct d2_tok_form_s* form;    
	//struct d2_tok* tok;
	char* endptr;

	for(tokenum = TOK_EMPTY, form = d2_tok_form; form->form; form++, tokenum++){		

		if(!strcmp(form->form, tok->rec.data)){
			tok->rec.code = tokenum;
			break;
		}

	}

	if(!form->form){

		//is digit? base10, hexa, scientific*
		tok->dval = strtold(tok->rec.data, &endptr);

		tok->rec.code = TOK_INVAL;
	}


	return E13_OK;
}

e13_t d2_combine(struct d2_tok* toklist_first){
		struct d2_tok* tok, *toktmp;
		size_t l;
		char c;
		int j;

		//1. resolve combo operators
		for(int i = 3; i > 1; i--){
			for(tok = toklist_first; tok; tok = tok->next){
				for(d2_tok_enum tok_enum= TOK_EMPTY; d2_tok_form[tok_enum].form; tok_enum++){
					if(strlen(d2_tok_form[tok_enum].form) == i){
						toktmp = tok;

						for(j = 0; j < i; j++){
							if(toktmp && toktmp->rec.data[0] == d2_tok_form[tok_enum].form[j]) toktmp = toktmp->next;
							else break;
						}

						if(j == i){//found a combination
							tok->rec.code = tok_enum;
							strcpy(tok->rec.data, d2_tok_form[tok_enum].form);//TODO: this should not lead to memmory corruption?
							toktmp = tok;
							for(j = 0; j < i; j++) toktmp = toktmp->next;
							tok->next = toktmp;//TODO: ->next?? aparently not!
							toktmp->prev = tok;//bypass extra tokens
						}

					}
				}
			}
		}//resolve combo operators ends

		//resolve else if
		for(tok = toklist_first; tok; tok = tok->next){
			if(tok->rec.code == TOK_ELSE && tok->next && tok->next->rec.code == TOK_IF){
				tok->rec.code = TOK_ELSE_IF;
				tok->next = tok->next->next;
				strcpy(tok->rec.data, d2_tok_form[TOK_ELSE_IF].form);
			}
		}

		//resolve label
		if( toklist_first &&
			toklist_first->rec.code == TOK_STRING &&
			toklist_first->next &&
			toklist_first->next->rec.code == TOK_LABEL &&
			!toklist_first->next->next
			){
				toklist_first->rec.code = TOK_LABEL;
				strcat(toklist_first->rec.data, toklist_first->next->rec.data);
				toklist_first->next = NULL;
		}

		//2. resolve scientific numbers
		for(tok = toklist_first; tok; tok = tok->next) {
			l = strlen(tok->rec.data)-1;
			c = tok->rec.data[l];
			if(c == 'e' || c == 'E'){
				tok->rec.data[l] = 0;
				if(strtold(tok->rec.data, NULL)){//TODO: check for RANGE errors
					tok->rec.data[l] = c;
					toktmp = tok->next;
					if(toktmp){
						if(toktmp->rec.code == TOK_NUMBER){
							tok->rec.code = TOK_NUMBER;
							strcat(tok->rec.data, toktmp->rec.data);
							tok->next = toktmp->next;
							if(toktmp->next){
								toktmp->next->prev = tok;
							}
							tok->dval = strtold(tok->rec.data, NULL);//TODO: check for range errors
						} else if(toktmp->rec.code == TOK_ADD || toktmp->rec.code == TOK_SUB){
							if((toktmp = toktmp->next)){
								if(toktmp->rec.code == TOK_NUMBER){
									tok->rec.code = TOK_NUMBER;
									sprintf(tok->rec.data, "%s%s%s", tok->rec.data, tok->next->rec.data, toktmp->rec.data);//TODO: ugly!
									tok->next = toktmp->next;
									if(toktmp->next){
										toktmp->next->prev = tok;
									}
								}
							}
						}
					}
				} else {//if strtold()
					tok->rec.data[l] = c;
				}//else (if strtold())
			}
		}
	return E13_OK;
}

e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first, size_t* ntok){
	
	struct d2_tok* toks, *tok, *toklist_last;
	char* bufdata;
	size_t bufdatasize;
	char* start, *end;
	size_t total;
	struct d2_tok_form_s* form;

	*ntok = __d2_estimate_ntokens(buf, d2_delimlist, d2_escape, d2_pack1, d2_pack2);

	dm_tok2("esttok = %lu\n", *ntok);

	//TODO: combine two malloc()s in one bigbuf : toks[]+bufdata

	toks = (struct d2_tok*)malloc((*ntok)*sizeof(struct d2_tok));
	if(!toks) return e13_error(E13_NOMEM);

	bufdatasize = strlen(buf) + (*ntok) + 1;
	bufdata = (char*)malloc(bufdatasize);
	if(!bufdata) return e13_error(E13_NOMEM);

	dm_tok2("bufsize = %lu\n", bufdatasize);

	//some init before the loop
	*toklist_first = NULL;
	start = buf;	
	*ntok = 0;
	total = 0;
	end = start;

//defines to speed up things
#define len (end - start + 1)
#define tokbuf (bufdata + total)

	while(*end){
		end = __d2_token(start, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		dm_tok2("start = %s, end = %s(%c)\n", start, end, end[0]);
		if(*end && !isspace(*end)){
			
			//init tok ptr
			tok = &toks[(*ntok)++];
			tok->next = NULL;
			tok->blockend = NULL;
			//tok->prev = NULL;//no need, will set later

			memcpy(tokbuf, start, len);

			tok->rec.data = tokbuf;//set before updating total

			dm_tok2("tokbuf = %s, len = %li, lentotal = %lu, tok = %p\n", tokbuf, len, total, (void*)tok);

			total += len;//update total
			*(tokbuf) = 0;//now total has been updated, terminate token buffer 
			total++;//add 1 for next round


			//phase b, lexical analysis 1 (2 is done via combine())

			form = d2_tok_form;
			for(d2_tok_enum tokenum = TOK_EMPTY; form->form; form++, tokenum++){

				dm_tok3("%s->%s\n", form->form, tok->rec.data);

				if(!strcmp(form->form, tok->rec.data)){
					tok->rec.code = tokenum;
					break;
				} else if((tok->dval = strtold(tok->rec.data, NULL))){
					tok->rec.code = TOK_NUMBER;
					break;
				}
			}

			if(!form->form){
				tok->rec.code = TOK_STRING;
			}

			dm_tok2("ntok = %li, data = %s, code = %i\n", *ntok, tok->rec.data, tok->rec.code);            

			//list assignment
			if(!(*toklist_first)){
				tok->prev = NULL;
				*toklist_first = tok;
				toklist_last = tok;
			} else {
				tok->prev = toklist_last;
				toklist_last->next = tok;
				toklist_last = tok;
			}
		}//if(*end && !isspace(*end))

		start = end + 1;//out of bounds
	}

#undef len
#undef tokbuf    

	return E13_OK;
}
