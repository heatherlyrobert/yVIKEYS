/*============================----beg-of-source---============================*/



/*---(headers)---------------------------*/
#include    <yX11.h>               /* heatherly program logger                */
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
#define YVIKEYS_VER_NUM   "0.6j"
#define YVIKEYS_VER_TXT   "layout command now preserves main drawing area cooordinations"


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



extern char        g_coord;
extern int         g_goffx;
extern int         g_gsizex;
extern int         g_goffy;
extern int         g_gsizey;
extern int         g_goffz;
extern int         g_gsizez;



typedef     struct timespec   tSPEC;
typedef     unsigned char     uchar;
typedef     signed char       schar;


/*---(normal)-------------------------*/
#define      W_TITLE       't'
#define      W_BUFFER      'b'
#define      W_FORMULA     'f'
#define      W_NAV         'n'
#define      W_MAIN        'm'
#define      W_PROGRESS    'p'
#define      W_STATUS      's'
#define      W_COMMAND     'c'
#define      W_DETAILS     'd'
#define      W_RIBBON      'r'
#define      W_VERSION     'v'
#define      W_KEYS        'k'
/*---(specialty)----------------------*/
#define      W_WINDOW      'W'
#define      W_GRID        'G'
#define      W_COORDS      'M'



char        yVIKEYS__view_find      (char *a_name);
char        yVIKEYS__view_abbr      (char  a_abbr);
char        yVIKEYS__view_resize      (cchar a_type);
char        yVIKEYS__view_size_clear (void);
char        yVIKEYS__view_widths     (cint a_wide);
char        yVIKEYS__view_heights    (cint a_tall);
char*       yVIKEYS__view_unit      (char *a_question, char a_index);
char        yVIKEYS_view_text         (cchar *a_text);
char        yVIKEYS_view_keys         (cchar *a_text);

extern char yVIKEYS__unit_answer [LEN_STR];
char        yVIKEYS__unit_quiet     (void);
char        yVIKEYS__unit_loud      (void);
char        yVIKEYS__unit_end       (void);
char*       yVIKEYS__mode_unit      (char *a_question);
char*       yVIKEYS__macro_unit     (char *a_question, char a_macro);
char*       yVIKEYS__cmds_unit      (char *a_question, char a_index);
char*       yVIKEYS__srch_unit      (char *a_question, char a_index);

char        yVIKEYS__map_load       (char a_style , tMAPPED *a_map);
char        yVIKEYS__map_move       (int  a_target, tMAPPED *a_map);
char*       yVIKEYS__map_unit       (char *a_question, char a_index);
char        yVIKEYS__map_screen     (tMAPPED *a_map);
char        yVIKEYS__unit_rowmap    (void);
char        yVIKEYS__unit_colmap    (void);

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
