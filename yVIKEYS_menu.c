/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



/*============================---- METIS TASKS ---============================*/
/*
 *
 * metis  tn4#·  add function to alter level two menu items type and keys
 * metis  dn4#·  add function to deactivate a menu tree and children
 * metis  dn4#·  add function to activate a menu tree and children
 * metis  dn4#·  add function to hide a menu tree and children
 * metis  tw1#·  suffix menus keys which require further input with =
 * metis  dw1··  look at linear menu search instead of my indexed way
 * metis  wl1··  assign unique greek letters to unassigned menu items (to index them)
 * metis  wl2··  build menu search for each level to get at unassigned options
 * metis  ww2··  create a vim-like update to only write files that are changed
 *
 *
 */

/*> ,x··46|··dw··2l3clMENU_GREY  ¥··3l··.··0j··,y                                     <*/
/*> ,x··74|··3clMENU_EXACT¥··n··,y                                                    <*/
/*> ,x··/ACTIVE, '·', "¦··,y                                                          <*/

/*> convert menu_add to structure entry ::  ,x··$·xx·a },¥··0·3l·dw·3dl·i{ MENU_BASE, '¥··2l·i', '¥··2l·i', '¥··2l·r'··f,f,·i  , MENU_ACTIVE, MENU_ACTIVE, MENU_CMD  ¥··0·dw·i   ¥··$·2h·i                   ¥··106|·dw·i, 0, 0, 0¥··j0··,y   <*/



#define     MENU_BASE        'b'  /* fully standard in base           */
#define     MENU_HELD        'h'  /* base saved name but not keys     */
#define     MENU_CUSTOM      'c'  /* fully custom, not in base        */

#define     MENU_PLACE       'p'  /* put new menu in heirarchy        */
#define     MENU_FIND        'f'  /* find entry in heirarchy          */
#define     MENU_MENU        'm'  /* find entry for menu              */
/*> #define     MENU_DELETE      'd'  /+ delete entry from heirarchy      +/          <*/
#define     MENU_ACTIVE      'y'  /* mark entry active in heirarchyi  */
#define     MENU_GREY        '-'  /* mark entry inactive in heirarchy */
#define     MENU_HIDE        '/'  /* hide entry in heirarchy          */

#define     MENU_GROUP       '>'  /* group type entry                 */
#define     MENU_NAMER       '!'  /* end item name/place holder       */
#define     MENU_EXACT       '·'  /* end item with all keys needed    */
#define     MENU_CMD         ':'  /* end item command with all keys   */
#define     MENU_MORE        '='  /* end item needing more keys       */
#define     MENU_TBD         '?'  /* end item not yet completed       */

static const uchar  *s_valid     = "·*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-_?!#,.èéêëìíîïðñòóôõö÷øùúûüýþÿ";



/*===[[ MENUS ]]==============================================================*/
#define    LEN_NAME        11
#define    MAX_MENU        1000
typedef    struct   cMENU   tMENU;
struct cMENU {
   /*---(original)----------*/
   uchar       base;                        /* original item (not added)      */
   /*---(organize)----------*/
   uchar       top;                         /* first level key                */
   uchar       mid;                         /* second level key               */
   uchar       bot;                         /* third level key                */
   /*---(master)------------*/
   uchar       name        [LEN_NAME];      /* name for use on screens        */
   uchar       active;                      /* program activation             */
   uchar       type;                        /* entry type   > ! = ·           */
   uchar       keys        [LEN_TITLE];     /* command or keys to execute     */
   /*---(done)--------------*/
};

static const tMENU  s_base [] = {
   /*---(root)---------------------------------------------------*/
   { 'b', '·', '·', '·', "ROOT"        , 'y', '>', "-"                              },
   /*---(file menu)----------------------------------------------*/
   { 'b', 'f', '·', '·', "files"       , 'y', '>', "-"                              },
   { 'b', 'f', 'W', '·', "workspace"   , '-', '>', "-"                              },
   { 'b', 'f', 'w', '·', "new"         , 'y', ':', ":new¦"                          },
   { 'b', 'f', 'c', '·', "chdir"       , 'y', '=', ":cd·"                           },
   { 'b', 'f', 'b', '·', "browse"      , 'y', '=', ":browse·"                       },
   { 'b', 'f', 'n', '·', "name"        , 'y', '=', ":file·"                         },
   { 'b', 'f', 'o', '·', "read"        , 'y', ':', ":read¦"                         },
   { 'b', 'f', 'm', '·', "import"      , '-', '>', "-"                              },
   { 'b', 'f', 'f', '·', "refresh"     , 'y', ':', ":refresh¦"                      },
   { 'b', 'f', 'r', '·', "restore"     , '-', '?', "-"                              },
   { 'b', 'f', 'v', '·', "version"     , 'y', '>', "-"                              },
   { 'b', 'f', 'v', 'c', "ctrl"        , 'y', ':', ":control¦"                      },
   { 'b', 'f', 'v', 'n', "noctrl"      , 'y', ':', ":nocontrol¦"                    },
   { 'b', 'f', 'v', '1', "major"       , 'y', ':', ":major¦"                        },
   { 'b', 'f', 'v', '2', "minor"       , 'y', ':', ":minor¦"                        },
   { 'b', 'f', 'v', '3', "bump"        , 'y', ':', ":bump¦"                         },
   { 'b', 'f', 'v', 't', "vertxt"      , 'y', '=', ":vertxt·"                       },
   { 'b', 'f', 'v', 'm', "manual"      , 'y', '=', ":vernum·"                       },
   { 'b', 'f', 'p', '·', "protect"     , '-', '>', "-"                              },
   { 'b', 'f', 'u', '·', "update"      , 'y', '?', "-"                              },
   { 'b', 'f', 's', '·', "write"       , 'y', ':', ":write¦"                        },
   { 'b', 'f', 'a', '·', "writeas"     , 'y', '=', ":writeas·"                      },
   { 'b', 'f', 'x', '·', "export"      , '-', '>', "-"                              },
   { 'b', 'f', 'q', '·', "quit"        , 'y', ':', ":quit¦"                         },
   /*---(edit menu)----------------------------------------------*/
   { 'b', 'e', '·', '·', "edit"        , '-', '>', "-"                              },
   { 'b', 'e', 'u', '·', "undo"        , '-', '·', "u"                              },
   { 'b', 'e', 'U', '·', "redo"        , '-', '·', "U"                              },
   { 'b', 'e', '_', '·', "status"      , '-', '·', ":status mundo¦"                 },
   { 'b', 'e', '?', '·', "history"     , '-', '>', "-"                              },
   { 'b', 'e', '?', 't', "track"       , '-', '·', "-"                              },
   { 'b', 'e', '?', 'u', "untrack"     , '-', '·', "-"                              },
   { 'b', 'e', '?', 'c', "clear"       , '-', '·', "-"                              },
   { 'b', 'e', '?', 'r', "review"      , '-', '·', "-"                              },
   { 'b', 'e', 'y', '·', "copy"        , '-', '·', "y"                              },
   { 'b', 'e', 'Y', '·', "cut"         , '-', '·', "Y"                              },
   { 'b', 'e', 'p', '·', "paste"       , '-', '·', "p"                              },
   { 'b', 'e', 'P', '·', "special"     , '-', '>', "-"                              },
   { 'b', 'e', 'x', '·', "clear"       , '-', '>', "-"                              },
   { 'b', 'e', 'x', '.', "inplace"     , '-', '·', "x."                             },
   { 'b', 'e', 'x', 'h', "left"        , '-', '·', "xh"                             },
   { 'b', 'e', 'x', 'l', "right"       , '-', '·', "xl"                             },
   { 'b', 'e', 'x', 'k', "up"          , '-', '·', "xk"                             },
   { 'b', 'e', 'x', 'j', "down"        , '-', '·', "xj"                             },
   { 'b', 'e', 'x', 'i', "in"          , '-', '·', "xi"                             },
   { 'b', 'e', 'x', 'o', "out"         , '-', '·', "xo"                             },
   { 'b', 'e', 'x', 'x', "col_lef"     , '-', '·', "xx"                             },
   { 'b', 'e', 'x', 'X', "col_rig"     , '-', '·', "xX"                             },
   { 'b', 'e', 'x', 'y', "row_abo"     , '-', '·', "xy"                             },
   { 'b', 'e', 'x', 'Y', "row_bel"     , '-', '·', "xY"                             },
   { 'b', 'e', 'x', 'z', "lvl_in"      , '-', '·', "xz"                             },
   { 'b', 'e', 'x', 'Z', "lvl_out"     , '-', '·', "xZ"                             },
   { 'b', 'e', 'd', '·', "delete"      , '-', '>', "-"                              },
   { 'b', 'e', 'd', 'l', "left"        , '-', '·', "dl"                             },
   { 'b', 'e', 'd', 'h', "right"       , '-', '·', "dh"                             },
   { 'b', 'e', 'd', 'k', "up"          , '-', '·', "dk"                             },
   { 'b', 'e', 'd', 'j', "down"        , '-', '·', "dj"                             },
   { 'b', 'e', 'd', 'i', "in"          , '-', '·', "di"                             },
   { 'b', 'e', 'd', 'o', "out"         , '-', '·', "do"                             },
   { 'b', 'e', 'd', 'x', "col_left"    , '-', '·', "dx"                             },
   { 'b', 'e', 'd', 'X', "col_right"   , '-', '·', "dX"                             },
   { 'b', 'e', 'd', 'y', "row_above"   , '-', '·', "dy"                             },
   { 'b', 'e', 'd', 'Y', "row_below"   , '-', '·', "dY"                             },
   { 'b', 'e', 'd', 'z', "lvl_in"      , '-', '·', "dz"                             },
   { 'b', 'e', 'd', 'Z', "lvl_out"     , '-', '·', "dZ"                             },
   { 'b', 'e', 'i', '·', "insert"      , '-', '>', "-"                              },
   { 'b', 'e', 'i', 'l', "left"        , '-', '·', "al"                             },
   { 'b', 'e', 'i', 'h', "right"       , '-', '·', "ah"                             },
   { 'b', 'e', 'i', 'k', "up"          , '-', '·', "ak"                             },
   { 'b', 'e', 'i', 'j', "down"        , '-', '·', "aj"                             },
   { 'b', 'e', 'i', 'i', "in"          , '-', '·', "ai"                             },
   { 'b', 'e', 'i', 'o', "out"         , '-', '·', "ao"                             },
   { 'b', 'e', 'i', 'x', "col_lef"     , '-', '·', "ax"                             },
   { 'b', 'e', 'i', 'X', "col_rig"     , '-', '·', "aX"                             },
   { 'b', 'e', 'i', 'y', "row_abo"     , '-', '·', "ay"                             },
   { 'b', 'e', 'i', 'Y', "row_bel"     , '-', '·', "aY"                             },
   { 'b', 'e', 'i', 'z', "lvl_in"      , '-', '·', "az"                             },
   { 'b', 'e', 'i', 'Z', "lvl_out"     , '-', '·', "aZ"                             },
   { 'b', 'e', 'g', '·', "group"       , '-', '>', "-"                              },
   { 'b', 'e', 'g', 'g', "group"       , '-', '·', "-"                              },
   { 'b', 'e', 'g', 'u', "ungroup"     , '-', '·', "-"                              },
   { 'b', 'e', 'g', 'r', "regroup"     , '-', '·', "-"                              },
   { 'b', 'e', 'g', 'm', "merge"       , '-', '·', "-"                              },
   { 'b', 'e', 'g', 'f', "free"        , '-', '·', "-"                              },
   { 'b', 'e', 'g', 'b', "boundary"    , '-', '·', "-"                              },
   { 'b', 'e', 'a', '·', "align"       , '-', '>', "-"                              },
   { 'b', 'e', 'a', 'k', "top"         , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'j', "bottom"      , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'h', "left"        , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'l', "right"       , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'o', "front"       , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'i', "back"        , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'x', "dist-x"      , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'y', "dist-y"      , '-', '·', "-"                              },
   { 'b', 'e', 'a', 'z', "dist-z"      , '-', '·', "-"                              },
   { 'b', 'e', 'o', '·', "order"       , '-', '>', "-"                              },
   { 'b', 'e', 'o', 'n', "front"       , '-', '·', "-"                              },
   { 'b', 'e', 'o', 'i', "forward"     , '-', '·', "-"                              },
   { 'b', 'e', 'o', 'o', "backward"    , '-', '·', "-"                              },
   { 'b', 'e', 'o', 'f', "back"        , '-', '·', "-"                              },
   { 'b', 'e', 'o', 'r', "reverse"     , '-', '·', "-"                              },
   { 'b', 'e', 'l', '·', "layer"       , '-', '=', "-"                              },
   /*---(view menu)-----------------------------------------------------------*/
   { 'b', 'v', '·', '·', "view"        , 'y', '>', "-"                              },
   { 'b', 'v', 'w', '·', "window"      , 'y', '>', "-"                              },
   { 'b', 'v', 'l', '·', "layout"      , 'y', '>', "-"                              },
   { 'b', 'v', 'l', 'n', "minimal"     , 'y', ':', ":layout···min¦"                 },
   { 'b', 'v', 'l', 'n', "work"        , 'y', ':', ":layout···work¦"                },
   { 'b', 'v', 'l', 'n', "gyges"       , 'y', ':', ":layout···gyges¦"               },
   { 'b', 'v', 'l', 'x', "maximum"     , 'y', ':', ":layout···max¦"                 },
   { 'b', 'v', 't', '·', "title"       , 'y', '>', "-"                              },
   { 'b', 'v', 't', 's', "show"        , 'y', ':', ":title····show¦"                },
   { 'b', 'v', 't', 'h', "hide"        , 'y', ':', ":title····hide¦"                },
   { 'b', 'v', 't', 'e', "enable"      , 'y', ':', ":title····enable¦"              },
   { 'b', 'v', 't', 'd', "disable"     , 'y', ':', ":title····disable¦"             },
   { 'b', 'v', 'v', '·', "version"     , 'y', '>', "-"                              },
   { 'b', 'v', 'v', 's', "show"        , 'y', ':', ":version··show¦"                },
   { 'b', 'v', 'v', 'h', "hide"        , 'y', ':', ":version··hide¦"                },
   { 'b', 'v', 'v', 'e', "enable"      , 'y', ':', ":version··enable¦"              },
   { 'b', 'v', 'v', 'd', "disable"     , 'y', ':', ":version··disable¦"             },
   { 'b', 'v', 'b', '·', "buffer"      , 'y', '>', "-"                              },
   { 'b', 'v', 'b', 's', "show"        , 'y', ':', ":buffer···show¦"                },
   { 'b', 'v', 'b', 'h', "hide"        , 'y', ':', ":buffer···hide¦"                },
   { 'b', 'v', 'b', 'e', "enable"      , 'y', ':', ":buffer···enable¦"              },
   { 'b', 'v', 'b', 'd', "disable"     , 'y', ':', ":buffer···disable¦"             },
   { 'b', 'v', 'f', '·', "formula"     , 'y', '>', "-"                              },
   { 'b', 'v', 'f', 's', "show"        , 'y', ':', ":formula··show¦"                },
   { 'b', 'v', 'f', 'h', "hide"        , 'y', ':', ":formula··hide¦"                },
   { 'b', 'v', 'f', 'e', "enable"      , 'y', ':', ":formula··enable¦"              },
   { 'b', 'v', 'f', 'd', "disable"     , 'y', ':', ":formula··disable¦"             },
   { 'b', 'v', 'n', '·', "xaxis"       , 'y', '>', "-"                              },
   { 'b', 'v', 'n', 's', "show"        , 'y', ':', ":nav······show¦"                },
   { 'b', 'v', 'n', 'h', "hide"        , 'y', ':', ":nav······hide¦"                },
   { 'b', 'v', 'n', 'e', "enable"      , 'y', ':', ":nav······enable¦"              },
   { 'b', 'v', 'n', 'd', "disable"     , 'y', ':', ":nav······disable¦"             },
   { 'b', 'v', 'X', '·', "xaxis"       , 'y', '>', "-"                              },
   { 'b', 'v', 'X', 's', "show"        , 'y', ':', ":xaxis····show¦"                },
   { 'b', 'v', 'X', 'h', "hide"        , 'y', ':', ":xaxis····hide¦"                },
   { 'b', 'v', 'X', 'e', "enable"      , 'y', ':', ":xaxis····enable¦"              },
   { 'b', 'v', 'X', 'd', "disable"     , 'y', ':', ":xaxis····disable¦"             },
   { 'b', 'v', 'Y', '·', "yaxis"       , 'y', '>', "-"                              },
   { 'b', 'v', 'Y', 's', "show"        , 'y', ':', ":yaxis····show¦"                },
   { 'b', 'v', 'Y', 'h', "hide"        , 'y', ':', ":yaxis····hide¦"                },
   { 'b', 'v', 'Y', 'e', "enable"      , 'y', ':', ":yaxis····enable¦"              },
   { 'b', 'v', 'Y', 'd', "disable"     , 'y', ':', ":yaxis····disable¦"             },
   { 'b', 'v', 'a', '·', "alt"         , 'y', '>', "-"                              },
   { 'b', 'v', 'a', 's', "show"        , 'y', ':', ":alt······show¦"                },
   { 'b', 'v', 'a', 'h', "hide"        , 'y', ':', ":alt······hide¦"                },
   { 'b', 'v', 'a', 'e', "enable"      , 'y', ':', ":alt······enable¦"              },
   { 'b', 'v', 'a', 'd', "disable"     , 'y', ':', ":alt······disable¦"             },
   { 'b', 'v', 'd', '·', "details"     , 'y', '>', "-"                              },
   { 'b', 'v', 'd', 's', "show"        , 'y', ':', ":details··show¦"                },
   { 'b', 'v', 'd', 'h', "hide"        , 'y', ':', ":details··hide¦"                },
   { 'b', 'v', 'd', 'e', "enable"      , 'y', ':', ":details··enable¦"              },
   { 'b', 'v', 'd', 'd', "disable"     , 'y', ':', ":details··disable¦"             },
   { 'b', 'v', 'r', '·', "ribbon"      , 'y', '>', "-"                              },
   { 'b', 'v', 'r', 's', "show"        , 'y', ':', ":ribbon···show¦"                },
   { 'b', 'v', 'r', 'h', "hide"        , 'y', ':', ":ribbon···hide¦"                },
   { 'b', 'v', 'r', 'e', "enable"      , 'y', ':', ":ribbon···enable¦"              },
   { 'b', 'v', 'r', 'd', "disable"     , 'y', ':', ":ribbon···disable¦"             },
   { 'b', 'v', 'p', '·', "progress"    , 'y', '>', "-"                              },
   { 'b', 'v', 'p', 's', "show"        , 'y', ':', ":progress·show¦"                },
   { 'b', 'v', 'p', 'h', "hide"        , 'y', ':', ":progress·hide¦"                },
   { 'b', 'v', 'p', 'e', "enable"      , 'y', ':', ":progress·enable¦"              },
   { 'b', 'v', 'p', 'd', "disable"     , 'y', ':', ":progress·disable¦"             },
   { 'b', 'v', 'x', '·', "modes"       , 'y', '>', "-"                              },
   { 'b', 'v', 'x', 's', "show"        , 'y', ':', ":modes····show¦"                },
   { 'b', 'v', 'x', 'h', "hide"        , 'y', ':', ":modes····hide¦"                },
   { 'b', 'v', 'x', 'e', "enable"      , 'y', ':', ":modes····enable¦"              },
   { 'b', 'v', 'x', 'd', "disable"     , 'y', ':', ":modes····disable¦"             },
   { 'b', 'v', 's', '·', "status"      , 'y', '>', "-"                              },
   { 'b', 'v', 's', 's', "show"        , 'y', ':', ":status···show¦"                },
   { 'b', 'v', 's', 'h', "hide"        , 'y', ':', ":status···hide¦"                },
   { 'b', 'v', 's', 'e', "enable"      , 'y', ':', ":status···enable¦"              },
   { 'b', 'v', 's', 'd', "disable"     , 'y', ':', ":status···disable¦"             },
   { 'b', 'v', 'c', '·', "command"     , 'y', '>', "-"                              },
   { 'b', 'v', 'c', 's', "show"        , 'y', ':', ":command··show¦"                },
   { 'b', 'v', 'c', 'h', "hide"        , 'y', ':', ":command··hide¦"                },
   { 'b', 'v', 'c', 'e', "enable"      , 'y', ':', ":command··enable¦"              },
   { 'b', 'v', 'c', 'd', "disable"     , 'y', ':', ":command··disable¦"             },
   { 'b', 'v', 'k', '·', "keys"        , 'y', '>', "-"                              },
   { 'b', 'v', 'k', 's', "show"        , 'y', ':', ":keys·····show¦"                },
   { 'b', 'v', 'k', 'h', "hide"        , 'y', ':', ":keys·····hide¦"                },
   { 'b', 'v', 'k', 'e', "enable"      , 'y', ':', ":keys·····enable¦"              },
   { 'b', 'v', 'k', 'd', "disable"     , 'y', ':', ":keys·····disable¦"             },
   { 'b', 'v', 'C', '·', "cursor"      , 'y', '>', "-"                              },
   { 'b', 'v', 'C', 's', "show"        , 'y', ':', ":cursor···show¦"                },
   { 'b', 'v', 'C', 'h', "hide"        , 'y', ':', ":cursor···hide¦"                },
   { 'b', 'v', 'C', 'e', "enable"      , 'y', ':', ":cursor···enable¦"              },
   { 'b', 'v', 'C', 'd', "disable"     , 'y', ':', ":cursor···disable¦"             },
   { 'b', 'v', 'G', '·', "grid"        , 'y', '>', "-"                              },
   { 'b', 'v', 'G', 's', "show"        , 'y', ':', ":grid·····show¦"                },
   { 'b', 'v', 'G', 'h', "hide"        , 'y', ':', ":grid·····hide¦"                },
   { 'b', 'v', 'G', 'e', "enable"      , 'y', ':', ":grid·····enable¦"              },
   { 'b', 'v', 'G', 'd', "disable"     , 'y', ':', ":grid·····disable¦"             },
   { 'b', 'v', 'E', '·', "edges"       , 'y', '>', "-"                              },
   { 'b', 'v', 'E', 's', "show"        , 'y', ':', ":edges····show¦"                },
   { 'b', 'v', 'E', 'h', "hide"        , 'y', ':', ":edges····hide¦"                },
   { 'b', 'v', 'E', 'e', "enable"      , 'y', ':', ":edges····enable¦"              },
   { 'b', 'v', 'E', 'd', "disable"     , 'y', ':', ":edges····disable¦"             },
   { 'b', 'v', 'U', '·', "guides"      , 'y', '>', "-"                              },
   { 'b', 'v', 'U', 's', "show"        , 'y', ':', ":guides···show¦"                },
   { 'b', 'v', 'U', 'h', "hide"        , 'y', ':', ":guides···hide¦"                },
   { 'b', 'v', 'U', 'e', "enable"      , 'y', ':', ":guides···enable¦"              },
   { 'b', 'v', 'U', 'd', "disable"     , 'y', ':', ":guides···disable¦"             },
   { 'b', 'v', 'O', '·', "overlay"     , 'y', '>', "-"                              },
   { 'b', 'v', 'O', 's', "show"        , 'y', ':', ":overlay··show¦"                },
   { 'b', 'v', 'O', 'h', "hide"        , 'y', ':', ":overlay··hide¦"                },
   { 'b', 'v', 'O', 'e', "enable"      , 'y', ':', ":overlay··enable¦"              },
   { 'b', 'v', 'O', 'd', "disable"     , 'y', ':', ":overlay··disable¦"             },
   { 'b', 'v', 'L', '·', "layers"      , 'y', '>', "-"                              },
   { 'b', 'v', 'L', 's', "show"        , 'y', ':', ":layers···show¦"                },
   { 'b', 'v', 'L', 'h', "hide"        , 'y', ':', ":layers···hide¦"                },
   { 'b', 'v', 'L', 'e', "enable"      , 'y', ':', ":layers···enable¦"              },
   { 'b', 'v', 'L', 'd', "disable"     , 'y', ':', ":layers···disable¦"             },
   { 'b', 'v', 'N', '·', "notes"       , 'y', '>', "-"                              },
   { 'b', 'v', 'N', 's', "show"        , 'y', ':', ":notes····show¦"                },
   { 'b', 'v', 'N', 'h', "hide"        , 'y', ':', ":notes····hide¦"                },
   { 'b', 'v', 'N', 'e', "enable"      , 'y', ':', ":notes····enable¦"              },
   { 'b', 'v', 'N', 'd', "disable"     , 'y', ':', ":notes····disable¦"             },
   { 'b', 'v', 'P', '·', "color"       , 'y', '>', "-"                              },
   { 'b', 'v', 'P', 's', "show"        , 'y', ':', ":color····show¦"                },
   { 'b', 'v', 'P', 'h', "hide"        , 'y', ':', ":color····hide¦"                },
   { 'b', 'v', 'P', 'e', "enable"      , 'y', ':', ":color····enable¦"              },
   { 'b', 'v', 'P', 'd', "disable"     , 'y', ':', ":color····disable¦"             },
   { 'b', 'v', 'B', '·', "back"        , 'y', '>', "-"                              },
   { 'b', 'v', 'B', 's', "show"        , 'y', ':', ":back·····show¦"                },
   { 'b', 'v', 'B', 'h', "hide"        , 'y', ':', ":back·····hide¦"                },
   { 'b', 'v', 'B', 'e', "enable"      , 'y', ':', ":back·····enable¦"              },
   { 'b', 'v', 'B', 'd', "disable"     , 'y', ':', ":back·····disable¦"             },
   { 'b', 'v', '?', '·', "help"        , 'y', '>', "-"                              },
   /*---(insert menu)--------------------------------------------*/
   { 'b', 'i', '·', '·', "insert"      , '-', '>', "-"                              },
   { 'b', 'i', 't', '·', "text"        , '-', '>', "-"                              },
   { 'b', 'i', 'i', '·', "image"       , '-', '>', "-"                              },
   { 'b', 'i', 'l', '·', "lines"       , '-', '>', "-"                              },
   { 'b', 'i', 'e', '·', "ellises"     , '-', '>', "-"                              },
   { 'b', 'i', 't', '·', "tris"        , '-', '>', "-"                              },
   { 'b', 'i', 'r', '·', "rects"       , '-', '>', "-"                              },
   { 'b', 'i', 'h', '·', "hexagons"    , '-', '>', "-"                              },
   { 'b', 'i', 'p', '·', "polygons"    , '-', '>', "-"                              },
   { 'b', 'i', 's', '·', "stars"       , '-', '>', "-"                              },
   { 'b', 'i', 'a', '·', "arrows"      , '-', '>', "-"                              },
   { 'b', 'i', 'f', '·', "flowchart"   , '-', '>', "-"                              },
   { 'b', 'i', 'j', '·', "joiners"     , '-', '>', "-"                              },
   { 'b', 'i', 'n', '·', "notes"       , '-', '>', "-"                              },
   { 'b', 'i', 'm', '·', "misc"        , '-', '>', "-"                              },
   { 'b', 'i', '-', '·', "nurbs"       , '-', '>', "-"                              },
   { 'b', 'i', '-', '·', "tiling"      , '-', '>', "-"                              },
   { 'b', 'i', 'b', '·', "beziers"     , '-', '>', "-"                              },
   { 'b', 'i', '-', '·', "3d"          , '-', '>', "-"                              },
   { 'b', 'i', '-', '·', "surfaces"    , '-', '>', "-"                              },
   { 'b', 'i', '-', '·', "meshes"      , '-', '>', "-"                              },
   /*---(select menu)--------------------------------------------*/
   { 'b', 's', '·', '·', "select"      , '-', '>', "-"                              },
   { 'b', 's', '!', '·', "highlight"   , '-', '·', "-"                              },
   { 'b', 's', '_', '·', "status"      , '-', '·', "-"                              },
   { 'b', 's', 'c', '·', "clear"       , '-', '·', "-"                              },
   { 'b', 's', 'a', '·', "all"         , '-', '·', "-"                              },
   { 'b', 's', 'b', '·', "buffer"      , '-', '·', "-"                              },
   { 'b', 's', 'w', '·', "window"      , '-', '·', "-"                              },
   { 'b', 's', 'l', '·', "layer"       , '-', '·', "-"                              },
   { 'b', 's', 'g', '·', "geometry"    , '-', '=', "-"                              },
   { 'b', 's', 't', '·', "type"        , '-', '=', "-"                              },
   { 'b', 's', 'x', '·', "regex"       , '-', '=', "-"                              },
   { 'b', 's', 'u', '·', "touching"    , '-', '·', "-"                              },
   { 'b', 's', 's', '·', "save"        , '-', '·', "-"                              },
   { 'b', 's', 'r', '·', "reselect"    , '-', '·', "-"                              },
   { 'b', 's', 'j', '·', "join"        , '-', '·', "-"                              },
   { 'b', 's', 'd', '·', "deselect"    , '-', '·', "-"                              },
   { 'b', 's', 'i', '·', "inverse"     , '-', '·', "-"                              },
   { 'b', 's', 'x', '·', "all_on_x"    , '-', '·', "-"                              },
   { 'b', 's', 'y', '·', "all_on_y"    , '-', '·', "-"                              },
   { 'b', 's', 'z', '·', "all_on_z"    , '-', '·', "-"                              },
   /*---(format menu)--------------------------------------------*/
   { 'b', 'o', '·', '·', "format"      , '-', '>', "-"                              },
   { 'b', 'o', 'f', '·', "face"        , '-', '>', "-"                              },
   { 'b', 'o', 'p', '·', "point"       , '-', '>', "-"                              },
   { 'b', 'o', 's', '·', "spacing"     , '-', '>', "-"                              },
   { 'b', 'o', 'i', '·', "indent"      , '-', '>', "-"                              },
   { 'b', 'o', 'c', '·', "color"       , '-', '>', "-"                              },
   { 'b', 'o', 'a', '·', "align"       , '-', '>', "-"                              },
   { 'b', 'o', 'n', '·', "numbers"     , '-', '>', "-"                              },
   { 'b', 'o', 'z', '·', "sfill"       , '-', '>', "-"                              },
   { 'b', 'o', 'd', '·', "decimals"    , '-', '>', "-"                              },
   { 'b', 'o', 'u', '·', "units"       , '-', '>', "-"                              },
   { 'b', 'o', 'l', '·', "lists"       , '-', '>', "-"                              },
   { 'b', 'o', 'y', '·', "style"       , '-', '>', "-"                              },
   /*---(modify menu)--------------------------------------------*/
   { 'b', 'm', '·', '·', "modify"      , '-', '>', "-"                              },
   { 'b', 'm', 'è', '·', "resize"      , '-', '=', "-"                              },
   { 'b', 'm', 'é', '·', "dims"        , '-', '=', "-"                              },
   { 'b', 'm', 'ê', '·', "extent"      , '-', '=', "-"                              },
   { 'b', 'm', 'ë', '·', "scale"       , '-', '=', "-"                              },
   { 'b', 'm', 'ì', '·', "trim"        , '-', '>', "-"                              },
   { 'b', 'm', 'í', '·', "join"        , '-', '>', "-"                              },
   { 'b', 'm', 'î', '·', "move"        , '-', '?', "-"                              },
   { 'b', 'm', 'ï', '·', "array"       , '-', '?', "-"                              },
   { 'b', 'm', 'ð', '·', "rotate"      , '-', '>', "-"                              },
   { 'b', 'm', 'ñ', '·', "snap"        , '-', '?', "-"                              },
   { 'b', 'm', 'ò', '·', "fill"        , '-', '>', "-"                              },
   { 'b', 'm', 'ó', '·', "hatching"    , '-', '>', "-"                              },
   { 'b', 'm', 'ô', '·', "outline"     , '-', '>', "-"                              },
   { 'b', 'm', 'õ', '·', "centers"     , '-', '?', "-"                              },
   { 'b', 'm', 'ö', '·', "ends"        , '-', '?', "-"                              },
   /*---(dataset menu)-------------------------------------------*/
   { 'b', 'd', '·', '·', "dataset"     , 'y', '>', "-"                              },
   { 'b', 'd', 's', '·', "sort"        , 'y', '>', "-"                              },
   { 'b', 'd', 'f', '·', "filter"      , 'y', '>', "-"                              },
   { 'b', 'd', '-', '·', "blur"        , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "enhance"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "distort"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "noise"       , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "edges"       , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "combine"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "light"       , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "shadow"      , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "pixelate"    , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "render"      , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "sharpen"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "smooth"      , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "stylize"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "liquify"     , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "oils"        , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "map"         , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "decor"       , 'y', '?', "-"                              },
   { 'b', 'd', '-', '·', "vanish"      , 'y', '?', "-"                              },
   /*---(tools menu)---------------------------------------------*/
   { 'b', 't', '·', '·', "tools"       , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "pen"         , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "callig"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "pencil"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "airbrush"    , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "chalk"       , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "brush"       , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "roller"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "spray"       , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "fill"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "sponge"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "blob"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "blur"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "sharpen"     , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "smudge"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "burn"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "eraser"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "magic_e"     , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "healer"      , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "highlight"   , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "zoom"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "move"        , '-', '>', "-"                              },
   { 'b', 't', '-', '·', "lasso"       , '-', '>', "-"                              },
   /*---(pallette menu)------------------------------------------*/
   { 'b', 'p', '·', '·', "palette"     , '-', '>', "-"                              },
   /*---(layers menu)--------------------------------------------*/
   { 'b', 'l', '·', '·', "layers"      , '-', '>', "-"                              },
   { 'b', 'l', '-', '·', "saved"       , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "highlight"   , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "status"      , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "new"         , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "rename"      , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "copy_all"    , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "copy_with"   , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "join"        , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "flatten"     , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "delete"      , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "type"        , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "hide"        , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "mask"        , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "alignment"   , '-', '?', "-"                              },
   { 'b', 'l', '-', '·', "locking"     , '-', '?', "-"                              },
   /*---(buffers menu)-------------------------------------------*/
   { 'b', 'b', '·', '·', "buffers"     , '-', '>', "-"                              },
   { 'b', 'b', '_', '·', "status"      , '-', '·', ":status buffer¦"                },
   { 'b', 'b', 'x', '·', "maximum"     , '-', '=', ":bmax·"                         },
   { 'b', 'b', 'b', '·', "browse"      , '-', '·', ":bbrowse·"                      },
   { 'b', 'b', 'g', '·', "goto"        , '-', '>', "-"                              },
   { 'b', 'b', 'g', 'a', "abbrev"      , '-', '=', ":buf·"                          },
   { 'b', 'b', 'g', 'r', "return"      , '-', '·', ",,"                             },
   { 'b', 'b', 'g', 'f', "first"       , '-', '·', ",["                             },
   { 'b', 'b', 'g', 'p', "prev"        , '-', '·', ",<"                             },
   { 'b', 'b', 'g', 'n', "next"        , '-', '·', ",>"                             },
   { 'b', 'b', 'g', 'l', "last"        , '-', '·', ",]"                             },
   { 'b', 'b', 'g', 'u', "summary"     , '-', '·', ",®"                             },
   { 'b', 'b', 'g', 'y', "system"      , '-', '·', ",¯"                             },
   { 'b', 'b', '-', '·', "showall"     , '-', '?', "-"                              },
   { 'b', 'b', 'w', '·', "new"         , '-', '?', "-"                              },
   { 'b', 'b', 'n', '·', "rename"      , '-', '=', ":btitle·"                       },
   { 'b', 'b', 'z', '·', "size"        , '-', '=', ":bsize·"                        },
   { 'b', 'b', 't', '·', "type"        , '-', '?', "-"                              },
   { 'b', 'b', 'f', '·', "defaults"    , '-', '>', "-"                              },
   { 'b', 'b', 'f', 'w', "col_wide"    , '-', '=', ":defwide·"                      },
   { 'b', 'b', 'f', 't', "row_tall"    , '-', '=', ":deftall·"                      },
   { 'b', 'b', 'f', 'd', "dep_thick"   , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "scale"       , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "delete"      , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "freeze"      , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "split"       , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "hiding"      , '-', '?', "-"                              },
   { 'b', 'b', '-', '·', "locking"     , '-', '?', "-"                              },
   /*---(snippet menu)-------------------------------------------*/
   { 'b', 'g', '·', '·', "language"    , '-', '>', "-"                              },
   /*---(auditor menu)-------------------------------------------*/
   { 'b', 'a', '·', '·', "auditor"     , '-', '>', "-"                              },
   { 'b', 'a', 's', '·', "spell"       , '-', '?', "-"                              },
   { 'b', 'a', 'd', '·', "dump"        , '-', '?', "-"                              },
   /*---(execute menu)-------------------------------------------*/
   { 'b', 'x', '·', '·', "execute"     , '-', '>', "-"                              },
   /*---(config menu)--------------------------------------------*/
   { 'b', 'c', '·', '·', "config"      , 'y', '>', "-"                              },
   { 'b', 'c', 's', '·', "search"      , 'y', '>', "-"                              },
   { 'b', 'c', 's', 'r', "reset all"   , 'y', '=', "-"                              },
   { 'b', 'c', 'M', '·', "menu"        , 'y', '>', "-"                              },
   { 'b', 'c', 'M', '1', "top-lef"     , 'y', ':', ":menu_loc···1¦"                 },
   { 'b', 'c', 'M', '2', "top-cen"     , 'y', ':', ":menu_loc···2¦"                 },
   { 'b', 'c', 'M', '3', "top-rig"     , 'y', ':', ":menu_loc···3¦"                 },
   { 'b', 'c', 'M', 'a', "ups-beg"     , 'y', ':', ":menu_loc···a¦"                 },
   { 'b', 'c', 'M', 't', "ups-cen"     , 'y', ':', ":menu_loc···t¦"                 },
   { 'b', 'c', 'M', 'u', "ups-rig"     , 'y', ':', ":menu_loc···u¦"                 },
   { 'b', 'c', 'M', '4', "mid-lef"     , 'y', ':', ":menu_loc···4¦"                 },
   { 'b', 'c', 'M', 's', "mid-beg"     , 'y', ':', ":menu_loc···s¦"                 },
   { 'b', 'c', 'M', '5', "mid-cen"     , 'y', ':', ":menu_loc···5¦"                 },
   { 'b', 'c', 'M', 'e', "mid-end"     , 'y', ':', ":menu_loc···e¦"                 },
   { 'b', 'c', 'M', '6', "mid-rig"     , 'y', ':', ":menu_loc···6¦"                 },
   { 'b', 'c', 'M', 'd', "low-beg"     , 'y', ':', ":menu_loc···d¦"                 },
   { 'b', 'c', 'M', 'b', "low-cen"     , 'y', ':', ":menu_loc···b¦"                 },
   { 'b', 'c', 'M', 'z', "low-end"     , 'y', ':', ":menu_loc···z¦"                 },
   { 'b', 'c', 'M', '7', "bot-lef"     , 'y', ':', ":menu_loc···7¦"                 },
   { 'b', 'c', 'M', '8', "bot-cen"     , 'y', ':', ":menu_loc···8¦"                 },
   { 'b', 'c', 'M', '9', "bot-rig"     , 'y', ':', ":menu_loc···9¦"                 },
   { 'b', 'c', 'F', '·', "float"       , 'y', '>', "-"                              },
   { 'b', 'c', 'F', 't', "top"         , 'y', ':', ":float_loc··t¦"                 },
   { 'b', 'c', 'F', 'k', "upper"       , 'y', ':', ":float_loc··k¦"                 },
   { 'b', 'c', 'F', 'm', "middle"      , 'y', ':', ":float_loc··m¦"                 },
   { 'b', 'c', 'F', 'j', "lower"       , 'y', ':', ":float_loc··j¦"                 },
   { 'b', 'c', 'F', 'b', "bottom"      , 'y', ':', ":float_loc··b¦"                 },
   { 'b', 'c', 'H', '·', "history"     , 'y', '>', "-"                              },
   { 'b', 'c', 'H', 'f', "full"        , 'y', ':', ":hist_loc···f¦"                 },
   { 'b', 'c', 'H', 'l', "left"        , 'y', ':', ":hist_loc···l¦"                 },
   { 'b', 'c', 'H', 'c', "center"      , 'y', ':', ":hist_loc···c¦"                 },
   { 'b', 'c', 'H', 'r', "right"       , 'y', ':', ":hist_loc···r¦"                 },
   /*---(macro menu)---------------------------------------------*/
   { 'b', 'r', '·', '·', "script"      , 'y', '>', "-"                              },
   { 'b', 'r', 'e', '·', "execute"     , 'y', '>', "-"                              },
   { 'b', 'r', 'e', '0', "blitz"       , 'y', ':', ":edelay 0¦"                     },
   { 'b', 'r', 'e', '1', "fast"        , 'y', ':', ":edelay 1¦"                     },
   { 'b', 'r', 'e', '2', "thousand"    , 'y', ':', ":edelay 2¦"                     },
   { 'b', 'r', 'e', '3', "hundred"     , 'y', ':', ":edelay 3¦"                     },
   { 'b', 'r', 'e', '4', "twenty"      , 'y', ':', ":edelay 4¦"                     },
   { 'b', 'r', 'e', '5', "tenth"       , 'y', ':', ":edelay 5¦"                     },
   { 'b', 'r', 'e', '6', "half"        , 'y', ':', ":edelay 6¦"                     },
   { 'b', 'r', 'e', '7', "second"      , 'y', ':', ":edelay 7¦"                     },
   { 'b', 'r', 'e', '8', "double"      , 'y', ':', ":edelay 8¦"                     },
   { 'b', 'r', 'e', '9', "triple"      , 'y', ':', ":edelay 9¦"                     },
   { 'b', 'r', 'e', '+', "faster"      , 'y', ':', ":edelay +¦"                     },
   { 'b', 'r', 'e', '-', "slower"      , 'y', ':', ":edelay -¦"                     },
   { 'b', 'r', 'e', 'n', "normal"      , 'y', ':', ":eupdate n¦"                    },
   { 'b', 'r', 'e', 's', "slow"        , 'y', ':', ":eupdate s¦"                    },
   { 'b', 'r', 'e', 'b', "blinks"      , 'y', ':', ":eupdate b¦"                    },
   { 'b', 'r', 'e', 'p', "peeks"       , 'y', ':', ":eupdate p¦"                    },
   { 'b', 'r', 'e', 'd', "blind"       , 'y', ':', ":eupdate d¦"                    },
   { 'b', 'r', 'e', 'D', "demo mode"   , 'y', ':', ":edelay 4¦"                     },
   { 'b', 'r', 'd', '·', "debug"       , 'y', '>', "-"                              },
   { 'b', 'r', 'd', '0', "blitz"       , 'y', ':', ":ddelay 0¦"                     },
   { 'b', 'r', 'd', '1', "fast"        , 'y', ':', ":ddelay 1¦"                     },
   { 'b', 'r', 'd', '2', "thousand"    , 'y', ':', ":ddelay 2¦"                     },
   { 'b', 'r', 'd', '3', "hundred"     , 'y', ':', ":ddelay 3¦"                     },
   { 'b', 'r', 'd', '4', "twenty"      , 'y', ':', ":ddelay 4¦"                     },
   { 'b', 'r', 'd', '5', "tenth"       , 'y', ':', ":ddelay 5¦"                     },
   { 'b', 'r', 'd', '6', "half"        , 'y', ':', ":ddelay 6¦"                     },
   { 'b', 'r', 'd', '7', "second"      , 'y', ':', ":ddelay 7¦"                     },
   { 'b', 'r', 'd', '8', "double"      , 'y', ':', ":ddelay 8¦"                     },
   { 'b', 'r', 'd', '9', "triple"      , 'y', ':', ":ddelay 9¦"                     },
   { 'b', 'r', 'd', '+', "faster"      , 'y', ':', ":ddelay +¦"                     },
   { 'b', 'r', 'd', '-', "slower"      , 'y', ':', ":ddelay -¦"                     },
   { 'b', 'r', 'd', 'n', "normal"      , 'y', ':', ":dupdate n¦"                    },
   { 'b', 'r', 'd', 's', "slow"        , 'y', ':', ":dupdate s¦"                    },
   { 'b', 'r', 'd', 'b', "blinks"      , 'y', ':', ":dupdate b¦"                    },
   { 'b', 'r', 'd', 'p', "peeks"       , 'y', ':', ":dupdate p¦"                    },
   { 'b', 'r', 'd', 'd', "blind"       , 'y', ':', ":dupdate d¦"                    },
   { 'b', 'r', 'w', '·', "new"         , '-', '·', "-"                              },
   { 'b', 'r', 'o', '·', "open"        , '-', '·', "-"                              },
   { 'b', 'r', 'm', '·', "import"      , '-', '·', "-"                              },
   { 'b', 'r', 's', '·', "save"        , '-', '·', "-"                              },
   { 'b', 'r', 'x', '·', "export"      , '-', '·', "-"                              },
   { 'b', 'r', 'i', '·', "list"        , '-', '·', "-"                              },
   { 'b', 'r', 'g', '·', "globals"     , '-', '·', "-"                              },
   /*---(share menu)---------------------------------------------*/
   { 'b', 'h', '·', '·', "share"       , '-', '>', "-"                              },
   /*---(footer)-------------------------------------------------*/
   {  0 ,  0 ,  0 ,  0 , ""            , 0  , 0  , ""                               },
   /*---(done)---------------------------------------------------*/
};


typedef    struct   cLINK   tLINK;
struct cLINK {
   char        active;                      /* current active status          */
   tMENU      *data;                        /* pointer to menu entry          */
   tLINK      *m_next;                      /* master list next               */
   tLINK      *m_prev;                      /* master list prev               */
   tLINK      *s_next;                      /* next sibling                   */
   tLINK      *c_first;                     /* first child                    */
   uchar       c_count;                     /* count of children              */
};

/*---(true useful vars)---------------*/
static tLINK  *s_head   = NULL;              /* head of link chain            */
static tLINK  *s_tail   = NULL;              /* tail of link chain            */
static char    s_config = 'x';               /* how much base to load         */
/*---(menu grphics/unit testing)------*/
static tLINK  *s_parent = NULL;              /* result of last find           */
static tLINK  *s_found  = NULL;              /* result of last find           */
static char    s_level  = 0;                 /* level of last find            */
/*---(DEBUGGING FASTER)---------------*/
static short   s_nmenu  = 0;                 /* all menu items in list        */
static short   s_nbase  = 0;                 /* base menu items in list       */
static short   s_nreal  = 0;                 /* active menu items in list     */
static short   s_ngrey  = 0;                 /* inactive items in list        */
static short   s_nhide  = 0;                 /* hidden items in list          */
static short   s_ntops  = 0;                 /* top level items in list       */
static short   s_nmids  = 0;                 /* mid level items in list       */
static short   s_nbots  = 0;                 /* bot level items in list       */
/*---(done)---------------------------*/


#define  MAX_PLACE   50
static struct {
   tLINK*      item;
   char        align;
   int         x, xo;
   int         y, yo;
} s_places   [MAX_PLACE];
static int     s_nplace = 0;


static float   s_xpos   [LEN_LABEL] = { 0.0, 2.0, 5.0, 7.0, 7.5, 7.5, 7.0, 5.0, 2.0, 0.0,-2.0,-5.0,-7.0,-7.5,-7.5,-7.0,-5.0,-2.0 };
static float   s_ypos   [LEN_LABEL] = { 3.0, 3.0, 2.0, 1.0, 0.0,-1.0,-2.0,-3.0,-4.0,-4.0,-4.0,-3.0,-2.0,-1.0, 0.0, 1.0, 2.0, 3.0 };
static int     s_align  [LEN_LABEL] = {   2,   1,   1,   1,   1,   1,   1,   1,   1,   2,   3,   3,   3,   3,   3,   3,   3,   3 };
static float   s_abbr   [LEN_LABEL] = { 1.0,   0,   0,   0,   0,   0,   0,   0,   0,-1.0,   0,   0,   0,   0,   0,   0,   0,   0 };

static char    s_type   = YVIKEYS_FLAT;
static char    s_anchor = YVIKEYS_TOPCEN;
static float   s_ce     = 0.0;
static float   s_x      = 140.0;
static float   s_mi     = 0.0;
static float   s_y      = 100.0;
static float   s_z      = 300.0;
static int     s_pt     = 8;
static int     s_xoff   = 15.0;
static int     s_yoff   =  0.0;



char yvikeys_menu_place   (tLINK *a_new);


/*====================------------------------------------====================*/
/*===----                       shared helpers                         ----===*/
/*====================------------------------------------====================*/
static void  o___SHARED__________o () { return; }

char
yvikeys_menu__fix_path    (uchar *a_path, int *a_len, uchar *a_fixed)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   static int  x_len       =    0;
   static uchar x_path     [LEN_LABEL];
   int         i           =    0;
   static uchar x_save     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_len   != NULL)   *a_len = 0;
   if (a_fixed != NULL)   strlcpy (a_fixed, "", LEN_LABEL);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_path);
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_snote   (a_path);
   /*---(short-cutting)------------------*/
   DEBUG_CMDS   yLOG_snote   (x_save);
   if (strcmp (a_path, x_save) == 0) {
      DEBUG_CMDS   yLOG_snote   ("short-cut invoked");
      if (a_len   != NULL)   *a_len = x_len;
      if (a_fixed != NULL)   strlcpy (a_fixed, x_path, LEN_LABEL);
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(check length)-------------------*/
   x_len = strlen (a_path);
   DEBUG_CMDS   yLOG_sint    (x_len);
   --rce;  if (x_len < 1 || x_len > 4) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check prefix)-------------------*/
   --rce;  if (a_path [0] != G_KEY_BSLASH && a_path [0] != G_CHAR_DBSLASH) {
      DEBUG_CMDS   yLOG_snote   ("must start with \\ or µ");
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix)----------------------------*/
   snprintf (x_path, 5, "µ%s······", a_path + 1);
   DEBUG_CMDS   yLOG_snote   (x_path);
   /*---(valid characters)---------------*/
   --rce;  for (i = 1; i < 4; ++i) {
      if (strchr (s_valid, x_path [i]) != NULL)   continue;
      DEBUG_CMDS   yLOG_complex ("bad char"  , "pos %d, %c/%3d", i, x_path [i], x_path [i]);
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check spacers)------------------*/
   --rce;  if (x_path [1] == G_CHAR_SPACE && x_path [2] != G_CHAR_SPACE) {
      DEBUG_CMDS   yLOG_note    ("pos1/2, spacer (·) followed by specific character");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_path [2] == G_CHAR_SPACE && x_path [3] != G_CHAR_SPACE) {
      DEBUG_CMDS   yLOG_note    ("pos2/3, spacer (·) followed by specific character");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check wildcarding)--------------*/
   --rce;  if (x_path [1] == '*') {
      if (x_path [2] != G_CHAR_SPACE && x_path [2] != '*') {
         DEBUG_CMDS   yLOG_note    ("pos1/2, asterisk (*) followed by specific character");
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   --rce;  if (x_path [2] == '*') {
      if (x_path [3] != G_CHAR_SPACE && x_path [3] != '*') {
         DEBUG_CMDS   yLOG_note    ("pos2/3, asterisk (*) followed by specific character");
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(fix length)---------------------*/
   x_len = 4;
   if (x_path [3] == G_CHAR_SPACE)  --x_len;
   if (x_path [2] == G_CHAR_SPACE)  --x_len;
   if (x_path [1] == G_CHAR_SPACE)  --x_len;
   /*---(prepare)------------------------*/
   strlcpy (x_save, a_path, LEN_LABEL);
   if (a_len   != NULL)   *a_len = x_len;
   if (a_fixed != NULL)   strlcpy (a_fixed, x_path, LEN_LABEL);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        memory allocation                     ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char
yvikeys_menu__newlink   (tMENU *a_menu)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tLINK      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_menu);
   --rce;  if (a_menu == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_snote   (a_menu->name);
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tLINK *) malloc (sizeof (tLINK));
   }
   DEBUG_CMDS   yLOG_sint    (x_tries);
   DEBUG_CMDS   yLOG_spoint  (x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CMDS   yLOG_snote   ("FAILED");
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_CMDS   yLOG_snote   ("populate");
   x_new->data     = a_menu;
   x_new->active   = x_new->data->active;
   x_new->m_next   = NULL;
   x_new->m_prev   = NULL;
   x_new->s_next   = NULL;
   x_new->c_first  = NULL;
   x_new->c_count  = 0;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
   /*---(now place)----------------------*/
   rc = yvikeys_menu_place (x_new);
   --rce;  if (rc <= 0) {
      DEBUG_CMDS   yLOG_note    ("placement failed, freeing link");
      free (x_new);
      return rce;
   }
   /*---(tie to master list)-------------*/
   if (s_head == NULL) {
      DEBUG_CMDS   yLOG_note    ("nothing in master list, make first");
      s_head         = x_new;
   } else  {
      DEBUG_CMDS   yLOG_note    ("append to master list");
      s_tail->m_next = x_new;
      x_new->m_prev  = s_tail;
   }
   s_tail        = x_new;
   /*---(update counts)------------------*/
   ++s_nmenu;
   if (x_new->data->base  == MENU_BASE)    ++s_nbase;
   if (x_new->active      == MENU_ACTIVE)  ++s_nreal;
   if (x_new->active      == MENU_GREY)    ++s_ngrey;
   if (x_new->active      == MENU_HIDE)    ++s_nhide;
   DEBUG_CMDS   yLOG_complex ("counters"  , "%dm, %dr, %db", s_nmenu, s_nreal, s_nbase);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_menu__newmenu   (uchar *a_path, char *a_name, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tMENU      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_path);
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_snote   (a_path);
   DEBUG_CMDS   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_spoint  (a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tMENU *) malloc (sizeof (tMENU));
   }
   DEBUG_CMDS   yLOG_sint    (x_tries);
   DEBUG_CMDS   yLOG_spoint  (x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CMDS   yLOG_snote   ("FAILED");
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_CMDS   yLOG_snote   ("populate");
   x_new->base     = MENU_CUSTOM;
   x_new->top      = a_path [1];
   x_new->mid      = a_path [2];
   x_new->bot      = a_path [3];
   strlcpy (x_new->name, a_name, LEN_LABEL);
   x_new->active   = MENU_ACTIVE;
   strlcpy (x_new->keys, a_keys, LEN_TITLE );
   /*---(typing)-------------------------*/
   x_len = strlen (a_keys);
   if      (a_keys [x_len - 1] == G_CHAR_RETURN) x_new->type   = MENU_EXACT;
   else if (a_keys [0]         != ':'          ) x_new->type   = MENU_EXACT;
   else                                          x_new->type   = MENU_MORE;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
   /*---(now place)----------------------*/
   rc = yvikeys_menu__newlink (x_new);
   --rce;  if (rc != 0) {
      DEBUG_CMDS   yLOG_note    ("link failed, freeing menu item");
      free (x_new);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    hierarchy actions                         ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

int
yvikeys_menu__action    (uchar *a_path, int a_max, int a_lvl, tLINK *a_parent, char a_act, tLINK *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   tLINK      *x_curr      = NULL;
   tLINK      *x_sib       = NULL;
   uchar       x_abbr      =  '·';
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%p %d %d %p %c %p", a_path, a_max, a_lvl, a_parent, a_act, a_new);
   /*---(defense)------------------------*/
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_path"    , a_path);
   --rce;  if (a_parent == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_parent"  , a_parent->data->name);
   /*---(walk children)------------------*/
   x_curr = a_parent->c_first;
   DEBUG_CMDS   yLOG_point   ("c_first"   , x_curr);
   while (x_curr != NULL) {
      /*---(prepare)-----------*/
      DEBUG_CMDS   yLOG_complex ("checking"  , "%c %c %c %s", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name);
      switch (a_lvl) {
      case 1 : x_abbr = x_curr->data->top;  break;
      case 2 : x_abbr = x_curr->data->mid;  break;
      case 3 : x_abbr = x_curr->data->bot;  break;
      }
      /*---(filter)------------*/
      if (a_path [a_lvl] != x_abbr && a_path [a_lvl] != '*') {
         x_sib  = x_curr;
         x_curr = x_curr->s_next;
         continue;
      }
      /*---(act)---------------*/
      if (a_lvl == a_max || a_path [a_lvl] == '*' || (a_path [a_lvl] != '*' && a_path [a_lvl + 1] == '*')) {
         DEBUG_CMDS   yLOG_note    ("found existing with exact path");
         switch (a_act) {
         case MENU_FIND    :
         case MENU_MENU    :
            DEBUG_CMDS   yLOG_note    ("saving to s_found/s_level for unit testing");
            s_parent = a_parent;
            s_found  = x_curr;
            s_level  = a_lvl;
            ++c;
            break;
            /*> case MENU_DELETE  :                                                      <* 
             *>    DEBUG_CMDS   yLOG_note    ("deleting current");                       <* 
             *>    rc = yvikeys_menu__delete (a_parent, x_sib, x_curr);                  <* 
             *>    ++c;                                                                  <* 
             *>    break;                                                                <*/
         case MENU_ACTIVE  :
            DEBUG_CMDS   yLOG_note    ("marking current as active");
            switch (x_curr->active) {
            case MENU_ACTIVE : --s_nreal;  break;
            case MENU_GREY   : --s_ngrey;  break;
            case MENU_HIDE   : --s_nhide;  break;
            }
            x_curr->active = MENU_ACTIVE;
            ++s_nreal;
            ++c;
            break;
         case MENU_GREY    :
            DEBUG_CMDS   yLOG_note    ("marking current as grey");
            switch (x_curr->active) {
            case MENU_ACTIVE : --s_nreal;  break;
            case MENU_GREY   : --s_ngrey;  break;
            case MENU_HIDE   : --s_nhide;  break;
            }
            x_curr->active = MENU_GREY;
            ++s_ngrey;
            ++c;
            break;
         case MENU_HIDE    :
            DEBUG_CMDS   yLOG_note    ("marking current as hidden");
            switch (x_curr->active) {
            case MENU_ACTIVE : --s_nreal;  break;
            case MENU_GREY   : --s_ngrey;  break;
            case MENU_HIDE   : --s_nhide;  break;
            }
            x_curr->active = MENU_HIDE;
            ++s_nhide;
            ++c;
            break;
         case MENU_PLACE   :
            DEBUG_CMDS   yLOG_note    ("can not add when already exists");
            DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(recurse)-----------*/
      if (a_lvl < a_max) {
         if (a_act == MENU_MENU) {
            s_found  = x_curr;
            s_level  = a_lvl;
         }
         DEBUG_CMDS   yLOG_note    ("matched so far, recurse into menu another level");
         rc = yvikeys_menu__action (a_path, a_max, a_lvl + 1, x_curr, a_act, a_new);
         DEBUG_CMDS   yLOG_value   ("rc"        , rc);
         if (rc >= 0)  c += rc;
      }
      /*---(errors)------------*/
      if (rc < 0) {
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(next)--------------*/
      x_sib  = x_curr;
      x_curr = x_curr->s_next;
      /*---(done)--------------*/
   }
   /*---(fall-out empty)-----------------*/
   if (a_act != MENU_PLACE) {
      DEBUG_CMDS   yLOG_value   ("c"         , c);
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return c;
   }
   /*---(fall-out empty)-----------------*/
   --rce;  if (a_lvl != a_max) {
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return c;
   }
   /*---(add child)----------------------*/
   if (x_sib == NULL) {
      DEBUG_CMDS   yLOG_note    ("adding as first child");
      a_parent->c_first = a_new;
   } else  {
      DEBUG_CMDS   yLOG_note    ("adding to tail of list of children");
      x_sib->s_next    = a_new;
   }
   ++a_parent->c_count;
   DEBUG_CMDS   yLOG_value   ("c_count"   , a_parent->c_count);
   switch (a_lvl) {
   case 1 : ++s_ntops;  break;
   case 2 : ++s_nmids;  break;
   case 3 : ++s_nbots;  break;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 1;
}

int
yvikeys_menu_action     (char a_act, uchar *a_path, tLINK *a_link)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   uchar       x_path      [LEN_LABEL];
   int         x_max       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%c %-10p %-10p", a_act, a_path, a_link);
   /*---(prepare)------------------------*/
   s_parent = NULL; /* reset unit testing holder */
   s_found  = NULL; /* reset unit testing holder */
   s_level  =    0; /* reset unit testing holder */
   /*---(defense)------------------------*/
   --rce;  if (a_link   != NULL) {
      sprintf (x_path, "µ%c%c%c", a_link->data->top, a_link->data->mid, a_link->data->bot);
      x_len = 4;
   } else if (a_path != NULL) {
      rc = yvikeys_menu__fix_path (a_path, &x_len, x_path);
      DEBUG_CMDS   yLOG_value   ("fix_path"  , rc);
      if (rc < 0) {
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   } else {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("x_path"    , x_path);
   /*---(shortcut)-----------------------*/
   if ((a_act == MENU_FIND || a_act == MENU_MENU) && x_len == 1) {
      DEBUG_CMDS   yLOG_note    ("shortcut, looking for root");
      s_parent = s_head;
      s_found  = s_head;
      s_level  = 0;
      rc       = 1;
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   if (a_act == MENU_MENU) {
      s_parent = s_head;
      s_found  = s_head;
      s_level  = 0;
   }
   /*---(calc max)-----------------------*/
   if (x_len > 1 && x_path [1] != G_CHAR_SPACE)  ++x_max;
   if (x_len > 2 && x_path [2] != G_CHAR_SPACE)  ++x_max;
   if (x_len > 3 && x_path [3] != G_CHAR_SPACE)  ++x_max;
   DEBUG_CMDS   yLOG_value   ("x_max"     , x_max);
   /*---(check wildcarding)--------------*/
   --rce;  if (x_path [1] == '*') {
      if (x_path [2] != G_CHAR_SPACE && x_path [2] != '*') {
         DEBUG_CMDS   yLOG_note    ("asterisk (*) followed by specific character");
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   --rce;  if (x_path [2] == '*') {
      if (x_path [3] != G_CHAR_SPACE && x_path [3] != '*') {
         DEBUG_CMDS   yLOG_note    ("asterisk (*) followed by specific character");
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(run)----------------------------*/
   if (s_head == NULL) {
      rc = 1;
   } else {
      rc = yvikeys_menu__action    (x_path, x_max, 1, s_head, a_act, a_link);
      DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return rc;
}

char yvikeys_menu_place   (tLINK *a_new)  { return yvikeys_menu_action (MENU_PLACE , NULL  , a_new); }
int  yvikeys_menu_delete  (uchar *a_path) { return yvikeys_menu_action (MENU_PLACE , a_path, NULL ); }
int  yvikeys_menu_find    (uchar *a_path) { return yvikeys_menu_action (MENU_FIND  , a_path, NULL ); }
int  yvikeys_menu_menu    (uchar *a_path) { return yvikeys_menu_action (MENU_MENU  , a_path, NULL ); }
int  yVIKEYS_menu_active  (uchar *a_path) { return yvikeys_menu_action (MENU_ACTIVE, a_path, NULL ); }
int  yVIKEYS_menu_grey    (uchar *a_path) { return yvikeys_menu_action (MENU_GREY  , a_path, NULL ); }
int  yVIKEYS_menu_hide    (uchar *a_path) { return yvikeys_menu_action (MENU_HIDE  , a_path, NULL ); }



/*====================------------------------------------====================*/
/*===----                       working with base                      ----===*/
/*====================------------------------------------====================*/
static void  o___BASE____________o () { return; }

char
yvikeys_menu_cmds         (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   /*---(handle)-------------------------*/
   for (n = 0; n < MAX_MENU; ++n) {
      /*---(check for end)---------------*/
      if (s_base [n].name [0] == NULL)                break;
      if (strcmp (s_base [n].name, "-") == 0)         break;
      /*---(filter)----------------------*/
      if (s_base [n].bot != G_CHAR_SPACE)             continue;
      if (s_base [n].mid != G_CHAR_SPACE)             continue;
      /*---(matching)--------------------*/
      if (s_base [n].top != a_key)                    continue;
      return 1;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;;
}

char
yvikeys_menu__in_base     (uchar *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   uchar       x_path      [LEN_LABEL];
   /*---(fix path)-----------------------*/
   rc = yvikeys_menu__fix_path (a_path, NULL, x_path);
   --rce;  if (rc < 0) return rce;
   /*---(handle)-------------------------*/
   for (n = 0; n < MAX_MENU; ++n) {
      /*---(check for end)---------------*/
      if (s_base [n].name [0] == NULL)                break;
      if (strcmp (s_base [n].name, "-") == 0)         break;
      /*---(filter)----------------------*/
      if (s_base [n].top != x_path [1])               continue;
      if (s_base [n].mid != x_path [2])               continue;
      if (s_base [n].bot != x_path [3])               continue;
      /*---(matching)--------------------*/
      return 1;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;;
}

char
yvikeys_menu__base_num  (int n)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (s_head);
   --rce;  if (s_head == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add)----------------------------*/
   rc = yvikeys_menu__newlink (s_base + n);
   DEBUG_CMDS   yLOG_value   ("newlink"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__base_path (uchar *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   int         n           =    0;
   uchar       x_path      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(fix path)-----------------------*/
   rc = yvikeys_menu__fix_path (a_path, NULL, x_path);
   DEBUG_CMDS   yLOG_value   ("fix_path"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle)-------------------------*/
   for (n = 0; n < MAX_MENU; ++n) {
      /*---(check for end)---------------*/
      if (s_base [n].name [0] == NULL)                break;
      if (strcmp (s_base [n].name, "-") == 0)         break;
      /*---(filter)----------------------*/
      DEBUG_CMDS   yLOG_complex ("checking"  , "%c %c %c %s", s_base [n].top, s_base [n].mid, s_base [n].bot, s_base [n].name);
      if (s_base [n].top != x_path [1] && (x_path [1] != '*' && s_base [n].top != G_CHAR_SPACE)) {
         DEBUG_CMDS   yLOG_note    ("top rule bounded this one");
         continue;
      }
      if (s_base [n].mid != x_path [2] && (x_path [2] != '*' && s_base [n].top != G_CHAR_SPACE)) {
         DEBUG_CMDS   yLOG_note    ("mid rule bounded this one");
         continue;
      }
      if (s_base [n].bot != x_path [3] && (x_path [3] != '*' && s_base [n].top != G_CHAR_SPACE)) {
         DEBUG_CMDS   yLOG_note    ("bot rule bounded this one");
         continue;
      }
      /*---(create)----------------------*/
      rc = yvikeys_menu__newlink (s_base + n);
      /*---(done)------------------------*/
   }
   DEBUG_CMDS   yLOG_value   ("newlink"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__purge     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   tLINK      *x_curr      = NULL;
   tLINK      *x_prev      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_point   ("s_tail"    , s_tail);
   --rce;  if (s_tail == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_curr = s_tail;
   /*---(clear)--------------------------*/
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
   while (x_curr != s_head && x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("focus"     , "%c %s", x_curr->data->base, x_curr->data->name);
      x_prev = x_curr->m_prev;
      if (x_curr->data->base != MENU_BASE) {
         free (x_curr->data);
      }
      free (x_curr);
      x_curr = x_prev;
   }
   /*---(initialize pointers)------------*/
   DEBUG_CMDS   yLOG_note    ("pointers");
   s_head->m_next  = s_head->m_prev = s_head->s_next = s_head->c_first = NULL;
   s_tail   = s_head;
   s_parent = NULL;
   s_found  = NULL;
   /*---(initialize counters)------------*/
   DEBUG_CMDS   yLOG_note    ("counters");
   s_head->c_count = 0;
   s_level  = 0;
   s_nmenu  = 1;
   s_nbase  = 1;
   s_nreal  = 1;
   s_ngrey  = 0;
   s_nhide  = 0;
   s_ntops  = 0;
   s_nmids  = 0;
   s_nbots  = 0;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__base_load (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(load)---------------------------*/
   for (i = 0; i < MAX_MENU; ++i) {
      /*---(check for end)---------------*/
      if (s_base [i].name [0] == NULL)                break;
      if (strcmp (s_base [i].name, "-") == 0)         break;
      if (s_base [i].top ==  0 )  break;
      if (s_base [i].top == G_CHAR_SPACE)  continue;
      if (s_base [i].mid == G_CHAR_SPACE && strchr ("12x"  , s_config) != NULL) {
         yvikeys_menu__base_num (i);
         continue;
      }
      if (s_base [i].bot == G_CHAR_SPACE && strchr ("2x"   , s_config) != NULL) {
         yvikeys_menu__base_num (i);
         continue;
      }
      if (strchr ("x"    , s_config) != NULL) {
         yvikeys_menu__base_num (i);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_menu_config     (char a_layout)
{
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_value   ("a_layout"  , a_layout);
   yvikeys_menu__purge ();
   switch (a_layout) {
   case '-' :            s_config = '-'; break;
   case '1' :            s_config = '1'; break;
   case '2' :            s_config = '2'; break;
   case 'x' : default  : s_config = 'x'; break;
   }
   yvikeys_menu__base_load ();
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         menu display                         ----===*/
/*====================------------------------------------====================*/
static void  o___MENUS___________o () { return; }

char
yvikeys_menu_reanchor     (char a_anchor)
{
   return yvikeys_view_reanchor (YVIKEYS_MENUS, a_anchor);
}

char
yvikeys_menu_final      (void)
{
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*> yVIKEYS_cmds_addX (YVIKEYS_M_VIEW  , "help"        , ""    , "c"    , yvikeys_help               , "" );   <*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_MENUS)) {
      DEBUG_CMDS   yLOG_note    ("status is not ready for init");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(menus)--------------------------*/
   DEBUG_CMDS   yLOG_note    ("initialize menu system");
   yvikeys_menu__purge  ();
   /*---(add the root)-------------------*/
   s_head = s_tail = NULL;
   yvikeys_menu__newlink (s_base);
   /*---(handle total)-------------------*/
   yVIKEYS_menu_config ('x');
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_MENUS);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu_wrap       (void)
{
   yVIKEYS_menu_config ('-');
   if (s_head != NULL)  free  (s_head);
   s_head = s_tail = NULL;
   s_nmenu  = 0;
   s_nbase  = 0;
   s_nreal  = 0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     menu item placement                      ----===*/
/*====================------------------------------------====================*/
static void  o___PLACEMENT_______o () { return; }

char
yvikeys_menu__place_clear  (void)
{
   int         i           =    0;
   for (i = 0; i < MAX_PLACE; ++i) {
      s_places [i].item  = NULL;
      s_places [i].align = 0;
      s_places [i].x = s_places [i].xo = 0;
      s_places [i].y = s_places [i].yo = 0;
   }
   s_nplace = 0;
   return 0;
}

char
yvikeys_menu__place_round  (char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tLINK      *x_curr      = NULL;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_off, y_off;
   int         x_pos, y_pos;
   int         x_cen, x_mid;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(prepare)------------------------*/
   DEBUG_GRAF   yLOG_value   ("c_count"   , s_head->c_count);
   x_curr = s_head->c_first;
   x_cen  = x_left + (x_wide * 0.50);
   x_mid  = x_bott + (x_tall * 0.50);
   yvikeys_menu__place_clear ();
   /*---(walk children)------------------*/
   while (x_curr != NULL) {
      DEBUG_GRAF   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      /*---(alignment)---------*/
      switch (s_align [s_nplace]) {
      case 1 :  x_off =   1;  y_off =  0;          break;
      case 2 :  x_off =   0;  y_off = s_abbr [s_nplace];  break;
      case 3 :  x_off =  -1;  y_off =  0;          break;
      }
      /*---(position)----------*/
      x_pos = s_xpos [s_nplace];
      y_pos = s_ypos [s_nplace] + 1;
      if (myVIKEYS.env == YVIKEYS_CURSES) {
         /*> y_pos = x_bott + x_tall - s_ypos [s_nplace] + 4;                                <*/
         x_off  *=  2.0;
      } else if (s_type == YVIKEYS_FLAT) {
         x_pos  *=  8.0;
         y_pos  *= 14.0;
         y_off  *= 14.0;
         x_off  *= 15.0;
      } else {
         x_pos  *=  2.0;
         y_pos  *=  3.5;
         y_off  *=  3.0;
         x_off  *=  3.0;
      }
      /*---(alignment)---------*/
      if (x_off >  0)  s_places [s_nplace].align = YF_BASLEF;
      if (x_off == 0)  s_places [s_nplace].align = YF_BASCEN;
      if (x_off <  0)  s_places [s_nplace].align = YF_BASRIG;
      /*---(draw)--------------*/
      s_places [s_nplace].item = x_curr;
      s_places [s_nplace].x    = x_cen + x_pos;
      s_places [s_nplace].xo   = x_off;
      if (myVIKEYS.env == YVIKEYS_OPENGL) {
         s_places [s_nplace].y    = x_mid + y_pos;
         s_places [s_nplace].yo   = y_off;
      } else {
         s_places [s_nplace].y    = x_mid - y_pos - x_tall + 1;
         s_places [s_nplace].yo   = -y_off;
      }
      /*---(next)--------------*/
      x_curr = x_curr->s_next;
      ++s_nplace;
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__place_cols   (char a_type, int a_level, tLINK *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         a           =   10;
   int         t           [LEN_LABEL];
   int         x_entry     =    0;
   int         x_group     =    0;
   int         x_left, x_wide, x_bott, x_tall;
   float       x_pos, y_pos;
   float       x_off;
   int         y_vert;
   tLINK      *x_curr;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(col config)---------------------*/
   a = a_curr->c_count;
   if (a <= 10) {
      x_group = 3;
   } else if (a <= 20) {
      x_group = 0;
      if (a % 2 == 0)  a = a / 2.0;
      else             a = a / 2.0 + 1;
   } else {
      x_group = 2;
      if (a % 3 == 0)  a = a / 3.0;
      else             a = a / 3.0 + 1;
   }
   /*---(prepare)---------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      x_off  =  2.0;
      y_vert =  1;
   } else if (s_type == YVIKEYS_FLAT) {
      x_off  = 15.0;
      y_vert = 14;
   } else {
      x_off  =  3.0;
      y_vert = 14;
   }
   /*---(walk entries)-------------------*/
   x_curr = a_curr->c_first;
   yvikeys_menu__place_clear ();
   while (x_curr != NULL) {
      DEBUG_GRAF   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      /*---(position)--------------------*/
      if (myVIKEYS.env == YVIKEYS_CURSES) {
         if (x_entry == 0)  y_pos  = x_bott + (x_tall * 0.15);
         else               y_pos += y_vert;
      } else {
         if (x_entry == 0)  y_pos  = x_bott + (x_tall * 0.85) - (x_tall * 0.14 * x_entry);
         else               y_pos -= y_vert;
      }
      switch (x_group) {
      case 0 : x_pos = x_left + x_wide * 0.20;   break;
      case 1 : x_pos = x_left + x_wide * 0.58;   break;
      case 2 : x_pos = x_left + x_wide * 0.05;   break;
      case 3 : x_pos = x_left + x_wide * 0.38;   break;
      case 4 : x_pos = x_left + x_wide * 0.70;   break;
      }
      /*---(alignment)---------*/
      if (x_off >  0)  s_places [s_nplace].align = YF_BASLEF;
      if (x_off == 0)  s_places [s_nplace].align = YF_BASCEN;
      if (x_off <  0)  s_places [s_nplace].align = YF_BASRIG;
      /*---(draw)--------------*/
      s_places [s_nplace].item = x_curr;
      s_places [s_nplace].x    = x_pos;
      s_places [s_nplace].xo   = x_off;
      if (myVIKEYS.env == YVIKEYS_OPENGL)  s_places [s_nplace].y    = y_pos;
      else                                 s_places [s_nplace].y    = y_pos - x_tall + 1;
      s_places [s_nplace].yo   = 0;
      /*---(next)--------------*/
      ++s_nplace;
      /*---(figure next)-----------------*/
      x_curr = x_curr->s_next;
      ++x_entry;
      if (s_nplace != 0 && s_nplace % a == 0) {
         ++x_group;
         x_entry = 0;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        drawing menus                         ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

char
yvikeys_menu_shadow     (char a_type, int a_lef, int a_wide, int a_bot, int a_tall, int z)
{
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      DEBUG_GRAF   yLOG_note    ("draw black shadow");
      glColor4f (0.0f, 0.0f, 0.0f, 1.0f);
      glBegin(GL_POLYGON); {
         glVertex3f (a_lef         , a_bot + a_tall, z);
         glVertex3f (a_lef + a_wide, a_bot + a_tall, z);
         glVertex3f (a_lef + a_wide, a_bot         , z);
         glVertex3f (a_lef         , a_bot         , z);
      } glEnd();
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> create a shape mask for notes ------------------------------*/
yVIKEYS_menu_mask       (void *a_bounds, void *a_context, int a_left, int a_topp, int a_xmin, int a_ymax)
{
   /*---(locals)-----------+-----+-----+-*/
   Pixmap      *x_bounds;
   GC          *x_context;
   int         x_left, x_wide, x_bott, x_tall;
   /*---(quick out)----------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)    return 0;
   if (yVIKEYS_mode () != SMOD_MENUS)     return 0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex  ("args"      , "%p, %p, %3dl, %3dt, %3dx, %3dy", a_bounds, a_context, a_left, a_topp, a_xmin, a_ymax);
   /*---(get size)-----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(cast)---------------------------*/
   x_bounds  = (Pixmap *) a_bounds;
   x_context = (GC *) a_context;
   /*---(draw)---------------------------*/
   XFillRectangle (YX_DISP, *x_bounds, *x_context, a_left + (x_left - a_xmin), a_topp + a_ymax - (x_bott + x_tall), x_wide, x_tall);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu_fill       (char a_type, int a_len, int a_lvl, int a_lef, int a_wide, int a_bot, int a_tall, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_edge, y_edge;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_note    ("draw menu background");
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      if      (a_len == 't')      glColor4f (0.85f, 0.85f, 0.97f, 1.0f);
      else if (a_len == 'n')      glColor4f (0.50f, 0.50f, 0.85f, 1.0f);
      else if (a_len != a_lvl)    glColor4f (0.85f, 0.50f, 0.50f, 1.0f);
      else                        glColor4f (0.70f, 0.70f, 0.70f, 1.0f);
      x_edge = 3;
      y_edge = 3;
      glBegin(GL_POLYGON); {
         glVertex3f (a_lef          + x_edge, a_bot + a_tall - y_edge, z);
         glVertex3f (a_lef + a_wide - x_edge, a_bot + a_tall - y_edge, z);
         glVertex3f (a_lef + a_wide - x_edge, a_bot          + y_edge, z);
         glVertex3f (a_lef          + x_edge, a_bot          + y_edge, z);
      } glEnd();
      if      (a_len == 't')      glColor4f (0.88f, 0.88f, 1.00f, 1.0f);
      else if (a_len == 'n')      glColor4f (0.55f, 0.55f, 0.90f, 1.0f);
      else if (a_len != a_lvl)    glColor4f (0.87f, 0.52f, 0.52f, 1.0f);
      else                        glColor4f (0.72f, 0.72f, 0.72f, 1.0f);
      glBegin(GL_POLYGON); {
         glVertex3f (a_lef          + x_edge, a_bot + a_tall - y_edge, z);
         glVertex3f (a_lef + a_wide - x_edge, a_bot + a_tall - y_edge, z);
         glVertex3f (a_lef + (a_wide * 0.50), a_bot + (a_tall * 0.50), z);
      } glEnd();
      if      (a_len == 't')      glColor4f (0.82f, 0.82f, 0.94f, 1.0f);
      else if (a_len == 'n')      glColor4f (0.45f, 0.45f, 0.80f, 1.0f);
      else if (a_len != a_lvl)    glColor4f (0.83f, 0.48f, 0.48f, 1.0f);
      else                        glColor4f (0.68f, 0.68f, 0.68f, 1.0f);
      glBegin(GL_POLYGON); {
         glVertex3f (a_lef          + x_edge, a_bot          + y_edge, z);
         glVertex3f (a_lef + a_wide - x_edge, a_bot          + y_edge, z);
         glVertex3f (a_lef + (a_wide * 0.50), a_bot + (a_tall * 0.50), z);
      } glEnd();
   }
   /*---(ncurses)------------------------*/
   else {
      if (a_len <  0)           yCOLOR_curs ("m_main");
      else if (a_len != a_lvl)  yCOLOR_curs ("m_curr");
      else                      yCOLOR_curs ("m_prev");
      for (i = a_bot; i < a_bot + a_tall; ++i) {
         mvprintw   (i, a_lef, "%*.*s", a_wide, a_wide, "                                                  ");
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_menu_heads      (char a_type, int a_len, int a_lvl, void *a_found, int a_lef, int a_wide, int a_bot, int a_tall, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_cen, x_mid;
   int         x_edge, y_edge;
   int         x_len       =    0;
   char        t           [LEN_RECD];
   tLINK      *x_found     = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_note    ("draw menu background");
   /*---(prepare)------------------------*/
   x_found = (tLINK *) a_found;
   switch (a_lvl) {
   case  0           : strlcpy (t, "main menu", LEN_DESC); break;
   case  1           : sprintf (t, "µ%c (%s) sub-menu" , x_found->data->top, x_found->data->name); break;
   case  2           : sprintf (t, "µ%c%c (%s) options", x_found->data->top, x_found->data->mid, x_found->data->name); break;
   case MODE_SEARCH  : strlcpy (t, "search history", LEN_DESC); break;
   case MODE_COMMAND : strlcpy (t, "command history", LEN_DESC); break;
   }
   x_len  = strlen (t);
   x_cen  = a_lef + (a_wide * 0.50);
   x_mid  = a_bot + (a_tall * 0.50);
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      /*---(labels)-------------------------*/
      glColor4f (0.0, 0.0, 0.0, 1.0);
      x_edge =  5;
      y_edge = 15;
      /*---(title)--------------------------*/
      glPushMatrix(); {
         glTranslatef (x_cen, a_bot + a_tall - y_edge, z);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_TOPCEN, t);
      } glPopMatrix();
      /*---(prog name)-------------------*/
      glPushMatrix(); {
         glTranslatef (a_lef + x_edge, a_bot + a_tall - y_edge, z);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_TOPLEF, myVIKEYS.s_prog);
      } glPopMatrix();
      /*---(version)---------------------*/
      glPushMatrix(); {
         glTranslatef (a_lef + a_wide - x_edge, a_bot + a_tall - y_edge, z);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_TOPRIG, myVIKEYS.s_vernum);
      } glPopMatrix();
      /*---(footer)----------------------*/
      glPushMatrix(); {
         glTranslatef (x_cen, a_bot + y_edge, z);
         if (a_lvl >= 0 && a_lvl <= 2) yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BASCEN, "much wider, flatter, and universal");
      } glPopMatrix();
      /*---(done)------------------------*/
   }
   /*---(ncurses)------------------------*/
   else {
      if (a_len != a_lvl)  yCOLOR_curs ("m_curr");
      else                 yCOLOR_curs ("m_prev");
      /*---(title)--------------------------*/
      mvprintw   (a_bot - a_tall + 1, x_cen - (x_len / 2), "%s", t);
      /*---(prog name)-------------------*/
      mvprintw   (a_bot - a_tall + 1, a_lef + 1, "%s", myVIKEYS.s_prog);
      /*---(version)---------------------*/
      x_len = strlen (myVIKEYS.s_vernum);
      mvprintw   (a_bot - a_tall + 1, a_lef + a_wide - x_len - 1, "%s", myVIKEYS.s_vernum);
      /*---(footer)----------------------*/
      strlcpy (t, "much wider, flatter, and universal", LEN_RECD);
      x_len = strlen (t);
      if (a_lvl >= 0)  mvprintw   (a_bot, x_cen - (x_len / 2), "%s", t);
      /*---(done)------------------------*/
   }
   return 0;
}

char
yvikeys_menu__error     (char a_type, int a_len, int a_lvl, tLINK *a_found, int a_lef, int a_wide, int a_bot, int a_tall, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_cen, x_mid;
   int         x_edge, y_edge;
   int         x_len       =    0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   if (a_len == a_lvl)   return 0;
   DEBUG_GRAF   yLOG_note    ("draw error message");
   /*---(prepare)------------------------*/
   x_cen  = a_lef + (a_wide * 0.50);
   x_mid  = a_bot + (a_tall * 0.55);
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      /*---(error)-----------------------*/
      glPushMatrix(); {
         glColor4f    (1.0f, 1.0f, 1.0f, 1.0f);
         glTranslatef (x_cen, x_mid,  z);
         yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BASCEN, "ERROR KEYS");
         glTranslatef (  0.0, -a_tall * 0.10,   0.0);
         yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BASCEN, "menu locked, <esc> to exit");
      } glPopMatrix();
      /*---(done)------------------------*/
   }
   /*---(ncurses)------------------------*/
   else {
      /*---(error)-----------------------*/
      yCOLOR_curs ("!_errs");
      strlcpy (t, "ERROR KEY", LEN_RECD);
      x_len = strlen (t);
      mvprintw   (a_bot - 7, x_cen - (x_len / 2), "%s", t);
      strlcpy (t, "press ¥", LEN_RECD);
      x_len = strlen (t);
      mvprintw   (a_bot - 6, x_cen - (x_len / 2), "%s", t);
      /*---(done)------------------------*/
   }
   return 0;
}

char
yvikeys_menu__back      (int a_len, int a_level, tLINK *a_found)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left, x_wide, x_bott, x_tall;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "%d %d -- %c %c %c %-10.10s %c", a_len, a_level, a_found->data->top, a_found->data->mid, a_found->data->bot, a_found->data->name, a_found->active);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(make background)----------------*/
   s_pt     = myVIKEYS.point;
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      yvikeys_menu_shadow   (myVIKEYS.env, x_left, x_wide, x_bott, x_tall, 300);
      yvikeys_menu_fill     (myVIKEYS.env, a_len, a_level, x_left, x_wide, x_bott, x_tall, 310);
      yvikeys_menu_heads    (myVIKEYS.env, a_len, a_level, a_found, x_left, x_wide, x_bott, x_tall, 320);
      yvikeys_menu__error   (myVIKEYS.env, a_len, a_level, a_found, x_left, x_wide, x_bott, x_tall, 340);
   } else {
      yvikeys_menu_shadow   (myVIKEYS.env, x_left, x_wide, x_bott - x_tall, x_tall, 300);
      yvikeys_menu_fill     (myVIKEYS.env, a_len, a_level, x_left, x_wide, x_bott - x_tall + 1, x_tall, 310);
      yvikeys_menu_heads    (myVIKEYS.env, a_len, a_level, a_found, x_left, x_wide, x_bott, x_tall, 320);
      yvikeys_menu__error   (myVIKEYS.env, a_len, a_level, a_found, x_left, x_wide, x_bott, x_tall, 340);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

yvikeys_menu__color     (tLINK *a_item)
{
   switch (a_item->data->type) {
   case MENU_GROUP :
      if      (a_item->active   == MENU_GREY  )  glColor4f (0.4, 0.4, 0.4, 1.0);
      else if (a_item->c_count  == 0)            glColor4f (0.4, 0.4, 0.4, 1.0);
      else                                       glColor4f (0.0, 0.0, 0.0, 1.0);
      break;
   case MENU_NAMER : case MENU_TBD :
      glColor4f (0.4, 0.0, 0.0, 1.0);
      break;
   case MENU_EXACT : case MENU_CMD  : case MENU_MORE :
      if      (a_item->active   == MENU_ACTIVE)  glColor4f (0.0, 0.0, 0.0, 1.0);
      else if (a_item->active   == MENU_GREY  )  glColor4f (0.4, 0.4, 0.4, 1.0);
      else                                       glColor4f (0.4, 0.0, 0.0, 1.0);
      break;
   default  :
      glColor4f (1.0, 0.0, 0.0, 1.0);
      break;
   }
   return 0;
}

char
yvikeys_menu__entry     (char a_type, tLINK *a_item, int a_level, float x, int y, float z, float a_offx, float a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_LABEL];
   int         x_align     =    0;
   int         x_cen, x_mid;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "%dl %6.2fx %6.2fy %6.2fz %6.2fox %6.2foy", a_level, x, y, z, a_offx, a_offy);
   /*---(abbrev)----------------------*/
   switch (a_level) {
   case 0  : sprintf (t, "%c", a_item->data->top);   break;
   case 1  : sprintf (t, "%c", a_item->data->mid);   break;
   case 2  : sprintf (t, "%c", a_item->data->bot);   break;
   }
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      glPushMatrix(); {
         /*---(recenter)--------------------*/
         glTranslatef (x, y, z);
         /*---(alignment)-------------------*/
         if (a_offx >  0)  x_align = YF_BASLEF;
         if (a_offx == 0)  x_align = YF_BASCEN;
         if (a_offx <  0)  x_align = YF_BASRIG;
         yFONT_print (myVIKEYS.font, myVIKEYS.point, x_align, t);
         /*---(name)------------------------*/
         glTranslatef (a_offx, a_offy, 0.0);
         sprintf (t, "%.9s", a_item->data->name);
         if (a_level > 0) {
            if (a_item->data->type == MENU_GROUP)  strlcat (t, "+", LEN_LABEL);
            if (a_item->data->type == MENU_MORE )  strlcat (t, "=", LEN_LABEL);
         }
         yFONT_print (myVIKEYS.font, myVIKEYS.point, x_align, t);
         /*---(done)------------------------*/
      } glPopMatrix();
   }
   else {
      /*---(prepare)------------------------*/
      if (t [0] < 0)  strcpy (t, "-");
      if (strchr ("èéêëìíîïðñòóôõö÷øùúûüýþÿ", t [0]) != NULL)  strcpy (t, "-");
      mvprintw   (y, x, "%s", t);
      /*---(prepare)------------------------*/
      sprintf (t, "%.9s", a_item->data->name);
      if (a_level > 0 && a_item->data->type == MENU_GROUP)  strlcat (t, "+", LEN_LABEL);
      if (a_level > 0 && a_item->data->type == MENU_MORE )  strlcat (t, "=", LEN_LABEL);
      x_len = strlen (t);
      /*---(alignment)----------------------*/
      if      (a_offx >  0)  a_offx  += 0;
      else if (a_offx == 0)  a_offx  -= x_len / 2;
      else if (a_offx <  0)  a_offx  -= x_len - 1;
      x += a_offx;
      y += a_offy;
      /*---(name)---------------------------*/
      DEBUG_GRAF   yLOG_complex ("title"     , "%3dox, %3doy, %3dl, %3dx, %3dy", a_offx, a_offy, x_len, x, y);
      mvprintw   (y, x, "%s", t);
   }
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__show      (int a_level, tLINK *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tLINK      *x_curr      = NULL;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(prepare)------------------------*/
   DEBUG_GRAF   yLOG_value   ("c_count"   , s_head->c_count);
   if (a_level == 0)  yvikeys_menu__place_round (myVIKEYS.env);
   else               yvikeys_menu__place_cols  (myVIKEYS.env, a_level, a_curr);
   /*---(walk children)------------------*/
   for (i = 0; i < s_nplace; ++i) {
      x_curr = s_places [i].item;
      DEBUG_GRAF   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      yvikeys_menu__color (x_curr);
      yvikeys_menu__entry (myVIKEYS.env, x_curr, a_level, s_places [i].x, s_places [i].y, 330, s_places [i].xo, s_places [i].yo);
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu_draw          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         n           =   -1;
   /*---(header)----------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_GRAF   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_MENUS  )) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("s_head"    , s_head);
   --rce;  if (s_head == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find path)----------------------*/
   DEBUG_GRAF   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   n = yvikeys_menu_menu (myVIKEYS.m_path);
   DEBUG_GRAF   yLOG_point   ("s_found"   , s_found);
   --rce;  if (s_found == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_complex ("target"    , "%c %c %c %s", s_found->data->top, s_found->data->mid, s_found->data->bot, s_found->data->name);
   DEBUG_GRAF   yLOG_complex ("parent"    , "%c %c %c %s", s_parent->data->top, s_parent->data->mid, s_parent->data->bot, s_parent->data->name);
   DEBUG_GRAF   yLOG_value   ("level"     , s_level);
   /*---(draw back)----------------------*/
   x_len = strlen (myVIKEYS.m_path);
   DEBUG_GRAF   yLOG_value   ("x_len"     , x_len);
   yvikeys_menu__back (x_len - 1, s_level, s_found);
   /*---(main menu)----------------------*/
   yvikeys_menu__show (s_level, s_found);
   /*---(complete)--------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         mode handler                         ----===*/
/*====================------------------------------------====================*/
static void  o___SUBMODE_________o () { return; }

char
yvikeys_menu_start      (void)
{
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   strlcpy (myVIKEYS.m_path, "\\", LEN_LABEL);
   strlcpy (myVIKEYS.m_keys, ""  , LEN_TITLE );
   /*> OPENGL_mask ();                                                                <*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> allow menu selection ---------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
yvikeys_menu_smode      (int  a_major, int  a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        t           [LEN_LABEL];
   int         n           =    0;
   int         x_len       =    0;
   char        x_level     =    0;
   int         x_last      =    0;
   char        x_on        =    0;
   uchar       x_exec      [LEN_LABEL] = "·:=";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_MENUS  )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape, return to map mode");
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("return, return to map mode");
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_SPACE || a_minor == G_CHAR_SPACE) {
      DEBUG_USER   yLOG_note    ("ignore spaces");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(add to menu)--------------------*/
   sprintf (t, "%c", a_minor);
   strlcat (myVIKEYS.m_path, t, LEN_LABEL);
   DEBUG_USER   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   /*---(find path)----------------------*/
   n = yvikeys_menu_find (myVIKEYS.m_path);
   DEBUG_USER   yLOG_value   ("n"         , n);
   DEBUG_USER   yLOG_value   ("s_level"   , s_level);
   DEBUG_USER   yLOG_value   ("x_last"    , x_last);
   /*---(draw back)----------------------*/
   DEBUG_USER   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   x_len = strlen (myVIKEYS.m_path);
   DEBUG_USER   yLOG_value   ("x_len"     , x_len);
   /*---(check exec)---------------------*/
   --rce;  if (n > 0 && x_len - 1 == s_level) {
      DEBUG_USER   yLOG_char    ("type"      , s_found->data->type);
      if (strchr (x_exec, s_found->data->type) != NULL) {
         MODE_exit ();
         DEBUG_USER   yLOG_info    ("keys"      , s_found->data->keys);
         if (strlen (s_found->data->keys) == 1 && s_found->data->keys [0] == '-') {
            DEBUG_USER   yLOG_note    ("legal menu option, but action not set");
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         SOURCE_menu_prep ();
         strlcpy (myVIKEYS.m_keys, s_found->data->keys, LEN_TITLE);
         DEBUG_USER   yLOG_info    ("m_keys"    , myVIKEYS.m_keys);
         yvikeys_macro_menu_beg ();
         myVIKEYS.log_keys = '-';
         yVIKEYS_main_string (myVIKEYS.m_keys);
         myVIKEYS.log_keys = 'y';
         yvikeys_macro_menu_end ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      } else if (s_found->data->type == MENU_GROUP) {
         DEBUG_USER   yLOG_note    ("group menu option, continue to next level");
      }
   }
   /*---(check exec)---------------------*/
   --rce;  if (n <= 0) {
      if (x_len - 1 >= 3) {
         DEBUG_USER   yLOG_note    ("menu failed to match in 3 levels, exiting");
         MODE_exit ();
      } else {
         DEBUG_USER   yLOG_note    ("illegal option selected, menu frozen");
      }
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       adding/updating items                  ----===*/
/*====================------------------------------------====================*/
static void  o___UPDATING________o () { return; }

char
yVIKEYS_menu_alter      (uchar *a_path, char *a_keys)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                                 <* 
    *> char        rce         =  -10;                                                          <* 
    *> char        rc          =    0;                                                          <* 
    *> int         n           =   -1;                                                          <* 
    *> int         x_len       =    0;                                                          <* 
    *> char        x_path      [LEN_LABEL];                                                     <* 
    *> char        x_dup       =    0;                                                          <* 
    *> int         x_level     =    0;                                                          <* 
    *> int         x_last      =    0;                                                          <* 
    *> int         i           =    0;                                                          <* 
    *> /+---(header)-------------------------+/                                                 <* 
    *> DEBUG_CMDS   yLOG_enter   (__FUNCTION__);                                                <* 
    *> /+---(defense)------------------------+/                                                 <* 
    *> DEBUG_CMDS   yLOG_note    ("before status check");                                       <* 
    *> --rce;  if (!STATUS_operational (MODE_COMMAND)) {                                        <* 
    *>    DEBUG_CMDS   yLOG_note    ("can not configure until operational");                    <* 
    *>    DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                        <* 
    *>    return rce;                                                                           <* 
    *> }                                                                                        <* 
    *> DEBUG_CMDS   yLOG_note    ("after status check");                                        <* 
    *> /+---(defense)------------------------+/                                                 <* 
    *> DEBUG_CMDS   yLOG_point   ("a_path"    , a_path);                                        <* 
    *> --rce;  if (a_path == NULL) {                                                            <* 
    *>    DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                        <* 
    *>    return rce;                                                                           <* 
    *> }                                                                                        <* 
    *> DEBUG_CMDS   yLOG_info    ("a_path"    , a_path);                                        <* 
    *> DEBUG_CMDS   yLOG_point   ("a_keys"    , a_keys);                                        <* 
    *> --rce;  if (a_keys == NULL) {                                                            <* 
    *>    DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                        <* 
    *>    return rce;                                                                           <* 
    *> }                                                                                        <* 
    *> DEBUG_CMDS   yLOG_info    ("a_keys"    , a_keys);                                        <* 
    *> /+---(path)---------------------------+/                                                 <* 
    *> x_len = strlen (a_path);                                                                 <* 
    *> DEBUG_CMDS   yLOG_value   ("x_len"     , x_len);                                         <* 
    *> --rce;  if (x_len < 3 || x_len > 4) {                                                    <* 
    *>    DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                        <* 
    *>    return rce;                                                                           <* 
    *> }                                                                                        <* 
    *> strlcpy (x_path, a_path, LEN_LABEL);                                                     <* 
    *> /+> n = yvikeys_menu_find (x_path, &x_level, &x_last);                             <+/   <* 
    *> DEBUG_CMDS   yLOG_value   ("find"      , n);                                             <* 
    *> --rce;  if (n < 0) {                                                                     <* 
    *>    DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                        <* 
    *>    return rce;                                                                           <* 
    *> }                                                                                        <* 
    *> /+---(if making a group)--------------+/                                                 <* 
    *> --rce;  if (strcmp (a_keys, ">") == 0) {                                                 <* 
    *>    if (x_level != 2) {                                                                   <* 
    *>       DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);                                     <* 
    *>       return rce;                                                                        <* 
    *>    }                                                                                     <* 
    *>    s_base [n].type   = MENU_GROUP;                                                       <* 
    *>    strlcpy (s_base [n].keys, "-", LEN_TITLE );                                            <* 
    *>    s_base [n].active = MENU_ACTIVE;                                                      <* 
    *> }                                                                                        <* 
    *> /+---(if making a executable)---------+/                                                 <* 
    *> else {                                                                                   <* 
    *>    s_base [n].type   = '·';                                                              <* 
    *>    strlcpy (s_base [n].keys, a_keys, LEN_TITLE );                                         <* 
    *>    if (x_level == 2) {                                                                   <* 
    *>       s_base [n].active = MENU_ACTIVE;                                                   <* 
    *>       i = s_base [n].start;                                                              <* 
    *>       while (i >= 0) {                                                                   <* 
    *>          s_base [i].top    = '·';                                                        <* 
    *>          s_base [i].mid    = '·';                                                        <* 
    *>          s_base [i].bot    = '·';                                                        <* 
    *>          i = s_base [i].next;                                                            <* 
    *>       }                                                                                  <* 
    *>       yvikeys__menu_compress ();                                                         <* 
   *>    }                                                                                     <* 
      *> }                                                                                        <* 
      *> /+---(update count)-------------------+/                                                 <* 
      *> DEBUG_CMDS   yLOG_value   ("SUCCESS"   , s_nmenu);                                       <* 
      *> yvikeys__menu_update (n);                                                                <* 
      *> /+---(complete)-----------------------+/                                                 <* 
      *> DEBUG_CMDS   yLOG_exit    (__FUNCTION__);                                                <* 
      *> return 0;                                                                                <*/
}

char
yVIKEYS_menu_add        (uchar *a_path, char *a_name, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   int         x_len       =    0;
   uchar       x_path      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_note    ("before status check");
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_CMDS   yLOG_note    ("can not configure until operational");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_note    ("after status check");
   /*---(defense)------------------------*/
   rc = yvikeys_menu__fix_path (a_path, &x_len, x_path);
   DEBUG_CMDS   yLOG_point   ("fix_path"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check in base)------------------*/
   rc = yvikeys_menu__in_base (x_path);
   DEBUG_CMDS   yLOG_value   ("in_base"   , rc);
   /*---(add from base)------------------*/
   if (rc == 1) {
      rc = yvikeys_menu__base_path (x_path);
      DEBUG_CMDS   yLOG_value   ("by_path"   , rc);
      --rce;  if (rc <  0) {
         DEBUG_CMDS   yLOG_note    ("adding failed, freeing menu item");
         return rce;
      }
      rc = 0;
   }
   /*---(add wholly new)-----------------*/
   else {
      rc = yvikeys_menu__newmenu (x_path, a_name, a_keys);
      DEBUG_CMDS   yLOG_value   ("new"       , rc);
      --rce;  if (rc <  0) {
         DEBUG_CMDS   yLOG_note    ("adding failed, freeing menu item");
         return rce;
      }
      rc = 1;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        displaying help                       ----===*/
/*====================------------------------------------====================*/
static void  o___HELP____________o () { return; }

static char s_help = '-';

char yVIKEYS_help        (void)        { return s_help; }

char yvikeys_help        (char a_type) {s_help = a_type; return 0;}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

void* yvikeys_menu_found      (uchar *a_path) { yvikeys_menu_menu (a_path);  return s_found; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__menu_unit      (char *a_question, char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         c           =    0;
   int         n           =   -1;
   char        x_level     =    0;
   int         x_last      =    0;
   char        t           [LEN_TITLE ];
   char        s           [LEN_LABEL] = "entry";
   tLINK      *x_curr      = NULL;
   tMENU      *p           = NULL;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MENU unit        : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "count"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU count       : %3dn, %3db, %3da, %3dg, %3dh, %3dt, %3dm, %3db", s_nmenu, s_nbase, s_nreal, s_ngrey, s_nhide, s_ntops, s_nmids, s_nbots);
   }
   else if (strcmp (a_question, "master"         )   == 0) {
      i      = a_path;
      x_curr = s_head;
      while (x_curr != NULL) {
         p = x_curr->data;
         if (c == i) break;
         x_curr = x_curr->m_next;
         ++c;
      }
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU master      : %-3d · · · -          · -          - %-10p %-10p %-10p   0", i, NULL, NULL, NULL);
      else {
         strlcpy    (t, p->keys, LEN_TITLE );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU master      : %-3d %c %c %c %-10.10s %c %-10.10s %c %-10p %-10p %-10p %3d",
               i, p->top, p->mid, p->bot, p->name, p->type, t,
               x_curr->active, x_curr, x_curr->s_next, x_curr->c_first, x_curr->c_count);
      }
   }
   else if (strcmp (a_question, "entry"          )   == 0) {
      yvikeys_menu_find (a_path);
      if (s_found == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU entry       : %-3d · · · -          · -          -", s_level);
      else {
         p = s_found->data;
         strlcpy    (t, p->keys , LEN_TITLE );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU entry       : %-3d %c %c %c %-10.10s %c %-10.10s %c",
               s_level, p->top, p->mid, p->bot, p->name, p->type, t, s_found->active);
      }
   }
   else if (strcmp (a_question, "found"          )   == 0) {
      if (s_found == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU found       : %-3d · · · -          · -          -", s_level);
      else {
         p = s_found->data;
         strlcpy    (t, p->keys , LEN_TITLE );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU found       : %-3d %c %c %c %-10.10s %c %-10.10s %c",
               s_level, p->top, p->mid, p->bot, p->name, p->type, t, s_found->active);
      }
   }
   else if (strcmp (a_question, "place"          )   == 0) {
      i = a_path;
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU place  (%2d) : %2da   %4dx   %4dy   %4dxo  %4dyo", i, s_places [i].align, s_places [i].x, s_places [i].y, s_places [i].xo, s_places [i].yo);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

