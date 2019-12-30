/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



#define     S_SUNDO_MAX      1000
typedef struct cSUNDO  tSUNDO;
struct cSUNDO {
   int         seq;
   char        major;
   char        minor;
   int         cpos;
   uchar       before;
   uchar       after;
};
static tSUNDO s_sundos  [S_SUNDO_MAX];

static int    s_nsundo    =  0;
static int    s_csundo    = -1;
static int    s_nseq      = -1;



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_sundo_purge        (int a_start)
{
   int         i           =    0;
   for (i = a_start; i < S_SUNDO_MAX; ++i) {
      s_sundos [i].seq    = -1;
      s_sundos [i].major  = '-';
      s_sundos [i].minor  = '-';
      s_sundos [i].cpos   = -1;
      s_sundos [i].before = '-';
      s_sundos [i].after  = '-';
   }
   if (a_start == 0) {
      s_nsundo =  0;
      s_csundo = -1;
      s_nseq   = -1;
   }
   return 0;
}

char
yvikeys_sundo_init         (void)
{
   char        rc          =    0;
   rc = yvikeys_sundo_purge (0);
   /*> rc = yVIKEYS_view_optionX (YVIKEYS_STATUS, "sundo", yvikeys_sundo_status, "source editing undo stack");   <*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       change grouping                        ----===*/
/*====================------------------------------------====================*/
static void  o___GROUPING________o () { return; }

char
yvikeys_sundo_beg       (char *a_function)
{
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   DEBUG_EDIT   yLOG_schar   (myVIKEYS.repeating);
   DEBUG_EDIT   yLOG_sint    (REPEAT_original ());
   DEBUG_EDIT   yLOG_sint    (REPEAT_count    ());
   DEBUG_EDIT   yLOG_sint    (s_nseq);
   DEBUG_EDIT   yLOG_sint    (REPEAT_not  ());
   DEBUG_EDIT   yLOG_sint    (KEYS_unique ());
   if      (s_nseq < 0)                       ++s_nseq;
   else if (REPEAT_not () && KEYS_unique ())  ++s_nseq;
   DEBUG_EDIT   yLOG_sint    (s_nseq);
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yivkeys_sundo_chain     (void)
{
   --s_nseq;
   return 0;
}


char
yvikeys_sundo_end       (char *a_function)
{
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       recording changes                      ----===*/
/*====================------------------------------------====================*/
static void  o___RECORD__________o () { return; }

char
yvikeys_sundo_add       (char a_major, char a_minor, int a_pos, char a_before, char a_after)
{
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   ++s_csundo;
   s_sundos [s_csundo].seq    = s_nseq;
   s_sundos [s_csundo].major  = a_major;
   s_sundos [s_csundo].minor  = a_minor;
   s_sundos [s_csundo].cpos   = a_pos;
   s_sundos [s_csundo].before = chrvisible (a_before);
   s_sundos [s_csundo].after  = chrvisible (a_after);
   s_nsundo = s_csundo + 1;
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sundo_single    (char a_minor, int a_pos, char a_before, char a_after)
{
   yvikeys_sundo_add (G_KEY_SPACE, a_minor, a_pos, a_before, a_after);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       undoing actions                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNDO____________o () { return; }

char
yvikeys_sundo__undo_one     (int *a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_EDIT   yLOG_sint    (s_nsundo);
   DEBUG_EDIT   yLOG_sint    (s_csundo);
   --rce;  if (s_nsundo <= 0) {
      DEBUG_EDIT   yLOG_snote   ("stack too small");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_csundo <  0) {
      DEBUG_EDIT   yLOG_snote   ("current too small");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_pos == NULL) {
      DEBUG_EDIT   yLOG_snote   ("no position provided");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(position)-----------------------*/
   *a_pos = s_sundos [s_csundo].cpos;
   DEBUG_EDIT   yLOG_value   ("cpos"      , *a_pos);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].after);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].major);
   /*---(single char)--------------------*/
   if (s_sundos [s_csundo].major == G_KEY_SPACE) {
      switch (s_sundos [s_csundo].minor) {
      case 'r' : case 'R' :
         yvikeys_src_one_replace (s_sundos [s_csundo].before);
         break;
      case 'i' : case 'a' :
         yvikeys_src_one_delete  ();
         break;
      }
   }
   /*---(multi char)---------------------*/
   else if (s_sundos [s_csundo].major == 'd') {
      yvikeys_src_one_insert  (s_sundos [s_csundo].before);
   }
   else if (s_sundos [s_csundo].major == 'x') {
      yvikeys_src_one_replace (s_sundos [s_csundo].before);
   }
   /*---(update)-------------------------*/
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].before);
   --s_csundo;
   DEBUG_EDIT   yLOG_sint    (s_csundo);
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sundo_undo      (int *a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_seq       =   -1;
   int         i           =    0;
   int         c           =    0;
   int         rc          =    0;
   /*---(prepare)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   x_seq = s_sundos [s_csundo].seq;
   DEBUG_EDIT   yLOG_value   ("x_seq"     , x_seq);
   for (i = s_csundo; i >= 0; --i) {
      if (s_sundos [s_csundo].seq != x_seq)  break;
      rc = yvikeys_sundo__undo_one (a_pos);
      if (rc < 0) break;
      ++c;
   }
   if (c <= 0)  rc = -1;
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                       redoing actions                        ----===*/
/*====================------------------------------------====================*/
static void  o___REDO____________o () { return; }

char
yvikeys_sundo__redo_one (int *a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_EDIT   yLOG_sint    (s_nsundo);
   DEBUG_EDIT   yLOG_sint    (s_csundo);
   --rce;  if (s_csundo < -1) {
      DEBUG_EDIT   yLOG_snote   ("current too small");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_csundo >= s_nsundo - 1) {
      DEBUG_EDIT   yLOG_snote   ("current is too big");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_pos == NULL) {
      DEBUG_EDIT   yLOG_snote   ("no position provided");
      DEBUG_EDIT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(update)-------------------------*/
   ++s_csundo;
   DEBUG_EDIT   yLOG_sint    (s_csundo);
   /*---(position)-----------------------*/
   *a_pos = s_sundos [s_csundo].cpos;
   DEBUG_EDIT   yLOG_value   ("cpos"      , *a_pos);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].before);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].major);
   /*---(single char)--------------------*/
   if (s_sundos [s_csundo].major == G_KEY_SPACE) {
      switch (s_sundos [s_csundo].minor) {
      case 'r' : case 'R' :
         yvikeys_src_one_replace (s_sundos [s_csundo].after);
         break;
      case 'i' : case 'a' :
         yvikeys_src_one_insert  (s_sundos [s_csundo].after);
         break;
      }
   }
   /*---(multi char)---------------------*/
   else if (s_sundos [s_csundo].major == 'd') {
      yvikeys_src_one_delete  ();
   }
   else if (s_sundos [s_csundo].major == 'x') {
      yvikeys_src_one_replace (s_sundos [s_csundo].after);
   }
   /*---(update)-------------------------*/
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].after);
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sundo_redo             (int *a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_seq       =   -1;
   int         i           =    0;
   int         c           =    0;
   int         rc          =    0;
   /*---(prepare)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   x_seq = s_sundos [s_csundo + 1].seq;
   DEBUG_EDIT   yLOG_value   ("x_seq"     , x_seq);
   for (i = s_csundo; i < s_nsundo; ++i) {
      if (s_sundos [s_csundo + 1].seq != x_seq)  break;
      rc = yvikeys_sundo__redo_one (a_pos);
      if (rc < 0) break;
      ++c;
   }
   if (c <= 0)  rc = -1;
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                    status and information                    ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

char
yvikeys_sundo_status       (char *a_list)
{
   char        rce         =  -10;
   int         i           =    0;
   int         x_beg       =    0;
   int         x_end       =    0;
   char        t           [LEN_LABEL];
   char        x_ch        =  ' ';
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(fast path)-------------------*/
   if (s_nsundo == 0) {
      sprintf (a_list, "%3dn, %3dc", s_nsundo, s_csundo);
      return 0;
   }
   /*---(prepare)---------------------*/
   x_end = s_nsundo;
   x_beg = x_end - 10;
   if (x_beg < 0)  x_beg = 0;
   /*---(write line)------------------*/
   sprintf (a_list, "%3dn, %3dc, ", s_nsundo, s_csundo);
   for (i = x_beg; i < x_end; ++i) {
      x_ch = s_sundos [i].major;
      if (x_ch != G_KEY_SPACE)  sprintf (t, "%d%c%c%d%c%c,", s_sundos [i].seq, x_ch, s_sundos [i].minor, s_sundos [i].cpos, s_sundos [i].before, s_sundos [i].after);
      else              sprintf (t, "%d%c%d%c%c,"          , s_sundos [i].seq,       s_sundos [i].minor, s_sundos [i].cpos, s_sundos [i].before, s_sundos [i].after);
      strlcat (a_list, t, LEN_RECD);
   }
   /*---(complete)--------------------*/
   return 0;
}
