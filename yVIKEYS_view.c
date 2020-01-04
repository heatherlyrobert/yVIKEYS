/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*===[[ DESIGN NOTES ]]=======================================================*/
/*
 * definition
 *
 * situation
 *
 * challenge
 *
 * response
 *
 * known thine user, and tailor accordingly
 *
 * history -- applications were purely keyboard-driven, unforgiving, required
 * extensive training, but were extremely productive for users.  users were
 * employees, computer time was expensive, speed and accuracy was paramount.
 * keystroke focused, zero menus, zero buttons, zero mice, manuals.
 *
 * trend -- as expected, sales volume generates profit so the valuable user
 * base is the non-technical, novice, casual user.  users are typically not
 * even computer literate and a simple, carefree user experience is paramount.
 * visually oriented, iconbars and ribbons, simple functionality.
 *
 * skinning -- some applications have retained their deep capabilities but
 * "skinned" themselves with buttons and ribbons for more casual users.  this
 * is the best of both, but it is rare and typically a labor of love.
 *
 *
 * there are two ways of getting things done in yVIKEYS -- keys and commands.
 * keys and modes are used for all common activities.  commands are used for
 * complex requests.
 *
 * menus are a commmand classification system that allows a user to "look up"
 * a command through a standard system.  no menu item should exist without a
 * command or key to back it up.  menus are a front-end only.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/*===[[ METIS BACKLOG ]]======================================================*
 * metis  dn2x·  create a masking config for a callback -- needed for float, history, menu
 *
 */


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

#define      OWN_COMPLEX      "mso"
#define      OWN_MODERATE     "msp"
#define      OWN_SIMPLE       "po"

#define      MAX_PARTS         30
static tPARTS  s_parts [MAX_PARTS] = {
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_TITLE   , "title"       , OWN_FULLY  , 'y',    1, '-', '-', 0, 0, 0,    1, '-', '-', '-', 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "left hand title bar"                                },
   { YVIKEYS_VERSION , "version"     , OWN_FULLY  , 'y',    1, '/', '-', 0, 0, 0,    9, '-', 't', '-', 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "version display with debugging notice"              },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_BUFFER  , "buffer"      , OWN_FULLY  , '-',    2, '-', '-', 0, 0, 0,    7, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "buffer inventory at top"                            },
   { YVIKEYS_FORMULA , "formula"     , OWN_FULLY  , '-',    2, '-', '-', 0, 0, 0,    6, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "formula and source editing line at top"             },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_NAV     , "nav"         , OWN_PARTLY , '-',    2, '-', '-', 0, 0, 0,    3, '-', '-', 'y', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "navigation panel to display tags and other links"   },
   { YVIKEYS_LAYERS  , "layers"      , OWN_PARTLY , '-',    2, '-', '-', 0, 0, 0,    3, '-', '-', 'y', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "displays all selected laysers over working"         },
   { YVIKEYS_YAXIS   , "yaxis"       , OWN_PARTLY , '-',    3, '-', '-', 0, 0, 0,    4, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the y-axis labels left of the main screen"     },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_XAXIS   , "xaxis"       , OWN_PARTLY , '-',    3, '/', '-', 0, 0, 0,    5, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the x-axis labels above the main screen"       },
   { YVIKEYS_ALT     , "alt"         , OWN_LITTLE , '-',    5, '-', '-', 0, 0, 0,    4, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "alternate working area or view"                     },
   { YVIKEYS_PROGRESS, "progress"    , OWN_PARTLY , '-',    3, '/', '-', 0, 0, 0,    3, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "time and sequencing controls about status line"     },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_DETAILS , "details"     , OWN_PARTLY , '-',    6, '-', '-', 0, 0, 0,    3, '-', '-', 'y', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "display area for critical details to right"         },
   { YVIKEYS_RIBBON  , "ribbon"      , OWN_FULLY  , '-',    7, '-', '-', 0, 0, 0,    3, '-', '-', 'y', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "menu and icon display for navigation of commands"   },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_MODES   , "modes"       , OWN_FULLY  , 'y',    2, '/', '-', 0, 0, 0,    2, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational to display current mode/submode"      },
   { YVIKEYS_STATUS  , "status"      , OWN_FULLY  , 'y',    9, '-', 'x', 0, 0, 0,    2, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational status bar above command line"        },
   { YVIKEYS_COMMAND , "command"     , OWN_FULLY  , 'y',    2, '-', '-', 0, 0, 0,    1, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "command, search, and help message line at bottom"   },
   { YVIKEYS_KEYS    , "keys"        , OWN_FULLY  , 'y',    9, '/', 'c', 0, 0, 0,    1, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "latest keyboard characters typed"                   },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_GRID    , "grid"        , OWN_OVERLAY, '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "overlay main drawing with a grid"                   },
   { YVIKEYS_CURSOR  , "cursor"      , OWN_OVERLAY, '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "cursor display on screen"                           },
   { YVIKEYS_OVERLAY , "overlay"     , OWN_OVERLAY, '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_FLOAT   , "float"       , OWN_OVERLAY, '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },
   { YVIKEYS_HISTORY , "history"     , OWN_OVERLAY, '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "list of command/search history"                     },
   { YVIKEYS_MENUS   , "menus"       , OWN_OVERLAY, 'y',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "interactive menu overlay"                           },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { YVIKEYS_MAIN    , "main"        , OWN_MAIN   , 'y',    4, '-', '-', 0, 0, 0,    4, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "main working area in the middle"                    },
   { YVIKEYS_WINDOW  , "window"      , OWN_DATA   , '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "full screen width and height"                       },
   /*---abbr---------   ---name-----   ---own-----  on   horz--nox--tie-dw-wi-le  vert--noy--tie--und-dt-ta-bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  */
   { 0               , ""            , 0          , '-',    0, '-', '-', 0, 0, 0,    0, '-', '-', '-', 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  ""                                                   },
};




/*> static tPARTS  s_parts [MAX_PARTS] = {                                                                                                                                                                                                                                               <* 
 *>    /+---abbr---------   ---name-----   ---own-----  on    bt lr dw dt  x_t  x_t  y_t  y_t  und  wi ta le bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  +/   <* 
 *>    { YVIKEYS_TITLE   , "title"       , OWN_FULLY  , 'y',   1, 1, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "left hand title bar"                                },   <* 
 *>    { YVIKEYS_BUFFER  , "buffer"      , OWN_FULLY  , '-',   7, 2, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "buffer inventory at top"                            },   <* 
 *>    { YVIKEYS_FORMULA , "formula"     , OWN_FULLY  , '-',   6, 2, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "formula and source editing line at top"             },   <* 
 *>    { YVIKEYS_NAV     , "nav"         , OWN_PARTLY , '-',   3, 2, 0, 0, '-', '-', '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "navigation panel to display tags and other links"   },   <* 
 *>    { YVIKEYS_ALT     , "alt"         , OWN_LITTLE , '-',   4, 5, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "alternate working area or view"                     },   <* 
 *>    { YVIKEYS_PROGRESS, "progress"    , OWN_PARTLY , '-',   3, 3, 0, 0, '/', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "time and sequencing controls about status line"     },   <* 
 *>    { YVIKEYS_MODES   , "modes"       , OWN_FULLY  , 'y',   2, 2, 0, 0, '/', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational to display current mode/submode"      },   <* 
 *>    { YVIKEYS_STATUS  , "status"      , OWN_FULLY  , 'y',   2, 9, 0, 0, '-', 'x', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "informational status bar above command line"        },   <* 
 *>    { YVIKEYS_COMMAND , "command"     , OWN_FULLY  , 'y',   1, 2, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "command, search, and help message line at bottom"   },   <* 
 *>    { YVIKEYS_DETAILS , "details"     , OWN_PARTLY , '-',   3, 6, 0, 0, '-', '-', '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "display area for critical details to right"         },   <* 
 *>    { YVIKEYS_RIBBON  , "ribbon"      , OWN_FULLY  , '-',   3, 7, 0, 0, '-', '-', '-', '-', 'y',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "menu and icon display for navigation of commands"   },   <* 
 *>    { YVIKEYS_VERSION , "version"     , OWN_FULLY  , 'y',   9, 1, 0, 0, '/', '-', '-', 't', '-',  0, 0, 0, 0,  'r', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTRIG, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "version display with debugging notice"              },   <* 
 *>    { YVIKEYS_KEYS    , "keys"        , OWN_FULLY  , 'y',   1, 9, 0, 0, '/', 'c', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "latest keyboard characters typed"                   },   <* 
 *>    { YVIKEYS_XAXIS   , "xaxis"       , OWN_PARTLY , '-',   5, 3, 0, 0, '/', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the x-axis labels above the main screen"       },   <* 
 *>    { YVIKEYS_YAXIS   , "yaxis"       , OWN_PARTLY , '-',   4, 3, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, 0              , 0, 0, 0, 0, 0, 0,  "show the y-axis labels left of the main screen"     },   <* 
 *>    /+---abbr---------   ---name-----   ---own-----  on    bt lr dw dt  x_t  x_t  y_t  y_t  und  wi ta le bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  +/   <* 
 *>    { YVIKEYS_GRID    , "grid"        , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "overlay main drawing with a grid"                   },   <* 
 *>    { YVIKEYS_CURSOR  , "cursor"      , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "cursor display on screen"                           },   <* 
 *>    { YVIKEYS_OVERLAY , "overlay"     , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },   <* 
 *>    { YVIKEYS_LAYERS  , "layers"      , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "displays all selected laysers over working"         },   <* 
 *>    { YVIKEYS_FLOAT   , "float"       , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_BOTLEF, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "shown over the working screen"                      },   <* 
 *>    { YVIKEYS_HISTORY , "history"     , OWN_OVERLAY, '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "list of command/search history"                     },   <* 
 *>    { YVIKEYS_MENUS   , "menus"       , OWN_OVERLAY, 'y',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_TOPLEF, YCOLOR_CLEAR   , 0, 0, 0, 0, 0, 0,  "interactive menu overlay"                           },   <* 
 *>    /+---abbr---------   ---name-----   ---own-----  on    bt lr dw dt  x_t  x_t  y_t  y_t  und  wi ta le bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  +/   <* 
 *>    { YVIKEYS_WINDOW  , "window"      , OWN_DATA   , '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  "full screen width and height"                       },   <* 
 *>    { YVIKEYS_MAIN    , "main"        , OWN_MAIN   , 'y',   4, 4, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , YVIKEYS_MIDCEN, YCOLOR_GRY     , 0, 0, 0, 0, 0, 0,  "main working area in the middle"                    },   <* 
 *>    /+---abbr---------   ---name-----   ---own-----  on    bt lr dw dt  x_t  x_t  y_t  y_t  und  wi ta le bo   ori  source, txt  drawer  type---------  ---mgmt-------  ---anchor-----  ---color------- xm xl ym yl zm zl    12345678901234567890123456789012345678901234567890  +/   <* 
 *>    { 0               , ""            , 0          , '-',   0, 0, 0, 0, '-', '-', '-', '-', '-',  0, 0, 0, 0,  '-', NULL  , "",  NULL  , YVIKEYS_FLAT , YVIKEYS_AUTO  , 0             , 0              , 0, 0, 0, 0, 0, 0,  ""                                                   },   <* 
 *> };                                                                                                                                                                                                                                                                                   <*/
static int  s_npart     = MAX_PARTS;

/*  lef/rig1----- 2----- 3----- 4----- 5----- 6----- 7-----
 * top/bot
 *     7   vers-- buffer---------------------------------->
 *     6   ´----´ formula--------------------------------->
 *     5   ¨    ¨ ´----´ xaxis--------------> ´----´ ´----´
 *     4   ¨    ¨ ¨    ¨ yaxis´ MAIN-- alt--- ¨    ¨ ¨    ¨
 *     3   ¨    ¨ nav--´ progress------------ detail ribbon
 *     2   ¨    ¨ modes- status--------------------------->
 *     1   title´ command--------------------------> keys->
 *
 *     0 in either means do not consider/show
 *     9 means tied to another element, handle width/height special
 *
 */




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
      strlcpy (s_parts [n].text, "", LEN_FULL);
      s_parts [n].type   = YVIKEYS_FLAT;
      s_parts [n].color  = YCOLOR_GRY;
      s_parts [n].xmin   = s_parts [n].xlen   = 0;
      s_parts [n].ymin   = s_parts [n].ylen   = 0;
      s_parts [n].ymin   = s_parts [n].ylen   = 0;
   }
   yvikeys_mode__update  ();  /* this need quick contents/replacement */
   DEBUG_GRAF_M yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> locate screen parts by abbrev ------[ ------ [gz.742.001.01]*/ /*-[00.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_view__abbr      (cchar  a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        n           =   -1;
   static char x_abbr      =  '-';
   static char x_save      =  -10;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF   yLOG_sint    (a_abbr);
   /*---(short-cut)----------------------*/
   if (a_abbr == x_abbr) {
      DEBUG_GRAF   yLOG_snote   ("short-cut using saved");
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return x_save;
   }
   /*---(defense)------------------------*/
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
   DEBUG_GRAF   yLOG_sint    (n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_snote   (s_parts [n].name);
   /*---(save)---------------------------*/
   x_abbr = a_abbr;
   x_save = n;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return n;
}

char
yvikeys_view_init        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(screen parts)-------------------*/
   s_npart = 0;
   for (i = 0; i < MAX_PARTS; ++i) {
      if (s_parts [i].abbr == 0)  break;
      DEBUG_PROG   yLOG_info    ("name"      , s_parts [i].name);
      ++s_npart;
   }
   DEBUG_PROG   yLOG_value   ("s_npart"   , s_npart);
   /*---(ribbon)-------------------------*/
   for (i = 0; i < LEN_LABEL; ++i)  s_win.r_icons [i] = -1;
   s_win.r_nicon = 0;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      window sizing                           ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char
yvikeys_view_defs       (char a_part, char a_on, int a_nwide, int a_ntall, int a_owide, int a_otall, void *a_draw)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   tPARTS     *p           = NULL;
   /*---(prepare)------------------------*/
   n = yvikeys_view__abbr (a_part);
   --rce;  if (n < 0)  return rce;
   p = &(s_parts [n]);
   /*---(set defaults)-------------------*/
   p->on         = a_on;
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      p->def_wide   = a_nwide;
      p->def_tall   = a_ntall;
   } else {
      p->def_wide   = a_owide;
      p->def_tall   = a_otall;
   }
   if (a_draw != NULL && p->drawer == NULL)  p->drawer     = a_draw;
   /*---(initialize)---------------------*/
   p->wide       = 0;
   p->left       = 0;
   p->tall       = 0;
   p->bott       = 0;
   /*---(output)-------------------------*/
   DEBUG_GRAF   yLOG_complex ("part"      , "%-12.12s, on %c, wide %3d, tall %3d, draw %-10.10p", p->name, p->on, p->def_wide, p->def_tall, p->drawer);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_view_cursor     (char a_move, tPARTS **a_part, tPARTS **a_link)
{
   /*---(locals)-----------+-----+-----+-*/
   char         rce        =  -10;
   static char  n          =   -1;
   char         a          =    0;
   int          i          =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF   yLOG_schar   (a_move);
   DEBUG_GRAF   yLOG_sint    (s_npart);
   DEBUG_GRAF   yLOG_sint    (n);
   /*---(prepare)------------------------*/
   if (a_part != NULL)  *a_part = NULL;
   if (a_link != NULL)  *a_link = NULL;
   /*---(update position)----------------*/
   --rce;  switch (a_move) {
   case '[' : n =  0;           break;
   case '<' : --n;              break;
   case '>' : ++n;              break;
   case ']' : n = s_npart - 1;  break;
   default  :
      DEBUG_GRAF   yLOG_snote   ("illegal move");
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_sint    (n);
   /*---(check for trouble)--------------*/
   --rce;  if (n >= s_npart)  {
      n = s_npart - 1;
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (n <  0)        {
      n = 0;
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save primary)-------------------*/
   if (a_part != NULL) {
      *a_part = &(s_parts [n]);
      DEBUG_GRAF   yLOG_spoint  (*a_part);
   }
   /*---(check for link)-----------------*/
   if (a_link == NULL) {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (s_parts [n].x_tie == '-' && s_parts [n].y_tie == '-') {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(find link)----------------------*/
   for (i = 0; i < s_npart; ++i) {
      if (s_parts [i].abbr == s_parts [n].x_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
      if (s_parts [i].abbr == s_parts [n].y_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
   }
   DEBUG_GRAF   yLOG_spoint  (*a_link);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_view_pointer    (cchar a_abbr, tPARTS **a_part, tPARTS **a_link)
{
   /*---(locals)-----------+-----+-----+-*/
   char         rce        =  -10;
   char         n          =   -1;
   int          i          =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF   yLOG_schar   (a_abbr);
   /*---(prepare)------------------------*/
   if (a_part != NULL)  *a_part = NULL;
   if (a_link != NULL)  *a_link = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_abbr <= G_KEY_SPACE || a_abbr >= G_KEY_TILDA) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(find the screen part)-----------*/
   for (i = 0; i < s_npart; ++i) {
      if (s_parts [i].abbr     != a_abbr)             continue;
      n = i;
      break;
   }
   DEBUG_GRAF   yLOG_sint    (n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for trouble)--------------*/
   --rce;  if (n >= s_npart)  {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save primary)-------------------*/
   if (a_part != NULL) {
      *a_part = &(s_parts [n]);
      DEBUG_GRAF   yLOG_spoint  (*a_part);
   }
   /*---(check for link)-----------------*/
   if (a_link == NULL) {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (s_parts [n].x_tie == '-' && s_parts [n].y_tie == '-') {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(find link)----------------------*/
   for (i = 0; i < s_npart; ++i) {
      if (s_parts [i].abbr == s_parts [n].x_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
      if (s_parts [i].abbr == s_parts [n].y_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
   }
   DEBUG_GRAF   yLOG_spoint  (*a_link);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_view_find       (char *a_name, tPARTS **a_part, tPARTS **a_link)
{
   /*---(locals)-----------+-----+-----+-*/
   char         rce        =  -10;
   char         n          =   -1;
   int          i          =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF   yLOG_spoint  (a_name);
   /*---(prepare)------------------------*/
   if (a_part != NULL)  *a_part = NULL;
   if (a_link != NULL)  *a_link = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(find the screen part)-----------*/
   for (i = 0; i < s_npart; ++i) {
      if (s_parts [i].name [0] != a_name [0])         continue;
      if (strcmp (s_parts [i].name, a_name) != 0)     continue;
      n = i;
      break;
   }
   DEBUG_GRAF   yLOG_sint    (n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for trouble)--------------*/
   --rce;  if (n >= s_npart)  {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save primary)-------------------*/
   if (a_part != NULL) {
      *a_part = &(s_parts [n]);
      DEBUG_GRAF   yLOG_spoint  (*a_part);
   }
   /*---(check for link)-----------------*/
   if (a_link == NULL) {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (s_parts [n].x_tie == '-' && s_parts [n].y_tie == '-') {
      DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(find link)----------------------*/
   for (i = 0; i < s_npart; ++i) {
      if (s_parts [i].abbr == s_parts [n].x_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
      if (s_parts [i].abbr == s_parts [n].y_tie)  {
         *a_link = &(s_parts [i]);
         break;
      }
   }
   DEBUG_GRAF   yLOG_spoint  (*a_link);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_view__sizer     (char a_part, int a_wide, int a_tall, int a_left, int a_bott)
{
   char        rce         =  -10;
   int         n           =    0;
   tPARTS     *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yvikeys_view__abbr (a_part);
   --rce;  if (n < 0)  return rce;
   p = &(s_parts [n]);
   if (a_wide >= 0)  p->wide       = a_wide;
   if (a_tall >= 0)  p->tall       = a_tall;
   if (a_left >= 0)  p->left       = a_left;
   if (a_bott >= 0)  p->bott       = a_bott;
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
yvikeys_view_init_opengl(char *a_title, int a_wide, int a_tall)
{
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(start window)----------------*/
   yX11_start (a_title, a_wide, a_tall, YX_FOCUSABLE, YX_FIXED, '-');
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
   /*> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "palette"     , ""    , "isss" , yCOLOR_palette             , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "wheel"       , ""    , "s"    , yCOLOR_wheel               , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "degree"      , "deg" , "i"    , yCOLOR_deg                 , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "harmony"     , "har" , "s"    , yCOLOR_harm                , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "value"       , "val" , "s"    , yCOLOR_val                 , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "saturation"  , "sat" , "s"    , yCOLOR_sat                 , "" );   <*/
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_view_init_curses       (int a_wide, int a_tall)
{
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(setup ncurses)---------------*/
   initscr  ();     /* fire up ncurses with a default screen (stdscr)         */
   raw      ();     /* read key-by-key rather than waiting for \n (raw mode)  */
   noecho   ();     /* don't automatically echo keypresses to the screen      */
   ESCDELAY = 0;    /* so escape responds immediately                         */
   nodelay  (stdscr, TRUE);
   getmaxyx (stdscr, a_wide, a_tall);
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
   DEBUG_PROG   yLOG_complex ("args"      , "%-10.10p, %-10.10p, %c wide %3d tall %3d alt %3d", a_title, a_ver, a_env, a_wide, a_tall, a_alt);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (FMOD_VIEW)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set defaults)-------------------*/
   VIEW__purge ();
   yvikeys_sizes_defaults (a_env);
   /*---(commands)-----------------------*/
   /*> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "gridoff"     , ""    , "iii"  , VIEW__grid_offset          , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "gridsize"    , ""    , "iii"  , VIEW__grid_size            , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "layout"      , ""    , "s"    , VIEW__layout               , "" );   <* 
    *> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "layer"       , ""    , "s"    , VIEW__layer_set            , "" );   <*/
   /*---(options)------------------------*/
   /*> yVIKEYS_view_optionX (YVIKEYS_GRID, "norm"  , VIEW__grid_normal, "traditional cross-hatch grid");   <* 
    *> yVIKEYS_view_optionX (YVIKEYS_GRID, "zoom"  , VIEW__grid_zoom  , "zoom/targeting grid"         );   <*/
   /*---(open window)--------------------*/
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :
      yvikeys_view_init_opengl (a_title, a_wide, a_tall);
      break;
   case YVIKEYS_CURSES :  
      yvikeys_view_init_curses (a_wide, a_tall);
      break;
   }
   yVIKEYS_resize (a_wide, a_tall, a_alt);
   myVIKEYS.font       =  0;
   myVIKEYS.point      =  9;
   myVIKEYS.font_scale = 0.0;
   yvikeys_sizes_layout ("work");
   yvikeys_sizes_switch ("status", "empty");
   /*---(set text data)------------------*/
   if (a_title != NULL) {
      n = yvikeys_view__abbr (YVIKEYS_TITLE);
      strlcpy (s_parts [n].text, a_title, LEN_FULL  );
   }
   if (a_ver   != NULL) {
      n = yvikeys_view__abbr (YVIKEYS_VERSION);
      strlcpy (s_parts [n].text, a_ver  , LEN_FULL  );
   }
   n = yvikeys_view__abbr (YVIKEYS_COMMAND);
   MODE_message ();
   /*---(update status)------------------*/
   STATUS_init_set       (FMOD_VIEW);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yVIKEYS_run_curses    (cchar *a_title, cchar *a_ver) { return yVIKEYS_view_config (a_title, a_ver, YVIKEYS_CURSES, 0, 0, 0); }
char yVIKEYS_run_opengl    (cchar *a_title, cchar *a_ver, cint a_wide, cint a_tall) { return yVIKEYS_view_config (a_title, a_ver, YVIKEYS_OPENGL, a_wide, a_tall, 0); }

char
yVIKEYS_view_simple        (cchar a_part, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = yvikeys_view__abbr (a_part);
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
yVIKEYS_view_basic         (cchar a_part, cchar a_type, cchar a_anchor, cchar a_color, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(identify part)------------------*/
   DEBUG_PROG   yLOG_char    ("a_part"    , a_part);
   n = yvikeys_view__abbr (a_part);
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
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_PROG   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_PROG   yLOG_value   ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_PROG   yLOG_value   ("anchor"    , s_parts [n].anchor);
   yvikeys_sizes_anchor (n);
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
   n = yvikeys_view__abbr (a_part);
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
   yvikeys_sizes_resize ('r');
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
   n = yvikeys_view__abbr (YVIKEYS_TITLE   );
   s_parts [n].color = a_lef;
   n = yvikeys_view__abbr (YVIKEYS_VERSION );
   s_parts [n].color = a_lef + 5;
   /*---(bottom)-------------------------*/
   n = yvikeys_view__abbr (YVIKEYS_COMMAND );
   s_parts [n].color = a_bot;
   n = yvikeys_view__abbr (YVIKEYS_MODES   );
   s_parts [n].color = a_top;
   n = yvikeys_view__abbr (YVIKEYS_STATUS  );
   s_parts [n].color = a_bot + 5;
   n = yvikeys_view__abbr (YVIKEYS_KEYS    );
   s_parts [n].color = a_top;
   n = yvikeys_view__abbr (YVIKEYS_XAXIS   );
   s_parts [n].color = a_bot + 5;
   n = yvikeys_view__abbr (YVIKEYS_YAXIS   );
   s_parts [n].color = a_bot + 5;
   /*---(top)----------------------------*/
   n = yvikeys_view__abbr (YVIKEYS_BUFFER  );
   s_parts [n].color = a_top;
   n = yvikeys_view__abbr (YVIKEYS_FORMULA );
   s_parts [n].color = a_top + 5;
   /*---(right)--------------------------*/
   n = yvikeys_view__abbr (YVIKEYS_RIBBON  );
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
   n = yvikeys_view__abbr (a_part);
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
   else if (a_type == YVIKEYS_DEPTH)   s_parts [n].type = YVIKEYS_DEPTH;
   else                                s_parts [n].type = YVIKEYS_FLAT;
   DEBUG_PROG   yLOG_char    ("type"      , s_parts [n].type);
   /*---(anchor)-------------------------*/
   DEBUG_PROG   yLOG_value   ("a_anchor"  , a_anchor);
   if      (a_anchor <  0  )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else if (a_anchor >= 10 )  s_parts [n].anchor = YVIKEYS_MIDCEN;
   else                       s_parts [n].anchor = a_anchor;
   DEBUG_PROG   yLOG_value   ("anchor"    , s_parts [n].anchor);
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
yVIKEYS_view_size       (cchar a_part, int *a_left, int *a_wide, int *a_bott, int *a_tall, cchar *a_text)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_part"    , a_part);
   n = yvikeys_view__abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      if (a_left != NULL)  *a_left  = 0;
      if (a_bott != NULL)  *a_bott  = 0;
      if (a_wide != NULL)  *a_wide  = 0;
      if (a_tall != NULL)  *a_tall  = 0;
      if (a_text != NULL)  strlcpy (a_text, ""          , LEN_DESC);
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_note    ("save values for good entry");
   if (a_left != NULL)  *a_left  = s_parts [n].left;
   if (a_bott != NULL)  *a_bott  = s_parts [n].bott;
   if (a_wide != NULL)  *a_wide  = s_parts [n].wide;
   if (a_tall != NULL)  *a_tall  = s_parts [n].tall;
   if (a_text != NULL)  strlcpy (a_text, s_parts [n].text, LEN_FULL);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return s_parts [n].on;
}

char
yVIKEYS_view_showing    (cchar a_part)
{
   char        x_status    =  '-';
   x_status = yVIKEYS_view_size (a_part, NULL, NULL, NULL, NULL, NULL);
   if (x_status <   0 )  return -1;
   if (x_status == 'y')  return  1;
   return 0;
}

char
yVIKEYS_view_text       (cchar a_part, cchar *a_text)
{
   char        rc          =     0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   rc =  yVIKEYS_view_size   (a_part, NULL, NULL, NULL, NULL, a_text);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_view_keys       (cchar *a_text)
{
   char        rce         =  -10;
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yvikeys_view__abbr (YVIKEYS_KEYS );
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   strlcpy (s_parts [n].text, a_text, LEN_LABEL);
   DEBUG_GRAF   yLOG_info    ("text"      , s_parts [n].text);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_view_modes      (cchar *a_text)
{
   char        rce         =  -10;
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yvikeys_view__abbr (YVIKEYS_MODES);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   strlcpy (s_parts [n].text, a_text, LEN_LABEL);
   DEBUG_GRAF   yLOG_info    ("text"      , s_parts [n].text);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_view_bounds        (cchar a_part, int *a_xmin, int *a_xmax, int *a_xlen, int *a_ymin, int *a_ymax, int *a_ylen)
{
   char        n           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   n = yvikeys_view__abbr (a_part);
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_note    ("deal with a missing element");
      if (a_xmin != NULL)  *a_xmin  = 0;
      if (a_xmax != NULL)  *a_xmax  = 0;
      if (a_xlen != NULL)  *a_xlen  = 0;
      if (a_ymin != NULL)  *a_ymin  = 0;
      if (a_ymax != NULL)  *a_ymax  = 0;
      if (a_ylen != NULL)  *a_ylen  = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return n;
   }
   DEBUG_GRAF   yLOG_note    ("save values for good entry");
   if (a_xmin != NULL)  *a_xmin  = s_parts [n].xmin;
   if (a_xmax != NULL)  *a_xmax  = s_parts [n].xmin + s_parts [n].xlen;
   if (a_xlen != NULL)  *a_xlen  = s_parts [n].xlen;
   if (a_ymin != NULL)  *a_ymin  = s_parts [n].ymin;
   if (a_ymax != NULL)  *a_ymax  = s_parts [n].ymin + s_parts [n].ylen;
   if (a_ylen != NULL)  *a_ylen  = s_parts [n].ylen;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return s_parts [n].anchor;
}

char
yVIKEYS_view_anchor     (cchar a_part)
{
   return yVIKEYS_view_bounds (a_part, NULL, NULL, NULL, NULL, NULL, NULL);
}

char
yVIKEYS_view_type       (cchar a_part)
{
   char        n           =    0;
   n = yvikeys_view__abbr (a_part);
   if (n < 0)  return ' ';
   return s_parts [n].type;
}

char
yvikeys_view_reanchor   (cchar a_part, cint a_anchor)
{
   char        n           =    0;
   if (a_anchor < 1 || a_anchor > 9)  return -1;
   n = yvikeys_view__abbr (a_part);
   if (n < 0)  return -2;
   s_parts [n].anchor = a_anchor;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        standard drawing                      ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

char          /*----: draw the saved status ----------------------------------*/
VIEW__cursor             (void)
{
   int         x_lef   = g_xmap.ucur / s_mag;
   int         x_bot   = g_ymap.ucur / s_mag;
   int         x_rig   = x_lef + (g_gsizex / s_mag);
   int         x_top   = x_bot + (g_gsizey / s_mag);
   char        n           = 0;
   /*---(defense)------------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   n = yvikeys_view__abbr (YVIKEYS_CURSOR);
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
   n = yvikeys_view__abbr (YVIKEYS_GRID);
   if (s_parts [n].on == '-')  return 0;
   /*---(prepare)------------------------*/
   yVIKEYS_view_bounds  (YVIKEYS_MAIN, &x_lef, NULL, &x_wide, &y_bot, NULL, &y_tall);
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
   n = yvikeys_view__abbr (YVIKEYS_GRID);
   if (s_parts [n].on == '-')  return 0;
   /*---(prepare)------------------------*/
   yVIKEYS_view_bounds  (YVIKEYS_MAIN, &x_lef, NULL, &x_wide, &y_bot, NULL, &y_tall);
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
   n    = yvikeys_view__abbr   (YVIKEYS_RIBBON);
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

char
VIEW_status_default  (char *a_list)
{
   snprintf (a_list, LEN_FULL, "  ");
   return 0;
}

yvikeys_view__opengl     (char n)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_max;
   int         y_max;
   int         z_max;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex (s_parts [n].name, "%2d, %c, bott %4d, left %4d, wide %4d, tall %4d, on %c", n, s_parts [n].type, s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   /*---(prepare)------------------------*/
   x_max = s_parts [n].xmin + s_parts [n].xlen;
   y_max = s_parts [n].ymin + s_parts [n].ylen;
   z_max = s_parts [n].zmin + s_parts [n].zlen;
   /*---(setup view)---------------------*/
   DEBUG_GRAF   yLOG_note    ("set up the view");
   glViewport      (s_parts [n].left, s_parts [n].bott, s_parts [n].wide, s_parts [n].tall);
   glMatrixMode    (GL_PROJECTION);
   glLoadIdentity  ();
   if (s_parts [n].type  == YVIKEYS_FLAT )  glOrtho         (s_parts [n].xmin, x_max, s_parts [n].ymin, y_max, s_parts [n].zmin, z_max);
   else                                     gluPerspective  (45.0f, (GLfloat) s_parts [n].wide / (GLfloat) s_parts [n].tall, 0.01f, 4000.0f);
   glMatrixMode    (GL_MODELVIEW);
   /*---(background)---------------------*/
   if (s_parts [n].type  == YVIKEYS_FLAT ) {
      DEBUG_GRAF   yLOG_note    ("draw a background for ortho/flat");
      glPushMatrix    (); {
         yVIKEYS_view_color (s_parts [n].color, 1.0);
         if (s_parts [n].abbr == YVIKEYS_VERSION && yURG_debugmode () == 'y')
            glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
         if (s_parts [n].abbr == YVIKEYS_STATUS  && yVIKEYS_error  ())
            glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
         if (s_parts [n].abbr == YVIKEYS_MODES   && yvikeys_macro_emode () == MACRO_RUN)
            glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
         glBegin         (GL_POLYGON); {
            glVertex3f  (s_parts [n].xmin, y_max           , -100.0f);
            glVertex3f  (x_max           , y_max           , -100.0f);
            glVertex3f  (x_max           , s_parts [n].ymin, -100.0f);
            glVertex3f  (s_parts [n].xmin, s_parts [n].ymin, -100.0f);
         } glEnd   ();
      } glPopMatrix   ();
   }
   /*---(update text)--------------------*/
   DEBUG_GRAF   yLOG_point   ("source"    , s_parts [n].source);
   if (s_parts [n].source != NULL)  {
      DEBUG_GRAF   yLOG_note    ("draw a background for ortho/flat");
      s_parts [n].source (s_parts [n].text);
   }
   /*---(display text)-------------------*/
   DEBUG_GRAF   yLOG_point   ("text"      , s_parts [n].text);
   if (s_parts [n].text != NULL && strlen (s_parts [n].text) > 0) {
      DEBUG_GRAF   yLOG_info    ("text"      , s_parts [n].text);
      glPushMatrix    (); {
         if (s_parts [n].orient == 'r') {
            glTranslatef (-1.0f, 5.0f, 0.0f);
            glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
         } else {
            glTranslatef ( 3.0f, 1.0f, 0.0f);
         }
         yVIKEYS_view_color_adj (s_parts [n].color, YCOLOR_MIN, 0.8);
         yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, s_parts [n].text);
      } glPopMatrix   ();
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return n;
}

yvikeys_view__curses     (char n)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_len       =    0;
   int         x_beg       =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex (s_parts [n].name, "%2d, %c, bott %4d, left %4d, wide %4d, tall %4d, on %c", n, s_parts [n].type, s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   /*---(update text)--------------------*/
   DEBUG_GRAF   yLOG_point   ("source"    , s_parts [n].source);
   if (s_parts [n].source != NULL)  {
      DEBUG_GRAF   yLOG_note    ("draw a background for ortho/flat");
      s_parts [n].source (s_parts [n].text);
   }
   /*---(display text)-------------------*/
   DEBUG_GRAF   yLOG_point   ("text"      , s_parts [n].text);
   if (s_parts [n].text != NULL && strlen (s_parts [n].text) > 0) {
      DEBUG_GRAF   yLOG_info    ("text"      , s_parts [n].text);
      /*---(set color)---------*/
      switch (s_parts [n].abbr) {
      case YVIKEYS_TITLE    :
      case YVIKEYS_VERSION  :
         yCOLOR_curs ("status" );
         break;
      case YVIKEYS_COMMAND  :
         yCOLOR_curs ("command" );
         break;
      case YVIKEYS_MODES    :
         if (yvikeys_macro_emode () == MACRO_RUN)  yCOLOR_curs ("warn" );
         else                                      yCOLOR_curs ("root" );
         break;
      case YVIKEYS_KEYS     :
         yCOLOR_curs ("root" );
         break;
      case YVIKEYS_STATUS   :
         if (yVIKEYS_error ())  yCOLOR_curs ("warn"    );
         else                   yCOLOR_curs ("status"  );
         break;
      }
      /*---(display)-----------*/
      x_len = strlen (s_parts [n].text);
      x_beg = (s_parts [n].tall - x_len) / 2;
      if (s_parts [n].orient == 'r') {
         for (i = 0; i < s_parts [n].tall; ++i) {
            mvprintw (s_parts [n].bott - i, s_parts [n].left, "%s", " ");
         }
         for (i = 0; i < x_len; ++i) {
            mvprintw (s_parts [n].bott - i - x_beg, s_parts [n].left, "%c", s_parts [n].text [x_len - i - 1]);
         }
      } else {
         mvprintw (s_parts [n].bott, s_parts [n].left, "%-*.*s", s_parts [n].wide, s_parts [n].wide, s_parts [n].text);
      }
      attrset  (0);
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return n;
}

char
yvikeys_view__prepare    (char n)
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
   DEBUG_GRAF   yLOG_info    ("name"      , s_parts [n].name);
   /*---(prepare)------------------------*/
   x_max = s_parts [n].xmin + s_parts [n].xlen;
   y_max = s_parts [n].ymin + s_parts [n].ylen;
   z_max = s_parts [n].zmin + s_parts [n].zlen;
   if (s_parts [n].source != NULL)  {
      s_parts [n].source (s_parts [n].text);
      /*> printf ("source %p, s_parts [n].txt %s\n", s_parts [n].source, s_parts [n].text);   <*/
   }
   /*---(setup view)---------------------*/
   DEBUG_GRAF   yLOG_complex (s_parts [n].name, "%2d, bott %4d, left %4d, wide %4d, tall %4d, on %c", n, s_parts [n].bott, s_parts [n].left, s_parts [n].wide, s_parts [n].tall, s_parts [n].on);
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      glViewport      (s_parts [n].left, s_parts [n].bott, s_parts [n].wide, s_parts [n].tall);
      glMatrixMode    (GL_PROJECTION);
      glLoadIdentity  ();
      if (s_parts [n].type  == YVIKEYS_FLAT )  glOrtho         (s_parts [n].xmin, x_max, s_parts [n].ymin, y_max, s_parts [n].zmin, z_max);
      else                                     gluPerspective  (45.0f, (GLfloat) s_parts [n].wide / (GLfloat) s_parts [n].tall, 0.01f, 4000.0f);
      glMatrixMode    (GL_MODELVIEW);
      /*---(background)---------------------*/
      if (s_parts [n].type  == YVIKEYS_FLAT ) {
         DEBUG_GRAF   yLOG_note    ("draw background");
         glPushMatrix    (); {
            yVIKEYS_view_color (s_parts [n].color, 1.0);
            if (s_parts [n].abbr == YVIKEYS_VERSION && yURG_debugmode () == 'y')
               glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
            if (s_parts [n].abbr == YVIKEYS_STATUS  && yVIKEYS_error  ())
               glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
            if (s_parts [n].abbr == YVIKEYS_MODES   && yvikeys_macro_emode () == MACRO_RUN)
               glColor4f    (1.00f, 0.00f, 0.00f, 1.0f);
            glBegin         (GL_POLYGON); {
               glVertex3f  (s_parts [n].xmin, y_max           , -100.0f);
               glVertex3f  (x_max           , y_max           , -100.0f);
               glVertex3f  (x_max           , s_parts [n].ymin, -100.0f);
               glVertex3f  (s_parts [n].xmin, s_parts [n].ymin, -100.0f);
            } glEnd   ();
         } glPopMatrix   ();
      }
   } else if (myVIKEYS.env == YVIKEYS_CURSES) {
      ;;  /* not sure if i need to clear yet  */
   }
   /*---(display text)-------------------*/
   if (s_parts [n].text != NULL && strlen (s_parts [n].text) > 0) {
      DEBUG_GRAF   yLOG_note    ("draw text");
      if (myVIKEYS.env == YVIKEYS_OPENGL) {
         glPushMatrix    (); {
            if (s_parts [n].orient == 'r') {
               glTranslatef (-1.0f, 5.0f, 0.0f);
               glRotatef    ( 90.0, 0.0f, 0.0f, 1.0f);
            } else {
               glTranslatef ( 3.0f, 1.0f, 0.0f);
            }
            yVIKEYS_view_color_adj (s_parts [n].color, YCOLOR_MIN, 0.8);
            yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, s_parts [n].text);
         } glPopMatrix   ();
      } else if (myVIKEYS.env == YVIKEYS_CURSES) {
         /*  set color --------------*/
         switch (s_parts [n].abbr) {
         case YVIKEYS_TITLE    :
         case YVIKEYS_VERSION  :
            yCOLOR_curs ("status" );
            break;
         case YVIKEYS_COMMAND  :
            yCOLOR_curs ("command" );
            break;
         case YVIKEYS_MODES    :
            if (yvikeys_macro_emode () == MACRO_RUN)  yCOLOR_curs ("warn" );
            else                                      yCOLOR_curs ("root" );
            break;
         case YVIKEYS_KEYS     :
            yCOLOR_curs ("root" );
            break;
         case YVIKEYS_STATUS   :
            if (yVIKEYS_error ())  yCOLOR_curs ("warn"    );
            else                   yCOLOR_curs ("status"  );
            break;
         }
         x_len = strlen (s_parts [n].text);
         x_beg = (s_parts [n].tall - x_len) / 2;
         if (s_parts [n].orient == 'r') {
            for (i = 0; i < s_parts [n].tall; ++i) {
               mvprintw (s_parts [n].bott - i, s_parts [n].left, "%s", " ");
            }
            for (i = 0; i < x_len; ++i) {
               mvprintw (s_parts [n].bott - i - x_beg, s_parts [n].left, "%c", s_parts [n].text [x_len - i - 1]);
            }
         } else {
            mvprintw (s_parts [n].bott, s_parts [n].left, "%-*.*s", s_parts [n].wide, s_parts [n].wide, s_parts [n].text);
         }
         attrset  (0);
         /*> mvprintw (s_parts [n].bott, s_parts [n].left, "%-*.*s", s_parts [n].wide, s_parts [n].wide, s_parts [n].name);   <*/
         /*  clear color ------------*/
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return n;
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
      /*---(filter)------------*/
      DEBUG_GRAF   yLOG_complex ("part"      , "%2d, %-12.12s, %c, %c, %c, %-10.10p", n, s_parts [n].name, s_parts [n].own, s_parts [n].on, s_parts [n].type, s_parts [n].drawer);
      if (strchr (OWN_SETUP, s_parts [n].own) == NULL)  continue;
      if (s_parts [n].on   != 'y')                      continue;
      /*---(draw)--------------*/
      if      (myVIKEYS.env == YVIKEYS_OPENGL)  yvikeys_view__opengl (n);
      else if (myVIKEYS.env == YVIKEYS_CURSES)  yvikeys_view__curses (n);
      if (s_parts [n].drawer != NULL)  s_parts [n].drawer ();
      /*---(done)--------------*/
   }
   if (myVIKEYS.env == YVIKEYS_CURSES)  getyx (stdscr, y_cur, x_cur);
   /*---(on top of main)-----------------*/
   DEBUG_GRAF   yLOG_note    ("overlay screen elements");
   for (n = 0; n < s_npart; ++n) {
      /*---(filter)------------*/
      DEBUG_GRAF   yLOG_complex ("part"      , "%2d, %-12.12s, %c, %c, %c, %-10.10p", n, s_parts [n].name, s_parts [n].own, s_parts [n].on, s_parts [n].type, s_parts [n].drawer);
      if (s_parts [n].own    != OWN_OVERLAY)              continue;
      if (s_parts [n].on     != 'y')                      continue;
      /*---(draw)--------------*/
      if (s_parts [n].drawer != NULL)  s_parts [n].drawer ();
      if (s_parts [n].source != NULL)  s_parts [n].source (NULL);
      /*---(done)--------------*/
   }
   /*---(flush)--------------------------*/
   DEBUG_GRAF   yLOG_note    ("flush and show");
   switch (myVIKEYS.env) {
   case YVIKEYS_OPENGL :
      glXSwapBuffers (YX_DISP, YX_BASE);
      glFlush();
      break;
   case YVIKEYS_CURSES :
      if (myVIKEYS.cursor == 'y')  mvprintw (y_cur, x_cur, "");
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
   strlcpy  (yVIKEYS__unit_answer, "VIEW unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "size"           )   == 0) {
      n    = yvikeys_view__abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VIEW %-12.12s: on %c, left %4d, wide %4d, bott %4d, tall %4d", s_parts [n].name, s_parts [n].on, s_parts [n].left, s_parts [n].wide, s_parts [n].bott, s_parts [n].tall);
   }
   else if (strcmp (a_question, "bounds"         )   == 0) {
      n    = yvikeys_view__abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VIEW %-12.12s: an %d, xmin %4d, xmax %4d, ymin %4d, ymax %4d", s_parts [n].name, s_parts [n].anchor, s_parts [n].xmin, s_parts [n].xmin + s_parts [n].xlen, s_parts [n].ymin, s_parts [n].ymin + s_parts [n].ylen);
   }
   if      (strcmp (a_question, "active"         )   == 0) {
      n    = yvikeys_view__abbr   (a_index);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VIEW active      : %-12.12s, on %c", s_parts [n].name, s_parts [n].on);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


