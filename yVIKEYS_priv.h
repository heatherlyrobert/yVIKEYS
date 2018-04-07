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
#include    <sys/stat.h>          /* clibc  standard file handling            */
#include    <unistd.h>            /* clibc  linux/unix standard environment   */
#include    <time.h>              /* clibc  time related functions            */

/*---(posix standard)--------------------*/
#include    <GL/gl.h>             /* opengl standard primary header           */
#include    <GL/glx.h>            /* opengl standard X11 integration          */
#include    <ncurses.h>           /* CURSES mvprintw, refresh, getch, ...     */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes


/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "0.9m"
#define YVIKEYS_VER_TXT   "selection for x/col, y/row, and all xy working"


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

#define     G_STATUS_NULL        -1

#define     G_STATUS_INIT         6

#define     G_STATUS_CONF         8



#define     G_STAGE_NULL         -1
#define     G_STAGE_INIT          0
#define     G_STAGE_CONF          1

#define     G_STAGE_READY         5

#define     G_STAGE_OPER         10




typedef    struct    cSHARED    tSHARED;
struct cSHARED {
   /*---(debugging)-------*/
   int         logger;
   /*---(global flags-----*/
   char        done;                        /* flag indicating ready to quit  */
   char        trouble;                     /* flag indicating error        0 */
   char        redraw;
   char        repeating;                   /* note for repeating actions     */
   /*---(marks)-----------*/
   char        mark_show;      /* show temporary marks (y/n)                    */
   char        info_win;
   /*---(font)------------*/
   char        env;                         /* opengl vs ncurses              */
   int         font;                        /* yFONT font identifier          */
   int         point;                       /* yFONT font point to use        */
   float       font_scale;                  /* width of char in opengl coords */
   /*---(file hanndling)--*/
   char        f_loc       [LEN_RECD];      /* specific file location         */
   char        f_name      [LEN_RECD];      /* full file name                 */
   char        f_title     [LEN_RECD];      /* specific file base name        */
   int         f_lines;                     /* file line number               */
   char        f_recd      [LEN_RECD];      /* current file record            */
   char        f_type      [LEN_RECD];      /* current record verb            */
   char        f_vers;                      /* current record version         */
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


typedef     struct stat       tSTAT;
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


char        KEYS_status             (char *a_msg);
char        KEYS__logger            (uchar a_key);
char        KEYS_unique             (void);
char        KEYS_init               (void);
char*       KEYS__unit              (char *a_question, char a_index);
char        BASE_dump               (char *a_what);
char        KEYS_dump               (FILE *a_file);
char        BASE__unit_quiet        (void);
char        BASE__unit_loud         (void);
char        BASE__unit_end          (void);




/*---(status system)--------*/
char        STATUS_init             (void);
char        STATUS_wrap             (void);
char        STATUS_check_prep       (char a_abbr);
char        STATUS_check_needs      (char a_abbr);
char        STATUS_init_set         (char a_abbr);
char        STATUS_conf_set         (char a_abbr, char a_step);
char        STATUS_operational      (char a_abbr);
char        STATUS_dump             (FILE *a_file);
char*       STATUS__unit            (char *a_question, char a_abbr);
/*---(modes)----------------*/
char        MODE_init               (void);
char        MODE_enter              (char  a_mode);
char        MODE_exit               (void);
char        MODE_curr               (void);
char        MODE_prev               (void);
char        MODE_not                (char  a_mode);
char        MODE_status             (char *a_list);
char*       MODE_message            (void);
char*       MODE__unit              (char *a_question);
/*---(repeat)---------------*/
char        REPEAT_reset            (void);
char        REPEAT_init             (void);
char        REPEAT_done             (void);
char        REPEAT_umode            (uchar a_major, uchar a_minor);
char        REPEAT_normal           (void);
int         REPEAT_original         (void);
char        REPEAT_decrement        (void);
int         REPEAT_count            (void);
int         REPEAT_use              (void);
char        REPEAT_not              (void);
char        FORMAT_smode            (int a_major, int a_minor);

char        MAP_locator             (char *a_label, int *a_x, int *a_y, int *a_z);
char        MAP_addresser           (char *a_label, int  a_x, int  a_y, int  a_z);
char        MAP_init                (void);
char        MAP__load               (char a_style, tMAPPED *a_map, char a_which);
char        MAP__screen             (tMAPPED *a_map);
char        MAP__move               (int  a_target, tMAPPED *a_map);
char        MAP_jump                (int a_x, int a_y, int a_z);
char        MAP_mode                (char a_major, char a_minor);
char        MAP_xstatus             (char *a_list);
char        MAP_ystatus             (char *a_list);
char        MAP_status              (char *a_list);
char*       MAP__unit               (char *a_question, char a_index);
char        MAP__unit_ymap          (void);
char        MAP__unit_xmap          (void);
char        MAP_current             (int *a_x, int *a_y, int *a_z);

char        MAP__unit_quick         (void);
char        MAP__unit_mapper        (char a_type);
char        MAP__unit_locator       (char *a_label, int *x, int *y, int *z);
char        MAP__unit_addressor     (char *a_label, int x, int y, int z);

char        VISU_valid              (char a_visu);
char        VISU_status             (char *a_list);
char        VISU_status_saved       (char *a_list);
char        VISU_clear              (void);
char        VISU_exact              (int a_xbeg, int a_xend, int a_ybeg, int a_yend, int a_z);
char        VISU_range              (int *a_xbeg, int *a_xend, int *a_ybeg, int *a_yend, int *a_z);
char        VISU_smode              (int a_major, int a_minor);
char        VISU_read               (char a_visu, char *a_label);
char*       VISU__unit              (char *a_question, char a_index);


char        FILE_status             (char *a_list);
int         OUTP_write              (void);
char        INPT_edit               (void);
char        INPT__unit_reader       (char a_abbr);


char        SRC_UNDO_status            (char *a_list);
char        SRC_UNDO__redo             (void);
char        SRC_UNDO__undo             (void);
char*       SOURCE_label            (void);
char        SOURCE_init             (void);
char        SOURCE_other            (char a_type);
char        SOURCE_formula          (void);
char        SOURCE_command          (void);
char        SOURCE_float            (void);
char        SOURCE_status_select    (char *a_list);
char*       SOURCE__unit            (char *a_question, char a_reg);
char        SOURCE_status_words     (char *a_list);
char        SOURCE_start            (char *a_prefix);
char        SRC_REG_init            (void);
char        SRC_REG_valid           (cchar a_reg);
char        SRC_REG_status          (char *a_list);
char        SOURCE_mode             (int a_major, int a_minor);
char        SRC_REG_infowin         (char *a_entry, int a_index);
char        SRC_REG_smode           (int a_major, int a_minor);
char        SRC_REPL_umode           (int a_major, int a_minor);
char        SRC_INPT_umode             (int  a_major, int  a_minor);
char        HISTORY_display         (void);

char        WANDER_smode            (int  a_major, int  a_minor);


char        MARK_init               (void);
char        MARK_valid              (char a_mark);
char        MARK__check             (char *a_label);
char        MARK__find              (char *a_label);
char        MARK__which             (void);
char        MARK__set               (char a_mark);
char        MARK__unset             (char a_mark);
char        MARK__return            (char a_mark);
char*       MARK__unit              (char *a_question, char a_mark);
char        MARK__write             (char a_mark);
char        MARK_writeall           (FILE *a_file);
char        MARK_read               (char a_mark, char *a_label);
char        MARK_direct             (char *a_string);
char        MARK_infowin            (char *a_entry, int a_index);
char        MARK__listplus          (char *a_list);
char        MARK_status             (char *a_status);
char        MARK_smode              (int a_major, int a_minor);


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
char        CMDS_limits             (int *a_min, int *a_max);
char        CMDS_purge              (void);
char        CMDS_init               (void);
/*> char        CMDS__load              (char *a_command);                            <*/
char        CMDS__test              (char a_mode, char a_value);
char*       CMDS__unit              (char *a_question, char a_index);
int         CMDS__find              (char *a_name);
char*       CMDS_curr               (void);
char        CMDS__exec              (void);
int         CMDS_valid              (char a_mark);
char        CMDS__unit_null         (void);
/*---(search)---------------*/
char        HISTORY_limits          (char a_mode, int *a_min, int *a_max);
char        HISTORY_entry           (char a_mode, int a_index, char *a_entry, int a_max);
char*       HISTORY_use             (char a_mode, int a_index);
int         SRCH_valid              (char a_mark);
char        SRCH_init               (void);
char        SRCH__purge             (void);
char        SRCH__exec              (void);
char        SRCH_next               (char a_move);
/*---(unit testing)---------*/
char        SRCH__unit_null         (void);
char        SRCH__unit_searcher     (char *a_search);
char        SRCH__unit_clearer      (char *a_label);
char*       SRCH__unit              (char *a_question, char a_index);



char        FILE_init               (void);
char        FILE_controlled         (char  *a_yes);
char        FILE_control            (void);
char        FILE_nocontrol          (void);
char        FILE_version            (char  *a_ver);
char        FILE_vertxt             (char  *a_txt);
char        FILE_bump               (char  *a_type);
char        FILE_bump_major         (void);
char        FILE_bump_minor         (void);
char        FILE_bump_inc           (void);
char        FILE_loc                (char  *a_loc);
char        FILE_name               (char  *a_name);
char        FILE__unit_null         (void);
char*       FILE__unit              (char *a_question, int a_ref);


char        MAP_REG__by_abbr        (char a_reg);
char        MAP_REG__set            (char a_reg);
char        MAP_REG__reset          (void);
char        MAP_REG_init            (void);
char        MAP_REG_wrap            (void);
char        MAP_REG_save            (void);
char        MAP_REG_visual          (void);
char        MAP_REG_clear           (void);
char        MAP_REG_delete          (void);
char        MAP_REG_copy            (void);
char        MAP_REG_cut             (void);
char        MAP_REG_paste           (char *a_type);
char        MAP_REG_status          (char *a_status);
char        MAP_REG_smode           (int a_major, int a_minor);

char        MAP_REG__unit_purge        (void);
char        MAP_REG__unit_killer       (void *a_thing);
char        MAP_REG__unit_copier       (char a_type, long a_stamp);
char        MAP_REG__unit_clearer      (char a_1st, int x, int y, int z);
char        MAP_REG__unit_paster       (char a_regs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, void *a_thing);
char*       MAP_REG__unit              (char *a_question, char x, char y);


char        SRC_REG_writer          (char a_abbr);
char        SRCH_writer             (char a_abbr);
char        CMDS_writer             (char a_abbr);
char        MARK_writer             (char a_abbr);
char        VISU_writer             (char a_abbr);
char        MACRO_writer            (char a_abbr);

char        SRC_REG_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        SRCH_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        MARK_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        VISU_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        CMDS_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        MACRO_reader            (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);

#define    ACTION_FIND     'f'
#define    ACTION_ADD      '+'
#define    ACTION_DEL      '-'




#endif
/*============================----end-of-source---============================*/
