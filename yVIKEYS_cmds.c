/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



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
   { ""    , "void"                , 0},
   { "v"   , "void"                , 0},
   { "c"   , "char"                , 0},
   { "cc"  , "char, char"          , 0},
   { "i"   , "int"                 , 0},
   { "is"  , "int, char*"          , 0},
   { "s"   , "char*"               , 0},
   { "ss"  , "char*, char*"        , 0},
   { "si"  , "char*, int"          , 0},
   { "a"   , "char*"               , 0},  /* string incudes spaces            */
   { "ii"  , "int, int"            , 0},
   { "iii" , "int, int, int"       , 0},
   { "Cs"  , "char*, char*"        , 0},
   { "-"   , ""                    , 0},
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



#define   MAX_SRCH    2000
void     *s_srch      [MAX_SRCH];
int       s_nsrch     = 0;

char    (*s_searcher) (void *a_search);
char    (*s_clearer ) (void *a_search);

static char s_quoted    = '-';
static char s_escaped   = '-';



/*====================------------------------------------====================*/
/*===----                        search bar                            ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
yVIKEYS_srch_init       (void *a_searcher, void *a_clearer)
{
   s_nsrch    = MAX_SRCH;
   s_searcher = NULL;
   s_clearer  = NULL;
   yVIKEYS_srch_clear  ();
   yVIKEYS__srch_purge ();
   s_searcher = a_searcher;
   s_clearer  = a_clearer;
   return 0;
}

char
yVIKEYS__srch_purge     (void)
{
   int         i           = 0;
   for (i = 0; i < s_nsrch; ++i) {
      if (s_clearer != NULL)  s_clearer (s_srch [i]);
      s_srch [i] = NULL;
   }
   s_nsrch = 0;
   return 0;
}

char
yVIKEYS_srch_wrap    (void)
{
   yVIKEYS_srch_clear ();
   yVIKEYS__srch_purge ();
   return 0;
}

char
yVIKEYS_srch_start   (void)
{
   strlcpy     (s_search , "/", LEN_RECD);
   s_slen = 1;
   yVIKEYS__srch_purge ();
   s_quoted  = '-';
   s_escaped = '-';
   /*> yVIKEYS_view_text (s_search);                                                  <*/
   return 0;
}

char
yVIKEYS_srch_clear   (void)
{
   strlcpy     (s_search , "" , LEN_RECD);
   s_slen = 0;
   yVIKEYS__srch_purge ();
   s_quoted  = '-';
   s_escaped = '-';
   /*> yVIKEYS_view_text (s_search);                                                  <*/
   return 0;
}

char*
yVIKEYS_srch_curr    (void)
{
   return s_search;
}

char
yVIKEYS_srch_found   (void *a_match)
{
   s_srch [s_nsrch] = a_match;
   ++s_nsrch;
   return 0;
}

char
yVIKEYS_srch_exec    (void)
{
   if (s_searcher != NULL)  return s_searcher (s_search);
   return -1;
}

char         /*-> process keys for searching ---------[ ------ [gc.LE5.266.I3]*/ /*-[05.0000.102.M]-*/ /*-[--.---.---.--]-*/
yVIKEYS_srch_mode       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_temp      [11]        = "";
   char        rc          =    0;
   /*---(header)--------------------s----*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_value   ("a_major"   , a_major);
   DEBUG_USER   yLOG_value   ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (MODE_SEARCH )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_major != '/') {
      DEBUG_USER   yLOG_note    ("a_major is not a slash (/)");
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get existing len)---------------*/
   DEBUG_USER   yLOG_info    ("s_search"  , s_search);
   DEBUG_USER   yLOG_value   ("s_slen"    , s_slen);
   /*---(check for special codes)--------*/
   if (s_escaped != 'y' && a_minor == G_KEY_BSLASH) {
      s_escaped = 'y';
      DEBUG_USER   yLOG_note    ("begin escaped character");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   } else if (s_escaped == 'y') {
      s_escaped = '-';
      DEBUG_USER   yLOG_note    ("convert escaped character");
      a_minor = chrslashed (a_minor);
   }
   /*---(check for control keys)---------*/
   switch (a_minor) {
   case   G_KEY_RETURN :
      yVIKEYS_mode_exit ();
      yVIKEYS_srch_exec  ();
      DEBUG_USER   yLOG_note    ("return, execute search");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;   /* return  */
   case   G_KEY_ESCAPE :
      yVIKEYS_mode_exit ();
      yVIKEYS_srch_clear ();
      DEBUG_USER   yLOG_note    ("escape, ignore search");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for backspace)------------*/
   if (a_minor == G_KEY_DEL || a_minor == G_KEY_BS) {
      DEBUG_USER   yLOG_note    ("bs/del, remove character");
      --s_slen;
      if (s_search [s_slen] == G_KEY_DQUOTE) {
         if (s_quoted == 'y')  s_quoted = '-';
         else                  s_quoted = 'y';
      }
      if (s_slen < 1)   s_slen = 1;
      s_search [s_slen] = '\0';
      /*> yVIKEYS_view_text (s_search);                                               <*/
      DEBUG_USER   yLOG_info    ("s_search"  , s_search);
      DEBUG_USER   yLOG_value   ("s_slen"    , s_slen);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(char fixing)--------------------*/
   a_minor = chrvisible (a_minor);
   /*---(normal characters)--------------*/
   DEBUG_USER   yLOG_note    ("update search line");
   snprintf (x_temp, 10, "%c", a_minor);
   strcat   (s_search, x_temp);
   /*> yVIKEYS_view_text (s_search);                                                  <*/
   ++s_slen;
   DEBUG_USER   yLOG_info    ("s_search"  , s_search);
   DEBUG_USER   yLOG_value   ("s_slen"    , s_slen);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}




/*====================------------------------------------====================*/
/*===----                        command line                          ----===*/
/*====================------------------------------------====================*/
static void  o___COMMAND_________o () { return; }

char
yVIKEYS_cmds_init       (void)
{
   int i = 0;
   /*---(menus)--------------------------*/
   s_nmenu = 0;
   for (i = 0; i < MAX_MENU; ++i) {
      if (s_menus [i].name [0] == NULL)  break;
      s_menus [i].count = 0;
      ++s_nmenu;
   }
   /*---(menus)--------------------------*/
   s_nterm = 0;
   for (i = 0; i < MAX_TERMS; ++i) {
      if (s_terms [i].name [0] == '-')  break;
      s_terms [i].count = 0;
      ++s_nterm;
   }
   /*---(commands)-----------------------*/
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
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_cmds_wrap       (void)
{
   yVIKEYS_cmds_init ();
   return 0;
}

char
yVIKEYS_cmds_start      (void)
{
   strlcpy     (s_command , ":", LEN_COMMAND);
   s_clen = 1;
   s_quoted  = '-';
   s_escaped = '-';
   /*> yVIKEYS_view_text (s_command);                                                 <*/
   return 0;
}

char
yVIKEYS_cmds_clear      (void)
{
   strlcpy     (s_command , "" , LEN_COMMAND);
   s_clen = 0;
   s_quoted  = '-';
   s_escaped = '-';
   /*> yVIKEYS_view_text (s_command);                                                 <*/
   return 0;
}

char*
yVIKEYS_cmds_curr       (void)
{
   return s_command;
}

char
yVIKEYS__cmds_load      (char *a_command)
{
   yVIKEYS_cmds_clear ();
   if (a_command != NULL) {
      strlcpy (s_command , a_command , LEN_COMMAND);
      s_clen = strllen (s_command, LEN_COMMAND);
   }
   return 0;
}

char
yVIKEYS_cmds_direct     (char *a_command)
{
   yVIKEYS__cmds_load (a_command);
   return yVIKEYS_cmds_exec ();
}

char
yVIKEYS_cmds_menu    (char a_menu, char a_action)
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
yVIKEYS_cmds_term    (char *a_terms, char a_action)
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

int
yVIKEYS_cmds_find    (char *a_name)
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

char
yVIKEYS_cmds_add     (char a_menu, char *a_name, char *a_abbr, char *a_terms, void *a_func, char *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_dup       =    0;
   /*---(defense)------------------------*/
   --rce;  if (yVIKEYS_cmds_menu  (a_menu, ACTION_FIND) < 0)  return rce;
   --rce;  if (a_name  == NULL)                               return rce;
   --rce;  if (strllen (a_name, LEN_LABEL) <  1)              return rce;
   --rce;  if (a_abbr  == NULL)                               return rce;
   --rce;  if (strllen (a_abbr, LEN_LABEL) >  4)              return rce;
   --rce;  if (a_terms == NULL)                               return rce;
   --rce;  if (yVIKEYS_cmds_term (a_terms, ACTION_FIND) < 0)  return rce;
   --rce;  if (a_func  == NULL)                               return rce;
   /*---(check for dup)------------------*/
   x_dup = yVIKEYS_cmds_find (a_name);
   --rce;  if (x_dup >= 0)                                    return rce;
   x_dup = yVIKEYS_cmds_find (a_abbr);
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
      switch (a_terms [1]) {
      case   0  :
         s_cmds [s_ncmd].f.c   = a_func; break;
      case  'c' :
         s_cmds [s_ncmd].f.cc  = a_func; break;
      }
   case  'i' :
      switch (a_terms [1]) {
      case   0  :
         s_cmds [s_ncmd].f.i   = a_func; break;
      case  'i' :
         switch (a_terms [2]) {
         case   0  :
            s_cmds [s_ncmd].f.ii  = a_func; break;
         case  'i' :
            s_cmds [s_ncmd].f.iii = a_func; break;
         }
      case  's' :
         s_cmds [s_ncmd].f.is  = a_func; break;
      }
   case  's' :
      switch (a_terms [1]) {
      case   0  : s_cmds [s_ncmd].f.s   = a_func; break;
      case  'i' : s_cmds [s_ncmd].f.si  = a_func; break;
      case  's' : s_cmds [s_ncmd].f.ss  = a_func; break;
      }
   case  'C' :
      switch (a_terms [1]) {
      case  's' : s_cmds [s_ncmd].f.ss  = a_func; break;
      }
   }
   /*---(update count)-------------------*/
   ++s_ncmd;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
yVIKEYS__cmds_parse   (void)
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
yVIKEYS_cmds_exec     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
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
   rc = yVIKEYS__cmds_parse ();
   DEBUG_USER   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_USER   yLOG_note    ("could not parse");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find)---------------------------*/
   i = yVIKEYS_cmds_find (s_fields [0]);
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
   } else if (strcmp (s_cmds [i].terms, "ii"  ) == 0) {
      DEBUG_USER   yLOG_note    ("two integers");
      rc = s_cmds [i].f.ii  (atoi (s_fields [1]), atoi (s_fields [2]));
   } else if (strcmp (s_cmds [i].terms, "iii" ) == 0) {
      DEBUG_USER   yLOG_note    ("three integers");
      rc = s_cmds [i].f.iii (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));
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

char         /*-> process keys for input/append ------[ ------ [gc.LE5.266.I3]*/ /*-[05.0000.102.M]-*/ /*-[--.---.---.--]-*/
yVIKEYS_cmds_mode     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_temp      [11]        = "";
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_value   ("a_major"   , a_major);
   DEBUG_USER   yLOG_value   ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (MODE_COMMAND)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_major != ':') {
      DEBUG_USER   yLOG_note    ("a_major is not a colon (:)");
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get existing len)---------------*/
   DEBUG_USER   yLOG_info    ("s_command" , s_command);
   s_clen = strllen (s_command, LEN_COMMAND);
   DEBUG_USER   yLOG_value   ("s_clen"    , s_clen);
   /*---(check for quoting)--------------*/
   if (s_escaped != 'y' && a_minor == G_KEY_DQUOTE) {
      if (s_quoted != 'y') {
         DEBUG_USER   yLOG_note    ("entering quoted string");
         s_quoted = 'y';
      } else {
         DEBUG_USER   yLOG_note    ("exiting quoted string");
         s_quoted = '-';
      }
   }
   /*---(check for special codes)--------*/
   if (s_escaped != 'y' && a_minor == G_KEY_BSLASH) {
      s_escaped = 'y';
      DEBUG_USER   yLOG_note    ("begin escaped character");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   } else if (s_escaped == 'y') {
      s_escaped = '-';
      DEBUG_USER   yLOG_note    ("convert escaped character");
      a_minor = chrslashed (a_minor);
   }
   /*---(check for control keys)---------*/
   if (s_quoted != 'y') {
      switch (a_minor) {
      case   G_KEY_RETURN : case   G_KEY_ENTER  :
         DEBUG_USER   yLOG_note    ("return/enter, execute command");
         yVIKEYS_mode_exit ();
         rc = yVIKEYS_cmds_exec ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;   /* return  */
      case   G_KEY_ESCAPE :
         DEBUG_USER   yLOG_note    ("escape, ignore command");
         yVIKEYS_mode_exit ();
         yVIKEYS_cmds_clear ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
   }
   /*---(check for backspace)------------*/
   if (a_minor == G_KEY_DEL || a_minor == G_KEY_BS) {
      DEBUG_USER   yLOG_note    ("bs/del, remove character");
      --s_clen;
      if (s_command [s_clen] == G_KEY_DQUOTE) {
         if (s_quoted == 'y')  s_quoted = '-';
         else                  s_quoted = 'y';
      }
      if (s_clen < 1)   s_clen = 1;
      s_command [s_clen] = '\0';
      /*> yVIKEYS_view_text (s_command);                                              <*/
      DEBUG_USER   yLOG_info    ("s_command" , s_command);
      DEBUG_USER   yLOG_value   ("s_clen"    , s_clen);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(char fixing)--------------------*/
   a_minor = chrvisible (a_minor);
   /*---(normal characters)--------------*/
   DEBUG_USER   yLOG_note    ("update command line");
   snprintf (x_temp, 10, "%c", a_minor);
   strlcat   (s_command, x_temp, LEN_COMMAND);
   /*> yVIKEYS_view_text (s_command);                                                 <*/
   s_clen = strllen (s_command, LEN_COMMAND);
   DEBUG_USER   yLOG_info    ("s_command" , s_command);
   DEBUG_USER   yLOG_value   ("s_clen"    , s_clen);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
yVIKEYS__cmds_test      (char a_mode, char a_value)
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
yVIKEYS__cmds_unit      (char *a_question, char a_index)
{
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
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yVIKEYS__srch_unit      (char *a_question, char a_index)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRCH unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRCH global      : %2d[%-.40s]", s_slen, s_search);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




