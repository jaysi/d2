#include <ctype.h>
#include <stdio.h>
#include "lib13.h"
#include "token.h"

#define dm_tok2(fmt, ...) fprintf (stderr,fmt, __VA_ARGS__)

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


} d2_tok_form[] = { {""}, {"("}, {")"}, {"["}, {"]"}, {"++"}, {"--"}, {"!"}, {"~"}, {"*"}, {"/"}, {"%"},
						{"<<"}, {">>"}, {"<"}, {"<="}, {">"}, {">="}, {"=="}, {"!="}, {"&"}, {"^"}, {"|"},
						{"&&"}, {"||"}, {"?:"}, {"="}, {"+="}, {"-="}, {"*="}, {"/="}, {"%="}, {"<<="},
						{">>="}, {"&="}, {"^="}, {"|="}, {"},"}, {";"}, {"{"}, {"}"}, {"@"},
						{"if"}, {"else"}, {"else if"}, {"switch"}, {"case"}, {"default"},
						{"do"}, {"while"}, {"break"}, {"continue"},
						{"for"}, {"goto"}, {"return"}, {":"}, 
						{"thread"}, {"exit"},
						{"sys"},
						{"print"},
						{NULL} };

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

/*    
	phase 1:
		preprocess
	phase 2:
		split buffer to functions, look for ; or :
	phase 3:
		split a function to expressions
	phase 4:
		tokenize
	phase 5:
		lex
	phase 6:
		parse 
*/

e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first, size_t* ntok){
	
	struct d2_tok* toks, *tok, *toklist_last;	
	char* bufdata;
	size_t bufdatasize;
	char* start, *end;
    size_t len, lentotal;
	d2_tok_enum tokenum;
	struct d2_tok_form_s* form;

	*ntok = __d2_estimate_ntokens(buf, d2_delimlist, d2_escape, d2_pack1, d2_pack2);

    dm_tok2("esttok = %i\n", *ntok);

	toks = (struct d2_tok*)malloc((*ntok)*sizeof(struct d2_tok));
	if(!toks) return e13_error(E13_NOMEM);

	bufdatasize = strlen(buf) + (*ntok) + 1;
	bufdata = (char*)malloc(bufdatasize);
	if(!bufdata) return e13_error(E13_NOMEM);

    dm_tok2("bufsize = %i\n", bufdatasize);
		
	*toklist_first = NULL;
	start = buf;
	end = start;
    *ntok = 0;
    lentotal = 0;

	while(*end){
		end = __d2_token(start, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
        dm_tok2("end = %s(%c)\n", end, end[0]);
		if(*end && !isspace(*end)){

            len = end - start + 1;
            tok = &toks[(*ntok)++];
			memcpy(bufdata + lentotal, start, len);
            lentotal += len;
            dm_tok2("start = %s, len = %i, lentotal = %i, tok = %p\n", start, len, lentotal, tok);
			bufdata[lentotal++] = '\0';

			tok->rec.data = bufdata + lentotal;

			//phase 1, try to resolve tokens with sym table
			for(tokenum = TOK_EMPTY, form = d2_tok_form; form->form; form++, tokenum++){
				if(!strcmp(form->form, tok->rec.data)){
					tok->rec.code = tokenum;
					break;
				}
			}            
			if(!form->form){
				tok->rec.code = TOK_INVAL;
			}
            dm_tok2("ntok = %i, data = %s, code = %i\n", ntok, tok->rec.data, tok->rec.code);
            tok->next = NULL;

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
        start = end+1;//out of bounds
	}

	return E13_OK;
}

#ifdef TEST_TOKENIZE

int test_tokenize(){
	char exp[100];
    struct d2_tok* toklist_first, *tok;
    e13_t err;
    size_t ntok;
	printf("exp: ");
	fgets(exp, 100, stdin);
    err = d2_tokenize(exp, strlen(exp), &toklist_first, &ntok);

    if(err == E13_OK){
        for(tok = toklist_first; tok, ntok--; tok = tok->next){
            printf("%s -- %i\n", tok->rec.data, tok->rec.code);
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
		//printf("start= %s\n", s);
		e = __d2_token(s, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		//printf("end= %s\n", e);
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