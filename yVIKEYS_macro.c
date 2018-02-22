/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



#define     LEN_MACRO        2000



/*---(keyboard macro)-------*/
static char  s_macro_mode   =  '-';         /* keyboard, macro, silent        */
static char  s_macro_name   =  '-';         /* current macro name             */
static int   s_macro_pos    =   -1;         /* step in current macro          */
static int   s_macro_len    =    0;         /* length of macro                */
static char  s_macro_keys  [LEN_MACRO];     /* macro contents                 */
static char  s_macro_char   =  ' ';         /* interpreted current char       */
static char  s_macro_delay  =  '0';         /* time between steps             */
static int   s_macro_repeat =    0;         /* repeat counter                 */



typedef     struct        cMACRO    tMACRO;
struct cMACRO {
   char        keys        [LEN_MACRO];     /* keystrokes                     */
   char        active;                      /* run by who                     */
   char        pos;                         /* current position               */
   char        len;                         /* number of keys                 */
   char        cur;                         /* current key                    */
   char        repeat;                      /* number of repeats              */
};
static tMACRO  s_macros    [26];

static char    (*s_loader) (char a_name, char *a_keys);
static char    (*s_saver ) (char a_name, char *a_keys);



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize macro environment -------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
MACRO_init              (void)
{
   MACRO__purge ();
   s_macro_name     = '-';
   s_loader = NULL;
   s_saver  = NULL;
   yVIKEYS_cmds_add ('c', "macro"       , ""    , "a"    , MACRO_define         , "direct definition of a keyboard macro"                       );
   return 0;
}

char         /*-> initialize macro environment -------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_macro_config    (void *a_loader, void *a_saver)
{
   if (a_loader != NULL) s_loader = a_loader;
   if (a_saver  != NULL) s_saver  = a_saver;
   return 0;
}

char
MACRO_zero              (void)
{
   s_macro_repeat = 0;
}

char
MACRO_count        (void)
{
   if (s_macro_repeat < 1) return 0;
   else return 1;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
MACRO__clear            (char a_macro)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        x_index     =   -1;
   /*---(check)--------------------------*/
   x_index = MACRO__index (a_macro);
   if (x_index < 0)  return x_index;
   /*---(null keys)----------------------*/
   for (i = 0; i < LEN_MACRO; ++i)  s_macros [x_index]. keys [i] = G_KEY_NULL;
   /*---(reset)--------------------------*/
   s_macros [x_index].active    =  '-';
   s_macros [x_index].pos       =   -1;
   s_macros [x_index].len       =    0;
   s_macros [x_index].cur       =  ' ';
   s_macros [x_index].repeat    =    0;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
MACRO__purge            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        i           = 0;
   /*---(turn off)-----------------------*/
   SET_MACRO_OFF;
   /*---(clear saved)--------------------*/
   for (i = 'a'; i <= 'z'; ++i)  MACRO__clear (i);
   /*---(clear current)------------------*/
   MACRO_reset ();
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

char         /*-> check macro name for correctness ---[ leaf   [ge.320.114.30]*/ /*-[00.0000.053.C]-*/ /*-[--.---.---.--]-*/
MACRO__name             (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (a_name == '@')                    return 1;
   --rce;  if (a_name >= 'a' && a_name <= 'z')   return 0;
   --rce;  if (a_name >= 'A' && a_name <= 'Z')   return 2;
   /*---(complete)-----------------------*/
   return rce;
}

char         /*-> return index for macro name --------[ ------ [gc.A43.108.12]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
MACRO__index            (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   if (a_name >= 'a' && a_name <= 'z')   return a_name - 'a';
   if (a_name >= 'A' && a_name <= 'Z')   return a_name - 'A';
   /*---(complete)-----------------------*/
   return rce;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
MACRO_reset             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(mode)---------------------------*/
   DEBUG_SCRP   yLOG_snote   ("macro_off");
   SET_MACRO_OFF;
   /*---(macro values)-------------------*/
   DEBUG_SCRP   yLOG_snote   ("reset values");
   s_macro_pos    =  -1;
   s_macro_len    =   0;
   s_macro_delay  = '0';
   MACRO_zero ();
   /*---(macro keystrokes)---------------*/
   DEBUG_SCRP   yLOG_snote   ("null macro keys");
   for (i = 0; i < LEN_MACRO; ++i)  s_macro_keys [i] = G_KEY_NULL;
   s_macro_char   =   0;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       saving and fetching                    ----===*/
/*====================------------------------------------====================*/
static void  o___SAVING__________o () { return; }

char         /*-> save current recording -------------[ ------ [ge.631.052.22]*/ /*-[01.0000.014.4]-*/ /*-[--.---.---.--]-*/
MACRO__save             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   int         x_len       =    0;
   char        x_keys      [LEN_MACRO]  = "";
   char        x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(trim)---------------------------*/
   strlcpy (x_keys, s_macro_keys, LEN_MACRO);
   x_len = strlen (x_keys);
   if (x_len > 0)  x_keys [--x_len] = G_KEY_NULL;
   /*---(save yVIKEYS)-------------------*/
   MACRO__clear (s_macro_name);
   x_index = MACRO__index (s_macro_name);
   DEBUG_SCRP   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_macros [x_index].keys, x_keys, LEN_MACRO);
   s_macros [x_index].len = x_len;
   /*---(save gyges)---------------------*/
   if (s_saver != NULL)  s_saver (s_macro_name, x_keys);
   /*> rc = CELL_macro_set (x_keys);                                                 <* 
    *> DEBUG_SCRP   yLOG_value   ("rc"        , rc);                                  <* 
    *> --rce;  if (rc    <  0) {                                                      <* 
    *>    DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(clear)--------------------------*/
   MACRO_reset ();
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> fetch specific macro keystrokes ----[ ------ [ge.A52.134.63]*/ /*-[01.0000.023.#]-*/ /*-[--.---.---.--]-*/
MACRO__fetch            (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   char        x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   if (s_macro_name != a_name) MACRO_reset ();
   IF_MACRO_RECORDING          MACRO_reset ();
   s_macro_name  = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO__name (a_name) != 0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_macro_name = a_name;
   /*---(prefetch)--------------------*/
   if (s_loader != NULL) {
      MACRO__clear (s_macro_name);
      s_loader (s_macro_name, s_macro_keys);
      x_index = MACRO__index (s_macro_name);
      strlcpy (s_macros [x_index].keys, s_macro_keys, LEN_MACRO);
      s_macros [x_index].len = strllen (s_macro_keys, LEN_MACRO);
   }
   /*---(get contents)----------------*/
   x_index = MACRO__index (s_macro_name);
   DEBUG_SCRP   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy (s_macro_keys, s_macros [x_index].keys, LEN_MACRO);
   s_macro_len = s_macros [x_index].len;
   /*---(get contents gyges)----------*/
   /*> rc = CELL_macro_get (s_macro_keys);                                            <* 
    *> DEBUG_SCRP   yLOG_value   ("rc"        , rc);                                  <* 
    *> --rce;  if (rc    <  0) {                                                      <* 
    *>    MACRO_reset ();                                                     <* 
    *>    DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(check contents)--------------*/
   DEBUG_SCRP   yLOG_info    ("macro"     , s_macro_keys);
   s_macro_len = strlen (s_macro_keys);
   DEBUG_SCRP   yLOG_value   ("len"       , s_macro_len);
   --rce;  if (s_macro_len <= 0) {
      MACRO_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set globals)-----------------*/
   s_macro_keys [s_macro_len++] = G_CHAR_NULL;
   s_macro_keys [s_macro_len  ] = G_KEY_NULL;
   if (s_macro_pos < 0)  s_macro_char = 0;
   else                   s_macro_char = s_macro_keys [s_macro_pos];
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro recording                      ----===*/
/*====================------------------------------------====================*/
static void  o___RECORD__________o () { return; }

char         /*-> prepare a macro recording ----------[ ------ [ge.831.112.53]*/ /*-[01.0000.023.A]-*/ /*-[--.---.---.--]-*/
MACRO__rec_beg          (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   MACRO_reset ();
   s_macro_name = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO__name (a_name) <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert name)----------------*/
   if (a_name != '@')  s_macro_name = tolower (a_name);
   /*---(handle append)---------------*/
   if (tolower (a_name) != a_name) {
      MACRO__fetch (s_macro_name);
      s_macro_keys [--s_macro_len] = G_KEY_NULL;
      s_macro_pos      = s_macro_len - 1;
      if (s_macro_pos < 0)  s_macro_char = 0;
      else                  s_macro_char = s_macro_keys [s_macro_pos];
   }
   /*---(add placeholder)-------------*/
   s_macro_keys [s_macro_len++] = G_CHAR_PLACE;
   s_macro_keys [s_macro_len  ] = G_KEY_NULL;
   /*---(turn on record)--------------*/
   SET_MACRO_RECORD;
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> add a key to a macro ---------------[ leaf   [gc.440.102.10]*/ /*-[01.0000.103.9]-*/ /*-[--.---.---.--]-*/
MACRO_rec_key           (char a_key)
{
   IF_MACRO_RECORDING {
      switch (a_key) {
      case G_KEY_RETURN  :  a_key  = G_CHAR_RETURN;  break;  /* return char           */
      case G_KEY_ESCAPE  :  a_key  = G_CHAR_ESCAPE;  break;  /* escape char           */
      case G_KEY_TAB     :  a_key  = G_CHAR_TAB;     break;  /* tab char              */
      case G_KEY_BS      :  a_key  = G_CHAR_BS;      break;  /* backspace char        */
      case G_KEY_SPACE   :  a_key  = G_CHAR_SPACE;   break;  /* visual space          */
      }
      s_macro_char                    = a_key;
      s_macro_keys [s_macro_len - 1] = a_key;
      s_macro_keys [s_macro_len++  ] = G_CHAR_PLACE;
      s_macro_keys [s_macro_len    ] = G_KEY_NULL;
      s_macro_pos                   = s_macro_len - 2;
   }
   return 0;
}

char         /*-> put keys in globals ----------------[ leaf   [gz.530.101.60]*/ /*-[01.0000.01#.5]-*/ /*-[--.---.---.--]-*/
MACRO__rec_str          (char *a_keys)
{
   int         i           = 0;
   char        x_ch        = '-';
   /*---(look for suffix)----------------*/
   if (s_macro_keys [s_macro_len - 1] == (schar) G_CHAR_PLACE) {
      s_macro_keys [--s_macro_len  ] =  G_KEY_NULL;
   }
   if (s_macro_keys [s_macro_len - 1] == (schar) G_CHAR_NULL ) {
      s_macro_keys [--s_macro_len  ] =  G_KEY_NULL;
   }
   /*---(add keys)-----------------------*/
   if (a_keys == NULL)  strlcat (s_macro_keys, ""    , LEN_MACRO);
   else                 strlcat (s_macro_keys, a_keys, LEN_MACRO);
   s_macro_len  = strlen (s_macro_keys);
   /*---(fix keys)-----------------------*/
   strlencode   (s_macro_keys, ySTR_MAX, LEN_MACRO);
   /*---(add suffix)---------------------*/
   s_macro_keys [s_macro_len  ] =  G_CHAR_PLACE;
   s_macro_keys [++s_macro_len] =  G_KEY_NULL;
   /*---(update pos/char)----------------*/
   s_macro_pos = s_macro_len - 2;
   if (s_macro_pos < 0)  s_macro_char = 0;
   else                  s_macro_char = s_macro_keys [s_macro_pos];
   /*> printf ("s_macro_keys <<%s>>\n", s_macro_keys);                                <*/
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> complete a macro recording ---------[ ------ [gz.420.001.21]*/ /*-[02.0000.113.5]-*/ /*-[--.---.---.--]-*/
MACRO_rec_end           (void)
{
   IF_MACRO_RECORDING {
      if (s_macro_len >= 2 && s_macro_keys [s_macro_len - 2] == 'q') {
         s_macro_keys [s_macro_len - 2] = G_CHAR_NULL;
         s_macro_keys [s_macro_len - 1] = G_KEY_NULL;
         --s_macro_len;
      } else if (s_macro_len >= 1) {
         s_macro_keys [s_macro_len - 1] = G_CHAR_NULL;
      }
      s_macro_pos  = 0;
      s_macro_char = s_macro_keys [s_macro_pos];
      MACRO__save ();
      SET_MACRO_OFF;
   }
   return 0;
}

char         /*-> enter a macro directly -------------[ ------ [ge.850.137.A4]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
MACRO_define            (char *a_string)
{
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   --rce;  if (a_string == NULL)          return rce;
   --rce;  if (strlen (a_string) <  3)     return rce;
   --rce;  if (a_string [1]      != '=')   return rce;
   rc = MACRO__rec_beg (a_string [0]);
   if (rc < 0) {
      MACRO_reset ();
      return rc;
   }
   if (a_string [2] != G_KEY_DQUOTE) {
      rc = MACRO__rec_str (a_string + 2);
   } else {
      x_len = strlen (a_string);
      if (a_string [x_len - 1] != G_KEY_DQUOTE) {
         rc = MACRO__rec_str (a_string + 2);
      } else {
         a_string [--x_len] = G_KEY_NULL;
         rc = MACRO__rec_str (a_string + 3);
      }
   }
   if (rc < 0) {
      MACRO_reset ();
      return rc;
   }
   rc = MACRO_rec_end ();
   if (rc < 0) {
      MACRO_reset ();
      return rc;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         macro execution                      ----===*/
/*====================------------------------------------====================*/
static void  o___EXECUTE_________o () { return; }

char         /*-> set or adjust delay value ----------[ leaf   [gz.612.101.50]*/ /*-[01.0000.023.!]-*/ /*-[--.---.---.--]-*/
MACRO__delay            (char a_delay)
{
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(set)----------------------------*/
   if (a_delay >= '0' && a_delay <= '9') {
      DEBUG_SCRP   yLOG_note    ("set abs");
      s_macro_delay = a_delay;
   }
   if (a_delay == '-') {
      DEBUG_SCRP   yLOG_note    ("decrease (-)");
      if (s_macro_delay > '0')  --s_macro_delay;
   }
   if (a_delay == '+') {
      DEBUG_SCRP   yLOG_note    ("increase (+)");
      if (s_macro_delay < '9')  ++s_macro_delay;
   }
   /*---(new value)-------------------*/
   DEBUG_SCRP   yLOG_schar   (a_delay);
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare a macro execution ----------[ ------ [ge.832.122.52]*/ /*-[01.0000.112.5]-*/ /*-[--.---.---.--]-*/
MACRO_exec_beg          (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (MACRO__name (a_name) <  0)  {
      s_macro_name = '-';
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(normal)----------------------*/
   if (a_name == tolower (a_name)) {
      DEBUG_SCRP   yLOG_note    ("normal execution");
      if (s_macro_delay    != '0'       )  SET_MACRO_DELAY;
      IF_MACRO_OFF                          SET_MACRO_RUN;
      if (a_name != '@')  s_macro_name = a_name;
   }
   /*---(debugging/playback)----------*/
   else {
      DEBUG_SCRP   yLOG_note    ("debug execution");
      SET_MACRO_PLAYBACK;
      s_macro_name     = tolower (a_name);
   }
   /*---(macro name)------------------*/
   DEBUG_SCRP   yLOG_char    ("macro_name", s_macro_name);
   /*---(get macro)-------------------*/
   MACRO__fetch (s_macro_name);
   /*---(update repeat)---------------*/
   if (s_macro_repeat > 0)  --s_macro_repeat;
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.220.002.20]*/ /*-[00.0000.102.7]-*/ /*-[--.---.---.--]-*/
MACRO_exec_adv          (void)
{
   IF_MACRO_OFF   return 0;
   ++s_macro_pos;
   if (s_macro_pos < 0)  s_macro_char = 0;
   else                  s_macro_char = s_macro_keys [s_macro_pos];
   return 0;
}

char         /*-> return current key in macro --------[ ------ [ge.A63.034.52]*/ /*-[02.0000.102.8]-*/ /*-[--.---.---.--]-*/
MACRO_exec_key          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   /*---(header)-------------------------*/
   IF_MACRO_OFF   return 0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_value   ("macro_pos" , s_macro_pos);
   --rce;  if (s_macro_pos >= s_macro_len) {
      MACRO_reset ();
      DEBUG_SCRP   yLOG_note    ("past the end");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = s_macro_keys [s_macro_pos];
   DEBUG_SCRP   yLOG_char    ("x_ch"      , x_ch);
   if (x_ch <  0) {
      DEBUG_SCRP   yLOG_note    ("special character");
      DEBUG_SCRP   yLOG_value   ("256 + x_ch", 256 + x_ch);
      /*---(translate special)-----------*/
      switch (256 + x_ch) {
      case G_CHAR_RETURN  :  x_ch = G_KEY_RETURN;  break;
      case G_CHAR_ESCAPE  :  x_ch = G_KEY_ESCAPE;  break;
      case G_CHAR_BS      :  x_ch = G_KEY_BS;      break;
      case G_CHAR_TAB     :  x_ch = G_KEY_TAB;     break;
      case G_CHAR_SPACE   :  x_ch = G_KEY_SPACE;   break;
      case G_CHAR_GROUP   :  x_ch = G_KEY_GROUP;   break;
      case G_CHAR_FIELD   :  x_ch = G_KEY_FIELD;   break;
      case G_CHAR_ALT     :  x_ch = G_KEY_SPACE;   break;
      case G_CHAR_CONTROL :  x_ch = G_KEY_SPACE;   break;
      }
      DEBUG_SCRP   yLOG_value   ("x_ch (new)", x_ch);
      /*---(handle controls)-------------*/
      if (x_ch < 0) {
         x_ch = MACRO__exec_control (x_ch);
      }
   }
   if (x_ch == NULL) {
      DEBUG_SCRP   yLOG_note    ("end of macro");
      MACRO_reset ();
   }
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char         /*-> tbd --------------------------------[ ------ [gc.340.102.21]*/ /*-[00.0000.013.!]-*/ /*-[--.---.---.--]-*/
MACRO__exec_control     (char a_key)
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

char         /*-> tbd --------------------------------[ leaf   [gc.581.012.10]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
MACRO_exec_wait         (void)
{
   struct timespec x_delay;
   IF_MACRO_OFF   return 0;
   IF_MACRO_DELAY {
      DEBUG_SCRP   yLOG_note    ("process a delay");
      switch (s_macro_delay) {
      case '9' : x_delay.tv_sec = 3; x_delay.tv_nsec =         0; break;
      case '8' : x_delay.tv_sec = 2; x_delay.tv_nsec =         0; break;
      case '7' : x_delay.tv_sec = 1; x_delay.tv_nsec = 750000000; break;
      case '6' : x_delay.tv_sec = 1; x_delay.tv_nsec = 500000000; break;
      case '5' : x_delay.tv_sec = 1; x_delay.tv_nsec = 250000000; break;
      case '4' : x_delay.tv_sec = 1; x_delay.tv_nsec =         0; break;
      case '3' : x_delay.tv_sec = 0; x_delay.tv_nsec = 750000000; break;
      case '2' : x_delay.tv_sec = 0; x_delay.tv_nsec = 500000000; break;
      case '1' : x_delay.tv_sec = 0; x_delay.tv_nsec = 250000000; break;
      case '0' : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
      default  : x_delay.tv_sec = 0; x_delay.tv_nsec =         0; break;
      }
      nanosleep (&x_delay, NULL);
   }
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gc.A43.108.12]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
MACRO_exec_player       (char a_key)
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
         return -1;
      }
      IF_MACRO_DELAY {
         DEBUG_SCRP   yLOG_note    ("change delay to playback");
         SET_MACRO_PLAYBACK;
         DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
         return -1;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case '-'      : case '+'      :
      MACRO__delay (a_key);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_ESCAPE :
      DEBUG_SCRP   yLOG_note    ("escape");
      MACRO_reset ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_RETURN :
      DEBUG_SCRP   yLOG_note    ("return");
      SET_MACRO_RUN;
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return  0;
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
MACRO_smode             (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_majors    [LEN_MACRO]  = "q@";
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
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, nothing to do");
      MODE_exit  ();
      MACRO_reset       ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for recording)------------*/
   --rce;  if (a_major == 'q') {
      DEBUG_USER   yLOG_note    ("macro recording");
      /*---(check macro name)------------*/
      if (MACRO__name (a_minor) < 0)  {
         MODE_exit  ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(start recording)-------------*/
      MODE_exit  ();
      MACRO__rec_beg (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for execution)------------*/
   --rce;  if (a_major == '@') {
      DEBUG_USER   yLOG_note    ("macro execution");
      /*---(playback speed)--------------*/
      if (a_minor >= '0' && a_minor <= '9') {
         MACRO__delay (a_minor);
         DEBUG_USER   yLOG_char    ("delay"     , a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_major;
      }
      /*---(check macro name)------------*/
      if (MACRO__name (a_minor) < 0)  {
         MODE_exit  ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(execute)---------------------*/
      MODE_exit  ();
      s_macro_repeat = REPEAT_count ();
      DEBUG_USER   yLOG_value   ("repeat_m"  , s_macro_repeat);
      MACRO_exec_beg  (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
      /*---(done)------------------------*/
   }
   /*---(errors fall through)------------*/
   --rce;
   MACRO_reset ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char MACRO_get_mode          (void)         { return s_macro_mode; }
char MACRO_set_mode          (char a_mode) { s_macro_mode = a_mode; return 0; }



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
MACRO__unit             (char *a_question, char a_macro)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_index     =    0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MACRO unit       : question not understood");
   /*---(defenses)-----------------------*/
   a_macro = tolower (a_macro);
   if (MACRO__name (a_macro) < 0) {
      strcpy  (yVIKEYS__unit_answer, "MACRO unit       : not a valid macro name");
      return yVIKEYS__unit_answer;
   }
   x_index = MACRO__index (a_macro);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "curr"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO curr   (%c) : %c %2d %2d[%-.40s]", s_macro_name, s_macro_mode, s_macro_pos, s_macro_len, s_macro_keys);
   }
   else if (strcmp (a_question, "exec"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO exec   (%c) : %c %c %3d %02x %3d:%-.30s", s_macro_name, s_macro_mode, s_macro_delay, s_macro_pos, (uchar) s_macro_char, s_macro_len, s_macro_keys);
   }
   else if (strcmp (a_question, "keys"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO keys   (%c) : %-.45s", a_macro, s_macro_keys);
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      if (x_index < 0)  snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO saved  (%c) : not a valid macro name", a_macro);
      else              snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO saved  (%c) : %c %2d %2d[%-.40s]", a_macro, s_macros [x_index].active, s_macros [x_index].pos, s_macros [x_index].len, s_macros [x_index].keys);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



/*============================----end-of-source---============================*/
