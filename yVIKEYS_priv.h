/*============================----beg-of-source---============================*/



/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    <yLOG.h>               /* heatherly program logger                */
#include    <yURG.h>               /* heatherly program logger                */
#include    <ySTR.h>               /* heatherly program logger                */


/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */

/*---(posix standard)--------------------*/



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes


/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "0.5r"
#define YVIKEYS_VER_TXT   "moved all gyges repeat and macro in, and unit tested ;)"


/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */
/*---(string lengths)-----------------*/
#define     LEN_LABEL   20
#define     LEN_STR     200
#define     LEN_RECD    2000

typedef    struct    cSHARED    tSHARED;
struct cSHARED {
   /*---(repeating)-------*/
   char        repeat;         /* multiplier for normal keys                  */
   char        repeat_macro;   /* multiplier for macro execution              */
};
tSHARED     its;





typedef     struct timespec   tSPEC;
typedef     unsigned char     uchar;
typedef     signed char       schar;



extern char yVIKEYS__unit_answer [LEN_STR];
char*       yVIKEYS__macro_unit     (char *a_question, char a_macro);
char        yVIKEYS__unit_quiet     (void);
char        yVIKEYS__unit_loud      (void);
char        yVIKEYS__unit_end       (void);

/*---(program)--------------*/
char        yVIKEYS__macro_clear    (char a_macro);
char        yVIKEYS__macro_purge    (void);
char        yVIKEYS__macro_name     (char a_name);
char        yVIKEYS__macro_index    (char a_name);
/*---(storage)--------------*/
char        yVIKEYS__macro_save     (void);
char        yVIKEYS__macro_fetch    (char a_name);
/*---(record)---------------*/
char        yVIKEYS__macro_rec_beg  (char a_name);
char        yVIKEYS__macro_rec_str  (char *a_keys);
/*---(execute)--------------*/
char        yVIKEYS__macro_delay    (char a_delay);
char        yVIKEYS__macro_exec_ctl (char a_key);

#endif
/*============================----end-of-source---============================*/
