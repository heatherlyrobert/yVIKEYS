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
#define     MAX_MODES   50
#define     MAX_STACK   50





/*===[[ FILE-WIDE VARIABLES ]]================================================*/
static char    s_modes    [MAX_STACK];      /* vi-like mode stack             */
static int     s_nmode;                     /* depth of current mode stack    */
static char    s_cmode;                     /* current mode in stack          */



/*===[[ STRUCTURE ]]==========================================================*/
typedef  struct  cMODE_INFO  tMODE_INFO;
struct cMODE_INFO {
   char        abbr;                   /* single character abbreviation       */
   char        major;                  /* major mode (y/n)                    */
   char        show;                   /* show a message line (y/n)           */
   char        three       [ 5];       /* very short name                     */
   char        terse       [10];       /* short name                          */
   char        desc        [50];       /* description of mode                 */
   char        allow       [25];       /* allowed mode transitions            */
   int         count;                  /* number of times used                */
   char        mesg        [LEN_STR];  /* informative message for display     */
};
static tMODE_INFO  s_mode_info [MAX_MODES] = {
   /*-a-- -maj show --tla- ---terse----- ---description---------------------------------------- ----------------------------,----- 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789- */
   /*---(major modes)--------------------*/
   { 'G' , 'y', 'y', "GOD", "god"       , "god-mode allowing 3D omnicient viewing"             , "P"                        ,    0, "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                },
   { 'O' , 'y', 'y', "OMN", "omni"      , "omnipotent 3D manipulation mode"                    , "P"                        ,    0, "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                },
   { 'P' , 'y', 'y', "PRG", "progress"  , "progress timeline adding time dimension"            , ""                         ,    0, "horz=0LlhH$  vert=_KkjJG speed=<.> scale=+-"                                             },
   { 'M' , 'y', 'y', "MAP", "map"       , "map-mode providing 2D review of object collections" , "GVSI:/\"b\'$oe\\,@9"      ,    0, "horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}"      },
   { 'S' , 'y', 'y', "SRC", "source"    , "linewise review of textual content"                 , "Isrte"                    ,    0, "hor=0HhlL$bBeEwW  g/z=sh,le  sel=vV\"  pul=yYdDxX  put=pP  chg=rRiIaA  fnd=fnN"          },
   { 'I' , 'y', 'y', "INP", "input"     , "linewise creation and editing of textual content"   , ""                         ,    0, ""                                                                                        },
   { ':' , 'y', '-', "CMD", "command"   , "command line capability for advanced actions"       , ""                         ,    0, ""                                                                                        },
   { '/' , 'y', '-', "SCH", "search"    , "search mode to find data and objects"               , ""                         ,    0, ""                                                                                        },
   /*---(sub-modes)----------------------*/
   { '9' , '-', 'y', "rep", "repeat"    , "accumulate multiplier"                              , ""                         ,    0, "range 1-99"                                                                              },
   { '!' , '-', 'y', "fil", "filter"    , "process current/selection through external filter"  , ""                         ,    0, "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      },
   { 'v' , '-', 'y', "vis", "visual"    , "visual selection of objects for collection action"  , "$\""                      ,    0, "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      },
   { 's' , '-', 'y', "sel", "select"    , "visual selection within text content"               , "t"                        ,    0, "0HhlL$"                                                                                  },
   { 'e' , '-', 'y', "err", "errors"    , "display and action errors"                          , ""                         ,    0, ""                                                                                        },
   { 'r' , '-', 'y', "rep", "replace"   , "linewise overtyping of content in source mode"      , ""                         ,    0, "type over character marked with special marker"                                          },
   { '"' , '-', 'y', "reg", "register"  , "selecting specific registers for data movement"     , ""                         ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { 't' , '-', 'y', "trg", "text reg"  , "selecting specific registers for text movement"     , ""                         ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { ',' , '-', 'y', "buf", "buffer"    , "moving and selecting between buffers and windows"   , ""                         ,    0, "select=0...9  modes={ret}(esc}"                                                          },
   { 'w' , '-', 'y', "wdr", "wander"    , "formula creation by moving to target cells"         , ""                         ,    0, "modes={ret}{esc}"                                                                        },
   { '$' , '-', 'y', "frm", "format"    , "content formatting options"                         , ""                         ,    0, "ali=<|>[{^}] num=ifgcCaA$sSpP# tec=eEbBoOxXzZrR tim=tdTD dec=0-9 str=!-=_.+'\" wid=mhHlLnNwW tal=jJkK" },
   { 'o' , '-', 'y', "obj", "object"    , "object formatting and sizing options"               , ""                         ,    0, ""                                                                                        },
   { '\'', '-', 'y', "mrk", "mark"      , "object and location marking"                        , ""                         ,    0, "names=a-zA-Z0-9  actions=#!?_  special='[()]  wander=@  range=<>*"                       },
   { '@' , '-', 'y', "mac", "macro"     , "macro recording, execution, and maintenance"        , ""                         ,    0, "run=a-z"                                                                                 },
   { 'c' , '-', 'y', "chr", "char_find" , "linewise searching for characters"                  , ""                         ,    0, ""                                                                                        },
   { 'h' , '-', 'y', "hnt", "hint"      , "provides automatic and manual labeling hints"       , ""                         ,    0, ""                                                                                        },
   { '\\', '-', 'y', "mnu", "menus"     , "menu system"                                        , ""                         ,    0, ""                                                                                        },
   /*---(done)---------------------------*/
   { '-' , '-', 'y', "bad", "bad mode"  , "default message when mode is not understood"        , ""                         ,    0, "mode not understood"                                                                     },
};



static      char        s_majors       [MAX_MODES] = "";





/*====================------------------------------------====================*/
/*===----                      mode stack handling                     ----===*/
/*====================------------------------------------====================*/
static void  o___MODE_STACK______o () { return; }

char         /*--> prepare mode stack for use ------------[--------[--------]-*/
MODE_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        t           [5]         = "";
   /*---(prepare)------------------------*/
   strlcpy (s_majors, "", MAX_MODES);
   /*---(identify majors)----------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_mode_info [i].major != 'y')    continue;
      sprintf (t, "%c", s_mode_info [i].abbr);
      strlcat (s_majors, t, MAX_MODES);
   }
   /*---(clear stack)--------------------*/
   for (i = 0; i < MAX_STACK; ++i) {
      s_modes [i] = '-';
   }
   /*---(clear controls)-----------------*/
   s_nmode =  0;
   s_cmode = '-';
   /*---(go to default mode)-------------*/
   MODE_enter (MODE_MAP);
   /*---(complete)-----------------------*/
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
   /*> if (s_modes [s_nmode] == a_mode)  return 1;                            <*/
   /*---(validate mode)------------------*/
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_mode_info[i].abbr == '-'    )  break;
      if (s_mode_info[i].abbr != a_mode )  continue;
      ++s_mode_info [i].count;
      x_mode  = a_mode;
   }
   --rce;  if (x_mode  == '-')  return rce;
   /*---(check if allowed)---------------*/
   if (s_nmode > 0)  {
      for (i = 0; i < MAX_MODES; ++i) {
         if (s_mode_info[i].abbr == '-'    )  break;
         if (s_mode_info[i].abbr != s_cmode)  continue;
         x_index = i;
      }
      --rce;  if (x_index <   0 )  return rce;
      --rce;  if (strchr (s_mode_info[x_index].allow, a_mode) == NULL)  return rce;
   }
   /*---(add mode)-----------------------*/
   --rce;  if (s_nmode >= MAX_STACK)   return rce;
   s_modes [s_nmode] = a_mode;
   ++s_nmode;
   /*---(set global mode)----------------*/
   s_cmode = a_mode;
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
   --rce;  if (s_nmode <= 0)  return rce;
   --s_nmode;
   s_modes [s_nmode] = x_mode;
   x_mode = s_modes [s_nmode - 1];
   /*---(set global mode)----------------*/
   s_cmode = x_mode;
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_curr          (void)
{
   return s_cmode;
}

char
MODE_prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_mode      = '-';
   /*---(check stack)--------------------*/
   --rce;  if (s_nmode <= 1)            return rce;
   /*---(grab previous)------------------*/
   x_mode = s_modes [s_nmode - 2];
   if (strchr (s_majors, x_mode) != NULL)   return x_mode;
   /*---(go back one more)---------------*/
   --rce;  if (s_nmode <= 2)            return rce;
   x_mode = s_modes [s_nmode - 3];
   /*---(complete)-----------------------*/
   return x_mode;
}

char
MODE_not           (char a_mode)
{
   if (a_mode != s_modes [s_nmode - 1]) return -1;
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
   sprintf (a_list, "modes (%d)", s_nmode);
   for (i = 0; i < 8; ++i) {
      sprintf (t, " %c", s_modes [i]);
      strlcat (a_list, t, LEN_STR);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
MODE_message       (char *a_mesg, char *a_cmd)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_major     = ' ';
   char        x_minor     = ' ';
   for (i = 0; i < MAX_MODES; ++i) {
      if (s_mode_info[i].abbr == '-'   )  break;
      if (s_mode_info[i].abbr == s_cmode)  break;
   }
   if (s_mode_info [i].major == 'y')  {
      x_major = s_cmode;
   } else {
      x_major = MODE_prev  ();
      x_minor = s_cmode;
   }
   if (a_mesg != NULL) {
      switch (s_cmode) {
      case MODE_COMMAND :
      case MODE_SEARCH  :
         strlcpy (a_mesg, a_cmd, LEN_STR);
         break;
      default           :
         snprintf (a_mesg, LEN_STR, "[%c%c] %-3.3s : %s\n", x_major, x_minor, s_mode_info[i].three, s_mode_info[i].mesg);
         break;
      }
   }
   return 0;
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
      if (s_mode_info[i].abbr == '-'   )   break;
      if (s_mode_info[i].abbr != a_mode)   continue;
      x_mode = i;
   }
   --rce;  if (x_mode < 0)  return rce;
   /*---(update)-------------------------*/
   if (a_allow  != NULL) {
      strlcpy (s_mode_info [x_mode].allow, a_allow, 25);
   }
   if (a_mesg  != NULL) {
      strlcpy (s_mode_info [x_mode].mesg , a_mesg , LEN_STR);
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        repeat keys                           ----===*/
/*====================------------------------------------====================*/
static void  o___REPEAT__________o () { return; }

static   int   s_repeat   = 0;

char
REPEAT_reset            (void)
{
   s_repeat       = 0;
   MACRO_zero ();
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
   --rce;  if (MODE_not (SMOD_REPEAT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ESCAPE) {
      MODE_exit  ();
      s_repeat = 0;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for major)-----------------------*/
   if (s_repeat ==  0 && strchr ("123456789", a_major) != NULL) {
      DEBUG_USER   yLOG_note    ("assign starting repeat");
      s_repeat  = a_major - '0';
   }
   /*---(check for minor)-----------------------*/
   if (strchr ("0123456789",  a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("increment repeat");
      s_repeat *= 10;
      s_repeat += a_minor - '0';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(pass through)-------------------*/
   --s_repeat;
   if (s_repeat <  0) s_repeat =  0;
   /*> if (s_repeat > 99) s_repeat = 99;                                              <*/
   /*---(complete)-----------------------*/
   MODE_exit  ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_minor;
}

char REPEAT_normal      (void) { if (s_repeat < 1) return 0; else return 1; } 
char REPEAT_decrement   (void) { if (s_repeat > 0)  --s_repeat; }
int  REPEAT_count       (void) { return s_repeat; }



/*====================------------------------------------====================*/
/*===----                        unit testing                          ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

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
