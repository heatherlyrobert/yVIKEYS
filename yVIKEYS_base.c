/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



static char  s_keylog       [10000];
static int   s_nkeylog      = 0;
static char  s_keys         [LEN_LABEL] = "";
static char  s_display      [LEN_RECD ] = "";



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
   snprintf (yVIKEYS_ver, 100, "%s   %s : %s", t, YVIKEYS_VER_NUM, YVIKEYS_VER_TXT);
   return yVIKEYS_ver;
}

char
yVIKEYS_init         (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*----(modes)-------------------------*/
   MODE_init    ();
   MAP_init     ();
   SOURCE_init  ();
   TEXTREG_init ();
   MARK_init    ();
   SRCH_init    ();
   CMDS_init    ();
   MACRO_init   ();
   REPEAT_reset ();
   myVIKEYS.done      = '-';
   myVIKEYS.trouble   = '-';
   /*----(complete)----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_wrap         (void)
{
   VIEW_wrap ();
   return 0;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char          yVIKEYS__unit_answer [LEN_STR];

char       /*----: set up program urgents/debugging --------------------------*/
BASE__unit_quiet       (void)
{
   ySTR_debug ('-');
   myVIKEYS.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_QUIET);
   yVIKEYS_init ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
BASE__unit_loud        (void)
{
   ySTR_debug ('y');
   myVIKEYS.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_NOISE);
   yURG_name  ("kitchen"      , YURG_ON);
   /*> yURG_name  ("user"         , YURG_ON);                                         <* 
    *> yURG_name  ("graf"         , YURG_ON);                                         <* 
    *> yURG_name  ("wind"         , YURG_ON);                                         <* 
    *> yURG_name  ("yvikeys"      , YURG_ON);                                         <* 
    *> yURG_name  ("yvikeys_mode" , YURG_ON);                                         <* 
    *> yURG_name  ("yvikeys_keys" , YURG_ON);                                         <* 
    *> yURG_name  ("yvikeys_scale", YURG_ON);                                         <*/
   DEBUG_YVIKEYS yLOG_info     ("yVIKEYS"    , yVIKEYS_version   ());
   yVIKEYS_init ();
   return 0;
}

char       /*----: stop logging ----------------------------------------------*/
BASE__unit_end         (void)
{
   yVIKEYS_wrap ();
   yLOG_end     ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         main loop                            ----===*/
/*====================------------------------------------====================*/
static void  o___MAIN____________o () { return; }

char yVIKEYS_quit            (void) { if (myVIKEYS.done    == 'y') return 1; return 0; }
char yVIKEYS_error           (void) { if (myVIKEYS.trouble != '-') return 1; return 0; }

char         /*-> tbd --------------------------------[ leaf   [gz.430.151.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
BASE_key_status    (char *a_msg)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD];
   int         x_key       = 0;
   int         x_len       = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   int         x_start     = 0;             /* iterator -- keys               */
   x_len = strlen (s_keylog) - 1;
   x_start = x_len - (20 * 5);
   if (x_start < 0) x_start = 0;
   snprintf (a_msg, 500, "keys (%4d) %-100.100s", s_nkeylog, s_keylog + x_start);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
BASE__key_logger        (uchar a_key)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [10];
   int         x_key       =0;
   /*---(normal)-------------------------*/
   x_key = chrvisible (a_key);
   sprintf  (t, "%c", x_key);
   strlcat  (s_keylog, t, 10000);
   ++s_nkeylog;
   /*---(macro)--------------------------*/
   IF_MACRO_RECORDING {
      MACRO_rec_key (x_key);
   }
   /*---(complete)-----------------------*/
   return 0;
}

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
   /*---(defense)------------------------*/
   if (a_key == 0) {
      DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
      return a_key;
   }
   /*---(normal)-------------------------*/
   IF_MACRO_OFF {
      DEBUG_LOOP   yLOG_note    ("normal/macro off");
      if (MACRO_count ()) {
         DEBUG_USER   yLOG_note    ("but, in macro repeat mode");
         MACRO_exec_beg ('@');
         x_ch  = ' ';
      } else {
         x_ch  = a_key;
      }
   }
   /*---(run, delay, or playback)--------*/
   else IF_MACRO_PLAYING {
      DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");
      x_ch = MACRO_exec_key ();
      IF_MACRO_OFF {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return -1;
      }
      DEBUG_LOOP   yLOG_note    ("show screen");
      /*> if (a_runmode == RUN_USER)  CURS_main  ();                                     <*/
      MACRO_exec_wait ();
      DEBUG_LOOP   yLOG_note    ("read playback keystroke");
      x_play = a_key;
      DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);
      if (MACRO_exec_player (x_play) < 0) {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return -2;
      }
   }
   /*---(record)-------------------------*/
   DEBUG_LOOP   yLOG_note    ("handle keystroke normally");
   BASE__key_logger (x_ch);
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
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   /*---(defense)------------------------*/
   if (a_key == 0) {
      DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(prepare)------------------------*/
   myVIKEYS.trouble = '-';
   x_key = chrworking (a_key);
   /*---(handle count)-------------------*/
   if (MODE_curr () == SMOD_REPEAT) {
      rc = REPEAT_umode (x_major, x_key);
      if (rc >  0)  x_major = ' ';
   }
   /*---(main loop)----------------------*/
   while (1) {
      /*---(handle keystroke)------------*/
      switch (MODE_curr ()) {
      case MODE_GOD      : rc = BASE__________stub    (x_major , x_key);  break;
      case MODE_MAP      : rc = MAP_mode              (x_major , x_key);  break;
      case MODE_SOURCE   : rc = SOURCE_mode           (x_major , x_key);  break;
      case MODE_INPUT    : rc = INPUT_mode            (x_major , x_key);  break;
      case MODE_COMMAND  : rc = CMDS_mode             (x_major , x_key);  break;
      case MODE_SEARCH   : rc = SRCH_mode             (x_major , x_key);  break;
      case MODE_VISUAL   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_ERROR    : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_SELECT   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_TEXTREG  : rc = TEXTREG_smode         (x_major , x_key);  break;
      case SMOD_REPLACE  : rc = REPLACE_smode         (x_major , x_key);  break;
      case SMOD_FORMAT   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_BUFFER   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_WANDER   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_REGISTER : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_MARK     : rc = MARK_smode            (x_major , x_key);  break;
      case SMOD_MENUS    : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_MACRO    : rc = MACRO_smode           (x_major , x_key);  break;
      case SMOD_REPEAT   :                                                break;
      default            : rc = -1;  x_nomode = 'y';                      break;
      }
      /*---(translate unprintable)-------*/
      x_repeat = REPEAT_count ();
      /*> printf ("rc %3d, repeat %2d, major %c, minor %c\n", rc, x_repeat, x_major, x_key);   <*/
      if      (x_key == 0       )      snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_NULL  );
      else if (x_key == G_KEY_RETURN)  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_RETURN);
      else if (x_key == G_KEY_ESCAPE)  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_ESCAPE);
      else if (x_key == G_KEY_TAB   )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_TAB   );
      else if (x_key == G_KEY_BS    )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_BS    );
      else if (x_key == G_KEY_DEL   )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_BS    );
      else if (x_key == G_KEY_SPACE )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_SPACE );
      else if (x_key <= G_KEY_SPACE )  snprintf (s_keys,   9, "%2d %c%02x", x_repeat, x_major, x_key);
      else                             snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, x_key);
      /*---(multiplier)------------------*/
      if (rc >= 0 && x_repeat > 0 && MODE_curr () != SMOD_REPEAT) {
         REPEAT_decrement ();
         if (rc > 0)  x_major = rc;
         continue;
      }
      /*---(multiplier)------------------*/
      break;
      /*---(done)------------------------*/
   }
   /*---(setup for next keystroke)-------*/
   if      (rc == 0)    x_major = ' ';
   else if (rc >  0)    x_major = rc;
   else               { x_major = ' ';  myVIKEYS.trouble = 'y';  REPEAT_reset (); }
   /*---(setup status line)--------------*/
   if        (MODE_curr() == MODE_COMMAND) {
      MODE_message (s_display, CMDS_curr ());
   } else if (MODE_curr() == MODE_SEARCH ) {
      MODE_message (s_display, SRCH_curr ());
   } else if (x_savemode != MODE_curr()) {
      MODE_message (s_display, CMDS_curr ());
   }
   yVIKEYS_view_text (YVIKEYS_COMMAND, s_display);
   yVIKEYS_view_text (YVIKEYS_KEYS   , s_keys   );
   /*---(save current mode)--------------*/
   x_savemode = MODE_curr ();
   /*---(advance macros)-----------------*/
   IF_MACRO_ON  MACRO_exec_adv ();
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}

uchar        /*-> process input string in main loop --[ ------ [ge.C74.153.42]*/ /*-[02.0000.00#.D]-*/ /*-[--.---.---.--]-*/
BASE__main_string    (uchar *a_keys)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;
   int         i           =    0;
   int         x_len       =    0;
   char        x_ch        =  ' ';     /* current keystroke                   */
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_LOOP   yLOG_note    ("a_keys is null");
      DEBUG_LOOP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_LOOP   yLOG_info    ("a_keys"    , a_keys);
   x_len = strlen (a_keys);
   DEBUG_LOOP   yLOG_value   ("x_len"     , x_len);
   --rce;
   for (i = 0; i < x_len; ++i) {
      DEBUG_LOOP   yLOG_value   ("LOOP"      , i);
      /*---(get next char)---------------*/
      x_ch = a_keys [i];
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      if (x_ch <  0) {
         DEBUG_SCRP   yLOG_note    ("special character");
         DEBUG_SCRP   yLOG_value   ("256 + x_ch", 256 + x_ch);
         /*---(translate special)--------*/
         switch (256 + x_ch) {
         case G_CHAR_RETURN  :  x_ch = G_KEY_RETURN;  break;
         case G_CHAR_ESCAPE  :  x_ch = G_KEY_ESCAPE;  break;
         case G_CHAR_BS      :  x_ch = G_KEY_BS;      break;
         case G_CHAR_TAB     :  x_ch = G_KEY_TAB;     break;
         case G_CHAR_SPACE   :  x_ch = G_KEY_SPACE;   break;
         case G_CHAR_GROUP   :  x_ch = G_KEY_GROUP;   break;
         case G_CHAR_FIELD   :  x_ch = G_KEY_FIELD;   break;
         }
         DEBUG_SCRP   yLOG_value   ("x_ch (new)", x_ch);
      }
      /*---(handle input)----------------*/
      x_ch = yVIKEYS_main_input (RUN_TEST, x_ch);
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      if (x_ch == -1) continue;
      /*---(handle keystroke)------------*/
      rc = yVIKEYS_main_handle (x_ch);
      DEBUG_LOOP   yLOG_value   ("rc"        , rc);
      /*---(done)------------------------*/
   }
   DEBUG_LOOP   yLOG_note    ("main loop done");
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}


