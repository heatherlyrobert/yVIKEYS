/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*============================---- METIS TASKS ---============================*/
/*
 *
 * metis  wn1··  sort cells found into a understandable, responsible order 
 *
 *
 */

/*  menu structure
 *     -- chose wider (unweildy), not deeper (spooky)
 *     -- three layer ending in automatic exec, hitting enter, or text entry
 *     -- standard on first two layers
 *     -- layer one is locked, keys standard
 *     -- layer two 90% standard, can be added to, keys sometimes changed
 *     -- layer three is semi-standard and last available layer
 *     -- any layer can be searched and selected using /<text>¦
 *     -- multi-select is done through multiple iterations
 *
 *     \fs            --> saves the current file under the existing name
 *     \fa<name>¦     --> saves current file under a temporary name
 *     \fr<name>¦     --> changes the name of the current file
 *
 */
/*
 *  ppt   file, edit, view, insert, format, tools, slideshow, window, help, acrobat
 *        (home, insert, design, transitions, animations, slideshow, review, view, help)
 *
 *  xls   file, edit, view, insert, format, tools, data, window, help
 *        (home, insert, layout, formulas, data, review, view, help)
 *
 *  word  file, edit, view, insert, format, tools, table, reference, mailings, window, help
 *        (home, insert, layout, reference, mailings, review, view, help)
 *  
 *  123   worksheet, range, copy, move, file, print, graph, data, quit
 *
 *  gimp  file, edit, select, view, image, layer, colors, tools, filters, python-fu, windows, help
 *
 *  photoshop  file, edit, image, layer, select, filter, analysis, 3d, view, window, help
 *  
 *  numbers    file, edit, insert, table, organize, format, arrange, view, share, window, help
 *
 *  illustrator file, edit, object, type, select, effect, view, window, help
 *
 *  autocad   file edit, view, insert, format, tools, draw, dimension, modify, parametric, window, help
 *            (home, insert, annotate, parametric, view, manage, output, addins, ...)
 *
 *  access   file, edit, view, insert, format, records, tools, windows, help
 *
 *  pimavera  file, edit, view, project, enterprise, tools, admin, help
 *
 *  gnumeric    file, edit, view, insert, format, tools, statiscs, data, help
 *
 *  oocalc    file, edit, view, insert, format, tools, data, window
 *  oowriter  file, edit, view, insert, format, table, tools, window
 *  ooimpress file, edit, view, insert, format, tools, slideshow, window
 *  oodraw    file, edit, view, insert, format, tools, modify, window
 *
 *  mysql   file, edit, view, query, database, server, tools, scripting, help
 *
 *  visual studio   file, edit, view, project, build, debug, data, format, tools, window, help
 *
 */



/*> a  ,x··0··i   { '¥··2li',¥2li'¥··2li',¥2li'¥····2li',¥··lvwhd··32|P··20|i "¥··37|BEa"¥··wi, '¥2li',¥··2li'¥2li',¥··wi"¥··68|BEa"¥··wi, "¥··$a"¥··60a ¥··125|Da},¥··0j··,y   <*/
/*> a  ,x··0··i   { '¥··2li',¥2li'¥··2li',¥2li'¥····2li',¥··lvwhd··32|P··20|i "¥··37|BEa"¥··wi, '¥2li',¥··,y   */
/*> a  ,x··0··i   { '¥··2li',¥2li'¥··2li',¥2li'¥····2li',¥··lvwhd··,y   */



/*===[[ TERMS ]]==============================================================*/
#define    MAX_TERMS       20
typedef    struct   cTERMS  tTERMS;
struct cTERMS {
   char        name        [LEN_LABEL];
   char        disp        [LEN_DESC ];
};
static const tTERMS  s_terms [] = {
   { ""      , "void"                       },
   { "v"     , "void"                       },
   { "c"     , "char"                       },
   { "cc"    , "char, char"                 },
   { "i"     , "int"                        },
   { "is"    , "int, char*"                 },
   { "s"     , "char*"                      },
   { "ss"    , "char*, char*"               },
   { "si"    , "char*, int"                 },
   { "a"     , "char*"                      },  /* string includes spaces       */
   { "ii"    , "int, int"                   },
   { "iii"   , "int, int, int"              },
   { "sii"   , "char*, int, int"            },
   { "isss"  , "int, char*, char*, char*"   },
   { "Cs"    , "char*, char*"               },  /* command and arg both        */
   { "css"   , "char, char*, char*"         },
   { "cs"    , "char, char*"                },
   { "f"     , "float"                      },
   { "cciiii", "char, char, int, int, int, int"},
   { "iiii"  , "int, int, int, int"         },
   { "-"    , "-"                           },
};
static  int s_nterm  = 0;



#define     CMDS_BASE        'b'  /* fully standard in base           */
#define     CMDS_CUST        'c'  /* fully custom, not in base        */

#define     CMDS_FIND        'f'  /* find command in list             */
#define     CMDS_EXEC        'x'  /* execute command                  */

static const uchar  *s_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_@";

/*===[[ COMMANDS ]]===========================================================*/

#define  MAX_CMDS      1000
typedef  struct cCMDS   tCMDS;
struct  cCMDS {
   uchar       base;
   uchar       menu;                        /* category                       */
   uchar       name        [LEN_LABEL];     /* full command name              */
   uchar       abbr        [LEN_TERSE];     /* abbreviation of name           */
   union {
      char        (*v    ) (void);
      char        (*c    ) (char);
      char        (*cc   ) (char , char );
      char        (*i    ) (int);
      char        (*ii   ) (int, int);
      char        (*iii  ) (int, int, int);
      char        (*isss ) (int, char*, char*, char*);
      char        (*is   ) (int  , char*);
      char        (*s    ) (char*);
      char        (*ss   ) (char*, char*);
      char        (*si   ) (char*, int  );
      char        (*sii  ) (char*, int, int);
      char        (*css  ) (char, char*, char*);
      char        (*cs   ) (char, char*);
      char        (*f    ) (float);
      char        (*cciiii) (char, char, int, int, int, int);
      char        (*iiii)  (int, int, int, int);
   } f;
   uchar       terms       [LEN_TERSE];     /* type of terms/args             */
   uchar       desc        [LEN_DESC];      /* descriptive label              */
};
static const tCMDS  s_base      [] = {
   { 'b', 'f', "new"             , ""    , yvikeys_file_new          , ""     , "purge all contents and set-up a new, blank file"             },
   { 'b', 'f', "quit"            , "q"   , yvikeys_cmds__quit        , ""     , "quit current file (if no changes), exit if the only file"    },
   { 'b', 'f', "quitall"         , "qa"  , yvikeys_cmds__quit        , ""     , "quit all files (if no changes), and exit"                    },
   { 'b', 'f', "writequit"       , "wq"  , yvikeys_cmds__writequit   , ""     , ""                                                            },
   { 'b', 'f', "writequitall"    , "wqa" , yvikeys_cmds__writequit   , ""     , ""                                                            },
   { 'b', 'e', "dump"            , ""    , yvikeys_dump_exec         , "s"    , "dump a specified data table to the clipboard in flat text"   },
   { 'b', 'c', "menu"            , ""    , yvikeys_menu_reanchor     , "c"    , "change the menu anchoring"                                   },
   { 'b', 'f', "cd"              , ""    , yvikeys_file_loc          , "a"    , "set the default directory for file reading and writing"      },
   { 'b', 'f', "file"            , ""    , yvikeys_file_name         , "a"    , "rename a file for reading and writing"                       },
   { 'b', 'f', "browse"          , ""    , yvikeys_file_browse       , "a"    , "find existing file name for reading and writing"             },
   { 'b', 'f', "control"         , ""    , yvikeys_vers_control      , ""     , "turn version control ON for current file"                    },
   { 'b', 'f', "nocontrol"       , ""    , yvikeys_vers_nocontrol    , ""     , "turn version control OFF for current file"                   },
   { 'b', 'f', "vernum"          , ""    , yvikeys_vers_version      , "s"    , "set a specific file version ([0-9A-Z].[0-9A-Z][a-z])"        },
   { 'b', 'f', "vertxt"          , ""    , yvikeys_vers_vertxt       , "a"    , "set a file version description"                              },
   { 'b', 'f', "major"           , ""    , yvikeys_vers_bump_major   , ""     , "increment the version number by a MAJOR version"             },
   { 'b', 'f', "minor"           , ""    , yvikeys_vers_bump_minor   , ""     , "increment the version number by a MINOR version"             },
   { 'b', 'f', "bump"            , ""    , yvikeys_vers_bump_inc     , ""     , "increment the version number by a INC version"               },
   { 'b', 'f', "write"           , "w"   , yvikeys_file_writer       , ""     , "write/update the current file"                               },
   { 'b', 'f', "writeas"         , "was" , yvikeys_file_writeas      , "s"    , "write/update the current file"                               },
   { 'b', 'f', "read"            , ""    , yvikeys_file_reader       , ""     , "clear existing contents and open/read new file"              },
   { 'b', 'f', "edit"            , "e"   , yvikeys_file_reader       , ""     , "clear existing contents and open/read new file"              },
   { 'b', 'c', "delay"           , ""    , yvikeys_loop_set          , "ss"   , "adjust the main loop wait and screen update timings"         },
   { 'b', 'c', "play"            , ""    , yvikeys_prog_play         , ""     , "cause the progress to play"                                  },
   { 'b', 'c', "stop"            , ""    , yvikeys_prog_stop         , ""     , "cause the progress to stop"                                  },
   { 'b', 'c', "p_scale"         , ""    , yvikeys_scale_prog        , "s"    , "adjust the progress scale"                                   },
   { 'b', 'c', "p_speed"         , ""    , yvikeys_speed_prog        , "s"    , "adjust the progress speed"                                   },
   { 'b', 'e', "mark"            , ""    , yvikeys_mark_direct       , "s"    , ""                                                            },
   { 'b', 'r', "macro"           , ""    , yvikeys_macro__direct     , "a"    , "direct definition of a keyboard macro"                       },
   { 'b', 'r', "script"          , "@"   , yvikeys_script_start      , "s"    , "execution of macro script from a file"                       },
   { 'b', 'r', "playback"        , ""    , yvikeys_script_playback   , "s"    , "execution of macro script from a file"                       },
   { 'b', 'r', "follow"          , ""    , yvikeys_script_follow     , "s"    , "execution of macro script from a file"                       },
   { 'b', 'r', "blitz"           , ""    , yvikeys_script_blitz      , "s"    , "execution of macro script from a file"                       },
   { 'b', 'r', "edelay"          , ""    , yvikeys_macro_edelay      , "c"    , ""                                                            },
   { 'b', 'r', "eupdate"         , ""    , yvikeys_macro_eupdate     , "c"    , ""                                                            },
   { 'b', 'r', "ddelay"          , ""    , yvikeys_macro_ddelay      , "c"    , ""                                                            },
   { 'b', 'r', "dupdate"         , ""    , yvikeys_macro_dupdate     , "c"    , ""                                                            },
   { 'b', 'r', "flatten"         , ""    , yvikeys_macro_flatten     , "cc"   , ""                                                            },
   { 'b', 'r', "install"         , ""    , yvikeys_macro_install     , "c"    , ""                                                            },
   { 'b', 'v', "help"            , ""    , yvikeys_help              , "c"    , ""                                                            },
   { 'b', 'c', "sreg"            , ""    , yvikeys_sreg__direct      , "a"    , "direct definition of source registers"                       },
   { 'b', 'v', "palette"         , ""    , yCOLOR_palette            , "isss" , ""                                                            },
   { 'b', 'v', "wheel"           , ""    , yCOLOR_wheel              , "s"    , ""                                                            },
   { 'b', 'v', "degree"          , "deg" , yCOLOR_deg                , "i"    , ""                                                            },
   { 'b', 'v', "harmony"         , "har" , yCOLOR_harm               , "s"    , ""                                                            },
   { 'b', 'v', "value"           , "val" , yCOLOR_val                , "s"    , ""                                                            },
   { 'b', 'v', "saturation"      , "sat" , yCOLOR_sat                , "s"    , ""                                                            },
   { 'b', 'v', "title"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "version"         , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "buffer"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "formula"         , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "nav"             , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "xaxis"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "yaxis"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "alt"             , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "details"         , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "ribbon"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "progress"        , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "modes"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "status"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "command"         , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "keys"            , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "cursor"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "grid"            , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "edges"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "guides"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "overlay"         , "o"   , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "layers"          , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "notes"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "mask"            , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "color"           , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "back"            , ""    , yvikeys_sizes_switch      , "Cs"   , "allow control of individual sceen elements"                  },
   { 'b', 'v', "menu_loc"        , ""    , yvikeys_sizes_menu_loc    , "c"    , "change where the menu appears in the main window"            },
   { 'b', 'v', "float_loc"       , ""    , yvikeys_sizes_float_loc   , "c"    , "change where the float appears in the main window"           },
   { 'b', 'v', "hist_loc"        , ""    , yvikeys_sizes_hist_loc    , "c"    , "change where the history appears in the main window"         },
   { 'b', 'v', "gridoff"         , ""    , VIEW__grid_offset         , "iii"  , ""                                                            },
   { 'b', 'v', "gridsize"        , ""    , VIEW__grid_size           , "iii"  , ""                                                            },
   { 'b', 'v', "layout"          , ""    , yvikeys_sizes_layout      , "s"    , ""                                                            },
   { 'b', 'v', "layer"           , "l"   , yvikeys_layer_action      , "ss"   , ""                                                            },
   { 'b', 'i', "note"            , ""    , yvikeys_note              , "a"    , "manage screen annotations (notes)"                           },
   /*---(yX11)--------------------------------------------------------------------------------------------------------------------------------*/
   { 'b', 'x', "desktop"         , ""    , yX11_desk_goto            , "c"    , "move between window manager desktops"                        },
   { 'b', 'x', "term"            , ""    , yX11_yvikeys_term         , "a"    , "create a terminal session"                                   },
   { 'b', 'x', "winname"         , ""    , yX11_yvikeys_name         , "a"    , "name an existing window"                                     },
   { 'b', 'x', "sendkeys"        , ""    , yX11_yvikeys_sendkeys     , "a"    , "sendkeys to a specific window"                               },
   { 'b', 'x', "blitzkeys"       , ""    , yX11_yvikeys_blitzkeys    , "a"    , "sendkeys to a specific window"                               },
   { 'b', 'x', "winexact"        , ""    , yX11_yvikeys_winexact     , "cciiii", "sendkeys to a specific window"                              },
   { 'b', 'x', "winplace"        , ""    , yX11_yvikeys_winplace     , "a"    , "sendkeys to a specific window"                               },
   { 'b', 'x', "winbring"        , ""    , yX11_yvikeys_winbring     , "c"    , "sendkeys to a specific window"                               },
   { 'b', 'x', "wingoto"         , ""    , yX11_yvikeys_wingoto      , "c"    , "sendkeys to a specific window"                               },
   { 'b', 'x', "winsend"         , ""    , yX11_yvikeys_winsend      , "cc"   , "sendkeys to a specific window"                               },
   { 'b', 'x', "wintake"         , ""    , yX11_yvikeys_wintake      , "cc"   , "sendkeys to a specific window"                               },
   { 'b', 'x', "mydesk"          , ""    , yX11_yvikeys_mydesk       , "c"    , "change position of current window"                          },
   { 'b', 'x', "myhome"          , ""    , yX11_yvikeys_myhome       , "ii"   , "change position of current window"                          },
   { 'b', 'x', "mysize"          , ""    , yX11_yvikeys_mysize       , "ii"   , "change size of current window"                              },
   { 'b', 'x', "mysizer"         , ""    , yX11_yvikeys_mysizer      , "iiii" , "reset all desktops, windows, and shortcuts"                  },
   /*---(done)--------------------------------------------------------------------------------------------------------------------------------*/
   { 0  , 0  , "-"               , ""    , NULL                      , ""     , ""                                                            },
};



typedef  struct cLINK   tLINK;
struct cLINK {
   tCMDS      *data;     /* actual command                  */
   char        nlen;     /* length of name (speed searches) */
   char        alen;     /* length of name (speed searches) */
   char        nterms;   /* index to terms db (speed exec)  */
   tLINK      *m_next;
};
/*---(true useful vars)---------------*/
static tLINK  *s_head   = NULL;              /* head of link chain            */
static tLINK  *s_tail   = NULL;              /* tail of link chain            */
/*---(menu grphics/unit testing)------*/
static tLINK  *s_found  = NULL;              /* result of last find           */
/*---(DEBUGGING FASTER)---------------*/
static short   s_ncmd   = 0;                 /* all menu items in list        */
static short   s_nbase  = 0;                 /* base menu items in list       */
/*---(field parsing)------------------*/
static char    s_fields [10][LEN_RECD];
static int     s_nfield =  0;
static char    s_all    [LEN_RECD]       = "";
/*---(done)---------------------------*/






/*====================------------------------------------====================*/
/*===----                     support functions                        ----===*/
/*====================------------------------------------====================*/
static void  o___C_SUPPORT_______o () { return; }

char
yvikeys_cmds__name      (uchar *a_name, char a_type, int *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_len != NULL)  *a_len = 0;
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (a_name);
   DEBUG_CMDS   yLOG_sint    (x_len);
   --rce;  if (a_type != 'a' && (x_len < 1 || x_len > 18)) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_type == 'a' && x_len <= 0) {
      DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   --rce;  if (a_type == 'a' && x_len > 4) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check characters)---------------*/
   --rce;  if (strchr ("0123456789_", a_name [0]) != NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   for (i = 0; i < x_len; ++i) {
      if (strchr (s_valid, a_name [i]) != NULL)  continue;
      DEBUG_CMDS   yLOG_snote   ("bad character in name");
      DEBUG_CMDS   yLOG_sint    (i);
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(output)-------------------------*/
   if (a_len != NULL)  *a_len = x_len;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      quick ones                              ----===*/
/*====================------------------------------------====================*/
static void  o___C_QUICK_________o () { return; }

char yvikeys_cmds__quit      (void) { myVIKEYS.done = 'y';     return 0; }
char yvikeys_cmds__write     (void) { yvikeys_file_writer ();  return 0; }
char yvikeys_cmds__writequit (void) { yvikeys_file_writer (); myVIKEYS.done = 'y'; return 0; }



/*====================------------------------------------====================*/
/*===----                      terms support                           ----===*/
/*====================------------------------------------====================*/
static void  o___TERMS___________o () { return; }

char
yvikeys_cmds__term   (char *a_terms)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_terms);
   --rce;  if (a_terms == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(walk)---------------------------*/
   for (i = 0; i < s_nterm; ++i) {
      if (a_terms [0] != s_terms [i].name [0])       continue;
      if (strcmp (a_terms, s_terms [i].name) != 0)   continue;
      DEBUG_CMDS   yLOG_snote   ("found");
      DEBUG_CMDS   yLOG_sint    (i);
      DEBUG_CMDS   yLOG_sexit   (__FUNCTION__);
      return i;
   }
   /*---(failed)-------------------------*/
   --rce;
   DEBUG_CMDS   yLOG_snote   ("FAILED");
   DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
   return rce;
}

char
yvikeys_cmds__launch      (tLINK *a_link)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   DEBUG_CMDS   yLOG_info    ("terms"     , a_link->data->terms);
   DEBUG_CMDS   yLOG_value   ("nterms"    , a_link->nterms);
   DEBUG_CMDS   yLOG_point   ("function"  , a_link->data->f.v);
   /*---(run)----------------------------*/
   switch (a_link->nterms) {
   case  0 : rc = a_link->data->f.v      ();                                                                break;
   case  1 : rc = a_link->data->f.v      ();                                                                break;
   case  2 : rc = a_link->data->f.c      (s_fields [1][0]);                                                 break;
   case  3 : rc = a_link->data->f.cc     (s_fields [1][0], s_fields [2][0]);                                break;
   case  4 : rc = a_link->data->f.i      (atoi (s_fields [1]));                                             break;
   case  5 : rc = a_link->data->f.is     (atoi (s_fields [1]), s_fields [1]);                               break;
   case  6 : rc = a_link->data->f.s      (s_fields [1]);                                                    break;
   case  7 : rc = a_link->data->f.ss     (s_fields [1], s_fields [2]);                                      break;
   case  8 : rc = a_link->data->f.si     (s_fields [1], atoi (s_fields [2]));                               break;
   case  9 : rc = a_link->data->f.s      (s_all);                                                           break;
   case 10 : rc = a_link->data->f.ii     (atoi (s_fields [1]), atoi (s_fields [2]));                        break;
   case 11 : rc = a_link->data->f.iii    (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));   break;
   case 12 : rc = a_link->data->f.sii    (s_fields [1], atoi (s_fields [2]), atoi (s_fields [3]));          break;
   case 13 : rc = a_link->data->f.isss   (atoi (s_fields [1]), s_fields [2], s_fields [3], s_fields [4]);   break;
   case 14 : rc = a_link->data->f.ss     (s_fields [0], s_fields [1]);                                      break;
   case 15 : rc = a_link->data->f.css    (s_fields [1][0], s_fields [2], s_fields [3]);                     break;
   case 16 : rc = a_link->data->f.cs     (s_fields [1][0], s_fields [2]);                                   break;
   case 17 : rc = a_link->data->f.f      (atof (s_fields [1]));                                             break;
   case 18 : rc = a_link->data->f.cciiii (s_fields [1][0], s_fields [2][0], atoi (s_fields [3]), atoi (s_fields [4]), atoi (s_fields [5]), atoi (s_fields [6]));  break;
   case 19 : rc = a_link->data->f.iiii   (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]), atoi (s_fields [4]));  break;
   default : rc = -1;                                                                                       break;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      memory allocation                       ----===*/
/*====================------------------------------------====================*/
static void  o___C_MEMORY________o () { return; }

char
yvikeys_cmds__newlink   (tCMDS *a_cmd)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tLINK      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   tLINK      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_point   ("a_cmd"     , a_cmd);
   --rce;  if (a_cmd == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_cmd"     , a_cmd);
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tLINK *) malloc (sizeof (tLINK));
   }
   DEBUG_CMDS   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_CMDS   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CMDS   yLOG_note    ("FAILED");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_CMDS   yLOG_note    ("populate");
   x_new->data     = a_cmd;
   x_new->nlen     = strlen (a_cmd->name);
   x_new->nterms   = yvikeys_cmds__term (a_cmd->terms);
   x_new->alen     = strlen (a_cmd->abbr);
   x_new->m_next   = NULL;
   /*---(tie to master list)-------------*/
   if (s_head == NULL) {
      DEBUG_CMDS   yLOG_note    ("nothing in master list, make first");
      s_head         = x_new;
   } else  {
      DEBUG_CMDS   yLOG_note    ("append to master list");
      s_tail->m_next = x_new;
   }
   s_tail        = x_new;
   DEBUG_CMDS   yLOG_point   ("s_head"    , s_head);
   DEBUG_CMDS   yLOG_point   ("->m_next"  , s_head->m_next);
   DEBUG_CMDS   yLOG_point   ("s_tail"    , s_tail);
   DEBUG_CMDS   yLOG_point   ("m_next"    , x_new->m_next);
   /*---(list commands)------------------*/
   x_curr = s_head;
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("entry"     , "%-10.10s %10p %10p", x_curr->data->name, x_curr, x_curr->m_next);
      x_curr = x_curr->m_next;
   }
   /*---(update counts)------------------*/
   ++s_ncmd;
   if (x_new->data->base  == CMDS_BASE)    ++s_nbase;
   DEBUG_CMDS   yLOG_value   ("s_ncmd"    , s_ncmd);
   DEBUG_CMDS   yLOG_value   ("s_nbase"   , s_nbase);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_cmds__newcmd    (uchar a_menu, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCMDS   *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   tLINK      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tCMDS *) malloc (sizeof (tCMDS));
   }
   DEBUG_CMDS   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_CMDS   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CMDS   yLOG_note    ("FAILED");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_CMDS   yLOG_note    ("populate");
   x_new->base     = CMDS_CUST;
   x_new->menu     = a_menu;
   strlcpy (x_new->name , a_name , LEN_LABEL);
   strlcpy (x_new->abbr , a_abbr , LEN_TERSE);
   x_new->f.v      = a_func;
   strlcpy (x_new->terms, a_terms, LEN_TERSE);
   strlcpy (x_new->desc , a_desc , LEN_DESC );
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   /*---(now place)----------------------*/
   rc = yvikeys_cmds__newlink (x_new);
   --rce;  if (rc != 0) {
      DEBUG_CMDS   yLOG_note    ("link failed, freeing menu item");
      free (x_new);
      return rce;
   }
   /*---(list commands)------------------*/
   x_curr = s_head;
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("entry"     , "%-10.10s %10p %10p", x_curr->data->name, x_curr, x_curr->m_next);
      x_curr = x_curr->m_next;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_cmds__purge     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   tLINK      *x_curr      = NULL;
   tLINK      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   x_curr = s_head;
   /*---(clear)--------------------------*/
   DEBUG_CMDS   yLOG_value   ("s_ncmd"    , s_ncmd);
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("focus"     , "%c %s", x_curr->data->base, x_curr->data->name);
      x_next = x_curr->m_next;
      if (x_curr->data->base != CMDS_BASE) {
         free (x_curr->data);
      }
      x_curr->data = NULL;
      free (x_curr);
      x_curr = NULL;
      x_curr = x_next;
   }
   /*---(initialize pointers)------------*/
   DEBUG_CMDS   yLOG_note    ("pointers");
   s_head   = NULL;
   s_found  = NULL;
   /*---(initialize counters)------------*/
   DEBUG_CMDS   yLOG_note    ("counters");
   s_ncmd   = 0;
   s_nbase  = 0;
   /*---(fields)-------------------------*/
   for (i = 0; i < 10; ++i)  s_fields [i][0] = '\0';
   s_nfield  = 0;
   s_all [0] = '\0';
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_cmds__load_name (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defence)------------------------*/
   rc = yvikeys_cmds__name (a_name, 'n', NULL);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(load)---------------------------*/
   for (i = 0; i < MAX_CMDS; ++i) {
      /*---(check for end)---------------*/
      if (s_base [i].name [0] == NULL)                break;
      if (strcmp (s_base [i].name, "-") == 0)         break;
      /*---(filter)----------------------*/
      if (s_base [i].name [0] != a_name [0])          continue;
      if (strcmp (s_base [i].name, a_name) != 0)      continue;
      rc = yvikeys_cmds__newlink (s_base + i);
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_cmds__load      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(load)---------------------------*/
   for (i = 0; i < MAX_CMDS; ++i) {
      /*---(check for end)---------------*/
      if (s_base [i].name [0] == NULL)                break;
      if (strcmp (s_base [i].name, "-") == 0)         break;
      rc = yvikeys_cmds__newlink (s_base + i);
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       list actions                           ----===*/
/*====================------------------------------------====================*/
static void  o___C_ACTIONS_______o () { return; }

char
yvikeys_cmds__action    (char a_act, uchar *a_name, char *a_rc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_found     =  '-';
   tLINK      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_found  = NULL; /* reset unit testing holder */
   if (a_rc != NULL)  *a_rc = 0;
   /*---(defence)------------------------*/
   rc = yvikeys_cmds__name (a_name, 'n', &x_len);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_name"    , a_name);
   /*---(find)---------------------------*/
   x_curr = s_head;
   --rce;  while (x_curr != NULL) {
      /*---(output)----------------------*/
      DEBUG_CMDS   yLOG_complex ("checking"  , "%c %-5.5s %s", x_curr->data->base, x_curr->data->abbr, x_curr->data->name);
      /*---(check abbr)------------------*/
      if (x_curr->alen > 0 && x_curr->alen == x_len) {
         if (x_curr->data->abbr [0] == a_name [0]) {
            if (strcmp (x_curr->data->abbr, a_name) == 0) {
               x_found = 'y';
               strcpy (s_fields [0], x_curr->data->name);
            }
         }
      }
      /*---(check name)------------------*/
      if (x_curr->nlen > 0 && x_curr->nlen == x_len) {
         if (x_curr->data->name [0] == a_name [0]) {
            if (strcmp (x_curr->data->name, a_name) == 0) x_found = 'y';
         }
      }
      /*---(action)----------------------*/
      if (x_found == 'y') {
         s_found = x_curr;
         switch (a_act) {
         case CMDS_FIND :
            DEBUG_CMDS   yLOG_note    ("found and returning");
            DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
            return 1;
         case CMDS_EXEC :
            DEBUG_CMDS   yLOG_note    ("found and executing");
            rc = yvikeys_cmds__launch (x_curr);
            DEBUG_CMDS   yLOG_value   ("launch"    , rc);
            if (a_rc != NULL)  *a_rc = rc;
            DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
            return 0;
         default :
            DEBUG_CMDS   yLOG_note    ("found but action not understood");
            DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(next)------------------------*/
      x_curr = x_curr->m_next;
   }
   /*---(complete)-----------------------*/
   --rce;
   DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char yvikeys_cmds__find   (uchar *a_name)             { return yvikeys_cmds__action (CMDS_FIND, a_name, NULL); }
char yvikeys_cmds__exec   (uchar *a_name, char *a_rc) { return yvikeys_cmds__action (CMDS_EXEC, a_name, a_rc); }



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___COMMAND_________o () { return; }

char
yvikeys_cmds_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_COMMAND)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(terms)--------------------------*/
   DEBUG_PROG   yLOG_note    ("initialize term system");
   s_nterm = 0;
   for (i = 0; i < MAX_TERMS; ++i) {
      if (s_terms [i].name [0] == '-')  break;
      ++s_nterm;
   }
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("initialize command system");
   yvikeys_cmds__purge ();
   yvikeys_cmds__load  ();
   /*---(update status)------------------*/
   DEBUG_PROG   yLOG_note    ("update status");
   STATUS_init_set   (MODE_COMMAND);
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("add universal commands");
   myVIKEYS.done = '-';
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_cmds_wrap       (void)
{
   yvikeys_cmds__purge ();
   return  0;
}

char
yVIKEYS_cmds_direct     (char *a_text)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   int         b, x, y, z;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_text"    , a_text);
   x_len = strlen (a_text);
   DEBUG_CMDS   yLOG_value   ("x_len"     , x_len);
   /*---(check address)------------------*/
   rc = yvikeys_map_locator (a_text + 1, &b, &x, &y, &z);
   DEBUG_CMDS   yLOG_value   ("locator"   , rc);
   --rce;  if (rc >= 0) {
      DEBUG_CMDS   yLOG_complex ("position"  , "%3db, %3dx, %3dy, %3dz", b, x, y, z);
      rc = yVIKEYS_jump (b, x, y, z);
      DEBUG_CMDS   yLOG_value   ("jump"      , rc);
      if (rc < 0) {
         DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(normal command)-----------------*/
   rc = yvikeys_hist_text (MODE_COMMAND, a_text);
   DEBUG_CMDS   yLOG_value   ("history"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_hist_exec (MODE_COMMAND);
   DEBUG_CMDS   yLOG_value   ("exec"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_cmds_add     (uchar a_menu, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_terms     =    0;
   int         n           =   -1;
   int         x_len       =    0;
   char        x_dup       =    0;
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
   --rce;  if (a_menu == 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   n  = yvikeys_menu_cmds (a_menu);
   DEBUG_CMDS   yLOG_value   ("menu"      , n);
   --rce;  if (n <= 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_cmds__name (a_name, 'n', NULL);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_cmds__name (a_abbr, 'a', &x_len);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_terms"   , a_terms);
   x_terms = yvikeys_cmds__term (a_terms);
   --rce;  if (x_terms < 0) {
      DEBUG_CMDS   yLOG_note    ("term specification not found in inventory");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_func"    , a_func);
   --rce;  if (a_func  == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_desc"    , a_desc);
   --rce;  if (a_desc == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for dup)------------------*/
   x_dup = yvikeys_cmds__find (a_name);
   DEBUG_CMDS   yLOG_value   ("dup name"  , x_dup);
   --rce;  if (x_dup > 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_dup = yvikeys_cmds__find (a_abbr);
   DEBUG_CMDS   yLOG_value   ("dup abbr"  , x_dup);
   --rce;  if (x_dup > 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(call add)-----------------------*/
   rc = yvikeys_cmds__newcmd    (a_menu, a_name, a_abbr, a_terms, a_func, a_desc);
   DEBUG_CMDS   yLOG_value   ("newcmd"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds__parse   (uchar *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   char       *q           = " ";
   char       *r           = NULL;
   uchar       x_work      [LEN_RECD]   = "";
   int         x_len       = 0;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   strlcpy (x_work, a_string, LEN_RECD);
   x_len = strllen (x_work, LEN_RECD);
   strldchg (x_work, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   DEBUG_CMDS   yLOG_info    ("x_work"    , x_work);
   /*---(parse command)------------------*/
   p     = strtok_r (x_work, q, &r);
   ++p;
   x_len = strllen (p, LEN_RECD);
   DEBUG_CMDS   yLOG_info    ("g_cmd"     , p);
   if (strllen (x_work, LEN_RECD) > x_len) {
      strlcpy (s_all, p + x_len + 1, LEN_RECD);
      strltrim (s_all, ySTR_BOTH, LEN_RECD);
   }
   DEBUG_CMDS   yLOG_info    ("s_all"     , s_all);
   /*---(parse)--------------------------*/
   for (i = 0; i < 10; ++i)  strlcpy (s_fields [i], "", LEN_RECD);
   for (i = 0; i < 10; ++i) {
      DEBUG_CMDS   yLOG_value   ("i"         , i);
      DEBUG_CMDS   yLOG_info    ("p"         , p);
      strltrim (p, ySTR_BOTH, LEN_RECD);
      strlcpy (s_fields [i], p, LEN_RECD);
      s_nfield = i + 1;
      p = strtok_r (NULL  , q, &r);
      if (p == NULL)  break;
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds_exec     (uchar *a_command, char *a_rc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_rc        =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_rc != NULL)  *a_rc = 0;
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_CMDS   yLOG_note    ("can not execute until operational");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_command" , a_command);
   /*---(look for system)---------------*/
   if (a_command [1] == '!') {
      DEBUG_CMDS   yLOG_note    ("this is a direct run");
      rc = system (a_command + 2);
      DEBUG_CMDS   yLOG_value   ("rc"        , rc);
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)-------------------------*/
   rc = yvikeys_cmds__parse (a_command);
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_note    ("could not parse");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   rc = yvikeys_cmds__exec (s_fields [0], &x_rc);
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   DEBUG_CMDS   yLOG_value   ("x_rc"      , x_rc);
   /*---(output)-------------------------*/
   if (a_rc != NULL)  *a_rc = x_rc;
   if (rc >= 0 && x_rc < 0)  rc = x_rc;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char
yvikeys_cmds__test      (char a_mode, char a_value)
{
   switch (a_mode) {
   case '+' : a_value += 1;  break;
   case '5' : a_value += 5;  break;
   case '-' : a_value -= 1;  break;
   case '0' : a_value  = 0;  break;
   case '9' : a_value -= 9;  break;
   }
   return a_value;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds__unit      (char *a_question, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   int         n           =    0;
   char        r           [LEN_RECD ] = "";
   char        s           [LEN_RECD ] = "";
   char        t           [LEN_RECD ] = "";
   tLINK      *x_curr      = NULL;
   tCMDS      *p           = NULL;
   int         c           =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "CMDS unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   n = a_name;
   /*> if      (strcmp (a_question, "global"         )   == 0) {                                                       <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS global      : %2d %2d[%-.40s]", s_ncmd, s_len, s_current);   <* 
    *> }                                                                                                               <*/
   if (strcmp (a_question, "counts"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS count       : %3dn, %3db", s_ncmd, s_nbase);
   }
   else if (strcmp (a_question, "term_count"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS term count  : %d", s_nterm);
   }
   else if (strcmp (a_question, "term"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS term    (%2d): %-5.5s %s", n, s_terms [n].name, s_terms [n].disp);
   }
   else if (strcmp (a_question, "master"         )   == 0) {
      x_curr = s_head;
      DEBUG_CMDS   yLOG_value   ("n"         , n);
      while (x_curr != NULL) {
         DEBUG_CMDS   yLOG_complex ("entry"     , "%d %-10.10s %10p %10p", c, x_curr->data->name, x_curr, x_curr->m_next);
         if (c == n) {
            DEBUG_CMDS   yLOG_point   ("found"     , x_curr);
            break;
         }
         x_curr = x_curr->m_next;
         ++c;
      }
      DEBUG_CMDS   yLOG_point   ("found"     , x_curr);
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS master (%2d) : -  0:-            0:-     0:-    %p", n, NULL);
      else {
         p = x_curr->data;
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS master (%2d) : %c %2d:%-12.12s %1d:%-4.4s %2d:%-4.4s %p",
               n, p->base, x_curr->nlen, p->name, x_curr->alen, p->abbr, x_curr->nterms, p->terms, p->f.v);
      }
   }
   else if (strcmp (a_question, "found"          )   == 0) {
      yvikeys_cmds__find (a_name);
      if (s_found == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS found       : -  0:-            0:-     0:-    %p", NULL);
      else {
         p = s_found->data;
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS found       : %c %2d:%-12.12s %1d:%-4.4s %2d:%-4.4s %p",
               p->base, s_found->nlen, p->name, s_found->alen, p->abbr, s_found->nterms, p->terms, p->f.v);
      }
   }
   /*> else if (strcmp (a_question, "history"        )   == 0) {                                                                                                             <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS history     : %2d %c %2d %3d  %-.40s", n, s_runs [n].mark, s_runs [n].count, s_runs [n].found, s_runs [n].text);   <* 
    *> }                                                                                                                                                                     <*/
   else if (strcmp (a_question, "parse"          )   == 0) {
      if (n >= 0 && n <= 9)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS parse       : %2d of %2d, %2d[%s]", n, s_nfield, strlen (s_fields [n]), s_fields [n]);
      else                   snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS parse all   : %2d[%s]"            , strlen (s_all), s_all);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


