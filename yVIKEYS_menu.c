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
 *
 *
 */



/*===[[ MENUS ]]==============================================================*/
#define    MAX_MENU        1000
typedef    struct   cMENU   tMENU;
struct cMENU {
   /*---(organize)----------*/
   char        top;                         /* first level key                */
   char        mid;                         /* second level key               */
   char        bot;                         /* third level key                */
   /*---(master)------------*/
   char        name        [LEN_LABEL];     /* name for use on screens        */
   char        active;                      /* program activation             */
   char        type;                        /* entry type   > ! = ·           */
   char        keys        [LEN_LABEL];     /* command or keys to execute     */
   /*---(traverse)----------*/
   short       next;                        /* next sibling                   */
   short       start;                       /* first child                    */
   short       count;                       /* count of children              */
   /*---(done)--------------*/
};
static int    s_nmenu  = 0;
static int    s_ntops  = 0;
static tMENU  s_menus [MAX_MENU] = {
   /*---(file menu)----------------------------------------------*/
   { 'f', '·', '·', "files"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'W', '·', "workspace"        , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'w', '·', "new"              , 'y', '·', ":new¦"             , 0, 0, 0 },
   { 'f', 'c', '·', "chdir"            , 'y', '=', ":cd·"              , 0, 0, 0 },
   { 'f', 'b', '·', "browse"           , 'y', '=', ":browse·"          , 0, 0, 0 },
   { 'f', 'n', '·', "rename"           , 'y', '=', ":file·"            , 0, 0, 0 },
   { 'f', 'o', '·', "read"             , 'y', '·', ":read¦"            , 0, 0, 0 },
   { 'f', 'm', '·', "import"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'f', '·', "refresh"          , 'y', '·', ":refresh¦"         , 0, 0, 0 },
   { 'f', 'r', '·', "restore"          , '·', '!', "-"                 , 0, 0, 0 },
   { 'f', 'v', '·', "version"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'p', '·', "protect"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 's', '·', "write"            , 'y', '·', ":write¦"           , 0, 0, 0 },
   { 'f', 'a', '·', "writeas"          , 'y', '=', ":writeas·"         , 0, 0, 0 },
   { 'f', 'x', '·', "export"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'q', '·', "quit"             , 'y', '·', ":quit¦"            , 0, 0, 0 },
   /*---(edit menu)----------------------------------------------*/
   { 'e', '·', '·', "edit"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'u', '·', "undo"             , 'y', '·', "u"                 , 0, 0, 0 },
   { 'e', 'U', '·', "redo"             , 'y', '·', "U"                 , 0, 0, 0 },
   { 'e', '_', '·', "status"           , 'y', '·', ":status mundo¦"    , 0, 0, 0 },
   { 'e', '?', '·', "history"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', '?', 't', "track"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'u', "untrack"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'c', "clear"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'r', "review"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'y', '·', "copy"             , 'y', '·', "y"                 , 0, 0, 0 },
   { 'e', 'Y', '·', "cut"              , 'y', '·', "Y"                 , 0, 0, 0 },
   { 'e', 'p', '·', "paste"            , 'y', '·', "p"                 , 0, 0, 0 },
   { 'e', 'P', '·', "special"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'x', '·', "clear"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'x', '.', "inplace"          , 'y', '·', "x."                , 0, 0, 0 },
   { 'e', 'x', 'l', "left"             , 'y', '·', "xl"                , 0, 0, 0 },
   { 'e', 'x', 'h', "right"            , 'y', '·', "xh"                , 0, 0, 0 },
   { 'e', 'x', 'k', "up"               , 'y', '·', "xk"                , 0, 0, 0 },
   { 'e', 'x', 'j', "down"             , 'y', '·', "xj"                , 0, 0, 0 },
   { 'e', 'x', 'i', "in"               , 'y', '·', "xi"                , 0, 0, 0 },
   { 'e', 'x', 'o', "out"              , 'y', '·', "xo"                , 0, 0, 0 },
   { 'e', 'x', 'x', "col_lef"          , 'y', '·', "xx"                , 0, 0, 0 },
   { 'e', 'x', 'X', "col_rig"          , 'y', '·', "xX"                , 0, 0, 0 },
   { 'e', 'x', 'y', "row_abo"          , 'y', '·', "xy"                , 0, 0, 0 },
   { 'e', 'x', 'Y', "row_bel"          , 'y', '·', "xY"                , 0, 0, 0 },
   { 'e', 'x', 'z', "lvl_in"           , 'y', '·', "xz"                , 0, 0, 0 },
   { 'e', 'x', 'Z', "lvl_out"          , 'y', '·', "xZ"                , 0, 0, 0 },
   { 'e', 'd', '·', "delete"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'd', 'l', "left"             , 'y', '·', "dl"                , 0, 0, 0 },
   { 'e', 'd', 'h', "right"            , 'y', '·', "dh"                , 0, 0, 0 },
   { 'e', 'd', 'k', "up"               , 'y', '·', "dk"                , 0, 0, 0 },
   { 'e', 'd', 'j', "down"             , 'y', '·', "dj"                , 0, 0, 0 },
   { 'e', 'd', 'i', "in"               , 'y', '·', "di"                , 0, 0, 0 },
   { 'e', 'd', 'o', "out"              , 'y', '·', "do"                , 0, 0, 0 },
   { 'e', 'd', 'x', "col_left"         , 'y', '·', "dx"                , 0, 0, 0 },
   { 'e', 'd', 'X', "col_right"        , 'y', '·', "dX"                , 0, 0, 0 },
   { 'e', 'd', 'y', "row_above"        , 'y', '·', "dy"                , 0, 0, 0 },
   { 'e', 'd', 'Y', "row_below"        , 'y', '·', "dY"                , 0, 0, 0 },
   { 'e', 'd', 'z', "lvl_in"           , 'y', '·', "dz"                , 0, 0, 0 },
   { 'e', 'd', 'Z', "lvl_out"          , 'y', '·', "dZ"                , 0, 0, 0 },
   { 'e', 'i', '·', "insert"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'i', 'l', "left"             , 'y', '·', "al"                , 0, 0, 0 },
   { 'e', 'i', 'h', "right"            , 'y', '·', "ah"                , 0, 0, 0 },
   { 'e', 'i', 'k', "up"               , 'y', '·', "ak"                , 0, 0, 0 },
   { 'e', 'i', 'j', "down"             , 'y', '·', "aj"                , 0, 0, 0 },
   { 'e', 'i', 'i', "in"               , 'y', '·', "ai"                , 0, 0, 0 },
   { 'e', 'i', 'o', "out"              , 'y', '·', "ao"                , 0, 0, 0 },
   { 'e', 'i', 'x', "col_lef"          , 'y', '·', "ax"                , 0, 0, 0 },
   { 'e', 'i', 'X', "col_rig"          , 'y', '·', "aX"                , 0, 0, 0 },
   { 'e', 'i', 'y', "row_abo"          , 'y', '·', "ay"                , 0, 0, 0 },
   { 'e', 'i', 'Y', "row_bel"          , 'y', '·', "aY"                , 0, 0, 0 },
   { 'e', 'i', 'z', "lvl_in"           , 'y', '·', "az"                , 0, 0, 0 },
   { 'e', 'i', 'Z', "lvl_out"          , 'y', '·', "aZ"                , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', "-------------"     , 0, 0, 0 },
   { 'e', 'g', '·', "group"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'g', "group"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'u', "ungroup"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'r', "regroup"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'm', "merge"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'f', "free"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'b', "boundary"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', '·', "align"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'k', "top"              , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'j', "bottom"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'h', "left"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'l', "right"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'o', "front"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'i', "back"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'x', "dist-x"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'y', "dist-y"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'z', "dist-z"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', '·', "order"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'n', "front"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'i', "forward"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'o', "backward"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'f', "back"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'r', "reverse"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'l', '·', "layer"            , 'y', '=', "-"                 , 0, 0, 0 },
   /*---(view menu)----------------------------------------------*/
   { 'v', '·', '·', "view"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'w', '·', "window"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'l', '·', "layout"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 't', '·', "title"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'v', '·', "version"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'b', '·', "buffers"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'f', '·', "formula"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'n', '·', "nav"              , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'p', '·', "progress"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 's', '·', "status"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'k', '·', "keys"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'c', '·', "command"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'a', '·', "alt"              , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'd', '·', "details"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'r', '·', "ribbon"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'C', '·', "color"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'X', '·', "xaxis"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'Y', '·', "yaxis"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'L', '·', "layers"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'G', '·', "grid"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'E', '·', "edges"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'U', '·', "guides"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'O', '·', "overlay"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'N', '·', "notes"            , 'y', '>', "-"                 , 0, 0, 0 },
   /*---(insert menu)--------------------------------------------*/
   { 'i', '·', '·', "insert"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 't', '·', "text"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'i', '·', "image"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'l', '·', "lines"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'e', '·', "ellises"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 't', '·', "tris"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'r', '·', "rects"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'h', '·', "hexagons"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'p', '·', "polygons"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 's', '·', "stars"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'a', '·', "arrows"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'f', '·', "flowchart"        , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'j', '·', "joiners"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'n', '·', "notes"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'm', '·', "misc"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "nurbs"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "tiling"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'b', '·', "beziers"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "3d"               , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "surfaces"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "meshes"           , 'y', '>', "-"                 , 0, 0, 0 },
   /*---(select menu)--------------------------------------------*/
   { 's', '·', '·', "select"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 's', '!', '·', "highlight"        , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', '_', '·', "status"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'c', '·', "clear"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'a', '·', "all"              , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'b', '·', "buffer"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'w', '·', "window"           , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'l', '·', "layer"            , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'g', '·', "geometry"         , 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 't', '·', "type"             , 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 'x', '·', "regex"            , 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 'u', '·', "touching"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 's', '·', "save"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'r', '·', "reselect"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'j', '·', "join"             , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'd', '·', "deselect"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'i', '·', "inverse"          , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'x', '·', "all_on_x"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'y', '·', "all_on_y"         , 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'z', '·', "all_on_z"         , 'y', '·', "-"                 , 0, 0, 0 },
   /*---(format menu)--------------------------------------------*/
   { 'o', '·', '·', "format"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'f', '·', "face"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'p', '·', "point"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 's', '·', "spacing"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'i', '·', "indent"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'c', '·', "color"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'a', '·', "align"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'n', '·', "numbers"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'z', '·', "sfill"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'd', '·', "decimals"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'u', '·', "units"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'l', '·', "lists"            , 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'y', '·', "style"            , 'y', '>', "-"                 , 0, 0, 0 },
   /*---(modify menu)--------------------------------------------*/
   { 'm', '·', '·', "modify"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'è', '·', "resize"           , 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', 'é', '·', "dims"             , 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', 'ê', '·', "extent"           , 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', 'ë', '·', "scale"            , 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', 'ì', '·', "trim"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'í', '·', "join"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'î', '·', "move"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', 'ï', '·', "array"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', 'ð', '·', "rotate"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'ñ', '·', "snap"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', 'ò', '·', "fill"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'ó', '·', "hatching"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'ô', '·', "outline"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', 'õ', '·', "centers"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', 'ö', '·', "ends"             , 'y', '-', "-"                 , 0, 0, 0 },
   /*---(dataset menu)-------------------------------------------*/
   { 'd', '·', '·', "dataset"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', 's', '·', "sort"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', 'f', '·', "filter"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "blur"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "enhance"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "distort"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "noise"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "edges"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "combine"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "light"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "shadow"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "pixelate"         , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "render"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "sharpen"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "smooth"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "stylize"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "liquify"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "oils"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "map"              , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "decor"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "vanish"           , 'y', '-', "-"                 , 0, 0, 0 },
   /*---(tools menu)---------------------------------------------*/
   { 't', '·', '·', "tools"            , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "pen"              , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "callig"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "pencil"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "airbrush"         , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "chalk"            , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "brush"            , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "roller"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "spray"            , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "fill"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "sponge"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "blob"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "blur"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "sharpen"          , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "smudge"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "burn"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "eraser"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "magic_e"          , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "healer"           , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "highlight"        , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "zoom"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "move"             , '-', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "lasso"            , '-', '>', "-"                 , 0, 0, 0 },
   /*---(pallette menu)------------------------------------------*/
   { 'p', '·', '·', "palette"          , '-', '>', "-"                 , 0, 0, 0 },
   /*---(layers menu)--------------------------------------------*/
   { 'l', '·', '·', "layers"           , 'y', '>', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "saved"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "highlight"        , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "status"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "new"              , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "rename"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "copy_all"         , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "copy_with"        , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "join"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "flatten"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "delete"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "type"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "hide"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "mask"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "alignment"        , 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "locking"          , 'y', '-', "-"                 , 0, 0, 0 },
   /*---(buffers menu)-------------------------------------------*/
   { 'b', '·', '·', "buffers"          , 'y', '>', "-"                 , 0, 0, 0 },
   { 'b', '_', '·', "status"           , 'y', '·', ":status buffer¦"   , 0, 0, 0 },
   { 'b', 'x', '·', "maximum"          , 'y', '=', ":bmax·"            , 0, 0, 0 },
   { 'b', 'b', '·', "browse"           , 'y', '·', ":bbrowse·"         , 0, 0, 0 },
   { 'b', 'g', '·', "goto"             , 'y', '>', "-"                 , 0, 0, 0 },
   { 'b', 'g', 'a', "abbrev"           , 'y', '=', ":buf·"             , 0, 0, 0 },
   { 'b', 'g', 'r', "return"           , 'y', '·', ",,"                , 0, 0, 0 },
   { 'b', 'g', 'f', "first"            , 'y', '·', ",["                , 0, 0, 0 },
   { 'b', 'g', 'p', "prev"             , 'y', '·', ",<"                , 0, 0, 0 },
   { 'b', 'g', 'n', "next"             , 'y', '·', ",>"                , 0, 0, 0 },
   { 'b', 'g', 'l', "last"             , 'y', '·', ",]"                , 0, 0, 0 },
   { 'b', 'g', 'u', "summary"          , 'y', '·', ",®"                , 0, 0, 0 },
   { 'b', 'g', 'y', "system"           , 'y', '·', ",¯"                , 0, 0, 0 },
   { 'b', '-', '·', "showall"          , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', 'w', '·', "new"              , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', 'n', '·', "rename"           , 'y', '=', ":btitle·"          , 0, 0, 0 },
   { 'b', 'z', '·', "size"             , 'y', '=', ":bsize·"           , 0, 0, 0 },
   { 'b', 't', '·', "type"             , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', 'f', '·', "defaults"         , 'y', '>', "-"                 , 0, 0, 0 },
   { 'b', 'f', 'w', "col_wide"         , 'y', '=', ":defwide·"         , 0, 0, 0 },
   { 'b', 'f', 't', "row_tall"         , 'y', '=', ":deftall·"         , 0, 0, 0 },
   { 'b', 'f', 'd', "dep_thick"        , 'y', '!', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "scale"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "delete"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "freeze"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "split"            , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "hiding"           , 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "locking"          , 'y', '-', "-"                 , 0, 0, 0 },
   /*---(snippet menu)-------------------------------------------*/
   { 'g', '·', '·', "language"         , '-', '>', "-"                 , 0, 0, 0 },
   /*---(auditor menu)-------------------------------------------*/
   { 'a', '·', '·', "auditor"          , '-', '>', "-"                 , 0, 0, 0 },
   { 'a', 's', '·', "spell"            , '-', '-', "-"                 , 0, 0, 0 },
   { 'a', 'd', '·', "dump"             , '-', '-', "-"                 , 0, 0, 0 },
   /*---(execute menu)-------------------------------------------*/
   { 'x', '·', '·', "execute"          , '-', '>', "-"                 , 0, 0, 0 },
   /*---(config menu)--------------------------------------------*/
   { 'c', '·', '·', "config"           , '-', '>', "-"                 , 0, 0, 0 },
   /*---(macro menu)---------------------------------------------*/
   { 'r', '·', '·', "script"           , '-', '>', "-"                 , 0, 0, 0 },
   { 'r', 'w', '·', "new"              , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 'o', '·', "open"             , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 'm', '·', "import"           , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 's', '·', "save"             , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 'x', '·', "export"           , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 'i', '·', "list"             , '-', '·', "-"                 , 0, 0, 0 },
   { 'r', 'g', '·', "globals"          , '-', '·', "-"                 , 0, 0, 0 },
   /*---(share menu)---------------------------------------------*/
   { 'h', '·', '·', "share"            , '-', '>', "-"                 , 0, 0, 0 },
   /*---(footer)-------------------------------------------------*/
   {  0 ,  0 ,  0 , NULL               ,  0,   0, NULL                 , 0, 0, 0 },
   /*---(done)---------------------------------------------------*/
};




/*====================------------------------------------====================*/
/*===----                         menu display                         ----===*/
/*====================------------------------------------====================*/
static void  o___MENUS___________o () { return; }

static int  *s_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_?!#,.èéêëìíîïðñòóôõö÷øùúûüýþÿ";

char
yvikeys__menu_count     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   s_nmenu = 0;
   for (i = 0; i < MAX_MENU; ++i) {
      if (s_menus [i].name [0] == NULL)         break;
      if (strcmp (s_menus [i].name, "-") == 0)  break;
      ++s_nmenu;
   }
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
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
   for (i = 0; i < s_nmenu; ++i) {
      if (s_menus [i].name [0] == NULL)         break;
      if (strcmp (s_menus [i].name, "-") == 0)  break;
      s_menus [i].next    = -1;
      s_menus [i].start   = -1;
      s_menus [i].count   =  0;
   }
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__menu_wipe      (int i)
{
   s_menus [i].top     = '·';
   s_menus [i].mid     = '·';
   s_menus [i].bot     = '·';
   strlcpy (s_menus [i].name, "-", LEN_LABEL);
   s_menus [i].active  = '-';
   s_menus [i].type    = '·';
   strlcpy (s_menus [i].keys, "-", LEN_LABEL);
   s_menus [i].next    = -1;
   s_menus [i].start   = -1;
   s_menus [i].count   =  0;
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
   s_nmenu = 0;
   for (i = 0; i < MAX_MENU; ++i) {
      yvikeys__menu_wipe (i);
   }
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
   for (i = 0; i < s_nmenu; ++i) {
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
      for (i = 0; i < s_nmenu; ++i) {
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
         for (i = 0; i < s_nmenu; ++i) {
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
   for (i = 0; i < s_nmenu; ++i) {
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
   for (i = 0; i < s_nmenu; ++i) {
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
   yvikeys__menu_rptg ();
   return 0;
}


char
yvikeys_menu_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
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
   a = s_nmenu;
   for (i = 0; i < s_nmenu; ++i) {
      DEBUG_CMDS   yLOG_complex ("check"     , "%3d %c %c %c %-10.10s", i, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].name);
      if (s_menus [i].top != '·')  continue;
      for (j = i + 1; j < s_nmenu; ++j) {
         DEBUG_CMDS   yLOG_complex ("move up"   , "%3d %c %c %c %-10.10s", j, s_menus [j].top, s_menus [j].mid, s_menus [j].bot, s_menus [j].name);
         s_menus [j - 1].top     = s_menus [j].top;
         s_menus [j - 1].mid     = s_menus [j].mid;
         s_menus [j - 1].bot     = s_menus [j].bot;
         strlcpy (s_menus [j - 1].name, s_menus [j].name, LEN_LABEL);
         s_menus [j - 1].active  = s_menus [j].active;
         s_menus [j - 1].type    = s_menus [j].type;
         strlcpy (s_menus [j - 1].keys, s_menus [j].keys, LEN_LABEL);
         ++c;
      }
      --s_nmenu;
      yvikeys__menu_wipe (s_nmenu);
      --i;
   }
   DEBUG_CMDS   yLOG_value   ("c"         , c);
   DEBUG_CMDS   yLOG_value   ("s_nmenu"   , s_nmenu);
   /*---(relink)-------------------------*/
   yvikeys_menu_init ();
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
      if (s_menus [i].top == x_ch)   break;
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


static int     s_xpos   [LEN_LABEL] = {   0,  15,  40,  50,  55,  55,  50,  40,  15,   0, -15, -40, -50, -55, -55, -50, -40, -15 };
static int     s_ypos   [LEN_LABEL] = { -15, -15, -30, -45, -60, -75, -90,-105,-120,-120,-120,-105, -90, -75, -60, -45, -30, -15 };
static int     s_align  [LEN_LABEL] = {   2,   1,   1,   1,   1,   1,   1,   1,   1,   2,   3,   3,   3,   3,   3,   3,   3,   3 };
static int     s_abbr   [LEN_LABEL] = {  15,   0,   0,   0,   0,   0,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0 };

char
yvikeys__menu_back      (int a_len, int a_level, int a_last)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_on        =  '-';
   int         x_left, x_wide, x_bott, x_tall, x_top, x_mid;
   int         x_xmin, x_xmax, x_ymin, x_ymax;
   int         x, y;
   int         i           =    0;
   int         t           [LEN_RECD];
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_value   ("a_len"     , a_len);
   DEBUG_CMDS   yLOG_value   ("a_level"   , a_level);
   DEBUG_CMDS   yLOG_value   ("a_last"    , a_last);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   x_on = yVIKEYS_view_bounds (YVIKEYS_MENUS, &x_xmin, &x_xmax, &x_ymin, &x_ymax);
   DEBUG_CMDS   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy", x_xmin, x_xmax, x_ymin, x_ymax);
   /*---(opengl)-------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      /*---(black background)---------------*/
      glColor4f (0.0f, 0.0f, 0.0f, 1.0f);
      glBegin(GL_POLYGON); {
         glVertex3f (  10.0,   -70.0,    90.0);
         glVertex3f ( 290.0,   -70.0,    90.0);
         glVertex3f ( 290.0,  -270.0,    90.0);
         glVertex3f (  10.0,  -270.0,    90.0);
      } glEnd();
      /*---(color fill)---------------------*/
      if (a_len != a_level) {
         glColor4f (0.8f, 0.0f, 0.0f, 1.0f);
      } else {
         switch (a_level) {
         case  0 :  glColor4f (0.0f, 0.8f, 0.8f, 1.0f);  break;
         case  1 :  glColor4f (0.7f, 0.5f, 0.3f, 1.0f);  break;
         case  2 :  glColor4f (0.3f, 0.8f, 0.3f, 1.0f);  break;
         }
      }
      glBegin(GL_POLYGON); {
         glVertex3f (  12.0,   -72.0,   100.0);
         glVertex3f ( 288.0,   -72.0,   100.0);
         glVertex3f ( 288.0,  -268.0,   100.0);
         glVertex3f (  12.0,  -268.0,   100.0);
      } glEnd();
      /*---(labels)-------------------------*/
      glPushMatrix(); {
         glColor4f (0.0, 0.0, 0.0, 1.0);
         glTranslatef( 150.0,  -85.0, 120.0);
         switch (a_level) {
         case  0 : strlcpy (t, "main menu", LEN_LABEL); break;
         case  1 : sprintf (t, "\\%c (%s) sub-menu", s_menus [a_last].top, s_menus [a_last].name); break;
         case  2 : sprintf (t, "\\%c%c (%s) options", s_menus [a_last].top, s_menus [a_last].mid, s_menus [a_last].name); break;
         }
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BASCEN, t);
         glTranslatef(-130.0,    0.0,   0.0);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BASLEF, myVIKEYS.s_prog);
         glTranslatef( 260.0,    0.0,   0.0);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BASRIG, myVIKEYS.s_vernum);
         glTranslatef(-130.0, -180.0,   0.0);
         yFONT_print  (myVIKEYS.font, myVIKEYS.point, YF_BASCEN, "much wider, flatter, and universal");
         if (a_len != a_level) {
            glColor4f    (1.0f, 1.0f, 1.0f, 1.0f);
            glTranslatef (   0.0,   50.0,  50.0);
            yFONT_print  (myVIKEYS.font, 16, YF_BASCEN, "ERROR KEYS");
            glTranslatef (   0.0,  -15.0,   0.0);
            yFONT_print  (myVIKEYS.font, 10, YF_BASCEN, "menu locked, <esc> to exit");
         }
      } glPopMatrix();
   }
   if (myVIKEYS.env == YVIKEYS_CURSES) {
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
yvikeys__menu_entry_gl  (int i, int a_top, int a_mid, int a_level, int a_group, int a_entry, int a_offx, int a_offy)
{
   /*---(locals)-----------+-----+-----+-*/
   int         t           [LEN_LABEL];
   int         x_align     =    0;
   glPushMatrix(); {
      /*---(recenter)--------------------*/
      glTranslatef( 150.0, -105.0, 120.0);
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
         case 0 : glTranslatef (-100.0, -15.0 * a_entry,   0.0);  break;
         case 1 : glTranslatef (  20.0, -15.0 * a_entry,   0.0);  break;
         case 2 : glTranslatef (-130.0, -15.0 * a_entry,   0.0);  break;
         case 3 : glTranslatef ( -35.0, -15.0 * a_entry,   0.0);  break;
         case 4 : glTranslatef (  60.0, -15.0 * a_entry,   0.0);  break;
         }
      } else {
         glTranslatef (a_group, a_entry,   0.0);
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
      yFONT_print (myVIKEYS.font, myVIKEYS.point, x_align, t);
      glTranslatef (a_offx, a_offy, 0.0);
      sprintf (t, "%.9s", s_menus [i].name);
      if (a_level > 0 && s_menus [i].type == '>')  strlcat (t, "+", LEN_LABEL);
      if (a_level > 0 && s_menus [i].type == '=')  strlcat (t, "=", LEN_LABEL);
      yFONT_print (myVIKEYS.font, myVIKEYS.point, x_align, t);
      glTranslatef (-a_offx, -a_offy, 0.0);
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
   int         x_xmin, x_xmax, x_ymin, x_ymax;
   int         x_top, x_mid;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   yVIKEYS_view_bounds (YVIKEYS_MENUS, &x_xmin, &x_xmax, &x_ymin, &x_ymax);
   DEBUG_CMDS   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy", x_xmin, x_xmax, x_ymin, x_ymax);
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
      x_top = x_bott - x_tall + 2;
      x_mid = x_left + (x_wide / 2);
      /*---(display)---------------------*/
      switch (myVIKEYS.env) {
      case YVIKEYS_OPENGL :  yvikeys__menu_entry_gl (i, 150  , -105  , a_level, x_group, x_entry, 15.0, 0.0); break;
      case YVIKEYS_CURSES :  yvikeys__menu_entry_nc (i, x_mid, x_top , a_level, x_group, x_entry,  2.0, 0.0); break;
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
   int         x_xmin, x_xmax, x_ymin, x_ymax;
   int         x, y;
   int         x_len       =    0;
   int         x_offx      =    0;
   int         x_offy      =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_char    ("env"       , myVIKEYS.env);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size   (YVIKEYS_MENUS, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_CMDS   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   x_on = yVIKEYS_view_bounds (YVIKEYS_MENUS, &x_xmin, &x_xmax, &x_ymin, &x_ymax);
   DEBUG_CMDS   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy", x_xmin, x_xmax, x_ymin, x_ymax);
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
               case 1 :  x_offx =  15;  x_offy =  0;          break;
               case 2 :  x_offx =   0;  x_offy = s_abbr [c];  break;
               case 3 :  x_offx = -15;  x_offy =  0;          break;
               }
               yvikeys__menu_entry_gl (i, 150  , -105  , 0, s_xpos [c], s_ypos [c], x_offx, x_offy);
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
         x = x_mid + s_xpos [c] /  7;   /* 6 is more ellipse/rounded */
         y = x_top - s_ypos [c] / 15;
         switch (s_align [c]) {
         case 1 :  x_offx =   2;  x_offy =  0;               break;
         case 2 :  x_offx =   0;  x_offy = s_abbr [c] / 15;  break;
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
   strlcpy (myVIKEYS.m_keys, ""  , LEN_LABEL);
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
   /*---(check exec)---------------------*/
   if (n >= 0 && x_len - 1 == x_level) {
      DEBUG_USER   yLOG_char    ("type"      , s_menus [n].type);
      if (n >= 0 && strchr ("·=", s_menus [n].type) != NULL) {
         MODE_exit ();
         DEBUG_USER   yLOG_info    ("keys"      , s_menus [n].keys);
         if (strlen (s_menus [n].keys) == 1 && s_menus [n].keys [0] == '-') {
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, -1);
            return -1;
         }
         SOURCE_menu_prep ();
         strlcpy (myVIKEYS.m_keys, s_menus [n].keys, LEN_LABEL);
         yVIKEYS_main_string (myVIKEYS.m_keys);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
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
      strlcpy (s_menus [n].keys, "-", LEN_LABEL);
      s_menus [n].active = 'y';
   }
   /*---(if making a executable)---------*/
   else {
      s_menus [n].type   = '·';
      strlcpy (s_menus [n].keys, a_keys, LEN_LABEL);
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
   s_menus [s_nmenu].top    = a_path [1];
   s_menus [s_nmenu].mid    = a_path [2];
   s_menus [s_nmenu].bot    = a_path [3];
   /*---(name and keys)------------------*/
   strlcpy (s_menus [s_nmenu].name, a_name, LEN_LABEL);
   strlcpy (s_menus [s_nmenu].keys, a_keys, LEN_LABEL);
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
   strlcpy (s_menus [s_nmenu].keys, "-"   , LEN_LABEL);
   yvikeys_menu_init ();
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
   char        t           [LEN_LABEL];
   char        s           [LEN_LABEL] = "entry";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MENU unit        : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "count"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU count       : %d", s_nmenu);
   }
   else if (strcmp (a_question, "index"          )   == 0) {
      i = a_path;
      strlcpy    (t, s_menus [i].keys, LEN_LABEL);
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
         strlcpy    (t, s_menus [i].keys, LEN_LABEL);
         strlencode (t, ySTR_MAX, LEN_LABEL);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU %-5.5s       : %1d %3d %-10.10s  %c  %c  %s", s, x_level, i     , s_menus [i].name, s_menus [i].active, s_menus [i].type, t);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

