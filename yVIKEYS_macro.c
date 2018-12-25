/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char s_macro_status   = G_STAGE_NULL;


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




#define      S_MACRO_MAX      150
static char  S_MACRO_LIST   [S_MACRO_MAX];



typedef     struct        cMACRO    tMACRO;
struct cMACRO {
   /*---(base)--------------*/
   char        keys        [LEN_MACRO];     /* keystrokes                     */
   char        len;                         /* number of keys                 */
   /*---(exec)--------------*/
   char        active;                      /* run by who                     */
   char        pos;                         /* current position               */
   char        cur;                         /* current key                    */
   char        repeat;                      /* number of repeats              */
   /*---(usage)-------------*/
   int         count;
   char        rc;
   /*---(done)--------------*/
};
static tMACRO  s_macros    [S_MACRO_MAX];
static int     s_nmacro    =    0;



static char    (*s_loader) (char a_name, char *a_keys);
static char    (*s_saver ) (char a_name, char *a_keys);

#define     MACRO_LOWER    'a'
#define     MACRO_UPPER    'A'
#define     MACRO_SYSTEM   '0'
#define     MACRO_ALL      '*'



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize macro environment -------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_init      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(macro abbrev list)--------------*/
   strlcpy (S_MACRO_LIST, gvikeys_lower , S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_upper , S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_number, S_MACRO_MAX);
   strlcat (S_MACRO_LIST, gvikeys_greek , S_MACRO_MAX);
   s_nmacro = strlen (S_MACRO_LIST);
   /*---(clear)--------------------------*/
   s_macro_name     = '-';
   s_loader = NULL;
   s_saver  = NULL;
   yvikeys_macro__purge (MACRO_ALL);
   /*---(update stage)-------------------*/
   DEBUG_PROG   yLOG_note    ("update status");
   s_macro_status = G_STAGE_READY;
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("add commands");
   yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "macro"       , ""    , "a"    , yvikeys_macro__direct, "direct definition of a keyboard macro"                       );
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
   DEBUG_SCRP   yLOG_value   ("stage"     , s_macro_status);
   --rce;  if (s_macro_status <  G_STAGE_INIT) {
      DEBUG_SCRP   yLOG_note    ("must be called after init");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, -66);
      return -66;
   }
   /*---(save)---------------------------*/
   if (a_loader != NULL) s_loader = a_loader;
   DEBUG_SCRP   yLOG_point   ("loader"    , s_loader);
   if (a_saver  != NULL) s_saver  = a_saver;
   DEBUG_SCRP   yLOG_point   ("saver"     , s_saver);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_macro_zero      (void)
{
   s_macro_repeat = 0;
}

char
yvikeys_macro_count     (void)
{
   if (s_macro_repeat < 1) return 0;
   else return 1;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__clear    (char a_macro)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   char        x_index     =   -1;
   /*---(check)--------------------------*/
   x_index = yvikeys_macro__index (a_macro);
   if (x_index < 0)  return x_index;
   /*---(null keys)----------------------*/
   for (i = 0; i < LEN_MACRO; ++i)  s_macros [x_index]. keys [i] = G_KEY_NULL;
   /*---(reset)--------------------------*/
   s_macros [x_index].active    =  '-';
   s_macros [x_index].pos       =   -1;
   s_macros [x_index].len       =    0;
   s_macros [x_index].cur       =  ' ';
   s_macros [x_index].repeat    =    0;
   s_macros [x_index].count     =    0;
   s_macros [x_index].rc        =    0;
   /*---(complete)-----------------------*/
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
   SET_MACRO_OFF;
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  :  break;
   case YVIKEYS_UPPER  :  break;
   case YVIKEYS_NUMBER :  break;
   case YVIKEYS_GREEK  :  break;
   case YVIKEYS_FULL   :  break;
   default           :
                          DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
                          return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_SCRP   yLOG_value   ("s_nmacro"  , s_nmacro);
   for (i = 0; i < s_nmacro; ++i) {
      x_abbr = S_MACRO_LIST [i];
      if (a_scope == YVIKEYS_UPPER  && strchr (gvikeys_upper , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_macro__clear (x_abbr);
   }
   /*---(clear current)------------------*/
   yvikeys_macro_reset ();
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

char 
yvikeys_macro__valid    (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
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
yvikeys_macro__index    (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   n  = strlchr (S_MACRO_LIST, a_abbr, S_MACRO_MAX);
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n  < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}

char         /*-> fully reset macro mode -------------[ leaf   [gz.532.011.00]*/ /*-[00.0000.183.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_reset     (void)
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
yvikeys_macro__save     (void)
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
   yvikeys_macro__clear (s_macro_name);
   x_index = yvikeys_macro__index (s_macro_name);
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
   yvikeys_macro_reset ();
   yvikeys_macro_zero  ();
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> fetch specific macro keystrokes ----[ ------ [ge.A52.134.63]*/ /*-[01.0000.023.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__fetch    (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_ch        =  ' ';
   char        x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   if (s_macro_name != a_name) yvikeys_macro_reset ();
   IF_MACRO_RECORDING          yvikeys_macro_reset ();
   s_macro_name  = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (yvikeys_macro__valid (a_name) != 0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_macro_name = a_name;
   /*---(prefetch)--------------------*/
   if (s_loader != NULL) {
      yvikeys_macro__clear (s_macro_name);
      s_loader (s_macro_name, s_macro_keys);
      x_index = yvikeys_macro__index (s_macro_name);
      strlcpy (s_macros [x_index].keys, s_macro_keys, LEN_MACRO);
      s_macros [x_index].len = strllen (s_macro_keys, LEN_MACRO);
   }
   /*---(get contents)----------------*/
   x_index = yvikeys_macro__index (s_macro_name);
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
    *>    yvikeys_macro_reset ();                                                     <* 
    *>    DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(check contents)--------------*/
   DEBUG_SCRP   yLOG_info    ("macro"     , s_macro_keys);
   s_macro_len = strlen (s_macro_keys);
   DEBUG_SCRP   yLOG_value   ("len"       , s_macro_len);
   --rce;  if (s_macro_len <= 0) {
      yvikeys_macro_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set globals)-----------------*/
   s_macro_keys [s_macro_len++] = G_CHAR_NULL;
   s_macro_keys [s_macro_len  ] = G_KEY_NULL;
   if (s_macro_pos < 0)  s_macro_char = 0;
   else                  s_macro_char = s_macro_keys [s_macro_pos];
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
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)---------------------*/
   yvikeys_macro_reset ();
   s_macro_name = '-';
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (yvikeys_macro__valid (a_name) <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert name)----------------*/
   s_macro_name = a_name;
   yvikeys_macro__clear (a_name);
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
      s_macro_char                    = a_key;
      s_macro_keys [s_macro_len - 1] = a_key;
      s_macro_keys [s_macro_len++  ] = G_CHAR_PLACE;
      s_macro_keys [s_macro_len    ] = G_KEY_NULL;
      s_macro_pos                   = s_macro_len - 2;
   }
   return 0;
}

char         /*-> put keys in globals ----------------[ leaf   [gz.530.101.60]*/ /*-[01.0000.01#.5]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__recstr   (char *a_keys)
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
yvikeys_macro_recend    (void)
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
      yvikeys_macro__save ();
      SET_MACRO_OFF;
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
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_point   ("a_string"  , a_string);
   /*---(defense)------------------------*/
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
      switch (a_string [0]) {
      case '*'  : yvikeys_macro__purge (YVIKEYS_FULL  ); break;
      case 'a'  : yvikeys_macro__purge (YVIKEYS_LOWER ); break;
      case 'A'  : yvikeys_macro__purge (YVIKEYS_UPPER ); break;
      case '0'  : yvikeys_macro__purge (YVIKEYS_NUMBER); break;
      case 'è'  : yvikeys_macro__purge (YVIKEYS_GREEK ); break;
      default   :
                  DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
                  return rce;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for clear)----------------*/
   --rce;  if (x_len == 2) {
      DEBUG_SCRP   yLOG_char    ("equal"     , a_string [1]);
      if (a_string [1] != '=') {
         DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = yvikeys_macro__clear (a_string [0]);
      DEBUG_SCRP   yLOG_value   ("clear"     , rc);
      if (rc < 0) {
         DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for define)---------------*/
   DEBUG_SCRP   yLOG_char    ("equal"     , a_string [1]);
   --rce;  if (a_string [1] != '=') {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(begin recording)----------------*/
   rc = yvikeys_macro__recbeg (a_string [0]);
   DEBUG_SCRP   yLOG_char    ("recbeg"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add keys)-----------------------*/
   if (a_string [2] != G_KEY_DQUOTE) {
      DEBUG_SCRP   yLOG_note    ("normal/unquoted macro format");
      rc = yvikeys_macro__recstr (a_string + 2);
   } else {
      DEBUG_SCRP   yLOG_note    ("quoted macro format");
      x_len = strlen (a_string);
      if (a_string [x_len - 1] != G_KEY_DQUOTE) {
         rc = yvikeys_macro__recstr (a_string + 2);
      } else {
         a_string [--x_len] = G_KEY_NULL;
         rc = yvikeys_macro__recstr (a_string + 3);
      }
   }
   DEBUG_SCRP   yLOG_char    ("recstr"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro_reset ();
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(end recording)------------------*/
   rc = yvikeys_macro_recend ();
   DEBUG_SCRP   yLOG_char    ("recend"    , rc);
   --rce;  if (rc < 0) {
      yvikeys_macro_reset ();
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
yvikeys_macro_exebeg    (char a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   /*---(check macro name)------------*/
   DEBUG_SCRP   yLOG_char    ("a_name"    , a_name);
   if (yvikeys_macro__valid (a_name) <  0)  {
      s_macro_name = '-';
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(normal)----------------------*/
   /*> if (a_name == tolower (a_name)) {                                              <*/
   DEBUG_SCRP   yLOG_note    ("normal execution");
   if (s_macro_delay    != '0'       )  SET_MACRO_DELAY;
   IF_MACRO_OFF                         SET_MACRO_RUN;
   if (a_name != '@')  s_macro_name = a_name;
   /*> }                                                                              <*/
   /*---(debugging/playback)----------*/
   /*> else {                                                                         <* 
    *>    DEBUG_SCRP   yLOG_note    ("debug execution");                              <* 
    *>    SET_MACRO_PLAYBACK;                                                         <* 
    *>    s_macro_name     = tolower (a_name);                                        <* 
    *> }                                                                              <*/
   /*---(macro name)------------------*/
   DEBUG_SCRP   yLOG_char    ("macro_name", s_macro_name);
   /*---(get macro)-------------------*/
   yvikeys_macro__fetch (s_macro_name);
   /*---(update repeat)---------------*/
   if (s_macro_repeat > 0)  --s_macro_repeat;
   /*---(update count)----------------*/
   ++s_macros [yvikeys_macro__index (s_macro_name)].count;
   /*---(complete)--------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gc.220.002.20]*/ /*-[00.0000.102.7]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exeadv    (void)
{
   IF_MACRO_OFF   return 0;
   ++s_macro_pos;
   if (s_macro_pos < 0)  s_macro_char = 0;
   else                  s_macro_char = s_macro_keys [s_macro_pos];
   return 0;
}

char         /*-> return current key in macro --------[ ------ [ge.A63.034.52]*/ /*-[02.0000.102.8]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exekey    (void)
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
      yvikeys_macro_reset ();
      DEBUG_SCRP   yLOG_note    ("past the end");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = s_macro_keys [s_macro_pos];
   DEBUG_SCRP   yLOG_value   ("x_ch"      , x_ch);
   x_ch = chrworking (x_ch);
   DEBUG_SCRP   yLOG_value   ("x_ch"      , x_ch);
   /*---(handle controls)-------------*/
   if (x_ch < 0) {
      x_ch = yvikeys_macro__exectl (x_ch);
   }
   if (x_ch == NULL) {
      DEBUG_SCRP   yLOG_note    ("end of macro");
      yvikeys_macro_reset ();
   }
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

char         /*-> tbd --------------------------------[ leaf   [gc.581.012.10]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_macro_exewait   (void)
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
yvikeys_macro_exeplay   (char a_key)
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
      yvikeys_macro__delay (a_key);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_ESCAPE :
      DEBUG_SCRP   yLOG_note    ("escape");
      yvikeys_macro_reset ();
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return -1;
      break;
   case G_KEY_RETURN : case G_KEY_ENTER  :
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
yvikeys_macro_smode     (char a_major, char a_minor)
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
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN || a_minor == G_KEY_ENTER ) {
      DEBUG_USER   yLOG_note    ("escape/return, nothing to do");
      MODE_exit  ();
      yvikeys_macro_reset       ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for recording)------------*/
   --rce;  if (a_major == 'q') {
      DEBUG_USER   yLOG_note    ("macro recording");
      /*---(check macro name)------------*/
      if (yvikeys_macro__valid (a_minor) < 0)  {
         MODE_exit  ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(start recording)-------------*/
      MODE_exit  ();
      yvikeys_macro__recbeg (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for execution)------------*/
   --rce;  if (a_major == '@') {
      DEBUG_USER   yLOG_note    ("macro execution");
      /*---(playback speed)--------------*/
      if (a_minor >= '0' && a_minor <= '9') {
         yvikeys_macro__delay (a_minor);
         DEBUG_USER   yLOG_char    ("delay"     , a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_major;
      }
      /*---(check macro name)------------*/
      if (yvikeys_macro__valid (a_minor) < 0)  {
         MODE_exit  ();
         DEBUG_USER   yLOG_note    ("bad name");
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(execute)---------------------*/
      MODE_exit  ();
      s_macro_repeat = REPEAT_count () + 1;
      DEBUG_USER   yLOG_value   ("repeat_m"  , s_macro_repeat);
      yvikeys_macro_exebeg  (a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
      /*---(done)------------------------*/
   }
   /*---(errors fall through)------------*/
   --rce;
   yvikeys_macro_reset ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char yvikeys_macro_modeget   (void)         { return s_macro_mode; }
char yvikeys_macro_modeset   (char a_mode) { s_macro_mode = a_mode; return 0; }



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
   char        x_cmd       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
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
   /*---(write)-----------------------*/
   yPARSE_fullwrite ("macro", a_abbr, s_macros [n].keys);
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
   DEBUG_SRCH   yLOG_note    ("saving values");
   yvikeys_macro__clear (x_abbr);
   strlcpy (s_macros [n].keys, x_keys, LEN_RECD );
   s_macros [n].len = strlen (x_keys);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}



char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
MACRO_writer           (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_beg       =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(prepare)----------------s-------*/
   yVIKEYS_unit_reset ();
   if (a_abbr == 0) {
      x_beg = 0;
      x_end = S_MACRO_MAX - 1;
   } else {
      x_beg = x_end = yvikeys_macro__index (a_abbr);
      if (x_beg <  0)  return rce;
   }
   /*---(find marked entries)------------*/
   for (i = x_beg; i <= x_end; ++i) {
      if (s_macros [i].len <= 0)  continue;
      yVIKEYS_file_write (SMOD_MACRO, &S_MACRO_LIST [i], &s_macros [i].count, &s_macros [i].rc, s_macros [i].keys, NULL, NULL, NULL, NULL, NULL);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
MACRO_reader            (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
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
   --rce;  if (a[0] < 'a' || a[0] > 'z') {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_index = yvikeys_macro__index (a[0]);
   DEBUG_SRCH   yLOG_value   ("index"     , x_index);
   /*---(search)-------------------------*/
   DEBUG_SRCH   yLOG_point   ("search"    , b);
   --rce;  if (b == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strlcpy (s_macros [x_index].keys, d, LEN_RECD);
   s_macros [x_index].count  = atoi (b);
   s_macros [x_index].rc     = atoi (c);
   /*---(initialize other)---------------*/
   s_macros [x_index].active = '-';
   s_macros [x_index].pos    =   -1;
   s_macros [x_index].len    = strlen (s_macros [x_index].keys);
   s_macros [x_index].cur    =  ' ';
   s_macros [x_index].repeat =    0;
   /*---(save)---------------------------*/
   if (s_saver != NULL)  s_saver (a[0], d);
   /*---(complete)-----------------------*/
   DEBUG_SRCH  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_macro__unit     (char *a_question, char a_macro)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_index     =    0;
   char        x_list      [LEN_RECD];
   int         c           =    0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MACRO unit       : question not understood");
   /*---(simple questions)---------------*/
   if      (strcmp (a_question, "curr"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO curr   (%c) : %c %2d %2d[%-.40s]", s_macro_name, s_macro_mode, s_macro_pos, s_macro_len, s_macro_keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "exec"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO exec   (%c) : %c %c %3d %02x %3d:%-.30s", s_macro_name, s_macro_mode, s_macro_delay, s_macro_pos, (uchar) s_macro_char, s_macro_len, s_macro_keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "keys"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO keys   (%c) : %-.45s", a_macro, s_macro_keys);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "list"           )   == 0) {
      yvikeys_macro_list (&c, x_list);
      snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO list       : %2d %-.45s", c, x_list);
      return yVIKEYS__unit_answer;
   }
   /*---(complex questions)--------------*/
   x_index = yvikeys_macro__index (a_macro);
   if (yvikeys_macro__valid (a_macro) < 0) {
      strcpy  (yVIKEYS__unit_answer, "MACRO unit       : not a valid macro name");
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      if (x_index < 0)  snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO saved  (%c) : not a valid macro name", a_macro);
      else              snprintf (yVIKEYS__unit_answer, LEN_MACRO, "MACRO saved  (%c) : %c %2d %2d[%-.40s]", a_macro, s_macros [x_index].active, s_macros [x_index].pos, s_macros [x_index].len, s_macros [x_index].keys);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



/*============================----end-of-source---============================*/
