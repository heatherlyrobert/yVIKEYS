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
char  yVIKEYS__view_reset      (void);


static int    s_main_wide  = 0;
static int    s_main_tall  = 0;
static int    s_alt_wide   = 0;
static int    s_full_wide  = 0;
static int    s_full_tall  = 0;



/*===[[ LAYOUT ]]=============================================================*/
/*---(window formatting)-------------------*/
typedef struct  cWIN  tWIN;
struct cWIN {
   /*---(header)------------*/
   char        a_format;                    /* format identifier              */
   char        a_adapt;                     /* adapt to add elements          */
   int         a_wide;                      /* asking width                   */
   int         a_tall;                      /* asking heigth                  */
   /*---(main sizes)--------*/
   int         grid_on;                     /* show grid over main y/n        */
   int         cursor_on;                   /* show grid over main y/n        */
   int         m_xmin;
   int         m_xmax;
   int         m_ymin;
   int         m_ymax;
   /*---(ribbon)------------*/
   int         r_icons     [LEN_LABEL];     /* actual icons for ribbon        */
   int         r_nicon;                     /* number of icons                */
   /*---(display)-----------*/
   char        face;
   int         font;    
   int         icons;
   /*---(done)--------------*/
};
static tWIN   s_win;


#define      OWN_FULLY        'y'   /* only change color, text, and hiding    */
#define      OWN_PARTLY       'p'   /* change hiding, color, and drawing      */
#define      OWN_LITTLE       's'   /* change anything and everything         */
#define      OWN_MAIN         'm'   /* change anything and everything         */
#define      OWN_OVERLAY      'o'   /* drawn after everything else            */
#define      OWN_DATA         '-'   /* just a data holder                     */

#define      OWN_SETUP        "ypsm"
#define      OWN_HIDE         "ypso"
#define      OWN_REANCHOR     "yp"

#define      OWN_COMPLEX      "ms"
#define      OWN_MODERATE     "msp"
#define      OWN_SIMPLE       "p"

#define      MAX_PARTS         20
typedef  struct  cPARTS     tPARTS;
struct cPARTS {
   /*---(main)-----------------*/
   cchar       abbr;                        /* short name of screen element   */
   cchar       name        [LEN_LABEL];     /* name of screen element         */
   cchar       own;                         /* controlled entirely by yVIKEYS */
   char        on;                          /* show or hide y/n               */
   /*---(sizing)---------------*/
   cchar       bottop;                      /* evaluation order bottom-to-top */
   cchar       lefrig;                      /* evaluation order left-to-right */
   cint        def_wide;                    /* default width                  */
   cint        def_tall;                    /* default height                 */
   cchar       x_tie;                       /* tie to another field in x-axis */
   cchar       y_tie;                       /* tie to another field in y-axis */
   cchar       under;                       /* fall below buffer/formula      */
   int         wide;                        /* screen width                   */
   int         tall;                        /* screen height                  */
   int         left;                        /* screen left                    */
   int         bott;                        /* screen bottom                  */
   /*---(for std elements)-----*/
   char        orient;                      /* orientation of text            */
   char        text        [LEN_STR ];      /* optional text                  */
   /*---(special drawing)------*/
   char        (*drawer) (void);            /* drawing function               */
   char        type;                        /* ortho vs 3d                    */
   char        mgmt;                        /* auto vs custom setup           */
   char        anchor;                      /* fixed point for resizing       */
   int         color;                       /* background color               */
   int         xmin;                        /* x-coord minimum                */
   int         xlen;                        /* x-coord range                  */
   int         ymin;                        /* y-coord minimum                */
   int         ylen;                        /* y-coord range                  */
   int         zmin;                        /* z-coord minimum                */
   int         zlen;                        /* z-coord range                  */
   /*---(other)----------------*/
   cchar       desc        [LEN_DESC ];     /* explanation of element         */
   /*---(done)-----------------*/
};
static tPARTS  s_parts [MAX_PARTS] = {
   /*---abbr---------   ---name-----   ---own-----  on    bt lr d_wi d_ta  x_t  y_t  und  wi ta le bo   ori  txt  drawing-function------   type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_TITLE   , "title"       , OWN_FULLY  , 'y',   1, 1,  15,   0, '-', '-', '-',  0, 0, 0, 0,  'r', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "left hand title bar"                                },
   { YVIKEYS_BUFFER  , "buffer"      , OWN_FULLY  , '-',   6, 2,   0,  15, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "buffer inventory at top"                            },
   { YVIKEYS_FORMULA , "formula"     , OWN_FULLY  , '-',   5, 2,   0,  15, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "formula and source editing line at top"             },
   { YVIKEYS_NAV     , "nav"         , OWN_PARTLY , '-',   3, 2, 150,   0, '-', '-', 'y',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "navigation panel to display tags and other links"   },
   { YVIKEYS_ALT     , "alt"         , OWN_LITTLE , '-',   4, 4,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "alternate working area or view"                     },
   { YVIKEYS_PROGRESS, "progress"    , OWN_PARTLY , '-',   3, 3,   0, 100, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "time and sequencing controls about status line"     },
   { YVIKEYS_STATUS  , "status"      , OWN_FULLY  , 'y',   2, 2,   0,  15, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational status bar above command line"        },
   { YVIKEYS_COMMAND , "command"     , OWN_FULLY  , 'y',   1, 2,   0,  15, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "command, search, and help message line at bottom"   },
   { YVIKEYS_DETAILS , "details"     , OWN_PARTLY , '-',   3, 5, 250,   0, '-', '-', 'y',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "display area for critical details to right"         },
   { YVIKEYS_RIBBON  , "ribbon"      , OWN_FULLY  , '-',   3, 6,  40,   0, '-', '-', 'y',  0, 0, 0, 0,  '-', "",  yVIKEYS_view_ribbon    , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "menu and icon display for navigation of commands"   },
   { YVIKEYS_VERSION , "version"     , OWN_FULLY  , 'y',   7, 1,  15,  40, '-', 't', '-',  0, 0, 0, 0,  'r', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "version display with debugging notice"              },
   { YVIKEYS_KEYS    , "keys"        , OWN_FULLY  , 'y',   1, 7,  40,  15, 'c', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "latest keyboard characters typed"                   },
   { YVIKEYS_GRID    , "grid"        , OWN_OVERLAY, '-',   0, 0,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "overlay main drawing with a grid"                   },
   { YVIKEYS_CURSOR  , "cursor"      , OWN_OVERLAY, '-',   0, 0,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "cursor display on screen"                           },
   { YVIKEYS_WINDOW  , "window"      , OWN_DATA   , '-',   0, 0,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "Full screen width and height"                       },
   { YVIKEYS_MAIN    , "main"        , OWN_MAIN   , 'y',   4, 3,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "main working area in the middle"                    },
   { 0               , ""            , 0          , '-',   0, 0,   0,   0, '-', '-', '-',  0, 0, 0, 0,  '-', "",  NULL                   , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  ""                                                   },
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
   /*---name------    tbfnmapscdrvk    ,    12345678901234567890123456789012345678901234567890  */
   { "min"         , "----m--------"   , "smallest footprint, least elements showing"           },
   { "work"        , "t---m--sc--vk"   , "more balanced display of common elements"             },
   { "rob"         , "t---m--sc--v-"   , "more balanced display of common elements"             },
   { "max"         , "tbfnm-pscdrvk"   , "everything displays at one time"                      },
   { "ycolor"      , "tbfnmapsc-rvk"   , "more balanced display of common elements"             },
   { ""            , "----m--------"   , ""                                                     },
};
static int  s_nlayout   = 0;



int         g_goffx   =   0;
int         g_gsizex  =  60;
int         g_goffy   =   0;
int         g_gsizey  =  60;
int         g_goffz   =   0;
int         g_gsizez  =  60;

static  float  s_mag;



/*====================------------------------------------====================*/
/*===----                       utility functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

char         /*-> purge all changeable fields --------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_purge      (void)
{
   int         n           =    0;
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("count"     , s_npart);
   for (n = 0; n < s_npart; ++n) {
      s_parts [n].left   = s_parts [n].wide   = 0;
      s_parts [n].bott   = s_parts [n].tall   = 0;
      strlcpy (s_parts [n].text, "", LEN_LABEL);
      if (s_parts [n].abbr != YVIKEYS_RIBBON)  s_parts [n].drawer = NULL;
      s_parts [n].type   = YVIKEYS_FLAT;
      s_parts [n].color  = YCOLOR_GRY;
      s_parts [n].xmin   = s_parts [n].xlen   = 0;
      s_parts [n].ymin   = s_parts [n].ylen   = 0;
      s_parts [n].ymin   = s_parts [n].ylen   = 0;
   }
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare for auto-layout ------------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_reset      (void)
{
   int         n           =    0;
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("count"     , s_npart);
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].abbr != YVIKEYS_MAIN  )  s_parts [n].on   = '-';
      s_parts [n].left   = s_parts [n].wide   =   0;
      s_parts [n].bott   = s_parts [n].tall   =   0;
      /*> strlcpy (s_parts [n].text, "", LEN_LABEL);                                  <*/
      DEBUG_GRAF_M yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   }
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> locate screen parts by name --------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_find      (cchar *a_name)
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

char         /*-> locate screen parts by abbrev ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_abbr      (cchar  a_abbr)
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

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_widths     (cint a_wide, cint a_alt)
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
         else if (s_parts [n].abbr   == YVIKEYS_MAIN)     s_parts [n].wide = a_wide;
         else if (s_parts [n].abbr   == YVIKEYS_ALT )     s_parts [n].wide = a_alt;
         else if (s_parts [n].abbr   == YVIKEYS_PROGRESS) s_parts [n].wide = a_wide;
         else if (s_parts [n].x_tie  != '-'   ) {
            s_parts [n].wide  = s_parts [n].def_wide;
            s_parts [n].left -= s_parts [n].def_wide;
            a = yVIKEYS__view_abbr (s_parts [n].x_tie);
            s_parts [a].wide  = cw - s_parts [n].wide - s_parts [a].left;
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
      if (s_parts [n].abbr == YVIKEYS_PROGRESS)  s_parts [n].wide = a_wide + a_alt;
      if (s_parts [n].wide != 0  )  continue;
      DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
      s_parts [n].wide = cw - s_parts [n].left;
   }
   /*---(full)---------------------------*/
   s_full_wide = cw;
   DEBUG_GRAF_M yLOG_value   ("full_wide" , s_full_wide);
   n = yVIKEYS__view_abbr (YVIKEYS_WINDOW);
   s_parts [n].wide = cw;
   /*---(complete)-----------------------*/
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize heights based on layout -----[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_heights    (cint a_tall)
{
   int         i           =    0;
   int         n           =    0;
   int         h           =    0;          /* tallest at level               */
   int         ch          =    0;          /* cumulative bottom              */
   int         a           =    0;
   char        x_skip      =  '-';
   int         x_under     =    0;
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
         else if (s_parts [n].abbr   == YVIKEYS_MAIN)     s_parts [n].tall = a_tall;
         else if (s_parts [n].abbr   == YVIKEYS_ALT )     s_parts [n].tall = a_tall;
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
   /*---(fill in heights)----------------*/
   DEBUG_GRAF_M yLOG_note    ("fill in remaining");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on   == '-')  continue;
      if (s_parts [n].tall != 0  )  continue;
      DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
      s_parts [n].tall = ch - s_parts [n].bott;
   }
   /*---(deal with unders)---------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_BUFFER);
   x_under += s_parts [n].tall;
   n = yVIKEYS__view_abbr (YVIKEYS_FORMULA);
   x_under += s_parts [n].tall;
   DEBUG_GRAF_M yLOG_value   ("x_under"   , x_under);
   DEBUG_GRAF_M yLOG_note    ("deal with unders");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on    == '-')  continue;
      if (s_parts [n].under != 'y')  continue;
      DEBUG_GRAF_M yLOG_info    ("name"      , s_parts [n].name);
      s_parts [n].tall = s_parts [n].tall - x_under;
   }
   /*---(full)---------------------------*/
   s_full_tall = ch;
   DEBUG_GRAF_M yLOG_value   ("full_tall" , s_full_tall);
   n = yVIKEYS__view_abbr (YVIKEYS_WINDOW);
   s_parts [n].tall = ch;
   /*---(complete)-----------------------*/
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__view_reanchor    (cchar a)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_len       =    0;
   int         y_len       =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(handle auto first)--------------*/
   if (s_parts [a].mgmt != YVIKEYS_AUTO) return 0;
   s_parts [a].type = YVIKEYS_FLAT;
   x_len = s_parts [a].xlen = s_parts [a].wide;
   y_len = s_parts [a].ylen = s_parts [a].tall;
   switch (s_parts [a].anchor) {
   case YVIKEYS_TOPLEF :
      s_parts [a].xmin = 0;
      s_parts [a].ymin = 0 - y_len;
      break;
   case YVIKEYS_TOPCEN :
      s_parts [a].xmin = 0 - (x_len / 2);
      s_parts [a].ymin = 0 - y_len;
      break;
   case YVIKEYS_TOPRIG :
      s_parts [a].xmin = 0 - x_len;
      s_parts [a].ymin = 0 - y_len;
      break;
   case YVIKEYS_MIDLEF :
      s_parts [a].xmin = 0;
      s_parts [a].ymin = 0 - (y_len / 2);
      break;
   case YVIKEYS_MIDCEN :
      s_parts [a].xmin = 0 - (x_len / 2);
      s_parts [a].ymin = 0 - (y_len / 2);
      break;
   case YVIKEYS_MIDRIG :
      s_parts [a].xmin = 0 - x_len;
      s_parts [a].ymin = 0 - (y_len / 2);
      break;
   case YVIKEYS_BOTLEF :
      s_parts [a].xmin = 0;
      s_parts [a].ymin = 0;
      break;
   case YVIKEYS_BOTCEN :
      s_parts [a].xmin = 0 - (x_len / 2);
      s_parts [a].ymin = 0;
      break;
   case YVIKEYS_BOTRIG :
      s_parts [a].xmin = 0 - x_len;
      s_parts [a].ymin = 0;
      break;
   }
   s_parts [a].zmin = -500;
   s_parts [a].zlen = 1000;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize all based on main -----------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS__view_resize     (cchar a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   /*---(widths)-------------------------*/
   yVIKEYS__view_widths     (s_main_wide, s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("main_wide" , s_main_wide);
   DEBUG_GRAF   yLOG_value   ("alt_wide"  , s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_wide);
   /*---(heights)------------------------*/
   yVIKEYS__view_heights    (s_main_tall);
   DEBUG_GRAF   yLOG_value   ("main_tall" , s_main_tall);
   DEBUG_GRAF   yLOG_value   ("full_tall" , s_full_tall);
   /*---(display)------------------------*/
   for (n = 0; n < s_npart; ++n) {
      yVIKEYS__view_reanchor (n);
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "xmin %4d, xlen %4d, ymin %4d, ylen %4d, zmin %4d, zlen %4d", s_parts [n].xmin, s_parts [n].xlen, s_parts [n].ymin, s_parts [n].ylen, s_parts [n].zmin, s_parts [n].zlen);
   }
   /*---(size window)--------------------*/
   if (a_type == 'r')   yX11_resize (s_full_wide, s_full_tall);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_resize      (cint a_wide, cint a_tall, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_wide"    , a_wide);
   DEBUG_GRAF   yLOG_value   ("a_tall"    , a_tall);
   DEBUG_GRAF   yLOG_value   ("a_alt"     , a_alt);
   /*---(update globals)-----------------*/
   if (a_wide > 50)   s_main_wide = a_wide;
   if (a_tall > 50)   s_main_tall = a_tall;
   if (a_alt  > 50)   s_alt_wide  = a_alt;
   yVIKEYS__view_resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      window layout                           ----===*/
/*====================------------------------------------====================*/
static void  o___LAYOUT__________o () { return; }

char
yVIKEYS__view_switch     (char *a_name, char *a_opt)
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
   --rce;  if (s_parts [n].type == YVIKEYS_DISABLE) {
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
   if (s_parts [n].abbr == YVIKEYS_TITLE   && s_parts [n].on == '-') {
      a = yVIKEYS__view_abbr (YVIKEYS_VERSION);
      s_parts [a].on = '-';
   }
   if (s_parts [n].abbr == YVIKEYS_COMMAND && s_parts [n].on == '-') {
      a = yVIKEYS__view_abbr (YVIKEYS_KEYS   );
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
   yVIKEYS__view_reset ();
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

char
yVIKEYS__view_gridoff      (int a_x, int a_y, int a_z)
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
yVIKEYS__view_gridsize     (int a_x, int a_y, int a_z)
{
   if (a_x > 0) {
      g_gsizex = a_x;
      if (a_y <= 0) g_gsizey = a_x;
      if (a_z <= 0) g_gsizez = a_x;
   }
   /*> printf ("gridx off %3d size %3d\n", g_goffx, g_gsizex);                        <*/
   return 0;
}

char yVIKEYS_layout_min (void) { yVIKEYS__view_layout ("min"); return 0; }



/*====================------------------------------------====================*/
/*===----                      program level                           ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yVIKEYS_view_init       (cchar *a_title, cchar *a_ver, cint a_wide, cint a_tall, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         n           =    0;
   char        rc          =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(screen parts)-------------------*/
   s_npart = 0;
   for (i = 0; i < MAX_PARTS; ++i) {
      if (s_parts [i].abbr == 0)  break;
      DEBUG_GRAF   yLOG_info    ("name"      , s_parts [i].name);
      if (strchr (OWN_HIDE, s_parts [i].own) != NULL) {
         DEBUG_GRAF   yLOG_note    ("adding view menu command to show/hide");
         yVIKEYS_cmds_add ('v', s_parts [i].name , ""    , "Cs"   , yVIKEYS__view_switch       , "" );
      }
      ++s_npart;
   }
   DEBUG_GRAF   yLOG_value   ("s_npart"   , s_npart);
   yVIKEYS__view_purge ();
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
   yVIKEYS_cmds_add ('v', "gridoff"     , ""    , "iii"  , yVIKEYS__view_gridoff      , "" );
   yVIKEYS_cmds_add ('v', "gridsize"    , ""    , "iii"  , yVIKEYS__view_gridsize     , "" );
   yVIKEYS_cmds_add ('v', "layout"      , ""    , "s"    , yVIKEYS__view_layout       , "" );
   yVIKEYS_cmds_add ('v', "palette"     , ""    , "isss" , yVIKEYS_view_palette       , "" );
   /*---(key data)-----------------------*/
   s_main_wide  = a_wide;
   s_main_tall  = a_tall;
   s_alt_wide   = a_alt;
   DEBUG_GRAF   yLOG_value   ("main_wide" , s_main_wide);
   DEBUG_GRAF   yLOG_value   ("main_tall" , s_main_tall);
   DEBUG_GRAF   yLOG_value   ("alt_wide"  , s_alt_wide);
   /*---(open window)--------------------*/
   rc = yX11_start      (a_title, a_wide, a_tall, YX_FOCUSABLE, YX_FIXED, '-');
   yVIKEYS__view_layout ("work");
   /*> yVIKEYS_view_coords_set (YVIKEYS_MAIN, 0, a_wide, 0, a_tall);                        <*/
   /*---(set text data)------------------*/
   if (a_title != NULL) {
      n = yVIKEYS__view_abbr (YVIKEYS_TITLE);
      strlcpy (s_parts [n].text, a_title, LEN_DESC );
   }
   if (a_ver   != NULL) {
      n = yVIKEYS__view_abbr (YVIKEYS_VERSION);
      strlcpy (s_parts [n].text, a_ver  , LEN_DESC );
   }
   n = yVIKEYS__view_abbr (YVIKEYS_COMMAND);
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
yVIKEYS_view_simple        (cchar a_part, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_GRAF   yLOG_char    ("a_part"    , a_part);
   n = yVIKEYS__view_abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_SIMPLE, s_parts [n].own) == NULL) {
      DEBUG_GRAF   yLOG_note    ("can not use function on this element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(save color)---------------------*/
   DEBUG_GRAF   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_GRAF   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_GRAF   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)              s_parts [n].drawer = a_drawer;
   DEBUG_GRAF   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_moderate      (cchar a_part, cchar a_type, cchar a_anchor, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_GRAF   yLOG_char    ("a_part"    , a_part);
   n = yVIKEYS__view_abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_MODERATE, s_parts [n].own) == NULL) {
      DEBUG_GRAF   yLOG_note    ("can not use function on this element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(perspective)--------------------*/
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   if      (a_type == '-')            ;
   else if (a_type == YVIKEYS_DISABLE) s_parts [n].type = YVIKEYS_DISABLE;
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_GRAF   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_GRAF   yLOG_char    ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_GRAF   yLOG_char    ("anchor"    , s_parts [n].anchor);
   yVIKEYS__view_reanchor (n);
   /*---(save color)---------------------*/
   DEBUG_GRAF   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_GRAF   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_GRAF   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)              s_parts [n].drawer = a_drawer;
   DEBUG_GRAF   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_colors        (cint a_lef, cint a_bot, cint a_top, cint a_rig)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(left)---------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_TITLE   );
   s_parts [n].color = a_lef;
   n = yVIKEYS__view_abbr (YVIKEYS_VERSION );
   s_parts [n].color = a_lef + 5;
   /*---(bottom)-------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_COMMAND );
   s_parts [n].color = a_bot;
   n = yVIKEYS__view_abbr (YVIKEYS_STATUS  );
   s_parts [n].color = a_bot + 5;
   n = yVIKEYS__view_abbr (YVIKEYS_KEYS    );
   s_parts [n].color = a_bot + 5;
   /*---(top)----------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_BUFFER  );
   s_parts [n].color = a_top;
   n = yVIKEYS__view_abbr (YVIKEYS_FORMULA );
   s_parts [n].color = a_top + 5;
   /*---(right)--------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_RIBBON  );
   s_parts [n].color = a_rig;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_setup         (cchar a_part, cchar a_type, cchar a_anchor, cint a_xmin, cint a_xlen, cint a_ymin, cint a_ylen, cint a_zmin, cint a_zlen, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_GRAF   yLOG_char    ("a_part"    , a_part);
   n = yVIKEYS__view_abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_COMPLEX, s_parts [n].own) == NULL) {
      DEBUG_GRAF   yLOG_note    ("can not use function on this element");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(perspective)--------------------*/
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   if      (a_type == '-')            ;
   else if (a_type == YVIKEYS_DISABLE) s_parts [n].type = YVIKEYS_DISABLE;
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_GRAF   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_GRAF   yLOG_char    ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_GRAF   yLOG_char    ("anchor"    , s_parts [n].anchor);
   /*---(save x-coords)------------------*/
   DEBUG_GRAF   yLOG_value   ("a_xmin"    , a_xmin);
   DEBUG_GRAF   yLOG_value   ("a_xlen"    , a_xlen);
   if (a_xlen != 0) {
      s_parts [n].xmin  = a_xmin;
      if      (a_xlen == -1)  s_parts [n].xlen  = a_xmin - s_parts [n].wide;
      else if (a_xlen ==  1)  s_parts [n].xlen  = a_xmin + s_parts [n].wide;
      else                    s_parts [n].xlen  = a_xlen;
   }
   DEBUG_GRAF   yLOG_value   ("xmin"      , s_parts [n].xmin);
   DEBUG_GRAF   yLOG_value   ("xlen"      , s_parts [n].xlen);
   /*---(save y-coords)------------------*/
   DEBUG_GRAF   yLOG_value   ("a_ymin"    , a_ymin);
   DEBUG_GRAF   yLOG_value   ("a_ylen"    , a_ylen);
   if (a_ylen != 0) {
      s_parts [n].ymin  = a_ymin;
      if      (a_ylen == -1)  s_parts [n].ylen  = a_ymin - s_parts [n].tall;
      else if (a_ylen ==  1)  s_parts [n].ylen  = a_ymin + s_parts [n].tall;
      else                    s_parts [n].ylen  = a_ylen;
   }
   DEBUG_GRAF   yLOG_value   ("ymin"      , s_parts [n].ymin);
   DEBUG_GRAF   yLOG_value   ("ylen"      , s_parts [n].ylen);
   /*---(save z-coords)------------------*/
   DEBUG_GRAF   yLOG_value   ("a_zmin"    , a_zmin);
   DEBUG_GRAF   yLOG_value   ("a_zlen"    , a_zlen);
   if (a_ylen != 0) {
      if (a_zmin == 0 && a_zlen == 0) {
         s_parts [n].zmin  = -500;
         s_parts [n].zlen  = 1000;
      } else {
         s_parts [n].zmin  = a_zmin;
         s_parts [n].zlen  = a_zlen;
      }
   }
   DEBUG_GRAF   yLOG_value   ("zmin"      , s_parts [n].zmin);
   DEBUG_GRAF   yLOG_value   ("zlen"      , s_parts [n].zlen);
   /*---(save color)---------------------*/
   DEBUG_GRAF   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_GRAF   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_GRAF   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)              s_parts [n].drawer = a_drawer;
   DEBUG_GRAF   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_CUSTOM;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_palette     (cint a_deg, cchar *a_harm, cchar *a_sat, cchar *a_val)
{
   yCOLOR_init     (YCOLOR_WHEEL );
   yCOLOR_palette  (a_deg, a_harm, a_sat, a_val);
   return 0;
}

char
yVIKEYS_view_wrap       (void)
{
   yX11_end     ();
   return 0;
}

char
yVIKEYS_view_text       (cchar a_part, cchar *a_text)
{
   int         n           =    0;
   n = yVIKEYS__view_abbr (a_part);
   if (n < 0)  return -1;
   strlcpy (s_parts [n].text, a_text, LEN_DESC);
   return 0;
}

char
yVIKEYS_view_size       (cchar a_part, int *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text)
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
yVIKEYS_view_coords        (cchar a_part, int *a_xmin, int *a_xlen, int *a_ymin, int *a_ylen)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yVIKEYS__view_abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      if (a_xmin != NULL)  *a_xmin  = 0;
      if (a_xlen != NULL)  *a_xlen  = 0;
      if (a_ymin != NULL)  *a_ymin  = 0;
      if (a_ylen != NULL)  *a_ylen  = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_note    ("save values for good entry");
   if (a_xmin != NULL)  *a_xmin  = s_parts [n].xmin;
   if (a_xlen != NULL)  *a_xlen  = s_parts [n].xlen;
   if (a_ymin != NULL)  *a_ymin  = s_parts [n].ymin;
   if (a_ylen != NULL)  *a_ylen  = s_parts [n].ylen;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
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

char          /*----: draw the saved status ----------------------------------*/
yVIKEYS_view_cursor      (void)
{
   int         x_lef   = s_colmap.cur / s_mag;
   int         x_bot   = s_rowmap.cur / s_mag;
   int         x_rig   = x_lef + (g_gsizex / s_mag);
   int         x_top   = x_bot + (g_gsizey / s_mag);
   char        n           = 0;
   /*---(defense)------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_CURSOR);
   if (s_parts [n].on == '-')  return 0;
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
   char        n           = 0;
   /*---(defense)------------------------*/
   n = yVIKEYS__view_abbr (YVIKEYS_GRID);
   if (s_parts [n].on == '-')  return 0;
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
      n    = yVIKEYS__view_abbr   (YVIKEYS_COMMAND);
      if (s_parts [n].on == '-')  x_use = 'y';
   }
   if (x_use != 'y')    return 0;
   /*---(draw)---------------------------*/
   yVIKEYS_view_coords  (YVIKEYS_MAIN, &x_lef, &x_len, &x_bot, NULL);
   x_bot += 20;
   x_top = x_bot + 15;
   x_lef = x_lef + 50;
   /*> printf ("n %2d, s_parts[n].left %4d, x_lef = %4d\n", n, s_parts [n].left, x_lef);   <*/
   x_rig = x_lef + x_len - 100;
   /*> printf ("n %2d, s_parts[n].wide %4d, x_rig = %4d\n", n, s_parts [n].wide, x_rig);   <*/
   glPushMatrix    (); {
      yCOLOR_set (s_parts [n].color, 1.0);
      glBegin         (GL_POLYGON); {
         glVertex3f  (x_lef , x_top, 50.0f);
         glVertex3f  (x_rig , x_top, 50.0f);
         glVertex3f  (x_rig , x_bot, 50.0f);
         glVertex3f  (x_lef , x_bot, 50.0f);
      } glEnd   ();
      yVIKEYS_view_color (COLOR_BLACK);
      glLineWidth  ( 2);
      glBegin(GL_LINE_STRIP); {
         glVertex3f  (x_lef , x_top, 50.0f);
         glVertex3f  (x_rig , x_top, 50.0f);
         glVertex3f  (x_rig , x_bot, 50.0f);
         glVertex3f  (x_lef , x_bot, 50.0f);
         glVertex3f  (x_lef , x_top, 50.0f);
      } glEnd   ();
      yVIKEYS_view_color (COLOR_BLACK);
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
   /*---(locals)-----------+-----------+-*/
   int         x_max;
   int         y_max;
   int         z_max;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [a].name);
   /*---(prepare)------------------------*/
   x_max = s_parts [a].xmin + s_parts [a].xlen;
   y_max = s_parts [a].ymin + s_parts [a].ylen;
   z_max = s_parts [a].zmin + s_parts [a].zlen;
   /*---(setup view)---------------------*/
   DEBUG_GRAF   yLOG_value   ("a"         , a);
   DEBUG_GRAF   yLOG_complex (s_parts [a].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [a].bott, s_parts [a].left, s_parts [a].wide, s_parts [a].tall, s_parts [a].on);
   glViewport      (s_parts [a].left, s_parts [a].bott, s_parts [a].wide, s_parts [a].tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   glOrtho         (s_parts [a].xmin, x_max, s_parts [a].ymin, y_max, s_parts [a].zmin, z_max);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   DEBUG_GRAF   yLOG_note    ("draw background");
   glPushMatrix    (); {
      /*> yVIKEYS_view_color (s_parts [a].color);                                     <*/
      yCOLOR_set (s_parts [a].color, 1.0);
      if (s_parts [a].abbr == YVIKEYS_VERSION && yURG_debugmode () == 'y')  yVIKEYS_view_color (COLOR_WARN );
      glBegin         (GL_POLYGON); {
         glVertex3f  (s_parts [a].xmin, y_max           , -100.0f);
         glVertex3f  (x_max           , y_max           , -100.0f);
         glVertex3f  (x_max           , s_parts [a].ymin, -100.0f);
         glVertex3f  (s_parts [a].xmin, s_parts [a].ymin, -100.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(display text)-------------------*/
   DEBUG_GRAF   yLOG_note    ("draw text");
   if (s_parts [a].text != NULL && strlen (s_parts [a].text) > 0) {
      glPushMatrix    (); {
         yVIKEYS_view_color (COLOR_BLACK);
         if (s_parts [a].orient == 'r') {
            /*> glTranslatef (x_max + 1,   5.0f,    0.0f);                            <*/
            glTranslatef (1.0f     ,   5.0f,    0.0f);
            glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
         } else {
            glTranslatef (3.0f            ,   0.0f,    0.0f);
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
      DEBUG_GRAF   yLOG_info    ("part"      , s_parts [n].name);
      DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
      DEBUG_GRAF   yLOG_char    ("on"        , s_parts [n].on);
      if (strchr (OWN_SETUP, s_parts [n].own) == NULL)  continue;
      if (s_parts [n].on   == '-')                      continue;
      if (s_parts [n].type == YVIKEYS_DISABLE)          continue;
      rc = yVIKEYS__view_opengl (n);
      if (s_parts [n].drawer != NULL)  s_parts [n].drawer ();
   }
   /*---(main)---------------------------*/
   yVIKEYS_view_grid   ();
   yVIKEYS_view_cursor ();
   yVIKEYS_view_float  ();
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


