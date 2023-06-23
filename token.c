#include "lib13.h"
#include "token.h"

char* d2_tok_form[] = { "", "(", ")", "[", "]", "++", "--", "!", "~", "*", "/", "%",
                        "<<", ">>", "<", "<=", ">", ">=", "==", "!=", "&", "^", "|",
                        "&&", "||", "?:", "=", "+=", "-=", "*=", "/=", "%=", "<<=",
                        ">>=", "&=", "^=", "|=", ",", NULL};

char* __d2_token(char* start, char delim[], char esc, char pack1, char pack2){
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
										return start==end?end:end-1;
								}
								if(pack) continue;
								d++;
						}
				} else { //if(!pack)
					if(*end == pack) return end;
				}
				end++;
		}
		return end;
}

size_t __d2_count_token(char* start, char delim[], char esc, char pack1, char pack2){
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
        split an entry to expressions, look for ; or :
    phase 2:
        tokenize
    phase 3:
        lex
    phase 4:
        parse 
*/

e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first){
    
    *toklist_first = (struct d2_tok*)malloc(sizeof struct d2_tok);
    if(!(*toklist_first)) return e13_error(E13_NOMEM);
    
        

    return E13_OK;
}

#ifdef TEST_TOKEN

int test_tokenize(){
	char exp[100], tk[20], *s, *e;
	printf("exp: ");
	scanf("%s", exp);
	s = exp;
	e = s;
	while(*e){
		e = __d2_tokenize(s, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		if(*e){
			memcpy(tk, s, e-s+1);
			tk[e-s+1] = '\0';
			printf("-> (%i) %s\n", (int)tk[0],tk);
			s = e+1;//out of bounds
		}
	}
	printf("exp: %p, s: %p, e: %p, diff: %tp, sizeof(mem): %u\n", exp, s, e, (ptrdiff_t)(e-exp), __d2_count_token(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2));
	return 0;
}

#endif