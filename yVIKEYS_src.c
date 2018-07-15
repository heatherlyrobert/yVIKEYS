/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


typedef  struct cEDIT   tEDIT;
struct cEDIT {
   char        type;                      /* type of input                    */
   char        label       [LEN_LABEL];   /* source label                     */
   char        original    [LEN_RECD];    /* pre-edit content                 */
   char        contents    [LEN_RECD];    /* working content                  */
   int         wide;                      /* full display space width         */
   int         apos;                      /* available space for display      */
   int         npos;                      /* length of edited content         */
   int         cpos;                      /* current character position       */
   int         bpos;                      /* beginning of visiable part       */
   int         epos;                      /* end of visiable part             */
   char        words       [LEN_RECD];    /* working space for wbe keys       */
};
static tEDIT   s_src;
static tEDIT   s_cmd;
static tEDIT  *s_cur    = &s_src;


#define      EDIT_NORM        '-'
#define      EDIT_CMDS        'c'
#define      EDIT_SRCH        's'



#define     SELC_NOT       '-'
#define     SELC_YES       'y'
static char  s_live    = '-';
static int   s_root    = 0;
static int   s_bsel    = 0;
static int   s_esel    = 0;



#define     MAX_TREG     100
typedef struct  cSRC_REG  tSRC_REG;
struct cSRC_REG {
   char        label       [10];            /* original cell                  */
   int         bpos;                        /* original starting point        */
   int         epos;                        /* original ending point          */
   int         len;                         /* length of text                 */
   char        data        [LEN_RECD];       /* text                           */
   char        source;                      /* user, file, import             */
};
static      tSRC_REG    s_tregs  [MAX_TREG];
static      int         s_ntreg  = 32;
static char             s_ctreg  = '"';
static char             s_wtreg  = '"';


#define     TREG_NONE     '-'
#define     TREG_USER     'u'
#define     TREG_FILE     'f'
#define     TREG_IMPORT   'i'

static char    s_tregnames     [MAX_TREG] = "\"abcdefghijklmnopqrstuvwxyz0123456789+-";

static char    (*s_saver) (char *a_contents);



#define     MAX_SRC_UNDO      1000
typedef struct cSRC_UNDO  tSRC_UNDO;
struct cSRC_UNDO {
   int         seq;
   char        major;
   char        minor;
   int         cpos;
   uchar       before;
   uchar       after;
};
static tSRC_UNDO s_sundos  [MAX_SRC_UNDO];

static int    s_nsundo    =  0;
static int    s_csundo    = -1;
static int    s_nseq      = -1;



/*====================------------------------------------====================*/
/*===----                      working functions                       ----===*/
/*====================------------------------------------====================*/
static void  o___WORKERS_________o () { return; }

static char
ONE__replace            (char a_key)
{
   s_cur->contents [s_cur->cpos] = a_key;
   return 0;
}

static char
ONE__delete             (void)
{
   int         i           =   0;
   for (i = s_cur->cpos; i < s_cur->npos; ++i) {
      s_cur->contents [i] = s_cur->contents [i + 1];
   }
   --s_cur->npos;
   return 0;
}

static char
ONE__backspace          (void)
{
   if (s_cur->cpos <= 0)  return -1;
   --s_cur->cpos;
   ONE__delete  ();
   return 0;
}

static char
ONE__insert             (char a_key)
{
   int         i           =   0;
   for (i = s_cur->npos + 1; i >= s_cur->cpos; --i) {
      s_cur->contents [i + 1] = s_cur->contents [i];
   }
   ++s_cur->npos;
   ONE__replace (a_key);
   return 0;
}

static char
ONE__append             (char a_key)
{
   if (s_cur->npos == 0) {
      ONE__insert (a_key);
   } else {
      ++s_cur->cpos;
      ONE__insert (a_key);
   }
   ++s_cur->npos;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        undo and redo                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNDO_REDO_______o () { return; }

char
SRC_UNDO_status            (char *a_list)
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

char
SRC_UNDO__purge            (int a_start)
{
   int         i           =    0;
   for (i = a_start; i < MAX_SRC_UNDO; ++i) {
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
SRC_UNDO__chain_next       (void)
{
   --s_nseq;
   return 0;
}

char
SRC_UNDO__beg              (char *a_function)
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
SRC_UNDO__end              (char *a_function)
{
   DEBUG_EDIT   yLOG_senter  (__FUNCTION__);
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
SRC_UNDO__add              (char a_major, char a_minor, int a_pos, char a_before, char a_after)
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
SRC_UNDO__single           (char a_minor, char a_before, char a_after)
{
   SRC_UNDO__add (G_KEY_SPACE, a_minor, s_cur->cpos, a_before, a_after);
   return 0;
}

char
SRC_UNDO__undo_one         (void)
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
   /*---(position)-----------------------*/
   s_cur->cpos = s_sundos [s_csundo].cpos;
   DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].after);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].major);
   /*---(single char)--------------------*/
   if (s_sundos [s_csundo].major == G_KEY_SPACE) {
      switch (s_sundos [s_csundo].minor) {
      case 'r' : case 'R' :
         ONE__replace (s_sundos [s_csundo].before);
         break;
      case 'i' : case 'a' :
         ONE__delete  ();
         break;
      }
   }
   /*---(multi char)---------------------*/
   else if (s_sundos [s_csundo].major == 'd') {
      ONE__insert  (s_sundos [s_csundo].before);
   }
   else if (s_sundos [s_csundo].major == 'x') {
      ONE__replace (s_sundos [s_csundo].before);
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
SRC_UNDO__undo             (void)
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
      rc = SRC_UNDO__undo_one ();
      if (rc < 0) break;
      ++c;
   }
   if (c <= 0)  rc = -1;
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
SRC_UNDO__redo_one         (void)
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
   /*---(update)-------------------------*/
   ++s_csundo;
   DEBUG_EDIT   yLOG_sint    (s_csundo);
   /*---(position)-----------------------*/
   s_cur->cpos = s_sundos [s_csundo].cpos;
   DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].before);
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].major);
   /*---(single char)--------------------*/
   if (s_sundos [s_csundo].major == G_KEY_SPACE) {
      switch (s_sundos [s_csundo].minor) {
      case 'r' : case 'R' :
         ONE__replace (s_sundos [s_csundo].after);
         break;
      case 'i' : case 'a' :
         ONE__insert  (s_sundos [s_csundo].after);
         break;
      }
   }
   /*---(multi char)---------------------*/
   else if (s_sundos [s_csundo].major == 'd') {
      ONE__delete  ();
   }
   else if (s_sundos [s_csundo].major == 'x') {
      ONE__replace (s_sundos [s_csundo].after);
   }
   /*---(update)-------------------------*/
   DEBUG_EDIT   yLOG_schar   (s_sundos [s_csundo].after);
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
SRC_UNDO__redo             (void)
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
      rc = SRC_UNDO__redo_one ();
      if (rc < 0) break;
      ++c;
   }
   if (c <= 0)  rc = -1;
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return rc;
}


/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char         /*-> accept all source changes ----------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE__accept          (void)
{
   char        rc          =    0;
   char        t           [LEN_RECD] = "";
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   switch (MODE_curr ()) {
   case MODE_SOURCE  :
      DEBUG_EDIT   yLOG_note    ("save source back");
      if (s_saver != NULL && s_cur->npos > 0) {
         if (strcmp (s_cur->original, s_cur->contents) != 0)
            strlcpy (t, s_cur->contents, LEN_RECD);
         strldchg (t, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
         s_saver (t);
      }
      break;
   case MODE_COMMAND :
      DEBUG_EDIT   yLOG_note    ("execute command");
      rc = yVIKEYS_cmds_direct (s_cur->contents);
      strlcpy (s_cur->contents, "", LEN_RECD);
      break;
   case MODE_SEARCH  :
      DEBUG_EDIT   yLOG_note    ("execute search");
      rc = yVIKEYS_srch_direct (s_cur->contents);
      strlcpy (s_cur->contents, "", LEN_RECD);
      break;
   }
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   strlcpy (s_cur->original, s_cur->contents, LEN_RECD);
   s_cur->npos  = s_cur->bpos  = s_cur->cpos  = s_cur->epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   SRC_UNDO__purge (0);
   yVIKEYS_source (s_src.label, s_src.original);
   MAP_reposition  ();
   DEBUG_EDIT   yLOG_value   ("rc"        , rc);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> reject all source changes ----------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE__reset           (void)
{
   switch (MODE_curr ()) {
   case MODE_COMMAND :
   case MODE_SEARCH  :
      strlcpy (s_cur->original, "", LEN_RECD);
      break;
   }
   strlcpy (s_cur->contents, s_cur->original, LEN_RECD);
   s_cur->npos  = s_cur->bpos  = s_cur->cpos  = s_cur->epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   SRC_UNDO__purge (0);
   yVIKEYS_source (s_src.label, s_src.original);
   MAP_reposition  ();
   return 0;
}

char         /*-> prepare for source mode move -------[ leaf   [gz.412.001.00]*/ /*-[00.0000.313.!]-*/ /*-[--.---.---.--]-*/
SOURCE__prep            (void)
{
   /*---(prepare)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_info    ("contents"  , s_cur->contents);
   s_cur->npos     = strlen (s_cur->contents);
   /*---(display debugging)--------------*/
   DEBUG_EDIT   yLOG_value   ("npos"     , s_cur->npos);
   DEBUG_EDIT   yLOG_value   ("apos"     , s_cur->apos);
   DEBUG_EDIT   yLOG_value   ("bpos"     , s_cur->bpos);
   DEBUG_EDIT   yLOG_value   ("cpos"     , s_cur->cpos);
   DEBUG_EDIT   yLOG_value   ("epos"     , s_cur->epos);
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE__words_mode      (char a_1st, char a_curr, char a_save)
{
   /*---(locals)-----------+-----+-----+-*/
   char       *x_form      = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$";
   char       *x_word      = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-";
   char        x_mode      =  ' ';
   /*---(formula)------------------------*/
   if (strchr ("=#", a_1st) != NULL) {
      if      (strchr (x_form, a_curr) != NULL)     x_mode = 'w';
      else if (strchr ("¹ "  , a_curr) != NULL)     x_mode = '¹';
      else                                          x_mode = 's';
   }
   /*---(word)---------------------------*/
   else {
      if      (strchr ("¹ "  , a_curr) != NULL)     x_mode = '¹';
      else                                          x_mode = 'w';
   }
   /*---(complete)-----------------------*/
   return x_mode;
}

char
SOURCE__words_update    (int a_pos, char a_curr)
{
   /*---(before start)-------------------*/
   if (a_curr < 0)  return 0;
   /*---(beginning)----------------------*/
   if (a_curr == '<')   s_cur->words [a_pos] = '<';
   /*---(ending)-------------------------*/
   if (a_curr == '>') {
      if (s_cur->words [a_pos] == '<')  s_cur->words [a_pos] = 'B';
      else                              s_cur->words [a_pos] = '>';
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
SOURCE__words          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_change    =  '-';
   int         i           =    0;
   char        x_save      =  '¹';
   char        x_mode      =  '¹';
   /*---(prepare)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_value   ("s_npos"    , s_cur->npos);
   DEBUG_EDIT   yLOG_info    ("contents"  , s_cur->contents);
   s_cur->words [s_cur->npos] = 0;
   for (i = 0; i < s_cur->npos; ++i) {
      /*---(initialize)------------------*/
      s_cur->words [i] = '¹';
      /*---(check)-----------------------*/
      x_mode = SOURCE__words_mode (s_cur->contents [0], s_cur->contents [i], x_save);
      if (x_mode == x_save)  continue;
      /*---(update current)--------------*/
      if (x_save == '¹' && x_mode == 'w')   SOURCE__words_update (i    , '<');
      if (x_save == '¹' && x_mode == 's')   SOURCE__words_update (i    , '<');
      if (x_save == 'w' && x_mode == 's')   SOURCE__words_update (i    , '<');
      if (x_save == 's' && x_mode == 'w')   SOURCE__words_update (i    , '<');
      /*---(update previous)-------------*/
      if (x_save == 'w' && x_mode == '¹')   SOURCE__words_update (i - 1, '>'); 
      if (x_save == 's' && x_mode == '¹')   SOURCE__words_update (i - 1, '>'); 
      if (x_save == 'w' && x_mode == 's')   SOURCE__words_update (i - 1, '>');
      if (x_save == 's' && x_mode == 'w')   SOURCE__words_update (i - 1, '>');
      /*---(save)------------------------*/
      x_save = x_mode;
      /*---(done)------------------------*/
   }
   if (x_mode != '¹')  SOURCE__words_update (s_cur->npos - 1, '>');
   DEBUG_EDIT   yLOG_info    ("words"     , s_cur->words);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> complete a source mode move --------[ leaf   [gz.E45.001.A0]*/ /*-[02.0000.513.!]-*/ /*-[--.---.---.--]-*/
SOURCE__done            (void)
{
   /*---(prepare)------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_info    ("s_contents", s_cur->contents);
   s_cur->npos     = strlen (s_cur->contents);
   /*---(display debugging)--------------*/
   DEBUG_EDIT   yLOG_value   ("s_npos"   , s_cur->npos);
   DEBUG_EDIT   yLOG_value   ("s_apos"   , s_cur->apos);
   DEBUG_EDIT   yLOG_value   ("s_bpos"   , s_cur->bpos);
   DEBUG_EDIT   yLOG_value   ("s_cpos"   , s_cur->cpos);
   DEBUG_EDIT   yLOG_value   ("s_epos"   , s_cur->epos);
   /*---(check over/underrun)---------*/
   DEBUG_EDIT   yLOG_note    ("correct s_npos over/underruns");
   if (s_cur->npos  >= LEN_RECD) {
      s_cur->npos = LEN_RECD - 1;
      s_cur->contents [s_cur->npos] = '\0';
   }
   if (s_cur->npos < 0) {
      s_cur->npos = 0;
   }
   /*---(check min/max)---------------*/
   DEBUG_EDIT   yLOG_note    ("correct min/max limits");
   if (s_cur->cpos >=  s_cur->npos)     s_cur->cpos = s_cur->npos - 1;
   if (s_cur->cpos <   0      )    s_cur->cpos = 0;
   if (s_cur->bpos <   0      )    s_cur->bpos = 0;
   /*---(small strings)---------------*/
   if (s_cur->npos == 0) {
      DEBUG_EDIT   yLOG_note    ("check and handle small string");
      s_cur->cpos = s_cur->bpos = s_cur->epos = 0;
   }
   /*---(small strings)---------------*/
   else if (s_cur->npos <= s_cur->apos) {
      DEBUG_EDIT   yLOG_note    ("check and handle small string");
      s_cur->bpos = 0;
      s_cur->epos = s_cur->npos - 1;
   }
   /*---(long strings)----------------*/
   else {
      DEBUG_EDIT   yLOG_note    ("check and handle long string");
      s_cur->epos = s_cur->bpos + s_cur->apos - 1;
      /*---(check off left side)------*/
      DEBUG_EDIT   yLOG_note    ("check off left side of screen");
      if (s_cur->cpos <   s_cur->bpos)  {
         s_cur->bpos = s_cur->cpos;
         s_cur->epos = s_cur->bpos + s_cur->apos - 1;
      }
      /*---(check off right side)-----*/
      DEBUG_EDIT   yLOG_note    ("check off right side of screen");
      if (s_cur->cpos >   s_cur->epos)  {
         s_cur->epos = s_cur->cpos;
         s_cur->bpos = s_cur->epos - s_cur->apos + 1;
      }
      /*---(check scrolling-----------*/
      DEBUG_EDIT   yLOG_note    ("make sure right is locked to end");
      if (s_cur->epos >=  s_cur->npos)  {
         s_cur->epos = s_cur->npos - 1;
         s_cur->bpos = s_cur->epos - s_cur->apos + 1;
      }
   }
   /*---(selection update)---------------*/
   if (s_live != SELC_YES) {
      s_root = s_bsel = s_esel = s_cur->cpos;
   } else if (s_cur->cpos < s_root) {
      s_bsel = s_cur->cpos;
      s_esel = s_root;
   } else {
      s_bsel = s_root;
      s_esel = s_cur->cpos;
   }
   /*---(update word breaks)-------------*/
   SOURCE__words ();
   /*---(display debugging)--------------*/
   DEBUG_EDIT   yLOG_value   ("s_npos"   , s_cur->npos);
   DEBUG_EDIT   yLOG_value   ("s_apos"   , s_cur->apos);
   DEBUG_EDIT   yLOG_value   ("s_bpos"   , s_cur->bpos);
   DEBUG_EDIT   yLOG_value   ("s_cpos"   , s_cur->cpos);
   DEBUG_EDIT   yLOG_value   ("s_epos"   , s_cur->epos);
   /*---(complete)--------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare contents for new edit ------[ leaf   [gz.311.101.00]*/ /*-[00.0000.203.!]-*/ /*-[--.---.---.--]-*/
SOURCE_start       (char *a_prefix)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_EDIT   yLOG_point   ("a_prefix"  , a_prefix);
   --rce;  if (a_prefix == NULL) {
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_EDIT   yLOG_info    ("a_prefix"  , a_prefix);
   /*---(select mode)--------------------*/
   switch (a_prefix [0]) {
   case ':' :
      s_cur = &s_cmd;
      MODE_enter  (MODE_COMMAND);
      break;
   case '/' :
      s_cur = &s_cmd;
      MODE_enter  (MODE_SEARCH );
      break;
   case '¦' :
      s_cur = &s_src;
      MODE_enter  (MODE_SOURCE );
      break;
   default  :
      s_cur = &s_src;
      MODE_enter  (MODE_SOURCE );
      break;
   }
   /*---(clear sundo)--------------------*/
   SRC_UNDO__purge (0);
   /*---(populate globals)---------------*/
   if (a_prefix [0] != '¦')  strlcpy (s_cur->contents, a_prefix , LEN_RECD);
   s_cur->npos = strllen (s_cur->contents, LEN_RECD);
   s_cur->bpos = s_cur->cpos = 0;
   s_cur->epos = s_cur->npos;
   SOURCE__done ();
   /*---(go info input)------------------*/
   if (a_prefix [0] != '¦') {
      MODE_enter  (UMOD_SRC_INPT  );
      SRC_INPT_umode  ('m', 'a');
   }
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> return current source label --------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE_label            (void)
{
   return s_src.label;
}

char         /*-> go back to defaults ----------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__reset         (void)
{
   s_live  = SELC_NOT;
   s_root  = s_esel  = s_bsel  = s_cur->cpos;
   s_ctreg = '"';
   return 0;
}

char
SRC_REG_valid          (cchar a_reg)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < s_ntreg; ++i) {
      if (s_tregnames [i] == a_reg)  return  i;
   }
   return -1;
}



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize source environment ------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE_init             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_SOURCE)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(pointer)------------------------*/
   DEBUG_PROG   yLOG_note    ("clear s_saver pointer");
   s_saver  = NULL;
   /*---(command/search)-----------------*/
   DEBUG_PROG   yLOG_note    ("clear command/search values");
   s_cur = &s_cmd;
   s_cur->type  = EDIT_CMDS;
   strlcpy (s_cur->original, "" , LEN_RECD );
   strlcpy (s_cur->label   , "-", LEN_LABEL);
   SOURCE__reset ();
   /*---(source)-------------------------*/
   DEBUG_PROG   yLOG_note    ("clear source values");
   s_cur = &s_src;
   s_cur->type  = EDIT_NORM;
   strlcpy (s_cur->original, "" , LEN_RECD );
   strlcpy (s_cur->label   , "-", LEN_LABEL);
   /*---(clear)--------------------------*/
   SOURCE__reset ();
   SRC_UNDO__purge (0);
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_SOURCE);
   STATUS_init_set   (UMOD_SRC_REPL);
   STATUS_init_set   (UMOD_SRC_INPT);
   STATUS_init_set   (SMOD_SRC_UNDO);
   STATUS_init_set   (UMOD_HISTORY);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
SRC_REG_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_SRC_REG)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(registers)----------------------*/
   s_ctreg  = '"';
   DEBUG_PROG   yLOG_char    ("s_ctreg"   , s_ctreg);
   s_wtreg  = '"';
   DEBUG_PROG   yLOG_char    ("s_wtreg"   , s_wtreg);
   DEBUG_PROG   yLOG_info    ("tregnames" , s_tregnames);
   s_ntreg  = strllen (s_tregnames, MAX_TREG);
   DEBUG_PROG   yLOG_value   ("s_ntreg"   , s_ntreg);
   /*---(purge)--------------------------*/
   for (i = 0; i < MAX_TREG; ++i) {
      strlcpy (s_tregs [i].label, "-", 10);
      s_tregs [i].bpos   =  0;
      s_tregs [i].epos   =  0;
      s_tregs [i].len    =  0;
      strlcpy (s_tregs [i].data , "", LEN_RECD);
      s_tregs [i].source = TREG_NONE;
   }
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_SRC_REG);
   yVIKEYS_file_add (SMOD_SRC_REG, SRC_REG_writer, SRC_REG_reader);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initialize source environment ------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_src_config        (void *a_saver)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (MODE_SOURCE)) {
      DEBUG_EDIT   yLOG_note    ("init must complete before config");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save pointer)-------------------*/
   if (a_saver != NULL)  s_saver  = a_saver;
   /*---(update status)------------------*/
   STATUS_conf_set   (MODE_SOURCE, '1');
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_source          (char *a_label, char *a_contents)
{
   /*---(fix pointer)--------------------*/
   s_cur = &s_src;
   s_cur->type  = EDIT_NORM;
   /*---(check inputs)-------------------*/
   if (a_label    == NULL)  strlcpy (s_cur->label   , "-"       , LEN_RECD);
   else                     strlcpy (s_cur->label   , a_label   , LEN_RECD);
   if (a_contents == NULL)  strlcpy (s_cur->original, ""        , LEN_RECD);
   else                     strlcpy (s_cur->original, a_contents, LEN_RECD);
   strldchg (s_cur->original, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   strlcpy (s_cur->contents, s_cur->original, LEN_RECD);
   /*---(reset content)------------------*/
   s_cur->npos = strllen (s_cur->contents, LEN_RECD);
   s_cur->bpos = s_cur->cpos = 0;
   s_cur->epos = s_cur->npos;
   /*---(process)------------------------*/
   SOURCE__done ();
   /*---(complete)-----------------------*/
   return 0;
}

char
SOURCE_other            (char a_type)
{
   /*---(fix pointer)--------------------*/
   switch (a_type) {
   case EDIT_CMDS :
      s_cur = &s_cmd;
      s_cur->type  = EDIT_CMDS;
      break;
   case EDIT_SRCH :
      s_cur = &s_cmd;
      s_cur->type  = EDIT_SRCH;
      break;
   default        :
      return -1;
      break;
   }
   /*---(check inputs)-------------------*/
   strlcpy (s_cur->label   , "-"       , LEN_RECD);
   strlcpy (s_cur->original, ""        , LEN_RECD);
   strlcpy (s_cur->contents, s_cur->original, LEN_RECD);
   /*---(reset content)------------------*/
   s_cur->npos = strllen (s_cur->contents, LEN_RECD);
   s_cur->bpos = s_cur->cpos = 0;
   s_cur->epos = s_cur->npos;
   /*---(process)------------------------*/
   SOURCE__done ();
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       drawing functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

char
SOURCE__color           (char a_display)
{
   /*---(locals)-----------+------+----+-*/
   char        x_edit      =   '-';
   char        x_code      =   '-';
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_char    ("a_display" , a_display);
   DEBUG_EDIT   yLOG_char    ("MODE_curr" , MODE_curr ());
   DEBUG_EDIT   yLOG_char    ("MODE_prev" , MODE_prev ());
   /*---(fast-track formula)-------------*/
   if (a_display == YVIKEYS_FORMULA) {
      if (MODE_curr () != MODE_SOURCE && MODE_prev () != MODE_SOURCE)  {
         DEBUG_EDIT   yLOG_note    ("formula fast track");
         x_code = 'm';
      }
   }
   /*---(fast-track command)-------------*/
   if (a_display == YVIKEYS_COMMAND) {
      if (strchr (":/", MODE_curr ()) == NULL && strchr (":/", MODE_prev ()) == NULL) {
         DEBUG_EDIT   yLOG_note    ("command fast track");
         x_code = 'c';
      }
   }
   /*---(check focused)------------------*/
   if (x_code == '-' && strchr (MODES_ONELINE, MODE_curr ()) != NULL)  {
      DEBUG_EDIT   yLOG_note    ("current source mode");
      x_edit = 'y';
      x_code = 's';
   }
   /*---(check editing)------------------*/
   else if (x_code == '-' && strchr (MODES_ONELINE, MODE_prev ()) != NULL)  {
      DEBUG_EDIT   yLOG_note    ("sub-mode editing");
      x_edit = 'y';
      switch (MODE_curr ()) {
      case SMOD_SRC_REG : x_code = 't';  break;
      case UMOD_SRC_REPL : x_code = 'r';  break;
      case UMOD_SRC_INPT   : x_code = 'i';  break;
      case UMOD_WANDER  : x_code = 'w';  break;
      default           : x_code = '-';  break;
      }
   }
   /*---(just a safety)------------------*/
   else if (x_code == '-') {
      DEBUG_EDIT   yLOG_note    ("catch for mistakes");
      x_code = 'e';
   }
   /*---(set curses color)---------------*/
   attrset     (0);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      DEBUG_EDIT   yLOG_note    ("assign a ncurses color");
      switch (x_code) {
      case  'm' :  yCOLOR_curs ("map"    );  break;
      case  's' :  yCOLOR_curs ("source" );  break;
      case  'c' :  yCOLOR_curs ("command");  break;
      case  't' :  yCOLOR_curs ("textreg");  break;
      case  'r' :  yCOLOR_curs ("replace");  break;
      case  'i' :  yCOLOR_curs ("input"  );  break;
      case  'w' :  yCOLOR_curs ("wander" );  break;
      case  'e' :  yCOLOR_curs ("error"  );  break;
      default   :  yCOLOR_curs ("map"    );  break;
      }
   }
   /*---(set opengl color)---------------*/
   else {
      DEBUG_EDIT   yLOG_note    ("assign a opengl color");
      switch (x_code) {
      case  'm' :  glColor4f   (0.5, 0.5, 0.5, 1.0);  break;
      case  'c' :  glColor4f   (0.5, 0.5, 0.5, 1.0);  break;
      case  's' :  glColor4f   (0.0, 1.0, 0.0, 1.0);  break;
      case  't' :  glColor4f   (0.0, 1.0, 1.0, 1.0);  break;
      case  'r' :  glColor4f   (1.0, 0.0, 1.0, 1.0);  break;
      case  'i' :  glColor4f   (0.0, 0.0, 1.0, 1.0);  break;
      case  'w' :  glColor4f   (1.0, 0.0, 0.0, 1.0);  break;
      case  'e' :  glColor4f   (1.0, 0.0, 0.0, 1.0);  break;
      default   :  glColor4f   (0.5, 0.5, 0.5, 1.0);  break;
      }
   }

   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_char    ("x_edit"    , x_edit);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return x_edit;
}

char
SOURCE__opengl          (tEDIT *a_cur, int a_left, int a_wide, int a_bott, int a_tall, char a_edit)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD];
   char        c1          =  ' ';
   char        c2          =  ' ';
   float       x_beg       =    0;
   float       x_end       =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(fast path for command)----------*/
   if (a_cur->type == EDIT_CMDS && a_edit != 'y') {
      DEBUG_EDIT   yLOG_note    ("fast path to display mode message");
      glPushMatrix    (); {
         glTranslatef ( 0.0f, 1.0f, 0.0f);
         glColor4f   (0.0, 0.0, 0.0, 1.0);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, MODE_message ());
      } glPopMatrix   ();
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(background)---------------------*/
   glPushMatrix    (); {
      glBegin         (GL_POLYGON); {
         glVertex3f  (0     , a_tall, -50.0f);
         glVertex3f  (a_wide, a_tall, -50.0f);
         glVertex3f  (a_wide, 0     , -50.0f);
         glVertex3f  (0     , 0     , -50.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(selection)----------------------*/
   if (a_edit == 'y' && s_live == SELC_YES && a_cur->npos > 0) {
      x_beg = s_bsel;
      x_end = s_esel;
      if (x_beg < a_cur->bpos)  x_beg = a_cur->bpos;
      if (x_end > a_cur->epos)  x_end = a_cur->epos;
      x_beg += 5;
      x_end += 6;
      x_beg *= myVIKEYS.font_scale;
      x_end *= myVIKEYS.font_scale;
      glColor4f   (0.8, 0.8, 0.8, 1.0);
      glPushMatrix    (); {
         glBegin         (GL_POLYGON); {
            glVertex3f  (x_beg , a_tall, -40.0f);
            glVertex3f  (x_end , a_tall, -40.0f);
            glVertex3f  (x_end , 0     , -40.0f);
            glVertex3f  (x_beg , 0     , -40.0f);
         } glEnd   ();
      } glPopMatrix   ();
   }
   /*---(current)------------------------*/
   if (a_edit == 'y' && a_cur->npos > 0) {
      x_beg = (a_cur->cpos + 5) * myVIKEYS.font_scale;
      x_end = (a_cur->cpos + 6) * myVIKEYS.font_scale;
      glColor4f   (0.8, 0.8, 0.0, 1.0);
      glPushMatrix    (); {
         glBegin         (GL_POLYGON); {
            glVertex3f  (x_beg , a_tall, -30.0f);
            glVertex3f  (x_end , a_tall, -30.0f);
            glVertex3f  (x_end , 0     , -30.0f);
            glVertex3f  (x_beg , 0     , -30.0f);
         } glEnd   ();
      } glPopMatrix   ();
   }
   /*---(markers)------------------------*/
   if (a_cur->npos != 0) {
      if (a_cur->bpos >  0)   c1 = '<';
      else                    c1 = ']';
   }
   if (a_cur->npos != 0) {
      if (a_cur->epos != a_cur->npos - 1) c2 = '>';
      else                                c2 = '[';
   }
   /*---(text)---------------------------*/
   /*> if (a_cur->npos == 0) sprintf (t, "   %c", G_CHAR_NULL);                                                                              <* 
    *> else                  sprintf (t, "%4d%c%-*.*s%c"  , a_cur->npos, c1, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos, c2);   <*/
   if (a_cur->npos == 0) sprintf (t, "   %c", G_CHAR_NULL);
   else                  sprintf (t, "%4d%c%-*.*s%c"  , a_cur->cpos, c1, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos, c2);
   glPushMatrix    (); {
      glTranslatef ( 0.0f, 1.0f, 0.0f);
      glColor4f   (0.0, 0.0, 0.0, 1.0);
      yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, t);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE__curses          (tEDIT *a_cur, int a_left, int a_bott, char a_edit)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_beg       =    0;
   char        x_end       =    0;
   char        x_len       =    0;
   char        c           =  ' ';
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(fast path for command)----------*/
   if (a_cur->type == EDIT_CMDS && a_edit != 'y') {
      mvprintw (a_bott, a_left, "%-*.*s", a_cur->wide, a_cur->wide, MODE_message ());
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(base content)-------------------*/
   if (a_cur->npos == 0) mvprintw (a_bott, a_left, "   %c %-*.*s ", G_CHAR_NULL, a_cur->apos, a_cur->apos, " ");
   else                  mvprintw (a_bott, a_left, "%4d %-*.*s ", a_cur->npos, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos);
   /*---(selection)----------------------*/
   if (a_edit == 'y' && s_live == SELC_YES && a_cur->npos > 0) {
      x_beg = s_bsel;
      x_end = s_esel;
      if (x_beg < a_cur->bpos)  x_beg = a_cur->bpos;
      if (x_end > a_cur->epos)  x_end = a_cur->epos;
      x_len = x_end - x_beg + 1;
      attrset  (0);
      yCOLOR_curs ("select" );
      mvprintw (a_bott, a_left + 5 + x_beg - a_cur->bpos, "%-*.*s", x_len, x_len, a_cur->contents + x_beg);
   }
   /*---(current)------------------------*/
   if (a_edit == 'y' && a_cur->npos > 0) {
      attrset  (0);
      yCOLOR_curs ("map"    );
      mvprintw (a_bott, a_left + 5 + a_cur->cpos - a_cur->bpos, "%c", a_cur->contents [a_cur->cpos]);
   }
   /*---(markers)------------------------*/
   attrset  (0);
   yCOLOR_curs ("h_used");
   if (a_cur->npos != 0) {
      if (a_cur->bpos >  0)   c = '<';
      else                    c = ']';
      mvprintw (a_bott, a_left + 4, "%c", c);
   }
   if (a_cur->npos != 0) {
      if (a_cur->epos != a_cur->npos - 1) c = '>';
      else                                c = '[';
      mvprintw (a_bott, a_left + a_cur->wide - 1, "%c", c);
   }
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_display             (tEDIT *a_cur, char a_mode)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_left      =    0;
   int         x_wide      =    0;
   int         x_bott      =    0;
   int         x_tall      =    0;
   char        x_edit      =  ' ';
   char        x_on        =  '-';
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (a_mode, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   if (myVIKEYS.env == YVIKEYS_CURSES)  a_cur->wide = x_wide;
   else                                 a_cur->wide = x_wide / myVIKEYS.font_scale;
   a_cur->apos = a_cur->wide - 6;
   DEBUG_EDIT   yLOG_char    ("on"        , x_on);
   DEBUG_EDIT   yLOG_value   ("wide"      , a_cur->wide);
   DEBUG_EDIT   yLOG_value   ("apos"      , a_cur->apos);
   DEBUG_EDIT   yLOG_info    ("contents"  , a_cur->contents);
   DEBUG_EDIT   yLOG_value   ("npos"      , a_cur->npos);
   DEBUG_EDIT   yLOG_char    ("type"      , a_cur->type);
   /*---(defense)------------------------*/
   if (x_on != 'y') {
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(display)------------------------*/
   x_edit = SOURCE__color (a_mode);
   DEBUG_EDIT   yLOG_char    ("x_edit"    , x_edit);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      SOURCE__curses (a_cur, x_left, x_bott, x_edit);
   } else {
      SOURCE__opengl (a_cur, x_left, x_wide, x_bott, x_tall, x_edit);
   }
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_formula             (void)
{
   tEDIT      *x_cur       = NULL;
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   x_cur  = &s_src;
   SOURCE_display (x_cur, YVIKEYS_FORMULA);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_command             (void)
{
   tEDIT      *x_cur       = NULL;
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   x_cur  = &s_cmd;
   SOURCE_display (x_cur, YVIKEYS_COMMAND);
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_float               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_mode      =  '-';
   char        x_on        =  '-';
   tEDIT      *x_cur       = NULL;
   /*---(header)----------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_char    ("mode_curr" , MODE_curr ());
   DEBUG_EDIT   yLOG_char    ("mode_prev" , MODE_prev ());
   /*---(check modes)-----------------*/
   if      (strchr (MODES_ONELINE, MODE_curr ()) != NULL )      x_mode = MODE_curr ();
   else if (strchr (MODES_ONELINE, MODE_prev ()) != NULL )      x_mode = MODE_prev ();
   DEBUG_EDIT   yLOG_char    ("x_mode"    , x_mode);
   if (x_mode == '-')  {
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check modes)-----------------*/
   --rce;  switch (x_mode) {
   case MODE_SOURCE  :
      DEBUG_EDIT   yLOG_note    ("mode source");
      x_on   = yVIKEYS_view_size     (YVIKEYS_FORMULA, NULL, NULL, NULL, NULL, NULL);
      x_cur  = &s_src;
      break;
   case MODE_COMMAND :
   case MODE_SEARCH  :
      DEBUG_EDIT   yLOG_note    ("mode command/search");
      x_on   = yVIKEYS_view_size     (YVIKEYS_COMMAND, NULL, NULL, NULL, NULL, NULL);
      x_cur  = &s_cmd;
      break;
   default :
      DEBUG_EDIT   yLOG_note    ("default");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_EDIT   yLOG_char    ("x_on"      , x_on);
   if (x_on != 'y')  SOURCE_display (x_cur, YVIKEYS_FLOAT);
   /*---(complete)--------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.C53.244.61]*/ /*-[02.0000.204.!]-*/ /*-[--.---.---.--]-*/
SOURCE_status_select    (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_len       = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(write line)------------------*/
   x_len = s_esel - s_bsel + 1;
   sprintf (a_list , "selection %c %3d %3d %3d[%-*.*s]",
         s_live, s_bsel, s_esel, x_len,
         x_len, x_len, s_cur->contents + s_bsel);
   /*---(complete)--------------------*/
   return 0;
}

char
SOURCE_status_words    (char *a_list)
{
   sprintf (a_list, "word %s", s_cur->words);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.C53.244.61]*/ /*-[02.0000.204.!]-*/ /*-[--.---.---.--]-*/
SRC_REG_status          (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
   int         x_len       = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(buffer number)------------------*/
   x_reg  = SRC_REG_valid   (s_wtreg);
   /*---(write line)------------------*/
   sprintf (a_list , "treg %c %2d %2d  %3d [%-40.40s]  %-7.7s %3d %3d  %c",
         s_wtreg, s_ntreg, x_reg,
         s_tregs [x_reg].len   ,
         s_tregs [x_reg].data  ,
         s_tregs [x_reg].label ,
         s_tregs [x_reg].bpos  ,
         s_tregs [x_reg].epos  ,
         s_tregs [x_reg].source);
   /*---(complete)--------------------*/
   return 0;
}








/*====================------------------------------------====================*/
/*===----                      key movements                           ----===*/
/*====================------------------------------------====================*/
static void  o___MOVEMENTS_______o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SOURCE__simple          (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT  yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != G_KEY_SPACE) {
      DEBUG_EDIT   yLOG_note    ("a_major was not empty");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_EDIT  yLOG_info    ("g_hsimple" , g_hsimple);
   --rce;  if (strchr (g_hsimple, a_minor) == 0) {
      DEBUG_EDIT   yLOG_note    ("a_minor was not a valid option");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SOURCE__prep ();
   /*---(horizontal moves)---------------*/
   switch (a_minor) {
   case '0' : s_cur->cpos  = 0;                    break;
   case 'H' : s_cur->cpos -= 5;                    break;
   case 'h' : --s_cur->cpos;                       break;
   case 'l' : ++s_cur->cpos;                       break;
   case 'L' : s_cur->cpos += 5;                    break;
   case '$' : s_cur->cpos  = s_cur->npos - 1;           break;
   }
   /*---(wrapup)-------------------------*/
   SOURCE__done  ();
   /*---(complete)--------------------*/
   DEBUG_EDIT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SOURCE__goto       (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT  yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'g') {
      DEBUG_EDIT   yLOG_note    ("a_major was not g (goto)");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_EDIT  yLOG_info    ("g_hgoto"  , g_hgoto);
   --rce;  if (strchr (g_hgoto, a_minor) == 0) {
      DEBUG_EDIT   yLOG_note    ("a_minor was not a valid option");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SOURCE__prep ();
   /*---(goto moves)---------------------*/
   x_qtr = s_cur->apos / 4;
   switch (a_minor) {
   case 'S' : s_cur->cpos  = s_cur->bpos - x_qtr * 4;  break;
   case 'H' : s_cur->cpos  = s_cur->bpos - x_qtr * 2;  break;
   case 's' : s_cur->cpos  = s_cur->bpos;              break;
   case 'h' : s_cur->cpos  = s_cur->bpos + x_qtr * 1;  break;
   case 'c' : s_cur->cpos  = s_cur->bpos + x_qtr * 2;  break;
   case 'l' : s_cur->cpos  = s_cur->bpos + x_qtr * 3;  break;
   case 'e' : s_cur->cpos  = s_cur->epos;              break;
   case 'L' : s_cur->cpos  = s_cur->bpos + x_qtr * 6;  break;
   case 'E' : s_cur->cpos  = s_cur->bpos + x_qtr * 8;  break;
   }
   /*---(wrapup)-------------------------*/
   SOURCE__done  ();
   /*---(complete)--------------------*/
   DEBUG_EDIT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SOURCE__scroll     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT  yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'z') {
      DEBUG_EDIT   yLOG_note    ("a_major was not z (scroll)");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_EDIT  yLOG_info    ("g_hscroll" , g_hscroll);
   --rce;  if (strchr (g_hscroll, a_minor) == 0) {
      DEBUG_EDIT   yLOG_note    ("a_minor was not a valid option");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SOURCE__prep ();
   /*---(goto moves)---------------------*/
   x_qtr = s_cur->apos / 4;
   switch (a_minor) {
   case 's' : s_cur->bpos += s_cur->cpos - (s_cur->bpos + x_qtr * 0);  break;
   case 'h' : s_cur->bpos += s_cur->cpos - (s_cur->bpos + x_qtr * 1);  break;
   case 'c' : s_cur->bpos += s_cur->cpos - (s_cur->bpos + x_qtr * 2);  break;
   case 'l' : s_cur->bpos += s_cur->cpos - (s_cur->bpos + x_qtr * 3);  break;
   case 'e' : s_cur->bpos += s_cur->cpos - (s_cur->epos);              break;
   }
   /*---(wrapup)-------------------------*/
   SOURCE__done  ();
   /*---(complete)--------------------*/
   DEBUG_EDIT  yLOG_exit    (__FUNCTION__);
   return 0;
}

int
SOURCE__nextword        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(word forward)-------------------*/
   for (i = s_cur->cpos + 1; i < s_cur->npos; ++i) {
      if (strchr ("<B", s_cur->words [i]) == NULL)  continue;
      return i - s_cur->cpos;
   }
   /*---(complete)-----------------------*/
   return s_cur->npos - s_cur->cpos;
}

int
SOURCE__endword         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(word forward)-------------------*/
   for (i = s_cur->cpos + 1; i < s_cur->npos; ++i) {
      if (strchr (">B", s_cur->words [i]) == NULL)  continue;
      return i - s_cur->cpos + 1;
   }
   /*---(complete)-----------------------*/
   return s_cur->npos - s_cur->cpos;
}

int
SOURCE__prevword        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_len       =    0;
   /*---(word backward)------------------*/
   for (i = s_cur->cpos - 1; i >= 0 ; --i) {
      if (strchr ("<B", s_cur->words [i]) == NULL)  continue;
      x_len       = s_cur->cpos - i;
      s_cur->cpos = i;
      return x_len;
   }
   /*---(all)----------------------------*/
   x_len       = s_cur->cpos - 1;
   s_cur->cpos = 0;
   return x_len;
}

char
SOURCE__word            (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char       *x_word      = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-";
   int         x_not       =    0;
   int         x_yes       =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT  yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_EDIT  yLOG_info    ("g_hword"   , g_hword);
   --rce;  if (strchr (g_hword, a_minor) == 0) {
      DEBUG_EDIT   yLOG_note    ("a_minor was not a valid option");
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SOURCE__prep ();
   /*---(words)--------------------------*/
   if (strchr ("wW", a_minor) != 0) {
      for (i = s_cur->cpos + 1; i < s_cur->npos; ++i) {
         if (strchr ("<B", s_cur->words [i]) == NULL)  continue;
         s_cur->cpos = i;
         break;
      }
   }
   /*---(ends)---------------------------*/
   if (strchr ("eE", a_minor) != 0) {
      for (i = s_cur->cpos + 1; i < s_cur->npos; ++i) {
         if (strchr (">B", s_cur->words [i]) == NULL)  continue;
         s_cur->cpos = i;
         break;
      }
   }
   /*---(beginnings)---------------------*/
   if (strchr ("bB", a_minor) != 0) {
      for (i = s_cur->cpos - 1; i >= 0; --i) {
         if (strchr ("<B", s_cur->words [i]) == NULL)  continue;
         s_cur->cpos = i;
         break;
      }
   }
   /*---(wrapup)-------------------------*/
   SOURCE__done  ();
   /*---(complete)--------------------*/
   DEBUG_EDIT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE__charfindrev     (uchar a_ch)
{
   int         i           =    0;
   for (i = s_cur->cpos - 1; i >= 0; --i) {
      if (s_cur->contents [i] != a_ch)  continue;
      s_cur->cpos = i;
      SOURCE__done ();
      return 0;
   }
   return -1;
}

char
SOURCE__charfind        (uchar a_ch)
{
   int         i           =    0;
   for (i = s_cur->cpos + 1; i < s_cur->npos; ++i) {
      if (s_cur->contents [i] != a_ch)  continue;
      s_cur->cpos = i;
      SOURCE__done ();
      return 0;
   }
   return -1;
}

char         /*-> complex delete action --------------[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
SOURCE_delete          (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        i           =    0;
   char        x_len       =    0;
   char        x_pos       =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_EDIT   yLOG_char    ("s_live"    , s_live);
   --rce;  if (s_live != SELC_NOT) {
      DEBUG_EDIT   yLOG_note    ("function only handles non-selected text");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_EDIT   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   --rce;  if (a_minor == 0 || strchr ("hlwbe0$", a_minor) == NULL) {
      DEBUG_EDIT   yLOG_note    ("source only allows right and left");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_EDIT   yLOG_value   ("npos"      , s_cur->npos);
   --rce;  if (s_cur->npos <= 0) {
      DEBUG_EDIT   yLOG_note    ("nothing text to delete");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   --rce;  if (a_minor == 'h' && s_cur->cpos <= 0) {
      DEBUG_EDIT   yLOG_note    ("nothing left to delete");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_minor == 'l' && s_cur->cpos >= s_cur->npos) {
      DEBUG_EDIT   yLOG_note    ("nothing right to delete");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   switch (a_minor) {
   case 'h' : x_len = 1;                             break;
   case 'l' : x_len = 1;                             break;
   case 'w' : x_len = SOURCE__nextword ();           break;
   case 'b' : x_len = SOURCE__prevword ();           break;
   case 'e' : x_len = SOURCE__endword  ();           break;
   case '0' : x_len = s_cur->cpos; s_cur->cpos = 0;  break;
   case '$' : x_len = s_cur->npos - s_cur->cpos;     break;
   }
   if (strchr ("hb0$", a_minor) != NULL)  x_pos = s_cur->cpos;
   else                                   x_pos = s_cur->cpos + x_len;
   /*---(end)----------------------------*/
   SRC_UNDO__beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      if (a_major == 'd') {
         if (a_minor == 'h')  --s_cur->cpos;
         SRC_UNDO__add (a_major, 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
         ONE__delete ();
      } else {
         if (a_minor == 'h')  --s_cur->cpos;
         SRC_UNDO__add (a_major, 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_CHAR_SPACE);
         ONE__replace (G_CHAR_SPACE);
         if (a_minor != 'h')  ++s_cur->cpos;
      }
   }
   SRC_UNDO__end (__FUNCTION__);
   if (a_major == 'x' && a_minor != 'h')  s_cur->cpos = x_pos;
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      text reg actions                        ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char         /*-> process keys for register actions --[ ------ [gz.320.011.02]*/ /*-[01.0000.113.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__clear        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =   0;
   /*---(begin)--------------------------*/
   SRC_UNDO__beg (__FUNCTION__);
   for (i = s_bsel; i <= s_esel; ++i) {
      s_cur->cpos = i;
      SRC_UNDO__add ('x', 'l', i, s_cur->contents [i], G_CHAR_SPACE);
      ONE__replace (G_CHAR_SPACE);
   }
   SRC_UNDO__end (__FUNCTION__);
   /*---(reposition)---------------------*/
   /*> s_cur->cpos = s_esel + 1;                                                      <*/
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register action ---[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__delete        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   int         x_diff      =   0;
   int         i           =   0;
   /*---(short-path)---------------------*/
   SRC_UNDO__beg (__FUNCTION__);
   /*---(short-path)---------------------*/
   if (s_live == SELC_NOT && a_minor == 'D') {
      if (s_cur->cpos <= 0)  return -1;
      --s_cur->cpos;
      SRC_UNDO__add (a_major, tolower (a_minor), i, s_cur->contents [i], G_KEY_NULL);
      ONE__delete ();
      return 0;
   }
   else {
      /*---(set size)-----------------------*/
      x_diff  = s_esel - s_bsel + 1;
      /*---(delete)-------------------------*/
      s_cur->cpos = s_bsel;
      for (i = 0; i < x_diff; ++i) {
         SRC_UNDO__add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
         ONE__delete ();
      }
   }
   SRC_UNDO__end (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [ge.640.051.04]*/ /*-[00.0000.213.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__copy          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_label     [10]        = "";
   char        rce         = -10;
   char        t           [LEN_RECD];
   /*---(identify register)--------------*/
   x_index = SRC_REG_valid  (tolower (s_ctreg));
   if (x_index < 0)  return -1;
   /*---(set size)-----------------------*/
   x_start = s_cur->contents + s_bsel;
   x_len   = s_esel - s_bsel + 1;
   /*---(copy)---------------------------*/
   if (s_ctreg == tolower (s_ctreg)) {
      strlcpy (s_tregs [x_index].data, x_start, x_len + 1);
      s_tregs [x_index].len    = x_len;
      s_tregs [x_index].bpos   = s_bsel;
      s_tregs [x_index].epos   = s_esel;
      strlcpy (s_tregs [x_index].label, s_cur->label, LEN_LABEL);
      s_tregs [x_index].source = TREG_USER;
   }
   /*---(append)-------------------------*/
   else {
      strlcpy (t, x_start, x_len + 1);
      strlcat (s_tregs [x_index].data, t, LEN_RECD);
      s_tregs [x_index].len    = strllen (s_tregs [x_index].data, LEN_RECD);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> replace only selection long --------[ ------ [gz.740.061.21]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__replace        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_len       =   0;
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = SRC_REG_valid  (s_ctreg);
   if (x_index < 0)  return -1;
   /*> x_len = s_esel - s_bsel + 1;                                                   <*/
   /*> if (x_len > s_tregs [x_index].len)  x_len = s_tregs [x_index].len;             <*/
   x_len = s_tregs [x_index].len;
   s_cur->cpos = s_bsel;
   /*---(set the start)------------------*/
   SRC_UNDO__beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      SRC_UNDO__add   (G_KEY_SPACE, 'r', s_cur->cpos, s_cur->contents [s_cur->cpos], s_tregs [x_index].data [i]);
      ONE__replace (s_tregs [x_index].data [i]);
      ++s_cur->cpos;
   }
   SRC_UNDO__end (__FUNCTION__);
   --s_cur->cpos;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> insert/append register contents ----[ ------ [gz.640.151.11]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
SRC_REG__paste          (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = SRC_REG_valid  (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(prepare)------------------------*/
   if (a_dir == 'a' && s_cur->npos > 0) {
      ++s_cur->cpos;
   }
   /*---(set the start)------------------*/
   SRC_UNDO__beg (__FUNCTION__);
   for (i = 0; i < s_tregs [x_index].len; ++i) {
      SRC_UNDO__add (G_KEY_SPACE, 'i', s_cur->cpos, G_CHAR_NULL, s_tregs [x_index].data [i]);
      ONE__insert (s_tregs [x_index].data [i]);
      ++s_cur->cpos;
   }
   SRC_UNDO__end (__FUNCTION__);
   --s_cur->cpos;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       main handlers                          ----===*/
/*====================------------------------------------====================*/
static void  o___MODES___________o () { return; }

char         /*-> process keys for cell edit mode ----[ leaf   [ge.#R6.23#.HG]*/ /*-[05.0000.102.#]-*/ /*-[--.---.---.--]-*/
SOURCE_mode             (int a_major, int a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate the vi-normal mode, but only horizontally.
    *   all formulas and contents should be relatively short or they
    *   should be broken up (formulas) or in a database (text).  so,
    *   forget wordwrap.
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   int         i           =    0;
   int         n           =    0;
   static char x_char      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   DEBUG_USER   yLOG_info    ("valid"     , MODES_ONELINE);
   --rce;  if (strchr (MODES_ONELINE, MODE_curr ()) == NULL ) {
      DEBUG_USER   yLOG_note    ("not an acceptable mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(universal)----------------------*/
   switch (a_minor) {
   case G_KEY_SPACE  :
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      rc = 0;
      break;
   case G_KEY_ESCAPE :
      if (s_live == SELC_YES) {
         DEBUG_USER   yLOG_note    ("escape, means get out of selection");
         SRC_REG__reset ();
      } else {
         DEBUG_USER   yLOG_note    ("escape, means forget and return to previous mode");
         SOURCE__reset  ();
         SOURCE__done   ();
         MODE_exit      ();
      }
      rc = 0;
      break;
   case G_KEY_RETURN :
      DEBUG_USER   yLOG_note    ("enter, means save and return to previous mode");
      rc = SOURCE__accept ();
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      SOURCE__done   ();
      MODE_exit      ();
      if (rc > 0)   rc = 0;
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single letter)------------------*/
   if (a_major == G_KEY_SPACE) {
      /*---(repeats)---------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         MODE_enter  (UMOD_REPEAT);
         rc = a_minor;
      }
      /*---(select related)--------------*/
      if (strchr ("yYpP", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         s_ctreg = '"';
         MODE_enter (SMOD_SRC_REG);
         rc = SRC_REG_smode (G_KEY_SPACE, a_minor);
         return rc;
      }
      if (s_live == SELC_YES && strchr ("xXdD", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         s_ctreg = '"';
         MODE_enter (SMOD_SRC_REG);
         rc = SRC_REG_smode (G_KEY_SPACE, a_minor);
         return rc;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr (g_multisrc, a_minor) != 0) {
         return a_minor;
      }
      /*---(mode changes)----------------*/
      switch (a_minor) {
      case  'v' :
         if (s_live == SELC_YES) {
            DEBUG_USER   yLOG_note    ("flip selection ends");
            if      (s_root == s_bsel) { s_root = s_esel; s_cur->cpos = s_bsel; }
            else if (s_root == s_esel) { s_root = s_bsel; s_cur->cpos = s_esel; }
         } else {
            DEBUG_USER   yLOG_note    ("start selection of text");
            s_live = SELC_YES;
            s_root = s_cur->cpos;
         }
         rc     = 0;
         break;
      case  '#' :
         DEBUG_USER   yLOG_note    ("current char find");
         x_char = s_cur->contents [s_cur->cpos];
         rc = SOURCE__charfind (x_char);
         break;
      case  'n' :
         DEBUG_USER   yLOG_note    ("char find next");
         rc = SOURCE__charfind (x_char);
         break;
      case  'N' :
         DEBUG_USER   yLOG_note    ("char find reverse");
         rc = SOURCE__charfindrev (x_char);
         break;
      case  '"' :
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         MODE_enter (SMOD_SRC_REG);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         rc = a_minor;
         break;
      case  'r' :
         DEBUG_USER   yLOG_note    ("enter replace mode");
         MODE_enter (UMOD_SRC_REPL);
         if (REPEAT_count () == 0) {
            SRC_REPL_umode ('m', a_minor);
            rc = 0;
         } else {
            rc = a_minor;
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         break;
      case  'R' :
         DEBUG_USER   yLOG_note    ("enter replace mode");
         MODE_enter (UMOD_SRC_REPL);
         SRC_REPL_umode ('m', a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         rc = 0;
         break;
      case  'i' : case  'a' : case  'I' : case  'A' :
         DEBUG_USER   yLOG_note    ("enter input mode");
         if (a_minor == 'A')  s_cur->cpos = s_cur->npos - 1;
         if (a_minor == 'I')  s_cur->cpos = 0;
         MODE_enter (UMOD_SRC_INPT);
         SRC_INPT_umode ('m', tolower (a_minor));
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         rc = tolower (a_minor);
         break;
      case  'D' :
         /*> rc = SOURCE_delete     ('d', '$');                                       <*/
         rc = SOURCE_delete     ('d', 'l');
         SOURCE__done ();
         break;
      case  'X' :
         /*> rc = SOURCE_delete     ('x', '$');                                       <*/
         rc = SOURCE_delete     ('x', 'l');
         SOURCE__done ();
         break;
      }
      /*---(check for backspace)------------*/
      if (a_minor == G_KEY_BS) {
         DEBUG_USER   yLOG_note    ("handle a backspace");
         rc = SOURCE_delete     ('d', 'h');
         SOURCE__done ();
         /*> if (s_cur->cpos > 0) {                                                                      <* 
          *>    SRC_UNDO__beg (__FUNCTION__);                                                               <* 
          *>    SRC_UNDO__add ('d', 'h', s_cur->cpos - 1, s_cur->contents [s_cur->cpos - 1], G_KEY_NULL);   <* 
          *>    rc = ONE__backspace ();                                                                  <* 
          *>    SRC_UNDO__end (__FUNCTION__);                                                               <* 
          *> }                                                                                           <*/
      }
      if (a_minor == G_KEY_DEL) {
         DEBUG_USER   yLOG_note    ("handle a delete");
         rc = SOURCE_delete     ('d', 'l');
         SOURCE__done ();
         /*> if (s_cur->cpos < s_cur->npos) {                                                    <* 
          *>    SRC_UNDO__beg (__FUNCTION__);                                                       <* 
          *>    SRC_UNDO__add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);   <* 
          *>    rc = ONE__delete ();                                                             <* 
          *>    SRC_UNDO__end (__FUNCTION__);                                                       <* 
          *> }                                                                                   <*/
      }
      /*---(basic movement)--------------*/
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = SOURCE__simple  (a_major, a_minor);
      }
      /*---(word movement)---------------*/
      if (strchr (g_hword, a_minor) != 0) {
         rc = SOURCE__word    (a_major, a_minor);
      }
      /*---(other stuff)-----------------*/
      switch (a_minor) {
      case '|' :
         s_cur->cpos = REPEAT_use ();
         rc     = 0;
         SOURCE__done ();
         break;
      case 'u' :
         SRC_REG__reset ();
         rc = SRC_UNDO__undo ();
         SOURCE__done ();
         break;
      case 'U' :
         SRC_REG__reset ();
         rc = SRC_UNDO__redo ();
         SOURCE__done ();
         break;
      }
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(delete-family)------------------*/
   /*---(multi-key)----------------------*/
   else {
      switch (a_major) {
      case 'd' : case 'x' :
         rc = SOURCE_delete     (a_major, a_minor);
         SOURCE__done ();
         break;
      case 'g' :  rc = SOURCE__goto      (a_major, a_minor);  break;
      case 'z' :  rc = SOURCE__scroll    (a_major, a_minor);  break;
      case 'f' :  x_char = a_minor;
                  rc = SOURCE__charfind  (x_char);
                  break;
      case 'F' :  x_char = a_minor;
                  rc = SOURCE__charfindrev (x_char);
                  break;
      case 'c' :  n = REPEAT_use ();
                  for (i = 0; i <= n; ++i) {
                     SOURCE_delete  ('d', a_minor);
                     SOURCE__done   ();
                  }
                  MODE_enter (UMOD_SRC_INPT);
                  SRC_INPT_umode ('m', tolower (a_minor));
                  DEBUG_USER   yLOG_exit    (__FUNCTION__);
                  rc = tolower (a_minor);
                  return rc;
                  break;
      default  :  rc = -1;                              break;
      }
   }
   /*---(complete)-----------------------*/
   if (rc < 0) {
      DEBUG_USER   yLOG_note    ("return code is negative");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
SRC_REG_infowin    (char *a_entry, int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         a           =    0;
   int         n           =    0;
   char        t           [LEN_RECD]  = "";
   /*---(status)-------------------------*/
   strlcpy (a_entry, " -  ----                                                                      ", LEN_RECD);
   if (a_index <   0)  return 0;
   if (a_index >= 26)  return 0;
   a = 'a' + a_index;
   n = SRC_REG_valid  (a);
   if (s_tregs [n].len > 0) {
      strlcpy    (t, s_tregs [n].data, LEN_RECD);
      strlencode (t, ySTR_MAX, LEN_RECD);
      sprintf (a_entry, " %c  %4d  %s "        , a, s_tregs [n].len, t);
   } else {
      sprintf (a_entry, " %c  ----                                                                      ", a);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
SRC_REG_writer         (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_beg       =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(prepare)----------------s-------*/
   yVIKEYS_unit_reset ();
   if (a_abbr == 0) {
      x_beg = 1;
      x_end = s_ntreg - 1;
   } else {
      x_beg = x_end = SRC_REG_valid (a_abbr);
      if (x_beg <= 0)  return rce;
   }
   /*---(find marked entries)------------*/
   for (i = x_beg; i <= x_end; ++i) {
      if (s_tregs [i].len <= 0)  continue;
      yVIKEYS_file_write (SMOD_SRC_REG, &(s_tregnames [i]), s_tregs [i].data, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
SRC_REG_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_SRCH   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_SRCH   yLOG_char    ("version"   , n);
   --rce;  if (n != 'A') {
      DEBUG_SRCH   yLOG_note    ("illegal version");
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_SRCH   yLOG_value   ("mark"      , a[0]);
   --rce;  if (strchr (s_tregnames, a[0]) == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SRCH   yLOG_char    ("mark"      , a[0]);
   x_index = SRC_REG_valid  (a[0]);
   --rce;  if (x_index <= 0)  return rce;
   DEBUG_SRCH   yLOG_value   ("index"     , x_index);
   /*---(search)-------------------------*/
   DEBUG_SRCH   yLOG_point   ("contents"  , b);
   --rce;  if (b == NULL) {
      DEBUG_SRCH   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_SRCH   yLOG_note    ("saving values");
   strldchg (b, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   strlcpy (s_tregs [x_index].data, b, LEN_RECD);
   s_tregs [x_index].len    = strllen (s_tregs [x_index].data, LEN_RECD);
   s_tregs [x_index].bpos   = 0;
   s_tregs [x_index].epos   = s_tregs [x_index].len - 1;
   strlcpy (s_tregs [x_index].label, "-", LEN_LABEL);
   s_tregs [x_index].source = TREG_FILE;
   /*---(complete)-----------------------*/
   DEBUG_SRCH  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for register actions --[ leaf   [ge.QG5.287.FB]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
SRC_REG_smode           (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =  -1;
   int         x_buf       =  -1;
   int         x_index     =   0;
   int         i           =   0;
   char       *x_start     = NULL;
   char        x_label     [10]        = "";
   int         x_diff      =   0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   myVIKEYS.info_win = '-';
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_SRC_REG )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(escape)-------------------------*/
   if (a_minor == G_KEY_ESCAPE)  {
      DEBUG_USER   yLOG_note    ("escape and return to previous mode");
      SRC_REG__reset ();
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(select register name)-----------*/
   --rce;  if (a_major == '"') {
      if (strchr (s_tregnames, tolower (a_minor)) != 0) {
         DEBUG_USER   yLOG_note    ("select a text register");
         s_ctreg = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      else if (a_minor == '?') {
         DEBUG_USER   yLOG_note    ("show text register inventory");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         myVIKEYS.info_win = 'y';
         return  a_major;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(select register action)---------*/
   --rce;  if (a_major == G_KEY_SPACE) {
      switch (a_minor) {
      case  '!' :
         s_wtreg = s_ctreg;
         yVIKEYS_cmds_direct (":status treg");
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      case  '#' :
         DEBUG_USER   yLOG_note    ("wipe text register");
         x_index = SRC_REG_valid  (s_ctreg);
         if (x_index < 0)  return -1;
         strlcpy (s_tregs [x_index].label, "-", 10);
         s_tregs [x_index].bpos   =  0;
         s_tregs [x_index].epos   =  0;
         s_tregs [x_index].len    =  0;
         strlcpy (s_tregs [x_index].data , "", LEN_RECD);
         s_tregs [x_index].source = TREG_NONE;
         MODE_exit ();
         break;
      case  'y' :
         DEBUG_USER   yLOG_note    ("yank selection text to register");
         SRC_REG__copy   ();
         MODE_exit   ();
         break;
      case  'x' : case  'X' :
         DEBUG_USER   yLOG_note    ("clear selection text");
         SRC_REG__copy   ();
         SRC_REG__clear  (a_major, a_minor);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'd' : case  'D' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         SRC_REG__copy   ();
         SRC_REG__delete (a_major, a_minor);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'r' :
         DEBUG_USER   yLOG_note    ("replace text from register");
         SRC_REG__replace ();
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  's' :
         DEBUG_USER   yLOG_note    ("substitute selection text");
         SRC_REG__delete   (a_major, 'd');
         SRC_UNDO__chain_next ();
         SRC_REG__paste    ('i');
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'p' : case  'a' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         SRC_REG__paste   ('a');
         /*> ++s_cur->cpos;                                                           <*/
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'P' : case  'i' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         SRC_REG__paste   ('i');
         MODE_exit   ();
         SOURCE__done    ();
         break;
         /*> case  'g' :                                                                  <* 
          *>    DEBUG_USER   yLOG_note    ("go to beginning selection position");         <* 
          *>    x_index = REG__reg2index (s_ctreg);                                        <* 
          *>    s_cur->cpos = s_tregs [x_index].bpos;                                       <* 
          *>    SOURCE__done ();                                                             <* 
          *>    MODE_exit ();                                                             <* 
          *>    break;                                                                    <*/
         /*> case  'G' :                                                                  <* 
          *>    DEBUG_USER   yLOG_note    ("go to ending selection position");            <* 
          *>    x_index = REG__reg2index (s_ctreg);                                        <* 
          *>    s_cur->cpos = s_tregs [x_index].epos;                                       <* 
          *>    SOURCE__done ();                                                             <* 
          *>    MODE_exit ();                                                             <* 
          *>    break;                                                                    <*/
      }
   }
   SRC_REG__reset ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> replace sub-mode -------------------[ ------ [ge.RG6.25#.E5]*/ /*-[02.0000.112.E]-*/ /*-[--.---.---.--]-*/
SRC_REPL_umode    (int a_major, int a_minor)
{  /*---(design notes)-------------------*/
   /*
    *   very limited sub-mode of SOURCE mode.  allows a user to "type over"
    *   existing text including past the end of the line.  there are no
    *   special keys except ESCAPE to return to SOURCE mode.
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   static char x_saved     = '\0';
   static char x_prev      =  '-';
   static char x_mode      =  '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_SRC_REPL)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SOURCE__prep   ();
   /*---(check for mark)-----------------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      x_mode  = a_minor;
      x_saved = s_cur->contents [s_cur->cpos];
      ONE__replace (G_CHAR_PLACE);
      if (x_saved == NULL)  s_cur->contents [s_cur->cpos + 1] = NULL;
      SOURCE__done   ();
      SRC_UNDO__beg (__FUNCTION__);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(escaped chars)------------------*/
   if (a_major != 'r' && a_minor == '\\' && x_prev != '\\') {
      DEBUG_USER   yLOG_note    ("found a leading backslash");
      x_prev = '\\';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(mode changes)-------------------*/
   else if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      x_mode  = '-';
      rc = ONE__replace (x_saved);
      MODE_exit ();
      SRC_UNDO__end (__FUNCTION__);
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && strchr (MODES_ONELINE, MODE_curr ()) != NULL) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         SOURCE_mode (G_KEY_SPACE, a_minor);
      }
   }
   /*---(handle repeat r)----------------*/
   else if (a_major == 'r') {
      DEBUG_USER   yLOG_note    ("replace repeatedly");
      x_mode  = '-';
      if (a_minor >= 32 && a_minor < 127 && a_minor != '\\') {
         SRC_UNDO__beg (__FUNCTION__);
         SRC_UNDO__single ('r', s_cur->contents [s_cur->cpos], a_minor);
         rc = ONE__replace (a_minor);
         SRC_UNDO__end (__FUNCTION__);
         ++s_cur->cpos;
         if (REPEAT_count () == 0)  MODE_exit ();
      } else {
         rc = -1;
         MODE_exit ();
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle little r)----------------*/
   else if (x_mode == 'r') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      x_mode  = '-';
      if (x_prev == '\\') {
         x_prev = '-';
         SRC_UNDO__single ('r', x_saved, chrslashed (a_minor));
         rc = ONE__replace (chrslashed (a_minor));
      } else if (a_minor >= 32 && a_minor < 127) {
         if (a_minor == G_KEY_SPACE)  a_minor = G_CHAR_SPACE;
         SRC_UNDO__single ('r', x_saved, a_minor);
         rc = ONE__replace (a_minor);
      } else {
         rc = ONE__replace (x_saved);
      }
      SRC_UNDO__end (__FUNCTION__);
      /*> if (REPEAT_count () == 0) {                                                 <* 
       *>    MODE_exit ();                                                            <* 
       *> } else {                                                                    <* 
       *>    ++s_cur->cpos;                                                           <* 
       *> }                                                                           <*/
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
   }
   /*---(handle big R)-------------------*/
   else {
      /*---(check for backspace)------------*/
      if (a_minor == G_KEY_DEL || a_minor == G_KEY_BS) {
         DEBUG_USER   yLOG_note    ("handle a backspace/delete");
         if (s_cur->cpos > 0) {
            rc = ONE__replace (x_saved);
            --s_cur->cpos;
            x_saved = s_cur->contents [s_cur->cpos];
            rc = ONE__replace (G_CHAR_PLACE);
         }
      }
      /*---(handle new character)-----------*/
      else {
         DEBUG_USER   yLOG_note    ("replace and move right");
         if (x_prev == '\\') {
            x_prev = '-';
            SRC_UNDO__single ('R', x_saved, chrslashed (a_minor));
            rc = ONE__replace (chrslashed (a_minor));
         } else if (a_minor >= 32 && a_minor < 127) {
            if (a_minor == G_KEY_SPACE)  a_minor = G_CHAR_SPACE;
            SRC_UNDO__single ('R', x_saved, a_minor);
            rc = ONE__replace (a_minor);
         } else {
            rc = ONE__replace (x_saved);
         }
         ++s_cur->cpos;
         x_saved = s_cur->contents [s_cur->cpos];
         if (x_saved == NULL)  s_cur->contents [s_cur->cpos + 1] = NULL;
         rc = ONE__replace (G_CHAR_PLACE);
      }
   }
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
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
      case UMOD_VISUAL    : strlcpy (x_entry, " - --x-- --y-- -z- --x-- --y-- -z-    - --x-- --y-- -z- --x-- --y-- -z- ", LEN_RECD);  break;
      case SMOD_MACRO     : strlcpy (x_entry, " - len ---macro-keys--------------------------------------------------- ", LEN_RECD);  break;
      case SMOD_SRC_REG   : strlcpy (x_entry, " -   len  ---text-register-contents------------------------------------ ", LEN_RECD);  break;
      }
      mvprintw (x_bott - x_tall + 1, x_left, "%-*.*s", x_wide, x_wide, x_entry);
      attrset     (0);
      for (i = 0; i < x_tall - 2; ++i) {
         if ((i % 2) == 0)  yCOLOR_curs ("h_current"    );
         else               yCOLOR_curs ("map"          );
         switch (MODE_curr ()) {
         case UMOD_MARK      : MARK_infowin    (x_entry, i);  break;
         case UMOD_VISUAL    : VISU_infowin    (x_entry, i);  break;
                               /*> case SMOD_MACRO     : MACRO_infowin (x_entry, i);  break;                <*/
         case SMOD_SRC_REG   : SRC_REG_infowin (x_entry, i);  break;
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
   if (myVIKEYS.info_win == 'y') {
      HISTORY_infowin ();
      return 0;
   }
   if (MODE_not (UMOD_HISTORY))   return 0;
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
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_info    ("a_contents", a_contents);
   /*---(remove existing)----------------*/
   s_cur->cpos = 0;
   DEBUG_EDIT   yLOG_value   ("npos"      , s_cur->npos);
   DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   x_len       = s_cur->npos;
   SRC_UNDO__beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      DEBUG_EDIT   yLOG_value   ("i"         , i);
      SRC_UNDO__add ('d', 'l', 0, s_cur->contents [0], G_CHAR_NULL);
      ONE__delete ();
      DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   }
   /*---(add new)------------------------*/
   x_len = strllen (a_contents, LEN_RECD);
   s_cur->cpos = 0;
   for (i = 0; i < x_len; ++i) {
      ONE__append (a_contents [i]);
      SRC_UNDO__add (G_KEY_SPACE, 'a', s_cur->cpos, G_KEY_SPACE, s_cur->contents [s_cur->cpos]);
   }
   SRC_UNDO__end (__FUNCTION__);
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
      return 0;
   }
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("return, return to source mode");
      MODE_exit ();
      HISTORY_choose (HISTORY_use (MODE_curr (), s_now));
      /*> strlcpy (s_cur->contents, HISTORY_use (MODE_curr (), s_now), LEN_RECD);     <*/
      s_cur->cpos = 1;
      SOURCE__done ();
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
   if (s_now >= s_max) { s_now = s_max - 1; return -1; }
   if (s_now <  0    ) { s_now = 0;         return -1; }
   s_top = s_now - (s_lines / 2);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for input mode --------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
SRC_INPT_umode             (int  a_major, int  a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate a very basic vi-input mode by handling
    *   all characters, ignoring new line, and popping out with escape
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   static char x_prev      =  '-';
   char        rc          =    0;
   static char x_mode      =  '-';
   char        x_prevmode  =  '-';
   char        x_history   [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_SRC_INPT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   rc = -1;
   SOURCE__prep ();
   /*---(check for initial mark)---------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      x_mode  = a_minor;
      if (a_minor == 'a')  ONE__append (G_CHAR_PLACE);
      else                 ONE__insert (G_CHAR_PLACE);
      SOURCE__done   ();
      SRC_UNDO__beg (__FUNCTION__);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(escaped chars)------------------*/
   if (a_minor == '\\' && x_prev != '\\') {
      DEBUG_USER   yLOG_note    ("found a leading backslash");
      x_prev = '\\';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (x_prev == '\\') {
      DEBUG_USER   yLOG_note    ("converting backsplash character");
      x_prev = '-';
      SRC_UNDO__single (x_mode, G_CHAR_NULL, chrslashed (a_minor));
      rc = ONE__insert (chrslashed (a_minor));
      ++s_cur->cpos;
   }
   /*---(mode changes)-------------------*/
   else if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      x_mode  = '-';
      rc = ONE__delete ();
      --s_cur->cpos;
      SRC_UNDO__end (__FUNCTION__);
      MODE_exit ();
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && strchr (MODES_ONELINE, MODE_curr ()) != NULL) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         rc = SOURCE_mode (G_KEY_SPACE, a_minor);
      }
   }
   /*---(check for backspace)------------*/
   else if (a_minor == G_KEY_BS) {
      DEBUG_USER   yLOG_note    ("handle a backspace");
      if (s_cur->cpos > 0) {
         SRC_UNDO__add ('d', 'h', s_cur->cpos - 1, s_cur->contents [s_cur->cpos - 1], G_KEY_NULL);
         rc = ONE__backspace ();
      }
   }
   else if (a_minor == G_KEY_DEL) {
      DEBUG_USER   yLOG_note    ("handle a delete");
      if (s_cur->cpos < s_cur->npos - 1) {
         ++s_cur->cpos;
         SRC_UNDO__add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
         rc = ONE__delete ();
         --s_cur->cpos;
      }
   }
   /*---(handle new character)-----------*/
   else if (a_minor >= 32 && a_minor < 127) {
      DEBUG_USER   yLOG_note    ("move remaining chars to the right");
      if (a_minor == G_KEY_SPACE)  a_minor = G_CHAR_SPACE;
      SRC_UNDO__single (x_mode, G_CHAR_NULL, a_minor);
      rc = ONE__insert (a_minor);
      ++s_cur->cpos;
   }
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(check for history)--------------*/
   x_prevmode = MODE_prev ();
   sprintf (x_history, "%c%c%c¤"  , x_prevmode, x_prevmode, x_prevmode);
   if (strcmp (s_cur->contents, x_history) == 0) {
      rc = ONE__delete ();
      --s_cur->cpos;
      SRC_UNDO__end (__FUNCTION__);
      SOURCE__done ();
      MODE_exit ();
      /*> if (x_prevmode == MODE_SEARCH)  strlcpy (s_cur->contents, "/", LEN_RECD);   <* 
       *> else                            strlcpy (s_cur->contents, ":", LEN_RECD);   <*/
      /*> SOURCE__done   ();                                                          <*/
      HISTORY_start ();
      MODE_enter  (UMOD_HISTORY);
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     unit testing functions                   ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
SOURCE__unit            (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_index     =    0;
   char        t           [LEN_RECD];
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRC unit         : question not understood", LEN_STR);
   x_index = SRC_REG_valid  (a_reg);
   if (x_index < 0)  x_index = 0;
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "position"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC position     : %3dw, %3da, %3db, %3dc, %3de", s_cur->wide, s_cur->apos, s_cur->bpos, s_cur->cpos, s_cur->epos);
   }
   else if (strcmp (a_question, "contents"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC contents     : %3d:%-.40s:", s_cur->npos, s_cur->contents);
   }
   else if (strcmp (a_question, "words"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC words        : %3d:%-.40s:", s_cur->npos, s_cur->words);
   }
   else if (strcmp (a_question, "source"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC source       : %3d:%-.40s:", s_src.npos, s_src.contents);
   }
   else if (strcmp (a_question, "command"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC command      : %3d:%-.40s:", s_cmd.npos, s_cmd.contents);
   }
   else if (strcmp (a_question, "label"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC label        : %s", s_cur->label);
   }
   else if (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC selection    :   %cl, %3dr, %3db, %3dc, %3de", s_live, s_root, s_bsel, s_cur->cpos, s_esel);
   }
   else if (strcmp (a_question, "textreg"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC textreg      : %2d %3d:%-20.20s: %3d %3d %c %s", x_index, s_tregs [x_index].len, s_tregs [x_index].data, s_tregs [x_index].bpos, s_tregs [x_index].epos, s_tregs [x_index].source, s_tregs [x_index].label);
   }
   else if (strcmp (a_question, "undo"           )   == 0) {
      SRC_UNDO_status (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC undo queue   : %-.50s", t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




