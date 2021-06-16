/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*
 * ---------- EXTERNAL INTERFACE ----------
 *
 *  yVIKEYS_srch_config -- must be called before use
 *
 *  yVIKEYS_srch_found  -- register selected/active finds
 *
 *
 * ---------- REQUIRED SUPPORT ----------
 *
 *  searcher            -- function to scan content
 *
 *  unsearcher          -- function to clear finds
 *
 *
 * ---------- FUTURE IDEAS ----------
 *
 *  complex regex...
 *     -- Ð Ô Ñ Õ Ò Ó  instead of just & and |
 *     -- /&´´´¦ to add AND phrase to current search
 *     -- /|´´´¦ to add OR phrase to current search
 *     -- //&a¦ merges save search 'a' with current as AND
 *     -- //|b¦ merges save search 'b' with current as OR
 *
 *
 */




#define   MAX_SRCH    2000
typedef   struct   cFIND   tFIND;
/*---(true useful vars)---------------*/
static tFIND  *s_head   = NULL;              /* head of link chain            */
static tFIND  *s_tail   = NULL;              /* tail of link chain            */
static short   s_count  = 0;                 /* total items in list           */

static tFIND  *s_csrch  = NULL;              /* current search find           */
static short   s_nsrch  = 0;                 /* all search items in list      */

static tFIND  *s_chint  = NULL;              /* current hint                  */
static short   s_nhint  = 0;                 /* all hint items in list        */

static tFIND  *s_ctags  = NULL;              /* current tag                   */
static short   s_ntags  = 0;                 /* all tags items in list        */


char    (*s_searcher)   (char *a_search)              = NULL;
char    (*s_unsearcher) (int b, int x, int y, int z)  = NULL;
static char   s_search  [LEN_LABEL]  = "-";



#define    IF_SRCH    if      (a_mode == MODE_SEARCH)
#define    EL_HINT    else if (a_mode == SMOD_HINT)
#define    EL_TAGS    else if (a_mode == SMOD_TAGS)



/*====================------------------------------------====================*/
/*===----                      program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_srch_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_SEARCH)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear everything)---------------*/
   yvikeys_srch_wrap ();
   /*---(clear history)------------------*/
   DEBUG_PROG   yLOG_note    ("clear all history");
   yvikeys_hist_text   (MODE_SEARCH, NULL);
   yvikeys_hist__purge (MODE_SEARCH);
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_SEARCH);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_srch_config     (void *a_searcher, void *a_unsearcher)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_SRCH  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (MODE_SEARCH)) {
      DEBUG_SRCH   yLOG_note    ("init must complete before config");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update searcher)----------------*/
   DEBUG_SRCH   yLOG_point   ("searcher"  , a_searcher);
   --rce;  if (a_searcher == NULL) {
      DEBUG_SRCH   yLOG_note    ("without searcher callback, search can not function");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_searcher = a_searcher;
   /*---(update unsearcher)--------------*/
   DEBUG_SRCH   yLOG_point   ("unsearcher", a_unsearcher);
   --rce;  if (a_unsearcher == NULL) {
      DEBUG_SRCH   yLOG_note    ("without clearer callback, search can not function");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_unsearcher = a_unsearcher;
   /*---(update status)------------------*/
   STATUS_conf_set   (MODE_SEARCH, '1');
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_srch_purge   (void) { yvikeys_srch__purge (MODE_SEARCH); }

char
yvikeys_srch_wrap            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(clear callbacks)----------------*/
   DEBUG_PROG   yLOG_note    ("clear callbacks");
   s_searcher   = NULL;
   s_unsearcher = NULL;
   strlcpy (s_search, "-", LEN_LABEL);
   /*---(clear current)------------------*/
   DEBUG_PROG   yLOG_note    ("initialize search results");
   yvikeys_srch_purge ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                  search, hint, and tags                     ----===*/
/*====================------------------------------------====================*/
static void  o___SHARED__________o () { return; }

char
yvikeys_srch__purge     (char a_mode)
{
   /*---(locals)-----------+-----+-----+-*/
   tFIND      *x_curr      = NULL;
   tFIND      *x_prev      = NULL;
   tFIND      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH  yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH  yLOG_char    ("a_mode"    , a_mode);
   /*---(clear history)------------------*/
   x_curr = s_head;
   while (x_curr != NULL) {
      /*--(cycle)--------------*/
      DEBUG_SRCH  yLOG_complex ("current"   , "%-10.10s, %c, prev %-10.10p, next %p", x_curr->label, x_curr->mode, x_curr->m_prev, x_curr->m_next);
      while (x_curr != NULL && x_curr->mode != a_mode) {
         x_curr = x_curr->m_next;
         if (x_curr != NULL)  DEBUG_SRCH  yLOG_complex ("... and"   , "%-10.10s, %c, prev %-10.10p, next %p", x_curr->label, x_curr->mode, x_curr->m_prev, x_curr->m_next);
      }
      if (x_curr == NULL)  break;
      /*--(prepare)------------*/
      x_prev = x_curr->m_prev;
      x_next = x_curr->m_next;
      /*--(clear in app)-------*/
      DEBUG_SRCH  yLOG_note    ("unsearch");
      IF_SRCH {
         if (s_unsearcher != NULL)  s_unsearcher (x_curr->b_pos, x_curr->x_pos, x_curr->y_pos, x_curr->z_pos);
      } EL_HINT {
         if (s_unhinter   != NULL)  s_unhinter   (x_curr->b_pos, x_curr->x_pos, x_curr->y_pos, x_curr->z_pos);
      }
      /*--(pull out)-----------*/
      DEBUG_SRCH  yLOG_note    ("unlink from master");
      if (x_prev != NULL)   x_prev->m_next = x_next;
      else                  s_head         = x_next;
      if (x_next != NULL)   x_next->m_prev = x_prev;
      else                  s_tail         = x_prev;
      /*--(ground/careful)-----*/
      DEBUG_SRCH  yLOG_note    ("ground and free");
      x_curr->m_prev = NULL;
      x_curr->m_next = NULL;
      /*--(free memory)--------*/
      if (x_curr->label != NULL)  free (x_curr->label);
      free (x_curr);
      --s_count;
      IF_SRCH  --s_nsrch;
      EL_HINT  --s_nhint;
      EL_TAGS  --s_ntags;
      /*--(next)---------------*/
      x_curr = x_next;
      /*--(done)---------------*/
   }
   /*---(current pointers)---------------*/
   DEBUG_SRCH  yLOG_point   ("s_head"    , s_head);
   DEBUG_SRCH  yLOG_point   ("s_tail"    , s_tail);
   DEBUG_SRCH  yLOG_value   ("s_count"   , s_count);
   IF_SRCH  s_csrch = NULL;
   EL_HINT  s_chint = NULL;
   EL_TAGS  s_ctags = NULL;
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_srch__found     (char a_mode, char *a_label, int a_buf, int x, int y, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tFIND      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   tFIND      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_char    ("a_mode"    , a_mode);
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tFIND *) malloc (sizeof (tFIND));
   }
   DEBUG_SRCH   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_SRCH   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_SRCH   yLOG_note    ("FAILED");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_SRCH   yLOG_note    ("populate");
   if (a_label != NULL)  x_new->label = strdup (a_label);
   else                  x_new->label = NULL;
   x_new->mode    = a_mode;
   x_new->b_pos   = a_buf;
   x_new->x_pos   = x;
   x_new->y_pos   = y;
   x_new->z_pos   = z;
   x_new->m_next  = NULL;
   x_new->m_prev  = NULL;
   /*---(tie to master list)-------------*/
   ++s_count;
   if (s_head == NULL) {
      DEBUG_SRCH   yLOG_note    ("nothing in list, make first");
      s_head           = x_new;
   } else  {
      DEBUG_SRCH   yLOG_note    ("append to list");
      s_tail->m_next   = x_new;
      x_new->m_prev    = s_tail;
   }
   s_tail       = x_new;
   IF_SRCH {
      s_csrch    = s_tail;
      ++s_nsrch;
      DEBUG_SRCH   yLOG_value   ("s_nsrch"   , s_nsrch);
   }
   EL_HINT {
      s_chint    = s_tail        = x_new;
      ++s_nhint;
      DEBUG_SRCH   yLOG_value   ("s_nhint"   , s_nhint);
   }
   EL_TAGS {
      s_ctags    = s_tail        = x_new;
      ++s_ntags;
      DEBUG_SRCH   yLOG_value   ("s_ntags"   , s_ntags);
   }
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_srch__by_cursor  (char a_mode, char a_move, void **r_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tFIND      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_char    ("a_mode"    , a_mode);
   /*---(default)------------------------*/
   if (r_curr != NULL)  *r_curr = NULL;
   /*---(defenses)-----------------------*/
   DEBUG_SRCH   yLOG_char    ("a_move"    , a_move);
   --rce;  if (strchr (YDLST_BOUNCES YDLST_DANGERS, a_move) == NULL) {
      IF_SRCH  s_csrch  = NULL;
      EL_HINT  s_chint  = NULL;
      EL_TAGS  s_ctags  = NULL;
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_point   ("s_head"   , s_head);
   --rce;  if (s_head == NULL) {
      IF_SRCH  s_csrch  = NULL;
      EL_HINT  s_chint  = NULL;
      EL_TAGS  s_ctags  = NULL;
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(starting point)-----------------*/
   IF_SRCH  x_curr = s_csrch;
   EL_HINT  x_curr = s_chint;
   EL_TAGS  x_curr = s_ctags;
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   /*---(if nowhere, start at head)------*/
   if (x_curr  == NULL)  x_curr = s_head;
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   /*---(handle move)--------------------*/
   switch (a_move) {
   case YDLST_HEAD : case YDLST_DHEAD :
      x_curr   = s_head;
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_next;
      break;
   case YDLST_PREV : case YDLST_DPREV :
      x_curr = x_curr->m_prev;
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_prev;
      break;
   case YDLST_CURR : case YDLST_DCURR :
      break;
   case YDLST_NEXT : case YDLST_DNEXT :
      x_curr = x_curr->m_next;
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_next;
      break;
   case YDLST_TAIL : case YDLST_DTAIL :
      x_curr = s_tail;
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_prev;
      break;
   }
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   /*---(bouncing)-----------------------*/
   --rce;  if (x_curr == NULL) {
      switch (a_move)  {
      case YDLST_PREV :
         x_curr   = s_head;
         while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_next;
         rc = rce;
         break;
      case YDLST_NEXT :
         x_curr = s_tail;
         while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_prev;
         rc = rce;
         break;
      }
      if (x_curr == NULL) {
         if (r_curr != NULL)  *r_curr = x_curr;
         DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save back)----------------------*/
   IF_SRCH  s_csrch  = x_curr;
   EL_HINT  s_chint  = x_curr;
   EL_TAGS  s_ctags  = x_curr;
   if (r_curr != NULL)  *r_curr = x_curr;
   /*---(move)---------------------------*/
   yVIKEYS_jump (x_curr->b_pos, x_curr->x_pos, x_curr->y_pos, x_curr->z_pos);
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_srch__by_index   (char a_mode, int a_index, tFIND **r_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tFIND      *x_curr      = NULL;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   DEBUG_SRCH   yLOG_char    ("a_mode"    , a_mode);
   /*---(default)------------------------*/
   if (r_curr != NULL)  *r_curr = NULL;
   /*---(defenses)-----------------------*/
   DEBUG_SRCH   yLOG_value   ("a_index"   , a_index);
   --rce;  if (a_index < 0) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_point   ("s_head"   , s_head);
   --rce;  if (s_head == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(if nowhere, start at head)------*/
   x_curr = s_head;
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   /*---(handle move)--------------------*/
   while (x_curr != NULL) {
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_next;
      if (x_curr == NULL)  break;
      if (n == a_index)    break;
      ++n;
      x_curr = x_curr->m_next;
   }
   DEBUG_SRCH   yLOG_point   ("x_curr"    , x_curr);
   /*---(busts)--------------------------*/
   --rce;  if (x_curr == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save back)----------------------*/
   if (r_curr != NULL)  *r_curr = x_curr;
   /*---(move)---------------------------*/
   yVIKEYS_jump (x_curr->b_pos, x_curr->x_pos, x_curr->y_pos, x_curr->z_pos);
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
yvikeys_srch__list      (char a_mode, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   char        rce         =  -10;
   tFIND      *x_curr      = NULL;
   char        x_entry     [LEN_LABEL];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strlcpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strlcpy (a_list, ",", LEN_RECD);
   x_curr = s_head;
   while (x_curr != NULL) {
      while (x_curr != NULL && x_curr->mode != a_mode)  x_curr = x_curr->m_next;
      if (x_curr == NULL)  break;
      if (i > 10)          break;
      if (x_curr->label != NULL)   sprintf  (x_entry, "%s,", x_curr->label);
      else                         strlcpy  (x_entry, "??", LEN_LABEL);
      strlcat    (a_list, x_entry, LEN_RECD);
      x_curr = x_curr->m_next;
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}

int
yvikeys_srch__count (char a_mode)
{
   switch (a_mode) {
   case MODE_SEARCH :  return s_nsrch;  break;
   case SMOD_HINT   :  return s_nhint;  break;
   case SMOD_TAGS   :  return s_ntags;  break;
   }
   return -1;
}

char
yvikeys_srch__curr      (char a_mode, char *a_label)
{
   strcpy (a_label, "-");
   switch (a_mode) {
   case MODE_SEARCH :  if (s_csrch != NULL)  strcpy (a_label, s_csrch->label);  break;
   case SMOD_HINT   :  if (s_chint != NULL)  strcpy (a_label, s_chint->label);  break;
   case SMOD_TAGS   :  if (s_ctags != NULL)  strcpy (a_label, s_ctags->label);  break;
   }
   return 0; 
}



/*====================------------------------------------====================*/
/*===----                     search finding                          ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char yvikeys_srch_by_cursor (char a_move, tFIND **r_curr)  { return yvikeys_srch__by_cursor (MODE_SEARCH, a_move, r_curr); }
char yvikeys_srch_by_index  (int a_index, tFIND **r_curr)  { return yvikeys_srch__by_index (MODE_SEARCH, a_index, r_curr); }
char yvikeys_srch_list      (char *a_list)                 { return yvikeys_srch__list (MODE_SEARCH, a_list); }
int  yvikeys_srch_count     (void)                         { return yvikeys_srch__count (MODE_SEARCH); }
char yvikeys_srch_curr      (char *a_label)                { return yvikeys_srch__curr (MODE_SEARCH, a_label); }



/*====================------------------------------------====================*/
/*===----                     search execution                        ----===*/
/*====================------------------------------------====================*/
static void  o___EXECUTE_________o () { return; }

char
yvikeys_srch_direct     (char *a_text)
{
   char        rc          =    0;
   rc = yvikeys_hist_text (MODE_SEARCH, a_text);
   if (rc < 0)  return -1;
   rc = yvikeys_hist_exec (MODE_SEARCH);
   return rc;
}

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
yvikeys_srch_exec     (uchar *a_search, int *a_found)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_rc        =    0;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   if (a_found != NULL)  *a_found = 0;
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (MODE_SEARCH)) {
      DEBUG_SRCH   yLOG_note    ("can not execute until operational");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_info    ("a_search"  , a_search);
   /*---(parse)-------------------------*/
   DEBUG_SRCH   yLOG_point   ("s_searcher", s_searcher);
   --rce;  if (s_searcher == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)----------------------------*/
   rc = s_searcher (a_search);
   DEBUG_SRCH   yLOG_value   ("rc"        , rc);
   DEBUG_SRCH   yLOG_value   ("s_nsrch"   , s_nsrch);
   if (a_found != NULL)  *a_found = s_nsrch;
   strlcpy (s_search, a_search, LEN_LABEL);
   /*---(position to first)--------------*/
   if (s_nsrch > 0)  yvikeys_srch_by_cursor (YDLST_DHEAD, NULL);
   /*---(complete)-----------------------*/
   DEBUG_SRCH   yLOG_exit    (__FUNCTION__);
   return rc;
}

char yVIKEYS_srch_found   (char *a_label, int b, int x, int y, int z) { yvikeys_srch__found (MODE_SEARCH, a_label, b, x, y, z); }



/*====================------------------------------------====================*/
/*===----                       input and output                       ----===*/
/*====================------------------------------------====================*/
static void  o___FILES___________o () { return; }

char yvikeys_srch_writer  (void)                { return yvikeys_hist__writer (MODE_SEARCH ); }
char yvikeys_srch_reader  (int n, char *a_verb) { return yvikeys_hist__reader  (MODE_SEARCH , n, a_verb); }



/*====================------------------------------------====================*/
/*===----                        screen display                        ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char
yvikeys_srch_status          (char *a_list)
{
   char        x_list      [LEN_RECD]  = "";
   char        x_label     [LEN_LABEL] = "";
   yvikeys_srch_list (x_list);
   yvikeys_srch_curr (x_label);
   snprintf (a_list, LEN_FULL, "search  %-10.10s  %2d  %-5.5s  %s", s_search, yvikeys_srch_count (), x_label, x_list);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
yvikeys__unit_searcher  (char *a_search)
{
   if (a_search == NULL)  return 0;
   if      (strcmp ("/1st", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /1st");
      yVIKEYS_srch_found ("0k11"     ,   0,  10,  10,   0);
      yVIKEYS_srch_found ("0p12"     ,   0,  15,  11,   0);
      yVIKEYS_srch_found ("3d6"      ,   3,   3,   5,   0);
   }
   else if (strcmp ("/2nd", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /2nd");
      yVIKEYS_srch_found ("0k11"     ,   0,  10,  10,   0);
      yVIKEYS_srch_found ("0b3"      ,   0,   1,   2,   0);
      yVIKEYS_srch_found ("2b5"      ,   2,   1,   4,   0);
      yVIKEYS_srch_found ("Za1"      ,  35,   0,   0,   0);
   }
   else if (strcmp ("/another", a_search) == 0) {
      DEBUG_SRCH   yLOG_note    ("found /another");
      yVIKEYS_srch_found ("2b5"      ,   2,   1,   4,   0);
   } else {
      DEBUG_SRCH   yLOG_note    ("nothing found");
   }
   return 0;
}

char
yvikeys__unit_unsearcher (char *a_junk)
{
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_srch__unit      (char *a_question, char n)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "-";
   tFIND      *x_curr      = NULL;
   int         c           =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRCH unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "results"        )   == 0) {
      yvikeys_srch_list (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH results     : %2d %-.40s", s_nsrch, t);
   }
   else if (strcmp (a_question, "oneline"        )   == 0) {
      DEBUG_SRCH   yLOG_value   ("n"         , n);
      yvikeys_srch_by_index (n, &x_curr);
      if (x_curr == NULL) snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH oneline     : %2d of %2d  -            -    -    -    -", n, s_nsrch);
      else {
         if (x_curr->label != NULL)  sprintf  (t, "%-10.10s", x_curr->label);
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRCH oneline     : %2d of %2d  %-10.10s %3db %3dx %3dy %3dz", n, s_nsrch, t,  x_curr->b_pos,  x_curr->x_pos, x_curr->y_pos, x_curr->z_pos);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

