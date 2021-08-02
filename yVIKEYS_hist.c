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
   uchar       mark;                    /* if marked for quick use           */
   uchar      *text;                    /* command or search line used       */
   char        ran;                     /* could it execute                  */
   uchar       count;                   /* how many times has it been used   */
   int         found;                   /* what where the results            */
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
/*---(current)-----------------*/
static uchar   s_current   [LEN_RECD] = "";
static int     s_len       = 0;
/*---(done)--------------------*/



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
      DEBUG_HIST   yLOG_note    ("same as last, short-cut");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(switch)-------------------------*/
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



/*====================------------------------------------====================*/
/*===----                      histtory marks                         ----===*/
/*====================------------------------------------====================*/
static void  o___MARKS___________o () { return; }

char 
yvikeys_hist__valid     (char a_abbr)
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

char
yvikeys_hist__unmark  (uchar a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tHIST      *x_curr      = NULL;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_char    ("a_mark"    , a_mark);
   --rce;  if (strchr (S_HIST_LIST, tolower (a_mark)) == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(walk-entries)-------------------*/
   x_curr = *s_head;
   while (x_curr != NULL) {
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);
      if (a_mark == x_curr->mark)   x_curr->mark = '-';
      x_curr = x_curr->m_next;
      ++c;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__mark    (uchar a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_char    ("a_mark"    , a_mark);
   --rce;  if (strchr (S_HIST_LIST, a_mark) == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   --rce;  if (*s_curr == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make sure not used)-------------*/
   yvikeys_hist__unmark (a_mark);
   /*---(mark current)-------------------*/
   (*s_curr)->mark = a_mark;
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    memory allocation                         ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

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
   x_new->ran      = 0;
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
   /*---(update current)-----------------*/
   *s_curr  = *s_tail;
   *s_index = *s_count - 1;
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
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
   strlcat (S_HIST_LIST, YSTR_LOWER , S_HIST_MAX);
   /*> strlcat (S_HIST_LIST, YSTR_UPPER , S_HIST_MAX);                             <*/
   strlcat (S_HIST_LIST, YSTR_GREEK , S_HIST_MAX);
   DEBUG_HIST   yLOG_info    ("LIST"      , S_HIST_LIST);
   /*---(clear history)------------------*/
   DEBUG_HIST   yLOG_note    ("clear all history");
   /*---(initialize)--------------*/
   yvikeys_hist__purge (MODE_COMMAND);
   yvikeys_hist__purge (MODE_SEARCH);
   s_nall      = 0;       /* full count         */
   /*---(switchable)--------------*/
   yvikeys_hist__switcher (MODE_COMMAND, 'y');
   yvikeys_hist_text      (MODE_COMMAND, NULL);
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



/*====================------------------------------------====================*/
/*===----                      display screens                         ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

static char    s_on    = '-';
static int     s_first = 0;
static int     s_now   = 0;
static int     s_min   = 0;
static int     s_topp  = 0;
static int     s_tall  = 0;
static int     s_bott  = 0;
static int     s_left  = 0;
static int     s_wide  = 0;
static int     s_righ  = 0;
static int     s_max   = 0;
static int     s_lines = 0;
static int     s_chars = 0;

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
yvikeys_hist__bounds    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_mode      =  '-';
   int         m           =    1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   s_on    = yVIKEYS_view_size     (YVIKEYS_HISTORY, &s_left, &s_wide, &s_bott, &s_tall, NULL);
   x_mode = MODE_curr ();
   if (x_mode == UMOD_HISTORY)  x_mode = MODE_prev ();
   yvikeys_hist_limits   (x_mode, &s_min, &s_max);
   s_now   = *s_index;
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      s_lines = s_tall - 4;
      s_chars = s_wide - 2;
   } else {
      s_lines = (s_tall / 15.0) - 4;
      s_chars = (s_wide /  7.5) - 2;
   }
   s_first = s_now - ((s_lines - 2) / 2);
   s_topp  = s_bott - s_lines + 1;
   s_righ  = s_left + s_wide - 1;
   DEBUG_HIST   yLOG_complex ("values"    , "%3dc %3dl %3dn %3df", *s_count, s_lines, s_now, s_first);
   DEBUG_HIST   yLOG_complex ("sizes"     , "%3dl %3dw %3dr %3db %3dt %3dt ", s_left, s_wide, s_righ, s_bott, s_tall, s_topp);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__entry      (uchar *a_entry, int a_len, char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        x_pref      [LEN_RECD];
   char        x_suff      [LEN_RECD];
   int         x_size      =    0;
   int         x_found     =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)  return rce;
   /*---(spacer)-------------------------*/
   /*> DEBUG_HIST   yLOG_complex ("args"      , "%-10p, %3dl, %ct", a_entry, a_len, a_type);   <*/
   if (a_type == ' ') {
      sprintf (a_entry, "%-*.*s%c", a_len - 1, a_len - 1, "´    -    ", '´');
      DEBUG_HIST   yLOG_delim   ("blank"     , a_entry);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(header)-------------------------*/
   if (a_type == 'h') {
      if (a_len >= 60) {
         x_size = a_len - 4 - 17;
         if (MODE_prev () == MODE_SEARCH)   sprintf (x_pref, "ref %*.*s", x_size, x_size, "----------SEARCH HISTORY-------------------------------------------------------------");
         else                               sprintf (x_pref, "ref %*.*s", x_size, x_size, "----------COMMAND HISTORY------------------------------------------------------------");
         sprintf (x_suff, "runs exec retn m");
      } else if (a_len >= 40) {
         x_size = a_len - 4 - 7;
         if (MODE_prev () == MODE_SEARCH)   sprintf (x_pref, "ref %*.*s", x_size, x_size, "-----SEARCH HISTORY-------------------------------------------------------------");
         else                               sprintf (x_pref, "ref %*.*s", x_size, x_size, "-----COMMAND HISTORY------------------------------------------------------------");
         sprintf (x_suff, "retn m");
      } else {
         x_size = a_len - 2;
         if (MODE_prev () == MODE_SEARCH)   sprintf (x_pref, "%*.*s", x_size, x_size, "´--SEARCH HISTORY-------------------------------------------------------------");
         else                               sprintf (x_pref, "%*.*s", x_size, x_size, "´--COMMAND HISTORY------------------------------------------------------------");
         sprintf (x_suff, "m");
      }
      sprintf (a_entry, "%s %s", x_pref, x_suff);
      return 0;
   }
   /*---(footer)-------------------------*/
   if (a_type == 'f') {
      if (a_len >= 40) {
         sprintf (a_entry, "%-*.*s %c", a_len - 2, a_len - 2, "¦ to choose, ¥ to leave, _KkjJ~ to move       ", '´');
      } else {
         sprintf (a_entry, "%-*.*s %c", a_len - 2, a_len - 2, "¦ pick, ¥, _KkjJ~       ", '´');
      }
      /*> DEBUG_HIST   yLOG_delim   ("footer"    , a_entry);                          <*/
      return 0;
   }
   /*---(create)-------------------------*/
   x_len   = strlen ((*s_curr)->text);
   x_found = (*s_curr)->found;
   if (x_found > 999)  x_found = 999;
   if      (a_len >= 60) {
      x_size = a_len - 4 - 17;
      sprintf (x_pref, "%-3d %-*.*s", *s_index, x_size, x_size, (*s_curr)->text);
      sprintf (x_suff, "%3dc %3dr %3df %c", (*s_curr)->count, (*s_curr)->ran, x_found, (*s_curr)->mark);
   } else if (a_len >= 40) {
      x_size = a_len - 4 - 7;
      sprintf (x_pref, "%-3d %-*.*s", *s_index, x_size, x_size, (*s_curr)->text);
      sprintf (x_suff, "%3df %c", x_found, (*s_curr)->mark);
   } else {
      x_size = a_len - 2;
      sprintf (x_pref, "%-*.*s", x_size, x_size, (*s_curr)->text);
      sprintf (x_suff, "%c", (*s_curr)->mark);
   }
   if (x_size < x_len)  x_pref [strlen (x_pref) - 1] = '>';
   sprintf (a_entry, "%s %s", x_pref, x_suff);
   /*> DEBUG_HIST   yLOG_delim   ("entry"     , a_entry);                             <*/
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
yvikeys_hist_info       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_edit      =  ' ';
   char        x_entry     [LEN_RECD]  = "";
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   yvikeys_hist__bounds ();
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      yCOLOR_curs ("h_used");
      /*> 1234567890123456789012345678901234::1234567890123456789012345678901234            <*/
      switch (MODE_curr ()) {
      case UMOD_MARK      : strlcpy (x_entry, " -  --label--- --x-- --y-- --z--      -  --label--- --x-- --y-- --z--   ", LEN_RECD);  break;
      case UMOD_VISUAL    : strlcpy (x_entry, " -  --label---  --label---            -  --label---  --label---         ", LEN_RECD);  break;
      case SMOD_MACRO     : strlcpy (x_entry, " - len ---macro-keys--------------------------------------------------- ", LEN_RECD);  break;
      case SMOD_SREG      : strlcpy (x_entry, " -   len  ---text-register-contents------------------------------------ ", LEN_RECD);  break;
      }
      mvprintw (s_bott - s_tall + 1, s_left, "%-*.*s", s_wide, s_wide, x_entry);
      attrset     (0);
      for (i = 0; i < s_lines; ++i) {
         if ((i % 2) == 0)  yCOLOR_curs ("h_curr");
         else               yCOLOR_curs ("i_maps");
         switch (MODE_curr ()) {
         case UMOD_MARK      : yvikeys_mark_info    (x_entry, i);  break;
         case UMOD_VISUAL    : yvikeys_visu_info    (i, x_entry);  break;
                               /*> case SMOD_MACRO     : MACRO_infowin (x_entry, i);  break;                <*/
         case SMOD_SREG      : yvikeys_sreg_info    (i, x_entry);  break;
         }
         mvprintw (s_bott - s_tall + 2 + i, s_left             , "%-*.*s", s_wide, s_wide, x_entry);
         attrset     (0);
      }
      attrset     (0);
      yCOLOR_curs ("h_used");
      mvprintw (s_bott, s_left             , "%-*.*s", s_wide, s_wide, " ¦ to choose, ¥ to escape ------------------------------------------------------------");
      attrset     (0);
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__back      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left, x_wide, x_bott, x_tall;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   yVIKEYS_view_size   (YVIKEYS_HISTORY, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(make background)----------------*/
   /*> s_pt     = myVIKEYS.point;                                                     <*/
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      yvikeys_menu_shadow   (myVIKEYS.env, x_left, x_wide, x_bott, x_tall, 300);
      yvikeys_menu_fill     (myVIKEYS.env, MODE_prev (), MODE_prev (), x_left, x_wide, x_bott, x_tall, 310);
      yvikeys_menu_heads    (myVIKEYS.env, MODE_prev (), MODE_prev (), NULL, x_left, x_wide, x_bott, x_tall, 320);
   } else {
      yvikeys_menu_shadow   (myVIKEYS.env, x_left, x_wide, x_bott, x_tall, 300);
      yvikeys_menu_fill     (myVIKEYS.env, MODE_prev (), MODE_prev (), x_left, x_wide, x_bott, x_tall, 310);
      yvikeys_menu_heads    (myVIKEYS.env, MODE_prev (), MODE_prev (), NULL, x_left, x_wide, x_bott, x_tall, 320);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__show      (char a_type, int i, char *a_text, char rc, char a_part)
{
   int         x_lef, x_rig, x_top, x_bot;
   if (a_type == YVIKEYS_CURSES) {
      if      (rc != 0)   {
         if ((i % 2) == 0)           yCOLOR_curs ("w_titl");
         else                        yCOLOR_curs ("h_used");
      }
      else if (*s_index == s_now)      yCOLOR_curs ("i_srcs");
      else if ((i % 2) == 0)           yCOLOR_curs ("h_curr");
      else                             yCOLOR_curs ("i_maps");
      /*---(display)--------*/
      mvprintw (s_bott - s_tall + 2 + i, s_left, " %s ", a_text);
      attrset     (0);
   }
   else {
      x_lef = s_left + 10;
      x_rig = s_righ - 10;
      x_bot = s_bott + s_tall - (i + 3) * 15 -  8;
      x_top = x_bot + 15;
      if (i % 2 == 0) {
         if      (a_part != ' ')      glColor4f (0.5, 0.5, 0.5, 0.2);
         else if (a_text [5] == '-')  glColor4f (0.5, 0.5, 0.5, 0.2);
         else if (*s_index == s_now)  glColor4f (0.5, 0.5, 0.0, 0.2);
         else                         glColor4f (0.0, 0.5, 0.0, 0.2);
         glBegin(GL_POLYGON); {
            glVertex3f (x_lef, x_top, 340);
            glVertex3f (x_rig, x_top, 340);
            glVertex3f (x_rig, x_bot, 340);
            glVertex3f (x_lef, x_bot, 340);
         } glEnd();
      }
      glColor4f (0.0, 0.0, 0.0, 1.0);
      glPushMatrix(); {
         glTranslatef (s_left + 15, s_bott + s_tall - (i + 3) * 15 - 5, 350);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_TOPLEF, a_text);
      } glPopMatrix();
   }
   return 0;
}

char         /*-> show history on screen -------------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
yvikeys_hist_show       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_edit      =  ' ';
   char        x_entry     [LEN_RECD]  = "";
   int         i           =    0;
   int         x_index     =    0;
   tHIST      *x_curr      = NULL;
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST   yLOG_char    ("info_win"  , myVIKEYS.info_win);
   if (myVIKEYS.info_win == 'y') {
      yvikeys_hist_info ();;
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_HIST   yLOG_char    ("curr mode" , MODE_curr ());
   DEBUG_HIST   yLOG_char    ("prev mode" , MODE_prev ());
   if (MODE_not (UMOD_HISTORY)) {
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(prepare)------------------------*/
   yvikeys_hist__bounds ();
   yvikeys_hist__back   ();
   x_index = *s_index;
   x_curr  = *s_curr;
   yvikeys_hist__index (s_first);
   /*---(show screen)--------------------*/
   DEBUG_HIST   yLOG_complex ("values"    , "%3dc %3dl %3dn %3df", *s_count, s_lines, s_now, s_first);
   DEBUG_HIST   yLOG_complex ("sizes"     , "%3dl %3dw %3dr %3db %3dt %3dt ", s_left, s_wide, s_righ, s_bott, s_tall, s_topp);
   /*---(header)----------------------*/
   /*> yCOLOR_curs ("h_used" );                                                       <*/
   yvikeys_hist__entry (x_entry, s_chars, 'h');
   yvikeys_hist__show (myVIKEYS.env, -1, x_entry, 0, 'h');
   /*> mvprintw (s_bott - s_tall + 1, s_left, " %s ", x_entry);                       <* 
    *> attrset     (0);                                                               <*/
   /*---(middle)----------------------*/
   /*> yCOLOR_curs ("h_used" );                                                       <*/
   for (i = 0; i < s_lines; ++i) {
      /*---(content)--------*/
      rc = 1;
      if (*s_curr  == NULL)     yvikeys_hist__entry (x_entry, s_chars, ' ');
      else if (s_first + i < 0) yvikeys_hist__entry (x_entry, s_chars, ' ');
      else {
         if      (s_first + i == 0)  rc = 0;
         else if (s_first + i >  0)  rc = yvikeys_hist__cursor ('>');
         if (rc == 0)  yvikeys_hist__entry (x_entry, s_chars, 'e');
         else          yvikeys_hist__entry (x_entry, s_chars, ' ');
      }
      DEBUG_HIST   yLOG_delim   ("x_entry"   , x_entry);
      /*---(color)----------*/
      /*> if      (rc != 0)   {                                                       <* 
       *>    if ((i % 2) == 0)           yCOLOR_curs ("title"        );               <* 
       *>    else                        yCOLOR_curs ("h_used" );                     <* 
       *> }                                                                           <* 
       *> else if (*s_index == s_now)      yCOLOR_curs ("source"       );             <* 
       *> else if ((i % 2) == 0)           yCOLOR_curs ("h_current"    );             <* 
       *> else                             yCOLOR_curs ("map"          );             <*/
      /*---(display)--------*/
      /*> sprintf (t, "%2d %4d %2d[%s]", i, rc, strlen (x_entry), x_entry);           <*/
      /*> yvikeys_hist__show (myVIKEYS.env, i, t, rc);                                <*/
      yvikeys_hist__show (myVIKEYS.env, i, x_entry, rc, ' ');
      /*> mvprintw (s_bott - s_tall + 2 + i, s_left, " %s ", x_entry);                <* 
       *> attrset     (0);                                                            <*/
      /*---(next)-----------*/
   }
   /*---(footer)----------------------*/
   yvikeys_hist__entry (x_entry, s_chars, 'f');
   yvikeys_hist__show (myVIKEYS.env, i, x_entry, 0, 'f');
   /*> yCOLOR_curs ("h_used" );                                                       <* 
    *> yvikeys_hist__entry (x_entry, s_chars - 2, 'f');                                <* 
    *> mvprintw (s_bott, s_left, " %s ", x_entry);                                    <* 
    *> attrset     (0);                                                               <*/
   /*---(reset)--------------------------*/
   *s_index = x_index;
   *s_curr  = x_curr;
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> create a shape mask for notes ------------------------------*/
yVIKEYS_hist_mask       (void *a_bounds, void *a_context, int a_left, int a_topp, int a_xmin, int a_ymax)
{
   /*---(locals)-----------+-----+-----+-*/
   Pixmap      *x_bounds;
   GC          *x_context;
   int         x_left, x_wide, x_bott, x_tall;
   /*---(quick out)----------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)    return 0;
   if (MODE_not (UMOD_HISTORY))           return 0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex  ("args"      , "%p, %p, %3dl, %3dt, %3dx, %3dy", a_bounds, a_context, a_left, a_topp, a_xmin, a_ymax);
   /*---(get size)-----------------------*/
   yVIKEYS_view_size   (YVIKEYS_HISTORY, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   /*---(cast)---------------------------*/
   x_bounds  = (Pixmap *) a_bounds;
   x_context = (GC *) a_context;
   /*---(draw)---------------------------*/
   XFillRectangle (YX_DISP, *x_bounds, *x_context, a_left + (x_left - a_xmin), a_topp + a_ymax - (x_bott + x_tall), x_wide, x_tall);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       file read/write                        ----===*/
/*====================------------------------------------====================*/
static void  o___FILES___________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_hist__write     (int c, char a_mode, char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   char        x_recd      [LEN_RECD ];
   tHIST      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   rc  = yvikeys_hist__valid (a_abbr);
   DEBUG_INPT   yLOG_value   ("index"     , rc);
   --rce; if (rc  < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(find mark)----------------------*/
   x_curr = *s_head;
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
   strlcpy  (x_recd, x_curr->text, LEN_RECD);
   if (a_mode == MODE_COMMAND)  yPARSE_vprintf (c, "command", x_curr->mark, x_recd);
   else                         yPARSE_vprintf (c, "search" , x_curr->mark, x_recd);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_hist__writer    (char a_mode)
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
   --rce;  if (!STATUS_operational (a_mode)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(switcher)-----------------------*/
   yvikeys_hist__switcher (a_mode, '-');
   /*---(prepare)------------------------*/
   x_end = strlen (S_HIST_LIST);
   /*> if (a_mode == MODE_COMMAND)  yPARSE_verb_begin ("command");                    <* 
    *> else                         yPARSE_verb_begin ("search");                     <*/
   /*---(walk list)----------------------*/
   for (i = 0; i < x_end; ++i) {
      rc = yvikeys_hist__write (c, a_mode, S_HIST_LIST [i]);
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

char yvikeys_cmds_writer  (void) { return yvikeys_hist__writer (MODE_COMMAND); }

char
yvikeys_hist__reader         (char a_mode, int n, char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_abbr      =    0;
   char        x_text       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (a_mode)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(switcher)-----------------------*/
   yvikeys_hist__switcher (a_mode, '-');
   /*---(get verb)-----------------------*/
   /*> rc = yPARSE_popstr (x_verb);                                                   <*/
   /*> DEBUG_INPT   yLOG_value   ("pop verb"  , rc);                                  <*/
   DEBUG_INPT   yLOG_info    ("a_verb"    , a_verb);
   --rce;  if (a_mode == MODE_SEARCH  && strcmp ("search" , a_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_mode == MODE_COMMAND && strcmp ("command", a_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   rc  = yvikeys_hist__valid (x_abbr);
   DEBUG_INPT   yLOG_value   ("index"     , rc);
   --rce; if (rc  < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get command)--------------------*/
   rc = yPARSE_popstr (x_text);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   strldchg (x_text, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("x_text"    , x_text);
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   rc = yvikeys_hist__new (a_mode, x_text);
   DEBUG_INPT   yLOG_value   ("new"       , rc);
   --rce; if (rc  < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   (*s_curr)->mark = x_abbr;
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}

char yvikeys_cmds_reader (int n, char *a_verb) { return yvikeys_hist__reader  (MODE_COMMAND, n, a_verb); }



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
yvikeys_hist__cursor    (char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
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
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   if (*s_curr == NULL) {
      switch (a_move) {
      case '<' :
         DEBUG_HIST   yLOG_note    ("bounced off head");
         *s_curr  = *s_head;
         *s_index = 0;
         rc = 1;
         break;
      case '>' :
         DEBUG_HIST   yLOG_note    ("bounced off tail");
         *s_curr  = *s_tail;
         *s_index = *s_count - 1;
         rc = 1;
         break;
      }
   }
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return rc;
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
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, (*s_curr)->text, (*s_curr), (*s_curr)->m_next, (*s_curr)->m_prev);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   if (a_index >= *s_count) {
      DEBUG_HIST   yLOG_note    ("greater than max is short-cut to tail");
      *s_curr  = *s_tail;
      *s_index = *s_count - 1;
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, (*s_curr)->text, (*s_curr), (*s_curr)->m_next, (*s_curr)->m_prev);
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
yvikeys_hist__find    (uchar *a_text)
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
   DEBUG_HIST   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(walk-entries)-------------------*/
   x_curr = *s_head;
   while (x_curr != NULL) {
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);
      if (a_text [0] == x_curr->text [0]) {
         if (strcmp (a_text, x_curr->text) == 0)   break;
      }
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
   *s_index = c;
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("*s_curr"   , *s_curr);
   DEBUG_HIST   yLOG_value   ("*s_index"  , *s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist__marked  (uchar a_mark)
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
   DEBUG_HIST   yLOG_char    ("a_mark"    , a_mark);
   --rce;  if (strchr (S_HIST_LIST, a_mark) == NULL) {
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(walk-entries)-------------------*/
   x_curr = *s_head;
   while (x_curr != NULL) {
      DEBUG_HIST   yLOG_complex ("entry"     , "%2d %-10.10s %10p %10p %10p", c, x_curr->text, x_curr, x_curr->m_next, x_curr->m_prev);
      if (a_mark == x_curr->mark)   break;
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
   *s_index = c;
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
   /*---(short-cut)----------------------*/
   if (*s_tail == *s_curr) {
      DEBUG_HIST   yLOG_note    ("already tail, nothing to do");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
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
/*===----                      command execution                      ----===*/
/*====================------------------------------------====================*/
static void  o___EXECUTION_______o () { return; }

char
yvikeys_hist_text       (char a_mode, char *a_text)
{
   if (a_text == NULL) {
      strlcpy (s_current, "" , LEN_RECD);
      s_len  = 0;
      return -1;
   }
   strlcpy  (s_current, a_text, LEN_RECD);
   strldchg (s_current, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
   s_len  = strllen (s_current, LEN_RECD);
   return 0;
}

char
yvikeys_hist_exec       (char a_mode)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_rc        =    0;
   int         x_found     =    0;
   uchar       x_mark      =  '-';
   tHIST      *x_curr      = NULL;
   int         i           =    0;
   int         n           =   -1;
   int         x_max       =    0;
   char        x_clear     [LEN_LABEL];
   char        x_repeat    [LEN_LABEL];
   char        t           [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_mode"    , a_mode);
   DEBUG_HIST   yLOG_value   ("s_len"     , s_len);
   DEBUG_HIST   yLOG_info    ("s_current" , s_current);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (a_mode)) {
      DEBUG_HIST   yLOG_note    ("can not execute until operational");
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set limits)---------------------*/
   rc = yvikeys_hist__switcher (a_mode, '-');
   DEBUG_HIST   yLOG_value   ("*s_count"  , *s_count);
   /*---(special sequences)--------------*/
   sprintf (x_clear  , "%c"       , a_mode);
   DEBUG_HIST   yLOG_info    ("x_clear"   , x_clear);
   sprintf (x_repeat , "%c%c"     , a_mode, a_mode);
   DEBUG_HIST   yLOG_info    ("x_repeat"  , x_repeat);
   /*---(unmark, no clear)---------------*/
   if (s_len == 2 && s_current [0] == 'u') {
      x_mark = s_current [1];
      DEBUG_HIST   yLOG_char    ("unmarking" , x_mark);
      yvikeys_hist__unmark (x_mark);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(mark, no clear)-----------------*/
   if (s_len == 2 && s_current [0] == 'm') {
      x_mark = s_current [1];
      DEBUG_HIST   yLOG_char    ("mark"      , x_mark);
      yvikeys_hist__cursor (']');
      yvikeys_hist__mark (x_mark);
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(clear results)------------------*/
   if (a_mode == MODE_SEARCH)  yvikeys_srch_purge  ();
   if (s_len <= 0) {
      DEBUG_HIST   yLOG_note    ("empty content string, leaving");
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (s_len == 1 && strcmp (s_current, x_clear) == 0) {
      DEBUG_HIST   yLOG_note    ("request to clear");
      if (x_clear [0] = MODE_SEARCH)  yvikeys_srch_purge ();
      DEBUG_HIST   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(repeat last)--------------------*/
   if (s_len == 2 && strcmp (s_current, x_repeat) == 0) {
      DEBUG_HIST   yLOG_note    ("request to repeat last");
      yvikeys_hist__cursor (']');
   }
   /*---(repeat complex)-----------------*/
   else if (s_len == 3 && strncmp (s_current, x_repeat, 2) == 0) {
      x_mark = s_current [2];
      DEBUG_HIST   yLOG_char    ("x_mark"    , x_mark);
      /*---(repeat marked)---------------*/
      if (strchr (S_HIST_LIST, x_mark) != NULL) {
         DEBUG_HIST   yLOG_note    ("request to repeat a marked one");
         rc = yvikeys_hist__marked (x_mark);
         DEBUG_HIST   yLOG_value   ("marked"    , rc);
         if (rc < 0) {
            DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         yvikeys_hist__roll ();
      }
      /*---(repeat by reference)---------*/
      else {
         DEBUG_HIST   yLOG_note    ("request to repeat a previous one");
         yvikeys_hist__cursor (']');
         n = atoi (s_current + 2);
         DEBUG_HIST   yLOG_value   ("n"         , n);
         for (i = 0; i < n; ++i)   yvikeys_hist__cursor ('<');
         yvikeys_hist__roll ();
      }
   }
   /*---(normal)-------------------------*/
   else {
      DEBUG_HIST   yLOG_note    ("typed request");
      rc = yvikeys_hist__find (s_current);
      if (rc < 0) {
         rc = yvikeys_hist__new (a_mode, s_current);
         if (rc < 0) {
            DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      } else {
         yvikeys_hist__roll ();
      }
   }
   /*---(make current)-------------------*/
   strlcpy (s_current, (*s_curr)->text, LEN_RECD);
   s_len = strllen (s_current, LEN_RECD);
   (*s_curr)->found  = 0;
   ++(*s_curr)->count;
   if ((*s_curr)->count > 200)  (*s_curr)->count = 200;
   x_curr = *s_curr;
   DEBUG_HIST   yLOG_value   ("s_len"     , s_len);
   DEBUG_HIST   yLOG_info    ("s_current" , s_current);
   /*---(execute)------------------------*/
   strldchg (s_current, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   switch (a_mode) {
   case MODE_COMMAND :
      DEBUG_HIST   yLOG_note    ("execute as command");
      rc = yvikeys_cmds_exec (s_current, &x_rc);
      x_curr->ran    = rc;
      x_curr->found  = x_rc;
      break;
   case MODE_SEARCH  :
      DEBUG_HIST   yLOG_note    ("execute as search");
      DEBUG_HIST   yLOG_point   ("srcurrent" , s_current);
      rc = yvikeys_srch_exec (s_current, &x_found);
      x_curr->ran    = rc;
      x_curr->found  = x_found;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      mode handling                          ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> allow selection of older entries ---[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
yvikeys_hist_umode      (int  a_major, int  a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_pos       =    0;
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
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape, return to source mode");
      MODE_exit ();
      MODE_exit ();
      myVIKEYS.cursor = 'y';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("return, return to source mode");
      MODE_exit ();
      myVIKEYS.cursor = 'y';
      if (*s_curr != NULL)   yvikeys_src_swapall ((*s_curr)->text);
      else                   MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle keys)--------------------*/
   switch (a_minor) {
   case '_' :
      yvikeys_hist__cursor ('[');
      break;
   case 'J' :
      yvikeys_hist__cursor ('>');
      yvikeys_hist__cursor ('>');
      yvikeys_hist__cursor ('>');
      yvikeys_hist__cursor ('>');
   case 'j' :
      yvikeys_hist__cursor ('>');
      break;
   case 'K' :
      yvikeys_hist__cursor ('<');
      yvikeys_hist__cursor ('<');
      yvikeys_hist__cursor ('<');
      yvikeys_hist__cursor ('<');
   case 'k' :
      yvikeys_hist__cursor ('<');
      break;
   case '~' :
      yvikeys_hist__cursor (']');
      break;
   }
   /*---(update)-------------------------*/
   yvikeys_hist__bounds ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       unit testing                          ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

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
      if (*s_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST curr %s (--) :  -  -                      -    -    -    -", t);
      else                  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST curr %s (%2d) : %2d  %-20.20s %3dc %3dr %3df   %c", t, *s_index, strlen ((*s_curr)->text), (*s_curr)->text, (*s_curr)->count, (*s_curr)->ran, (*s_curr)->found, (*s_curr)->mark);
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
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST %-4.4s   (%2d) :  -  -                      -    -    -    -", t, n);
      else {
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "HIST %-4.4s   (%2d) : %2d  %-20.20s %3dc %3dr %3df   %c",
               t, n, strlen (x_curr->text), x_curr->text, x_curr->count, x_curr->ran, x_curr->found, x_curr->mark);
      }
   }
   /*---(complete)-----------------------*/
   /*> printf ("%s\n", yVIKEYS__unit_answer);                                         <*/
   return yVIKEYS__unit_answer;
}

