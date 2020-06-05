/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"




#define     S_VISU_MAX     100
#define     VISU_NOT       '-'
#define     VISU_YES       'y'

typedef     struct cVISU    tVISU;
struct cVISU {
   /*---(flag)---------------------------*/
   char        active;
   /*---(root)---------------------------*/
   int         b_all;
   int         x_root;
   int         y_root;
   int         z_all;
   /*---(begin)--------------------------*/
   int         x_beg;
   int         y_beg;
   /*---(end)----------------------------*/
   int         x_end;
   int         y_end;
   /*---(labels)-------------------------*/
   char        b_label     [LEN_LABEL];
   char        e_label     [LEN_LABEL];
   /*---(special)------------------------*/
   char        x_lock;
   char        y_lock;
   char        source;
   /*---(end)----------------------------*/
};
static tVISU  s_visu;
static tVISU  s_prev;
static tVISU  s_visu_info [S_VISU_MAX];
static char   S_VISU_LIST [S_VISU_MAX];
static int    s_nvisu     =   0;

static char s_visu_head   = '-';
static char s_visu_curr   = '-';
static char s_visu_tail   = '-';

#define     VISU_ALL      '*'
#define     VISU_LOWER    'a'
#define     VISU_UPPER    'A'
#define     VISU_SYSTEM   '0'



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

char 
yvikeys_visu__valid     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_VISU_LIST, a_abbr, S_VISU_MAX);
   DEBUG_HIST   yLOG_sint    (rc);
   --rce;  if (rc < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

int  
yvikeys_visu__index     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   n  = strlchr (S_VISU_LIST, a_abbr, S_VISU_MAX);
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n  < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}

char yvikeys_visu_getlive    (void) { return s_visu.active; }
char yvikeys_visu_islive     (void) { if (s_visu.active == VISU_YES)  return 1; return 0; }
char yvikeys_visu_isdead     (void) { if (s_visu.active == VISU_NOT)  return 1; return 0; }
char yvikeys_visu_makelive   (void) { s_visu.active = VISU_YES; return 0; }
char yvikeys_visu_makedead   (void) { s_visu.active = VISU_NOT; return 0; }

char
yvikeys_visu__wipe           (tVISU *a_dst)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_VISU   yLOG_spoint  (a_dst);
   --rce;  if (a_dst == NULL) {
      DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   /*---(active)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("inactive");
   a_dst->active = VISU_NOT;
   /*---(buf)----------------------------*/
   DEBUG_VISU   yLOG_snote   ("buf");
   a_dst->b_all  = 0;
   /*---(x_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("x");
   a_dst->x_root = 0;
   a_dst->x_beg  = 0;
   a_dst->x_end  = 0;
   /*---(y_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("y");
   a_dst->y_root = 0;
   a_dst->y_beg  = 0;
   a_dst->y_end  = 0;
   /*---(z_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("z");
   a_dst->z_all  = 0;
   /*---(labels)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("labels");
   strlcpy (a_dst->b_label, ""   , LEN_LABEL);
   strlcpy (a_dst->e_label, ""   , LEN_LABEL);
   /*---(locks)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("locks");
   a_dst->x_lock = '-';
   a_dst->y_lock = '-';
   a_dst->source = '-';
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_visu__copy           (tVISU *a_dst, tVISU *a_src)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_VISU   yLOG_spoint  (a_dst);
   --rce;  if (a_dst == NULL) {
      DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   DEBUG_VISU   yLOG_spoint  (a_src);
   --rce;  if (a_src == NULL) {
      DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   /*---(active)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("active");
   a_dst->active = VISU_YES;
   /*---(buf)----------------------------*/
   DEBUG_VISU   yLOG_snote   ("buf");
   a_dst->b_all  = a_src->b_all;
   /*---(x_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("x");
   a_dst->x_root = a_src->x_beg;  /* always override root with beg  */
   a_dst->x_beg  = a_src->x_beg;
   a_dst->x_end  = a_src->x_end;
   /*---(y_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("y");
   a_dst->y_root = a_src->y_beg;  /* always override root with beg  */
   a_dst->y_beg  = a_src->y_beg;
   a_dst->y_end  = a_src->y_end;
   /*---(z_pos)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("z");
   a_dst->z_all  = a_src->z_all;
   /*---(labels)-------------------------*/
   DEBUG_VISU   yLOG_snote   ("labels");
   strlcpy (a_dst->b_label, a_src->b_label, LEN_LABEL);
   strlcpy (a_dst->e_label, a_src->e_label, LEN_LABEL);
   /*---(locks)--------------------------*/
   DEBUG_VISU   yLOG_snote   ("locks");
   a_dst->x_lock = a_src->x_lock;
   a_dst->y_lock = a_src->y_lock;
   a_dst->source = a_src->source;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char  /*-> adjust the visual selection --------[ ------ [ge.760.324.D2]*/ /*-[01.0000.015.X]-*/ /*-[--.---.---.--]-*/
yvikeys_visu_update       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         b, x, y, z;
   /*---(prepare)------------------------*/
   rc = yvikeys_map_current  (NULL, &b, &x, &y, &z);
   --rce;  if (rc < 0) {
      return rce;
   }
   /*---(not-live)-----------------------*/
   if (s_visu.active == VISU_NOT) {
      s_visu.b_all   = b;
      s_visu.x_root  = s_visu.x_beg   = s_visu.x_end   = x;
      s_visu.y_root  = s_visu.y_beg   = s_visu.y_end   = y;
      s_visu.z_all   = z;
      yvikeys_map_addresser (s_visu.b_label, b, x, y, z);
      --rce;  if (rc < 0) {
         return rce;
      }
      strlcpy  (s_visu.e_label, s_visu.b_label, LEN_LABEL);
      return 0;
   }
   /*---(x)------------------------------*/
   if (s_visu.y_lock != 'y') {
      if (x < s_visu.x_root) {
         s_visu.x_beg  = x;
         s_visu.x_end  = s_visu.x_root;
      } else {
         s_visu.x_beg  = s_visu.x_root;
         s_visu.x_end  = x;
      }
   }
   /*---(y)------------------------------*/
   if (s_visu.x_lock != 'y') {
      if (y < s_visu.y_root) {
         s_visu.y_beg  = y;
         s_visu.y_end  = s_visu.y_root;
      } else {
         s_visu.y_beg  = s_visu.y_root;
         s_visu.y_end  = y;
      }
   }
   /*---(set labels)---------------------*/
   rc = yvikeys_map_addresser (s_visu.b_label, s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
   --rce;  if (rc < 0) {
      return rce;
   }
   rc = yvikeys_map_addresser (s_visu.e_label, s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   --rce;  if (rc < 0) {
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      visual sequence                         ----===*/
/*====================------------------------------------====================*/
static void  o___SEQUENCE________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.640.021.20]*/ /*-[01.0000.044.!]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__range     (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(find next)----------------------*/
   s_visu_head = '-';
   for (i = 1; i < s_nvisu; ++i) {
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_head = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
      DEBUG_VISU   yLOG_snote   ("HEAD");
      break;
   }
   /*---(find last)----------------------*/
   s_visu_tail = '-';
   for (i = s_nvisu - 1; i >  0; --i) {
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_tail = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
      DEBUG_VISU   yLOG_snote   ("TAIL");
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.043.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__prev      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("visu_curr" , s_visu_curr);
   x_index = yvikeys_visu__index (s_visu_curr);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find previous)------------------*/
   DEBUG_VISU   yLOG_note    ("search for previous mark");
   for (i = x_index - 1; i > 0; --i) {
      if (s_visu_info [i].active == VISU_NOT )  continue;
      DEBUG_VISU   yLOG_value   ("found"     , i);
      x_mark = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_VISU   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.842.053.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__next      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("visu_curr" , s_visu_curr);
   x_index = yvikeys_visu__index (s_visu_curr);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find next)----------------------*/
   DEBUG_VISU   yLOG_note    ("search for next mark");
   for (i = x_index + 1; i < s_nvisu; ++i) {
      if (s_visu_info [i].active == VISU_NOT )  continue;
      DEBUG_VISU   yLOG_value   ("found"     , i);
      x_mark = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_VISU   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                       visual history                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_HISTORY____o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__unset     (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(check mark)---------------------*/
   n = yvikeys_visu__index (a_abbr);
   DEBUG_VISU   yLOG_value   ("index"     , n);
   --rce;  if (n < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   rc = yvikeys_visu__wipe (&s_visu_info [n]);
   DEBUG_VISU   yLOG_value   ("wipe"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set inactive)-------------------*/
   DEBUG_VISU   yLOG_note    ("set as inactive");
   s_visu_info [n].active = VISU_NOT;
   /*---(update range)-------------------*/
   yvikeys_visu__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__set             (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("active"    , s_visu.active);
   if (s_visu.active != VISU_YES) {
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   n = yvikeys_visu__index (a_abbr);
   DEBUG_VISU   yLOG_value   ("index"     , n);
   --rce;  if (n < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(copy)---------------------------*/
   rc = yvikeys_visu__copy (&s_visu_info [n], &s_visu);
   DEBUG_VISU   yLOG_value   ("copy"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set active)---------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [n].active = VISU_YES;
   /*---(update current)-----------------*/
   DEBUG_VISU   yLOG_note    ("update current");
   s_visu_curr = a_abbr;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range");
   yvikeys_visu__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__return    (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   char        x_preserve  =  '-';
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_abbr"    , a_abbr);
   /*---(look for sequences)-------------*/
   DEBUG_VISU   yLOG_note    ("check special shortcuts");
   switch (a_abbr) {
   case '[' : a_abbr = s_visu_head;             break;
   case '<' : a_abbr = yvikeys_visu__prev ();   break;
   case '>' : a_abbr = yvikeys_visu__next ();   break;
   case ']' : a_abbr = s_visu_tail;             break;
   }
   --rce;  if (a_abbr < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   n = yvikeys_visu__index (a_abbr);
   DEBUG_VISU   yLOG_value   ("n"         , n);
   --rce;  if (n < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_visu_info [n].active != VISU_YES) {
      DEBUG_VISU   yLOG_note    ("nothing saved under this reference");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(backup current)-----------------*/
   x_preserve = s_visu.active;
   if (x_preserve == VISU_YES && a_abbr == '\'') {
      rc = yvikeys_visu__copy (&s_prev, &s_visu);
      DEBUG_VISU   yLOG_value   ("backup"    , rc);
      --rce;  if (rc < 0) {
         DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(return saved)-------------------*/
   rc = yvikeys_visu__copy (&s_visu, &s_visu_info [n]);
   DEBUG_VISU   yLOG_value   ("copy"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(backup current)-----------------*/
   if (x_preserve == VISU_YES && a_abbr == '\'') {
      rc = yvikeys_visu__copy (&s_visu_info [0], &s_prev);
      DEBUG_VISU   yLOG_value   ("set prev"  , rc);
      --rce;  if (rc < 0) {
         DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(set active)---------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu.active = VISU_YES;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("jump to range");
   yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_BASICS_____o () { return; }

char         /*-> remove marks -----------------------[ ------ [gz.642.141.12]*/ /*-[01.0000.033.3]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__purge     (char a_scope)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           = 0;
   char        x_abbr      = 0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  : case YVIKEYS_UPPER  : case YVIKEYS_NUMBER :
   case YVIKEYS_GREEK  : case YVIKEYS_FULL   :
      break;
   default :
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_VISU   yLOG_value   ("nvisu"     , s_nvisu);
   for (i = 0; i < s_nvisu; ++i) {
      x_abbr = S_VISU_LIST [i];
      if (a_scope == YVIKEYS_UPPER  && strchr (gvikeys_upper , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_visu__wipe  (&s_visu_info [i]);
   }
   /*---(globals)------------------------*/
   DEBUG_VISU   yLOG_note    ("initialize globals");
   s_visu_head  = '-';
   s_visu_curr  = '-';
   s_visu_tail  = '-';
   yvikeys_visu__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initialize all marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_visu_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (UMOD_VISUAL)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(visu abbrev list)---------------*/
   strlcpy (S_VISU_LIST, "'"           , S_VISU_MAX);
   strlcat (S_VISU_LIST, gvikeys_lower , S_VISU_MAX);
   strlcat (S_VISU_LIST, gvikeys_upper , S_VISU_MAX);
   strlcat (S_VISU_LIST, gvikeys_number, S_VISU_MAX);
   strlcat (S_VISU_LIST, gvikeys_greek , S_VISU_MAX);
   DEBUG_PROG   yLOG_info    ("LIST"      , S_VISU_LIST);
   s_nvisu  = strlen (S_VISU_LIST);
   /*---(clear)--------------------------*/
   yvikeys_visu__purge  (YVIKEYS_FULL);
   yvikeys_visu__wipe   (&s_visu);
   s_visu.active = VISU_NOT;
   /*> yVIKEYS_view_optionX (YVIKEYS_STATUS, "visual" , yvikeys_visu_status , "details of visual selection"                );   <*/
   /*---(read/write)---------------------*/
   rc = yPARSE_handler_max (UMOD_VISUAL  , "visu_mark" , 7.2, "cLL---------", -1, yvikeys_visu__reader, yvikeys_visu__writer_all, "------------" , "a,beg,end", "map mode visual selections");
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_VISUAL);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual status                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_STATUS_____o () { return; }

char
yVIKEYS_root            (int b, int x, int y, int z)
{
   if (s_visu.active == VISU_NOT)  return 0;
   if (b != s_visu.b_all )  return 0;
   if (x != s_visu.x_root)  return 0;
   if (y != s_visu.y_root)  return 0;
   if (z != s_visu.z_all)   return 0;
   return 1;
}

char
yVIKEYS_visual          (int b, int x, int y, int z)
{
   if (s_visu.active == VISU_NOT)  return 0;
   if (b != s_visu.b_all)   return 0;
   if (x <  s_visu.x_beg)   return 0;
   if (x >  s_visu.x_end)   return 0;
   if (y <  s_visu.y_beg)   return 0;
   if (y >  s_visu.y_end)   return 0;
   if (z != s_visu.z_all)   return 0;
   return 1;
}

/*> char         /+-> tbd --------------------------------[ ------ [ge.420.132.11]+/ /+-[00.0000.114.!]-+/ /+-[--.---.---.--]-+/                                   <* 
 *> yvikeys_visu_info       (char *a_entry, int a_index)                                                                                                           <* 
 *> {                                                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                                                    <* 
 *>    int         a           =    0;                                                                                                                             <* 
 *>    int         n           =    0;                                                                                                                             <* 
 *>    char        t           [LEN_RECD]  = "";                                                                                                                   <* 
 *>    /+---(status)-------------------------+/                                                                                                                    <* 
 *>    strlcpy (a_entry, "                                                                              ", LEN_RECD);                                              <* 
 *>    if (a_index <   0)  return 0;                                                                                                                               <* 
 *>    if (a_index >= 26)  return 0;                                                                                                                               <* 
 *>    a = 'a' + a_index;                                                                                                                                          <* 
 *>    n = yvikeys_visu__index (a);                                                                                                                                <* 
 *>    if (s_visu_info [n].active == VISU_NOT )  sprintf (t, " %c  -           -                    ", a);                                                         <* 
 *>    else                                      sprintf (t, " %c  %-10.10s  %-10.10s           "        , a, s_visu_info [n].b_label, s_visu_info [n].e_label);   <* 
 *>    strlcpy (a_entry, t, LEN_RECD);                                                                                                                             <* 
 *>    a = 'A' + a_index;                                                                                                                                          <* 
 *>    n = yvikeys_visu__index (a);                                                                                                                                <* 
 *>    if (s_visu_info [n].active == VISU_NOT )  sprintf (t, " %c  -           -                    ", a);                                                         <* 
 *>    else                                      sprintf (t, " %c  %-10.10s  %-10.10s           "        , a, s_visu_info [n].b_label, s_visu_info [n].e_label);   <* 
 *>    strlcat (a_entry, t, LEN_RECD);                                                                                                                             <* 
 *>    /+---(complete)-----------------------+/                                                                                                                    <* 
 *>    return 0;                                                                                                                                                   <* 
 *> }                                                                                                                                                              <*/

char
yvikeys_visu__line    (char a_abbr, char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   tVISU      *x_visu      = NULL;
   char        x_size      [LEN_LABEL] = "n/a";
   char        x_root      [LEN_LABEL] = "n/a";
   char        x_beg       [LEN_LABEL] = "n/a";
   char        x_end       [LEN_LABEL] = "n/a";
   /*---(defense)------------------------*/
   --rce; if (a_entry == NULL)   return rce;
   strlcpy (a_entry, "", LEN_RECD);
   --rce;  if (a_abbr != ':') {
      n = yvikeys_visu__index (a_abbr);
      if (n  < 0)   return rce;
      x_visu = &s_visu_info [n];
   } else {
      x_visu = &s_visu;
   }
   /*---(fill)---------------------------*/
   if (x_visu->active == VISU_YES) {
      yvikeys_map_addresser (x_root, x_visu->b_all, x_visu->x_root, x_visu->y_root, x_visu->z_all);
      --rce;  if (rc < 0) return rce;
      sprintf (x_size, "%dx%d", x_visu->x_end - x_visu->x_beg + 1, x_visu->y_end - x_visu->y_beg + 1);
      strlcpy (x_beg, x_visu->b_label, LEN_LABEL);
      strlcpy (x_end, x_visu->e_label, LEN_LABEL);
   }
   sprintf (a_entry, "%c %c %-10.10s %-10.10s %-10.10s %-10.10s %c %c %c",
         a_abbr, x_visu->active,
         x_beg, x_end, x_size, x_root,
         x_visu->x_lock, x_visu->y_lock, x_visu->source);
   /*---(complete)-----------------------*/
   if (x_visu->active == VISU_NOT)  return 0;
   return 1;
}

char
yvikeys_visu_status   (char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        t           [LEN_FULL]   = "";
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)   return rce;
   /*---(get info)-----------------------*/
   rc = yvikeys_visu__line (':', t);
   --rce;  if (rc < 0)            return rce;
   /*---(generate stats)--------------*/
   sprintf (a_entry, "[ select %s ]", t);
   /*---(complete)--------------------*/
   return 0;
}

char
yvikeys_visu_info     (int a_index, char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_abbr      =  '-';
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)    return rce;
   --rce;  if (a_index < 0)        return rce;
   --rce;  if (a_index >= s_nvisu) return rce;
   /*---(get info)-----------------------*/
   x_abbr = S_VISU_LIST [a_index];
   rc = yvikeys_visu__line (x_abbr, a_entry);
   --rce;  if (rc < 0)             return rce;
   /*---(complete)--------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual setting                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_SETTING____o () { return; }

char
yvikeys_visu_reverse    (void)
{
   /*---(if x locked)--------------------*/
   if (s_visu.y_lock == 'y') {
      if (s_visu.y_root == s_visu.y_beg) {
         s_visu.y_root = s_visu.y_end;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_root, s_visu.y_beg, s_visu.z_all);
      }
      else  {
         s_visu.y_root = s_visu.y_beg;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_root, s_visu.y_end, s_visu.z_all);
      }
      return 0;
   }
   /*---(if y locked)--------------------*/
   if (s_visu.x_lock == 'y') {
      if (s_visu.x_root == s_visu.x_beg) {
         s_visu.x_root = s_visu.x_end;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_beg, s_visu.y_root, s_visu.z_all);
      }
      else  {
         s_visu.x_root = s_visu.x_beg;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_root, s_visu.z_all);
      }
      return 0;
   }
   /*---(root at beg)--------------------*/
   if (s_visu.x_root == s_visu.x_beg && s_visu.y_root == s_visu.y_beg) {
      s_visu.x_root = s_visu.x_end;
      s_visu.y_root = s_visu.y_end;
      yVIKEYS_jump (s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
      return 0;
   }
   /*---(root at end)--------------------*/
   s_visu.x_root = s_visu.x_beg;
   s_visu.y_root = s_visu.y_beg;
   yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   /*---(complete)-----------------------*/
   return 0;
}

char       /*-> clear the selection ----------------[ ------ [gz.742.001.13]*/ /*-[01.0000.743.A]-*/ /*-[--.---.---.--]-*/
yvikeys_visu_clear          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x, y;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(set prev)-----------------------*/
   yvikeys_visu__set ('\'');
   /*---(backup)-------------------------*/
   s_visu.active  = VISU_NOT;
   if (s_visu.y_lock == 'y')  x = s_visu.x_root;
   else                       x = s_visu.x_beg;
   if (s_visu.x_lock == 'y')  y = s_visu.y_root;
   else                       y = s_visu.y_beg;
   yVIKEYS_jump (s_visu.b_all, x, y, s_visu.z_all);
   s_visu.x_lock = '-';
   s_visu.y_lock = '-';
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> apply a specific selection ---------[ ------ [ge.E54.125.43]*/ /*-[01.0000.213.T]-*/ /*-[--.---.---.--]-*/
yvikeys_visu_exact      (int b, int xb, int xe, int yb, int ye, int z)
{  /*---(design notes)--------------------------------------------------------*/
   /* if the two ends of the range are legal, this function will change the   */
   /* current selection to the boundaries passed as arguments.                */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_beg       [LEN_LABEL];
   char        x_end       [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_complex ("args"      , "%3db,  %4d to %4dx,  %4d to %4dy,  %4dz", b, xb, xe, yb, ye, z);
   /*---(prepare)------------------------*/
   DEBUG_VISU   yLOG_note    ("clear existing ranges");
   rc = yvikeys_visu_clear ();
   /*---(set labels)---------------------*/
   rc = yvikeys_map_addresser (x_beg, b, xb, yb, z);
   DEBUG_VISU   yLOG_value   ("beg label" , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_map_addresser (x_end, b, xe, ye, z);
   DEBUG_VISU   yLOG_value   ("end label" , rc);
   --rce;  if (rc < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set range)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range live");
   s_visu.active  = VISU_YES;
   /*---(locations)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range coordinates");
   s_visu.b_all  = b;
   s_visu.x_root = s_visu.x_beg  = xb;
   s_visu.x_end  = xe;
   s_visu.y_root = s_visu.y_beg  = yb;
   s_visu.y_end  = ye;
   s_visu.z_all  = z;
   strlcpy (s_visu.b_label, x_beg, LEN_LABEL);
   strlcpy (s_visu.e_label, x_end, LEN_LABEL);
   /*---(jump to the end)----------------*/
   yVIKEYS_jump (b, xe, ye, z);
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_visu_locking    (char a_type)
{
   char        rce         =  -10;
   char        rc          =    0;
   switch (a_type) {
   case 'x'  :
      DEBUG_USER   yLOG_note    ("x for x_axis/col selection");
      if (s_visu.x_lock == 'y') {
         s_visu.x_lock = '-';
         if (s_visu.y_root <= g_ymap.gcur) {
            s_visu.y_beg  = s_visu.y_root;
            s_visu.y_end  = g_ymap.gcur;
         } else {
            s_visu.y_end  = s_visu.y_root;
            s_visu.y_beg  = g_ymap.gcur;
         }
      } else {
         s_visu.x_lock = 'y';
         s_visu.y_beg  = g_ymap.gmin;
         s_visu.y_end  = g_ymap.gmax;
      }
      break;
   case 'y'  :
      DEBUG_USER   yLOG_note    ("y for y-axis/row selection");
      if (s_visu.y_lock == 'y') {
         s_visu.y_lock = '-';
         if (s_visu.x_root <= g_xmap.gcur) {
            s_visu.x_beg  = s_visu.x_root;
            s_visu.x_end  = g_xmap.gcur;
         } else {
            s_visu.x_end  = s_visu.x_root;
            s_visu.x_beg  = g_xmap.gcur;
         }
      } else {
         s_visu.y_lock = 'y';
         s_visu.x_beg  = g_xmap.gmin;
         s_visu.x_end  = g_xmap.gmax;
      }
      break;
   case '!'  :
      DEBUG_USER   yLOG_note    ("! for screen selection");
      s_visu.y_lock = '-';
      s_visu.y_root = g_ymap.gbeg;
      s_visu.y_beg  = g_ymap.gbeg;
      s_visu.y_end  = g_ymap.gend;
      s_visu.x_lock = '-';
      s_visu.x_root = g_xmap.gbeg;
      s_visu.x_beg  = g_xmap.gbeg;
      s_visu.x_end  = g_xmap.gend;
      yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
      break;
   case '*'  :
      DEBUG_USER   yLOG_note    ("* for all on current z selection");
      s_visu.y_lock = 'y';
      s_visu.y_beg  = g_ymap.gmin;
      s_visu.y_end  = g_ymap.gmax;
      s_visu.x_lock = 'y';
      s_visu.x_beg  = g_xmap.gmin;
      s_visu.x_end  = g_xmap.gmax;
      break;
   default   :
      DEBUG_USER   yLOG_note    ("option not understood");
      return -1;
   }
   /*---(set labels)---------------------*/
   rc = yvikeys_map_addresser (s_visu.b_label, s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
   --rce;  if (rc < 0) {
      return rce;
   }
   rc = yvikeys_map_addresser (s_visu.e_label, s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   --rce;  if (rc < 0) {
      return rce;
   }
   return 0;
}

char
yvikeys_visu_ends       (char a_type)
{
   switch (a_type) {
   case '0'  :
      DEBUG_USER   yLOG_note    ("0 for left selection");
      s_visu.x_beg  = g_xmap.gmin;
      s_visu.x_end  = g_xmap.gcur;
      break;
   case '$'  :
      DEBUG_USER   yLOG_note    ("$ for right selection");
      s_visu.x_beg  = g_xmap.gcur;
      s_visu.x_end  = g_xmap.gmax;
      break;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual ranges                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_RANGES_____o () { return; }

static char    s_valid = '-';
static int     s_b = 0;
static int     s_x = 0;
static int     s_y = 0;
static int     s_z = 0;

char         /*-> return selection range borders -----[ leaf   [gc.730.107.C0]*/ /*-[01.0000.204.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_visu_coords     (int *b, int *xb, int *xe, int *yb, int *ye, int *z)
{
   if (b  != NULL) *b  = s_visu.b_all;
   if (xb != NULL) *xb = s_visu.x_beg;
   if (xe != NULL) *xe = s_visu.x_end;
   if (yb != NULL) *yb = s_visu.y_beg;
   if (ye != NULL) *ye = s_visu.y_end;
   if (z  != NULL) *z  = s_visu.z_all;
   return 0;
}

char
yVIKEYS_first       (int *a_b, int *a_x, int *a_y, int *a_z)
{
   s_b = s_visu.b_all;
   s_x = s_visu.x_beg;
   s_y = s_visu.y_beg;
   s_z = s_visu.z_all;
   if (a_b != NULL) *a_b = s_b;
   if (a_x != NULL) *a_x = s_x;
   if (a_y != NULL) *a_y = s_y;
   if (a_z != NULL) *a_z = s_z;
   s_valid = 'y';
   return 0;
}

char
yVIKEYS_next        (int *a_b, int *a_x, int *a_y, int *a_z)
{
   if (s_valid != 'y')  return -1;
   ++s_x;
   if (s_x > s_visu.x_end) {
      s_x = s_visu.x_beg;
      ++s_y;
      if (s_y > s_visu.y_end) {
         s_valid = '-';
      }
   }
   if (a_b != NULL) *a_b = s_b;
   if (a_x != NULL) *a_x = s_x;
   if (a_y != NULL) *a_y = s_y;
   if (a_z != NULL) *a_z = s_z;
   if (s_valid != 'y')  return -1;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     file input and output                    ----===*/
/*====================------------------------------------====================*/
static void  o___FILE____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__writer     (int c, char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (UMOD_VISUAL)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   n  = yvikeys_visu__index   (a_abbr);
   DEBUG_OUTP   yLOG_value   ("index"     , n);
   --rce; if (n  < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   DEBUG_OUTP   yLOG_value   ("live"      , s_visu_info [n].active);
   if (s_visu_info [n].active != 'y')  {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write)-----------------------*/
   yPARSE_vprintf (c, "visu_mark", a_abbr, s_visu_info [n].b_label, s_visu_info [n].e_label);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_visu__writer_all (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (UMOD_VISUAL)) {
      DEBUG_OUTP   yLOG_note    ("can not execute until operational");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   /*> yPARSE_verb_begin ("visu_mark");                                               <*/
   /*---(walk list)----------------------*/
   for (i = 0; i <= s_nvisu; ++i) {
      rc = yvikeys_visu__writer (c, S_VISU_LIST [i]);
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

char
yvikeys_visu__reader    (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   char        x_verb      [LEN_LABEL];
   char        x_abbr      =    0;
   int         n           =    0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (UMOD_VISUAL)) {
      DEBUG_INPT   yLOG_note    ("can not execute until operational");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("visu_mark", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(abbreviation)-------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   n  = yvikeys_visu__index (x_abbr);
   DEBUG_INPT   yLOG_value   ("n"         , n);
   --rce; if (n < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yvikeys_visu__wipe (&s_visu_info [n]);
   DEBUG_INPT   yLOG_value   ("wipe"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get beginning)------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop keys"  , rc);
   DEBUG_INPT   yLOG_info    ("beg"       , x_label);
   strlcpy (s_visu_info [n].b_label, x_label, LEN_LABEL);
   rc = yvikeys_map_locator (s_visu_info [n].b_label, &s_visu_info [n].b_all, &s_visu_info [n].x_beg, &s_visu_info [n].y_beg, &s_visu_info [n].z_all);
   DEBUG_INPT   yLOG_value   ("locator"   , rc);
   --rce;  if (rc < 0) {
      yvikeys_visu__unset (x_abbr);
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get ending)---------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop keys"  , rc);
   DEBUG_INPT   yLOG_info    ("end"       , x_label);
   strlcpy (s_visu_info [n].e_label, x_label, LEN_LABEL);
   rc = yvikeys_map_locator (s_visu_info [n].e_label, NULL, &s_visu_info [n].x_end, &s_visu_info [n].y_end, NULL);
   DEBUG_INPT   yLOG_value   ("locator"   , rc);
   --rce;  if (rc < 0) {
      yvikeys_visu__unset (x_abbr);
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set active)---------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [n].active = VISU_YES;
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}



/*====================------------------------------------====================*/
/*===----                       visual mode                            ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
yvikeys_visu_umode      (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   char       *x_majors    = "VM";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   myVIKEYS.info_win = '-';
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_VISUAL )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(exit mode)----------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape means leave");
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(major check)--------------------*/
   DEBUG_USER   yLOG_info    ("x_majors"  , x_majors);
   --rce;  if (strchr (x_majors, a_major) == NULL) {
      DEBUG_USER   yLOG_note    ("major not valid");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(common quick, any major)--------*/
   --rce;  if (strchr("!", a_minor) != NULL) {
      switch (a_minor) {
      case '!' :
         DEBUG_USER   yLOG_note    ("use visual status bar");
         yVIKEYS_cmds_direct (":status visual");
         break;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(common complex)-----------------*/
   --rce;  if (a_minor == '?') {
      DEBUG_USER   yLOG_note    ("show mark info window");
      myVIKEYS.info_win = 'y';
      return a_major;
   }
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'M') {
      DEBUG_USER   yLOG_note    ("handle visual selection saving (M)");
      rc = yvikeys_visu__set (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == 'V') {
      DEBUG_USER   yLOG_note    ("handle visual selection return (V)");
      rc = yvikeys_visu__return (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(failure)------------------------*/
   --rce;
   MODE_exit ();
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_visu__unit      (char *a_question, char a_which)
{
   int         n           =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "VISU unit        : question not understood", LEN_FULL);
   /*---(simple)-------------------------*/
   if      (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VISU selection   : %c  %3db,  %4d to %4dx,  %4d to %4dy,  %4dz", yvikeys_visu_getlive (), s_visu.b_all, s_visu.x_beg, s_visu.x_end, s_visu.y_beg, s_visu.y_end, s_visu.z_all);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "range"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VISU range       : %c to %c,  %c", s_visu_head, s_visu_tail, s_visu_curr);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "cursor"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VISU cursor      : %c  %3db,  %4dx,  %4dy,  %4dz", yvikeys_visu_getlive (), s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
   }
   /*---(complex)------------------------*/
   n = yvikeys_visu__index (a_which);
   if (n < 0) {
      strlcpy  (yVIKEYS__unit_answer, "VISU unit        : not a valid range name", LEN_FULL);
      return yVIKEYS__unit_answer;
   }
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "saved"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VISU saved   (%c) : %c  %3db,  %4d to %4dx,  %4d to %4dy,  %4dz", a_which, s_visu_info [n].active, s_visu_info [n].b_all, s_visu_info [n].x_beg, s_visu_info [n].x_end, s_visu_info [n].y_beg, s_visu_info [n].y_end, s_visu_info [n].z_all);
   }
   else if (strcmp (a_question, "labels"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "VISU labels  (%c) : %c    beg=%-12.12s, end=%s", a_which, s_visu_info [n].active, s_visu_info [n].b_label, s_visu_info [n].e_label);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}
