#ifndef D2_KEYWORD_H
#define D2_KEYWORD_H

#include "d2.h"

struct d2_else_s {
	struct d2_exp *exp_first;
} struct d2_elseif_s {
	struct d2_exp *cond_exp;
	struct d2_exp *exp_first;
	struct d2_elseif_s *next;
} struct d2_if_s {
	struct d2_exp *cond_exp;
	struct d2_exp *exp_first;
	struct d2_else_s *_else;
	struct d2_elseif *_elseif_first;
} struct d2_case_s {
	struct d2_exp *cond_exp;
	struct d2_exp *exp_first;
	int has_break;		//has break?
	struct d2_case_s *next;
} struct d2_default_s {
	struct d2_exp *exp_first;
	int has_break;		//has break?    
} struct d2_switch_s {
	struct d2_exp *cond_exp;
	struct d2_case_s *_case;
	struct d2_default_s *_default;
} struct d2_for_s {
	struct d2_exp *init_cond_exp_first, *end_cond_exp_first,
	    *resume_exp_first;
	struct d2_exp *exp_first;

} struct d2_while_s {

} struct d2_do_s {
	struct d2_while_s *_while;
}
#endif				//D2_KEYWORD_H
