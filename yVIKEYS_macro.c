/*============================---beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*===[[ METIS ]]==============================================================*/
/*
 * metis  ww2o·  macro should abort if anything causes an error
 * metis  ww8o·  global library of loadable macros referred to by short names
 * metis  ww8#·  command to flatten multi-layer macro into one single macro
 * metis  ww8o·  export macros to file for safekeeping and transfer
 * metis  ww8o·  import macros to transfer between applications
 * metis  dm1o·  macro recordings can not use @@ to call (circular)
 * metis  dm2o·  use Ë, to run for a certain number of steps
 *
 */
/*
 *
 * pauses are only useful for a couple reasons...
 *   1) to help debugging, but its easier to slow down whole script
 *   2) to help video scripting allow time to digest/voice over
 *   3) to slow a script when other systems need time to respond
 *
 *   · is a spacer and takes zero time, but is useful for visual clarity
 *     when editing/writing scripts
 *       \·v·w·r·
 *
 *   ´ is a 1 key delay, as if one keystoke was processed, this is
 *     very useful for demonstrations, keyboard is active for controls
 *       \´v´w´r
 *
 *   Ï is a 5 key delay, as if five keystokes were processed, this is
 *     very useful for demonstrations, keyboard is active for controls
 *       \·Ï·v·Ï·w·Ï·r
 *
 *   « is a 0.5s delay, regardless of execution speed, less useful for demo
 *     but more traditional for handling predictable response wait times
 *       \·«·v·«·w·«·r·«
 *       \··«··v··«··w··«··r··«··
 *       \··««··v··««··w··««··r··««·««·««·««·««··
 *
 *    º···» means all character inside these brackets are in blitz mode,
 *      meaning as fast as possible rather than configured execution speed.
 *      blitz mode also means to screen updates, i.e., invisible actions ;)
 *      this is very handy for leaving complications out of demonstrations
 *
 */



#define      S_MACRO_MAX      75
static char  S_MACRO_LIST   [S_MACRO_MAX];



typedef     struct        cMACRO    tMACRO;
struct cMACRO {
   /*---(running)-----------*/
   char        runby;                       /* run by who                     */
   /*---(contents)----------*/
   short       len;                         /* number of keys                 */
   uchar      *keys;                        /* keystrokes                     */
   /*---(execute)-----------*/
   short       pos;                         /* current position               */
   uchar       cur;                         /* current key                    */
   uchar       repeat;                      /* number of repeats              */ /*---(done)--------------*/
};
static tMACRO  s_macros    [S_MACRO_MAX];
static int     s_nmacro    =    0;

static char    s_emode     =  '-';          /* run, playback, delay, etc      */
static char    s_ename     =  '-';
static char    s_ecurr     =   -1;
static char    s_esave     =  '-';          /* saved mode for menus           */
static short   s_epos      =   -1;

static char    s_edelay    = MACRO_BLITZ;   /* execution delay between steps  */
static char    s_ddelay    = MACRO_BLITZ;   /* debug delay between steps      */
static char    s_eupdate   = MACRO_NORMAL;  /* execution sceen update speed   */
static char    s_dupdate   = MACRO_NORMAL;  /* debug sceen update speed       */
static char    s_pause     =    0;
static char    s_skips     =    0;
static char    s_blitzing  =  '-';          /* macro blitzing mode º´´´»      */
static char    s_blitz     =  '-';          /* stay in blitz mode (duration)  */

static char    s_rmode     =  '-';          /* recording or not               */
static char    s_rname     =  '-';
static char    s_rcurr     =   -1;
/*> static char   *s_rbackup   = NULL;                                                <*/
static uchar   s_rkeys     [LEN_RECD];
static short   s_rlen      =    0;
static short   s_rpos      =    0;
static uchar   s_rcur      =  '-';


static char    (*s_loader) (char a_name, char *a_keys);
static char    (*s_saver ) (char a_name, char *a_keys);

#define     MACRO_LOWER    'a'
#define     MACRO_SYSTEM   '0'
#define     MACRO_ALL      '*'

char yvikeys_macro_edelay  (char a_delay);
char yvikeys_macro_eupdate (char a_update);
char yvikeys_macro_ddelay  (char a_delay);
char yvikeys_macro_dupdate (char a_update);


/*====================------------------------------------====================*/
/*===----                        finding/checking                      ----===*/
/*====================------------------------------------====================*/
static void  o___FINDING_________o () { return; }

int  
yvikeys_macro__index    (uchar a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   if (a_abbr == ',')  a_abbr = '.';
   n  = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
   return n;
}

int  
yvikeys_macro__rset     (uchar a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   if (a_abbr == ',')  a_abbr = '.';
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
yvikeys_macro__eset     (uchar a_abbr)
{
   int         n           =   -1;
   if (a_abbr == 0)  return n;
   if (a_abbr == ',')  a_abbr = '.';
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
yvikeys_macro__clear    (uchar a_abbr)
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
   if (s_macros [n].keys != g_stub)  free (s_macros [n].keys);
   s_macros [n].keys      = g_stub;
   s_macros [n].len       =    0;
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
yvikeys_macro__wipe     (uchar a_abbr)
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
   case YVIKEYS_LOWER  : case YVIKEYS_NUMBER :
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
   strlcat (S_MACRO_LIST, gvikeys_number, S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_greek , S_MACRO_MAX);
   strlcat (S_MACRO_LIST, "."           , S_MACRO_MAX);
   DEBUG_PROG   yLOG_info    ("LIST"      , S_MACRO_LIST);
   s_nmacro = strlen (S_MACRO_LIST);
   /*---(clear exec)---------------------*/
   s_emode     = '-';
   s_ename     = '-';
   s_ecurr     =  -1;
   s_edelay    = MACRO_BLITZ;
   s_eupdate   = MACRO_NORMAL;
   s_ddelay    = MACRO_BLITZ;
   s_dupdate   = MACRO_NORMAL;
   s_pause     =  0;
   s_blitz     = '-';
   s_blitzing  = '-';
   /*---(clear rec)----------------------*/
   s_rmode     = '-';
   s_rname     = '-';
   s_rcurr     =  -1;
   s_rkeys [0] = '\0';
   s_rlen      =   0;
   s_rpos      =   0;
   s_rcur      = '-';
   /*---(clear pointers)-----------------*/
   s_loader = NULL;
   s_saver  = NULL;
   /*---(clear data)---------------------*/
   yvikeys_macro__purge (MACRO_ALL);
   strlcpy (myVIKEYS.m_script, "", LEN_DESC);
   /*---(status)-------------------------*/
   /*> yVIKEYS_view_optionX (YVIKEYS_STATUS, "macro"  , yvikeys_macro_estatus , "details of macro playback"                );   <* 
    *> yVIKEYS_view_optionX (YVIKEYS_STATUS, "record" , yvikeys_macro_rstatus , "details of macro recording"               );   <*/
   /*---(update status)------------------*/
   DEBUG_PROG   yLOG_note    ("update status");
   STATUS_init_set   (SMOD_MACRO);
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
   /*---(save to macro)------------------*/
   if (s_rlen == 0)   s_macros [s_rcurr].keys  = g_stub;
   else               s_macros [s_rcurr].keys  = strdup (s_rkeys);
   s_macros [s_rcurr].len   = strlen (s_rkeys);
   /*---(trim)---------------------------*/
   strlcpy (x_keys, s_rkeys, LEN_RECD);
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
   uchar       x_ch        =  ' ';
   char        x_index     =    0;
   /*---(quick out)----------------------*/
   if (s_ename == '.')  return 0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prefetch)--------------------*/
   if (s_loader != NULL) {
      DEBUG_SCRP   yLOG_value   ("s_ecurr"   , s_ecurr);
      DEBUG_SCRP   yLOG_char    ("s_ename"   , s_ename);
      yvikeys_macro__wipe (s_ename);
      s_loader (s_ename, s_macros [s_ecurr].keys);
      if (s_macros [s_ecurr].keys != NULL) {
         s_macros [s_ecurr].len = strlen (s_macros [s_ecurr].keys);
         x_ch = s_macros [s_ecurr].keys [s_macros [s_ecurr].len - 1];
         if (x_ch != G_CHAR_HALT)  s_macros [s_ecurr].keys [s_macros [s_ecurr].len++] = G_CHAR_HALT;
         s_macros [s_ecurr].keys [s_macros [s_ecurr].len  ] = G_KEY_NULL;
      } else {
         s_macros [s_ecurr].len = 0;
      }
   }
   /*---(information)-----------------*/
   DEBUG_SCRP   yLOG_info    ("macro"     , s_macros [s_ecurr].keys);
   DEBUG_SCRP   yLOG_value   ("len"       , s_macros [s_ecurr].len);
   /*---(set globals)-----------------*/
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
   if (a_name == tolower (a_name) || s_macros [s_rcurr].len <= 0) {
      strlcpy (s_rkeys, "¤", LEN_RECD);
   } else {
      strlcpy (s_rkeys, s_macros [s_rcurr].keys, LEN_RECD);
   }
   s_rlen = strlen (s_rkeys);
   s_rpos = s_rlen - 2;
   /*---(turn on record)--------------*/
   DEBUG_SCRP   yLOG_value   ("keys"      , s_rkeys);
   SET_MACRO_RECORD;
   DEBUG_SCRP   yLOG_char    ("s_rmode"   , s_rmode);
   if (myVIKEYS.loud == 'y')  yvikeys_sizes_switch ("status", "record");
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
      s_rcur               = a_key;
      s_rkeys [s_rlen - 1] = a_key;
      s_rkeys [s_rlen++  ] = G_CHAR_PLACE;
      s_rkeys [s_rlen    ] = G_KEY_NULL;
      s_rpos               = s_rlen - 2;
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
      if (s_rkeys [s_rlen - 1] == G_CHAR_PLACE) {
         s_rkeys [--s_rlen  ] =  G_KEY_NULL;
      }
      if (s_rkeys [s_rlen - 1] == G_CHAR_HALT ) {
         s_rkeys [--s_rlen  ] =  G_KEY_NULL;
      }
      /*---(add keys)-----------------------*/
      if (a_keys == NULL)  strlcat (s_rkeys, ""    , LEN_RECD);
      else                 strlcat (s_rkeys, a_keys, LEN_RECD);
      s_rlen  = strlen (s_rkeys);
      /*---(fix keys)-----------------------*/
      strlencode   (s_rkeys, ySTR_MAX, LEN_RECD);
      /*---(add suffix)---------------------*/
      s_rkeys [s_rlen  ] =  G_CHAR_PLACE;
      s_rkeys [++s_rlen] =  G_KEY_NULL;
      /*---(update pos/char)----------------*/
      s_rpos = s_rlen - 2;
      if (s_rpos < 0)  s_rpos = 0;
      /*> printf ("s_rkeys <<%s>>\n", s_rkeys);                                <*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> complete a macro recording ---------[ ------ [gz.420.001.21]*/ /*-[02.0000.113.5]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_recend    (void)
{
   IF_MACRO_RECORDING {
      if (s_rlen > 2 && s_rkeys [s_rlen - 2] == 'q') {
         s_rkeys [s_rlen - 2] = G_CHAR_HALT;
         s_rkeys [s_rlen - 1] = G_KEY_NULL;
         --s_rlen;
         yvikeys_macro__save ();
         yvikeys_macro__wipe (s_rname);
      } else if (s_rlen == 2 && s_rkeys [s_rlen - 2] == 'q') {
         s_rkeys [0] = G_KEY_NULL;
         s_rlen      = 0;
         yvikeys_macro__save ();
         yvikeys_macro__wipe (s_rname);
      }
      s_rkeys [0] = G_KEY_NULL;
      s_rlen      = 0;
      s_rpos      =   0;
      s_rcur      = '-';
      SET_MACRO_IGNORE;
      s_rcurr = -1;
   }
   return 0;
}

/*
 *   :macro a                purges lower case macros
 *   :macro A                purges upper case macros
 *   :macro è                purges greek letter macros
 *   :macro 0                purges numeric macros
 *   :macro a=               clears macro 'a'
 *   :macro a=...            assigns keys (...) to macro 'a'
 *   :macro a>               writes macro 'a' to vi_clip.txt
 *   :macro a<               reads macro 'a' from vi_clip.txt
 *   :macro a!---.---.---    assigns global macro ---.---.--- to 'a'
 *   :macro
 *
 *
 *
 */

char
yvikeys_macro__recdir   (char a_id, char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   char       *p           = NULL;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(begin recording)----------------*/
   DEBUG_SCRP   yLOG_note    ("three plus character option (define)");
   rc = yvikeys_macro__recbeg (a_id);
   DEBUG_SCRP   yLOG_value   ("recbeg"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (a_id);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix quotes)---------------------*/
   x_len = strlen (a_string);
   p = a_string + 2;
   if (a_string [2] == G_KEY_DQUOTE && a_string [x_len - 1] == G_KEY_DQUOTE) {
      DEBUG_SCRP   yLOG_note    ("quoted macro format");
      a_string [--x_len] = G_KEY_NULL;
      p = a_string + 3;
   }
   /*---(add keys)-----------------------*/
   rc = yvikeys_macro__recstr (p);
   DEBUG_SCRP   yLOG_value   ("recstr"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (a_id);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(append end record key)----------*/
   rc = yvikeys_macro_reckey ('q');
   /*---(end recording)------------------*/
   rc = yvikeys_macro_recend ();
   DEBUG_SCRP   yLOG_value   ("recend"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro__clear (a_id);
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro__export   (char a_id)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   n = yvikeys_macro__index (a_id);
   --rce;  if (n <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(write)--------------------------*/
   rc = yvikeys_dump_write (s_macros [n].keys);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_macro__import   (char a_id)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   char        x_direct    [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read)---------------------------*/
   rc = yvikeys_dump_read  (0, x_recd, NULL);
   DEBUG_SCRP   yLOG_value   ("read"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("x_recd"    , x_recd);
   /*---(add)----------------------------*/
   sprintf (x_direct, "%c=%s", a_id, x_recd);
   DEBUG_SCRP   yLOG_info    ("x_direct"  , x_direct);
   rc = yvikeys_macro__recdir (a_id, x_direct);
   DEBUG_SCRP   yLOG_value   ("recdir"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_macro__copy     (char a_id, char a_src)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        s           =   -1;
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char        x_direct    [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_id"      , a_id);
   DEBUG_SCRP   yLOG_char    ("a_src"     , a_src);
   s = yvikeys_macro__index (a_src);
   DEBUG_SCRP   yLOG_value   ("s"         , s);
   --rce;  if (s <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(copy)---------------------------*/
   strlcpy (x_recd, s_macros [s].keys, LEN_RECD);
   x_len = strlen (x_recd);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <=  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_char    ("tail"      , chrvisible (x_recd [x_len - 1]));
   if (x_recd [x_len - 1] == '³' )  x_recd [--x_len] = '\0';
   sprintf (x_direct, "%c=%s", a_id, x_recd);
   DEBUG_SCRP   yLOG_info    ("x_direct"  , x_direct);
   rc = yvikeys_macro__recdir (a_id, x_direct);
   DEBUG_SCRP   yLOG_value   ("recdir"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_macro__2sreg    (char a_id, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_id"      , a_id);
   n = yvikeys_macro__index (a_id);
   DEBUG_SCRP   yLOG_value   ("n"         , n);
   --rce;  if (n <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(copy)---------------------------*/
   strlcpy (x_recd, s_macros [n].keys, LEN_RECD);
   x_len = strlen (x_recd);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <=  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_char    ("tail"      , chrvisible (x_recd [x_len - 1]));
   if (x_recd [x_len - 1] == '³' )  x_recd [--x_len] = '\0';
   rc = yvikeys_sreg_push       (a_reg, x_recd);
   DEBUG_SCRP   yLOG_value   ("push"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_macro__4sreg    (char a_id, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char        x_direct    [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_id"      , a_id);
   /*---(copy)---------------------------*/
   rc = yvikeys_sreg_pop        (a_reg, x_recd);
   DEBUG_SCRP   yLOG_value   ("pop"       , rc);
   --rce;  if (rc <   0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   sprintf (x_direct, "%c=%s", a_id, x_recd);
   DEBUG_SCRP   yLOG_info    ("x_direct"  , x_direct);
   rc = yvikeys_macro__recdir (a_id, x_direct);
   DEBUG_SCRP   yLOG_value   ("recdir"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_macro__central  (char a_id, char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   FILE       *f           = NULL;
   char        x_recd      [LEN_RECD];
   char        x_seek      [LEN_RECD];
   char        x_key       [LEN_RECD];
   int         x_len       =    0;
   char        x_direct    [LEN_RECD];
   char        x_found     =  '-';
   int         c           =    0;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   strlcpy (x_seek, a_string + 2, 42);
   x_len = strlen (x_seek);
   DEBUG_SCRP   yLOG_info    ("x_seek"    , x_seek);
   /*---(read)---------------------------*/
   f = fopen (FILE_REPO, "r");
   DEBUG_SCRP   yLOG_point   ("f"         , f);
   --rce;  if (f == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  while (x_found == '-') {
      if (feof (f)) {
         DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      fgets (x_recd, LEN_RECD, f);
      ++c;
      if (x_recd [0] == '\0')         continue;
      if (x_recd [0] == '\n')         continue;
      if (x_recd [0] == '#')          continue;
      if (x_recd [0] == ' ')          continue;
      DEBUG_SCRP   yLOG_info    ("x_recd"    , x_recd);
      if (x_recd [0] != x_seek [0])  continue;
      if (x_len == 6)  sprintf (x_key, "%2.2s%2.2s%2.2s", x_recd, x_recd + 15, x_recd + 30);
      else {
         strlcpy  (x_key, x_recd, 42);
         strldchg (x_key, '', '.', LEN_RECD);
         strltrim (x_key, ySTR_MAX, LEN_RECD);
      }
      DEBUG_SCRP   yLOG_info    ("x_key"     , x_key);
      if (strcmp (x_seek, x_key) != 0)  continue;
      x_found = 'y';
   }
   fclose (f);
   DEBUG_SCRP   yLOG_value   ("c"         , c);
   DEBUG_SCRP   yLOG_char    ("x_found"   , x_found);
   --rce;  if (x_found != 'y') {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   p = x_recd + 98;
   x_len = strlen (p);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   DEBUG_SCRP   yLOG_char    ("tail"      , chrvisible (p [x_len - 1]));
   if (p [x_len - 1] == '\n')  p [--x_len] = '\0';
   DEBUG_SCRP   yLOG_char    ("tail"      , chrvisible (p [x_len - 1]));
   if (p [x_len - 1] == '³' )  p [--x_len] = '\0';
   DEBUG_SCRP   yLOG_info    ("p"         , p);
   sprintf (x_direct, "%c=%s", a_id, p);
   DEBUG_SCRP   yLOG_info    ("x_direct"  , x_direct);
   rc = yvikeys_macro__recdir (a_id, x_direct);
   DEBUG_SCRP   yLOG_value   ("recdir"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> enter a macro directly -------------[ ------ [ge.850.137.A4]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__direct   (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_id        =  '-';
   char        x_div       =  '-';
   char       *x_valid     = "*aA0è";
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
   x_id  = a_string [0];
   DEBUG_SCRP   yLOG_char    ("x_id"      , x_id);
   x_div = a_string [1];
   DEBUG_SCRP   yLOG_char    ("x_div"     , x_div);
   /*---(check for purge)----------------*/
   --rce;  if (x_len == 1) {
      rc = yvikeys_macro__purge  (x_id);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check for shorts)---------------*/
   --rce;  if (x_len == 2) {
      switch (x_div) {
      case '=' : rc = yvikeys_macro__clear  (x_id);        break;
      case '-' : rc = yvikeys_macro__export (x_id);        break;
      case '+' : rc = yvikeys_macro__import (x_id);        break;
      case '>' : rc = yvikeys_macro__2sreg  (x_id, '"');   break;
      case '<' : rc = yvikeys_macro__4sreg  (x_id, '"');   break;
      default  : rc = yvikeys_macro__copy   (x_id, x_div); break;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(copy)---------------------------*/
   --rce;  if (x_len == 3 && x_div == '>') {
      rc = yvikeys_macro__2sreg   (x_id, a_string [2]);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   --rce;  if (x_len == 3 && x_div == '<') {
      rc = yvikeys_macro__4sreg   (x_id, a_string [2]);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(recording)----------------------*/
   --rce;  if (x_len >  2 && x_div == '=') {
      rc = yvikeys_macro__recdir  (x_id, a_string);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(global)-------------------------*/
   --rce;  if (x_len >  2 && x_div == '!') {
      rc = yvikeys_macro__central (x_id, a_string);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       speed and controls                     ----===*/
/*====================------------------------------------====================*/
static void  o___SPEEDS__________o () { return; }

char         /*-> set or adjust delay value ----------[ leaf   [gz.612.101.50]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__delay    (char a_which, char a_delay)
{
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(set)----------------------------*/
   if (a_delay >= MACRO_BLITZ && a_delay <= MACRO_TRIPLE) {
      DEBUG_SCRP   yLOG_note    ("set abs");
      if (a_which == 'd')  s_ddelay = a_delay;
      else                 s_edelay = a_delay;
   }
   if (a_delay == '-') {
      DEBUG_SCRP   yLOG_note    ("decrease (-)");
      if (s_ddelay > MACRO_BLITZ) {
         if (a_which == 'd')  --s_ddelay;
         else                 --s_edelay;
      }
   }
   if (a_delay == '+') {
      DEBUG_SCRP   yLOG_note    ("increase (+)");
      if (s_ddelay < MACRO_TRIPLE) {
         if (a_which == 'd')  ++s_ddelay;
         else                 ++s_edelay;
      }
   }
   /*---(new value)-------------------*/
   DEBUG_SCRP   yLOG_schar   (s_edelay);
   DEBUG_SCRP   yLOG_schar   (s_ddelay);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> set or adjust update value ---------[ leaf   [gz.612.101.50]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__update   (char a_which, char a_update)
{
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_schar   (a_update);
   /*---(set)----------------------------*/
   DEBUG_SCRP   yLOG_note    ("set abs");
   if (a_which == 'd')  s_dupdate = a_update;
   else                 s_eupdate = a_update;
   /*---(new value)-------------------*/
   DEBUG_SCRP   yLOG_schar   (s_eupdate);
   DEBUG_SCRP   yLOG_schar   (s_dupdate);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_macro_edelay  (char a_delay)    { return yvikeys_macro__delay  ('e', a_delay);  }
char yvikeys_macro_eupdate (char a_update)   { return yvikeys_macro__update ('e', a_update); }

char yvikeys_macro_ddelay  (char a_delay)    { return yvikeys_macro__delay  ('d', a_delay);  }
char yvikeys_macro_dupdate (char a_update)   { return yvikeys_macro__update ('d', a_update); }

char
yvikeys_macro_set2stop  (void)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   SET_MACRO_STOP;
   s_macros [s_ecurr].pos    = -1;
   s_macros [s_ecurr].cur    = '·';
   s_macros [s_ecurr].repeat =  0;
   s_ecurr = -1;
   s_epos  = -1;
   yvikeys_loop_normal ();
   return 0;
}

char
yvikeys_macro_set2play  (void)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   SET_MACRO_PLAYBACK;
   yvikeys_loop_normal ();
   if (myVIKEYS.loud == 'y')  yvikeys_sizes_switch ("status", "macro");
   return 0;
}

char
yvikeys_macro_set2delay (void)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   SET_MACRO_DELAY;
   yvikeys_loop_macro (s_ddelay, s_dupdate);
   if (myVIKEYS.loud == 'y')  yvikeys_sizes_switch ("status", "macro");
   return 0;
}

char
yvikeys_macro_set2run   (void)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   DEBUG_SCRP   yLOG_char    ("blitzing"  , s_blitzing);
   if (s_blitzing == 'y') {
      yvikeys_macro_set2blitz ();
   } else {
      SET_MACRO_RUN;
      yvikeys_loop_macro (s_edelay, s_eupdate);
      if (myVIKEYS.loud == 'y')  yvikeys_sizes_switch ("status", "macro");
   }
   return 0;
}

char
yvikeys_macro_set2blitz (void)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   DEBUG_SCRP   yLOG_char    ("blitzing"  , s_blitzing);
   /*> if (s_blitzing == 'y') return 0;                                               <*/
   SET_MACRO_RUN;
   yvikeys_loop_macro (MACRO_BLITZ, MACRO_BLIND);
   s_blitzing = 'y';
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         menu support                         ----===*/
/*====================------------------------------------====================*/
static void  o___MENUS___________o () { return; }

char
yvikeys_macro_menu_beg  (void)
{
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   IF_MACRO_PLAYING  {
      DEBUG_SCRP   yLOG_char    ("emode bef" , s_emode);
      s_esave = s_emode;
      DEBUG_SCRP   yLOG_char    ("esave"     , s_esave);
      yvikeys_macro_set2blitz ();
      SET_MACRO_STOP;
      DEBUG_SCRP   yLOG_char    ("emode aft" , s_emode);
   } else {
      DEBUG_SCRP   yLOG_note    ("nothing to do");
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_menu_end  (void)
{
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   if (s_esave != '?') {
      DEBUG_SCRP   yLOG_char    ("emode bef" , s_emode);
      DEBUG_SCRP   yLOG_char    ("esave"     , s_esave);
      s_emode = s_esave;
      DEBUG_SCRP   yLOG_char    ("emode aft" , s_emode);
      switch (s_emode) {
      case MACRO_RUN      :
         if      (s_blitz    == 'y')  yvikeys_macro_set2blitz ();
         else if (s_blitzing == 'y')  yvikeys_macro_set2blitz ();
         else                         yvikeys_macro_set2run   ();
         break;
      case MACRO_DELAY    :
         yvikeys_macro_set2delay ();
         break;
      case MACRO_PLAYBACK :
         yvikeys_macro_set2play  ();
         break;
      }
   } else {
      DEBUG_SCRP   yLOG_note    ("nothing to do");
   }
   s_esave = '?';
   DEBUG_SCRP   yLOG_char    ("esave aft" , s_esave);
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         script files                         ----===*/
/*====================------------------------------------====================*/
static void  o___SCRIPT__________o () { return; }

static  FILE  *s_script = NULL;
static  int    s_line   =    0;
static  char   s_style  =  '.';

char
yvikeys_script_open     (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_name      [LEN_HUND]  = "";
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_SCRP   yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("a_name"    , a_name);
   /*---(fix)----------------------------*/
   if (strstr (a_name, ".script") == NULL)  sprintf (x_name, "%s.script", a_name);
   else                                     strlcpy (x_name, a_name, LEN_HUND);
   /*---(prepare)------------------------*/
   DEBUG_SCRP   yLOG_point   ("s_script"  , s_script);
   --rce;  if (s_script != NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("x_name"    , x_name);
   /*---(read)---------------------------*/
   s_script = fopen (x_name, "r");
   DEBUG_SCRP   yLOG_point   ("s_script"  , s_script);
   --rce;  if (s_script == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_script_read     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       x_recd      [LEN_RECD];
   uchar       x_ready     [LEN_RECD];
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_point   ("s_script"  , s_script);
   --rce;  if (s_script == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(next line)----------------------*/
   --rce;  while (1) {
      /*---(done with line)-----------------*/
      DEBUG_SCRP   yLOG_value   ("feof"      , feof (s_script));
      if (feof (s_script)) {
         yvikeys_script_close ();
         DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(process line)-------------------*/
      fgets (x_recd, LEN_RECD, s_script);
      ++s_line;
      if (x_recd [0] <  ' ') {
         DEBUG_SCRP   yLOG_note    ("blank leader, skipping");
         continue;
      }
      if (x_recd [0] == '#') {
         DEBUG_SCRP   yLOG_note    ("comment marker, skipping");
         continue;
      }
      x_len = strlen (x_recd);
      if (x_recd [x_len - 1] == '\n')  x_recd [--x_len] = '\0';
      DEBUG_SCRP   yLOG_info    ("x_recd"    , x_recd);
      /*---(functions)----------------------*/
      if (strncmp (x_recd, "macro ", 6) == 0) {
         rc = yvikeys_macro__recdir (x_recd [6], x_recd + 6);
         DEBUG_SCRP   yLOG_value   ("macro"     , rc);
         if (rc < 0) {
            DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(normal)-------------------------*/
      else {
         sprintf (x_ready, ".=%s", x_recd);
         rc = yvikeys_macro__recdir ('.', x_ready);
         DEBUG_SCRP   yLOG_value   ("normal"    , rc);
         if (rc < 0) {
            DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         rc = yvikeys_macro_exebeg  (s_style);
         DEBUG_SCRP   yLOG_value   ("execute"   , rc);
         if (rc < 0) {
            DEBUG_SCRP   yLOG_note    ("can not execute");
            DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         break;
      }
      /*---(done)---------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_script_close    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(clear script macros)------------*/
   yvikeys_macro__purge (YVIKEYS_GREEK);
   yvikeys_macro__clear ('.');
   /*---(prepare)------------------------*/
   DEBUG_SCRP   yLOG_point   ("s_script"  , s_script);
   --rce;  if (s_script == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = fclose (s_script);
   DEBUG_SCRP   yLOG_value   ("fclose"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(ground pointer)-----------------*/
   s_script = NULL;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_script__start   (char *a_name, char a_style)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   rc = yvikeys_script_open (a_name);
   DEBUG_SCRP   yLOG_value   ("open"      , rc);
   if (rc < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_style = a_style;
   DEBUG_SCRP   yLOG_char    ("s_style"   , s_style);
   if (s_blitz == 'y')  s_blitzing = 'y';
   DEBUG_SCRP   yLOG_char    ("s_blitz"   , s_blitz);
   DEBUG_SCRP   yLOG_char    ("s_blitzing", s_blitzing);
   rc = yvikeys_script_read ();
   DEBUG_SCRP   yLOG_value   ("read"      , rc);
   if (rc < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_macros [s_ecurr].pos       =   -1;
   /*> rc = yvikeys_macro_exebeg  ('.');                                              <* 
    *> DEBUG_SCRP   yLOG_value   ("execute"   , rc);                                  <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_SCRP   yLOG_note    ("can not execute");                              <* 
    *>    DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_script_start    (char *a_name) { return yvikeys_script__start (a_name, '.'); }
char yvikeys_script_follow   (char *a_name) { return yvikeys_script__start (a_name, '.'); }
char yvikeys_script_playback (char *a_name) { return yvikeys_script__start (a_name, ','); }
char yvikeys_script_blitz    (char *a_name) { s_blitz  = 'y';  return yvikeys_script__start (a_name, '.'); }



/*====================------------------------------------====================*/
/*===----                         macro execution                      ----===*/
/*====================------------------------------------====================*/
static void  o___EXECUTE_________o () { return; }

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
   x_curr  = s_ecurr;
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
   /*---(execution style)-------------*/
   IF_MACRO_OFF {
      /*---(normal)-------------------*/
      if (a_name == tolower (a_name) && a_name != ',') {
         DEBUG_SCRP   yLOG_note    ("normal execution");
         if (s_ddelay    != '0'      )  SET_MACRO_DELAY;
         IF_MACRO_OFF                   SET_MACRO_RUN;
      }
      /*---(debugging/playback)-------*/
      else {
         DEBUG_SCRP   yLOG_note    ("debug/playback execution");
         SET_MACRO_PLAYBACK;
      }
   }
   /*---(macro name)------------------*/
   DEBUG_SCRP   yLOG_char    ("macro_name", s_ename);
   /*---(get macro)-------------------*/
   yvikeys_macro__fetch ();
   /*---(update settings)-------------*/
   s_epos  = -1;
   s_pause =  0;
   s_skips =  0;
   yvikeys_macro_ereset ();
   if (strchr (".,", a_name) == NULL)  yvikeys_macro_exeadv (0);
   s_macros [s_ecurr].runby  = x_curr;
   s_macros [s_ecurr].repeat = REPEAT_count ();
   REPEAT_macro ();
   DEBUG_USER   yLOG_value   ("repeat"    , s_macros [s_ecurr].repeat);
   /*---(reset main delay)---------------*/
   DEBUG_SCRP   yLOG_char    ("s_blitz"   , s_blitz);
   DEBUG_SCRP   yLOG_char    ("s_blitzing", s_blitzing);
   if (s_blitz == 'y')         yvikeys_macro_set2blitz ();
   else {
      IF_MACRO_RUN             yvikeys_macro_set2run   ();
      else IF_MACRO_PLAYBACK   yvikeys_macro_set2play  ();
      else                     yvikeys_macro_set2delay ();
   }
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_skips     (void)
{
   ++s_skips;
   DEBUG_SCRP   yLOG_sint    (s_skips);
   DEBUG_SCRP   yLOG_sint    (myVIKEYS.macro_skip);
   if (s_skips < myVIKEYS.macro_skip) {
      DEBUG_SCRP   yLOG_snote   ("no position update due yet");
      return -1;
   }
   s_skips = 0;
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.220.002.20]*/ /*-[00.0000.102.7]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exeadv    (uchar a_play)
{
   char        rc          =   -1;
   uchar       x_ch        =    0;
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_sint    (a_play);
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
      if (yvikeys_macro_skips () < 0) {
         DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      break;
   case MACRO_PLAYBACK :
      DEBUG_SCRP   yLOG_snote   ("playback mode");
      x_ch = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
      DEBUG_SCRP   yLOG_schar   (x_ch);
      if (a_play != 0) {
         DEBUG_SCRP   yLOG_snote   ("play character, no position update requested");
         DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      break;
   case MACRO_RUN      :
      if (yvikeys_macro_skips () < 0) {
         DEBUG_SCRP   yLOG_snote   ("normal mode, in a skip cycle");
         DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
         return 0;
      }
      DEBUG_SCRP   yLOG_snote   ("normal run mode");
      break;
   }
   /*---(next key)-----------------------*/
   if (yvikeys_keys_repeating ()) {
      DEBUG_LOOP   yLOG_snote   ("older keys");
      s_macros [s_ecurr].cur = yvikeys_keys_keygpos ();
      yvikeys_keys_repos (yvikeys_keys_gpos + 1);
   } else if (s_pause > 0) {
      DEBUG_LOOP   yLOG_snote   ("pausing");
      DEBUG_SCRP   yLOG_sint    (s_macros [s_ecurr].pos);
      --s_pause;
   } else {
      DEBUG_LOOP   yLOG_snote   ("new keystroke");
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
   uchar       x_uch       =  ' ';
   int         x_runby     =   -1;
   static int  x_pause     =    0;
   char        x_last      =  '-';
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_ecurr"   , s_ecurr);
   DEBUG_SCRP   yLOG_char    ("abbr"      , S_MACRO_LIST [s_ecurr]);
   DEBUG_SCRP   yLOG_value   ("pos"       , s_macros [s_ecurr].pos);
   /*---(handle playback/delay)----------*/
   DEBUG_SCRP   yLOG_value   ("s_epos"    , s_epos);
   if (s_macros [s_ecurr].pos == s_epos) {
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return G_KEY_NOOP;  /* return a no-action */
   }
   /*---(handle end of macro)------------*/
   DEBUG_SCRP   yLOG_value   ("runby"     , s_macros [s_ecurr].runby);
   DEBUG_SCRP   yLOG_value   ("repeat"    , s_macros [s_ecurr].repeat);
   --rce;  if (s_macros [s_ecurr].pos >= s_macros [s_ecurr].len - 1) {
      /*---(restart)------------------------*/
      if (s_macros [s_ecurr].repeat > 0) {
         DEBUG_SCRP   yLOG_note    ("repeats left, so restart");
         s_macros [s_ecurr].pos    =  0;
         s_epos = -1;
         s_macros [s_ecurr].cur    = '·';
         --s_macros [s_ecurr].repeat;
      }
      /*---(pop)----------------------------*/
      else if (s_macros [s_ecurr].runby >= 0) {
         DEBUG_SCRP   yLOG_note    ("return to caller/runby");
         s_macros [s_ecurr].pos    = -1;
         s_epos = -1;
         s_macros [s_ecurr].cur    = '·';
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
         /*---(check script)-----------*/
         x_last = s_ename;
         DEBUG_SCRP   yLOG_char    ("x_last"     , x_last);
         yvikeys_macro_set2stop ();
         if (x_last == '.')  {
            rc = yvikeys_script_read ();
            if (rc < 0)  s_blitz = '-';
         } else s_blitz = '-';
         DEBUG_SCRP   yLOG_char    ("s_blitz"   , s_blitz);
         DEBUG_SCRP   yLOG_char    ("s_blitzing", s_blitzing);
         /*> s_macros [s_ecurr].pos    = -1;                                          <* 
          *> s_macros [s_ecurr].cur    = '·';                                         <* 
          *> s_macros [s_ecurr].repeat =  0;                                          <*/
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return G_KEY_NOOP;
      }
   }
   /*---(get next key)-------------------*/
   x_uch = s_macros [s_ecurr].keys [s_macros [s_ecurr].pos];
   DEBUG_SCRP   yLOG_value   ("x_uch"     , x_uch);
   /*---(check key)----------------------*/
   if (x_uch == G_CHAR_SPACE) {
      if (yVIKEYS_mode () == UMOD_SRC_INPT) {
         DEBUG_SCRP   yLOG_note    ("found a spacer (·) in input mode");
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      else if (yVIKEYS_mode () == UMOD_SENDKEYS) {
         DEBUG_SCRP   yLOG_note    ("found a spacer (·) in sendkeys mode");
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         /*> return G_CHAR_STORAGE;                                                   <*/
         return G_KEY_NOOP;
      }
      IF_MACRO_RUN {
         DEBUG_SCRP   yLOG_note    ("found a spacer (·) in macro run mode");
         s_skips = myVIKEYS.macro_skip;
         ++s_epos;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return G_KEY_ACK;
      } else {
         DEBUG_SCRP   yLOG_note    ("found a spacer (·) in macro playback/debug mode");
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return G_KEY_NOOP;
      }
   }
   /*---(translate)----------------------*/
   x_uch = chrworking (x_uch);
   DEBUG_SCRP   yLOG_char    ("x_uch"     , x_uch);
   /*---(handle controls)-------------*/
   if (x_uch < 0 || x_uch > 127) {
      x_uch = yvikeys_macro__exectl (x_uch);
   }
   /*---(next)------------------------*/
   s_epos = s_macros [s_ecurr].pos;   /* x_pos is static, so this is key */
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return x_uch;
}

char         /*-> tbd --------------------------------[ ------ [gc.340.102.21]*/ /*-[00.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__exectl   (uchar a_key)
{
   float       x_delay     =  0.0;
   if (a_key >= 0 && a_key <= 127)  return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("a_key"     , a_key);
   switch (a_key) {
   case G_CHAR_SLBRACK :
      DEBUG_SCRP   yLOG_note    ("blitz (º)");
      IF_MACRO_RUN {
         yvikeys_macro_set2blitz ();
         s_blitzing = 'y';
      }
      else  DEBUG_SCRP   yLOG_note    ("blitzing not active in debug/playback");
      a_key = G_KEY_ACK;
      break;
   case  G_CHAR_SRBRACK :
      DEBUG_SCRP   yLOG_note    ("unblitz (»)");
      IF_MACRO_RUN {
         if (s_blitz != 'y') {
            s_blitzing = '-';
            yvikeys_macro_set2run   ();
         }
      }
      else  DEBUG_SCRP   yLOG_note    ("blitzing not active in debug/playback");
      a_key = G_KEY_ACK;
      break;
   case  G_CHAR_BIGDOT  :
      DEBUG_SCRP   yLOG_note    ("wait one tick/beat (´)");
      if (yVIKEYS_mode () != UMOD_SRC_INPT)  a_key = G_KEY_NOOP;
      break;
   case G_CHAR_HUGEDOT :
      IF_MACRO_RUN {
         DEBUG_SCRP   yLOG_note    ("wait five tick/beat (Ï), pause only in run");
         s_pause =  4;
      } else {
         DEBUG_SCRP   yLOG_note    ("wait five tick/beat (Ï), becomes one beat in playback/debug");
         s_pause =  0;
      }
      a_key = G_KEY_NOOP;
      break;
   case G_CHAR_WAIT    :
      DEBUG_SCRP   yLOG_note    ("wait («)");
      DEBUG_SCRP   yLOG_double  ("delay"     , myVIKEYS.delay);
      DEBUG_SCRP   yLOG_value   ("skip"      , myVIKEYS.macro_skip);
      x_delay = myVIKEYS.delay * (1 + myVIKEYS.macro_skip);
      DEBUG_SCRP   yLOG_double  ("x_delay"   , x_delay);
      IF_MACRO_NOT_RUN {
         DEBUG_SCRP   yLOG_note    ("pauses not useful in debug/playback");
         s_pause =  0;
      } else if (x_delay >= 0.500) {
         DEBUG_SCRP   yLOG_note    ("running too slow, pauses not required");
         s_pause =  0;
      } else if (x_delay < 0.009) {
         DEBUG_SCRP   yLOG_note    ("running too fast, pauses are proportional");
         s_pause = 19;       /* for total of 20 loops in main */
      } else {
         DEBUG_SCRP   yLOG_note    ("sweet spot, pauses are exactly 0.5s");
         s_pause = trunc (0.5 / x_delay) - 1;
      }
      DEBUG_SCRP   yLOG_value   ("s_pause"   , s_pause);
      a_key = G_KEY_NOOP;
      break;
   case G_CHAR_BREAK   :
      DEBUG_SCRP   yLOG_note    ("break (ª)");
      yvikeys_macro_set2play ();
      a_key = G_KEY_NOOP;
      break;
   case G_CHAR_DISPLAY :
      DEBUG_SCRP   yLOG_note    ("display (©)");
      a_key = G_KEY_NOOP;
      break;
   case G_CHAR_SPACE   :
      DEBUG_SCRP   yLOG_note    ("macro spacer (·), fast skip only in run");
      IF_MACRO_RUN  a_key = G_KEY_ACK;
      else          a_key = G_KEY_NOOP;
      break;
   case G_CHAR_HALT    :
      DEBUG_SCRP   yLOG_note    ("halt (³)");
      yvikeys_macro_set2stop ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   default             :
      DEBUG_SCRP   yLOG_note    ("other key, pass through");
      break;
   }
   DEBUG_SCRP   yLOG_value   ("a_key"     , a_key);
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return a_key;
}

char         /*-> tbd --------------------------------[ ------ [gc.A43.108.12]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exeplay   (uchar a_key)
{
   char        rc          =    1;
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("a_key"     , a_key);
   switch (a_key) {
   case '0' : case '1' : case '2' : case '3' : case '4' :
   case '5' : case '6' : case '7' : case '8' : case '9' :
   case '-'      : case '+'      :
      yvikeys_macro__delay ('d', a_key);
      break;
   case 'n' : case 's' : case 'b' : case 'p' : case 'd' :
      s_dupdate = a_key;
      break;
   case ',' :
      DEBUG_SCRP   yLOG_note    ("delay");
      yvikeys_macro_set2delay ();
      break;
   case '.' :
      DEBUG_SCRP   yLOG_note    ("playback");
      yvikeys_macro_set2play ();
      break;
   case G_KEY_ESCAPE : case G_CHAR_ESCAPE :
      DEBUG_SCRP   yLOG_note    ("escape");
      yvikeys_macro_set2stop ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_RETURN : case G_KEY_ENTER  : case G_CHAR_RETURN :
      DEBUG_SCRP   yLOG_note    ("return");
      yvikeys_macro_set2run   ();
      break;
   default  :
      rc = 0;
      break;
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     sub-mode key handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process macro sub-mode keys --------[ ------ [ge.H65.229.88]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_smode     (uchar a_major, uchar a_minor)
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
/*> macro   a 0 n -- --- --- ··´····´···                                              <*/
/*> 123456789-123456789-123456789-123456                                              <*/
/*> a 99 ····´····´···· ´ ····´····´····                        <*/
/*> ··´····´····´····´····´···· ´ ····´····´····´····´····´··                         <*/

/*> char                                                                                                                                                                           <* 
 *> yvikeys_macro_edots          (char *a_out)                                                                                                                                     <* 
 *> {  /+---(design notes)-----------------------+/                                                                                                                                <* 
 *>    /+  27 is for full size version                                                                                                                                             <* 
 *>     *  15 is for metis like width                                                                                                                                              <* 
 *>     *                                                                                                                                                                          <* 
 *>     +/                                                                                                                                                                         <* 
 *>    short       x_size      =  14;                                                                                                                                              <* 
 *>    char        x_bef       [LEN_RECD] = "";                                                                                                                                    <* 
 *>    char        x_aft       [LEN_RECD] = "";                                                                                                                                    <* 
 *>    short       x_pos       =    0;                                                                                                                                             <* 
 *>    short       x_keys      =   0;                                                                                                                                              <* 
 *>    short       x_fill      =   0;                                                                                                                                              <* 
 *>    int         x_rem       =    0;                                                                                                                                             <* 
 *>    char        x_dots      [LEN_HUND] = "··+····+····+····+····+···· + ····+····+····+····+····+··";                                                                           <* 
 *>    /+---(default)------------------------+/                                                                                                                                    <* 
 *>    if (s_ecurr < 0) {                                                                                                                                                          <* 
 *>       snprintf (a_out, LEN_FULL, "%c %*.*s", s_ename, a_size * 2 + 1, a_size * 2 + 1, x_dots + 27 - a_size);                                                                   <* 
 *>       return 0;                                                                                                                                                                <* 
 *>    }                                                                                                                                                                           <* 
 *>    /+---(left/past)----------------------+/                                                                                                                                    <* 
 *>    x_off  = 27 - x_size;                                                                                                                                                       <* 
 *>    x_pos  = s_macros [s_ecurr].pos;                                                                                                                                            <* 
 *>    x_keys = x_pos;                                                                                                                                                             <* 
 *>    if (x_keys > x_size)   x_keys = x_size;                                                                                                                                     <* 
 *>    x_fill = x_size - x_keys;                                                                                                                                                   <* 
 *>    sprintf (x_bef, "%c %*.*s%*.*s %c ", s_ename, x_fill, x_fill, x_dots + x_off, x_keys, x_keys, s_macros [s_ecurr].keys + x_pos - x_keys, s_macros [s_ecurr].keys + x_pos);   <* 
 *>    /+---(left/past)----------------------+/                                                                                                                                    <* 
 *>    x_off  = 30;                                                                                                                                                                <* 
 *>    x_rem = s_macros [s_ecurr].len - s_macros [s_ecurr].pos - 1;                                                                                                                <* 
 *>    if      (x_rem <=  0)  sprintf (x_aft , "%27.27s", x_rdots);                                                                                                                <* 
 *>    else if (x_rem >  27)  sprintf (x_aft , "%-26.26s>", s_macros [s_ecurr].keys + s_macros [s_ecurr].pos + 1);                                                                 <* 
 *>    else                   sprintf (x_aft , "%*.*s%*.*s"   , x_rem, x_rem, s_macros [s_ecurr].keys + x_pos + 1, 27 - x_rem, 27 - x_rem, x_rdots + x_rem);                       <* 
 *>    return 0;                                                                                                                                                                   <* 
 *> }                                                                                                                                                                              <*/

char
yvikeys_macro_estatus_OLD    (char *a_list)
{
   char        x_bef       [LEN_RECD] = "";
   char        x_aft       [LEN_RECD] = "";
   int         x_pos       = 0;
   int         x_rem       = 0;  
   char        x_dots      [LEN_HUND] = "···························";
   char        x_ldots     [LEN_HUND] = "··Ü····Ü····Ü····Ü····Ü····";
   char        x_rdots     [LEN_HUND] = "····Ü····Ü····Ü····Ü····Ü··";
   uchar       x_ch        = ' ';
   if (s_ecurr < 0) {
      snprintf (a_list, LEN_FULL, "macro   %c %c %c -- --- --- %s ´ %s", s_ename, s_ddelay, s_dupdate, x_ldots, x_rdots);
      return 0;
   }
   x_pos = s_macros [s_ecurr].pos;
   x_ch  = s_macros [s_ecurr].cur;
   if (x_ch < 32)  x_ch = '£';
   if      (x_pos <=  0)  sprintf (x_bef , "%27.27s", x_ldots);
   else if (x_pos >  27)  sprintf (x_bef , "<%-26.26s", s_macros [s_ecurr].keys + x_pos - 26);
   else                   sprintf (x_bef , "%*.*s%*.*s"   , 27 - x_pos, 27 - x_pos, x_ldots, x_pos, x_pos, s_macros [s_ecurr].keys);
   x_rem = s_macros [s_ecurr].len - s_macros [s_ecurr].pos - 1;
   if      (x_rem <=  0)  sprintf (x_aft , "%27.27s", x_rdots);
   else if (x_rem >  27)  sprintf (x_aft , "%-26.26s>", s_macros [s_ecurr].keys + s_macros [s_ecurr].pos + 1);
   else                   sprintf (x_aft , "%*.*s%*.*s"   , x_rem, x_rem, s_macros [s_ecurr].keys + x_pos + 1, 27 - x_rem, 27 - x_rem, x_rdots + x_rem);
   snprintf (a_list, LEN_FULL, "macro   %c %c %c %2d %3d %3d %27s %c %s", s_ename, s_ddelay, s_dupdate, s_macros [s_ecurr].repeat, x_pos, s_macros [s_ecurr].len, x_bef, x_ch, x_aft);
   return 0;
}

char
yvikeys_macro_estatus        (char *a_list)
{
   char        x_bef       [LEN_RECD] = "";
   char        x_aft       [LEN_RECD] = "";
   int         x_pos       = 0;
   int         x_rem       = 0;  
   char        x_dots      [LEN_HUND] = "····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····";
   int         x_len       = 0;
   uchar       x_ch        = ' ';
   int         x_wide;
   int         w;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get size)-----------------------*/
   yVIKEYS_view_size   (YVIKEYS_STATUS, NULL, &x_wide, NULL, NULL, NULL);
   DEBUG_SCRP   yLOG_value   ("x_wide"    , x_wide);
   if (myVIKEYS.env != YVIKEYS_CURSES)    x_wide /= 7.5;
   DEBUG_SCRP   yLOG_value   ("x_wide"    , x_wide);
   w = ((x_wide - 30) / 2);
   DEBUG_SCRP   yLOG_value   ("w"         , w);
   x_len = strlen (x_dots);
   p =  x_dots + x_len - w;
   /*---(idle version)-------------------*/
   if (s_ecurr < 0) {
      snprintf (a_list, LEN_FULL, "macro   %c %c %c -- --- --- %*.*s [*] %*.*s", s_ename, s_ddelay, s_dupdate, w, w, p, w, w, x_dots);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(position)-----------------------*/
   x_pos = s_macros [s_ecurr].pos;
   x_ch  = s_macros [s_ecurr].cur;
   if (x_ch < 32)  x_ch = '£';
   /*---(prefix)-------------------------*/
   if      (x_pos <=  0)  sprintf (x_bef , "%*.*s", w, w, p);
   else if (x_pos >   w)  sprintf (x_bef , "<%-*.*s", w - 1, w - 1, s_macros [s_ecurr].keys + x_pos - w);
   else                   sprintf (x_bef , "%*.*s%*.*s"   , w - x_pos, w - x_pos, p, x_pos, x_pos, s_macros [s_ecurr].keys);
   /*---(suffix)-------------------------*/
   x_rem = s_macros [s_ecurr].len - s_macros [s_ecurr].pos - 1;
   if      (x_rem <=  0)  sprintf (x_aft , "%*.*s", w, w, x_dots);
   else if (x_rem >   w)  sprintf (x_aft , "%-*.*s>", w - 1, w - 1, s_macros [s_ecurr].keys + s_macros [s_ecurr].pos + 1);
   else                   sprintf (x_aft , "%*.*s%*.*s"   , x_rem, x_rem, s_macros [s_ecurr].keys + x_pos + 1, w - x_rem, w - x_rem, x_dots + x_rem);
   /*---(concat)-------------------------*/
   snprintf (a_list, LEN_FULL, "macro   %c %c %c %2d %3d %3d %s [%c] %s", s_ename, s_ddelay, s_dupdate, s_macros [s_ecurr].repeat, x_pos, s_macros [s_ecurr].len, x_bef, x_ch, x_aft);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_rstatus        (char *a_list)
{
   char        x_dots      [LEN_HUND] = "····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····+····";
   /*> char        x_dots      [LEN_HUND] = "····´····´····´····´····´····´····´····´····´····´····´····´····´";   <*/
   int         x_len       = 0;
   int         x_wide;
   int         w;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(get size)-----------------------*/
   yVIKEYS_view_size   (YVIKEYS_STATUS, NULL, &x_wide, NULL, NULL, NULL);
   DEBUG_SCRP   yLOG_value   ("x_wide"    , x_wide);
   if (myVIKEYS.env != YVIKEYS_CURSES)    x_wide /= 7.5;
   DEBUG_SCRP   yLOG_value   ("x_wide"    , x_wide);
   w = x_wide - 15;
   DEBUG_SCRP   yLOG_value   ("w"         , w);
   DEBUG_SCRP   yLOG_value   ("s_rcurr"   , s_rcurr);
   if (s_rcurr < 0) {
      snprintf (a_list, LEN_FULL, "record  - ---  %*.*s", w, w, x_dots);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_SCRP   yLOG_info    ("s_rkeys"   , s_rkeys);
   x_len = strlen (s_rkeys) - 1;
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   if (x_len == 0) {
      snprintf (a_list, LEN_FULL, "record  %c   0  %*.*s", s_rname, w, w, x_dots);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   w -= 3;
   if (x_len == 1) {
      snprintf (a_list, LEN_FULL, "record  %c %3d  [%c] %*.*s", s_rname, x_len, s_rkeys [x_len - 1], w - x_len, w - x_len, x_dots + x_len);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   w -= 1;
   snprintf (a_list, LEN_FULL, "record  %c %3d  %*.*s [%c] %*.*s", s_rname, x_len, x_len - 1, x_len - 1, s_rkeys, s_rkeys [x_len - 1], w - x_len, w - x_len, x_dots + x_len);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_rstatus_OLD    (char *a_list)
{
   char        x_dots      [LEN_HUND] = "····´····´····´····´····´····´····´····´····´····´····´····´····´";
   int         x_len       = 0;
   if (s_rcurr < 0) {
      snprintf (a_list, LEN_FULL, "record  %c --- %s", s_rname, x_dots);
      return 0;
   }
   x_len = s_macros [s_rcurr].len;
   snprintf (a_list, LEN_FULL, "record  %c %3d %*.*s%*.*s", s_rname, x_len, x_len, x_len, s_rkeys, 65 - x_len, 65 - x_len, x_dots + x_len);
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
yvikeys_macro_writer    (int c, uchar a_abbr)
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
   yPARSE_vprintf (c, "macro", a_abbr, x_keys);
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
   /*> yPARSE_verb_begin ("macro");                                                   <*/
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      rc = yvikeys_macro_writer (c, S_MACRO_LIST [i]);
      if (rc < 1)  continue;
      ++c;
      /*> yPARSE_verb_break (c);                                                      <*/
   }
   /*---(wrap-up)------------------------*/
   /*> yPARSE_verb_end   (c);                                                         <*/
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
   char        x_keys      [LEN_RECD];
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
   if (x_keys [strlen (x_keys) - 1] != '³')   strlcat (x_keys, "³"   , LEN_RECD );
   s_macros [n].keys = strdup (x_keys);
   s_macros [n].len  = strlen (x_keys);
   if (s_saver != NULL)  s_saver (x_abbr, x_keys);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

int
yvikeys_macro_dump      (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*//*  27 + 20 + 5 = 52 */
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_end = strlen (S_MACRO_LIST);
   x_len = 80 - 52 - strlen (myVIKEYS.s_prog);
   fprintf (a_file, "##===[[ yVIKEYS macro dump from %s (:dump macros) ]]%*.*s##\n", myVIKEYS.s_prog, x_len, x_len, "======================================================================================================================================");
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      if (s_macros [i].len <= 0)  continue;
      fprintf (a_file, "%c=%s\n", S_MACRO_LIST [i], s_macros [i].keys);
      ++c;
   }
   fprintf (a_file, "##===[[ end-of-dump (%2d recs) ]]==============================================##\n", c);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}



/*====================------------------------------------====================*/
/*===----                  packaging and installing                    ----===*/
/*====================------------------------------------====================*/
static void  o___FLATTEN_________o () { return; }

static int   s_index         = 0;
static char  s_abbrs         [S_MACRO_MAX];

char
yvikeys_macro__packone  (char a_lvl, char a_src, char *a_out)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_in        =    0;
   int         i           =    0;
   int         x_pos       =    0;
   char        x_sub       [LEN_LABEL];
   char        x_msg       [LEN_DESC];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("a_lvl"     , a_lvl);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_src"     , a_src);
   x_in   = yvikeys_macro__index (a_src);
   DEBUG_SCRP   yLOG_value   ("x_in"      , x_in);
   --rce; if (x_in < 0) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_point   ("a_out"     , a_out);
   --rce; if (a_out == NULL) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_rlen = strlen (a_out);
   DEBUG_SCRP   yLOG_value   ("s_rlen"    , s_rlen);
   /*---(prefix)-------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_abbrs"   , s_abbrs [x_in]);
   if (s_abbrs [x_in] > 0) {
      DEBUG_SCRP   yLOG_note    ("repeating macro");
      sprintf (x_sub, "¸@%d¹", s_abbrs [x_in]);
      strlcat (a_out, x_sub, LEN_RECD);
      s_rlen = strlen (a_out);
      DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(prefix)-------------------------*/
   if (a_lvl > 0)  {
      DEBUG_SCRP   yLOG_note    ("prefixing");
      strlcat (a_out, "¸", LEN_RECD);
      s_abbrs [x_in] = ++s_index;
      DEBUG_SCRP   yLOG_value   ("s_abbrs"   , s_abbrs [x_in]);
      ++s_rlen;
      DEBUG_SCRP   yLOG_value   ("s_rlen"    , s_rlen);
   }
   /*---(copy)---------------------------*/
   for (i = 0; i < s_macros [x_in].len; ++i) {
      DEBUG_SCRP   yLOG_char    ("char"      , s_macros [x_in].keys [i]);
      if (s_macros [x_in].keys [i] == G_CHAR_HALT)  break;
      if (s_macros [x_in].keys [i] == '@') {
         rc = yvikeys_macro__packone (a_lvl + 1, s_macros [x_in].keys [++i], a_out);
         DEBUG_SCRP   yLOG_value   ("flatone"   , rc);
         if (rc < 0) {
            DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
            return rc;
         }
         continue;
      }
      a_out [s_rlen++]   = s_macros [x_in].keys [i];
      a_out [s_rlen]     = '\0';
      if (s_rlen < 30)  sprintf (x_msg, "[%s]", a_out);
      else              sprintf (x_msg, "<%s]", a_out + s_rlen - 30);
      DEBUG_SCRP   yLOG_complex ("added"     , "%3d, %3d, %s", s_rlen, s_rlen, x_msg);
   }
   /*---(suffix)-------------------------*/
   if (a_lvl > 0)  {
      DEBUG_SCRP   yLOG_note    ("suffixing");
      strlcat (a_out, "¹", LEN_RECD);
      ++s_rlen;
      DEBUG_SCRP   yLOG_value   ("s_rlen"    , s_rlen);
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_flatten   (char a_src, char a_dst)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        rc          =    0;
   char        x_out       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(destination)--------------------*/
   DEBUG_SCRP   yLOG_char    ("a_dst"     , a_dst);
   x_out  = yvikeys_macro__index (a_dst);
   DEBUG_SCRP   yLOG_char    ("x_out"     , x_out);
   --rce; if (x_out < 0) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yvikeys_macro__clear (a_dst);
   /*---(prepare)------------------------*/
   s_index = 0;
   for (i = 0; i < S_MACRO_MAX; ++i)  s_abbrs [i] = 0;
   /*---(call on main)-------------------*/
   rc = yvikeys_macro__packone (0, a_src, s_rkeys);
   DEBUG_SCRP   yLOG_value   ("flatone"   , rc);
   /*---(save)---------------------------*/
   strlcat (s_rkeys, "³"   , LEN_RECD );
   s_macros [x_out].keys = strdup (s_rkeys);
   s_macros [x_out].len  = strlen (s_rkeys);
   /*---(clear)--------------------------*/
   s_rkeys [0] = '\0';
   s_rlen      = 0;
   /*---(complete)-----------------------*/
   DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
   return rc;
}

int
yvikeys_macro__instone   (int a_lvl, int a_in, int a_len, int a_pos, int a_out)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_pos       =    0;
   int         x_len       =    0;
   uchar       x_sub       [LEN_LABEL];
   uchar       x_msg       [LEN_DESC];
   uchar       x_keys      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_complex ("args"      , "%2dl, %2di, %2dl, %2dp, %2do", a_lvl, a_in, a_len, a_pos, a_out);
   for (i = a_pos; i < a_len; ++i) {
      DEBUG_SCRP   yLOG_char    ("char"      , s_rkeys [i]);
      /*---(layering operator)-----------*/
      if (s_rkeys [i] == G_CHAR_SLPAREN) {
         /*---(repeat sub-macro call)----*/
         if (s_rkeys [i + 1] == '@') {
            DEBUG_SCRP   yLOG_note    ("sub-macro repeat call");
            sprintf (x_sub, "@%c", s_rkeys [i + 2]);
            strlcat (x_keys, x_sub, LEN_RECD);
            x_len = strlen (x_keys);
            i += 3;
         }
         /*---(create new sub-macro)-----*/
         else {
            DEBUG_SCRP   yLOG_note    ("sub-macro first call");
            ++s_index;
            i = yvikeys_macro__instone (a_lvl + 1, a_in, a_len, i + 1, s_index);
            sprintf (x_sub, "@%c", S_MACRO_LIST [s_index]);
            strlcat (x_keys, x_sub, LEN_RECD);
            x_len = strlen (x_keys);
         }
         /*---(done)---------------------*/
      }
      /*---(end of sub-macro)------------*/
      else if (s_rkeys [i] == G_CHAR_SRPAREN) {
         strlcat (x_keys, "³", LEN_RECD);
         x_len = strlen (x_keys);
         DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
         break;
      }
      /*---(normal)----------------------*/
      else {
         DEBUG_SCRP   yLOG_note    ("normal addition of keys");
         x_pos = x_len++;
         x_keys [x_pos] = s_rkeys [i];
         x_keys [x_pos + 1] = '\0';
         if (x_pos < 30)  sprintf (x_msg, "[%s]", x_keys);
         else             sprintf (x_msg, "<%s]", x_keys + x_pos - 30);
         DEBUG_SCRP   yLOG_complex ("added"     , "%c, %3d, %3d, %s", '0' + a_out, x_pos, x_len, x_msg);
      }
      /*---(done)------------------------*/
   }
   /*---(put in place)-------------------*/
   yvikeys_macro__clear (S_MACRO_LIST [a_out]);
   s_macros [a_out].keys = strdup (x_keys);
   s_macros [a_out].len  = x_len;
   /*---(complete)-----------------------*/
   DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
   return i;
}

char
yvikeys_macro_install   (char a_src)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        rc          =    0;
   char        x_dst       =  '0';
   char        x_in        =    0;
   char        x_out       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_src"     , a_src);
   x_in   = yvikeys_macro__index (a_src);
   DEBUG_SCRP   yLOG_value   ("x_in"      , x_in);
   --rce; if (x_in  < 0) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_rkeys, s_macros [x_in].keys, LEN_RECD);
   s_rlen = s_macros [x_in].len;
   DEBUG_SCRP   yLOG_value   ("s_rlen"    , s_rlen);
   --rce; if (s_rlen  <= 0) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(destination)--------------------*/
   DEBUG_SCRP   yLOG_char    ("x_dst"     , x_dst);
   x_out  = yvikeys_macro__index (x_dst);
   DEBUG_SCRP   yLOG_value   ("x_out"     , x_out);
   --rce; if (x_out < 0) { 
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yvikeys_macro__clear (x_dst);
   /*---(prepare)------------------------*/
   s_index = x_out;
   for (i = 0; i < S_MACRO_MAX; ++i)  s_abbrs [i] = 0;
   /*---(call on main)-------------------*/
   rc = yvikeys_macro__instone  (0, x_in, s_rlen, 0, s_index);
   DEBUG_SCRP   yLOG_value   ("instone"   , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       window manager                         ----===*/
/*====================------------------------------------====================*/
static void  o___WINDOWS_________o () { return; }

char
yvikeys_macro_term (uchar *a_string)
{
   DEBUG_SCRP   yLOG_note    (__FUNCTION__);
   DEBUG_SCRP   yLOG_info    ("a_string"  , a_string);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__unit     (char *a_question, uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   char        x_list      [LEN_RECD];
   char        t           [LEN_RECD];
   int         c           =    0;
   int         x_len       =    0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MACRO unit       : question not understood");
   /*---(simple questions)---------------*/
   if      (strcmp (a_question, "rec"            )   == 0) {
      if (s_rcurr < 0) snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO rec    (%c) : macro pointer grounded", s_rname);
      else {
         sprintf (t, "[%-.33s]", s_rkeys);
         if (s_rlen > 33)  t [34] = '>';
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO rec    (%c) : %c %2d %2d%s", s_rname, s_rmode, s_rpos, s_rlen, t);
      }
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "exec"           )   == 0) {
      if (s_ecurr < 0)  snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO exec   (%c) : macro pointer grounded", s_ename);
      else             snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO exec   (%c) : %c %c %3d %02x %3d[%-.30s]", s_ename, s_emode, s_ddelay, s_macros [s_ecurr].pos, (uchar) s_macros [s_ecurr].cur, s_macros [s_ecurr].len, s_macros [s_ecurr].keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "keys"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO keys   (%c) : %-.45s", a_abbr, s_macros [s_ecurr].keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "list"           )   == 0) {
      yvikeys_macro_list (&c, x_list);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO list       : %2d %-.45s", c, x_list);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "speed"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO speed    %c : %8.6fd %5.3fu %2d/%2ds, deb %c/%c, exe %c/%c, %2dp", s_blitzing, myVIKEYS.delay, myVIKEYS.update, s_skips, myVIKEYS.macro_skip, s_ddelay, s_dupdate, s_edelay, s_eupdate, s_pause);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "clip"           )   == 0) {
      yvikeys_dump_read (a_abbr, t, &x_len);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO clip  (%2d) : %2d[%.40s]", a_abbr, x_len, t);
      return yVIKEYS__unit_answer;
   }
   /*---(complex questions)--------------*/
   n = yvikeys_macro__index (a_abbr);
   if (yvikeys_macro__index (a_abbr) < 0) {
      strcpy  (yVIKEYS__unit_answer, "MACRO unit       : not a valid macro name");
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      if (n < 0)  snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO saved  (%c) : not a valid macro name", a_abbr);
      else {
         sprintf (t, "[%-.33s]", s_macros [n].keys);
         if (s_macros [n].len > 33)  t [34] = '>';
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO saved  (%c) : %2d%-35.35s %2d %2d %2d", a_abbr, s_macros [n].len, t, s_macros [n].pos, s_macros [n].runby, s_macros [n].repeat);
      }
   }
   else if (strcmp (a_question, "full"           )   == 0) {
      if (n < 0)  snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO full   (%c) : not a valid macro name", a_abbr);
      else        snprintf (yVIKEYS__unit_answer, LEN_RECD, "MACRO full   (%c) : %s", a_abbr, s_macros [n].keys);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



/*============================----end-of-source---============================*/
