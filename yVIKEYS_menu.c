/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



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
   char        mgmt;                        /* changes allowed (y/n)          */
   char        def;                         /* default active flag            */
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
   { 'f', '·', '·', "files"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'W', '·', "workspc"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'w', '·', "new"              , '´', 'y', 'y', '·', ":new¦"             , 0, 0, 0 },
   { 'f', 'o', '·', "open"             , '´', 'y', 'y', '·', ":edit·"            , 0, 0, 0 },
   { 'f', 'c', '·', "chdir"            , '´', 'y', 'y', '·', ":cd·"              , 0, 0, 0 },
   { 'f', 'm', '·', "import"           , '·', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'n', '·', "rename"           , '´', 'y', 'y', '·', ":file·"            , 0, 0, 0 },
   { 'f', 'f', '·', "refresh"          , '·', 'y', 'y', '·', ":refresh¦"         , 0, 0, 0 },
   { 'f', 'r', '·', "restore"          , '·', '·', '·', '!', "-"                 , 0, 0, 0 },
   { 'f', 'v', '·', "version"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'p', '·', "protect"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 's', '·', "save"             , '´', 'y', 'y', '!', ":w¦"               , 0, 0, 0 },
   { 'f', 'a', '·', "saveas"           , '´', 'y', 'y', '!', "-"                 , 0, 0, 0 },
   { 'f', 'S', '·', "saveall"          , '´', 'y', 'y', '!', ":wa¦"              , 0, 0, 0 },
   { 'f', 'x', '·', "export"           , '·', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'f', 'q', '·', "quit"             , '´', 'y', 'y', '·', ":q¦"               , 0, 0, 0 },
   /*---(edit menu)----------------------------------------------*/
   { 'e', '·', '·', "edit"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'u', '·', "undo"             , '´', 'y', 'y', '·', "u"                 , 0, 0, 0 },
   { 'e', 'U', '·', "redo"             , '´', 'y', 'y', '·', "U"                 , 0, 0, 0 },
   { 'e', '_', '·', "status"           , '´', 'y', 'y', '·', ":status mundo¦"    , 0, 0, 0 },
   { 'e', '?', '·', "history"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', '?', 't', "track"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'u', "untrack"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'c', "clear"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', '?', 'r', "review"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'y', '·', "copy"             , '´', 'y', 'y', '·', "y"                 , 0, 0, 0 },
   { 'e', 'Y', '·', "cut"              , '´', 'y', 'y', '·', "Y"                 , 0, 0, 0 },
   { 'e', 'p', '·', "paste"            , '´', 'y', 'y', '·', "p"                 , 0, 0, 0 },
   { 'e', 'P', '·', "special"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'x', '·', "clear"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'x', '.', "inplace"          , '·', 'y', 'y', '·', "x."                , 0, 0, 0 },
   { 'e', 'x', 'l', "left"             , '·', 'y', 'y', '·', "xl"                , 0, 0, 0 },
   { 'e', 'x', 'h', "right"            , '·', 'y', 'y', '·', "xh"                , 0, 0, 0 },
   { 'e', 'x', 'k', "up"               , '·', 'y', 'y', '·', "xk"                , 0, 0, 0 },
   { 'e', 'x', 'j', "down"             , '·', 'y', 'y', '·', "xj"                , 0, 0, 0 },
   { 'e', 'x', 'i', "in"               , '·', 'y', 'y', '·', "xi"                , 0, 0, 0 },
   { 'e', 'x', 'o', "out"              , '·', 'y', 'y', '·', "xo"                , 0, 0, 0 },
   { 'e', 'x', 'x', "col_lef"          , '·', 'y', 'y', '·', "xx"                , 0, 0, 0 },
   { 'e', 'x', 'X', "col_rig"          , '·', 'y', 'y', '·', "xX"                , 0, 0, 0 },
   { 'e', 'x', 'y', "row_abo"          , '·', 'y', 'y', '·', "xy"                , 0, 0, 0 },
   { 'e', 'x', 'Y', "row_bel"          , '·', 'y', 'y', '·', "xY"                , 0, 0, 0 },
   { 'e', 'x', 'z', "lvl_in"           , '·', 'y', 'y', '·', "xz"                , 0, 0, 0 },
   { 'e', 'x', 'Z', "lvl_out"          , '·', 'y', 'y', '·', "xZ"                , 0, 0, 0 },
   { 'e', 'd', '·', "delete"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'd', 'l', "left"             , '·', 'y', 'y', '·', "dl"                , 0, 0, 0 },
   { 'e', 'd', 'h', "right"            , '·', 'y', 'y', '·', "dh"                , 0, 0, 0 },
   { 'e', 'd', 'k', "up"               , '·', 'y', 'y', '·', "dk"                , 0, 0, 0 },
   { 'e', 'd', 'j', "down"             , '·', 'y', 'y', '·', "dj"                , 0, 0, 0 },
   { 'e', 'd', 'i', "in"               , '·', 'y', 'y', '·', "di"                , 0, 0, 0 },
   { 'e', 'd', 'o', "out"              , '·', 'y', 'y', '·', "do"                , 0, 0, 0 },
   { 'e', 'd', 'x', "col_left"         , '·', 'y', 'y', '·', "dx"                , 0, 0, 0 },
   { 'e', 'd', 'X', "col_right"        , '·', 'y', 'y', '·', "dX"                , 0, 0, 0 },
   { 'e', 'd', 'y', "row_above"        , '·', 'y', 'y', '·', "dy"                , 0, 0, 0 },
   { 'e', 'd', 'Y', "row_below"        , '·', 'y', 'y', '·', "dY"                , 0, 0, 0 },
   { 'e', 'd', 'z', "lvl_in"           , '·', 'y', 'y', '·', "dz"                , 0, 0, 0 },
   { 'e', 'd', 'Z', "lvl_out"          , '·', 'y', 'y', '·', "dZ"                , 0, 0, 0 },
   { 'e', 'i', '·', "insert"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'i', 'l', "left"             , '·', 'y', 'y', '·', "al"                , 0, 0, 0 },
   { 'e', 'i', 'h', "right"            , '·', 'y', 'y', '·', "ah"                , 0, 0, 0 },
   { 'e', 'i', 'k', "up"               , '·', 'y', 'y', '·', "ak"                , 0, 0, 0 },
   { 'e', 'i', 'j', "down"             , '·', 'y', 'y', '·', "aj"                , 0, 0, 0 },
   { 'e', 'i', 'i', "in"               , '·', 'y', 'y', '·', "ai"                , 0, 0, 0 },
   { 'e', 'i', 'o', "out"              , '·', 'y', 'y', '·', "ao"                , 0, 0, 0 },
   { 'e', 'i', 'x', "col_lef"          , '·', 'y', 'y', '·', "ax"                , 0, 0, 0 },
   { 'e', 'i', 'X', "col_rig"          , '·', 'y', 'y', '·', "aX"                , 0, 0, 0 },
   { 'e', 'i', 'y', "row_abo"          , '·', 'y', 'y', '·', "ay"                , 0, 0, 0 },
   { 'e', 'i', 'Y', "row_bel"          , '·', 'y', 'y', '·', "aY"                , 0, 0, 0 },
   { 'e', 'i', 'z', "lvl_in"           , '·', 'y', 'y', '·', "az"                , 0, 0, 0 },
   { 'e', 'i', 'Z', "lvl_out"          , '·', 'y', 'y', '·', "aZ"                , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 'e', 'g', '·', "group"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'g', "group"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'u', "ungroup"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'r', "regroup"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'm', "merge"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'f', "free"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'g', 'b', "boundary"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', '·', "align"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'k', "top"              , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'j', "bottom"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'h', "left"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'l', "right"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'o', "front"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'i', "back"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'x', "dist-x"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'y', "dist-y"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'a', 'z', "dist-z"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', '·', "order"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'n', "front"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'i', "forward"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'o', "backward"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'f', "back"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'o', 'r', "reverse"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'e', 'l', '·', "layer"            , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   /*---(view menu)----------------------------------------------*/
   { 'v', '·', '·', "view"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'w', '·', "window"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'l', '·', "layout"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 't', '·', "title"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'v', '·', "version"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'b', '·', "buffers"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'f', '·', "formula"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'n', '·', "nav"              , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'p', '·', "progress"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 's', '·', "status"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'k', '·', "keys"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'c', '·', "command"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'a', '·', "alt"              , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'd', '·', "details"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'r', '·', "ribbon"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'C', '·', "color"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'X', '·', "xaxis"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'Y', '·', "yaxis"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'L', '·', "layers"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'G', '·', "grid"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'E', '·', "edges"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'U', '·', "guides"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'O', '·', "overlay"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'v', 'N', '·', "notes"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(insert menu)--------------------------------------------*/
   { 'i', '·', '·', "insert"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 't', '·', "text"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'l', '·', "lines"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'e', '·', "ellises"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 't', '·', "tris"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'r', '·', "rects"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 'i', 'h', '·', "hexagons"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'p', '·', "polygons"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 's', '·', "stars"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'a', '·', "arrows"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'f', '·', "flowchart"        , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 'i', 'j', '·', "joiners"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'n', '·', "notes"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', 'm', '·', "misc"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "nurbs"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "tiling"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 'i', 'b', '·', "beziers"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "3d"               , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "surfaces"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'i', '-', '·', "meshes"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 'i', 'i', '·', "image"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(select menu)--------------------------------------------*/
   { 's', '·', '·', "select"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 's', '!', '·', "highlight"        , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', '_', '·', "status"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'c', '·', "clear"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'a', '·', "all"              , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'b', '·', "buffer"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'w', '·', "window"           , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 's', 'l', '·', "layer"            , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'g', '·', "geometry"         , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 't', '·', "type"             , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 'x', '·', "regex"            , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 's', 'u', '·', "touching"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 's', 's', '·', "save"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'r', '·', "reselect"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'j', '·', "join"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'd', '·', "deselect"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { '·', '·', '·', "---------"        , '-', '-', '-', '-', "-------------"     , 0, 0, 0 },
   { 's', 'i', '·', "inverse"          , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'x', '·', "all_on_x"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'y', '·', "all_on_y"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 's', 'z', '·', "all_on_z"         , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   /*---(format menu)--------------------------------------------*/
   { 'o', '·', '·', "format"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'f', '·', "face"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'p', '·', "point"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 's', '·', "spacing"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'i', '·', "indent"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'c', '·', "color"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'a', '·', "align"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'n', '·', "numbers"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'z', '·', "sfill"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'd', '·', "decimals"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'u', '·', "units"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'l', '·', "lists"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'o', 'y', '·', "style"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(modify menu)--------------------------------------------*/
   { 'm', '·', '·', "modify"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "resize"           , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "dims"             , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "extent"           , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "scale"            , '´', 'y', 'y', '=', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "trim"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "join"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "move"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "array"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "rotate"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "snap"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "fill"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "hatching"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "outline"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "centers"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'm', '-', '·', "ends"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(dataset menu)-------------------------------------------*/
   { 'd', '·', '·', "dataset"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', 's', '·', "sort"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', 'f', '·', "filter"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "blur"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "enhance"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "distort"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "noise"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "edges"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "combine"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "light"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "shadow"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "pixelate"         , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "render"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "sharpen"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "smooth"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "stylize"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "liquify"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "oils"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "map"              , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "decor"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'd', '-', '·', "vanish"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(tools menu)---------------------------------------------*/
   { 't', '·', '·', "tools"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "pen"              , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "callig"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "pencil"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "airbrush"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "chalk"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "brush"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "roller"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "spray"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "fill"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "sponge"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "blob"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "blur"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "sharpen"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "smudge"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "burn"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "eraser"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "magic_e"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "healer"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "highlight"        , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "zoom"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "move"             , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 't', '-', '·', "lasso"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(pallette menu)------------------------------------------*/
   { 'p', '·', '·', "palette"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(layers menu)--------------------------------------------*/
   { 'l', '·', '·', "layers"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "saved"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "highlight"        , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "status"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "new"              , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "rename"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "copy_all"         , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "copy_with"        , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "join"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "flatten"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "delete"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "type"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "hide"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "mask"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "alignment"        , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'l', '-', '·', "locking"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(buffers menu)-------------------------------------------*/
   { 'b', '·', '·', "buffers"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "showall"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "status"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "new"              , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "rename"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "type"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "size"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "scale"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "delete"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "freeze"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "split"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "hiding"           , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'b', '-', '·', "locking"          , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(snippet menu)-------------------------------------------*/
   { 'g', '·', '·', "language"         , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(auditor menu)-------------------------------------------*/
   { 'a', '·', '·', "auditor"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'a', 's', '·', "spell"            , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   { 'a', 'd', '·', "dump"             , '´', 'y', 'y', '-', "-"                 , 0, 0, 0 },
   /*---(execute menu)-------------------------------------------*/
   { 'x', '·', '·', "execute"          , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(config menu)--------------------------------------------*/
   { 'c', '·', '·', "config"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(macro menu)---------------------------------------------*/
   { 'r', '·', '·', "script"           , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   { 'r', '-', '·', "new"              , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'r', '-', '·', "open"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'r', '-', '·', "save"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   { 'r', '-', '·', "list"             , '´', 'y', 'y', '·', "-"                 , 0, 0, 0 },
   /*---(share menu)---------------------------------------------*/
   { 'h', '·', '·', "share"            , '´', 'y', 'y', '>', "-"                 , 0, 0, 0 },
   /*---(footer)-------------------------------------------------*/
   {  0 ,  0 ,  0 , NULL               ,  0 ,  0 ,  0,   0, NULL                 , 0, 0, 0 },
   /*---(done)---------------------------------------------------*/
};




/*====================------------------------------------====================*/
/*===----                         menu display                         ----===*/
/*====================------------------------------------====================*/
static void  o___MENUS___________o () { return; }


static int  *s_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!-_.";

char
yvikeys__menu_wipe      (void)
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
   yvikeys__menu_wipe ();
   yvikeys__menu_tops ();
   yvikeys__menu_mids ();
   yvikeys__menu_bots ();
   yvikeys__menu_rptg ();
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
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(handle total)-------------------*/
   for (i = 0; i < s_nmenu; ++i) {
      s_menus [i].next    = -1;
      s_menus [i].start   = -1;
      s_menus [i].count   =  0;
      if (s_menus [i].top != '·')  continue;
      for (j = i + 1; j < s_nmenu; ++j) {
         s_menus [j - 1].top     = s_menus [j].top;
         s_menus [j - 1].mid     = s_menus [j].mid;
         s_menus [j - 1].bot     = s_menus [j].bot;
         strlcpy (s_menus [j - 1].name, s_menus [j].name, LEN_LABEL);
         s_menus [j - 1].mgmt    = s_menus [j].mgmt;
         s_menus [j - 1].def     = s_menus [j].def;
         s_menus [j - 1].active  = s_menus [j].active;
         s_menus [j - 1].type    = s_menus [j].type;
         strlcpy (s_menus [j - 1].keys, s_menus [j].keys, LEN_LABEL);
      }
   }
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

char yvikeys__menu_exec      (char *a_keys) { return yVIKEYS_main_string (a_keys); }


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
      case '·' :
         if (s_menus [i].active != 'y') glColor4f (0.4, 0.4, 0.4, 1.0);
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
      sprintf (t, "%s", s_menus [i].name);
      if (a_level > 0 && s_menus [i].type == '>')  strlcat (t, "+", LEN_LABEL);
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
   case '·' :
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
      case 3 : x = a_mid -  5; y = a_top + a_entry;    break;
      case 4 : x = a_mid +  9; y = a_top + a_entry;    break;
      }
   } else {
      x = a_group;
      y = a_entry;
   }
   DEBUG_CMDS   yLOG_complex ("position"  , "%3dg, %3de, %3dm, %3dt, %3dx, %3dy", a_group, a_entry, a_mid, a_top, x, y);
   /*---(prepare)------------------------*/
   mvprintw   (y, x, "%s", t);
   /*---(prepare)------------------------*/
   sprintf (t, "%s", s_menus [i].name);
   if (a_level > 0 && s_menus [i].type == '>')  strlcat (t, "+", LEN_LABEL);
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
   --rce;  if (x_len != 3) {
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
   if (strcmp (a_keys, ">") == 0) {
      s_menus [n].type   = '>';
      strlcpy (s_menus [n].keys, "-", LEN_LABEL);
      s_menus [n].active = 'y';
   }
   /*---(if making a executable)---------*/
   else {
      s_menus [n].type   = '·';
      strlcpy (s_menus [n].keys, a_keys, LEN_LABEL);
      s_menus [n].active = 'y';
      i = s_menus [n].start;
      while (i >= 0) {
         s_menus [i].top    = '·';
         s_menus [i].mid    = '·';
         s_menus [i].bot    = '·';
         i = s_menus [i].next;
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
   /*---(fill in)------------------------*/
   s_menus [s_nmenu].mgmt   = '·';
   s_menus [s_nmenu].def    = 'y';
   s_menus [s_nmenu].active = 'y';
   s_menus [s_nmenu].type   = '·';
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



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

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
      s_menus [i].top     = '·';
      s_menus [i].mid     = '·';
      s_menus [i].bot     = '·';
      strlcpy (s_menus [i].name, "-", LEN_LABEL);
      s_menus [i].mgmt    = '·';
      s_menus [i].def     = '-';
      s_menus [i].active  = s_menus [i].def;
      s_menus [i].type    = '·';
      strlcpy (s_menus [i].keys, "-", LEN_LABEL);
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
yvikeys__menu_addgroup  (char a_top, char a_mid, char *a_name)
{
   s_menus [s_nmenu].top    = a_top;
   s_menus [s_nmenu].mid    = a_mid;
   strlcpy (s_menus [s_nmenu].name, a_name, LEN_LABEL);
   s_menus [s_nmenu].type   = '>';
   s_menus [s_nmenu].def    = 'y';
   s_menus [s_nmenu].active = s_menus [s_nmenu].def;
   strlcpy (s_menus [s_nmenu].keys, "-"   , LEN_LABEL);
   yvikeys_menu_init ();
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__menu_unit      (char *a_question, char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        x_level     =    0;
   int         x_last      =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MENU unit        : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "count"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU count       : %d", s_nmenu);
   }
   else if (strcmp (a_question, "index"          )   == 0) {
      i = a_path;
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU index       : %-2d %c %c %c %-10.10s %c %c %-10.10s %3d %3d %3d", i, s_menus [i].top, s_menus [i].mid, s_menus [i].bot, s_menus [i].name, s_menus [i].active, s_menus [i].type, s_menus [i].keys, s_menus [i].next, s_menus [i].start, s_menus [i].count);
   }
   else if (strcmp (a_question, "entry"          )   == 0) {
      i = yvikeys__menu_find (a_path, &x_level, &x_last);
      if (i < 0 && x_last < 0) {
         strlcpy  (yVIKEYS__unit_answer, "MENU unit        : menu item not found", LEN_FULL);
      }
      else if (i >= 0)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU entry       : %1d %3d %-10.10s  %c  %c  %s", x_level, i     , s_menus [i].name, s_menus [i].active, s_menus [i].type, s_menus [i].keys);
      else              snprintf (yVIKEYS__unit_answer, LEN_FULL, "MENU last        : %1d %3d %-10.10s  %c  %c  %s", x_level, x_last, s_menus [x_last].name, s_menus [x_last].active, s_menus [x_last].type, s_menus [x_last].keys);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

