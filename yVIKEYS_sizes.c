/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 *  this file was separated from yVIKEYS_view to reduce complexity at the
 *  sacrifice of speed, i.e., two files having to coordinate at arms length.
 *
 *  having this separate will make it easier to focus in yVIKEYS_view and
 *  may turn up bugs once i can not just use pointers directly ;)
 *
 *  the functions in this file are almost always only startup/init run.
 *  occasionally they run for resizings, but that does not require super
 *  fast response.
 *
 *  CRITICAL : sizing is totally different in ncurses vs opengl
 *    ncurses sizing is subtractive from a terminal screen size
 *    opengl resizes the window keeping the main window constant
 *
 */



static int    s_orig_wide  = 0;
static int    s_orig_tall  = 0;
static int    s_main_wide  = 0;
static int    s_main_tall  = 0;
static int    s_alt_wide   = 0;
static int    s_full_wide  = 0;
static int    s_full_tall  = 0;



static int    s_yaxis      =  0;
static int    s_xaxis      =  0;
static int    s_prog       =  0;
static int    s_cum        =  0;



#define      MAX_LAYOUT        10
typedef struct cLAYOUT   tLAYOUT;
struct cLAYOUT {
   char        name        [LEN_LABEL];     /* title for layout               */
   char        parts       [LEN_DESC ];     /* selection of elements          */
   char        desc        [LEN_DESC ];     /* explanation of layout          */
};
static const tLAYOUT   s_layouts [] = {
   /*---name------    tv bf nLY Xmap dr xsck G FMH  ,  12345678901234567890123456789012345678901234567890  */
   { "ANSWER_KEY"  , "tv bf nLY Xmap dr xsck G FMH" , "answer key to order and spacing"                      },
   { "min"         , "-- -- --- -m-- -- ---- - FMH" , "smallest footprint, least elements showing"           },
   { "starter"     , "-- -- --- -m-- -- --ck - FMH" , "little footprint for new applications"                },
   { "work"        , "tv -- --- -m-- -- xsck - FMH" , "more balanced display of common elements"             },
   { "rob"         , "tv -- --- -m-- -- xsc- - FMH" , "more balanced display of common elements"             },
   { "max"         , "tv bf n-- -m-p dr xsck - FMH" , "everything displays at one time"                      },
   { "ycolor"      , "tv bf n-- -map -r xsck - FMH" , "more balanced display of common elements"             },
   { "gyges"       , "-- -f --Y Xm-- -- xsck - FMH" , "prepared for gyges spreadsheet"                       },
   { "everything"  , "tv bf n-Y Xmap dr xsck - FMH" , "everything displays at one time"                      },
   { ""            , "-- -- --- ---- -- ---- - FMH" , ""                                                     },
};
static int  s_nlayout   = MAX_LAYOUT;



#define      MAX_OPTION      50
typedef  struct  cOPTION    tOPTION;
struct cOPTION  {
   char        part;
   char        opt         [LEN_LABEL];
   void       *source;
   char        desc        [LEN_DESC ];
};
tOPTION  s_options [MAX_OPTION ] = {
   { YVIKEYS_STATUS  , "empty"        , VIEW_status_default    , "empty status display"        },
   { YVIKEYS_STATUS  , "mode"         , MODE_status            , "display the mode stack"      },
   { YVIKEYS_STATUS  , "xmap"         , yvikeys_map_xstatus    , "x-axis position details"     },
   { YVIKEYS_STATUS  , "ymap"         , yvikeys_map_ystatus    , "y-axis position details"     },
   { YVIKEYS_STATUS  , "keys"         , yvikeys_keys_status    , "displays keystroke history"  },
   { YVIKEYS_STATUS  , "treg"         , yvikeys_sreg_status    , "displays contents of treg"   },
   { YVIKEYS_STATUS  , "words"        , SOURCE_status_words    , "displays word breaks"        },
   { YVIKEYS_STATUS  , "select"       , yvikeys_sreg_status_sel, "displays selection status"   },
   { YVIKEYS_STATUS  , "visual"       , yvikeys_visu_status    , "visual selection in map"     },
   { YVIKEYS_STATUS  , "file"         , yvikeys_file_status    , "file, control, and version"  },
   { YVIKEYS_STATUS  , "regs"         , yvikeys_mreg_status    , "current register information"},
   { YVIKEYS_STATUS  , "delay"        , yvikeys_delay_status   , "main loop timing settings"   },
   { YVIKEYS_STATUS  , "prog"         , yvikeys_prog_status    , "progress bar playing"        },
   { YVIKEYS_STATUS  , "main"         , yvikeys_main_status    , "main loop timing results"    },
   { YVIKEYS_STATUS  , "file"         , yvikeys_file_status    , "current fully qualified file name and default location"  },
   { YVIKEYS_STATUS  , "version"      , yvikeys_vers_status    , "current file verion control status, number and text"     },
   { YVIKEYS_STATUS  , "mark"         , yvikeys_mark_status    , "details of mark environment"                },
   { YVIKEYS_STATUS  , "macro"        , yvikeys_macro_estatus  , "details of macro playback"                },
   { YVIKEYS_STATUS  , "hint"         , yvikeys_hint_status    , "details of hint environment"              },
   { YVIKEYS_STATUS  , "search"       , yvikeys_srch_status    , "details of search environment"            },
   { YVIKEYS_STATUS  , "record"       , yvikeys_macro_rstatus  , "details of macro recording"               },
   { YVIKEYS_STATUS  , "sreg"         , yvikeys_sreg_status    , "details of current source register" },
   { YVIKEYS_STATUS  , "sundo"        , yvikeys_sundo_status   , "source editing undo stack" },
   { YVIKEYS_GRID    , "norm"         , VIEW__grid_normal      , "traditional cross-hatch grid" },
   { YVIKEYS_GRID    , "zoom"         , VIEW__grid_zoom        , "zoom/targeting grid"          },
   { YVIKEYS_STATUS  , "visual"       , yvikeys_visu_status    , "details of visual selection"  },
   { NULL            , ""             , NULL                   , ""                            },
};
static int  s_noption  = MAX_OPTION;



/*====================------------------------------------====================*/
/*===----                      setting defaults                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_sizes_init      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(part options)-------------------*/
   s_noption = 0;
   for (i = 0; i < MAX_OPTION; ++i) {
      if (s_options [i].part == 0)  break;
      ++s_noption;
   }
   /*---(screen layouts)-----------------*/
   s_nlayout = 0;
   for (i = 0; i < MAX_LAYOUT; ++i) {
      if (s_layouts [i].name [0] == 0)  break;
      ++s_nlayout;
   }
   myVIKEYS.loc_menu   = 't';
   myVIKEYS.loc_float  = 't';
   myVIKEYS.loc_hist   = 'f';
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      setting defaults                        ----===*/
/*====================------------------------------------====================*/
static void  o___DEFAULTS________o () { return; }

char
yvikeys_sizes_defaults  (cchar a_env)
{
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(global)-------------------------*/
   myVIKEYS.env = a_env;
   /*                                       -curses-- -opengl--                       */
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_TITLE   , 'y',   1,   0,  15,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_VERSION , 'y',   1,   5,  15,  40,  NULL);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_BUFFER  , '-',   0,   1,   0,  15,  NULL);
   yvikeys_view_defs (YVIKEYS_FORMULA , '-',   0,   1,   0,  15,  SOURCE_formula);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_NAV     , '-',  20,   0, 150,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_LAYERS  , '-',  20,   0, 150,   0,  yvikeys_layer_show);
   yvikeys_view_defs (YVIKEYS_YAXIS   , '-',   5,   0,  40,   0,  NULL);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_XAXIS   , '-',   0,   1,   0,  15,  NULL);
   yvikeys_view_defs (YVIKEYS_MAIN    , 'y',   0,   0,   0,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_ALT     , '-',   0,   0,   0,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_PROGRESS, '-',   0,  10,   0, 100,  NULL);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_DETAILS , '-',  20,   0, 250,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_RIBBON  , '-',   5,   0,  40,   0,  VIEW__ribbon);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_MODES   , 'y',   5,   1,  40,  15,  NULL);
   yvikeys_view_defs (YVIKEYS_STATUS  , 'y',   0,   1,   0,  15,  NULL);
   yvikeys_view_defs (YVIKEYS_COMMAND , 'y',   0,   1,   0,  15,  SOURCE_command);
   yvikeys_view_defs (YVIKEYS_KEYS    , 'y',   5,   1,  40,  15,  NULL);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_GRID    , '-',   0,   0,   0,   0,  NULL);
   yvikeys_view_defs (YVIKEYS_CURSOR  , '-',   0,   0,   0,   0,  VIEW__cursor);
   yvikeys_view_defs (YVIKEYS_OVERLAY , '-',   0,   0,   0,   0,  NULL);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_FLOAT   , 'y',   0,   1,   0,  15,  SOURCE_float);
   yvikeys_view_defs (YVIKEYS_HISTORY , 'y',   0,   0,   0,   0,  yvikeys_hist_show);
   yvikeys_view_defs (YVIKEYS_MENUS   , 'y',  40,  14, 280, 200,  yvikeys_menu_draw);
   yvikeys_view_defs (YVIKEYS_NOTES   , 'y',   0,   0,   0,   0,  yvikeys_note_draw);
   /*---------------- ---part--------- -on- wide tall wide tall ---drawer------------ */
   yvikeys_view_defs (YVIKEYS_WINDOW  , '-',   0,   0,   0,   0,  NULL);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                   setting horizontal values                  ----===*/
/*====================------------------------------------====================*/
static void  o___HORZONTAL_______o () { return; }

char
yvikeys_sizes_horz_fixed (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   int         x_max       =    0;          /* widest at level                */
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_cum = 0;
   /*---(find fixed widths)--------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      x_max = 0;
      yvikeys_view_cursor ('[', &p, NULL);
      while (p != NULL) {
         DEBUG_GRAF   yLOG_complex ("checking"  , "%-10.10p, %c %-10.10s, on %c, horz %d, wide %3d", p, p->abbr, p->name, p->on, p->horz, p->wide);
         if (p->horz == x_lvl  ) {
            /*---(set width)----------------*/
            if    (p->on  != 'y')   p->wide = 0;
            else                    p->wide = p->def_wide;
            /*---(find greatest)------------*/
            if (p->nox == '-'  && p->wide > x_max)   x_max = p->wide;
            /*---(read-out)-----------------*/
            DEBUG_GRAF   yLOG_complex ("fixed"     , "%c %-12.12s wide %3d, max %3d, cum %3d", p->abbr, p->name, p->wide, x_max, s_cum);
            /*---(special)------------------*/
            if (p->abbr == YVIKEYS_YAXIS   )  s_yaxis = p->wide;
            /*---(done)---------------------*/
         }
         yvikeys_view_cursor ('>', &p, NULL);
      }
      s_cum += x_max;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_horz_auto  (cint a_wide, cint a_alt)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   int         x_float     = 40;
   int         x_hist      = 60;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(major sizes)--------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      s_full_wide = s_cum + a_wide + a_alt;
      s_main_wide = a_wide;
   } else {
      s_full_wide = a_wide;
      s_main_wide = a_wide - s_cum - a_alt;
      x_float /= 10;
      x_hist  /= 10;
   }
   s_alt_wide  = a_alt;
   DEBUG_GRAF   yLOG_complex ("bigs"      , "full %3d, main %3d, alt %3d, s_cum %3d", s_full_wide, s_main_wide, s_alt_wide, s_cum);
   /*---(walk auto parts)----------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      /*---(filter)-------------------*/
      if (p->abbr != YVIKEYS_WINDOW && (p->on != 'y' || p->mgmt != YVIKEYS_AUTO)) {
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(set)----------------------*/
      switch (p->abbr) {
      case YVIKEYS_WINDOW   : p->wide = s_full_wide;                   break;
      case YVIKEYS_MAIN     :
      case YVIKEYS_NOTES    : p->wide = s_main_wide;                   break;
      case YVIKEYS_ALT      : p->wide = s_alt_wide;                    break;
      case YVIKEYS_PROGRESS : p->wide = s_main_wide + a_alt;           break;
      case YVIKEYS_XAXIS    : p->wide = s_main_wide + a_alt + s_yaxis; break;
      default               : yvikeys_view_cursor ('>', &p, NULL);     continue;
      }
      /*---(display)------------------*/
      DEBUG_GRAF   yLOG_complex ("automatic" , "%c %-12.12s %3d wide", p->abbr, p->name, p->wide);
      /*---(done)---------------------*/
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_horz_var   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   int         x_max       =    0;          /* widest at level                */
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(set lefts)----------------------*/
   s_cum = 0;
   /*---(find var widths)----------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      x_max = 0;
      yvikeys_view_cursor ('[', &p, NULL);
      while (p != NULL) {
         if (p->horz == x_lvl  ) {
            /*---(set left)-----------------*/
            if (p->x_tie  == '-')   p->left = s_cum;
            /*---(find greatest)------------*/
            if (p->nox == '-'  && p->wide > x_max)   x_max = p->wide;
            /*---(read-out)-----------------*/
            DEBUG_GRAF   yLOG_complex ("variable"  , "%c %-12.12s wide %3d, max %3d, cum %3d", p->abbr, p->name, p->wide, x_max, s_cum);
            /*---(done)---------------------*/
         }
         yvikeys_view_cursor ('>', &p, NULL);
      }
      s_cum += x_max;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_horz_link  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   tPARTS     *p           = NULL;
   tPARTS     *a           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(find var widths)----------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      yvikeys_view_cursor ('[', &p, &a);
      while (p != NULL) {
         if (p->horz == x_lvl && a != NULL) {
            /*---(set left)-----------------*/
            if (p->x_tie  != '-')  {
               /*---(rightside is variable)----------*/
               /*   [--------------full]----------------]               */
               /*   xxxx[--a--][-----------p----------->?               */
               if (a->on != 'y' || a->wide > 0) {
                  DEBUG_GRAF   yLOG_note    ("leftmost is fixed length or off");
                  p->left = a->left + a->wide;
                  if (p->on == 'y')  p->wide  = s_full_wide - p->left;
                  else               p->wide  = 0;
                  /*---(status size)-----*/
                  if (p->abbr == YVIKEYS_STATUS) {
                     if      (p->wide <= 40)   myVIKEYS.status_w = 'S';
                     else if (p->wide <= 65)   myVIKEYS.status_w = 'M';
                     else                      myVIKEYS.status_w = 'L';
                  }
               }
               /*---(leftside is variable)-----------*/
               /*   [--------------full]----------------]               */
               /*   xxxx[-----------a----------->?[--p--]               */
               else {
                  DEBUG_GRAF   yLOG_note    ("leftmost is variable length");
                  p->left  = s_full_wide - p->wide;
                  if (a->on == 'y')  a->wide  = p->left - a->left;
                  else               a->wide  = 0;
               }
               /*---(read-out)-----------------*/
               DEBUG_GRAF   yLOG_complex ("linked"    , "%c %-12.12s wide %3d AND %c %-12.12s wide %3d", p->abbr, p->name, p->wide, a->abbr, a->name, a->wide);
            }
            /*---(done)---------------------*/
         }
         yvikeys_view_cursor ('>', &p, &a);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_horz__float (tPARTS *p, int a_left)
{
   p->wide = s_main_wide * 0.90;
   p->left = a_left + (s_main_wide * 0.05);
   DEBUG_GRAF   yLOG_complex ("horz_float", "%c, %3da, %3ds, %3dw, %3dl", myVIKEYS.loc_float, a_left, s_main_wide, p->wide, p->left);
   return 0;
}

char
yvikeys_sizes_horz__menu  (tPARTS *p, int a_left)
{
   DEBUG_GRAF   yLOG_value   ("a_left"    , a_left);
   DEBUG_GRAF   yLOG_value   ("wide"      , p->wide);
   p->wide = p->def_wide;
   switch (p->anchor) {
   case YVIKEYS_TOPLEF : case YVIKEYS_MIDLEF : case YVIKEYS_BOTLEF :
      p->left = a_left;
      break;
   case YVIKEYS_UPSBEG : case YVIKEYS_MIDBEG : case YVIKEYS_LOWBEG :
      p->left = a_left + (s_main_wide * 0.10);
      break;
   case YVIKEYS_TOPCEN : case YVIKEYS_MIDCEN : case YVIKEYS_BOTCEN : case YVIKEYS_UPSCEN : case YVIKEYS_LOWCEN :
      p->left = a_left + (s_main_wide * 0.50) - (p->wide * 0.50);
      break;
   case YVIKEYS_UPSEND : case YVIKEYS_MIDEND : case YVIKEYS_LOWEND :
      p->left = a_left + (s_main_wide * 0.90) - p->wide;
      break;
   case YVIKEYS_TOPRIG : case YVIKEYS_MIDRIG : case YVIKEYS_BOTRIG :
      p->left = a_left + s_main_wide - p->wide;
      break;
   }
   DEBUG_GRAF   yLOG_complex ("horz_menu" , "%c, %3da, %3ds, %3dw, %3dl", myVIKEYS.loc_menu, a_left, s_main_wide, p->wide, p->left);
   return 0;
}

char
yvikeys_sizes_horz__hist  (tPARTS *p, int a_left)
{
   switch (p->anchor) {
   case YVIKEYS_MIDLEF :
      p->wide = s_main_wide * 0.45;
      p->left = a_left + (s_main_wide * 0.05);
      break;
   case YVIKEYS_MIDCEN :
      p->wide = s_main_wide * 0.50;
      p->left = a_left + (s_main_wide * 0.25);
      break;
   case YVIKEYS_MIDRIG :
      p->wide = s_main_wide * 0.45;
      p->left = a_left + (s_main_wide * 0.50);
      break;
   case YVIKEYS_ALLALL :
      p->wide = s_main_wide * 0.80;
      p->left = a_left + (s_main_wide * 0.10);
      break;
   }
   DEBUG_GRAF   yLOG_complex ("horz_hist" , "%c, %3da, %3ds, %3dw, %3dl", myVIKEYS.loc_hist, a_left, s_main_wide, p->wide, p->left);
   return 0;
}

char
yvikeys_sizes_horz_float (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left      =    0;
   int         x_curses    =    0;
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(get main info)------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL )  {
      yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, NULL, NULL, NULL, NULL);
   } else {
      yVIKEYS_view_size   (YVIKEYS_MAIN, &x_left, NULL, NULL, NULL, NULL);
   }
   /*---(walk floats)--------------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      /*---(filter)-------------------*/
      if (p->mgmt != YVIKEYS_AUTO) {
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(set)----------------------*/
      switch (p->abbr) {
      case YVIKEYS_FLOAT    :
         yvikeys_sizes_horz__float (p, x_left);
         break;
      case YVIKEYS_MENUS    :
         yvikeys_sizes_horz__menu  (p, x_left);
         break;
      case YVIKEYS_HISTORY  :
         yvikeys_sizes_horz__hist  (p, x_left);
         break;
      default               :
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(display)------------------*/
      DEBUG_GRAF   yLOG_complex ("overlay"   , "%c %-12.12s %3d wide", p->abbr, p->name, p->wide);
      /*---(done)---------------------*/
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_horz_other  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(walk floats)--------------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      /*---(filter)-------------------*/
      if (p->on != 'y' || p->wide != 0) {
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(set)----------------------*/
      p->wide = s_full_wide - p->left;
      /*---(display)------------------*/
      DEBUG_GRAF   yLOG_complex ("other"     , "%c %-12.12s %3d wide", p->abbr, p->name, p->wide);
      /*---(done)---------------------*/
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_horz       (cint a_wide, cint a_alt)
{
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "wide %3d, alt %3d", a_wide, a_alt);
   /*---(widths)-------------------------*/
   yvikeys_sizes_horz_fixed ();
   yvikeys_sizes_horz_auto  (a_wide, a_alt);
   yvikeys_sizes_horz_var   ();
   yvikeys_sizes_horz_link  ();
   yvikeys_sizes_anchor     (YVIKEYS_MAIN);
   yvikeys_sizes_horz_float ();
   yvikeys_sizes_horz_other ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    setting vertical values                   ----===*/
/*====================------------------------------------====================*/
static void  o___VERTICAL________o () { return; }

char         /*-> resize heights based on layout -----[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_vert_fixed (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   int         x_max       =    0;          /* widest at level                */
   tPARTS     *p           = NULL;
   char        x_skip      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_cum = 0;
   /*---(find fixed heights)-------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      x_max = 0;
      yvikeys_view_cursor ('[', &p, NULL);
      while (p != NULL) {
         DEBUG_GRAF   yLOG_complex ("checking"  , "%-10.10p, %c %-10.10s, on %c, vert %d, %c %c %c", p, p->abbr, p->name, p->on, p->vert, p->y_tie, p->under, p->noy);
         if (p->vert == x_lvl  ) {
            x_skip = '-';
            /*---(set height)---------------*/
            if    (p->on  != 'y')   p->tall = 0;
            else                    p->tall = p->def_tall;
            /*---(find greatest)------------*/
            if (p->y_tie != '-')    x_skip = 'y';
            if (p->under != '-')    x_skip = 'y';
            if (p->noy   != '-')    x_skip = 'y';
            if (x_skip == '-' && p->tall > x_max)   x_max = p->tall;
            /*---(read-out)-----------------*/
            DEBUG_GRAF   yLOG_complex ("fixed"     , "%c %-12.12s tall %3d, bott %3d, max %3d, cum %3d, %c", p->abbr, p->name, p->tall, p->bott, x_max, s_cum, x_skip);
            /*---(special)------------------*/
            if (p->abbr == YVIKEYS_PROGRESS)  s_prog   = p->tall;
            if (p->abbr == YVIKEYS_XAXIS   )  s_xaxis  = p->tall;
            /*---(done)---------------------*/
         }
         yvikeys_view_cursor ('>', &p, NULL);
      }
      s_cum += x_max;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_vert_auto  (cint a_tall)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   int         x_other     =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(major sizes)--------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      s_full_tall = s_cum + a_tall;
      s_main_tall = a_tall;
      x_other     = a_tall + s_prog + s_xaxis;
   } else {
      s_full_tall = a_tall;
      s_main_tall = a_tall - s_cum;
      x_other     = a_tall - s_cum + s_prog;
   }
   DEBUG_GRAF   yLOG_complex ("bigs"      , "full %3d, main %3d, other %3d, s_cum %3d", s_full_tall, s_main_tall, x_other, s_cum);
   /*---(walk auto parts)----------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      /*---(filter)-------------------*/
      if (p->abbr != YVIKEYS_WINDOW && (p->on != 'y' || p->mgmt != YVIKEYS_AUTO)) {
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(set)----------------------*/
      switch (p->abbr) {
      case YVIKEYS_WINDOW   : p->tall = s_full_tall;               break;
      case YVIKEYS_MAIN     :
      case YVIKEYS_NOTES    :
      case YVIKEYS_ALT      :
      case YVIKEYS_YAXIS    : p->tall = s_main_tall;               break;
      case YVIKEYS_NAV      :
      case YVIKEYS_DETAILS  :
      case YVIKEYS_RIBBON   : p->tall = x_other;                   break;
      default               : yvikeys_view_cursor ('>', &p, NULL); continue;
      }
      /*---(display)------------------*/
      DEBUG_GRAF   yLOG_complex ("automatic" , "%c %-12.12s tall %3d, bott %3d", p->abbr, p->name, p->tall, p->bott);
      /*---(done)---------------------*/
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_vert_var   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   int         x_max       =    0;          /* widest at level                */
   tPARTS     *p           = NULL;
   char        x_skip      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(set lefts)----------------------*/
   s_cum = 0;
   /*---(find var widths)----------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      x_max = 0;
      yvikeys_view_cursor ('[', &p, NULL);
      while (p != NULL) {
         DEBUG_GRAF   yLOG_complex ("checking"  , "%-10.10p, %c %-10.10s, on %c, vert %d, %c %c %c", p, p->abbr, p->name, p->on, p->vert, p->y_tie, p->under, p->noy);
         if (p->vert == x_lvl  ) {
            x_skip = '-';
            /*---(set bottom)---------------*/
            if (p->y_tie == '-')    p->bott = s_cum;
            /*---(find greatest)------------*/
            if (p->y_tie != '-')    x_skip = 'y';
            if (p->under != '-')    x_skip = 'y';
            if (p->noy   != '-')    x_skip = 'y';
            if (x_skip == '-' && p->tall > x_max)   x_max = p->tall;
            /*---(read-out)-----------------*/
            DEBUG_GRAF   yLOG_complex ("variable"  , "%c %-12.12s tall %3d, bott %3d, max %3d, cum %3d, %c", p->abbr, p->name, p->tall, p->bott, x_max, s_cum, x_skip);
            /*---(done)---------------------*/
         }
         yvikeys_view_cursor ('>', &p, NULL);
      }
      s_cum += x_max;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_vert_link  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_lvl       =    0;          /* evaluation level               */
   tPARTS     *p           = NULL;
   tPARTS     *a           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(find var widths)----------------*/
   for (x_lvl = 1; x_lvl <= 9; ++x_lvl) {
      DEBUG_GRAF   yLOG_value   ("LEVEL"     , x_lvl);
      yvikeys_view_cursor ('[', &p, &a);
      while (p != NULL) {
         DEBUG_GRAF   yLOG_complex ("checking"  , "%-10.10p, %c %-10.10s, on %c, vert %d, %c %c %c", p, p->abbr, p->name, p->on, p->vert, p->y_tie, p->under, p->noy);
         if (p->vert == x_lvl && a != NULL) {
            /*---(set left)-----------------*/
            if (p->y_tie  != '-')  {
               a->tall  = s_full_tall - a->bott - p->tall;
               p->bott  = s_full_tall - p->tall;
               /*---(read-out)-----------------*/
               DEBUG_GRAF   yLOG_complex ("linked"    , "%c %-12.12s tall %3d, bott %3d AND %c %-12.12s tall %3d, bott %3d", p->abbr, p->name, p->tall, p->bott, a->abbr, a->name, a->tall, a->bott);
            }
         }
         yvikeys_view_cursor ('>', &p, &a);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_vert__float (tPARTS *p, int a_bott)
{
   p->tall = p->def_tall;
   switch (p->anchor) {
   case YVIKEYS_TOPCEN : p->bott = a_bott + s_main_tall - (p->tall * 2.0); break;
   case YVIKEYS_UPSCEN : p->bott = a_bott + s_main_tall * (0.75) - (p->tall * 0.5);  break;
   case YVIKEYS_MIDCEN : p->bott = a_bott + s_main_tall * (0.50) - (p->tall * 0.5);  break;
   case YVIKEYS_LOWCEN : p->bott = a_bott + s_main_tall * (0.25) - (p->tall * 0.5);  break;
   case YVIKEYS_BOTCEN : p->bott = a_bott + p->tall;  break;
   default    :
                         DEBUG_GRAF   yLOG_note    ("no appropriate float location found");
                         break;
   }
   DEBUG_GRAF   yLOG_complex ("vert_float", "%c, %3da, %3ds, %3dt, %3db", myVIKEYS.loc_float, a_bott, s_main_tall, p->tall, p->bott);
   return 0;
}

char
yvikeys_sizes_vert__menu  (tPARTS *p, int a_bott)
{
   p->tall = p->def_tall;
   switch (p->anchor) {
   case YVIKEYS_TOPLEF : case YVIKEYS_TOPCEN : case YVIKEYS_TOPRIG :
      p->bott = a_bott + s_main_tall - p->tall;
      break;
   case YVIKEYS_UPSBEG : case YVIKEYS_UPSCEN : case YVIKEYS_UPSEND :
      p->bott = a_bott + (s_main_tall * 0.90) - p->tall;
      break;
   case YVIKEYS_MIDLEF : case YVIKEYS_MIDCEN : case YVIKEYS_MIDRIG : case YVIKEYS_MIDBEG : case YVIKEYS_MIDEND :
      p->bott = a_bott + (s_main_tall * 0.50) - (p->tall * 0.50);
      break;
   case YVIKEYS_LOWBEG : case YVIKEYS_LOWCEN : case YVIKEYS_LOWEND :
      p->bott = a_bott + (s_main_tall * 0.10);
      break;
   case YVIKEYS_BOTLEF : case YVIKEYS_BOTCEN : case YVIKEYS_BOTRIG :
      p->bott = a_bott;
      break;
   default    :
      DEBUG_GRAF   yLOG_note    ("no appropriate menu location found");
      break;
   }
   DEBUG_GRAF   yLOG_complex ("vert_menu" , "%c, %3da, %3ds, %3dt, %3db", myVIKEYS.loc_menu, a_bott, s_main_tall, p->tall, p->bott);
   return 0;
}

char
yvikeys_sizes_vert__hist  (tPARTS *p, int a_bott)
{
   p->tall = s_main_tall * 0.90;
   p->bott = a_bott + (s_main_tall * 0.05);
   DEBUG_GRAF   yLOG_complex ("vert_hist" , "%c, %3da, %3ds, %3dt, %3db", myVIKEYS.loc_hist, a_bott, s_main_tall, p->tall, p->bott);
   return 0;
}

char
yvikeys_sizes_vert_float (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_bott      =    0;
   int         x_curses    =    0;
   int         x_top       =    0;
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(get main info)------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL )  {
      yVIKEYS_view_bounds (YVIKEYS_MAIN, NULL, NULL, NULL, &x_bott, NULL, NULL);
   } else {
      yVIKEYS_view_size   (YVIKEYS_MAIN, NULL, NULL, &x_bott, NULL, NULL);
   }
   DEBUG_GRAF   yLOG_complex ("working"   , "bott %3d tall %3d", x_bott, s_main_tall);
   /*---(walk floats)--------------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      /*---(filter)-------------------*/
      if (p->mgmt != YVIKEYS_AUTO) {
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(set)----------------------*/
      switch (p->abbr) {
      case YVIKEYS_FLOAT    :
         yvikeys_sizes_vert__float (p, x_bott);
         break;
      case YVIKEYS_MENUS    :
         yvikeys_sizes_vert__menu  (p, x_bott);
         break;
      case YVIKEYS_HISTORY  :
         yvikeys_sizes_vert__hist  (p, x_bott);
         break;
      default               :
         yvikeys_view_cursor ('>', &p, NULL);
         continue;
      }
      /*---(display)------------------*/
      DEBUG_GRAF   yLOG_complex ("overlay"   , "%c %-12.12s tall %3d, bott %3d", p->abbr, p->name, p->tall, p->bott);
      /*---(done)---------------------*/
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> flip bottoms for ncurses -----------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_vert_fill  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(fill in widths)-----------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      if (p->on   == 'y' && p->tall ==  0 ) {
         p->tall = s_full_tall - p->bott;
         DEBUG_GRAF   yLOG_complex ("fill"      , "%c %-12.12s tall %3d, bott %3d", p->abbr, p->name, p->tall, p->bott);
      }
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> flip bottoms for ncurses -----------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_vert_flip  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      DEBUG_GRAF   yLOG_note    ("not required for opengl");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(display)------------------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      DEBUG_GRAF   yLOG_complex (p->name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", p->bott, p->left, p->wide, p->tall, p->on);
      p->bott = s_full_tall - p->bott - 1;
      DEBUG_GRAF   yLOG_complex (p->name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", p->bott, p->left, p->wide, p->tall, p->on);
      if (p->bott < 0)  p->bott = 0;
      DEBUG_GRAF   yLOG_complex (p->name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", p->bott, p->left, p->wide, p->tall, p->on);
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> resize widths based on layout ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_vert       (cint a_tall)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;          /* current part                   */
   int         x_under     =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_tall"    , a_tall);
   /*---(heights)------------------------*/
   yvikeys_sizes_vert_fixed ();
   yvikeys_sizes_vert_auto  (a_tall);
   yvikeys_sizes_vert_var   ();
   yvikeys_sizes_vert_link  ();
   yvikeys_sizes_anchor     (YVIKEYS_MAIN);
   yvikeys_sizes_vert_float ();
   yvikeys_sizes_vert_fill  ();
   yvikeys_sizes_vert_flip  ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      movable parts                           ----===*/
/*====================------------------------------------====================*/
static void  o___MOVABLE_________o () { return; }

char
yvikeys_sizes_menu_loc  (char a_loc)
{
   char        rce         =  -10;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_loc"     , a_loc);
   --rce;  if (a_loc == 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("a_loc"     , a_loc);
   DEBUG_GRAF   yLOG_info    ("valid"     , YVIKEYS_LOC_MENU);
   --rce;  if (strchr (YVIKEYS_LOC_MENU, a_loc) == NULL)  {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   myVIKEYS.loc_menu = a_loc;
   yvikeys_view_reanchor (YVIKEYS_MENUS, a_loc);
   yvikeys_sizes_resize ('-');
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_float_loc (char a_loc)
{
   char        rce         =  -10;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_value   ("a_loc"     , a_loc);
   --rce;  if (a_loc == 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("a_loc"     , a_loc);
   DEBUG_GRAF   yLOG_info    ("valid"     , YVIKEYS_LOC_FLOAT);
   if (strchr (YVIKEYS_LOC_FLOAT, a_loc) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   myVIKEYS.loc_float = a_loc;
   yvikeys_view_reanchor (YVIKEYS_FLOAT, a_loc);
   yvikeys_sizes_resize ('-');
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_hist_loc  (char a_loc)
{
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   if (a_loc == 0)                       return -1;
   if (strchr (YVIKEYS_LOC_HIST, a_loc) == NULL)   return -2;
   myVIKEYS.loc_hist  = a_loc;
   yvikeys_view_reanchor (YVIKEYS_HISTORY, a_loc);
   yvikeys_sizes_resize ('-');
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         anchoring                            ----===*/
/*====================------------------------------------====================*/
static void  o___ANCHOR__________o () { return; }

char
yvikeys_sizes__anchor   (tPARTS *p)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_len       =    0;
   int         y_len       =    0;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(handle auto first)--------------*/
   if (p->mgmt != YVIKEYS_AUTO) {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   p->type = YVIKEYS_FLAT;
   x_len = p->xlen = p->wide;
   y_len = p->ylen = p->tall;
   /*---(horzontal)----------------------*/
   switch (p->anchor) {
   case YVIKEYS_TOPLEF : case YVIKEYS_MIDLEF : case YVIKEYS_BOTLEF :
      p->xmin = 0;
      break;
   case YVIKEYS_UPSBEG : case YVIKEYS_MIDBEG : case YVIKEYS_LOWBEG :
      p->xmin = 0 - (x_len * 0.75);
      break;
   case YVIKEYS_TOPCEN : case YVIKEYS_UPSCEN : case YVIKEYS_MIDCEN : case YVIKEYS_LOWCEN : case YVIKEYS_BOTCEN :
      p->xmin = 0 - (x_len * 0.50);
      break;
   case YVIKEYS_UPSEND : case YVIKEYS_MIDEND : case YVIKEYS_LOWEND :
      p->xmin = 0 - (x_len * 0.25);
      break;
   case YVIKEYS_TOPRIG : case YVIKEYS_MIDRIG : case YVIKEYS_BOTRIG :
      p->xmin = 0 - x_len;
      break;
   }
   /*---(vertical)-----------------------*/
   switch (p->anchor) {
   case YVIKEYS_TOPLEF : case YVIKEYS_TOPCEN : case YVIKEYS_TOPRIG :
      p->ymin = 0 - y_len;
      break;
   case YVIKEYS_UPSBEG : case YVIKEYS_UPSCEN : case YVIKEYS_UPSEND :
      p->ymin = 0 - (y_len * 0.75);
      break;
   case YVIKEYS_MIDLEF : case YVIKEYS_MIDBEG : case YVIKEYS_MIDCEN : case YVIKEYS_MIDEND : case YVIKEYS_MIDRIG : 
      p->ymin = 0 - (y_len * 0.50);
      break;
   case YVIKEYS_LOWBEG : case YVIKEYS_LOWCEN : case YVIKEYS_LOWEND :
      p->ymin = 0 - (y_len * 0.75);
      break;
   case YVIKEYS_BOTLEF : case YVIKEYS_BOTCEN : case YVIKEYS_BOTRIG :
      p->ymin = 0;
      break;
   }
   /*---(depth)--------------------------*/
   p->zmin = -500;
   p->zlen = 1000;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_anchor     (cchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   yvikeys_view_pointer (a_abbr, &p, NULL);
   if (p == NULL)  return -10;
   yvikeys_sizes__anchor (p);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_anchor_all (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(find fixed heights)-------------*/
   yvikeys_view_cursor ('[', &p, NULL);
   while (p != NULL) {
      yvikeys_sizes__anchor (p);
      DEBUG_GRAF   yLOG_complex (p->name, "bott %4d, left %4d, wide %4d, tall %4d, on %c", p->bott, p->left, p->wide, p->tall, p->on);
      DEBUG_GRAF   yLOG_complex (p->name, "xmin %4d, xlen %4d, ymin %4d, ylen %4d, zmin %4d, zlen %4d", p->xmin, p->xlen, p->ymin, p->ylen, p->zmin, p->zlen);
      yvikeys_view_cursor ('>', &p, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       overall sizing                         ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char         /*-> resize all based on main -----------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sizes_resize     (cchar a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   tPARTS     *p           = NULL;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   DEBUG_PROG   yLOG_complex ("origs"     , "%3dw, %3dt, %3da", s_orig_wide, s_orig_tall, s_alt_wide);
   /*---(size window)--------------------*/
   if (a_type == 'r' && myVIKEYS.env == YVIKEYS_CURSES) {
      getmaxyx (stdscr, s_orig_tall, s_orig_wide);
   }
   /*---(widths)-------------------------*/
   DEBUG_GRAF   yLOG_value   ("orig_wide" , s_orig_wide);
   yvikeys_sizes_horz  (s_orig_wide, s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("main_wide" , s_main_wide);
   DEBUG_GRAF   yLOG_value   ("alt_wide"  , s_alt_wide);
   DEBUG_GRAF   yLOG_value   ("full_wide" , s_full_wide);
   /*---(heights)------------------------*/
   DEBUG_GRAF   yLOG_value   ("orig_tall" , s_orig_tall);
   yvikeys_sizes_vert  (s_orig_tall);
   DEBUG_GRAF   yLOG_value   ("main_tall" , s_main_tall);
   DEBUG_GRAF   yLOG_value   ("full_tall" , s_full_tall);
   /*---(display)------------------------*/
   yvikeys_sizes_anchor_all ();
   /*---(other)--------------------------*/
   yvikeys_note_resize ();
   /*---(size window)--------------------*/
   if (a_type == 'r' && myVIKEYS.env == YVIKEYS_OPENGL) {
      yX11_resize (s_full_wide, s_full_tall);
   }
   myVIKEYS.redraw = 'y';
   /*---(update map)---------------------*/
   yvikeys_view_pointer (YVIKEYS_MAIN, &p, NULL);
   g_xmap.uavail = p->wide;
   g_ymap.uavail = p->tall;
   /*> if (myVIKEYS.env == YVIKEYS_CURSES) {                                          <* 
    *>    g_xmap.uavail = p->wide;                                                    <* 
    *>    g_ymap.uavail = p->tall;                                                    <* 
    *> } else {                                                                       <* 
    *>    g_xmap.uavail = p->wide / 15;                                               <* 
    *>    g_ymap.uavail = p->tall / 15;                                               <* 
    *> }                                                                              <*/
   yVIKEYS_map_refresh ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_resize          (cint a_wide, cint a_tall, cint a_alt)
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
   yvikeys_sizes_resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yVIKEYS_status_width    (void) { return myVIKEYS.status_w; }



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
   n = yvikeys_view__abbr (a_part);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);                    <*/
   /*> DEBUG_GRAF   yLOG_char    ("abbr"      , s_parts [n].abbr);                    <*/
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
   s_options [s_noption].part   = a_part;
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
yvikeys_sizes__linked     (tPARTS *p)
{
   tPARTS     *a           = NULL;
   tPARTS     *x_ind       = NULL;
   tPARTS     *x_dep       = NULL;
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   /*---(get alternate)------------------*/
   switch (p->abbr) {
   case YVIKEYS_TITLE    :  yvikeys_view_pointer (YVIKEYS_VERSION, &a, NULL);  break;
   case YVIKEYS_STATUS   :  yvikeys_view_pointer (YVIKEYS_MODES  , &a, NULL);  break;
   case YVIKEYS_COMMAND  :  yvikeys_view_pointer (YVIKEYS_KEYS   , &a, NULL);  break;
   case YVIKEYS_VERSION  :  yvikeys_view_pointer (YVIKEYS_TITLE  , &a, NULL);  break;
   case YVIKEYS_MODES    :  yvikeys_view_pointer (YVIKEYS_STATUS , &a, NULL);  break;
   case YVIKEYS_KEYS     :  yvikeys_view_pointer (YVIKEYS_COMMAND, &a, NULL);  break;
   case YVIKEYS_NAV      :  yvikeys_view_pointer (YVIKEYS_LAYERS , &a, NULL);  break;
   case YVIKEYS_LAYERS   :  yvikeys_view_pointer (YVIKEYS_NAV    , &a, NULL);  break;
   default   :
                            DEBUG_GRAF   yLOG_snote   ("not a target");
                            DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
                            return 0;;
   }
   /*---(handle switching)---------------*/
   switch (p->abbr) {
   case YVIKEYS_TITLE    : case YVIKEYS_STATUS   : case YVIKEYS_COMMAND  :
      DEBUG_GRAF   yLOG_snote   ("independent");
      if      (p->on == '-' && a->on == 'y')  a->on = '-';
      break;
   case YVIKEYS_VERSION  : case YVIKEYS_MODES    : case YVIKEYS_KEYS     :
      DEBUG_GRAF   yLOG_snote   ("dependent");
      if      (a->on == 'X')                  p->on = '-';
      else if (p->on == 'y' && a->on == '-')  a->on = 'y';
      break;
   case YVIKEYS_NAV      : case YVIKEYS_LAYERS   :
      DEBUG_GRAF   yLOG_snote   ("either/or");
      if      (p->on == 'y' && a->on == 'y')  a->on = '-';
      break;
   }
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_switch     (char *a_name, char *a_opt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tPARTS     *p           = NULL;
   char        x_on        =  '-';
   char        i           =    0;
   char        n           =   -1;
   char        x_good      =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   yvikeys_view_find (a_name, &p, NULL);
   --rce;  if (p == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_opt == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_opt"     , a_opt);
   --rce;  if (p->on == YVIKEYS_DISABLE) {
      if (strcmp (a_opt, "enable" ) != 0) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(set the flag)-------------------*/
   DEBUG_GRAF   yLOG_char    ("current"   , p->on);
   x_on = p->on;
   if        (strcmp (a_opt, "hide"   ) == 0) {
      p->on  = '-';
      x_good = 'y';
   } else if (strcmp (a_opt, "show"   ) == 0) {
      p->on  = 'y';
      x_good = 'y';
   } else if (strcmp (a_opt, "enable" ) == 0) {
      p->on  = 'y';
      x_good = 'y';
   } else if (strcmp (a_opt, "disable") == 0) {
      p->on  = 'X';
      x_good = 'y';
   } else {
      for (i = 0; i < s_noption; ++i) {
         if (s_options [i].part == NULL)                   break;
         if (s_options [i].part != p->abbr)       continue;
         if (s_options [i].opt  [0] != a_opt  [0])         continue;
         if (strcmp (s_options [i].opt , a_opt ) != NULL)  continue;
         p->on     = 'y';
         p->source = s_options [i].source;
         x_good = 'y';
         break;
      }
   }
   DEBUG_GRAF   yLOG_char    ("x_good"    , x_good);
   --rce;  if (x_good != 'y') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("new"       , p->on);
   DEBUG_GRAF   yLOG_point   ("source"    , p->source);
   /*---(handle linkages)----------------*/
   yvikeys_sizes__linked     (p);
   /*---(resize)-------------------------*/
   if (x_on != p->on) {
      DEBUG_GRAF   yLOG_note    ("must resize");
      yvikeys_sizes_resize ('r');
      myVIKEYS.redraw = 'y';
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sizes_layout     (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        n           =   -1;
   int         x_len       =    0;
   uchar       x_ch        =    0;
   uchar       x_key       =    0;
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_PROG   yLOG_complex ("origs"     , "%3dw, %3dt, %3da", s_orig_wide, s_orig_tall, s_alt_wide);
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
   yvikeys_sizes_defaults  (myVIKEYS.env);
   /*---(set parts)----------------------*/
   x_len = strlen (s_layouts [n].parts);
   for (i = 0; i < x_len; ++i) {
      x_ch  = s_layouts [n].parts [i];
      x_key = s_layouts [0].parts [i];
      if (x_key == ' ' || x_key == '-')   continue;
      yvikeys_view_pointer (x_key, &p, NULL);
      if (p == NULL)                      continue;
      if (x_ch == x_key && p->on != 'X')  p->on = 'y';
      if (x_ch == ' ' || x_ch == '-')     p->on = '-';
   }
   /*---(reset)--------------------------*/
   yvikeys_sizes_resize ('r');
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}





