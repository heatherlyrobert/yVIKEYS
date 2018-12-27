/*============================----beg-of-source---============================*/



/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */
#include    <sys/stat.h>          /* clibc  standard file handling            */
#include    <unistd.h>            /* clibc  linux/unix standard environment   */
#include    <time.h>              /* clibc  time related functions            */
#include    <math.h>              /* clibc  mathematical functions            */
/*---(posix standard)--------------------*/
#include    <GL/gl.h>             /* opengl standard primary header           */
#include    <GL/glx.h>            /* opengl standard X11 integration          */
#include    <ncurses.h>           /* CURSES mvprintw, refresh, getch, ...     */
/*---(custom)----------------------------*/
#include    <yURG.h>               /* heatherly urgent processing             */
#include    <yLOG.h>               /* heatherly program logging               */
#include    <yX11.h>               /* heatherly xlib/glx setup/teardown       */
#include    <ySTR.h>               /* heatherly string processing             */
#include    <yFONT.h>              /* heatherly texture mapped fonts          */
#include    <yCOLOR.h>             /* heatherly opengl color handling         */
#include    <yGOD.h>               /* heatherly opengl godview                */
#include    <yPARSE.h>             /* heatherly file reading and writing      */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "1.1n"
#define YVIKEYS_VER_TXT   "totally clean master unit test run"



typedef struct timespec  tTSPEC;


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
   long long   last;
   long long   beg;
   long long   end;
   /*---(global flags-----*/
   char        done;                        /* flag indicating ready to quit  */
   char        trouble;                     /* flag indicating error        0 */
   char        redraw;                      /* force redraw based on changes  */
   char        repeating;                   /* note for repeating actions     */
   /*---(main loop)-------*/
   float       delay;                       /* requested loop sleep timing    */
   float       update;                      /* requested screen update timing */
   int         secs;                        /* loop sleep second part         */
   long        nsec;                        /* loop sleep nanosec part        */
   int         loops;                       /* loops before screen update     */
   char        blocking;                    /* keyboard input blocks          */
   /*---(progress)--------*/
   char        p_play;                      /* is progress playing            */
   int         p_scale;                     /* progress bar scale             */
   int         p_speed;                     /* progress bar speed             */
   float       p_adv;                       /* progress play advancing        */
   float       p_inc;                       /* progress horizontal moves      */
   char        p_pos;                       /* position of current bar (shcle)*/
   float       p_cur;                       /* current timeline seconds       */
   float       p_beg;                       /* beg second for timeline play   */
   float       p_end;                       /* end second for timeline play   */
   float       p_len;                       /* length of script               */
   int         p_line;                      /* current progress window line   */
   int         p_lines;                     /* progress window lines avail    */
   char        p_debug; 
   char        p_redraw;                    /* force redraw based on changes  */
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
#define     IF_MACRO_OFF         if (yvikeys_macro_modeget () == MACRO_OFF      ) 
#define     IF_MACRO_RUN         if (yvikeys_macro_modeget () == MACRO_RUN      ) 
#define     IF_MACRO_NOT_RUN     if (yvikeys_macro_modeget () != MACRO_RUN      ) 
#define     IF_MACRO_DELAY       if (yvikeys_macro_modeget () == MACRO_DELAY    ) 
#define     IF_MACRO_PLAYBACK    if (yvikeys_macro_modeget () == MACRO_PLAYBACK ) 
#define     IF_MACRO_MOVING      if (yvikeys_macro_modeget () == MACRO_RUN      || yvikeys_macro_modeget () == MACRO_DELAY   ) 
#define     IF_MACRO_NOT_PLAYING if (yvikeys_macro_modeget () == MACRO_OFF      || yvikeys_macro_modeget () == MACRO_RECORD  )
#define     IF_MACRO_PLAYING     if (yvikeys_macro_modeget () != MACRO_OFF      && yvikeys_macro_modeget () != MACRO_RECORD  )
#define     IF_MACRO_RECORDING   if (yvikeys_macro_modeget () == MACRO_RECORD   ) 
#define     IF_MACRO_ON          if (yvikeys_macro_modeget () != MACRO_OFF      ) 
/*---(setting)---------*/
#define     SET_MACRO_OFF        yvikeys_macro_modeset (MACRO_OFF);
#define     SET_MACRO_RUN        yvikeys_macro_modeset (MACRO_RUN);
#define     SET_MACRO_PLAYBACK   yvikeys_macro_modeset (MACRO_PLAYBACK);
#define     SET_MACRO_DELAY      yvikeys_macro_modeset (MACRO_DELAY);
#define     SET_MACRO_RECORD     yvikeys_macro_modeset (MACRO_RECORD);


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

#define     YVIKEYS_UPPER     'A'
#define     YVIKEYS_LOWER     'a'
#define     YVIKEYS_NUMBER    '0'
#define     YVIKEYS_GREEK     'è'
#define     YVIKEYS_FULL      '*'

extern char *gvikeys_upper;
extern char *gvikeys_lower;
extern char *gvikeys_number;
extern char *gvikeys_greek;



char        VIEW__reset             (void);
char        VIEW__find              (cchar *a_name);
char        VIEW__abbr              (cchar  a_abbr);
char        VIEW_wrap                 (void);
char        VIEW__switch             (char *a_name, char *a_opt);

char        VIEW__widths            (cint a_wide, cint a_alt);
char        VIEW__heights           (cint a_tall);
char        VIEW__resize            (cchar a_type);
char        VIEW__grid_offset       (int a_x, int a_y, int a_z);
char        VIEW__grid_size         (int a_x, int a_y, int a_z);
char        VIEW_status_default     (char *a_list);

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


char        GOD_mode                (char a_major, char a_minor);
char        PROGRESS_mode           (char a_major, char a_minor);
char        yvikeys_scale           (char a_mode , char *a_scale);
char        yvikeys_speed           (char a_mode , char *a_speed);
char        yvikeys_speed_prog      (char *a_speed);
char        yvikeys_scale_prog      (char *a_scale);


char        yvikeys_map_locator     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z);
char        yvikeys_map_addresser   (char *a_label, int a_buf, int a_x, int a_y, int a_z);
char        yvikeys_map_init        (void);
char        yvikeys__map_load       (char a_style, tMAPPED *a_map, char a_which);
char        yvikeys__screen         (tMAPPED *a_map);
char        yvikeys__map_move       (int  a_target, tMAPPED *a_map);
char        yvikeys_map_mode        (char a_major, char a_minor);
char        yvikeys_map_xstatus     (char *a_list);
char        yvikeys_map_ystatus     (char *a_list);
char        yvikeys_map_status      (char *a_list);
char*       yvikeys__unit_map       (char *a_question, char a_index);
char        yvikeys__unit_map_ymap  (void);
char        yvikeys__unit_map_xmap  (void);
char        yvikeys_map_current     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z);
char        yvikeys_map_reposition  (void);
char        yvikeys_bufs_init       (void);
char        yvikeys_bufs_umode      (uchar a_major, uchar a_minor);

char        yvikeys__unit_quick     (void);
char        yvikeys__unit_map_map   (char a_type);
char        yvikeys__unit_map_loc   (char *a_label, int *a_buf, int *x, int *y, int *z);
char        yvikeys__unit_map_addr  (char *a_label, int a_buf, int x, int y, int z);

char        VISU_valid              (char a_visu);
char        VISU_status             (char *a_list);
char        VISU_status_saved       (char *a_list);
char        VISU_clear              (void);
char        VISU_exact              (int a_xbeg, int a_xend, int a_ybeg, int a_yend, int a_z);
char        VISU_range              (int *a_xbeg, int *a_xend, int *a_ybeg, int *a_yend, int *a_z);
char        VISU_smode              (int a_major, int a_minor);
char        VISU_read               (char a_visu, char *a_label);
char*       VISU__unit              (char *a_question, char a_index);


char        yvikeys_file_status             (char *a_list);
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


/*345678901-12345678901-12345678901-1234-12345678901-12345678901-12345678901-1*/
char        yvikeys_mark_init            (void);
char        yvikeys_mark__valid          (char a_abbr);
int         yvikeys_mark__index          (char a_abbr);
char        yvikeys_mark__abbr           (int  a_index);
char        yvikeys_mark__find           (char *a_label);
char        yvikeys_mark__which          (void);
char        yvikeys_mark__set            (char a_mark);
char        yvikeys_mark__unset          (char a_mark);
char        yvikeys_mark__return         (char a_mark);
char*       yvikeys_mark__unit           (char *a_question, char a_mark);
char        yvikeys_mark_writer          (char a_abbr);
char        yvikeys_mark_writer_all      (void);
char        yvikeys_mark_reader          (void);
char        yvikeys_mark_direct          (char *a_string);
char        yvikeys_mark_info            (char *a_entry, int a_index);
char        yvikeys_mark_listplus        (char *a_list);
char        yvikeys_mark_status          (char *a_status);
char        yvikeys_mark_smode           (int a_major, int a_minor);


/*---(program)--------------*/
char        yvikeys_macro_init      (void);
char        yvikeys_macro_zero      (void);
char        yvikeys_macro_count     (void);
char        yvikeys_macro__clear    (char a_macro);
char        yvikeys_macro__purge    (char a_scope);
char        yvikeys_macro_reset     (void);
char        yvikeys_macro__valid    (char a_abbr);
int         yvikeys_macro__index    (char a_abbr);
/*---(storage)--------------*/
char        yvikeys_macro__save     (void);
char        yvikeys_macro__fetch    (char a_name);
/*---(record)---------------*/
char        yvikeys_macro__recbeg   (char a_name);
char        yvikeys_macro_reckey    (char a_key);
char        yvikeys_macro__recstr   (char *a_keys);
char        yvikeys_macro_recend    (void);
char        yvikeys_macro__direct   (char *a_string);
char*       yvikeys_macro__unit     (char *a_question, char a_macro);
/*---(execute)--------------*/
char        yvikeys_macro__delay    (char a_delay);
char        yvikeys_macro__exectl   (char a_key);
char        yvikeys_macro_exebeg    (char a_name);
char        yvikeys_macro_exeadv    (void);
char        yvikeys_macro_exewait   (void);
char        yvikeys_macro_exekey    (void);
char        yvikeys_macro_exeplay   (char a_key);
char        yvikeys_macro_smode     (char a_major, char a_minor);
char        yvikeys_macro_modeget   ();
char        yvikeys_macro_modeset   (char a_mode);
/*---(status)---------------*/
char        yvikeys_macro_list      (int *a_count, char *a_list);
/*---(file)-----------------*/
char        yvikeys_macro_writer    (char a_abbr);
char        yvikeys_macro_writer_all(void);
char        yvikeys_macro_reader    (void);

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
char        yvikeys_cmds__valid     (char a_abbr);
int         yvikeys_cmds__index     (char a_abbr);
char        yvikeys_cmds__reader    (void);
char        yvikeys_cmds__writer    (char a_abbr);
char        yvikeys_cmds__writer_all(void);
char        CMDS__unit_null         (void);
/*---(search)---------------*/
char        HISTORY_limits          (char a_mode, int *a_min, int *a_max);
char        HISTORY_entry           (char a_mode, int a_index, char *a_entry, int a_max);
char*       HISTORY_use             (char a_mode, int a_index);
char        yvikeys_srch__valid     (char a_abbr);
int         yvikeys_srch__index     (char a_abbr);
int         SRCH_find_abbr          (char a_abbr);
char        SRCH_init               (void);
char        SRCH__purge             (void);
char        SRCH__exec              (void);
char        SRCH_next               (char a_move);
char        yvikeys_srch__reader    (void);
char        yvikeys_srch__writer    (char a_abbr);
char        yvikeys_srch__writer_all(void);
/*---(unit testing)---------*/
char        SRCH__unit_null         (void);
char        SRCH__unit_searcher     (char *a_search);
char        SRCH__unit_clearer      (char *a_label);
char*       SRCH__unit              (char *a_question, char a_index);


char        yvikeys_loop_init       (void);
int         yvikeys_loop_getch      (void);
char        yvikeys_loop_delay      (char *a_delay);
char        yvikeys_loop_update     (char *a_update);
char        yvikeys_loop_beg        (void);
char        yvikeys_loop_prog       (void);
char        yvikeys_loop_sleep      (uchar a_key, char a_draw);
char        yvikeys_delay_status    (char *a_list);
char        yvikeys_prog_status     (char *a_list);
char        yvikeys_main_status     (char *a_list);
char        yvikeys_loop_set        (char *a_delay, char *a_update);
char*       GOD__unit               (char *a_question, char a_mark);




char        yvikeys_file_init               (void);
char        yvikeys_file_controlled         (char  *a_yes);
char        yvikeys_file_control            (void);
char        yvikeys_file_nocontrol          (void);
char        yvikeys_file_version            (char  *a_ver);
char        yvikeys_file_vertxt             (char  *a_txt);
char        yvikeys_file_bump               (char  *a_type);
char        yvikeys_file_bump_major         (void);
char        yvikeys_file_bump_minor         (void);
char        yvikeys_file_bump_inc           (void);
char        yvikeys_file_loc                (char  *a_loc);
char        yvikeys_file_name               (char  *a_name);
char        FILE__unit_null         (void);
char*       FILE__unit              (char *a_question, int a_ref);

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yvikeys_file_prog_writer    (void);
char        yvikeys_file_time_writer    (void);
char        yvikeys_file_vers_writer    (void);
char        yvikeys_file_writer         (void);

char        yvikeys_file_reader         (void);

char        yvikeys_regs__valid     (char a_abbr);
int         yvikeys_regs__index     (char a_abbr);
char        yvikeys__regs_set            (char a_reg);
char        yvikeys_regs_init            (void);
char        yvikeys_regs_wrap            (void);
char        yvikeys_regs_save            (void);
char        yvikeys_regs_visual          (void);
char        yvikeys_regs_clear           (void);
char        yvikeys_regs_paste      (char *a_type);
char        yvikeys_regs_status     (char *a_status);
char        yvikeys_regs_smode           (int a_major, int a_minor);

char        yvikeys__unit_regs_purge   (void);
char        yvikeys__unit_regs_clearer (char a_1st, int x, int y, int z);
char        yvikeys__unit_regs_copier  (char a_type, long a_stamp);
char        yvikeys__unit_regs_paster  (char a_regs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, void *a_thing);
char        yvikeys__unit_regs_regkill (void *a_thing);
char        yvikeys_regs__unit_config  (void);
char*       yvikeys__unit_regs         (char *a_question, char x, char y);


char        SRC_REG_writer          (char a_abbr);
char        VISU_writer             (char a_abbr);
char        yvikeys_macro_writer    (char a_abbr);

char        SRC_REG_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);
char        VISU_reader             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i);


char        yvikeys_hist_init       (void);
char        yvikeys_hist_undo       (void);
char        yvikeys_hist_redo       (void);


#define    ACTION_FIND     'f'
#define    ACTION_ADD      '+'
#define    ACTION_DEL      '-'




#endif
/*============================----end-of-source---============================*/
