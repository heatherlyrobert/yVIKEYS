/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

static char s_mark_head   = '-';
static char s_mark_tail   = '-';

static char s_mark_curr   [LEN_LABEL] = "-";
static char s_mark_prev   [LEN_LABEL] = "-";
static char s_mark_pprev  [LEN_LABEL] = "-";



#define     S_MARK_MAX       150
typedef  struct cMARK  tMARK;
struct cMARK {
   char        label       [LEN_LABEL];
   int         b_pos;
   int         x_pos;
   int         y_pos;
   int         z_pos;
   char        source;
};
tMARK       s_mark_info [S_MARK_MAX];
static char S_MARK_LIST [S_MARK_MAX];
static int  s_nmark       = 0;


#define     MARK_NONE     '-'
#define     MARK_USER     'u'
#define     MARK_IMPORT   'i'
#define     MARK_AUTO     'a'

char         yvikeys_mark__purge   (char  a_scope);
char         yvikeys_mark__unset   (char  a_mark);
char         yvikeys_mark__range   (void);
char         yvikeys_mark__prev    (void);
char         yvikeys_mark__next    (void);



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize all marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_init            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (UMOD_MARK)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(macro abbrev list)--------------*/
   strlcpy (S_MARK_LIST, "'"           , S_MARK_MAX);
   strlcat (S_MARK_LIST, gvikeys_lower , S_MARK_MAX);
   strlcat (S_MARK_LIST, gvikeys_upper , S_MARK_MAX);
   strlcat (S_MARK_LIST, gvikeys_number, S_MARK_MAX);
   strlcat (S_MARK_LIST, gvikeys_greek , S_MARK_MAX);
   strlcat (S_MARK_LIST, "()"          , S_MARK_MAX);
   DEBUG_PROG   yLOG_info    ("LIST"      , S_MARK_LIST);
   s_nmark = strlen (S_MARK_LIST);
   /*---(clear)--------------------------*/
   yvikeys_mark__purge  (YVIKEYS_FULL);
   /*---(globals)------------------------*/
   myVIKEYS.mark_show = '-';
   yVIKEYS_cmds_add (YVIKEYS_M_EDIT  , "mark"        , ""    , "s"    , yvikeys_mark_direct        , "" );
   /*---(read/write)---------------------*/
   rc = yPARSE_handler (UMOD_MARK    , "loc_mark"  , 7.1, "cL----------", yvikeys_mark_reader, yvikeys_mark_writer_all, "------------" , "a,label", "map mode location marks");
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_MARK);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> remove marks -----------------------[ ------ [gz.642.141.12]*/ /*-[01.0000.033.3]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__purge          (char a_scope)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        x_abbr      =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MARK   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  : case YVIKEYS_UPPER  : case YVIKEYS_NUMBER :
   case YVIKEYS_GREEK  : case YVIKEYS_FULL   :
      break;
   default :
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_MARK   yLOG_value   ("nmark"     , s_nmark);
   for (i = 0; i < s_nmark; ++i) {
      x_abbr = S_MARK_LIST [i];
      if (a_scope == YVIKEYS_UPPER  && strchr (gvikeys_upper , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_mark__unset (x_abbr);
   }
   /*---(globals)------------------------*/
   DEBUG_MARK   yLOG_note    ("initialize globals");
   s_mark_head  = '-';
   s_mark_tail  = '-';
   strlcpy (s_mark_curr , "-", LEN_LABEL);
   strlcpy (s_mark_prev , "-", LEN_LABEL);
   strlcpy (s_mark_pprev, "-", LEN_LABEL);
   yvikeys_mark__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       supporting functions                   ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

int  
yvikeys_mark__index     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   n  = strlchr (S_MARK_LIST, a_abbr, S_MARK_MAX);
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n  < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}



/*====================------------------------------------====================*/
/*===----                        setting and unsetting                 ----===*/
/*====================------------------------------------====================*/
static void  o___SETTING_________o () { return; }

char
yvikeys_mark__history   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_index     =    0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(get current)--------------------*/
   rc = yvikeys_map_current  (x_label, NULL, NULL, NULL, NULL);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle history)------------------*/
   strlcpy (s_mark_pprev , s_mark_prev    , LEN_LABEL);
   strlcpy (s_mark_prev  , s_mark_curr    , LEN_LABEL);
   strlcpy (s_mark_curr  , x_label        , LEN_LABEL);
   DEBUG_MARK   yLOG_info    ("mark_pprev", s_mark_pprev);
   DEBUG_MARK   yLOG_info    ("mark_prev" , s_mark_prev);
   DEBUG_MARK   yLOG_info    ("mark_curr" , s_mark_curr);
   /*---(set prev mark)------------------*/
   if (strcmp (s_mark_prev, "-") != 0) {
      x_index = yvikeys_mark__find (s_mark_prev);
   } else {
      x_index = yvikeys_mark__find (s_mark_curr);
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(fill in data)-------------------*/
   strlcpy (s_mark_info [0].label, s_mark_info [x_index].label, LEN_LABEL);
   DEBUG_MARK   yLOG_info    ("'"         , s_mark_info [0].label);
   s_mark_info [0].b_pos  = s_mark_info [x_index].b_pos;
   s_mark_info [0].x_pos  = s_mark_info [x_index].x_pos;
   s_mark_info [0].y_pos  = s_mark_info [x_index].y_pos;
   s_mark_info [0].z_pos  = s_mark_info [x_index].z_pos;
   s_mark_info [0].source = MARK_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__set             (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   int         n           =   0;
   char        x_label     [10];
   int         b, x, y, z;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   n = yvikeys_mark__index (a_mark);
   DEBUG_MARK   yLOG_value   ("n"   , n);
   --rce;  if (n < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (n == 0) {
      DEBUG_MARK   yLOG_note    ("never directly set previous mark");
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(get current)--------------------*/
   rc = yvikeys_map_current  (x_label, &b, &x, &y, &z);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark label)---------------------*/
   DEBUG_MARK   yLOG_note    ("save current position");
   strlcpy (s_mark_info [n].label, x_label, LEN_LABEL);
   s_mark_info [n].b_pos  = b;
   s_mark_info [n].x_pos  = x;
   s_mark_info [n].y_pos  = y;
   s_mark_info [n].z_pos  = z;
   s_mark_info [n].source = MARK_USER;
   /*---(set history)--------------------*/
   yvikeys_mark__history ();
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   yvikeys_mark__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear a mark -----------------------[ ------ [ge.732.142.12]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__unset           (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   int         x_index     =   0;
   char        x_label     [10];
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = yvikeys_mark__index (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear mark)---------------------*/
   DEBUG_MARK   yLOG_note    ("clear values");
   strlcpy (s_mark_info [x_index].label, "-", LEN_LABEL);
   s_mark_info [x_index].source  = MARK_NONE;
   s_mark_info [x_index].b_pos   = 0;
   s_mark_info [x_index].x_pos   = 0;
   s_mark_info [x_index].y_pos   = 0;
   s_mark_info [x_index].z_pos   = 0;
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   yvikeys_mark__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.E83.174.A7]*/ /*-[01.0000.013.#]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__return        (char a_mark)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_index     =    0;
   char        x_label     [10];
   char        x_prev      =   -1;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(look for sequences)-------------*/
   DEBUG_MARK   yLOG_note    ("check special shortcuts");
   switch (a_mark) {
   case '[' : a_mark = s_mark_head;             break;
   case '<' : a_mark = yvikeys_mark__prev ();   break;
   case '>' : a_mark = yvikeys_mark__next ();   break;
   case ']' : a_mark = s_mark_tail;             break;
   }
   --rce;  if (a_mark < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   x_index = yvikeys_mark__index (a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for existance)------------*/
   strlcpy (x_label, s_mark_info [x_index].label, LEN_LABEL);
   DEBUG_MARK   yLOG_info    ("x_label"   , x_label);
   DEBUG_MARK   yLOG_char    ("source"    , s_mark_info [x_index].source);
   --rce;  if (s_mark_info [x_index].source == MARK_NONE) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(move)---------------------------*/
   DEBUG_MARK   yLOG_note    ("jump to mark");
   yVIKEYS_jump (s_mark_info [x_index].b_pos, s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   /*---(set history)--------------------*/
   yvikeys_mark__history ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___FINDING_________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gc.430.022.40]*/ /*-[01.0000.013.5]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__which     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char        x_label     [LEN_LABEL];
   /*---(get current)--------------------*/
   rc = yvikeys_map_current  (x_label, NULL, NULL, NULL, NULL);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find label)---------------------*/
   --rce;  for (i = 1; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE)          continue;
      if (strcmp (s_mark_info [i].label, x_label) != 0) continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return rce;
}

char         /*-> tbd --------------------------------[ leaf   [ge.732.133.30]*/ /*-[01.0000.024.#]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__find      (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_point   ("a_label"   , a_label);
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_label"   , a_label);
   --rce;  if (strcmp (a_label, "-") == 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(search)-------------------------*/
   DEBUG_MARK   yLOG_note    ("searching");
   for (i = 1; i < s_nmark; ++i) {
      if (strcmp (s_mark_info [i].label, "")      == 0) continue;
      if (strcmp (s_mark_info [i].label, "-")     == 0) continue;
      if (strcmp (s_mark_info [i].label, a_label) != 0) continue;
      DEBUG_MARK   yLOG_value   ("i"         , i);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return i;
   }
   /*---(complete)-----------------------*/
   --rce;
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___SEQUENCE________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.640.021.20]*/ /*-[01.0000.044.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__range     (void)
{
   /*---(design notes)-------------------*/
   /*
    *  do not include single quote register #0 in searches
    */
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   /*---(find next)----------------------*/
   s_mark_head = '-';
   for (i = 1; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE) continue;
      s_mark_head = S_MARK_LIST [i];
      break;
   }
   /*---(find last)----------------------*/
   s_mark_tail = '-';
   for (i = s_nmark - 1; i >  0; --i) {
      if (s_mark_info [i].source == MARK_NONE) continue;
      s_mark_tail = S_MARK_LIST [i];
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.043.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__prev      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = yvikeys_mark__find (s_mark_curr);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(find previous)------------------*/
   DEBUG_MARK   yLOG_note    ("search for previous mark");
   for (i = x_index - 1; i > 0; --i) {
      if (s_mark_info [i].source == MARK_NONE)  continue;
      DEBUG_MARK   yLOG_value   ("found"     , i);
      x_mark = S_MARK_LIST [i];
      DEBUG_MARK   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_MARK   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.842.053.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__next      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = yvikeys_mark__find (s_mark_curr);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(find next)----------------------*/
   DEBUG_MARK   yLOG_note    ("search for next mark");
   for (i = x_index + 1; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE)  continue;
      DEBUG_MARK   yLOG_value   ("found"     , i);
      x_mark = S_MARK_LIST [i];
      DEBUG_MARK   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_MARK   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

/*> char         /+-> tbd --------------------------------[ ------ [ge.833.224.31]+/ /+-[01.0000.114.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> MARK_entry           (char a_mark, char *a_entry)                                                                              <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    int         x_index     =   0;                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_MARK   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);                                                                           <* 
 *>    DEBUG_MARK   yLOG_point   ("a_entry"   , a_entry);                                                                          <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    --rce;  if (a_entry == NULL) {                                                                                              <* 
 *>       DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    strlcpy (a_entry, "  - :           ", LEN_DESC);                                                                            <* 
 *>    /+---(check mark)---------------------+/                                                                                    <* 
 *>    x_index = yvikeys_mark__index (a_mark);                                                                                             <* 
 *>    --rce;  if (x_index < 0) {                                                                                                  <* 
 *>       DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_MARK   yLOG_value   ("a_mark"    , a_mark);                                                                           <* 
 *>    DEBUG_MARK   yLOG_value   ("x_index"   , x_index);                                                                          <* 
 *>    /+---(copy)---------------------------+/                                                                                    <* 
 *>    sprintf (a_entry, "  %c : %-8.8s  ", a_mark, s_mark_info [x_index].label);                                                  <* 
 *>    DEBUG_MARK   yLOG_info    ("a_entry"   , a_entry);                                                                          <* 
 *>    --rce;  if (strcmp ("", s_mark_info [x_index].label) == 0) {                                                                <* 
 *>       DEBUG_MARK   yLOG_exitr   (__FUNCTION__, -(rce));                                                                        <* 
 *>       return -(rce);                                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_MARK   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_info       (char *a_entry, int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         a           =    0;
   int         n           =    0;
   char        t           [LEN_RECD]  = "";
   /*---(status)-------------------------*/
   strlcpy (a_entry, " -  -              -     -     -      -  -              -     -     -         ", LEN_RECD);
   if (a_index <   0)  return 0;
   if (a_index >= 26)  return 0;
   a = 'a' + a_index;
   n = yvikeys_mark__index (a);
   if (s_mark_info [n].source == MARK_NONE)  sprintf (t, " %c  -              -     -     -     ", a);
   else                                      sprintf (t, " %c  %-10.10s %5d,%5d,%5d     "        , a, s_mark_info [n].label, s_mark_info [n].x_pos, s_mark_info [n].y_pos, s_mark_info [n].z_pos);
   strlcpy (a_entry, t, LEN_RECD);
   a = 'A' + a_index;
   n = yvikeys_mark__index (a);
   if (s_mark_info [n].source == MARK_NONE)  sprintf (t, " %c  -              -     -     -     ", a);
   else                                      sprintf (t, " %c  %-10.10s %5d,%5d,%5d     "        , a, s_mark_info [n].label, s_mark_info [n].x_pos, s_mark_info [n].y_pos, s_mark_info [n].z_pos);
   strlcat (a_entry, t, LEN_RECD);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_status     (char *a_status)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_list      [LEN_RECD]  = "";
   int         c           =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_status  == NULL)  return rce;
   /*---(status)-------------------------*/
   yvikeys_mark_listplus (x_list);
   c = strldcnt (x_list, ':', LEN_RECD);
   snprintf (a_status, 500, " %c,%c,%c,%c %2d %s ", myVIKEYS.mark_show, s_mark_head, s_mark_prev, s_mark_tail, c, x_list);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.103.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_hint_marklist   (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", LEN_RECD);
   for (i = 0; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE) continue;
      sprintf    (x_entry, "%s,", s_mark_info [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [ge.540.142.30]*/ /*-[01.0000.123.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_listplus   (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        rce         = -10;
   char        x_entry     [20];
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", LEN_RECD);   /* special for a null list */
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", LEN_RECD);
   for (i = 1; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE) continue;
      sprintf    (x_entry, "%c:%s,", S_MARK_LIST [i], s_mark_info [i].label);
      strncat    (a_list, x_entry, LEN_RECD);
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     file input and output                    ----===*/
/*====================------------------------------------====================*/
static void  o___FILE____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_writer     (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   char        x_cmd       [LEN_RECD ];
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yPARSE_outclear  ();
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("a_abbr"    , a_abbr);
   n  = yvikeys_mark__index (a_abbr);
   DEBUG_OUTP   yLOG_char    ("index"     , n);
   --rce; if (n  < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   DEBUG_OUTP   yLOG_char    ("label"     , s_mark_info [n].label);
   if (strcmp (s_mark_info [n].label, "-") == 0) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write)-----------------------*/
   yPARSE_fullwrite ("loc_mark", a_abbr, s_mark_info [n].label);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys_mark_writer_all (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(prepare)------------------------*/
   x_end = strlen (S_MARK_LIST);
   yPARSE_verb_begin ("loc_mark");
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      rc = yvikeys_mark_writer (S_MARK_LIST [i]);
      if (rc < 1)  continue;
      ++c;
      yPARSE_verb_break (c);
   }
   /*---(wrap-up)------------------------*/
   yPARSE_verb_end   (c);
   /*---(complete)-----------------------*/
   return c;
}

char
yvikeys_mark_reader     (void)
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
   /*---(get verb)-----------------------*/
   rc = yPARSE_popstr (x_verb);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_verb"    , x_verb);
   --rce;  if (strcmp ("loc_mark", x_verb) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   rc = yPARSE_popchar (&x_abbr);
   DEBUG_INPT   yLOG_value   ("pop abbr"  , rc);
   DEBUG_INPT   yLOG_char    ("abbr"      , x_abbr);
   n  = yvikeys_mark__index (x_abbr);
   DEBUG_INPT   yLOG_char    ("n"         , n);
   --rce; if (n < 0) { 
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get command)--------------------*/
   rc = yPARSE_popstr (x_label);
   DEBUG_INPT   yLOG_value   ("pop verb"  , rc);
   DEBUG_INPT   yLOG_info    ("x_label"   , x_label);
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strlcpy (s_mark_info [n].label, x_label, LEN_LABEL);
   rc = yvikeys_map_locator (s_mark_info [n].label, &s_mark_info [n].b_pos, &s_mark_info [n].x_pos, &s_mark_info [n].y_pos, &s_mark_info [n].z_pos);
   --rce;  if (rc < 0) {
      yvikeys_mark__unset (x_abbr);
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_mark_info [n].source = MARK_IMPORT;
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 1;
}



/*====================------------------------------------====================*/
/*===----                      mark command line                       ----===*/
/*====================------------------------------------====================*/
static void  o___COMMAND_________o () { return; }

char         /*-> enter a mark directly --------------[ ------ [ge.D54.139.83]*/ /*-[02.0000.00#.O]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_direct     (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   int         x_index     =   -1;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_point   ("a_string"  , a_string);
   /*---(defense)------------------------*/
   --rce;  if (a_string == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_string"  , a_string);
   /*---(check for purge)----------------*/
   if (strcmp ("clear", a_string) == 0) {
      yvikeys_mark__purge (YVIKEYS_LOWER);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("purge", a_string) == 0) {
      yvikeys_mark__purge (YVIKEYS_FULL);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("reset" , a_string) == 0) {
      yvikeys_mark__purge (YVIKEYS_NUMBER);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(defense)------------------------*/
   x_len = strlen (a_string);
   DEBUG_MARK   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <  2) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("2nd char"  , a_string [1]);
   --rce;  if (a_string [1] != '=') {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for unset)----------------*/
   --rce;  if (x_len == 2) {
      rc = yvikeys_mark__unset (a_string [0]);
      DEBUG_MARK   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check mark)---------------------*/
   x_index = yvikeys_mark__index (a_string [0]);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_string [0]);
   /*---(save)---------------------------*/
   strlcpy (s_mark_info [x_index].label, a_string + 2, LEN_LABEL);
   rc = yvikeys_map_locator (s_mark_info [x_index].label, &s_mark_info [x_index].b_pos, &s_mark_info [x_index].x_pos, &s_mark_info [x_index].y_pos, &s_mark_info [x_index].z_pos);
   --rce;  if (rc < 0) {
      yvikeys_mark__unset (a_string [0]);
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_mark_info [x_index].source = MARK_IMPORT;
   /*---(go there)-----------------------*/
   yVIKEYS_jump (s_mark_info [x_index].b_pos, s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   /*---(set history)--------------------*/
   DEBUG_MARK   yLOG_note    ("set history");
   yvikeys_mark__history ();
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   yvikeys_mark__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         mode handling                        ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
yvikeys_mark_smode      (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_majors    = "m'";
   static char x_prev      =  '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_USER   yLOG_char    ("x_prev"    , x_prev);
   myVIKEYS.info_win = '-';
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_MARK   )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(exit mode)----------------------*/
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return means leave");
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
   /*---(mode change)--------------------*/
   /*> if (a_minor == '*') {                                                          <* 
    *>    DEBUG_USER   yLOG_note    ("enter visual mode from < to >");                <* 
    *>    MODE_exit ();                                                               <* 
    *>    MODE_enter  (UMOD_VISUAL);                                                  <* 
    *>    rc = VISU_mark    ();                                                       <* 
    *>    if (rc < 0) {                                                               <* 
    *>       MODE_exit ();                                                            <* 
    *>       DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);                           <* 
    *>       return rce;                                                              <* 
    *>    }                                                                           <* 
    *>    yvikeys_mark__return  ('>');                                                        <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(common quick)-------------------*/
   --rce;  if (strchr("#_!", a_minor) != NULL) {
      switch (a_minor) {
      case '#' :
         DEBUG_USER   yLOG_note    ("unset mark under cursor");
         rc = yvikeys_mark__which ();
         if (rc < 0) {
            MODE_exit ();
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         yvikeys_mark__unset (rc);
         break;
      case '_' :
         DEBUG_USER   yLOG_note    ("toggle mark show and hide");
         if (myVIKEYS.mark_show == 'y')   myVIKEYS.mark_show = '-';
         else                             myVIKEYS.mark_show = 'y';
         break;
      case '!' :
         DEBUG_USER   yLOG_note    ("use mark status bar");
         yVIKEYS_cmds_direct (":status mark");
         break;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(common complex)-----------------*/
   --rce;  if (a_minor == '?') {
      DEBUG_USER   yLOG_note    ("show mark info window");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      myVIKEYS.info_win = 'y';
      return a_major;
   }
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("handling mark (m)");
      rc = yvikeys_mark__set (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == '\'') {
      DEBUG_USER   yLOG_note    ("handling return (')");
      rc = yvikeys_mark__return (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
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



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.940.221.74]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_mark__unit           (char *a_question, char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_RECD];
   int         x_index     = 0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MARK             : question not understood");
   /*---(defense)------------------------*/
   switch (a_mark) {
   case  0   :
      x_index = 0;  break;
   default   :
      x_index = yvikeys_mark__index (a_mark);
      if (x_index < 0) {
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK invalid     : %c not defined", a_mark);
      }
   }
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "list"        )   == 0) {
      yvikeys_mark_listplus (x_list);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK list        : %-.80s", x_list);
   }
   else if (strcmp (a_question, "info"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK info        : %c %c %-12.12s %4db %4dx %4dy %4dz", a_mark, s_mark_info [x_index].source, s_mark_info [x_index].label, s_mark_info [x_index].b_pos, s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   }
   else if (strcmp (a_question, "range"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK range       : %c to %c", s_mark_head, s_mark_tail);
   }
   /*> else if (strcmp (a_question, "mark_entry"  )   == 0) {                         <* 
    *>    MARK_entry (a_mark, x_list);                                                <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK entry       :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*> else if (strcmp (a_question, "mark_status" )   == 0) {                         <* 
    *>    yvikeys_mark_status (x_list);                                                       <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK status      :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


