/*============================----beg-of-source---============================*/



/*---(headers)---------------------------*/
#include    <yX11.h>               /* heatherly program logger                */
#include    <yLOG.h>               /* heatherly program logger                */
#include    <yURG.h>               /* heatherly program logger                */
#include    <ySTR.h>               /* heatherly program logger                */
#include    <yFONT.h>              /* heatherly program logger                */
#include    <yCOLOR.h>             /* heatherly color library                 */


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
#define YVIKEYS_VER_NUM   "0.6m"
#define YVIKEYS_VER_TXT   "cleaned up cmds and view function names, and overlay added"


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
   /*---(quit)---------------------------*/
   char        done;                        /* flag indicating ready to quit  */
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





char        VIEW__reset             (void);
char        VIEW__find              (cchar *a_name);
char        VIEW__abbr              (cchar  a_abbr);

char        VIEW__widths            (cint a_wide, cint a_alt);
char        VIEW__heights           (cint a_tall);
char        VIEW__resize            (cchar a_type);
char        VIEW__grid_offset       (int a_x, int a_y, int a_z);
char        VIEW__grid_size         (int a_x, int a_y, int a_z);

char*       VIEW__unit              (char *a_question, char a_index);



extern char yVIKEYS__unit_answer [LEN_STR];
char        yVIKEYS__unit_quiet     (void);
char        yVIKEYS__unit_loud      (void);
char        yVIKEYS__unit_end       (void);
char*       yVIKEYS__mode_unit      (char *a_question);
char*       yVIKEYS__macro_unit     (char *a_question, char a_macro);

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
/*---(commands)-------------*/
char        CMDS_init               (void);
char        CMDS__load              (char *a_command);
char        CMDS__test              (char a_mode, char a_value);
char*       CMDS__unit              (char *a_question, char a_index);
char        CMDS_start              (void);
int         CMDS__find              (char *a_name);
char        CMDS__clear             (void);
char*       CMDS_curr               (void);
char        CMDS__exec              (void);
char        CMDS_mode               (char a_major, char a_minor);
/*---(search)---------------*/
char        SRCH__purge             (void);
char        SRCH_start              (void);
char        SRCH__clear             (void);
char*       SRCH_curr               (void);
char        SRCH__exec              (void);
char        SRCH_mode               (char a_major, char a_minor);
char*       SRCH__unit              (char *a_question, char a_index);


#define    ACTION_FIND     'f'
#define    ACTION_ADD      '+'
#define    ACTION_DEL      '-'




#endif
/*============================----end-of-source---============================*/
