/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

static char s_mark_head   = '-';
static char s_mark_tail   = '-';

static char s_mark_curr   [LEN_LABEL] = "-";
static char s_mark_prev   [LEN_LABEL] = "-";
static char s_mark_pprev  [LEN_LABEL] = "-";



#define     S_MAX_MARK       150
typedef  struct cMARK  tMARK;
struct cMARK {
   char        label       [LEN_LABEL];
   int         x_pos;
   int         y_pos;
   int         z_pos;
   char        source;
};
tMARK       s_mark_info [S_MAX_MARK];
static char S_MARK_LIST [S_MAX_MARK] = "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()";
static int  s_nmark       = 0;

#define     MARK_ALL      '*'
#define     MARK_LOWER    'a'
#define     MARK_UPPER    'A'
#define     MARK_SYSTEM   '0'

#define     MARK_NONE     '-'
#define     MARK_USER     'u'
#define     MARK_IMPORT   'i'
#define     MARK_AUTO     'a'

char         MARK__purge           (char  a_scope);
char         MARK__unset           (char  a_mark);
char         MARK__range           (void);
char         MARK__prev            (void);
char         MARK__next            (void);



/*====================------------------------------------====================*/
/*===----                         location marks                       ----===*/
/*====================------------------------------------====================*/
static void  o___MARKS___________o () { return; }

char         /*-> initialize all marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
MARK_init            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (UMOD_MARK)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   s_nmark = strllen (S_MARK_LIST, 100);
   MARK__purge  ('*');
   /*---(globals)------------------------*/
   myVIKEYS.mark_show = '-';
   yVIKEYS_cmds_add ('e', "mark"        , ""    , "s"    , MARK_direct                , "" );
   /*---(read/write)---------------------*/
   yVIKEYS_file_add (UMOD_MARK   , MARK_writer, MARK_reader);
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_MARK);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> remove marks -----------------------[ ------ [gz.642.141.12]*/ /*-[01.0000.033.3]-*/ /*-[--.---.---.--]-*/
MARK__purge          (char a_scope)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_mark      = 0;
   char       *x_upper     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   char       *x_sys       = "0123456789";
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   DEBUG_MARK   yLOG_value   ("nmark"     , s_nmark);
   for (i = 0; i < s_nmark; ++i) {
      x_mark = S_MARK_LIST [i];
      if (a_scope == MARK_UPPER  && strchr (x_upper, x_mark) == NULL)  continue;
      if (a_scope == MARK_LOWER  && strchr (x_lower, x_mark) == NULL)  continue;
      if (a_scope == MARK_SYSTEM && strchr (x_sys  , x_mark) == NULL)  continue;
      MARK__unset (x_mark);
   }
   /*---(globals)------------------------*/
   DEBUG_MARK   yLOG_note    ("initialize globals");
   s_mark_head  = '-';
   s_mark_tail  = '-';
   strlcpy (s_mark_curr , "-", LEN_LABEL);
   strlcpy (s_mark_prev , "-", LEN_LABEL);
   strlcpy (s_mark_pprev, "-", LEN_LABEL);
   MARK__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_BASICS_____o () { return; }

char         /*-> check mark validity ----------------[ leaf   [ge.833.144.30]*/ /*-[01.0000.0A4.F]-*/ /*-[--.---.---.--]-*/
MARK_valid            (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *x_mark      = NULL;
   int         x_index     =   0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_senter  (__FUNCTION__);
   DEBUG_MARK   yLOG_sint    (a_mark);
   /*---(defenses)-----------------------*/
   --rce;  if (a_mark == '\0') {
      DEBUG_MARK   yLOG_snote   ("null is invalid");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   x_mark = strchr (S_MARK_LIST, a_mark);
   DEBUG_MARK   yLOG_spoint  (x_mark);
   --rce;  if (x_mark == NULL) {
      DEBUG_MARK   yLOG_snote   ("not valid");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to index)---------------*/
   x_index = (int) (x_mark - S_MARK_LIST);
   DEBUG_MARK   yLOG_sint    (x_index);
   /*---(check limits)-------------------*/
   --rce;  if (x_index >= s_nmark) {
      DEBUG_MARK   yLOG_snote   ("over max");
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_sexit   (__FUNCTION__);
   return x_index;
}

char         /*-> check mark validity ----------------[ leaf   [ge.833.144.30]*/ /*-[01.0000.0A4.F]-*/ /*-[--.---.---.--]-*/
MARK__check           (char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         x_len       =    0;
   int        *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()-:;";
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_senter  (__FUNCTION__);
   /*---(check label)--------------------*/
   DEBUG_MARK   yLOG_spoint  (a_label);
   --rce;  if (a_label == NULL) {
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check len)----------------------*/
   x_len = strllen (a_label, LEN_LABEL);
   DEBUG_MARK   yLOG_sint    (x_len);
   --rce;  if (x_len <= 2) {
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check characters)---------------*/
   --rce;  for (i = 0; i < x_len; ++i) {
      if (strchr (x_valid, a_label [i]) != NULL) continue;
      DEBUG_MARK   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
MARK__history         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(cycle history)------------------*/
   strlcpy (s_mark_pprev , s_mark_prev    , LEN_LABEL);
   strlcpy (s_mark_prev  , s_mark_curr    , LEN_LABEL);
   strlcpy (s_mark_curr  , SOURCE_label (), LEN_LABEL);
   DEBUG_MARK   yLOG_info    ("mark_pprev", s_mark_pprev);
   DEBUG_MARK   yLOG_info    ("mark_prev" , s_mark_prev);
   DEBUG_MARK   yLOG_info    ("mark_curr" , s_mark_curr);
   /*---(set prev mark)------------------*/
   if (strcmp (s_mark_prev, "-") != 0) {
      x_index = MARK__find (s_mark_prev);
      /*> strlcpy (s_mark_info [0].label, s_mark_prev, LEN_LABEL);                    <*/
   } else {
      x_index = MARK__find (s_mark_curr);
      /*> strlcpy (s_mark_info [0].label, s_mark_curr, LEN_LABEL);                    <*/
   }
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   /*---(fill in data)-------------------*/
   strlcpy (s_mark_info [0].label, s_mark_info [x_index].label, LEN_LABEL);
   DEBUG_MARK   yLOG_info    ("'"         , s_mark_info [0].label);
   s_mark_info [0].x_pos  = s_mark_info [x_index].x_pos;
   s_mark_info [0].y_pos  = s_mark_info [x_index].y_pos;
   s_mark_info [0].z_pos  = s_mark_info [x_index].z_pos;
   s_mark_info [0].source = MARK_AUTO;
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
MARK__set             (char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   char        rce         = -10;
   int         x_index     =   0;
   char        x_label     [10];
   char        x_prev      =   -1;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_mark);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_index == 0) {
      DEBUG_MARK   yLOG_note    ("never directly set previous mark");
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check label)--------------------*/
   DEBUG_MARK   yLOG_note    ("check label");
   rc = MARK__check (SOURCE_label ());
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark label)---------------------*/
   DEBUG_MARK   yLOG_note    ("save current position");
   strlcpy (s_mark_info [x_index].label, SOURCE_label (), LEN_LABEL);
   MAP_locator (SOURCE_label (), &s_mark_info [x_index].x_pos, &s_mark_info [x_index].y_pos, &s_mark_info [x_index].z_pos);
   s_mark_info [x_index].source = MARK_USER;
   /*---(set history)--------------------*/
   MARK__history ();
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear a mark -----------------------[ ------ [ge.732.142.12]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
MARK__unset           (char a_mark)
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
   x_index = MARK_valid (a_mark);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear mark)---------------------*/
   DEBUG_MARK   yLOG_note    ("clear values");
   strlcpy (s_mark_info [x_index].label, "-", LEN_LABEL);
   s_mark_info [x_index].source  = MARK_NONE;
   s_mark_info [x_index].x_pos   = 0;
   s_mark_info [x_index].y_pos   = 0;
   s_mark_info [x_index].z_pos   = 0;
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.E83.174.A7]*/ /*-[01.0000.013.#]-*/ /*-[--.---.---.--]-*/
MARK__return        (char a_mark)
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
   case '[' : a_mark = s_mark_head;     break;
   case '<' : a_mark = MARK__prev ();   break;
   case '>' : a_mark = MARK__next ();   break;
   case ']' : a_mark = s_mark_tail;     break;
   }
   --rce;  if (a_mark < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_mark);
   x_index = MARK_valid (a_mark);
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
   MAP_jump (s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   /*---(set history)--------------------*/
   MARK__history ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_FIND_______o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gc.430.022.40]*/ /*-[01.0000.013.5]-*/ /*-[--.---.---.--]-*/
MARK__which        (void)
{
   int         i           = 0;
   char        x_label     [LEN_LABEL];
   strlcpy (x_label, SOURCE_label (), LEN_LABEL);
   for (i = 1; i < s_nmark; ++i) {
      if (s_mark_info [i].source == MARK_NONE)          continue;
      if (strcmp (s_mark_info [i].label, x_label) != 0) continue;
      return S_MARK_LIST [i];
   }
   /*---(complete)-----------------------*/
   return -1;
}

char         /*-> tbd --------------------------------[ leaf   [ge.732.133.30]*/ /*-[01.0000.024.#]-*/ /*-[--.---.---.--]-*/
MARK__find          (char *a_label)
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
static void  o___MARK_SEQ________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.640.021.20]*/ /*-[01.0000.044.!]-*/ /*-[--.---.---.--]-*/
MARK__range         (void)
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
MARK__prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = MARK__find (s_mark_curr);
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
MARK__next          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   x_index = MARK__find (s_mark_curr);
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
static void  o___MARK_INFO_______o () { return; }

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
 *>    x_index = MARK_valid (a_mark);                                                                                             <* 
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
MARK_infowin       (char *a_entry, int a_index)
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
   n = MARK_valid (a);
   if (s_mark_info [n].source == MARK_NONE)  sprintf (t, " %c  -              -     -     -     ", a);
   else                                      sprintf (t, " %c  %-10.10s %5d,%5d,%5d     "        , a, s_mark_info [n].label, s_mark_info [n].x_pos, s_mark_info [n].y_pos, s_mark_info [n].z_pos);
   strlcpy (a_entry, t, LEN_RECD);
   a = 'A' + a_index;
   n = MARK_valid (a);
   if (s_mark_info [n].source == MARK_NONE)  sprintf (t, " %c  -              -     -     -     ", a);
   else                                      sprintf (t, " %c  %-10.10s %5d,%5d,%5d     "        , a, s_mark_info [n].label, s_mark_info [n].x_pos, s_mark_info [n].y_pos, s_mark_info [n].z_pos);
   strlcat (a_entry, t, LEN_RECD);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
MARK_status        (char *a_status)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_list      [LEN_RECD]  = "";
   int         c           =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_status  == NULL)  return rce;
   /*---(status)-------------------------*/
   MARK__listplus (x_list);
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
MARK__listplus      (char *a_list)
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
static void  o___MARK_FILE_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
MARK_writer           (char a_abbr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_beg       =    0;
   char        x_end       =    0;
   char        i           =    0;
   char        c           =    0;
   /*---(prepare)----------------s-------*/
   yVIKEYS_unit_reset ();
   if (a_abbr == 0) {
      x_beg = 1;
      x_end = s_nmark - 1;
   } else {
      x_beg = x_end = MARK_valid (a_abbr);
      if (x_beg <= 0)  return rce;
   }
   /*---(find marked entries)------------*/
   for (i = x_beg; i <= x_end; ++i) {
      if (s_mark_info [i].source == MARK_NONE)  continue;
      yVIKEYS_file_write (UMOD_MARK, &(S_MARK_LIST [i]), &s_mark_info [i].x_pos, &s_mark_info [i].y_pos, &s_mark_info [i].z_pos, NULL, NULL, NULL, NULL, NULL);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
MARK_reader           (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_index     =    0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_MARK   yLOG_char    ("version"   , n);
   --rce;  if (n != 'B') {
      DEBUG_MARK   yLOG_note    ("illegal version");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_MARK   yLOG_value   ("mark"      , a[0]);
   x_index = MARK_valid (a[0]);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("mark"      , a[0]);
   /*---(address)------------------------*/
   rc = MAP_addresser (s_mark_info [x_index].label, atoi (b), atoi (c), atoi (d));
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("label"     , s_mark_info [x_index].label);
   /*---(save)---------------------------*/
   DEBUG_MARK   yLOG_note    ("assign values to mark");
   s_mark_info [x_index].x_pos = atoi (b);
   DEBUG_MARK   yLOG_value   ("x_pos"     , s_mark_info [x_index].x_pos);
   s_mark_info [x_index].y_pos = atoi (c);
   DEBUG_MARK   yLOG_value   ("y_pos"     , s_mark_info [x_index].y_pos);
   s_mark_info [x_index].z_pos = atoi (d);
   DEBUG_MARK   yLOG_value   ("z_pos"     , s_mark_info [x_index].z_pos);
   s_mark_info [x_index].source = MARK_IMPORT;
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      mark command line                       ----===*/
/*====================------------------------------------====================*/
static void  o___MARK_COMMAND____o () { return; }

char         /*-> enter a mark directly --------------[ ------ [ge.D54.139.83]*/ /*-[02.0000.00#.O]-*/ /*-[--.---.---.--]-*/
MARK_direct          (char *a_string)
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
      MARK__purge (MARK_LOWER);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("purge", a_string) == 0) {
      MARK__purge (MARK_ALL);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("reset" , a_string) == 0) {
      MARK__purge (MARK_SYSTEM);
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
      rc = MARK__unset (a_string [0]);
      DEBUG_MARK   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check mark)---------------------*/
   x_index = MARK_valid (a_string [0]);
   DEBUG_MARK   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_char    ("a_mark"    , a_string [0]);
   /*---(save)---------------------------*/
   strlcpy (s_mark_info [x_index].label, a_string + 2, LEN_LABEL);
   rc = MAP_locator (s_mark_info [x_index].label, &s_mark_info [x_index].x_pos, &s_mark_info [x_index].y_pos, &s_mark_info [x_index].z_pos);
   --rce;  if (rc < 0) {
      MARK__unset (a_string [0]);
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_mark_info [x_index].source = MARK_IMPORT;
   /*---(go there)-----------------------*/
   MAP_jump (s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   /*---(set history)--------------------*/
   DEBUG_MARK   yLOG_note    ("set history");
   MARK__history ();
   /*---(update range)-------------------*/
   DEBUG_MARK   yLOG_note    ("update the range");
   MARK__range ();
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         mode handling                        ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
MARK_smode         (int a_major, int a_minor)
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
   DEBUG_USER   yLOG_char    ("x_majors"  , x_majors);
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
    *>    MARK__return  ('>');                                                        <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(common quick)-------------------*/
   --rce;  if (strchr("#_!", a_minor) != NULL) {
      switch (a_minor) {
      case '#' :
         DEBUG_USER   yLOG_note    ("unset mark under cursor");
         rc = MARK__which ();
         if (rc < 0) {
            MODE_exit ();
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
         MARK__unset (rc);
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
      rc = MARK__set (a_minor);
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
      rc = MARK__return (a_minor);
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
MARK__unit         (char *a_question, char a_mark)
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
      x_index = MARK_valid (a_mark);
      if (x_index < 0) {
         snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK invalid     : %c not defined", a_mark);
      }
   }
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "list"        )   == 0) {
      MARK__listplus (x_list);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK list        : %-.80s", x_list);
   }
   else if (strcmp (a_question, "info"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK info        : %c %c %-12.12s %4dx %4dy %4dz", a_mark, s_mark_info [x_index].source, s_mark_info [x_index].label, s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);
   }
   else if (strcmp (a_question, "range"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK range       : %c to %c", s_mark_head, s_mark_tail);
   }
   /*> else if (strcmp (a_question, "mark_entry"  )   == 0) {                         <* 
    *>    MARK_entry (a_mark, x_list);                                                <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK entry       :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*> else if (strcmp (a_question, "mark_status" )   == 0) {                         <* 
    *>    MARK_status (x_list);                                                       <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK status      :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


