#ifndef D2_MSG_H
#define D2_MSG_H

#ifndef QT
#define PARAEND "\n"
#else
#define PARAEND ""
#endif

#ifdef WIN32
#define EOL	"\n\r"
#else
#define EOL	"\n"
#endif

#define TODO_PROMPT() fprintf(stderr, "TODO: %s-%s\n", __FILENAME__, __func__)

#define NOTSET "N/A"
#define ANSDESC "Last answer"

#define ICON_SIZE       4
#define ICON_ERR        "[#] "
#define ICON_WARN       "[!] "
#define ICON_INFO       "[i] "
#define ICON_ANS        "[=] "
#define ICON_RESULT     "[:] "
#define ICON_HELPBLOCK  "--] "
#define ICON_INFOBLOCK  "[{] "
#define ICON_HELPINFO   "- [ "
#define ICON_NOP        "[   "
#define ICON_QUESTION	"[?] "

#define COMMENT_SIGN "#"
#define COMMENT_CHAR '#'
#define COMMENT_SIGN_SIZE 1

#define MSG_MOREARGS    "Need more arguments [%s]."PARAEND
#define MSG_BADARG      "Bad argument type [%s]."PARAEND
#define MSG_NOMEM       "Out of memmory."PARAEND
#define MSG_EXISTS      "Already exists [%s]."PARAEND
#define MSG_NOTFOUND    "Not found [%s]."PARAEND
#define MSG_ANOTFOUND   "Array Not found [%s[%i][%i]]."PARAEND
#define MSG_FORBID      "Operation not permitted [%s]."PARAEND
#define MSG_BADCALC     "Calculation error."PARAEND
#define MSG_BADLOGIC    "Logic error."PARAEND
#define MSG_BADLOGICOM  "Combination error '%s %s'"PARAEND
#define MSG_BADEXP      "Expression error [%s]."PARAEND
#define MSG_IMPLEMENT   "Not implemented on your platform [%s]."PARAEND
#define MSG_FORMAT      "Bad format [%s]."PARAEND
#define MSG_READY       " > "
#define MSG_NOOPEN      "Could not open file [%s]."PARAEND
#define MSG_NOREAD      "Could not read from file [%s]."PARAEND
#define MSG_ERROR       "Error [%s]."PARAEND
#define MSG_OK          "Ok."PARAEND
#define MSG_EMPTY       "Empty resource [%s]."PARAEND
#define MSG_FAIL        "Failed to complete operation [%s]."PARAEND
#define MSG_BUSY        "Resource is busy [%s]."PARAEND
#define MSG_RANGE       "Out of range [%s]."PARAEND
#define MSG_BADUSE      "Wrong usage [%s]."PARAEND
#define MSG_OVERFLOW    "Overflow [%s]."PARAEND
#define MSG_SYNTAX2     "Syntax error near '%s %s'."PARAEND
#define MSG_MATCH       "Not found a match near '%s %s'."PARAEND
#define MSG_PARSE2      "Parse error near '%s %s'."PARAEND
#define MSG_CHILDPROC   "Child Process"
#define MSG_BADLOCALE   "Bad locale."PARAEND
#define MSG_SIZEMIS     "Size mismatch [%s->%i != %s->%i]."PARAEND
#define MSG_SSTAT       "State saved to [ %s ]."PARAEND
#define MSG_CIRCULAR    "Circular refrence %s[%i][%i] & %s[%i][%i]."PARAEND
#define MSG_NOLOCKFILE  "Could not create lock file."PARAEND
#define MSG_LOCALLOCK   "Another instance is running, Continue?"

//ABOUT MSGS
#define MSG_ABOUT_HELP      HELPINFO_ICON"\n[ Double, the programmable command line calculator. ]"\
							PARAEND

#define MSG_ABOUT_VER       NOP_ICON"[ engine version: "SRCVER", ui version: "UIVER" ]"\
							PARAEND

#define MSG_ABOUT_NOTES NOP_ICON"* The purpose was, creating a program to make some engineering\n"\
						"  tasks ( which may need some simple scripting and automation )\n"\
						"  easier, it helped me lots while writing invioces; I hope it\n"\
						"  becomes useful for others with similar needs.\n"\
						"* Try \"help item\" for a quick help on item.\n"\
						"* Be careful using EXPERIMENTAL features, they are\n"\
						"  still under developement and better to be avoided\n"\
						"  while being tested, see quick help of the functions."\
						PARAEND

#define MSG_ABOUT_ME     HELPINFO_ICON"[ by:    %s                        ]\n"\
						 "[ email: %s                    ]\n"\
						 "[ web:   %s ]"\
						 PARAEND

#define MSG_ABOUT_LIC   NOP_ICON"* The current version of this software is free for\n"\
						"  commercial and non-commercial use,\n"\
						"  this may change in future versions, however!"\
						PARAEND

#define NOVAREXP ""

#endif				//D2_MSG_H
