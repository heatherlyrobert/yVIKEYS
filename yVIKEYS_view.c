/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


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
#define     COLOR_CURSOR        'C'


char  yVIKEYS_view_ribbon      (void);
char  yVIKEYS__view_clear      (void);


static int    s_awide      = 0;
static int    s_atall      = 0;
static int    s_fwide      = 0;
static int    s_ftall      = 0;

char   (*s_drawer) (void);


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
   /*> char        w_title     [LEN_DESC];      /+ window title                   +/   <*/
   /*> int         w_wide;                      /+ window width                   +/   <*/
   /*> int         w_tall;                      /+ window heigth                  +/   <*/
   /*---(title sizes)-------*/
   /*> char        t_on;                        /+ title shown y/n                +/   <*/
   /*> char        t_text      [LEN_DESC];      /+ title text                     +/   <*/
   /*> int         t_wide;                      /+ width  of title line           +/   <*/
   /*> int         t_left;                      /+ left   of title line           +/   <*/
   /*> int         t_tall;                      /+ height of title line           +/   <*/
   /*> int         t_bott;                      /+ bottom of title line           +/   <*/
   /*---(buffer sizes)------*/
   /*> char        b_on;                        /+ buffer shown y/n               +/   <*/
   /*> int         b_wide;                      /+ width  of buffer bar           +/   <*/
   /*> int         b_left;                      /+ left   of buffer bar           +/   <*/
   /*> int         b_tall;                      /+ height of buffer bar           +/   <*/
   /*> int         b_bott;                      /+ bottom of buffer bar           +/   <*/
   /*---(formula sizes)-----*/
   /*> char        f_on;                        /+ formula shown y/n              +/   <*/
   /*> int         f_wide;                      /+ width  of formula line         +/   <*/
   /*> int         f_left;                      /+ left   of formula line         +/   <*/
   /*> int         f_tall;                      /+ height of formula line         +/   <*/
   /*> int         f_bott;                      /+ bottom of formula line         +/   <*/
   /*---(nav sizes)---------*/
   /*> char        n_on;                        /+ nav panel shown y/n            +/   <*/
   /*> int         n_wide;                      /+ width  of nav panel            +/   <*/
   /*> int         n_left;                      /+ left   of nav panel            +/   <*/
   /*> int         n_tall;                      /+ height of nav panel            +/   <*/
   /*> int         n_bott;                      /+ bottom of nav panel            +/   <*/
   /*---(main sizes)--------*/
   int         m_wide;                      /* width  of main window          */
   int         m_left;                      /* left   of main window          */
   int         m_tall;                      /* height of main window          */
   int         m_bott;                      /* bottom of main window          */
   int         g_on;                        /* show grid over main y/n        */
   int         m_xmin;
   int         m_xmax;
   int         m_ymin;
   int         m_ymax;
   /*---(progress)----------*/
   /*> char        p_on;                        /+ show progress y/n              +/   <*/
   /*> int         p_wide;                      /+ width  of progress window      +/   <*/
   /*> int         p_left;                      /+ left   of progress window      +/   <*/
   /*> int         p_tall;                      /+ height of progress window      +/   <*/
   /*> int         p_bott;                      /+ bottom of progress window      +/   <*/
   /*---(status sizes)------*/
   /*> char        s_on;                        /+ show status y/n                +/   <*/
   /*> char        s_text      [LEN_DESC];      /+ current text in status line    +/   <*/
   /*> int         s_wide;                      /+ width  of status line          +/   <*/
   /*> int         s_left;                      /+ left   of status line          +/   <*/
   /*> int         s_tall;                      /+ height of status line          +/   <*/
   /*> int         s_bott;                      /+ bottom of status line          +/   <*/
   /*---(command sizes)-----*/
   /*> char        c_on;                        /+ show command y/n               +/   <*/
   /*> char        c_text      [LEN_DESC];      /+ current text in command mode   +/   <*/
   /*> int         c_wide;                      /+ width  of command line         +/   <*/
   /*> int         c_left;                      /+ left   of command line         +/   <*/
   /*> int         c_tall;                      /+ height of command line         +/   <*/
   /*> int         c_bott;                      /+ bottom of command line         +/   <*/
   /*---(details)-----------*/
   /*> char        d_on;                        /+ show detail y/n                +/   <*/
   /*> int         d_wide;                      /+ width  of detail window        +/   <*/
   /*> int         d_left;                      /+ left   of detail window        +/   <*/
   /*> int         d_tall;                      /+ height of detail window        +/   <*/
   /*> int         d_bott;                      /+ bottom of detail window        +/   <*/
   /*---(ribbon sizes)------*/
   /*> char        r_on;                        /+ show ribbon y/n                +/   <*/
   /*> int         r_wide;                      /+ width  of ribbon/button bar    +/   <*/
   /*> int         r_left;                      /+ left   of ribbon/button bar    +/   <*/
   /*> int         r_tall;                      /+ height of ribbon/button bar    +/   <*/
   /*> int         r_bott;                      /+ bottom of ribbon/button bar    +/   <*/
   int         r_icons     [LEN_LABEL];     /* actual icons for ribbon        */
   int         r_nicon;                     /* number of icons                */
   /*---(version)-----------*/
   /*> char        v_on;                        /+ show version y/n               +/   <*/
   /*> char        v_text      [LEN_DESC];      /+ current version                +/   <*/
   /*> int         v_wide;                      /+ width  of version              +/   <*/
   /*> int         v_left;                      /+ left   of version              +/   <*/
   /*> int         v_tall;                      /+ height of version              +/   <*/
   /*> int         v_bott;                      /+ bottom of version              +/   <*/
   /*---(keys)--------------*/
   /*> char        k_on;                        /+ show keys y/n                  +/   <*/
   /*> char        k_text      [LEN_DESC];      /+ current keys                   +/   <*/
   /*> int         k_wide;                      /+ width  of key display          +/   <*/
   /*> int         k_left;                      /+ left   of key display          +/   <*/
   /*> int         k_tall;                      /+ height of key display          +/   <*/
   /*> int         k_bott;                      /+ bottom of key display          +/   <*/
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
   char        own;                         /* controlled entirely by yVIKEYS */
   char        on;                          /* show or hide y/n               */
   char        bottop;                      /* evaluation order bottom-to-top */
   char        lefrig;                      /* evaluation order left-to-right */
   int         def_wide;                    /* default width                  */
   int         def_tall;                    /* default height                 */
   char        x_tie;                       /* tie to another field in x-axis */
   char        y_tie;                       /* tie to another field in y-axis */
   int         wide;                        /* screen width                   */
   int         tall;                        /* screen height                  */
   int         left;                        /* screen left                    */
   int         bott;                        /* screen bottom                  */
   char        orient;                      /* orientation of text            */
   char        text        [LEN_STR ];      /* optional text                  */
   int         color;                       /* background color               */
   char        (*f) (void);                 /* drawing function               */
   char        desc        [LEN_DESC ];     /* explanation of element         */
};
static tPARTS  s_parts [MAX_PARTS] = {
   /*----------   ---name-----   own  on   bt lr d_wi d_ta  x_t  y_t  wi ta le bo  ori  txt color         drawing-function------    12345678901234567890123456789012345678901234567890  */
   { W_TITLE   , "title"       , 'y', 'y',  1, 1,  15,   0, '-', '-',  0, 0, 0, 0, 'r', "", COLOR_ACC_L , NULL                   , "left hand title bar"                                },
   { W_BUFFER  , "buffer"      , 'y', '-',  6, 2,   0,  15, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "buffer inventory at top"                            },
   { W_FORMULA , "formula"     , 'y', '-',  5, 2,   0,  15, '-', '-',  0, 0, 0, 0, '-', "", COLOR_DARK  , NULL                   , "formula and source editing line at top"             },
   { W_NAV     , "nav"         , 'y', '-',  3, 2, 150,   0, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "navigation panel to display tags and other links"   },
   { W_MAIN    , "main"        , '-', 'y',  4, 3,   0,   0, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "main working area in the middle"                    },
   { W_PROGRESS, "progress"    , 'y', '-',  3, 3,   0, 100, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "time and sequencing controls about status line"     },
   { W_STATUS  , "status"      , 'y', 'y',  2, 2,   0,  15, '-', '-',  0, 0, 0, 0, '-', "", COLOR_DARK  , NULL                   , "informational status bar above command line"        },
   { W_COMMAND , "command"     , 'y', 'y',  1, 2,   0,  15, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "command, search, and help message line at bottom"   },
   { W_DETAILS , "details"     , 'y', '-',  3, 4, 250,   0, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "display area for critical details to right"         },
   { W_RIBBON  , "ribbon"      , 'y', '-',  3, 5,  40,   0, '-', '-',  0, 0, 0, 0, '-', "", COLOR_ACC_D , yVIKEYS_view_ribbon    , "menu and icon display for navigation of commands"   },
   { W_VERSION , "version"     , 'y', 'y',  7, 1,  15,  40, '-', 't',  0, 0, 0, 0, 'r', "", COLOR_ACC_D , NULL                   , "version display with debugging notice"              },
   { W_KEYS    , "keys"        , 'y', 'y',  1, 6,  40,  15, 'c', '-',  0, 0, 0, 0, '-', "", COLOR_ACC_D , NULL                   , "latest keyboard characters typed"                   },
   { W_GRID    , "grid"        , 'y', '-',  0, 0,   0,   0, '-', '-',  0, 0, 0, 0, '-', "", COLOR_BASE  , NULL                   , "overlay main drawing with a grid"                   },
   { W_COORDS  , "coords"      , '-', '-',  0, 0,   0,   0, '-', '-',  0, 0, 0, 0, '-', "", 0           , NULL                   , "actual x,y coordinates in main window"              },
   { W_WINDOW  , "window"      , '-', '-',  0, 0,   0,   0, '-', '-',  0, 0, 0, 0, '-', "", 0           , NULL                   , "Full screen width and height"                       },
   { 0         , ""            , '-', '-',  0, 0,   0,   0, '-', '-',  0, 0, 0, 0, '-', "", 0           , NULL                   , ""                                                   },
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
   /*---name------    tbfnmpscdrvk    ,    12345678901234567890123456789012345678901234567890  */
   { "min"         , "----m-------"   , "smallest footprint, least elements showing"           },
   { "work"        , "t---m-sc--vk"   , "more balanced display of common elements"             },
   { "max"         , "tbfnmpscdrvk"   , "everything displays at one time"                      },
   { ""            , "----m-------"   , ""                                                     },
};
static int  s_nlayout   = 0;



int         g_goffx   =   0;
int         g_gsizex  =   1;
int         g_goffy   =   0;
int         g_gsizey  =   1;
int         g_goffz   =   0;
int         g_gsizez  =   1;

static  float  s_mag;


#define     ICON_SET  "/usr/local/share/fonts/outline_icons.png"



/*====================------------------------------------====================*/
/*===----                      window layout                           ----===*/
/*====================------------------------------------====================*/
static void  o___FIND____________o () { return; }

char
yVIKEYS__view_find      (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
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
   /*---(find the screen part)-----------*/
   for (i = 0; i < s_npart; ++i) {
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
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return n;
}

char
yVIKEYS__view_abbr      (char  a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_sint    (a_abbr);
   --rce;  if (a_abbr <= G_KEY_SPACE || a_abbr >= G_KEY_TILDA) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_schar   (a_abbr);
   DEBUG_GRAF   yLOG_sint    (s_npart);
   /*---(find the screen part)-----------*/
   for (i = 0; i < s_npart; ++i) {
      DEBUG_GRAF   yLOG_sint    (i);
      if (s_parts [i].abbr != a_abbr)         continue;
      n = i;
      break;
   }
   DEBUG_GRAF   yLOG_snote   (s_parts [i].name);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return n;
}



/*====================------------------------------------====================*/
/*===----                      window sizing                           ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char
yVIKEYS__view_widths     (cint a_wide)
{
   int         i           =    0;
   int         n           =    0;
   int         w           =    0;          /* widest at level                */
   int         cw          =    0;          /* cumulative left                */
   int         a           =    0;
   char        x_skip      =  '-';
   /*---(header)----------------------*/
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("a_wide"    , a_wide);
   /*---(do lefts first)-----------------*/
   for (i = 1; i < 9; ++i) {
      DEBUG_GRAF_M yLOG_value   ("i (level)" , i);
      w = 0;
      for (n = 0; n < s_npart; ++n) {
         if (s_parts [n].lefrig != i  )  continue;
         DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
         /*---(set left)-----------------*/
         s_parts [n].left = cw;
         DEBUG_GRAF_M yLOG_value   ("cw"        , cw);
         /*---(set width)----------------*/
         x_skip = '-';
         if      (s_parts [n].on     == '-'   )     s_parts [n].wide = 0;
         else if (s_parts [n].abbr   == W_MAIN)     s_parts [n].wide = a_wide;
         else if (s_parts [n].abbr   == W_PROGRESS) s_parts [n].wide = a_wide;
         else if (s_parts [n].x_tie  != '-'   ) {
            s_parts [n].wide  = s_parts [n].def_wide;
            s_parts [n].left -= s_parts [n].def_wide;
            a = yVIKEYS__view_abbr (s_parts [n].x_tie);
            s_parts [a].wide  = cw - s_parts [n].wide;
            x_skip = 'y';
         }
         else                                       s_parts [n].wide = s_parts [n].def_wide;
         /*---(greatest at level)--------*/
         if (s_parts [n].wide > w && x_skip == '-')   w = s_parts [n].wide;
         /*---(report out)---------------*/
         DEBUG_GRAF_M yLOG_value   ("left"      , s_parts [n].left);
         DEBUG_GRAF_M yLOG_value   ("wide"      , s_parts [n].wide);
         /*---(done)---------------------*/
      }
      cw += w;
   }
   DEBUG_GRAF_M yLOG_value   ("max wide"  , cw);
   /*---(fill in widths)-----------------*/
   DEBUG_GRAF_M yLOG_note    ("fill in remaining");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on   == '-')  continue;
      if (s_parts [n].wide != 0  )  continue;
      DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
      s_parts [n].wide = cw - s_parts [n].left;
   }
   /*---(full)---------------------------*/
   s_fwide = cw;
   DEBUG_GRAF_M yLOG_value   ("s_fwide"   , s_fwide);
   n = yVIKEYS__view_abbr (W_WINDOW);
   s_parts [n].wide = cw;
   /*---(complete)-----------------------*/
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__view_heights    (cint a_tall)
{
   int         i           =    0;
   int         n           =    0;
   int         h           =    0;          /* tallest at level               */
   int         ch          =    0;          /* cumulative bottom              */
   int         a           =    0;
   char        x_skip      =  '-';
   /*---(header)----------------------*/
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("a_tall"    , a_tall);
   /*---(do botts first)-----------------*/
   for (i = 1; i < 9; ++i) {
      DEBUG_GRAF_M yLOG_value   ("i (level)" , i);
      h = 0;
      for (n = 0; n < s_npart; ++n) {
         if (s_parts [n].bottop != i  )  continue;
         DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
         /*---(set bott)-----------------*/
         s_parts [n].bott = ch;
         DEBUG_GRAF_M yLOG_value   ("ch"        , ch);
         /*---(set width)----------------*/
         x_skip = '-';
         if      (s_parts [n].on     == '-'   )     s_parts [n].tall = 0;
         else if (s_parts [n].abbr   == W_MAIN)     s_parts [n].tall = a_tall;
         else if (s_parts [n].y_tie  != '-'   ) {
            s_parts [n].tall  = s_parts [n].def_tall;
            s_parts [n].bott -= s_parts [n].def_tall;
            a = yVIKEYS__view_abbr (s_parts [n].y_tie);
            s_parts [a].tall  = ch - s_parts [n].tall;
            x_skip = 'y';
         }
         else                                       s_parts [n].tall = s_parts [n].def_tall;
         /*---(greatest at level)--------*/
         if (s_parts [n].tall > h && x_skip == '-')   h = s_parts [n].tall;
         /*---(report out)---------------*/
         DEBUG_GRAF_M yLOG_value   ("bott"      , s_parts [n].bott);
         DEBUG_GRAF_M yLOG_value   ("tall"      , s_parts [n].tall);
         /*---(done)---------------------*/
      }
      ch += h;
   }
   DEBUG_GRAF_M yLOG_value   ("max tall"  , ch);
   /*---(fill in widths)-----------------*/
   DEBUG_GRAF_M yLOG_note    ("fill in remaining");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on   == '-')  continue;
      if (s_parts [n].tall != 0  )  continue;
      DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
      s_parts [n].tall = ch - s_parts [n].bott;
   }
   /*---(full)---------------------------*/
   s_ftall = ch;
   DEBUG_GRAF_M yLOG_value   ("s_ftall"   , s_ftall);
   n = yVIKEYS__view_abbr (W_WINDOW);
   s_parts [n].tall = ch;
   /*---(complete)-----------------------*/
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__view_resize     (char a_type)
{
   int         i           =    0;
   int         n           =    0;
   int         w           =    0;          /* widest at level                */
   int         cw          =    0;          /* cumulative left                */
   int         x_wide, x_tall;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   /*---(widths)-------------------------*/
   yVIKEYS__view_widths     (s_awide);
   DEBUG_GRAF   yLOG_value   ("s_awide"   , s_awide);
   DEBUG_GRAF   yLOG_value   ("s_fwide"   , s_fwide);
   /*---(heights)------------------------*/
   yVIKEYS__view_heights    (s_atall);
   DEBUG_GRAF   yLOG_value   ("s_atall"   , s_atall);
   DEBUG_GRAF   yLOG_value   ("s_ftall"   , s_ftall);
   /*---(display)------------------------*/
   for (n = 0; n < s_npart; ++n) {
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   }
   /*---(size window)--------------------*/
   if (a_type == 'r')   yX11_resize (s_fwide, s_ftall);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_resize      (int a_wide, int a_tall)
{
   if (a_wide > 50)  s_awide = a_wide;
   if (a_tall > 50)  s_atall = a_tall;
   yVIKEYS__view_resize ('r');
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      window layout                           ----===*/
/*====================------------------------------------====================*/
static void  o___LAYOUT__________o () { return; }

char
yVIKEYS__view_clear      (void)
{
   int         n           =    0;
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("count"     , s_npart);
   for (n = 0; n < s_npart; ++n) {
      s_parts [n].on   = '-';
      s_parts [n].left =   0;
      s_parts [n].wide =   0;
      s_parts [n].bott =   0;
      s_parts [n].tall =   0;
      /*> strlcpy (s_parts [n].text, "", LEN_LABEL);                                  <*/
      DEBUG_GRAF_M yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   }
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__view_set        (char *a_name, char *a_opt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_on        =  '-';
   char        i           =    0;
   char        n           =   -1;
   char        a           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   n = yVIKEYS__view_find (a_name);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set the flag)-------------------*/
   DEBUG_GRAF   yLOG_char    ("current"   , s_parts [n].on);
   x_on = s_parts [n].on;
   if (strcmp (a_opt, "hide") == 0)  s_parts [n].on = '-';
   if (strcmp (a_opt, "show") == 0)  s_parts [n].on = 'y';
   DEBUG_GRAF   yLOG_char    ("new"       , s_parts [n].on);
   /*---(handle linkages)----------------*/
   if (s_parts [n].abbr == W_TITLE   && s_parts [n].on == '-') {
      a = yVIKEYS__view_abbr (W_VERSION);
      s_parts [a].on = '-';
   }
   if (s_parts [n].abbr == W_COMMAND && s_parts [n].on == '-') {
      a = yVIKEYS__view_abbr (W_KEYS   );
      s_parts [a].on = '-';
   }
   /*---(resize)-------------------------*/
   if (x_on != s_parts [n].on) {
      DEBUG_GRAF   yLOG_note    ("must resize");
      yVIKEYS__view_resize ('r');
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__view_layout     (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        n           =   -1;
   char        p           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_name"    , a_name);
   /*---(find the layout)----------------*/
   for (i = 0; i < s_nlayout; ++i) {
      DEBUG_GRAF   yLOG_info    ("check"     , s_layouts [i].name);
      if (s_layouts [i].name [0] != a_name [0])         continue;
      if (strcmp (s_layouts [i].name, a_name) != 0)     continue;
      n = i;
      break;
   }
   DEBUG_GRAF   yLOG_value   ("result"    , n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear the parts)----------------*/
   yVIKEYS__view_clear ();
   /*---(set parts)----------------------*/
   i = 0;
   while (s_layouts [n].parts [i] != 0) {
      if (s_layouts [n].parts [i] != '-') {
         p = yVIKEYS__view_abbr (s_layouts [n].parts [i]);
         if (p >= 0) {
            DEBUG_GRAF   yLOG_info    ("turn on"   , s_parts [p].name);
            s_parts [p].on = 'y';
         }
      }
      ++i;
   }
   /*---(reset)--------------------------*/
   yVIKEYS__view_resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      program level                           ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yVIKEYS_view_init       (char *a_title, char *a_ver, int a_wide, int a_tall, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         n           =    0;
   char        rc          =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(functions)----------------------*/
   s_drawer  = NULL;
   if (a_drawer != NULL)  s_drawer = a_drawer;
   /*---(screen parts)-------------------*/
   s_npart = 0;
   for (i = 0; i < MAX_PARTS; ++i) {
      if (s_parts [i].abbr == 0)  break;
      DEBUG_GRAF   yLOG_info    ("name"      , s_parts [i].name);
      if (s_parts [i].own != '-') {
         DEBUG_GRAF   yLOG_note    ("adding view menu command to show/hide");
         yVIKEYS_cmds_add ('v', s_parts [i].name , ""    , "Cs"   , yVIKEYS__view_set          , "" );
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
   /*---(ribbon)-------------------------*/
   for (i = 0; i < LEN_LABEL; ++i)  s_win.r_icons [i] = -1;
   s_win.r_nicon = 0;
   /*---(commands)-----------------------*/
   yVIKEYS_cmds_add ('v', "gridoff"     , ""    , "iii"  , yVIKEYS_view_set_gridoff  , "" );
   yVIKEYS_cmds_add ('v', "gridsize"    , ""    , "iii"  , yVIKEYS_view_set_gridsize , "" );
   /*---(key data)-----------------------*/
   s_awide   = a_wide;
   s_atall   = a_tall;
   DEBUG_GRAF   yLOG_value   ("s_awide"   , s_awide);
   DEBUG_GRAF   yLOG_value   ("s_atall"   , s_atall);
   /*---(open window)--------------------*/
   rc = yX11_start      (a_title, 500, 300, YX_FOCUSABLE, YX_FIXED, '-');
   yVIKEYS__view_layout ("work");
   /*---(set text data)------------------*/
   if (a_title != NULL) {
      n = yVIKEYS__view_abbr (W_TITLE);
      strlcpy (s_parts [n].text, a_title, LEN_DESC );
   }
   if (a_ver   != NULL) {
      n = yVIKEYS__view_abbr (W_VERSION);
      strlcpy (s_parts [n].text, a_ver  , LEN_DESC );
   }
   n = yVIKEYS__view_abbr (W_COMMAND);
   yVIKEYS_mode_mesg (s_parts [n].text, yVIKEYS_cmds_curr ());
   /*---(color)--------------------------*/
   DEBUG_GRAF   yLOG_note    ("clearing");
   glClearColor    (1.0f, 1.0f, 1.0f, 1.0f);
   glClearDepth    (1.0f);
   /*---(textures)-----------------------*/
   DEBUG_GRAF   yLOG_note    ("textures");
   glEnable        (GL_TEXTURE_2D);    /* NEW */
   /*---(blending)-----------------------*/
   DEBUG_GRAF   yLOG_note    ("blending");
   glShadeModel    (GL_SMOOTH);
   glEnable        (GL_DEPTH_TEST);
   glEnable        (GL_ALPHA_TEST);
   glAlphaFunc     (GL_GEQUAL, 0.0125);
   glEnable        (GL_BLEND);
   glBlendFunc     (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthFunc     (GL_LEQUAL);
   /*---(anti-aliasing)------------------*/
   DEBUG_GRAF   yLOG_note    ("anti-aliasing");
   glHint          (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   /*---(special polygon antialiasing)----------*/
   DEBUG_GRAF   yLOG_note    ("polygon");
   glEnable        (GL_POLYGON_SMOOTH);
   glPolygonMode   (GL_FRONT_AND_BACK, GL_FILL);
   glHint          (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   /*---(simple defaulting)--------------*/
   DEBUG_GRAF   yLOG_note    ("sizes");
   glLineWidth     (0.50f);
   /*---(process immediately)------------*/
   DEBUG_GRAF   yLOG_note    ("flush");
   glFlush       ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_wrap       (void)
{
   yX11_end     ();
   return 0;
}



char
yVIKEYS_view_main_coords (int a_xmin, int a_xmax, int a_ymin, int a_ymax)
{
   s_win.m_xmin = a_xmin;
   s_win.m_xmax = a_xmax;
   s_win.m_ymin = a_ymin;
   s_win.m_ymax = a_ymax;
   return 0;
}

char
yVIKEYS_view_text       (cchar *a_text)
{
   int         n           =    0;
   n = yVIKEYS__view_abbr (W_COMMAND);
   if (n < 0)  return -1;
   strlcpy (s_parts [n].text, a_text, LEN_DESC);
   return 0;
}

char
yVIKEYS_view_keys       (cchar *a_text)
{
   int         n           =    0;
   n = yVIKEYS__view_abbr (W_KEYS   );
   if (n < 0)  return -1;
   strlcpy (s_parts [n].text, a_text, LEN_DESC);
   return 0;
}

char
yVIKEYS_view_corners    (cchar a_part, int *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yVIKEYS__view_abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      if (a_left != NULL)  *a_left  = 0;
      if (a_bott != NULL)  *a_bott  = 0;
      if (a_wide != NULL)  *a_wide  = 0;
      if (a_tall != NULL)  *a_tall  = 0;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      return n;
   }
   DEBUG_GRAF   yLOG_note    ("save values for good entry");
   if (a_left != NULL)  *a_left  = s_parts [n].left;
   if (a_bott != NULL)  *a_bott  = s_parts [n].bott;
   if (a_wide != NULL)  *a_wide  = s_parts [n].wide;
   if (a_tall != NULL)  *a_tall  = s_parts [n].tall;
   if (a_text != NULL)  strlcpy (a_text, s_parts [n].text, LEN_DESC);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return s_parts [n].on;
}

char
yVIKEYS_view_color      (char a_color)
{
   switch (a_color) {
   case  COLOR_BASE   :
      glColor4f (0.30f, 0.50f, 0.30f, 1.00f);       /* nice medium green          */
      break;
   case  COLOR_DARK   :
      glColor4f (0.25f, 0.40f, 0.25f, 1.00f);
      break;
   case  COLOR_LIGHT  :
      glColor4f (0.40f, 0.65f, 0.35f, 1.00f);
      break;
   case  COLOR_MUTED  :
      glColor4f (0.30f, 0.10f, 0.00f, 0.30f);
      break;
   case  COLOR_ACC_L  :
      glColor4f (1.00f, 0.60f, 0.30f, 0.30f);
      break;
   case  COLOR_ACC_D  :
      glColor4f (0.70f, 0.20f, 0.10f, 0.40f);
      break;
   case  COLOR_ACC_O  :
      glColor4f (0.80f, 0.30f, 0.00f, 0.30f);
      break;
   case  COLOR_GRID_L :
      glColor4f (1.00f, 1.00f, 1.00f, 0.50f);
      break;
   case  COLOR_GRID_D :
      glColor4f (0.00f, 0.00f, 0.00f, 0.70f);
      break;
   case  COLOR_TXT_D  :
      glColor4f (0.20f, 0.20f, 0.20f, 1.00f);
      break;
   case  COLOR_TXT_L  :
      glColor4f (0.00f, 0.00f, 0.00f, 0.70f);
      break;
   case  COLOR_BLACK  :
      glColor4f (0.00f, 0.00f, 0.00f, 1.00f);
      break;
   case  COLOR_WARN   :
      glColor4f (0.50f, 0.00f, 0.00f, 1.00f);
      break;
   case  COLOR_CURSOR :
      glColor4f    (1.00f, 0.00f, 0.00f, 0.2f);
      break;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        standard drawing                      ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

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
yVIKEYS_view_cursor      (void)
{
   int         x_lef   = s_colmap.cur / s_mag;
   int         x_bot   = s_rowmap.cur / s_mag;
   int         x_rig   = x_lef + (g_gsizex / s_mag);
   int         x_top   = x_bot + (g_gsizey / s_mag);
   yVIKEYS_view_color (COLOR_CURSOR);
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
yVIKEYS_view_grid        (void)
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
         glTranslatef  (x_inc / s_mag, 0.0    ,  0.0);
         ++c;
      }
   } glPopMatrix   ();
   /*---(y grid)-------------------------*/
   glPushMatrix    (); {
      x_beg = s_rowmap.gmin;
      /*> printf ("x_beg %3d, x_end %3d, x_inc %3d\n", x_beg, x_end, x_inc);          <*/
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
         glTranslatef  (0.0, x_inc / s_mag,  0.0);
         ++c;
      }
   } glPopMatrix   ();
   /*---(xy dots)------------------------*/
   glPushMatrix    (); {
      /*> glScalef      (s_mag, s_mag, s_mag);                                        <*/
      glPointSize   (5);
      glLineWidth   (1.5);
      glColor4f     (0.0, 0.3, 0.0, 0.5);
      /*> yVIKEYS_view_color (COLOR_TXT_D);                                           <*/
      glTranslatef  (0.0, g_goffy, 50.0);
      for (y = s_rowmap.gmin; y <= s_rowmap.gmax; y += g_gsizey) {
         for (x = s_colmap.gmin; x <= s_colmap.gmax; x += g_gsizex) {
            /*> glBegin         (GL_POINTS); {                                        <* 
             *>    glVertex3f  (x / s_mag, y / s_mag,  0.0f);                         <* 
             *> } glEnd   ();                                                         <*/
            glBegin         (GL_LINES); {
               glVertex3f  ((x - 5) / s_mag, y / s_mag,  0.0f);
               glVertex3f  ((x + 5) / s_mag, y / s_mag,  0.0f);
               glVertex3f  (x / s_mag, (y - 5) / s_mag,  0.0f);
               glVertex3f  (x / s_mag, (y + 5) / s_mag,  0.0f);
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
   /*> /+---(setup view)---------------------+/                                       <* 
    *> glViewport      (s_win.r_left, s_win.r_bott, s_win.r_wide, s_win.r_tall);      <* 
    *> glMatrixMode    (GL_PROJECTION);                                               <* 
    *> glLoadIdentity  ();                                                            <* 
    *> glOrtho         ( 0.0f, s_win.r_wide, 0.0f, s_win.r_tall,  -500.0,   500.0);   <* 
    *> glMatrixMode    (GL_MODELVIEW);                                                <* 
    *> /+---(draw back)----------------------+/                                       <* 
    *> glPushMatrix    (); {                                                          <* 
    *>    yVIKEYS_view_color (COLOR_ACC_D);                                           <* 
    *>    glBegin         (GL_POLYGON); {                                             <* 
    *>       glVertex3f  (0.0f      , s_win.r_tall,  0.0f);                           <* 
    *>       glVertex3f  (s_win.r_wide, s_win.r_tall,  0.0f);                         <* 
    *>       glVertex3f  (s_win.r_wide, 0.0f      ,  0.0f);                           <* 
    *>       glVertex3f  (0.0f      , 0.0f      ,  0.0f);                             <* 
    *>    } glEnd   ();                                                               <* 
    *> } glPopMatrix   ();                                                            <*/
   /*---(draw icons)---------------------*/
   /*> glPushMatrix    (); {                                                          <* 
    *>    yVIKEYS_view_color (COLOR_BLACK);                                           <* 
    *>    glTranslatef  (2, s_win.r_tall - 3, 10.0);                                  <* 
    *>    for (i = 0; i < s_win.r_nicon; ++i) {                                       <* 
    *>       yFONT_iconno (s_win.r_icons [i], x_side);                                <* 
    *>       glTranslatef  (0, -x_side,  0.0);                                        <* 
    *>    }                                                                           <* 
    *> } glPopMatrix   ();                                                            <*/
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_float      (void)
{
   /*---(locals)-----------+------+----+-*/
   char        x_use       =   '-';
   int         x_top       =     0;
   int         x_bot       =     0;
   int         x_lef       =     0;
   int         x_rig       =     0;
   int         x_len       =     0;
   int         n           =     0;
   /*---(defense)------------------------*/
   if (yVIKEYS_mode_curr() == MODE_COMMAND || yVIKEYS_mode_curr() == MODE_SEARCH) {
      n    = yVIKEYS__view_abbr   (W_COMMAND);
      if (s_parts [n].on == '-')  x_use = 'y';
   }
   if (x_use != 'y')    return 0;
   /*---(draw)---------------------------*/
   x_bot = s_win.m_bott + 20;
   x_top = x_bot + 15;
   x_lef = 20;
   x_rig = s_win.m_wide - 20;
   glPushMatrix    (); {
      yVIKEYS_view_color (COLOR_DARK );
      glBegin         (GL_POLYGON); {
         glVertex3f  (x_lef , x_top, 50.0f);
         glVertex3f  (x_rig , x_top, 50.0f);
         glVertex3f  (x_rig , x_bot, 50.0f);
         glVertex3f  (x_lef , x_bot, 50.0f);
      } glEnd   ();
      yVIKEYS_view_color (COLOR_BLACK);
      glBegin(GL_LINE_STRIP); {
         glVertex3f  (x_lef , x_top, 50.0f);
         glVertex3f  (x_rig , x_top, 50.0f);
         glVertex3f  (x_rig , x_bot, 50.0f);
         glVertex3f  (x_lef , x_bot, 50.0f);
         glVertex3f  (x_lef , x_top, 50.0f);
      } glEnd   ();
      yVIKEYS_view_color (COLOR_TXT_L);
      glTranslatef (x_lef + 3, x_bot,   60.0f);
      switch (yVIKEYS_mode_curr ()) {
      case MODE_COMMAND :  yFONT_print  (s_win.font,  11, YF_BOTLEF, yVIKEYS_cmds_curr ());  break;
      case MODE_SEARCH  :  yFONT_print  (s_win.font,  11, YF_BOTLEF, yVIKEYS_srch_curr ());  break;
      }
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}


char
yVIKEYS__view_opengl     (char a)
{
   /*---(defense)------------------------*/
   if (s_parts [a].on  == '-')  return -2;
   if (s_parts [a].own == '-')  return -3;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(setup view)---------------------*/
   DEBUG_GRAF   yLOG_note    ("view setup");
   DEBUG_GRAF   yLOG_value   ("a"         , a);
   DEBUG_GRAF   yLOG_complex (s_parts [a].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [a].bott, s_parts [a].left, s_parts [a].wide, s_parts [a].tall, s_parts [a].on);
   glViewport      (s_parts [a].left, s_parts [a].bott, s_parts [a].wide, s_parts [a].tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         ( 0.0f, s_parts [a].wide, 0.0f, s_parts [a].tall,  -500.0,   500.0);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   DEBUG_GRAF   yLOG_note    ("draw background");
   glPushMatrix    (); {
      /*> glColor4f     (1.0, 1.0, 1.0, 1.0);                                         <*/
      yVIKEYS_view_color (s_parts [a].color);
      /*> printf ("color for %s is %c\n", s_parts [a].name, s_parts [a].color);       <*/
      if (s_parts [a].abbr == W_VERSION && yURG_debugmode () == 'y')  yVIKEYS_view_color (COLOR_WARN );
      glBegin         (GL_POLYGON); {
         glVertex3f  (0.0f            , s_parts [a].tall,  0.0f);
         glVertex3f  (s_parts [a].wide, s_parts [a].tall,  0.0f);
         glVertex3f  (s_parts [a].wide, 0.0f            ,  0.0f);
         glVertex3f  (0.0f            , 0.0f            ,  0.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display text)-------------------*/
   DEBUG_GRAF   yLOG_note    ("draw text");
   if (s_parts [a].text != NULL && strlen (s_parts [a].text) > 0) {
      glPushMatrix    (); {
         yVIKEYS_view_color (COLOR_BLACK);
         if (s_parts [a].orient == 'r') {
            glTranslatef (s_parts [a].wide + 1,   5.0f,    0.0f);
            glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
         } else {
            glTranslatef (3.0f            ,   1.0f,    0.0f);
         }
         yFONT_print  (s_win.font,  11, YF_BOTLEF, s_parts [a].text);
      } glPopMatrix   ();
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return a;
}

char
yVIKEYS_view_all         (float a_mag)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   s_mag = a_mag;
   DEBUG_GRAF   yLOG_double  ("s_mag"     , s_mag);
   glClearColor    (1.0f, 1.0f, 1.0f, 1.0f);
   glClear         (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   /*---(draw each element)--------------*/
   for (n = 0; n < s_npart; ++n) {
      DEBUG_GRAF   yLOG_value   ("part"      , n);
      if (s_parts [n].own == 'y')  rc = yVIKEYS__view_opengl (n);
   }
   /*---(main)---------------------------*/
   if (s_drawer != NULL) s_drawer ();
   /*> yVIKEYS_view_float  ();                                                        <*/
   yVIKEYS_view_grid   ();
   yVIKEYS_view_cursor ();
   /*---(flush)--------------------------*/
   glXSwapBuffers(DISP, BASE);
   glFlush();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_unit      (char *a_question, char a_index)
{
   char        n;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "VIEW unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "size"           )   == 0) {
      n    = yVIKEYS__view_abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VIEW %-12.12s: on %c, left %4d, wide %4d, bott %4d, tall %4d", s_parts [n].name, s_parts [n].on, s_parts [n].left, s_parts [n].wide, s_parts [n].bott, s_parts [n].tall);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


