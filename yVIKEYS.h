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
#define YVIKEYS_VER_NUM   "0.5b"
#define YVIKEYS_VER_TXT   "add all the common MODE functions from gyges/arachne"


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



/*===[[ FUNCTION PROTOTYPES ]]================================================*/
/*---(mode stack)-----------*/
char        MODE_init          (void);
char        MODE_enter         (char  a_mode);
char        MODE_return        (void);
char        MODE_curr          (void);
char        MODE_prev          (void);
char        MODE_not           (char  a_mode);
char        MODE_list          (char *a_list);
char        MODE_message       (void);



#endif
/*============================----end-of-source---============================*/
