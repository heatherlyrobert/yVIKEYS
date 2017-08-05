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
   /*-a-- -maj show --tla- ---terse----- ---description---------------------------------------- -----------------------,----- 123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789- */
   /*---(major modes)--------------------*/
   { 'G' , 'y', 'y', "GOD", "god"       , "god-mode allowing 3D omnicient viewing"             , "P"                   ,    0, "linear=LnhHJjkKIioO  rotate=PpaAYytTRrwW"                                                },
   { 'P' , 'y', 'y', "PRG", "progress"  , "progress timeline adding time dimension"            , ""                    ,    0, "horz=0LlhH$  vert=_KkjJG speed=<.> scale=+-"                                             },
   { 'M' , 'y', 'y', "MAP", "map"       , "map-mode providing 2D review of object collections" , "GVSI:/\"b\'$oe\\"    ,    0, "horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}"      },
   { 'V' , 'y', 'y', "VIS", "visual"    , "visual selection of objects for collection action"  , "$\""                 ,    0, "0HhlL$_KkjJG  gz=sh,letk.jb  dxy  !: ~uU /nN oO sS"                                      },
   { 'S' , 'y', 'y', "SRC", "source"    , "linewise review of textual content"                 , "Isrte"               ,    0, "hor=0HhlL$bBeEwW  g/z=sh,le  sel=vV\"  pul=yYdDxX  put=pP  chg=rRiIaA  fnd=fnN"          },
   { 'I' , 'y', 'y', "INP", "input"     , "linewise creation and editing of textual content"   , ""                    ,    0, ""                                                                                        },
   { ':' , 'y', '-', "CMD", "command"   , "command line capability for advanced actions"       , ""                    ,    0, ""                                                                                        },
   /*---(sub-modes)----------------------*/
   { 'e' , '-', 'y', "err", "errors"    , "display and action errors"                          , ""                    ,    0, ""                                                                                        },
   { 's' , '-', 'y', "sel", "select"    , "visual selection within text content"               , "t"                   ,    0, "0HhlL$"                                                                                  },
   { 'r' , '-', 'y', "rep", "replace"   , "linewise overtyping of content in source mode"      , ""                    ,    0, "type over character marked with special marker"                                          },
   { '"' , '-', 'y', "reg", "register"  , "selecting specific registers for data movement"     , ""                    ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { 't' , '-', 'y', "trg", "text reg"  , "selecting specific registers for text movement"     , ""                    ,    0, "regs=\"a-zA-Z-+0  pull=yYxXdD  -/+=vVcCtTsSfF  push=pPrRmMaAiIoObB  mtce=#?!g"           },
   { ',' , '-', 'y', "buf", "buffer"    , "moving and selecting between buffers and windows"   , ""                    ,    0, "select=0...9  modes={ret}(esc}"                                                          },
   { '@' , '-', 'y', "wdr", "wander"    , "formula creation by moving to target cells"         , ""                    ,    0, "modes={ret}{esc}"                                                                        },
   { '$' , '-', 'y', "frm", "format"    , "content formatting options"                         , ""                    ,    0, "ali=<|>[{^}] num=ifgcCaA$sSpP# tec=eEbBoOxXzZrR tim=tdTD dec=0-9 str=!-=_.+'\" wid=mhHlLnNwW tal=jJkK" },
   { 'o' , '-', 'y', "obj", "object"    , "object formatting and sizing options"               , ""                    ,    0, ""                                                                                        },
   { '\'', '-', 'y', "mrk", "mark"      , "object and location marking"                        , ""                    ,    0, "set=a-zA-Z()  del=#*  hlp=?!@_  go='a-zA-Z()[<>]"                                        },
   { '\\', '-', 'y', "mnu", "menus"     , "menu system"                                        , ""                    ,    0, ""                                                                                        },
   /*---(done)---------------------------*/
   { '-' , '-', 'y', "bad", "bad mode"  , "default message when mode is not understood"        , ""                    ,    0, "mode not understood"                                                                     },
};



static      char        s_majors       [MAX_MODES] = "";



/*====================------------------------------------====================*/
/*===----                      mode stack handling                     ----===*/
/*====================------------------------------------====================*/
static void  o___MODE_STACK______o () { return; }

char         /*--> prepare mode stack for use ------------[--------[--------]-*/
yVIKEYS_mode_init  (void)
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
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> add a mode to the stack ---------------[--------[--------]-*/
yVIKEYS_mode_enter (char a_mode)
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
yVIKEYS_mode_exit  (void)
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
yVIKEYS_mode_curr  (void)
{
   return s_cmode;
}

char
yVIKEYS_mode_prev  (void)
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
yVIKEYS_mode_not   (char a_mode)
{
   if (a_mode != s_modes [s_nmode - 1]) return -1;
   return 0;
}

char       /*----: list the current mode stack -------------------------------*/
yVIKEYS_mode_list  (char *a_list)
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
yVIKEYS_mode_mesg  (char *a_mesg, char *a_cmd)
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
      x_major = yVIKEYS_mode_prev ();
      x_minor = s_cmode;
   }
   if (a_mesg != NULL) {
      switch (s_cmode) {
      case MODE_COMMAND :
      case MODE_SEARCH  :
         strlcpy (a_mesg, a_cmd, LEN_STR);
         break;
      default           :
         sprintf (a_mesg, "[%c%c] %-3.3s : %s\n", x_major, x_minor, s_mode_info[i].three, s_mode_info[i].mesg);
         break;
      }
   }
   return 0;
}



/*============================----end-of-source---============================*/
