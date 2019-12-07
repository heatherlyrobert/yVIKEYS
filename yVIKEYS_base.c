/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 * metis  tn4··  main loop must kick into immediate mode when close group is found
 *
 */

char *gvikeys_lower   = "abcdefghijklmnopqrstuvwxyz";
char *gvikeys_upper   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char *gvikeys_number  = "0123456789";
char *gvikeys_greek   = "èéêëìíîïðñòóôõö÷øùúûüýþÿ";



static char  s_keys_log   [10000];
static char  s_keys_multi [10000];
static char  s_keys_mode  [10000];
static char  s_keys_error [10000];
static int   s_nkey      = 0;
static int   s_gpos      = 0;
static char  s_keys_last  [LEN_LABEL];



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void      o___UTILITY_________________o (void) {;}
char        yVIKEYS_ver     [500];

char*      /* ---- : return library versioning information -------------------*/
yVIKEYS_version    (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc    ]", 15);
#elif  __HEPH__  > 0
   strncpy (t, "[hephaestus ]", 15);
#else
   strncpy (t, "[unknown    ]", 15);
#endif
   snprintf (yVIKEYS_ver, 100, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return yVIKEYS_ver;
}

char         /*-> initialize all of yvikeys ----------[ leaf   [gn.530.341.50]*/ /*-[02.0000.000.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_init         (char a_mode)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*----(first)-------------------------*/
   STATUS_init  ();
   srand (time (NULL));
   /*----(early)-------------------------*/
   yPARSE_init  ('y', NULL, '-');
   yPARSE_delimiters  ("");
   MODE_init            (a_mode);
   yvikeys_view_init    ();
   yvikeys_file_init    ();
   /*----(middling)----------------------*/
   yvikeys_menu_init    ();
   yvikeys_cmds_init    ();
   yvikeys_srch_init    ();
   /*----(later)-------------------------*/
   KEYS_init    ();
   yvikeys_map_init     ();
   SOURCE_init  ();
   yvikeys_macro_init   ();
   /*----(latest)------------------------*/
   yvikeys_mreg_init    ();
   yvikeys_sreg_init    ();
   yvikeys_sundo_init   ();
   yvikeys_mark_init    ();
   yvikeys_visu_init    ();
   REPEAT_init  ();
   yvikeys_bufs_init    ();
   yvikeys_hist_init    ();
   yvikeys_loop_init    ();
   /*----(globals)-----------------------*/
   myVIKEYS.done      = '-';
   myVIKEYS.trouble   = '-';
   myVIKEYS.info_win  = '-';
   /*----(complete)----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_wrap         (void)
{
   yPARSE_wrap ();
   VIEW_wrap   ();
   STATUS_wrap ();
   yvikeys_mreg_wrap   ();
   yvikeys_menu_wrap   ();
   return 0;
}

char yVIKEYS_quit            (void) { if (myVIKEYS.done    == 'y') return 1; return 0; }
char yVIKEYS_error           (void) { if (myVIKEYS.trouble != '-') return 1; return 0; }

char
BASE_dump               (char *a_what)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   FILE       *f           = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_what == NULL)  return rce;
   /*---(open file)----------------------*/
   f = fopen ("/root/z_gehye/vi_clip.txt", "w");
   --rce;  if (f == NULL)       return rce;
   /*---(dump)---------------------------*/
   if      (strcmp (a_what, "keys"      ) == 0)  KEYS_dump       (f);
   else if (strcmp (a_what, "status"    ) == 0)  STATUS_dump     (f);
   /*---(close)--------------------------*/
   fclose (f);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> enter a object directly ------------[ ------ [ge.850.137.A4]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
yvikeys_base_direct     (char a_mode, char *a_string, void *a_purger (), void *a_clearer (), void *a_saver ())
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_ch        =  '-';
   char       *x_valid     = "*aA0è";
   int         x_beg       =    0;
   char        x_string    [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (a_mode)) {
      DEBUG_PROG   yLOG_note    ("can not execute until operational");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_point   ("a_string"  , a_string);
   --rce;  if (a_string == NULL) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG   yLOG_info    ("a_string"  , a_string);
   x_len = strlen (a_string);
   DEBUG_PROG   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for purge)----------------*/
   if (x_len == 1) {
      DEBUG_PROG   yLOG_note    ("one character option (purge)");
      DEBUG_PROG   yLOG_point   ("a_purger"  , a_purger);
      --rce;  if (a_purger == NULL) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_PROG   yLOG_info    ("x_valid"   , x_valid);
      x_ch = a_string [0];
      DEBUG_PROG   yLOG_char    ("x_ch"      , x_ch);
      --rce;  if (strchr (x_valid, x_ch) == NULL) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = a_purger (x_ch);
      DEBUG_PROG   yLOG_value   ("purge"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for equal)----------------*/
   DEBUG_PROG   yLOG_note    ("check for equal sign");
   x_ch = a_string [1];
   DEBUG_PROG   yLOG_char    ("x_ch"      , x_ch);
   --rce;  if (x_ch != '=') {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_ch = a_string [0];
   DEBUG_PROG   yLOG_char    ("abbr"      , x_ch);
   /*---(check for clear)----------------*/
   if (x_len == 2) {
      DEBUG_PROG   yLOG_note    ("two character option (clear)");
      DEBUG_PROG   yLOG_point   ("a_clearer" , a_clearer);
      --rce;  if (a_clearer == NULL) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = a_clearer (x_ch);
      DEBUG_PROG   yLOG_value   ("clear"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle quotes)------------------*/
   DEBUG_PROG   yLOG_note    ("multi-char option (save)");
   --rce;  if (a_string [2] != G_KEY_DQUOTE) {
      DEBUG_PROG   yLOG_note    ("normal/unquoted format");
      strlcpy (x_string, a_string + 2, LEN_RECD);
   } else {
      DEBUG_PROG   yLOG_note    ("quoted format");
      if (a_string [x_len - 1] != G_KEY_DQUOTE) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      strlcpy (x_string, a_string + 3, LEN_RECD);
      x_string [x_len - 4] = '\0';
   }
   DEBUG_PROG   yLOG_info    ("x_string", x_string);
   /*---(clear option)-------------------*/
   x_len = strlen (x_string);
   DEBUG_PROG   yLOG_value   ("x_len"     , x_len);
   if (x_len == 0) {
      DEBUG_PROG   yLOG_point   ("a_clearer" , a_clearer);
      --rce;  if (a_clearer == NULL) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = a_clearer (x_ch);
      DEBUG_PROG   yLOG_value   ("clear"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_PROG   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(save)---------------------------*/
   DEBUG_PROG   yLOG_point   ("a_saver" , a_saver);
   --rce;  if (a_saver == NULL) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = a_saver (x_ch, x_string);
   DEBUG_PROG   yLOG_value   ("save"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char          yVIKEYS__unit_answer [LEN_FULL];

char       /*----: set up program urgents/debugging --------------------------*/
BASE__unit_quiet       (void)
{
   int         x_narg       = 1;
   char       *x_args [20]  = {"yVIKEYS_unit" };
   /*> yURG_logger   (x_narg, x_args);                                                <*/
   /*> yURG_urgs     (x_narg, x_args);                                                <*/
   yVIKEYS_init (MODE_MAP);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
BASE__unit_loud        (void)
{
   int         x_narg       = 1;
   char       *x_args [20]  = {"yVIKEYS_unit" };
   yURG_logger   (x_narg, x_args);
   yURG_urgs     (x_narg, x_args);
   yURG_name  ("kitchen"      , YURG_ON);
   yURG_name  ("yurg"         , YURG_ON);
   yURG_name  ("edit"         , YURG_ON);
   yURG_name  ("mark"         , YURG_ON);
   yURG_name  ("mode"         , YURG_ON);
   yURG_name  ("map"          , YURG_ON);
   yURG_name  ("map_mas"      , YURG_ON);
   yURG_name  ("cmds"         , YURG_ON);
   yURG_name  ("srch"         , YURG_ON);
   yURG_name  ("hist"         , YURG_ON);
   yURG_name  ("graf"         , YURG_ON);
   yURG_name  ("ystr"         , YURG_ON);
   yURG_name  ("yparse"       , YURG_ON);
   DEBUG_YVIKEYS yLOG_info     ("yVIKEYS"    , yVIKEYS_version   ());
   yVIKEYS_init (MODE_MAP);
   return 0;
}

char       /*----: stop logging ----------------------------------------------*/
BASE__unit_end         (void)
{
   yVIKEYS_wrap ();
   yLOGS_end    ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        key logging                           ----===*/
/*====================------------------------------------====================*/
static void  o___LOGGING_________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.430.151.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
KEYS_status        (char *a_msg)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD];
   int         x_key       = 0;
   int         x_len       = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   int         x_start     = 0;             /* iterator -- keys               */
   x_len = strlen (s_keys_log) - 1;
   x_start = x_len - 40;
   if (x_start < 0) x_start = 0;
   snprintf (a_msg, 500, "keys    %-5d %s¤", s_nkey, s_keys_log + x_start);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
KEYS__multi             (int a_pos)
{
   char        x_mode      =  ' ';
   /*---(defense)------------------------*/
   if (a_pos < 0 || a_pos >  s_nkey)                             return 0;
   if (s_keys_log [a_pos] == G_KEY_NULL )                        return 0;
   if (s_keys_log [a_pos] == G_KEY_SPACE)                        return 0;
   if (s_keys_log [a_pos] == G_CHAR_SPACE)                       return 0;
   /*---(key for mode)-------------------*/
   x_mode = s_keys_mode [a_pos];
   if (a_pos > 0 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 1];
   if (a_pos > 1 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 2];
   if (a_pos > 2 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 3];
   if (a_pos > 3 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 4];
   switch (x_mode) {
   case MODE_SOURCE  : case MODE_COMMAND : case MODE_SEARCH  :
      if (strchr (g_multisrc, s_keys_log [a_pos]) != NULL)       return 1;
      break;
   case MODE_MAP     :
      if (strchr (g_multimap, s_keys_log [a_pos]) != NULL)       return 1;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
KEYS__logger            (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_key       =    0;
   int         x_multi     =  '-';
   /*---(key)----------------------------*/
   x_key = chrvisible (a_key);
   s_keys_log   [s_nkey]     = x_key;
   s_keys_log   [s_nkey + 1] = 0;
   /*---(mode)---------------------------*/
   s_keys_mode  [s_nkey]     = MODE_curr ();
   s_keys_mode  [s_nkey + 1] = 0;
   /*---(error)--------------------------*/
   s_keys_error [s_nkey]     = '-';
   s_keys_error [s_nkey + 1] = 0;
   /*---(multi)--------------------------*/
   x_multi = KEYS__multi (s_nkey);
   switch (s_keys_multi [s_nkey - 1]) {
   case 'p' :
      s_keys_multi [s_nkey] = 's';
      break;
   case 's' : case '-' : default  :
      if (x_multi)  s_keys_multi [s_nkey] = 'p';
      else          s_keys_multi [s_nkey] = '-';
      break;
   }
   s_keys_multi [s_nkey + 1] = 0;
   /*---(update count)-------------------*/
   ++s_nkey;
   ++s_gpos;
   /*---(macro)--------------------------*/
   IF_MACRO_RECORDING {
      yvikeys_macro_reckey (x_key);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
KEYS__error             (void)
{
   s_keys_error [s_nkey - 1] = 'E';
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
KEYS_unique             (void)
{  /*    return 1 if not a repeat sequence, 0 if repeating   */
   /*    five mode options :    - -    - p    p s    s -    s p
   */
   /*---(locals)-----------+-----+-----+-*/
   uchar       m1, m2, m3, m4;
   uchar       c1, c2, c3, c4;
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_senter  (__FUNCTION__);
   DEBUG_LOOP   yLOG_sint    (s_nkey);
   /*---(basic defense)------------------*/
   if (s_nkey < 2) {
      DEBUG_LOOP   yLOG_snote   ("too few keys, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   /*---(prepare)------------------------*/
   c3 = s_keys_log   [s_nkey - 2];
   m3 = s_keys_multi [s_nkey - 2];
   c4 = s_keys_log   [s_nkey - 1];
   m4 = s_keys_multi [s_nkey - 1];
   /*---(filter)-------------------------*/
   DEBUG_LOOP   yLOG_sint    (m3);
   if (strchr ("ps-", m3) == NULL) {
      DEBUG_LOOP   yLOG_snote   ("m3 is weird, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (m4);
   if (strchr ("ps-", m4) == NULL) {
      DEBUG_LOOP   yLOG_snote   ("m4 is weird, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   if (m3 == 's' && m4 == '-') {
      DEBUG_LOOP   yLOG_snote   ("coming off combo, obviously unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   if (m4 == 'p') {
      DEBUG_LOOP   yLOG_snote   ("starting combo, obviously unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (c3);
   if (c3 == G_CHAR_SPACE) {
      DEBUG_LOOP   yLOG_snote   ("c3 is space, so unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (c4);
   if (c4 == G_CHAR_SPACE) {
      DEBUG_LOOP   yLOG_snote   ("c4 is space, so unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   /*---(single)-------------------------*/
   if (m3 == '-' && m4 == '-' && c3 == c4)       return 0;
   /*---(filter)-------------------------*/
   if (s_nkey < 4)                               return 1;
   /*---(prepare)------------------------*/
   c1 = s_keys_log   [s_nkey - 4];
   m1 = s_keys_multi [s_nkey - 4];
   c2 = s_keys_log   [s_nkey - 3];
   m2 = s_keys_multi [s_nkey - 3];
   /*---(filter)-------------------------*/
   if (strchr ("ps-", m1) == NULL)               return 0;
   if (strchr ("ps-", m2) == NULL)               return 0;
   if (m1 != 'p')                                return 1;
   if (m2 != 's')                                return 1;
   /*---(double)-------------------------*/
   if (c4 == c2 && c3 == c1)                     return 0;
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
   return 1;
}

int  yVIKEYS_keys_nkey       (void) { return s_nkey; }
int  yvikeys_keys_gpos       (void) { return s_gpos; }

char
yvikeys_keys_keygpos    (void)
{
   if (s_gpos < 0)         return 0;
   if (s_gpos >= s_nkey)   return 0;
   return s_keys_log [s_gpos];
}

char
yvikeys_keys_repeating  (void)
{
   if (s_gpos < s_nkey)  return 1;
   return 0;
}

char*
yVIKEYS_keys_last       (void)
{
   sprintf (s_keys_last, "(%c) %c", s_keys_mode [s_nkey - 1], s_keys_log [s_nkey - 1]);
   return s_keys_last;
}

char
KEYS_dump               (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   fprintf (a_file, "yVIKEYS, key logging report                                                               (:dump keys)\n");
   fprintf (a_file, "len = %d\n", s_nkey);
   fprintf (a_file, "          ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "%-10d", i);
   fprintf (a_file, "\n");
   fprintf (a_file, "type----  ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "-123456789");
   fprintf (a_file, "\n");
   fprintf (a_file, "keys      %s\n", s_keys_log);
   fprintf (a_file, "mode      %s\n", s_keys_mode);
   fprintf (a_file, "multi     %s\n", s_keys_multi);
   fprintf (a_file, "error     %s\n", s_keys_error);
   fprintf (a_file, "type----  ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "-123456789");
   fprintf (a_file, "\n");
   fprintf (a_file, "          ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "%-10d", i);
   fprintf (a_file, "\n");
   /*---(complete)-----------------------*/
   return 0;
}

char
KEYS_init               (void)
{
   strlcpy (s_keys_log  , "", LEN_RECD);
   strlcpy (s_keys_mode , "", LEN_RECD);
   strlcpy (s_keys_multi, "", LEN_RECD);
   strlcpy (s_keys_error, "", LEN_RECD);
   s_nkey = 0;
   s_gpos = 0;
   return 0;
}

char
KEYS_repos              (int a_pos)
{
   s_gpos = a_pos;
}



/*====================------------------------------------====================*/
/*===----                         main loop                            ----===*/
/*====================------------------------------------====================*/
static void  o___MAIN____________o () { return; }

uchar        /*-> gather main loop keyboard input ----[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_input      (char a_runmode, uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   uchar       x_ch        = ' ';
   uchar       x_play      = ' ';
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_char    ("a_runmode" , a_runmode);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   DEBUG_LOOP   yLOG_char    ("macro rec" , yvikeys_macro_rmode ());
   /*---(fixes)--------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      if (a_key == G_KEY_ENTER)  a_key = G_KEY_RETURN; /* X11 sends incorrently  */
      if (a_key == G_KEY_DEL  )  a_key = G_KEY_BS;     /* X11 sends incorrectly  */
   }
   /*---(normal)-------------------------*/
   IF_MACRO_NOT_PLAYING {
      /*---(repeating)---------*/
      if (yvikeys_keys_repeating ()) {
         DEBUG_LOOP   yLOG_note    ("normal mode, group repeating older keys");
         x_ch = s_keys_log [s_gpos];
         ++s_gpos;
      }
      /*---(not-repeating)-----*/
      else {
         DEBUG_LOOP   yLOG_note    ("normal mode, new keystroke and recording");
         x_ch  = a_key;
         if (x_ch != 0)  KEYS__logger (x_ch);
      }
      /*---(done)--------------*/
   }
   /*---(run, delay, or playback)--------*/
   else IF_MACRO_PLAYING {
      /*---(playback)----------*/
      DEBUG_LOOP   yLOG_note    ("handle playback control");
      x_play = a_key;
      DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);
      if (yvikeys_macro_exeplay (x_play) < 0) {
         DEBUG_LOOP   yLOG_note    ("terminated, do not execute next key");
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*---(not-repeating)-----*/
      DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");
      x_ch = (uchar) yvikeys_macro_exekey ();
      IF_MACRO_OFF {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return x_ch;
      }
      /*---(advance)-----------*/
      DEBUG_LOOP   yLOG_note    ("advance");
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      if (x_play == 0)  x_play = x_ch;
      yvikeys_macro_exeadv (x_play);
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char
BASE__________stub      (char a_major, char a_minor)
{
   MODE_exit  ();
   return a_minor;
}

uchar        /*-> process main loop keyboard input ---[ leaf   [gc.GD1.132.IM]*/ /*-[05.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_handle     (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          = 0;             /* generic return code            */
   static char x_major     = ' ';           /* saved keystroke from last loop */
   static char x_savemode  = '-';           /* to identify mode changes       */
   int         x_repeat    = 0;             /* store current repeat count     */
   char        x_nomode    = '-';           /* flag illegal mode              */
   uchar       x_key       = ' ';
   char        x_keys      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   /*---(defense)------------------------*/
   if (a_key == 0) {
      DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_LOOP   yLOG_note    ("REAL KEY");
   /*---(prepare)------------------------*/
   myVIKEYS.trouble   = '-';
   x_key = chrworking (a_key);
   DEBUG_LOOP   yLOG_value   ("x_key"     , x_key);
   /*---(handle count)-------------------*/
   if (MODE_curr () == UMOD_REPEAT) {
      rc = REPEAT_umode (x_major, x_key);
      if (rc >  0)  x_major = G_KEY_SPACE;
   }
   /*---(main loop)----------------------*/
   while (1) {
      DEBUG_LOOP   yLOG_char    ("MODE bef"  , MODE_curr ());
      /*---(handle keystroke)------------*/
      switch (MODE_curr ()) {
      case MODE_GOD      : rc = GOD_mode              (x_major , x_key);  break;
      case MODE_PROGRESS : rc = PROGRESS_mode         (x_major , x_key);  break;
      case MODE_MAP      : rc = yvikeys_map_mode      (x_major , x_key);  break;
      case MODE_SOURCE   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_SRC_INPT : rc = SRC_INPT_umode        (x_major , x_key);  break;
      case SMOD_SREG     : rc = yvikeys_sreg_smode    (x_major , x_key);  break;
      case UMOD_SRC_REPL : rc = SRC_REPL_umode        (x_major , x_key);  break;
      case UMOD_SRC_UNDO : rc = BASE__________stub    (x_major , x_key);  break;
      case UMOD_MAP_UNDO : rc = BASE__________stub    (x_major , x_key);  break;
      case MODE_COMMAND  : rc = SOURCE_mode           (x_major , x_key);  break;
      case MODE_SEARCH   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_HISTORY  : rc = HISTORY_smode         (x_major , x_key);  break;
      case UMOD_VISUAL   : rc = yvikeys_visu_umode    (x_major , x_key);  break;
      case SMOD_ERROR    : rc = BASE__________stub    (x_major , x_key);  break;
      case XMOD_FORMAT   : rc = FORMAT_xmode          (x_major , x_key);  break;
      case XMOD_UNITS    : rc = UNITS_xmode           (x_major , x_key);  break;
      case SMOD_BUFFER   : rc = yvikeys_bufs_umode    (x_major , x_key);  break;
      case UMOD_WANDER   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_MREG     : rc = yvikeys_mreg_smode    (x_major , x_key);  break;
      case UMOD_MARK     : rc = yvikeys_mark_smode    (x_major , x_key);  break;
      case SMOD_MENUS    : rc = yvikeys_menu_smode    (x_major , x_key);  break;
      case SMOD_MACRO    : rc = yvikeys_macro_smode   (x_major , x_key);  break;
      case UMOD_REPEAT   :                                                break;
      default            : rc = -1;  x_nomode = 'y';                      break;
      }
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_LOOP   yLOG_char    ("MODE aft"  , MODE_curr ());
      /*---(translate unprintable)-------*/
      x_repeat = REPEAT_count ();
      DEBUG_USER   yLOG_value   ("x_repeat"  , x_repeat);
      /*> snprintf (x_keys,   9, "%2d %c%c"  , x_repeat, x_major, chrvisible (x_key));   <*/
      snprintf (x_keys,   9, " %2x%c%c"  , x_key, x_major, chrvisible (x_key));
      /*---(loop repeats)----------------*/
      if (rc == 0 && x_repeat > 0 && MODE_curr () != UMOD_REPEAT) {
         DEBUG_USER   yLOG_note    ("repeating");
         REPEAT_decrement ();
         continue;
      }
      /*---(loop repeats)----------------*/
      if (rc <= 0 && MODE_curr () != UMOD_REPEAT) {
         DEBUG_USER   yLOG_note    ("complete repeat");
         REPEAT_done ();
      }
      break;
      /*---(done)------------------------*/
   }
   /*---(setup for next keystroke)-------*/
   if      (rc == 0)    x_major = G_KEY_SPACE;
   else if (rc >  0)    x_major = rc;
   else               { x_major = G_KEY_SPACE;  KEYS__error ();  myVIKEYS.trouble = 'y';  REPEAT_reset (); }
   yvikeys_view_keys (x_keys);
   /*---(save current mode)--------------*/
   x_savemode = MODE_curr ();
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}

uchar        /*-> process input string in main loop --[ ------ [ge.C74.153.42]*/ /*-[02.0000.00#.D]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_string  (uchar *a_keys)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;
   int         i           =    0;
   int         j           =   -1;
   int         x_len       =    0;
   uchar       x_ch        =  ' ';     /* current keystroke                   */
   char        x_keys      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_LOOP   yLOG_note    ("a_keys is null");
      DEBUG_LOOP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy    (x_keys, a_keys  , LEN_RECD);
   strlencode (x_keys, ySTR_MAX, LEN_RECD);
   DEBUG_LOOP   yLOG_info    ("x_keys"    , x_keys);
   x_len = strlen (a_keys);
   DEBUG_LOOP   yLOG_value   ("x_len"     , x_len);
   --rce;
   for (i = 0; i < x_len; ++i) {
      DEBUG_LOOP   yLOG_value   ("LOOP"      , i);
      /*---(get next char)---------------*/
      if (i != j) {
         DEBUG_LOOP   yLOG_note    ("new keystroke");
         DEBUG_LOOP   yLOG_value   ("a_keys[i]" , a_keys[i]);
         DEBUG_LOOP   yLOG_char    ("a_keys[i]" , chrvisible (a_keys[i]));
         x_ch = chrworking (a_keys [i]);
      } else {
         DEBUG_LOOP   yLOG_note    ("repeat loop");
         x_ch = 0;
      }
      j = i;
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      /*---(handle input)----------------*/
      x_ch = yVIKEYS_main_input (RUN_TEST, x_ch);
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      if (x_ch == -1) continue;
      /*---(handle keystroke)------------*/
      rc = yVIKEYS_main_handle (x_ch);
      DEBUG_LOOP   yLOG_value   ("rc"        , rc);
      /*---(check for macro)-------------*/
      DEBUG_LOOP   yLOG_char    ("macro exe" , yvikeys_macro_emode ());
      DEBUG_LOOP   yLOG_value   ("macro cnt" , yvikeys_macro_count ());
      DEBUG_LOOP   yLOG_value   ("s_nkey"    , s_nkey);
      DEBUG_LOOP   yLOG_value   ("s_gpos"    , s_gpos);
      IF_MACRO_MOVING  {
         DEBUG_LOOP   yLOG_note    ("macro running and used step, back up loop counter");
         --i;
      }
      if (yvikeys_keys_repeating ()) {
         DEBUG_LOOP   yLOG_note    ("group using older keystrokes, back up loop counter");
         --i;
      }
      /*---(done)------------------------*/
   }
   DEBUG_LOOP   yLOG_note    ("main loop done");
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> handle main loop for ncurses -------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main            (char *a_delay, char *a_update, void *a_altinput ())
{
   /*---(locals)-----------+-----------+-*/
   int         x_loop      = 0;
   int         x_ch        = ' ';      /* current keystroke                   */
   uchar       x_key       = ' ';      /* current keystroke                   */
   char        rc          = 0;
   char        x_draw      = '-';
   char        x_group     = '-';
   /*---(prepare)------------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   yvikeys_loop_set     (a_delay, a_update);
   yVIKEYS_view_all (0.0);
   /*---(main-loop)----------------------*/
   while (1) {
      /*---(get input)-------------------*/
      x_ch = yvikeys_loop_getch ();
      DEBUG_GRAF  yLOG_value   ("x_ch"      , x_ch);
      yvikeys_loop_beg   ();
      /*---(specialty actions)-----------*/
      if (x_ch == KEY_RESIZE)  yVIKEYS_view_resize (0, 0, 0);
      if (x_ch == -102)        yVIKEYS_view_resize (0, 0, 0);
      if (x_ch < 0)  x_key = 0;
      else           x_key = x_ch;
      /*---(keyboard input)--------------*/
      DEBUG_GRAF  yLOG_value   ("x_key"     , x_key);
      x_key = yVIKEYS_main_input  (RUN_USER, x_key);
      yVIKEYS_main_handle (x_key);
      if (yVIKEYS_quit ())  break;
      /*---(alternate input)-------------*/
      if (a_altinput != NULL)   a_altinput ();
      /*---(repeating)-------------------*/
      if (x_key == ')' && yvikeys_macro_emode () == MACRO_STOP) {
         DEBUG_GRAF  yLOG_note    ("kick into grouping (non-macro) fast execution mode");
         yvikeys_macro_set2blitz ();
         x_group = 'y';
         continue;
      }
      if (!yvikeys_keys_repeating () && x_group == 'y') {
         DEBUG_GRAF  yLOG_note    ("end grouping (non-macro) fast execution mode");
         yvikeys_macro_set2run ();
         x_group = '-';
      }
      /*---(showing)---------------------*/
      ++x_loop;
      x_draw = '-';
      if ((x_loop % myVIKEYS.loops) == 0) {
         x_draw = 'y';
         yvikeys_loop_prog ();
         yVIKEYS_view_all (0.0);
      }
      /*---(sleeping)--------------------*/
      yvikeys_loop_sleep (x_key, x_draw);
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(complete)-----------------------*/
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
KEYS__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "KEYS unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "log"            )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS log         : %2d[%-.40s]", s_nkey, s_keys_log);
   }
   else if (strcmp (a_question, "mode"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS mode        : %2d[%-.40s]", s_nkey, s_keys_mode);
   }
   else if (strcmp (a_question, "multi"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS multi       : %2d[%-.40s]", s_nkey, s_keys_multi);
   }
   else if (strcmp (a_question, "error"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS error       : %2d[%-.40s]", s_nkey, s_keys_error);
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      KEYS_status (t);
      strltrim (t, ySTR_BOTH, LEN_RECD);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "%-.60s", t);
   }
   else if (strcmp (a_question, "pos"          )  == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS pos         : %3dn %3dp", s_nkey, s_gpos);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


