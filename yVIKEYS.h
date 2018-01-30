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
#define     MODE_VISUAL    'V'
#define     MODE_SOURCE    'S'
#define     MODE_INPUT     'I'
#define     MODE_COMMAND   ':'
#define     MODE_SEARCH    '/'
/*---(sub-modes)--------------s----------*/
#define     SMOD_REPEAT    '9'    /* accumulate multiplier                    */
#define     SMOD_ERROR     'e'    /* error reporting and actions              */
#define     SMOD_SELECT    's'    /* visual selection of chars in source mode */
#define     SMOD_REPLACE   'r'    /* replacing characters in source mode      */
#define     SMOD_REGISTER  '"'    /* register actions                         */
#define     SMOD_TEXTREG   't'    /* text register actions                    */
#define     SMOD_BUFFER    ','    /* selecting buffers                        */
#define     SMOD_WANDER    'w'    /* formula creation by pointing             */
#define     SMOD_FORMAT    '$'    /* content formatting                       */
#define     SMOD_OBJECT    'o'    /* object formatting                        */
#define     SMOD_MARK      '\''   /* location and object marking              */
#define     SMOD_MACRO     '@'    /* macro processing                         */
#define     SMOD_HINT      'h'    /* hint labels                              */
#define     SMOD_CHAR      'c'    /* linewise character finding               */
#define     SMOD_MENUS     '\\'   /* show menu system                         */


typedef  const char cchar;
typedef  const int  cint;
typedef  const float  cfloat;
/*---(mode)------------*/
#define     MACRO_OFF          '-'      /* normal keyboard input              */
#define     MACRO_RUN          'M'      /* macro running with redisplay       */
#define     MACRO_DELAY        'D'      /* macro delay playback controls      */
#define     MACRO_PLAYBACK     'P'      /* macro under playback controls      */
#define     MACRO_RECORD       'r'      /* macro recording                    */
/*---(conditions)------*/
#define     IF_MACRO_OFF         if (yVIKEYS_macro_get_mode () == MACRO_OFF      ) 
#define     IF_MACRO_RUN         if (yVIKEYS_macro_get_mode () == MACRO_RUN      ) 
#define     IF_MACRO_NOT_RUN     if (yVIKEYS_macro_get_mode () != MACRO_RUN      ) 
#define     IF_MACRO_DELAY       if (yVIKEYS_macro_get_mode () == MACRO_DELAY    ) 
#define     IF_MACRO_PLAYBACK    if (yVIKEYS_macro_get_mode () == MACRO_PLAYBACK ) 
#define     IF_MACRO_MOVING      if (yVIKEYS_macro_get_mode () == MACRO_RUN      || yVIKEYS_macro_get_mode () == MACRO_DELAY   ) 
#define     IF_MACRO_NOT_PLAYING if (yVIKEYS_macro_get_mode () == MACRO_OFF      || yVIKEYS_macro_get_mode () == MACRO_RECORD  )
#define     IF_MACRO_PLAYING     if (yVIKEYS_macro_get_mode () != MACRO_OFF      && yVIKEYS_macro_get_mode () != MACRO_RECORD  )
#define     IF_MACRO_RECORDING   if (yVIKEYS_macro_get_mode () == MACRO_RECORD   ) 
#define     IF_MACRO_ON          if (yVIKEYS_macro_get_mode () != MACRO_OFF      ) 
/*---(setting)---------*/
#define     SET_MACRO_OFF        yVIKEYS_macro_set_mode (MACRO_OFF);
#define     SET_MACRO_RUN        yVIKEYS_macro_set_mode (MACRO_RUN);
#define     SET_MACRO_PLAYBACK   yVIKEYS_macro_set_mode (MACRO_PLAYBACK);
#define     SET_MACRO_DELAY      yVIKEYS_macro_set_mode (MACRO_DELAY);
#define     SET_MACRO_RECORD     yVIKEYS_macro_set_mode (MACRO_RECORD);


#define LEN_MAP      10000
typedef  struct cMAPPED  tMAPPED;
struct cMAPPED {
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
tMAPPED     s_colmap;
tMAPPED     s_rowmap;
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
#define      YVIKEYS_FLOAT       'F'



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


extern char yVIKEYS_ver     [500];

/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(debugging)------------*/
char*       yVIKEYS_version           (void);
char        yVIKEYS_debug             (char    a_flag  );

char        yVIKEYS_init              (void);
char        yVIKEYS_quit              (void);
char        yVIKEYS_wrap              (void);

/*---(mode stack)-----------*/
char        yVIKEYS_mode_init         (void);
char        yVIKEYS_mode_enter        (char a_mode);
char        yVIKEYS_mode_exit         (void);
char        yVIKEYS_mode_curr         (void);
char        yVIKEYS_mode_prev         (void);
char        yVIKEYS_mode_not          (char    a_mode  );
char        yVIKEYS_mode_list         (char   *a_list  );
char        yVIKEYS_mode_mesg         (char   *a_mesg   , char   *a_cmd);
char        yVIKEYS_mode_change       (char a_mode, char *a_allow, char *a_mesg);
/*---(view)-----------------*/
char        yVIKEYS_view_init         (cchar *a_title, cchar *a_ver , cint a_wide, cint a_tall, cint a_alt);
char        yVIKEYS_view_resize       (cint   a_wide , cint   a_tall, cint a_alt);
char        yVIKEYS_view_setup        (cchar  a_part , cchar  a_type, cchar a_anchor, cint a_xmin, cint a_xlen, cint a_ymin, cint a_ylen, cint a_zmin, cint z_len, cchar a_color, void *a_drawer);
char        yVIKEYS_view_moderate     (cchar  a_part , cchar  a_type, cchar a_anchor, cchar a_color, void *a_drawer);
char        yVIKEYS_view_simple       (cchar  a_part , cchar  a_color, void *a_drawer);
char        yVIKEYS_view_palette      (cint a_deg, cchar *a_harm, cchar *a_sat, cchar *a_val);
char        yVIKEYS_view_colors       (cint a_lef, cint a_bot, cint a_top, cint a_rig);
char        yVIKEYS_view_color        (cint a_color, cfloat a_alpha);
char        yVIKEYS_view_color_adj    (cint a_base, cint a_adj, cfloat a_alpha);
char        yVIKEYS_view_color_clear  (cint a_color);

char        yVIKEYS_view_size         (cchar  a_part , int   *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text);
char        yVIKEYS_view_bounds       (cchar  a_part , int   *a_xmin, int *a_xmax, int *a_ymin, int *a_ymax);
char        yVIKEYS_view_coords       (cchar  a_part , int   *a_xmin, int *a_xlen, int *a_ymin, int *a_ylen);
char        yVIKEYS_view_text         (cchar  a_part , cchar *a_text);
char        yVIKEYS_view_wrap         (void);
char        yVIKEYS_view_all          (float a_mag);

char        yVIKEYS_view_ribbon_clear (void);
char        yVIKEYS_view_ribbon       (char *a_cat, char *a_name);

/*---(map mode)-------------*/
char        yVIKEYS_map_init          (char a_coord, void *a_col_mapper, void *a_row_mapper);
char        yVIKEYS_map_mode          (char a_major, char a_minor);

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
char        yVIKEYS_macro_init      (void *a_loader, void *a_saver);
char        yVIKEYS_macro_wrap      (void);
char        yVIKEYS_macro_reset     (void);
char        yVIKEYS_macro_rec_key   (char a_key);
char        yVIKEYS_macro_rec_end   (void);
char        yVIKEYS_macro_define    (char *a_string);
char        yVIKEYS_macro_exec_beg  (char a_name);
char        yVIKEYS_macro_exec_adv  (void);
char        yVIKEYS_macro_exec_wait (void);
char        yVIKEYS_macro_exec_key  (void);
char        yVIKEYS_macro_exec_play (char a_key);
char        yVIKEYS_macro_smode     (char a_major, char a_minor);
char        yVIKEYS_macro_get_mode  ();
char        yVIKEYS_macro_set_mode  (char a_mode);

/*---(sub-modes)------------*/
char        yVIKEYS_repeat_init     (void);
char        yVIKEYS_repeat_umode    (char a_major, char a_minor);
char        yVIKEYS_repeat_norm     (void);
char        yVIKEYS_repeat_dec      (void);
char        yVIKEYS_repeat_macro    (void);
int         yVIKEYS_repeat_value    (void);

/*---(commands)-------------*/
char        yVIKEYS_cmds_add        (char a_cat, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc);
char        yVIKEYS_cmds_direct     (char *a_command);

/*---(search)---------------*/
char        yVIKEYS_srch_init       (void *a_searcher, void *a_clearer);
char        yVIKEYS_srch_found      (void *a_match);
char        yVIKEYS_srch_direct     (char *a_search);



#endif
/*============================----end-of-source---============================*/
