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
   /*----(modes)-------------------------*/
   yVIKEYS_mode_init   ();
   yVIKEYS_mode_enter  (MODE_MAP);
   /*----(commands)----------------------*/
   yVIKEYS_cmds_init   ();
   /*----(complete)----------------------*/
   return 0;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char          yVIKEYS__unit_answer [LEN_STR];

char       /*----: set up program urgents/debugging --------------------------*/
yVIKEYS__unit_quiet    (void)
{
   ySTR_debug ('-');
   its.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_QUIET);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yVIKEYS__unit_loud     (void)
{
   ySTR_debug ('y');
   its.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_NOISE);
   yURG_name  ("user"         , YURG_ON);
   yURG_name  ("graf"         , YURG_ON);
   yURG_name  ("wind"         , YURG_ON);
   yURG_name  ("yvikeys"      , YURG_ON);
   yURG_name  ("yvikeys_mode" , YURG_ON);
   yURG_name  ("yvikeys_keys" , YURG_ON);
   yURG_name  ("yvikeys_scale", YURG_ON);
   DEBUG_YVIKEYS yLOG_info     ("yVIKEYS"    , yVIKEYS_version   ());
   return 0;
}

char       /*----: stop logging ----------------------------------------------*/
yVIKEYS__unit_end      (void)
{
   yLOG_end     ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         main loop                            ----===*/
/*====================------------------------------------====================*/
static void  o___MAIN____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_record     (char a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [10];
   int         x_key       =0;
   /*---(normal)-------------------------*/
   sprintf  (t, "%c", a_curr);
   strlcat  (s_keylog, t, 10000);
   ++s_nkeylog;
   /*---(macro)--------------------------*/
   IF_MACRO_RECORDING {
      yVIKEYS_macro_rec_key (a_curr);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> gather main loop keyboard input ----[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_input      (char a_runmode, char a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_ch        = ' ';
   char        x_play      = ' ';
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_char    ("a_runmode" , a_runmode);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   /*---(normal)-------------------------*/
   IF_MACRO_OFF {
      DEBUG_LOOP   yLOG_note    ("normal/macro off");
      if (yVIKEYS_repeat_macro ()) {
         DEBUG_USER   yLOG_note    ("but, in macro repeat mode");
         yVIKEYS_macro_exec_beg ('@');
         x_ch  = ' ';
      } else {
         x_ch  = a_key;
      }
   }
   /*---(run, delay, or playback)--------*/
   else IF_MACRO_PLAYING {
      DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");
      x_ch = yVIKEYS_macro_exec_key ();
      IF_MACRO_OFF {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return -1;
      }
      DEBUG_LOOP   yLOG_note    ("show screen");
      /*> if (a_runmode == RUN_USER)  CURS_main  ();                                     <*/
      yVIKEYS_macro_exec_wait ();
      DEBUG_LOOP   yLOG_note    ("read playback keystroke");
      x_play = a_key;
      DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);
      if (yVIKEYS_macro_exec_play (x_play) < 0) {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return -2;
      }
   }
   /*---(record)-------------------------*/
   DEBUG_LOOP   yLOG_note    ("handle keystroke normally");
   yVIKEYS_main_record (x_ch);
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char
yVIKEYS_______stub      (char a_major, char a_minor)
{
   yVIKEYS_mode_exit ();
   return a_minor;
}

char         /*-> process main loop keyboard input ---[ leaf   [gc.GD1.132.IM]*/ /*-[05.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_handle     (char a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          = 0;             /* generic return code            */
   static char x_major     = ' ';           /* saved keystroke from last loop */
   static char x_savemode  = '-';           /* to identify mode changes       */
   int         x_repeat    = 0;             /* store current repeat count     */
   char        x_error     = '-';           /* flag error status              */
   char        x_nomode    = '-';           /* flag illegal mode              */
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   /*---(defense)------------------------*/
   if (a_key < 0) {
      DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
      return a_key;
   }
   /*---(handle count)-------------------*/
   if (yVIKEYS_mode_curr () == SMOD_REPEAT) {
      rc = yVIKEYS_repeat_umode (x_major, a_key);
      if (rc >  0)  x_major = ' ';
   }
   /*---(main loop)----------------------*/
   while (1) {
      /*---(handle keystroke)------------*/
      switch (yVIKEYS_mode_curr ()) {
      case MODE_GOD      : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case MODE_MAP      : rc = yVIKEYS_map_mode      (x_major , a_key);  break;
      case MODE_SOURCE   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case MODE_INPUT    : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case MODE_COMMAND  : rc = yVIKEYS_cmds_mode     (x_major , a_key);  break;
      case MODE_SEARCH   : rc = yVIKEYS_srch_mode     (x_major , a_key);  break;
      case MODE_VISUAL   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_ERROR    : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_SELECT   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_TEXTREG  : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_REPLACE  : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_FORMAT   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_BUFFER   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_WANDER   : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_REGISTER : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_MARK     : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_MENUS    : rc = yVIKEYS_______stub    (x_major , a_key);  break;
      case SMOD_MACRO    : rc = yVIKEYS_macro_smode   (x_major , a_key);  break;
      default            : rc = -1;  x_nomode = 'y';                      break;
      }
      /*---(translate unprintable)-------*/
      x_repeat = yVIKEYS_repeat_value ();
      if      (a_key == 0       )      snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_NULL  );
      else if (a_key == G_KEY_RETURN)  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_RETURN);
      else if (a_key == G_KEY_ESCAPE)  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_ESCAPE);
      else if (a_key == G_KEY_TAB   )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_TAB   );
      else if (a_key == G_KEY_BS    )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_BS    );
      else if (a_key == G_KEY_DEL   )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_BS    );
      else if (a_key == G_KEY_SPACE )  snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, G_CHAR_SPACE );
      else if (a_key <= G_KEY_SPACE )  snprintf (s_keys,   9, "%2d %c%02x", x_repeat, x_major, a_key);
      else                             snprintf (s_keys,   9, "%2d %c%c"  , x_repeat, x_major, a_key);
      /*---(multiplier)------------------*/
      if (rc == 0 && x_repeat > 0 && yVIKEYS_mode_curr () != SMOD_REPEAT) {
         yVIKEYS_repeat_dec ();
         continue;
      }
      /*---(multiplier)------------------*/
      break;
      /*---(done)------------------------*/
   }
   /*---(setup for next keystroke)-------*/
   if      (rc == 0)    x_major = ' ';
   else if (rc >  0)    x_major = rc;
   else               { x_major = ' ';  x_error = 'y';  yVIKEYS_repeat_init (); }
   /*---(setup status line)--------------*/
   if        (yVIKEYS_mode_curr() == MODE_COMMAND) {
      yVIKEYS_mode_mesg (s_display, yVIKEYS_cmds_curr ());
   } else if (yVIKEYS_mode_curr() == MODE_SEARCH ) {
      yVIKEYS_mode_mesg (s_display, yVIKEYS_srch_curr ());
   } else if (x_savemode != yVIKEYS_mode_curr()) {
      yVIKEYS_mode_mesg (s_display, yVIKEYS_cmds_curr ());
   }
   yVIKEYS_view_text (s_display);
   yVIKEYS_view_keys (s_keys   );
   /*---(save current mode)--------------*/
   x_savemode = yVIKEYS_mode_curr ();
   /*---(advance macros)-----------------*/
   IF_MACRO_ON  yVIKEYS_macro_exec_adv ();
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}


