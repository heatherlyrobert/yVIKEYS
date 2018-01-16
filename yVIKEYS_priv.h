/*============================----beg-of-source---============================*/



/*---(headers)---------------------------*/
#include    <yLOG.h>               /* heatherly program logger                */
#include    <yURG.h>               /* heatherly program logger                */
#include    <ySTR.h>               /* heatherly program logger                */
#include    <yFONT.h>              /* heatherly program logger                */


/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */

/*---(posix standard)--------------------*/
#include   <GL/gl.h>              /* opengl standard primary header           */
#include   <GL/glx.h>             /* opengl standard X11 integration          */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes


/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "0.6a"
#define YVIKEYS_VER_TXT   "build early map movements based on a map/grid, some unit testing"


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
   /*---(debugging)-------*/
   int         logger;
   /*---(repeating)-------*/
   char        repeat;         /* multiplier for normal keys                  */
   char        repeat_macro;   /* multiplier for macro execution              */
};
tSHARED     its;





typedef     struct timespec   tSPEC;
typedef     unsigned char     uchar;
typedef     signed char       schar;



extern char yVIKEYS__unit_answer [LEN_STR];
char        yVIKEYS__unit_quiet     (void);
char        yVIKEYS__unit_loud      (void);
char        yVIKEYS__unit_end       (void);
char*       yVIKEYS__mode_unit      (char *a_question);
char*       yVIKEYS__macro_unit     (char *a_question, char a_macro);
char*       yVIKEYS__cmds_unit      (char *a_question, char a_index);
char*       yVIKEYS__srch_unit      (char *a_question, char a_index);

char        yVIKEYS__map_load       (char a_style, tMAPPED *a_map);
char        yVIKEYS__map_move       (int *a_index, int *a_grid, int a_target, tMAPPED *a_map);

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


char        yVIKEYS__cmds_load      (char *a_command);
char        yVIKEYS__cmds_test      (char a_mode, char a_value);

char        yVIKEYS__srch_purge     (void);


#define    ACTION_FIND     'f'
#define    ACTION_ADD      '+'
#define    ACTION_DEL      '-'




#endif
/*============================----end-of-source---============================*/
