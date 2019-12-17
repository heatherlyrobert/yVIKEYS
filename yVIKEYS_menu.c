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



/*===[[ MENUS ]]==============================================================*/
#define    MAX_MENU        1000
typedef    struct   cMENU   tMENU;
struct cMENU {
   /*---(original)----------*/
   char        base;                        /* original item (not added)      */
   /*---(organize)----------*/
   char        top;                         /* first level key                */
   char        mid;                         /* second level key               */
   char        bot;                         /* third level key                */
   /*---(master)------------*/
   char        name        [LEN_LABEL];     /* name for use on screens        */
   char        defact;                      /* default activation             */
   char        active;                      /* program activation             */
   char        type;                        /* entry type   > ! = ·           */
   char        keys        [LEN_HUND ];     /* command or keys to execute     */
   /*---(traverse)----------*/
   short       next;                        /* next sibling                   */
   short       start;                       /* first child                    */
   short       count;                       /* count of children              */
   /*---(done)--------------*/
};
static int    s_nmenu  = 0;                 /* active menu items in list      */
static int    s_nreal  = 0;                 /* all menu items in list         */
static int    s_ntops  = 0;                 /* top level items in list        */
static tMENU  s_menus [MAX_MENU] = {
   /*---(file menu)----------------------------------------------*/
   { 'y', 'f', '·', '·', "files"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'W', '·', "workspace"        , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'w', '·', "new"              , 'y', 'y', '·', ":new¦"             , 0, 0, 0 },
   { 'y', 'f', 'c', '·', "chdir"            , 'y', 'y', '=', ":cd·"              , 0, 0, 0 },
   { 'y', 'f', 'b', '·', "browse"           , 'y', 'y', '=', ":browse·"          , 0, 0, 0 },
   { 'y', 'f', 'n', '·', "name"             , 'y', 'y', '=', ":file·"            , 0, 0, 0 },
   { 'y', 'f', 'o', '·', "read"             , 'y', 'y', '·', ":read¦"            , 0, 0, 0 },
   { 'y', 'f', 'm', '·', "import"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'f', '·', "refresh"          , 'y', 'y', '·', ":refresh¦"         , 0, 0, 0 },
   { 'y', 'f', 'r', '·', "restore"          , '·', '·', '!', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'v', '·', "version"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'p', '·', "protect"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'u', '·', "update"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'f', 's', '·', "write"            , 'y', 'y', '·', ":write¦"           , 0, 0, 0 },
   { 'y', 'f', 'a', '·', "writeas"          , 'y', 'y', '=', ":writeas·"         , 0, 0, 0 },
   { 'y', 'f', 'x', '·', "export"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'f', 'q', '·', "quit"             , 'y', 'y', '·', ":quit¦"            , 0, 0, 0 },
   /*---(edit menu)----------------------------------------------*/
   { 'y', 'e', '·', '·', "edit"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'u', '·', "undo"             , 'y', 'y', '·', "u"                 , 0, 0, 0 },
   { 'y', 'e', 'U', '·', "redo"             , 'y', 'y', '·', "U"                 , 0, 0, 0 },
   { 'y', 'e', '_', '·', "status"           , 'y', 'y', '·', ":status mundo¦"    , 0, 0, 0 },
   { 'y', 'e', '?', '·', "history"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', '?', 't', "track"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', '?', 'u', "untrack"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', '?', 'c', "clear"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', '?', 'r', "review"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'y', '·', "copy"             , 'y', 'y', '·', "y"                 , 0, 0, 0 },
   { 'y', 'e', 'Y', '·', "cut"              , 'y', 'y', '·', "Y"                 , 0, 0, 0 },
   { 'y', 'e', 'p', '·', "paste"            , 'y', 'y', '·', "p"                 , 0, 0, 0 },
   { 'y', 'e', 'P', '·', "special"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'x', '·', "clear"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'x', '.', "inplace"          , 'y', 'y', '·', "x."                , 0, 0, 0 },
   { 'y', 'e', 'x', 'l', "left"             , 'y', 'y', '·', "xl"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'h', "right"            , 'y', 'y', '·', "xh"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'k', "up"               , 'y', 'y', '·', "xk"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'j', "down"             , 'y', 'y', '·', "xj"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'i', "in"               , 'y', 'y', '·', "xi"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'o', "out"              , 'y', 'y', '·', "xo"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'x', "col_lef"          , 'y', 'y', '·', "xx"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'X', "col_rig"          , 'y', 'y', '·', "xX"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'y', "row_abo"          , 'y', 'y', '·', "xy"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'Y', "row_bel"          , 'y', 'y', '·', "xY"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'z', "lvl_in"           , 'y', 'y', '·', "xz"                , 0, 0, 0 },
   { 'y', 'e', 'x', 'Z', "lvl_out"          , 'y', 'y', '·', "xZ"                , 0, 0, 0 },
   { 'y', 'e', 'd', '·', "delete"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'd', 'l', "left"             , 'y', 'y', '·', "dl"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'h', "right"            , 'y', 'y', '·', "dh"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'k', "up"               , 'y', 'y', '·', "dk"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'j', "down"             , 'y', 'y', '·', "dj"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'i', "in"               , 'y', 'y', '·', "di"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'o', "out"              , 'y', 'y', '·', "do"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'x', "col_left"         , 'y', 'y', '·', "dx"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'X', "col_right"        , 'y', 'y', '·', "dX"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'y', "row_above"        , 'y', 'y', '·', "dy"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'Y', "row_below"        , 'y', 'y', '·', "dY"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'z', "lvl_in"           , 'y', 'y', '·', "dz"                , 0, 0, 0 },
   { 'y', 'e', 'd', 'Z', "lvl_out"          , 'y', 'y', '·', "dZ"                , 0, 0, 0 },
   { 'y', 'e', 'i', '·', "insert"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'i', 'l', "left"             , 'y', 'y', '·', "al"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'h', "right"            , 'y', 'y', '·', "ah"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'k', "up"               , 'y', 'y', '·', "ak"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'j', "down"             , 'y', 'y', '·', "aj"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'i', "in"               , 'y', 'y', '·', "ai"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'o', "out"              , 'y', 'y', '·', "ao"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'x', "col_lef"          , 'y', 'y', '·', "ax"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'X', "col_rig"          , 'y', 'y', '·', "aX"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'y', "row_abo"          , 'y', 'y', '·', "ay"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'Y', "row_bel"          , 'y', 'y', '·', "aY"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'z', "lvl_in"           , 'y', 'y', '·', "az"                , 0, 0, 0 },
   { 'y', 'e', 'i', 'Z', "lvl_out"          , 'y', 'y', '·', "aZ"                , 0, 0, 0 },
   { 'y', 'e', 'g', '·', "group"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'g', "group"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'u', "ungroup"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'r', "regroup"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'm', "merge"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'f', "free"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'g', 'b', "boundary"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', '·', "align"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'k', "top"              , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'j', "bottom"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'h', "left"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'l', "right"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'o', "front"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'i', "back"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'x', "dist-x"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'y', "dist-y"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'a', 'z', "dist-z"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', '·', "order"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', 'n', "front"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', 'i', "forward"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', 'o', "backward"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', 'f', "back"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'o', 'r', "reverse"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 'e', 'l', '·', "layer"            , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   /*---(view menu)----------------------------------------------*/
   { 'y', 'v', '·', '·', "view"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'w', '·', "window"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'l', '·', "layout"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 't', '·', "title"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'v', '·', "version"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'b', '·', "buffers"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'f', '·', "formula"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'n', '·', "nav"              , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'p', '·', "progress"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 's', '·', "status"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'x', '·', "modes"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'k', '·', "keys"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'c', '·', "command"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'a', '·', "alt"              , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'd', '·', "details"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'r', '·', "ribbon"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'M', '·', "menu"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'C', '·', "color"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'X', '·', "xaxis"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'Y', '·', "yaxis"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'L', '·', "layers"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'G', '·', "grid"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'E', '·', "edges"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'U', '·', "guides"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'O', '·', "overlay"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', 'N', '·', "notes"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', '?', '·', "help"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'v', '?', 'x', "cancel"           , 'y', 'y', '·', ":help -¦"          , 0, 0, 0 },
   /*---(insert menu)--------------------------------------------*/
   { 'y', 'i', '·', '·', "insert"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 't', '·', "text"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'i', '·', "image"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'l', '·', "lines"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'e', '·', "ellises"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 't', '·', "tris"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'r', '·', "rects"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'h', '·', "hexagons"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'p', '·', "polygons"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 's', '·', "stars"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'a', '·', "arrows"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'f', '·', "flowchart"        , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'j', '·', "joiners"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'n', '·', "notes"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'm', '·', "misc"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', '-', '·', "nurbs"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', '-', '·', "tiling"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', 'b', '·', "beziers"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', '-', '·', "3d"               , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', '-', '·', "surfaces"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'i', '-', '·', "meshes"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(select menu)--------------------------------------------*/
   { 'y', 's', '·', '·', "select"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 's', '!', '·', "highlight"        , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', '_', '·', "status"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'c', '·', "clear"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'a', '·', "all"              , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'b', '·', "buffer"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'w', '·', "window"           , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'l', '·', "layer"            , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'g', '·', "geometry"         , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 's', 't', '·', "type"             , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 's', 'x', '·', "regex"            , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 's', 'u', '·', "touching"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 's', '·', "save"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'r', '·', "reselect"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'j', '·', "join"             , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'd', '·', "deselect"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'i', '·', "inverse"          , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'x', '·', "all_on_x"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'y', '·', "all_on_y"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'y', 's', 'z', '·', "all_on_z"         , 'y', 'y', '·', "-"                 , 0, 0, 0 },
   /*---(format menu)--------------------------------------------*/
   { 'y', 'o', '·', '·', "format"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'f', '·', "face"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'p', '·', "point"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 's', '·', "spacing"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'i', '·', "indent"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'c', '·', "color"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'a', '·', "align"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'n', '·', "numbers"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'z', '·', "sfill"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'd', '·', "decimals"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'u', '·', "units"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'l', '·', "lists"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'o', 'y', '·', "style"            , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(modify menu)--------------------------------------------*/
   { 'y', 'm', '·', '·', "modify"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'è', '·', "resize"           , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'é', '·', "dims"             , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ê', '·', "extent"           , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ë', '·', "scale"            , 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ì', '·', "trim"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'í', '·', "join"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'î', '·', "move"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ï', '·', "array"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ð', '·', "rotate"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ñ', '·', "snap"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ò', '·', "fill"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ó', '·', "hatching"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ô', '·', "outline"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'õ', '·', "centers"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'm', 'ö', '·', "ends"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(dataset menu)-------------------------------------------*/
   { 'y', 'd', '·', '·', "dataset"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'd', 's', '·', "sort"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'd', 'f', '·', "filter"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "blur"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "enhance"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "distort"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "noise"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "edges"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "combine"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "light"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "shadow"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "pixelate"         , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "render"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "sharpen"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "smooth"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "stylize"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "liquify"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "oils"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "map"              , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "decor"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'd', '-', '·', "vanish"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(tools menu)---------------------------------------------*/
   { 'y', 't', '·', '·', "tools"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "pen"              , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "callig"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "pencil"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "airbrush"         , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "chalk"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "brush"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "roller"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "spray"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "fill"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "sponge"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "blob"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "blur"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "sharpen"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "smudge"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "burn"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "eraser"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "magic_e"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "healer"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "highlight"        , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "zoom"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "move"             , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 't', '-', '·', "lasso"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   /*---(pallette menu)------------------------------------------*/
   { 'y', 'p', '·', '·', "palette"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   /*---(layers menu)--------------------------------------------*/
   { 'y', 'l', '·', '·', "layers"           , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "saved"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "highlight"        , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "status"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "new"              , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "rename"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "copy_all"         , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "copy_with"        , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "join"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "flatten"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "delete"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "type"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "hide"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "mask"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "alignment"        , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'l', '-', '·', "locking"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(buffers menu)-------------------------------------------*/
   { 'y', 'b', '·', '·', "buffers"          , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'b', '_', '·', "status"           , 'y', 'y', '·', ":status buffer¦"   , 0, 0, 0 },
   { 'y', 'b', 'x', '·', "maximum"          , 'y', 'y', '=', ":bmax·"            , 0, 0, 0 },
   { 'y', 'b', 'b', '·', "browse"           , 'y', 'y', '·', ":bbrowse·"         , 0, 0, 0 },
   { 'y', 'b', 'g', '·', "goto"             , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'b', 'g', 'a', "abbrev"           , 'y', 'y', '=', ":buf·"             , 0, 0, 0 },
   { 'y', 'b', 'g', 'r', "return"           , 'y', 'y', '·', ",,"                , 0, 0, 0 },
   { 'y', 'b', 'g', 'f', "first"            , 'y', 'y', '·', ",["                , 0, 0, 0 },
   { 'y', 'b', 'g', 'p', "prev"             , 'y', 'y', '·', ",<"                , 0, 0, 0 },
   { 'y', 'b', 'g', 'n', "next"             , 'y', 'y', '·', ",>"                , 0, 0, 0 },
   { 'y', 'b', 'g', 'l', "last"             , 'y', 'y', '·', ",]"                , 0, 0, 0 },
   { 'y', 'b', 'g', 'u', "summary"          , 'y', 'y', '·', ",®"                , 0, 0, 0 },
   { 'y', 'b', 'g', 'y', "system"           , 'y', 'y', '·', ",¯"                , 0, 0, 0 },
   { 'y', 'b', '-', '·', "showall"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', 'w', '·', "new"              , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', 'n', '·', "rename"           , 'y', 'y', '=', ":btitle·"          , 0, 0, 0 },
   { 'y', 'b', 'z', '·', "size"             , 'y', 'y', '=', ":bsize·"           , 0, 0, 0 },
   { 'y', 'b', 't', '·', "type"             , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', 'f', '·', "defaults"         , 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'y', 'b', 'f', 'w', "col_wide"         , 'y', 'y', '=', ":defwide·"         , 0, 0, 0 },
   { 'y', 'b', 'f', 't', "row_tall"         , 'y', 'y', '=', ":deftall·"         , 0, 0, 0 },
   { 'y', 'b', 'f', 'd', "dep_thick"        , 'y', 'y', '!', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "scale"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "delete"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "freeze"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "split"            , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "hiding"           , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'y', 'b', '-', '·', "locking"          , 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(snippet menu)-------------------------------------------*/
   { 'y', 'g', '·', '·', "language"         , '-', '-', '>', "-"                 , 0, 0, 0 },
   /*---(auditor menu)-------------------------------------------*/
   { 'y', 'a', '·', '·', "auditor"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'a', 's', '·', "spell"            , '-', '-', '-', "-"                 , 0, 0, 0 },
   { 'y', 'a', 'd', '·', "dump"             , '-', '-', '-', "-"                 , 0, 0, 0 },
   /*---(execute menu)-------------------------------------------*/
   { 'y', 'x', '·', '·', "execute"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   /*---(config menu)--------------------------------------------*/
   { 'y', 'c', '·', '·', "config"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'c', 's', '·', "search"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'c', 's', 'r', "reset all"        , '-', '-', '=', "-"                 , 0, 0, 0 },
   /*---(macro menu)---------------------------------------------*/
   { 'y', 'r', '·', '·', "script"           , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'e', '·', "execute"          , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'd', '·', "debug"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'w', '·', "new"              , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'o', '·', "open"             , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'm', '·', "import"           , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 's', '·', "save"             , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'x', '·', "export"           , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'i', '·', "list"             , '-', '-', '·', "-"                 , 0, 0, 0 },
   { 'y', 'r', 'g', '·', "globals"          , '-', '-', '·', "-"                 , 0, 0, 0 },
   /*---(share menu)---------------------------------------------*/
   { 'y', 'h', '·', '·', "share"            , '-', '-', '>', "-"                 , 0, 0, 0 },
   /*---(footer)-------------------------------------------------*/
   {  0 ,  0 ,  0 ,  0 , NULL               ,   0,   0,   0, NULL                , 0, 0, 0 },
   /*---(done)---------------------------------------------------*/
};




/*====================------------------------------------====================*/
/*===----                         menu display                         ----===*/
/*====================------------------------------------====================*/
static void  o___MENUS___________o () { return; }

static int  *s_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-_?!#,.èéêëìíîïðñòóôõö÷øùúûüýþÿ";

char
yvikeys__menu_count     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   s_nmenu = 0;
   s_nreal = 0;
   for (i = 0; i < MAX_MENU; ++i) {
      if (s_menus [i].name [0] == NULL)         break;
      if (strcmp (s_menus [i].name, "-") == 0)  break;
      ++s_nmenu;
      if (s_menus [i].active   != 'y')          continue;
      ++s_nreal;
   }
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
   DEBUG_CMDS   yLOG_value   ("s_nreal"   , s_nreal);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_unlink    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   DEBUG_CMDS   yLOG_value   ("s_nreal"   , s_nreal);
   for (i = 0; i < s_nreal; ++i) {
      if (s_menus [i].name [0] == NULL)         break;
      if (strcmp (s_menus [i].name, "-") == 0)  break;
      s_menus [i].next    = -1;
      s_menus [i].start   = -1;
      s_menus [i].count   =  0;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_wipe      (int i)
{
   if (s_menus [i].base == 'y') {
      s_menus [i].active  = '-';
   } else {
      s_menus [i].base    = '-';
      s_menus [i].top     = '·';
      s_menus [i].mid     = '·';
      s_menus [i].bot     = '·';
      strlcpy (s_menus [i].name, "-", LEN_LABEL);
      s_menus [i].defact  = '-';
      s_menus [i].active  = '-';
      s_menus [i].type    = '·';
      strlcpy (s_menus [i].keys, "-", LEN_HUND );
      s_menus [i].next    = -1;
      s_menus [i].start   = -1;
      s_menus [i].count   =  0;
   }
   return 0;
}

char
yvikeys_menu_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   for (i = 0; i < MAX_MENU; ++i) {
      yvikeys__menu_wipe (i);
      if (s_menus [i].base == 'y')  s_menus [i].active  = s_menus [i].defact;
   }
   yvikeys__menu_count ();
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_cleanse   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   for (i = 0; i < MAX_MENU; ++i) {
      yvikeys__menu_wipe (i);
   }
   yvikeys__menu_count ();
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_tops      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_last      =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle tops)--------------------*/
   s_ntops =  0;
   x_last  = -1;
   for (i = 0; i < s_nreal; ++i) {
      /*---(filter)------------*/
      if (s_menus [i].top ==  0 )                     continue;
      if (s_menus [i].top == '·')                     continue;
      if (s_menus [i].mid != '·')                     continue;
      if (strchr (s_valid, s_menus [i].top) == NULL)  continue;
      /*---(update)------------*/
      if (x_last >= 0)  s_menus [x_last].next = i;
      x_last = i;
      ++s_ntops;
      /*---(done)--------------*/
   }
   DEBUG_CMDS   yLOG_value   ("s_ntops"   , s_ntops);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_mids      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_ctop      =    0;
   char        x_atop      =  '·';
   int         x_last      =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle mids)--------------------*/
   x_ctop  =  0;
   while (x_ctop >= 0) {
      x_atop  = s_menus [x_ctop].top;
      x_last  = -1;
      for (i = 0; i < s_nreal; ++i) {
         /*---(filter)------------*/
         if (s_menus [i].top != x_atop)                  continue;
         if (s_menus [i].mid ==  0 )                     continue;
         if (s_menus [i].mid == '·')                     continue;
         if (s_menus [i].bot != '·')                     continue;
         if (strchr (s_valid, s_menus [i].mid) == NULL)  continue;
         /*---(update)------------*/
         if (x_last <  0)  s_menus [x_ctop].start = i;
         else              s_menus [x_last].next  = i;;
         ++s_menus [x_ctop].count;
         x_last = i;
         /*---(done)--------------*/
      }
      x_ctop = s_menus [x_ctop].next;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_bots      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_ctop      =    0;
   char        x_atop      =  '·';
   int         x_cmid      =    0;
   char        x_amid      =  '·';
   int         x_last      =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle bots)--------------------*/
   x_ctop  =  0;
   while (x_ctop >= 0) {
      x_atop  = s_menus [x_ctop].top;
      x_cmid  = s_menus [x_ctop].start;
      while (x_cmid >= 0) {
         x_amid  = s_menus [x_cmid].mid;
         x_last  = -1;
         for (i = 0; i < s_nreal; ++i) {
            /*---(filter)------------*/
            if (s_menus [i].top != x_atop)                  continue;
            if (s_menus [i].mid != x_amid)                  continue;
            if (s_menus [i].bot ==  0 )                     continue;
            if (s_menus [i].bot == '·')                     continue;
            if (strchr (s_valid, s_menus [i].bot) == NULL)  continue;
            /*---(update)------------*/
            if (x_last <  0)  s_menus [x_cmid].start = i;
            else              s_menus [x_last].next  = i;;
            ++s_menus [x_cmid].count;
            x_last = i;
            /*---(done)--------------*/
         }
         x_cmid = s_menus [x_cmid].next;
      }
      x_ctop = s_menus [x_ctop].next;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_rptg      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(reporting)----------------------*/
   for (i = 0; i < s_nreal; ++i) {
      DEBUG_CMDS   yLOG_complex ("entry"     , "%3d %c %c %c %-10.10s %3d %3d %3d", i, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].name, s_menus [i].next, s_menus [i].start, s_menus [i].count);
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_update    (int n)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_last      =    0;
   s_menus [n].start = -1;
   s_menus [n].count =  0;
   x_last  = -1;
   for (i = 0; i < s_nreal; ++i) {
      /*---(filter)------------*/
      if (s_menus [i].top != s_menus [n].top)         continue;
      if (s_menus [i].mid != s_menus [n].mid)         continue;
      if (s_menus [i].bot ==  0 )                     continue;
      if (s_menus [i].bot == '·')                     continue;
      if (strchr (s_valid, s_menus [i].bot) == NULL)  continue;
      /*---(update)------------*/
      if (x_last <  0)  s_menus [n].start = i;
      else              s_menus [x_last].next  = i;;
      ++s_menus [n].count;
      x_last = i;
      /*---(done)--------------*/
   }
   /*> yvikeys__menu_rptg ();                                                         <*/
   return 0;
}

char
yvikeys_menu_reanchor     (int a_anchor)
{
   return yvikeys_view_reanchor (YVIKEYS_MENUS, a_anchor);
}

char
yvikeys_menu_prepare    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_COMMAND)) {
      DEBUG_CMDS   yLOG_note    ("status is not ready for init");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(menus)--------------------------*/
   DEBUG_CMDS   yLOG_note    ("initialize menu system");
   yvikeys__menu_count  ();
   yvikeys__menu_unlink ();
   yvikeys__menu_tops   ();
   yvikeys__menu_mids   ();
   yvikeys__menu_bots   ();
   yvikeys__menu_rptg   ();
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
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
yvikeys_menu_wrap       (void)
{
   yvikeys__menu_unlink ();
   yvikeys__menu_cleanse ();
   return 0;
}

char
yvikeys__menu_compress  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         j           =    0;
   int         c           =    0;
   int         a           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   a = s_nreal;
   for (i = 0; i < a; ++i) {
      DEBUG_CMDS   yLOG_complex ("check"     , "%3d %c %c %c %c %-10.10s", i, s_menus [i].base, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].name);
      if (s_menus [i].base != 'y')  continue;
      if (s_menus [i].top  != '·')  continue;
      for (j = i + 1; j < a; ++j) {
         DEBUG_CMDS   yLOG_complex ("move up"   , "%3d %c %c %c %-10.10s", j, s_menus [j].top, s_menus [j].mid, s_menus [j].bot, s_menus [j].name);
         s_menus [j - 1].base    = s_menus [j].base;
         s_menus [j - 1].top     = s_menus [j].top;
         s_menus [j - 1].mid     = s_menus [j].mid;
         s_menus [j - 1].bot     = s_menus [j].bot;
         strlcpy (s_menus [j - 1].name, s_menus [j].name, LEN_LABEL);
         s_menus [j - 1].active  = s_menus [j].active;
         s_menus [j - 1].type    = s_menus [j].type;
         strlcpy (s_menus [j - 1].keys, s_menus [j].keys, LEN_HUND );
         ++c;
      }
      --a;
      --s_nreal;
      --s_nmenu;
      yvikeys__menu_wipe (a);
      --i;
   }
   DEBUG_CMDS   yLOG_value   ("c"         , c);
   DEBUG_CMDS   yLOG_value   ("s_nreal"   , s_nreal);
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
   /*---(relink)-------------------------*/
   yvikeys_menu_prepare ();
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys__menu_find      (char *a_path, char *a_level, int *a_last)
{
   char        rce         =  -10;
   int         i           =    0;
   char        x_ch        =  '-';
   if (a_level != NULL)  *a_level =  0;
   if (a_last  != NULL)  *a_last  = -1;
   --rce;  if (a_path  == NULL)        return rce;
   --rce;  if (a_level == NULL)        return rce;
   --rce;  if (a_last  == NULL)        return rce;
   --rce;  if (a_path [0] != '\\' && a_path [0] != 'µ')     return rce;
   /*---(level one)----------------------*/
   x_ch = a_path [1];
   --rce;  if (x_ch == '\0')  return rce;
   while (i >= 0) {
      if (s_menus [i].top == x_ch)    break;
      i = s_menus [i].next;
   }
   --rce;  if (i < 0) return rce;
   *a_level = 1;
   *a_last  = i;
   /*---(level two)----------------------*/
   x_ch = a_path [2];
   if (x_ch == '\0')  return i;
   i = s_menus [i].start;
   while (i >= 0) {
      if (s_menus [i].mid == x_ch)   break;
      i = s_menus [i].next;
   }
   --rce;  if (i < 0) return rce;
   *a_level = 2;
   *a_last  = i;
   /*---(level three)--------------------*/
   x_ch = a_path [3];
   if (x_ch == '\0')  return i;
   i = s_menus [i].start;
   while (i >= 0) {
      if (s_menus [i].bot == x_ch)   break;
      i = s_menus [i].next;
   }
   --rce;  if (i < 0) return rce;
   *a_level = 3;
   *a_last  = i;
   /*---(too long)-----------------------*/
   --rce;  if (strlen (a_path) > 4)    return rce;
   /*---(complete)-----------------------*/
   return i;
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
yvikeys__menu_back      (int a_len, int a_level, int a_last)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_on        =  '-';
   int         x_left, x_wide, x_bott, x_tall, x_top, x_mid;
   int         x_xmin, x_xmax, x_ymin, x_ymax;
   int         i           =    0;
   int         t           [LEN_RECD];
   int         x_len       =    0;
   int         x_adj       =    5;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_value   ("a_len"     , a_len);
   DEBUG_CMDS   yLOG_value   ("a_level"   , a_level);
   DEBUG_CMDS   yLOG_value   ("a_last"    , a_last);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   s_pt     = myVIKEYS.point;
   /*---(opengl)-------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      /*> x_adj  = 15.0;                                                              <*/
      s_xoff = 15.0;
      /*---(get sizes)----------------------*/
      DEBUG_CMDS   yLOG_note    ("draw black background");
      s_type   = yVIKEYS_view_type   (YVIKEYS_MENUS);
      s_anchor = yVIKEYS_view_anchor (YVIKEYS_MENUS);
      if (s_type == YVIKEYS_DEPTH) {
         s_x    =  35;
         s_y    =  25;
         s_z    = -150;
         s_pt   = 2;
         s_xoff = 15.0 / 4.0;
         s_yoff = 15.0 / 4.0;
         x_adj  = 35.0;
      }
      x_on = yVIKEYS_view_bounds (YVIKEYS_MAIN , &x_xmin, &x_xmax, NULL, &x_ymin, &x_ymax, NULL);
      DEBUG_CMDS   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy, %3dz", x_xmin, x_xmax, x_ymin, x_ymax, s_z);
      /*---(set x center)----------------*/
      switch (s_anchor) {
      case YVIKEYS_TOPRIG : case YVIKEYS_MIDRIG : case YVIKEYS_BOTRIG   :
         if (s_type == YVIKEYS_DEPTH)   s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0 + x_adj;
         else                           s_ce  = x_xmax - s_x;
         break;
      case YVIKEYS_TOPCEN : case YVIKEYS_MIDCEN : case YVIKEYS_BOTCEN   :
         s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0;
         break;
      case YVIKEYS_TOPLEF : case YVIKEYS_MIDLEF : case YVIKEYS_BOTLEF   :
         if (s_type == YVIKEYS_DEPTH)   s_ce  = x_xmin + (x_xmax - x_xmin) / 2.0 - x_adj;
         else                           s_ce  = x_xmin + s_x;
         break;
      }
      DEBUG_CMDS   yLOG_value   ("s_ce"         , s_ce);
      /*---(set y midpoint)--------------*/
      switch (s_anchor) {
      case YVIKEYS_TOPLEF : case YVIKEYS_TOPCEN : case YVIKEYS_TOPRIG :
         if (s_type == YVIKEYS_DEPTH)   s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0 + x_adj;
         else                           s_mi  = x_ymax - s_x;
         break;
      case YVIKEYS_MIDLEF : case YVIKEYS_MIDCEN : case YVIKEYS_MIDRIG :
         s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0;
         break;
      case YVIKEYS_BOTLEF : case YVIKEYS_BOTCEN : case YVIKEYS_BOTRIG :
         if (s_type == YVIKEYS_DEPTH)   s_mi  = x_ymin + (x_ymax - x_ymin) / 2.0 - x_adj;
         else                           s_mi  = x_ymax + s_x;
         break;
      }
      DEBUG_CMDS   yLOG_value   ("s_mi"         , s_mi);
      /*---(black background)---------------*/
      glColor4f (0.0f, 0.0f, 0.0f, 1.0f);
      glBegin(GL_POLYGON); {
         glVertex3f (s_ce - s_x, s_mi + s_y, s_z);
         glVertex3f (s_ce + s_x, s_mi + s_y, s_z);
         glVertex3f (s_ce + s_x, s_mi - s_y, s_z);
         glVertex3f (s_ce - s_x, s_mi - s_y, s_z);
      } glEnd();
      /*> glBegin(GL_POLYGON); {                                                      <* 
       *>    glVertex3f (  10.0,   -70.0,    90.0);                                   <* 
       *>    glVertex3f ( 290.0,   -70.0,    90.0);                                   <* 
       *>    glVertex3f ( 290.0,  -270.0,    90.0);                                   <* 
       *>    glVertex3f (  10.0,  -270.0,    90.0);                                   <* 
       *> } glEnd();                                                                  <*/
      /*---(color fill)---------------------*/
      if (a_len != a_level) {
         glColor4f (0.8f, 0.0f, 0.0f, 1.0f);
      } else {
         switch (a_level) {
         case  0 :  glColor4f (0.3f, 0.7f, 0.7f, 1.0f);  break;
         case  1 :  glColor4f (0.7f, 0.5f, 0.3f, 1.0f);  break;
         case  2 :  glColor4f (0.3f, 0.8f, 0.3f, 1.0f);  break;
         }
      }
      glBegin(GL_POLYGON); {
         glVertex3f (s_ce - s_x * 0.97, s_mi + s_y * 0.97, s_z + 2);
         glVertex3f (s_ce + s_x * 0.97, s_mi + s_y * 0.97, s_z + 2);
         glVertex3f (s_ce + s_x * 0.97, s_mi - s_y * 0.97, s_z + 2);
         glVertex3f (s_ce - s_x * 0.97, s_mi - s_y * 0.97, s_z + 2);
      } glEnd();
      /*> glBegin(GL_POLYGON); {                                                      <* 
       *>    glVertex3f (  12.0,   -72.0,   100.0);                                   <* 
       *>    glVertex3f ( 288.0,   -72.0,   100.0);                                   <* 
       *>    glVertex3f ( 288.0,  -268.0,   100.0);                                   <* 
       *>    glVertex3f (  12.0,  -268.0,   100.0);                                   <* 
       *> } glEnd();                                                                  <*/
      /*---(labels)-------------------------*/
      glPushMatrix(); {
         glColor4f (0.0, 0.0, 0.0, 1.0);
         glTranslatef(s_ce, s_mi, s_z + 4);
         glTranslatef(0.0 , s_y * 0.86, 0.0);
         switch (a_level) {
         case  0 : strlcpy (t, "main menu", LEN_LABEL); break;
         case  1 : sprintf (t, "\\%c (%s) sub-menu", s_menus [a_last].top, s_menus [a_last].name); break;
         case  2 : sprintf (t, "\\%c%c (%s) options", s_menus [a_last].top, s_menus [a_last].mid, s_menus [a_last].name); break;
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
   }
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      s_xoff =  2.0;
      /*---(get sizes)----------------------*/
      x_on = yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
      DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
      x_top = x_bott - x_tall;
      x_mid = x_left + (x_wide / 2);
      yCOLOR_curs ("menu"   );
      for (i = 0; i < x_tall; ++i) {
         mvprintw   (x_top + i, x_left, "%*.*s", x_wide, x_wide, "                                                  ");
      }
      switch (a_level) {
      case  0 : strlcpy (t, "main menu", LEN_LABEL); break;
      case  1 : sprintf (t, "\\%c (%s) sub-menu", s_menus [a_last].top, s_menus [a_last].name); break;
      case  2 : sprintf (t, "\\%c%c (%s) options", s_menus [a_last].top, s_menus [a_last].mid, s_menus [a_last].name); break;
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
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_entry_gl  (int i, int a_level, int a_group, int a_entry, float x, float y, float a_offx, float a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   int         t           [LEN_LABEL];
   int         x_align     =    0;
   glPushMatrix(); {
      /*---(recenter)--------------------*/
      glTranslatef (s_ce, s_mi, s_z + 6);
      glTranslatef (0.0 , s_y * 0.56, 0.0);
      /*---(color)-----------------------*/
      switch (s_menus [i].type) {
      case '>' :
         if (s_menus [i].count  == 0) glColor4f (0.4, 0.4, 0.4, 1.0);
         else glColor4f (0.0, 0.0, 0.0, 1.0);
         break;
      case '!' :
         glColor4f (0.4, 0.0, 0.0, 1.0);
         break;
      case '·' : case '=' :
         if      (s_menus [i].active != 'y')   glColor4f (0.4, 0.4, 0.4, 1.0);
         else if (s_menus [i].keys [0] == '-') glColor4f (0.4, 0.0, 0.0, 1.0);
         else glColor4f (0.0, 0.0, 0.0, 1.0);
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
      case 0  : sprintf (t, "%c", s_menus [i].top);   break;
      case 1  : sprintf (t, "%c", s_menus [i].mid);   break;
      case 2  : sprintf (t, "%c", s_menus [i].bot);   break;
      }
      yFONT_print (myVIKEYS.font, s_pt, x_align, t);
      glTranslatef (a_offx, a_offy, 0.0);
      sprintf (t, "%.9s", s_menus [i].name);
      if (a_level > 0 && s_menus [i].type == '>')  strlcat (t, "+", LEN_LABEL);
      if (a_level > 0 && s_menus [i].type == '=')  strlcat (t, "=", LEN_LABEL);
      yFONT_print (myVIKEYS.font, s_pt, x_align, t);
      /*> glTranslatef (-a_offx, -a_offy, 0.0);                                       <*/
   } glPopMatrix();
   return 0;
}

char
yvikeys__menu_entry_nc  (int i, int a_mid, int a_top, int a_level, int a_group, int a_entry, int a_offx, int a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x, y;
   int         t           [LEN_LABEL];
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(abbrev)-------------------------*/
   switch (a_level) {
   case 0  : sprintf (t, "%c", s_menus [i].top);   break;
   case 1  : sprintf (t, "%c", s_menus [i].mid);   break;
   case 2  : sprintf (t, "%c", s_menus [i].bot);   break;
   }
   DEBUG_CMDS   yLOG_complex ("who"       , "%3d %s %s", i, t, s_menus [i].name);
   /*---(color)--------------------------*/
   switch (s_menus [i].type) {
   case '>' :
      if (s_menus [i].count  == 0)  yCOLOR_curs ("menu_na" );
      else                          yCOLOR_curs ("menu"    );
      break;
   case '!' :
      yCOLOR_curs ("menu_bad");
      break;
   case '·' : case '=' :
      if (s_menus [i].active != 'y')        yCOLOR_curs ("menu_na" );
      else if (s_menus [i].keys [0] == '-') yCOLOR_curs ("menu_bad");
      else                                  yCOLOR_curs ("menu"    );
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
   sprintf (t, "%.9s", s_menus [i].name);
   if (a_level > 0 && s_menus [i].type == '>')  strlcat (t, "+", LEN_LABEL);
   if (a_level > 0 && s_menus [i].type == '=')  strlcat (t, "=", LEN_LABEL);
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
yvikeys__menu_column    (int a_level, int a_count, int i)
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
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(col config)---------------------*/
   a = a_count;
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
   while (i >= 0) {
      /*---(filter)----------------------*/
      if (s_menus [i].active == '/') {
         i = s_menus [i].next;
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
      case YVIKEYS_OPENGL :  yvikeys__menu_entry_gl (i, a_level, x_group, x_entry, 0, 0, x_xoff, 0.0); break;
      case YVIKEYS_CURSES :  yvikeys__menu_entry_nc (i, x_mid, x_top , a_level, x_group, x_entry, 2.0, 0.0); break;
      }
      /*---(figure next)-----------------*/
      i = s_menus [i].next;
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
yvikeys__menu_main      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =   0;
   int         c           =    0;
   int         t           [LEN_LABEL];
   char        x_on        =  '-';
   int         x_left, x_wide, x_bott, x_tall, x_top, x_mid;
   int         x, y;
   int         x_len       =    0;
   float       x_offx      =    0;
   float       x_offy      =    0;
   float       x_pos, y_pos;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(opengl)-------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      glPushMatrix(); {
         i = 0;
         glPushMatrix(); {
            /*> glTranslatef( 150.0, -105.0, 120.0);                                  <*/
            while (i >= 0) {
               DEBUG_CMDS   yLOG_value   ("i"         , i);
               DEBUG_CMDS   yLOG_char    ("abbr"      , s_menus [i].top);
               DEBUG_CMDS   yLOG_info    ("name"      , s_menus [i].name);
               /*---(filter)----------------------*/
               if (s_menus [i].active == '/') {
                  i = s_menus [i].next;
                  continue;
               }
               switch (s_align [c]) {
               case 1 :  x_offx =   1;  x_offy =  0;          break;
               case 2 :  x_offx =   0;  x_offy = s_abbr [c];  break;
               case 3 :  x_offx =  -1;  x_offy =  0;          break;
               }
               x_pos = s_xpos [c];
               y_pos = s_ypos [c] - 4.0;
               if (s_type == YVIKEYS_FLAT) {
                  x_pos  *=  8.0;
                  y_pos  *= 14.0;
                  x_offy *= 14.0;
                  x_offx *= 15.0;
               } else {
                  x_pos  *=  2.0;
                  y_pos  *=  3.5;
                  x_offy *=  3.0;
                  x_offx *=  3.0;
                  /*> x_offy  =  0.0;                                                 <*/
                  /*> x_offx  =  0.0;                                                 <*/
               }
               yvikeys__menu_entry_gl (i, 0, 0, 0, x_pos, y_pos, x_offx, x_offy);
               i = s_menus [i].next;
               ++c;
               DEBUG_CMDS   yLOG_value   ("next"      , i);
            }
            /*> yvikeys_menu__round (myVIKEYS.font);                                            <*/
         } glPopMatrix();
      } glPopMatrix();
   }
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      x_top = x_bott - x_tall + 2;
      x_mid = x_left + (x_wide / 2);
      while (i >= 0) {
         DEBUG_CMDS   yLOG_value   ("i"         , i);
         DEBUG_CMDS   yLOG_char    ("abbr"      , s_menus [i].top);
         DEBUG_CMDS   yLOG_info    ("name"      , s_menus [i].name);
         /*---(filter)----------------------*/
         if (s_menus [i].active == '/') {
            i = s_menus [i].next;
            continue;
         }
         x = x_mid + trunc (s_xpos [c]);        /* 6 is more ellipse/rounded */
         y = x_top - s_ypos [c] + 4;   
         switch (s_align [c]) {
         case 1 :  x_offx =   2;  x_offy =  0;               break;
         case 2 :  x_offx =   0;  x_offy = s_abbr [c];       break;
         case 3 :  x_offx =  -2;  x_offy =  0;               break;
         }
         yvikeys__menu_entry_nc (i, x_mid, x_top, 0, x, y, x_offx, x_offy);
         i = s_menus [i].next;
         ++c;
         DEBUG_CMDS   yLOG_value   ("next"      , i);
      }
   }
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_menu_draw          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   int         x_point     =    8;
   int         x_len       =    0;
   char        x_level     =    0;
   int         x_last      =    0;
   /*---(header)----------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   if (MODE_not (SMOD_MENUS  )) {
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(find path)----------------------*/
   n = yvikeys__menu_find (myVIKEYS.m_path, &x_level, &x_last);
   DEBUG_USER   yLOG_value   ("n"         , n);
   DEBUG_USER   yLOG_value   ("x_level"   , x_level);
   DEBUG_USER   yLOG_value   ("x_last"    , x_last);
   /*---(draw back)----------------------*/
   DEBUG_USER   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   x_len = strlen (myVIKEYS.m_path);
   DEBUG_USER   yLOG_value   ("x_len"     , x_len);
   yvikeys__menu_back (x_len - 1, x_level, x_last);
   /*---(main menu)----------------------*/
   switch (x_level) {
   case 0 :  yvikeys__menu_main ();        break;
   case 1 :  yvikeys__menu_column (x_level, s_menus [x_last].count, s_menus [x_last].start);  break;
   case 2 :  yvikeys__menu_column (x_level, s_menus [x_last].count, s_menus [x_last].start);  break;
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
   n = yvikeys__menu_find (myVIKEYS.m_path, &x_level, &x_last);
   DEBUG_USER   yLOG_value   ("n"         , n);
   DEBUG_USER   yLOG_value   ("x_level"   , x_level);
   DEBUG_USER   yLOG_value   ("x_last"    , x_last);
   /*---(draw back)----------------------*/
   DEBUG_USER   yLOG_info    ("m_path"    , myVIKEYS.m_path);
   x_len = strlen (myVIKEYS.m_path);
   DEBUG_USER   yLOG_value   ("x_len"     , x_len);
   /*---(check exec)---------------------*/
   --rce;  if (n >= 0 && x_len - 1 == x_level) {
      DEBUG_USER   yLOG_char    ("type"      , s_menus [n].type);
      if (strchr ("·=", s_menus [n].type) != NULL) {
         MODE_exit ();
         DEBUG_USER   yLOG_info    ("keys"      , s_menus [n].keys);
         if (strlen (s_menus [n].keys) == 1 && s_menus [n].keys [0] == '-') {
            DEBUG_USER   yLOG_note    ("legal menu option, but action not set");
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         SOURCE_menu_prep ();
         strlcpy (myVIKEYS.m_keys, s_menus [n].keys, LEN_HUND);
         DEBUG_USER   yLOG_info    ("m_keys"    , myVIKEYS.m_keys);
         yvikeys_macro_menu_beg ();
         myVIKEYS.log_keys = '-';
         yVIKEYS_main_string (myVIKEYS.m_keys);
         myVIKEYS.log_keys = 'y';
         yvikeys_macro_menu_end ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      } else if (s_menus [n].type == '>') {
         DEBUG_USER   yLOG_note    ("group menu option, continue to next level");
      }
   }
   /*---(check exec)---------------------*/
   --rce;  if (n < 0) {
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
yVIKEYS_menu_alter      (char *a_path, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   int         x_len       =    0;
   char        x_path      [LEN_LABEL];
   char        x_dup       =    0;
   int         x_level     =    0;
   int         x_last      =    0;
   int         i           =    0;
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
   DEBUG_CMDS   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_path"    , a_path);
   DEBUG_CMDS   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_keys"    , a_keys);
   /*---(path)---------------------------*/
   x_len = strlen (a_path);
   DEBUG_CMDS   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 3 || x_len > 4) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (x_path, a_path, LEN_LABEL);
   n = yvikeys__menu_find (x_path, &x_level, &x_last);
   DEBUG_CMDS   yLOG_value   ("find"      , n);
   --rce;  if (n < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(if making a group)--------------*/
   --rce;  if (strcmp (a_keys, ">") == 0) {
      if (x_level != 2) {
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      s_menus [n].type   = '>';
      strlcpy (s_menus [n].keys, "-", LEN_HUND );
      s_menus [n].active = 'y';
   }
   /*---(if making a executable)---------*/
   else {
      s_menus [n].type   = '·';
      strlcpy (s_menus [n].keys, a_keys, LEN_HUND );
      if (x_level == 2) {
         s_menus [n].active = 'y';
         i = s_menus [n].start;
         while (i >= 0) {
            s_menus [i].top    = '·';
            s_menus [i].mid    = '·';
            s_menus [i].bot    = '·';
            i = s_menus [i].next;
         }
         yvikeys__menu_compress ();
      }
   }
   /*---(update count)-------------------*/
   DEBUG_CMDS   yLOG_value   ("SUCCESS"   , s_nmenu);
   yvikeys__menu_update (n);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_menu_add        (char *a_path, char *a_name, char *a_keys)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   int         x_len       =    0;
   char        x_path      [LEN_LABEL];
   char        x_dup       =    0;
   int         x_level     =    0;
   int         x_last      =    0;
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
   DEBUG_CMDS   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_path"    , a_path);
   DEBUG_CMDS   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_name"    , a_name);
   DEBUG_CMDS   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_keys"    , a_keys);
   /*---(path)---------------------------*/
   x_len = strlen (a_path);
   DEBUG_CMDS   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len != 4) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (x_path, a_path, LEN_LABEL);
   x_path [3] = '\0';
   DEBUG_CMDS   yLOG_info    ("x_path"    , x_path);
   n = yvikeys__menu_find (x_path, &x_level, &x_last);
   DEBUG_CMDS   yLOG_value   ("find"      , n);
   --rce;  if (n < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_menus [s_nmenu].base   = '-';
   s_menus [s_nmenu].top    = a_path [1];
   s_menus [s_nmenu].mid    = a_path [2];
   s_menus [s_nmenu].bot    = a_path [3];
   /*---(name and keys)------------------*/
   strlcpy (s_menus [s_nmenu].name, a_name, LEN_LABEL);
   strlcpy (s_menus [s_nmenu].keys, a_keys, LEN_HUND );
   x_len = strlen (a_keys);
   if      (a_keys [x_len - 1] == '¦' ) s_menus [s_nmenu].type   = '·';
   else if (a_keys [x_len - 1] == '\n') s_menus [s_nmenu].type   = '·';
   else if (a_keys [0]         != ':' ) s_menus [s_nmenu].type   = '·';
   else                                 s_menus [s_nmenu].type   = '=';
   /*---(fill in)------------------------*/
   s_menus [s_nmenu].active = 'y';
   s_menus [s_nmenu].next   =  -1;
   s_menus [s_nmenu].start  =  -1;
   s_menus [s_nmenu].count  =   0;
   /*---(update count)-------------------*/
   ++s_nmenu;
   DEBUG_CMDS   yLOG_value   ("SUCCESS"   , s_nmenu);
   yvikeys__menu_update (n);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_addgroup  (char a_top, char a_mid, char *a_name)
{
   s_menus [s_nmenu].top    = a_top;
   s_menus [s_nmenu].mid    = a_mid;
   strlcpy (s_menus [s_nmenu].name, a_name, LEN_LABEL);
   s_menus [s_nmenu].type   = '>';
   s_menus [s_nmenu].active = 'y';
   strlcpy (s_menus [s_nmenu].keys, "-"   , LEN_HUND );
   yvikeys_menu_prepare ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       updating active flag                   ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIVATION______o () { return; }

int
yvikeys__menu_activate  (char a_set, char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =   -1;
   int         x_len       =    0;
   int         x_level     =    0;
   char        x_path      [LEN_LABEL];
   int         x_last      =    0;
   int         i           =    0;
   char        x_top       =  '·';
   char        x_mid       =  '·';
   char        x_bot       =  '·';
   int         c           =    0;
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
   DEBUG_CMDS   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_path"    , a_path);
   /*---(path)---------------------------*/
   x_len = strlen (a_path);
   DEBUG_CMDS   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <  1 || x_len > 4) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_path [0] != 'µ' && a_path [0] != '\\') {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (x_len) {
   case  4 :  x_bot = a_path [3];
   case  3 :  x_mid = a_path [2];
   case  2 :  x_top = a_path [1];
   }
   /*---(alter active)-------------------*/
   DEBUG_CMDS   yLOG_complex ("keys"      , "%c %c %c", x_top, x_mid, x_bot);
   for (i = 0; i < s_nmenu; ++i) {
      DEBUG_CMDS   yLOG_complex ("check"     , "%3d %c %c %c %c %-10.10s", i, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].active, s_menus [i].name);
      if (x_top != '·' && s_menus [i].top != x_top)  continue;
      if (x_mid != '·' && s_menus [i].mid != x_mid)  continue;
      if (x_bot != '·' && s_menus [i].bot != x_bot)  continue;
      DEBUG_CMDS   yLOG_note    ("FOUND");
      s_menus [i].active = a_set;
      ++c;
   }
   /*---(handle parents)-----------------*/
   if (a_set == 'y' && c > 0) {
      if (x_len >= 4) {
         strlcpy (x_path, a_path, LEN_LABEL);
         x_path [3] = '\0';
         DEBUG_CMDS   yLOG_info    ("x_path"    , x_path);
         n = yvikeys__menu_find (x_path, &x_level, &x_last);
         DEBUG_CMDS   yLOG_value   ("find"      , n);
         if (n >= 0) {
            s_menus [n].active = a_set;
            ++c;
         }
      }
      if (x_len >= 3) {
         strlcpy (x_path, a_path, LEN_LABEL);
         x_path [2] = '\0';
         DEBUG_CMDS   yLOG_info    ("x_path"    , x_path);
         n = yvikeys__menu_find (x_path, &x_level, &x_last);
         DEBUG_CMDS   yLOG_value   ("find"      , n);
         if (n >= 0) {
            s_menus [n].active = a_set;
            ++c;
         }
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return c;
}

int yVIKEYS_menu_active   (char *a_path) { return yvikeys__menu_activate ('y', a_path); }
int yVIKEYS_menu_deactive (char *a_path) { return yvikeys__menu_activate ('-', a_path); }
int yVIKEYS_menu_hide     (char *a_path) { return yvikeys__menu_activate ('/', a_path); }



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
   char        x_level     =    0;
   int         x_last      =    0;
   char        t           [LEN_HUND ];
   char        s           [LEN_LABEL] = "entry";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MENU unit        : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "count"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU count       : %4d %4d", s_nreal, s_nmenu);
   }
   else if (strcmp (a_question, "index"          )   == 0) {
      i = a_path;
      strlcpy    (t, s_menus [i].keys, LEN_HUND );
      strlencode (t, ySTR_MAX, LEN_LABEL);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU index       : %-2d %c %c %c %-10.10s %c %c %-10.10s %3d %3d %3d", i, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].name, s_menus [i].active, s_menus [i].type, t, s_menus [i].next, s_menus [i].start, s_menus [i].count);
   }
   else if (strcmp (a_question, "entry"          )   == 0) {
      i = yvikeys__menu_find (a_path, &x_level, &x_last);
      if (i < 0 && x_last < 0) {
         strlcpy  (yVIKEYS__unit_answer, "MENU unit        : menu item not found", LEN_FULL);
      }
      else {
         if (i < 0) {
            i = x_last;
            strlcpy (s, "last ", LEN_LABEL);
         }
         strlcpy    (t, s_menus [i].keys, LEN_HUND );
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU %-5.5s       : %1d %3d %-10.10s  %c  %c  %s", s, x_level, i     , s_menus [i].name, s_menus [i].active, s_menus [i].type, t);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

