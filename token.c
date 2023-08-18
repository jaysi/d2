#include <ctype.h>
#include <stdio.h>
#include "lib13.h"
#include "token.h"
#include "d2.h"

#define dm(fmt, ...)	fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok2(fmt, ...)
#define dm_tok3(fmt, ...)

#define TEST_TOKENIZE


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

} d2_tok_form[] = {
					//EMPTY
					{""},

					//100
					{"("},
					{")"},
					{"["},
					{"]"},

					//200
					{"++"},
					{"--"},
					{"!"},
					{"~"},

					//300
					{"*"},
					{"/"},
					{"%"},

					//400
					{"+"},
					{"-"},

					//500
					{"<<"},
					{">>"},

					//600
					{"<"},
					{"<="},
					{">"},
					{">="},

					//700
					{"=="},
					{"!="},

					//800
					{"&"},

					//900
					{"^"},

					//1000
					{"|"},

					//1100
					{"&&"},

					//1200
					{"||"},

					//1300
					{"?:"},

					//1400
					{"="},
					{"+="},
					{"-="},
					{"*="},
					{"/="},
					{"%="},
					{"<<="},
					{">>="},
					{"&="},
					{"^="},
					{"|="},

					//1500
					{","},
					{";"},

					//1600
					{"{"},
					{"}"},

					//access
					{"@"},

					//1700 - flow control - c keywords
					{"if"},
					{"else"},
					{"else if"},
					{"switch"},
					{"case"},
					{"default"},
					{"do"},
					{"while"},
					{"break"},
					{"continue"},
					{"for"},
					{"goto"},
					{"return"},

					//label
					{":"},

					//scripting 
					{"thread"},
					{"exit"},
					{"sys"},
					{"print"},

					//operands

					//vars

					//fns

					//INVAL
					{NULL}
};

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
				if(*end == esc) {end+=2; continue;}                
				if(!pack){                        
						d = delim;
						while(*d){
								if(*d == *end){
										if(*end == pack1 || *end == pack2){
												pack = *end;
												break;
										}
										cnt++;
								}
								if(pack) continue;
								d++;
						}
				} else { //if(!pack)
					if(*end == pack) cnt++;
				}
				end++;
		}
		return ++cnt;
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
        if( toklist_first->rec.code == TOK_STRING &&
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

	dm_tok2("esttok = %i\n", *ntok);

	//TODO: combine two malloc()s in one bigbuf : toks[]+bufdata

	toks = (struct d2_tok*)malloc((*ntok)*sizeof(struct d2_tok));
	if(!toks) return e13_error(E13_NOMEM);

	bufdatasize = strlen(buf) + (*ntok) + 1;
	bufdata = (char*)malloc(bufdatasize);
	if(!bufdata) return e13_error(E13_NOMEM);

	dm_tok2("bufsize = %i\n", bufdatasize);

	//some init before the loop
	*toklist_first = NULL;
	start = buf;	
	*ntok = 0;
	total = 0;

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

			dm_tok2("tokbuf = %s, len = %i, lentotal = %i, tok = %p\n", tokbuf, len, total, tok);

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

struct d2_tok* d2_blockize(struct d2_tok* first){
    struct d2_tok* tok = first;
    
    while(tok){
        switch(tok->rec.code){
            case TOK_BLOCK_OPEN:
                tok = d2_blockize(tok->next);
                break;
            case TOK_BLOCK_CLOSE:
                first->blockend = tok->prev;
                return tok->next;
                break;
            default:
                tok = tok->next;
                break;
        }
    }
    return NULL;
}

#ifdef TEST_TOKENIZE

int test_tokenize(){
	char exp[100];
	struct d2_tok* toklist_first, *tok, *toklist_last;
	e13_t err;
	size_t ntok;
	printf("exp: ");
	fgets(exp, 100, stdin);

	//phase a and b
	err = d2_tokenize(exp, strlen(exp), &toklist_first, &ntok);    

	if(err == E13_OK){
		
		//phase c and b1
		d2_combine(toklist_first);

        d2_blockize(toklist_first);

		for(tok = toklist_first; tok; tok = tok->next){
			printf("%s -- %i", tok->rec.data, tok->rec.code);
            tok->blockend?printf("{%s..%s}\n", tok->rec.data, tok->blockend->rec.data):printf("\n");
			if(!tok->next) toklist_last = tok;
		}
	}

	return 0;
}

int test_tokenize2(){
	char exp[100], tk[20], *s, *e;
	printf("exp: ");
	fgets(exp, 100, stdin);
	s = exp;
	e = s;
	while(*e){
		e = __d2_token(s, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		if(!isspace(*e) && *e){
			memcpy(tk, s, e-s+1);
			tk[e-s+1] = '\0';
			printf("-> (%i) %s\n", (int)tk[0],tk);			
		}//if(*e)
		s = e+1;//out of bounds
	}
	//printf("exp: %p, s: %p, e: %p, diff: %tp, sizeof(mem): %u\n", exp, s, e, (ptrdiff_t)(e-exp), __d2_estimate_ntokens(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2));
	return 0;
}


#endif//TEST_TOKENIZE