/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */


static char    s_message  [LEN_RECD];


static char    (*s_formatter) (int a_major, int a_minor);
static char    (*s_uniter)    (int a_major, int a_minor);


/*===[[ FILE-WIDE VARIABLES ]]================================================*/
#define     MAX_STACK   50
static char    s_mode_stack    [MAX_STACK]; /* vi-like mode stack             */
static int     s_mode_depth;                /* depth of current mode stack    */
static char    s_mode_curr;                 /* current mode in stack          */



/*===[[ STRUCTURE ]]==========================================================*/
#define     MAX_MODES   100
typedef  struct  cMODE_INFO  tMODE_INFO;
struct cMODE_INFO {
   /*---(desc)--------------*/
   cchar       abbr;                   /* single character abbreviation       */
   cchar       type;                   /* type of mode, major, sub, micro, .. */
   cchar       show;                   /* show a message line (y/n)           */
   cchar       three       [LEN_SHORT];/* very short name                     */
   cchar       terse       [LEN_TERSE];/* short name                          */
   cchar       cat;                    /* category for reporting              */
   /*---(movement)----------*/
   char        allow       [LEN_LABEL];/* allowed mode transitions            */
   /*---(status)------------*/
   cchar       expect      [LEN_DESC]; /* expected prep and setup             */
   char        actual      [LEN_DESC]; /* actual prep and setup               */
   /*---(message)-----------*/
   cchar       desc        [LEN_DESC]; /* description of mode                 */
   char        mesg        [LEN_HUND];;/* informative message for display     */
   /*---(usage)-------------*/
   int         count;                  /* number of times used                */
   /*---(done)--------------*/
};
/*    MODE_    major mode, used for most working time
 *    SMOD_    sub-mode, supports major mode(s), used for short-period
 *    UMOD_    micro-mode, very simple supporting capability, used very quickly
 *    XMOD_    external-mode, custom to applicaton supported
 *    FMOD_    fundamental, not a usable mode, but a building block
 */
#define       MODE_MAJOR    'M'


static tMODE_INFO  s_modes [MAX_MODES] = {
   /*-abbr-------- type show  -tla- ---terse----- cat allow  ---expected-status----------------------    ---actual-status-------------------------   ---description--------------------------------------- ---message-------------------------------------------------------------------------------    use */
   /*---(fundamental)-----------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { FMOD_STATUS  , 'F', '-', "sta", "status"    , 0, ""  , "----- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "interal status and locking to prevent trouble"      , ""                                                                                        ,    0 },
   { FMOD_MODE    , 'F', '-', "mod", "modes"     , 0, ""  , "5---- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal mode setting and tracking"                 , ""                                                                                        ,    0 },
   { FMOD_VIEW    , 'F', '-', "vew", "viewing"   , 0, ""  , "5-:-- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal opengl and curses drawing manangement"     , ""                                                                                        ,    0 },
   { FMOD_FILE    , 'F', '-', "fil", "files"     , 0, ""  , "5---- p i :---- n 1---- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal file reading and writing"                  , ""                                                                                        ,    0 },
   /*---(map)-------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_MAP     , 'M', 'y', "MAP", "map"       , 1, ""  , "5---- p i ----- n 1---- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "map-mode providing 2D review of object collections" , "horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}"      ,    0 },
   { UMOD_VISUAL  , 'u', 'y', "vis", "visual"    , 1, ""  , "5f--- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "visual selection history and access"                , "index=a-zA-Z0-9   special=!?"                                                            ,    0 },
   { SMOD_MREG    , 's', 'y', "reg", "register"  , 1, ""  , "5f--- p i ----- n 1---- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "selecting specific registers for data movement"     , "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           ,    0 },
   { UMOD_MARK    , 'u', 'y', "mrk", "mark"      , 1, ""  , "5f:-- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "object and location marking"                        , "names=a-zA-Z0-9  actions=#!?_  special='[()]  wander=@  range=<>*"                       ,    0 },
   { SMOD_MACRO   , 's', 'y', "mac", "macro"     , 1, ""  , "5f:-- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "macro recording, execution, and maintenance"        , "run=a-z"                                                                                 ,    0 },
   { XMOD_FORMAT  , 'x', 'y', "frm", "format"    , 1, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "content formatting options"                         , ""                                                                                        ,    0 },
   { XMOD_UNITS   , 'x', 'y', "unt", "units"     , 1, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "content formatting options"                         , "off -, (+24) Y Z E P T G M K H D . d c m u n p f a z y (-24)"                            ,    0 },
   { XMOD_OBJECT  , 'x', 'y', "obj", "object"    , 1, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "object formatting and sizing options"               , ""                                                                                        ,    0 },
   { UMOD_MAP_UNDO, 's', 'y', "mun", "map-undo"  , 1, ""  , "5---- p i ----- n 1---- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "map level undo and redo"                            , ""                                                                                        ,    0 },
   /*---(source)----------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_SOURCE  , 'M', 'y', "SRC", "source"    , 2, ""  , "5---- p i ----- n 1---- r 0MV------- d o", "----- - - ----- - ----- - ---------- - -", "linewise review of textual content"                 , "hor=0HhlL$bBeEwW  g/z=sh,le  sel=vV\"  pul=yYdDxX  put=pP  chg=rRiIaA  fnd=fnN"          ,    0 },
   { SMOD_SREG    , 's', 'y', "srg", "src reg"   , 2, ""  , "5f:-- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "selecting specific registers for text movement"     , "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           ,    0 },
   { UMOD_SRC_REPL, 'u', 'y', "rpl", "replace"   , 2, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "linewise overtyping of content in source mode"      , "type over character marked with special marker"                                          ,    0 },
   { UMOD_SRC_INPT, 'u', 'y', "inp", "input"     , 2, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "linewise creation and editing of textual content"   , ""                                                                                        ,    0 },
   { UMOD_WANDER  , 'u', 'y', "wdr", "wander"    , 2, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "formula creation by moving to target cells"         , "modes={ret}{esc}"                                                                        ,    0 },
   { UMOD_SRC_UNDO, 's', 'y', "sun", "src-undo"  , 2, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "source level undo and redo"                         , ""                                                                                        ,    0 },
   /*---(power)-----------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_COMMAND , 'M', '-', "CMD", "command"   , 3, ""  , "5f--- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "command line capability for advanced actions"       , ""                                                                                        ,    0 },
   { MODE_SEARCH  , 'M', '-', "SCH", "search"    , 3, ""  , "5f--- p i ----- n 1---- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "search mode to find data and objects"               , ""                                                                                        ,    0 },
   { UMOD_HISTORY , 'u', 'y', "his", "history"   , 3, ""  , "5---- p i ----- n ----- r 0-V------- d o", "----- - - ----- - ----- - ---------- - -", "search and command history access"                  , ""                                                                                        ,    0 },
   { SMOD_FILTER  , 's', 'y', "fil", "filter"    , 3, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "process current/selection through external filter"  , "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      ,    0 },
   { SMOD_ERROR   , 's', 'y', "err", "errors"    , 3, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "display and action errors"                          , ""                                                                                        ,    0 },
   /*---(overall)---------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_GOD     , 'M', 'y', "GOD", "god"       , 4, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "god-mode allowing 3D omnicient viewing"             , "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                ,    0 },
   { MODE_OMNI    , 's', 'y', "OMN", "omni"      , 4, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "omnipotent 3D manipulation mode"                    , "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                ,    0 },
   { SMOD_BUFFER  , 's', 'y', "buf", "buffer"    , 4, ""  , "5---- p i ----- n 1---- r M--------- d o", "----- - - ----- - ----- - ---------- - -", "moving and selecting between buffers and windows"   , "select=0...9  modes={ret}(esc}"                                                          ,    0 },
   { SMOD_MENUS   , 's', 'y', "mnu", "menus"     , 4, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "interactive menu system for accessing commands"     , ""                                                                                        ,    0 },
   { SMOD_HINT    , 's', 'y', "hnt", "hint"      , 4, ""  , "5---- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "provides automatic and manual labeling hints"       , ""                                                                                        ,    0 },
   { UMOD_REPEAT  , 'u', 'y', "rep", "repeat"    , 4, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "accumulate multiplier"                              , "range 1-99"                                                                              ,    0 },
   /*---(time)------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_PROGRESS, 'M', 'y', "PRG", "progress"  , 5, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "progress timeline adding time dimension"            , "horz=0LlhH$  vert=_KkjJG speed=<.> scale=+-"                                             ,    0 },
   /*---(done)------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { 0            , '-', 'y', "bad", "bad mode"  , 0, ""  , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "default message when mode is not understood"        , "mode not understood"                                                                              ,    0 },
};
static int  s_nmode   = 0;


s_modechanges  [MAX_MODES][LEN_TERSE] = {
   /*    base              1               2               3               4               5               6               7               8               9               10       */
   /*12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234  12345678901234 */
   /*---(major mode-to-mode)---------*/
   { MODE_MAP      , UMOD_REPEAT   , MODE_GOD      , MODE_OMNI     , MODE_PROGRESS , MODE_SOURCE   , MODE_COMMAND  , MODE_SEARCH   , 0             , 0             , 0             },  /* all other modes */
   { MODE_GOD      , UMOD_REPEAT   , MODE_OMNI     , MODE_PROGRESS , MODE_SOURCE   , MODE_COMMAND  , MODE_SEARCH   , 0             , 0             , 0             , 0             },  /* all other modes */
   /*---(sub/umode/xmode)------------*/
   { MODE_MAP      , UMOD_VISUAL   , SMOD_MREG     , UMOD_MAP_UNDO , SMOD_BUFFER   , UMOD_MARK     , UMOD_MARK     , 0             , 0             , 0             , 0             },
   { MODE_MAP      , SMOD_MACRO    , XMOD_FORMAT   , XMOD_OBJECT   , SMOD_HINT     , SMOD_MENUS    , 0             , 0             , 0             , 0             , 0             },
   { MODE_PROGRESS , UMOD_REPEAT   , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             },
   { MODE_OMNI     , UMOD_REPEAT   , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             },
   /*---(source-related)-------------*/
   { MODE_SOURCE   , UMOD_REPEAT   , UMOD_SRC_INPT , UMOD_SRC_REPL , UMOD_SRC_UNDO , SMOD_SREG     , UMOD_WANDER   , 0             , 0             , 0             , 0             },
   { MODE_COMMAND  , UMOD_REPEAT   , UMOD_SRC_INPT , UMOD_SRC_REPL , UMOD_SRC_UNDO , SMOD_SREG     , UMOD_HISTORY  , 0             , 0             , 0             , 0             },
   { MODE_SEARCH   , UMOD_REPEAT   , UMOD_SRC_INPT , UMOD_SRC_REPL , UMOD_SRC_UNDO , SMOD_SREG     , UMOD_HISTORY  , 0             , 0             , 0             , 0             },
   /*---(other)----------------------*/
   { XMOD_FORMAT   , XMOD_UNITS    , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             },
   /*---(done)-----------------------*/
   { 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             , 0             },
};

static      char        s_majors       [MAX_MODES] = "";

/*> { XMOD_FORMAT  , '-', 'y', "frm", "format"    , "content formatting options"                         , ""                         ,    0, "ali=<|>[{^}] num=ifgcCaA$sSpP# tec=eEbBoOxXzZrR tim=tdTD dec=0-9 str=!-=_.+'\" wid=mhHlLnNwW tal=jJkK" },   <*/

#define       S_PRE        0
#define       S_PREP       6

#define       S_INIT       8

#define       S_NEEDS     10
#define       S_BASE      16

#define       S_CONF      18
#define       S_READY     24

#define       S_DEPS      26
#define       S_INTEG     37

#define       S_OPER      39



/*====================------------------------------------====================*/
/*===----                         shared functions                     ----===*/
/*====================------------------------------------====================*/
static void  o___SHARED__________o () { return; }

static  s_last       = '-';

char
MODE__by_abbr           (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   static int  n           =   -1;
   static char x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_PROG_M   yLOG_schar   (a_abbr);
   DEBUG_PROG_M   yLOG_schar   (x_abbr);
   /*---(short-cut)----------------------*/
   if (a_abbr == x_abbr) {
      DEBUG_PROG_M   yLOG_snote   ("fast");
      DEBUG_PROG_M   yLOG_sint    (n);
      return n;
   }
   /*---(lookup)-------------------------*/
   n = -1;
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == '-'    )  break;
      if (s_modes [i].abbr != a_abbr)   continue;
      n = i;
      DEBUG_PROG_M   yLOG_snote   ("found");
   }
   if (n <  0)  DEBUG_PROG_M   yLOG_snote   ("FAILED");
   if (n >= 0)  x_abbr = a_abbr;
   DEBUG_PROG_M   yLOG_sint    (n);
   s_last = n;
   /*---(complete)-----------------------*/
   return n;
}



/*====================------------------------------------====================*/
/*===----                      status checking                         ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_CHECK____o () { return; }

char
STATUS__check           (char a_abbr, char a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_index     =    0;
   char        x_loc       =    0;
   char        x_val       =    0;
   /*---(check mode)---------------------*/
   x_index  = MODE__by_abbr (a_abbr);
   if (x_index < 0)  {
      DEBUG_PROG_M   yLOG_snote   ("mode not found");
      return 0;
   }
   /*---(prepare)------------------------*/
   switch (a_target) {
   case 'p' :  x_loc = S_PREP ;  break;
   case 'i' :  x_loc = S_INIT ;  break;
   case 'n' :  x_loc = S_BASE ;  break;
   case 'r' :  x_loc = S_READY;  break;
   case 'o' :  x_loc = S_OPER ;  break;
   }
   /*---(look-up)------------------------*/
   DEBUG_PROG_M   yLOG_sint    (x_loc);
   x_val = s_modes [x_index].actual [x_loc];
   /*---(check)--------------------------*/
   DEBUG_PROG_M   yLOG_schar   (a_target);
   DEBUG_PROG_M   yLOG_schar   (x_val);
   if (x_val != a_target) {
      DEBUG_PROG_M   yLOG_snote   ("NOT READY");
      return 0;
   }
   DEBUG_PROG_M   yLOG_snote   ("ready");
   /*---(complete)-----------------------*/
   return 1;
}

char
STATUS_check_prep        (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__check  (a_abbr, 'p');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS_check_needs        (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   if (rc == 0)  rc = STATUS__check  (a_abbr, 'i');
   if (rc >  0)  rc = STATUS__check  (a_abbr, 'n');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS_operational         (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__check  (a_abbr, 'o');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        status filling                        ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_FILL_____o () { return; }

char
STATUS__filling         (char a_abbr, char a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_loc       =    0;
   char        x_len       =    0;
   int         n           =    0;
   int         i           =    0;
   int         c           =    0;
   /*---(prepare)------------------------*/
   switch (a_target) {
   case 'p' :
      rc    = STATUS__check  (a_abbr, 'i');
      x_loc = S_PRE;
      x_len =  5;
      break;
   case 'n' :
      rc    = STATUS__check  (a_abbr, 'i');
      x_loc = S_NEEDS;
      x_len =  5;
      break;
   case 'd' :
      rc    = STATUS__check  (a_abbr, 'r');
      x_loc = S_DEPS;
      x_len = 10;
      break;
   }
   /*---(check ready)--------------------*/
   if (rc < 1)   return rc;
   /*---(search for tasks)---------------*/
   for (n = 0; n < s_nmode; ++n) {
      for (i = 0; i < x_len; ++i) {
         if (s_modes [n].expect [x_loc + i] == '-')       continue;
         if (s_modes [n].expect [x_loc + i] != a_abbr)    continue;
         s_modes [n].actual [x_loc + i] = a_abbr;
         ++c;
         DEBUG_PROG_M   yLOG_schar   (s_modes [n].abbr);
      }
   }
   DEBUG_PROG_M   yLOG_snote   ("complete");
   DEBUG_PROG_M   yLOG_sint    (c);
   /*---(complete)-----------------------*/
   return c;
}

char
STATUS__prep_fill       (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__filling  (a_abbr, 'p');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS__need_fill       (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__filling  (a_abbr, 'n');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS__deps_fill       (char a_abbr)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__filling  (a_abbr, 'd');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      config specific                         ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_READY____o () { return; }

char
STATUS__conf_verify     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_LABEL] = "";
   char        x_expect    [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   /*---(check ready)--------------------*/
   n = s_last;
   strlcpy (x_expect, s_modes [n].expect + S_CONF, 5);
   strlcpy (x_actual, s_modes [n].actual + S_CONF, 5);
   if (strcmp (x_expect, x_actual) == 0) {
      s_modes [n].actual [S_READY] = 'r';
      DEBUG_PROG_M   yLOG_snote   ("marked ready, COMPLETE");
   } else {
      DEBUG_PROG_M   yLOG_snote   ("config not complete");
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        status updating                       ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_UPDATE___o () { return; }

char
STATUS__updating        (char a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_loc       =    0;
   char        x_len       =    0;
   char        x_mark      =    0;
   int         n           =    0;
   char        x_actual    [50] = "";
   char        x_expect    [50] = "";
   int         c           =    0;
   int         t           =    0;
   /*---(prepare)------------------------*/
   switch (a_target) {
   case 'p' :
      x_loc  = S_PRE;
      x_len  =  5;
      x_mark = S_PREP;
      break;
   case 'n' :
      x_loc  = S_NEEDS;
      x_len  =  5;
      x_mark = S_BASE;
      break;
   case 'd' :
      x_loc  = S_DEPS;
      x_len  = 10;
      x_mark = S_INTEG;
      break;
   case 'o' :
      x_loc  = S_PRE;
      x_len  = 30;
      x_mark = S_OPER;
      break;
   }
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [x_mark] == a_target) {
         ++t;
         continue;
      }
      strlcpy (x_expect, s_modes [n].expect + x_loc, x_len);
      strlcpy (x_actual, s_modes [n].actual + x_loc, x_len);
      if (strcmp (x_expect, x_actual) != 0) continue;
      s_modes [n].actual [x_mark] = a_target;
      ++c;
      ++t;
      DEBUG_PROG_M   yLOG_schar   (s_modes [n].abbr);
   }
   DEBUG_PROG_M   yLOG_snote   ("complete");
   DEBUG_PROG_M   yLOG_sint    (c);
   DEBUG_PROG_M   yLOG_sint    (t);
   /*---(complete)-----------------------*/
   return 0;
}

char
STATUS__prep_checkall   (void)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__updating  ('p');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS__need_checkall   (void)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__updating  ('n');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS__deps_checkall   (void)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__updating  ('d');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
STATUS__oper_checkall   (void)
{
   char        rc          =    0;
   DEBUG_PROG_M   yLOG_senter  (__FUNCTION__);
   rc = STATUS__updating  ('o');
   DEBUG_PROG_M   yLOG_sexit   (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      status setting                          ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_SETTING__o () { return; }

char
STATUS_init_set         (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(ready)--------------------------*/
   rc = STATUS_check_prep (a_abbr);
   --rce;  if (rc == 0) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update this status)-------------*/
   n = s_last;
   DEBUG_PROG   yLOG_note    ("mark as initialized");
   s_modes [n].actual [S_INIT ] = 'i';
   /*---(mark others lines)--------------*/
   STATUS__prep_fill      (a_abbr);
   STATUS__prep_checkall  ();
   STATUS__need_fill      (a_abbr);
   STATUS__need_checkall  ();
   STATUS__conf_verify    ();
   STATUS__deps_fill      (a_abbr);
   STATUS__deps_checkall  ();
   STATUS__oper_checkall  ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS_conf_set         (char a_abbr, char a_step)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(ready)--------------------------*/
   rc = STATUS_check_needs (a_abbr);
   --rce;  if (rc == 0) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(status)-------------------------*/
   n = s_last;
   /*---(ready)--------------------------*/
   DEBUG_PROG   yLOG_char    ("a_step"    , a_step);
   s_modes [n].actual [S_CONF + a_step - '1'] = a_step;
   /*---(mark others lines)--------------*/
   STATUS__prep_checkall  ();
   STATUS__need_checkall  ();
   STATUS__conf_verify    ();
   STATUS__deps_fill      (a_abbr);
   STATUS__deps_checkall  ();
   STATUS__oper_checkall  ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      status overall                          ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS_PROG_____o () { return; }

char
STATUS__purge           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   /*---(count status)-------------------*/
   for (i = 0; i < s_nmode; ++i) {
      DEBUG_PROG   yLOG_schar   (s_modes [i].abbr);
      strlcpy (s_modes [i].actual, "----- - - ----- - ----- - ---------- - -", LEN_DESC);
      s_modes [i].count = 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
STATUS_init             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(count status)-------------------*/
   s_nmode = 0;
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == 0  )  break;
      if (s_modes [i].abbr == '-')  break;
      ++s_nmode;
   }
   DEBUG_PROG   yLOG_value   ("s_nmode" , s_nmode);
   /*---(update)-------------------------*/
   STATUS__purge ();
   STATUS__prep_checkall ();
   STATUS_init_set       (FMOD_STATUS);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS_wrap             (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(purge)--------------------------*/
   STATUS__purge ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS_dump             (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   char        t           [LEN_LABEL];
   int         c           =    0;
   int         M           =    0;
   int         F           =    0;
   int         S           =    0;
   int         U           =    0;
   int         x           =    0;
   int         x_cat       =   -1;
   /*---(list)---------------------------*/
   fprintf (a_file, "yVIKEYS, capability status/readiness reporting                                          (:dump status)\n");
   fprintf (a_file, "                   ---expected-----------------------------   ---actual-------------------------------\n");
   fprintf (a_file, "---mode---   a c   prep--- i needs-- conf--- deps-------- o   prep--- i needs-- conf--- deps-------- o\n");
   for (n = 0; n < s_nmode; ++n) {
      if (x_cat != s_modes [n].cat)  fprintf (a_file, "\n");
      if (strchr ("MF", s_modes [n].type) != NULL)  strlcpy (t, s_modes [n].terse, LEN_LABEL);
      else                                          sprintf (t, " %-9.9s", s_modes [n].terse);
      fprintf (a_file, "%-10.10s   %c %c   %s   %s\n", t, s_modes [n].abbr, s_modes [n].type, s_modes [n].expect, s_modes [n].actual);
      x_cat = s_modes [n].cat;
      ++c;
      switch (s_modes [n].type) {
      case 'F' : ++F;  break;
      case 'M' : ++M;  break;
      case 's' : ++S;  break;
      case 'u' : ++U;  break;
      case 'x' : ++x;  break;
      }
   }
   fprintf (a_file, "\n");
   fprintf (a_file, "---mode---   a c   prep--- i needs-- conf--- deps-------- o   prep--- i needs-- conf--- deps-------- o\n");
   fprintf (a_file, "                   ---expected-----------------------------   ---actual-------------------------------\n");
   fprintf (a_file, "status mode count %d (fund %d, major %d, sub %d, micro %d, extern %d)\n", c, F, M, S, U, x);
   fprintf (a_file, "\n");
   fprintf (a_file, "prep  = must be initialized before this mode can initialize\n");
   fprintf (a_file, "i     = initialized and ready to configure\n");
   fprintf (a_file, "needs = must be initialized before this mode can configure\n");
   fprintf (a_file, "conf  = externally called steps in the configuration process\n");
   fprintf (a_file, "deps  = must be operational before this mode can be\n");
   fprintf (a_file, "o     = operational and ready to use\n");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      mode stack handling                     ----===*/
/*====================------------------------------------====================*/
static void  o___MODE_STACK______o () { return; }

char         /*--> prepare mode stack for use ------------[--------[--------]-*/
MODE_init          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         j           =    0;
   char        n           =    0;
   char        t           [5] = "";
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (FMOD_MODE)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   strlcpy (s_majors, "", MAX_MODES);
   /*---(identify majors)----------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes  [i].type != MODE_MAJOR)    continue;
      sprintf (t, "%c", s_modes  [i].abbr);
      strlcat (s_majors, t, MAX_MODES);
   }
   /*---(clear transitions)--------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == 0)  break;
      strcpy (s_modes [i].allow, "");
   }
   /*---(load transitions)---------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modechanges [i][0] == 0)  break;
      n = MODE__by_abbr (s_modechanges [i][0]);
      /*> printf ("%c %2d\n", s_modechanges [i][0], n);                               <*/
      for (j = 1; j < LEN_TERSE; ++j) {
         if (s_modechanges [i][j] == 0)  break;
         sprintf (t, "%c", s_modechanges [i][j]);
         strlcat (s_modes [n].allow, t, LEN_LABEL);
      }
   }
   /*---(clear stack)--------------------*/
   for (i = 0; i < MAX_STACK; ++i) {
      s_mode_stack [i] = '-';
   }
   /*---(clear controls)-----------------*/
   s_mode_depth =  0;
   s_mode_curr = '-';
   /*---(custom functions)---------------*/
   s_formatter = NULL;
   s_uniter    = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (FMOD_MODE);
   /*---(go to default mode)-------------*/
   MODE_enter (MODE_MAP);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add a mode to the stack ---------------[--------[--------]-*/
MODE_enter         (char a_mode)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   char        x_mode      = '-';
   int         x_index     = -1;
   /*---(check for dup)------------------*/
   /*> if (s_mode_stack [s_mode_depth] == a_mode)  return 1;                            <*/
   /*---(validate mode)------------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == '-'    )  break;
      if (s_modes [i].abbr != a_mode )  continue;
      ++s_modes  [i].count;
      x_mode  = a_mode;
   }
   --rce;  if (x_mode  == '-')  return rce;
   /*---(check if allowed)---------------*/
   if (s_mode_depth > 0)  {
      for (i = 0; i < MAX_MODES; ++i) {
         if (s_modes [i].abbr == '-'    )  break;
         if (s_modes [i].abbr != s_mode_curr)  continue;
         x_index = i;
      }
      --rce;  if (x_index <   0 )  return rce;
      --rce;  if (strchr (s_modes [x_index].allow, a_mode) == NULL)  return rce;
   }
   /*---(add mode)-----------------------*/
   --rce;  if (s_mode_depth >= MAX_STACK)   return rce;
   s_mode_stack [s_mode_depth] = a_mode;
   ++s_mode_depth;
   /*---(set global mode)----------------*/
   s_mode_curr = a_mode;
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_exit          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_mode      = '-';
   /*---(check stack)--------------------*/
   --rce;  if (s_mode_depth <= 0)  return rce;
   --s_mode_depth;
   s_mode_stack [s_mode_depth] = x_mode;
   x_mode = s_mode_stack [s_mode_depth - 1];
   /*---(set global mode)----------------*/
   s_mode_curr = x_mode;
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_curr          (void)
{
   return s_mode_curr;
}

char
yVIKEYS_mode       (void)
{
   return s_mode_curr;
}

char
MODE_prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_mode      = '-';
   /*---(check stack)--------------------*/
   --rce;  if (s_mode_depth <= 1)                return '-';
   /*---(grab previous)------------------*/
   x_mode = s_mode_stack [s_mode_depth - 2];
   if (strchr (s_majors, x_mode) != NULL)   return x_mode;
   /*---(go back one more)---------------*/
   --rce;  if (s_mode_depth <= 2)                return '-';
   x_mode = s_mode_stack [s_mode_depth - 3];
   /*---(complete)-----------------------*/
   return x_mode;
}

char
MODE_not           (char a_mode)
{
   if (a_mode != s_mode_stack [s_mode_depth - 1]) return -1;
   return 0;
}

char       /*----: list the current mode stack -------------------------------*/
MODE_status        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           =   0;
   char        t           [10];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "modes (%d)", s_mode_depth);
   for (i = 0; i < 8; ++i) {
      sprintf (t, " %c", s_mode_stack [i]);
      strlcat (a_list, t, LEN_FULL);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char*
MODE_message       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_major     = ' ';
   char        x_minor     = ' ';
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == '-'   )   break;
      if (s_modes [i].abbr == s_mode_curr)  break;
   }
   if (s_modes  [i].type == MODE_MAJOR)  {
      x_major = s_mode_curr;
   } else {
      x_major = MODE_prev  ();
      x_minor = s_mode_curr;
   }
   snprintf (s_message, LEN_RECD, "[%c%c] %-3.3s : %s\n", x_major, x_minor, s_modes [i].three, s_modes [i].mesg);
   return s_message;
}

char
yVIKEYS_mode_change  (char a_mode, char *a_allow, char *a_mesg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   int         x_mode      = -1;
   /*---(find)---------------------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_modes [i].abbr == '-'   )   break;
      if (s_modes [i].abbr != a_mode)   continue;
      x_mode = i;
   }
   --rce;  if (x_mode < 0)  return rce;
   /*---(update)-------------------------*/
   if (a_allow  != NULL) {
      strlcpy (s_modes  [x_mode].allow, a_allow, 25);
   }
   if (a_mesg  != NULL) {
      strlcpy (s_modes  [x_mode].mesg , a_mesg , LEN_FULL);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        repeat keys                           ----===*/
/*====================------------------------------------====================*/
static void  o___REPEAT__________o () { return; }

static   int   s_request  = 0;
static   int   s_repeat   = 0;

static   int   s_level                 = 0;;
static   int   s_rep     [LEN_LABEL];
static   uchar s_src     [LEN_LABEL];
static   int   s_pos     [LEN_LABEL];

char
REPEAT_done             (void)
{
   myVIKEYS.repeating = '-';
   s_request          = 0;
   return 0;
}

char
REPEAT_reset            (void)
{
   myVIKEYS.repeating = '-';
   s_request      = 0;
   s_repeat       = 0;
   yvikeys_macro_zero ();
   return 0;
}

char
REPEAT_macro            (void)
{
   myVIKEYS.repeating = '-';
   s_request      = 0;
   s_repeat       = 0;
   return 0;
}

char
REPEAT_init             (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           =   0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (UMOD_REPEAT)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(setup)--------------------------*/
   REPEAT_reset ();
   /*---(group)--------------------------*/
   for (i = 0; i < LEN_LABEL; ++i) {
      s_pos [i]  = -1;
      s_src [i]  = '-';
      s_rep [i]  = -1;
   }
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_REPEAT);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> accumulate multiplier --------------[ ------ [ge.A43.214.63]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
REPEAT_umode            (uchar a_major, uchar a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_REPEAT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(turn off repeating)--------------------*/
   myVIKEYS.repeating = '-';
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ENTER || a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("aborting repeat");
      MODE_exit  ();
      s_repeat = 0;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for major)-----------------------*/
   if (s_repeat ==  0 && strchr ("123456789", a_major) != NULL) {
      DEBUG_USER   yLOG_note    ("assign starting repeat");
      s_repeat  = a_major - '0';
      DEBUG_USER   yLOG_value   ("s_repeat"  , s_repeat);
   }
   /*---(check for minor)-----------------------*/
   if (strchr ("0123456789",  a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("increment repeat");
      s_repeat *= 10;
      s_repeat += a_minor - '0';
      DEBUG_USER   yLOG_value   ("s_repeat"  , s_repeat);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(pass through)-------------------*/
   DEBUG_USER   yLOG_note    ("prepare repeat for use");
   DEBUG_USER   yLOG_value   ("s_repeat"  , s_repeat);
   --s_repeat;
   if (s_repeat <  0) s_repeat =  0;
   s_request = s_repeat;
   myVIKEYS.repeating = 'y';
   /*---(complete)-----------------------*/
   MODE_exit  ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_minor;
}

char REPEAT_normal      (void) { if (s_repeat < 1) return 0; else return 1; } 
char REPEAT_decrement   (void) { if (s_repeat > 0)  --s_repeat; }
int  REPEAT_count       (void) { return s_repeat; }
int  REPEAT_original    (void) { return s_request;}
int  REPEAT_use         (void) { int a = s_repeat; s_repeat = 0; return a; }
char REPEAT_not         (void) { if (myVIKEYS.repeating == '-')  return 1; if (s_repeat == s_request) return 1; return 0; }



/*====================------------------------------------====================*/
/*===----                        repeat keys                           ----===*/
/*====================------------------------------------====================*/
static void  o___GROUPING________o () { return; }

char
REPEAT_group_beg        (void)
{
   /*---(defense)------------------------*/
   if (s_repeat <= 0)  return 0;
   /*---(populate new level)-------------*/
   ++s_level;
   IF_MACRO_PLAYING {
      yvikeys_macro_pos (&(s_src [s_level]), &(s_pos [s_level]));
   } else {
      s_pos [s_level] = yvikeys_keys_gpos ();
      s_src [s_level] = '´';
   }
   s_rep [s_level] = s_repeat;
   /*---(clear normal repeat)------------*/
   s_repeat = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
REPEAT_group_end        (void)
{
   /*---(defense)------------------------*/
   if (s_level <= 0)  return 0;
   /*---(check for done)-----------------*/
   if (s_rep [s_level] <= 0) {
      s_rep [s_level] = -1;
      s_src [s_level] = '-';
      s_pos [s_level] = -1;
      --s_level;
   }
   /*---(return to beginning)------------*/
   else {
      --(s_rep [s_level]);
      if (s_src [s_level] == (uchar) '´')  KEYS_repos (s_pos [s_level]);
      else                                 yvikeys_macro_repos (s_pos [s_level]);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       custom modes                           ----===*/
/*====================------------------------------------====================*/
static void  o___CUSTOM__________o () { return; }

char
yVIKEYS_mode_formatter  (void *a_formatter, void *a_uniter)
{
   s_formatter = a_formatter;
   s_uniter    = a_uniter;
   return 0;
}

char         /*-> keys for formatting micro-mode -----[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
FORMAT_xmode            (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (XMOD_FORMAT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for standard controls)----*/
   switch (a_minor) {
   case   G_KEY_RETURN : case   G_KEY_ESCAPE :
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;   /* escape  */
   }
   /*---(call-out)-----------------------*/
   DEBUG_USER   yLOG_point   ("formatter" , s_formatter);
   if (s_formatter != NULL)  rc = s_formatter (a_major, a_minor);
   else                      MODE_exit   ();
   /*---(units)--------------------------*/
   if (a_minor == 'F') {
      MODE_enter (XMOD_UNITS);
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> keys for units micro-mode ----------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
UNITS_xmode             (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   char       *x_valid     = "-YZEPTGMKHD.dcmunpfazy";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (XMOD_UNITS)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(call-out)-----------------------*/
   if (strchr (x_valid, a_minor) != NULL) {
      DEBUG_USER   yLOG_point   ("uniter"    , s_uniter);
      if (s_uniter != NULL)  rc = s_uniter (a_major, a_minor);
   } else {
      rc = rce;
   }
   MODE_exit   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        unit testing                          ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
STATUS__unit            (char *a_question, char a_abbr)
{
   int         n           =    0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "STATUS unit      : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "actual"       )  == 0) {
      n = MODE__by_abbr (a_abbr);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "STATUS actual    : %c  %s", a_abbr, s_modes [n].actual);
   }
   else if (strcmp (a_question, "expect"       )  == 0) {
      n = MODE__by_abbr (a_abbr);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "STATUS expect    : %c  %s", a_abbr, s_modes [n].expect);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
MODE__unit             (char *a_question)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_FULL];
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MODE unit        : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "stack"        )  == 0) {
      MODE_status (x_list);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MODE stack       : %s", x_list);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
REPEAT__unit           (char *a_question)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        t           [LEN_FULL];
   char        x_list      [LEN_FULL]  = "";
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "REPEAT unit      : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "count"        )  == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "REPEAT count     : %d", s_repeat);
   }
   else if (strcmp (a_question, "groups"       )  == 0) {
      for (i = 1; i <= 5; ++i) {
         if (s_pos [i] >= 0)  sprintf (t, "%02d/%c/%03d", s_rep [i], s_src [i], s_pos [i]);
         else                 sprintf (t, "--/-/---");
         strlcat (x_list, t, LEN_FULL);
         if (i < 5)  strlcat (x_list, ", ", LEN_FULL);
      }
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "REPEAT groups    : %1d %s", s_level, x_list);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}






/*============================----end-of-source---============================*/
