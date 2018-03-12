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
   char        abbr;                   /* single character abbreviation       */
   char        type;                   /* type of mode, major, sub, micro, .. */
   char        show;                   /* show a message line (y/n)           */
   char        three       [ 5];       /* very short name                     */
   char        terse       [10];       /* short name                          */
   char        cat;                    /* category for reporting              */
   /*---(movement)----------*/
   char        allow       [25];       /* allowed mode transitions            */
   /*---(status)------------*/
   char        expect      [50];       /* expected prep and setup             */
   char        actual      [50];       /* actual prep and setup               */
   /*---(message)-----------*/
   char        desc        [50];       /* description of mode                 */
   char        mesg        [LEN_STR];  /* informative message for display     */
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
   /*-abbr-------- type show  -tla- ---terse----- cat ---allowable---------    ---expected-status----------------------    ---actual-status-------------------------   ---description--------------------------------------- ---message-------------------------------------------------------------------------------    use */
   /*---(fundamental)--------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { FMOD_STATUS  , 'F', '-', "sta", "status"    , 0, ""                    , "----- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "interal status and locking to prevent trouble"      , ""                                                                                        ,    0 },
   { FMOD_MODE    , 'F', '-', "mod", "modes"     , 0, ""                    , "5---- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal mode setting and tracking"                 , ""                                                                                        ,    0 },
   { FMOD_VIEW    , 'F', '-', "vew", "viewing"   , 0, ""                    , "5-:-- p i ----- n ----- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal opengl and curses drawing manangement"     , ""                                                                                        ,    0 },
   { FMOD_FILE    , 'F', '-', "fil", "files"     , 0, ""                    , "5---- p i :---- n 1---- r ---------- d o", "----- - - ----- - ----- - ---------- - -", "internal file reading and writing"                  , ""                                                                                        ,    0 },
   /*---(map)----------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_MAP     , 'M', 'y', "MAP", "map"       , 1, "GvS:/\"b\'$oe\\,@9"  , "5---- p i ----- n 1---- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "map-mode providing 2D review of object collections" , "horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}"      ,    0 },
   { UMOD_VISUAL  , 'u', 'y', "vis", "visual"    , 1, ""                    , "5f--- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "visual selection history and access"                , "index=a-zA-Z0-9   special=!?"                                                            ,    0 },
   { SMOD_REGISTER, 's', 'y', "reg", "register"  , 1, ""                    , "5f--- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "selecting specific registers for data movement"     , "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           ,    0 },
   { UMOD_MARK    , 'u', 'y', "mrk", "mark"      , 1, ""                    , "5f:-- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "object and location marking"                        , "names=a-zA-Z0-9  actions=#!?_  special='[()]  wander=@  range=<>*"                       ,    0 },
   { SMOD_MACRO   , 's', 'y', "mac", "macro"     , 1, ""                    , "5f:-- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "macro recording, execution, and maintenance"        , "run=a-z"                                                                                 ,    0 },
   { XMOD_FORMAT  , 'x', 'y', "frm", "format"    , 1, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "content formatting options"                         , ""                                                                                        ,    0 },
   { XMOD_OBJECT  , 'x', 'y', "obj", "object"    , 1, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "object formatting and sizing options"               , ""                                                                                        ,    0 },
   { SMOD_MUNDO   , 's', 'y', "mun", "map-undo"  , 1, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "map level undo and redo"                            , ""                                                                                        ,    0 },
   /*---(source)-------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_SOURCE  , 'M', 'y', "SRC", "source"    , 2, "isrte9"              , "5---- p i ----- n 1---- r 0MV------- d o", "----- - - ----- - ----- - ---------- - -", "linewise review of textual content"                 , "hor=0HhlL$bBeEwW  g/z=sh,le  sel=vV\"  pul=yYdDxX  put=pP  chg=rRiIaA  fnd=fnN"          ,    0 },
   { SMOD_TEXTREG , 's', 'y', "trg", "text reg"  , 2, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "selecting specific registers for text movement"     , "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           ,    0 },
   { UMOD_REPLACE , 'u', 'y', "rep", "replace"   , 2, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "linewise overtyping of content in source mode"      , "type over character marked with special marker"                                          ,    0 },
   { UMOD_INPUT   , 'u', 'y', "inp", "input"     , 2, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "linewise creation and editing of textual content"   , ""                                                                                        ,    0 },
   { UMOD_WANDER  , 'u', 'y', "wdr", "wander"    , 2, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "formula creation by moving to target cells"         , "modes={ret}{esc}"                                                                        ,    0 },
   { SMOD_SUNDO   , 's', 'y', "sun", "src-undo"  , 2, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "source level undo and redo"                         , ""                                                                                        ,    0 },
   /*---(power)--------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_COMMAND , 'M', '-', "CMD", "command"   , 3, "isrte9h"             , "5f--- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "command line capability for advanced actions"       , ""                                                                                        ,    0 },
   { MODE_SEARCH  , 'M', '-', "SCH", "search"    , 3, "isrte9h"             , "5f--- p i ----- n 1---- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "search mode to find data and objects"               , ""                                                                                        ,    0 },
   { UMOD_HISTORY , 'u', 'y', "his", "history"   , 3, ""                    , "5---- p i ----- n ----- r 0-V------- d o", "----- - - ----- - ----- - ---------- - -", "search and command history access"                  , ""                                                                                        ,    0 },
   { SMOD_FILTER  , 's', 'y', "fil", "filter"    , 3, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "process current/selection through external filter"  , "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      ,    0 },
   { SMOD_ERROR   , 's', 'y', "err", "errors"    , 3, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "display and action errors"                          , ""                                                                                        ,    0 },
   /*---(overall)------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_GOD     , 'M', 'y', "GOD", "god"       , 4, "P"                   , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "god-mode allowing 3D omnicient viewing"             , "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                ,    0 },
   { MODE_OMNI    , 's', 'y', "OMN", "omni"      , 4, "P"                   , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "omnipotent 3D manipulation mode"                    , "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                ,    0 },
   { SMOD_BUFFER  , 's', 'y', "buf", "buffer"    , 4, ""                    , "5---- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "moving and selecting between buffers and windows"   , "select=0...9  modes={ret}(esc}"                                                          ,    0 },
   { SMOD_MENUS   , 's', 'y', "mnu", "menus"     , 4, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "menu system"                                        , ""                                                                                        ,    0 },
   { SMOD_HINT    , 's', 'y', "hnt", "hint"      , 4, ""                    , "5---- p i ----- n ----- r 0M-------- d o", "----- - - ----- - ----- - ---------- - -", "provides automatic and manual labeling hints"       , ""                                                                                        ,    0 },
   { UMOD_REPEAT  , 'u', 'y', "rep", "repeat"    , 4, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "accumulate multiplier"                              , "range 1-99"                                                                              ,    0 },
   /*---(time)---------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { MODE_PROGRESS, 'M', 'y', "PRG", "progress"  , 5, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "progress timeline adding time dimension"            , "horz=0LlhH$  vert=_KkjJG speed=<.> scale=+-"                                             ,    0 },
   /*---(done)---------------------------------------------------------*//* prep--- - needs-- conf--- deps-------- -    prep--- - needs-- conf--- deps-------- -*/
   { 0            , '-', 'y', "bad", "bad mode"  , 0, ""                    , "5---- p i ----- n ----- r 0--------- d o", "----- - - ----- - ----- - ---------- - -", "default message when mode is not understood"        , "mode not understood"                                                                              ,    0 },
};
static int  s_nmode   = 0;

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

char
MODE__by_abbr           (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   static int  n           =   -1;
   static char x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_senter  (__FUNCTION__);
   DEBUG_MODE   yLOG_schar   (a_abbr);
   DEBUG_MODE   yLOG_schar   (x_abbr);
   /*---(short-cut)----------------------*/
   if (a_abbr == x_abbr) {
      DEBUG_MODE   yLOG_snote   ("short-cut");
      DEBUG_MODE   yLOG_sint    (n);
      DEBUG_MODE   yLOG_sexit   (__FUNCTION__);
      return n;
   }
   /*---(lookup)-------------------------*/
   n = -1;
   for (i = 0; i < s_nmode; ++i) {
      if (s_modes [i].abbr == '-'    )  break;
      if (s_modes [i].abbr != a_abbr)   continue;
      n = i;
      DEBUG_MODE   yLOG_snote   ("FOUND");
   }
   DEBUG_MODE   yLOG_sint    (n);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_sexit   (__FUNCTION__);
   return n;
}

char
STATUS_prep_done        (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(find)---------------------------*/
   n = MODE__by_abbr (a_abbr);
   DEBUG_MODE   yLOG_value   ("n"         , n);
   if (n < 0)  {
      DEBUG_MODE   yLOG_note    ("can not find mode");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = s_modes [n].actual [S_PREP ];
   DEBUG_MODE   yLOG_char    ("status"    , rc);
   if (rc != 'p') {
      DEBUG_MODE   yLOG_note    ("not 'p' so not ready");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
STATUS_needs_met        (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(find)---------------------------*/
   n = MODE__by_abbr (a_abbr);
   DEBUG_MODE   yLOG_value   ("n"         , n);
   if (n < 0)  {
      DEBUG_MODE   yLOG_note    ("can not find mode");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = s_modes [n].actual [S_INIT];
   DEBUG_MODE   yLOG_char    ("status"    , rc);
   if (rc != 'i') {
      DEBUG_MODE   yLOG_note    ("not 'i' so not ready");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = s_modes [n].actual [S_BASE];
   DEBUG_MODE   yLOG_char    ("status"    , rc);
   if (rc != 'n') {
      DEBUG_MODE   yLOG_note    ("not 'n' so not ready");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
STATUS_complete         (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(find)---------------------------*/
   n = MODE__by_abbr (a_abbr);
   DEBUG_MODE   yLOG_value   ("n"         , n);
   if (n < 0)  {
      DEBUG_MODE   yLOG_note    ("can not find mode");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = s_modes [n].actual [S_OPER];
   DEBUG_MODE   yLOG_char    ("status"    , rc);
   if (rc != 'o') {
      DEBUG_MODE   yLOG_note    ("not 'o' so not ready");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
STATUS__prep_fill       (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   int         i           =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(check initialized)--------------*/
   n = MODE__by_abbr (a_abbr);
   if (s_modes [n].actual [S_INIT ] != 'i') {
      DEBUG_MODE   yLOG_note    ("not intialized");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      for (i = 0; i < 5; ++i) {
         if (s_modes [n].expect [S_PRE + i] == '-')       continue;
         if (s_modes [n].expect [S_PRE + i] != a_abbr)    continue;
         DEBUG_MODE   yLOG_value   ("needed for", s_modes [n].abbr);
         s_modes [n].actual [S_PRE + i] = a_abbr;
         ++c;
      }
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__need_fill       (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   int         i           =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(check initialized)--------------*/
   n = MODE__by_abbr (a_abbr);
   if (s_modes [n].actual [S_INIT ] != 'i') {
      DEBUG_MODE   yLOG_note    ("not intialized");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      for (i = 0; i < 5; ++i) {
         if (s_modes [n].expect [S_NEEDS + i] == '-')       continue;
         if (s_modes [n].expect [S_NEEDS + i] != a_abbr)    continue;
         DEBUG_MODE   yLOG_value   ("needed for", s_modes [n].abbr);
         s_modes [n].actual [S_NEEDS + i] = a_abbr;
         ++c;
      }
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__deps_fill       (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   int         i           =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   DEBUG_MODE   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(check ready)--------------------*/
   n = MODE__by_abbr (a_abbr);
   if (s_modes [n].actual [S_INIT ] != 'i') {
      DEBUG_MODE   yLOG_note    ("not intialized");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (s_modes [n].actual [S_READY] != 'r') {
      DEBUG_MODE   yLOG_note    ("not configured/ready");
      DEBUG_MODE   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      for (i = 0; i < 10; ++i) {
         if (s_modes [n].expect [S_DEPS + i] == '-')       continue;
         if (s_modes [n].expect [S_DEPS + i] != a_abbr)    continue;
         DEBUG_MODE   yLOG_value   ("needed for", s_modes [n].abbr);
         s_modes [n].actual [S_DEPS + i] = a_abbr;
         ++c;
      }
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__prep_checkall   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_LABEL] = "";
   char        x_expect    [LEN_LABEL] = "";
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [S_PREP ] == 'p')  continue;
      strlcpy (x_expect, s_modes [n].expect + S_PRE, 5);
      strlcpy (x_actual, s_modes [n].actual + S_PRE, 5);
      if (strcmp (x_expect, x_actual) != 0) continue;
      s_modes [n].actual [S_PREP] = 'p';
      DEBUG_MODE   yLOG_char    ("ready"     , s_modes [n].abbr);
      ++c;
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__need_checkall   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_LABEL] = "";
   char        x_expect    [LEN_LABEL] = "";
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [S_BASE ] == 'n')  continue;
      strlcpy (x_expect, s_modes [n].expect + S_NEEDS, 5);
      strlcpy (x_actual, s_modes [n].actual + S_NEEDS, 5);
      if (strcmp (x_expect, x_actual) != 0) continue;
      s_modes [n].actual [S_BASE ] = 'n';
      DEBUG_MODE   yLOG_char    ("ready"     , s_modes [n].abbr);
      ++c;
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__conf_checkall   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_LABEL] = "";
   char        x_expect    [LEN_LABEL] = "";
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [S_READY] == 'r')  continue;
      if (s_modes [n].actual [S_INIT ] != 'i')  continue;
      if (s_modes [n].actual [S_BASE ] != 'n')  continue;
      strlcpy (x_expect, s_modes [n].expect + S_CONF, 5);
      strlcpy (x_actual, s_modes [n].actual + S_CONF, 5);
      if (strcmp (x_expect, x_actual) != 0) continue;
      s_modes [n].actual [S_READY] = 'r';
      DEBUG_MODE   yLOG_char    ("ready"     , s_modes [n].abbr);
      ++c;
      STATUS__deps_fill (s_modes [n].abbr);
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__deps_checkall   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_DESC ] = "";
   char        x_expect    [LEN_DESC ] = "";
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [S_INTEG] == 'd')  continue;
      strlcpy (x_expect, s_modes [n].expect + S_DEPS, 10);
      strlcpy (x_actual, s_modes [n].actual + S_DEPS, 10);
      if (strcmp (x_expect, x_actual) != 0) continue;
      s_modes [n].actual [S_INTEG] = 'd';
      DEBUG_MODE   yLOG_char    ("integ"     , s_modes [n].abbr);
      ++c;
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS__oper_checkall   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =   -1;
   char        x_actual    [LEN_DESC ] = "";
   char        x_expect    [LEN_DESC ] = "";
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_MODE   yLOG_enter   (__FUNCTION__);
   /*---(search for prep tasks)----------*/
   for (n = 0; n < s_nmode; ++n) {
      if (s_modes [n].actual [S_OPER ] == 'o')  continue;
      if (s_modes [n].actual [S_INTEG] != 'd')  continue;
      strlcpy (x_expect, s_modes [n].expect, LEN_DESC);
      strlcpy (x_actual, s_modes [n].actual, LEN_DESC);
      if (strncmp (x_expect, x_actual, 30) != 0) continue;
      s_modes [n].actual [S_OPER ] = 'o';
      ++c;
      DEBUG_MODE   yLOG_char    ("oper"      , s_modes [n].abbr);
   }
   DEBUG_MODE   yLOG_note    ("review complete");
   DEBUG_MODE   yLOG_value   ("found"     , c);
   /*---(complete)-----------------------*/
   DEBUG_MODE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS_init_set         (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   /*---(ready)--------------------------*/
   rc = STATUS_prep_done (a_abbr);
   --rce;  if (rc == 0)  return rce;
   /*---(find)---------------------------*/
   n = MODE__by_abbr (a_abbr);
   --rce;  if (n < 0)  return rce;
   /*---(status)-------------------------*/
   s_modes [n].actual [S_INIT ] = 'i';
   /*---(mark others)--------------------*/
   STATUS__prep_fill      (a_abbr);
   STATUS__need_fill      (a_abbr);
   STATUS__prep_checkall  ();
   STATUS__need_checkall  ();
   STATUS__conf_checkall  ();
   STATUS__deps_checkall  ();
   STATUS__oper_checkall  ();
   /*---(complete)-----------------------*/
   return 0;
}

char
STATUS_conf_set         (char a_abbr, char a_step)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   /*---(ready)--------------------------*/
   rc = STATUS_needs_met (a_abbr);
   --rce;  if (rc == 0)  return rce;
   /*---(find)---------------------------*/
   n = MODE__by_abbr (a_abbr);
   --rce;  if (n < 0)  return rce;
   /*---(status)-------------------------*/
   s_modes [n].actual [S_CONF + a_step - '1'] = a_step;
   /*---(mark others)--------------------*/
   STATUS__need_checkall  ();
   STATUS__conf_checkall  ();
   STATUS__deps_checkall  ();
   STATUS__oper_checkall  ();
   /*---(complete)-----------------------*/
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
   STATUS__prep_checkall ();
   STATUS_init_set       (FMOD_STATUS);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
STATUS_dump             (char *a_what)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   FILE       *f           = NULL;
   int         n           =    0;
   char        t           [LEN_LABEL];
   int         c           =    0;
   int         M           =    0;
   int         F           =    0;
   int         S           =    0;
   int         U           =    0;
   int         x           =    0;
   int         x_cat       =   -1;
   /*---(open file)----------------------*/
   f = fopen ("/root/z_gehye/vi_clip.txt", "w");
   if (f == NULL)  return rce;
   /*---(list)---------------------------*/
   fprintf (f, "yVIKEYS, capability status/readiness reporting                                          (:dump status)\n");
   fprintf (f, "                   ---expected-----------------------------   ---actual-------------------------------\n");
   fprintf (f, "---mode---   a c   prep--- i needs-- conf--- deps-------- o   prep--- i needs-- conf--- deps-------- o\n");
   for (n = 0; n < s_nmode; ++n) {
      if (x_cat != s_modes [n].cat)  fprintf (f, "\n");
      if (strchr ("MF", s_modes [n].type) != NULL)  strlcpy (t, s_modes [n].terse, LEN_LABEL);
      else                                          sprintf (t, " %-9.9s", s_modes [n].terse);
      fprintf (f, "%-10.10s   %c %c   %s   %s\n", t, s_modes [n].abbr, s_modes [n].type, s_modes [n].expect, s_modes [n].actual);
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
   fprintf (f, "\n");
   fprintf (f, "---mode---   a c   prep--- i needs-- conf--- deps-------- o   prep--- i needs-- conf--- deps-------- o\n");
   fprintf (f, "                   ---expected-----------------------------   ---actual-------------------------------\n");
   fprintf (f, "status mode count %d (fund %d, major %d, sub %d, micro %d, extern %d)\n", c, F, M, S, U, x);
   fprintf (f, "\n");
   fprintf (f, "prep  = must be initialized before this mode can initialize\n");
   fprintf (f, "i     = initialized and ready to configure\n");
   fprintf (f, "needs = must be initialized before this mode can configure\n");
   fprintf (f, "conf  = externally called steps in the configuration process\n");
   fprintf (f, "deps  = must be operational before this mode can be\n");
   fprintf (f, "o     = operational and ready to use\n");
   /*---(close)--------------------------*/
   fclose (f);
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
   int         i           = 0;
   char        t           [5]         = "";
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_prep_done  (FMOD_MODE)) {
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
   /*---(clear stack)--------------------*/
   for (i = 0; i < MAX_STACK; ++i) {
      s_mode_stack [i] = '-';
   }
   /*---(clear controls)-----------------*/
   s_mode_depth =  0;
   s_mode_curr = '-';
   /*---(custom functions)---------------*/
   s_formatter = NULL;
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
      strlcat (a_list, t, LEN_STR);
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
      strlcpy (s_modes  [x_mode].mesg , a_mesg , LEN_STR);
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
   MACRO_zero ();
   return 0;
}

char
REPEAT_init             (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_prep_done  (UMOD_REPEAT)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(setup)--------------------------*/
   REPEAT_reset ();
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
/*===----                       custom modes                           ----===*/
/*====================------------------------------------====================*/
static void  o___CUSTOM__________o () { return; }

char
yVIKEYS_mode_formatter  (void *a_formatter)
{
   s_formatter = a_formatter;
   return 0;
}

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
FORMAT_smode            (int a_major, int a_minor)
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
      snprintf (yVIKEYS__unit_answer, LEN_STR, "STATUS actual    : %c  %s", a_abbr, s_modes [n].actual);
   }
   else if (strcmp (a_question, "expect"       )  == 0) {
      n = MODE__by_abbr (a_abbr);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "STATUS expect    : %c  %s", a_abbr, s_modes [n].expect);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
MODE__unit             (char *a_question)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_STR];
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MODE unit        : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "stack"        )  == 0) {
      MODE_status (x_list);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MODE stack       : %s", x_list);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
REPEAT__unit           (char *a_question)
{
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "REPEAT unit      : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "count"        )  == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "REPEAT count     : %d", s_repeat);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}






/*============================----end-of-source---============================*/
