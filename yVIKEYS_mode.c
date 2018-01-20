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


/*===[[ LAYOUT ]]=============================================================*/
/*---(window formatting)-------------------*/
typedef struct  cWIN  tWIN;
struct cWIN {
   /*---(header)------------*/
   char        a_format;                    /* format identifier              */
   char        a_adapt;                     /* adapt to add elements          */
   int         a_wide;                      /* asking width                   */
   int         a_tall;                      /* asking heigth                  */
   /*---(window sizes)------*/
   char        w_title     [LEN_DESC];      /* window title                   */
   int         w_wide;                      /* window width                   */
   int         w_tall;                      /* window heigth                  */
   /*---(title sizes)-------*/
   char        t_on;                        /* title shown y/n                */
   char        t_text      [LEN_DESC];      /* title text                     */
   int         t_wide;                      /* width  of title line           */
   int         t_left;                      /* left   of title line           */
   int         t_tall;                      /* height of title line           */
   int         t_bott;                      /* bottom of title line           */
   char        t_ver       [LEN_LABEL];     /* title version info             */
   /*---(buffer sizes)------*/
   char        b_on;                        /* buffer shown y/n               */
   int         b_wide;                      /* width  of buffer bar           */
   int         b_left;                      /* left   of buffer bar           */
   int         b_tall;                      /* height of buffer bar           */
   int         b_bott;                      /* bottom of buffer bar           */
   /*---(formula sizes)-----*/
   char        f_on;                        /* formula shown y/n              */
   int         f_wide;                      /* width  of formula line         */
   int         f_left;                      /* left   of formula line         */
   int         f_tall;                      /* height of formula line         */
   int         f_bott;                      /* bottom of formula line         */
   /*---(nav sizes)---------*/
   char        n_on;                        /* nav panel shown y/n            */
   int         n_wide;                      /* width  of nav panel            */
   int         n_left;                      /* left   of nav panel            */
   int         n_tall;                      /* height of nav panel            */
   int         n_bott;                      /* bottom of nav panel            */
   /*---(main sizes)--------*/
   int         m_wide;                      /* width  of main window          */
   int         m_left;                      /* left   of main window          */
   int         m_tall;                      /* height of main window          */
   int         m_bott;                      /* bottom of main window          */
   int         g_on;                        /* show grid over main y/n        */
   /*---(progress)----------*/
   char        p_on;                        /* show progress y/n              */
   int         p_wide;                      /* width  of progress window      */
   int         p_left;                      /* left   of progress window      */
   int         p_tall;                      /* height of progress window      */
   int         p_bott;                      /* bottom of progress window      */
   /*---(status sizes)------*/
   char        s_on;                        /* show status y/n                */
   char        s_text      [LEN_DESC];      /* current text in status line    */
   int         s_wide;                      /* width  of status line          */
   int         s_left;                      /* left   of status line          */
   int         s_tall;                      /* height of status line          */
   int         s_bott;                      /* bottom of status line          */
   /*---(command sizes)-----*/
   char        c_on;                        /* show command y/n               */
   char        c_text      [LEN_DESC];      /* current text in command mode   */
   int         c_wide;                      /* width  of command line         */
   int         c_left;                      /* left   of command line         */
   int         c_tall;                      /* height of command line         */
   int         c_bott;                      /* bottom of command line         */
   char        c_keys      [LEN_LABEL];     /* current keys feedback          */
   /*---(details)-----------*/
   char        d_on;                        /* show detail y/n                */
   int         d_wide;                      /* width  of detail window        */
   int         d_left;                      /* left   of detail window        */
   int         d_tall;                      /* height of detail window        */
   int         d_bott;                      /* bottom of detail window        */
   /*---(ribbon sizes)------*/
   char        r_on;                        /* show ribbon y/n                */
   int         r_wide;                      /* width  of ribbon/button bar    */
   int         r_left;                      /* left   of ribbon/button bar    */
   int         r_tall;                      /* height of ribbon/button bar    */
   int         r_bott;                      /* bottom of ribbon/button bar    */
   int         r_icons     [LEN_LABEL];     /* actual icons for ribbon        */
   int         r_nicon;                     /* number of icons                */
   /*---(version)-----------*/
   char        v_on;                        /* show version y/n               */
   char        v_text      [LEN_DESC];      /* current version                */
   int         v_wide;                      /* width  of version              */
   int         v_left;                      /* left   of version              */
   int         v_tall;                      /* height of version              */
   int         v_bott;                      /* bottom of version              */
   /*---(keys)--------------*/
   char        k_on;                        /* show keys y/n                  */
   char        k_text      [LEN_DESC];      /* current keys                   */
   int         k_wide;                      /* width  of key display          */
   int         k_left;                      /* left   of key display          */
   int         k_tall;                      /* height of key display          */
   int         k_bott;                      /* bottom of key display          */
   /*---(display)-----------*/
   char        face;
   int         font;    
   int         icons;
   /*---(done)--------------*/
};
static tWIN   s_win;


#define      MAX_PARTS         20
typedef  struct  cPARTS     tPARTS;
struct cPARTS {
   char        abbr;                        /* short name of screen element   */
   char        name        [LEN_LABEL];     /* name of screen element         */
   char       *flag;                        /* switch                         */
   char        desc        [LEN_DESC ];     /* explanation of element         */
};
static tPARTS  s_parts [MAX_PARTS] = {
   /* -    ---name-----   ---flag------   12345678901234567890123456789012345678901234567890  */
   { 't', "title"       , &(s_win.t_on), "left hand title bar"                                },
   { 'b', "buffer"      , &(s_win.b_on), "buffer inventory at top"                            },
   { 'f', "formula"     , &(s_win.f_on), "formula and source editing line at top"             },
   { 'n', "nav"         , &(s_win.n_on), "navigation panel to display tags and other links"   },
   { 'm', "main"        , NULL         , "main working area in the middle"                    },
   { 'p', "progress"    , &(s_win.p_on), "time and sequencing controls about status line"     },
   { 's', "status"      , &(s_win.s_on), "informational status bar above command line"        },
   { 'c', "command"     , &(s_win.c_on), "command, search, and help message line at bottom"   },
   { 'd', "details"     , &(s_win.d_on), "display area for critical details to right"         },
   { 'r', "ribbon"      , &(s_win.r_on), "menu and icon display for navigation of commands"   },
   { 'v', "version"     , &(s_win.v_on), "version display with debugging notice"              },
   { 'k', "keys"        , &(s_win.k_on), "latest keyboard characters typed"                   },
   { 'g', "grid"        , &(s_win.g_on), "overlay main drawing with a grid"                   },
   {  0 , ""            , NULL         , ""                                                   },
};
static int  s_npart     = 0;

#define      MAX_LAYOUT        20
typedef struct cLAYOUT   tLAYOUT;
struct cLAYOUT {
   char        name        [LEN_LABEL];     /* title for layout               */
   char        parts       [MAX_PARTS];     /* selection of elements          */
   char        desc        [LEN_DESC ];     /* explanation of layout          */
};
tLAYOUT   s_layouts [MAX_LAYOUT] = {
   /*---name------    tbfnpscdrvk    ,    12345678901234567890123456789012345678901234567890  */
   { "min"         , "-----------"     "smallest footprint, least elements showing"           },
   { "work"        , "t----sc--vk"     "more balanced display of common elements"             },
   { "max"         , "tbfnpscdrvk"     "everything displays at one time"                      },
   { ""            , "-----------"     ""                                                     },
};
static int  s_nlayout   = 0;



int         g_goffx   =   0;
int         g_gsizex  =   1;
int         g_goffy   =   0;
int         g_gsizey  =   1;
int         g_goffz   =   0;
int         g_gsizez  =   1;

char    (*s_sizer ) (int, int);
char    (*s_drawer) (float);


#define     ICON_SET  "/usr/local/share/fonts/outline_icons.png"


#define     COLOR_BASE          'b'
#define     COLOR_DARK          '-'
#define     COLOR_LIGHT         '+'
#define     COLOR_MUTED         'm'
#define     COLOR_ACC_L         '1'
#define     COLOR_ACC_D         '2'
#define     COLOR_ACC_O         'o'
#define     COLOR_TXT_L         'w'
#define     COLOR_TXT_D         'k'
#define     COLOR_GRID_L        'g'
#define     COLOR_GRID_D        'G'
#define     COLOR_BLACK         'K'
#define     COLOR_WARN          'W'





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

char*        /*-> unit test accessor -----------------[ light  [us.420.111.11]*/ /*-[01.0000.00#.Z]-*/ /*-[--.---.---.--]-*/
yVIKEYS__mode_unit     (char *a_question)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_STR];
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MODE unit        : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "stack"        )  == 0) {
      yVIKEYS_mode_list (x_list);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MODE stack       : %s", x_list);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



/*====================------------------------------------====================*/
/*===----                      window layout                           ----===*/
/*====================------------------------------------====================*/
static void  o___VIEW____________o () { return; }

char
yVIKEYS_view_set         (char *a_name, char *a_opt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_on        =  '-';
   char        i           =    0;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_name"    , a_name);
   DEBUG_GRAF   yLOG_point   ("a_opt"     , a_opt);
   --rce;  if (a_opt  == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_opt"     , a_opt);
   for (i = 0; i <= s_npart; ++i) {
      DEBUG_GRAF   yLOG_info    ("check"     , s_parts [i].name);
      if (s_parts [i].name [0] != a_name [0])         continue;
      if (strcmp (s_parts [i].name, a_name) != 0)     continue;
      n = i;
   }
   DEBUG_GRAF   yLOG_value   ("result"    , n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("current"   , *(s_parts [n].flag));
   x_on = *(s_parts [n].flag);
   if (strcmp (a_opt, "hide") == 0)  *(s_parts [n].flag) = '-';
   if (strcmp (a_opt, "show") == 0)  *(s_parts [n].flag) = 'y';
   DEBUG_GRAF   yLOG_char    ("new"       , *(s_parts [n].flag));
   if (x_on != *(s_parts [n].flag)) {
      DEBUG_GRAF   yLOG_note    ("must resize");
      yVIKEYS_view_resize ('r', NULL, 0, 0, '-');
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_set_layout  (char *a_opt)
{
   if (strcmp (a_opt, "min") == 0) {
      s_win.t_on = '-';
      s_win.c_on = '-';
      s_win.s_on = '-';
      s_win.r_on = '-';
   } else if (strcmp (a_opt, "work") == 0) {
      s_win.t_on = 'y';
      s_win.c_on = 'y';
      s_win.s_on = 'y';
      s_win.r_on = 'y';
   }
   yVIKEYS_view_resize ('r', NULL, 0, 0, '-');
   return 0;
}

char
yVIKEYS_view_init       (void *a_sizer, char *a_title, char *a_ver)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        rc          =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(screen parts)-------------------*/
   s_npart = 0;
   for (i = 0; i < MAX_PARTS; ++i) {
      if (s_parts [i].abbr == 0)  break;
      DEBUG_GRAF   yLOG_info    ("name"      , s_parts [i].name);
      if (s_parts [i].flag != NULL) {
         DEBUG_GRAF   yLOG_note    ("adding view menu command to show/hide");
         yVIKEYS_cmds_add ('v', s_parts [i].name , ""    , "Cs"   , yVIKEYS_view_set          , "" );
      }
      ++s_npart;
   }
   DEBUG_GRAF   yLOG_value   ("s_npart"   , s_npart);
   /*---(screen layouts)-----------------*/
   s_nlayout = 0;
   for (i = 0; i < MAX_LAYOUT; ++i) {
      if (s_layouts [i].name [0] == 0)  break;
      ++s_nlayout;
   }
   /*---(commands)-----------------------*/
   s_sizer = NULL;
   if (a_sizer != NULL)  s_sizer = a_sizer;
   /*> yVIKEYS_cmds_add ('v', "layout"      , ""    , "s"    , yVIKEYS_view_set_layout   , "" );   <* 
    *> yVIKEYS_cmds_add ('v', "status"      , ""    , "s"    , yVIKEYS_view_set_status   , "" );   <* 
    *> yVIKEYS_cmds_add ('v', "command"     , ""    , "s"    , yVIKEYS_view_set_command  , "" );   <* 
    *> yVIKEYS_cmds_add ('v', "title"       , ""    , "s"    , yVIKEYS_view_set_title    , "" );   <* 
    *> yVIKEYS_cmds_add ('v', "ribbon"      , ""    , "s"    , yVIKEYS_view_set_ribbon   , "" );   <* 
    *> yVIKEYS_cmds_add ('v', "formula"     , ""    , "s"    , yVIKEYS_view_set_formula  , "" );   <*/
   /*> yVIKEYS_cmds_add ('v', "grid"        , ""    , "s"    , yVIKEYS_view_set_grid     , "" );   <*/
   yVIKEYS_cmds_add ('v', "gridoff"     , ""    , "iii"  , yVIKEYS_view_set_gridoff  , "" );
   yVIKEYS_cmds_add ('v', "gridsize"    , ""    , "iii"  , yVIKEYS_view_set_gridsize , "" );
   if (a_title != NULL)  strlcpy (s_win.t_text, a_title, LEN_DESC );
   if (a_ver   != NULL)  strlcpy (s_win.t_ver , a_ver  , LEN_LABEL);
   s_win.t_on    = 'y';
   s_win.v_on    = 'y';
   s_win.c_on    = 'y';
   s_win.k_on    = '-';
   s_win.s_on    = 'y';
   s_win.p_on    = '-';
   s_win.d_on    = '-';
   s_win.r_on    = '-';
   s_win.b_on    = '-';
   s_win.f_on    = '-';
   s_win.n_on    = '-';
   s_win.g_on    = '-';
   for (i = 0; i < LEN_LABEL; ++i)  s_win.r_icons [i] = -1;
   s_win.r_nicon = 0;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_resize     (cchar a_format, cchar *a_title, cint a_wide, cint a_tall, cchar a_adapt)
{
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(save input)---------------------*/
   if (a_title != NULL && a_title [0] != NULL)  strlcpy (s_win.w_title, a_title, LEN_DESC);
   if (strchr ("-r ", a_format) == NULL)        s_win.a_format = a_format;
   if (a_wide > 50)                             s_win.a_wide   = a_wide;
   if (a_tall > 50)                             s_win.a_tall   = a_tall;
   if (strchr ("- " , a_adapt ) == NULL)        s_win.a_adapt  = a_adapt;
   /*---(display)------------------------*/
   DEBUG_GRAF   yLOG_char    ("format"    , s_win.a_format);
   DEBUG_GRAF   yLOG_info    ("title"     , s_win.w_title);
   DEBUG_GRAF   yLOG_value   ("wide"      , s_win.a_wide);
   DEBUG_GRAF   yLOG_value   ("tall"      , s_win.a_tall);
   DEBUG_GRAF   yLOG_char    ("adapt"     , s_win.a_adapt);
   /*---(widths)-------------------------*/
   if (s_win.t_on != '-')  s_win.t_wide =  15;
   else                    s_win.t_wide =   0;
   if (s_win.v_on != '-')  s_win.v_wide =  15;
   else                    s_win.v_wide =   0;
   if (s_win.d_on != '-')  s_win.d_wide = 150;
   else                    s_win.d_wide =   0;
   if (s_win.n_on != '-')  s_win.n_wide = 150;
   else                    s_win.n_wide =   0;
   if (s_win.r_on != '-')  s_win.r_wide =  40;
   else                    s_win.r_wide =   0;
   if (s_win.k_on != '-')  s_win.k_wide =  40;
   else                    s_win.k_wide =   0;
   if (s_win.a_adapt == 'y') {
      s_win.m_wide = s_win.a_wide;
      s_win.w_wide = s_win.m_wide + s_win.t_wide + s_win.n_wide + s_win.d_wide + s_win.r_wide;
   } else {
      s_win.w_wide = s_win.a_wide;
      s_win.m_wide = s_win.w_wide - s_win.t_wide - s_win.n_wide - s_win.d_wide - s_win.r_wide;
   }
   s_win.s_wide = s_win.b_wide = s_win.f_wide = s_win.w_wide - s_win.t_wide;
   s_win.c_wide = s_win.s_wide - s_win.k_wide;
   s_win.p_wide = s_win.m_wide + s_win.n_wide + s_win.d_wide;
   /*---(lefts)--------------------------*/
   s_win.t_left = s_win.v_left =   0;
   s_win.n_left = s_win.c_left = s_win.s_left = s_win.b_left = s_win.f_left = s_win.t_wide;
   s_win.m_left = s_win.n_left + s_win.n_wide;
   s_win.d_left = s_win.m_left + s_win.m_wide;
   s_win.r_left = s_win.d_left + s_win.d_wide;
   s_win.k_left = s_win.w_wide - s_win.k_wide;
   /*---(talls)--------------------------*/
   if (s_win.v_on != '-')  s_win.v_tall =  40;
   else                    s_win.v_tall =   0;
   if (s_win.p_on != '-')  s_win.p_tall = 100;
   else                    s_win.p_tall =   0;
   if (s_win.s_on != '-')  s_win.s_tall =  15;
   else                    s_win.s_tall =   0;
   if (s_win.c_on != '-')  s_win.c_tall =  15;
   else                    s_win.c_tall =   0;
   if (s_win.k_on != '-')  s_win.k_tall =  15;
   else                    s_win.k_tall =   0;
   if (s_win.b_on != '-')  s_win.b_tall =  15;
   else                    s_win.b_tall =   0;
   if (s_win.f_on != '-')  s_win.f_tall =  15;
   else                    s_win.f_tall =   0;
   if (s_win.a_adapt == 'y') {
      s_win.m_tall = s_win.a_tall;
      s_win.w_tall = s_win.m_tall + s_win.p_tall + s_win.b_tall + s_win.c_tall + s_win.s_tall + s_win.f_tall;
   } else {
      s_win.w_tall = s_win.a_tall;
      s_win.m_tall = s_win.w_tall - s_win.p_tall - s_win.b_tall - s_win.c_tall - s_win.s_tall - s_win.f_tall;
   }
   s_win.t_tall = s_win.w_tall - s_win.v_tall;
   s_win.d_tall = s_win.n_tall = s_win.r_tall = s_win.w_tall - s_win.b_tall - s_win.f_tall - s_win.c_tall - s_win.s_tall;
   /*---(bottoms)------------------------*/
   s_win.c_bott = s_win.t_bott = s_win.k_bott =  0;  
   s_win.s_bott = s_win.c_tall;
   s_win.p_bott = s_win.n_bott = s_win.d_bott = s_win.r_bott = s_win.s_bott + s_win.s_tall;
   s_win.m_bott = s_win.p_bott + s_win.p_tall;
   s_win.b_bott = s_win.w_tall - s_win.b_tall;
   s_win.f_bott = s_win.b_bott - s_win.f_tall;
   s_win.v_bott = s_win.w_tall - s_win.v_tall;
   /*---(readout)------------------------*/
   DEBUG_GRAF   yLOG_complex ("window"    , "bott %4d, left %4d, wide %4d, tall %4d", 0         , 0         , s_win.w_wide, s_win.w_tall);
   DEBUG_GRAF   yLOG_complex ("title"     , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.t_bott, s_win.t_left, s_win.t_wide, s_win.t_tall);
   DEBUG_GRAF   yLOG_complex ("buffer"    , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.b_bott, s_win.b_left, s_win.b_wide, s_win.b_tall, s_win.b_on);
   DEBUG_GRAF   yLOG_complex ("formula"   , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.f_bott, s_win.f_left, s_win.f_wide, s_win.f_tall, s_win.f_on);
   DEBUG_GRAF   yLOG_complex ("progress"  , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.p_bott, s_win.p_left, s_win.p_wide, s_win.p_tall, s_win.p_on);
   DEBUG_GRAF   yLOG_complex ("status"    , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.s_bott, s_win.s_left, s_win.s_wide, s_win.s_tall, s_win.s_on);
   DEBUG_GRAF   yLOG_complex ("command"   , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.c_bott, s_win.c_left, s_win.c_wide, s_win.c_tall, s_win.c_on);
   DEBUG_GRAF   yLOG_complex ("nav"       , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.n_bott, s_win.n_left, s_win.n_wide, s_win.n_tall, s_win.n_on);
   DEBUG_GRAF   yLOG_complex ("details"   , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.d_bott, s_win.d_left, s_win.d_wide, s_win.d_tall, s_win.d_on);
   DEBUG_GRAF   yLOG_complex ("ribbon"    , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.r_bott, s_win.r_left, s_win.r_wide, s_win.r_tall, s_win.r_on);
   DEBUG_GRAF   yLOG_complex ("version"   , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.v_bott, s_win.v_left, s_win.v_wide, s_win.v_tall, s_win.v_on);
   DEBUG_GRAF   yLOG_complex ("keys"      , "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_win.k_bott, s_win.k_left, s_win.k_wide, s_win.k_tall, s_win.k_on);
   DEBUG_GRAF   yLOG_complex ("main"      , "bott %4d, left %4d, wide %4d, tall %4d, -- -" , s_win.m_bott, s_win.m_left, s_win.m_wide, s_win.m_tall);
   /*---(sizer)--------------------------*/
   if (a_format != '-' && s_sizer != NULL) {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return s_sizer (s_win.w_wide, s_win.w_tall);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_text       (cchar a_part, cchar *a_text)
{
   switch (a_part) {
   case 't' :
      strlcpy (s_win.t_text, a_text, LEN_DESC);
      break;
   case 's' :
      strlcpy (s_win.s_text, a_text, LEN_DESC);
      break;
   case 'c' :
      strlcpy (s_win.c_text, a_text, LEN_DESC);
      break;
   }
   return 0;
}

char
yVIKEYS_view_corners    (cchar a_part, int *a_left, int *a_bott, int *a_wide, int *a_tall, cchar *a_text)
{
   switch (a_part) {
   case 'w' :
      if (a_left != NULL)  *a_left  = 0;
      if (a_bott != NULL)  *a_bott  = 0;
      if (a_wide != NULL)  *a_wide  = s_win.w_wide;
      if (a_tall != NULL)  *a_tall  = s_win.w_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     'y';
      break;
   case 't' :
      if (a_left != NULL)  *a_left  = s_win.t_left;
      if (a_bott != NULL)  *a_bott  = s_win.t_bott;
      if (a_wide != NULL)  *a_wide  = s_win.t_wide;
      if (a_tall != NULL)  *a_tall  = s_win.t_tall;
      if (a_text != NULL)  strlcpy (a_text, s_win.t_text, LEN_DESC);
      return     s_win.t_on;
      break;
   case 'c' :
      if (a_left != NULL)  *a_left  = s_win.c_left;
      if (a_bott != NULL)  *a_bott  = s_win.c_bott;
      if (a_wide != NULL)  *a_wide  = s_win.c_wide;
      if (a_tall != NULL)  *a_tall  = s_win.c_tall;
      if (a_text != NULL)  strlcpy (a_text, s_win.c_text, LEN_DESC);
      return     s_win.c_on;
      break;
   case 's' :
      if (a_left != NULL)  *a_left  = s_win.s_left;
      if (a_bott != NULL)  *a_bott  = s_win.s_bott;
      if (a_wide != NULL)  *a_wide  = s_win.s_wide;
      if (a_tall != NULL)  *a_tall  = s_win.s_tall;
      if (a_text != NULL)  strlcpy (a_text, s_win.s_text, LEN_DESC);
      return     s_win.s_on;
      break;
   case 'm' :
      if (a_left != NULL)  *a_left  = s_win.m_left;
      if (a_bott != NULL)  *a_bott  = s_win.m_bott;
      if (a_wide != NULL)  *a_wide  = s_win.m_wide;
      if (a_tall != NULL)  *a_tall  = s_win.m_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     'y';
      break;
   case 'r' :
      if (a_left != NULL)  *a_left  = s_win.r_left;
      if (a_bott != NULL)  *a_bott  = s_win.r_bott;
      if (a_wide != NULL)  *a_wide  = s_win.r_wide;
      if (a_tall != NULL)  *a_tall  = s_win.r_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     s_win.r_on;
      break;
   case 'd' :
      if (a_left != NULL)  *a_left  = s_win.d_left;
      if (a_bott != NULL)  *a_bott  = s_win.d_bott;
      if (a_wide != NULL)  *a_wide  = s_win.d_wide;
      if (a_tall != NULL)  *a_tall  = s_win.d_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     s_win.d_on;
      break;
   case 'p' :
      if (a_left != NULL)  *a_left  = s_win.p_left;
      if (a_bott != NULL)  *a_bott  = s_win.p_bott;
      if (a_wide != NULL)  *a_wide  = s_win.p_wide;
      if (a_tall != NULL)  *a_tall  = s_win.p_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     s_win.p_on;
      break;
   case 'b' :
      if (a_left != NULL)  *a_left  = s_win.b_left;
      if (a_bott != NULL)  *a_bott  = s_win.b_bott;
      if (a_wide != NULL)  *a_wide  = s_win.b_wide;
      if (a_tall != NULL)  *a_tall  = s_win.b_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     s_win.b_on;
      break;
   case 'f' :
      if (a_left != NULL)  *a_left  = s_win.f_left;
      if (a_bott != NULL)  *a_bott  = s_win.f_bott;
      if (a_wide != NULL)  *a_wide  = s_win.f_wide;
      if (a_tall != NULL)  *a_tall  = s_win.f_tall;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return     s_win.f_on;
      break;
   }
   return '-';
}

char
yVIKEYS_view_color      (char a_color)
{
   switch (a_color) {
   case  'b' :  /* background  COLOR_BASE     */
      glColor4f (0.30f, 0.50f, 0.30f, 1.00f);       /* nice medium green          */
      break;
   case  '-' :  /* darker      COLOR_DARK     */
      glColor4f (0.25f, 0.40f, 0.25f, 1.00f);
      break;
   case  '+' :  /* lighter     COLOR_LIGHT    */
      glColor4f (0.40f, 0.65f, 0.35f, 1.00f);
      break;
   case  'm' :  /* muted       COLOR_MUTED    */
      glColor4f (0.30f, 0.10f, 0.00f, 0.30f);
      break;
   case  '1' :  /* contrast 1  COLOR_ACC_L    */
      glColor4f (1.00f, 0.60f, 0.30f, 0.30f);
      break;
   case  '2' :  /* contrast 2  COLOR_ACC_D    */
      glColor4f (0.70f, 0.20f, 0.10f, 0.40f);
      break;
   case  '3' :  /* contrast 3  COLOR_ACC_O    */
      glColor4f (0.80f, 0.30f, 0.00f, 0.30f);
      break;
   case  'g' :  /* contrast 2  COLOR_GRID_L   */
      glColor4f (1.00f, 1.00f, 1.00f, 0.50f);
      break;
   case  'G' :  /* contrast 2  COLOR_GRID_D   */
      glColor4f (0.00f, 0.00f, 0.00f, 0.70f);
      break;
   case  'k' :  /* dark text   COLOR_TXT_D    */
      glColor4f (0.20f, 0.20f, 0.20f, 1.00f);
      break;
   case  'w' :  /* white text  COLOR_TXT_L    */
      glColor4f (0.00f, 0.00f, 0.00f, 0.70f);
      break;
   case  'K' :  /* dark text   COLOR_BLACK    */
      glColor4f (0.00f, 0.00f, 0.00f, 1.00f);
      break;
   case  'W' :  /* dark text   COLOR_WARN     */
      glColor4f (0.25f, 0.00f, 0.00f, 1.00f);
      break;
   }
   return 0;
}

char
yVIKEYS_view_command    (void)
{
   /*> printf ("c %c %3d %3d %3d %3d %s\n", x_on, x_left, x_bott, x_wide, x_tall, x_text);   <*/
   if (s_win.c_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.c_left, s_win.c_bott, s_win.c_wide, s_win.c_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.c_wide, 0.0f, s_win.c_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BASE );
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f      , s_win.c_tall,  0.0f);
         glVertex3f  (s_win.c_wide, s_win.c_tall,  0.0f);
         glVertex3f  (s_win.c_wide, 0.0f      ,  0.0f);
         glVertex3f  (0.0f      , 0.0f      ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display)------------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BLACK);
      glTranslatef (    3.0f,    1.0f,    0.0f);
      yFONT_print  (s_win.font,  11, YF_BOTLEF, s_win.c_text);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_status     (void)
{
   /*> printf ("s %c %3d %3d %3d %3d %s\n", x_on, x_left, x_bott, x_wide, x_tall, x_text);   <*/
   if (s_win.s_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.s_left, s_win.s_bott, s_win.s_wide, s_win.s_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.s_wide, 0.0f, s_win.s_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   /*> if (my.key_error == 'y') yVIKEYS_view_color (COLOR_WARN );                         <* 
    *> else                     yVIKEYS_view_color (COLOR_DARK );                         <*/
   yVIKEYS_view_color (COLOR_DARK );
   glPushMatrix    (); {
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f      , s_win.s_tall,  0.0f);
         glVertex3f  (s_win.s_wide, s_win.s_tall,  0.0f);
         glVertex3f  (s_win.s_wide, 0.0f      ,  0.0f);
         glVertex3f  (0.0f      , 0.0f      ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display)------------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_TXT_L);
      glTranslatef (    3.0f,    1.0f,    0.0f);
      yFONT_print  (s_win.font,  11, YF_BOTLEF, s_win.s_text);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_buffer      (void)
{
   if (s_win.b_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.b_left, s_win.b_bott, s_win.b_wide, s_win.b_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.b_wide, 0.0f, s_win.b_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BASE );
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f  , s_win.b_tall,  0.0f);
         glVertex3f  (s_win.b_wide, s_win.b_tall,  0.0f);
         glVertex3f  (s_win.b_wide, 0.0f  ,  0.0f);
         glVertex3f  (0.0f  , 0.0f  ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_formula     (void)
{
   if (s_win.f_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.f_left, s_win.f_bott, s_win.f_wide, s_win.f_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.f_wide, 0.0f, s_win.f_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_DARK );
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f  , s_win.f_tall,  0.0f);
         glVertex3f  (s_win.f_wide, s_win.f_tall,  0.0f);
         glVertex3f  (s_win.f_wide, 0.0f  ,  0.0f);
         glVertex3f  (0.0f  , 0.0f  ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_keys        (void)
{
   if (s_win.k_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.k_left, s_win.k_bott, s_win.k_wide, s_win.k_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.k_wide, 0.0f, s_win.k_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_ACC_D);
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f  , s_win.k_tall,  0.0f);
         glVertex3f  (s_win.k_wide, s_win.k_tall,  0.0f);
         glVertex3f  (s_win.k_wide, 0.0f  ,  0.0f);
         glVertex3f  (0.0f  , 0.0f  ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display)------------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BLACK);
      glTranslatef (2.0,   2.0f,    0.0f);
      glColor4f    (0.00f, 0.00f, 0.00f, 1.00f);
      glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
      yFONT_print  (s_win.font,  11, YF_BOTRIG, s_win.k_text);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_version     (void)
{
   if (s_win.v_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.v_left, s_win.v_bott, s_win.v_wide, s_win.v_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.v_wide, 0.0f, s_win.v_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      if (yURG_debugmode () == 'y')  yVIKEYS_view_color (COLOR_WARN );
      else                           yVIKEYS_view_color (COLOR_ACC_D);
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f  , s_win.v_tall,  0.0f);
         glVertex3f  (s_win.v_wide, s_win.v_tall,  0.0f);
         glVertex3f  (s_win.v_wide, 0.0f  ,  0.0f);
         glVertex3f  (0.0f  , 0.0f  ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display)------------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BLACK);
      glTranslatef (2.0,   2.0f,    0.0f);
      glColor4f    (0.00f, 0.00f, 0.00f, 1.00f);
      glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
      yFONT_print  (s_win.font,  11, YF_BOTRIG, s_win.v_text);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_title       (void)
{
   /*> printf ("t %c %3d %3d %3d %3d %s\n", x_on, x_left, x_bott, x_wide, x_tall, x_text);   <*/
   if (s_win.t_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.t_left, s_win.t_bott, s_win.t_wide, s_win.t_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.t_wide, 0.0f, s_win.t_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_ACC_L);
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f  , s_win.t_tall,  0.0f);
         glVertex3f  (s_win.t_wide, s_win.t_tall,  0.0f);
         glVertex3f  (s_win.t_wide, 0.0f  ,  0.0f);
         glVertex3f  (0.0f  , 0.0f  ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display)------------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BLACK);
      glTranslatef (s_win.t_wide - 1,   2.0f,    0.0f);
      glColor4f    (0.00f, 0.00f, 0.00f, 1.00f);
      glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
      yFONT_print  (s_win.font,  11, YF_BOTLEF, s_win.t_text);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_set_gridoff  (int a_x, int a_y, int a_z)
{
   if (a_x > 0) {
      g_goffx = a_x;
      if (a_y <= 0) g_goffy = a_x;
      if (a_z <= 0) g_goffz = a_x;
   }
   /*> printf ("gridx off %3d size %3d\n", g_goffx, g_gsizex);                        <*/
   return 0;
}

char
yVIKEYS_view_set_gridsize (int a_x, int a_y, int a_z)
{
   if (a_x > 0) {
      g_gsizex = a_x;
      if (a_y <= 0) g_gsizey = a_x;
      if (a_z <= 0) g_gsizez = a_x;
   }
   /*> printf ("gridx off %3d size %3d\n", g_goffx, g_gsizex);                        <*/
   return 0;
}

char          /*----: draw the saved status ----------------------------------*/
yVIKEYS_view_cursor      (float a_mag)
{
   int         x_lef   = s_colmap.cur / a_mag;
   int         x_bot   = s_rowmap.cur / a_mag;
   int         x_rig   = x_lef + (g_gsizex / a_mag);
   int         x_top   = x_bot + (g_gsizey / a_mag);
   glColor4f    (1.00f, 0.00f, 0.00f, 0.2f);
   glBegin      (GL_POLYGON); {
      glVertex3f   (x_lef, x_bot, 25.0);
      glVertex3f   (x_lef, x_top, 25.0);
      glVertex3f   (x_rig, x_top, 25.0);
      glVertex3f   (x_rig, x_bot, 25.0);
      glVertex3f   (x_lef, x_bot, 25.0);
   } glEnd ();
   return 0;
}

char
yVIKEYS_view_grid        (float a_mag)
{
   int         i           = 0;
   int         x           = 0;
   int         y           = 0;
   int         c           = 0;
   int         cx          = 0;
   int         cy          = 0;
   int         x_beg       = 0;
   int         x_end       = 0;
   int         x_inc       = 0;
   /*---(defense)------------------------*/
   if (s_win.g_on == '-')  return 0;
   /*---(x grid)-------------------------*/
   glPushMatrix    (); {
      x_beg = s_colmap.gmin;
      x_end = s_colmap.gmax;
      x_inc = g_gsizex;
      glColor4f     (0.0, 0.3, 0.0, 0.5);
      glLineWidth   (1.5);
      glTranslatef  (g_goffx, 0.0    , 50.0);
      c = 0;
      for (i = x_beg; i <= x_end; i += x_inc) {
         if (c % 5 ==  0) {
            glBegin         (GL_LINES); {
               glVertex3f  (0.0f  , s_rowmap.gmin,  0.0f);
               glVertex3f  (0.0f  , s_rowmap.gmax,  0.0f);
            } glEnd   ();
         }
         glTranslatef  (x_inc / a_mag, 0.0    ,  0.0);
         ++c;
      }
   } glPopMatrix   ();
   /*---(y grid)-------------------------*/
   glPushMatrix    (); {
      x_beg = s_rowmap.gmin;
      printf ("x_beg %3d, x_end %3d, x_inc %3d\n", x_beg, x_end, x_inc);
      x_end = s_rowmap.gmax;
      x_inc = g_gsizey;
      glColor4f     (0.0, 0.3, 0.0, 0.5);
      glLineWidth   (1.5);
      glTranslatef  (0.0, g_goffy, 50.0);
      c = 0;
      for (i = x_beg; i <= x_end; i += x_inc) {
         if (c % 5 ==  0) {
            glBegin         (GL_LINES); {
               glVertex3f  (s_colmap.gmin, 0.0f,  0.0f);
               glVertex3f  (s_colmap.gmax, 0.0f,  0.0f);
            } glEnd   ();
         }
         glTranslatef  (0.0, x_inc / a_mag,  0.0);
         ++c;
      }
   } glPopMatrix   ();
   /*---(xy dots)------------------------*/
   glPushMatrix    (); {
      /*> glScalef      (a_mag, a_mag, a_mag);                                        <*/
      glPointSize   (5);
      glLineWidth   (1.5);
      glColor4f     (0.0, 0.3, 0.0, 0.5);
      /*> yVIKEYS_view_color (COLOR_TXT_D);                                           <*/
      glTranslatef  (0.0, g_goffy, 50.0);
      for (y = s_rowmap.gmin; y <= s_rowmap.gmax; y += g_gsizey) {
         for (x = s_colmap.gmin; x <= s_colmap.gmax; x += g_gsizex) {
            /*> glBegin         (GL_POINTS); {                                        <* 
             *>    glVertex3f  (x / a_mag, y / a_mag,  0.0f);                         <* 
             *> } glEnd   ();                                                         <*/
            glBegin         (GL_LINES); {
               glVertex3f  ((x - 5) / a_mag, y / a_mag,  0.0f);
               glVertex3f  ((x + 5) / a_mag, y / a_mag,  0.0f);
               glVertex3f  (x / a_mag, (y - 5) / a_mag,  0.0f);
               glVertex3f  (x / a_mag, (y + 5) / a_mag,  0.0f);
            } glEnd   ();
            ++cx;
         }
         ++cy;
      }
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_ribbon_add  (char *a_cat, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_icon      =    0;
   --rce;  if (a_cat  == NULL)  return rce;
   --rce;  if (a_name == NULL)  return rce;
   x_icon = yFONT_icon (a_cat, a_name, -1);
   /*> printf ("%2d  %-20.20s  %-20.20s  %3d\n", s_win.r_nicon, a_cat, a_name, x_icon);   <*/
   --rce;  if (x_icon < 0)  return rce;
   s_win.r_icons [s_win.r_nicon] = x_icon;
   ++s_win.r_nicon;
   return 0;
}

char
yVIKEYS_view_ribbon      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_side      =   35;
   /*> printf ("t %c %3d %3d %3d %3d %s\n", x_on, x_left, x_bott, x_wide, x_tall, x_text);   <*/
   if (s_win.r_on == '-')  return 0;
   /*---(setup view)---------------------*/
   glViewport      (s_win.r_left, s_win.r_bott, s_win.r_wide, s_win.r_tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_win.r_wide, 0.0f, s_win.r_tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(draw back)----------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_ACC_D);
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f      , s_win.r_tall,  0.0f);
         glVertex3f  (s_win.r_wide, s_win.r_tall,  0.0f);
         glVertex3f  (s_win.r_wide, 0.0f      ,  0.0f);
         glVertex3f  (0.0f      , 0.0f      ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(draw icons)---------------------*/
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_BLACK);
      glTranslatef  (2, s_win.r_tall - 3, 10.0);
      for (i = 0; i < s_win.r_nicon; ++i) {
         yFONT_iconno (s_win.r_icons [i], x_side);
         glTranslatef  (0, -x_side,  0.0);
      }
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_all         (void *a_drawer, float a_mag)
{
   /*---(clear)--------------------------*/
   glClear         (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   /*---(top)----------------------------*/
   yVIKEYS_view_buffer      ();
   yVIKEYS_view_formula     ();
   /*---(left)---------------------------*/
   yVIKEYS_view_title       ();
   yVIKEYS_view_version     ();
   /*> yVIKEYS_view_nav         ();                                                   <*/
   /*---(bottom)-------------------------*/
   /*> yVIKEYS_view_progress    ();                                                   <*/
   yVIKEYS_view_status      ();
   yVIKEYS_view_command     ();
   yVIKEYS_view_keys        ();
   /*---(right)--------------------------*/
   /*> yVIKEYS_view_details     ();                                                   <*/
   yVIKEYS_view_ribbon      ();
   /*---(main)---------------------------*/
   if (a_drawer != NULL) {
      s_drawer = a_drawer;
      s_drawer (a_mag);
   }
   yVIKEYS_view_grid   (a_mag);
   yVIKEYS_view_cursor (a_mag);
   /*---(flush)--------------------------*/
   glXSwapBuffers(DISP, BASE);
   glFlush();
   /*---(complete)-----------------------*/
   return 0;
}








/*============================----end-of-source---============================*/
