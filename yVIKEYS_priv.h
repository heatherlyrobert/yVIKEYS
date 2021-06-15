/*============================----beg-of-source---============================*/ #ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes



/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/

/*===[[ ONE_LINERS ]]=========================================================*/

#define     P_FOCUS     "RS (run-time support)"
#define     P_NICHE     "us (user control)"
#define     P_SUBJECT   "keyboard mini-language"
#define     P_PURPOSE   "efficient, versatle, and standard keyboard mini-language"

#define     P_NAMESAKE  "briareos-hecatoncheires (hundred-handed)"
#define     P_HERITAGE  "briareos, the strong-one, is one of the three hecatoncheires" 
#define     P_IMAGERY   "ugly, impossibly powerful, one-hundred handed, fifty headed giant"
#define     P_REASON    ""

#define     P_ONELINE   P_NAMESAKE " " P_SUBJECT

#define     P_BASENAME  "yVIKEYS"
#define     P_FULLNAME  "/usr/local/lib64/libyVIKEYS"
#define     P_SUFFIX    "htags"
#define     P_CONTENT   "code navigation file"

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "large       (appoximately 10,000 slocl)"
#define     P_DEPENDS   "none"

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2010-01"

#define     P_VERMAJOR  "1.-- = working for everyday use, features still evolving but stable"
#define     P_VERMINOR  "1.5- = updates to support developing application"
#define     P_VERNUM    "1.5b"
#define     P_VERTXT    "broke out hint and made dramatic improvements, unit testing"

#define     P_PRIORITY  "direct, simple, brief, vigorous, and lucid (h.w. fowler)"
#define     P_PRINCIPAL "[grow a set] and build your wings on the way down (r. bradbury)"
#define     P_REMINDER  "there are many better options, but i *own* every byte of this one"

/*===[[ END_HEADER ]]=========================================================*/



/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */
#include    <sys/stat.h>          /* clibc  standard file handling            */
#include    <unistd.h>            /* clibc  linux/unix standard environment   */
#include    <time.h>              /* clibc  time related functions            */
#include    <math.h>              /* clibc  mathematical functions            */
#include    <dirent.h>            /* clibc  directory reading and decoding    */
/*---(posix standard)--------------------*/
#include    <GL/gl.h>             /* opengl standard primary header           */
#include    <GL/glx.h>            /* opengl standard X11 integration          */
#include    <ncurses.h>           /* CURSES mvprintw, refresh, getch, ...     */
#include    <X11/extensions/shape.h>   /* xwindows shape extension            */
/*---(custom)----------------------------*/
#include    <yURG.h>               /* heatherly urgent processing             */
#include    <yLOG.h>               /* heatherly program logging               */
#include    <yX11.h>               /* heatherly xlib/glx setup/teardown       */
#include    <ySTR.h>               /* heatherly string processing             */
#include    <yFONT.h>              /* heatherly texture mapped fonts          */
#include    <yCOLOR.h>             /* heatherly opengl color handling         */
#include    <yGOD.h>               /* heatherly opengl godview                */
#include    <yPARSE.h>             /* heatherly file reading and writing      */
#include    <yREGEX.h>             /* heatherly regular expressions           */
#include    <yDLST_solo.h>         /* heatherly double-doubly linked-list     */



extern char    (*s_searcher)   (char *a_search);
extern char    (*s_unsearcher) (int b, int x, int y, int z);
extern char    (*s_hinter)     (char *a_hint);
extern char    (*s_unhinter)   (int b, int x, int y, int z);



typedef   struct timespec  tTSPEC;
typedef   struct   cFIND   tFIND;
struct cFIND {
   char        mode;
   char       *label;
   int         b_pos;
   int         x_pos;
   int         y_pos;
   int         z_pos;
   tFIND      *m_prev;
   tFIND      *m_next;
};


/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */
/*---(string lengths)-----------------*/
#define     DEG2RAD  (3.1415927 / 180.0)
#define     RAD2DEG  (180.0 / 3.1415927)

#define     FILE_CLIP           "/root/z_gehye/vi_clip.txt"
#define     FILE_REPO           "/home/shared/yVIKEYS/repository.macro"


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
   /*---(using prog)------*/
   char        s_prog      [LEN_LABEL];     /* program name                   */
   char        s_fullname  [LEN_DESC];      /* program name with path         */
   char        s_vernum    [LEN_LABEL];     /* program version number         */
   char        s_vertxt    [LEN_DESC];      /* program version text           */
   char        s_namesake  [LEN_DESC];      /* program greek name             */
   char        s_ext       [LEN_LABEL];     /* file extention                 */
   char        s_filetype  [LEN_DESC];      /* description of file type       */
   char        m_script    [LEN_DESC];      /* pre-run script                 */
   /*---(debugging)-------*/
   int         logger;
   long long   last;
   long long   beg;
   long long   end;
   char        loud;                        /* macro execution displays       */
   /*---(global flags-----*/
   char        done;                        /* flag indicating ready to quit  */
   char        trouble;                     /* flag indicating error        0 */
   char        redraw;                      /* force redraw based on changes  */
   char        repeating;                   /* note for repeating actions     */
   char        log_keys;                    /* allows keys to be hidden       */
   char        cursor;                      /* show cursor in ncurses (or not)*/
   char        status_w;                    /* width category of status       */
   /*---(main loop)-------*/
   float       delay;                       /* requested loop sleep timing    */
   int         macro_skip;                  /* diff between playback and exec */
   float       update;                      /* requested screen update timing */
   int         secs;                        /* loop sleep second part         */
   long        nsec;                        /* loop sleep nanosec part        */
   int         loops;                       /* loops before screen update     */
   char        blocking;                    /* keyboard input blocks          */
   char        mode_text   [LEN_TERSE];     /* current mode for display       */
   /*---(progress)--------*/
   char        p_unit      [LEN_TERSE];
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
   char        p_repeat;                    /* progress is loop/continuous    */
   int         p_line;                      /* current progress window line   */
   int         p_lines;                     /* progress window lines avail    */
   char        p_debug; 
   char        p_redraw;                    /* force redraw based on changes  */
   /*---(marks)-----------*/
   char        mark_show;      /* show temporary marks (y/n)                    */
   char        info_win;
   /*---(font)------------*/
   char        env;                         /* opengl vs ncurses              */
   int         font;                        /* yFONT fixed font identifier    */
   int         point;                       /* yFONT font point to use        */
   float       font_scale;                  /* width of char in opengl coords */
   int         fancy;                       /* yFONT fancy font identifier    */
   /*---(file hanndling)--*/
   char        f_control;                   /* file version control flag      */
   char        f_vernum    [LEN_LABEL];     /* file version number            */
   char        f_vertxt    [LEN_HUND ];     /* file version text              */
   char        f_loc       [LEN_RECD ];     /* specific file location         */
   char        f_name      [LEN_RECD ];     /* full file name                 */
   char        f_title     [LEN_RECD ];     /* specific file base name        */
   int         f_lines;                     /* file line number               */
   char        f_recd      [LEN_RECD ];     /* current file record            */
   char        f_type      [LEN_RECD ];     /* current record verb            */
   char        f_vers;                      /* current record version         */
   /*---(menus)-----------*/
   char        m_path      [LEN_LABEL];     /* key path in menu               */
   char        m_keys      [LEN_HUND ];     /* resulting keys from menu       */
   char        loc_menu;                    /* menu location                  */
   char        loc_float;                   /* float location                 */
   char        loc_hist;                    /* history location               */
};
tSHARED     myVIKEYS;

/*---(mode)------------*/
#define     MACRO_STOP         '-'      /* normal keyboard input              */
#define     MACRO_RUN          'M'      /* macro running with redisplay       */
#define     MACRO_DELAY        'D'      /* macro delay playback controls      */
#define     MACRO_PLAYBACK     'P'      /* macro under playback controls      */
/*---(conditions)------*/
#define     IF_MACRO_OFF         if (yvikeys_macro_emode () == MACRO_STOP     ) 
#define     IF_MACRO_RUN         if (yvikeys_macro_emode () == MACRO_RUN      ) 
#define     IF_MACRO_NOT_RUN     if (yvikeys_macro_emode () != MACRO_RUN      ) 
#define     IF_MACRO_DELAY       if (yvikeys_macro_emode () == MACRO_DELAY    ) 
#define     IF_MACRO_PLAYBACK    if (yvikeys_macro_emode () == MACRO_PLAYBACK ) 
#define     IF_MACRO_MOVING      if (yvikeys_macro_emode () == MACRO_RUN      || yvikeys_macro_emode () == MACRO_DELAY   ) 
#define     IF_MACRO_NOT_MOVING  if (yvikeys_macro_emode () != MACRO_RUN      && yvikeys_macro_emode () != MACRO_DELAY   ) 
#define     IF_MACRO_NOT_PLAYING if (yvikeys_macro_emode () == MACRO_STOP     )
#define     IF_MACRO_PLAYING     if (yvikeys_macro_emode () != MACRO_STOP     )
#define     IF_MACRO_ON          if (yvikeys_macro_emode () != MACRO_STOP     ) 
/*---(setting)---------*/
#define     SET_MACRO_OFF        yvikeys_macro_modeset (MACRO_STOP);
#define     SET_MACRO_STOP       yvikeys_macro_modeset (MACRO_STOP);
#define     SET_MACRO_RUN        yvikeys_macro_modeset (MACRO_RUN);
#define     SET_MACRO_PLAYBACK   yvikeys_macro_modeset (MACRO_PLAYBACK);
#define     SET_MACRO_DELAY      yvikeys_macro_modeset (MACRO_DELAY);
/*---(speeds)----------*/
#define     MACRO_BLITZ        '0'
#define     MACRO_FAST         '1'
#define     MACRO_THOU         '2'
#define     MACRO_HUND         '3'
#define     MACRO_TWENTY       '4'
#define     MACRO_TENTH        '5'
#define     MACRO_HALF         '6'
#define     MACRO_SEC          '7'
#define     MACRO_DOUBLE       '8'
#define     MACRO_TRIPLE       '9'
/*---(updates)---------*/
#define     MACRO_NORMAL       'n'  /* normal updates */
#define     MACRO_SLOWER       's'  /* slower updates */
#define     MACRO_BLINKS       'b'  /* stop action looking */
#define     MACRO_PEEKS        'p'  /* very slow screen updates */
#define     MACRO_BLIND        'd'  /* no screen updates */


/*---(recording)-------*/
#define     MACRO_IGNORE       'i'      /* no recording                       */
#define     MACRO_RECORD       'r'      /* macro recording                    */
#define     IF_MACRO_RECORDING   if (yvikeys_macro_rmode () == MACRO_RECORD   ) 
#define     SET_MACRO_RECORD     yvikeys_macro_modeset (MACRO_RECORD);
#define     SET_MACRO_IGNORE     yvikeys_macro_modeset (MACRO_IGNORE);

typedef  struct  cPARTS     tPARTS;
struct cPARTS {
   /*---(main)-----------------*/
   cchar       abbr;                        /* short name of screen element   */
   cchar       name        [LEN_LABEL];     /* name of screen element         */
   cchar       own;                         /* controlled entirely by yVIKEYS */
   char        on;                          /* show or hide y/n               */
   /*---(horizontal)-----------*/
   cchar       horz;                        /* evaluation order left-to-right */
   cchar       nox;                         /* do not add to x cumulatives    */
   cchar       x_tie;                       /* tie to another field in x-axis */
   int         def_wide;                    /* default width                  */
   int         wide;                        /* screen width                   */
   int         left;                        /* screen left                    */
   /*---(vertical)-------------*/
   cchar       vert;                        /* evaluation order bottom-to-top */
   cchar       noy;                         /* do not add to y cumulatives    */
   cchar       y_tie;                       /* tie to another field in y-axis */
   cchar       under;                       /* fall below buffer/formula      */
   int         def_tall;                    /* default height                 */
   int         tall;                        /* screen height                  */
   int         bott;                        /* screen bottom                  */
   /*---(for std elements)-----*/
   char        orient;                      /* orientation of text            */
   char        (*source) (char*);           /* content source                 */
   char        text        [2000    ];      /* optional text                  */
   /*---(special drawing)------*/
   char        (*drawer) (void);            /* drawing function               */
   char        type;                        /* ortho vs 3d                    */
   char        mgmt;                        /* auto vs custom setup           */
   uchar       anchor;                      /* fixed point for resizing       */
   int         color;                       /* background color               */
   int         xmin;                        /* x-coord minimum                */
   int         xlen;                        /* x-coord range                  */
   int         ymin;                        /* y-coord minimum                */
   int         ylen;                        /* y-coord range                  */
   int         zmin;                        /* z-coord minimum                */
   int         zlen;                        /* z-coord range                  */
   /*---(other)----------------*/
   cchar       desc        [LEN_DESC ];     /* explanation of element         */
   /*---(done)-----------------*/
};


extern char        g_coord;
extern int         g_goffx;
extern int         g_gsizex;
extern int         g_goffy;
extern int         g_gsizey;
extern int         g_goffz;
extern int         g_gsizez;

extern uchar *g_stub;

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


typedef     struct dirent     tDIRENT;
typedef     struct stat       tSTAT;
typedef     struct timespec   tSPEC;
typedef     unsigned char     uchar;
typedef     signed char       schar;

#define     YVIKEYS_UPPER     'A'
#define     YVIKEYS_LOWER     'a'
#define     YVIKEYS_NUMBER    '0'
#define     YVIKEYS_GREEK     'è'
#define     YVIKEYS_OTHER     '-'
#define     YVIKEYS_FULL      '*'




char        yvikeys_view__abbr      (cchar  a_abbr);
char        yvikeys_view_pointer    (cchar  a_abbr, tPARTS **a_part, tPARTS **a_link);
char        yvikeys_view_cursor     (char   a_move, tPARTS **a_part, tPARTS **a_link);
char        yvikeys_view_find       (char  *a_name, tPARTS **a_part, tPARTS **a_link);

char        yvikeys_view_defs       (char a_part, char a_on, int a_nwide, int a_ntall, int a_owide, int a_otall, void *a_draw);
char        yvikeys_view__size      (char a_part, int a_wide, int a_tall, int a_left, int a_bott);
char        yvikeys_view_keys       (cchar *a_text);
char        yvikeys_view_modes      (cchar *a_text);
char        yvikeys_view_init       (void);
char        yvikeys_view_reanchor   (char a_part, char a_anchor);
char        VIEW_wrap               (void);

char        VIEW__grid_offset       (int a_x, int a_y, int a_z);
char        VIEW__grid_size         (int a_x, int a_y, int a_z);
char        VIEW_status_default     (char *a_list);
char        VIEW__ribbon             (void);
char        VIEW__grid_normal        (void);
char        VIEW__grid_zoom          (void);
char        VIEW__cursor             (void);

char*       VIEW__unit              (char *a_question, char a_index);



extern char yVIKEYS__unit_answer [LEN_FULL];


/*---(keys)-----------------*/
char        yvikeys_keys_status     (char *a_msg);
char        yvikeys_keys__logger    (uchar a_key);
char        yvikeys_keys_unique     (void);
char        yvikeys_keys_init       (void);
char*       KEYS__unit              (char *a_question, char a_index);
char        yvikeys_set_error       (void);
int         yvikeys_keys_gpos       (void);
char        yvikeys_keys_keygpos    (void);
char        yvikeys_keys_repeating  (void);
char        yvikeys_keys_dump       (FILE *a_file);
char        yvikeys_keys_repos      (int a_pos);
char        yvikeys_sendkeys_prep   (void);
char        SENDKEYS_umode          (uchar a_major, uchar a_minor);
/*---(dumps)----------------*/
char        yvikeys_dump_exec       (char *a_what);
char        yvikeys_dump_write      (cchar *a_recd);
char        yvikeys_dump_read       (int a_line, char *a_recd, int *a_len);
char        yvikeys_dump_init       (void);
char        yvikeys_dump_purge      (void);
char        yvikeys_dump__fake      (FILE *f);
char*       yvikeys_dump__unit      (char *a_question, char a_index);
/*---(unit testing)---------*/
char        yvikeys__unit_quiet     (void);
char        yvikeys__unit_loud      (void);
char        yvikeys__unit_end       (void);
char        yvikeys__unit_reset     (void);

/*> char        yvikeys_base_direct     (char a_mode, char *a_string, void *a_purger (), void *a_clearer (), void *a_saver ());   <*/



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
char        MODE_init               (char  a_mode);
char        yvikeys_mode__update    (void);
char        MODE_enter              (char  a_mode);
char        MODE_exit               (void);
char        MODE_curr               (void);
char        MODE_prev               (void);
char        MODE_not                (char  a_mode);
char        MODE_status             (char *a_list);
char*       MODE_message            (void);
char*       MODE__unit              (char *a_question);
/*---(repeat)---------------*/
char        REPEAT_group_beg        (void);
char        REPEAT_group_end        (void);
char        REPEAT_reset            (void);
char        REPEAT_macro            (void);
char        REPEAT_init             (void);
char        REPEAT_done             (void);
char        REPEAT_umode            (uchar a_major, uchar a_minor);
char        REPEAT_normal           (void);
int         REPEAT_original         (void);
char        REPEAT_decrement        (void);
int         REPEAT_count            (void);
int         REPEAT_use              (void);
char        REPEAT_not              (void);
char        FORMAT_xmode            (int a_major, int a_minor);
char        UNITS_xmode             (int a_major, int a_minor);
char        PALETTE_xmode           (int a_major, int a_minor);


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

char        yvikeys__map_office     (char a_dir, char *a_minor);
char        yvikeys__map_page       (char a_dir, char *a_minor);
char        yvikeys_map_mode        (uchar a_major, uchar a_minor);
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

char        yvikeys_map_wander_prep (void);
char        yvikeys_map_wander      (uchar a_major, uchar a_minor);
char        yvikeys_src_wander_pos  (void);
char        yvikeys_src_wander      (char *a_new);
char        yvikeys_src_wander_done (void);
char        yvikeys_src_wander_cpos (int a_offset);


char        yvikeys__unit_quick     (void);
char        yvikeys__unit_map_map   (char a_type);
char        yvikeys__unit_map_loc   (char *a_label, int *a_buf, int *x, int *y, int *z);
char        yvikeys__unit_map_addr  (char *a_label, int a_buf, int x, int y, int z);

char        yvikeys_visu__valid     (char a_abbr);
int         yvikeys_visu__index     (char a_abbr);
char        yvikeys_visu_islive     (void);
char        yvikeys_visu_isdead     (void);
char        yvikeys_visu_makelive   (void);
char        yvikeys_visu_makedead   (void);
char        yvikeys_visu_locking    (char a_type);
char        yvikeys_visu_ends       (char a_type);
char        yvikeys_visu_update     (void);

char        yvikeys_visu_init       (void);
char        yvikeys_visu_info       (int a_index, char *a_entry);

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yvikeys_visu_reverse    (void);
char        yvikeys_visu_status     (char *a_list);
char        yvikeys_visu_clear      (void);
char        yvikeys_visu_exact      (int b, int xb, int xe, int yb, int ye, int z);
char        yvikeys_visu_umode      (int a_major, int a_minor);
char        VISU_read               (char a_visu, char *a_label);
char        yvikeys_visu__reader    (void);
char        yvikeys_visu__writer    (int c, char a_abbr);
char        yvikeys_visu__writer_all(void);
char*       yvikeys_visu__unit      (char *a_question, char a_index);


char        yvikeys_file_status     (char *a_list);


char        yvikeys_sundo_purge     (int a_start);
char        yvikeys_sundo_init      (void);
char        yvikeys_sundo_beg       (char *a_function);
char        yvikeys_sundo_chain     (void);
char        yvikeys_sundo_end       (char *a_function);
char        yvikeys_sundo_add       (char a_major, char a_minor, int a_pos, char a_before, char a_after);
char        yvikeys_sundo_single    (char a_minor, int a_pos, char a_before, char a_after);
char        yvikeys_sundo_status    (char *a_list);
char        yvikeys_sundo_redo      (int *a_pos);
char        yvikeys_sundo_undo      (int *a_pos);

char*       SOURCE_label            (void);
char        SOURCE_init             (void);
char        SOURCE_other            (char a_type);
char        SOURCE_formula          (void);
char        SOURCE_command          (void);
char        SOURCE_float            (void);
char        SOURCE_menu_prep        (void);
char*       SOURCE__unit            (char *a_question, char a_reg);
char        SOURCE_status_words     (char *a_list);
char        SOURCE_start            (char *a_prefix);
char        SOURCE_mode             (int a_major, int a_minor);
char        SOURCE__done            (void);

/*345678901-12345678901-12345678901-1234-12345678901-12345678901-12345678901-1*/
/*---(one-char actions)------------------*/
char        yvikeys_src_one_replace      (char a_key);
char        yvikeys_src_one_delete       (void);
char        yvikeys_src_one_backspace    (void);
char        yvikeys_src_one_insert       (char a_key);
char        yvikeys_src_one_append       (char a_key);
/*---(selection actions)-----------------*/
char        yvikeys_src_clear            (char a_major, char a_minor);
char        yvikeys_src_delete           (char a_major, char a_minor);
char        yvikeys_src_copy             (void);
char        yvikeys_src_replace          (void);
char        yvikeys_src_paste            (char a_dir);
char        yvikeys_src_swapall          (char *a_new);
int         yvikeys_src_cpos             (void);
int         yvikeys_src_npos             (void);
char       *yvikeys_src_contents         (void);

/*345678901-12345678901-12345678901-1234-12345678901-12345678901-12345678901-1*/
char        yvikeys_sreg_init            (void);
int         yvikeys_sreg__index          (uchar a_abbr);
/*---(data)---------------------------*/
char        yvikeys_sreg_setreg          (uchar a_abbr);
char        yvikeys_sreg_setwork         (uchar a_abbr);
char        yvikeys_sreg_clear           (uchar a_abbr);
char        yvikeys_sreg_push            (uchar a_abbr, char *a_data);
char        yvikeys_sreg_pop             (uchar a_abbr, char *a_data);
char        yvikeys_sreg_save            (char *a_label, char *a_data);
char        yvikeys_sreg_fetch           (int *a_len, char *a_data);
/*> char        yvikeys_sreg_append          (char *a_data);                          <*/
/*---(selection)----------------------*/
char        yvikeys_sreg_reset           (int a_pos);
char        yvikeys_sreg_update          (int a_pos);
int         yvikeys_sreg_reverse         (void);
int         yvikeys_sreg_exact           (int a_beg, int a_end, int a_root);
char        yvikeys_sreg_selected        (int *a_beg, int *a_end, int *a_root);
char        yvikeys_sreg_all             (void);
char        yvikeys_sreg_getlive         (void);
char        yvikeys_sreg_islive          (void);
char        yvikeys_sreg_isdead          (void);
char        yvikeys_sreg_makelive        (void);
char        yvikeys_sreg_makedead        (void);
/*---(mode)---------------------------*/
char        yvikeys_sreg__direct         (char *a_string);
char        yvikeys_sreg_smode           (int a_major, int a_minor);
/*---(status)-------------------------*/
char        yvikeys_sreg_status          (char *a_list);
char        yvikeys_sreg_status_sel      (char *a_list);
char        yvikeys_sreg_info            (int a_index, char *a_entry);
int         yvikeys_sreg_dump            (FILE *a_file);
/*---(unit test)----------------------*/
char*       yvikeys_sreg__unit           (char *a_question, char a_reg);


char        SRC_REPL_umode           (int a_major, int a_minor);
char        SRC_INPT_umode             (int  a_major, int  a_minor);
char        WANDER_smode            (int  a_major, int  a_minor);



/*===[[ MARK.C ]]=============================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)--------------*/
char        yvikeys_mark_init       (void);
char        yvikeys_mark__purge     (char a_scope);
/*---(support)--------------*/
char        yvikeys_mark__valid     (char a_abbr);
int         yvikeys_mark__index     (char a_abbr);
/*---(setting)--------------*/
char        yvikeys_mark__history   (void);
char        yvikeys_mark__set       (char a_mark);
char        yvikeys_mark__unset     (char a_mark);
char        yvikeys_mark__return    (char a_mark);
/*---(finding)--------------*/
char        yvikeys_mark__which     (void);
char        yvikeys_mark__find      (char *a_label);
/*---(sequence)-------------*/
char        yvikeys_mark__range     (void);
char        yvikeys_mark__prev      (void);
char        yvikeys_mark__next      (void);
/*---(status)---------------*/
char        yvikeys_mark_info       (char *a_entry, int a_index);
char        yvikeys_mark_status     (char *a_status);
char        yVIKEYS_hint_marklist   (char *a_list);
char        yvikeys_mark_listplus   (char *a_list);
/*---(file)-----------------*/
char        yvikeys_mark_writer     (int c, char a_abbr);
char        yvikeys_mark_writer_all (void);
char        yvikeys_mark_reader     (void);
/*---(command)--------------*/
char        yvikeys_mark_direct     (char *a_string);
/*---(mode)-----------------*/
char        yvikeys_mark_smode      (int a_major, int a_minor);
/*---(unittest)-------------*/
char*       yvikeys_mark__unit      (char *a_question, char a_mark);
/*---(done)-----------------*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/




/*===[[ MARK.C ]]=============================================================*/
/*345678901-12345678901-12345678901-1234-12345678901-12345678901-12345678901-1*/
/*---(program)--------------*/
char        yvikeys_hint_init       (void);
char        yVIKEYS_hint_config     (void *a_hinter, void *a_unhinter);
char        yvikeys_hint_purge      (void);
char        yvikeys_hint_wrap       (void);
/*---(search)---------------*/
char        yvikeys_hint_by_cursor  (char a_move, tFIND **r_curr);
char        yvikeys_hint_by_index   (int a_index, tFIND **r_curr);
char        yvikeys_hint_list       (char *a_list);
int         yvikeys_hint_count      (void);
char        yvikeys_hint_curr       (char *a_label);
/*---(execute)--------------*/
char        yVIKEYS_hint_found      (char *a_label, int b, int x, int y, int z);
char        yvikeys_hint_direct     (char *a_hint);
/*---(drawing)--------------*/
char        yVIKEYS_hinting         (void);
char        yvikeys_hint_status     (char *a_list);
/*---(unittest)-------------*/
char        yvikeys__unit_hinter    (char *a_hint);
char        yvikeys__unit_unhinter  (int b, int x, int y, int z);
/*---(done)-----------------*/


/*---(program)--------------*/
int         yvikeys_macro__index    (uchar a_abbr);
char        yvikeys_macro__clear    (uchar a_abbr);
char        yvikeys_macro__restart  (void);
char        yvikeys_macro_resetall  (void);

char        yvikeys_macro_init      (void);
char        yvikeys_macro_zero      (void);
char        yvikeys_macro_count     (void);
char        yvikeys_macro__purge    (char a_scope);
/*---(storage)--------------*/
char        yvikeys_macro__save     (void);
char        yvikeys_macro__fetch    (void);
/*---(record)---------------*/
char        yvikeys_macro__recbeg   (char a_name);
char        yvikeys_macro_reckey    (char a_key);
char        yvikeys_macro__recstr   (char *a_keys);
char        yvikeys_macro_recend    (void);
char        yvikeys_macro__direct   (char *a_string);
char*       yvikeys_macro__unit     (char *a_question, uchar a_abbr);
/*---(execute)--------------*/
char        yvikeys_macro__delay    (char a_which, char a_delay);
char        yvikeys_macro__update   (char a_which, char a_update);
char        yvikeys_macro_pos       (char *a_name, int *a_pos);
char        yvikeys_macro_repos     (int a_pos);
char        yvikeys_macro__exectl   (uchar a_key);
char        yvikeys_macro_exebeg    (char a_name);
char        yvikeys_macro_exeadv    (uchar a_play);
char        yvikeys_macro_exekey    (void);
char        yvikeys_macro_exeplay   (uchar a_key);
char        yvikeys_macro_smode     (uchar a_major, uchar a_minor);
char        yvikeys_macro_rstatus   (char *a_list);
char        yvikeys_macro_estatus   (char *a_list);
char        yvikeys_macro_emode     ();
char        yvikeys_macro_rmode     ();
char        yvikeys_macro_modeset   (char a_mode);
char        yvikeys_macro_edelay    (char a_delay);
char        yvikeys_macro_eupdate   (char a_update);
char        yvikeys_macro_ddelay    (char a_delay);
char        yvikeys_macro_dupdate   (char a_update);
/*---(state changes)--------*/
char        yvikeys_macro_set2stop  (void);
char        yvikeys_macro_set2play  (void);
char        yvikeys_macro_set2delay (void);
char        yvikeys_macro_set2run   (void);
char        yvikeys_macro_set2blitz (void);
char        yvikeys_macro_menu_beg  (void);
char        yvikeys_macro_menu_end  (void);
char        yvikeys_help            (char a_type);
/*---(status)---------------*/
char        yvikeys_macro_list      (int *a_count, char *a_list);
/*---(file)-----------------*/
char        yvikeys_macro_writer    (int c, uchar a_abbr);
char        yvikeys_macro_writer_all(void);
char        yvikeys_macro_reader    (void);
int         yvikeys_macro_dump      (FILE *a_file);
char        yvikeys_macro_flatten   (char a_src, char a_dst);
char        yvikeys_macro_install   (char a_src);
/*---(scripts)--------------*/
char        yvikeys_script_open     (char *a_name);
char        yvikeys_script_read     (void);
char        yvikeys_script_close    (void);
char        yvikeys_script_start    (char *a_name);
char        yvikeys_script_follow   (char *a_name);
char        yvikeys_script_playback (char *a_name);
char        yvikeys_script_blitz    (char *a_name);


char        yvikeys_macro_term      (uchar *a_string);



/*---(commands)-------------*/
char        CMDS_limits             (int *a_min, int *a_max);
char        yvikeys_cmds__name      (uchar *a_name, char a_type, int *a_len);
char        yvikeys_cmds__quit      (void);
char        yvikeys_cmds__writequit (void);
char        yvikeys_cmds__term      (char *a_terms);
char        yvikeys_cmds__purge     (void);
char        yvikeys_cmds__load_name (char *a_name);
char        yvikeys_cmds__load      (void);
char        yvikeys_cmds_init       (void);
char        yvikeys_cmds_wrap       (void);
char        yvikeys_cmds__test      (char a_mode, char a_value);
char*       yvikeys_cmds__unit      (char *a_question, char *a_name);
char        yvikeys_cmds_exec       (uchar *a_command, char *a_rc);
char        yvikeys_cmds__find      (uchar *a_name);
char        yvikeys_cmds__parse     (uchar *a_string);
char        yvikeys_cmds__reader    (void);
char        yvikeys_cmds__writer    (char a_abbr);
char        yvikeys_cmds__writer_all(void);
/*---(search)---------------*/
int         SRCH_find_abbr          (char a_abbr);
char        yvikeys_srch_init       (void);
char        yvikeys_srch__purge     (char a_mode);
char        yvikeys_srch_purge      (void);
char        yvikeys_tags_purge      (void);
char        SRCH__exec              (void);
char        yvikeys_srch__found     (char a_mode, char *a_label, int a_buf, int x, int y, int z);
char        yvikeys_srch__by_cursor (char a_mode, char a_move, void **r_curr);
char        yvikeys_srch__by_index  (char a_mode, int a_index, tFIND **r_curr);
char        yvikeys_srch_by_cursor  (char a_move, tFIND **r_curr);
char        yvikeys_tags_by_cursor  (char a_move, tFIND **r_curr);
char        yvikeys_srch_by_index   (int a_index, tFIND **r_curr);
char        yvikeys_tags_by_index   (int a_index, tFIND **r_curr);
char        yvikeys_srch_exec       (uchar *a_search, int *a_found);
int         yvikeys_srch__count     (char a_mode);
char        yvikeys_srch__curr      (char a_mode, char *a_label);
/*---(unit testing)---------*/
char        SRCH__unit_null         (void);
char        SRCH__unit_searcher     (char *a_search);
char        SRCH__unit_clearer      (char *a_label);
char*       SRCH__unit              (char *a_question, char a_index);
char*       MENU__unit              (char *a_question, char *a_path);


char        yvikeys__unit_hinter    (char *a_hint);
char        yvikeys__unit_unhinter  (int b, int x, int y, int z);




char        yvikeys_menu__fix_path  (uchar *a_path, int *a_len, uchar *a_fixed);
char        yvikeys_menu_cmds       (uchar a_key);
char        yvikeys_menu__in_base   (uchar *a_path);
char        yvikeys_menu__base_path (uchar *a_path);
char        yvikeys_menu__base_num  (int n);

int         yvikeys_menu_find       (uchar *a_path);
int         yvikeys_menu_menu       (uchar *a_path);

char        yvikeys_menu_reanchor   (char a_anchor);
char        yvikeys_menu_init       (void);
char        yvikeys_menu_final      (void);
char        yvikeys_menu_wrap       (void);
char        yvikeys_menu_start      (void);
char        yvikeys_menu_smode      (int  a_major, int  a_minor);
char        yvikeys_menu_shadow     (char a_type, int a_lef, int a_wide, int a_bot, int a_tall, int z);
char        yvikeys_menu_fill       (char a_type, int a_len, int a_lvl, int a_lef, int a_wide, int a_bot, int a_tall, int z);
char        yvikeys_menu_heads      (char a_type, int a_len, int a_lvl, void *a_found, int a_lef, int a_wide, int a_bot, int a_tall, int z);
char        yvikeys_menu_draw       (void);
char        yvikeys__menu_cleanse   (void);
char        yvikeys_menu__place_round  (char a_type);
void*       yvikeys_menu_found      (uchar *a_path);
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/


char        yvikeys_prog_play       (void);
char        yvikeys_prog_stop       (void);
char        yvikeys_loop_init       (void);
int         yvikeys_loop_getch      (void);
char        yvikeys_loop_delay      (char *a_delay);
char        yvikeys_loop_update     (char *a_update);
char        yvikeys_loop_beg        (void);
char        yvikeys_loop_prog       (void);
char        yvikeys_loop_sleep      (uchar a_key, char a_draw);
char        yvikeys_loop_macro      (char a_delay, char a_update);
char        yvikeys_loop_normal     (void);
char        yvikeys_loop_blitz      (void);
char        yvikeys_loop_unblitz    (void);
char        yvikeys_delay_status    (char *a_list);
char        yvikeys_prog_status     (char *a_list);
char        yvikeys_main_status     (char *a_list);
char        yvikeys_loop_set        (char *a_delay, char *a_update);
char*       yvikeys_god__unit       (char *a_question, char a_mark);




char        yvikeys_file_init            (void);
char        yvikeys_file_new             (void);
char        yvikeys_vers_controlled      (char  *a_yes);
char        yvikeys_vers_control         (void);
char        yvikeys_vers_nocontrol       (void);
char        yvikeys_vers_version         (char  *a_ver);
char        yvikeys_vers_vertxt          (char  *a_txt);
char        yvikeys_vers_bump            (char  *a_type);
char        yvikeys_vers_bump_major      (void);
char        yvikeys_vers_bump_minor      (void);
char        yvikeys_vers_bump_inc        (void);
char        yvikeys_vers_status          (char *a_list);
int         yvikeys__file_regex          (char a_type, char *a_ext, char *a_base, char *a_entry, char *a_match);
char        yvikeys__file_stripext       (char *a_loc);
char        yvikeys__file_path           (char *a_path);
char        yvikeys_file_loc             (char *a_loc);
char        yvikeys_file_name            (char *a_name);
char        yvikeys_file_browse          (char *a_name);
char        FILE__unit_null              (void);
char*       FILE__unit                   (char *a_question, int a_ref);

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yvikeys_file_prog_writer    (void);
char        yvikeys_file_time_writer    (void);
char        yvikeys_file_vers_writer    (void);
char        yvikeys_file_writer         (void);
char        yvikeys_file_writeas        (char *a_name);

char        yvikeys_file_reader         (void);

char        yvikeys_mreg__valid     (char a_abbr);
int         yvikeys_mreg__index     (char a_abbr);
char        yvikeys_mreg__set            (char a_reg);
char        yvikeys_mreg_init            (void);
char        yvikeys_mreg_wrap            (void);
char        yvikeys_mreg_save            (void);
char        yvikeys_mreg_visual          (void);
char        yvikeys_mreg_clear           (void);
char        yvikeys_mreg_clear_combo     (void);
char        yvikeys_mreg_paste           (char *a_type);
char        yvikeys_mreg_paste_combo     (char *a_type);
char        yvikeys_mreg_status          (char *a_status);
char        yvikeys_mreg_smode           (int a_major, int a_minor);

char        yvikeys_mreg__unit_purge   (void);
char        yvikeys_mreg__unit_clearer (char a_1st, int b, int x, int y, int z);
char        yvikeys_mreg__unit_copier  (char a_type, long a_stamp);
char        yvikeys_mreg__unit_paster  (char a_regs, char a_pros, char a_intg, char a_1st, int a_boff, int a_xoff, int a_yoff, int a_zoff, void *a_thing);
char        yvikeys_mreg__unit_regkill (void *a_thing);
char        yvikeys_mreg__unit_config  (void);
char*       yvikeys_mreg__unit         (char *a_question, char x, char y);




/*---(support)------------------------*/
char        yvikeys_hist__valid     (char a_abbr);
char        yvikeys_hist__switcher  (char a_mode, char a_force);
/*---(memory)-------------------------*/
char        yvikeys_hist__new       (char a_mode, uchar *a_text);
char        yvikeys_hist__purge     (char a_type);
/*---(locating)-----------------------*/
char        yvikeys_hist__cursor    (char a_move);
char        yvikeys_hist__index     (int a_index);
char        yvikeys_hist__find      (uchar *a_text);
char        yvikeys_hist__roll      (void);
/*---(marks)--------------------------*/
char        yvikeys_hist__mark      (uchar a_mark);
char        yvikeys_hist__unmark    (uchar a_mark);
char        yvikeys_hist__marked    (uchar a_mark);
/*---(program)------------------------*/
char        yvikeys_hist_init       (void);
char        yvikeys_hist_wrap       (void);
char        yvikeys_hist_undo       (void);
char        yvikeys_hist_redo       (void);
char        yvikeys_hist_text       (char a_mode, char *a_text);
char        yvikeys_hist_exec       (char a_mode);
/*---(files)--------------------------*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yvikeys_hist__write     (int c, char a_mode, char a_abbr);
char        yvikeys_hist__writer    (char a_mode);
char        yvikeys_cmds_writer     (void);
char        yvikeys_srch_writer     (void);
char        yvikeys_hist__reader    (char a_mode, int n, char *a_verb);
char        yvikeys_cmds_reader     (int n, char *a_verb);
char        yvikeys_srch_reader     (int n, char *a_verb);
/*---(display)------------------------*/
char        yvikeys_hist_limits     (char a_mode, int *a_min, int *a_max);
char        yvikeys_hist__entry     (uchar *a_entry, int a_max, char a_type);
char        yvikeys_hist_show       (void);
char        yvikeys_hist_umode      (int  a_major, int  a_minor);
/*---(unit_test)----------------------*/
char*       yvikeys_hist__unit      (char *a_question, int n);



/*---(program)------------------------*/
char        yvikeys_layer_init      (void);
char        yvikeys_layer_wrap      (void);
char        yvikeys_layer_purge     (void);
/*---(support)------------------------*/
char        yvikeys_layer__valid    (uchar a_abbr);
char        yvikeys_layer__name     (uchar *a_name);
/*---(search)-------------------------*/
char        yvikeys_layer_cursor    (char a_move);
char        yvikeys_layer_find      (uchar *a_name);
/*---(ordering)-----------------------*/
char        yvikeys_layer__none     (void);
char        yvikeys_layer_action    (uchar *a_name, uchar *a_action);
/*---(display)------------------------*/
char        yvikeys_layer_status    (char *a_line);
char        yvikeys_layer_show      (void);
/*---(annotate)-----------------------*/
char        yvikeys_note_resize     (void);
char        yvikeys_note__purge     (char a_init);
char        yvikeys_note__append    (char n, char xr, char yr, char a_size, char *a_text);
char        yvikeys_note__remove    (char n);
char        yvikeys_note__totop     (char n);
char        yvikeys_note__settarg   (char n, char *p);
char        yvikeys_note__retarg    (char n);
char        yvikeys_note            (char *a_all);
char        yVIKEYS_circle          (int x, int y, int r);
char        yVIKEYS_box             (int x, int y, int w, int h);
char        yvikeys_note_draw       (void);
/*---(unit_test)----------------------*/
char        yvikeys_layer__unit_null(void);
char*       yvikeys_layer__unit     (char *a_question, uchar *a_key);


/*---(program)------------------------*/
char        yvikeys_sizes_init      (void);
/*---(defaults)-----------------------*/
char        yvikeys_sizes_defaults   (cchar a_env);
/*---(horizontal)---------------------*/
char        yvikeys_sizes_horz_fixed (void);
char        yvikeys_sizes_horz_auto  (cint a_wide, cint a_alt);
char        yvikeys_sizes_horz_var   (void);
char        yvikeys_sizes_horz_link  (void);
char        yvikeys_sizes_horz_float (void);
char        yvikeys_sizes_horz_other (void);
char        yvikeys_sizes_horz       (cint a_wide, cint a_alt);
/*---(vertical)-----------------------*/
char        yvikeys_sizes_vert_fixed (void);
char        yvikeys_sizes_vert_auto  (cint a_tall);
char        yvikeys_sizes_vert_var   (void);
char        yvikeys_sizes_vert_link  (void);
char        yvikeys_sizes_vert_float (void);
char        yvikeys_sizes_vert_fill  (void);
char        yvikeys_sizes_vert_flip  (void);
char        yvikeys_sizes_vert       (cint a_tall);
/*---(movable)------------------------*/
char        yvikeys_sizes_menu_loc   (char a_loc);
char        yvikeys_sizes_float_loc  (char a_loc);
char        yvikeys_sizes_hist_loc   (char a_loc);
/*---(anchoring)----------------------*/
char        yvikeys_sizes_anchor     (cchar a_abbr);
char        yvikeys_sizes_anchor_all (void);
/*---(sizing)-------------------------*/
char        yvikeys_sizes_resize    (cchar a_type);
/*---(switching)----------------------*/
char        yvikeys_sizes_switch    (char *a_name, char *a_opt);
/*---(layout)-------------------------*/
char        yvikeys_sizes_layout    (char *a_name);


#define    ACTION_FIND     'f'
#define    ACTION_ADD      '+'
#define    ACTION_DEL      '-'




#endif
/*============================----end-of-source---============================*/
