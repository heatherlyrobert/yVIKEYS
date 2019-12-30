/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 *
 *     0-9            critical study, overlay, and focus, on top of everything
 *     a-z            are normal and independent
 *     è-ÿ            are underlayment and independent
 *
 *  default added to normal range
 *     layer outline  +     add to middle, rolls down others as needed
 *
 *  adding to a specific place
 *     layer entries  m     make layer m, if m exists, shift all up
 *     layer entries  M     make layer m, if m exists, shift all down
 *     layer baseline [     put on top of normal range
 *     layer baseline ]     put on lowest of normal range
 *     layer testsite <     put before/above last added of any range
 *     layer tunnels  >     put after/below last added of any range
 *
 *  acts on one of three ranges depending on mark
 *     layer a[       to top/front      within range (rolling as needed)
 *     layer a<       up/forward        within range (swapping as needed)
 *     layer a>       down/backward     within range (swapping as needed)
 *     layer a]       to bottom/back    within range (rolling as needed)
 *     layer ac       move a to c and shift c down
 *     layer aC       move a to c and shift c upward
 *
 *  move between ranges
 *     layer 5f       swap 5 and f between ranges
 *
 *  stop dislaying layer
 *     layer a#       remove mark from layer
 *     layer none     remove all marks
 *
 *
 *
 */

#define      MAX_LAYERS    75
typedef struct cLAYER  tLAYER;
struct cLAYER {
   uchar      *name;                   /* reference name                      */
   uchar       mark;                   /* one char sequencing mark            */
   char        (*drawer) (void);       /* function to display                 */
   uchar      *desc;                   /* helpful description                 */
   tLAYER     *m_prev;
   tLAYER     *m_next;
};

static tLAYER  *s_head     = NULL;
static tLAYER  *s_tail     = NULL;
static tLAYER  *s_curr     = NULL;
static int      s_max      = 0;
static int      s_count    = 0;
static int      s_index    = 0;
static uchar    s_layer    = 'm';



static uchar   s_valid [MAX_LAYERS];
static tLAYER *s_marks [MAX_LAYERS];



uchar     *g_stub   = "";



char       yvikeys_layer__unmark    (tLAYER *a_curr);
char       yvikeys_layer__mark      (tLAYER *a_curr, char a_pos);



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_layer_init       (void)
{
   int         i           =    0;
   s_head  = NULL;
   s_tail  = NULL;
   s_curr  = NULL;
   s_count = 0;
   s_index = 0;
   strlcpy (s_valid, gvikeys_number, MAX_LAYERS);
   strlcat (s_valid, gvikeys_lower , MAX_LAYERS);
   strlcat (s_valid, gvikeys_greek , MAX_LAYERS);
   s_max = strlen (s_valid);
   for (i = 0; i < s_max; ++i)  s_marks [i] = NULL;
   return 0;
}

char
yvikeys_layer_wrap       (void)
{
   yvikeys_layer_purge ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     validation helpers                       ----===*/
/*====================------------------------------------====================*/
static void  o___HELPERS_________o () { return; }

char
yvikeys_layer__valid    (uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   uchar      *p           = NULL;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_sint    (a_abbr);
   --rce;  if (a_abbr == 0) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(validate)-----------------------*/
   p = strchr (s_valid, a_abbr);
   DEBUG_GRAF   yLOG_spoint  (p);
   --rce;  if (p == NULL) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_schar   (a_abbr);
   /*---(find index)---------------------*/
   n = p - s_valid;
   DEBUG_GRAF   yLOG_sint    (n);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return n;
}

char
yvikeys_layer__name     (uchar *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   uchar       x_ch        =  '-';
   uchar      *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-";
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_snote   (a_name);
   x_len = strlen (a_name);
   DEBUG_GRAF   yLOG_sint    (x_len);
   --rce;  if (x_len <= 2) {
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  for (i = 0; i < x_len; ++i) {
      x_ch = a_name [i];
      DEBUG_GRAF   yLOG_schar   (x_ch);
      if (strchr (x_valid, x_ch) != NULL)  continue;
      DEBUG_GRAF   yLOG_snote   ("illegal");
      DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char
yvikeys_layer_cursor    (char a_move)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_enter   (__FUNCTION__);
   DEBUG_HIST   yLOG_char    ("a_move"    , a_move);
   /*---(defenses)-----------------------*/
   DEBUG_HIST   yLOG_point   ("s_head"  , s_head);
   --rce;  if (s_head == NULL) {
      s_curr = NULL;
      s_index = -1;
      DEBUG_HIST   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_HIST   yLOG_point   ("s_curr"  , s_curr);
   if (s_curr == NULL) {
      s_curr = s_head;
      s_index = 0;
      DEBUG_HIST   yLOG_point   ("fixed"     , s_curr);
   }
   DEBUG_HIST   yLOG_value   ("s_index"  , s_index);
   /*---(handle move)--------------------*/
   DEBUG_HIST   yLOG_note    ("check special shortcuts");
   switch (a_move) {
   case '[' :
      s_curr  = s_head;
      s_index = 0;
      break;
   case '<' :
      s_curr  = s_curr->m_prev;
      --(s_index);
      break;
   case '>' :
      s_curr  = s_curr->m_next;
      ++(s_index);
      break;
   case ']' :
      s_curr  = s_tail;
      s_index = s_count - 1;
      break;
   }
   /*---(safeties)-----------------------*/
   DEBUG_HIST   yLOG_point   ("s_curr"   , s_curr);
   if (s_curr == NULL) {
      switch (a_move) {
      case '<' :
         DEBUG_HIST   yLOG_note    ("bounced off head");
         s_curr = s_head;
         s_index = 0;
         rc = 1;
         break;
      case '>' :
         DEBUG_HIST   yLOG_note    ("bounced off tail");
         s_curr = s_tail;
         s_index = s_count - 1;
         rc = 1;
         break;
      }
   }
   /*---(output)-------------------------*/
   DEBUG_HIST   yLOG_point   ("s_curr"  , s_curr);
   DEBUG_HIST   yLOG_value   ("s_index"  , s_index);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_layer_find      (uchar *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tLAYER     *x_curr      = NULL;
   char        c           =   -1;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   rc = yvikeys_layer__name (a_name);
   DEBUG_GRAF   yLOG_value   ("name"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find the screen part)-----------*/
   x_curr = s_head;
   while (x_curr != NULL) {
      DEBUG_GRAF   yLOG_info    ("check"     , x_curr->name);
      ++c;
      if (x_curr->name [0] == a_name [0] && strcmp (x_curr->name, a_name) == 0)   break;
      x_curr = x_curr->m_next;
   }
   DEBUG_GRAF   yLOG_value   ("x_curr"    , x_curr);
   if (x_curr == NULL) {
      DEBUG_GRAF   yLOG_note    ("not found");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(set as current)-----------------*/
   DEBUG_GRAF   yLOG_note    ("setting current");
   s_curr  = x_curr;
   s_index = c;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 1;
}



/*====================------------------------------------====================*/
/*===----                        memory allocation                     ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char
yVIKEYS_dataset_add     (uchar *a_name, void *a_drawer, uchar *a_desc)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tLAYER     *x_new       = NULL;
   char        x_tries     =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_drawer"  , a_drawer);
   --rce;  if (a_drawer == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for dup)------------------*/
   rc = yvikeys_layer_find (a_name);
   DEBUG_GRAF   yLOG_value   ("find"      , rc);
   --rce;  if (rc !=  0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tLAYER *) malloc (sizeof (tLAYER));
   }
   DEBUG_GRAF   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_GRAF   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_GRAF   yLOG_note    ("FAILED");
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_GRAF   yLOG_note    ("populate");
   x_new->mark   = '-';
   x_new->name   = strdup (a_name);
   x_new->drawer = a_drawer;
   if (a_desc != NULL)  x_new->desc   = strdup (a_desc);
   else                 x_new->desc   = g_stub;
   x_new->m_next = NULL;
   x_new->m_prev = NULL;
   /*---(tie to master list)-------------*/
   if (s_head == NULL) {
      DEBUG_GRAF   yLOG_note    ("nothing in master list, make first");
      s_head         = x_new;
   } else  {
      DEBUG_GRAF   yLOG_note    ("append to master list");
      s_tail->m_next = x_new;
      x_new->m_prev  = s_tail;
   }
   s_tail        = x_new;
   /*---(update counts)------------------*/
   ++s_count;
   /*---(set current)--------------------*/
   s_curr  = x_new;
   s_index = s_count - 1;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_dataset_delete  (uchar *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   rc = yvikeys_layer_find (a_name);
   DEBUG_GRAF   yLOG_value   ("find"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(remove from list)---------------*/
   DEBUG_GRAF   yLOG_note    ("removing from master list");
   if (s_curr->m_prev != NULL)  s_curr->m_prev->m_next = s_curr->m_next;
   else                         s_head                 = s_curr->m_next;
   if (s_curr->m_next != NULL)  s_curr->m_next->m_prev = s_curr->m_prev;
   s_tail                 = s_curr->m_prev;
   /*---(free)---------------------------*/
   DEBUG_GRAF   yLOG_note    ("freeing name and desc");
   if (s_curr->name != g_stub)  free (s_curr->name);
   s_curr->name = NULL;
   if (s_curr->desc != g_stub)  free (s_curr->desc);
   s_curr->desc = NULL;
   /*---(update pointers)----------------*/
   DEBUG_GRAF   yLOG_note    ("updating pointers");
   s_curr = s_curr->m_prev;
   --s_index;
   if (s_curr == NULL) {
      s_curr  = s_head;
      s_index = 0;
   }
   /*---(update count)-------------------*/
   DEBUG_GRAF   yLOG_note    ("updating counts");
   --s_count;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_layer_purge      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tLAYER     *x_curr      = NULL;
   tLAYER     *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(walk and free)------------------*/
   x_curr = s_tail;
   while (x_curr != NULL) {
      x_next = x_curr->m_prev;
      if (x_curr->mark != '-')     yvikeys_layer__unmark (x_curr);
      if (x_curr->name != g_stub)  free (x_curr->name);
      x_curr->name = NULL;
      if (x_curr->desc != g_stub)  free (x_curr->desc);
      x_curr->desc = NULL;
      free (x_curr);
      x_curr = x_next;
   }
   /*---(reset static)-------------------*/
   s_head  = NULL;
   s_tail  = NULL;
   s_curr  = NULL;
   s_count = 0;
   s_index = 0;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       layer ordering                         ----===*/
/*====================------------------------------------====================*/
static void  o___ORDERING________o () { return; }

char
yvikeys_layer__unmark    (tLAYER *a_curr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_curr == NULL)  return rce;
   n = yvikeys_layer__valid (a_curr->mark);
   --rce;  if (n < 0)           return rce;
   /*---(unmark)-------------------------*/
   a_curr->mark = '-';
   s_marks [n]  = NULL;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_layer__none     (void)
{
   int         i           =    0;
   for (i = 0; i < s_max; ++i)  yvikeys_layer__unmark (s_marks [i]);
   return 0;
}

char
yvikeys_layer__mark      (tLAYER *a_curr, char a_pos)
{
   a_curr->mark    = s_valid [a_pos];
   s_marks [a_pos] = a_curr;
   return 0;
}

char
yvikeys_layer__add       (tLAYER *a_curr, char a_pos, char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   char        i           =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_senter  (__FUNCTION__);
   DEBUG_GRAF   yLOG_spoint  (a_curr);
   DEBUG_GRAF   yLOG_sint    (a_pos);
   /*---(spot is open)-------------------*/
   if (s_marks [a_pos] == NULL) {
      DEBUG_GRAF   yLOG_snote   ("spot is open, just assign");
      yvikeys_layer__unmark (a_curr);
   }
   /*---(move others fore/up)------------*/
   else if (a_dir == '<') {
      /*---(find open)---------*/
      for (i = a_pos; i >= 0; --i)  if (s_marks [i] == NULL)  break;
      /*---(no open)-----------*/
      if (i < 0) {
         DEBUG_GRAF   yLOG_snote   ("none open before/above");
         DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(safe to unmark)----*/
      yvikeys_layer__unmark (a_curr);
      /*---(shift up)----------*/
      for (n = i; n < a_pos; ++n) {
         s_marks [n] = s_marks [n + 1];
         (s_marks [n])->mark = s_valid [n];
      }
      /*---(done)--------------*/
   }
   /*---(move others back/down)----------*/
   else if (a_dir == '>') {
      /*---(find open)---------*/
      for (i = a_pos; i < s_max; ++i)  if (s_marks [i] == NULL)  break;
      /*---(no open)-----------*/
      if (i >= s_max) {
         DEBUG_GRAF   yLOG_snote   ("none open after/below");
         DEBUG_GRAF   yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(safe to unmark)----*/
      yvikeys_layer__unmark (a_curr);
      /*---(shift up)----------*/
      for (n = i; n > a_pos; --n) {
         s_marks [n] = s_marks [n - 1];
         (s_marks [n])->mark = s_valid [n];
      }
      /*---(done)--------------*/
   }
   /*---(assign)-------------------------*/
   yvikeys_layer__mark (a_curr, a_pos);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_layer__adds      (uchar *a_name, uchar a_action)
{
   /*---(design notes)-------------------*/
   /*
    *  default added to normal range
    *     layer outline  +     add to middle, rolls down others as needed
    *
    *  adding to a specific place
    *     layer entries  f     make layer f, if f exists, shift all up
    *     layer entries  P     make layer p, if p exists, shift all down
    *     layer baseline [     put on top of normal range
    *     layer baseline ]     put on lowest of normal range
    *     layer testsite <     put before/above last added of any range
    *     layer tunnels  >     put after/below last added of any range
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   uchar       x_layer     =  '-';
   char        x_dir       =  '>';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "%-10p, %c", a_name, a_action);
   /*---(check for layer)----------------*/
   rc = yvikeys_layer_find (a_name);
   DEBUG_GRAF   yLOG_value   ("find"      , rc);
   --rce;  if (rc <=  0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(actions)------------------------*/
   x_layer = s_layer;
   switch (a_action) {
   case '+' :
      x_layer = 'm';
      x_dir   = '>';
      break;
   case '[' :
      x_layer = 'a';
      x_dir   = '>';
      break;
   case '>' :
      ++x_layer;
      x_dir   = '>';
      break;
   case '<' :
      --x_layer;
      x_dir   = '<';
      break;
   case ']' :
      x_layer = 'z';
      x_dir   = '<';
      break;
   default  :
      x_layer = tolower (a_action);
      x_dir   = '>';
      if (a_action != tolower (a_action))  x_dir = '<';
      break;
   }
   /*---(fix layer)----------------------*/
   if (x_layer == '/')       x_layer = '0';
   if (x_layer == ':')       x_layer = 'a';
   if (x_layer == '`')       x_layer = '9';
   if (x_layer == 'Ù')       x_layer = 'z';
   if (x_layer == '{')       x_layer = 'è';
   if (x_layer ==  0 )       x_layer = 'ÿ';
   /*---(check mark)---------------------*/
   n  = yvikeys_layer__valid (x_layer);
   DEBUG_GRAF   yLOG_value   ("valid"     , n);
   if (n < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add layer)----------------------*/
   rc = yvikeys_layer__add   (s_curr, n, x_dir);
   DEBUG_GRAF   yLOG_value   ("add"       , rc);
   if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   s_layer = x_layer;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_layer__all      (void)
{
   int         x_beg       =    0;
   tLAYER     *x_curr      = NULL;
   yvikeys_layer__none ();;
   x_beg  = 23 - (s_count / 2);
   x_curr = s_head;
   while (x_curr != NULL) {
      if (x_curr == s_head)  yvikeys_layer__adds (x_curr->name, s_valid [x_beg]);
      else                   yvikeys_layer__adds (x_curr->name, '>');
      x_curr = x_curr->m_next;
   }
   return 0;
}

char
yvikeys_layer_action     (uchar *a_name, uchar *a_action)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_name      [LEN_LABEL] = "";
   uchar       x_action    =  '-';
   char        n           =    0;
   tLAYER     *x_curr      = NULL;
   uchar       x_layer     =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "name %-10p, action %-10p", a_name, a_action);
   /*---(defense)------------------------*/
   --rce;  if (a_name == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_name"    , a_name);
   /*---(figure out subject)-------------*/
   x_len = strlen (a_name);
   DEBUG_GRAF   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len < 1) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle current)-----------------*/
   if (x_len == 1) {
      DEBUG_GRAF   yLOG_note    ("identified current run");
      /*---(check for mark)-------*/
      n = yvikeys_layer__valid (a_name [0]);
      DEBUG_GRAF   yLOG_value   ("valid"     , n);
      --rce;  if (n <  0) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_curr = s_marks [n];
      DEBUG_GRAF   yLOG_point   ("x_curr"    , x_curr);
      --rce;  if (x_curr == NULL) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = yvikeys_layer_find (x_curr->name);
      DEBUG_GRAF   yLOG_value   ("find"      , rc);
      --rce;  if (rc !=  0) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle moves)-------------------*/
   else if (x_len == 2) {
      DEBUG_GRAF   yLOG_note    ("identified abbreviation run");
      /*---(check for mark)-------*/
      n = yvikeys_layer__valid (a_name [0]);
      DEBUG_GRAF   yLOG_value   ("valid"     , n);
      --rce;  if (n <  0) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_curr = s_marks [n];
      DEBUG_GRAF   yLOG_point   ("x_curr"    , x_curr);
      --rce;  if (x_curr == NULL) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_layer = a_name [0];
      yvikeys_layer__unmark (x_curr);
      strlcpy (x_name, x_curr->name, LEN_LABEL);
      /*---(copy action)-------*/
      x_action = a_name [1];
      if      (x_action == '#') {
         DEBUG_GRAF   yLOG_note    ("just unmarking, all done");
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if      (x_action == '<')     x_action = --x_layer;
      else if (x_action == '>')     x_action = ++x_layer;
      if (x_action == '/')          x_action = '0';
      if (x_action == ':')          x_action = 'a';
      if (x_action == '`')          x_action = '9';
      if (x_action == (uchar) 'ç')  x_action = 'z';
      if (x_action == '{')          x_action = 'è';
      if (x_action ==  0 )          x_action = 'ÿ';
      /*---(done)--------------*/
   }
   /*---(handle none)--------------------*/
   else if (strcmp (a_name, "none") == 0) {
      DEBUG_GRAF   yLOG_note    ("identified remove all run");
      yvikeys_layer__none ();
   }
   /*---(handle all)---------------------*/
   else if (strcmp (a_name, "all") == 0) {
      DEBUG_GRAF   yLOG_note    ("identified add all run");
      yvikeys_layer__all ();
   }
   /*---(handle adds)--------------------*/
   else {
      DEBUG_GRAF   yLOG_note    ("identified named layer run");
      /*---(copy name)---------*/
      strlcpy (x_name, a_name, LEN_LABEL);
      /*---(check action)------*/
      --rce;  if (a_action == NULL) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_GRAF   yLOG_info    ("a_action"  , a_action);
      x_len = strlen (a_action);
      DEBUG_GRAF   yLOG_value   ("x_len"     , x_len);
      --rce;  if (x_len != 1) {
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_action = a_action [0];
      /*---(done)--------------*/
   }
   /*---(handle adds)--------------------*/
   rc = yvikeys_layer__adds (x_name, x_action);
   if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      display handling                        ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char         /*-> linear list of marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_layer__line          (char *a_list, char a_range)
{
   char        n           =    0;
   char        c           =    0;
   char        s           [LEN_TERSE];
   char        x_beg       =    0;
   char        x_end       =    0;
   switch (a_range) {
   case 'F' :  x_beg =  0; x_end = s_max - 1;  break;
   case '0' :  x_beg =  0; x_end = 9;          break;
   case 'a' :  x_beg = 10; x_end = 35;         break;
   case 'è' :  x_beg = 36; x_end = s_max - 1;  break;
   }
   strlcpy (a_list, "", LEN_HUND);
   for (n = x_beg; n <= x_end; ++n) {
      if (a_range == 'F' && n == 10)  strlcat (a_list, " ", LEN_HUND);
      if (a_range == 'F' && n == 36)  strlcat (a_list, " ", LEN_HUND);
      if (s_marks [n] != NULL)  { sprintf (s, "%c" ,s_valid [n]);  strlcat (a_list, s, LEN_HUND); ++c; }
      else                        strlcat (a_list, "-"        , LEN_HUND);
   }
   return c;
}

char
yvikeys_layer_status         (char *a_line)
{
   char        x_list      [LEN_HUND] = "";
   char        c           =    0;
   int         x_wide      =    0;
   char        x_mark      =  '-';
   char        x_name      [LEN_LABEL] = "";
   if (s_curr != NULL) {
      x_mark = s_curr->mark;
      strlcpy (x_name, s_curr->name, LEN_LABEL);
   }
   switch (myVIKEYS.status_w) {
   case 'L' :
      c = yvikeys_layer__line (x_list, 'F');
      snprintf (a_line, LEN_FULL, "layer %2d %s %2d %c %-12.12s ´", c, x_list, s_count, x_mark, x_name);
      break;
   case 'M' :
      c = yvikeys_layer__line (x_list, 'a');
      snprintf (a_line, LEN_FULL, "layer %2d %s %2d %c %-12.12s ´", c, x_list, s_count, x_mark, x_name);
      break;
   case 'S' :
      c = yvikeys_layer__line (x_list, 'a');
      snprintf (a_line, LEN_FULL, "´%s´", x_list);
      break;
   }
   return 0;
}

char
yvikeys_layer_show       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tLAYER     *x_curr      = NULL;
   char        n           =    0;
   for (n = 0; n < s_max; ++n) {
      x_curr = s_marks [n];
      if (x_curr == NULL)          continue;
      if (x_curr->drawer == NULL)  continue;
      x_curr->drawer ();
   }
   return 0;
}

char
yvikeys_layer_list       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tLAYER     *x_curr      = NULL;
   char        c           =    0;
   /*---(find the screen part)-----------*/
   printf ("\n\nlayer inventory...\n");
   x_curr = s_head;
   while (x_curr != NULL) {
      printf ("  %2d, on %c, name %-12.12s, drawer %p\n", c, x_curr->mark, x_curr->name, x_curr->drawer);
      x_curr = x_curr->m_next;
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          unit testing                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }


char yvikeys_layer__unit_null (void)   { return 0; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_layer__unit     (char *a_question, uchar *a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   tLAYER     *x_curr      = NULL;
   int         n           =    0;
   int         c           =    0;
   char        s           [LEN_LABEL];
   char        t           [LEN_HUND]  = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "LAYER unit       : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "count"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER count      : %d", s_count);
   }
   else if (strcmp (a_question, "entry"          ) == 0) {
      n = a_key;
      x_curr = s_head;
      while (x_curr != NULL) {
         if (c == n) break;
         x_curr = x_curr->m_next;
         ++c;
      }
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER entry (%2d) : -                      -            -", n);
      else                 snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER entry (%2d) : %-20.20s   %-10p   %c", n, x_curr->name, x_curr->drawer, x_curr->mark);
   }
   else if (strcmp (a_question, "current"        ) == 0) {
      if (s_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER current    : -                      -            -");
      else                 snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER current    : %-20.20s   %-10p   %c", s_curr->name, s_curr->drawer, s_curr->mark);
   }
   else if (strcmp (a_question, "layer"          ) == 0) {
      n = yvikeys_layer__valid (a_key [0]);
      if (n >= 0)   x_curr = s_marks [n];
      if (x_curr == NULL)  snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER entry (%2d) : -                      -            -", n);
      else                 snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER entry (%2d) : %-20.20s   %-10p   %c", n, x_curr->name, x_curr->drawer, x_curr->mark);
   }
   else if (strcmp (a_question, "layers"         ) == 0) {
      for (n = 0; n < s_max; ++n) {
         if (n == 10)  strlcat (t, " ", LEN_HUND);
         if (n == 36)  strlcat (t, " ", LEN_HUND);
         if (s_marks [n] != NULL)  { sprintf (s, "%c" ,s_valid [n]);  strlcat (t, s, LEN_HUND); ++c; }
         else                        strlcat (t, "-"        , LEN_HUND);
      }
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER layers(%2d) : %s", c, t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



