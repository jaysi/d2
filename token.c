#include "lib13.h"
#include "token.h"

#define TEST_TOKENIZE


/*
    source code shape:    
    block;
        exp;
        block;
            exp;
        label:
*/

char* d2_tok_form[] = { "", "(", ")", "[", "]", "++", "--", "!", "~", "*", "/", "%",
                        "<<", ">>", "<", "<=", ">", ">=", "==", "!=", "&", "^", "|",
                        "&&", "||", "?:", "=", "+=", "-=", "*=", "/=", "%=", "<<=",
                        ">>=", "&=", "^=", "|=", ",", ";", "{", "}", "@",
                        "if", "else", "else if", "switch", "case", "default",
                        "do", "while", "break", "continue",
                        "for", "goto", "return", ":", 
                        "thread", "exit",
                        "sys",
                        "print",
                        NULL};

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

e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first){
    
    struct d2_tok* toks, *tok;
    size_t esttok, ntok;
    char* bufdata;
    size_t bufdatasize;
    char* start, *end, *form;
	enum tok_enum;

    esttok = __d2_estimate_ntokens(buf, d2_delimlist, d2_escape, d2_pack1, d2_pack2);

    toks = (struct d2_tok*)malloc(esttok*sizeof struct d2_toks);
    if(!toks) return e13_error(E13_NOMEM);

    bufdatasize = strlen(buf) + esttok + 1;
    bufdata = (char*)malloc(bufdatasize);
    if(!bufdata) return e13_error(E13_NOMEM);
    
    tok = &toks[0];
	*toklist_first = tok;

    start = buf;
    end = start;

	while(*end){
		end = __d2_tokenize(start, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		if(*end && !isspace(*end)){
			ntok++;
			memcpy(bufdata + end - start, start, end-start+1);
			bufdata[end-start+1] = '\0';			
			start = end+1;//out of bounds

			tok->rec.data = bufdata + end - start;

			for(tok_enum = TOK_EMPTY, form = d2_tok_form[0]; form; form++, tok_enum++){
				if(!strcmp(form, tok->rec.data)){
					tok->rec.code = tok_enum;
					break;
				}
			}

		}
	}

    return E13_OK;
}

#ifdef TEST_TOKENIZE

int test_tokenize(){
	char exp[100], tk[20], *s, *e;
	printf("exp: ");
	gets(exp);
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
	printf("exp: %p, s: %p, e: %p, diff: %tp, sizeof(mem): %u\n", exp, s, e, (ptrdiff_t)(e-exp), __d2_estimate_ntokens(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2));
	return 0;
}

#endif