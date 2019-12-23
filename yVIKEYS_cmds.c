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


#define     DEG2RAD  (3.1415927 / 180.0)
#define     RAD2DEG  (180.0 / 3.1415927)


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
   { ""     , "void"                       },
   { "v"    , "void"                       },
   { "c"    , "char"                       },
   { "cc"   , "char, char"                 },
   { "i"    , "int"                        },
   { "is"   , "int, char*"                 },
   { "s"    , "char*"                      },
   { "ss"   , "char*, char*"               },
   { "si"   , "char*, int"                 },
   { "a"    , "char*"                      },  /* string incudes spaces       */
   { "ii"   , "int, int"                   },
   { "iii"  , "int, int, int"              },
   { "sii"  , "char*, int, int"            },
   { "isss" , "int, char*, char*, char*"   },
   { "Cs"   , "char*, char*"               },  /* command and arg both        */
   { "-"    , "-"                          },
};
static  int s_nterm  = 0;



#define     CMDS_BASE        'b'  /* fully standard in base           */
#define     CMDS_CUST        'c'  /* fully custom, not in base        */

#define     CMDS_FIND        'f'  /* find command in list             */
#define     CMDS_EXEC        'x'  /* execute command                  */

static const uchar  *s_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

/*===[[ COMMANDS ]]===========================================================*/

#define  MAX_CMDS      1000
typedef  struct cCMDS   tCMDS;
struct  cCMDS {
   char        base;
   char        menu;                        /* category                       */
   char        name        [LEN_LABEL];     /* full command name              */
   char        abbr        [LEN_TERSE];     /* abbreviation of name           */
   union {
      char        (*v   ) (void);           /* function pointer               */
      char        (*c   ) (char);           /* function pointer               */
      char        (*cc  ) (char , char );   /* function pointer               */
      char        (*i   ) (int);            /* function pointer               */
      char        (*ii  ) (int, int);       /* function pointer               */
      char        (*iii ) (int, int, int);  /* function pointer               */
      char        (*isss) (int, char*, char*, char*);
      char        (*is  ) (int  , char*);   /* function pointer               */
      char        (*s   ) (char*);          /* function pointer               */
      char        (*ss  ) (char*, char*);   /* function pointer               */
      char        (*si  ) (char*, int  );   /* function pointer               */
      char        (*sii ) (char*, int, int);  /* function pointer               */
   } f;
   char        terms       [LEN_TERSE];     /* type of terms/args             */
   char        desc        [LEN_DESC];      /* descriptive label              */
};
static const tCMDS  s_base      [] = {
   { CMDS_BASE, 'f', "quit"            , "q"   , yvikeys_cmds__quit        , ""    , "quit current file (if no changes), exit if the only file"    },
   { CMDS_BASE, 'f', "quitall"         , "qa"  , yvikeys_cmds__quit        , ""    , "quit all files (if no changes), and exit"                    },
   { CMDS_BASE, 'f', "writequit"       , "wq"  , yvikeys_cmds__writequit   , ""    , ""                                                            },
   { CMDS_BASE, 'f', "writequitall"    , "wqa" , yvikeys_cmds__writequit   , ""    , ""                                                            },
   { CMDS_BASE, 'e', "dump"            , ""    , yvikeys_dump_exec         , "s"   , "dump a specified data table to the clipboard in flat text"   },
   { CMDS_BASE, 'c', "menu"            , ""    , yvikeys_menu_reanchor     , "i"   , "change the menu anchoring"                                   },
   { CMDS_BASE, 'f', "cd"              , ""    , yvikeys_file_loc          , "a"   , "set the default directory for file reading and writing"      },
   { CMDS_BASE, 'f', "file"            , ""    , yvikeys_file_name         , "a"   , "rename a file for reading and writing"                       },
   { CMDS_BASE, 'f', "browse"          , ""    , yvikeys_file_browse       , "a"   , "find existing file name for reading and writing"             },
   { CMDS_BASE, 'f', "control"         , ""    , yvikeys_vers_control      , ""    , "turn version control ON for current file"                    },
   { CMDS_BASE, 'f', "nocontrol"       , ""    , yvikeys_vers_nocontrol    , ""    , "turn version control OFF for current file"                   },
   { CMDS_BASE, 'f', "vernum"          , ""    , yvikeys_vers_version      , "s"   , "set a specific file version ([0-9A-Z].[0-9A-Z][a-z])"        },
   { CMDS_BASE, 'f', "vertxt"          , ""    , yvikeys_vers_vertxt       , "a"   , "set a file version description"                              },
   { CMDS_BASE, 'f', "major"           , ""    , yvikeys_vers_bump_major   , ""    , "increment the version number by a MAJOR version"             },
   { CMDS_BASE, 'f', "minor"           , ""    , yvikeys_vers_bump_minor   , ""    , "increment the version number by a MINOR version"             },
   { CMDS_BASE, 'f', "bump"            , ""    , yvikeys_vers_bump_inc     , ""    , "increment the version number by a INC version"               },
   { CMDS_BASE, 'f', "write"           , "w"   , yvikeys_file_writer       , ""    , "write/update the current file"                               },
   { CMDS_BASE, 'f', "writeas"         , "was" , yvikeys_file_writeas      , "s"   , "write/update the current file"                               },
   { CMDS_BASE, 'f', "read"            , ""    , yvikeys_file_reader       , ""    , "clear existing contents and open/read new file"              },
   { CMDS_BASE, 'f', "edit"            , "e"   , yvikeys_file_reader       , ""    , "clear existing contents and open/read new file"              },
   { CMDS_BASE, 'c', "delay"           , ""    , yvikeys_loop_set          , "ss"  , "adjust the main loop wait and screen update timings"         },
   { CMDS_BASE, 'c', "p_scale"         , ""    , yvikeys_scale_prog        , "s"   , "adjust the progress scale"                                   },
   { CMDS_BASE, 'c', "p_speed"         , ""    , yvikeys_speed_prog        , "s"   , "adjust the progress speed"                                   },
   { CMDS_BASE, 'e', "mark"            , ""    , yvikeys_mark_direct       , "s"   , ""                                                            },
   { CMDS_BASE, 'r', "macro"           , ""    , yvikeys_macro__direct     , "a"   , "direct definition of a keyboard macro"                       },
   { CMDS_BASE, 'r', "edelay"          , ""    , yvikeys_macro_edelay      , "c"   , ""                                                            },
   { CMDS_BASE, 'r', "eupdate"         , ""    , yvikeys_macro_eupdate     , "c"   , ""                                                            },
   { CMDS_BASE, 'r', "ddelay"          , ""    , yvikeys_macro_ddelay      , "c"   , ""                                                            },
   { CMDS_BASE, 'r', "dupdate"         , ""    , yvikeys_macro_dupdate     , "c"   , ""                                                            },
   { CMDS_BASE, 'r', "flatten"         , ""    , yvikeys_macro_flatten     , "cc"  , ""                                                            },
   { CMDS_BASE, 'r', "install"         , ""    , yvikeys_macro_install     , "c"   , ""                                                            },
   { CMDS_BASE, 'v', "help"            , ""    , yvikeys_help              , "c"   , ""                                                            },
   { CMDS_BASE, 'c', "sreg"            , ""    , yvikeys_sreg__direct      , "a"   , "direct definition of source registers"                       },
   { CMDS_BASE, 'v', "palette"         , ""    , yCOLOR_palette            , "isss", ""                                                            },
   { CMDS_BASE, 'v', "wheel"           , ""    , yCOLOR_wheel              , "s"   , ""                                                            },
   { CMDS_BASE, 'v', "degree"          , "deg" , yCOLOR_deg                , "i"   , ""                                                            },
   { CMDS_BASE, 'v', "harmony"         , "har" , yCOLOR_harm               , "s"   , ""                                                            },
   { CMDS_BASE, 'v', "value"           , "val" , yCOLOR_val                , "s"   , ""                                                            },
   { CMDS_BASE, 'v', "saturation"      , "sat" , yCOLOR_sat                , "s"   , ""                                                            },
   { CMDS_BASE, 'v', "title"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "version"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "buffer"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "formula"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "nav"             , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "xaxis"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "yaxis"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "alt"             , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "details"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "ribbon"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "progress"        , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "modes"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "status"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "command"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "keys"            , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "cursor"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "grid"            , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "edges"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "guides"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "overlay"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "layers"          , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "notes"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "color"           , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "back"            , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "menu"            , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "history"         , ""    , VIEW__switch              , "Cs"  , "allow control of individual sceen elements"                  },
   { CMDS_BASE, 'v', "gridoff"         , ""    , VIEW__grid_offset         , "iii" , ""                                                            },
   { CMDS_BASE, 'v', "gridsize"        , ""    , VIEW__grid_size           , "iii" , ""                                                            },
   { CMDS_BASE, 'v', "layout"          , ""    , VIEW__layout              , "s"   , ""                                                            },
   { CMDS_BASE, 'v', "layer"           , ""    , VIEW__layer_set           , "s"   , ""                                                            },
   { 0        , 0  , "-"               , ""    , NULL                      , ""    , ""                                                            },
};



typedef  struct cLINK   tLINK;
struct cLINK {
   tCMDS   *data;
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
/*---(current)-----------------*/
static char    s_current   [LEN_RECD] = "";
static int     s_len       = 0;
/*---(done)---------------------------*/





/*===[[ SEARCH ]]=============================================================*/

#define   MAX_SRCH    2000
typedef   struct   cSRCH   tSRCH;
struct cSRCH {
   int         pass;
   char        label       [LEN_LABEL];
   int         b_pos;
   int         x_pos;
   int         y_pos;
   int         z_pos;
};
static tSRCH     s_srch      [MAX_SRCH];
static int       s_nsrch     = 0;
static int       s_csrch     = 0;

char    (*s_searcher) (char *a_search);
char    (*s_clearer ) (int a_buf, int a_x, int a_y, int a_z);



/*====================------------------------------------====================*/
/*===----                     support functions                        ----===*/
/*====================------------------------------------====================*/
static void  o___C_SUPPORT_______o () { return; }

char
yvikeys_cmds__name      (uchar *a_name, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (a_name);
   DEBUG_CMDS   yLOG_sint    (x_len);
   --rce;  if (x_len < 1 || x_len > 18) {
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
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
   --rce;  for (i = 0; i < x_len; ++i) {
      if (strchr (s_valid, a_name [i]) != NULL)  continue;
      DEBUG_CMDS   yLOG_snote   ("bad character in name");
      DEBUG_CMDS   yLOG_sint    (i);
      DEBUG_CMDS   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
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
   /*---(run)----------------------------*/
   switch (a_link->nterms) {
   case  0 : rc = a_link->data->f.v    ();                                                                break;
   case  1 : rc = a_link->data->f.v    ();                                                                break;
   case  2 : rc = a_link->data->f.c    (s_fields [1][0]);                                                 break;
   case  3 : rc = a_link->data->f.cc   (s_fields [1][0], s_fields [2][0]);                                break;
   case  4 : rc = a_link->data->f.i    (atoi (s_fields [1]));                                             break;
   case  5 : rc = a_link->data->f.is   (atoi (s_fields [1]), s_fields [1]);                               break;
   case  6 : rc = a_link->data->f.s    (s_fields [1]);                                                    break;
   case  7 : rc = a_link->data->f.ss   (s_fields [1], s_fields [2]);                                      break;
   case  8 : rc = a_link->data->f.si   (s_fields [1], atoi (s_fields [2]));                               break;
   case  9 : rc = a_link->data->f.s    (s_all);                                                           break;
   case 10 : rc = a_link->data->f.ii   (atoi (s_fields [1]), atoi (s_fields [2]));                        break;
   case 11 : rc = a_link->data->f.iii  (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));   break;
   case 12 : rc = a_link->data->f.sii  (s_fields [1], atoi (s_fields [2]), atoi (s_fields [3]));          break;
   case 13 : rc = a_link->data->f.isss (atoi (s_fields [1]), s_fields [2], s_fields [3], s_fields [4]);   break;
   case 14 : rc = a_link->data->f.ss   (s_fields [0], s_fields [1]);                                      break;
   default : rc = -1;                                                                                  break;
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
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   x_curr = s_head;
   /*---(clear)--------------------------*/
   DEBUG_CMDS   yLOG_value   ("s_ncmd"    , s_ncmd);
   while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("focus"     , "%c %s", x_curr->data->base, x_curr->data->name);
      if (x_curr->data->base != CMDS_BASE) {
         free (x_curr->data);
      }
      free (x_curr);
      x_curr = x_curr->m_next;
   }
   /*---(initialize pointers)------------*/
   DEBUG_CMDS   yLOG_note    ("pointers");
   s_head   = NULL;
   s_found  = NULL;
   /*---(initialize counters)------------*/
   DEBUG_CMDS   yLOG_note    ("counters");
   s_ncmd   = 0;
   s_nbase  = 0;
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
   rc = yvikeys_cmds__name (a_name, 'n');
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

int
yvikeys_cmds__action    (char a_act, uchar *a_name)
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
   /*---(defence)------------------------*/
   rc = yvikeys_cmds__name (a_name, 'n');
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find)---------------------------*/
   x_curr = s_head;
   --rce;  while (x_curr != NULL) {
      /*---(output)----------------------*/
      DEBUG_CMDS   yLOG_complex ("checking"  , "%c %-5.5s %s", x_curr->data->base, x_curr->data->abbr, x_curr->data->name);
      /*---(check abbr)------------------*/
      if (x_curr->alen > 0 && x_curr->alen == x_len) {
         if (x_curr->data->abbr [0] == a_name [0]) {
            if (strcmp (x_curr->data->abbr, a_name) == 0) x_found = 'y';
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
            rc = yvikeys_cmds__launch (x_curr->data);
            DEBUG_CMDS   yLOG_value   ("launch"    , rc);
            DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
            return rc;
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
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return rc;
}

char yvikeys_cmds__find   (uchar *a_name) { return yvikeys_cmds__action (CMDS_FIND  , a_name); }
char yvikeys_cmds__exec   (uchar *a_name) { return yvikeys_cmds__action (CMDS_EXEC  , a_name); }



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
   /*---(yparse)-------------------------*/
   rc = yPARSE_handler (MODE_COMMAND , "command"   , 7.4, "cO----------", -1, yvikeys_cmds__reader, yvikeys_cmds__writer_all, "------------" , "a,command-----------------", "command history"           );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
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
   rc = HISTORY__load (MODE_COMMAND, a_text);
   DEBUG_CMDS   yLOG_value   ("history"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = HISTORY__exec (MODE_COMMAND);
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
   rc = yvikeys_cmds__name (a_name, 'n');
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_cmds__name (a_abbr, 'a');
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
yvikeys_cmds_exec     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("s_current" , s_current);
   /*---(look for system)---------------*/
   if (s_current [1] == '!') {
      DEBUG_CMDS   yLOG_note    ("this is a direct run");
      rc = system (s_current + 2);
      DEBUG_CMDS   yLOG_value   ("rc"        , rc);
      DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)-------------------------*/
   rc = yvikeys_cmds__parse (s_current);
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_note    ("could not parse");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find)---------------------------*/
   i = yvikeys_cmds__exec (s_fields [0]);
   DEBUG_CMDS   yLOG_value   ("i"         , i);
   --rce;  if (i < 0) {
      DEBUG_CMDS   yLOG_note    ("command not found");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   /*> --rce; if (strcmp (s_base [i].terms, ""    ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("void type, no args");                                         <* 
    *>    rc = s_base [i].f.v   ();                                                                 <* 
    *> } else if (strcmp (s_base [i].terms, "c"   ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("one char arg");                                               <* 
    *>    rc = s_base [i].f.c   (s_fields [1][0]);                                                  <* 
    *> } else if (strcmp (s_base [i].terms, "cc"  ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("two char args");                                              <* 
    *>    rc = s_base [i].f.cc  (s_fields [1][0], s_fields [2][0]);                                 <* 
    *> } else if (strcmp (s_base [i].terms, "s"   ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("one string arg");                                             <* 
    *>    rc = s_base [i].f.s   (s_fields [1]);                                                     <* 
    *> } else if (strcmp (s_base [i].terms, "a"   ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("one long string arg");                                        <* 
    *>    rc = s_base [i].f.s   (s_all);                                                            <* 
    *> } else if (strcmp (s_base [i].terms, "ss"  ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("two string args");                                            <* 
    *>    rc = s_base [i].f.ss  (s_fields [1], s_fields [2]);                                       <* 
    *> } else if (strcmp (s_base [i].terms, "i"   ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("single integer");                                             <* 
    *>    /+> printf ("s_base [i].f.i %s %d\n", s_fields [1], atoi (s_fields [1]));       <+/       <* 
    *>    rc = s_base [i].f.i   (atoi (s_fields [1]));                                              <* 
    *> } else if (strcmp (s_base [i].terms, "ii"  ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("two integers");                                               <* 
    *>    rc = s_base [i].f.ii  (atoi (s_fields [1]), atoi (s_fields [2]));                         <* 
    *> } else if (strcmp (s_base [i].terms, "iii" ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("three integers");                                             <* 
    *>    rc = s_base [i].f.iii (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));    <* 
    *> } else if (strcmp (s_base [i].terms, "sii" ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("string and two integers");                                    <* 
    *>    rc = s_base [i].f.sii (s_fields [1], atoi (s_fields [2]), atoi (s_fields [3]));           <* 
    *> } else if (strcmp (s_base [i].terms, "isss") == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("integer and three strings");                                  <* 
    *>    rc = s_base [i].f.isss (atoi (s_fields [1]), s_fields [2], s_fields [3], s_fields [4]);   <* 
    *> } else if (strcmp (s_base [i].terms, "is"  ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("integer arg and string arg");                                 <* 
    *>    rc = s_base [i].f.is  (atoi (s_fields [1]), s_fields [1]);                                <* 
    *> } else if (strcmp (s_base [i].terms, "Cs"  ) == 0) {                                         <* 
    *>    DEBUG_CMDS   yLOG_note    ("command name and string arg");                                <* 
    *>    rc = s_base [i].f.ss  (s_fields [0], s_fields [1]);                                       <* 
    *> } else {                                                                                     <* 
    *>    DEBUG_CMDS   yLOG_note    ("crazy other shit, please update or fix");                     <* 
    *>    rc = -1;                                                                                  <* 
    *>    /+> my.key_error = 'y';                                                      <+/          <* 
    *> }                                                                                            <*/
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return rc;
}

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
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS global      : %2d %2d[%-.40s]", s_ncmd, s_len, s_current);
   }
   else if (strcmp (a_question, "counts"         )   == 0) {
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



/*====================------------------------------------====================*/
/*===----                      program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_srch_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_SEARCH)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear callbacks)----------------*/
   DEBUG_PROG   yLOG_note    ("clear callbacks");
   s_searcher = NULL;
   s_clearer  = NULL;
   /*---(clear history)------------------*/
   DEBUG_PROG   yLOG_note    ("clear all history");
   HISTORY__load  (MODE_SEARCH, NULL);
   yvikeys_hist__purge (MODE_SEARCH);
   /*---(clear current)------------------*/
   DEBUG_PROG   yLOG_note    ("initialize search results");
   s_nsrch    = MAX_SRCH;
   yvikeys_srch__purge ();
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_SEARCH);
   /*---(yparse)-------------------------*/
   rc = yPARSE_handler (MODE_SEARCH  , "search"    , 7.5, "cO----------", -1, yvikeys_srch__reader, yvikeys_srch__writer_all, "------------" , "a,search"                  , "search history"            );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_srch__purge     (void)
{
   int         i           = 0;
   for (i = 0; i < s_nsrch; ++i) {
      if (s_clearer != NULL)  s_clearer (s_srch [i].b_pos, s_srch [i].x_pos, s_srch [i].y_pos, s_srch [i].z_pos);
      s_srch [i].pass   = -1;
      strlcpy (s_srch [i].label, "-", LEN_LABEL);
      s_srch [i].b_pos  =  0;
      s_srch [i].x_pos  =  0;
      s_srch [i].y_pos  =  0;
      s_srch [i].z_pos  =  0;
   }
   s_nsrch = 0;
   s_csrch = 0;
   return 0;
}

char
yVIKEYS_srch_config     (void *a_searcher, void *a_clearer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SRCH  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (MODE_SEARCH)) {
      DEBUG_SRCH   yLOG_note    ("init must complete before config");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update searcher)----------------*/
   DEBUG_SRCH   yLOG_point   ("searcher"  , a_searcher);
   --rce;  if (a_searcher == NULL) {
      DEBUG_SRCH   yLOG_note    ("without searcher callback, search can not function");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_searcher = a_searcher;
   /*---(update clearer)-----------------*/
   DEBUG_SRCH   yLOG_point   ("clearer"   , a_clearer);
   --rce;  if (a_clearer == NULL) {
      DEBUG_SRCH   yLOG_note    ("without clearer callback, search can not function");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_clearer  = a_clearer;
   /*---(update status)------------------*/
   STATUS_conf_set   (MODE_SEARCH, '1');
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_srch_found   (char *a_label, int a_buf, int x, int y, int z)
{
   /*> s_srch [s_nsrch].pass   = s_npass - 1;                                         <* 
    *> if (a_label != NULL)  strlcpy (s_srch [s_nsrch].label, a_label, LEN_LABEL);    <* 
    *> s_srch [s_nsrch].b_pos = a_buf;                                                <* 
    *> s_srch [s_nsrch].x_pos = x;                                                    <* 
    *> s_srch [s_nsrch].y_pos = y;                                                    <* 
    *> s_srch [s_nsrch].z_pos = z;                                                    <* 
    *> ++s_nsrch;                                                                     <* 
    *> ++s_passes [s_npass - 1].found;                                                <*/
   return 0;
}

char
yVIKEYS_srch_direct     (char *a_text)
{
   char        rc          =    0;
   rc = HISTORY__load (MODE_SEARCH, a_text);
   if (rc < 0)  return -1;
   rc = HISTORY__exec (MODE_SEARCH);
   return rc;
}

char
yvikeys_srch_next       (char a_move)
{
   char        rce         =  -10;
   char        x_seq       = s_csrch;
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_char    ("a_move"    , a_move);
   DEBUG_SRCH   yLOG_value   ("nsrch"     , s_nsrch);
   DEBUG_SRCH   yLOG_value   ("csrch"     , s_csrch);
   DEBUG_SRCH   yLOG_value   ("x_seq"     , x_seq);
   --rce;  if (s_nsrch <= 0) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  switch (a_move) {
   case '[' :  x_seq = 0;            break;
   case '<' :  --x_seq;              break;
   case 'N' :  --x_seq;              break;
   case 'n' :  ++x_seq;              break;
   case '>' :  ++x_seq;              break;
   case ']' :  x_seq = s_nsrch - 1;  break;
   default  :
               DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
   }
   DEBUG_SRCH   yLOG_value   ("x_seq"     , x_seq);
   --rce;  if (x_seq >= s_nsrch) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_seq <  0      ) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_value   ("x_seq"     , x_seq);
   s_csrch = x_seq;
   yVIKEYS_jump (s_srch [s_csrch].b_pos, s_srch [s_csrch].x_pos, s_srch [s_csrch].y_pos, s_srch [s_csrch].z_pos);
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}




char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
yvikeys_srch_list       (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", LEN_RECD);
   for (i = 0; i < s_nsrch; ++i) {
      if (i > 10) break;
      sprintf    (x_entry, "%s,", s_srch [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       helpers                                ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_UTIL_______o () { return; }



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_PROG_______o () { return; }



/*====================------------------------------------====================*/
/*===----                        running a command                     ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_RUN________o () { return; }



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
SRCH__unit_searcher     (char *a_search)
{
   if (a_search == NULL)  return 0;
   if      (strcmp ("/1st", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /1st");
      yVIKEYS_srch_found ("0k11"     ,   0,  10,  10,   0);
      yVIKEYS_srch_found ("0p12"     ,   0,  15,  11,   0);
      yVIKEYS_srch_found ("3d6"      ,   3,   3,   5,   0);
   }
   else if (strcmp ("/2nd", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /2nd");
      yVIKEYS_srch_found ("0k11"     ,   0,  10,  10,   0);
      yVIKEYS_srch_found ("0b3"      ,   0,   1,   2,   0);
      yVIKEYS_srch_found ("2b5"      ,   2,   1,   4,   0);
      yVIKEYS_srch_found ("Za1"      ,  35,   0,   0,   0);
   }
   else if (strcmp ("/another", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /another");
      yVIKEYS_srch_found ("2b5"      ,   2,   1,   4,   0);
   } else {
      DEBUG_SRCH   yLOG_note    ("nothing found");
   }
   return 0;
}

char
SRCH__unit_clearer      (char *a_junk)
{
   return 0;
}

char
SRCH__unit_null         (void)
{
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
SRCH__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRCH unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH global      : %2d[%-.40s]", s_len, s_current);
   }
   /*> else if (strcmp (a_question, "history"        )   == 0) {                                                                                                                                                   <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH history     : %2d %c %2d %2d  %-.40s", a_index, s_passes [a_index].mark, s_passes [a_index].count, s_passes [a_index].found, s_passes [a_index].text);   <* 
    *> }                                                                                                                                                                                                           <*/
   else if (strcmp (a_question, "results"        )   == 0) {
      yvikeys_srch_list (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH results     : %2d %-.40s", s_nsrch, t);
   }
   else if (strcmp (a_question, "oneline"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH oneline     : %2dn, %2dc, %2dp, %-10.10s, %3db, %3dx, %3dy, %3dz", s_nsrch, a_index, s_srch [a_index].pass, s_srch [a_index].label,  s_srch [a_index].b_pos,  s_srch [a_index].x_pos, s_srch [a_index].y_pos, s_srch [a_index].z_pos);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


