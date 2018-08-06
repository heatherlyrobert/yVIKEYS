/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



static char  s_keys_log   [10000];
static char  s_keys_multi [10000];
static char  s_keys_mode  [10000];
static char  s_keys_error [10000];
static int   s_nkey      = 0;




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
   /*----(first)-------------------------*/
   STATUS_init  ();
   srand (time (NULL));
   /*----(early)-------------------------*/
   MODE_init    ();
   FILE_init    ();
   /*----(middling)----------------------*/
   CMDS_init    ();
   SRCH_init    ();
   /*----(later)-------------------------*/
   KEYS_init    ();
   MAP_init     ();
   SOURCE_init  ();
   MACRO_init   ();
   /*----(latest)------------------------*/
   yvikeys_regs_init    ();
   SRC_REG_init ();
   MARK_init    ();
   VISU_init    ();
   REPEAT_init  ();
   yvikeys_bufs_init    ();
   yvikeys_hist_init    ();
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
   VIEW_wrap   ();
   STATUS_wrap ();
   yvikeys_regs_wrap   ();
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
   yURG_name  ("edit"         , YURG_ON);
   yURG_name  ("mark"         , YURG_ON);
   yURG_name  ("mode"         , YURG_ON);
   yURG_name  ("map"          , YURG_ON);
   yURG_name  ("map_mas"      , YURG_ON);
   yURG_name  ("srch"         , YURG_ON);
   yURG_name  ("hist"         , YURG_ON);
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
   x_start = x_len - (20 * 5);
   if (x_start < 0) x_start = 0;
   snprintf (a_msg, 500, "keys (%d) %-100s", s_nkey, s_keys_log + x_start);
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
   /*---(macro)--------------------------*/
   IF_MACRO_RECORDING {
      MACRO_rec_key (x_key);
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
   /*---(basic defense)------------------*/
   if (s_nkey < 2)                               return 1;
   /*---(prepare)------------------------*/
   c3 = s_keys_log   [s_nkey - 2];
   m3 = s_keys_multi [s_nkey - 2];
   c4 = s_keys_log   [s_nkey - 1];
   m4 = s_keys_multi [s_nkey - 1];
   /*---(filter)-------------------------*/
   if (strchr ("ps-", m3) == NULL)               return 1;
   if (strchr ("ps-", m4) == NULL)               return 1;
   if (m3 == 's' && m4 == '-')                   return 1;
   if (m4 == 'p')                                return 1;
   if (c3 == G_CHAR_SPACE)                       return 1;
   if (c4 == G_CHAR_SPACE)                       return 1;
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
   return 1;
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
   strlcpy (s_keys_log, "", LEN_RECD);
   s_nkey = 0;
   return 0;
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
   DEBUG_LOOP   yLOG_char    ("macromode" , MACRO_get_mode ());
   /*---(fixes)--------------------------*/
   if (a_key == G_KEY_ENTER)  a_key = G_KEY_RETURN;
   if (a_key == G_KEY_DEL  )  a_key = G_KEY_BS;     /* X11 sends incorrectly  */
   /*---(normal)-------------------------*/
   IF_MACRO_NOT_PLAYING {
      DEBUG_LOOP   yLOG_note    ("normal or macro recording");
      if (MACRO_count ()) {
         DEBUG_USER   yLOG_note    ("but, in macro repeat mode");
         MACRO_exec_beg ('@');
         x_ch  = G_KEY_SPACE;
      } else {
         x_ch  = a_key;
         if (x_ch == 0) {
            DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
            return x_ch;
         }
      }
   }
   /*---(run, delay, or playback)--------*/
   else IF_MACRO_PLAYING {
      DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");
      x_ch = MACRO_exec_key ();
      IF_MACRO_OFF {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         /*> return -1;                                                               <*/
         return x_ch;
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
   KEYS__logger (x_ch);
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
   /*---(prepare)------------------------*/
   myVIKEYS.trouble   = '-';
   x_key = chrworking (a_key);
   /*---(handle count)-------------------*/
   if (MODE_curr () == UMOD_REPEAT) {
      rc = REPEAT_umode (x_major, x_key);
      if (rc >  0)  x_major = G_KEY_SPACE;
   }
   /*---(main loop)----------------------*/
   while (1) {
      /*---(handle keystroke)------------*/
      switch (MODE_curr ()) {
      case MODE_GOD      : rc = BASE__________stub    (x_major , x_key);  break;
      case MODE_MAP      : rc = MAP_mode              (x_major , x_key);  break;
      case MODE_SOURCE   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_SRC_INPT : rc = SRC_INPT_umode           (x_major , x_key);  break;
      case SMOD_SRC_REG  : rc = SRC_REG_smode         (x_major , x_key);  break;
      case UMOD_SRC_REPL : rc = SRC_REPL_umode        (x_major , x_key);  break;
      case UMOD_SRC_UNDO : rc = BASE__________stub    (x_major , x_key);  break;
      case UMOD_HISTORY  : rc = HISTORY_smode         (x_major , x_key);  break;
      case MODE_COMMAND  : rc = SOURCE_mode           (x_major , x_key);  break;
      case MODE_SEARCH   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_VISUAL   : rc = VISU_smode            (x_major , x_key);  break;
      case SMOD_ERROR    : rc = BASE__________stub    (x_major , x_key);  break;
      case XMOD_FORMAT   : rc = FORMAT_smode          (x_major , x_key);  break;
      case SMOD_BUFFER   : rc = yvikeys_bufs_umode    (x_major , x_key);  break;
      case UMOD_WANDER   : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_MAP_REG  : rc = yvikeys_regs_smode         (x_major , x_key);  break;
      case UMOD_MARK     : rc = MARK_smode            (x_major , x_key);  break;
      case SMOD_MENUS    : rc = BASE__________stub    (x_major , x_key);  break;
      case SMOD_MACRO    : rc = MACRO_smode           (x_major , x_key);  break;
      case UMOD_REPEAT   :                                                break;
      default            : rc = -1;  x_nomode = 'y';                      break;
      }
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      /*---(translate unprintable)-------*/
      x_repeat = REPEAT_count ();
      /*> snprintf (x_keys,   9, "%2d %c%c"  , x_repeat, x_major, chrvisible (x_key));   <*/
      snprintf (x_keys,   9, "%2x %c%c"  , x_key, x_major, chrvisible (x_key));
      /*---(loop repeats)----------------*/
      if (rc == 0 && x_repeat > 0 && MODE_curr () != UMOD_REPEAT) {
         REPEAT_decrement ();
         continue;
      }
      /*---(loop repeats)----------------*/
      if (rc <= 0 && MODE_curr () != UMOD_REPEAT) {
         REPEAT_done ();
      }
      break;
      /*---(done)------------------------*/
   }
   /*---(setup for next keystroke)-------*/
   if      (rc == 0)    x_major = G_KEY_SPACE;
   else if (rc >  0)    x_major = rc;
   else               { x_major = G_KEY_SPACE;  KEYS__error ();  myVIKEYS.trouble = 'y';  REPEAT_reset (); }
   yVIKEYS_view_text (YVIKEYS_KEYS   , x_keys   );
   /*---(save current mode)--------------*/
   x_savemode = MODE_curr ();
   /*---(advance macros)-----------------*/
   IF_MACRO_ON  MACRO_exec_adv ();
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
   int         x_len       =    0;
   uchar       x_ch        =  ' ';     /* current keystroke                   */
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
      DEBUG_LOOP   yLOG_value   ("a_keys[i]" , a_keys[i]);
      DEBUG_LOOP   yLOG_char    ("a_keys[i]" , a_keys[i]);
      x_ch = chrworking (a_keys [i]);
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
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

char         /*-> tbd --------------------------------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main       (float a_delay)
{
   /*---(locals)-----------+-----------+-*/
   int         x_loop      = 0;
   int         x_ch        = ' ';      /* current keystroke                   */
   uchar       x_key       = ' ';      /* current keystroke                   */
   char        rc          = 0;
   tTSPEC      x_dur;
   /*---(for timer)------------------------*/
   x_dur.tv_sec    = 0;
   x_dur.tv_nsec   = a_delay * 1000000;
   /*---(main-loop)----------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   while (1) {
      x_ch = getch ();
      if (x_ch == KEY_RESIZE)  yVIKEYS_view_resize (0, 0, 0);
      if (x_ch < 0)  x_key = 0;
      else           x_key = x_ch;
      DEBUG_GRAF  yLOG_value   ("x_key"     , x_key);
      x_key = yVIKEYS_main_input  (RUN_USER, x_key);
      yVIKEYS_main_handle (x_key);
      if (yVIKEYS_quit ())  break;
      ++x_loop;
      if ((x_loop % 20) == 0)  yVIKEYS_view_all (0.0);
      /*---(sleeping)--------------------*/
      nanosleep    (&x_dur, NULL);
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
   strlcpy  (yVIKEYS__unit_answer, "KEYS unit        : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "log"            )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "KEYS log         : %2d[%-.40s]", s_nkey, s_keys_log);
   }
   else if (strcmp (a_question, "mode"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "KEYS mode        : %2d[%-.40s]", s_nkey, s_keys_mode);
   }
   else if (strcmp (a_question, "multi"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "KEYS multi       : %2d[%-.40s]", s_nkey, s_keys_multi);
   }
   else if (strcmp (a_question, "error"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "KEYS error       : %2d[%-.40s]", s_nkey, s_keys_error);
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      KEYS_status (t);
      strltrim (t, ySTR_BOTH, LEN_RECD);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "%-.60s", t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


