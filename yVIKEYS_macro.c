/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*===[[ METIS ]]==============================================================*/
/*
 * metis  tn1#�  @@ is not working on last macro executed
 * metis  tn2#�  repeat value on macros appears to lock up the application
 * metis  dw4#�  create a repeat stack controlled by parens in macros (maybe special parens)
 * metis  ww2��  macro should abort if anything causes an error
 * metis  tw2#�  when macros execute, do not re-record internal keystrokes
 * metis  tw1��  macro debug should start in manual mode and require input
 *
 *
 *
 */



#define      S_MACRO_MAX      75
static char  S_MACRO_LIST   [S_MACRO_MAX];



typedef     struct        cMACRO    tMACRO;
struct cMACRO {
   /*---(running)-----------*/
   char        runby;                       /* run by who                     */
   /*---(contents)----------*/
   char        len;                         /* number of keys                 */
   char        keys        [LEN_RECD];      /* keystrokes                     */
   /*---(execute)-----------*/
   char        pos;                         /* current position               */
   char        cur;                         /* current key                    */
   char        repeat;                      /* number of repeats              */
   /*---(done)--------------*/
};
static tMACRO  s_macros    [S_MACRO_MAX];
static int     s_nmacro    =    0;

static char    s_emode     =  '-';          /* run, playback, delay, etc      */
static char    s_delay     =  '0';          /* time between steps             */
static char    s_update    =  'n';          /* macro exec sceen update speed  */
static char    s_ename     =  '-';
static int     s_ecurr     =   -1;

static char    s_rmode     =  '-';          /* recording or not               */
static char    s_rname     =  '-';
static int     s_rcurr     =   -1;



static char    (*s_loader) (char a_name, char *a_keys);
static char    (*s_saver ) (char a_name, char *a_keys);

#define     MACRO_LOWER    'a'
#define     MACRO_UPPER    'A'
#define     MACRO_SYSTEM   '0'
#define     MACRO_ALL      '*'



/*====================------------------------------------====================*/
/*===----                        finding/checking                      ----===*/
/*====================------------------------------------====================*/
static void  o___FINDING_________o () { return; }

int  
yvikeys_macro__index    (char a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   n  = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
   return n;
}

int  
yvikeys_macro__rset     (char a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   n  = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
   if (n >= 0) {
      s_rname = a_abbr;
      s_rcurr = n;
   } else {
      s_rname = '-';
      s_rcurr = -1;
   }
   return n;
}

int  
yvikeys_macro__eset     (char a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   n  = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
   if (n >= 0) {
      s_ename = a_abbr;
      s_ecurr = n;
   } else {
      s_ename = '-';
      s_ecurr = -1;
   }
   return n;
}



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___CLEARING________o () { return; }

char         /*-> fully reset a macro ----------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__clear    (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =   -1;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   DEBUG_PROG   yLOG_schar   (a_abbr);
   /*---(defense)------------------------*/
   n = yvikeys_macro__index (a_abbr);
   DEBUG_PROG   yLOG_sint    (n);
   if (n < 0) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(running)------------------------*/
   DEBUG_PROG   yLOG_snote   ("runby");
   s_macros [n].runby     =   -1;
   /*---(contents)-----------------------*/
   DEBUG_PROG   yLOG_snote   ("data");
   s_macros [n].len       =    0;
   for (i = 0; i < LEN_RECD; ++i)  s_macros [n]. keys [i] = G_KEY_NULL;
   /*---(execute)------------------------*/
   DEBUG_PROG   yLOG_snote   ("exec");
   s_macros [n].pos       =   -1;
   s_macros [n].cur       =    0;
   s_macros [n].repeat    =    0;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> fully reset a macro ----------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__wipe     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   DEBUG_PROG   yLOG_schar   (a_abbr);
   /*---(defense)------------------------*/
   n = yvikeys_macro__index (a_abbr);
   DEBUG_PROG   yLOG_sint    (n);
   if (n < 0) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(running)------------------------*/
   DEBUG_PROG   yLOG_snote   ("runby");
   s_macros [n].runby     =   -1;
   /*---(execute)------------------------*/
   DEBUG_PROG   yLOG_snote   ("exec");
   s_macros [n].pos       =   -1;
   s_macros [n].cur       =    0;
   s_macros [n].repeat    =    0;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__purge    (char a_scope)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(turn off)-----------------------*/
   SET_MACRO_IGNORE;
   SET_MACRO_STOP;
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  : case YVIKEYS_UPPER  : case YVIKEYS_NUMBER :
   case YVIKEYS_GREEK  : case YVIKEYS_FULL   :
      break;
   default :
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_nmacro"  , s_nmacro);
   for (i = 0; i < s_nmacro; ++i) {
      x_abbr = S_MACRO_LIST [i];
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_macro__clear (x_abbr);
   }
   /*---(clear current)------------------*/
   s_emode = '-';
   s_ename = '-';
   s_ecurr = -1;
   s_rmode = '-';
   s_rname = '-';
   s_rcurr = -1;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_resetall  (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(mode)---------------------------*/
   DEBUG_SCRP   yLOG_snote   ("macro_off");
   SET_MACRO_IGNORE;
   SET_MACRO_STOP;
   /*---(reset positions)----------------*/
   for (i = 0; i < s_nmacro; ++i) {
      x_abbr = S_MACRO_LIST [i];
      yvikeys_macro__wipe  (x_abbr);
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_rreset    (void)
{
   if (s_rcurr < 0)  return 0;
   yvikeys_macro__wipe (S_MACRO_LIST [s_rcurr]);
   return 0;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_ereset    (void)
{
   if (s_rcurr < 0)  return 0;
   yvikeys_macro__wipe (S_MACRO_LIST [s_ecurr]);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize macro environment -------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_init      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_MACRO)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(macro abbrev list)--------------*/
   strlcpy (S_MACRO_LIST, ""            , S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_lower , S_MACRO_MAX);
   /*> strlcat (S_MACRO_LIST, gvikeys_upper , S_MACRO_MAX);                           <*/
   strlcat (S_MACRO_LIST, gvikeys_number, S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_greek , S_MACRO_MAX);
   DEBUG_PROG   yLOG_info    ("LIST"      , S_MACRO_LIST);
   s_nmacro = strlen (S_MACRO_LIST);
   /*---(clear)--------------------------*/
   s_emode   = '-';
   s_ename   = '-';
   s_ecurr   =  -1;
   s_rmode   = '-';
   s_rname   = '-';
   s_rcurr   =  -1;
   s_loader = NULL;
   s_saver  = NULL;
   yvikeys_macro__purge (MACRO_ALL);
   /*---(update status)------------------*/
   DEBUG_PROG   yLOG_note    ("update status");
   STATUS_init_set   (SMOD_MACRO);
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("add commands");
   yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "macro"       , ""    , "a"    , yvikeys_macro__direct, "direct definition of a keyboard macro"                       );
   yVIKEYS_view_option (YVIKEYS_STATUS, "macro"  , yvikeys_macro_estatus , "details of macro playback"                );
   yVIKEYS_view_option (YVIKEYS_STATUS, "record" , yvikeys_macro_rstatus , "details of macro recording"               );
   /*---(read/write)---------------------*/
   rc = yPARSE_handler (SMOD_MACRO   , "macro"     , 7.3, "cO----------", yvikeys_macro_reader, yvikeys_macro_writer_all, "------------" , "a,keys", "keyboard macros"           );
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initialize macro environment -------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_macro_config    (void *a_loader, void *a_saver)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_MACRO)) {
      DEBUG_SCRP   yLOG_note    ("init must complete before config");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   if (a_loader != NULL) s_loader = a_loader;
   DEBUG_SCRP   yLOG_point   ("loader"    , s_loader);
   if (a_saver  != NULL) s_saver  = a_saver;
   DEBUG_SCRP   yLOG_point   ("saver"     , s_saver);
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_MACRO, '1');
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_zero      (void)
{
   if (s_ecurr < 0)  return 0;
   s_macros [s_ecurr].repeat = 0;
   return 0;
}

char
yvikeys_macro_count     (void)
{
   if (s_ecurr < 0)                   return 0;
   return s_macros [s_ecurr].repeat;
}



/*====================------------------------------------====================*/
/*===----                       saving and fetching                    ----===*/
/*====================------------------------------------====================*/
static void  o___SAVING__________o () { return; }

char         /*-> save current recording -------------[ ------ [ge.631.052.22]*/ /*-[01.0000.014.4]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__save     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        x_keys      [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_rcurr"    , s_rcurr);
   --rce;  if (s_rcurr < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(trim)---------------------------*/
   strlcpy (x_keys, s_macros [s_rcurr].keys, LEN_RECD);
   x_len = strlen (x_keys);
   if (x_len > 0)  x_keys [--x_len] = G_KEY_NULL;
   /*---(save gyges)---------------------*/
   if (s_saver != NULL)  s_saver (s_rname, x_keys);
   /*---(clear)--------------------------*/
   yvikeys_macro__wipe (s_rname);
   yvikeys_macro_zero  ();
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> fetch specific macro keystrokes ----[ ------ [ge.A52.134.63]*/ /*-[01.0000.023.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__fetch    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   char        x_index     =    0;
   /*---(defense)------------------------*/
   if (s_loader == NULL)    return 0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prefetch)--------------------*/
   s_loader (s_ename, s_macros [s_ecurr].keys);
   s_macros [s_ecurr].len = strlen (s_macros [s_ecurr].keys);
   DEBUG_SCRP   yLOG_info    ("macro"     , s_macros [s_ecurr].keys);
   DEBUG_SCRP   yLOG_value   ("len"       , s_macros [s_ecurr].len);
   /*---(set globals)-----------------*/
   s_macros [s_ecurr].keys [s_macros [s_ecurr].len++] = G_CHAR_NULL;
   s_macros [s_ecurr].keys [s_macros [s_ecurr].len  ] = G_KEY_NULL;
   if (s_macros [s_ecurr].pos < 0)  s_macros [s_ecurr].cur = 0;
   else                             s_macros [s_ecurr].cur = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro recording                      ----===*/
/*====================------------------------------------====================*/
static void  o___RECORD__________o () { return; }

static char  s_backup  [LEN_RECD];

char         /*-> prepare a macro recording ----------[ ------ [ge.831.112.53]*/ /*-[01.0000.023.A]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__recbeg   (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_SCRP   yLOG_note    ("can not execute until operational");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_char    ("s_rmode"   , s_rmode);
   --rce;  IF_MACRO_RECORDING {
      DEBUG_SCRP   yLOG_note    ("can not record two macros at once");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   n = yvikeys_macro__index  (tolower (a_name));
   DEBUG_SCRP   yLOG_value   ("n"         , n);
   if (n <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for illegal)-----------*/
   DEBUG_SCRP   yLOG_value   ("s_rcurr"   , s_rcurr);
   --rce;  if (s_rcurr >= 0) {
      DEBUG_SCRP   yLOG_note    ("already recording a macro");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("s_ecurr"   , s_ecurr);
   --rce;  if (n == s_ecurr) {
      DEBUG_SCRP   yLOG_note    ("currently executing this macro");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("pos"       , s_macros [n].pos);
   --rce;  if (s_macros [n].pos >= 0) {
      DEBUG_SCRP   yLOG_note    ("this macro is active");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("runby"     , s_macros [n].runby);
   --rce;  if (s_macros [n].runby >= 0) {
      DEBUG_SCRP   yLOG_note    ("this macro is running higher in tree");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (yvikeys_macro__rset   (tolower (a_name)) <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("s_rcurr"   , s_rcurr);
   /*---(prepare)---------------------*/
   strlcpy (s_backup, s_macros [s_rcurr].keys, LEN_RECD);
   DEBUG_SCRP   yLOG_info ("s_backup"  , s_backup);
   /*---(normal mode)-----------------*/
   if (a_name == tolower (a_name) || s_macros [s_rcurr].len < 1) {
      yvikeys_macro__clear (a_name);
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len++] = G_CHAR_PLACE;
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len  ] = G_KEY_NULL;
   }
   /*---(append mode)-----------------*/
   else {
      yvikeys_macro__wipe  (a_name);
   }
   /*---(turn on record)--------------*/
   DEBUG_SCRP   yLOG_value   ("keys"      , s_macros [s_rcurr].keys);
   SET_MACRO_RECORD;
   DEBUG_SCRP   yLOG_char    ("s_rmode"   , s_rmode);
   VIEW__switch ("status", "record");
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> add a key to a macro ---------------[ leaf   [gc.440.102.10]*/ /*-[01.0000.103.9]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_reckey    (char a_key)
{
   IF_MACRO_RECORDING {
      switch (a_key) {
      case G_KEY_RETURN  :  a_key  = G_CHAR_RETURN;  break;  /* return char           */
      case G_KEY_ENTER   :  a_key  = G_CHAR_RETURN;  break;  /* return char           */
      case G_KEY_ESCAPE  :  a_key  = G_CHAR_ESCAPE;  break;  /* escape char           */
      case G_KEY_TAB     :  a_key  = G_CHAR_TAB;     break;  /* tab char              */
      case G_KEY_BS      :  a_key  = G_CHAR_BS;      break;  /* backspace char        */
      case G_KEY_SPACE   :  a_key  = G_CHAR_SPACE;   break;  /* visual space          */
      }
      s_macros [s_rcurr].cur                              = a_key;
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 1] = a_key;
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len++  ] = G_CHAR_PLACE;
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len    ] = G_KEY_NULL;
      s_macros [s_rcurr].pos                              = s_macros [s_rcurr].len - 2;
   }
   return 0;
}

char         /*-> put keys in globals ----------------[ leaf   [gz.530.101.60]*/ /*-[01.0000.01#.5]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__recstr   (char *a_keys)
{
   int         i           = 0;
   char        x_ch        = '-';
   IF_MACRO_RECORDING {
      /*---(look for suffix)----------------*/
      if (s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 1] == (schar) G_CHAR_PLACE) {
         s_macros [s_rcurr].keys [--s_macros [s_rcurr].len  ] =  G_KEY_NULL;
      }
      if (s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 1] == (schar) G_CHAR_NULL ) {
         s_macros [s_rcurr].keys [--s_macros [s_rcurr].len  ] =  G_KEY_NULL;
      }
      /*---(add keys)-----------------------*/
      if (a_keys == NULL)  strlcat (s_macros [s_rcurr].keys, ""    , LEN_RECD);
      else                 strlcat (s_macros [s_rcurr].keys, a_keys, LEN_RECD);
      s_macros [s_rcurr].len  = strlen (s_macros [s_rcurr].keys);
      /*---(fix keys)-----------------------*/
      strlencode   (s_macros [s_rcurr].keys, ySTR_MAX, LEN_RECD);
      /*---(add suffix)---------------------*/
      s_macros [s_rcurr].keys [s_macros [s_rcurr].len  ] =  G_CHAR_PLACE;
      s_macros [s_rcurr].keys [++s_macros [s_rcurr].len] =  G_KEY_NULL;
      /*---(update pos/char)----------------*/
      s_macros [s_rcurr].pos = s_macros [s_rcurr].len - 2;
      if (s_macros [s_rcurr].pos < 0)  s_macros [s_rcurr].cur = 0;
      else                  s_macros [s_rcurr].cur = s_macros [s_rcurr].keys [s_macros [s_rcurr].pos];
      /*> printf ("s_macros [s_rcurr].keys <<%s>>\n", s_macros [s_rcurr].keys);                                <*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> complete a macro recording ---------[ ------ [gz.420.001.21]*/ /*-[02.0000.113.5]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_recend    (void)
{
   IF_MACRO_RECORDING {
      if (s_macros [s_rcurr].len > 2 && s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 2] == 'q') {
         s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 2] = G_CHAR_NULL;
         s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 1] = G_KEY_NULL;
         --s_macros [s_rcurr].len;
      } else if (s_macros [s_rcurr].len == 2 && s_macros [s_rcurr].keys [s_macros [s_rcurr].len - 2] == 'q') {
         s_macros [s_rcurr].keys [0] = G_KEY_NULL;
         s_macros [s_rcurr].len      = 0;
      } else {
         strlcpy (s_macros [s_rcurr].keys, s_backup, LEN_RECD);
         s_macros [s_rcurr].len      = strlen (s_backup);
      }
      yvikeys_macro__wipe (s_rname);
      yvikeys_macro__save ();
      SET_MACRO_IGNORE;
      s_rcurr = -1;
   }
   return 0;
}

char         /*-> enter a macro directly -------------[ ------ [ge.850.137.A4]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__direct   (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_div       =  '-';
   char        x_ch        =  '-';
   char       *x_valid     = "*aA0�";
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_SCRP   yLOG_note    ("can not execute until operational");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_point   ("a_string"  , a_string);
   --rce;  if (a_string == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("a_string"  , a_string);
   x_len = strlen (a_string);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for purge)----------------*/
   --rce;  if (x_len == 1) {
      DEBUG_SCRP   yLOG_note    ("one character option (purge)");
      DEBUG_SCRP   yLOG_info    ("x_valid"   , x_valid);
      x_ch = a_string [0];
      DEBUG_SCRP   yLOG_char    ("x_ch"      , x_ch);
      if (strchr (x_valid, x_ch) != NULL) {
         rc = yvikeys_macro__purge (x_ch);
         DEBUG_SCRP   yLOG_value   ("purge"     , rc);
         if (rc < 0) {
            DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for equal)----------------*/
   DEBUG_SCRP   yLOG_note    ("check for equal sign");
   x_div = a_string [1];
   DEBUG_SCRP   yLOG_char    ("x_div"     , x_div);
   --rce;  if (x_div != '=') {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = a_string [0];
   DEBUG_SCRP   yLOG_char    ("abbr"      , x_ch);
   /*---(check for clear)----------------*/
   --rce;  if (x_len == 2) {
      DEBUG_SCRP   yLOG_note    ("two character option (clear)");
      rc = yvikeys_macro__clear (x_ch);
      DEBUG_SCRP   yLOG_value   ("clear"     , rc);
      if (rc < 0) {
         DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(begin recording)----------------*/
   DEBUG_SCRP   yLOG_note    ("three plus character option (define)");
   rc = yvikeys_macro__recbeg (x_ch);
   DEBUG_SCRP   yLOG_value   ("recbeg"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (x_ch);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add keys)-----------------------*/
   if (a_string [2] != G_KEY_DQUOTE) {
      DEBUG_SCRP   yLOG_note    ("normal/unquoted macro format");
      DEBUG_SCRP   yLOG_info    ("string"    , a_string + 2);
      rc = yvikeys_macro__recstr (a_string + 2);
   } else {
      DEBUG_SCRP   yLOG_note    ("quoted macro format");
      x_len = strlen (a_string);
      if (a_string [x_len - 1] != G_KEY_DQUOTE) {
         DEBUG_SCRP   yLOG_info    ("string"    , a_string + 2);
         rc = yvikeys_macro__recstr (a_string + 2);
      } else {
         a_string [--x_len] = G_KEY_NULL;
         DEBUG_SCRP   yLOG_info    ("string"    , a_string + 3);
         rc = yvikeys_macro__recstr (a_string + 3);
      }
   }
   DEBUG_SCRP   yLOG_value   ("recstr"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (x_ch);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(append end record key)----------*/
   rc = yvikeys_macro_reckey ('q');
   /*---(end recording)------------------*/
   rc = yvikeys_macro_recend ();
   DEBUG_SCRP   yLOG_value   ("recend"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (x_ch);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro execution                      ----===*/
/*====================------------------------------------====================*/
static void  o___EXECUTE_________o () { return; }

char         /*-> set or adjust delay value ----------[ leaf   [gz.612.101.50]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__delay    (char a_delay)
{
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(set)----------------------------*/
   if (a_delay >= '0' && a_delay <= '9') {
      DEBUG_SCRP   yLOG_note    ("set abs");
      s_delay = a_delay;
   }
   if (a_delay == '-') {
      DEBUG_SCRP   yLOG_note    ("decrease (-)");
      if (s_delay > '0')  --s_delay;
   }
   if (a_delay == '+') {
      DEBUG_SCRP   yLOG_note    ("increase (+)");
      if (s_delay < '9')  ++s_delay;
   }
   /*---(new value)-------------------*/
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_pos       (char *a_name, int *a_pos)
{
   *a_name = s_ename;
   *a_pos  = s_macros [s_ecurr].pos;
   return 0;
}

char
yvikeys_macro_repos     (int a_pos)
{
   s_macros [s_ecurr].pos = a_pos;
   s_macros [s_ecurr].cur = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
   return 0;
}

char         /*-> prepare a macro execution ----------[ ------ [ge.832.122.52]*/ /*-[01.0000.112.5]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exebeg    (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   int         x_curr      =   -1;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_SCRP   yLOG_note    ("can not execute until operational");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check macro name)------------*/
   x_curr = s_ecurr;
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   n = yvikeys_macro__index  (tolower (a_name));
   DEBUG_SCRP   yLOG_value   ("n"         , n);
   if (n <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for illegal)-----------*/
   DEBUG_SCRP   yLOG_value   ("s_rcurr"   , s_rcurr);
   --rce;  if (n == s_rcurr) {
      DEBUG_SCRP   yLOG_note    ("currently recording");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("s_ecurr"   , s_ecurr);
   --rce;  if (n == s_ecurr) {
      DEBUG_SCRP   yLOG_note    ("currently executing");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("pos"       , s_macros [n].pos);
   --rce;  if (s_macros [n].pos >= 0) {
      DEBUG_SCRP   yLOG_note    ("macro is active");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("runby"     , s_macros [n].runby);
   --rce;  if (s_macros [n].runby >= 0) {
      DEBUG_SCRP   yLOG_note    ("macro is running higher");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set as current)--------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   --rce;  if (yvikeys_macro__eset   (tolower (a_name)) <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(normal)----------------------*/
   if (a_name == tolower (a_name)) {
      DEBUG_SCRP   yLOG_note    ("normal execution");
      if (s_delay    != '0'       )  SET_MACRO_DELAY;
      IF_MACRO_OFF                   SET_MACRO_RUN;
   }
   /*---(debugging/playback)----------*/
   else {
      DEBUG_SCRP   yLOG_note    ("debug execution");
      SET_MACRO_PLAYBACK;
   }
   /*---(macro name)------------------*/
   DEBUG_SCRP   yLOG_char    ("macro_name", s_ename);
   /*---(get macro)-------------------*/
   yvikeys_macro__fetch ();
   /*---(update settings)-------------*/
   yvikeys_macro_ereset ();
   yvikeys_macro_exeadv ('�');
   s_macros [s_ecurr].runby  = x_curr;
   s_macros [s_ecurr].repeat = REPEAT_count ();
   REPEAT_macro ();
   DEBUG_USER   yLOG_value   ("repeat"    , s_macros [s_ecurr].repeat);
   /*---(reset main delay)---------------*/
   yvikeys_loop_macro (s_delay, s_update);
   VIEW__switch ("status", "macro");
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.220.002.20]*/ /*-[00.0000.102.7]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exeadv    (char a_play)
{
   /*---(locals)-----------+-----+-----+-*/
   static int  x_skips     =    0;
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_sint    (s_ecurr);
   if (s_ecurr < 0) {
      DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   DEBUG_SCRP   yLOG_schar   (S_MACRO_LIST [s_ecurr]);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_schar   (s_emode);
   switch (s_emode) {
   case MACRO_STOP     :
      DEBUG_SCRP   yLOG_snote   ("stopped, get out");
      DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
      return 0;
      break;
   case MACRO_DELAY    :
      DEBUG_SCRP   yLOG_snote   ("delay mode");
      ++x_skips;
      DEBUG_SCRP   yLOG_sint    (x_skips);
      DEBUG_SCRP   yLOG_sint    (myVIKEYS.macro_skip);
      if (x_skips < myVIKEYS.macro_skip) {
         DEBUG_SCRP   yLOG_snote   ("no position update due yet");
         DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      x_skips = 0;
      break;
   case MACRO_PLAYBACK :
      DEBUG_SCRP   yLOG_snote   ("playback mode");
      DEBUG_SCRP   yLOG_schar   (a_play);
      if (a_play == 0 || strchr ("� ", a_play) == NULL) {
         DEBUG_SCRP   yLOG_snote   ("no position update requested");
         DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      break;
   case MACRO_RUN      :
      DEBUG_SCRP   yLOG_snote   ("normal run mode");
      break;
   }
   /*---(next key)-----------------------*/
   if (yvikeys_keys_repeating ()) {
      DEBUG_LOOP   yLOG_note    ("group repeating older keys");
      s_macros [s_ecurr].cur = yvikeys_keys_keygpos ();
      KEYS_repos (yvikeys_keys_gpos + 1);
   } else {
      DEBUG_LOOP   yLOG_note    ("new keystroke");
      ++s_macros [s_ecurr].pos;
      DEBUG_SCRP   yLOG_sint    (s_macros [s_ecurr].pos);
      if (s_macros [s_ecurr].pos < 0)  s_macros [s_ecurr].cur = 0;
      else                             s_macros [s_ecurr].cur = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
   }
   DEBUG_SCRP   yLOG_schar   (s_macros [s_ecurr].cur);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> return current key in macro --------[ ------ [ge.A63.034.52]*/ /*-[02.0000.102.8]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exekey    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   int         x_runby     =   -1;
   static int  x_pos       =   -1;
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_ecurr"   , s_ecurr);
   DEBUG_SCRP   yLOG_char    ("abbr"      , S_MACRO_LIST [s_ecurr]);
   DEBUG_SCRP   yLOG_value   ("pos"       , s_macros [s_ecurr].pos);
   /*---(handle playback/delay)----------*/
   DEBUG_SCRP   yLOG_value   ("x_pos"     , x_pos);
   if (s_macros [s_ecurr].pos == x_pos) {
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;  /* return a no-action */
   }
   /*---(handle end of macro)------------*/
   DEBUG_SCRP   yLOG_value   ("runby"     , s_macros [s_ecurr].runby);
   DEBUG_SCRP   yLOG_value   ("repeat"    , s_macros [s_ecurr].repeat);
   --rce;  if (s_macros [s_ecurr].pos >= s_macros [s_ecurr].len - 1) {
      /*---(restart)------------------------*/
      if (s_macros [s_ecurr].repeat > 0) {
         DEBUG_SCRP   yLOG_note    ("repeats left, so restart");
         s_macros [s_ecurr].pos    =  0;
         x_pos = -1;
         s_macros [s_ecurr].cur    = '�';
         --s_macros [s_ecurr].repeat;
      }
      /*---(pop)----------------------------*/
      else if (s_macros [s_ecurr].runby >= 0) {
         DEBUG_SCRP   yLOG_note    ("return to caller/runby");
         s_macros [s_ecurr].pos    = -1;
         x_pos = -1;
         s_macros [s_ecurr].cur    = '�';
         s_macros [s_ecurr].repeat =  0;
         x_runby = s_macros [s_ecurr].runby;
         s_macros [s_ecurr].runby  = -1;
         s_ecurr = x_runby;
         s_ename = S_MACRO_LIST [s_ecurr];
         DEBUG_SCRP   yLOG_value   ("s_ecurr"    , s_ecurr);
         DEBUG_SCRP   yLOG_char    ("abbr"      , S_MACRO_LIST [s_ecurr]);
         DEBUG_SCRP   yLOG_value   ("pos"       , s_macros [s_ecurr].pos);
         DEBUG_SCRP   yLOG_value   ("runby"     , s_macros [s_ecurr].runby);
         DEBUG_SCRP   yLOG_value   ("repeat"    , s_macros [s_ecurr].repeat);
      }
      /*---(reset)--------------------------*/
      else {
         DEBUG_SCRP   yLOG_note    ("macro complete");
         SET_MACRO_STOP;
         s_macros [s_ecurr].pos    = -1;
         x_pos   = -1;
         s_macros [s_ecurr].cur    = '�';
         s_macros [s_ecurr].repeat =  0;
         s_ecurr = -1;
         yvikeys_loop_normal ();
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
   }
   /*---(handle next key)----------------*/
   x_ch = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
   DEBUG_SCRP   yLOG_value   ("x_ch"      , x_ch);
   x_ch = chrworking (x_ch);
   DEBUG_SCRP   yLOG_char    ("x_ch"      , x_ch);
   x_pos = s_macros [s_ecurr].pos;
   DEBUG_SCRP   yLOG_value   ("x_pos"     , x_pos);
   /*---(handle controls)-------------*/
   if (x_ch < 0) {
      x_ch = yvikeys_macro__exectl (x_ch);
   }
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char         /*-> tbd --------------------------------[ ------ [gc.340.102.21]*/ /*-[00.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__exectl   (char a_key)
{
   if (a_key >= 0)  return 0;
   switch (256 + a_key) {
   case G_CHAR_WAIT    : sleep (1);            a_key = G_KEY_SPACE;  break;
   case G_CHAR_BREAK   : SET_MACRO_PLAYBACK;   a_key = G_KEY_SPACE;  break;
   case G_CHAR_HALT    :                       a_key = G_KEY_NULL;   break;
                                               /*> case G_CHAR_DISPLAY : CURS_main ();         a_key = G_KEY_SPACE;  break;       <*/
   case G_CHAR_NULL    :                       a_key = G_KEY_NULL;   break;
   default             :                       a_key = G_KEY_NULL;   break;
   }
   return a_key;
}

char         /*-> tbd --------------------------------[ ------ [gc.A43.108.12]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exeplay   (uchar a_key)
{
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("a_key"     , a_key);
   switch (a_key) {
   case '.'      :
      DEBUG_SCRP   yLOG_note    ("dot (.)");
      IF_MACRO_PLAYBACK {
         DEBUG_SCRP   yLOG_note    ("change playback to delay");
         SET_MACRO_DELAY;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      IF_MACRO_DELAY {
         DEBUG_SCRP   yLOG_note    ("change delay to playback");
         SET_MACRO_PLAYBACK;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      break;
   case '-'      : case '+'      :
      yvikeys_macro__delay (a_key);
      break;
   case 'n' : case 's' : case 'b' : case 'p' : case 'd' :
      s_update = a_key;
      break;
   case G_KEY_ESCAPE : case G_CHAR_ESCAPE :
      DEBUG_SCRP   yLOG_note    ("escape");
      yvikeys_macro_ereset ();
      yvikeys_loop_normal ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_RETURN : case G_KEY_ENTER  : case G_CHAR_RETURN :
      DEBUG_SCRP   yLOG_note    ("return");
      SET_MACRO_RUN;
      break;
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     sub-mode key handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process macro sub-mode keys --------[ ------ [ge.H65.229.88]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_smode     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_majors    [LEN_LABEL] = "q@<>";
   char        x_keys      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_MACRO))  {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN || a_minor == G_KEY_ENTER ) {
      DEBUG_USER   yLOG_note    ("escape/return, nothing to do");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for recording)------------*/
   --rce;  if (a_major == 'q') {
      DEBUG_USER   yLOG_note    ("macro recording");
      /*---(start recording)-------------*/
      MODE_exit  ();
      rc = yvikeys_macro__recbeg (a_minor);
      if (rc < 0) {
         DEBUG_USER   yLOG_note    ("can not execute");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for execution)------------*/
   --rce;  if (a_major == '@') {
      /*---(export/import)---------------*/
      if (strchr ("<>", a_minor) != NULL) {
         DEBUG_USER   yLOG_note    ("macro copy/paste from unnamed register");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      DEBUG_USER   yLOG_note    ("macro execution");
      /*---(check for previous)----------*/
      if (a_minor == '@') {
         DEBUG_USER   yLOG_note    ("rerun previously used macro");
         a_minor = s_ename;
      }
      /*---(execute)---------------------*/
      MODE_exit  ();
      rc = yvikeys_macro_exebeg  (a_minor);
      if (rc < 0) {
         DEBUG_USER   yLOG_note    ("can not execute");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
      /*---(done)------------------------*/
   }
   /*---(check for copy/paste)-----------*/
   --rce;  if (a_major == '>') {
      rc = yvikeys_macro__eset  (a_minor);
      if (rc < 0) {
         DEBUG_USER   yLOG_note    ("can not find macro");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      yvikeys_macro__clear (a_minor);
      yvikeys_sreg_setreg  ('"');
      yvikeys_sreg_fetch   (&(s_macros [s_ecurr].len), &(s_macros [s_ecurr].keys));
      s_macros [s_ecurr].keys [s_macros [s_ecurr].len++] = G_CHAR_NULL;
      s_macros [s_ecurr].keys [s_macros [s_ecurr].len  ] = G_KEY_NULL;
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   --rce;  if (a_major == '<') {
      /*---(test name)----------------------*/
      rc = yvikeys_macro__eset  (a_minor);
      if (rc < 0) {
         DEBUG_USER   yLOG_note    ("can not find macro");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(keys)---------------------------*/
      strlcpy (x_keys, s_macros [s_ecurr].keys, LEN_RECD);
      if (s_macros [s_ecurr].len > 0)  x_keys [s_macros [s_ecurr].len - 1] = G_KEY_NULL;
      yvikeys_sreg_setreg  ('"');
      yvikeys_sreg_save    ("macro", x_keys);
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(errors fall through)------------*/
   --rce;
   yvikeys_macro_rreset ();
   yvikeys_macro_ereset ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char yvikeys_macro_emode     (void)         { return s_emode; }
char yvikeys_macro_rmode     (void)         { return s_rmode; }

char
yvikeys_macro_modeset   (char a_mode)
{
   if      (a_mode == MACRO_IGNORE)  s_rmode = a_mode;
   else if (a_mode == MACRO_RECORD)  s_rmode = a_mode;
   else                              s_emode = a_mode;
   return 0;
}

char
yvikeys_macro_estatus        (char *a_list)
{
   char        x_bef       [LEN_RECD] = "";
   char        x_aft       [LEN_RECD] = "";
   int         x_pos       = 0;
   int         x_rem       = 0;
   char        x_dots      [LEN_HUND] = "���������������������������";
   char        x_ldots     [LEN_HUND] = "���������������������������";
   char        x_rdots     [LEN_HUND] = "���������������������������";
   if (s_ecurr < 0) {
      snprintf (a_list, LEN_FULL, "macro   %c %c %c -- --- --- ��������������������������� � ���������������������������", s_ename, s_delay, s_update);
      return 0;
   }
   x_pos = s_macros [s_ecurr].pos;
   if      (x_pos <=  0)  sprintf (x_bef , "%27.27s", x_ldots);
   else if (x_pos >  27)  sprintf (x_bef , "<%-26.26s", s_macros [s_ecurr].keys + x_pos - 26);
   else                   sprintf (x_bef , "%*.*s%*.*s"   , 27 - x_pos, 27 - x_pos, x_ldots, x_pos, x_pos, s_macros [s_ecurr].keys);
   x_rem = s_macros [s_ecurr].len - s_macros [s_ecurr].pos - 1;
   if      (x_rem <=  0)  sprintf (x_aft , "%27.27s", x_rdots);
   else if (x_rem >  27)  sprintf (x_aft , "%-26.26s>", s_macros [s_ecurr].keys + s_macros [s_ecurr].pos + 1);
   else                   sprintf (x_aft , "%*.*s%*.*s"   , x_rem, x_rem, s_macros [s_ecurr].keys + x_pos + 1, 27 - x_rem, 27 - x_rem, x_rdots + x_rem);
   snprintf (a_list, LEN_FULL, "macro   %c %c %c %2d %3d %3d %27s %c %s", s_ename, s_delay, s_update, s_macros [s_ecurr].repeat, x_pos, s_macros [s_ecurr].len, x_bef, s_macros [s_ecurr].cur, x_aft);
   return 0;
}

char
yvikeys_macro_rstatus        (char *a_list)
{
   char        x_dots      [LEN_HUND] = "�����������������������������������������������������������������";
   int         x_len       = 0;
   if (s_rcurr < 0) {
      snprintf (a_list, LEN_FULL, "record  %c --- %s", s_rname, x_dots);
      return 0;
   }
   x_len = s_macros [s_rcurr].len;
   snprintf (a_list, LEN_FULL, "record  %c %3d %*.*s%*.*s", s_rname, x_len, x_len, x_len, s_macros [s_rcurr].keys, 65 - x_len, 65 - x_len, x_dots + x_len);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      status reporting                        ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_list      (int *a_count, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         c           =    0;
   char        x_entry     [LEN_LABEL];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   if (a_list != NULL)  strncpy (a_list, ",", LEN_RECD);
   /*---(walk the list)------------------*/
   for (i = 0; i < s_nmacro; ++i) {
      if (s_macros [i].len <= 0) continue;
      if (a_list != NULL)  sprintf    (x_entry, "%c,", S_MACRO_LIST [i]);
      if (a_list != NULL)  strncat    (a_list, x_entry, LEN_RECD);
      ++c;
   }
   /*---(catch empty)--------------------*/
   if (a_list != NULL && strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   if (a_count != NULL)  *a_count = c;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     file input and output                    ----===*/
/*====================------------------------------------====================*/
static void  o___FILE____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_writer    (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   char        x_keys      [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   n  = yvikeys_macro__index (a_abbr);
   DEBUG_OUTP   yLOG_char    ("index"     , n);
   --rce; if (n  < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   DEBUG_OUTP   yLOG_value   ("len"       , s_macros [n].len);
   if (s_macros [n].len <= 0)  {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(keys)---------------------------*/
   strlcpy (x_keys, s_macros [n].keys, LEN_RECD);
   x_keys [s_macros [n].len - 1] = G_KEY_NULL;
   /*---(write)-----------------------*/
   yPARSE_fullwrite ("macro", a_abbr, x_keys);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_macro_writer_all (void)
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
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_end = strlen (S_MACRO_LIST);
   yPARSE_verb_begin ("macro");
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      rc = yvikeys_macro_writer (S_MACRO_LIST [i]);
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
yvikeys_macro_reader    (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_abbr      =    0;
   int         n           =    0;
   char        x_keys      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MACRO)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("macro", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   n  = yvikeys_macro__index (x_abbr);
   DEBUG_INPT   yLOG_char    ("n"         , n);
   --rce; if (n < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get command)--------------------*/
   rc = yPARSE_popstr (x_keys);
   DEBUG_INPT   yLOG_value   ("pop keys"  , rc);
   DEBUG_INPT   yLOG_info    ("x_keys"    , x_keys);
   /*---(save)---------------------------*/
   DEBUG_INPT   yLOG_note    ("saving values");
   yvikeys_macro__clear (x_abbr);
   strlcpy (s_macros [n].keys, x_keys, LEN_RECD );
   strlcat (s_macros [n].keys, "�"   , LEN_RECD );
   s_macros [n].len = strlen (s_macros [n].keys);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__unit     (char *a_question, char a_macro)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   char        x_list      [LEN_RECD];
   char        t           [LEN_HUND];
   int         c           =    0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MACRO unit       : question not understood");
   /*---(simple questions)---------------*/
   if      (strcmp (a_question, "rec"            )   == 0) {
      if (s_rcurr < 0) snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO rec    (%c) : macro pointer grounded", s_rname);
      else {
         sprintf (t, "[%-.33s]", s_macros [s_rcurr].keys);
         if (s_macros [s_rcurr].len > 33)  t [34] = '>';
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO rec    (%c) : %c %2d %2d%s", s_rname, s_rmode, s_macros [s_rcurr].pos, s_macros [s_rcurr].len, t);
      }
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "exec"           )   == 0) {
      if (s_ecurr < 0)  snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO exec   (%c) : macro pointer grounded", s_ename);
      else             snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO exec   (%c) : %c %c %3d %02x %3d[%-.30s]", s_ename, s_emode, s_delay, s_macros [s_ecurr].pos, (uchar) s_macros [s_ecurr].cur, s_macros [s_ecurr].len, s_macros [s_ecurr].keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "keys"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO keys   (%c) : %-.45s", a_macro, s_macros [s_ecurr].keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "list"           )   == 0) {
      yvikeys_macro_list (&c, x_list);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO list       : %2d %-.45s", c, x_list);
      return yVIKEYS__unit_answer;
   }
   /*---(complex questions)--------------*/
   n = yvikeys_macro__index (a_macro);
   if (yvikeys_macro__index (a_macro) < 0) {
      strcpy  (yVIKEYS__unit_answer, "MACRO unit       : not a valid macro name");
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      if (n < 0)  snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO saved  (%c) : not a valid macro name", a_macro);
      else {
         sprintf (t, "[%-.33s]", s_macros [n].keys);
         if (s_macros [n].len > 33)  t [34] = '>';
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO saved  (%c) : %2d%-35.35s %2d %2d %2d", a_macro, s_macros [n].len, t, s_macros [n].pos, s_macros [n].runby, s_macros [n].repeat);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



/*============================----end-of-source---============================*/
