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



extern char yVIKEYS_ver     [500];

/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(debugging)------------*/
char*       yVIKEYS_version           (void);
char        yVIKEYS_debug             (char    a_flag  );

/*---(mode stack)-----------*/
char        yVIKEYS_mode_init         (void);
char        yVIKEYS_mode_enter        (char    a_mode  );
char        yVIKEYS_mode_exit         (void);
char        yVIKEYS_mode_curr         (void);
char        yVIKEYS_mode_prev         (void);
char        yVIKEYS_mode_not          (char    a_mode  );
char        yVIKEYS_mode_list         (char   *a_list  );
char        yVIKEYS_mode_mesg         (char   *a_mesg   , char   *a_cmd);
char        yVIKEYS_mode_change       (char a_mode, char *a_allow, char *a_mesg);
/*---(view)-----------------*/
char        yVIKEYS_view_init         (void *a_sizer, char *a_title, char *a_ver);
char        yVIKEYS_view_resize       (cchar a_format, cchar *a_title, cint a_wide, cint a_tall, cchar a_adapt);
char        yVIKEYS_view_text         (cchar a_part, cchar *a_text);
char        yVIKEYS_view_corners      (cchar a_part, int *a_left, int *a_bott, int *a_wide, int *a_tall, cchar *a_text);
char        yVIKEYS_view_title        (void);
char        yVIKEYS_view_command      (void);
char        yVIKEYS_view_status       (void);
char        yVIKEYS_view_ribbon_clear (void);
char        yVIKEYS_view_ribbon_add   (char *a_cat, char *a_name);
char        yVIKEYS_view_ribbon       (void);

/*---(map mode)-------------*/
char        yVIKEYS_map_init          (void *a_col_mapper, void *a_row_mapper);

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
char        yVIKEYS_cmds_init       (void);
char        yVIKEYS_cmds_wrap       (void);
int         yVIKEYS_cmds_find       (char *a_name);
char        yVIKEYS_cmds_add        (char a_cat, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc);
char        yVIKEYS_cmds_start      (void);
char        yVIKEYS_cmds_clear      (void);
char*       yVIKEYS_cmds_curr       (void);
char        yVIKEYS_cmds_exec       (void);
char        yVIKEYS_cmds_direct     (char *a_command);
char        yVIKEYS_cmds_mode       (char a_major, char a_minor);

/*---(search)---------------*/
char        yVIKEYS_srch_init       (void *a_searcher, void *a_clearer);
char        yVIKEYS_srch_wrap       (void);
char        yVIKEYS_srch_start      (void);
char        yVIKEYS_srch_clear      (void);
char*       yVIKEYS_srch_curr       (void);
char        yVIKEYS_srch_exec       (void);
char        yVIKEYS_srch_direct     (char *a_search);
char        yVIKEYS_srch_mode       (char a_major, char a_minor);



#endif
/*============================----end-of-source---============================*/
