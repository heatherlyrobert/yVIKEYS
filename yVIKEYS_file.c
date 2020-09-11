/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*============================---- METIS TASKS ---============================*/
/*
 * metis  tn1#·  add menu control to version unit testing (units 01-05)
 * metis  tn1#·  add menu control to file naming/renaming and change directory
 * metis  tn1#·  add menu control to file writing, save as, etc
 * metis  tn1#·  add menu control to file reading, updating, etc
 * metis  dw2#·  build and test regex directory entry function and unit test
 * metis  dw2#·  update file open to use regex support function and unit test
 * metis  dw2#·  update change directory to expect regex (and take first)
 * metis  dw1#·  update file status bar to have current directory too
 * metis  dw2#·  add file status to unit testing, like with other statuses
 * metis  dw2#·  add unit testing for file...browse option
 * metis  wl8··  add protect option for file compression, meaning gzip
 * metis  wl8··  add protect option for file encryption, one standard
 * metis  wl8··  add protect option for file passwords
 * metis  ww1··  file read needs undo history for merges (likely clean opens)
 * metis  dn1··  unit testing for file new to make sure really fresh
 * metis  dn1#·  unit testing for file browse to test regex link
 *
 *
 */


/*===[[ DECISION :: FORMAT ]]=================================================*/

/*
 *   SUMMARY - almost always use easily debuggable 8-bit safe ascii flat files
 *
 *   SITUATION
 *      yVIKEYS will be the foundation for a large and diverse set of
 *      applications on which i will constantly depend.
 *
 *   CHALLENGE
 *      since i am a dedicated dog-fooder, i just do not have the time or
 *      inclination to create perfectly tuned file formats for each situation.
 *
 *   TRADE-OFF
 *      best-case-----------------  my-choice------------------------------
 *      compact/encoded             8bit-safe ascii flat files
 *      fast read/write             human readable and vim/awk/sed editable
 *      clever methods              clear and simple
 *      demonstrate brilliance      easily debuggable
 *      praise and accolades        laughter and derision
 *                                  save fancy formats for o/s tools
 *
 *    OBJECTIVES
 *      -- ascii text only that can be read, altered, and created in an editor
 *      -- 8 bit safe (unsigned char) for transportability
 *      -- columnar for readability, even though it wastes space
 *      -- ascii field separator delimited to make column boundaries clear
 *      -- record type fields to aid parsing and interpretation
 *      -- internal comments, headings, and counts for human readability
 *      -- some additional contextual data if it helps readability
 *
 */



/*
 *     c   = char
 *     a   = address label   (12 char)
 *     i   = decimal         (5 char)
 *     f   = float           (10 char)
 *     T   = terse string    (10 char)
 *     N   = name string     (20 char)
 *     D   = desc string     (60 char)
 *     S   = open string     (no trunc)
 *
 *
 */
/*---(globals)----------+-----------+-*/

#define     FILE_BLANK  "untitled"
#define     FILE_CALL_ONCE    '1'
#define     FILE_CALL_ASCII   'a'

#define     MAX_SECTION   50
typedef struct  cSECTION  tSECTION;
struct cSECTION {
   cchar       type;
   cchar       abbr;
   cchar       name        [LEN_DESC ];
   cchar       label       [LEN_LABEL];
   cchar       version;
   cchar       specs       [LEN_LABEL];
   cchar       column      [ 9][LEN_LABEL];
   char        (*writer)   (void);
   char        (*reader)   (char, void*,void*,void*,void*,void*,void*,void*,void*,void*);
   int         try;
   int         bad;
};
static tSECTION  s_sections [MAX_SECTION] = {
   /* -   ----abbr---    ---name------------    --label---   ver   ---specs--    ---1st----    ---2nd----    ---3rd----    ---4th----    ---5th----    ---6th----    ---7th----    ---8th----    ---9th----   writer  reader  try  bad */
   { 'i', UMOD_MARK   , "location marks"     , "loc_mark"  , 'B', "ciii------", "-a"        , "--x"       , "--y"       , "--z"       , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'i', UMOD_VISUAL , "visual marks"       , "visu_mark" , 'A', "ciiiii----", "-a"        , "xbeg"      , "ybeg"      , "xend"      , "yend"      , "zall"      , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'i', SMOD_MACRO  , "saved macros"       , "macro"     , 'A', "ciiO------", "-a"        , "count"     , "rc"        , "command"   , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'i', MODE_SEARCH , "search history"     , "search"    , 'A', "ciiO------", "-a"        , "count"     , "found"     , "search"    , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'i', MODE_COMMAND, "command history"    , "command"   , 'A', "ciiO------", "-a"        , "count"     , "rc"        , "command"   , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'i', SMOD_SREG   , "text registers"     , "text_reg"  , 'A', "cO--------", "-a"        , "data"      , ""          , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'e', FILE_DEPCEL , "dependent cells"    , "cell_dep"  , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'e', FILE_FREECEL, "independent cells"  , "cell_free" , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'e', FILE_TABS   , "tab (v-axis)"       , "tab"       , 'I', "Naaiiic---", "name"      , "min"       , "max"       , "x_size"    , "y_size"    , "z_size"    , "type"      , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'e', FILE_COLS   , "columns (x-axis)"   , "width"     , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { 'e', FILE_ROWS   , "rows (y-axis)"      , "height"    , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
   { '-', 0           , ""                   , ""          , '-', "----------", ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },
};
static int  s_nsection   = 0;


static FILE    *s_file      = NULL;          /* file pointer                   */
static char     s_fields    [20][LEN_RECD];
static int      s_nfield    =    0;
static int      s_lines     =    0;


static char    *s_valid     = "csLDSif-";


char    (*s_handlers) (void);
char    (*s_prepper)  (char a_pass);
char    (*s_finisher) (char a_pass);





/*====================------------------------------------====================*/
/*===----                          supporting                          ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char
yvikeys__file_by_abbr   (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   static char x_last      =  '-';
   static char n           =    0;
   int         i           =    0;
   /*---(short-cut)----------------------*/
   if (a_abbr == x_last)  return n;
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   x_last = a_abbr;
   return n;
}

char
yvikeys__file_by_label  (char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   static char x_last      [LEN_LABEL] = "-";
   static char n           =    0;
   int         i           =    0;
   /*---(short-cut)----------------------*/
   if (strcmp (a_label, x_last) == 0)  return n;
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].label [0] != a_label [0])      continue;
      if (strcmp (s_sections [i].label, a_label) != 0)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   strlcpy (x_last, a_label, LEN_LABEL);
   return n;
}

char
yvikeys_file_status          (char *a_list)
{
   snprintf (a_list, LEN_FULL, "file    [%-43.43s] [%-23.23s]", myVIKEYS.f_title, myVIKEYS.f_loc);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          program level                       ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_file_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (FMOD_FILE)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(reset globals)------------------*/
   DEBUG_PROG   yLOG_note    ("set defaults");
   myVIKEYS.f_control = '-';
   strlcpy (myVIKEYS.f_vernum  , "----"  , LEN_LABEL);
   strlcpy (myVIKEYS.f_vertxt  , "-----" , LEN_DESC );
   strlcpy (myVIKEYS.s_prog    , "-"     , LEN_LABEL);
   strlcpy (myVIKEYS.s_fullname, "-"     , LEN_DESC );
   strlcpy (myVIKEYS.s_vernum  , "-.--"  , LEN_LABEL);
   strlcpy (myVIKEYS.s_vertxt  , "----"  , LEN_DESC );
   strlcpy (myVIKEYS.s_namesake, ""      , LEN_DESC );
   strlcpy (myVIKEYS.s_ext     , ""      , LEN_LABEL);
   strlcpy (myVIKEYS.s_filetype, ""      , LEN_DESC );
   sprintf (myVIKEYS.f_loc     , "%s/", getcwd (NULL, 0));
   if (strcmp (myVIKEYS.f_loc, "//") == 0)  strlcpy (myVIKEYS.f_loc, "/", LEN_LABEL);
   DEBUG_PROG   yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   /*---(pointers)-----------------------*/
   s_handlers  = NULL;
   s_prepper   = NULL;
   s_finisher  = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (FMOD_FILE);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_file_handlers   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(handlers)-----------------------*/
   rc = yPARSE_handler_max (UMOD_MARK    , "loc_mark"  , 7.1, "cT----------", -1, yvikeys_mark_reader, yvikeys_mark_writer_all, "------------" , "a,label", "map mode location marks");
   DEBUG_FILE   yLOG_value   ("loc_mark"  , rc);
   rc = yPARSE_handler_max (UMOD_VISUAL  , "visu_mark" , 7.2, "cTT---------", -1, yvikeys_visu__reader, yvikeys_visu__writer_all, "------------" , "a,beg,end", "map mode visual selections");
   DEBUG_FILE   yLOG_value   ("visu_mark" , rc);
   rc = yPARSE_handler_max (SMOD_MACRO   , "macro"     , 7.3, "cO----------", -1, yvikeys_macro_reader, yvikeys_macro_writer_all, "------------" , "a,keys", "keyboard macros"           );
   DEBUG_FILE   yLOG_value   ("macro"     , rc);
   rc = yPARSE_handler_max (MODE_COMMAND , "command"   , 7.4, "cO----------", -1, yvikeys_cmds_reader, yvikeys_cmds_writer, "------------" , "a,command-----------------", "command history"           );
   DEBUG_FILE   yLOG_value   ("command"   , rc);
   rc = yPARSE_handler_max (MODE_SEARCH  , "search"    , 7.5, "cO----------", -1, yvikeys_srch_reader, yvikeys_srch_writer, "------------" , "a,search"                  , "search history"            );
   DEBUG_FILE   yLOG_value   ("search"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_whoami          (char *a_full, char *a_vernum, char *a_vertxt, char *a_namesake, char *a_ext, char *a_filetype, void *a_handlers, void *a_prepper, void *a_finisher)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (FMOD_FILE)) {
      DEBUG_PROG   yLOG_note    ("init must be successfully called first");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(full executable name)-----------*/
   DEBUG_PROG   yLOG_point   ("a_full"    , a_full);
   --rce;  if (a_full == NULL) {
      DEBUG_PROG   yLOG_note    ("requires a program name");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (myVIKEYS.s_fullname, a_full, LEN_DESC);
   DEBUG_PROG   yLOG_info    ("s_fullname", myVIKEYS.s_fullname);
   /*---(program name)-------------------*/
   p = strrchr (a_full, '/');
   if (p != NULL)   strlcpy (myVIKEYS.s_prog, p + 1 , LEN_LABEL);
   else             strlcpy (myVIKEYS.s_prog, a_full, LEN_LABEL);
   DEBUG_PROG   yLOG_info    ("s_prog"    , myVIKEYS.s_prog);
   /*---(calling program version)--------*/
   DEBUG_PROG   yLOG_point   ("a_vernum"  , a_vernum);
   --rce;  if (a_vernum != NULL) {
      strlcpy (myVIKEYS.s_vernum, a_vernum, LEN_LABEL);
      DEBUG_PROG   yLOG_info    ("s_vernum"  , myVIKEYS.s_vernum);
   }
   /*---(calling program ver desc)-------*/
   DEBUG_PROG   yLOG_point   ("a_vertxt"  , a_vertxt);
   --rce;  if (a_vertxt != NULL) {
      strlcpy (myVIKEYS.s_vertxt, a_vertxt, LEN_DESC);
      DEBUG_PROG   yLOG_info    ("s_vertxt"  , myVIKEYS.s_vertxt);
   }
   /*---(calling one-line desc)----------*/
   DEBUG_PROG   yLOG_point   ("a_namesake" , a_namesake);
   --rce;  if (a_namesake != NULL) {
      strlcpy (myVIKEYS.s_namesake, a_namesake, LEN_DESC);
      DEBUG_PROG   yLOG_info    ("s_namesake", myVIKEYS.s_namesake);
   }
   /*---(default extension)--------------*/
   DEBUG_PROG   yLOG_point   ("a_ext"     , a_ext);
   --rce;  if (a_ext == NULL) {
      DEBUG_PROG   yLOG_note    ("requires a standard file extension");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (myVIKEYS.s_ext, a_ext, LEN_LABEL);
   DEBUG_PROG   yLOG_info    ("s_ext"     , myVIKEYS.s_ext);
   /*---(calling content)----------------*/
   DEBUG_PROG   yLOG_point   ("a_filetype", a_filetype);
   --rce;  if (a_filetype != NULL) {
      strlcpy (myVIKEYS.s_filetype, a_filetype, LEN_DESC);
      DEBUG_PROG   yLOG_info    ("s_filetype", myVIKEYS.s_filetype);
   }
   /*---(handlers)-----------------------*/
   DEBUG_PROG   yLOG_point   ("a_handlers", a_handlers);
   if (a_handlers != NULL)   s_handlers = a_handlers;
   /*---(writer)-------------------------*/
   DEBUG_PROG   yLOG_point   ("a_prepper" , a_prepper);
   if (a_prepper != NULL)   s_prepper = a_prepper;
   /*---(writer)-------------------------*/
   DEBUG_PROG   yLOG_point   ("a_finisher", a_finisher);
   if (a_finisher != NULL)   s_finisher = a_finisher;
   /*---(update stage)-------------------*/
   STATUS_conf_set (FMOD_FILE, '1');
   /*---(default file name)--------------*/
   yvikeys_file_name (NULL);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_file_add        (char a_abbr, void *a_writer, void *a_reader)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (FMOD_FILE)) {
      DEBUG_PROG   yLOG_note    ("init must be successfully called first");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find entry)---------------------*/
   n = yvikeys__file_by_abbr (a_abbr);
   if (n < 0)  return -1;
   DEBUG_PROG   yLOG_info    ("type"      , s_sections [n].name);
   /*---(writer)-------------------------*/
   DEBUG_PROG   yLOG_point   ("a_writer"  , a_writer);
   --rce;  if (a_writer == NULL) {
      DEBUG_PROG   yLOG_note    ("requires a data writer");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_sections [n].writer = a_writer;
   /*---(reader)-------------------------*/
   DEBUG_PROG   yLOG_point   ("a_reader"  , a_reader);
   --rce;  if (a_reader == NULL) {
      DEBUG_PROG   yLOG_note    ("requires a data reader");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_sections [n].reader = a_reader;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_file_new        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run prepper)--------------------*/
   DEBUG_INPT  yLOG_point   ("s_prepper" , s_prepper);
   if (s_prepper != NULL)  rc = s_prepper (0);
   DEBUG_OUTP   yLOG_value   ("prep"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          versioning                          ----===*/
/*====================------------------------------------====================*/
static void  o___VERSIONING______o () { return; }

char yvikeys_vers_bump_major    (void)  { return yvikeys_vers_bump ("M"); }
char yvikeys_vers_bump_minor    (void)  { return yvikeys_vers_bump ("m"); }
char yvikeys_vers_bump_inc      (void)  { return yvikeys_vers_bump ("i"); }

char         /*-> tbd --------------------------------[ ------ [ge.C70.13#.E1]*/ /*-[02.0000.03#.T]-*/ /*-[--.---.---.--]-*/
yvikeys_vers_bump          (char *a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   char        x_type      = ' ';
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense : not controlled)-------*/
   --rce;  if (myVIKEYS.f_control != 'y')        return rce;
   /*---(defense: a_type)----------------*/
   --rce;  if (a_type == NULL)                   return rce;
   --rce;  if (a_type [0] == '\0')               return rce;
   x_type = a_type [0];
   --rce;  if (strchr ("Mmi", x_type) == NULL)   return rce;
   yvikeys_vers_vertxt (NULL);
   /*---(tiny)---------------------------*/
   if (strchr ("i", x_type) != NULL) {
      if (myVIKEYS.f_vernum [3] <  'z') {
         ++myVIKEYS.f_vernum[3];
         return 0;
      }
   }
   myVIKEYS.f_vernum [3] = 'a';
   /*---(minor)--------------------------*/
   if (strchr ("mi", x_type) != NULL) {
      if (myVIKEYS.f_vernum [2] <  '9') {
         ++myVIKEYS.f_vernum[2];
         return 0;
      }
      if (myVIKEYS.f_vernum [2] == '9') {
         myVIKEYS.f_vernum  [2] =  'A';
         return 0;
      }
      if (myVIKEYS.f_vernum [2] <  'Z') {
         ++myVIKEYS.f_vernum[2];
         return 0;
      }
   }
   myVIKEYS.f_vernum [2] = '0';
   /*---(major)--------------------------*/
   if (strchr ("Mmi", x_type) != NULL) {
      if (myVIKEYS.f_vernum [0] <  '9') {
         ++myVIKEYS.f_vernum[0];
         return 0;
      }
      if (myVIKEYS.f_vernum [0] == '9') {
         myVIKEYS.f_vernum  [0] =  'A';
         return 0;
      }
      if (myVIKEYS.f_vernum [0] <  'Z') {
         ++myVIKEYS.f_vernum[0];
         return 0;
      }
   }
   /*---(complete)-----------------------*/
   strlcpy (myVIKEYS.f_vernum, "Z.Zz", LEN_LABEL);
   --rce;  return  rce;
}

char         /*-> tbd --------------------------------[ ------ [gc.520.103.41]*/ /*-[02.0000.02#.G]-*/ /*-[--.---.---.--]-*/
yvikeys_vers_controlled    (char *a_yes)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yvikeys_vers_vertxt (NULL);
   if (a_yes [0] == 'n') {
      if (myVIKEYS.f_control == 'y') {
         myVIKEYS.f_control = '-';
         strlcpy (myVIKEYS.f_vernum, "----", LEN_LABEL);
      }
      return 0;
   }
   if (a_yes [0] == 'y') {
      if (myVIKEYS.f_control == '-') {
         myVIKEYS.f_control = 'y';
         strlcpy (myVIKEYS.f_vernum, "0.0a", LEN_LABEL);
      }
      return 0;
   }
   return -1;
}

char yvikeys_vers_control       (void)  { return yvikeys_vers_controlled ("y"); }
char yvikeys_vers_nocontrol     (void)  { return yvikeys_vers_controlled ("n"); }

char         /*-> tbd --------------------------------[ ------ [ge.880.13#.G1]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_vers_version       (char *a_ver)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       = 0;
   char        rce         = -10;
   char        x_work      [10];
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense : not controlled)-------*/
   --rce;  if (myVIKEYS.f_control != 'y')   return rce;
   /*---(defense : empty)----------------*/
   --rce;  if (a_ver == NULL)               return rce;
   x_len = strlen (a_ver);
   --rce;  if (strcmp (a_ver, "") == 0)     return rce;
   --rce;  if (x_len <= 0)                  return rce;
   /*---(defense: bad length)------------*/
   --rce;  if (x_len < 4)                   return rce;
   --rce;  if (x_len > 4)                   return rce;
   /*---(prepare)------------------------*/
   strlcpy  (x_work, a_ver, LEN_LABEL);
   /*---(test chars)---------------------*/
   --rce;  if (strchr ("abcdefghijklmnopqrstuvwxyz",           x_work [3]) == 0)  return rce;
   --rce;  if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", x_work [2]) == 0)  return rce;
   --rce;  if (x_work [1] != '.')                       return rce;
   --rce;  if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", x_work [0]) == 0)  return rce;
   /*---(check increase only)------------*/
   --rce;  if (x_work [0] <  myVIKEYS.f_vernum [0])    return rce;
   if (x_work [0] == myVIKEYS.f_vernum [0]) {
      --rce;  if (x_work [2] <  myVIKEYS.f_vernum [2])    return rce;
      if (x_work [2] == myVIKEYS.f_vernum [2]) {
         --rce;  if (x_work [3] <  myVIKEYS.f_vernum [3])    return rce;
         --rce;  if (x_work [3] == myVIKEYS.f_vernum [3])    return rce;
      }
   }
   /*---(finalize)-----------------------*/
   strlcpy (myVIKEYS.f_vernum, x_work, LEN_LABEL);
   yvikeys_vers_vertxt (NULL);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.330.114.30]*/ /*-[00.0000.03#.7]-*/ /*-[--.---.---.--]-*/
yvikeys_vers_vertxt        (char *a_txt)
{
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (myVIKEYS.f_vertxt, "-----", LEN_DESC);
   --rce;  if (myVIKEYS.f_control   != 'y' )  return rce;
   --rce;  if (a_txt      == NULL)  return rce;
   --rce;  if (a_txt [0]  == '\0')  return rce;
   strlcpy  (myVIKEYS.f_vertxt, a_txt, LEN_DESC);
   strltrim (myVIKEYS.f_vertxt, ySTR_BOTH, LEN_DESC);
   return 0;
}

char
yvikeys_vers_status          (char *a_list)
{
   snprintf (a_list, LEN_FULL, "version %c [%-4.4s] [%-60.60s]", myVIKEYS.f_control, myVIKEYS.f_vernum, myVIKEYS.f_vertxt);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        file name updates                     ----===*/
/*====================------------------------------------====================*/
static void  o___NAMING__________o () { return; }

int
yvikeys__file_regex             (char a_type, char *a_ext, char *a_base, char *a_entry, char *a_match)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   DIR        *x_dir       = NULL;          /* directory pointer              */
   tDIRENT    *x_file      = NULL;          /* directory entry pointer        */
   char        x_full      [LEN_RECD];
   tSTAT       s;
   int         x_extlen    =    0;
   int         x_len       =    0;
   int         x_checked   =    0;
   int         x_matches   =    0;
   int         i           =    0;
   int        *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   DEBUG_INPT   yLOG_point   ("a_match"   , a_match);
   if (a_match != NULL)  strlcpy (a_match, "", LEN_RECD);
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_char    ("a_type"    , a_type);
   --rce;  if (strchr ("fd", a_type) == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_point   ("a_entry"   , a_entry);
   --rce;  if (a_entry == NULL || a_entry [0] == 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_delim   ("a_entry"   , a_entry);
   DEBUG_INPT   yLOG_point   ("a_ext"     , a_ext);
   if (a_type == 'f' && a_ext != NULL && a_ext [0] != '\0') {
      DEBUG_INPT   yLOG_info    ("a_ext"     , a_ext);
      x_extlen = strlen (a_ext);
      DEBUG_INPT   yLOG_value   ("x_extlen"  , x_extlen);
   }
   DEBUG_INPT   yLOG_point   ("a_bas"     , a_base);
   --rce;  if (a_base == NULL || a_base [0] == '\0') {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("a_bas"     , a_base);
   /*---(compile search)-----------------*/
   rc = yREGEX_comp (a_entry);
   DEBUG_SRCH   yLOG_value   ("comp"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_SRCH   yLOG_note    ("could not compile search");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(open dir)-----------------------*/
   x_dir = opendir (a_base);
   DEBUG_INPT   yLOG_point   ("x_dir"      , x_dir);
   --rce;  if (x_dir == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   DEBUG_INPT   yLOG_note    ("openned successfully");
   while (1) {
      DEBUG_INPT   yLOG_note    ("processing entries");
      /*---(read a directory entry)------*/
      x_file = readdir (x_dir);
      DEBUG_INPT   yLOG_point   ("x_file"    , x_file);
      if (x_file == NULL)  break;
      ++x_checked;
      /*---(filter basics)------------------*/
      x_len = strlen (x_file->d_name);
      DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
      if (x_len < 1) continue;
      DEBUG_INPT   yLOG_delim   ("->d_name"  , x_file->d_name);
      if (x_file->d_name [0] == '.' && strcmp (x_file->d_name, "..") != 0) {
         DEBUG_INPT   yLOG_note    ("hidden file, prefixed with dot (.)");
         continue;
      }
      /*---(stat it)------------------------*/
      sprintf (x_full, "%s%s", a_base, x_file->d_name);
      rc = stat (x_full, &s);
      DEBUG_INPT   yLOG_value   ("stat"      , rc);
      if (rc < 0) continue;
      DEBUG_INPT   yLOG_value   ("mode"      , s.st_mode);
      if (a_type == 'd' && !S_ISDIR (s.st_mode)) {
         DEBUG_INPT   yLOG_note    ("looking for dirs, this is not one");
         continue;
      }
      if (a_type == 'f' && !S_ISREG (s.st_mode)) {
         DEBUG_INPT   yLOG_note    ("looking for reg files, this is not one");
         continue;
      }
      /*---(filter by extension)---------*/
      if (x_extlen > 0) {
         DEBUG_INPT   yLOG_info    ("p.ext"     , x_file->d_name + x_len - x_extlen);
         if (strncmp (x_file->d_name + x_len - x_extlen, a_ext, x_extlen) != 0) {
            DEBUG_INPT   yLOG_note    ("suffix does not match, SKIP");
            continue;
         }
      }
      /*---(filter by name)--------------*/
      rc = yREGEX_exec (x_file->d_name);
      DEBUG_INPT   yLOG_value   ("exec"      , rc);
      if (rc <= 0) {
         DEBUG_INPT   yLOG_note    ("regex failed to match");
         continue;
      }
      /*---(check for bad characters)-------*/
      for (i = 0; i < x_len; ++i) {
         if (strchr (x_valid, x_file->d_name [i]) != NULL)   continue;
         DEBUG_INPT   yLOG_note    ("bad character in target name");
         continue;
      }
      /*---(save)------------------------*/
      DEBUG_INPT   yLOG_note    ("found a match");
      ++x_matches;
      if (x_matches == 1 && a_match != NULL) {
         strlcpy (a_match, x_file->d_name, LEN_RECD);
      }
      /*---(done)------------------------*/
   }
   DEBUG_INPT   yLOG_value   ("x_checked" , x_checked);
   DEBUG_INPT   yLOG_value   ("x_matches" , x_matches);
   /*---(close dir)----------------------*/
   DEBUG_INPT   yLOG_note    ("closing directory");
   rc = closedir (x_dir);
   DEBUG_INPT   yLOG_value   ("close_rc"  , rc);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return x_matches;
}

char
yvikeys__file_stripext    (char *a_loc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   int         x_extlen    =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check extension)----------------*/
   DEBUG_INPT   yLOG_info    ("s_ext"     , myVIKEYS.s_ext);
   x_extlen = strllen (myVIKEYS.s_ext, LEN_LABEL);
   DEBUG_INPT   yLOG_value   ("x_extlen"  , x_extlen);
   if (x_extlen <= 0) {
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(deal with null/empty)-----------*/
   DEBUG_INPT   yLOG_point   ("a_loc"     , a_loc);
   --rce;  if (a_loc == NULL || a_loc [0] == 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("a_loc"     , a_loc);
   /*---(check lengths)------------------*/
   x_len    = strllen (a_loc, LEN_RECD);
   DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 2) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_len < x_extlen + 1) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check extension)----------------*/
   DEBUG_INPT   yLOG_info    ("tail end"  , a_loc + x_len - x_extlen - 1);
   --rce;  if (strcmp (a_loc + x_len - x_extlen, myVIKEYS.s_ext) != 0)  {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_char    ("dot place" , a_loc [x_len - x_extlen - 1]);
   --rce;  if (a_loc [x_len - x_extlen - 1] != '.') {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update)-------------------------*/
   a_loc [x_len - x_extlen - 1] = '\0';
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__file_path      (char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_curr      [LEN_RECD];
   char        x_work      [LEN_RECD];
   char        t           [LEN_RECD];
   char        x_final     [LEN_RECD];
   int         x_len       =    0;
   char       *p           = NULL;               /* strtok pointer            */
   char       *q           = "/";                /* strtok delimeters         */
   char       *r           = NULL;               /* strtok context variable   */
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(deal with null/empty)-----------*/
   DEBUG_INPT   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL || a_path [0] == 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("a_path"    , a_path);
   strlcpy (x_work, a_path, LEN_RECD);
   /*---(absolute marker)----------------*/
   getcwd (x_curr, LEN_RECD);
   if (strncmp (x_work, "./", 2) == 0) {
      strlcpy (x_final, x_curr, LEN_RECD);
      strlcat (x_final, "/"   , LEN_RECD);
      x_work [0] = '/';
   } else if (x_work [0] == '/')  strlcpy (x_final, "/", LEN_RECD);
   else                           strlcpy (x_final, myVIKEYS.f_loc, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("x_final"   , x_final);
   x_len = strlen (x_final);
   /*---(loop)---------------------------*/
   p = strtok_r (x_work, q, &r);
   --rce;  while (p != NULL) {
      /*---(head)------------------------*/
      DEBUG_INPT   yLOG_info    ("p"         , p);
      /*---(goto parent)-----------------*/
      if (strcmp (p, "..") == 0) {
         if (x_len > 1)  x_final [--x_len] = '\0';
         p = strrchr (x_final, '/');
         if (p != NULL)  p [1] = '\0';
      }
      /*---(goto child)------------------*/
      else {
         rc = yvikeys__file_regex ('d', NULL, x_final, p, t);
         DEBUG_INPT   yLOG_value   ("matches"   , rc);
         if (rc != 1)  {
            DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         strlcat (x_final, t  , LEN_RECD);
         strlcat (x_final, "/", LEN_RECD);
      }
      /*---(next)------------------------*/
      DEBUG_INPT   yLOG_info    ("x_final"   , x_final);
      ++c;
      x_len = strlen (x_final);
      p = strtok_r (NULL, q, &r);
      /*---(done)------------------------*/
   }
   /*---(save)---------------------------*/
   DEBUG_INPT   yLOG_value   ("levels"    , c);
   strlcpy (a_path, x_final, LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return c;
}

char         /*-> tbd --------------------------------[ leaf   [gc.C55.124.30]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_temploc           (char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSTAT       s;
   int         x_len       =    0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(deal with null/empty)-----------*/
   DEBUG_INPT   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL || a_path [0] == 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("a_path"    , a_path);
   /*---(update)-------------------------*/
   rc = yvikeys__file_path (a_path);
   DEBUG_HIST   yLOG_value   ("path"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.C55.124.30]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys_file_loc                (char *a_path)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSTAT       s;
   int         x_len       =    0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(deal with null/empty)-----------*/
   DEBUG_INPT   yLOG_point   ("a_path"    , a_path);
   --rce;  if (a_path == NULL || a_path [0] == 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("a_path"    , a_path);
   /*---(save)---------------------------*/
   strlcpy (t, a_path, LEN_RECD);
   rc = yvikeys__file_path (t);
   DEBUG_HIST   yLOG_value   ("path"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   strlcpy (myVIKEYS.f_loc  , t, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.C55.124.30]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_namer     (char a_type, char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        x_work      [LEN_RECD]   = "";
   char        x_dir       [LEN_RECD]   = "";
   char        t           [LEN_RECD];
   char       *p           = NULL;
   int         x_len       =    0;
   int        *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(deal with empties)--------------*/
   DEBUG_INPT   yLOG_point   ("a_name"    , a_name);
   if (a_name == NULL || a_name [0] == 0) {
      DEBUG_INPT   yLOG_note    ("a_name was null, using defaults");
      strlcpy (myVIKEYS.f_name , FILE_BLANK, LEN_RECD);
      sprintf (myVIKEYS.f_title, "%s%s.%s"   , myVIKEYS.f_loc, myVIKEYS.f_name, myVIKEYS.s_ext);
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_name [0] == '/')  strlcpy  (x_work, a_name, LEN_RECD);
   else                    snprintf (x_work, LEN_RECD, "%s%s", myVIKEYS.f_loc, a_name);
   DEBUG_INPT   yLOG_info    ("x_work"    , x_work);
   /*---(strip extensions)---------------*/
   rc = yvikeys__file_stripext (x_work);
   DEBUG_INPT   yLOG_value   ("stripext"  , rc);
   /*---(divide out location)------------*/
   p = strrchr (x_work, '/');
   DEBUG_INPT   yLOG_point   ("p"         , p);
   --rce;  if (p != NULL) {
      DEBUG_INPT   yLOG_note    ("fully qualified name, with directory");
      *p = 0;
      sprintf (x_dir, "%s/", x_work, LEN_RECD);
      rc = yvikeys__file_temploc (x_dir);
      if (rc < 0) {
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rc;
      }
   } else {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for bad characters)-------*/
   p++;
   x_len = strllen (p, LEN_RECD);
   DEBUG_HIST   yLOG_value   ("x_len"     , x_len);
   /*---(check for regex)----------------*/
   --rce;  if (a_type == 'r' && x_len > 0) {
      if (x_len <= 0) {
         DEBUG_INPT   yLOG_note    ("file regex empty");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      rc = yvikeys__file_regex ('f', myVIKEYS.s_ext, x_dir, p, t);
      if (rc <= 0) {
         DEBUG_INPT   yLOG_note    ("regex not found");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      if (rc >  1) {
         DEBUG_INPT   yLOG_note    ("too many matches found");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      rc = yvikeys__file_stripext (t);
      strlcpy (myVIKEYS.f_name , t, LEN_RECD);
   }
   /*---(check for fixed name)-----------*/
   else if (x_len > 0) {
      for (i = 0; i < x_len; ++i) {
         if (strchr (x_valid, p [i]) != NULL)   continue;
         DEBUG_INPT   yLOG_note    ("bad character in file name");
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      strlcpy (myVIKEYS.f_name , p         , LEN_RECD);
   }
   /*---(check for empty, again)---------*/
   else {
      strlcpy (myVIKEYS.f_name , FILE_BLANK, LEN_RECD);
   }
   /*---(report out)---------------------*/
   sprintf (myVIKEYS.f_title, "%s%s.%s", x_dir, myVIKEYS.f_name, myVIKEYS.s_ext);
   DEBUG_INPT   yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   DEBUG_INPT   yLOG_info    ("f_name"    , myVIKEYS.f_name);
   DEBUG_INPT   yLOG_info    ("f_title"   , myVIKEYS.f_title);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_file_name   (char *a_name) { return yvikeys__file_namer ('-', a_name); }
char yvikeys_file_browse (char *a_name) { return yvikeys__file_namer ('r', a_name); }



/*====================------------------------------------====================*/
/*===----                       shared functions                       ----===*/
/*====================------------------------------------====================*/
static void  o___SHARED__________o () { return; }

char         /*-> open file for reading and prep -----[ leaf   [ge.723.023.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_open      (char *a_dir)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_info    ("f_title"   , myVIKEYS.f_title);
   /*---(open file)----------------------*/
   s_file = fopen (myVIKEYS.f_title, a_dir);
   DEBUG_INPT  yLOG_point   ("s_file"    , s_file);
   --rce;  if (s_file == NULL) {
      DEBUG_INPT  yLOG_note    ("file could not be openned");
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_note    ("file successfully opened");
   /*---(prepare)------------------*/
   DEBUG_INPT  yLOG_note    ("set file globals");
   myVIKEYS.f_lines = 0;
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> close file for reading and wrap ----[ leaf   [ge.411.011.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_close     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(close file)---------------------*/
   if (s_file == NULL) {
      DEBUG_INPT  yLOG_note    ("no file to close");
   } else {
      DEBUG_INPT  yLOG_note    ("close file");
      fclose  (s_file);
   }
   s_file == NULL;
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        output related                        ----===*/
/*====================------------------------------------====================*/
static void  o___OUTPUT__________o () { return; }

char         /*-> write file header ------------------[ leaf   [ge.850.154.20]*/ /*-[03.0000.01#.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_intro      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;          /* generic return code            */
   int         i           =    0;          /* iterator -- tab                */
   char        x_temp      [100];
   time_t      x_time;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(write header)---------------------*/
   yPARSE_section ("GENERAL");
   /*---(format identifiers)---------------*/
   rc = yPARSE_vprintf (-1, "source"   , myVIKEYS.s_prog, myVIKEYS.s_vernum, myVIKEYS.s_vertxt);
   DEBUG_INPT   yLOG_value   ("source"    , rc);
   /*---(timestamp)------------------------*/
   x_time = time (NULL);
   strftime (x_temp, 100, "%Y.%m.%d.%H.%M.%S", localtime (&x_time));
   rc = yPARSE_vprintf (-1, "written"  , x_temp);
   DEBUG_INPT   yLOG_value   ("written"   , rc);
   /*---(version)------------------------*/
   if (myVIKEYS.f_control == 'y') {
      rc = yPARSE_vprintf (-1, "version"  , myVIKEYS.f_vernum, myVIKEYS.f_vertxt);
      DEBUG_INPT   yLOG_value   ("version"   , rc);
   } else {
      rc = yPARSE_vprintf (-1, "version"  , "n/a", "untracked");
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
yvikeys__file_columns   (char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rce         =  -10;
   int         i           =    0;
   int         x_label     [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   DEBUG_INPT   yLOG_point   ("s_file"    , s_file);
   DEBUG_INPT   yLOG_value   ("a_index"   , a_index);
   /*---(defense)------------------------*/
   --rce;  if (s_file == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prefix)-------------------------*/
   fprintf (s_file, "#---------  ver ");
   /*---(columns)------------------------*/
   DEBUG_INPT   yLOG_info    ("specs"     , s_sections [a_index].specs);
   for (i = 0; i < 10; ++i) {
      if (s_sections [a_index].specs [i] == '-')  break;
      sprintf (x_label, "%s%-100.100s", s_sections [a_index].column [i], "-----------------------------------------------------------------------------");
      switch (s_sections [a_index].specs [i]) {
      case  'c'  :
         fprintf (s_file, " %-3.3s "  , x_label);
         break;
      case  'a'  :
         fprintf (s_file, " %-12.12s ", x_label);
         break;
      case  'T'  :
         fprintf (s_file, " %-10.10s ", x_label);
         break;
      case  'N'  :
         fprintf (s_file, " %-20.20s ", x_label);
         break;
      case  'D'  :
         fprintf (s_file, " %-60.60s ", x_label);
         break;
      case  'O'  :
         fprintf (s_file, " %s "      , x_label);
         break;
      case  'i'  :
         fprintf (s_file, " %-5.5s "  , x_label);
         break;
      case  'f'  :
         fprintf (s_file, " %-10.10s ", x_label);
         break;
      default    :
         fprintf (s_file, " ?%s? "     , x_label);
         break;
      }
   }
   /*---(suffix)-------------------------*/
   fprintf (s_file, "\n");
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_file_write      (char a_abbr, void *a, void *b, void *c, void *d, void *e, void *f, void *g, void *h, void *i)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   int         x           =    0;
   char        t           [LEN_RECD ];
   void       *x_field     [9];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(get section)-----------------*/
   n = yvikeys__file_by_abbr (a_abbr);
   DEBUG_OUTP   yLOG_value   ("n"         , n);
   --rce;  if (n < 0)  {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)---------------------*/
   x_field [0] = a;
   x_field [1] = b;
   x_field [2] = c;
   x_field [3] = d;
   x_field [4] = e;
   x_field [5] = f;
   x_field [6] = g;
   x_field [7] = h;
   x_field [8] = i;
   /*---(columns)---------------------*/
   DEBUG_OUTP   yLOG_info    ("label"     , s_sections [n].label);
   DEBUG_OUTP   yLOG_char    ("ver"       , s_sections [n].version);
   DEBUG_OUTP   yLOG_info    ("specs"     , s_sections [n].specs);
   sprintf (myVIKEYS.f_recd, "%-10.10s  -%c- ", s_sections [n].label, s_sections [n].version);
   DEBUG_OUTP   yLOG_note    ("write fields");
   for (x = 0; x < 9; ++x) {
      DEBUG_OUTP   yLOG_value   ("x"         , x);
      DEBUG_OUTP   yLOG_char    ("spec"      , s_sections [n].specs [x]);
      if (s_sections [n].specs [x] == '-')  break;
      DEBUG_OUTP   yLOG_point   ("x_field"   , x_field [x]);
      if (x_field [x] == NULL)  break;
      strlcpy (t, "", LEN_LABEL);
      switch (s_sections [n].specs [x]) {
      case  'c'  :
         DEBUG_OUTP   yLOG_note    ("character");
         sprintf (t, "  %c  "     , *((char   *) x_field [x]));
         break;
      case  'T'  :
         DEBUG_OUTP   yLOG_note    ("terse string");
         sprintf (t, " %-10.10s ", (char *)      x_field [x]);
         break;
      case  'a'  :
         DEBUG_OUTP   yLOG_note    ("address");
         sprintf (t, " %-12.12s ", (char *)      x_field [x]);
         break;
      case  'N'  :
         DEBUG_OUTP   yLOG_note    ("name string");
         sprintf (t, " %-20.20s " , (char *)     x_field [x]);
         break;
      case  'D'  :
         DEBUG_OUTP   yLOG_note    ("desc string");
         sprintf (t, " %-60.60s " , (char *)     x_field [x]);
         break;
      case  'O'  :
         DEBUG_OUTP   yLOG_note    ("open string");
         sprintf (t, " %s "       , (char *)     x_field [x]);
         break;
      case  'i'  :
         DEBUG_OUTP   yLOG_note    ("integer");
         sprintf (t, " %5d "      , *((int    *) x_field [x]));
         break;
      case  'f'  :
         DEBUG_OUTP   yLOG_note    ("long float");
         sprintf (t, " %-10.3lf " , *((double *) x_field [x]));
         break;
      default    :
         DEBUG_OUTP   yLOG_note    ("unknown");
         sprintf (t, " ??? ");
         break;
      }
      DEBUG_OUTP   yLOG_info    ("t"         , t);
      strlcat (myVIKEYS.f_recd, t, LEN_RECD);
   }
   DEBUG_OUTP   yLOG_info    ("f_recd"    , myVIKEYS.f_recd);
   ++s_lines;
   DEBUG_OUTP   yLOG_value   ("s_lines"   , s_lines);
   /*---(write)--------------------------*/
   DEBUG_OUTP   yLOG_point   ("s_file"    , s_file);
   if (s_file != NULL) {
      DEBUG_OUTP   yLOG_note    ("write to file");
      fprintf (s_file, "%s\n", myVIKEYS.f_recd);
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 1;
}

/*> static char                                                                                                                                                                                                            <* 
 *> OUTP_writer             (char n, int a_entry)                                                                                                                                                                          <* 
 *> {                                                                                                                                                                                                                      <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                                                                                                            <* 
 *>    char        rce         =  -10;                                                                                                                                                                                     <* 
 *>    char        rc          =    0;                                                                                                                                                                                     <* 
 *>    int         i           =    0;                                                                                                                                                                                     <* 
 *>    int        *x_field     [9];                                                                                                                                                                                        <* 
 *>    char        t           [LEN_RECD ];                                                                                                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                                                                                                            <* 
 *>    DEBUG_INPT   yLOG_enter   (__FUNCTION__);                                                                                                                                                                           <* 
 *>    DEBUG_INPT   yLOG_value   ("n"         , n);                                                                                                                                                                        <* 
 *>    DEBUG_INPT   yLOG_value   ("a_entry"   , a_entry);                                                                                                                                                                  <* 
 *>    /+---(gather next)-----------------+/                                                                                                                                                                               <* 
 *>    strlcpy (myVIKEYS.f_recd, "", LEN_RECD);                                                                                                                                                                            <* 
 *>    /+> if (s_sections [n].writer != NULL)  rc = s_sections [n].writer (i, x_field [0], x_field [1], x_field [2], x_field [3], x_field [4], x_field [5], x_field [6], x_field [7], x_field [8]);   <+/                  <* 
 *>    for (i = 0; i < 9; ++i) x_field [i] = NULL;                                                                                                                                                                         <* 
 *>    /+> printf ("file   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <+/   <* 
 *>    /+> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <+/   <* 
 *>    /+> rc = VISU_writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <+/                                                 <* 
 *>    DEBUG_INPT   yLOG_point   ("writer"    , s_sections [n].writer);                                                                                                                                                    <* 
 *>    rc = s_sections [n].writer.a (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);                                               <* 
 *>    DEBUG_INPT   yLOG_value   ("rc"        , rc);                                                                                                                                                                       <* 
 *>    --rce;  if (rc <= 0) {                                                                                                                                                                                              <* 
 *>       DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                                                                                                                                                                   <* 
 *>       return rc;                                                                                                                                                                                                       <* 
 *>    }                                                                                                                                                                                                                   <* 
 *>    /+> printf ("after  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <+/   <* 
 *>    /+> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <+/   <* 
 *>    rc = yVIKEYS_file_write (n, x_field);                                                                                                                                                                               <* 
 *>    DEBUG_INPT   yLOG_value   ("rc"        , rc);                                                                                                                                                                       <* 
 *>    /+---(complete)-----------------------+/                                                                                                                                                                            <* 
 *>    DEBUG_INPT   yLOG_exit    (__FUNCTION__);                                                                                                                                                                           <* 
 *>    return rc;                                                                                                                                                                                                          <* 
 *> }                                                                                                                                                                                                                      <*/

int 
OUTP__write_type         (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   int         n           =   -1;
   char        x_upper     [LEN_LABEL];
   void       *x_field     [9];
   char        t           [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_OUTP   yLOG_info    ("a_name"    , a_name);
   /*---(find entry)---------------------*/
   /*> n = FILE__by_name (a_name);                                                    <*/
   DEBUG_OUTP   yLOG_value   ("n"         , n);
   if (n < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(write header)-------------------*/
   DEBUG_OUTP   yLOG_info    ("name"      , s_sections [n].name);
   strlcpy (x_upper, s_sections [n].name, LEN_LABEL);
   for (i = 0; i < strllen (x_upper, LEN_LABEL); ++i)  x_upper [i] = toupper (x_upper [i]);
   DEBUG_OUTP   yLOG_info    ("upper"     , x_upper);
   if (s_file != NULL) {
      DEBUG_OUTP   yLOG_note    ("write the header");
      fprintf (s_file, "\n\n\n#===[[ %-20.20s ]]===============================================================================================#\n",
            x_upper);
      yvikeys__file_columns (n);
   }
   /*---(write entries)------------------*/
   s_lines = 0;
   rc = s_sections [n].writer ();
   DEBUG_OUTP   yLOG_value   ("rc"        , rc);
   DEBUG_OUTP   yLOG_value   ("s_lines"   , s_lines);
   /*---(write footer)-------------------*/
   if (s_file != NULL) {
      DEBUG_OUTP   yLOG_note    ("write the footer");
      if (s_lines == 0)  fprintf (s_file, "# no %s\n", s_sections [n].name);
      else               fprintf (s_file, "# complete with %d line(s)\n", s_lines);
      fflush  (s_file);
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return s_lines;
}

char
yvikeys_file_prog_writer (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(header)-------------------------*/
   yPARSE_section ("INTRODUCTION");
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(write line)---------------------*/
   rc = yPARSE_vprintf (-1, "source"   , myVIKEYS.s_prog, myVIKEYS.s_vernum, myVIKEYS.s_vertxt);
   DEBUG_INPT   yLOG_value   ("source"    , rc);
   --rce; if (rc < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_file_time_writer (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_temp      [100];
   time_t      x_time;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(timestamp)------------------------*/
   x_time = time (NULL);
   strftime (x_temp, 100, "%y.%m.%d.%H.%M.%S.%V.%w", localtime (&x_time));
   rc = yPARSE_vprintf (-1, "written"  , x_temp);
   DEBUG_INPT   yLOG_value   ("written"   , rc);
   --rce; if (rc < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_file_vers_writer (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(version)------------------------*/
   if (myVIKEYS.f_control == 'y') {
      rc = yPARSE_vprintf (-1, "version"  , 'y', myVIKEYS.f_vernum, myVIKEYS.f_vertxt);
   } else {
      rc = yPARSE_vprintf (-1, "version"  , '-', "n/a", "untracked");
   }
   DEBUG_INPT   yLOG_value   ("version"   , rc);
   --rce; if (rc < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_file_writer     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   /*---(stage check)-----------------*/
   yURG_stage_check (YURG_STAGE_OUTP);
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(open file)----------------------*/
   DEBUG_INPT  yLOG_info    ("f_title"   , myVIKEYS.f_title);
   DEBUG_INPT  yLOG_info    ("s_fullname", myVIKEYS.s_fullname);
   DEBUG_INPT  yLOG_info    ("s_namesake", myVIKEYS.s_namesake);
   rc = yPARSE_planned (NULL, myVIKEYS.f_title, NULL);
   DEBUG_OUTP   yLOG_value   ("open"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(load handlers)------------------*/
   rc = yvikeys_file_handlers ();
   if (s_handlers != NULL)  rc = s_handlers ();
   /*---(header)-------------------------*/
   rc = yPARSE_header  (myVIKEYS.s_fullname, myVIKEYS.s_vernum, myVIKEYS.s_vertxt, myVIKEYS.s_namesake, myVIKEYS.s_filetype, NULL);
   /*---(intro)--------------------------*/
   rc = yPARSE_write_all ();
   DEBUG_OUTP   yLOG_value   ("yparse"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(close file)---------------------*/
   rc = yPARSE_close_out ();
   DEBUG_OUTP   yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.320.121.32]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
yvikeys_file_writeas    (char *a_name)
{
   char        rc          =    0;
   char        x_name      [LEN_RECD]  = "";
   strlcpy (x_name, myVIKEYS.f_name, LEN_RECD);
   if (rc >= 0)  rc = yvikeys_file_name   (a_name);
   if (rc >= 0)  rc = yvikeys_file_writer ();
   yvikeys_file_name   (x_name);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         input related                        ----===*/
/*====================------------------------------------====================*/
static void  o___INPUT___________o () { return; }

char         /*-> file reading driver ----------------[ leaf   [ge.851.163.30]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
INPT__parse              (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char       *p           = NULL;               /* strtok pointer            */
   char       *q           = "";               /* strtok delimeters         */
   char       *r           = NULL;               /* strtok context variable   */
   /*---(cleanse)---------------------*/
   for (s_nfield = 0; s_nfield < 20; ++s_nfield) {
      strlcpy (s_fields [s_nfield], "", LEN_RECD);
   }
   s_nfield = 0;
   strlcpy (x_recd, myVIKEYS.f_recd, LEN_RECD);
   /*---(parse fields)----------------*/
   p = strtok_r (x_recd, q, &r);
   while (p != NULL) {
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      DEBUG_INPT  yLOG_bullet  (s_nfield    , p);
      strlcpy   (s_fields [s_nfield++], p, LEN_RECD);
      p = strtok_r (NULL     , q, &r);
   }
   --rce;  if (s_nfield < 1) {
      DEBUG_INPT  yLOG_note    ("no fields found");
      return -rce;
   }
   strlcpy   (myVIKEYS.f_type, s_fields [0], LEN_RECD);
   DEBUG_INPT  yLOG_info    ("type"      , myVIKEYS.f_type);
   if (s_fields [1][0] == '-' && s_fields [1][2] == '-')
      myVIKEYS.f_vers  = s_fields [1][1];
   else
      myVIKEYS.f_vers  = '?';
   DEBUG_INPT  yLOG_char    ("vers"      , myVIKEYS.f_vers);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_file_reader     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         x_pass      =    1;
   char        x_max       =    1;
   /*---(stage check)-----------------*/
   yURG_stage_check (YURG_STAGE_INPT);
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (FMOD_FILE)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(multi-pass)---------------------*/
   while (x_pass <= x_max) {
      /*---(run prepper)----------*/
      DEBUG_INPT  yLOG_point   ("s_prepper" , s_prepper);
      if (s_prepper != NULL)  rc = s_prepper (x_pass);
      DEBUG_OUTP   yLOG_value   ("prep"      , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      x_max = rc;
      /*---(open file)------------*/
      DEBUG_INPT  yLOG_info    ("f_title"   , myVIKEYS.f_title);
      rc = yPARSE_planned (myVIKEYS.f_title, NULL, NULL);
      DEBUG_OUTP   yLOG_value   ("open"      , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(load handlers)--------*/
      rc = yvikeys_file_handlers ();
      DEBUG_OUTP   yLOG_value   ("handlers"  , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      if (s_handlers != NULL)  rc = s_handlers ();
      DEBUG_OUTP   yLOG_value   ("locals"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(intro)----------------*/
      rc = yPARSE_read_all ();
      DEBUG_OUTP   yLOG_value   ("yparse"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(close file)-----------*/
      rc = yPARSE_close ();
      DEBUG_OUTP   yLOG_value   ("close"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(run finisher)---------*/
      DEBUG_INPT  yLOG_point   ("s_finisher", s_finisher);
      if (s_finisher != NULL)  rc = s_finisher (x_pass);
      DEBUG_OUTP   yLOG_value   ("prep"      , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(pass done)------------*/
      ++x_pass;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_unit_reset      (void)
{
   strlcpy (myVIKEYS.f_recd, "", LEN_RECD);
   return 0;
}




/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
FILE__unit         (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_name      [LEN_LABEL];
   char        t           [LEN_DESC ];
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "FILE             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "ver_num"   )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE ver_num     : %s", myVIKEYS.f_vernum);
   } else if (strcmp (a_question, "version"   )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE version     : %c %-4.4s %s", myVIKEYS.f_control, myVIKEYS.f_vernum, myVIKEYS.f_vertxt);
   } else if (strcmp (a_question, "recd"      )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE recd        : %s", myVIKEYS.f_recd);
   } else if (strcmp (a_question, "loc"       )    == 0) {
      if (myVIKEYS.f_loc [0] == 0)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE loc         : -");
      else                          snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE loc         : %s", myVIKEYS.f_loc);
   } else if (strcmp (a_question, "title"     )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "FILE title       : %s", myVIKEYS.f_title);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


