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

#define      S_HIST_MAX    100
static char  S_HIST_LIST [S_HIST_MAX];



/*===[[ TERMS ]]==============================================================*/
#define    MAX_TERMS       50
typedef    struct   cTERMS  tTERMS;
struct cTERMS {
   cchar       name        [LEN_LABEL];
   cchar       disp        [LEN_DESC ];
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
   { "sii"  , "char*, int, int"            , 0},
   { "isss" , "int, char*, char*, char*"   , 0},
   { "Cs"   , "char*, char*"               , 0},
   { "-"    , ""                           , 0},
};
static  int s_nterm  = 0;



/*===[[ COMMANDS ]]===========================================================*/
#define     LEN_COMMAND    2000

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
      char        (*sii ) (char*, int, int);  /* function pointer               */
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


static tCOMMAND  s_fails     [MAX_CMDS];




/*===[[ HISTORY ]]============================================================*/
#define   MAX_PASS     1000
typedef struct cPASSES   tPASSES;
struct cPASSES {
   char        mark;
   char        text        [LEN_COMMAND];
   int         count;
   int         found;
};
/*---(command history)---------*/
static tPASSES   s_runs      [MAX_PASS];
static int       s_nrun      = 0;
/*---(search history)----------*/
static tPASSES   s_passes    [MAX_PASS];
static int       s_npass     = 0;
/*---(current)-----------------*/
static char      s_current   [LEN_COMMAND];
static int       s_len       = 0;



/*===[[ SEARCH ]]=============================================================*/
/*> static char s_search     [LEN_COMMAND];                                           <* 
 *> static int  s_slen       = 0;                                                     <*/

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
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char
HISTORY_limits          (char a_mode, int *a_min, int *a_max)
{
   if (a_min != NULL)  *a_min = 0;
   switch (a_mode) {
   case MODE_COMMAND :
      if (a_max != NULL)  *a_max = s_nrun;
      break;
   case MODE_SEARCH  :
      if (a_max != NULL)  *a_max = s_npass;
      break;
   }
   return 0;
}

char
HISTORY_entry           (char a_mode, int a_index, char *a_entry, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tPASSES    *x_pass      = NULL;
   char        x_len       =    0;
   int         x_found     =    0;
   int         x_count     =    0;
   int         x_max       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)  return rce;
   /*---(set limits)---------------------*/
   switch (a_mode) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;
   }
   /*---(blank line)---------------------*/
   if (a_index < 0 || a_index >= x_max) {
      sprintf (a_entry, "%-*.*s", a_max, a_max, " ");
      return 0;
   }
   /*---(create)-------------------------*/
   x_len = a_max - 8;
   x_found = (x_pass + a_index)->found;
   if (x_found > 999)  x_found = 999;
   x_count = (x_pass + a_index)->count;
   if (x_count > 99)  x_count = 99;
   sprintf (a_entry, " %-4d  %-2d  %-3d  %c  %-*.*s ",
         a_index, x_count, x_found, (x_pass + a_index)->mark,
         x_len, x_len, (x_pass + a_index)->text);
   /*---(complete)-----------------------*/
   return 0;
}

char*
HISTORY_use             (char a_mode, int a_index)
{
   switch (a_mode) {
   case MODE_COMMAND :
      return s_runs [a_index].text;
      break;
   case MODE_SEARCH  :
      return s_passes [a_index].text;
      break;
   }
}



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char 
yvikeys_cmds__valid     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_HIST_LIST, a_abbr, S_HIST_MAX);
   DEBUG_HIST   yLOG_sint    (rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

int  
yvikeys_cmds__index     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_HIST_LIST, a_abbr, S_HIST_MAX);
   DEBUG_HIST   yLOG_sint    (rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_HIST   yLOG_snote   ("search");
   for (i = 0; i < s_nrun; ++i) {
      if (s_runs [i].mark != a_abbr)  continue;
      n = i;
      break;
   }
   DEBUG_HIST   yLOG_sint    (n);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}

char yvikeys_srch__valid  (char a_abbr) { return yvikeys_cmds__valid (a_abbr); }

int  
yvikeys_srch__index     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1; 
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_HIST_LIST, a_abbr, S_HIST_MAX);
   DEBUG_HIST   yLOG_sint    (rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_HIST   yLOG_snote   ("search");
   for (i = 0; i < s_npass; ++i) {
      if (s_passes [i].mark != a_abbr)  continue;
      n = i;
      break;
   }
   DEBUG_HIST   yLOG_sint    (n);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}

char yvikeys_cmds__quit      (void) { myVIKEYS.done = 'y';     return 0; }
char yvikeys_cmds__write     (void) { yvikeys_file_writer ();  return 0; }

char
yvikeys_cmds__writequit (void)
{
   yvikeys_file_writer ();
   myVIKEYS.done = 'y';
   return 0;
}

int
yvikeys_cmds__find      (char *a_name)
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
/*===----                      history handling                       ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

char
HISTORY__purge          (char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   /*---(set pointer)--------------------*/
   switch (a_type) {
   case MODE_COMMAND :  x_pass = &s_runs;    break;
   case MODE_SEARCH  :  x_pass = &s_passes;  break;
   default :  return -1;
   }
   /*---(clear history)------------------*/
   for (i = 0; i < MAX_PASS; ++i) {
      (x_pass + i)->mark  = '-';
      strlcpy ((x_pass + i)->text, "-", LEN_RECD);
      (x_pass + i)->found = 0;
      (x_pass + i)->count = 0;
   }
   /*---(zero counters)------------------*/
   switch (a_type) {
   case MODE_COMMAND :  s_nrun  = 0;   break;
   case MODE_SEARCH  :  s_npass = 0;   break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
HISTORY__unmark      (char a_type, char a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   int         x_max       =    0;
   /*---(set limits)---------------------*/
   switch (a_type) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;
   }
   /*---(clear old mark)-----------------*/
   for (i = 0; i < x_max; ++i) {
      if ((x_pass + i)->mark != a_mark)  continue;
      (x_pass + i)->mark = '-';
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
HISTORY__mark        (char a_type, char a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   int         x_max       =    0;
   /*---(set limits)---------------------*/
   switch (a_type) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;
   }
   /*---(clear old mark)-----------------*/
   for (i = 0; i < x_max; ++i) {
      if ((x_pass + i)->mark != a_mark)  continue;
      (x_pass + i)->mark = '-';
   }
   /*---(clear new mark)-----------------*/
   (x_pass + x_max - 1)->mark  = a_mark;
   /*---(complete)-----------------------*/
   return 0;
}

char
HISTORY__roll        (char a_mode, int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         rce         =  -10;
   int         i           =    0;
   int         x_count     =    0;
   int         x_found     =    0;
   char        x_mark      =    0;
   int         x_max       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_index < 0)            return rce;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_value   ("a_mode"    , a_mode);
   DEBUG_HIST   yLOG_value   ("a_index"   , a_index);
   /*---(set pointer)--------------------*/
   switch (a_mode) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;   break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass;  break;
   default :  return -1;
   }
   /*---(save current)-------------------*/
   DEBUG_HIST   yLOG_note    ("save current");
   strlcpy (s_current, (x_pass + a_index)->text, LEN_RECD);
   x_found = (x_pass + a_index)->found;
   x_count = (x_pass + a_index)->count;
   x_mark  = (x_pass + a_index)->mark;
   /*---(roll rest backward)-------------*/
   DEBUG_HIST   yLOG_note    ("roll history through max back one");
   for (i = a_index; i < x_max - 1; ++i) {
      strlcpy ((x_pass + i)->text, (x_pass + i + 1)->text, LEN_RECD);
      (x_pass + i)->found = (x_pass + i + 1)->found;
      (x_pass + i)->count = (x_pass + i + 1)->count;
      (x_pass + i)->mark  = (x_pass + i + 1)->mark;
   }
   /*---(place current at end)-----------*/
   DEBUG_HIST   yLOG_note    ("put current back at end");
   strlcpy ((x_pass + x_max - 1)->text, s_current, LEN_RECD);
   (x_pass + x_max - 1)->found = x_found;
   (x_pass + x_max - 1)->count = x_count;
   (x_pass + x_max - 1)->mark  = x_mark;
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
HISTORY__return      (char a_mode, char a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   int         n           =   -1;
   int         x_max       =    0;
   /*---(set limits)---------------------*/
   switch (a_mode) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;
   }
   /*---(clear old mark)-----------------*/
   for (i = 0; i < x_max; ++i) {
      if ((x_pass + i)->mark != a_mark)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   /*---(set global)---------------------*/
   HISTORY__roll (a_mode, n);
   /*---(complete)-----------------------*/
   return 0;
}

int
HISTORY__find         (char a_mode, char *a_text)
{
   /*---(locals)-----------+-----+-----+-*/
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   int         n           =   -1;
   int         x_max       =    0;
   /*---(set limits)---------------------*/
   switch (a_mode) {
   case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;
   case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;
   }
   /*---(look for duplicate)-------------*/
   for (i = 0; i < x_max; ++i) {
      if ((x_pass + i)->text [0] != a_text [0])      continue;
      if ((x_pass + i)->text [1] != a_text [1])      continue;
      if ((x_pass + i)->text [2] != a_text [2])      continue;
      if (strcmp ((x_pass + i)->text, a_text) != 0)  continue;
      n = i;
      break;
   }
   /*---(set global)---------------------*/
   if (n >= 0)  {
      HISTORY__roll (a_mode, n);
   } else {
      strlcpy ((x_pass + x_max)->text, a_text, LEN_RECD);
      switch (a_mode) {
      case MODE_COMMAND :  x_max = ++s_nrun;  break;
      case MODE_SEARCH  :  x_max = ++s_npass; break;
      }
   }
   /*---(complete)-----------------------*/
   return x_max;
}

char
HISTORY__load           (char a_mode, char *a_text)
{
   if (a_text == NULL) {
      strlcpy (s_current, "" , LEN_COMMAND);
      s_len  = 0;
      return -1;
   }
   strlcpy  (s_current, a_text, LEN_COMMAND);
   strldchg (s_current, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
   s_len  = strllen (s_current, LEN_COMMAND);
   return 0;
}

char
HISTORY__exec           (char a_mode)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   tPASSES    *x_pass      = NULL;
   int         i           =    0;
   int         n           =   -1;
   int         x_max       =    0;
   char        x_clear     [LEN_LABEL];
   char        x_repeat    [LEN_LABEL];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_mode"    , a_mode);
   DEBUG_HIST   yLOG_value   ("s_len"     , s_len);
   DEBUG_HIST   yLOG_info    ("s_current" , s_current);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (a_mode)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set limits)---------------------*/
   switch (a_mode) {
   case MODE_COMMAND :
      x_pass   = &s_runs;
      x_max    = s_nrun;
      break;
   case MODE_SEARCH  : 
      x_pass   = &s_passes;
      x_max    = s_npass;
      break;
   }
   DEBUG_HIST   yLOG_value   ("x_max"     , x_max);
   sprintf (x_clear  , "%c"       , a_mode);
   DEBUG_HIST   yLOG_info    ("x_clear"   , x_clear);
   sprintf (x_repeat , "%c%c"     , a_mode, a_mode);
   DEBUG_HIST   yLOG_info    ("x_repeat"  , x_repeat);
   /*---(unmark, no clear)---------------*/
   if (s_len == 2 && s_current [0] == 'u') {
      DEBUG_HIST   yLOG_char    ("unmarking" , s_current [1]);
      HISTORY__unmark (a_mode, s_current [1]);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(mark, no clear)-----------------*/
   if (s_len == 2 && s_current [0] == 'm') {
      DEBUG_HIST   yLOG_char    ("marking"   , s_current [1]);
      HISTORY__mark (a_mode, s_current [1]);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(clear results)------------------*/
   if (a_mode == MODE_SEARCH)  yvikeys_srch__purge  ();
   if (s_len <= 0) {
      DEBUG_HIST   yLOG_note    ("empty content string, leaving");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (s_len == 1 && strcmp (s_current, x_clear) == 0) {
      DEBUG_HIST   yLOG_note    ("request to clear");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(repeat last)--------------------*/
   else if (s_len == 2 && strcmp (s_current, x_repeat) == 0) {
      DEBUG_HIST   yLOG_note    ("request to repeat last");
      if (x_max == 0) return -1;
      n = x_max - 1;
      HISTORY__roll (a_mode, n);
   }
   /*---(repeat complex)-----------------*/
   else if (s_len > 2 && strncmp (s_current, x_repeat, 2) == 0) {
      /*---(repeat marked)---------------*/
      if (s_len == 3 && strchr (S_HIST_LIST, s_current [2]) != NULL) {
         DEBUG_HIST   yLOG_note    ("request to repeat a marked one");
         rc = HISTORY__return (a_mode, s_current [2]);
         if (rc < 0)  return rc;
      }
      /*---(repeat by reference)---------*/
      else {
         DEBUG_HIST   yLOG_note    ("request to repeat a previous one");
         n = x_max - atoi (s_current + 2) - 1;
         DEBUG_HIST   yLOG_value   ("n"         , n);
         if (n < 0) n = 0;
         HISTORY__roll (a_mode, n);
      }
   }
   /*---(normal)-------------------------*/
   else {
      DEBUG_HIST   yLOG_note    ("typed request");
      x_max = HISTORY__find (a_mode, s_current);
   }
   /*---(make current)-------------------*/
   strlcpy (s_current, (x_pass + x_max - 1)->text, LEN_RECD);
   s_len = strllen (s_current, LEN_RECD);
   (x_pass + x_max - 1)->found = 0;
   ++(x_pass + x_max - 1)->count;
   DEBUG_HIST   yLOG_value   ("s_len"     , s_len);
   DEBUG_HIST   yLOG_info    ("s_current" , s_current);
   /*---(execute)------------------------*/
   strldchg (s_current, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   switch (a_mode) {
   case MODE_COMMAND :
      DEBUG_HIST   yLOG_note    ("execute as command");
      rc = yvikeys_cmds_exec ();
      (x_pass + x_max - 1)->found = rc;
      break;
   case MODE_SEARCH  : 
      DEBUG_HIST   yLOG_note    ("execute as search");
      rc = s_searcher (s_current);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return rc;
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
   HISTORY__purge (MODE_SEARCH);
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
yvikeys_cmds__purge     (void)
{
   int         i           =    0;
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
   return 0;
}

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
   /*---(macro abbrev list)--------------*/
   DEBUG_PROG   yLOG_note    ("initialize hist list");
   strlcpy (S_HIST_LIST, ""            , S_HIST_MAX);
   strlcat (S_HIST_LIST, gvikeys_lower , S_HIST_MAX);
   strlcat (S_HIST_LIST, gvikeys_upper , S_HIST_MAX);
   /*> strlcat (S_HIST_LIST, gvikeys_number, S_HIST_MAX);                             <*/
   strlcat (S_HIST_LIST, gvikeys_greek , S_HIST_MAX);
   DEBUG_PROG   yLOG_info    ("LIST"      , S_HIST_LIST);
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
   yvikeys_cmds__purge ();
   /*---(clear history)------------------*/
   DEBUG_PROG   yLOG_note    ("clear all history");
   HISTORY__load  (MODE_COMMAND, NULL);
   HISTORY__purge (MODE_COMMAND);
   /*---(update status)------------------*/
   DEBUG_PROG   yLOG_note    ("update status");
   STATUS_init_set   (MODE_COMMAND);
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("add universal commands");
   myVIKEYS.done = '-';
   rc = yVIKEYS_cmds_add (YVIKEYS_M_FILE  , "quit"        , "q"   , ""     , yvikeys_cmds__quit     , "quit current file (if no changes), exit if the only file"    );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_FILE  , "quitall"     , "qa"  , ""     , yvikeys_cmds__quit     , "quit all files (if no changes), and exit"                    );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_FILE  , "writequit"   , "wq"  , ""     , yvikeys_cmds__writequit, ""                                                            );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_FILE  , "writequitall", "wqa" , ""     , yvikeys_cmds__writequit, ""                                                            );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_EDIT  , "dump"        , ""    , "s"    , yvikeys_clip_dump      , "dump a specified data table to the clipboard in flat text"   );
   rc = yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "menu"        , ""    , "i"    , yvikeys_menu_reanchor  , "change the menu anchoring");
   /*---(yparse)-------------------------*/
   rc = yPARSE_handler (MODE_COMMAND , "command"   , 7.4, "cO----------", -1, yvikeys_cmds__reader, yvikeys_cmds__writer_all, "------------" , "a,command-----------------", "command history"           );
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
   s_srch [s_nsrch].pass   = s_npass - 1;
   if (a_label != NULL)  strlcpy (s_srch [s_nsrch].label, a_label, LEN_LABEL);
   s_srch [s_nsrch].b_pos = a_buf;
   s_srch [s_nsrch].x_pos = x;
   s_srch [s_nsrch].y_pos = y;
   s_srch [s_nsrch].z_pos = z;
   ++s_nsrch;
   ++s_passes [s_npass - 1].found;
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



char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds__writer    (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   char        x_cmd       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   n  = yvikeys_cmds__index (a_abbr);
   DEBUG_OUTP   yLOG_value   ("index"     , n);
   if (n == -1) { 
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   --rce; if (n  < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter)-------------------------*/
   DEBUG_OUTP   yLOG_char    ("mark"      , s_runs [n].mark);
   if (s_runs [n].mark == '-') {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write)-----------------------*/
   strlcpy  (x_cmd, s_runs [n].text, LEN_RECD);
   yPARSE_fullwrite ("command", s_runs [n].mark, x_cmd);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_cmds__writer_all     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_end = strlen (S_HIST_LIST);
   yPARSE_verb_begin ("command");
   /*---(walk list)----------------------*/
   for (i = 0; i < x_end; ++i) {
      rc = yvikeys_cmds__writer (S_HIST_LIST [i]);
      if (rc < 1)  continue;
      ++c;
      yPARSE_verb_break (c);
   }
   /*---(wrap-up)------------------------*/
   yPARSE_verb_end   (c);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_srch__writer    (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   char        x_srch       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_SEARCH)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   n  = yvikeys_srch__index (a_abbr);
   DEBUG_OUTP   yLOG_char    ("index"     , n);
   if (n == -1) { 
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   --rce; if (n  < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(filter)-------------------------*/
   DEBUG_OUTP   yLOG_char    ("mark"      , s_passes [n].mark);
   if (s_passes [n].mark == '-') {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write)-----------------------*/
   strlcpy  (x_srch, s_passes [n].text, LEN_RECD);
   yPARSE_fullwrite ("search", s_passes [n].mark, x_srch);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_srch__writer_all     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_SEARCH)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_end = strlen (S_HIST_LIST);
   yPARSE_verb_begin ("search");
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      rc = yvikeys_srch__writer (S_HIST_LIST [i]);
      if (rc < 1)  continue;
      ++c;
      yPARSE_verb_break (c);
   }
   /*---(wrap-up)------------------------*/
   yPARSE_verb_end   (c);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char
yvikeys_cmds__reader (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         n           =    0;
   char        x_verb      [LEN_LABEL];
   char        x_abbr      =    0;
   char        x_cmd       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("command", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   n  = yvikeys_cmds__valid (x_abbr);
   DEBUG_INPT   yLOG_char    ("strlchr"   , n);
   --rce; if (n   < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get command)--------------------*/
   rc = yPARSE_popstr (x_cmd);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   strldchg (x_cmd, G_KEY_SPACE, G_CHAR_STORAGE, LEN_COMMAND);
   DEBUG_INPT   yLOG_info    ("x_cmd"     , x_cmd);
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strlcpy (s_runs [s_nrun].text, x_cmd, LEN_RECD);
   s_runs [s_nrun].mark  = x_abbr;
   ++s_nrun;
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_srch__reader         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_abbr      =    0;
   char        x_srch       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_SEARCH)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("search", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   rc  = yvikeys_srch__valid (x_abbr);
   DEBUG_INPT   yLOG_value   ("index"     , rc);
   --rce; if (rc  < 0) { 
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get command)--------------------*/
   rc = yPARSE_popstr (x_srch);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   strldchg (x_srch, G_KEY_SPACE, G_CHAR_STORAGE, LEN_COMMAND);
   DEBUG_INPT   yLOG_info    ("x_srch"    , x_srch);
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strlcpy (s_passes [s_npass].text, x_srch, LEN_RECD);
   s_passes [s_npass].mark  = x_abbr;
   ++s_npass;
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
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

char
yvikeys_cmds__menu          (char a_menu, char a_action)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*> for (i = 0; i < s_nmenu; ++i) {                                                <* 
    *>    if (a_menu != s_menus [i].abbr)   continue;                                 <* 
    *>    switch (a_action) {                                                         <* 
    *>    case '+' :                                                                  <* 
    *>       ++(s_menus [i].count);                                                   <* 
    *>       break;                                                                   <* 
    *>    case '-' :                                                                  <* 
    *>       if (s_menus [i].count < 1)  return -1;                                   <* 
    *>       --(s_menus [i].count);                                                   <* 
    *>       break;                                                                   <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   return -3;
}

char
yvikeys_cmds__terms  (char *a_terms, char a_action)
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
   int         n           =   -1;
   char        x_dup       =    0;
   char        t           [LEN_LABEL];
   char        x_level     =    0;
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
   --rce;  if (a_menu == 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   sprintf (t, "µ%c", a_menu);
   DEBUG_CMDS   yLOG_info    ("t"         , t);
   n  = yvikeys__menu_find (t, &x_level, &x_last);
   DEBUG_CMDS   yLOG_value   ("menu"      , n);
   --rce;  if (n < 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name  == NULL || strllen (a_name, LEN_LABEL) <  1) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_info    ("a_name"    , a_name);
   DEBUG_CMDS   yLOG_point   ("a_abbr"    , a_abbr);
   --rce;  if (a_abbr  == NULL || strllen (a_abbr, LEN_LABEL) >  4) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_terms"   , a_terms);
   --rce;  if (a_terms == NULL || yvikeys_cmds__terms (a_terms, ACTION_FIND) < 0) {
      DEBUG_CMDS   yLOG_note    ("term specification not found in inventory");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_CMDS   yLOG_point   ("a_func"    , a_func);
   --rce;  if (a_func  == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for dup)------------------*/
   x_dup = yvikeys_cmds__find (a_name);
   DEBUG_CMDS   yLOG_value   ("dup name"  , x_dup);
   --rce;  if (x_dup >= 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_dup = yvikeys_cmds__find (a_abbr);
   DEBUG_CMDS   yLOG_value   ("dup abbr"  , x_dup);
   --rce;  if (x_dup >= 0) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
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
   s_cmds [s_ncmd].f.v   = a_func;
   /*---(update count)-------------------*/
   ++s_ncmd;
   DEBUG_CMDS   yLOG_value   ("SUCCESS"   , s_ncmd);
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        running a command                     ----===*/
/*====================------------------------------------====================*/
static void  o___CMDS_RUN________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds__parse   (void)
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
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   strlcpy (x_work, s_current, LEN_COMMAND);
   x_len = strllen (x_work, LEN_COMMAND);
   strldchg (x_work, G_CHAR_STORAGE, G_KEY_SPACE, LEN_COMMAND);
   DEBUG_CMDS   yLOG_info    ("x_work"    , x_work);
   /*---(parse command)------------------*/
   p     = strtok_r (x_work, q, &r);
   ++p;
   x_len = strllen (p, LEN_COMMAND);
   DEBUG_CMDS   yLOG_info    ("g_cmd"     , p);
   if (strllen (x_work, LEN_COMMAND) > x_len) {
      strlcpy (s_all, p + x_len + 1, LEN_COMMAND);
      strltrim (s_all, ySTR_BOTH, LEN_COMMAND);
   }
   DEBUG_CMDS   yLOG_info    ("s_all"     , s_all);
   /*---(parse)--------------------------*/
   for (i = 0; i < 10; ++i)  strlcpy (s_fields [i], "", LEN_COMMAND);
   for (i = 0; i < 10; ++i) {
      DEBUG_CMDS   yLOG_value   ("i"         , i);
      DEBUG_CMDS   yLOG_info    ("p"         , p);
      strltrim (p, ySTR_BOTH, LEN_COMMAND);
      strlcpy (s_fields [i], p, LEN_COMMAND);
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
   rc = yvikeys_cmds__parse ();
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_CMDS   yLOG_note    ("could not parse");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find)---------------------------*/
   i = yvikeys_cmds__find (s_fields [0]);
   DEBUG_CMDS   yLOG_value   ("i"         , i);
   --rce;  if (i < 0) {
      DEBUG_CMDS   yLOG_note    ("command not found");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   --rce; if (strcmp (s_cmds [i].terms, ""    ) == 0) {
      DEBUG_CMDS   yLOG_note    ("void type, no args");
      rc = s_cmds [i].f.v   ();
   } else if (strcmp (s_cmds [i].terms, "c"   ) == 0) {
      DEBUG_CMDS   yLOG_note    ("one char arg");
      rc = s_cmds [i].f.c   (s_fields [1][0]);
   } else if (strcmp (s_cmds [i].terms, "cc"  ) == 0) {
      DEBUG_CMDS   yLOG_note    ("two char args");
      rc = s_cmds [i].f.cc  (s_fields [1][0], s_fields [2][0]);
   } else if (strcmp (s_cmds [i].terms, "s"   ) == 0) {
      DEBUG_CMDS   yLOG_note    ("one string arg");
      rc = s_cmds [i].f.s   (s_fields [1]);
   } else if (strcmp (s_cmds [i].terms, "a"   ) == 0) {
      DEBUG_CMDS   yLOG_note    ("one long string arg");
      rc = s_cmds [i].f.s   (s_all);
   } else if (strcmp (s_cmds [i].terms, "ss"  ) == 0) {
      DEBUG_CMDS   yLOG_note    ("two string args");
      rc = s_cmds [i].f.ss  (s_fields [1], s_fields [2]);
   } else if (strcmp (s_cmds [i].terms, "i"   ) == 0) {
      DEBUG_CMDS   yLOG_note    ("single integer");
      /*> printf ("s_cmds [i].f.i %s %d\n", s_fields [1], atoi (s_fields [1]));       <*/
      rc = s_cmds [i].f.i   (atoi (s_fields [1]));
   } else if (strcmp (s_cmds [i].terms, "ii"  ) == 0) {
      DEBUG_CMDS   yLOG_note    ("two integers");
      rc = s_cmds [i].f.ii  (atoi (s_fields [1]), atoi (s_fields [2]));
   } else if (strcmp (s_cmds [i].terms, "iii" ) == 0) {
      DEBUG_CMDS   yLOG_note    ("three integers");
      rc = s_cmds [i].f.iii (atoi (s_fields [1]), atoi (s_fields [2]), atoi (s_fields [3]));
   } else if (strcmp (s_cmds [i].terms, "sii" ) == 0) {
      DEBUG_CMDS   yLOG_note    ("string and two integers");
      rc = s_cmds [i].f.sii (s_fields [1], atoi (s_fields [2]), atoi (s_fields [3]));
   } else if (strcmp (s_cmds [i].terms, "isss") == 0) {
      DEBUG_CMDS   yLOG_note    ("integer and three strings");
      rc = s_cmds [i].f.isss (atoi (s_fields [1]), s_fields [2], s_fields [3], s_fields [4]);
   } else if (strcmp (s_cmds [i].terms, "is"  ) == 0) {
      DEBUG_CMDS   yLOG_note    ("integer arg and string arg");
      rc = s_cmds [i].f.is  (atoi (s_fields [1]), s_fields [1]);
   } else if (strcmp (s_cmds [i].terms, "Cs"  ) == 0) {
      DEBUG_CMDS   yLOG_note    ("command name and string arg");
      rc = s_cmds [i].f.ss  (s_fields [0], s_fields [1]);
   } else {
      DEBUG_CMDS   yLOG_note    ("crazy other shit, please update or fix");
      rc = -1;
      /*> my.key_error = 'y';                                                      <*/
   }
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_value   ("rc"        , rc);
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   /*---(complete)-----------------------*/
   return rc;
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
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
CMDS__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "CMDS unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "global"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS global      : %2d %2d[%-.40s]", s_ncmd, s_len, s_current);
   }
   /*> else if (strcmp (a_question, "menu_count"     )   == 0) {                         <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS menu count  : %d", s_nmenu);   <* 
    *> }                                                                                 <*/
   /*> else if (strcmp (a_question, "menu"           )   == 0) {                                                                                                                         <* 
    *>    /+> snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS menu    (%2d): %c %-12.12s %d", a_index, s_menus [a_index].abbr, s_menus [a_index].name, s_menus [a_index].count);   <+/   <* 
    *> }                                                                                                                                                                                 <*/
   else if (strcmp (a_question, "term_count"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS term count  : %d", s_nterm);
   }
   else if (strcmp (a_question, "term"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS term    (%2d): %-5.5s %-30.30s %d", a_index, s_terms [a_index].name, s_terms [a_index].disp, s_terms [a_index].count);
   }
   else if (strcmp (a_question, "command"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS command (%2d): %c %-12.12s %-4.4s %-4.4s %3s", a_index, s_cmds [a_index].menu, s_cmds [a_index].name, s_cmds [a_index].abbr, s_cmds [a_index].terms, (s_cmds [a_index].f.v == NULL) ? "---" : "SET");
   }
   else if (strcmp (a_question, "history"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "CMDS history     : %2d %c %2d %3d  %-.40s", a_index, s_runs [a_index].mark, s_runs [a_index].count, s_runs [a_index].found, s_runs [a_index].text);
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
   else if (strcmp (a_question, "history"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH history     : %2d %c %2d %2d  %-.40s", a_index, s_passes [a_index].mark, s_passes [a_index].count, s_passes [a_index].found, s_passes [a_index].text);
   }
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


