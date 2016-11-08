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



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define YVIKEYS_VER_NUM   "0.5j"
#define YVIKEYS_VER_TXT   "added basic key movement functions, tested on arachne"


/*===[[ CONSTANTS ]]==========================================================*/
/*---(major modes)-----------------------*/
#define     MODE_GOD       'G'
#define     MODE_PROGRESS  'P'
#define     MODE_MAP       'M'
#define     MODE_VISUAL    'V'
#define     MODE_SOURCE    'S'
#define     MODE_INPUT     'I'
#define     MODE_COMMAND   ':'
#define     MODE_SEARCH    '/'
/*---(sub-modes)--------------s----------*/
#define     SMOD_REPLACE   'r'    /* replacing characters in source mode      */
#define     SMOD_SELECT    's'    /* visual selection of chars in source mode */
#define     SMOD_TEXTREG   't'    /* text register actions                    */
#define     SMOD_ERROR     'e'    /* error reporting and actions              */
#define     SMOD_REGISTER  '"'    /* register actions                         */
#define     SMOD_BUFFER    ','    /* selecting buffers                        */
#define     SMOD_WANDER    '@'    /* formula creation by pointing             */
#define     SMOD_FORMAT    '$'    /* content formatting                       */
#define     SMOD_OBJECT    'o'    /* object formatting                        */
#define     SMOD_MARK      '\''   /* location and object marking              */
#define     SMOD_MENUS     '\\'   /* show menu system                         */


extern char yVIKEYS_ver     [500];

/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(debugging)------------*/
char*       yVIKEYS_version    (void);
char        yVIKEYS_debug      (char    a_flag  );

/*---(mode stack)-----------*/
char        yVIKEYS_mode_init  (void);
char        yVIKEYS_mode_enter (char    a_mode  );
char        yVIKEYS_mode_exit  (void);
char        yVIKEYS_mode_curr  (void);
char        yVIKEYS_mode_prev  (void);
char        yVIKEYS_mode_not   (char    a_mode  );
char        yVIKEYS_mode_list  (char   *a_list  );
char        yVIKEYS_mode_mesg  (char   *a_mesg   , char   *a_cmd);

/*---(speed)----------------*/
char        yVIKEYS_speed_set  (char   *a_code   , double *a_waitns);
char        yVIKEYS_speed_stop (double *a_waitns);
char        yVIKEYS_speed_play (double *a_waitns);
char        yVIKEYS_speed_more (double *a_waitns);
char        yVIKEYS_speed_less (double *a_waitns);
char        yVIKEYS_speed_desc (char   *a_text  );
char        yVIKEYS_speed_adv  (double *a_pos   );

/*---(scale)----------------*/
char        yVIKEYS_scale_set  (char   *a_code   , float  *a_inc);
char        yVIKEYS_scale_more (float  *a_inc   );
char        yVIKEYS_scale_less (float  *a_inc   );
char        yVIKEYS_scale_desc (char   *a_text  );
char        yVIKEYS_scale_base (float  *a_multi  , char   *a_base);

/*---(keys)-----------------*/
char        yVIKEYS_keys_horz  (char a_minor, double *a_base, double a_inc, double a_min, double a_max);
char        yVIKEYS_keys_vert  (char a_minor, double *a_base, double a_inc, double a_min, double a_max);
char        yVIKEYS_keys_zoom  (char a_minor, double *a_base, double a_inc, double a_min, double a_max);


#endif
/*============================----end-of-source---============================*/
