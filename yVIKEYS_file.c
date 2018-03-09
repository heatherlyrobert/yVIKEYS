/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char s_file_status   = G_STAGE_NULL;

/*
 *     c   = char
 *     L   = label  (10 char)
 *     N   = name   (20 char)
 *     D   = desc   (60 char)
 *     S   = string (no trunc)
 *     d   = 5 decimal
 *
 *
 */
/*---(globals)----------+-----------+-*/
char        ver_ctrl    = '-';
char        ver_num     [10]        = "----";
char        ver_txt     [100]       = "-----";

#define     FILE_BLANK  "untitled"

#define     MAX_SECTION   50
typedef struct  cSECTION  tSECTION;
struct cSECTION {
   char        type;
   char        abbr;
   char        name        [LEN_DESC ];
   char        label       [LEN_LABEL];
   char        version;
   char        specs       [LEN_LABEL];
   char        column      [ 9][LEN_LABEL];
   char        (*writer) (int , void*,void*,void*,void*,void*,void*,void*,void*,void*);
   char        (*reader) (char, void*,void*,void*,void*,void*,void*,void*,void*,void*);
   int         try;
   int         bad;
};
static tSECTION  s_sections [MAX_SECTION] = {
   /* -   ----abbr---    ---name------------    --label---   ver   ---specs--    ---1st----    ---2nd----    ---3rd----    ---4th----    ---5th----    ---6th----    ---7th----    ---8th----    ---9th----   writer---------  reader---------  try  bad */
   { 's', SMOD_MARK   , "location marks"     , "loc_mark"  , 'B', "ciii------", "a"         , "x"         , "y"         , "z"         , ""          , ""          , ""          , ""          , ""          , MARK_writer    , MARK_reader    ,   0,   0 },
   { 's', SMOD_VISUAL , "visual marks"       , "visu_mark" , 'A', "ciiiiii---", "a"         , "xbeg"      , "ybeg"      , "zbeg"      , "xend"      , "yend"      , "zend"      , ""          , ""          , VISU_writer    , VISU_reader    ,   0,   0 },
   { 's', MODE_SEARCH , "search history"     , "search"    , 'A', "ciiS------", "a"         , "count"     , "found"     , "search"    , ""          , ""          , ""          , ""          , ""          , SRCH_writer    , SRCH_reader    ,   0,   0 },
   { 's', MODE_COMMAND, "command history"    , "command"   , 'A', "ciiS------", "a"         , "count"     , "rc"        , "search"    , ""          , ""          , ""          , ""          , ""          , CMDS_writer    , CMDS_reader    ,   0,   0 },
   { '-', 0           , ""                   , ""          , '-', "----------", ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , NULL           , NULL           ,   0,   0 },
};
static int  s_nsection   = 0;


static FILE    *s_file;                      /* file pointer                   */
static char     s_fields    [20][LEN_RECD];
static int      s_nfield    =   0;
static char     s_prog      [LEN_LABEL]   = "-";
static char     s_ext       [LEN_LABEL]   = "";
static char     s_vernum    [LEN_LABEL]   = "-.--";
static char     s_vertxt    [LEN_DESC ]   = "----";


static char    *s_valid     = "csLDSif-";



/*====================------------------------------------====================*/
/*===----                          program level                       ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
FILE_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(add commands)-------------------*/
   rc = yVIKEYS_cmds_add ('f', "control"     , ""    , ""     , FILE_control         , "turn version control ON for current file"                    );
   rc = yVIKEYS_cmds_add ('f', "nocontrol"   , ""    , ""     , FILE_nocontrol       , "turn version control OFF for current file"                   );
   rc = yVIKEYS_cmds_add ('f', "version"     , ""    , "s"    , FILE_version         , "set a specific file version ([0-9A-Z].[0-9A-Z][a-z])"        );
   rc = yVIKEYS_cmds_add ('f', "vertxt"      , ""    , "a"    , FILE_vertxt          , "set a file version description"                              );
   rc = yVIKEYS_cmds_add ('f', "major"       , ""    , ""     , FILE_bump_major      , "increment the version number by a MAJOR version"             );
   rc = yVIKEYS_cmds_add ('f', "minor"       , ""    , ""     , FILE_bump_minor      , "increment the version number by a MINOR version"             );
   rc = yVIKEYS_cmds_add ('f', "bump"        , ""    , ""     , FILE_bump_inc        , "increment the version number by a INC version"               );
   /*---(reset globals)------------------*/
   strlcpy (ver_num , "----" , LEN_LABEL);
   strlcpy (ver_txt , "-----", LEN_DESC );
   strlcpy (s_prog  , "-"    , LEN_DESC );
   strlcpy (s_ext   , "-"    , LEN_DESC );
   strlcpy (s_vernum, "-"    , LEN_DESC );
   strlcpy (s_vertxt, "-"    , LEN_DESC );
   /*---(update status)------------------*/
   s_file_status = G_STAGE_INIT;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_file_config     (char *a_prog, char *a_ext, char *a_vernum, char *a_vertxt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_value   ("stage"     , s_file_status);
   --rce;  if (s_file_status <  G_STAGE_INIT) {
      DEBUG_INPT   yLOG_note    ("must be called after init");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(program name)-------------------*/
   DEBUG_INPT   yLOG_point   ("a_prog"    , a_prog);
   --rce;  if (a_prog == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a standard file extension");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_prog, a_prog, LEN_LABEL);
   DEBUG_INPT   yLOG_info    ("s_prog"    , s_prog);
   /*---(default extension)--------------*/
   DEBUG_INPT   yLOG_point   ("a_ext"     , a_ext);
   --rce;  if (a_ext == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a standard file extension");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_ext, a_ext, LEN_LABEL);
   DEBUG_INPT   yLOG_info    ("s_ext"     , s_ext);
   /*---(calling program version)--------*/
   DEBUG_INPT   yLOG_point   ("a_vernum"  , a_vernum);
   --rce;  if (a_vernum != NULL) {
      strlcpy (s_vernum, a_vernum, LEN_LABEL);
      DEBUG_INPT   yLOG_info    ("s_vernum"  , s_vernum);
   }
   /*---(calling program desc)-----------*/
   DEBUG_INPT   yLOG_point   ("a_vertxt"  , a_vertxt);
   --rce;  if (a_vertxt != NULL) {
      strlcpy (s_vertxt, a_vertxt, LEN_DESC);
      DEBUG_INPT   yLOG_info    ("s_vertxt"  , s_vertxt);
   }
   /*---(update stage)-------------------*/
   s_file_status = G_STAGE_READY;
   /*---(default file name)--------------*/
   FILE_loc  (NULL);
   FILE_name (NULL);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_file_add        (char *a_label, char *a_name, char a_ver, void *a_writer, void *a_reader, char *a_specs, char *a_title [9][LEN_LABEL])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_value   ("stage"     , s_file_status);
   --rce;  if (s_file_status <  G_STAGE_INIT) {
      DEBUG_INPT   yLOG_note    ("must be called after init");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(label)--------------------------*/
   DEBUG_INPT   yLOG_point   ("a_label"   , a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a label for data type");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_sections [s_nsection].label, a_label, LEN_LABEL);
   DEBUG_INPT   yLOG_info    ("a_label"   , s_sections [s_nsection].label);
   /*---(name)---------------------------*/
   --rce;  if (a_name != NULL) {
      strlcpy (s_sections [s_nsection].name, a_name, LEN_DESC);
   } else {
      strlcpy (s_sections [s_nsection].name, a_label, LEN_DESC);
   }
   DEBUG_INPT   yLOG_info    ("a_name"    , s_sections [s_nsection].name);
   /*---(version)------------------------*/
   DEBUG_INPT   yLOG_value   ("a_ver"     , a_ver);
   --rce;  if (a_ver == 0 || strchr ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", a_ver) == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a correct version number");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_sections [s_nsection].version = a_ver;
   DEBUG_INPT   yLOG_char    ("a_ver"     , s_sections [s_nsection].version);
   /*---(writer)-------------------------*/
   DEBUG_INPT   yLOG_point   ("a_writer"  , a_writer);
   --rce;  if (a_writer == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a data writer");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_sections [s_nsection].writer = a_writer;
   /*---(reader)-------------------------*/
   DEBUG_INPT   yLOG_point   ("a_reader"  , a_reader);
   --rce;  if (a_reader == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a data reader");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_sections [s_nsection].reader = a_reader;
   /*---(label)--------------------------*/
   DEBUG_INPT   yLOG_point   ("a_specs"   , a_specs);
   --rce;  if (a_specs == NULL) {
      DEBUG_INPT   yLOG_note    ("requires a spec list");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_sections [s_nsection].specs, a_specs, LEN_LABEL);
   DEBUG_INPT   yLOG_info    ("a_specs"   , s_sections [s_nsection].specs);
   /*---(titles)-------------------------*/
   for (i = 0; i < 9; ++i) {
      if (a_title [i] != NULL) {
         strlcpy (s_sections [s_nsection].column [i], a_title [i], LEN_LABEL);
      } else {
         strlcpy (s_sections [s_nsection].column [i], ""         , LEN_LABEL);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                          versioning                          ----===*/
/*====================------------------------------------====================*/
static void  o___VERSIONING______o () { return; }

char FILE_bump_major    (void)  { return FILE_bump ("M"); }
char FILE_bump_minor    (void)  { return FILE_bump ("m"); }
char FILE_bump_inc      (void)  { return FILE_bump ("i"); }

char         /*-> tbd --------------------------------[ ------ [ge.C70.13#.E1]*/ /*-[02.0000.03#.T]-*/ /*-[--.---.---.--]-*/
FILE_bump          (char *a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   char        x_type      = ' ';
   /*---(defense : not controlled)-------*/
   if (s_file_status <  G_STAGE_READY)   return -66;
   --rce;  if (ver_ctrl != 'y')  return rce;
   /*---(defense: a_type)----------------*/
   --rce;  if (a_type == NULL)                   return rce;
   --rce;  if (a_type [0] == '\0')               return rce;
   x_type = a_type [0];
   --rce;  if (strchr ("Mmi", x_type) == NULL)   return rce;
   FILE_vertxt (NULL);
   /*---(tiny)---------------------------*/
   if (strchr ("i", x_type) != NULL) {
      if (ver_num [3] <  'z') {
         ++ver_num[3];
         return 0;
      }
   }
   ver_num [3] = 'a';
   /*---(minor)--------------------------*/
   if (strchr ("mi", x_type) != NULL) {
      if (ver_num [2] <  '9') {
         ++ver_num[2];
         return 0;
      }
      if (ver_num [2] == '9') {
         ver_num  [2] =  'A';
         return 0;
      }
      if (ver_num [2] <  'Z') {
         ++ver_num[2];
         return 0;
      }
   }
   ver_num [2] = '0';
   /*---(major)--------------------------*/
   if (strchr ("Mmi", x_type) != NULL) {
      if (ver_num [0] <  '9') {
         ++ver_num[0];
         return 0;
      }
      if (ver_num [0] == '9') {
         ver_num  [0] =  'A';
         return 0;
      }
      if (ver_num [0] <  'Z') {
         ++ver_num[0];
         return 0;
      }
   }
   /*---(complete)-----------------------*/
   strlcpy (ver_num, "Z.Zz", LEN_LABEL);
   --rce;  return  rce;
}

char FILE_control       (void)  { return FILE_controlled ("y"); }
char FILE_nocontrol     (void)  { return FILE_controlled ("n"); }

char         /*-> tbd --------------------------------[ ------ [gc.520.103.41]*/ /*-[02.0000.02#.G]-*/ /*-[--.---.---.--]-*/
FILE_controlled    (char *a_yes)
{
   if (s_file_status <  G_STAGE_READY)   return -66;
   FILE_vertxt (NULL);
   if (a_yes [0] == 'n') {
      if (ver_ctrl == 'y') {
         ver_ctrl = '-';
         strlcpy (ver_num, "----", LEN_LABEL);
      }
      return 0;
   }
   if (a_yes [0] == 'y') {
      if (ver_ctrl == '-') {
         ver_ctrl = 'y';
         strlcpy (ver_num, "0.0a", LEN_LABEL);
      }
      return 0;
   }
   return -1;
}

char         /*-> tbd --------------------------------[ ------ [ge.880.13#.G1]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
FILE_version       (char *a_ver)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       = 0;
   char        rce         = -10;
   char        x_work      [10];
   /*---(defense : not controlled)-------*/
   if (s_file_status <  G_STAGE_READY)   return -66;
   --rce;  if (ver_ctrl != 'y')  return rce;
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
   --rce;  if (x_work [0] <  ver_num [0])    return rce;
   if (x_work [0] == ver_num [0]) {
      --rce;  if (x_work [2] <  ver_num [2])    return rce;
      if (x_work [2] == ver_num [2]) {
         --rce;  if (x_work [3] <  ver_num [3])    return rce;
         --rce;  if (x_work [3] == ver_num [3])    return rce;
      }
   }
   /*---(finalize)-----------------------*/
   strlcpy (ver_num, x_work, LEN_LABEL);
   FILE_vertxt (NULL);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.330.114.30]*/ /*-[00.0000.03#.7]-*/ /*-[--.---.---.--]-*/
FILE_vertxt        (char *a_txt)
{
   char        rce         =  -10;
   if (s_file_status <  G_STAGE_READY)   return -66;
   strlcpy (ver_txt, "-----", LEN_DESC);
   --rce;  if (ver_ctrl   != 'y' )  return rce;
   --rce;  if (a_txt      == NULL)  return rce;
   --rce;  if (a_txt [0]  == '\0')  return rce;
   strlcpy (ver_txt, a_txt, LEN_DESC);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.C55.124.30]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
FILE_loc                (char *a_loc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tSTAT       s;
   int         x_len       =    0;
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   if (s_file_status <  G_STAGE_READY)   return -66;
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(deal with null/empty)-----------*/
   DEBUG_INPT   yLOG_point   ("a_loc"     , a_loc);
   if (a_loc == NULL || a_loc [0] == 0) {
      DEBUG_INPT   yLOG_note    ("a_loc was null, clear loc");
      strlcpy (myVIKEYS.f_loc  , ""        , LEN_RECD);
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for current)--------------*/
   DEBUG_INPT   yLOG_point   ("a_loc"     , a_loc);
   if (strcmp (a_loc, "./") == 0) {
      DEBUG_INPT   yLOG_note    ("a_loc set to current");
      getcwd (t, LEN_RECD);
      sprintf (myVIKEYS.f_loc  , "%s/", t);
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(stat it)------------------------*/
   rc = stat (a_loc, &s);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_note    ("a_loc does not exist");
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_INPT   yLOG_value   ("mode"      , s.st_mode);
   --rce;  if (!S_ISDIR (s.st_mode)) {
      DEBUG_INPT   yLOG_note    ("a_loc is not a directory");
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(save directory)-----------------*/
   x_len = strllen (a_loc, LEN_RECD);
   if (a_loc [x_len - 1] == '/')  strlcpy (myVIKEYS.f_loc  , a_loc     , LEN_RECD);
   else                           sprintf (myVIKEYS.f_loc  , "%s/", a_loc);
   /*---(report out)---------------------*/
   DEBUG_INPT   yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.C55.124.30]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
FILE_name               (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        t           [LEN_STR]   = "";
   char        d           [LEN_STR]   = "";
   char       *p           = NULL;
   int         x_len       =    0;
   int         x_extlen    =    0;
   int        *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
   /*---(header)-------------------------*/
   if (s_file_status <  G_STAGE_READY)   return -66;
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*---(deal with empties)--------------*/
   DEBUG_INPT   yLOG_point   ("a_name"    , a_name);
   if (a_name == NULL || a_name [0] == 0) {
      DEBUG_INPT   yLOG_note    ("a_name was null, using defaults");
      strlcpy (myVIKEYS.f_name , FILE_BLANK, LEN_RECD);
      sprintf (myVIKEYS.f_title, "%s%s.%s"   , myVIKEYS.f_loc, myVIKEYS.f_name, s_ext);
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   strlcpy (t, a_name, LEN_STR);
   DEBUG_INPT   yLOG_info    ("t"         , t);
   /*---(strip extensions)---------------*/
   DEBUG_INPT   yLOG_info    ("s_ext"     , s_ext);
   x_extlen = strllen (s_ext, LEN_LABEL);
   DEBUG_INPT   yLOG_value   ("x_extlen"  , x_extlen);
   while (1) {
      DEBUG_INPT   yLOG_info    ("t"         , t);
      x_len = strllen (t, LEN_RECD);
      DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
      if (x_len < x_extlen + 1)                        break;
      DEBUG_INPT   yLOG_info    ("tail end"  , t + x_len - x_extlen - 1);
      if (strcmp (t + x_len - x_extlen, s_ext) != 0)   break;
      if (t [x_len - x_extlen - 1] != '.')             break;
      t [x_len - x_extlen - 1] = 0;
   }
   /*---(divide out location)------------*/
   p = strrchr (t, '/');
   DEBUG_INPT   yLOG_point   ("p"         , p);
   if (p != NULL) {
      DEBUG_INPT   yLOG_note    ("fully qualified name, with directory");
      *p = 0;
      sprintf (d, "%s/", t  , LEN_RECD);
      rc = FILE_loc (d);
      if (rc < 0) {
         DEBUG_INPT   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      strlcpy (t, p + 1     , LEN_RECD);
   } else {
      DEBUG_INPT   yLOG_note    ("no directory assigned");
   }
   /*---(check for bad characters)-------*/
   x_len = strllen (t, LEN_RECD);
   --rce;  for (i = 0; i < x_len; ++i) {
      if (strchr (x_valid, t [i]) != NULL)   continue;
      DEBUG_INPT   yLOG_note    ("bad character in file name");
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check for empty, again)---------*/
   if (x_len == 0)  strlcpy (myVIKEYS.f_name , FILE_BLANK, LEN_RECD);
   else             strlcpy (myVIKEYS.f_name , t         , LEN_RECD);
   /*---(report out)---------------------*/
   sprintf (myVIKEYS.f_title, "%s%s.%s", myVIKEYS.f_loc, myVIKEYS.f_name, s_ext);
   DEBUG_INPT   yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   DEBUG_INPT   yLOG_info    ("f_name"    , myVIKEYS.f_name);
   DEBUG_INPT   yLOG_info    ("f_title"   , myVIKEYS.f_title);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       shared functions                       ----===*/
/*====================------------------------------------====================*/
static void  o___SHARED__________o () { return; }

char         /*-> open file for reading and prep -----[ leaf   [ge.723.023.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
FILE_open          (char *a_dir)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   DEBUG_INPT  yLOG_info    ("f_name"    , myVIKEYS.f_name);
   DEBUG_INPT  yLOG_info    ("f_title"   , myVIKEYS.f_title);
   /*---(open file)----------------------*/
   s_file = fopen (myVIKEYS.f_title, a_dir);
   DEBUG_INPT  yLOG_point   ("s_file"    , s_file);
   --rce;  if (s_file == NULL) {
      DEBUG_INPT  yLOG_note    ("file could not be openned");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return 0;
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
FILE_close         (void)
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
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        output related                        ----===*/
/*====================------------------------------------====================*/
static void  o___OUTPUT__________o () { return; }

static char  /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP__sec_columns       (FILE *a_file, char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_label     [LEN_RECD ];
   /*---(prefix)-------------------------*/
   fprintf (a_file, "#---------  ver  ");
   /*---(columns)------------------------*/
   for (i = 0; i < 10; ++i) {
      if (s_sections [a_index].specs [i] == '-')  break;
      sprintf (x_label, "%s%-100.100s", s_sections [a_index].column [i], "-");
      switch (s_sections [a_index].specs [i]) {
      case  'c'  :
         fprintf (a_file, "%-3.3s  "  , x_label);
         break;
      case  's'  :
         fprintf (a_file, "%-10.10s  ", x_label);
         break;
      case  'L'  :
         fprintf (a_file, "%-20.20s  ", x_label);
         break;
      case  'D'  :
         fprintf (a_file, "%-60.60s  ", x_label);
         break;
      case  'S'  :
         fprintf (a_file, "%s  "      , x_label);
         break;
      case  'i'  :
         fprintf (a_file, "%-5.5s  "  , x_label);
         break;
      case  'f'  :
         fprintf (a_file, "%-10.10s  ", x_label);
         break;
      }
   }
   /*---(suffix)-------------------------*/
   fprintf (a_file, "\n");
   /*---(complete)-----------------------*/
   return 0;
}

static char
OUTP_writer             (char n, int a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   int        *x_field     [9];
   char        t           [LEN_RECD ];
   /*---(gather next)-----------------*/
   sprintf (myVIKEYS.f_recd, "");
   /*> if (s_sections [n].writer != NULL)  rc = s_sections [n].writer (i, x_field [0], x_field [1], x_field [2], x_field [3], x_field [4], x_field [5], x_field [6], x_field [7], x_field [8]);   <*/
   for (i = 0; i < 9; ++i) x_field [i] = NULL;
   /*> printf ("file   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   /*> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <*/
   /*> rc = VISU_writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   rc = s_sections [n].writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);
   /*> printf ("after  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   /*> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <*/
   if (rc <= 0)  return rc;
   /*---(columns)---------------------*/
   sprintf (myVIKEYS.f_recd, "%-10.10s  -%c- ", s_sections [n].label, s_sections [n].version);
   /*> sprintf (t, " %d   "   , rc);                                                 <*/
   /*> strlcat (myVIKEYS.f_recd, t, LEN_RECD);                                        <*/
   for (i = 0; i < 9; ++i) {
      if (s_sections [n].specs [i] == '-')  break;
      if (x_field [i] == NULL)  break;
      switch (s_sections [n].specs [i]) {
      case  'c'  :
         sprintf (t, "  %c  "     , *((char   *) x_field [i]));
         break;
      case  's'  :
         fprintf (t, " %-10.10s  ", (char *)     x_field [i]);
         break;
      case  'L'  :
         sprintf (t, " %-20.20s " , (char *)     x_field [i]);
         break;
      case  'D'  :
         sprintf (t, " %-60.60s " , (char *)     x_field [i]);
         break;
      case  'S'  :
         sprintf (t, " %s "       , (char *)     x_field [i]);
         break;
      case  'i'  :
         sprintf (t, " %5d "      , *((int    *) x_field [i]));
         break;
      case  'f'  :
         sprintf (t, " %-10.3lf " , *((double *) x_field [i]));
         break;
      default    :
         strlcpy (t, " ", LEN_LABEL);
         break;
      }
      strlcat (myVIKEYS.f_recd, t, LEN_RECD);
   }
   /*---(complete)-----------------------*/
   return 1;
}

int 
OUTP_write_type         (FILE *a_file, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   int         n           =   -1;
   int         c           =    0;
   char        x_upper     [LEN_LABEL];
   void       *x_field     [9];
   char        t           [LEN_RECD ];
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   /*---(write header)-------------------*/
   strlcpy (x_upper, s_sections [n].name, LEN_LABEL);
   for (i = 0; i < strllen (x_upper, LEN_LABEL); ++i)  x_upper [i] = toupper (x_upper [i]);
   if (a_file != NULL) {
      fprintf (a_file, "#===[[ %-20.20s ]]===============================================================================================#\n",
            x_upper);
      OUTP__sec_columns (a_file, n);
   }
   /*---(write entries)------------------*/
   for (i = 0;  i < 127; ++i) {
      rc = OUTP_writer (n, i);
      /*> if (rc <= 0)  continue;                                                     <*/
      if (rc == 0)  continue;
      if (rc <  0)  break;
      if (a_file != NULL)  fprintf (a_file, "%s\n", myVIKEYS.f_recd);
      ++c;
   }
   /*---(write footer)-------------------*/
   if (a_file != NULL) {
      if (c == 0)  fprintf (a_file, "# no %s\n", s_sections [n].name);
      else         fprintf (a_file, "# complete with %d lines\n", c);
   }
   /*---(complete)-----------------------*/
   return c;
}

int 
OUTP_write              (void)
{
   char        rc          =    0;
   if (s_file_status <  G_STAGE_READY)   return -66;
   rc = FILE_open  ("w");
   if (rc < 0)  return rc;
   OUTP_write_type (s_file, SMOD_MARK);
   OUTP_write_type (s_file, SMOD_VISUAL);
   OUTP_write_type (s_file, MODE_SEARCH);
   OUTP_write_type (s_file, MODE_COMMAND);
   FILE_close ();
}

char
OUTP__unit_writer       (char a_abbr, char a_item)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_index     =   -1;
   /*---(cleanse)------------------------*/
   sprintf (myVIKEYS.f_recd, "");
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   --rce;  if (n < 0)  return rce;
   /*---(find item)----------------------*/
   switch (s_sections [n].abbr) {
   case SMOD_MARK    :  x_index = MARK_valid (a_item);  break;
   case SMOD_VISUAL  :  x_index = VISU_valid (a_item);  break;
   case MODE_SEARCH  :  x_index = SRCH_valid (a_item);  break;
   case MODE_COMMAND :  x_index = CMDS_valid (a_item);  break;
   }
   --rce;  if (x_index < 0)  return rce;
   /*---(process)------------------------*/
   rc = OUTP_writer (n, x_index);
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         input related                        ----===*/
/*====================------------------------------------====================*/
static void  o___INPUT___________o () { return; }

char         /*-> file reading driver ----------------[ leaf   [ge.632.025.30]*/ /*-[01.0001.013.!]-*/ /*-[--.---.---.--]-*/
INPT_read          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         x_len       =    0;               /* string length             */
   /*---(read next good one)-------------*/
   while (1) {
      /*---(read next)-------------------*/
      ++myVIKEYS.f_lines;
      DEBUG_INPT  yLOG_value   ("line"      , myVIKEYS.f_lines);
      fgets (myVIKEYS.f_recd, LEN_RECD, s_file);
      /*---(check eof)-------------------*/
      if (feof (s_file))  {
         DEBUG_INPT  yLOG_note    ("end of file reached");
         return rce;
      }
      /*---(check length)----------------*/
      x_len = strlen (myVIKEYS.f_recd);
      if (x_len <= 0)  {
         DEBUG_INPT  yLOG_note    ("record empty");
         continue;
      }
      /*---(fix)-------------------------*/
      myVIKEYS.f_recd [--x_len] = '\0';
      DEBUG_INPT  yLOG_value   ("length"    , x_len);
      DEBUG_INPT  yLOG_info    ("fixed"     , myVIKEYS.f_recd);
      /*---(filter)----------------------*/
      if (strchr ("# ", myVIKEYS.f_recd [0]) != NULL) {
         DEBUG_INPT  yLOG_note    ("comment/empty line, skipping");
         continue;
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> file reading driver ----------------[ leaf   [ge.851.163.30]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
INPT_parse              (void)
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

char         /*-> file reading driver ----------------[ leaf   [ge.C71.072.GA]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
INTP_read_all      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_celltry   = 0;
   int         x_cellbad   = 0;
   /*---(header)-------------------------*/
   if (s_file_status <  G_STAGE_READY)   return -66;
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(open file)----------------------*/
   rc = FILE_open   ("r");
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(read lines)---------------------*/
   DEBUG_INPT  yLOG_note    ("read lines");
   while (s_file != NULL) {
      /*---(read and clean)--------------*/
      rc = INPT_read ();
      if (rc < 0)  break;
      rc = INPT_parse ();
      if (rc < 0)  break;
      /*---(find type)-------------------*/
      n = -1;
      for (i = 0; i < s_nsection; ++i) {
         if (s_sections [i].label [0] != myVIKEYS.f_type [0])        continue;
         if (strcmp (s_sections [i].label, myVIKEYS.f_type) != 0)    continue;
         n = i;
         break;
      }
      if (n < 0)  continue;
      /*---(handle)----------------------*/
      ++s_sections [n].try;
      rc = -1;
      if (s_sections [n].writer != NULL) {
         rc = s_sections [n].writer (myVIKEYS.f_vers, s_fields [3], s_fields [4], s_fields [5], s_fields [6], s_fields [7], s_fields [8], s_fields [9], s_fields [10], s_fields [11]);
      }
      if (rc < 0)  ++s_sections [n].bad;
      /*---(done)------------------------*/
   }
   /*---(close file)---------------------*/
   FILE_close ();
   /*---(complete)-----------------------*/
   DEBUG_INPT yLOG_exit    (__FUNCTION__);
   return 0;
}

char
INPT__unit_reader       (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_index     =   -1;
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   --rce;  if (n < 0)     return rce;
   /*---(parse the record)---------------*/
   rc = INPT_parse ();
   --rce;  if (rc < 0)    return  rce;
   /*---(use the reader)-----------------*/
   if (s_sections [n].writer != NULL) {
      rc = s_sections [n].reader (myVIKEYS.f_vers, s_fields [2], s_fields [3], s_fields [4], s_fields [5], s_fields [6], s_fields [7], s_fields [8], s_fields [9], s_fields [10]);
   }
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
FILE__unit_null         (void)
{
   s_file_status  = G_STAGE_NULL;
   return 0;
}

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
      snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE ver_num     : %s", ver_num);
   } else if (strcmp (a_question, "version"   )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE version     : %c %-4.4s %s", ver_ctrl, ver_num, ver_txt);
   } else if (strcmp (a_question, "recd"      )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE recd        : %s", myVIKEYS.f_recd);
   } else if (strcmp (a_question, "loc"       )    == 0) {
      if (myVIKEYS.f_loc [0] == 0)  snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE loc         : -");
      else                          snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE loc         : %s", myVIKEYS.f_loc);
   } else if (strcmp (a_question, "title"     )    == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE title       : %s", myVIKEYS.f_title);
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      switch (s_file_status) {
      case -1 : strlcpy (t, "null (not even initialized)", LEN_DESC);    break;
      case  0 : strlcpy (t, "initialized only"           , LEN_DESC);    break;
      case  5 : strlcpy (t, "configured and ready"       , LEN_DESC);    break;
      }
      snprintf (yVIKEYS__unit_answer, LEN_STR, "FILE status      : stage %2d, %s", s_file_status, t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


