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


char  VIEW__ribbon             (void);
char  VIEW__grid_normal        (void);
char  VIEW__grid_zoom          (void);
char  VIEW__cursor             (void);
/*> char  VIEW__float              (void);                                            <*/
char  VIEW__layer_show         (void);
char  VIEW__layer_set          (char *a_name);



static int    s_orig_wide  = 0;
static int    s_orig_tall  = 0;
static int    s_main_wide  = 0;
static int    s_main_tall  = 0;
static int    s_alt_wide   = 0;
static int    s_full_wide  = 0;
static int    s_full_tall  = 0;


static char   s_empty       [500] = "                                                                                                                                                                                                                                                                                                            ";


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
   /*> char        face;                                                              <*/
   /*> int         font;                                                              <*/
   int         icons;
   /*---(done)--------------*/
};
static tWIN   s_win;


#define      OWN_FULLY        'y'   /* only change color, text, and hiding    */
#define      OWN_PARTLY       'p'   /* change hiding, color, and drawing      */
#define      OWN_LITTLE       's'   /* change anything and everything         */
#define      OWN_MAIN         'm'   /* change anything and everything         */
#define      OWN_UNDERLAY     'u'   /* drawn before main                      */
#define      OWN_OVERLAY      'o'   /* drawn after everything else            */
#define      OWN_DATA         '-'   /* just a data holder                     */

#define      OWN_SETUP        "ypsm"
#define      OWN_HIDE         "ypso"
#define      OWN_REANCHOR     "yp"

#define      OWN_COMPLEX      "ms"
#define      OWN_MODERATE     "msp"
#define      OWN_SIMPLE       "po"

#define      MAX_PARTS         50
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
   int         def_wide;                    /* default width                  */
   int         def_tall;                    /* default height                 */
   cchar       x_tie;                       /* tie to another field in x-axis */
   cchar       y_tie;                       /* tie to another field in y-axis */
   cchar       under;                       /* fall below buffer/formula      */
   int         wide;                        /* screen width                   */
   int         tall;                        /* screen height                  */
   int         left;                        /* screen left                    */
   int         bott;                        /* screen bottom                  */
   /*---(for std elements)-----*/
   char        orient;                      /* orientation of text            */
   char        (*source) (char*);           /* content source                 */
   char        text        [2000    ];      /* optional text                  */
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
   /*---abbr---------   ---name-----   ---own-----  on    bt lr dw dt  x_t  y_t  und  wi ta le bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_TITLE   , "title"       , OWN_FULLY  , 'y',   1, 1, 0, 0, '-', '-', '-',  0, 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "left hand title bar"                                },
   { YVIKEYS_BUFFER  , "buffer"      , OWN_FULLY  , '-',   7, 2, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "buffer inventory at top"                            },
   { YVIKEYS_FORMULA , "formula"     , OWN_FULLY  , '-',   6, 2, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "formula and source editing line at top"             },
   { YVIKEYS_NAV     , "nav"         , OWN_PARTLY , '-',   3, 2, 0, 0, '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "navigation panel to display tags and other links"   },
   { YVIKEYS_ALT     , "alt"         , OWN_LITTLE , '-',   4, 5, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "alternate working area or view"                     },
   { YVIKEYS_PROGRESS, "progress"    , OWN_PARTLY , '-',   3, 3, 0, 0, '/', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "time and sequencing controls about status line"     },
   { YVIKEYS_STATUS  , "status"      , OWN_FULLY  , 'y',   2, 2, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational status bar above command line"        },
   { YVIKEYS_COMMAND , "command"     , OWN_FULLY  , 'y',   1, 2, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "command, search, and help message line at bottom"   },
   { YVIKEYS_DETAILS , "details"     , OWN_PARTLY , '-',   3, 6, 0, 0, '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "display area for critical details to right"         },
   { YVIKEYS_RIBBON  , "ribbon"      , OWN_FULLY  , '-',   3, 7, 0, 0, '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "menu and icon display for navigation of commands"   },
   { YVIKEYS_VERSION , "version"     , OWN_FULLY  , 'y',   8, 1, 0, 0, '-', 't', '-',  0, 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "version display with debugging notice"              },
   { YVIKEYS_KEYS    , "keys"        , OWN_FULLY  , 'y',   1, 8, 0, 0, 'c', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "latest keyboard characters typed"                   },
   { YVIKEYS_GRID    , "grid"        , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "overlay main drawing with a grid"                   },
   { YVIKEYS_CURSOR  , "cursor"      , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "cursor display on screen"                           },
   { YVIKEYS_XAXIS   , "xaxis"       , OWN_PARTLY , '-',   5, 3, 0, 0, '/', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the x-axis labels above the main screen"       },
   { YVIKEYS_YAXIS   , "yaxis"       , OWN_PARTLY , '-',   4, 3, 0, 0, '-', '/', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the y-axis labels left of the main screen"     },
   { YVIKEYS_OVERLAY , "overlay"     , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },
   { YVIKEYS_LAYERS  , "layers"      , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "displays all selected laysers over working"         },
   { YVIKEYS_FLOAT   , "float"       , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },
   { YVIKEYS_HISTORY , "history"     , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "list of command/search history"                     },
   { YVIKEYS_WINDOW  , "window"      , OWN_DATA   , '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "full screen width and height"                       },
   { YVIKEYS_MAIN    , "main"        , OWN_MAIN   , 'y',   4, 4, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "main working area in the middle"                    },
   { 0               , ""            , 0          , '-',   0, 0, 0, 0, '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  ""                                                   },
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
   /*---name------    tbfnmapscdrvkxy  ,    12345678901234567890123456789012345678901234567890  */
   { "min"         , "----m----------" , "smallest footprint, least elements showing"           },
   { "work"        , "t---m--sc--vk--" , "more balanced display of common elements"             },
   { "rob"         , "t---m--sc--v---" , "more balanced display of common elements"             },
   { "max"         , "tbfnm-pscdrvk--" , "everything displays at one time"                      },
   { "ycolor"      , "tbfnmapsc-rvk--" , "more balanced display of common elements"             },
   { "gyges"       , "--f-m--sc----xy" , "prepared for gyges spreadsheet"                       },
   { ""            , "----m----------" , ""                                                     },
};
static int  s_nlayout   = 0;


#define      MAX_OPTION     200
typedef  struct  cOPTION    tOPTION;
struct cOPTION  {
   char        part;
   char        opt         [LEN_LABEL];
   void       *source;
   char        desc        [LEN_DESC ];
};
tOPTION  s_options [MAX_OPTION ] = {
   { YVIKEYS_STATUS  , "mode"         , MODE_status         , "display the mode stack"      },
   { YVIKEYS_STATUS  , "xmap"         , MAP_xstatus         , "x-axis position details"     },
   { YVIKEYS_STATUS  , "ymap"         , MAP_ystatus         , "y-axis position details"     },
   { YVIKEYS_STATUS  , "keys"         , KEYS_status         , "displays keystroke history"  },
   { YVIKEYS_STATUS  , "treg"         , SRC_REG_status      , "displays contents of treg"   },
   { YVIKEYS_STATUS  , "words"        , SOURCE_status_words , "displays word breaks"        },
   { YVIKEYS_STATUS  , "select"       , SOURCE_status_select, "displays selection status"   },
   { YVIKEYS_STATUS  , "sundo"        , SRC_UNDO_status     , "source editing undo stack"   },
   { YVIKEYS_STATUS  , "visual"       , VISU_status         , "visual selection in map"     },
   { YVIKEYS_STATUS  , "file"         , FILE_status         , "file, control, and version"  },
   { YVIKEYS_STATUS  , "regs"         , yvikeys_regs_status , "current register information"},
   { YVIKEYS_STATUS  , "delay"        , yvikeys_delay_status, "main loop timing settings"   },
   { YVIKEYS_STATUS  , "main"         , yvikeys_main_status , "main loop timing results"    },
   { NULL            , ""             , NULL                , ""                            },
};
static int  s_noption  = 0;



#define      MAX_LAYERS    200
typedef struct cLAYER  tLAYER;
struct cLAYER {
   char        real;
   char        name        [LEN_LABEL];
   char        on;
   char        (*drawer) (void);
   char        desc        [LEN_DESC ];
};
tLAYER  s_layers [MAX_LAYERS] = {
   { '-', "show"        , '-', NULL, "show layers on display"                  },
   { '-', "hide"        , '-', NULL, "do not show layers on display"           },
   { '-', "all"         , '-', NULL, "select all all layers for display"       },
   { '-', "none"        , '-', NULL, "unselect all layers for display"         },
};
static int s_nlayer     = 0;


int         g_goffx   =   0;
int         g_gsizex  =   1;
int         g_goffy   =   0;
int         g_gsizey  =   1;
int         g_goffz   =   0;
int         g_gsizez  =   1;

static  float  s_mag;



/*====================------------------------------------====================*/
/*===----                       utility functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

char         /*-> purge all changeable fields --------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__purge              (void)
{
   int         n           =    0;
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("count"     , s_npart);
   for (n = 0; n < s_npart; ++n) {
      s_parts [n].left   = s_parts [n].wide   = 0;
      s_parts [n].bott   = s_parts [n].tall   = 0;
      strlcpy (s_parts [n].text, "", LEN_STR);
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
VIEW__reset              (void)
{
   int         n           =    0;
   DEBUG_GRAF_M yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF_M yLOG_value   ("count"     , s_npart);
   for (n = 0; n < s_npart; ++n) {
      s_parts [n].on   = '-';
      if (s_parts [n].abbr == YVIKEYS_MAIN   )  s_parts [n].on   = 'y';
      if (s_parts [n].abbr == YVIKEYS_FLOAT  )  s_parts [n].on   = 'y';
      if (s_parts [n].abbr == YVIKEYS_HISTORY)  s_parts [n].on   = 'y';
      s_parts [n].left   = s_parts [n].wide   =   0;
      s_parts [n].bott   = s_parts [n].tall   =   0;
      DEBUG_GRAF_M yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   }
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> locate screen parts by name --------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__find              (cchar *a_name)
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
VIEW__abbr              (cchar  a_abbr)
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
VIEW_defaults            (cchar a_env)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   tPARTS     *p           = NULL;
   /*---(header)----------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   DEBUG_PROG   yLOG_char    ("a_env"     , a_env);
   myVIKEYS.env = a_env;
   /*---(drawers)------------------------*/
   for (n = 0; n < s_npart; ++n) {
      s_parts [n].drawer = NULL;
   }
   /*---(setup opengl)-------------------*/
   if (a_env == YVIKEYS_OPENGL) {
      for (n = 0; n < s_npart; ++n) {
         p = &(s_parts [n]);
         switch (s_parts [n].abbr) {
         case YVIKEYS_TITLE   : p->on = 'y';  p->def_wide =  15;  break;
         case YVIKEYS_BUFFER  : p->on = '-';  p->def_tall =  15;  break;
         case YVIKEYS_FORMULA : p->on = '-';  p->def_tall =  15;  p->drawer = SOURCE_formula;  break;
         case YVIKEYS_NAV     : p->on = '-';  p->def_wide = 150;  break;
         case YVIKEYS_ALT     : p->on = '-';                      break;
         case YVIKEYS_PROGRESS: p->on = '-';  p->def_tall = 100;  break;
         case YVIKEYS_STATUS  : p->on = 'y';  p->def_tall =  15;  break;
         case YVIKEYS_COMMAND : p->on = 'y';  p->def_tall =  15;  p->drawer = SOURCE_command;   break;
         case YVIKEYS_DETAILS : p->on = '-';  p->def_wide = 250;  break;
         case YVIKEYS_RIBBON  : p->on = '-';  p->def_wide =  40;  p->drawer = VIEW__ribbon;     break;
         case YVIKEYS_VERSION : p->on = 'y';  p->def_wide =  15;  p->def_tall =  40;            break;
         case YVIKEYS_KEYS    : p->on = 'y';  p->def_wide =  40;  p->def_tall =  15;            break;
         case YVIKEYS_GRID    : p->on = '-';                      break;
         case YVIKEYS_CURSOR  : p->on = '-';                      p->drawer = VIEW__cursor;     break;
         case YVIKEYS_XAXIS   : p->on = '-';  p->def_tall =  15;  break;
         case YVIKEYS_YAXIS   : p->on = '-';  p->def_wide =  30;  break;
         case YVIKEYS_OVERLAY : p->on = '-';                      break;
         case YVIKEYS_LAYERS  : p->on = '-';                      p->drawer = VIEW__layer_show; break;
         case YVIKEYS_FLOAT   : p->on = 'y';  p->def_tall =  15;  p->drawer = SOURCE_float;     break;
         case YVIKEYS_HISTORY : p->on = 'y';  p->drawer = HISTORY_display;  break;
         case YVIKEYS_WINDOW  : p->on = '-';                      break;
         case YVIKEYS_MAIN    : p->on = 'y';                      break;
         }
         DEBUG_PROG   yLOG_complex ("part"      , "%-12.12s, on %c, wide %3d, tall %3d", p->name, p->on, p->def_wide, p->def_tall);
      }
   }
   /*---(setup ncurses)------------------*/
   if (a_env == YVIKEYS_CURSES) {
      for (n = 0; n < s_npart; ++n) {
         p = &(s_parts [n]);
         switch (s_parts [n].abbr) {
         case YVIKEYS_TITLE   : p->on = 'y';  p->def_wide =   1;  break;
         case YVIKEYS_BUFFER  : p->on = '-';  p->def_tall =   1;  break;
         case YVIKEYS_FORMULA : p->on = '-';  p->def_tall =   1;  p->drawer = SOURCE_formula;  break;
         case YVIKEYS_NAV     : p->on = '-';  p->def_wide =  20;  break;
         case YVIKEYS_ALT     : p->on = '-';                      break;
         case YVIKEYS_PROGRESS: p->on = '-';  p->def_tall =  10;  break;
         case YVIKEYS_STATUS  : p->on = 'y';  p->def_tall =   1;  break;
         case YVIKEYS_COMMAND : p->on = 'y';  p->def_tall =   1;  p->drawer = SOURCE_command;  break;
         case YVIKEYS_DETAILS : p->on = '-';  p->def_wide =  20;  break;
         case YVIKEYS_RIBBON  : p->on = '-';  p->def_wide =   5;  break;
         case YVIKEYS_VERSION : p->on = 'y';  p->def_wide =   1;  p->def_tall =   5;  break;
         case YVIKEYS_KEYS    : p->on = 'y';  p->def_wide =   5;  p->def_tall =   1;  break;
         case YVIKEYS_GRID    : p->on = '-';                      break;
         case YVIKEYS_CURSOR  : p->on = '-';                      break;
         case YVIKEYS_XAXIS   : p->on = '-';  p->def_tall =   1;  break;
         case YVIKEYS_YAXIS   : p->on = '-';  p->def_wide =   4;  break;
         case YVIKEYS_OVERLAY : p->on = '-';                      break;
         case YVIKEYS_LAYERS  : p->on = '-';                      break;
         case YVIKEYS_FLOAT   : p->on = 'y';  p->def_tall =   1;  p->drawer = SOURCE_float;     break;
         case YVIKEYS_HISTORY : p->on = 'y';  p->drawer = HISTORY_display;  break;
         case YVIKEYS_WINDOW  : p->on = '-';                      break;
         case YVIKEYS_MAIN    : p->on = 'y';                      break;
         }
         DEBUG_PROG   yLOG_complex ("part"      , "%-12.12s, on %c, wide %3d, tall %3d", p->name, p->on, p->def_wide, p->def_tall);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return n;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__widths_wide        (cint a_wide, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;          /* evaluation level               */
   int         n           =    0;          /* current part                   */
   int         w           =    0;          /* widest at level                */
   int         x_cum       =    0;          /* cumulative width               */
   int         x_yaxis     =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_wide"    , a_wide);
   DEBUG_GRAF   yLOG_value   ("a_alt"     , a_alt);
   /*---(fixed widths)-------------------*/
   for (i = 1; i < 9; ++i) {
      w = 0;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].lefrig != i  )  continue;
         /*---(set width)----------------*/
         if      (s_parts [n].on        != 'y')   s_parts [n].wide = 0;
         else                                     s_parts [n].wide = s_parts [n].def_wide;
         /*---(find greatest)------------*/
         if (s_parts [n].x_tie == '-'  && s_parts [n].wide > w)   w = s_parts [n].wide;
         DEBUG_GRAF   yLOG_complex ("fixed"     , "%c %-12.12s %3d wide (cum = %3d)", s_parts [n].abbr, s_parts [n].name, s_parts [n].wide, x_cum);
         /*---(save)---------------------*/
         if (s_parts [n].abbr == YVIKEYS_YAXIS   )  x_yaxis += s_parts [n].wide;
         /*---(done)---------------------*/
      }
      x_cum += w;
   }
   DEBUG_GRAF   yLOG_value   ("x_cum"     , x_cum);
   /*---(variable widths)----------------*/
   DEBUG_GRAF   yLOG_char    ("s_env"     , myVIKEYS.env);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      s_full_wide = x_cum + a_wide + a_alt;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].on != 'y')  continue;
         /*---(set)----------------------*/
         switch (s_parts [n].abbr) {
         case YVIKEYS_MAIN     : s_parts [n].wide = s_main_wide = a_wide;     break;
         case YVIKEYS_FLOAT    : s_parts [n].wide = a_wide - 40;              break;
         case YVIKEYS_HISTORY  : s_parts [n].wide = a_wide - 60;              break;
         case YVIKEYS_ALT      : s_parts [n].wide = s_alt_wide  = a_alt;      break;
         case YVIKEYS_PROGRESS : s_parts [n].wide = a_wide + a_alt;           break;
         case YVIKEYS_XAXIS    : s_parts [n].wide = a_wide + a_alt + x_yaxis; break;
         default               : continue;                                    break;
         }
         DEBUG_GRAF   yLOG_complex ("opengl"    , "%c %-12.12s %3d wide", s_parts [n].abbr, s_parts [n].name, s_parts [n].wide);
         /*---(done)---------------------*/
      }
   } else {
      s_full_wide = a_wide;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].on != 'y')  continue;
         /*---(set)----------------------*/
         switch (s_parts [n].abbr) {
         case YVIKEYS_MAIN     : s_parts [n].wide = s_main_wide = a_wide - x_cum - a_alt;  break;
         case YVIKEYS_FLOAT    : s_parts [n].wide = a_wide - x_cum - a_alt - 4;            break;
         case YVIKEYS_HISTORY  : s_parts [n].wide = a_wide - x_cum - a_alt - 6;            break;
         case YVIKEYS_ALT      : s_parts [n].wide = s_alt_wide  = a_alt;                   break;
         case YVIKEYS_PROGRESS : s_parts [n].wide = a_wide - x_cum;           break;
         case YVIKEYS_XAXIS    : s_parts [n].wide = a_wide - x_cum + x_yaxis; break;
         default               : continue;                                    break;
         }
         DEBUG_GRAF   yLOG_complex ("curses"    , "%c %-12.12s %3d wide", s_parts [n].abbr, s_parts [n].name, s_parts [n].wide);
         /*---(done)---------------------*/
      }
   }
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_wide);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__widths_left        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;          /* evaluation level               */
   int         n           =    0;          /* current part                   */
   int         a           =    0;          /* alternate part                 */
   int         w           =    0;          /* widest at level                */
   int         x_cum       =    0;          /* cumulative width               */
   char        x_skip      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(set lefts)----------------------*/
   for (i = 1; i < 9; ++i) {
      w = 0;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].lefrig != i  )  continue;
         /*---(set)----------------------*/
         s_parts [n].left = x_cum;
         /*---(set special)--------------*/
         x_skip = '-';
         if (s_parts [n].x_tie  != '-'   ) {
            a = VIEW__abbr (s_parts [n].x_tie);
            if (a >= 0 && s_parts [n].on  == 'y')  {
               s_parts [a].wide  = s_full_wide - s_parts [a].left - s_parts [n].wide;
               s_parts [n].left  = s_full_wide - s_parts [n].wide;
               DEBUG_GRAF   yLOG_complex ("alternate" , "%c %-12.12s %3d left (cum %3d)", s_parts [a].abbr, s_parts [a].name, s_parts [a].left, x_cum);
            }
         }
         if (s_parts [n].x_tie != '-')  x_skip = 'y';
         /*---(greatest at level)--------*/
         if (s_parts [n].wide > w && x_skip == '-')   w = s_parts [n].wide;
         DEBUG_GRAF   yLOG_complex ("left"      , "%c %-12.12s %3d left (cum %3d)", s_parts [n].abbr, s_parts [n].name, s_parts [n].left, x_cum);
         /*---(done)---------------------*/
      }
      x_cum += w;
   }
   /*---(floater)------------------------*/
   n = VIEW__abbr (YVIKEYS_MAIN  );
   x_cum = s_parts [n].left;
   n = VIEW__abbr (YVIKEYS_FLOAT );
   if (myVIKEYS.env == YVIKEYS_OPENGL )  s_parts [n].left = x_cum + 20;
   if (myVIKEYS.env == YVIKEYS_CURSES )  s_parts [n].left = x_cum +  2;
   n = VIEW__abbr (YVIKEYS_HISTORY );
   if (myVIKEYS.env == YVIKEYS_OPENGL )  s_parts [n].left = x_cum + 30;
   if (myVIKEYS.env == YVIKEYS_CURSES )  s_parts [n].left = x_cum +  3;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__widths             (cint a_wide, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;          /* current part                   */
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_wide"    , a_wide);
   /*---(widths)-------------------------*/
   VIEW__widths_wide  (a_wide, a_alt);
   /*---(lefts)--------------------------*/
   VIEW__widths_left  ();
   /*---(fill in widths)-----------------*/
   DEBUG_GRAF   yLOG_note    ("fill in remaining");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on   != 'y')  continue;
      if (s_parts [n].wide !=  0 )  continue;
      s_parts [n].wide = s_full_wide - s_parts [n].left;
      DEBUG_GRAF   yLOG_complex ("fill"      , "%c %-12.12s %3d wide", s_parts [n].abbr, s_parts [n].name, s_parts [n].wide);
   }
   /*---(full)---------------------------*/
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_wide);
   n = VIEW__abbr (YVIKEYS_WINDOW);
   s_parts [n].wide = s_full_wide;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize heights based on layout -----[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__heights_tall       (cint a_tall)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;          /* evaluation level               */
   int         n           =    0;          /* current part                   */
   int         h           =    0;          /* widest at level                */
   int         x_cum       =    0;          /* cumulative width               */
   int         x_prog      =    0;
   int         x_xaxis     =    0;
   char        x_skip      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_tall"    , a_tall);
   /*---(fixed heights)------------------*/
   for (i = 1; i < 9; ++i) {
      h = 0;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].bottop != i  )  continue;
         /*---(set width)----------------*/
         x_skip = '-';
         if      (s_parts [n].on        != 'y')   s_parts [n].tall = 0;
         else                                     s_parts [n].tall = s_parts [n].def_tall;
         /*---(find greatest)------------*/
         if (s_parts [n].y_tie != '-')  x_skip = 'y';
         if (s_parts [n].under != '-')  x_skip = 'y';
         if (x_skip == '-'  && s_parts [n].tall > h)   h = s_parts [n].tall;
         DEBUG_GRAF   yLOG_complex ("fixed"     , "%c %-12.12s %3d tall (cum = %3d)", s_parts [n].abbr, s_parts [n].name, s_parts [n].tall, x_cum);
         /*---(save)---------------------*/
         if (s_parts [n].abbr == YVIKEYS_PROGRESS)  x_prog  = s_parts [n].tall;
         if (s_parts [n].abbr == YVIKEYS_XAXIS   )  x_xaxis = s_parts [n].tall;
         /*---(done)---------------------*/
      }
      x_cum += h;
   }
   DEBUG_GRAF   yLOG_value   ("x_cum"     , x_cum);
   /*---(variable heights)---------------*/
   DEBUG_GRAF   yLOG_char    ("s_env"     , myVIKEYS.env);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      s_full_tall = x_cum + a_tall;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].on != 'y')  continue;
         /*---(set)----------------------*/
         switch (s_parts [n].abbr) {
         case YVIKEYS_MAIN     : s_parts [n].tall = s_main_tall = a_tall;      break;
         case YVIKEYS_ALT      : s_parts [n].tall = a_tall;                    break;
         case YVIKEYS_NAV      :
         case YVIKEYS_DETAILS  :
         case YVIKEYS_RIBBON   : s_parts [n].tall = a_tall + x_prog + x_xaxis; break;
         case YVIKEYS_YAXIS    : s_parts [n].tall = a_tall;                    break;
         default               : continue;                                     break;
         }
         DEBUG_GRAF   yLOG_complex ("opengl"    , "%c %-12.12s %3d tall", s_parts [n].abbr, s_parts [n].name, s_parts [n].tall);
         /*---(done)---------------------*/
      }
   } else {
      s_full_tall = a_tall;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].on != 'y')  continue;
         /*---(set)----------------------*/
         switch (s_parts [n].abbr) {
         case YVIKEYS_MAIN     : s_parts [n].tall = s_main_tall = a_tall - x_cum;  break;
         case YVIKEYS_ALT      : s_parts [n].tall = a_tall - x_cum;                break;
         case YVIKEYS_NAV      :
         case YVIKEYS_DETAILS  :
         case YVIKEYS_RIBBON   : s_parts [n].tall = a_tall - x_cum + x_prog;       break;
         case YVIKEYS_YAXIS    : s_parts [n].tall = a_tall - x_cum;            break;
         default               : continue;                                         break;
         }
         DEBUG_GRAF   yLOG_complex ("curses"    , "%c %-12.12s %3d tall", s_parts [n].abbr, s_parts [n].name, s_parts [n].tall);
         /*---(done)---------------------*/
      }
   }
   DEBUG_GRAF   yLOG_value   ("full_tall" , s_full_tall);
   /*---(floater)------------------------*/
   n = VIEW__abbr (YVIKEYS_MAIN  );
   x_cum = s_parts [n].tall;
   n = VIEW__abbr (YVIKEYS_FLOAT );
   s_parts [n].tall = s_parts [n].def_tall;
   n = VIEW__abbr (YVIKEYS_HISTORY );
   if (myVIKEYS.env == YVIKEYS_OPENGL )  s_parts [n].tall = (int) ((x_cum - 60) / 15) * 15;
   if (myVIKEYS.env == YVIKEYS_CURSES )  s_parts [n].tall = x_cum -  4;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize heights based on layout -----[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__heights_bott       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;          /* evaluation level               */
   int         n           =    0;          /* current part                   */
   int         a           =    0;          /* alternate part                 */
   int         h           =    0;          /* widest at level                */
   int         x_cum       =    0;          /* cumulative width               */
   char        x_skip      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(set bottoms)--------------------*/
   for (i = 1; i < 9; ++i) {
      h = 0;
      for (n = 0; n < s_npart; ++n) {
         /*---(filter)-------------------*/
         if (s_parts [n].bottop != i  )  continue;
         /*---(set)----------------------*/
         s_parts [n].bott = x_cum;
         /*---(set special)--------------*/
         x_skip = '-';
         if (s_parts [n].y_tie  != '-'   ) {
            a = VIEW__abbr (s_parts [n].y_tie);
            if (a >= 0 && s_parts [n].on  == 'y')  {
               s_parts [a].tall  = s_full_tall - s_parts [a].bott - s_parts [n].tall;
               s_parts [n].bott  = s_full_tall - s_parts [n].tall;
               DEBUG_GRAF   yLOG_complex ("alternate" , "%c %-12.12s %3d bott (cum %3d)", s_parts [a].abbr, s_parts [a].name, s_parts [a].bott, x_cum);
            }
         }
         if (s_parts [n].y_tie != '-')  x_skip = 'y';
         if (s_parts [n].under != '-')  x_skip = 'y';
         /*---(greatest at level)--------*/
         if (s_parts [n].tall > h && x_skip == '-')   h = s_parts [n].tall;
         DEBUG_GRAF   yLOG_complex ("bott"      , "%c %-12.12s %3d bott (cum %3d)", s_parts [n].abbr, s_parts [n].name, s_parts [n].bott, x_cum);
         /*---(done)---------------------*/
      }
      x_cum += h;
   }
   /*---(floater)------------------------*/
   n = VIEW__abbr (YVIKEYS_MAIN  );
   x_cum = s_parts [n].bott;
   n = VIEW__abbr (YVIKEYS_FLOAT );
   if (myVIKEYS.env == YVIKEYS_OPENGL )  s_parts [n].bott = x_cum + 60;
   if (myVIKEYS.env == YVIKEYS_CURSES )  s_parts [n].bott = x_cum +  6;
   n = VIEW__abbr (YVIKEYS_HISTORY );
   if (myVIKEYS.env == YVIKEYS_OPENGL )  s_parts [n].bott = x_cum + 30;
   if (myVIKEYS.env == YVIKEYS_CURSES )  s_parts [n].bott = x_cum +  2;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> flip bottoms for ncurses -----------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__heights_flip       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;          /* current part                   */
   int         x_tall      =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      DEBUG_GRAF   yLOG_note    ("not required for opengl");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(display)------------------------*/
   for (n = 0; n < s_npart; ++n) {
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
      s_parts [n].bott = s_full_tall - s_parts [n].bott - 1;
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
      if (s_parts [n].bott < 0)  s_parts [n].bott = 0;
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
VIEW__heights            (cint a_tall)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;          /* current part                   */
   int         x_under     =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_tall"    , a_tall);
   /*---(heights)------------------------*/
   VIEW__heights_tall (a_tall);
   /*---(bottoms)------------------------*/
   VIEW__heights_bott ();
   /*---(fill in widths)-----------------*/
   DEBUG_GRAF   yLOG_note    ("fill in remaining");
   for (n = 0; n < s_npart; ++n) {
      if (s_parts [n].on   != 'y')  continue;
      if (s_parts [n].tall !=  0 )  continue;
      s_parts [n].tall = s_full_tall - s_parts [n].bott;
      DEBUG_GRAF   yLOG_complex ("fill"      , "%c %-12.12s %3d tall", s_parts [n].abbr, s_parts [n].name, s_parts [n].tall);
   }
   /*---(full)---------------------------*/
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_tall);
   n = VIEW__abbr (YVIKEYS_WINDOW);
   s_parts [n].tall = s_full_tall;
   s_parts [n].bott = 0;
   /*---(flip ncurses)-------------------*/
   VIEW__heights_flip  ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__reanchor           (cchar a)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_len       =    0;
   int         y_len       =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(handle auto first)--------------*/
   if (s_parts [a].mgmt != YVIKEYS_AUTO) {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
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
VIEW__resize             (cchar a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   /*---(size window)--------------------*/
   if (a_type == 'r' && myVIKEYS.env == YVIKEYS_CURSES) {
      getmaxyx (stdscr, s_orig_tall, s_orig_wide);
   }
   /*---(widths)-------------------------*/
   DEBUG_GRAF   yLOG_value   ("orig_wide" , s_orig_wide);
   VIEW__widths   (s_orig_wide, s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("main_wide" , s_main_wide);
   DEBUG_GRAF   yLOG_value   ("alt_wide"  , s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_wide);
   /*---(heights)------------------------*/
   DEBUG_GRAF   yLOG_value   ("orig_tall" , s_orig_tall);
   VIEW__heights  (s_orig_tall);
   DEBUG_GRAF   yLOG_value   ("main_tall" , s_main_tall);
   DEBUG_GRAF   yLOG_value   ("full_tall" , s_full_tall);
   /*---(display)------------------------*/
   for (n = 0; n < s_npart; ++n) {
      VIEW__reanchor        (n);
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
      DEBUG_GRAF   yLOG_complex (s_parts [n].name, "xmin %4d, xlen %4d, ymin %4d, ylen %4d, zmin %4d, zlen %4d", s_parts [n].xmin, s_parts [n].xlen, s_parts [n].ymin, s_parts [n].ylen, s_parts [n].zmin, s_parts [n].zlen);
   }
   /*---(size window)--------------------*/
   if (a_type == 'r' && myVIKEYS.env == YVIKEYS_OPENGL) {
      yX11_resize (s_full_wide, s_full_tall);
   }
   myVIKEYS.redraw = 'y';
   /*---(update map)---------------------*/
   n = VIEW__abbr (YVIKEYS_MAIN);
   g_xmap.avail = s_parts [n].wide;
   g_ymap.avail = s_parts [n].tall;
   yVIKEYS_map_refresh ();
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
   if (a_wide > 10)   s_orig_wide = a_wide;
   if (a_tall > 10)   s_orig_tall = a_tall;
   if (a_alt  > 10)   s_alt_wide  = a_alt;
   VIEW__resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      window layout                           ----===*/
/*====================------------------------------------====================*/
static void  o___LAYOUT__________o () { return; }

char
yVIKEYS_view_option      (char a_part, char *a_opt, void *a_source, char *a_desc)
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
   n = VIEW__abbr (a_part);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_GRAF   yLOG_char    ("abbr"      , s_parts [n].abbr);
   --rce;  if (a_opt == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_opt"     , a_opt);
   --rce;  if (a_source == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_source"  , a_source);
   /*---(add)----------------------------*/
   s_options [s_noption].part   = s_parts [n].abbr;
   strlcpy (s_options [s_noption].opt, a_opt, LEN_LABEL);
   s_options [s_noption].source = a_source;
   if (a_desc != NULL)  strlcpy (s_options [s_noption].desc, a_desc, LEN_LABEL);
   ++s_noption;
   DEBUG_GRAF   yLOG_value   ("s_noption" , s_noption);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__switch             (char *a_name, char *a_opt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_on        =  '-';
   char        i           =    0;
   char        n           =   -1;
   char        a           =   -1;
   char        x_good      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   n = VIEW__find (a_name);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_parts [n].type == YVIKEYS_DISABLE) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_opt == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_opt"     , a_opt);
   /*---(set the flag)-------------------*/
   DEBUG_GRAF   yLOG_char    ("current"   , s_parts [n].on);
   x_on = s_parts [n].on;
   if      (strcmp (a_opt, "hide") == 0) {
      s_parts [n].on = '-';
      x_good = 'y';
   } else if (strcmp (a_opt, "show") == 0) {
      s_parts [n].on = 'y';
      x_good = 'y';
   } else {
      for (i = 0; i < s_noption; ++i) {
         if (s_options [i].part == NULL)                   break;
         if (s_options [i].part != s_parts [n].abbr)       continue;
         if (s_options [i].opt  [0] != a_opt  [0])         continue;
         if (strcmp (s_options [i].opt , a_opt ) != NULL)  continue;
         s_parts [n].on     = 'y';
         s_parts [n].source = s_options [i].source;
         x_good = 'y';
         break;
      }
   }
   DEBUG_GRAF   yLOG_char    ("x_good"    , x_good);
   --rce;  if (x_good != 'y') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("new"       , s_parts [n].on);
   DEBUG_GRAF   yLOG_point   ("source"    , s_parts [n].source);
   /*---(handle linkages)----------------*/
   if (s_parts [n].abbr == YVIKEYS_TITLE   && s_parts [n].on == '-') {
      a = VIEW__abbr (YVIKEYS_VERSION);
      s_parts [a].on = '-';
   }
   if (s_parts [n].abbr == YVIKEYS_COMMAND && s_parts [n].on == '-') {
      a = VIEW__abbr (YVIKEYS_KEYS   );
      s_parts [a].on = '-';
   }
   /*---(resize)-------------------------*/
   if (x_on != s_parts [n].on) {
      DEBUG_GRAF   yLOG_note    ("must resize");
      VIEW__resize ('r');
      myVIKEYS.redraw = 'y';
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__layout             (char *a_name)
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
   VIEW__reset  ();
   /*---(set parts)----------------------*/
   i = 0;
   while (s_layouts [n].parts [i] != 0) {
      if (s_layouts [n].parts [i] != '-') {
         p = VIEW__abbr (s_layouts [n].parts [i]);
         if (p >= 0) {
            DEBUG_GRAF   yLOG_info    ("turn on"   , s_parts [p].name);
            s_parts [p].on = 'y';
         }
      }
      ++i;
   }
   /*---(reset)--------------------------*/
   VIEW__resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__grid_offset          (int a_x, int a_y, int a_z)
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
VIEW__grid_size            (int a_x, int a_y, int a_z)
{
   if (a_x > 0) {
      g_gsizex = a_x;
      if (a_y <= 0) g_gsizey = a_x;
      if (a_z <= 0) g_gsizez = a_x;
   }
   /*> printf ("gridx off %3d size %3d\n", g_goffx, g_gsizex);                        <*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      program level                           ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
VIEW__init_opengl       (char *a_title)
{
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(start window)----------------*/
   yX11_start (a_title, s_orig_wide, s_orig_tall, YX_FOCUSABLE, YX_FIXED, '-');
   /*---(color)--------------------------*/
   DEBUG_GRAF   yLOG_note    ("clearing");
   /*> glClearColor    (1.0f, 1.0f, 1.0f, 1.0f);                                      <*/
   glClearColor    (0.3f, 0.3f, 0.3f, 1.0f);
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
   glFlush         ();
   /*---(color options)------------------*/
   yCOLOR_init     (YCOLOR_WHEEL );
   yVIKEYS_cmds_add ('v', "palette"     , ""    , "isss" , yCOLOR_palette             , "" );
   yVIKEYS_cmds_add ('v', "wheel"       , ""    , "s"    , yCOLOR_wheel               , "" );
   yVIKEYS_cmds_add ('v', "degree"      , "deg" , "i"    , yCOLOR_deg                 , "" );
   yVIKEYS_cmds_add ('v', "harmony"     , "har" , "s"    , yCOLOR_harm                , "" );
   yVIKEYS_cmds_add ('v', "value"       , "val" , "s"    , yCOLOR_val                 , "" );
   yVIKEYS_cmds_add ('v', "saturation"  , "sat" , "s"    , yCOLOR_sat                 , "" );
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__init_curses       (void)
{
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(setup ncurses)---------------*/
   initscr  ();     /* fire up ncurses with a default screen (stdscr)         */
   raw      ();     /* read key-by-key rather than waiting for \n (raw mode)  */
   noecho   ();     /* don't automatically echo keypresses to the screen      */
   ESCDELAY = 0;    /* so escape responds immediately                         */
   nodelay  (stdscr, TRUE);
   getmaxyx (stdscr, s_orig_wide, s_orig_tall);
   clear    ();
   touchwin (stdscr);
   start_color ();
   use_default_colors();
   yCOLOR_curs_init  ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_disable    (cchar a_part)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = VIEW__abbr (a_part);
   DEBUG_PROG   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_PROG   yLOG_note    ("deal with a missing element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_PROG   yLOG_info    ("name"      , s_parts [n].name);
   /*---(perspective)--------------------*/
   s_parts [n].type = YVIKEYS_DISABLE;
   DEBUG_PROG   yLOG_char    ("type"      , s_parts [n].type);
   s_parts [n].on   = YVIKEYS_DISABLE;
   DEBUG_PROG   yLOG_char    ("on"        , s_parts [n].on);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_font       (cchar a_fixed)
{
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   myVIKEYS.font       = a_fixed;
   myVIKEYS.font_scale = yFONT_width (myVIKEYS.font, myVIKEYS.point);
   DEBUG_GRAF   yLOG_double  ("font_scale", myVIKEYS.font_scale);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_config     (cchar *a_title, cchar *a_ver, cchar a_env, cint a_wide, cint a_tall, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         n           =    0;
   char        rc          =    0;
   /*---(header)----------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (FMOD_VIEW)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(screen parts)-------------------*/
   s_npart = 0;
   for (i = 0; i < MAX_PARTS; ++i) {
      if (s_parts [i].abbr == 0)  break;
      DEBUG_PROG   yLOG_info    ("name"      , s_parts [i].name);
      ++s_npart;
   }
   DEBUG_PROG   yLOG_value   ("s_npart"   , s_npart);
   VIEW__purge ();
   /*---(set defaults)-------------------*/
   VIEW_defaults (a_env);
   /*---(screen commands)----------------*/
   for (i = 0; i < s_npart; ++i) {
      DEBUG_PROG   yLOG_info    ("name"      , s_parts [i].name);
      if (s_parts [i].abbr == YVIKEYS_LAYERS) ;
      else if (strchr (OWN_HIDE, s_parts [i].own) != NULL) {
         DEBUG_PROG   yLOG_note    ("adding view menu command to show/hide");
         yVIKEYS_cmds_add ('v', s_parts [i].name , ""    , "Cs"   , VIEW__switch               , "" );
      }
   }
   /*---(screen layouts)-----------------*/
   s_nlayout = 0;
   for (i = 0; i < MAX_LAYOUT; ++i) {
      if (s_layouts [i].name [0] == 0)  break;
      ++s_nlayout;
   }
   /*---(part options)-------------------*/
   s_noption = 0;
   for (i = 0; i < MAX_OPTION; ++i) {
      if (s_options [i].part == 0)  break;
      ++s_noption;
   }
   /*---(ribbon)-------------------------*/
   for (i = 0; i < LEN_LABEL; ++i)  s_win.r_icons [i] = -1;
   s_win.r_nicon = 0;
   /*---(commands)-----------------------*/
   yVIKEYS_cmds_add ('v', "gridoff"     , ""    , "iii"  , VIEW__grid_offset          , "" );
   yVIKEYS_cmds_add ('v', "gridsize"    , ""    , "iii"  , VIEW__grid_size            , "" );
   yVIKEYS_cmds_add ('v', "layout"      , ""    , "s"    , VIEW__layout               , "" );
   yVIKEYS_cmds_add ('v', "layer"       , ""    , "s"    , VIEW__layer_set            , "" );
   /*---(options)------------------------*/
   yVIKEYS_view_option (YVIKEYS_GRID, "norm"  , VIEW__grid_normal, "traditional cross-hatch grid");
   yVIKEYS_view_option (YVIKEYS_GRID, "zoom"  , VIEW__grid_zoom  , "zoom/targeting grid"         );
   /*---(key data)-----------------------*/
   s_orig_wide  = a_wide;
   s_orig_tall  = a_tall;
   s_alt_wide   = a_alt;
   DEBUG_PROG   yLOG_value   ("orig_wide" , s_orig_wide);
   DEBUG_PROG   yLOG_value   ("orig_tall" , s_orig_tall);
   DEBUG_PROG   yLOG_value   ("alt_wide"  , s_alt_wide);
   /*---(open window)--------------------*/
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :
      VIEW__init_opengl (a_title);
      break;
   case YVIKEYS_CURSES :  
      VIEW__init_curses ();
      break;
   }
   myVIKEYS.font       =  0;
   myVIKEYS.point      =  9;
   myVIKEYS.font_scale = 0.0;
   VIEW__layout ("work");
   /*---(set text data)------------------*/
   if (a_title != NULL) {
      n = VIEW__abbr (YVIKEYS_TITLE);
      strlcpy (s_parts [n].text, a_title, LEN_STR  );
   }
   if (a_ver   != NULL) {
      n = VIEW__abbr (YVIKEYS_VERSION);
      strlcpy (s_parts [n].text, a_ver  , LEN_STR  );
   }
   n = VIEW__abbr (YVIKEYS_COMMAND);
   MODE_message ();
   /*---(update status)------------------*/
   STATUS_init_set   (FMOD_VIEW);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_simple        (cchar a_part, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = VIEW__abbr (a_part);
   DEBUG_PROG   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_PROG   yLOG_note    ("deal with a missing element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_PROG   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_PROG   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_SIMPLE, s_parts [n].own) == NULL) {
      DEBUG_PROG   yLOG_note    ("can not use function on this element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(save color)---------------------*/
   DEBUG_PROG   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_PROG   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_PROG   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)     s_parts [n].drawer = a_drawer;
   DEBUG_PROG   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_moderate      (cchar a_part, cchar a_type, cchar a_anchor, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = VIEW__abbr (a_part);
   DEBUG_PROG   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_PROG   yLOG_note    ("deal with a missing element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_PROG   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_PROG   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_MODERATE, s_parts [n].own) == NULL) {
      DEBUG_PROG   yLOG_note    ("can not use function on this element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(perspective)--------------------*/
   DEBUG_PROG   yLOG_char    ("a_type"    , a_type);
   if      (a_type == '-')            ;
   else if (a_type == YVIKEYS_DISABLE) s_parts [n].type = YVIKEYS_DISABLE;
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_PROG   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_PROG   yLOG_char    ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_PROG   yLOG_char    ("anchor"    , s_parts [n].anchor);
   VIEW__reanchor        (n);
   /*---(save color)---------------------*/
   DEBUG_PROG   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_PROG   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_PROG   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)     s_parts [n].drawer = a_drawer;
   DEBUG_PROG   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_defsize       (cchar a_part, cint a_wide, cint a_tall)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = VIEW__abbr (a_part);
   DEBUG_PROG   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_PROG   yLOG_note    ("deal with a missing element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_PROG   yLOG_info    ("name"      , s_parts [n].name);
   /*---(size)---------------------------*/
   if (a_wide > 0)  s_parts [n].def_wide = a_wide;
   if (a_tall > 0)  s_parts [n].def_tall = a_tall;
   VIEW__resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_color         (cint a_color, cfloat a_alpha)
{
   yCOLOR_set (a_color, a_alpha);
   return 0;
}

char
yVIKEYS_view_color_adj     (cint a_base, cint a_adj, cfloat a_alpha)
{
   int         x_base      =    0;
   x_base = a_base / 10;
   yCOLOR_set ((x_base * 10) + a_adj, a_alpha);
   return 0;
}

char
yVIKEYS_view_color_clear   (cint a_color)
{
   yCOLOR_set_clear (a_color);
   return 0;
}

char
yVIKEYS_view_colors        (cint a_lef, cint a_bot, cint a_top, cint a_rig)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(left)---------------------------*/
   n = VIEW__abbr (YVIKEYS_TITLE   );
   s_parts [n].color = a_lef;
   n = VIEW__abbr (YVIKEYS_VERSION );
   s_parts [n].color = a_lef + 5;
   /*---(bottom)-------------------------*/
   n = VIEW__abbr (YVIKEYS_COMMAND );
   s_parts [n].color = a_bot;
   n = VIEW__abbr (YVIKEYS_STATUS  );
   s_parts [n].color = a_bot + 5;
   n = VIEW__abbr (YVIKEYS_KEYS    );
   s_parts [n].color = a_bot + 5;
   n = VIEW__abbr (YVIKEYS_XAXIS   );
   s_parts [n].color = a_bot + 5;
   n = VIEW__abbr (YVIKEYS_YAXIS   );
   s_parts [n].color = a_bot + 5;
   /*---(top)----------------------------*/
   n = VIEW__abbr (YVIKEYS_BUFFER  );
   s_parts [n].color = a_top;
   n = VIEW__abbr (YVIKEYS_FORMULA );
   s_parts [n].color = a_top + 5;
   /*---(right)--------------------------*/
   n = VIEW__abbr (YVIKEYS_RIBBON  );
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
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = VIEW__abbr (a_part);
   DEBUG_PROG   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_PROG   yLOG_note    ("deal with a missing element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_PROG   yLOG_info    ("name"      , s_parts [n].name);
   DEBUG_PROG   yLOG_char    ("own"       , s_parts [n].own);
   if (strchr (OWN_COMPLEX, s_parts [n].own) == NULL) {
      DEBUG_PROG   yLOG_note    ("can not use function on this element");
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return n;
   }
   /*---(perspective)--------------------*/
   DEBUG_PROG   yLOG_char    ("a_type"    , a_type);
   if      (a_type == '-')            ;
   else if (a_type == YVIKEYS_DISABLE) s_parts [n].type = YVIKEYS_DISABLE;
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_PROG   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_PROG   yLOG_char    ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_PROG   yLOG_char    ("anchor"    , s_parts [n].anchor);
   /*---(save x-coords)------------------*/
   DEBUG_PROG   yLOG_value   ("a_xmin"    , a_xmin);
   DEBUG_PROG   yLOG_value   ("a_xlen"    , a_xlen);
   if (a_xlen != 0) {
      s_parts [n].xmin  = a_xmin;
      if      (a_xlen == -1)  s_parts [n].xlen  = a_xmin - s_parts [n].wide;
      else if (a_xlen ==  1)  s_parts [n].xlen  = a_xmin + s_parts [n].wide;
      else                    s_parts [n].xlen  = a_xlen;
   }
   DEBUG_PROG   yLOG_value   ("xmin"      , s_parts [n].xmin);
   DEBUG_PROG   yLOG_value   ("xlen"      , s_parts [n].xlen);
   /*---(save y-coords)------------------*/
   DEBUG_PROG   yLOG_value   ("a_ymin"    , a_ymin);
   DEBUG_PROG   yLOG_value   ("a_ylen"    , a_ylen);
   if (a_ylen != 0) {
      s_parts [n].ymin  = a_ymin;
      if      (a_ylen == -1)  s_parts [n].ylen  = a_ymin - s_parts [n].tall;
      else if (a_ylen ==  1)  s_parts [n].ylen  = a_ymin + s_parts [n].tall;
      else                    s_parts [n].ylen  = a_ylen;
   }
   DEBUG_PROG   yLOG_value   ("ymin"      , s_parts [n].ymin);
   DEBUG_PROG   yLOG_value   ("ylen"      , s_parts [n].ylen);
   /*---(save z-coords)------------------*/
   DEBUG_PROG   yLOG_value   ("a_zmin"    , a_zmin);
   DEBUG_PROG   yLOG_value   ("a_zlen"    , a_zlen);
   if (a_ylen != 0) {
      if (a_zmin == 0 && a_zlen == 0) {
         s_parts [n].zmin  = -500;
         s_parts [n].zlen  = 1000;
      } else {
         s_parts [n].zmin  = a_zmin;
         s_parts [n].zlen  = a_zlen;
      }
   }
   DEBUG_PROG   yLOG_value   ("zmin"      , s_parts [n].zmin);
   DEBUG_PROG   yLOG_value   ("zlen"      , s_parts [n].zlen);
   /*---(save color)---------------------*/
   DEBUG_PROG   yLOG_value   ("a_color"   , a_color);
   if (a_color >= 0 && a_color < 50)  s_parts [n].color  = a_color;
   else                               s_parts [n].color  = 0;
   DEBUG_PROG   yLOG_value   ("color"     , s_parts [n].color);
   /*---(save drawer)--------------------*/
   DEBUG_PROG   yLOG_point   ("a_drawer"  , a_drawer);
   if (a_drawer != NULL)              s_parts [n].drawer = a_drawer;
   DEBUG_PROG   yLOG_point   ("drawer"    , s_parts [n].drawer);
   /*---(finish up)----------------------*/
   s_parts [n].mgmt   = YVIKEYS_CUSTOM;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW_wrap               (void)
{
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :  yX11_end  ();  break;
   case YVIKEYS_CURSES :  endwin    ();  break;
   }
   return 0;
}

char
yVIKEYS_view_text       (cchar a_part, cchar *a_text)
{
   int         n           =    0;
   n = VIEW__abbr (a_part);
   if (n < 0)  return -1;
   strlcpy (s_parts [n].text, a_text, LEN_STR);
   return 0;
}

char
yVIKEYS_view_size       (cchar a_part, int *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = VIEW__abbr (a_part);
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
   if (a_text != NULL)  strlcpy (a_text, s_parts [n].text, LEN_STR);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return s_parts [n].on;
}

char
yVIKEYS_view_bounds        (cchar a_part, int *a_xmin, int *a_xmax, int *a_ymin, int *a_ymax)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = VIEW__abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      if (a_xmin != NULL)  *a_xmin  = 0;
      if (a_xmax != NULL)  *a_xmax  = 0;
      if (a_ymin != NULL)  *a_ymin  = 0;
      if (a_ymax != NULL)  *a_ymax  = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_note    ("save values for good entry");
   if (a_xmin != NULL)  *a_xmin  = s_parts [n].xmin;
   if (a_xmax != NULL)  *a_xmax  = s_parts [n].xmin + s_parts [n].xlen;
   if (a_ymin != NULL)  *a_ymin  = s_parts [n].ymin;
   if (a_ymax != NULL)  *a_ymax  = s_parts [n].ymin + s_parts [n].ylen;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_coords        (cchar a_part, int *a_xmin, int *a_xlen, int *a_ymin, int *a_ylen)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = VIEW__abbr (a_part);
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



/*====================------------------------------------====================*/
/*===----                        standard drawing                      ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

char          /*----: draw the saved status ----------------------------------*/
VIEW__cursor             (void)
{
   int         x_lef   = g_xmap.cur / s_mag;
   int         x_bot   = g_ymap.cur / s_mag;
   int         x_rig   = x_lef + (g_gsizex / s_mag);
   int         x_top   = x_bot + (g_gsizey / s_mag);
   char        n           = 0;
   /*---(defense)------------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   n = VIEW__abbr (YVIKEYS_CURSOR);
   if (s_parts [n].on == '-')  return 0;
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
VIEW__grid_zoom          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           = 0;
   int         x           = 0;
   int         x_lef       = 0;
   int         x_wide      = 0;
   int         y           = 0;
   int         y_bot       = 0;
   int         y_tall      = 0;
   /*---(defense)------------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   n = VIEW__abbr (YVIKEYS_GRID);
   if (s_parts [n].on == '-')  return 0;
   /*---(prepare)------------------------*/
   yVIKEYS_view_coords  (YVIKEYS_MAIN, &x_lef, &x_wide, &y_bot, &y_tall);
   /*---(x grid)-------------------------*/
   glPushMatrix    (); {
      glColor4f     (0.0, 0.3, 0.0, 0.2);
      glLineWidth   (1.5);
      glTranslatef  (g_goffx, g_goffy, 50.0);
      for (y = y_bot; y <= y_bot + y_tall; y += y_tall / 3) {
         glBegin         (GL_LINES); {
            glVertex3f  (x_lef         , y,  0.0f);
            glVertex3f  (x_lef + x_wide, y,  0.0f);
         } glEnd   ();
         for (x = x_lef; x <= x_lef + x_wide; x += x_wide / 3) {
            if (y == y_bot) {
               glBegin         (GL_LINES); {
                  glVertex3f  (x  , y_bot         ,  0.0f);
                  glVertex3f  (x  , y_bot + y_tall,  0.0f);
               } glEnd   ();
            }
         }
      }
      glColor4f     (0.3, 0.0, 0.0, 0.2);
      glBegin         (GL_LINES); {
         glVertex3f  (-x_wide / 6,  y_tall / 6,  0.0f);
         glVertex3f  ( x_wide / 6,  y_tall / 6,  0.0f);
         glVertex3f  (-x_wide / 6, -y_tall / 6,  0.0f);
         glVertex3f  ( x_wide / 6, -y_tall / 6,  0.0f);
         glVertex3f  (-x_wide / 6,  y_tall / 6,  0.0f);
         glVertex3f  (-x_wide / 6, -y_tall / 6,  0.0f);
         glVertex3f  ( x_wide / 6,  y_tall / 6,  0.0f);
         glVertex3f  ( x_wide / 6, -y_tall / 6,  0.0f);
      } glEnd   ();

   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
VIEW__grid_normal        (void)
{
   char        rce         =  -10;
   char        n           = 0;
   int         x           = 0;
   int         x_lef       = 0;
   int         x_wide      = 0;
   int         x_inc       = 1;
   int         x_cnt       = 1;
   int         y           = 0;
   int         y_bot       = 0;
   int         y_tall      = 0;
   int         y_inc       = 1;
   int         y_cnt       = 0;
   /*---(defense)------------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   n = VIEW__abbr (YVIKEYS_GRID);
   if (s_parts [n].on == '-')  return 0;
   /*---(prepare)------------------------*/
   yVIKEYS_view_coords  (YVIKEYS_MAIN, &x_lef, &x_wide, &y_bot, &y_tall);
   --rce;  if (g_gsizex < 1)  return rce;
   --rce;  if (g_gsizey < 1)  return rce;
   --rce;  if (x_wide / g_gsizex > 50)  return rce;
   --rce;  if (y_tall / g_gsizey > 50)  return rce;
   x_inc = g_gsizex;
   y_inc = g_gsizey;
   /*---(x grid)-------------------------*/
   glPushMatrix    (); {
      glColor4f     (0.0, 0.3, 0.0, 0.2);
      glLineWidth   (1.5);
      glTranslatef  (g_goffx, g_goffy, 50.0);
      y_cnt = 0;
      for (y = y_bot; y <= y_bot + y_tall; y += y_inc) {
         /*---(hozt lines)---------------*/
         if ((y_cnt % 5) == 0) {
            glBegin         (GL_LINES); {
               glVertex3f  (x_lef         , y,  0.0f);
               glVertex3f  (x_lef + x_wide, y,  0.0f);
            } glEnd   ();
         }
         ++y_cnt;
         x_cnt = 0;
         for (x = x_lef; x <= x_lef + x_wide; x += x_inc) {
            /*---(vert lines)------------*/
            if (y == y_bot && (x_cnt % 5) ==  0) {
               glBegin         (GL_LINES); {
                  glVertex3f  (x  , y_bot         ,  0.0f);
                  glVertex3f  (x  , y_bot + y_tall,  0.0f);
               } glEnd   ();
            }
            ++x_cnt;
            /*---(crosses)---------------*/
            glBegin         (GL_LINES); {
               glVertex3f  ((x - 5), y      ,  0.0f);
               glVertex3f  ((x + 5), y      ,  0.0f);
               glVertex3f  (x      , (y - 5),  0.0f);
               glVertex3f  (x      , (y + 5),  0.0f);
            } glEnd   ();
         }
      }
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_view_ribbon      (char *a_cat, char *a_name)
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
VIEW__ribbon             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_side      =   35;
   int         n           =     0;
   /*---(draw icons)---------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   n    = VIEW__abbr   (YVIKEYS_RIBBON);
   if (s_parts [n].on == '-')  return 0;
   glPushMatrix    (); {
      yVIKEYS_view_color_adj (s_parts [n].color, YCOLOR_MIN, 1.0);
      glTranslatef  (2, - 3, 10.0);
      for (i = 0; i < s_win.r_nicon; ++i) {
         yFONT_iconno (s_win.r_icons [i], x_side);
         glTranslatef  (0, -x_side,  0.0);
      }
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   return 0;
}

/*> char                                                                                                           <* 
 *> VIEW__float             (void)                                                                                 <* 
 *> {                                                                                                              <* 
 *>    /+---(locals)-----------+------+----+-+/                                                                    <* 
 *>    char        x_use       =   '-';                                                                            <* 
 *>    int         x_top       =     0;                                                                            <* 
 *>    int         x_bot       =     0;                                                                            <* 
 *>    int         x_lef       =     0;                                                                            <* 
 *>    int         x_rig       =     0;                                                                            <* 
 *>    int         x_len       =     0;                                                                            <* 
 *>    int         n           =     0;                                                                            <* 
 *>    /+---(header)-------------------------+/                                                                    <* 
 *>    DEBUG_GRAF   yLOG_enter   (__FUNCTION__);                                                                   <* 
 *>    DEBUG_GRAF   yLOG_char    ("mode_curr" , MODE_curr ());                                                     <* 
 *>    /+---(defense)------------------------+/                                                                    <* 
 *>    if (MODE_curr () == MODE_COMMAND || MODE_curr () == MODE_SEARCH) {                                          <* 
 *>       DEBUG_GRAF   yLOG_note    ("command/search mode");                                                       <* 
 *>       n    = VIEW__abbr   (YVIKEYS_COMMAND);                                                                   <* 
 *>       if (s_parts [n].on == '-')  x_use = 'y';                                                                 <* 
 *>    } else {                                                                                                    <* 
 *>       DEBUG_GRAF   yLOG_note    ("other mode");                                                                <* 
 *>       n    = VIEW__abbr   (YVIKEYS_FORMULA);                                                                   <* 
 *>       if (s_parts [n].on == '-')  x_use = 'y';                                                                 <* 
 *>    }                                                                                                           <* 
 *>    DEBUG_GRAF   yLOG_char    ("x_use"     , x_use);                                                            <* 
 *>    if (x_use != 'y') {                                                                                         <* 
 *>       DEBUG_GRAF   yLOG_exit    (__FUNCTION__);                                                                <* 
 *>       return 0;                                                                                                <* 
 *>    }                                                                                                           <* 
 *>    /+---(draw)---------------------------+/                                                                    <* 
 *>    n    = VIEW__abbr    (YVIKEYS_FLOAT  );                                                                     <* 
 *>    yVIKEYS_view_coords  (YVIKEYS_FLOAT, &x_lef, &x_len, &x_bot, NULL);                                         <* 
 *>    x_rig = x_lef + x_len;                                                                                      <* 
 *>    if (myVIKEYS.env == YVIKEYS_OPENGL) {                                                                       <* 
 *>       DEBUG_GRAF   yLOG_note    ("opengl environment");                                                        <* 
 *>       glPushMatrix    (); {                                                                                    <* 
 *>          yVIKEYS_view_color (s_parts [n].color, 1.0);                                                          <* 
 *>          glBegin         (GL_POLYGON); {                                                                       <* 
 *>             glVertex3f  (x_lef , x_top, 50.0f);                                                                <* 
 *>             glVertex3f  (x_rig , x_top, 50.0f);                                                                <* 
 *>             glVertex3f  (x_rig , x_bot, 50.0f);                                                                <* 
 *>             glVertex3f  (x_lef , x_bot, 50.0f);                                                                <* 
 *>          } glEnd   ();                                                                                         <* 
 *>          yVIKEYS_view_color_adj (s_parts [n].color, YCOLOR_MIN, 1.0);                                          <* 
 *>          glLineWidth  ( 2);                                                                                    <* 
 *>          glBegin(GL_LINE_STRIP); {                                                                             <* 
 *>             glVertex3f  (x_lef , x_top, 50.0f);                                                                <* 
 *>             glVertex3f  (x_rig , x_top, 50.0f);                                                                <* 
 *>             glVertex3f  (x_rig , x_bot, 50.0f);                                                                <* 
 *>             glVertex3f  (x_lef , x_bot, 50.0f);                                                                <* 
 *>             glVertex3f  (x_lef , x_top, 50.0f);                                                                <* 
 *>          } glEnd   ();                                                                                         <* 
 *>          glTranslatef (x_lef + 3, x_bot,   60.0f);                                                             <* 
 *>          switch (MODE_curr ()) {                                                                               <* 
 *>          case MODE_COMMAND :  yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, CMDS_curr ());  break;   <* 
 *>          case MODE_SEARCH  :  yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, SRCH_curr ());  break;   <* 
 *>          }                                                                                                     <* 
 *>       } glPopMatrix   ();                                                                                      <* 
 *>    }                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                    <* 
 *>    DEBUG_GRAF   yLOG_exit    (__FUNCTION__);                                                                   <* 
 *>    return 0;                                                                                                   <* 
 *> }                                                                                                              <*/

char
VIEW__layer_list         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(find the screen part)-----------*/
   printf ("\n\nlayer inventory...\n");
   for (i = 0; i < s_nlayer; ++i) {
      printf ("  i %2d, on %c, name %-12.12s, drawer %p\n", i, s_layers [i].on, s_layers [i].name, s_layers [i].drawer);
   }
   /*---(complete)-----------------------*/
   return 0;
}

int
VIEW__layer_find         (char *a_name)
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
   for (i = 0; i < s_nlayer; ++i) {
      DEBUG_GRAF   yLOG_info    ("check"     , s_layers [i].name);
      if (s_layers [i].name [0] != a_name [0])         continue;
      if (strcmp (s_layers [i].name, a_name) != 0)     continue;
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
yVIKEYS_layer_add           (char *a_name, void *a_drawer, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_name"    , a_name);
   DEBUG_GRAF   yLOG_point   ("a_drawer"  , a_drawer);
   --rce;  if (a_drawer == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for dup)------------------*/
   n = VIEW__layer_find (a_name);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n >= 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add)----------------------------*/
   s_layers [s_nlayer].real   = 'y';
   strlcpy (s_layers [s_nlayer].name, a_name, LEN_LABEL);
   s_layers [s_nlayer].on     = '-';
   s_layers [s_nlayer].drawer = a_drawer;
   if (a_desc != NULL)  strlcpy (s_layers [s_nlayer].desc, a_desc, LEN_DESC);
   ++s_nlayer;
   /*> VIEW__layer_list ();                                                           <*/
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__layer_set          (char *a_name)
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
   n = VIEW__abbr (YVIKEYS_LAYERS);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_name"    , a_name);
   /*---(set the flag)-------------------*/
   DEBUG_GRAF   yLOG_char    ("current"   , s_parts [n].on);
   x_on = s_parts [n].on;
   if      (strcmp (a_name, "hide") == 0)  { s_parts [n].on = '-'; a = 0; }
   else if (strcmp (a_name, "show") == 0)  { s_parts [n].on = 'y'; a = 0; }
   else {
      for (i = 0; i < s_nlayer; ++i) {
         DEBUG_GRAF   yLOG_info    ("layer"     , s_layers [i].name);
         /*---(handle mass)--------------*/
         if (strcmp ("none", a_name) == 0) {
            DEBUG_GRAF   yLOG_note    ("turn all off");
            s_parts [n].on  = '-';
            if (s_layers [i].real == 'y') s_layers [i].on = '-';
            a = i;
            continue;
         }
         if (strcmp ("all" , a_name) == 0) {
            DEBUG_GRAF   yLOG_note    ("turn all on");
            s_parts [n].on  = 'y';
            if (s_layers [i].real == 'y') s_layers [i].on = 'y';
            a = i;
            continue;
         }
         /*---(handle individual)--------*/
         if (s_layers [i].real == '-')                     continue;
         if (s_layers [i].name [0] != a_name  [0])         continue;
         if (strcmp (s_layers [i].name, a_name) != 0)      continue;
         a = i;
         DEBUG_GRAF   yLOG_note    ("select this one");
         if (s_layers [i].on == 'y')  s_layers [i].on    = '-';
         else  {
            s_parts [n].on  = 'y';
            s_layers [i].on = 'y';
         }
         break;
      }
   }
   --rce;  if (a < 0) {
      DEBUG_GRAF   yLOG_note    ("no layer found");
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("new"       , s_parts [n].on);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
VIEW__layer_show         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*> printf ("start VIEW__layer_show\n");                                           <*/
   for (i = 0; i < s_nlayer; ++i) {
      /*> printf ("check i %2d, on %c, name %-12.12s, drawer %p\n", i, s_layers [i].on, s_layers [i].name, s_layers [i].drawer);   <*/
      if (s_layers [i].on != 'y'     )  continue;
      if (s_layers [i].drawer != NULL)  s_layers [i].drawer ();
   }
   return 0;
}

char
VIEW__opengl             (char a)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_max;
   int         y_max;
   int         z_max;
   int         i           =    0;
   int         x_len       =    0;
   int         x_beg       =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [a].name);
   /*---(prepare)------------------------*/
   x_max = s_parts [a].xmin + s_parts [a].xlen;
   y_max = s_parts [a].ymin + s_parts [a].ylen;
   z_max = s_parts [a].zmin + s_parts [a].zlen;
   if (s_parts [a].source != NULL)  {
      s_parts [a].source (s_parts [a].text);
      /*> printf ("source %p, s_parts [a].txt %s\n", s_parts [a].source, s_parts [a].text);   <*/
   }
   /*---(setup view)---------------------*/
   DEBUG_GRAF   yLOG_value   ("a"         , a);
   DEBUG_GRAF   yLOG_complex (s_parts [a].name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", s_parts [a].bott, s_parts [a].left, s_parts [a].wide, s_parts [a].tall, s_parts [a].on);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      glViewport      (s_parts [a].left, s_parts [a].bott, s_parts [a].wide, s_parts [a].tall);
      glMatrixMode    (GL_PROJECTION);
      glLoadIdentity  ();
      if (s_parts [a].type  == YVIKEYS_FLAT )  glOrtho         (s_parts [a].xmin, x_max, s_parts [a].ymin, y_max, s_parts [a].zmin, z_max);
      else                                     gluPerspective  (45.0f, (GLfloat) s_parts [a].wide / (GLfloat) s_parts [a].tall, 0.01f, 4000.0f);
      glMatrixMode    (GL_MODELVIEW);
      /*---(background)---------------------*/
      if (s_parts [a].type  == YVIKEYS_FLAT ) {
         DEBUG_GRAF   yLOG_note    ("draw background");
         glPushMatrix    (); {
            yVIKEYS_view_color (s_parts [a].color, 1.0);
            if (s_parts [a].abbr == YVIKEYS_VERSION && yURG_debugmode () == 'y')  yVIKEYS_view_color_adj (s_parts [a].color, YCOLOR_ACC, 1.0);
            if (s_parts [a].abbr == YVIKEYS_STATUS  && yVIKEYS_error  ())         yVIKEYS_view_color_adj (s_parts [a].color, YCOLOR_ACC, 1.0);
            glBegin         (GL_POLYGON); {
               glVertex3f  (s_parts [a].xmin, y_max           , -100.0f);
               glVertex3f  (x_max           , y_max           , -100.0f);
               glVertex3f  (x_max           , s_parts [a].ymin, -100.0f);
               glVertex3f  (s_parts [a].xmin, s_parts [a].ymin, -100.0f);
            } glEnd   ();
         } glPopMatrix   ();
      }
   } else if (myVIKEYS.env == YVIKEYS_CURSES) {
      ;;  /* not sure if i need to clear yet  */
   }
   /*---(display text)-------------------*/
   if (s_parts [a].text != NULL && strlen (s_parts [a].text) > 0) {
      DEBUG_GRAF   yLOG_note    ("draw text");
      if (myVIKEYS.env == YVIKEYS_OPENGL) {
         glPushMatrix    (); {
            if (s_parts [a].orient == 'r') {
               glTranslatef (-1.0f, 5.0f, 0.0f);
               glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
            } else {
               glTranslatef ( 3.0f, 1.0f, 0.0f);
            }
            yVIKEYS_view_color_adj (s_parts [a].color, YCOLOR_MIN, 0.8);
            yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, s_parts [a].text);
         } glPopMatrix   ();
      } else if (myVIKEYS.env == YVIKEYS_CURSES) {
         /*  set color --------------*/
         switch (s_parts [a].abbr) {
         case YVIKEYS_TITLE    :
         case YVIKEYS_VERSION  :
            yCOLOR_curs ("status" );
            break;
         case YVIKEYS_COMMAND  :
            yCOLOR_curs ("command" );
            break;
         case YVIKEYS_STATUS   :
            if (yVIKEYS_error ())  yCOLOR_curs ("warn"    );
            else                   yCOLOR_curs ("status"  );
            break;
         }
         x_len = strlen (s_parts [a].text);
         x_beg = (s_parts [a].tall - x_len) / 2;
         if (s_parts [a].orient == 'r') {
            for (i = 0; i < s_parts [a].tall; ++i) {
               mvprintw (s_parts [a].bott - i, s_parts [a].left, "%s", " ");
            }
            for (i = 0; i < x_len; ++i) {
               mvprintw (s_parts [a].bott - i - x_beg, s_parts [a].left, "%c", s_parts [a].text [x_len - i - 1]);
            }
         } else {
            mvprintw (s_parts [a].bott, s_parts [a].left, "%-*.*s", s_parts [a].wide, s_parts [a].wide, s_parts [a].text);
         }
         attrset  (0);
         /*> mvprintw (s_parts [a].bott, s_parts [a].left, "%-*.*s", s_parts [a].wide, s_parts [a].wide, s_parts [a].name);   <*/
         /*  clear color ------------*/
      }
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
   int         x_cur, y_cur;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   s_mag = a_mag;
   DEBUG_GRAF   yLOG_double  ("s_mag"     , s_mag);
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :
      if (myVIKEYS.font_scale <= 0.20) {
         myVIKEYS.font_scale = yFONT_width (myVIKEYS.font, myVIKEYS.point);
         DEBUG_GRAF   yLOG_double  ("font_scale", myVIKEYS.font_scale);
      }
      yCOLOR_set_clear (YCOLOR_BAS_MED);
      /*> glClearColor    (1.0f, 1.0f, 1.0f, 1.0f);                                   <*/
      /*> glClearColor    (0.3f, 0.3f, 0.3f, 1.0f);                                   <*/
      glClear         (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      break;
   case YVIKEYS_CURSES :
      if (myVIKEYS.redraw == 'y')  clear  ();
      break;
   }
   /*---(draw each element)--------------*/
   DEBUG_GRAF   yLOG_note    ("independent screen elements");
   for (n = 0; n < s_npart; ++n) {
      DEBUG_GRAF   yLOG_info    ("part"      , s_parts [n].name);
      DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
      DEBUG_GRAF   yLOG_char    ("on"        , s_parts [n].on);
      DEBUG_GRAF   yLOG_value   ("bott"      , s_parts [n].bott);
      DEBUG_GRAF   yLOG_value   ("tall"      , s_parts [n].tall);
      DEBUG_GRAF   yLOG_point   ("drawer"    , s_parts [n].drawer);
      if (strchr (OWN_SETUP, s_parts [n].own) == NULL)  continue;
      if (s_parts [n].on   == '-')                      continue;
      if (s_parts [n].type == YVIKEYS_DISABLE)          continue;
      rc = VIEW__opengl (n);
      if (s_parts [n].drawer != NULL)  s_parts [n].drawer ();
   }
   if (myVIKEYS.env == YVIKEYS_CURSES)  getyx (stdscr, y_cur, x_cur);
   /*---(on top of main)-----------------*/
   DEBUG_GRAF   yLOG_note    ("overlay screen elements");
   for (n = 0; n < s_npart; ++n) {
      DEBUG_GRAF   yLOG_info    ("part"      , s_parts [n].name);
      DEBUG_GRAF   yLOG_char    ("own"       , s_parts [n].own);
      DEBUG_GRAF   yLOG_char    ("on"        , s_parts [n].on);
      DEBUG_GRAF   yLOG_value   ("bott"      , s_parts [n].bott);
      DEBUG_GRAF   yLOG_value   ("tall"      , s_parts [n].tall);
      DEBUG_GRAF   yLOG_point   ("drawer"    , s_parts [n].drawer);
      if (s_parts [n].own    != OWN_OVERLAY)              continue;
      if (s_parts [n].on     == '-')                      continue;
      if (s_parts [n].type   == YVIKEYS_DISABLE)          continue;
      if (s_parts [n].drawer != NULL)  s_parts [n].drawer ();
      if (s_parts [n].source != NULL)  s_parts [n].source (NULL);
   }
   /*---(flush)--------------------------*/
   DEBUG_GRAF   yLOG_note    ("flush and show");
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :
      glXSwapBuffers(DISP, BASE);
      glFlush();
      break;
   case YVIKEYS_CURSES :
      mvprintw (y_cur, x_cur, "");
      refresh ();
      myVIKEYS.redraw = '-';
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
VIEW__unit              (char *a_question, char a_index)
{
   char        n;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "VIEW unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "size"           )   == 0) {
      n    = VIEW__abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VIEW %-12.12s: on %c, left %4d, wide %4d, bott %4d, tall %4d", s_parts [n].name, s_parts [n].on, s_parts [n].left, s_parts [n].wide, s_parts [n].bott, s_parts [n].tall);
   }
   if      (strcmp (a_question, "active"         )   == 0) {
      n    = VIEW__abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VIEW active      : %-12.12s, on %c", s_parts [n].name, s_parts [n].on);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


