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
#include    <ncurses.h>      /* CURSES : mvprintw, refresh, getch, ...        */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes


/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "0.7m"
#define YVIKEYS_VER_TXT   "added gyges replace and input modes, plus unit tests"


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
   /*---(global flags-----*/
   char        done;                        /* flag indicating ready to quit  */
   char        trouble;                     /* flag indicating error        0 */
};
tSHARED     myVIKEYS;

/*---(mode)------------*/
#define     MACRO_OFF          '-'      /* normal keyboard input              */
#define     MACRO_RUN          'M'      /* macro running with redisplay       */
#define     MACRO_DELAY        'D'      /* macro delay playback controls      */
#define     MACRO_PLAYBACK     'P'      /* macro under playback controls      */
#define     MACRO_RECORD       'r'      /* macro recording                    */
/*---(conditions)------*/
#define     IF_MACRO_OFF         if (MACRO_get_mode () == MACRO_OFF      ) 
#define     IF_MACRO_RUN         if (MACRO_get_mode () == MACRO_RUN      ) 
#define     IF_MACRO_NOT_RUN     if (MACRO_get_mode () != MACRO_RUN      ) 
#define     IF_MACRO_DELAY       if (MACRO_get_mode () == MACRO_DELAY    ) 
#define     IF_MACRO_PLAYBACK    if (MACRO_get_mode () == MACRO_PLAYBACK ) 
#define     IF_MACRO_MOVING      if (MACRO_get_mode () == MACRO_RUN      || MACRO_get_mode () == MACRO_DELAY   ) 
#define     IF_MACRO_NOT_PLAYING if (MACRO_get_mode () == MACRO_OFF      || MACRO_get_mode () == MACRO_RECORD  )
#define     IF_MACRO_PLAYING     if (MACRO_get_mode () != MACRO_OFF      && MACRO_get_mode () != MACRO_RECORD  )
#define     IF_MACRO_RECORDING   if (MACRO_get_mode () == MACRO_RECORD   ) 
#define     IF_MACRO_ON          if (MACRO_get_mode () != MACRO_OFF      ) 
/*---(setting)---------*/
#define     SET_MACRO_OFF        MACRO_set_mode (MACRO_OFF);
#define     SET_MACRO_RUN        MACRO_set_mode (MACRO_RUN);
#define     SET_MACRO_PLAYBACK   MACRO_set_mode (MACRO_PLAYBACK);
#define     SET_MACRO_DELAY      MACRO_set_mode (MACRO_DELAY);
#define     SET_MACRO_RECORD     MACRO_set_mode (MACRO_RECORD);


extern char        g_coord;
extern int         g_goffx;
extern int         g_gsizex;
extern int         g_goffy;
extern int         g_gsizey;
extern int         g_goffz;
extern int         g_gsizez;


extern char  g_vsimple   [LEN_DESC ];
extern char  g_vgoto     [LEN_DESC ];
extern char  g_vscroll   [LEN_DESC ];
extern char  g_hsimple   [LEN_DESC ];
extern char  g_hgoto     [LEN_DESC ];
extern char  g_hscroll   [LEN_DESC ];
extern char  g_hword     [LEN_DESC ];
extern char  g_multimap  [LEN_DESC ];
extern char  g_multisrc  [LEN_DESC ];
extern char  g_repeat    [LEN_DESC ];


typedef     struct timespec   tSPEC;
typedef     unsigned char     uchar;
typedef     signed char       schar;





char        VIEW__reset             (void);
char        VIEW__find              (cchar *a_name);
char        VIEW__abbr              (cchar  a_abbr);
char        VIEW_wrap                 (void);

char        VIEW__widths            (cint a_wide, cint a_alt);
char        VIEW__heights           (cint a_tall);
char        VIEW__resize            (cchar a_type);
char        VIEW__grid_offset       (int a_x, int a_y, int a_z);
char        VIEW__grid_size         (int a_x, int a_y, int a_z);

char*       VIEW__unit              (char *a_question, char a_index);



extern char yVIKEYS__unit_answer [LEN_STR];
char        BASE_key_status         (char *a_msg);
char        BASE__unit_quiet        (void);
char        BASE__unit_loud         (void);
char        BASE__unit_end          (void);
uchar       BASE__main_string       (uchar *a_keys);




/*---(mode stack)-----------*/
char        MODE_init                 (void);
char        MODE_enter                (char  a_mode);
char        MODE_exit                 (void);
char        MODE_curr                 (void);
char        MODE_prev                 (void);
char        MODE_not                  (char  a_mode);
char        MODE_status               (char *a_list);
char        MODE_message              (char *a_mesg, char *a_cmd);
char*       MODE__unit                (char *a_question);
/*---(repeat)---------------*/
char        REPEAT_reset            (void);
char        REPEAT_umode            (uchar a_major, uchar a_minor);
char        REPEAT_normal           (void);
char        REPEAT_decrement        (void);
int         REPEAT_count            (void);
int         REPEAT_use              (void);

char        MAP_init                (void);
char        MAP__load               (char a_style, tMAPPED *a_map, char a_which);
char        MAP__screen             (tMAPPED *a_map);
char        MAP__move               (int  a_target, tMAPPED *a_map);
char        MAP_mode                (char a_major, char a_minor);
char        MAP_xstatus             (char *a_list);
char        MAP_ystatus             (char *a_list);
char        MAP_status              (char *a_list);
char*       MAP__unit               (char *a_question, char a_index);
char        MAP__unit_ymap          (void);
char        MAP__unit_xmap          (void);

char        SRC_init                (void);
char        SRC_formula             (void);
char        SRC_select_status       (char *a_list);
char*       SRC__unit               (char *a_question, char a_reg);
char        SRC_words_status        (char *a_list);
char        TREG_init               (void);
char        TREG_smode              (int a_major, int a_minor);
char        TREG_status             (char *a_list);
char        REPL_smode              (int a_major, int a_minor);
char        INPT_mode               (int  a_major, int  a_minor);


/*---(program)--------------*/
char        MACRO_init              (void);
char        MACRO_zero              (void);
char        MACRO_count             (void);
char        MACRO__clear            (char a_macro);
char        MACRO__purge            (void);
char        MACRO_reset             (void);
char        MACRO__name             (char a_name);
char        MACRO__index            (char a_name);
/*---(storage)--------------*/
char        MACRO__save             (void);
char        MACRO__fetch            (char a_name);
/*---(record)---------------*/
char        MACRO__rec_beg          (char a_name);
char        MACRO_rec_key           (char a_key);
char        MACRO__rec_str          (char *a_keys);
char        MACRO_rec_end           (void);
char        MACRO_define            (char *a_string);
char*       MACRO__unit             (char *a_question, char a_macro);
/*---(execute)--------------*/
char        MACRO__delay            (char a_delay);
char        MACRO__exec_control     (char a_key);
char        MACRO_exec_beg          (char a_name);
char        MACRO_exec_adv          (void);
char        MACRO_exec_wait         (void);
char        MACRO_exec_key          (void);
char        MACRO_exec_player       (char a_key);
char        MACRO_smode             (char a_major, char a_minor);
char        MACRO_get_mode          ();
char        MACRO_set_mode          (char a_mode);

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
char        SRCH_init               (void);
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
