/*============================----beg-of-source---============================*/
#ifndef yVIKEYS
#define yKIKEYS yes



/*===[[ CONSTANTS ]]==========================================================*/
/*---(foundation)------------------------*/
#define     FMOD_STATUS    '5'
#define     FMOD_FILE      'f'
#define     FMOD_MODE      '0'
#define     FMOD_VIEW      'V'
#define     MODE_NONE      '-'
/*---(major modes)-----------------------*/
#define     MODE_GOD       'G'
#define     MODE_OMNI      'O'
#define     MODE_PROGRESS  'P'
#define     MODE_MAP       'M'
#define     MODE_SOURCE    'S'
#define     MODE_COMMAND   ':'
#define     MODE_SEARCH    '/'
#define     MODES_ONELINE  "S:/"
#define     MODES_EDITING  "S:/triw"
/*---(sub-modes)-------------------------*/
#define     SMOD_ERROR     'e'    /* error reporting and actions              */
#define     SMOD_MREG      'R'    /* register actions                         */
#define     SMOD_SREG      't'    /* text register actions                    */
#define     UMOD_SRC_UNDO  'u'    /* incremental text change undo/redo        */
#define     UMOD_MAP_UNDO  'm'    /* incremental map change undo/redo         */
#define     UMOD_HISTORY   'H'    /* dislay command/search history            */
#define     SMOD_BUFFER    ','    /* selecting buffers                        */
#define     XMOD_FORMAT    '$'    /* content formatting                       */
#define     XMOD_UNITS     'K'    /* content units for scaling                */
#define     XMOD_OBJECT    'o'    /* object formatting                        */
#define     SMOD_HINT      ';'    /* hint labels                              */
#define     SMOD_MACRO     '@'    /* macro processing                         */
#define     SMOD_MENUS     'g'    /* show menu system (gui)                   */
/*---(micro-modes)-----------------------*/
#define     UMOD_MARK      '\''   /* location and object marking              */
#define     UMOD_VISUAL    'v'    /* visual selection history                 */
#define     UMOD_REPEAT    '9'    /* accumulate multiplier                    */
#define     UMOD_SRC_REPL  'r'    /* replacing characters in source mode      */
#define     UMOD_SRC_INPT  'i'    /* direct input of text                     */
#define     UMOD_WANDER    'w'    /* formula creation by pointing             */
#define     UMOD_HINTS     'h'    /* hint labels                              */
#define     SMOD_FILTER    '!'    /* data filtering                           */
/*---(pseudo)----------------------------*/
#define     FILE_COLS      'X'    /* x-axis (cols)                            */
#define     FILE_ROWS      'Y'    /* y-axis (rows)                            */
#define     FILE_TABS      'Z'    /* z-axis (tabs)                            */
#define     FILE_DEPCEL    'd'
#define     FILE_FREECEL   'f'




typedef  const unsigned char  cuchar;
typedef  unsigned char        uchar;
typedef  const char           cchar;
typedef  const int            cint;
typedef  const float          cfloat;


/*===[[ SCREEN MAPS ]]========================================================*/
/*
 *  screen maps transpose artificial width blocks in an application to screen
 *  locations -- chars or pixels.  this is less important for things like
 *  pixel editors, but what makes movement in something like a spreadsheet
 *  possible.
 *
 *  u prefix means in screen units, which are characters in ncurses
 *  g prefix means in application units, which are cells in spreadsheets
 *
 *  if the range is 0 - 15
 *     count = 16
 *     min   =  0
 *     max   = 15
 *
 *
 *
 *
 */

typedef  struct cMAPPED  tMAPPED;
struct cMAPPED {
   /*---(identity)-------------*/
   char        which;                       /* b, x, y, or z                  */
   /*---(lefts)----------------*/
   int         umin;                        /* global min, used or not        */
   int         gmin;                        /* global min, used or not        */
   int         gamin;                       /* min of all used space          */
   int         glmin;                       /* min for col/row                */
   int         gprev;                       /* prev change for "end"          */
   /*---(current)--------------*/
   int         map         [10000];         /* full col/row map               */
   char        used        [10000];         /* full col/row map (of usage)    */
   /*---(rights)---------------*/
   int         gnext;                       /* next change for "end"          */
   int         glmax;                       /* max for col/row                */
   int         gamax;                       /* max of all used space          */
   int         gmax;                        /* global max, used or not        */
   int         umax;                        /* global max, used or not        */
   /*---(screen)---------------*/
   int         ubeg;                        /* beg abs pos shown on screen    */
   int         ucur;                        /* cur abs pos shown on screen    */
   int         uend;                        /* end abs pos shown on screen    */
   int         ulen;                        /* len shown on screen (end-beg+1)*/
   int         uavail;                      /* full availible room on screen  */
   int         utend;                       /* hypothetical abs end for screen*/
   /*---(grids)----------------*/
   int         gbeg;                        /* grid at start of screen        */
   int         gcur;                        /* current grid position          */
   int         gend;                        /* grid at end of screen          */
   /*---(done)-----------------*/
};
tMAPPED     g_bmap;
tMAPPED     g_xmap;
tMAPPED     g_ymap;
tMAPPED     g_zmap;

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
#define     YVIKEYS_BMAP            'b'
#define     YVIKEYS_XMAP            'x'
#define     YVIKEYS_YMAP            'y'
#define     YVIKEYS_ZMAP            'z'

#define     YVIKEYS_EMPTY           -666
/*---(completely clean and populate)-----*/
#define     YVIKEYS_INIT            'I'
/*---(changed focus of map)--------------*/
#define     YVIKEYS_MAJOR           'M'
/*---(same focus, different pos)---------*/
#define     YVIKEYS_MINOR           'n'
/*---(update as neccessary)--------------*/
#define     YVIKEYS_UPDATE          'u'

#define     YVIKEYS_RIGHT           'r'
#define     YVIKEYS_OFFICE          'o'

#define     YVIKEYS_DEPTH           '3'
#define     YVIKEYS_FLAT            'O'
#define     YVIKEYS_DISABLE         'X'

/*---(run as)----------*/
#define     RUN_USER           'i'      /* running as user (ncurses/opengl)   */
#define     RUN_TEST           '-'      /* running as test (no ncurses/opengl)*/



/*---(normal)-------------------------*/
#define      YVIKEYS_TITLE       't'
#define      YVIKEYS_BUFFER      'b'
#define      YVIKEYS_FORMULA     'f'
#define      YVIKEYS_NAV         'n'
#define      YVIKEYS_MAIN        'm'
#define      YVIKEYS_ALT         'a'
#define      YVIKEYS_PROGRESS    'p'
#define      YVIKEYS_MODES       'x'
#define      YVIKEYS_STATUS      's'
#define      YVIKEYS_COMMAND     'c'
#define      YVIKEYS_DETAILS     'd'
#define      YVIKEYS_RIBBON      'r'
#define      YVIKEYS_VERSION     'v'
#define      YVIKEYS_KEYS        'k'
/*---(specialty)----------------------*/
#define      YVIKEYS_WINDOW      'W'
#define      YVIKEYS_CURSOR      'C'
#define      YVIKEYS_GRID        'G'
#define      YVIKEYS_OVERLAY     'O'
#define      YVIKEYS_LAYERS      'L'
#define      YVIKEYS_FLOAT       'F'
#define      YVIKEYS_BACK        'B'
#define      YVIKEYS_XAXIS       'X'
#define      YVIKEYS_YAXIS       'Y'
#define      YVIKEYS_HISTORY     'H'
#define      YVIKEYS_MENUS       'M'



#define      YVIKEYS_AUTO        'a'
#define      YVIKEYS_CUSTOM      'c'

#define      YVIKEYS_ALLALL       0
#define      YVIKEYS_TOPLEF       1
#define      YVIKEYS_TOPCEN       2
#define      YVIKEYS_TOPRIG       3
#define      YVIKEYS_MIDLEF       4
#define      YVIKEYS_MIDCEN       5
#define      YVIKEYS_MIDRIG       6
#define      YVIKEYS_BOTLEF       7
#define      YVIKEYS_BOTCEN       8
#define      YVIKEYS_BOTRIG       9


#define      YVIKEYS_NONE        '-'
#define      YVIKEYS_OPENGL      'g'
#define      YVIKEYS_CURSES      'c'


/*> File Edit View Insert fOrmat Buffers Dataset Tools Layers Select Modify Palette Analyze eXecute Config sHOw macRos   <*/

/* abcdef_hi__lm_op_rstuv_x__ */ 

/*---(primary menus)------------------*//* microsoft, google, gnu, apple */
#define      YVIKEYS_M_FILE        'f'
#define      YVIKEYS_M_EDIT        'e'
#define      YVIKEYS_M_VIEW        'v'
#define      YVIKEYS_M_INSERT      'i'
#define      YVIKEYS_M_FORMAT      'o'
/*---(extended menus)-----------------*//* microsoft, google, gnu, apple */
#define      YVIKEYS_M_BUFFERS     'b'   /* replaces window */
#define      YVIKEYS_M_DATASET     'd'   /* includes filters menu */
#define      YVIKEYS_M_TOOLS       't'
/*---(more advanced)------------------*/
#define      YVIKEYS_M_LAYERS      'l'   /* gimp  */
#define      YVIKEYS_M_SELECT      's'   /* gimp  */
#define      YVIKEYS_M_MODIFY      'm'
#define      YVIKEYS_M_PALETTE     'p'
#define      YVIKEYS_M_LANG        'g'
#define      YVIKEYS_M_AUDIT       'a'
#define      YVIKEYS_M_EXECUTE     'x'   /* build, test, and execute (use) */
#define      YVIKEYS_M_CONFIG      'c'
/*---(perhaps useful)-----------------*/
#define      YVIKEYS_M_SCRIPT      'r'   /* keyboard macros and scripts      */
#define      YVIKEYS_M_SHOW        'h'   /* slideshow, present, printing     */
/*---(default)------------------------*/
#define      YVIKEYS_M_NONE        ' '
/*---(done)---------------------------*/


extern char yVIKEYS_ver     [500];

/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(debugging)------------*/
char*       yVIKEYS_version         (void);
char        yVIKEYS_debug           (char    a_flag  );

char        yVIKEYS_init            (char  a_mode);
char        yVIKEYS_quit            (void);
char        yVIKEYS_error           (void);
char        yVIKEYS_wrap            (void);

int         yVIKEYS_keys_nkey       (void);
char*       yVIKEYS_keys_last       (void);
char        yVIKEYS_dump_add        (char *a_name, void *a_provider);
uchar       yVIKEYS_main_input      (char  a_runmode, uchar a_key);
uchar       yVIKEYS_main_handle     (uchar a_key);
uchar       yVIKEYS_main_string     (uchar *a_keys);
char        yVIKEYS_main            (char *a_delay, char *a_update, void *a_altinput ());

char*       yVIKEYS__unit           (char *a_question, int a_num);

/*---(view)-----------------*/
char        yVIKEYS_view_font       (cchar  a_fixed);
char        yVIKEYS_view_config     (cchar *a_title, cchar *a_ver, cchar a_env, cint a_wide, cint a_tall, cint a_alt);
char        yVIKEYS_run_curses      (cchar *a_title, cchar *a_ver);
char        yVIKEYS_run_opengl      (cchar *a_title, cchar *a_ver, cint a_wide, cint a_tall);
char        yVIKEYS_view_resize     (cint   a_wide , cint   a_tall, cint a_alt);
char        yVIKEYS_view_setup      (cchar  a_part , cchar  a_type, cchar a_anchor, cint a_xmin, cint a_xlen, cint a_ymin, cint a_ylen, cint a_zmin, cint z_len, cchar a_color, void *a_drawer);
char        yVIKEYS_view_basic      (cchar  a_part , cchar  a_type, cchar a_anchor, cchar a_color, void *a_drawer);
char        yVIKEYS_view_simple     (cchar  a_part , cchar  a_color, void *a_drawer);
char        yVIKEYS_view_defsize    (cchar  a_part, cint a_wide, cint a_tall);
char        yVIKEYS_view_palette    (cint   a_deg, cchar *a_harm, cchar *a_sat, cchar *a_val);
char        yVIKEYS_view_colors     (cint   a_lef, cint a_bot, cint a_top, cint a_rig);
char        yVIKEYS_view_color      (cint   a_color, cfloat a_alpha);
char        yVIKEYS_view_color_adj  (cint   a_base, cint a_adj, cfloat a_alpha);
char        yVIKEYS_view_color_clear(cint   a_color);

char        yVIKEYS_view_showing    (cchar a_part);
char        yVIKEYS_view_size       (cchar  a_part , int   *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text);
char        yVIKEYS_view_bounds     (cchar  a_part , int   *a_xmin, int *a_xmax, int *a_xlen, int *a_ymin, int *a_ymax, int *a_ylen);
/*> char        yVIKEYS_view_coords     (cchar  a_part , int   *a_xmin, int *a_xlen, int *a_ymin, int *a_ylen);   <*/
char        yVIKEYS_view_text       (cchar  a_part , cchar *a_text);
char        yVIKEYS_view_all        (float a_mag);
char        yVIKEYS_view_type       (cchar a_part);
char        yVIKEYS_view_anchor     (cchar a_part);

char        yVIKEYS_view_option     (char a_part, char *a_opt, void *a_source, char *a_desc);
char        yVIKEYS_layer_add       (char *a_name, void *a_drawer, char *a_desc);

char        yVIKEYS_view_ribbon_clear (void);
char        yVIKEYS_view_ribbon     (char *a_cat, char *a_name);

char        yVIKEYS_menu            (char *a_prog, char *a_ver, int a_font, char *a_path, char *a_keys);
char        yVIKEYS_help            (void);


char        yVIKEYS_jump            (int a_buf, int a_x, int a_y, int a_z);

/*---(map mode)-------------*/
char        yVIKEYS_map_config      (char a_coord, void *a_mapper, void *a_locator, void *a_address);
char        yVIKEYS_map_refresh     (void);
char        yVIKEYS_clear_mapping   (tMAPPED *a_map);
char        yVIKEYS_uniform_mapping (char a_req, tMAPPED *a_map, int a_min, int a_max, int a_inc, int a_avail);
char        yVIKEYS_root            (int b, int x, int y, int z);
char        yVIKEYS_visual          (int b, int x, int y, int z);
char        yVIKEYS_bufs_config     (void *a_switcher, void *a_browser);

char        yVIKEYS_visu_coords     (int *b, int *xb, int *xe, int *yb, int *ye, int *z);
char        yVIKEYS_first           (int *a_b, int *a_x, int *a_y, int *a_z);
char        yVIKEYS_next            (int *a_b, int *a_x, int *a_y, int *a_z);

char        yVIKEYS_src_config      (void *a_saver);
char        yVIKEYS_source          (char *a_label, char *a_contents);


char        yVIKEYS_hist_marklist   (char *a_list);

char        yVIKEYS_mode            (void);
char        yVIKEYS_mode_change     (char a_mode, char *a_allow, char *a_mesg);
char        yVIKEYS_mode_formatter  (void *a_formatter, void *a_uniter);

/*---(speed)----------------*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yVIKEYS_speed_brief     (char   *a_text);
char        yVIKEYS_speed_desc      (char   *a_text);

/*---(scale)----------------*/
char        yVIKEYS_scale_brief     (char   *a_text);
char        yVIKEYS_scale_desc      (char   *a_text  );


char        yVIKEYS_prog_redraw     (void);
char        yVIKEYS_prog_script     (float a_beg, float  a_end, int    a_lines);
char        yVIKEYS_prog_cur        (char *a_pos, float *a_sec, float *a_scale, float *a_inc, int *a_line);

/*---(macros)---------------*/
char        yVIKEYS_macro_config    (void *a_loader, void *a_saver);

/*---(commands)-------------*/
char        yVIKEYS_menu_config     (char a_layout);
char        yVIKEYS_cmds_add        (uchar a_cat, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc);
char        yVIKEYS_cmds_direct     (char *a_command);
char        yVIKEYS_menu_alter      (uchar *a_path, char *a_keys);
char        yVIKEYS_menu_add        (uchar *a_path, char *a_name, char *a_keys);
int         yVIKEYS_menu_active     (uchar *a_path);
int         yVIKEYS_menu_grey       (uchar *a_path);
int         yVIKEYS_menu_hide       (uchar *a_path);

/*---(search)---------------*/
char        yVIKEYS_srch_config     (void *a_searcher, void *a_clearer);
char        yVIKEYS_srch_found      (char *a_label, int a_buf, int x, int y, int z);
char        yVIKEYS_srch_direct     (char *a_search);

/*---(files)----------------*/
char        yVIKEYS_whoami          (char *a_prog, char *a_ext, char *a_vernum, char *a_vertxt, char *a_full, char *a_desc);
char        yVIKEYS_file_add        (char  a_abbr, void *a_writer, void *a_reader);
char        yVIKEYS_file_write      (char a_abbr, void *a, void *b, void *c, void *d, void *e, void *f, void *g, void *h, void *i);

/*---(registers)------------*/
char        yVIKEYS_mreg_config     (void *a_clearer, void *a_copier, void *a_paster, void *a_regkill, void *a_exim);
char        yVIKEYS_mreg_add        (void *a_thing, char *a_label, char a_note);
char        yVIKEYS_mreg_inside     (int b, int x, int y, int z);

/*---(unit testing)---------*/
char        yVIKEYS_unit_reset      (void);



#endif
/*============================----end-of-source---============================*/
