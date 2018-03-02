/*============================----beg-of-source---============================*/

/*===[[ HEADER ]]=============================================================*/

/*   focus         : (--) -----
 *   niche         : (--) -----
 *   application   : yVIKEYS
 *   purpose       : efficient, versatle, standard keyboard mini-language
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : none
 *   size          : small       (less than 500 slocL)
 * 
 *   author        : rsheatherly
 *   created       : 2010-01     (when i started defining this standard)
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable)
 * 
 */
/*===[[ SUMMARY ]]============================================================*/

/*   yVIKEYS is a heatherly-specific standard mini-language for operating all
 *   my custom applications using a pure keyboard interface.  while it has deep roots
 *   in vi/vim, i expect it to adapt to be greater that just that.
 *
 */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS
#define yKIKEYS yes


/*===[[ CONSTANTS ]]==========================================================*/
/*---(major modes)-----------------------*/
#define     MODE_GOD       'G'
#define     MODE_OMNI      'O'
#define     MODE_PROGRESS  'P'
#define     MODE_MAP       'M'
#define     MODE_SOURCE    'S'
#define     MODE_COMMAND   ':'
#define     MODE_SEARCH    '/'
#define     MODES_ONELINE  "S:/"
#define     MODES_EDITING  "S:/trIw"
/*---(sub-modes)--------------s----------*/
#define     SMOD_REPEAT    '9'    /* accumulate multiplier                    */
#define     SMOD_ERROR     'e'    /* error reporting and actions              */

#define     SMOD_REPLACE   'r'    /* replacing characters in source mode      */
#define     SMOD_REGISTER  '"'    /* register actions                         */
#define     SMOD_TEXTREG   't'    /* text register actions                    */
#define     SMOD_INPUT     'i'    /* direct input of text                     */
#define     SMOD_WANDER    'w'    /* formula creation by pointing             */
#define     SMOD_SUNDO     's'    /* incremental text change undo/redo        */

#define     SMOD_BUFFER    ','    /* selecting buffers                        */
#define     SMOD_VISUAL    'v'    /* visual selection history                 */
#define     SMOD_MUNDO     'm'    /* incremental map change undo/redo         */
#define     SMOD_FORMAT    '$'    /* content formatting                       */
#define     SMOD_OBJECT    'o'    /* object formatting                        */
#define     SMOD_MARK      '\''   /* location and object marking              */
#define     SMOD_HINT      'h'    /* hint labels                              */

#define     SMOD_MACRO     '@'    /* macro processing                         */
#define     SMOD_MENUS     '\\'   /* show menu system                         */


typedef  const unsigned char  cuchar;
typedef  unsigned char        uchar;
typedef  const char           cchar;
typedef  const int            cint;
typedef  const float          cfloat;


#define LEN_MAP      10000
typedef  struct cMAPPED  tMAPPED;
struct cMAPPED {
   /*---(identity)-------------*/
   char        which;                       /* x, y, z, or t                  */
   /*---(lefts)----------------*/
   int         gmin;                        /* global min, used or not        */
   int         amin;                        /* min of all used space          */
   int         lmin;                        /* min for col/row                */
   int         prev;                        /* prev change for "end"          */
   /*---(current)--------------*/
   int         map         [LEN_MAP];       /* full col/row map               */
   /*---(rights)---------------*/
   int         next;                        /* next change for "end"          */
   int         lmax;                        /* max for col/row                */
   int         amax;                        /* max of all used space          */
   int         gmax;                        /* global max, used or not        */
   /*---(screen)---------------*/
   int         beg;                         /* beginning of screen            */
   int         cur;                         /* current index                  */
   int         end;                         /* end of screen                  */
   int         len;                         /* screen len                     */
   int         avail;                       /* full size of screen            */
   int         tend;                        /* true end of screen             */
   /*---(grids)----------------*/
   int         gbeg;                        /* grid at start of screen        */
   int         gcur;                        /* current grid position          */
   int         gend;                        /* grid at end of screen          */
   /*---(done)-----------------*/
};
tMAPPED     g_xmap;
tMAPPED     g_ymap;
tMAPPED     g_zmap;
tMAPPED     g_tmap;

#define     YVIKEYS_XMAP     'x'
#define     YVIKEYS_YMAP     'y'
#define     YVIKEYS_ZMAP     'z'
#define     YVIKEYS_TMAP     't'

#define     YVIKEYS_EMPTY   -666
#define     YVIKEYS_INIT    'i'
#define     YVIKEYS_UPDATE  'u'


#define     YVIKEYS_RIGHT    'r'
#define     YVIKEYS_OFFICE   'o'

#define     YVIKEYS_DEPTH    '3'
#define     YVIKEYS_FLAT     'O'
#define     YVIKEYS_DISABLE  'X'

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



#define      YVIKEYS_AUTO        'a'
#define      YVIKEYS_CUSTOM      'c'
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



extern char yVIKEYS_ver     [500];

/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(debugging)------------*/
char*       yVIKEYS_version           (void);
char        yVIKEYS_debug             (char    a_flag  );

char        yVIKEYS_init              (void);
char        yVIKEYS_quit              (void);
char        yVIKEYS_error             (void);
char        yVIKEYS_wrap              (void);

uchar       yVIKEYS_main_input        (char  a_runmode, uchar a_key);
uchar       yVIKEYS_main_handle       (uchar a_key);

/*---(view)-----------------*/
char        yVIKEYS_view_font         (cchar  a_fixed);
char        yVIKEYS_view_config       (cchar *a_title, cchar *a_ver, cchar a_env, cint a_wide, cint a_tall, cint a_alt);
char        yVIKEYS_view_resize       (cint   a_wide , cint   a_tall, cint a_alt);
char        yVIKEYS_view_setup        (cchar  a_part , cchar  a_type, cchar a_anchor, cint a_xmin, cint a_xlen, cint a_ymin, cint a_ylen, cint a_zmin, cint z_len, cchar a_color, void *a_drawer);
char        yVIKEYS_view_moderate     (cchar  a_part , cchar  a_type, cchar a_anchor, cchar a_color, void *a_drawer);
char        yVIKEYS_view_simple       (cchar  a_part , cchar  a_color, void *a_drawer);
char        yVIKEYS_view_defsize      (cchar  a_part, cint a_wide, cint a_tall);
char        yVIKEYS_view_palette      (cint   a_deg, cchar *a_harm, cchar *a_sat, cchar *a_val);
char        yVIKEYS_view_colors       (cint   a_lef, cint a_bot, cint a_top, cint a_rig);
char        yVIKEYS_view_color        (cint   a_color, cfloat a_alpha);
char        yVIKEYS_view_color_adj    (cint   a_base, cint a_adj, cfloat a_alpha);
char        yVIKEYS_view_color_clear  (cint   a_color);

char        yVIKEYS_view_size         (cchar  a_part , int   *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text);
char        yVIKEYS_view_bounds       (cchar  a_part , int   *a_xmin, int *a_xmax, int *a_ymin, int *a_ymax);
char        yVIKEYS_view_coords       (cchar  a_part , int   *a_xmin, int *a_xlen, int *a_ymin, int *a_ylen);
char        yVIKEYS_view_text         (cchar  a_part , cchar *a_text);
char        yVIKEYS_view_all          (float a_mag);

char        yVIKEYS_view_option       (char a_part, char *a_opt, void *a_source, char *a_desc);
char        yVIKEYS_layer_add         (char *a_name, void *a_drawer, char *a_desc);

char        yVIKEYS_view_ribbon_clear (void);
char        yVIKEYS_view_ribbon       (char *a_cat, char *a_name);

/*---(map mode)-------------*/
char        yVIKEYS_map_config        (char a_coord, void *a_mapper);
char        yVIKEYS_map_refresh       (void);
char        yVIKEYS_root              (int x, int y, int z);
char        yVIKEYS_visual            (int x, int y, int z);

char        yVIKEYS_src_config        (void *a_saver);
char        yVIKEYS_source            (char *a_label, char *a_contents);


char        yVIKEYS_hint_config     (void *a_locator);
char        yVIKEYS_hist_marklist   (char *a_list);

char        yVIKEYS_mode_change       (char a_mode, char *a_allow, char *a_mesg);
char        yVIKEYS_mode_formatter    (void *a_formatter);

/*---(speed)----------------*/
char        yVIKEYS_speed_set    (char   *a_code   , double *a_waitns);
char        yVIKEYS_speed_stop   (double *a_waitns);
char        yVIKEYS_speed_play   (double *a_waitns);
char        yVIKEYS_speed_more   (double *a_waitns);
char        yVIKEYS_speed_less   (double *a_waitns);
char        yVIKEYS_speed_desc   (char   *a_text  );
char        yVIKEYS_speed_adv    (double *a_pos   );

/*---(scale)----------------*/
char        yVIKEYS_scale_set    (char   *a_code   , double *a_inc);
char        yVIKEYS_scale_more   (double *a_inc   );
char        yVIKEYS_scale_less   (double *a_inc   );
char        yVIKEYS_scale_desc   (char   *a_text  );
char        yVIKEYS_scale_base   (double *a_multi  , char   *a_base);

/*---(keys)-----------------*/
char        yVIKEYS_keys_horz    (char a_minor, double *a_base, double a_inc, double a_min, double a_max);
char        yVIKEYS_keys_vert    (char a_minor, double *a_base, double a_inc, double a_min, double a_max);
char        yVIKEYS_keys_zoom    (char a_minor, double *a_base, double a_inc, double a_min, double a_max);

/*---(macros)---------------*/
char        yVIKEYS_macro_config    (void *a_loader, void *a_saver);

/*---(commands)-------------*/
char        yVIKEYS_cmds_add        (char a_cat, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc);
char        yVIKEYS_cmds_direct     (char *a_command);

/*---(search)---------------*/
char        yVIKEYS_srch_config     (void *a_searcher, void *a_clearer);
char        yVIKEYS_srch_found      (char *a_label );
char        yVIKEYS_srch_direct     (char *a_search);



#endif
/*============================----end-of-source---============================*/
