/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char    (*s_undo) (void);
static char    (*s_redo) (void);



/*===[[ HISTORY ]]============================================================*/
#define   MAX_PASS     1000
typedef struct cHIST     tHIST;
struct cHIST {
   uchar       mark;
   uchar      *text;
   int         count;
   int         found;
   tHIST      *m_next;
   tHIST      *m_prev;
};
/*---(command history)---------*/
static tHIST    *s_hrun      = NULL;    /* head               */
static tHIST    *s_trun      = NULL;    /* tail               */
static tHIST    *s_crun      = NULL;    /* current            */
static int       s_irun      = 0;       /* index              */
static int       s_nrun      = 0;       /* count              */
/*---(search history)----------*/
static tHIST    *s_hpass     = NULL;    /* head               */
static tHIST    *s_tpass     = NULL;    /* tail               */
static tHIST    *s_cpass     = NULL;    /* current            */
static int       s_ipass     = 0;       /* index              */
static int       s_npass     = 0;       /* count              */
/*---(switchable)--------------*/
static tHIST   **s_head      = &s_hrun; /* head               */
static tHIST   **s_tail      = &s_trun; /* tail               */
static tHIST   **s_curr      = &s_crun; /* current            */
static int      *s_index     = &s_irun; /* index              */
static int      *s_count     = &s_nrun; /* count              */
static int       s_nall      = 0;       /* full count         */



#define      S_HIST_MAX    100
static char  S_HIST_LIST [S_HIST_MAX];




/*====================------------------------------------====================*/
/*===----                      support functions                       ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char
yvikeys_hist__switcher  (char a_mode, char a_force)
{
   /*---(locals)-----------+-----+-----+-*/
   static char x_save      =  '-';
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_mode"    , a_mode);
   /*---(defense)------------------------*/
   if (a_force != 'y' && a_mode == x_save) {
      DEBUG_HIST   yLOG_exit    ("same as last, short-cut");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(switch)-------------------------*/
   /*> DEBUG_HIST   yLOG_complex ("&command"  , "%10ph %10pt %10pc %10pn", &s_hrun , &s_trun , &s_crun , &s_nrun );   <*/
   /*> DEBUG_HIST   yLOG_complex ("command"   , "%10ph %10pt %10pc %10dn",  s_hrun ,  s_trun ,  s_crun ,  s_nrun );   <*/
   /*> DEBUG_HIST   yLOG_complex ("&search"   , "%10ph %10pt %10pc %10pn", &s_hpass, &s_tpass, &s_cpass, &s_npass);   <*/
   /*> DEBUG_HIST   yLOG_complex ("search"    , "%10ph %10pt %10pc %10dn",  s_hpass,  s_tpass,  s_cpass,  s_npass);   <*/
   switch (a_mode) {
   case MODE_SEARCH  :
      DEBUG_HIST   yLOG_note    ("switching to search");
      s_head  = &s_hpass;
      s_tail  = &s_tpass;
      s_curr  = &s_cpass;
      s_index = &s_ipass;
      s_count = &s_npass;
      break;
   case MODE_COMMAND :
   default :
      DEBUG_HIST   yLOG_note    ("switching to command");
      s_head  = &s_hrun;
      s_tail  = &s_trun;
      s_curr  = &s_crun;
      s_index = &s_irun;
      s_count = &s_nrun;
      break;
   }
   /*> DEBUG_HIST   yLOG_complex ("&current"  , "%10ph %10pt %10pc %10pn", &s_head , &s_tail , &s_curr , &s_count);   <*/
   /*> DEBUG_HIST   yLOG_complex ("current"   , "%10ph %10pt %10pc %10pn",  s_head ,  s_tail ,  s_curr ,  s_count);   <*/
   /*> DEBUG_HIST   yLOG_complex ("*current"  , "%10ph %10pt %10pc %10dn", *s_head , *s_tail , *s_curr , *s_count);   <*/
   /*---(save)---------------------------*/
   x_save = a_mode;
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist_limits     (char a_mode, int *a_min, int *a_max)
{
   yvikeys_hist__switcher (a_mode, '-');
   if (a_min != NULL)  *a_min = 0;
   if (a_max != NULL)  *a_max = *s_count;
   return 0;
}

char 
yvikeys_cmds__valid     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_HIST_LIST, a_abbr, S_HIST_MAX);
   DEBUG_HIST   yLOG_sint    (rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char yvikeys_srch__valid  (char a_abbr) { return yvikeys_cmds__valid (a_abbr); }



/*====================------------------------------------====================*/
/*===----                    memory allocation                         ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

/*> int                                                                               <* 
 *> yvikeys_srch__index     (char a_abbr)                                             <* 
 *> {                                                                                 <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                       <* 
 *>    char        rce         =  -10;                                                <* 
 *>    char        rc          =    0;                                                <* 
 *>    int         i           =    0;                                                <* 
 *>    int         n           =   -1;                                                <* 
 *>    /+---(header)-------------------------+/                                       <* 
 *>    DEBUG_HIST   yLOG_senter  (__FUNCTION__);                                      <* 
 *>    /+---(check)--------------------------+/                                       <* 
 *>    DEBUG_HIST   yLOG_snote   ("check");                                           <* 
 *>    rc = strlchr (S_HIST_LIST, a_abbr, S_HIST_MAX);                                <* 
 *>    DEBUG_HIST   yLOG_sint    (rc);                                                <* 
 *>    --rce;  if (rc < 0) {                                                          <* 
 *>       DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    /+---(search)-------------------------+/                                       <* 
 *>    DEBUG_HIST   yLOG_snote   ("search");                                          <* 
 *>    for (i = 0; i < s_npass; ++i) {                                                <* 
 *>       if (s_passes [i].mark != a_abbr)  continue;                                 <* 
 *>       n = i;                                                                      <* 
 *>       break;                                                                      <* 
 *>    }                                                                              <* 
 *>    DEBUG_HIST   yLOG_sint    (n);                                                 <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    DEBUG_HIST   yLOG_sexit   (__FUNCTION__);                                      <* 
 *>    return n;                                                                      <* 
 *> }                                                                                 <*/

char 
yvikeys_hist__new       (char a_mode, uchar *a_text)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tHIST      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST   yLOG_char    ("a_mode"    , a_mode);
   rc = yvikeys_hist__switcher (a_mode, '-');
   DEBUG_HIST   yLOG_value   ("switcher"  , rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_info    ("a_text"    , a_text);
   x_len = strlen (a_text);
   DEBUG_HIST   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 1) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tHIST *) malloc (sizeof (tHIST));
   }
   DEBUG_HIST   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_HIST   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_HIST   yLOG_note    ("FAILED");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_HIST   yLOG_note    ("populate");
   x_new->mark     = '-';
   x_new->text     = strdup (a_text);
   x_new->count    = 0;
   x_new->found    = 0;
   x_new->m_next   = NULL;
   x_new->m_prev   = NULL;
   /*---(tie to master list)-------------*/
   if (*s_head == NULL) {
      DEBUG_HIST   yLOG_note    ("nothing in list, make first");
      *s_head           = x_new;
   } else  {
      DEBUG_HIST   yLOG_note    ("append to list");
      (*s_tail)->m_next = x_new;
      x_new->m_prev     = *s_tail;
   }
   *s_tail        = x_new;
   /*---(update counts)------------------*/
   ++(*s_count);
   ++s_nall;
   DEBUG_HIST   yLOG_value   ("s_count"   , *s_count);
   DEBUG_HIST   yLOG_value   ("s_nall"    , s_nall);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   /*> printf ("done\n");                                                             <*/
   return 0;
}

char
yvikeys_hist__purge     (char a_mode)
{
   /*---(locals)-----------+-----+-----+-*/
   tHIST      *x_curr      = NULL;
   tHIST      *x_next      = NULL;
   /*---(set pointer)--------------------*/
   yvikeys_hist__switcher (a_mode, '-');
   /*---(clear history)------------------*/
   x_curr = *s_head;
   while (x_curr != NULL) {
      /*--(prepare)------------*/
      x_next = x_curr->m_next;
      /*--(ground/careful)-----*/
      x_curr->m_prev = NULL;
      x_curr->m_next = NULL;
      /*--(free memory)--------*/
      free (x_curr->text);
      free (x_curr);
      /*--(next)---------------*/
      x_curr = x_next;
      /*--(done)---------------*/
   }
   /*---(initialize pointers)------------*/
   *s_head  = NULL;
   *s_tail  = NULL;
   *s_curr  = NULL;
   *s_count = 0;
   s_nall   = s_nrun + s_npass;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      program level                           ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_hist_init       (void)
{
   /*---(ground pointers)----------------*/
   s_undo         = NULL;
   s_redo         = NULL;
   /*---(macro abbrev list)--------------*/
   DEBUG_HIST   yLOG_note    ("initialize hist list");
   strlcpy (S_HIST_LIST, ""            , S_HIST_MAX);
   strlcat (S_HIST_LIST, gvikeys_lower , S_HIST_MAX);
   strlcat (S_HIST_LIST, gvikeys_upper , S_HIST_MAX);
   strlcat (S_HIST_LIST, gvikeys_greek , S_HIST_MAX);
   DEBUG_HIST   yLOG_info    ("LIST"      , S_HIST_LIST);
   /*---(clear history)------------------*/
   DEBUG_HIST   yLOG_note    ("clear all history");
   /*---(initialize)--------------*/
   yvikeys_hist__purge (MODE_COMMAND);
   yvikeys_hist__purge (MODE_SEARCH);
   s_nall      = 0;       /* full count         */
   /*---(switchable)--------------*/
   yvikeys_hist__switcher (MODE_COMMAND, 'y');
   HISTORY__load  (MODE_COMMAND, NULL);
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_HISTORY);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> initialize history environment -----[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_hist_config     (void *a_undo, void *a_redo)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (UMOD_HISTORY)) {
      DEBUG_HIST   yLOG_note    ("init must complete before config");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save pointer)-------------------*/
   if (a_undo != NULL)  s_undo  = a_undo;
   if (a_redo != NULL)  s_redo  = a_redo;
   /*---(update status)------------------*/
   STATUS_conf_set   (UMOD_HISTORY, '1');
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist_wrap       (void)
{
   yvikeys_hist__purge    (MODE_COMMAND);
   yvikeys_hist__purge    (MODE_SEARCH);
   yvikeys_hist__switcher (MODE_COMMAND, 'y');
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     future expansion                         ----===*/
/*====================------------------------------------====================*/
static void  o___FUTURE__________o () { return; }

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
   yvikeys_hist_limits   (MODE_curr (), &s_min, &s_max);
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
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
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
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
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
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("info_win"  , myVIKEYS.info_win);
   if (myVIKEYS.info_win == 'y') {
      HISTORY_infowin ();
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_HIST   yLOG_char    ("curr mode" , MODE_curr ());
   DEBUG_HIST   yLOG_char    ("prev mode" , MODE_prev ());
   if (MODE_not (UMOD_HISTORY)) {
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_HISTORY, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      s_lines = x_tall - 2;
      yCOLOR_curs ("h_used" );
      if (MODE_prev () == MODE_SEARCH)  mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, " line  rn  cnt  m  ----------SEARCH HISTORY--------------------------------------------------------------------------");
      else                              mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, " line  rn  rc-  m  ----------COMMAND HISTORY-------------------------------------------------------------------------");
      attrset     (0);
      for (i = 0; i < x_tall - 2; ++i) {
         yvikeys_hist_entry (MODE_prev (), s_top + i, x_entry, x_wide);
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
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
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
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_info    ("a_contents", a_contents);
   /*---(remove existing)----------------*/
   x_len  = yvikeys_src_npos ();
   strlcpy (x_old, yvikeys_src_contents (), LEN_RECD);
   DEBUG_HIST   yLOG_value   ("x_len"     , x_len);
   DEBUG_HIST   yLOG_info    ("x_old"     , x_old);
   yvikeys_sundo_beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      DEBUG_HIST   yLOG_complex ("delete"    , "%2d, %c", i, x_old [i]);
      yvikeys_sundo_add ('d', 'l', 0, x_old [i], G_CHAR_NULL);
      yvikeys_src_one_delete ();
   }
   /*---(add new)------------------------*/
   x_len  = strllen (a_contents, LEN_RECD);
   for (i = 0; i < x_len; ++i) {
      DEBUG_HIST   yLOG_complex ("append"    , "%2d, %c", i, a_contents [i]);
      yvikeys_src_one_append (a_contents [i]);
      yvikeys_sundo_add (G_KEY_SPACE, 'a', i, G_KEY_NULL, a_contents [i]);
   }
   yvikeys_sundo_end (__FUNCTION__);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
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

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_cmds__writer    (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   char        x_cmd       [LEN_RECD ];
   tHIST      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   x_curr = s_hrun;
   while (x_curr != NULL) {
      if (x_curr->mark == a_abbr)  break;
      x_curr = x_curr->m_next;
   }
   DEBUG_OUTP   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr == NULL) { 
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write)-----------------------*/
   strlcpy  (x_cmd, x_curr->text, LEN_RECD);
   yPARSE_fullwrite ("command", x_curr->mark, x_cmd);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_cmds__writer_all     (void)
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
   --rce;  if (!STATUS_operational (MODE_COMMAND)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_end = strlen (S_HIST_LIST);
   yPARSE_verb_begin ("command");
   /*---(walk list)----------------------*/
   for (i = 0; i < x_end; ++i) {
      rc = yvikeys_cmds__writer (S_HIST_LIST [i]);
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

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_srch__writer    (char a_abbr)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rce         =  -10;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> char        n           =    0;                                                <* 
    *> char        x_srch       [LEN_RECD ];                                          <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> --rce;  if (!STATUS_operational (MODE_SEARCH)) {                               <* 
    *>    DEBUG_OUTP   yLOG_note    ("can not execute until operational");            <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(clear output)-------------------+/                                       <* 
    *> yPARSE_outclear  ();                                                           <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);                              <* 
    *> n  = yvikeys_srch__index (a_abbr);                                             <* 
    *> DEBUG_OUTP   yLOG_char    ("index"     , n);                                   <* 
    *> if (n == -1) {                                                                 <* 
    *>    DEBUG_OUTP   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> --rce; if (n  < 0) {                                                           <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(filter)-------------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_char    ("mark"      , s_passes [n].mark);                   <* 
    *> if (s_passes [n].mark == '-') {                                                <* 
    *>    DEBUG_OUTP   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(write)-----------------------+/                                          <* 
    *> strlcpy  (x_srch, s_passes [n].text, LEN_RECD);                                <* 
    *> yPARSE_fullwrite ("search", s_passes [n].mark, x_srch);                        <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_OUTP  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return 1;                                                                      <*/
}

char
yvikeys_srch__writer_all     (void)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> char        rce         =  -10;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> char        x_end       =    0;                                                <* 
    *> int         i           =    0;                                                <* 
    *> char        c           =    0;                                                <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_OUTP   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> --rce;  if (!STATUS_operational (MODE_SEARCH)) {                               <* 
    *>    DEBUG_OUTP   yLOG_note    ("can not execute until operational");            <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(prepare)------------------------+/                                       <* 
    *> x_end = strlen (S_HIST_LIST);                                                  <* 
    *> yPARSE_verb_begin ("search");                                                  <* 
    *> /+---(walk list)----------------------+/                                       <* 
    *> for (i = 0; i <= x_end; ++i) {                                                 <* 
    *>    rc = yvikeys_srch__writer (S_HIST_LIST [i]);                                <* 
    *>    if (rc < 1)  continue;                                                      <* 
    *>    ++c;                                                                        <* 
    *>    yPARSE_verb_break (c);                                                      <* 
    *> }                                                                              <* 
    *> /+---(wrap-up)------------------------+/                                       <* 
    *> yPARSE_verb_end   (c);                                                         <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_OUTP  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return c;                                                                      <*/
}

char
yvikeys_srch__reader         (void)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rce         =  -11;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> char        x_verb      [LEN_LABEL];                                           <* 
    *> char        x_abbr      =    0;                                                <* 
    *> char        x_srch       [LEN_RECD ];                                          <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_INPT   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> --rce;  if (!STATUS_operational (MODE_SEARCH)) {                               <* 
    *>    DEBUG_INPT   yLOG_note    ("can not execute until operational");            <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(get verb)-----------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_verb);                                                   <* 
    *> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <* 
    *> DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);                              <* 
    *> --rce;  if (strcmp ("search", x_verb) != 0) {                                  <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(label)--------------------------+/                                       <* 
    *> rc = yPARSE_popchar (&x_abbr);                                                 <* 
    *> DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);                                  <* 
    *> DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);                              <* 
    *> rc  = yvikeys_srch__valid (x_abbr);                                            <* 
    *> DEBUG_INPT   yLOG_value   ("index"     , rc);                                  <* 
    *> --rce; if (rc  < 0) {                                                          <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(get command)--------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_srch);                                                   <* 
    *> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <* 
    *> strldchg (x_srch, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                      <* 
    *> DEBUG_INPT   yLOG_info    ("x_srch"    , x_srch);                              <* 
    *> /+---(save)---------------------------+/                                       <* 
    *> DEBUG_SRCH   yLOG_note    ("saving values");                                   <* 
    *> strlcpy (s_passes [s_npass].text, x_srch, LEN_RECD);                           <* 
    *> s_passes [s_npass].mark  = x_abbr;                                             <* 
    *> ++s_npass;                                                                     <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return 1;                                                                      <*/
}

char
yvikeys_cmds__reader (void)
{
   /*> /+---(locals)-----------+-----------+-+/                                       <* 
    *> char        rce         =  -11;                                                <* 
    *> char        rc          =    0;                                                <* 
    *> int         n           =    0;                                                <* 
    *> char        x_verb      [LEN_LABEL];                                           <* 
    *> char        x_abbr      =    0;                                                <* 
    *> char        x_cmd       [LEN_RECD ];                                           <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_INPT   yLOG_enter   (__FUNCTION__);                                      <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> --rce;  if (!STATUS_operational (MODE_COMMAND)) {                              <* 
    *>    DEBUG_INPT   yLOG_note    ("can not execute until operational");            <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(get verb)-----------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_verb);                                                   <* 
    *> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <* 
    *> DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);                              <* 
    *> --rce;  if (strcmp ("command", x_verb) != 0) {                                 <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(label)--------------------------+/                                       <* 
    *> rc = yPARSE_popchar (&x_abbr);                                                 <* 
    *> DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);                                  <* 
    *> DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);                              <* 
    *> n  = yvikeys_cmds__valid (x_abbr);                                             <* 
    *> DEBUG_INPT   yLOG_char    ("strlchr"   , n);                                   <* 
    *> --rce; if (n   < 0) {                                                          <* 
    *>    DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(get command)--------------------+/                                       <* 
    *> rc = yPARSE_popstr (x_cmd);                                                    <* 
    *> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <* 
    *> strldchg (x_cmd, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                       <* 
    *> DEBUG_INPT   yLOG_info    ("x_cmd"     , x_cmd);                               <* 
    *> /+---(save)---------------------------+/                                       <* 
    *> DEBUG_SRCH   yLOG_note    ("saving values");                                   <* 
    *> strlcpy (s_runs [s_nrun].text, x_cmd, LEN_RECD);                               <* 
    *> s_runs [s_nrun].mark  = x_abbr;                                                <* 
    *> ++s_nrun;                                                                      <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                       <* 
    *> return 1;                                                                      <*/
}




/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char
yvikeys_hist_entry      (char a_mode, int a_index, char *a_entry, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tHIST      *x_pass      = NULL;
   char        x_len       =    0;
   int         x_found     =    0;
   int         x_count     =    0;
   int         x_max       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)  return rce;
   /*---(set limits)---------------------*/
   yvikeys_hist__switcher (a_mode, '-');
   yvikeys_hist_limits   (a_mode, NULL, &s_max);
   /*---(blank line)---------------------*/
   if (a_index < 0 || a_index >= x_max) {
      sprintf (a_entry, "%-*.*s", a_max, a_max, " ");
      return 0;
   }
   /*---(create)-------------------------*/
   /*> x_len = a_max - 8;                                                             <* 
    *> x_found = (x_pass + a_index)->found;                                           <* 
    *> if (x_found > 999)  x_found = 999;                                             <* 
    *> x_count = (x_pass + a_index)->count;                                           <* 
    *> if (x_count > 99)  x_count = 99;                                               <* 
    *> sprintf (a_entry, " %-4d  %-2d  %-3d  %c  %-*.*s ",                            <* 
    *>       a_index, x_count, x_found, (x_pass + a_index)->mark,                     <* 
    *>       x_len, x_len, (x_pass + a_index)->text);                                 <*/
   /*---(complete)-----------------------*/
   return 0;
}

char*
HISTORY_use             (char a_mode, int a_index)
{
   /*> switch (a_mode) {                                                              <* 
    *> case MODE_COMMAND :                                                            <* 
    *>    return s_runs [a_index].text;                                               <* 
    *>    break;                                                                      <* 
    *> case MODE_SEARCH  :                                                            <* 
    *>    return s_passes [a_index].text;                                             <* 
    *>    break;                                                                      <* 
    *> }                                                                              <*/
}

char
yvikeys_hist__cursor    (char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_move"    , a_move);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_point   ("*s_head"   , *s_head);
   --rce;  if (*s_head == NULL) {
      *s_curr  = NULL;
      *s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   if (*s_curr == NULL) {
      *s_curr  = *s_head;
      *s_index = 0;
      DEBUG_HIST   yLOG_point   ("fixed"     , *s_curr);
   }
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(handle move)--------------------*/
   DEBUG_HIST   yLOG_note    ("check special shortcuts");
   switch (a_move) {
   case '[' :
      *s_curr  = *s_head;
      *s_index = 0;
      break;
   case '<' :
      *s_curr  = (*s_curr)->m_prev;
      --(*s_index);
      break;
   case '>' :
      *s_curr  = (*s_curr)->m_next;
      ++(*s_index);
      break;
   case ']' :
      *s_curr  = *s_tail;
      *s_index = *s_count - 1;
      break;
   }
   /*---(safeties)-----------------------*/
   if (*s_curr == NULL) {
      switch (a_move) {
      case '<' :
         *s_curr  = *s_head;
         *s_index = 0;
         break;
      case '>' :
         *s_curr  = *s_tail;
         *s_index = *s_count - 1;
         break;
      }
   }
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__index     (int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tHIST      *x_curr      = NULL;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_point   ("*s_head"   , *s_head);
   --rce;  if (*s_head == NULL) {
      *s_curr  = NULL;
      *s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(short-cuts)---------------------*/
   DEBUG_HIST   yLOG_value   ("a_index"   , a_index);
   if (a_index <  0) {
      DEBUG_HIST   yLOG_note    ("less than min (neg) is short-cut to head");
      *s_curr  = *s_head;
      *s_index = 0;
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   if (a_index >= *s_count) {
      DEBUG_HIST   yLOG_note    ("greater than max is short-cut to tail");
      *s_curr  = *s_tail;
      *s_index = *s_count - 1;
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(walk-entries)-------------------*/
   x_curr = *s_head;
   while (x_curr != NULL) {
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);
      if (c == a_index) break;
      x_curr = x_curr->m_next;
      ++c;
   }
   /*---(check result)-------------------*/
   DEBUG_HIST   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set current)--------------------*/
   *s_curr  = x_curr;
   *s_index = a_index;
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__roll      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (*s_head == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (*s_curr == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(pull out of list)---------------*/
   if ((*s_curr)->m_prev != NULL)  (*s_curr)->m_prev->m_next = (*s_curr)->m_next;
   if ((*s_curr)->m_next != NULL)  (*s_curr)->m_next->m_prev = (*s_curr)->m_prev;
   /*---(fix current)--------------------*/
   if (*s_head == *s_curr)    *s_head = (*s_curr)->m_next;
   (*s_curr)->m_next  = NULL;
   (*s_curr)->m_prev  = *s_tail;
   /*---(append to end)------------------*/
   (*s_tail)->m_next  = *s_curr;
   *s_tail = *s_curr;
   /*---(update current)-----------------*/
   *s_curr  = *s_tail;
   *s_index = *s_count - 1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      history handling                       ----===*/
/*====================------------------------------------====================*/
static void  o___HISTORY_________o () { return; }

char
HISTORY__unmark      (char a_type, char a_mark)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> tHIST      *x_pass      = NULL;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> /+---(set limits)---------------------+/                                       <* 
    *> switch (a_type) {                                                              <* 
    *> case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;               <* 
    *> case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;               <* 
    *> }                                                                              <* 
    *> /+---(clear old mark)-----------------+/                                       <* 
    *> for (i = 0; i < x_max; ++i) {                                                  <* 
    *>    if ((x_pass + i)->mark != a_mark)  continue;                                <* 
    *>    (x_pass + i)->mark = '-';                                                   <* 
    *> }                                                                              <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> return 0;                                                                      <*/
}

char
HISTORY__mark        (char a_type, char a_mark)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> tHIST      *x_pass      = NULL;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> /+---(set limits)---------------------+/                                       <* 
    *> switch (a_type) {                                                              <* 
    *> case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;               <* 
    *> case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;               <* 
    *> }                                                                              <* 
    *> /+---(clear old mark)-----------------+/                                       <* 
    *> for (i = 0; i < x_max; ++i) {                                                  <* 
    *>    if ((x_pass + i)->mark != a_mark)  continue;                                <* 
    *>    (x_pass + i)->mark = '-';                                                   <* 
    *> }                                                                              <* 
    *> /+---(clear new mark)-----------------+/                                       <* 
    *> (x_pass + x_max - 1)->mark  = a_mark;                                          <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> return 0;                                                                      <*/
}

char
HISTORY__return      (char a_mode, char a_mark)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> tHIST      *x_pass      = NULL;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         n           =   -1;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> /+---(set limits)---------------------+/                                       <* 
    *> switch (a_mode) {                                                              <* 
    *> case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;               <* 
    *> case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;               <* 
    *> }                                                                              <* 
    *> /+---(clear old mark)-----------------+/                                       <* 
    *> for (i = 0; i < x_max; ++i) {                                                  <* 
    *>    if ((x_pass + i)->mark != a_mark)  continue;                                <* 
    *>    n = i;                                                                      <* 
    *>    break;                                                                      <* 
    *> }                                                                              <* 
    *> if (n < 0)  return -1;                                                         <* 
    *> /+---(set global)---------------------+/                                       <* 
    *> yvikeys_hist__roll (a_mode, n);                                                     <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> return 0;                                                                      <*/
}

int
HISTORY__find         (char a_mode, char *a_text)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> tHIST      *x_pass      = NULL;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         n           =   -1;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> /+---(set limits)---------------------+/                                       <* 
    *> switch (a_mode) {                                                              <* 
    *> case MODE_COMMAND :  x_pass = &s_runs;   x_max = s_nrun;  break;               <* 
    *> case MODE_SEARCH  :  x_pass = &s_passes; x_max = s_npass; break;               <* 
    *> }                                                                              <* 
    *> /+---(look for duplicate)-------------+/                                       <* 
    *> for (i = 0; i < x_max; ++i) {                                                  <* 
    *>    if ((x_pass + i)->text [0] != a_text [0])      continue;                    <* 
    *>    if ((x_pass + i)->text [1] != a_text [1])      continue;                    <* 
    *>    if ((x_pass + i)->text [2] != a_text [2])      continue;                    <* 
    *>    if (strcmp ((x_pass + i)->text, a_text) != 0)  continue;                    <* 
    *>    n = i;                                                                      <* 
    *>    break;                                                                      <* 
    *> }                                                                              <* 
    *> /+---(set global)---------------------+/                                       <* 
    *> if (n >= 0)  {                                                                 <* 
    *>    yvikeys_hist__roll (a_mode, n);                                                  <* 
    *> } else {                                                                       <* 
    *>    strlcpy ((x_pass + x_max)->text, a_text, LEN_RECD);                         <* 
    *>    switch (a_mode) {                                                           <* 
    *>    case MODE_COMMAND :  x_max = ++s_nrun;  break;                              <* 
    *>    case MODE_SEARCH  :  x_max = ++s_npass; break;                              <* 
    *>    }                                                                           <* 
    *> }                                                                              <* 
    *> /+---(complete)-----------------------+/                                       <* 
    *> return x_max;                                                                  <*/
}

char
HISTORY__load           (char a_mode, char *a_text)
{
   /*> if (a_text == NULL) {                                                          <* 
    *>    strlcpy (s_current, "" , LEN_RECD);                                         <* 
    *>    s_len  = 0;                                                                 <* 
    *>    return -1;                                                                  <* 
    *> }                                                                              <* 
    *> strlcpy  (s_current, a_text, LEN_RECD);                                        <* 
    *> strldchg (s_current, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                   <* 
    *> s_len  = strllen (s_current, LEN_RECD);                                        <* 
    *> return 0;                                                                      <*/
}

char
HISTORY__exec           (char a_mode)
{
   /*> /+---(locals)-----------+-----+-----+-+/                                       <* 
    *> char        rce         =  -10;                                                <* 
    *> int         rc          =    0;                                                <* 
    *> tHIST      *x_pass      = NULL;                                                <* 
    *> int         i           =    0;                                                <* 
    *> int         n           =   -1;                                                <* 
    *> int         x_max       =    0;                                                <* 
    *> char        x_clear     [LEN_LABEL];                                           <* 
    *> char        x_repeat    [LEN_LABEL];                                           <* 
    *> char        t           [LEN_RECD];                                            <* 
    *> /+---(header)-------------------------+/                                       <* 
    *> DEBUG_HIST   yLOG_enter   (__FUNCTION__);                                      <* 
    *> DEBUG_HIST   yLOG_char    ("a_mode"    , a_mode);                              <* 
    *> DEBUG_HIST   yLOG_value   ("s_len"     , s_len);                               <* 
    *> DEBUG_HIST   yLOG_info    ("s_current" , s_current);                           <* 
    *> /+---(defense)------------------------+/                                       <* 
    *> --rce;  if (!STATUS_operational (a_mode)) {                                    <* 
    *>    DEBUG_HIST   yLOG_note    ("can not execute until operational");            <* 
    *>    DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> /+---(set limits)---------------------+/                                       <* 
    *> switch (a_mode) {                                                              <* 
    *> case MODE_COMMAND :                                                            <* 
    *>    x_pass   = &s_runs;                                                         <* 
    *>    x_max    = s_nrun;                                                          <* 
    *>    break;                                                                      <* 
    *> case MODE_SEARCH  :                                                            <* 
    *>    x_pass   = &s_passes;                                                       <* 
    *>    x_max    = s_npass;                                                         <* 
    *>    break;                                                                      <* 
    *> }                                                                              <* 
    *> DEBUG_HIST   yLOG_value   ("x_max"     , x_max);                               <* 
    *> sprintf (x_clear  , "%c"       , a_mode);                                      <* 
    *> DEBUG_HIST   yLOG_info    ("x_clear"   , x_clear);                             <* 
    *> sprintf (x_repeat , "%c%c"     , a_mode, a_mode);                              <* 
    *> DEBUG_HIST   yLOG_info    ("x_repeat"  , x_repeat);                            <* 
    *> /+---(unmark, no clear)---------------+/                                       <* 
    *> if (s_len == 2 && s_current [0] == 'u') {                                      <* 
    *>    DEBUG_HIST   yLOG_char    ("unmarking" , s_current [1]);                    <* 
    *>    HISTORY__unmark (a_mode, s_current [1]);                                    <* 
    *>    DEBUG_HIST   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(mark, no clear)-----------------+/                                       <* 
    *> if (s_len == 2 && s_current [0] == 'm') {                                      <* 
    *>    DEBUG_HIST   yLOG_char    ("marking"   , s_current [1]);                    <* 
    *>    HISTORY__mark (a_mode, s_current [1]);                                      <* 
    *>    DEBUG_HIST   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(clear results)------------------+/                                       <* 
    *> if (a_mode == MODE_SEARCH)  yvikeys_srch__purge  ();                           <* 
    *> if (s_len <= 0) {                                                              <* 
    *>    DEBUG_HIST   yLOG_note    ("empty content string, leaving");                <* 
    *>    DEBUG_HIST   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> if (s_len == 1 && strcmp (s_current, x_clear) == 0) {                          <* 
    *>    DEBUG_HIST   yLOG_note    ("request to clear");                             <* 
    *>    DEBUG_HIST   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <* 
    *> /+---(repeat last)--------------------+/                                       <* 
    *> else if (s_len == 2 && strcmp (s_current, x_repeat) == 0) {                    <* 
    *>    DEBUG_HIST   yLOG_note    ("request to repeat last");                       <* 
    *>    if (x_max == 0) return -1;                                                  <* 
    *>    n = x_max - 1;                                                              <* 
    *>    yvikeys_hist__roll (a_mode, n);                                                  <* 
    *> }                                                                              <* 
    *> /+---(repeat complex)-----------------+/                                       <* 
   *> else if (s_len > 2 && strncmp (s_current, x_repeat, 2) == 0) {                 <* 
      *>    /+---(repeat marked)---------------+/                                       <* 
         *>    if (s_len == 3 && strchr (S_HIST_LIST, s_current [2]) != NULL) {            <* 
            *>       DEBUG_HIST   yLOG_note    ("request to repeat a marked one");            <* 
               *>       rc = HISTORY__return (a_mode, s_current [2]);                            <* 
               *>       if (rc < 0)  return rc;                                                  <* 
               *>    }                                                                           <* 
               *>    /+---(repeat by reference)---------+/                                       <* 
               *>    else {                                                                      <* 
                  *>       DEBUG_HIST   yLOG_note    ("request to repeat a previous one");          <* 
                     *>       n = x_max - atoi (s_current + 2) - 1;                                    <* 
                     *>       DEBUG_HIST   yLOG_value   ("n"         , n);                             <* 
                     *>       if (n < 0) n = 0;                                                        <* 
                     *>       yvikeys_hist__roll (a_mode, n);                                               <* 
                     *>    }                                                                           <* 
                     *> }                                                                              <* 
                     *> /+---(normal)-------------------------+/                                       <* 
                     *> else {                                                                         <* 
                        *>    DEBUG_HIST   yLOG_note    ("typed request");                                <* 
                           *>    x_max = HISTORY__find (a_mode, s_current);                                  <* 
                           *> }                                                                              <* 
                           *> /+---(make current)-------------------+/                                       <* 
                           *> strlcpy (s_current, (x_pass + x_max - 1)->text, LEN_RECD);                     <* 
                           *> s_len = strllen (s_current, LEN_RECD);                                         <* 
                           *> (x_pass + x_max - 1)->found = 0;                                               <* 
                           *> ++(x_pass + x_max - 1)->count;                                                 <* 
                           *> DEBUG_HIST   yLOG_value   ("s_len"     , s_len);                               <* 
                           *> DEBUG_HIST   yLOG_info    ("s_current" , s_current);                           <* 
                           *> /+---(execute)------------------------+/                                       <* 
                           *> strldchg (s_current, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);                   <* 
                           *> switch (a_mode) {                                                              <* 
                              *> case MODE_COMMAND :                                                            <* 
                                 *>    DEBUG_HIST   yLOG_note    ("execute as command");                           <* 
                                 *>    rc = yvikeys_cmds_exec ();                                                  <* 
                                 *>    (x_pass + x_max - 1)->found = rc;                                           <* 
                                 *>    break;                                                                      <* 
                                 *> case MODE_SEARCH  :                                                            <* 
                                 *>    DEBUG_HIST   yLOG_note    ("execute as search");                            <* 
                                 *>    rc = s_searcher (s_current);                                                <* 
                                 *>    break;                                                                      <* 
                                 *> }                                                                              <* 
                                 *> /+---(complete)-----------------------+/                                       <* 
                                 *> DEBUG_HIST   yLOG_exit    (__FUNCTION__);                                      <* 
                                 *> return rc;                                                                     <*/
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_hist__unit      (char *a_question, int n)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_HUND ] = "";
   tHIST      *x_curr      = NULL;
   int         c           =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "HIST unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "switch"         )   == 0) {
      if      (s_head  == NULL    )  strlcat (t, "h=----"  , LEN_HUND);
      else if (s_head  == &s_hrun )  strlcat (t, "h=CMDS"  , LEN_HUND);
      else if (s_head  == &s_hpass)  strlcat (t, "h=SRCH"  , LEN_HUND);
      else                           strlcat (t, "h=????"  , LEN_HUND);
      if      (s_tail  == NULL    )  strlcat (t, ", t=----", LEN_HUND);
      else if (s_tail  == &s_trun )  strlcat (t, ", t=CMDS", LEN_HUND);
      else if (s_tail  == &s_tpass)  strlcat (t, ", t=SRCH", LEN_HUND);
      else                           strlcat (t, ", t=????", LEN_HUND);
      if      (s_curr  == NULL    )  strlcat (t, ", c=----", LEN_HUND);
      else if (s_curr  == &s_crun )  strlcat (t, ", c=CMDS", LEN_HUND);
      else if (s_curr  == &s_cpass)  strlcat (t, ", c=SRCH", LEN_HUND);
      else                           strlcat (t, ", c=????", LEN_HUND);
      if      (s_count == NULL    )  strlcat (t, ", n=----", LEN_HUND);
      else if (s_count == &s_nrun )  strlcat (t, ", n=CMDS", LEN_HUND);
      else if (s_count == &s_npass)  strlcat (t, ", n=SRCH", LEN_HUND);
      else                           strlcat (t, ", n=????", LEN_HUND);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST switch      : %s", t);
   }
   else if (strcmp (a_question, "counts"         )   == 0) {
      /*> printf ("checking...\n");                                                   <*/
      /*> snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST count       : %3da, %3dc, %3ds, %3d", s_nall, s_nrun, s_npass, *s_count);   <*/
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST count       : %3da, %3dc, %3ds", s_nall, s_nrun, s_npass);
   }
   else if (strcmp (a_question, "current"        ) == 0) {
      if    (*s_head == s_hrun) strlcpy (t, "c", LEN_LABEL);
      else                      strlcpy (t, "s", LEN_LABEL);
      if (*s_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST curr %s (--) :  -:-                      -   -", t);
      else                  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST curr %s (%2d) : %2d:%-20.20s %3d %3d", t, *s_index, strlen ((*s_curr)->text), (*s_curr)->text, (*s_curr)->count, (*s_curr)->found);
   }
   else if (strcmp (a_question, "command"        )   == 0 || strcmp (a_question, "search"         )   == 0) {
      if (a_question [0] == 'c') {
         strlcpy (t, "cmds", LEN_LABEL);
         x_curr = s_hrun;
      } else {
         strlcpy (t, "srch", LEN_LABEL);
         x_curr = s_hpass;
      }
      DEBUG_HIST   yLOG_value   ("n"         , n);
      while (x_curr != NULL) {
         DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);
         /*> printf ("entry  %2d %-10.10s %10p %10p\n", c, x_curr->text, x_curr, x_curr->m_next);   <*/
         if (c == n) {
            /*> printf ("found\n");                                                   <*/
            DEBUG_HIST   yLOG_point   ("found"     , x_curr);
            break;
         }
         x_curr = x_curr->m_next;
         ++c;
      }
      /*> printf ("checking...%p\n", x_curr);                                         <*/
      DEBUG_HIST   yLOG_point   ("found"     , x_curr);
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST %-4.4s   (%2d) :  0:-                      0   0", t, n);
      else {
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST %-4.4s   (%2d) : %2d:%-20.20s %3d %3d",
               t, n, strlen (x_curr->text), x_curr->text, x_curr->count, x_curr->found);
      }
   }
   /*---(complete)-----------------------*/
   /*> printf ("%s\n", yVIKEYS__unit_answer);                                         <*/
   return yVIKEYS__unit_answer;
}

