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
#define     MENU_DELETE      'd'  /* delete entry from heirarchy      */
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
   uchar       keys        [LEN_HUND ];     /* command or keys to execute     */
   /*---(done)--------------*/
};

static const tMENU  s_base [] = {
   /*---(root)---------------------------------------------------*/
   { MENU_BASE, '·', '·', '·', "ROOT"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   /*---(file menu)----------------------------------------------*/
   { MENU_BASE, 'f', '·', '·', "files"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'W', '·', "workspace"   , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'w', '·', "new"         , MENU_ACTIVE, MENU_CMD  , ":new¦"                          },
   { MENU_BASE, 'f', 'c', '·', "chdir"       , MENU_ACTIVE, MENU_MORE , ":cd·"                           },
   { MENU_BASE, 'f', 'b', '·', "browse"      , MENU_ACTIVE, MENU_MORE , ":browse·"                       },
   { MENU_BASE, 'f', 'n', '·', "name"        , MENU_ACTIVE, MENU_MORE , ":file·"                         },
   { MENU_BASE, 'f', 'o', '·', "read"        , MENU_ACTIVE, MENU_CMD  , ":read¦"                         },
   { MENU_BASE, 'f', 'm', '·', "import"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'f', '·', "refresh"     , MENU_ACTIVE, MENU_CMD  , ":refresh¦"                      },
   { MENU_BASE, 'f', 'r', '·', "restore"     , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'f', 'v', '·', "version"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'v', 'c', "ctrl"        , MENU_ACTIVE, MENU_CMD  , ":control¦"                      },
   { MENU_BASE, 'f', 'v', 'n', "noctrl"      , MENU_ACTIVE, MENU_CMD  , ":nocontrol¦"                    },
   { MENU_BASE, 'f', 'v', '1', "major"       , MENU_ACTIVE, MENU_CMD  , ":major¦"                        },
   { MENU_BASE, 'f', 'v', '2', "minor"       , MENU_ACTIVE, MENU_CMD  , ":minor¦"                        },
   { MENU_BASE, 'f', 'v', 't', "vertxt"      , MENU_ACTIVE, MENU_MORE , ":vertxt·"                       },
   { MENU_BASE, 'f', 'v', 'm', "manual"      , MENU_ACTIVE, MENU_MORE , ":vernum·"                       },
   { MENU_BASE, 'f', 'p', '·', "protect"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'u', '·', "update"      , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'f', 's', '·', "write"       , MENU_ACTIVE, MENU_CMD  , ":write¦"                        },
   { MENU_BASE, 'f', 'a', '·', "writeas"     , MENU_ACTIVE, MENU_MORE , ":writeas·"                      },
   { MENU_BASE, 'f', 'x', '·', "export"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'f', 'q', '·', "quit"        , MENU_ACTIVE, MENU_CMD  , ":quit¦"                         },
   /*---(edit menu)----------------------------------------------*/
   { MENU_BASE, 'e', '·', '·', "edit"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'u', '·', "undo"        , MENU_GREY  , MENU_EXACT, "u"                              },
   { MENU_BASE, 'e', 'U', '·', "redo"        , MENU_GREY  , MENU_EXACT, "U"                              },
   { MENU_BASE, 'e', '_', '·', "status"      , MENU_GREY  , MENU_EXACT, ":status mundo¦"                 },
   { MENU_BASE, 'e', '?', '·', "history"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', '?', 't', "track"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', '?', 'u', "untrack"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', '?', 'c', "clear"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', '?', 'r', "review"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'y', '·', "copy"        , MENU_GREY  , MENU_EXACT, "y"                              },
   { MENU_BASE, 'e', 'Y', '·', "cut"         , MENU_GREY  , MENU_EXACT, "Y"                              },
   { MENU_BASE, 'e', 'p', '·', "paste"       , MENU_GREY  , MENU_EXACT, "p"                              },
   { MENU_BASE, 'e', 'P', '·', "special"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'x', '·', "clear"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'x', '.', "inplace"     , MENU_GREY  , MENU_EXACT, "x."                             },
   { MENU_BASE, 'e', 'x', 'h', "left"        , MENU_GREY  , MENU_EXACT, "xh"                             },
   { MENU_BASE, 'e', 'x', 'l', "right"       , MENU_GREY  , MENU_EXACT, "xl"                             },
   { MENU_BASE, 'e', 'x', 'k', "up"          , MENU_GREY  , MENU_EXACT, "xk"                             },
   { MENU_BASE, 'e', 'x', 'j', "down"        , MENU_GREY  , MENU_EXACT, "xj"                             },
   { MENU_BASE, 'e', 'x', 'i', "in"          , MENU_GREY  , MENU_EXACT, "xi"                             },
   { MENU_BASE, 'e', 'x', 'o', "out"         , MENU_GREY  , MENU_EXACT, "xo"                             },
   { MENU_BASE, 'e', 'x', 'x', "col_lef"     , MENU_GREY  , MENU_EXACT, "xx"                             },
   { MENU_BASE, 'e', 'x', 'X', "col_rig"     , MENU_GREY  , MENU_EXACT, "xX"                             },
   { MENU_BASE, 'e', 'x', 'y', "row_abo"     , MENU_GREY  , MENU_EXACT, "xy"                             },
   { MENU_BASE, 'e', 'x', 'Y', "row_bel"     , MENU_GREY  , MENU_EXACT, "xY"                             },
   { MENU_BASE, 'e', 'x', 'z', "lvl_in"      , MENU_GREY  , MENU_EXACT, "xz"                             },
   { MENU_BASE, 'e', 'x', 'Z', "lvl_out"     , MENU_GREY  , MENU_EXACT, "xZ"                             },
   { MENU_BASE, 'e', 'd', '·', "delete"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'd', 'l', "left"        , MENU_GREY  , MENU_EXACT, "dl"                             },
   { MENU_BASE, 'e', 'd', 'h', "right"       , MENU_GREY  , MENU_EXACT, "dh"                             },
   { MENU_BASE, 'e', 'd', 'k', "up"          , MENU_GREY  , MENU_EXACT, "dk"                             },
   { MENU_BASE, 'e', 'd', 'j', "down"        , MENU_GREY  , MENU_EXACT, "dj"                             },
   { MENU_BASE, 'e', 'd', 'i', "in"          , MENU_GREY  , MENU_EXACT, "di"                             },
   { MENU_BASE, 'e', 'd', 'o', "out"         , MENU_GREY  , MENU_EXACT, "do"                             },
   { MENU_BASE, 'e', 'd', 'x', "col_left"    , MENU_GREY  , MENU_EXACT, "dx"                             },
   { MENU_BASE, 'e', 'd', 'X', "col_right"   , MENU_GREY  , MENU_EXACT, "dX"                             },
   { MENU_BASE, 'e', 'd', 'y', "row_above"   , MENU_GREY  , MENU_EXACT, "dy"                             },
   { MENU_BASE, 'e', 'd', 'Y', "row_below"   , MENU_GREY  , MENU_EXACT, "dY"                             },
   { MENU_BASE, 'e', 'd', 'z', "lvl_in"      , MENU_GREY  , MENU_EXACT, "dz"                             },
   { MENU_BASE, 'e', 'd', 'Z', "lvl_out"     , MENU_GREY  , MENU_EXACT, "dZ"                             },
   { MENU_BASE, 'e', 'i', '·', "insert"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'i', 'l', "left"        , MENU_GREY  , MENU_EXACT, "al"                             },
   { MENU_BASE, 'e', 'i', 'h', "right"       , MENU_GREY  , MENU_EXACT, "ah"                             },
   { MENU_BASE, 'e', 'i', 'k', "up"          , MENU_GREY  , MENU_EXACT, "ak"                             },
   { MENU_BASE, 'e', 'i', 'j', "down"        , MENU_GREY  , MENU_EXACT, "aj"                             },
   { MENU_BASE, 'e', 'i', 'i', "in"          , MENU_GREY  , MENU_EXACT, "ai"                             },
   { MENU_BASE, 'e', 'i', 'o', "out"         , MENU_GREY  , MENU_EXACT, "ao"                             },
   { MENU_BASE, 'e', 'i', 'x', "col_lef"     , MENU_GREY  , MENU_EXACT, "ax"                             },
   { MENU_BASE, 'e', 'i', 'X', "col_rig"     , MENU_GREY  , MENU_EXACT, "aX"                             },
   { MENU_BASE, 'e', 'i', 'y', "row_abo"     , MENU_GREY  , MENU_EXACT, "ay"                             },
   { MENU_BASE, 'e', 'i', 'Y', "row_bel"     , MENU_GREY  , MENU_EXACT, "aY"                             },
   { MENU_BASE, 'e', 'i', 'z', "lvl_in"      , MENU_GREY  , MENU_EXACT, "az"                             },
   { MENU_BASE, 'e', 'i', 'Z', "lvl_out"     , MENU_GREY  , MENU_EXACT, "aZ"                             },
   { MENU_BASE, 'e', 'g', '·', "group"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'g', 'g', "group"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'g', 'u', "ungroup"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'g', 'r', "regroup"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'g', 'm', "merge"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'g', 'f', "free"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'g', 'b', "boundary"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', '·', "align"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'a', 'k', "top"         , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'j', "bottom"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'h', "left"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'l', "right"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'o', "front"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'i', "back"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'x', "dist-x"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'y', "dist-y"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'a', 'z', "dist-z"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'o', '·', "order"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'e', 'o', 'n', "front"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'o', 'i', "forward"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'o', 'o', "backward"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'o', 'f', "back"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'o', 'r', "reverse"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'e', 'l', '·', "layer"       , MENU_GREY  , MENU_MORE , "-"                              },
   /*---(view menu)-----------------------------------------------------------*/
   { MENU_BASE, 'v', '·', '·', "view"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'w', '·', "window"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'l', '·', "layout"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'l', 'n', "minimal"     , MENU_ACTIVE, MENU_CMD  , ":layout···min¦"                 },
   { MENU_BASE, 'v', 'l', 'n', "work"        , MENU_ACTIVE, MENU_CMD  , ":layout···work¦"                },
   { MENU_BASE, 'v', 'l', 'n', "gyges"       , MENU_ACTIVE, MENU_CMD  , ":layout···gyges¦"               },
   { MENU_BASE, 'v', 'l', 'x', "maximum"     , MENU_ACTIVE, MENU_CMD  , ":layout···max¦"                 },
   { MENU_BASE, 'v', 't', '·', "title"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 't', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":title····show¦"                },
   { MENU_BASE, 'v', 't', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":title····hide¦"                },
   { MENU_BASE, 'v', 't', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":title····enable¦"              },
   { MENU_BASE, 'v', 't', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":title····disable¦"             },
   { MENU_BASE, 'v', 'v', '·', "version"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'v', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":version··show¦"                },
   { MENU_BASE, 'v', 'v', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":version··hide¦"                },
   { MENU_BASE, 'v', 'v', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":version··enable¦"              },
   { MENU_BASE, 'v', 'v', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":version··disable¦"             },
   { MENU_BASE, 'v', 'b', '·', "buffer"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'b', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":buffer···show¦"                },
   { MENU_BASE, 'v', 'b', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":buffer···hide¦"                },
   { MENU_BASE, 'v', 'b', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":buffer···enable¦"              },
   { MENU_BASE, 'v', 'b', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":buffer···disable¦"             },
   { MENU_BASE, 'v', 'f', '·', "formula"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'f', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":formula··show¦"                },
   { MENU_BASE, 'v', 'f', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":formula··hide¦"                },
   { MENU_BASE, 'v', 'f', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":formula··enable¦"              },
   { MENU_BASE, 'v', 'f', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":formula··disable¦"             },
   { MENU_BASE, 'v', 'X', '·', "xaxis"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'X', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":xaxis····show¦"                },
   { MENU_BASE, 'v', 'X', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":xaxis····hide¦"                },
   { MENU_BASE, 'v', 'X', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":xaxis····enable¦"              },
   { MENU_BASE, 'v', 'X', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":xaxis····disable¦"             },
   { MENU_BASE, 'v', 'Y', '·', "yaxis"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'Y', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":yaxis····show¦"                },
   { MENU_BASE, 'v', 'Y', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":yaxis····hide¦"                },
   { MENU_BASE, 'v', 'Y', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":yaxis····enable¦"              },
   { MENU_BASE, 'v', 'Y', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":yaxis····disable¦"             },
   { MENU_BASE, 'v', 'a', '·', "alt"         , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'a', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":alt······show¦"                },
   { MENU_BASE, 'v', 'a', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":alt······hide¦"                },
   { MENU_BASE, 'v', 'a', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":alt······enable¦"              },
   { MENU_BASE, 'v', 'a', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":alt······disable¦"             },
   { MENU_BASE, 'v', 'd', '·', "details"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'd', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":details··show¦"                },
   { MENU_BASE, 'v', 'd', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":details··hide¦"                },
   { MENU_BASE, 'v', 'd', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":details··enable¦"              },
   { MENU_BASE, 'v', 'd', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":details··disable¦"             },
   { MENU_BASE, 'v', 'r', '·', "ribbon"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'r', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":ribbon···show¦"                },
   { MENU_BASE, 'v', 'r', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":ribbon···hide¦"                },
   { MENU_BASE, 'v', 'r', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":ribbon···enable¦"              },
   { MENU_BASE, 'v', 'r', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":ribbon···disable¦"             },
   { MENU_BASE, 'v', 'p', '·', "progress"    , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'p', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":progress·show¦"                },
   { MENU_BASE, 'v', 'p', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":progress·hide¦"                },
   { MENU_BASE, 'v', 'p', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":progress·enable¦"              },
   { MENU_BASE, 'v', 'p', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":progress·disable¦"             },
   { MENU_BASE, 'v', 'x', '·', "modes"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'x', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":modes····show¦"                },
   { MENU_BASE, 'v', 'x', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":modes····hide¦"                },
   { MENU_BASE, 'v', 'x', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":modes····enable¦"              },
   { MENU_BASE, 'v', 'x', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":modes····disable¦"             },
   { MENU_BASE, 'v', 's', '·', "status"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 's', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":status···show¦"                },
   { MENU_BASE, 'v', 's', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":status···hide¦"                },
   { MENU_BASE, 'v', 's', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":status···enable¦"              },
   { MENU_BASE, 'v', 's', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":status···disable¦"             },
   { MENU_BASE, 'v', 'c', '·', "command"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'c', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":command··show¦"                },
   { MENU_BASE, 'v', 'c', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":command··hide¦"                },
   { MENU_BASE, 'v', 'c', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":command··enable¦"              },
   { MENU_BASE, 'v', 'c', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":command··disable¦"             },
   { MENU_BASE, 'v', 'k', '·', "keys"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'k', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":keys·····show¦"                },
   { MENU_BASE, 'v', 'k', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":keys·····hide¦"                },
   { MENU_BASE, 'v', 'k', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":keys·····enable¦"              },
   { MENU_BASE, 'v', 'k', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":keys·····disable¦"             },
   { MENU_BASE, 'v', 'C', '·', "cursor"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'C', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":cursor···show¦"                },
   { MENU_BASE, 'v', 'C', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":cursor···hide¦"                },
   { MENU_BASE, 'v', 'C', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":cursor···enable¦"              },
   { MENU_BASE, 'v', 'C', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":cursor···disable¦"             },
   { MENU_BASE, 'v', 'G', '·', "grid"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'G', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":grid·····show¦"                },
   { MENU_BASE, 'v', 'G', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":grid·····hide¦"                },
   { MENU_BASE, 'v', 'G', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":grid·····enable¦"              },
   { MENU_BASE, 'v', 'G', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":grid·····disable¦"             },
   { MENU_BASE, 'v', 'E', '·', "edges"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'E', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":edges····show¦"                },
   { MENU_BASE, 'v', 'E', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":edges····hide¦"                },
   { MENU_BASE, 'v', 'E', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":edges····enable¦"              },
   { MENU_BASE, 'v', 'E', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":edges····disable¦"             },
   { MENU_BASE, 'v', 'U', '·', "guides"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'U', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":guides···show¦"                },
   { MENU_BASE, 'v', 'U', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":guides···hide¦"                },
   { MENU_BASE, 'v', 'U', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":guides···enable¦"              },
   { MENU_BASE, 'v', 'U', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":guides···disable¦"             },
   { MENU_BASE, 'v', 'O', '·', "overlay"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'O', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":overlay··show¦"                },
   { MENU_BASE, 'v', 'O', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":overlay··hide¦"                },
   { MENU_BASE, 'v', 'O', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":overlay··enable¦"              },
   { MENU_BASE, 'v', 'O', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":overlay··disable¦"             },
   { MENU_BASE, 'v', 'L', '·', "layers"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'L', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":layers···show¦"                },
   { MENU_BASE, 'v', 'L', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":layers···hide¦"                },
   { MENU_BASE, 'v', 'L', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":layers···enable¦"              },
   { MENU_BASE, 'v', 'L', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":layers···disable¦"             },
   { MENU_BASE, 'v', 'N', '·', "notes"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'N', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":notes····show¦"                },
   { MENU_BASE, 'v', 'N', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":notes····hide¦"                },
   { MENU_BASE, 'v', 'N', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":notes····enable¦"              },
   { MENU_BASE, 'v', 'N', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":notes····disable¦"             },
   { MENU_BASE, 'v', 'P', '·', "color"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'P', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":color····show¦"                },
   { MENU_BASE, 'v', 'P', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":color····hide¦"                },
   { MENU_BASE, 'v', 'P', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":color····enable¦"              },
   { MENU_BASE, 'v', 'P', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":color····disable¦"             },
   { MENU_BASE, 'v', 'B', '·', "back"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'B', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":back·····show¦"                },
   { MENU_BASE, 'v', 'B', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":back·····hide¦"                },
   { MENU_BASE, 'v', 'B', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":back·····enable¦"              },
   { MENU_BASE, 'v', 'B', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":back·····disable¦"             },
   { MENU_BASE, 'v', 'M', '·', "menu"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'M', '1', "top-lef"     , MENU_ACTIVE, MENU_CMD  , ":menu·····1¦"                   },
   { MENU_BASE, 'v', 'M', '2', "top-cen"     , MENU_ACTIVE, MENU_CMD  , ":menu·····2¦"                   },
   { MENU_BASE, 'v', 'M', '3', "top-rig"     , MENU_ACTIVE, MENU_CMD  , ":menu·····3¦"                   },
   { MENU_BASE, 'v', 'M', '4', "mid-lef"     , MENU_ACTIVE, MENU_CMD  , ":menu·····4¦"                   },
   { MENU_BASE, 'v', 'M', '5', "mid-cen"     , MENU_ACTIVE, MENU_CMD  , ":menu·····5¦"                   },
   { MENU_BASE, 'v', 'M', '6', "mid-rig"     , MENU_ACTIVE, MENU_CMD  , ":menu·····6¦"                   },
   { MENU_BASE, 'v', 'M', '7', "bot-lef"     , MENU_ACTIVE, MENU_CMD  , ":menu·····7¦"                   },
   { MENU_BASE, 'v', 'M', '8', "bot-cen"     , MENU_ACTIVE, MENU_CMD  , ":menu·····8¦"                   },
   { MENU_BASE, 'v', 'M', '9', "bot-rig"     , MENU_ACTIVE, MENU_CMD  , ":menu·····9¦"                   },
   { MENU_BASE, 'v', 'H', '·', "history"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'v', 'H', 's', "show"        , MENU_ACTIVE, MENU_CMD  , ":history··show¦"                },
   { MENU_BASE, 'v', 'H', 'h', "hide"        , MENU_ACTIVE, MENU_CMD  , ":history··hide¦"                },
   { MENU_BASE, 'v', 'H', 'e', "enable"      , MENU_ACTIVE, MENU_CMD  , ":history··enable¦"              },
   { MENU_BASE, 'v', 'H', 'd', "disable"     , MENU_ACTIVE, MENU_CMD  , ":history··disable¦"             },
   { MENU_BASE, 'v', '?', '·', "help"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   /*---(insert menu)--------------------------------------------*/
   { MENU_BASE, 'i', '·', '·', "insert"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 't', '·', "text"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'i', '·', "image"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'l', '·', "lines"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'e', '·', "ellises"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 't', '·', "tris"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'r', '·', "rects"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'h', '·', "hexagons"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'p', '·', "polygons"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 's', '·', "stars"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'a', '·', "arrows"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'f', '·', "flowchart"   , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'j', '·', "joiners"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'n', '·', "notes"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'm', '·', "misc"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', '-', '·', "nurbs"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', '-', '·', "tiling"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', 'b', '·', "beziers"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', '-', '·', "3d"          , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', '-', '·', "surfaces"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'i', '-', '·', "meshes"      , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(select menu)--------------------------------------------*/
   { MENU_BASE, 's', '·', '·', "select"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 's', '!', '·', "highlight"   , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', '_', '·', "status"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'c', '·', "clear"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'a', '·', "all"         , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'b', '·', "buffer"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'w', '·', "window"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'l', '·', "layer"       , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'g', '·', "geometry"    , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 's', 't', '·', "type"        , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 's', 'x', '·', "regex"       , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 's', 'u', '·', "touching"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 's', '·', "save"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'r', '·', "reselect"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'j', '·', "join"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'd', '·', "deselect"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'i', '·', "inverse"     , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'x', '·', "all_on_x"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'y', '·', "all_on_y"    , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 's', 'z', '·', "all_on_z"    , MENU_GREY  , MENU_EXACT, "-"                              },
   /*---(format menu)--------------------------------------------*/
   { MENU_BASE, 'o', '·', '·', "format"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'f', '·', "face"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'p', '·', "point"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 's', '·', "spacing"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'i', '·', "indent"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'c', '·', "color"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'a', '·', "align"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'n', '·', "numbers"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'z', '·', "sfill"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'd', '·', "decimals"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'u', '·', "units"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'l', '·', "lists"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'o', 'y', '·', "style"       , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(modify menu)--------------------------------------------*/
   { MENU_BASE, 'm', '·', '·', "modify"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'è', '·', "resize"      , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 'm', 'é', '·', "dims"        , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 'm', 'ê', '·', "extent"      , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 'm', 'ë', '·', "scale"       , MENU_GREY  , MENU_MORE , "-"                              },
   { MENU_BASE, 'm', 'ì', '·', "trim"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'í', '·', "join"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'î', '·', "move"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'm', 'ï', '·', "array"       , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'm', 'ð', '·', "rotate"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'ñ', '·', "snap"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'm', 'ò', '·', "fill"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'ó', '·', "hatching"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'ô', '·', "outline"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'm', 'õ', '·', "centers"     , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'm', 'ö', '·', "ends"        , MENU_GREY  , MENU_TBD  , "-"                              },
   /*---(dataset menu)-------------------------------------------*/
   { MENU_BASE, 'd', '·', '·', "dataset"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'd', 's', '·', "sort"        , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'd', 'f', '·', "filter"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'd', '-', '·', "blur"        , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "enhance"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "distort"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "noise"       , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "edges"       , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "combine"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "light"       , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "shadow"      , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "pixelate"    , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "render"      , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "sharpen"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "smooth"      , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "stylize"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "liquify"     , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "oils"        , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "map"         , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "decor"       , MENU_ACTIVE, MENU_TBD  , "-"                              },
   { MENU_BASE, 'd', '-', '·', "vanish"      , MENU_ACTIVE, MENU_TBD  , "-"                              },
   /*---(tools menu)---------------------------------------------*/
   { MENU_BASE, 't', '·', '·', "tools"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "pen"         , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "callig"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "pencil"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "airbrush"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "chalk"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "brush"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "roller"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "spray"       , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "fill"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "sponge"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "blob"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "blur"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "sharpen"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "smudge"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "burn"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "eraser"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "magic_e"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "healer"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "highlight"   , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "zoom"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "move"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 't', '-', '·', "lasso"       , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(pallette menu)------------------------------------------*/
   { MENU_BASE, 'p', '·', '·', "palette"     , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(layers menu)--------------------------------------------*/
   { MENU_BASE, 'l', '·', '·', "layers"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'l', '-', '·', "saved"       , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "highlight"   , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "status"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "new"         , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "rename"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "copy_all"    , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "copy_with"   , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "join"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "flatten"     , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "delete"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "type"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "hide"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "mask"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "alignment"   , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'l', '-', '·', "locking"     , MENU_GREY  , MENU_TBD  , "-"                              },
   /*---(buffers menu)-------------------------------------------*/
   { MENU_BASE, 'b', '·', '·', "buffers"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'b', '_', '·', "status"      , MENU_GREY  , MENU_EXACT, ":status buffer¦"                },
   { MENU_BASE, 'b', 'x', '·', "maximum"     , MENU_GREY  , MENU_MORE , ":bmax·"                         },
   { MENU_BASE, 'b', 'b', '·', "browse"      , MENU_GREY  , MENU_EXACT, ":bbrowse·"                      },
   { MENU_BASE, 'b', 'g', '·', "goto"        , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'b', 'g', 'a', "abbrev"      , MENU_GREY  , MENU_MORE , ":buf·"                          },
   { MENU_BASE, 'b', 'g', 'r', "return"      , MENU_GREY  , MENU_EXACT, ",,"                             },
   { MENU_BASE, 'b', 'g', 'f', "first"       , MENU_GREY  , MENU_EXACT, ",["                             },
   { MENU_BASE, 'b', 'g', 'p', "prev"        , MENU_GREY  , MENU_EXACT, ",<"                             },
   { MENU_BASE, 'b', 'g', 'n', "next"        , MENU_GREY  , MENU_EXACT, ",>"                             },
   { MENU_BASE, 'b', 'g', 'l', "last"        , MENU_GREY  , MENU_EXACT, ",]"                             },
   { MENU_BASE, 'b', 'g', 'u', "summary"     , MENU_GREY  , MENU_EXACT, ",®"                             },
   { MENU_BASE, 'b', 'g', 'y', "system"      , MENU_GREY  , MENU_EXACT, ",¯"                             },
   { MENU_BASE, 'b', '-', '·', "showall"     , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', 'w', '·', "new"         , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', 'n', '·', "rename"      , MENU_GREY  , MENU_MORE , ":btitle·"                       },
   { MENU_BASE, 'b', 'z', '·', "size"        , MENU_GREY  , MENU_MORE , ":bsize·"                        },
   { MENU_BASE, 'b', 't', '·', "type"        , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', 'f', '·', "defaults"    , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'b', 'f', 'w', "col_wide"    , MENU_GREY  , MENU_MORE , ":defwide·"                      },
   { MENU_BASE, 'b', 'f', 't', "row_tall"    , MENU_GREY  , MENU_MORE , ":deftall·"                      },
   { MENU_BASE, 'b', 'f', 'd', "dep_thick"   , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "scale"       , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "delete"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "freeze"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "split"       , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "hiding"      , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'b', '-', '·', "locking"     , MENU_GREY  , MENU_TBD  , "-"                              },
   /*---(snippet menu)-------------------------------------------*/
   { MENU_BASE, 'g', '·', '·', "language"    , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(auditor menu)-------------------------------------------*/
   { MENU_BASE, 'a', '·', '·', "auditor"     , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'a', 's', '·', "spell"       , MENU_GREY  , MENU_TBD  , "-"                              },
   { MENU_BASE, 'a', 'd', '·', "dump"        , MENU_GREY  , MENU_TBD  , "-"                              },
   /*---(execute menu)-------------------------------------------*/
   { MENU_BASE, 'x', '·', '·', "execute"     , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(config menu)--------------------------------------------*/
   { MENU_BASE, 'c', '·', '·', "config"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'c', 's', '·', "search"      , MENU_GREY  , MENU_GROUP, "-"                              },
   { MENU_BASE, 'c', 's', 'r', "reset all"   , MENU_GREY  , MENU_MORE , "-"                              },
   /*---(macro menu)---------------------------------------------*/
   { MENU_BASE, 'r', '·', '·', "script"      , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'r', 'e', '·', "execute"     , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'r', 'e', '0', "blitz"       , MENU_ACTIVE, MENU_CMD  , ":edelay 0¦"                     },
   { MENU_BASE, 'r', 'e', '1', "fast"        , MENU_ACTIVE, MENU_CMD  , ":edelay 1¦"                     },
   { MENU_BASE, 'r', 'e', '2', "thousand"    , MENU_ACTIVE, MENU_CMD  , ":edelay 2¦"                     },
   { MENU_BASE, 'r', 'e', '3', "hundred"     , MENU_ACTIVE, MENU_CMD  , ":edelay 3¦"                     },
   { MENU_BASE, 'r', 'e', '4', "twenty"      , MENU_ACTIVE, MENU_CMD  , ":edelay 4¦"                     },
   { MENU_BASE, 'r', 'e', '5', "tenth"       , MENU_ACTIVE, MENU_CMD  , ":edelay 5¦"                     },
   { MENU_BASE, 'r', 'e', '6', "half"        , MENU_ACTIVE, MENU_CMD  , ":edelay 6¦"                     },
   { MENU_BASE, 'r', 'e', '7', "second"      , MENU_ACTIVE, MENU_CMD  , ":edelay 7¦"                     },
   { MENU_BASE, 'r', 'e', '8', "double"      , MENU_ACTIVE, MENU_CMD  , ":edelay 8¦"                     },
   { MENU_BASE, 'r', 'e', '9', "triple"      , MENU_ACTIVE, MENU_CMD  , ":edelay 9¦"                     },
   { MENU_BASE, 'r', 'e', '+', "faster"      , MENU_ACTIVE, MENU_CMD  , ":edelay +¦"                     },
   { MENU_BASE, 'r', 'e', '-', "slower"      , MENU_ACTIVE, MENU_CMD  , ":edelay -¦"                     },
   { MENU_BASE, 'r', 'e', 'n', "normal"      , MENU_ACTIVE, MENU_CMD  , ":eupdate n¦"                    },
   { MENU_BASE, 'r', 'e', 's', "slow"        , MENU_ACTIVE, MENU_CMD  , ":eupdate s¦"                    },
   { MENU_BASE, 'r', 'e', 'b', "blinks"      , MENU_ACTIVE, MENU_CMD  , ":eupdate b¦"                    },
   { MENU_BASE, 'r', 'e', 'p', "peeks"       , MENU_ACTIVE, MENU_CMD  , ":eupdate p¦"                    },
   { MENU_BASE, 'r', 'e', 'd', "blind"       , MENU_ACTIVE, MENU_CMD  , ":eupdate d¦"                    },
   { MENU_BASE, 'r', 'e', 'D', "demo mode"   , MENU_ACTIVE, MENU_CMD  , ":edelay 4¦"                     },
   { MENU_BASE, 'r', 'd', '·', "debug"       , MENU_ACTIVE, MENU_GROUP, "-"                              },
   { MENU_BASE, 'r', 'd', '0', "blitz"       , MENU_ACTIVE, MENU_CMD  , ":ddelay 0¦"                     },
   { MENU_BASE, 'r', 'd', '1', "fast"        , MENU_ACTIVE, MENU_CMD  , ":ddelay 1¦"                     },
   { MENU_BASE, 'r', 'd', '2', "thousand"    , MENU_ACTIVE, MENU_CMD  , ":ddelay 2¦"                     },
   { MENU_BASE, 'r', 'd', '3', "hundred"     , MENU_ACTIVE, MENU_CMD  , ":ddelay 3¦"                     },
   { MENU_BASE, 'r', 'd', '4', "twenty"      , MENU_ACTIVE, MENU_CMD  , ":ddelay 4¦"                     },
   { MENU_BASE, 'r', 'd', '5', "tenth"       , MENU_ACTIVE, MENU_CMD  , ":ddelay 5¦"                     },
   { MENU_BASE, 'r', 'd', '6', "half"        , MENU_ACTIVE, MENU_CMD  , ":ddelay 6¦"                     },
   { MENU_BASE, 'r', 'd', '7', "second"      , MENU_ACTIVE, MENU_CMD  , ":ddelay 7¦"                     },
   { MENU_BASE, 'r', 'd', '8', "double"      , MENU_ACTIVE, MENU_CMD  , ":ddelay 8¦"                     },
   { MENU_BASE, 'r', 'd', '9', "triple"      , MENU_ACTIVE, MENU_CMD  , ":ddelay 9¦"                     },
   { MENU_BASE, 'r', 'd', '+', "faster"      , MENU_ACTIVE, MENU_CMD  , ":ddelay +¦"                     },
   { MENU_BASE, 'r', 'd', '-', "slower"      , MENU_ACTIVE, MENU_CMD  , ":ddelay -¦"                     },
   { MENU_BASE, 'r', 'd', 'n', "normal"      , MENU_ACTIVE, MENU_CMD  , ":dupdate n¦"                    },
   { MENU_BASE, 'r', 'd', 's', "slow"        , MENU_ACTIVE, MENU_CMD  , ":dupdate s¦"                    },
   { MENU_BASE, 'r', 'd', 'b', "blinks"      , MENU_ACTIVE, MENU_CMD  , ":dupdate b¦"                    },
   { MENU_BASE, 'r', 'd', 'p', "peeks"       , MENU_ACTIVE, MENU_CMD  , ":dupdate p¦"                    },
   { MENU_BASE, 'r', 'd', 'd', "blind"       , MENU_ACTIVE, MENU_CMD  , ":dupdate d¦"                    },
   { MENU_BASE, 'r', 'w', '·', "new"         , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 'o', '·', "open"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 'm', '·', "import"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 's', '·', "save"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 'x', '·', "export"      , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 'i', '·', "list"        , MENU_GREY  , MENU_EXACT, "-"                              },
   { MENU_BASE, 'r', 'g', '·', "globals"     , MENU_GREY  , MENU_EXACT, "-"                              },
   /*---(share menu)---------------------------------------------*/
   { MENU_BASE, 'h', '·', '·', "share"       , MENU_GREY  , MENU_GROUP, "-"                              },
   /*---(footer)-------------------------------------------------*/
   {  0       ,  0 ,  0 ,  0 , NULL          , 0          , 0         , NULL                             },
   /*---(done)---------------------------------------------------*/
};



typedef    struct   cMLINK  tMLINK;
struct cMLINK {
   char        active;                      /* current active status          */
   tMENU      *data;                        /* pointer to menu entry          */
   tMLINK     *m_next;                      /* master list next               */
   tMLINK     *m_prev;                      /* master list prev               */
   tMLINK     *s_next;                      /* next sibling                   */
   tMLINK     *c_first;                     /* first child                    */
   uchar       c_count;                     /* count of children              */
};

/*---(true useful vars)---------------*/
static tMLINK *s_head   = NULL;              /* head of link chain            */
static tMLINK *s_tail   = NULL;              /* tail of link chain            */
static char    s_config = 'x';               /* how much base to load         */
/*---(menu grphics/unit testing)------*/
static tMLINK *s_parent = NULL;              /* result of last find           */
static tMLINK *s_found  = NULL;              /* result of last find           */
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
   tMLINK     *x_new       = NULL;
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
      x_new = (tMLINK *) malloc (sizeof (tMLINK));
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
   strlcpy (x_new->keys, a_keys, LEN_HUND );
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

char
yvikeys_menu__delete    (tMLINK *a_parent, tMLINK *a_sib, tMLINK *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_sint    (a_curr->c_count);
   --rce;  if (a_curr->c_count > 0) {
      DEBUG_CMDS   yLOG_snote   ("got children, can not delete");
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(take out of master)-------------*/
   if (a_curr->m_prev != NULL)  a_curr->m_prev->m_next = a_curr->m_next;
   else                         s_head                 = a_curr->m_next;
   if (a_curr->m_next == NULL)  s_tail                 = a_curr->m_prev;
   /*---(update counts)------------------*/
   --s_nmenu;
   if (a_curr->active      == MENU_ACTIVE)  --s_nreal;
   if (a_curr->data->base  == MENU_BASE)    --s_nbase;
   DEBUG_CMDS   yLOG_sint    (s_nmenu);
   DEBUG_CMDS   yLOG_sint    (s_nreal);
   DEBUG_CMDS   yLOG_sint    (s_nbase);
   /*---(take out of heirarchy)----------*/
   if (a_sib  != NULL)          a_sib->s_next     = a_curr->s_next;
   else                         a_parent->c_first = NULL;
   --a_parent->c_count;
   DEBUG_CMDS   yLOG_sint    (a_parent->c_count);
   /*---(free)---------------------------*/
   free (a_curr);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    hierarchy actions                         ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

int
yvikeys_menu__action    (uchar *a_path, int a_max, int a_lvl, tMLINK *a_parent, char a_act, tMLINK *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   tMLINK     *x_curr      = NULL;
   tMLINK     *x_sib       = NULL;
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
         case MENU_DELETE  :
            DEBUG_CMDS   yLOG_note    ("deleting current");
            rc = yvikeys_menu__delete (a_parent, x_sib, x_curr);
            ++c;
            break;
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
yvikeys_menu_action     (char a_act, uchar *a_path, tMLINK *a_link)
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

char yvikeys_menu_place   (tMLINK *a_new) { return yvikeys_menu_action (MENU_PLACE , NULL  , a_new); }
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
   tMLINK     *x_curr      = NULL;
   tMLINK     *x_prev      = NULL;
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
yvikeys_menu_reanchor     (int a_anchor)
{
   return yvikeys_view_reanchor (YVIKEYS_MENUS, a_anchor);
}

char
yvikeys_menu_final      (void)
{
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   yVIKEYS_cmds_add (YVIKEYS_M_VIEW  , "help"        , ""    , "c"    , yvikeys_help               , "" );
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
   /*---(add the root)-------------------*/
   s_head = s_tail = NULL;
   yvikeys_menu__newlink (s_base);
   /*---(handle total)-------------------*/
   yvikeys_menu__base_load ();
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
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        drawing menus                         ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }


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
static float   s_z      =  90.0;
static int     s_pt     = 8;
static int     s_xoff   = 15.0;
static int     s_yoff   =  0.0;

char
yvikeys_menu__gl_center (int a_adj)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_xmin, x_xmax, x_ymin, x_ymax;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   yVIKEYS_view_bounds (YVIKEYS_MAIN , &x_xmin, &x_xmax, NULL, &x_ymin, &x_ymax, NULL);
   DEBUG_CMDS   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy, %3dz", x_xmin, x_xmax, x_ymin, x_ymax, s_z);
   s_anchor = yVIKEYS_view_anchor (YVIKEYS_MENUS);
   /*---(set x center)----------------*/
   switch (s_anchor) {
   case YVIKEYS_TOPRIG : case YVIKEYS_MIDRIG : case YVIKEYS_BOTRIG   :
      if (s_type == YVIKEYS_DEPTH)   s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0 + a_adj;
      else                           s_ce  = x_xmax - s_x;
      break;
   case YVIKEYS_TOPCEN : case YVIKEYS_MIDCEN : case YVIKEYS_BOTCEN   :
      s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0;
      break;
   case YVIKEYS_TOPLEF : case YVIKEYS_MIDLEF : case YVIKEYS_BOTLEF   :
      if (s_type == YVIKEYS_DEPTH)   s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0 - a_adj;
      else                           s_ce  = x_xmin + s_x;
      break;
   }
   DEBUG_CMDS   yLOG_value   ("s_ce"         , s_ce);
   /*---(set y midpoint)--------------*/
   switch (s_anchor) {
   case YVIKEYS_TOPLEF : case YVIKEYS_TOPCEN : case YVIKEYS_TOPRIG :
      if (s_type == YVIKEYS_DEPTH)   s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0 + a_adj;
      else                           s_mi  = x_ymax - s_x;
      break;
   case YVIKEYS_MIDLEF : case YVIKEYS_MIDCEN : case YVIKEYS_MIDRIG :
      s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0;
      break;
   case YVIKEYS_BOTLEF : case YVIKEYS_BOTCEN : case YVIKEYS_BOTRIG :
      if (s_type == YVIKEYS_DEPTH)   s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0 - a_adj;
      else                           s_mi  = x_ymax + s_x;
      break;
   }
   DEBUG_CMDS   yLOG_value   ("s_mi"         , s_mi);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__gl_back   (int a_len, int a_level, tMLINK *a_found)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_adj       =    5;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%d %d -- %c %c %c %-10.10s %c", a_len, a_level, a_found->data->top, a_found->data->mid, a_found->data->bot, a_found->data->name, a_found->active);
   /*---(prepare)------------------------*/
   s_pt     = myVIKEYS.point;
   /*> x_adj  = 15.0;                                                              <*/
   s_xoff = 15.0;
   /*---(get sizes)----------------------*/
   DEBUG_CMDS   yLOG_note    ("set sizes and offsets (DEPTH vs FLAT)");
   s_type   = yVIKEYS_view_type   (YVIKEYS_MENUS);
   if (s_type == YVIKEYS_DEPTH) {
      s_x    =  35;
      s_y    =  25;
      s_z    = -150;
      s_pt   = 2;
      s_xoff = 15.0 / 4.0;
      s_yoff = 15.0 / 4.0;
      x_adj  = 35.0;
   }
   /*---(anchoring)-------------------*/
   yvikeys_menu__gl_center (x_adj);
   /*---(black background)---------------*/
   DEBUG_CMDS   yLOG_note    ("draw black background");
   glColor4f (0.0f, 0.0f, 0.0f, 1.0f);
   glBegin(GL_POLYGON); {
      glVertex3f (s_ce - s_x, s_mi + s_y, s_z);
      glVertex3f (s_ce + s_x, s_mi + s_y, s_z);
      glVertex3f (s_ce + s_x, s_mi - s_y, s_z);
      glVertex3f (s_ce - s_x, s_mi - s_y, s_z);
   } glEnd();
   /*---(color fill)---------------------*/
   if (a_len != a_level) {
      DEBUG_CMDS   yLOG_note    ("a_len and a_level do not match, set color RED");
      glColor4f (0.8f, 0.0f, 0.0f, 1.0f);
   } else {
      DEBUG_CMDS   yLOG_note    ("set color for level");
      switch (a_level) {
      case  0 :  glColor4f (0.3f, 0.7f, 0.7f, 1.0f);  break;
      case  1 :  glColor4f (0.7f, 0.5f, 0.3f, 1.0f);  break;
      case  2 :  glColor4f (0.3f, 0.8f, 0.3f, 1.0f);  break;
      }
   }
   DEBUG_CMDS   yLOG_note    ("draw inset background over black frame");
   glBegin(GL_POLYGON); {
      glVertex3f (s_ce - s_x * 0.97, s_mi + s_y * 0.97, s_z + 2);
      glVertex3f (s_ce + s_x * 0.97, s_mi + s_y * 0.97, s_z + 2);
      glVertex3f (s_ce + s_x * 0.97, s_mi - s_y * 0.97, s_z + 2);
      glVertex3f (s_ce - s_x * 0.97, s_mi - s_y * 0.97, s_z + 2);
   } glEnd();
   /*---(labels)-------------------------*/
   glPushMatrix(); {
      glColor4f (0.0, 0.0, 0.0, 1.0);
      glTranslatef(s_ce, s_mi, s_z + 4);
      glTranslatef(0.0 , s_y * 0.86, 0.0);
      switch (a_level) {
      case  0 : strlcpy (t, "main menu", LEN_LABEL); break;
      case  1 : sprintf (t, "µ%c (%s) sub-menu", a_found->data->top, a_found->data->name); break;
      case  2 : sprintf (t, "µ%c%c (%s) options", a_found->data->top, a_found->data->mid, a_found->data->name); break;
      }
      yFONT_print (myVIKEYS.font, s_pt, YF_BASCEN, t);
      glTranslatef(-s_x * 0.93,    0.0,   0.0);
      yFONT_print (myVIKEYS.font, s_pt, YF_BASLEF, myVIKEYS.s_prog);
      glTranslatef(s_x * 0.93 * 2,    0.0,   0.0);
      yFONT_print (myVIKEYS.font, s_pt, YF_BASRIG, myVIKEYS.s_vernum);
      glTranslatef(0.0 , -s_y * 0.86, 0.0    );
      glTranslatef(-s_x * 0.90, -s_y * 0.94,   0.0);
      yFONT_print (myVIKEYS.font, s_pt, YF_BASCEN, "much wider, flatter, and universal");
      if (a_len != a_level) {
         glColor4f    (1.0f, 1.0f, 1.0f, 1.0f);
         glTranslatef (   0.0,  s_y  ,  10.0);
         yFONT_print  (myVIKEYS.font, s_pt, YF_BASCEN, "ERROR KEYS");
         glTranslatef (   0.0,  -s_y * 0.15,   0.0);
         yFONT_print  (myVIKEYS.font, s_pt, YF_BASCEN, "menu locked, <esc> to exit");
      }
   } glPopMatrix();
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__nc_back   (int a_len, int a_level, tMLINK *a_found)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_adj       =    5;
   char        t           [LEN_RECD];
   int         x_left, x_wide, x_bott, x_tall;
   int         x_top, x_mid;
   int         x_len;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%d %d -- %c %c %c %-10.10s %c", a_len, a_level, a_found->data->top, a_found->data->mid, a_found->data->bot, a_found->data->name, a_found->active);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   s_xoff =  2.0;
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   x_top = x_bott - x_tall;
   x_mid = x_left + (x_wide / 2);
   yCOLOR_curs ("menu"   );
   for (i = 0; i < x_tall; ++i) {
      mvprintw   (x_top + i, x_left, "%*.*s", x_wide, x_wide, "                                                  ");
   }
   switch (a_level) {
   case  0 : strlcpy (t, "main menu", LEN_LABEL); break;
   case  1 : sprintf (t, "µ%c (%s) sub-menu", s_found->data->top, s_found->data->name); break;
   case  2 : sprintf (t, "µ%c%c (%s) options", s_found->data->top, s_found->data->mid, s_found->data->name); break;
   }
   x_len = strlen (t);
   mvprintw   (x_top, x_mid - (x_len / 2), "%s", t);
   mvprintw   (x_top, x_left + 1, "%s", myVIKEYS.s_prog);
   x_len = strlen (myVIKEYS.s_vernum);
   mvprintw   (x_top, x_left + x_wide - x_len - 1, "%s", myVIKEYS.s_vernum);
   strlcpy (t, "much wider, flatter, and universal", LEN_RECD);
   x_len = strlen (t);
   mvprintw   (x_bott - 1, x_mid - (x_len / 2), "%s", t);
   if (a_len != a_level) {
      yCOLOR_curs ("error");
      strlcpy (t, "ERROR KEY", LEN_RECD);
      x_len = strlen (t);
      mvprintw   (x_bott - 8, x_mid - (x_len / 2), "%s", t);
      strlcpy (t, "press <esc>", LEN_RECD);
      x_len = strlen (t);
      mvprintw   (x_bott - 7, x_mid - (x_len / 2), "%s", t);
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__gl_entry  (tMLINK *a_item, int a_level, int a_group, int a_entry, float x, float y, float a_offx, float a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   int         t           [LEN_LABEL];
   int         x_align     =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%dl %dg %2de %6.2fx %6.2fy %6.2fox %6.2foy", a_level, a_group, a_entry, x, y, a_offx, a_offy);
   glPushMatrix(); {
      /*---(recenter)--------------------*/
      glTranslatef (s_ce, s_mi, s_z + 6);
      glTranslatef (0.0 , s_y * 0.56, 0.0);
      /*---(color)-----------------------*/
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
      /*---(position)--------------------*/
      if (a_level > 0) {
         switch (a_group) {
         case 0 : glTranslatef (-s_x * 0.70, -s_y * 0.14 * a_entry,   0.0);  break;
         case 1 : glTranslatef ( s_x * 0.15, -s_y * 0.14 * a_entry,   0.0);  break;
         case 2 : glTranslatef (-s_x * 0.90, -s_y * 0.14 * a_entry,   0.0);  break;
         case 3 : glTranslatef (-s_x * 0.28, -s_y * 0.14 * a_entry,   0.0);  break;
         case 4 : glTranslatef ( s_x * 0.35, -s_y * 0.14 * a_entry,   0.0);  break;
         }
      } else {
         glTranslatef (x, y,   0.0);
      }
      /*---(alignment)-------------------*/
      if (a_offx >  0)  x_align = YF_BASLEF;
      if (a_offx == 0)  x_align = YF_BASCEN;
      if (a_offx <  0)  x_align = YF_BASRIG;
      /*---(abbrev)----------------------*/
      switch (a_level) {
      case 0  : sprintf (t, "%c", a_item->data->top);   break;
      case 1  : sprintf (t, "%c", a_item->data->mid);   break;
      case 2  : sprintf (t, "%c", a_item->data->bot);   break;
      }
      yFONT_print (myVIKEYS.font, s_pt, x_align, t);
      /*---(name)------------------------*/
      glTranslatef (a_offx, a_offy, 0.0);
      sprintf (t, "%.9s", a_item->data->name);
      if (a_level > 0) {
         if (a_item->data->type == MENU_GROUP)  strlcat (t, "+", LEN_LABEL);
         if (a_item->data->type == MENU_MORE )  strlcat (t, "=", LEN_LABEL);
      }
      yFONT_print (myVIKEYS.font, s_pt, x_align, t);
      /*---(done)------------------------*/
   } glPopMatrix();
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__nc_entry  (tMLINK *a_item, int a_mid, int a_top, int a_level, int a_group, int a_entry, int a_offx, int a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x, y;
   uchar       t           [LEN_LABEL];
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_complex ("args"      , "%dl %dg %2de %6.2fx %6.2fy %6.2fox %6.2foy", a_level, a_group, a_entry, x, y, a_offx, a_offy);
   /*---(abbrev)-------------------------*/
   switch (a_level) {
   case 0  : sprintf (t, "%c", a_item->data->top);   break;
   case 1  : sprintf (t, "%c", a_item->data->mid);   break;
   case 2  : sprintf (t, "%c", a_item->data->bot);   break;
   }
   /*---(color)--------------------------*/
   switch (a_item->data->type) {
   case MENU_GROUP :
      if      (a_item->active   == MENU_GREY  )  yCOLOR_curs ("menu_na" );
      else if (a_item->c_count  == 0)            yCOLOR_curs ("menu_na" );
      else                                       yCOLOR_curs ("menu"    );
      break;
   case MENU_NAMER : case MENU_TBD :
      yCOLOR_curs ("menu_bad");
      break;
   case MENU_EXACT : case MENU_CMD  : case MENU_MORE :
      if      (a_item->active   == MENU_ACTIVE)  yCOLOR_curs ("menu"    );
      else if (a_item->active   == MENU_GREY  )  yCOLOR_curs ("menu_na" );
      else                                       yCOLOR_curs ("menu_bad");
      break;
   default  :
      yCOLOR_curs ("menu_bad");
      break;
   }
   /*---(position)-----------------------*/
   if (a_level > 0) {
      switch (a_group) {
      case 0 : x = a_mid - 15; y = a_top + a_entry;    break;
      case 1 : x = a_mid +  3; y = a_top + a_entry;    break;
      case 2 : x = a_mid - 19; y = a_top + a_entry;    break;
      case 3 : x = a_mid -  6; y = a_top + a_entry;    break;
      case 4 : x = a_mid +  7; y = a_top + a_entry;    break;
      }
   } else {
      x = a_group;
      y = a_entry;
   }
   DEBUG_CMDS   yLOG_complex ("position"  , "%3dg, %3de, %3dm, %3dt, %3dx, %3dy", a_group, a_entry, a_mid, a_top, x, y);
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
   y -= a_offy;
   /*---(name)---------------------------*/
   DEBUG_CMDS   yLOG_complex ("title"     , "%3dox, %3doy, %3dl, %3dx, %3dy", a_offx, a_offy, x_len, x, y);
   mvprintw   (y, x, "%s", t);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__column    (int a_level, tMLINK *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   int         a           =   10;
   int         c           =    0;
   int         t           [LEN_LABEL];
   int         x_entry     =    0;
   int         x_group     =    0;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_top, x_mid;
   float       x_xoff, x_yoff;
   tMLINK     *x_curr;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
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
   /*---(walk entries)-------------------*/
   x_curr = a_curr->c_first;
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      /*---(filter)----------------------*/
      if (x_curr->active == MENU_HIDE) {
         x_curr = x_curr->s_next;
         continue;
      }
      /*---(prepare)---------------------*/
      x_top  = x_bott - x_tall + 2;
      x_mid  = x_left + (x_wide / 2);
      if (s_type == YVIKEYS_FLAT) {
         x_xoff  = 15.0;
      } else {
         x_xoff  =  3.0;
      }
      /*---(display)---------------------*/
      switch (myVIKEYS.env) {
      case YVIKEYS_OPENGL :  yvikeys_menu__gl_entry (x_curr, a_level, x_group, x_entry, 0, 0, x_xoff, 0.0); break;
      case YVIKEYS_CURSES :  yvikeys_menu__nc_entry (x_curr, x_mid, x_top , a_level, x_group, x_entry, 2.0, 0.0); break;
      }
      /*---(figure next)-----------------*/
      x_curr = x_curr->s_next;
      ++c;
      ++x_entry;
      if (c != 0 && c % a == 0) {
         ++x_group;
         x_entry = 0;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__gl_main   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tMLINK     *x_curr      = NULL;
   int         c           =    0;
   float       x_off, y_off;
   float       x_pos, y_pos;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_CMDS   yLOG_value   ("c_count"   , s_head->c_count);
   x_curr = s_head->c_first;
   /*---(walk children)------------------*/
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      /*---(filter)------------*/
      if (x_curr->active == MENU_HIDE) {
         x_curr = x_curr->s_next;
         continue;
      }
      /*---(alignment)---------*/
      switch (s_align [c]) {
      case 1 :  x_off =   1;  y_off =  0;          break;
      case 2 :  x_off =   0;  y_off = s_abbr [c];  break;
      case 3 :  x_off =  -1;  y_off =  0;          break;
      }
      /*---(position)----------*/
      x_pos = s_xpos [c];
      y_pos = s_ypos [c] - 4.0;
      if (s_type == YVIKEYS_FLAT) {
         x_pos  *=  8.0;
         y_pos  *= 14.0;
         y_off *= 14.0;
         x_off *= 15.0;
      } else {
         x_pos  *=  2.0;
         y_pos  *=  3.5;
         y_off *=  3.0;
         x_off *=  3.0;
      }
      /*---(draw)--------------*/
      yvikeys_menu__gl_entry (x_curr, 0, 0, 0, x_pos, y_pos, x_off, y_off);
      /*---(next)--------------*/
      x_curr = x_curr->s_next;
      ++c;
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu__nc_main   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tMLINK     *x_curr      = NULL;
   int         c           =    0;
   int         t           [LEN_LABEL];
   int         x_left, x_wide, x_bott, x_tall, x_top, x_mid;
   int         x, y;
   float       x_offx      =    0;
   float       x_offy      =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(opengl)-------------------------*/
   x_top = x_bott - x_tall + 2;
   x_mid = x_left + (x_wide / 2);
   /*---(prepare)------------------------*/
   DEBUG_CMDS   yLOG_value   ("c_count"   , s_head->c_count);
   x_curr = s_head->c_first;
   /*---(walk children)------------------*/
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("x_curr"    , "%c %c %c %-10.10s %c", x_curr->data->top, x_curr->data->mid, x_curr->data->bot, x_curr->data->name, x_curr->active);
      /*---(filter)------------*/
      if (x_curr->active == MENU_HIDE) {
         x_curr = x_curr->s_next;
         continue;
      }
      /*---(position)----------*/
      x = x_mid + trunc (s_xpos [c]);        /* 6 is more ellipse/rounded */
      y = x_top - s_ypos [c] + 4;
      switch (s_align [c]) {
      case 1 :  x_offx =   2;  x_offy =  0;               break;
      case 2 :  x_offx =   0;  x_offy = s_abbr [c];       break;
      case 3 :  x_offx =  -2;  x_offy =  0;               break;
      }
      /*---(draw)--------------*/
      yvikeys_menu__nc_entry (x_curr, x_mid, x_top, 0, x, y, x_offx, x_offy);
      /*---(next)--------------*/
      x_curr = x_curr->s_next;
      ++c;
      /*---(done)--------------*/
   }
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
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
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_MENUS  )) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("s_head"    , s_head);
   --rce;  if (s_head == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find path)----------------------*/
   DEBUG_USER   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   n = yvikeys_menu_menu (myVIKEYS.m_path);
   DEBUG_USER   yLOG_point   ("s_found"   , s_found);
   --rce;  if (s_found == NULL) {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_complex ("target"    , "%c %c %c %s", s_found->data->top, s_found->data->mid, s_found->data->bot, s_found->data->name);
   DEBUG_CMDS   yLOG_complex ("parent"    , "%c %c %c %s", s_parent->data->top, s_parent->data->mid, s_parent->data->bot, s_parent->data->name);
   DEBUG_USER   yLOG_value   ("level"     , s_level);
   /*---(draw back)----------------------*/
   x_len = strlen (myVIKEYS.m_path);
   DEBUG_USER   yLOG_value   ("x_len"     , x_len);
   /*> yvikeys__menu_back (x_len - 1, s_level, s_parent);                             <*/
   if (myVIKEYS.env == YVIKEYS_OPENGL)   yvikeys_menu__gl_back (x_len - 1, s_level, s_found);
   else                                  yvikeys_menu__nc_back (x_len - 1, s_level, s_found);
   /*---(main menu)----------------------*/
   switch (s_level) {
   case 0 :
      if (myVIKEYS.env == YVIKEYS_OPENGL)   yvikeys_menu__gl_main ();
      else                                  yvikeys_menu__nc_main ();
      break;
   case 1 : case 2 :
      yvikeys_menu__column (s_level, s_found);
      break;
   }
   /*---(complete)--------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
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
   strlcpy (myVIKEYS.m_keys, ""  , LEN_HUND );
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
         strlcpy (myVIKEYS.m_keys, s_found->data->keys, LEN_HUND);
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
    *>    strlcpy (s_base [n].keys, "-", LEN_HUND );                                            <* 
    *>    s_base [n].active = MENU_ACTIVE;                                                      <* 
    *> }                                                                                        <* 
    *> /+---(if making a executable)---------+/                                                 <* 
    *> else {                                                                                   <* 
    *>    s_base [n].type   = '·';                                                              <* 
    *>    strlcpy (s_base [n].keys, a_keys, LEN_HUND );                                         <* 
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

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__menu_unit      (char *a_question, char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         c           =    0;
   int         n           =   -1;
   char        x_level     =    0;
   int         x_last      =    0;
   char        t           [LEN_HUND ];
   char        s           [LEN_LABEL] = "entry";
   tMLINK     *x_curr      = NULL;
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
         strlcpy    (t, p->keys, LEN_HUND );
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
         strlcpy    (t, p->keys , LEN_HUND );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU entry       : %-3d %c %c %c %-10.10s %c %-10.10s %c",
               s_level, p->top, p->mid, p->bot, p->name, p->type, t, s_found->active);
      }
   }
   else if (strcmp (a_question, "found"          )   == 0) {
      if (s_found == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU found       : %-3d · · · -          · -          -", s_level);
      else {
         p = s_found->data;
         strlcpy    (t, p->keys , LEN_HUND );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU found       : %-3d %c %c %c %-10.10s %c %-10.10s %c",
               s_level, p->top, p->mid, p->bot, p->name, p->type, t, s_found->active);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

