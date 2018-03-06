/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char s_srch_status   = G_STAGE_NULL;
static char s_cmds_status   = G_STAGE_NULL;


static char *S_HIST_LIST = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*===[[ MENUS ]]==============================================================*/
#define    MAX_MENU        20
typedef    struct   cMENU   tMENU;
struct cMENU {
   char        abbr;
   char        name        [LEN_LABEL];
   char        desc        [LEN_DESC ];
   int         count;
};
static tMENU  s_menus [MAX_MENU] = {
   { 'f', "file"      , "files, folders, printing, and exporting"      , 0},
   { 'e', "edit"      , "cut, copy, paste, find, clear, etc"           , 0},
   { 's', "select"    , ""                                             , 0},
   { 'v', "view"      , "display and layout of application elements"   , 0},
   { 'i', "insert"    , "add space, symbols, object, image, etc"       , 0},
   { 'o', "object"    , "resize, change, shape, move, grouping"        , 0},
   { 'f', "format"    , "themes, styles, formatting, data format"      , 0},
   { 'p', "pixel"     , "painting kind of actions"                     , 0},
   { 'x', "draw"      , "drawing/drafting kind of actions"             , 0},
   { 'd', "data"      , ""                                             , 0},
   { 'l', "layers"    , ""                                             , 0},
   { 'w', "layout"    , ""                                             , 0},
   { 't', "tools"     , "spelling, error checks, macros, controls"     , 0},
   { 'b', "buffers"   , ""                                             , 0},
   { 'a', "audit"     , ""                                             , 0},
   { 'c', "config"    , ""                                             , 0},
   { ' ', ""          , ""                                             , 0},
};
static  int s_nmenu  = 0;
#define     MENU_FILE       'f'
#define     MENU_EDIT       'e'
#define     MENU_SELECT     's'
#define     MENU_VIEW       'v'
#define     MENU_INSERT     'i'
#define     MENU_OBJECT     'o'
#define     MENU_FORMAT     'f'
#define     MENU_PIXEL      'p'
#define     MENU_DRAW       'x'
#define     MENU_DATA       'd'
#define     MENU_LAYERS     'l'
#define     MENU_LAYOUT     'w'
#define     MENU_TOOLS      't'
#define     MENU_BUFFER     'b'
#define     MENU_AUDIT      'a'



/*===[[ TERMS ]]==============================================================*/
#define    MAX_TERMS       50
typedef    struct   cTERMS  tTERMS;
struct cTERMS {
   char        name        [LEN_LABEL];
   char        disp        [LEN_DESC ];
   int         count;
};
static tTERMS  s_terms [MAX_TERMS] = {
   { ""     , "void"                       , 0},
   { "v"    , "void"                       , 0},
   { "c"    , "char"                       , 0},
   { "cc"   , "char, char"                 , 0},
   { "i"    , "int"                        , 0},
   { "is"   , "int, char*"                 , 0},
   { "s"    , "char*"                      , 0},
   { "ss"   , "char*, char*"               , 0},
   { "si"   , "char*, int"                 , 0},
   { "a"    , "char*"                      , 0},  /* string incudes spaces            */
   { "ii"   , "int, int"                   , 0},
   { "iii"  , "int, int, int"              , 0},
   { "isss" , "int, char*, char*, char*"   , 0},
   { "Cs"   , "char*, char*"               , 0},
   { "-"    , ""                           , 0},
};
static  int s_nterm  = 0;




/*===[[ COMMANDS ]]===========================================================*/
#define     LEN_COMMAND    2000
static char   s_command    [LEN_COMMAND];
static int    s_clen       = 0;

#define  MAX_CMDS      1000
typedef  struct cCOMMAND  tCOMMAND;
struct  cCOMMAND {
   char        menu;                        /* category                       */
   char        name        [LEN_LABEL];     /* full command name              */
   char        len;                         /* length of name                 */
   char        abbr        [LEN_LABEL];     /* abbreviation of name           */
   char        alen;                        /* length of abbreviation         */
   char        active;                      /* ready to use                   */
   char        redraw;                      /* redraw afterwards              */
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
   } f;
   char        terms       [LEN_LABEL];     /* type of terms/args             */
   char        nterm;                       /* number of terms/args           */
   char        desc        [LEN_DESC];      /* descriptive label              */
   char        disp        [LEN_DESC];      /* display version of command     */
};
static tCOMMAND  s_cmds      [MAX_CMDS];
static int       s_ncmd      = 0;
static char      s_fields    [10][LEN_COMMAND];
static int       s_nfield    =  0;
static char      s_all       [LEN_COMMAND]       = "";



/*===[[ SEARCH ]]=============================================================*/
#define     LEN_SEARCH     2000
static char s_search     [LEN_SEARCH];
static int  s_slen       = 0;

#define   MAX_PASS     200
typedef struct cPASSES   tPASSES;
struct cPASSES {
   char        mark;
   char        search       [LEN_SEARCH];
   int         found;
};
static tPASSES   s_passes    [MAX_PASS];
static int       s_npass     = 0;
static int       s_cpass     = 0;


#define   MAX_SRCH    2000
typedef   struct   cSRCH   tSRCH;
struct cSRCH {
   int         pass;
   char        label       [LEN_LABEL];
   int         x_pos;
   int         y_pos;
   int         z_pos;
};
static tSRCH     s_srch      [MAX_SRCH];
static int       s_nsrch     = 0;
static int       s_csrch     = 0;

char    (*s_searcher) (char *a_search);
char    (*s_clearer ) (int a_x, int a_y, int a_z);



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

char
SRCH_limits             (int *a_min, int *a_max)
{
   if (a_min != NULL)  *a_min = 0;
   if (a_max != NULL)  *a_max = s_npass;
   return 0;
}

char
SRCH_entry              (int a_index, char *a_entry, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_len       =    0;
   int         x_found     =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)  return rce;
   /*---(blank line)---------------------*/
   if (a_index < 0 || a_index >= s_npass) {
      sprintf (a_entry, "%-*.*s", a_max, a_max, " ");
      return 0;
   }
   /*---(create)-------------------------*/
   x_len = a_max - 8;
   x_found = s_passes [a_index].found;
   if (x_found > 99)  x_found = 99;
   sprintf (a_entry, " %-3d  %2d  %c  %-*.*s ",
         a_index, x_found, s_passes [a_index].mark,
         x_len, x_len, s_passes [a_index].search);
   /*---(complete)-----------------------*/
   return 0;
}

char*
SRCH_use                (int a_index)
{
   return s_passes [a_index].search;
}



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

int  
SRCH_valid           (char a_mark)
{
   char        rce         =  -10;
   char        x_mark      =  ' ';
   int         i           =    0;
   int         n           = s_npass;
   --rce;  if (a_mark == '\0')                        return rce;
   --rce;  if (strchr (S_HIST_LIST, a_mark) == NULL)  return rce;
   for (i = 0; i < s_npass; ++i) {
      if (s_passes [i].mark != a_mark)  continue;
      n = i;
      break;
   }
   return n;
}

char
SRCH__clear          (void)
{
   strlcpy     (s_search , "" , LEN_RECD);
   s_slen = 0;
   return 0;
}

char CMDS__quit              (void) { myVIKEYS.done = 'y'; return 0; }
char CMDS__writequit         (void) { myVIKEYS.done = 'y'; return 0; }

int
CMDS__find           (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   int         x_len       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_name  == NULL)                         return rce;
   x_len  = strllen (a_name, LEN_LABEL);
   --rce;  if (x_len   <  1)                            return rce;
   /*---(find)---------------------------*/
   for (i = 0; i < s_ncmd; ++i) {
      /*---(check abbr)------------------*/
      if (s_cmds [i].alen == x_len) {
         if (s_cmds [i].abbr [0] == a_name [0]) {
            if (strcmp (s_cmds [i].abbr, a_name) == 0)  return i;
         }
      }
      /*---(check name)------------------*/
      if (s_cmds [i].len  == x_len) {
         if (s_cmds [i].name [0] == a_name [0]) {
            if (strcmp (s_cmds [i].name, a_name) == 0)  return i;
         }
      }
   }
   /*---(complete)-----------------------*/
   return -1;
}



/*====================------------------------------------====================*/
/*===----                      program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
SRCH_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_value   ("stage"     , s_srch_status);
   --rce;  if (s_srch_status != G_STAGE_NULL) {
      DEBUG_PROG   yLOG_note    ("init called too late");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(clear callbacks)----------------*/
   DEBUG_PROG   yLOG_note    ("clear callbacks");
   s_searcher = NULL;
   s_clearer  = NULL;
   /*---(clear current)------------------*/
   DEBUG_PROG   yLOG_note    ("initialize search results");
   s_nsrch    = MAX_SRCH;
   SRCH__clear  ();
   SRCH__purge ();
   /*---(clear history)------------------*/
   for (i = 0; i < MAX_PASS; ++i) {
      s_passes [i].mark  = '-';
      strlcpy (s_passes [i].search, "-", LEN_RECD);
      s_passes [i].found = 0;
   }
   /*---(update stage)-------------------*/
   s_srch_status = G_STAGE_INIT;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SRCH__purge             (void)
{
   int         i           = 0;
   for (i = 0; i < s_nsrch; ++i) {
      if (s_clearer != NULL)  s_clearer (s_srch [i].x_pos, s_srch [i].y_pos, s_srch [i].z_pos);
      s_srch [i].pass   = -1;
      strlcpy (s_srch [i].label, "-", LEN_LABEL);
      s_srch [i].x_pos  =  0;
      s_srch [i].y_pos  =  0;
      s_srch [i].z_pos  =  0;
   }
   s_nsrch = 0;
   s_csrch = 0;
   return 0;
}

char
CMDS_init               (void)
{
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_value   ("stage"     , s_cmds_status);
   --rce;  if (s_cmds_status != G_STAGE_NULL) {
      DEBUG_PROG   yLOG_note    ("init called too late");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(menus)--------------------------*/
   DEBUG_PROG   yLOG_note    ("initialize menu system");
   s_nmenu = 0;
   for (i = 0; i < MAX_MENU; ++i) {
      if (s_menus [i].name [0] == NULL)  break;
      s_menus [i].count = 0;
      ++s_nmenu;
   }
   /*---(terms)--------------------------*/
   DEBUG_PROG   yLOG_note    ("initialize term system");
   s_nterm = 0;
   for (i = 0; i < MAX_TERMS; ++i) {
      if (s_terms [i].name [0] == '-')  break;
      s_terms [i].count = 0;
      ++s_nterm;
   }
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("initialize command system");
   s_ncmd = 0;
   for (i = 0; i < MAX_CMDS; ++i) {
      s_cmds [i].menu      = '-';
      s_cmds [i].name  [0] = NULL;
      s_cmds [i].len       = 0;
      s_cmds [i].abbr  [0] = NULL;
      s_cmds [i].alen      = 0;
      s_cmds [i].active    = '-';
      s_cmds [i].redraw    = '-';
      s_cmds [i].f.v       = NULL;
      s_cmds [i].terms [0] = 0;
      s_cmds [i].nterm     = 0;
      s_cmds [i].desc  [0] = 0;
      s_cmds [i].disp  [0] = 0;
   }
   /*---(update stage)-------------------*/
   s_cmds_status = G_STAGE_READY;
   DEBUG_PROG   yLOG_value   ("stage"     , s_cmds_status);
   /*---(other)--------------------------*/
   DEBUG_PROG   yLOG_note    ("add universal commands");
   myVIKEYS.done = '-';
   yVIKEYS_cmds_add ('f', "quit"        , "q"   , ""     , CMDS__quit           , "quit current file (if no changes), exit if the only file"    );
   yVIKEYS_cmds_add ('f', "quitall"     , "qa"  , ""     , CMDS__quit           , "quit all files (if no changes), and exit"                    );
   yVIKEYS_cmds_add ('f', "writequit"   , "wq"  , ""     , CMDS__writequit      , ""                                                            );
   yVIKEYS_cmds_add ('f', "writequitall", "wqa" , ""     , CMDS__writequit      , ""                                                            );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}




char
yVIKEYS_srch_config     (void *a_searcher, void *a_clearer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_value   ("stage"     , s_srch_status);
   --rce;  if (s_srch_status <  G_STAGE_INIT) {
      DEBUG_PROG   yLOG_note    ("must be called after init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(update searcher)----------------*/
   DEBUG_PROG   yLOG_point   ("searcher"  , a_searcher);
   --rce;  if (a_searcher == NULL) {
      DEBUG_PROG   yLOG_note    ("without searcher callback, search can not function");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_searcher = a_searcher;
   /*---(update clearer)-----------------*/
   DEBUG_PROG   yLOG_point   ("clearer"   , a_clearer);
   --rce;  if (a_clearer == NULL) {
      DEBUG_PROG   yLOG_note    ("without clearer callback, search can not function");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_clearer  = a_clearer;
   /*---(update stage)-------------------*/
   s_srch_status = G_STAGE_READY;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*
SRCH_curr            (void)
{
   return s_search;
}

char
yVIKEYS_srch_found   (char *a_label, int x, int y, int z)
{
   s_srch [s_nsrch].pass   = s_npass - 1;
   if (a_label != NULL)  strlcpy (s_srch [s_nsrch].label, a_label, LEN_LABEL);
   s_srch [s_nsrch].x_pos = x;
   s_srch [s_nsrch].y_pos = y;
   s_srch [s_nsrch].z_pos = z;
   ++s_nsrch;
   ++s_passes [s_cpass].found;
   return 0;
}

char
SRCH__mark           (char a_mark)
{
   int         i           =    0;
   for (i = 0; i < s_npass; ++i) {
      if (s_passes [i].mark != a_mark)  continue;
      s_passes [i].mark = '-';
   }
   s_passes [s_cpass].mark = a_mark;
   return 0;
}

char
SRCH__return         (char a_mark)
{
   int         i           =    0;
   int         n           =   -1;
   for (i = 0; i < s_npass; ++i) {
      if (s_passes [i].mark != a_mark)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   s_cpass = n;
   return 0;
}

char
SRCH__roll           (int a_index)
{
   int         rce         =  -10;
   int         i           =    0;
   --rce;  if (a_index < 0)            return rce;
   strlcpy (s_search, s_passes [a_index].search, LEN_RECD);
   for (i = a_index; i < s_npass - 1; ++i) {
      strlcpy (s_passes [i].search, s_passes [i + 1].search, LEN_RECD);
      s_passes [i].found = s_passes [i + 1].found;
   }
   s_cpass = s_npass - 1;
   strlcpy (s_passes [s_cpass].search, s_search, LEN_RECD);
   s_passes [s_cpass].found = 0;
   return 0;
}

char
SRCH__exec           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   /*---(defense)------------------------*/
   if (s_srch_status < G_STAGE_READY)              return -66;
   /*---(mark, no clear)-----------------*/
   if (s_slen == 2 && s_search [0] == 'm') {
      SRCH__mark (s_search [1]);
      return 0;
   }
   /*---(clear results)------------------*/
   SRCH__purge  ();
   /*---(handle simple)------------------*/
   if (s_srch_status < G_STAGE_READY)              return -66;
   if (s_slen <= 0)                                return 0;
   if (s_slen == 1 && strcmp (s_search, "/") == 0) return 0;
   /*---(repeat last)--------------------*/
   else if (s_slen == 2 && strcmp (s_search, "//") == 0) {
      if (s_npass == 0) return -1;
      s_cpass = s_npass - 1;
      strlcpy (s_search, s_passes [s_cpass].search, LEN_RECD);
   }
   /*---(repeat complex)-----------------*/
   else if (s_slen > 2 && strncmp (s_search, "//", 2) == 0) {
      /*---(repeat marked)---------------*/
      if (s_slen == 3 && strchr (S_HIST_LIST, s_search [2]) != NULL) {
         rc = SRCH__return (s_search [2]);
         if (rc < 0)  return rc;
      }
      /*---(repeat by reference)---------*/
      else {
         s_cpass = s_npass - atoi (s_search + 2) - 1;
         if (s_cpass < 0) s_cpass = 0;
      }
      /*---(copy to current)-------------*/
      strlcpy (s_search, s_passes [s_cpass].search, LEN_RECD);
   }
   /*---(normal)-------------------------*/
   else {
      strlcpy (s_passes [s_npass].search, s_search, LEN_RECD);
      s_cpass = s_npass;
      ++s_npass;
   }
   /*---(make current)-------------------*/
   s_passes [s_cpass].found = 0;
   /*---(execute)------------------------*/
   rc = s_searcher (s_search);
   /*---(complete)-----------------------*/
   return 0;
}

char
SRCH__load              (char *a_search)
{
   SRCH__clear ();
   if (a_search != NULL) {
      strlcpy (s_search, a_search, LEN_SEARCH);
      s_slen = strllen (s_search, LEN_SEARCH);
   }
   return 0;
}

char
yVIKEYS_srch_direct     (char *a_search)
{
   SRCH__load (a_search);
   return SRCH__exec ();
}

char
SRCH_next               (char a_move)
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
   MAP_jump (s_srch [s_csrch].x_pos, s_srch [s_csrch].y_pos, s_srch [s_csrch].z_pos);
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}


char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
SRCH_writer           (int n, int *a, int *b, int *c, int *d, int *e, int *f, int *g, int *h, int *i)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =    0;
   if (n >= strllen (S_HIST_LIST, LEN_DESC))   return -10;
   x_index = SRCH_valid (S_HIST_LIST [n]);
   if (s_passes [x_index].mark == '-') return 0;
   if (x_index == s_npass)             return 0;
   if (x_index >  s_npass)             return 0;
   /*> endwin ();                                                                                                      <* 
    *> printf ("visu   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", a, b, c, d, e, f, g, h, i);            <* 
    *> printf (" val   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", *a, *b, *c, *d, *e, *f, *g, *h, *i);   <*/
   *a = &s_passes [n].mark;
   *b = &s_passes [n].search;
   /*> printf (" val   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", *a, *b, *c, *d, *e, *f, *g, *h, *i);   <*/
   return 1;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
SRCH_reader           (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_index     =    0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_SRCH   yLOG_char    ("version"   , n);
   --rce;  if (n != 'A') {
      DEBUG_SRCH   yLOG_note    ("illegal version");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_SRCH   yLOG_value   ("mark"      , a[0]);
   --rce;  if (strchr (S_HIST_LIST, a[0]) == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_char    ("mark"      , a[0]);
   /*---(search)-------------------------*/
   DEBUG_SRCH   yLOG_point   ("search"    , b);
   --rce;  if (b == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strlcpy (s_passes [s_npass].search, b, LEN_RECD);
   s_passes [s_npass].mark = a[0];
   ++s_npass;
   s_cpass = s_npass - 1;
   /*---(complete)-----------------------*/
   DEBUG_SRCH  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
SRCH_list               (char *a_list)
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

char
CMDS__menu           (char a_menu, char a_action)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   for (i = 0; i < s_nmenu; ++i) {
      if (a_menu != s_menus [i].abbr)   continue;
      switch (a_action) {
      case '+' :
         ++(s_menus [i].count);
         break;
      case '-' :
         if (s_menus [i].count < 1)  return -1;
         --(s_menus [i].count);
         break;
      }
      return 0;
   }
   return -3;
}

char
CMDS__terms          (char *a_terms, char a_action)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   if (a_terms == NULL)  return -2;
   for (i = 0; i < s_nterm; ++i) {
      if     (strcmp (a_terms, s_terms [i].name) != 0)   continue;
      /*> printf ("term %s index = %d, count %d\n", a_terms, i, s_terms [i].count);   <*/
      switch (a_action) {
      case '+' :
         ++(s_terms [i].count);
         break;
      case '-' :
         if (s_terms [i].count < 1)  return -1;
         --(s_terms [i].count);
         break;
      }
      return 0;
   }
   return -3;
}

char
yVIKEYS_cmds_add     (char a_menu, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_dup       =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_value   ("stage"     , s_cmds_status);
   --rce;  if (s_cmds_status <  G_STAGE_INIT) {
      DEBUG_PROG   yLOG_note    ("must be called after init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(defense)------------------------*/
   --rce;  if (CMDS__menu  (a_menu, ACTION_FIND) < 0)         return rce;
   --rce;  if (a_name  == NULL)                               return rce;
   --rce;  if (strllen (a_name, LEN_LABEL) <  1)              return rce;
   --rce;  if (a_abbr  == NULL)                               return rce;
   --rce;  if (strllen (a_abbr, LEN_LABEL) >  4)              return rce;
   --rce;  if (a_terms == NULL)                               return rce;
   --rce;  if (CMDS__terms (a_terms, ACTION_FIND) < 0)        return rce;
   --rce;  if (a_func  == NULL)                               return rce;
   /*---(check for dup)------------------*/
   x_dup = CMDS__find (a_name);
   --rce;  if (x_dup >= 0)                                    return rce;
   x_dup = CMDS__find (a_abbr);
   --rce;  if (x_dup >= 0)                                    return rce;
   /*---(add data)-----------------------*/
   s_cmds [s_ncmd].menu  = a_menu;
   strlcpy (s_cmds [s_ncmd].name , a_name , LEN_LABEL);
   s_cmds [s_ncmd].len    = strllen (s_cmds [s_ncmd].name , LEN_LABEL);
   strlcpy (s_cmds [s_ncmd].abbr , a_abbr , LEN_LABEL);
   s_cmds [s_ncmd].alen   = strllen (s_cmds [s_ncmd].abbr , LEN_LABEL);
   strlcpy (s_cmds [s_ncmd].terms, a_terms, LEN_LABEL);
   s_cmds [s_ncmd].nterm  = strllen (s_cmds [s_ncmd].terms, LEN_LABEL);
   s_cmds [s_ncmd].active = 'y';
   strlcpy (s_cmds [s_ncmd].desc , a_desc , LEN_DESC );
   /*---(assign function pointer)--------*/
   switch (a_terms [0]) {
   case   0  :
      s_cmds [s_ncmd].f.v   = a_func; break;
   case  'c' :
      if      (strcmp (a_terms, "c"    ) == 0)  s_cmds [s_ncmd].f.c    = a_func;
      else if (strcmp (a_terms, "cc"   ) == 0)  s_cmds [s_ncmd].f.cc   = a_func;
   case  'i' :
      if      (strcmp (a_terms, "i"    ) == 0)  s_cmds [s_ncmd].f.i    = a_func;
      else if (strcmp (a_terms, "ii"   ) == 0)  s_cmds [s_ncmd].f.ii   = a_func;
      else if (strcmp (a_terms, "iii"  ) == 0)  s_cmds [s_ncmd].f.iii  = a_func;
      else if (strcmp (a_terms, "is"   ) == 0)  s_cmds [s_ncmd].f.is   = a_func;
      else if (strcmp (a_terms, "isss" ) == 0)  s_cmds [s_ncmd].f.isss = a_func;
   case  's' :
      if      (strcmp (a_terms, "s"    ) == 0)  s_cmds [s_ncmd].f.s    = a_func;
      else if (strcmp (a_terms, "si"   ) == 0)  s_cmds [s_ncmd].f.si   = a_func;
      else if (strcmp (a_terms, "ss"   ) == 0)  s_cmds [s_ncmd].f.ss   = a_func;
   case  'C' :
      if      (strcmp (a_terms, "Cs"   ) == 0)  s_cmds [s_ncmd].f.ss   = a_func;
   }
   /*---(update count)-------------------*/
   ++s_ncmd;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       entering a command                     ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_ENTRY______o () { return; }

char
CMDS_start              (void)
{
   strlcpy     (s_command , ":", LEN_COMMAND);
   s_clen = 1;
   return 0;
}

char
CMDS__clear             (void)
{
   strlcpy     (s_command , "" , LEN_COMMAND);
   s_clen = 0;
   return 0;
}

char*
CMDS_curr               (void)
{
   return s_command;
}



/*====================------------------------------------====================*/
/*===----                        running a command                     ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_RUN________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
CMDS__parse           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   char       *q           = " ";
   char       *r           = NULL;
   char        x_work      [LEN_COMMAND]   = "";
   int         x_len       = 0;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   strlcpy (x_work, s_command, LEN_COMMAND);
   x_len = strllen (x_work, LEN_COMMAND);
   for (i = 0; i < x_len; ++i) {
      if ((uchar) x_work [i] == G_CHAR_SPACE)   x_work [i] = G_KEY_SPACE;
   }
   DEBUG_USER   yLOG_info    ("x_work"    , x_work);
   /*---(parse command)------------------*/
   p     = strtok_r (x_work, q, &r);
   ++p;
   x_len = strllen (p, LEN_COMMAND);
   DEBUG_USER   yLOG_info    ("g_cmd"     , p);
   if (strllen (x_work, LEN_COMMAND) > x_len)  strlcpy (s_all, p + x_len + 1, LEN_COMMAND);
   DEBUG_USER   yLOG_info    ("s_all"     , s_all);
   /*---(parse)--------------------------*/
   for (i = 0; i < 10; ++i)  strlcpy (s_fields [i], "", LEN_COMMAND);
   for (i = 0; i < 10; ++i) {
      DEBUG_USER   yLOG_value   ("i"         , i);
      DEBUG_USER   yLOG_info    ("p"         , p);
      strlcpy (s_fields [i], p, LEN_COMMAND);
      s_nfield = i + 1;
      p = strtok_r (NULL  , q, &r);
      if (p == NULL)  break;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
CMDS__exec            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_value   ("stage"     , s_cmds_status);
   --rce;  if (s_cmds_status <  G_STAGE_READY) {
      DEBUG_PROG   yLOG_note    ("must be called after init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   DEBUG_USER   yLOG_info    ("s_command" , s_command);
   /*---(look for system)---------------*/
   if (s_command [1] == '!') {
      DEBUG_USER   yLOG_note    ("this is a direct run");
      rc = system (s_command + 2);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)-------------------------*/
   rc = CMDS__parse ();
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_USER   yLOG_note    ("could not parse");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find)---------------------------*/
   i = CMDS__find (s_fields [0]);
   DEBUG_USER   yLOG_value   ("i"         , i);
   --rce;  if (i < 0) {
      DEBUG_USER   yLOG_note    ("command not found");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   --rce; if (strcmp (s_cmds [i].terms, ""    ) == 0) {
      DEBUG_USER   yLOG_note    ("void type, no args");
      rc = s_cmds [i].f.v   ();
   } else if (strcmp (s_cmds [i].terms, "c"   ) == 0) {
      DEBUG_USER   yLOG_note    ("one char arg");
      rc = s_cmds [i].f.c   (s_fields [1][0]);
   } else if (strcmp (s_cmds [i].terms, "cc"  ) == 0) {
      DEBUG_USER   yLOG_note    ("two char args");
      rc = s_cmds [i].f.cc  (s_fields [1][0], s_fields [2][0]);
   } else if (strcmp (s_cmds [i].terms, "s"   ) == 0) {
      DEBUG_USER   yLOG_note    ("one string arg");
      rc = s_cmds [i].f.s   (s_fields [1]);
   } else if (strcmp (s_cmds [i].terms, "a"   ) == 0) {
      DEBUG_USER   yLOG_note    ("one long string arg");
      rc = s_cmds [i].f.s   (s_all);
   } else if (strcmp (s_cmds [i].terms, "ss"  ) == 0) {
      DEBUG_USER   yLOG_note    ("two string args");
      rc = s_cmds [i].f.ss  (s_fields [1], s_fields [2]);
   } else if (strcmp (s_cmds [i].terms, "i"   ) == 0) {
      DEBUG_USER   yLOG_note    ("single integer");
      /*> printf ("s_cmds [i].f.i %s %d\n", s_fields [1], atoi (s_fields [1]));       <*/
      rc = s_cmds [i].f.i   (atoi (s_fields [1]));
   } else if (strcmp (s_cmds [i].terms, "ii"  ) == 0) {
      DEBUG_USER   yLOG_note    ("two integers");
      rc = s_cmds [i].f.ii  (atoi (s_fields [1]), atoi (s_fields [2]));
   } else if (strcmp (s_cmds [i].terms, "iii" ) == 0) {
      DEBUG_USER   yLOG_note    ("three integers");
      rc = s_cmds [i].f.iii (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));
   } else if (strcmp (s_cmds [i].terms, "isss") == 0) {
      DEBUG_USER   yLOG_note    ("integer and three strings");
      rc = s_cmds [i].f.isss (atoi (s_fields [1]), s_fields [2], s_fields [3], s_fields [4]);
   } else if (strcmp (s_cmds [i].terms, "is"  ) == 0) {
      DEBUG_USER   yLOG_note    ("integer arg and string arg");
      rc = s_cmds [i].f.is  (atoi (s_fields [1]), s_fields [1]);
   } else if (strcmp (s_cmds [i].terms, "Cs"  ) == 0) {
      DEBUG_USER   yLOG_note    ("command name and string arg");
      rc = s_cmds [i].f.ss  (s_fields [0], s_fields [1]);
   } else {
      DEBUG_USER   yLOG_note    ("crazy other shit, please update or fix");
      rc = -1;
      /*> my.key_error = 'y';                                                      <*/
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return rc;
}

char
CMDS__load              (char *a_command)
{
   CMDS__clear ();
   if (a_command != NULL) {
      strlcpy (s_command , a_command , LEN_COMMAND);
      s_clen = strllen (s_command, LEN_COMMAND);
   }
   return 0;
}

char
yVIKEYS_cmds_direct     (char *a_command)
{
   CMDS__load (a_command);
   return CMDS__exec ();
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
CMDS__test              (char a_mode, char a_value)
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

char
CMDS__unit_null         (void)
{
   s_cmds_status  = G_STAGE_NULL;
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CMDS__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "CMDS unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS global      : %2d %2d[%-.40s]", s_ncmd, s_clen, s_command);
   }
   else if (strcmp (a_question, "menu_count"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS menu count  : %d", s_nmenu);
   }
   else if (strcmp (a_question, "menu"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS menu    (%2d): %c %-12.12s %d", a_index, s_menus [a_index].abbr, s_menus [a_index].name, s_menus [a_index].count);
   }
   else if (strcmp (a_question, "term_count"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS term count  : %d", s_nterm);
   }
   else if (strcmp (a_question, "term"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS term    (%2d): %-5.5s %-30.30s %d", a_index, s_terms [a_index].name, s_terms [a_index].disp, s_terms [a_index].count);
   }
   else if (strcmp (a_question, "command"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS command (%2d): %c %-12.12s %-4.4s %-4.4s %3s", a_index, s_cmds [a_index].menu, s_cmds [a_index].name, s_cmds [a_index].abbr, s_cmds [a_index].terms, (s_cmds [a_index].f.v == NULL) ? "---" : "SET");
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      switch (s_cmds_status) {
      case -1 : strlcpy (t, "null (not even initialized)", LEN_DESC);    break;
      case  0 : strlcpy (t, "initialized only"           , LEN_DESC);    break;
      case  5 : strlcpy (t, "configured and ready"       , LEN_DESC);    break;
      }
      snprintf (yVIKEYS__unit_answer, LEN_STR, "CMDS status      : stage %2d, %s", s_cmds_status, t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char
SRCH__unit_searcher     (char *a_search)
{
   if (a_search == NULL)  return 0;
   if      (strcmp ("/1st", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /1st");
      yVIKEYS_srch_found ("0k11"     ,  10,  10,   0);
      yVIKEYS_srch_found ("0p12"     ,  15,  11,   0);
      yVIKEYS_srch_found ("3d6"      ,   3,   5,   4);
   }
   else if (strcmp ("/2nd", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /2nd");
      yVIKEYS_srch_found ("0k11"     ,  10,  10,   0);
      yVIKEYS_srch_found ("0b3"      ,   1,   2,   0);
      yVIKEYS_srch_found ("2b5"      ,   1,   4,   3);
      yVIKEYS_srch_found ("Za1"      ,   0,   0,  36);
   }
   else if (strcmp ("/another", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /another");
      yVIKEYS_srch_found ("2b5"      ,   1,   4,   3);
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
   s_srch_status  = G_STAGE_NULL;
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
SRCH__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRCH unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH global      : %2d[%-.40s]", s_slen, s_search);
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      switch (s_srch_status) {
      case -1 : strlcpy (t, "null (not even initialized)", LEN_DESC);    break;
      case  0 : strlcpy (t, "initialized only"           , LEN_DESC);    break;
      case  5 : strlcpy (t, "configured and ready"       , LEN_DESC);    break;
      }
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH status      : stage %2d, %s", s_srch_status, t);
   }
   else if (strcmp (a_question, "history"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH history     : %2d %c %2d  %-.40s", a_index, s_passes [a_index].mark, s_passes [a_index].found, s_passes [a_index].search);
   }
   else if (strcmp (a_question, "results"        )   == 0) {
      SRCH_list (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH results     : %2d %2d %-.40s", s_cpass, s_nsrch, t);
   }
   else if (strcmp (a_question, "oneline"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH oneline     : %2dn, %2dc, %2dp, %-10.10s, %3dx, %3dy, %3dz", s_nsrch, a_index, s_srch [a_index].pass, s_srch [a_index].label,  s_srch [a_index].x_pos, s_srch [a_index].y_pos, s_srch [a_index].z_pos);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




