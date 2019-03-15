/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char    (*s_undo) (void);
static char    (*s_redo) (void);

char
yvikeys_hist_init       (void)
{
   s_undo         = NULL;
   s_redo         = NULL;
   STATUS_init_set   (UMOD_HISTORY);
   return 0;
}

char         /*-> initialize history environment -----[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_hist_config     (void *a_undo, void *a_redo)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (UMOD_HISTORY)) {
      DEBUG_EDIT   yLOG_note    ("init must complete before config");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save pointer)-------------------*/
   if (a_undo != NULL)  s_undo  = a_undo;
   if (a_redo != NULL)  s_redo  = a_redo;
   /*---(update status)------------------*/
   STATUS_conf_set   (UMOD_HISTORY, '1');
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist_undo       (void)
{
   if (s_undo != NULL)  return s_undo ();
   return -1;
}

char
yvikeys_hist_redo       (void)
{
   if (s_redo != NULL)  return s_redo ();
   return -1;
}

static int     s_min   = 0;
static int     s_top   = 0;
static int     s_now   = 0;
static int     s_max   = 0;
static int     s_lines = 0;

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
HISTORY_start           (void)
{
   yVIKEYS_view_size     (YVIKEYS_HISTORY, NULL, NULL, NULL, &s_lines, NULL);
   HISTORY_limits (MODE_curr (), &s_min, &s_max);
   s_now = s_max - 1;
   s_top = s_now - ((s_lines - 2) / 2);
   return 0;
}

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
HISTORY_infowin         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left      =    0;
   int         x_wide      =    0;
   int         x_bott      =    0;
   int         x_tall      =    0;
   char        x_edit      =  ' ';
   char        x_on        =  '-';
   char        x_entry     [LEN_RECD]  = "";
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_HISTORY, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      s_lines = x_tall - 2;
      yCOLOR_curs ("h_used" );
      /*> 1234567890123456789012345678901234::1234567890123456789012345678901234            <*/
      switch (MODE_curr ()) {
      case UMOD_MARK      : strlcpy (x_entry, " -  --label--- --x-- --y-- --z--      -  --label--- --x-- --y-- --z--   ", LEN_RECD);  break;
      case UMOD_VISUAL    : strlcpy (x_entry, " -  --label---  --label---            -  --label---  --label---         ", LEN_RECD);  break;
      case SMOD_MACRO     : strlcpy (x_entry, " - len ---macro-keys--------------------------------------------------- ", LEN_RECD);  break;
      case SMOD_SREG      : strlcpy (x_entry, " -   len  ---text-register-contents------------------------------------ ", LEN_RECD);  break;
      }
      mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, x_entry);
      attrset     (0);
      for (i = 0; i < x_tall - 2; ++i) {
         if ((i % 2) == 0)  yCOLOR_curs ("h_current"    );
         else               yCOLOR_curs ("map"          );
         switch (MODE_curr ()) {
         case UMOD_MARK      : yvikeys_mark_info    (x_entry, i);  break;
         case UMOD_VISUAL    : yvikeys_visu_info    (i, x_entry);  break;
                               /*> case SMOD_MACRO     : MACRO_infowin (x_entry, i);  break;                <*/
         case SMOD_SREG      : yvikeys_sreg_info    (i, x_entry);  break;
         }
         mvprintw (x_bott - x_tall + 2 + i, x_left             , "%-*.*s", x_wide, x_wide, x_entry);
         attrset     (0);
      }
      attrset     (0);
      yCOLOR_curs ("h_used" );
      mvprintw (x_bott, x_left             , "%-*.*s", x_wide, x_wide, " ¦ to choose, ¥ to escape ------------------------------------------------------------");
      attrset     (0);
   }
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
HISTORY_display         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left      =    0;
   int         x_wide      =    0;
   int         x_bott      =    0;
   int         x_tall      =    0;
   char        x_edit      =  ' ';
   char        x_on        =  '-';
   char        x_entry     [LEN_RECD]  = "";
   int         i           =    0;
   /*---(defense)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_char    ("info_win"  , myVIKEYS.info_win);
   if (myVIKEYS.info_win == 'y') {
      HISTORY_infowin ();
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_EDIT   yLOG_char    ("curr mode" , MODE_curr ());
   DEBUG_EDIT   yLOG_char    ("prev mode" , MODE_prev ());
   if (MODE_not (UMOD_HISTORY)) {
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_HISTORY, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      s_lines = x_tall - 2;
      yCOLOR_curs ("h_used" );
      if (MODE_prev () == MODE_SEARCH)  mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, " line  rn  cnt  m  ----------SEARCH HISTORY--------------------------------------------------------------------------");
      else                              mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, " line  rn  rc-  m  ----------COMMAND HISTORY-------------------------------------------------------------------------");
      attrset     (0);
      for (i = 0; i < x_tall - 2; ++i) {
         HISTORY_entry (MODE_prev (), s_top + i, x_entry, x_wide);
         if      (s_top + i == s_now)  yCOLOR_curs ("source"       );
         else if ((i % 2) == 0)        yCOLOR_curs ("h_current"    );
         else                          yCOLOR_curs ("map"          );
         mvprintw (x_bott - x_tall + 2 + i, x_left             , "%-*.*s", x_wide, x_wide, x_entry);
         attrset     (0);
      }
      yCOLOR_curs ("h_used" );
      mvprintw (x_bott, x_left             , "%-*.*s", x_wide, x_wide, " ¦ to choose, ¥ to leave, _KkjJ~ to move                                              ");
      attrset     (0);
   }
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
HISTORY_choose          (char* a_contents)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_len       =    0;
   char        x_old       [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_info    ("a_contents", a_contents);
   /*---(remove existing)----------------*/
   x_len  = yvikeys_src_npos ();
   strlcpy (x_old, yvikeys_src_contents (), LEN_RECD);
   DEBUG_EDIT   yLOG_value   ("x_len"     , x_len);
   DEBUG_EDIT   yLOG_info    ("x_old"     , x_old);
   yvikeys_sundo_beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      DEBUG_EDIT   yLOG_complex ("delete"    , "%2d, %c", i, x_old [i]);
      yvikeys_sundo_add ('d', 'l', 0, x_old [i], G_CHAR_NULL);
      yvikeys_src_one_delete ();
   }
   /*---(add new)------------------------*/
   x_len  = strllen (a_contents, LEN_RECD);
   for (i = 0; i < x_len; ++i) {
      DEBUG_EDIT   yLOG_complex ("append"    , "%2d, %c", i, a_contents [i]);
      yvikeys_src_one_append (a_contents [i]);
      yvikeys_sundo_add (G_KEY_SPACE, 'a', i, G_KEY_NULL, a_contents [i]);
   }
   yvikeys_sundo_end (__FUNCTION__);
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> allow selection of older entries ---[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
HISTORY_smode           (int  a_major, int  a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_HISTORY)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape, return to source mode");
      MODE_exit ();
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("return, return to source mode");
      MODE_exit ();
      yvikeys_src_swapall (HISTORY_use (MODE_curr (), s_now));
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   switch (a_minor) {
   case '_' : s_now  = 0;                 break;
   case 'J' : s_now += 5;                 break;
   case 'j' : s_now += 1;                 break;
   case 'k' : s_now -= 1;                 break;
   case 'K' : s_now -= 5;                 break;
   case '~' : s_now  = s_max - 1;         break;
   }
   DEBUG_USER   yLOG_value   ("s_now"     , s_now);
   if (s_now >= s_max) {
      s_now = s_max - 1;
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, -1);
      return -1;
   }
   if (s_now <  0    ) {
      s_now = 0; 
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, -1);
      return -1;
   }
   s_top = s_now - (s_lines / 2);
   DEBUG_USER   yLOG_value   ("s_top"     , s_top);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

