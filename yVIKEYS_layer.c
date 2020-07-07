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

#define      MAX_NOTES     10
static struct {
   char        xr, yr, size;
   short       x, y;
   uchar       w, h;
   char       *text;
   char        type;
   short       xt, yt;
   short       xb, yb;
   short       xe, ye;
} s_notes [MAX_NOTES];
static char s_nnote = 0;




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
   yvikeys_note__purge ('y');
   return 0;
}

char
yvikeys_layer_wrap       (void)
{
   yvikeys_layer_purge ();
   yvikeys_note__purge ('-');
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
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex ("args"      , "%-10.10p, %2d, %c", a_curr, a_pos, a_dir);
   /*---(spot is open)-------------------*/
   if (s_marks [a_pos] == NULL) {
      DEBUG_GRAF   yLOG_note    ("spot is open, just assign");
      yvikeys_layer__unmark (a_curr);
   }
   /*---(move others fore/up)------------*/
   else if (a_dir == '<') {
      DEBUG_GRAF   yLOG_note    ("spot filled, shift up");
      /*---(find open)---------*/
      for (i = a_pos; i >= 0; --i)  if (s_marks [i] == NULL)  break;
      DEBUG_GRAF   yLOG_value   ("i"         , i);
      /*---(no open)-----------*/
      if (i < 0) {
         DEBUG_GRAF   yLOG_note    ("none open before/above");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(safe to unmark)----*/
      DEBUG_GRAF   yLOG_note    ("unmark current");
      yvikeys_layer__unmark (a_curr);
      /*---(shift up)----------*/
      DEBUG_GRAF   yLOG_note    ("shift the ones above");
      for (n = i; n < a_pos; ++n) {
         s_marks [n] = s_marks [n + 1];
         (s_marks [n])->mark = s_valid [n];
      }
      /*---(done)--------------*/
   }
   /*---(move others back/down)----------*/
   else if (a_dir == '>') {
      DEBUG_GRAF   yLOG_note    ("spot filled, shift down");
      /*---(find open)---------*/
      for (i = a_pos; i < s_max; ++i)  if (s_marks [i] == NULL)  break;
      /*---(no open)-----------*/
      if (i >= s_max) {
         DEBUG_GRAF   yLOG_note    ("none open after/below");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
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
   DEBUG_GRAF   yLOG_note    ("now, assign the current");
   yvikeys_layer__mark (a_curr, a_pos);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
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
   case '_' : /* move to top     */ x_layer = '0'; x_dir   = '>';  break;
   case '[' : /* move to head    */ x_layer = 'a'; x_dir   = '>';  break;
   case '>' : /* shove right     */ ++x_layer;     x_dir   = '>';  break;
   case ')' : /* swap with right */ ++x_layer;     x_dir   = '<';  break;
   case '+' : /* put in middle   */ x_layer = 'm'; x_dir   = '>';  break;
   case '(' : /* swap with left  */ --x_layer;     x_dir   = '>';  break;
   case '<' : /* shove left      */ --x_layer;     x_dir   = '<';  break;
   case ']' : /* move to tail    */ x_layer = 'z'; x_dir   = '<';  break;
   case '~' : /* move to bottom  */ x_layer = 'ÿ'; x_dir   = '<';  break;
   default  : /* specific mark   */
                                    x_layer = tolower (a_action);
                                    x_dir   = '>';
                                    if (a_action != tolower (a_action))  x_dir = '<';
                                    break;
   }
   /*---(fix layer)----------------------*/
   if (x_layer == '/')          x_layer = '0';
   if (x_layer == ':')          x_layer = 'a';
   if (x_layer == '`')          x_layer = '9';
   if (x_layer == (uchar) 'ç')  x_layer = 'z';
   if (x_layer == '{')          x_layer = 'è';
   if (x_layer ==  0 )          x_layer = 'ÿ';
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
         if (strchr ("_[<(+)>]~", a_name [0]) == NULL) {
            DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         if (s_curr == NULL) {
            DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         DEBUG_GRAF   yLOG_note    ("handling a shift");
         s_layer = s_curr->mark;
         yvikeys_layer__unmark (s_curr);
         strlcpy (x_name, s_curr->name, LEN_LABEL);
         x_action = a_name [0];
         DEBUG_GRAF   yLOG_info    ("x_name"    , x_name);
         DEBUG_GRAF   yLOG_char    ("x_action"  , x_action);
      } else {
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
      /*> x_layer = a_name [0];                                                       <*/
      s_layer = a_name [0];
      yvikeys_layer__unmark (x_curr);
      strlcpy (x_name, x_curr->name, LEN_LABEL);
      /*---(copy action)-------*/
      x_action = a_name [1];
      if      (x_action == '#') {
         DEBUG_GRAF   yLOG_note    ("just unmarking, all done");
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return 0;
      }
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
/*===----                       annotated notes                        ----===*/
/*====================------------------------------------====================*/
static void  o___ANNOTATE________o () { return; }

char
yvikeys_note__size      (char a_type, char n, char xr, char yr, char a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left, x_wide, x_bott, x_tall;
   float       a, b;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(total size)---------------------*/
   switch (a_type) {
   case YVIKEYS_OPENGL :
      yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
      switch (a_size) {
      case '-'  : s_notes [n].w = 100; s_notes [n].h =  30;  break;
      case '='  : s_notes [n].w = 175; s_notes [n].h =  60;  break;
      case '+'  : s_notes [n].w = 250; s_notes [n].h =  80;  break;
      }
      break;
   case YVIKEYS_CURSES :
      yVIKEYS_view_size   (YVIKEYS_MAIN, &x_left, &x_wide, &x_bott, &x_tall, NULL);
      switch (a_size) {
      case '-'  : s_notes [n].w =  10; s_notes [n].h =   1;  break;
      case '='  : s_notes [n].w =  25; s_notes [n].h =   3;  break;
      case '+'  : s_notes [n].w =  40; s_notes [n].h =   4;  break;
      }
      break;
   }
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   /*---(vertical)-----------------------*/
   a = (8 - (yr - '1')) * (1.0/8.0);
   b = 1 - a;
   if (a_type == YVIKEYS_OPENGL)  {
      s_notes [n].y  = x_bott + (x_tall * a) + (s_notes [n].h * b);
   } else {
      s_notes [n].y  = x_bott - (x_tall * a) - (s_notes [n].h * b) + 1;
   }
   /*---(horizontal)---------------------*/
   a = (xr - '1') * (1.0/8.0);
   s_notes [n].x  = x_left + (x_wide * a) - (s_notes [n].w * a);
   /*> if (s_notes [n].x + s_notes [n].w >= x_left + x_wide)  s_notes [n].x = x_left + x_wide - s_notes [n].w - 1;   <*/
   /*---(report-out)---------------------*/
   DEBUG_GRAF   yLOG_complex  ("size"      , "%4dx, %4dy, %4dw, %4dh", s_notes [n].x, s_notes [n].y, s_notes [n].w, s_notes [n].h);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__grid      (char a_type, char n, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_inc , x_bump, y_inc , y_bump;
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_char    ("xt"        , xt);
   --rce;  if (xt == 0 || strchr (x_valid, xt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yt"        , yt);
   --rce;  if (xt == 0 || strchr (x_valid, yt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(default)------------------------*/
   if (xt == '-' || yt == '-' || a_type == YVIKEYS_CURSES) {
      s_notes [n].xb = s_notes [n].yb = 0;
      s_notes [n].xe = s_notes [n].ye = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(total size)---------------------*/
   yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   /*---(horizontal)---------------------*/
   s_notes [n].xb = s_notes [n].x + s_notes [n].w * 0.50;
   x_inc  = x_wide / 25;
   x_bump = x_inc * 0.50;
   s_notes [n].xe = x_left + (tolower (xt) - 'a') * x_inc;
   if (xt != tolower (xt))    s_notes [n].xe += x_bump;
   /*---(vertical)-----------------------*/
   s_notes [n].yb = s_notes [n].y - s_notes [n].h * 0.50;
   y_inc  = x_tall / 25;
   x_bump = y_inc * 0.50;
   s_notes [n].ye = x_bott + x_tall - (tolower (yt) - 'a') * y_inc;
   if (yt != tolower (yt))    s_notes [n].ye -= x_bump;
   /*---(complete)-----------------------*/
   s_notes [n].type = '*';
   /*---(report-out)---------------------*/
   DEBUG_GRAF   yLOG_complex  ("endpoint"  , "%4dx, %4dy, %4dx, %4dy", s_notes [n].xb, s_notes [n].yb, s_notes [n].xe, s_notes [n].ye);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__radial    (char a_type, char n, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_inc , x_bump, y_inc , y_bump;
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyz";
   float       x_rad       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("xt"        , xt);
   --rce;  if (xt <= 0 || xt > 360) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yt"        , yt);
   --rce;  if (xt == 0 || strchr (x_valid, yt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(default)------------------------*/
   if (xt == 0 || yt == '-' || a_type == YVIKEYS_CURSES) {
      s_notes [n].xb = s_notes [n].yb = 0;
      s_notes [n].xe = s_notes [n].ye = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(total size)---------------------*/
   yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   /*---(vertical)-----------------------*/
   s_notes [n].yb = s_notes [n].y - s_notes [n].h * 0.50;
   x_tall /= 2.0;
   y_inc  = x_tall / 25;
   y_bump = y_inc * 0.25;
   x_rad  = (yt - 'a') * y_inc - y_bump;
   /*---(horizontal)---------------------*/
   s_notes [n].xb = s_notes [n].x + s_notes [n].w * 0.50;
   s_notes [n].xe = x_rad * cos ((90 - xt) * DEG2RAD);
   s_notes [n].ye = x_rad * sin ((90 - xt) * DEG2RAD);
   /*---(complete)-----------------------*/
   s_notes [n].type = '>';
   /*---(report-out)---------------------*/
   DEBUG_GRAF   yLOG_complex  ("endpoint"  , "%4dx, %4dy, %4dx, %4dy", s_notes [n].xb, s_notes [n].yb, s_notes [n].xe, s_notes [n].ye);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__clear     (char a_init, char n)
{
   /*---(master)-------------------------*/
   if (a_init != 'y' && s_notes [n].xr != '0' && s_notes [n].text != NULL) free (s_notes [n].text);
   s_notes [n].text = NULL;
   s_notes [n].xr   = '0';
   s_notes [n].yr   = '0';
   s_notes [n].size = '=';
   /*---(position)-----------------------*/
   s_notes [n].x    = 0;
   s_notes [n].y    = 0;
   s_notes [n].w    = 0;
   s_notes [n].h    = 0;
   /*---(target)-------------------------*/
   s_notes [n].type = '-';
   s_notes [n].xt   = 0;
   s_notes [n].yt   = 0;
   s_notes [n].xb   = 0;
   s_notes [n].yb   = 0;
   s_notes [n].xe   = 0;
   s_notes [n].ye   = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_note__purge     (char a_init)
{
   int         i           =    0;
   for (i = 0; i < MAX_NOTES; ++i) {
      yvikeys_note__clear (a_init, i);
   }
   s_nnote = 0;
   return 0;
}

char
yvikeys_note__find      (char xr, char yr)
{
   int         i           =    0;
   for (i = 0; i < s_nnote; ++i) {
      if (s_notes [i].xr != xr)  continue;
      if (s_notes [i].yr != yr)  continue;
      return i;
   }
   return -1;
}

char
yvikeys_note__append_OLD(char xr, char yr, char a_size, char *a_text, char a_type, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   int         x_len       =    0;
   char        t           [LEN_TERSE] = "";
   char        u           [LEN_RECD]  = "";
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("s_nnote"   , s_nnote);
   --rce;  if (s_nnote >= MAX_NOTES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("xr"        , xr);
   --rce;  if (xr == 0 || strchr ("123456789", xr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yr"        , yr);
   --rce;  if (yr == 0 || strchr ("123456789", yr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("a_size"    , a_size);
   --rce;  if (a_size == 0 || strchr ("-=+", a_size) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL || a_text [0] == '\0') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   DEBUG_GRAF   yLOG_value   ("xt"        , xt);
   DEBUG_GRAF   yLOG_value   ("yt"        , yt);
   s_notes [s_nnote].xr   = xr;
   s_notes [s_nnote].yr   = yr;
   s_notes [s_nnote].size = a_size;
   s_notes [s_nnote].text = strdup (a_text);
   yvikeys_note__size   (myVIKEYS.env, s_nnote, xr, yr, a_size);
   s_notes [s_nnote].xt   = xt;
   s_notes [s_nnote].yt   = yt;
   switch (a_type) {
   case '*' : yvikeys_note__grid   (myVIKEYS.env, s_nnote, xt, yt); break;
   case '>' : yvikeys_note__radial (myVIKEYS.env, s_nnote, xt, yt); break;
              /*> case '@' : yvikeys_note__exact  (myVIKEYS.env, s_nnote, xt, yt); break;        <*/
              /*> case '~' : yvikeys_note__curr   (myVIKEYS.env, s_nnote, xt, yt); break;        <*/
   }
   ++s_nnote;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__append    (char n, char xr, char yr, char a_size, char *a_text)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        t           [LEN_TERSE] = "";
   char        u           [LEN_RECD]  = "";
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   DEBUG_GRAF   yLOG_value   ("s_nnote"   , s_nnote);
   --rce;  if (n < 0 || n >= MAX_NOTES || n > s_nnote) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("xr"        , xr);
   --rce;  if (xr == 0 || strchr ("123456789", xr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yr"        , yr);
   --rce;  if (yr == 0 || strchr ("123456789", yr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("a_size"    , a_size);
   --rce;  if (a_size == 0 || strchr ("-=+", a_size) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL || a_text [0] == '\0') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   s_notes [n].xr   = xr;
   s_notes [n].yr   = yr;
   s_notes [n].size = a_size;
   if (s_notes [n].text != NULL)  free (s_notes [n].text);
   s_notes [n].text = strdup (a_text);
   yvikeys_note__size   (myVIKEYS.env, n, xr, yr, a_size);
   if (n == s_nnote)  ++s_nnote;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__remove    (char n)
{
   char        rce         =  -10;
   int         i           =    0;
   --rce;  if (n < 0 || n > MAX_NOTES)  return rce;
   --rce;  if (n >= s_nnote)            return rce;
   if (s_notes [n].text != NULL)  free (s_notes [n].text);
   for (i = n; i < s_nnote; ++i) {
      s_notes [i].xr   = s_notes [i + 1].xr;
      s_notes [i].yr   = s_notes [i + 1].yr;
      s_notes [i].size = s_notes [i + 1].size;
      s_notes [i].x    = s_notes [i + 1].x;
      s_notes [i].y    = s_notes [i + 1].y;
      s_notes [i].w    = s_notes [i + 1].w;
      s_notes [i].h    = s_notes [i + 1].h;
      s_notes [i].text = s_notes [i + 1].text;
      s_notes [i].xt   = s_notes [i + 1].xt;
      s_notes [i].yt   = s_notes [i + 1].yt;
      s_notes [i].xb   = s_notes [i + 1].xb;
      s_notes [i].yb   = s_notes [i + 1].yb;
      s_notes [i].xe   = s_notes [i + 1].xe;
      s_notes [i].ye   = s_notes [i + 1].ye;
      if (i + 1 < MAX_NOTES)  yvikeys_note__clear ('y', i + 1);
   }
   --s_nnote;
   return 0;
}

char
yvikeys_note__totop     (char n)
{
   char        rce         =  -10;
   int         i           =    0;
   --rce;  if (n < 0 || n > MAX_NOTES)  return rce;
   --rce;  if (n >= s_nnote)            return rce;
   s_notes [s_nnote].xr   = s_notes [n].xr;
   s_notes [s_nnote].yr   = s_notes [n].yr;
   s_notes [s_nnote].size = s_notes [n].size;
   s_notes [s_nnote].x    = s_notes [n].x;
   s_notes [s_nnote].y    = s_notes [n].y;
   s_notes [s_nnote].w    = s_notes [n].w;
   s_notes [s_nnote].h    = s_notes [n].h;
   s_notes [s_nnote].text = s_notes [n].text;
   s_notes [s_nnote].xt   = s_notes [n].xt;
   s_notes [s_nnote].yt   = s_notes [n].yt;
   s_notes [s_nnote].xb   = s_notes [n].xb;
   s_notes [s_nnote].yb   = s_notes [n].yb;
   s_notes [s_nnote].xe   = s_notes [n].xe;
   s_notes [s_nnote].ye   = s_notes [n].ye;
   s_notes [n].text = NULL;
   ++s_nnote;
   yvikeys_note__remove (n);
   return 0;
}

char
yvikeys_note_resize     (void)
{
   int         i           =    0;
   for (i = 0; i < s_nnote; ++i) {
      yvikeys_note__size (myVIKEYS.env, i, s_notes [i].xr, s_notes [i].yr, s_notes [i].size);
   }
   return 0;
}

char
yvikeys_note__move      (char n, char xr, char yr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n < 0 || n >= s_nnote) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_value   ("xr"        , xr);
   --rce;  if (strchr ("123456789", xr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_value   ("yr"        , yr);
   --rce;  if (strchr ("123456789", yr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_notes [n].xr   = xr;
   s_notes [n].yr   = yr;
   yvikeys_note__size   (myVIKEYS.env, n, xr, yr, s_notes [n].size);
   yvikeys_note__retarg (n);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__note_pos_check (char xr, char yr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(verify)-------------------------*/
   --rce;  if (xr == 0 || strchr ("123456789", xr) == NULL)   return rce;
   --rce;  if (yr == 0 || strchr ("123456789", yr) == NULL)   return rce;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_note__settarg   (char n, char *p)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        u           [LEN_LABEL] = "";
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char        x_type      =  '-';
   short       xt          =  '-';
   short       yt          =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (myVIKEYS.env == YVIKEYS_CURSES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   strlcpy (u, p, LEN_LABEL);
   x_len = strlen (u);
   DEBUG_GRAF   yLOG_complex ("tail"      , "%2d[%s]", x_len, u);
   rc = -1;
   x_type = u [0];;
   switch (x_type) {
   case '*' :  /* 25 x 25 grid point (a-y)*/
      if (x_len != 3) break;
      DEBUG_GRAF   yLOG_note    ("25 x 25 grid type");
      xt     = u [1];
      yt     = u [2];
      if (strchr (x_valid, xt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("xt fail"   , xt);
         break;
      }
      if (strchr (x_valid, yt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("yt fail"   , yt);
         break;
      }
      rc     = 0;
      DEBUG_GRAF   yLOG_complex ("ready"     , "%c %c %c", x_type, xt, yt);
      yvikeys_note__grid   (myVIKEYS.env, n, xt, yt);
      break;
   case '@' :  /* radial point */
      DEBUG_GRAF   yLOG_note    ("radial point");
      if (x_len < 3 || x_len > 5)           break;
      yt     = u [x_len];
      u [x_len]  = '\0';
      if (strchr (x_valid, yt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("yt fail"   , yt);
         break;
      }
      xt = atoi (u + 1);
      if (xt <= 0) {
         DEBUG_GRAF   yLOG_value   ("xt fail"   , xt);
         break;
      }
      rc     = 0;
      DEBUG_GRAF   yLOG_complex ("ready"     , "%c %4d %c", x_type, xt, yt);
      break;
   case '#' :
      DEBUG_GRAF   yLOG_note    ("exact point");
      break;
   case '~' :
      DEBUG_GRAF   yLOG_note    ("current point");
      break;
   default  :
      break;
   }
   /*---(update)-------------------------*/
   s_notes [n].type = x_type;
   s_notes [n].xt   = xt;
   s_notes [n].yt   = yt;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__retarg    (char n)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_LABEL] = "";
   switch (s_notes [n].type) {
   case '*' :
      sprintf (t, "%c%c%c", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);
      break;
   case '@' :
      sprintf (t, "%c%d%c", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);
      break;
   }
   yvikeys_note__settarg (n, t);
   return 0;
}

char
yvikeys_note            (char *a_all)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   int         x_len       =    0;
   char        u           [LEN_RECD]  = "";
   char       *v           = NULL;
   char       *p           = NULL;
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char        x_type      =  '-';
   char        xr          =  '-';
   char        yr          =  '-';
   char        x_size      =  '=';
   short       xt          =  '-';
   short       yt          =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_all"     , a_all);
   --rce;  if (a_all == NULL || a_all [0] == '\0') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_all"     , a_all);
   x_len = strlen (a_all);
   DEBUG_GRAF   yLOG_value   ("x_len"     , x_len);
   /*---(clear all)----------------------*/
   if (a_all [0] == '-') {
      DEBUG_GRAF   yLOG_note    ("selected a full purge");
      yvikeys_note__purge ('-');
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(defense)------------------------*/
   xr = a_all [0];
   yr = a_all [1];
   rc = yvikeys__note_pos_check (xr, yr);
   --rce;  if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   n = yvikeys_note__find (xr, yr);
   /*---(handle)-------------------------*/
   x_size = a_all [2];
   strlcpy (u, a_all + 3, LEN_RECD);
   v = a_all + 2;
   --rce;  switch (x_size) {
   case  0  :   /* move to top */
      rc = yvikeys_note__totop (n);
      break;
   case '#' :   /* delete/remove specific note */
      rc = yvikeys_note__remove (n);
      break;
   case '>' :   /* update base location of note */
      if (n >= 0 && x_len == 5) {
         rc = yvikeys_note__move (n, u [0], u [1]);
         rc = yvikeys_note__totop (n);
      }
      else  rc = rce;
      break;
   case '-' : case '=' : case '+' :  /* add or change a note */
      p = strrchr (u, ' ');
      DEBUG_GRAF   yLOG_point   ("p"         , p);
      if (p != NULL) {
         DEBUG_GRAF   yLOG_char    ("p[0]"      , p [1]);
         if (strchr ("*@!$~", p [1]) != NULL)  {
            v = p + 1;;
            p [0]  = '\0';   /* trim from text */
         }
      } else v = NULL;
      DEBUG_GRAF   yLOG_point   ("v"         , v);
      if (n <  0)  n = s_nnote;
      DEBUG_GRAF   yLOG_value   ("n"         , n);
      DEBUG_GRAF   yLOG_info    ("u"         , u);
      yvikeys_note__append (n, xr, yr, x_size, u);
      if (v == NULL) {
         rc = yvikeys_note__totop (n);
         break;
      }
      DEBUG_GRAF   yLOG_info    ("v"         , v);
   case '*' : case '@' : case '!' : case '$' : case '~' :
      if (n < 0) {  rc = rce; break; }
      rc = yvikeys_note__settarg (n, v);
      /*> if (myVIKEYS.env == YVIKEYS_CURSES)  xt = yt = '-';                                                 <* 
       *> s_notes [n].xt   = xt;                                                                              <* 
       *> s_notes [n].yt   = yt;                                                                              <* 
       *> switch (x_size) {                                                                                   <* 
       *> case '*' : yvikeys_note__grid   (myVIKEYS.env, n, xt, yt); break;                                   <* 
       *> case '>' : yvikeys_note__radial (myVIKEYS.env, n, xt, yt); break;                                   <* 
       *>            /+> case '@' : yvikeys_note__exact  (myVIKEYS.env, s_nnote, xt, yt); break;        <+/   <* 
       *>            /+> case '~' : yvikeys_note__curr   (myVIKEYS.env, s_nnote, xt, yt); break;        <+/   <* 
       *> }                                                                                                   <*/
      rc = yvikeys_note__totop (n);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note_target     (int xb, int yb, int xe, int ye, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_LABEL];
   int         x_align     =    0;
   int         x_cen, x_mid;
   int         x_len       =    0;
   int         x_edge      =    8;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(base line)----------------------*/
   glColor4f    (0.34f, 0.34f, 0.34f, 1.00f);
   glLineWidth   (4.0);
   glBegin       (GL_LINES); {
      glVertex3f   (xb, yb, z);
      glVertex3f   (xe, ye, z);
   } glEnd();
   /*---(dotted overwrite)---------------*/
   glColor4f    (0.67f, 0.67f, 0.67f, 1.00f);
   glLineWidth   (4.0);
   glEnable      (GL_LINE_STIPPLE);
   glLineStipple (1, 0xf0f0);
   glBegin      (GL_LINES); {
      glVertex3f   (xb, yb, z + 2);
      glVertex3f   (xe, ye, z + 2);
   } glEnd ();
   glDisable     (GL_LINE_STIPPLE);
   /*---(dotted overwrite)---------------*/
   /*> glColor4f    (1.00f, 1.00f, 1.00f, 1.00f);                                     <* 
    *> glLineWidth   (1.0);                                                           <* 
    *> glBegin      (GL_LINES); {                                                     <* 
    *>    glVertex3f   (xb, yb, z + 4);                                               <* 
    *>    glVertex3f   (xe, ye, z + 4);                                               <* 
    *> } glEnd ();                                                                    <*/
   /*---(reset)--------------------------*/
   glLineWidth   (1.0);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note_text       (char a_type, char a_len, char a_lvl, char *a_text, int x, int y, int w, int h, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD];
   int         x_align     =    0;
   int         x_full, x_len;
   int         x_beg, x_next;
   int         x_edge      =    8;
   int         i, j;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      DEBUG_GRAF   yLOG_complex  ("opengl"    , "%4dx, %4dy, %4dw, %4dh", x, y, w, h);
      x_edge = 20;
      glPushMatrix(); {
         glTranslatef (x + w * 0.5, y - 18, z);
         glColor4f (0.0, 0.0, 0.0, 1.0);
         yFONT_printw (myVIKEYS.font, myVIKEYS.point, YF_TOPCEN, a_text, w - x_edge, h, 1.2);
      } glPopMatrix();
   }
   else {
      if (a_len != a_lvl)  yCOLOR_curs ("note_cur");
      else                 yCOLOR_curs ("note_old");
      strlcpy (t, a_text, LEN_RECD);
      x_edge = 1;
      x_beg  = x_next  = 0;
      x_full = strlen (t);
      for (i = 0; i < h; ++i) {
         x_len = strlen (t + x_next);
         x_next = x_beg + w - 3;
         DEBUG_GRAF   yLOG_complex ("current"   , "%1d %2d %2d[%s]", i, x_beg, x_len, t + x_beg);
         for (j = x_next; j > x_beg; --j) {
            DEBUG_GRAF   yLOG_complex ("check"     , "%2d %c", j, t [j]);
            if (strchr ("- ", t [j]) == NULL)  continue;
            t [j] = '\0';
            x_next = j + 1;
            break;
         }
         x_len = strlen (t + x_beg);
         mvprintw (y + i, x + ((w - x_len) / 2), "%s", t + x_beg);
         x_beg = x_next;
         if (x_beg > x_full) break;
      }
   }
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note_draw       (void)
{
   int         i           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   for (i = 0; i < s_nnote; ++i) {
      DEBUG_GRAF   yLOG_complex  ("note"      , "%4dl, %4dw, %4db, %4dt", s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h);
      if (myVIKEYS.env == YVIKEYS_OPENGL) {
         yvikeys_note_target   (s_notes [i].xb, s_notes [i].yb, s_notes [i].xe, s_notes [i].ye, 200 + (i * 10));
         yvikeys_menu_shadow   (myVIKEYS.env, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 202 + (i * 10));
         if (i == s_nnote - 1)  yvikeys_menu_fill     (myVIKEYS.env, 't', 1, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 204 + (i * 10));
         else                   yvikeys_menu_fill     (myVIKEYS.env, 'n', 0, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 204 + (i * 10));
         yvikeys_note_text     (myVIKEYS.env, 0, 0, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
      } else {
         yvikeys_menu_shadow   (myVIKEYS.env, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 202 + (i * 10));
         if (i == s_nnote - 1)  yvikeys_menu_fill     (myVIKEYS.env, 0, 1, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 204 + (i * 10));
         else                   yvikeys_menu_fill     (myVIKEYS.env, 0, 0, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 204 + (i * 10));
         if (i == s_nnote - 1)  yvikeys_note_text     (myVIKEYS.env, 0, 1, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
         else                   yvikeys_note_text     (myVIKEYS.env, 0, 0, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
      }
   }
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> create a shape mask for notes ------------------------------*/
yVIKEYS_note_mask       (void *a_bounds, void *a_context, int a_left, int a_topp, int a_xmin, int a_ymax)
{
   /*---(locals)-----------+-----+-----+-*/
   Pixmap      *x_bounds;
   GC          *x_context;
   int         i           =    0;
   /*---(quick out)----------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex  ("args"      , "%p, %p, %3dl, %3dt, %3dx, %3dy", a_bounds, a_context, a_left, a_topp, a_xmin, a_ymax);
   /*---(cast)---------------------------*/
   x_bounds  = (Pixmap *) a_bounds;
   x_context = (GC *) a_context;
   /*---(walk notes)---------------------*/
   for (i = 0; i < s_nnote; ++i) {
      DEBUG_GRAF   yLOG_complex  ("note"      , "%4dl, %4dw, %4db, %4dt", s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h);
      XFillRectangle (YX_DISP, *x_bounds, *x_context, a_left + (s_notes [i].x  - a_xmin), a_topp + a_ymax - s_notes [i].y , s_notes [i].w, s_notes [i].h);
      XDrawLine      (YX_DISP, *x_bounds, *x_context, a_left + (s_notes [i].xb - a_xmin), a_topp + a_ymax - s_notes [i].yb, a_left + (s_notes [i].xe - a_xmin), a_ymax - s_notes [i].ye);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
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
   else if (strcmp (a_question, "note"           ) == 0) {
      n = atoi (a_key);
      if (n < 0 || n > MAX_NOTES)     snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER note  (%2s) : - - -    -x    -y   -w  -h  -[]                   -,   -  - -    -        -,   -", "--");
      else if (s_notes [n].xr == '0') snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER note  (%2d) : - - -    -x    -y   -w  -h  -[]                   -,   -  - -    -        -,   -", n);
      else {
         sprintf  (t, "[%.15s]", s_notes [n].text);
         switch (s_notes [n].type) {
         case '-' : sprintf (s, "%c -    -   "  , s_notes [n].type); break;
         case '*' : sprintf (s, "%c %c    %c   ", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         case '>' : sprintf (s, "%c %-3d  %c   ", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         case '@' : sprintf (s, "%c %-4d,%-4d " , s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         case '~' : sprintf (s, "%c -    -   "  , s_notes [n].type); break;
         default  : sprintf (s, "BOOM BOOM  "); break;
         }
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER note  (%2d) : %c %c %c %4dx %4dy %3dw %2dh %2d%-17.17s %4d,%4d  %s  %4d,%4d", n, s_notes [n].xr, s_notes [n].yr, s_notes [n].size, s_notes [n].x, s_notes [n].y, s_notes [n].w, s_notes [n].h, strlen (s_notes [n].text), t, s_notes [n].xb, s_notes [n].yb, s, s_notes [n].xe, s_notes [n].ye);
      }
   }
   else if (strcmp (a_question, "stack"          ) == 0) {
      /*> snprintf (yVIKEYS__unit_answer, LEN_FULL, "LAYER note stack : %2d[%s]", s_nnote, s_snote);   <*/
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



