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



static char    (*s_saver) (char *a_contents);



/*====================------------------------------------====================*/
/*===----                      working functions                       ----===*/
/*====================------------------------------------====================*/
static void  o___WORKERS_________o () { return; }

char
yvikeys_src_one_replace            (char a_key)
{
   s_cur->contents [s_cur->cpos] = a_key;
   return 0;
}

char
yvikeys_src_one_delete             (void)
{
   int         i           =   0;
   for (i = s_cur->cpos; i < s_cur->npos; ++i) {
      s_cur->contents [i] = s_cur->contents [i + 1];
   }
   --s_cur->npos;
   return 0;
}

char
yvikeys_src_one_backspace          (void)
{
   if (s_cur->cpos <= 0)  return -1;
   --s_cur->cpos;
   yvikeys_src_one_delete  ();
   return 0;
}

char
yvikeys_src_one_insert             (char a_key)
{
   int         i           =   0;
   for (i = s_cur->npos + 1; i >= s_cur->cpos; --i) {
      s_cur->contents [i + 1] = s_cur->contents [i];
   }
   ++s_cur->npos;
   yvikeys_src_one_replace (a_key);
   return 0;
}

char
yvikeys_src_one_append             (char a_key)
{
   if (s_cur->npos == 0) {
      yvikeys_src_one_insert (a_key);
   } else {
      ++s_cur->cpos;
      yvikeys_src_one_insert (a_key);
   }
   ++s_cur->npos;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      text reg actions                        ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char         /*-> process keys for register actions --[ ------ [gz.320.011.02]*/ /*-[01.0000.113.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_clear       (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =   0;
   int         x_beg       =   0;
   int         x_end       =   0;
   /*---(prepare)------------------------*/
   yvikeys_sreg_selected (&x_beg, &x_end, NULL);
   /*---(begin)--------------------------*/
   yvikeys_sundo_beg (__FUNCTION__);
   /*---(act)----------------------------*/
   for (i = x_beg; i <= x_end; ++i) {
      s_cur->cpos = i;
      yvikeys_sundo_add ('x', 'l', i, s_cur->contents [i], G_CHAR_STORAGE);
      yvikeys_src_one_replace (G_CHAR_STORAGE);
   }
   /*---(end)----------------------------*/
   yvikeys_sundo_end (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register action ---[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_delete      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   int         x_diff      =   0;
   int         i           =   0;
   int         x_beg       =   0;
   int         x_end       =   0;
   /*---(prepare)------------------------*/
   yvikeys_sreg_selected (&x_beg, &x_end, NULL);
   /*---(short-path)---------------------*/
   yvikeys_sundo_beg (__FUNCTION__);
   /*---(short-path)---------------------*/
   if (yvikeys_sreg_isdead () && a_minor == 'D') {
      if (s_cur->cpos <= 0)  return -1;
      --s_cur->cpos;
      yvikeys_sundo_add (a_major, tolower (a_minor), i, s_cur->contents [i], G_KEY_NULL);
      yvikeys_src_one_delete ();
      return 0;
   }
   else {
      /*---(set size)-----------------------*/
      x_diff  = x_end - x_beg + 1;
      /*---(delete)-------------------------*/
      s_cur->cpos = x_beg;
      for (i = 0; i < x_diff; ++i) {
         yvikeys_sundo_add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
         yvikeys_src_one_delete ();
      }
   }
   yvikeys_sundo_end (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [ge.640.051.04]*/ /*-[00.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_copy       (void)
{
   /*---(locals)-----------+-----------+-*/
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_data      [LEN_RECD];
   int         x_beg       =   0;
   int         x_end       =   0;
   /*---(prepare)------------------------*/
   yvikeys_sreg_selected (&x_beg, &x_end, NULL);
   /*---(set size)-----------------------*/
   x_start = s_cur->contents + x_beg;
   x_len   = x_end - x_beg + 1;
   /*---(copy)---------------------------*/
   strlcpy (x_data, x_start, x_len + 1);
   yvikeys_sreg_save (s_cur->label, x_data);
   yvikeys_sreg_selected (NULL, NULL, &(s_cur->cpos));
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> replace with register text ---------[ ------ [gz.740.061.21]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_replace     (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_dlen      =   0;
   char        x_data      [LEN_RECD];
   int         x_beg       =   0;
   int         x_end       =   0;
   int         x_len       =   0;
   int         i           =   0;
   char        x_ch        = G_CHAR_STORAGE;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(get register data)--------------*/
   yvikeys_sreg_fetch    (&x_dlen, x_data);
   DEBUG_EDIT   yLOG_value   ("x_dlen"    , x_dlen);
   DEBUG_EDIT   yLOG_info    ("x_data"    , x_data);
   yvikeys_sreg_selected (&x_beg, &x_end, NULL);
   DEBUG_EDIT   yLOG_value   ("x_beg"     , x_beg);
   DEBUG_EDIT   yLOG_value   ("x_end"     , x_end);
   s_cur->cpos = x_beg;
   /*---(set the length)-----------------*/
   DEBUG_EDIT   yLOG_value   ("islive"    , yvikeys_sreg_islive ());
   if (yvikeys_sreg_islive ())  x_len = x_end - x_beg + 1;
   else                         x_len = x_dlen;
   DEBUG_EDIT   yLOG_value   ("x_len"     , x_len);
   /*---(set the start)------------------*/
   yvikeys_sundo_beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      /*---(get char)--------------------*/
      if (i < x_dlen)    x_ch = x_data [i];
      else               x_ch = G_CHAR_STORAGE;
      /*---(add to source)---------------*/
      if (s_cur->cpos < s_cur->npos) {
         yvikeys_sundo_add   (G_KEY_SPACE, 'r', s_cur->cpos, s_cur->contents [s_cur->cpos], x_ch);
         yvikeys_src_one_replace (x_ch);
         ++s_cur->cpos;
      } else {
         yvikeys_sundo_add   (G_KEY_SPACE, 'r', s_cur->cpos, s_cur->contents [s_cur->cpos], x_ch);
         yvikeys_src_one_append  (x_ch);
      }
      /*---(done)------------------------*/
   }
   yvikeys_sundo_end (__FUNCTION__);
   --s_cur->cpos;
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> insert/append register contents ----[ ------ [gz.640.151.11]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_paste       (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         x_dlen      =   0;
   char        x_data      [LEN_RECD];
   int         x_beg       =   0;
   int         x_end       =   0;
   int         i           =   0;
   /*---(get register data)--------------*/
   yvikeys_sreg_fetch    (&x_dlen, x_data);
   yvikeys_sreg_selected (&x_beg, &x_end, NULL);
   s_cur->cpos = x_beg;
   /*---(prepare)------------------------*/
   if (a_dir == 'a' && s_cur->npos > 0) {
      ++s_cur->cpos;
   }
   /*---(set the start)------------------*/
   yvikeys_sundo_beg (__FUNCTION__);
   for (i = 0; i < x_dlen; ++i) {
      yvikeys_sundo_add (G_KEY_SPACE, 'i', s_cur->cpos, G_CHAR_NULL, x_data [i]);
      yvikeys_src_one_insert (x_data [i]);
      ++s_cur->cpos;
   }
   yvikeys_sundo_end (__FUNCTION__);
   --s_cur->cpos;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> remove existing and add new --------[ ------ [gz.640.151.11]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys_src_swapall     (char *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_len       =    0;
   char        x_old       [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_info    ("a_new"     , a_new);
   /*---(start undo logging)-------------*/
   yvikeys_sundo_beg (__FUNCTION__);
   /*---(remove existing)----------------*/
   DEBUG_EDIT   yLOG_value   ("npos"      , s_cur->npos);
   x_len = s_cur->npos;
   s_cur->cpos = 0;
   for (i = 0; i < x_len; ++i) {
      DEBUG_EDIT   yLOG_complex ("delete"    , "%2d, %c", s_cur->cpos, s_cur->contents [s_cur->cpos]);
      yvikeys_sundo_add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_CHAR_NULL);
      yvikeys_src_one_delete ();
   }
   /*---(add new)------------------------*/
   x_len  = strllen (a_new, LEN_RECD);
   for (i = 0; i < x_len; ++i) {
      DEBUG_EDIT   yLOG_complex ("append"    , "%2d, %c", i, a_new [i]);
      yvikeys_src_one_append (a_new [i]);
      yvikeys_sundo_add (G_KEY_SPACE, 'a', i, G_KEY_NULL, a_new [i]);
   }
   /*---(stop undo logging)--------------*/
   yvikeys_sundo_end (__FUNCTION__);
   s_cur->cpos = 1;
   SOURCE__done ();
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
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
         strlcpy (t, s_cur->contents, LEN_RECD);
         strldchg (t, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
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
   yvikeys_sreg_reset  (0);
   yvikeys_sundo_purge (0);
   yVIKEYS_source (s_src.label, s_src.original);
   yvikeys_map_reposition  ();
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
   DEBUG_EDIT   yLOG_complex ("reset"     , "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
   yvikeys_sreg_reset  (0);
   yvikeys_sundo_purge (0);
   yVIKEYS_source (s_src.label, s_src.original);
   yvikeys_map_reposition  ();
   DEBUG_EDIT   yLOG_complex ("post-reset", "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
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
      else if (strchr ("² "  , a_curr) != NULL)     x_mode = '²';
      else                                          x_mode = 's';
   }
   /*---(word)---------------------------*/
   else {
      if      (strchr ("² "  , a_curr) != NULL)     x_mode = '²';
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
   char        x_save      =  '²';
   char        x_mode      =  '²';
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   DEBUG_EDIT   yLOG_value   ("s_npos"    , s_cur->npos);
   DEBUG_EDIT   yLOG_info    ("contents"  , s_cur->contents);
   /*---(mark tail)----------------------*/
   s_cur->words [s_cur->npos] = 0;
   if (s_cur->npos <= 0) {
      DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(walk string)--------------------*/
   for (i = 0; i < s_cur->npos; ++i) {
      /*---(initialize)------------------*/
      s_cur->words [i] = '²';
      /*---(check)-----------------------*/
      x_mode = SOURCE__words_mode (s_cur->contents [0], s_cur->contents [i], x_save);
      if (x_mode == x_save)  continue;
      /*---(update current)--------------*/
      if (x_save == '²' && x_mode == 'w')   SOURCE__words_update (i    , '<');
      if (x_save == '²' && x_mode == 's')   SOURCE__words_update (i    , '<');
      if (x_save == 'w' && x_mode == 's')   SOURCE__words_update (i    , '<');
      if (x_save == 's' && x_mode == 'w')   SOURCE__words_update (i    , '<');
      /*---(update previous)-------------*/
      if (x_save == 'w' && x_mode == '²')   SOURCE__words_update (i - 1, '>'); 
      if (x_save == 's' && x_mode == '²')   SOURCE__words_update (i - 1, '>'); 
      if (x_save == 'w' && x_mode == 's')   SOURCE__words_update (i - 1, '>');
      if (x_save == 's' && x_mode == 'w')   SOURCE__words_update (i - 1, '>');
      /*---(save)------------------------*/
      x_save = x_mode;
      /*---(done)------------------------*/
   }
   /*---(mark end)-----------------------*/
   if (x_mode != '·')  SOURCE__words_update (s_cur->npos - 1, '>');
   DEBUG_EDIT   yLOG_info    ("words"     , s_cur->words);
   /*---(complete)-----------------------*/
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
   DEBUG_EDIT   yLOG_complex ("limits"    , "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
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
   DEBUG_EDIT   yLOG_complex ("post-limit", "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
   /*---(selection update)---------------*/
   yvikeys_sreg_update (s_cur->cpos);
   DEBUG_EDIT   yLOG_complex ("post-sreg" , "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
   /*---(update word breaks)-------------*/
   SOURCE__words ();
   DEBUG_EDIT   yLOG_complex ("post-words", "%3dn %3db %3de %3dc", s_cur->npos, s_cur->bpos, s_cur->epos, s_cur->cpos);
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
   yvikeys_sundo_purge (0);
   /*---(populate globals)---------------*/
   SOURCE_menu_prep ();
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
   SOURCE__reset ();
   /*---(clear)--------------------------*/
   yvikeys_sundo_purge (0);
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_SOURCE);
   STATUS_init_set   (UMOD_SRC_REPL);
   STATUS_init_set   (UMOD_SRC_INPT);
   STATUS_init_set   (UMOD_SRC_UNDO);
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
   strldchg (s_cur->original, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);
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

int   yvikeys_src_cpos        (void) { return s_cur->cpos; }
int   yvikeys_src_npos        (void) { return s_cur->npos; }
char *yvikeys_src_contents    (void) { return s_cur->contents; }

char
yvikeys_src_wander_pos  (void)
{
   DEBUG_EDIT   yLOG_complex ("position"  , "%3db, %3dc, %3de, %3dn", s_cur->bpos, s_cur->cpos, s_cur->epos, s_cur->npos);
   return 0;
}

char
yvikeys_src_wander       (char *a_new)
{
   DEBUG_EDIT   yLOG_complex ("position"  , "%3db, %3dc, %3de, %3dn", s_cur->bpos, s_cur->cpos, s_cur->epos, s_cur->npos);
   strlcpy (s_cur->contents, a_new, LEN_LABEL);
   s_cur->npos = strllen (s_cur->contents, LEN_RECD);
   SOURCE__done();
   DEBUG_EDIT   yLOG_complex ("position"  , "%3db, %3dc, %3de, %3dn", s_cur->bpos, s_cur->cpos, s_cur->epos, s_cur->npos);
   return 0;
}

char yvikeys_src_wander_cpos  (int a_cpos) { s_cur->cpos = a_cpos; return 0; }

char
yvikeys_src_wander_done (void)
{
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   s_cur->npos = strlen (s_cur->contents);
   DEBUG_EDIT   yLOG_value   ("npos"      , s_cur->npos);
   for (i = 0; i < LEN_RECD; ++i) {
      if (s_cur->contents [i] != (char) G_CHAR_PLACE)  continue;
      s_cur->cpos = i;
      break;
   }
   DEBUG_EDIT   yLOG_value   ("cpos"      , s_cur->cpos);
   SOURCE__done();
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
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
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_display" , a_display);
   DEBUG_GRAF   yLOG_char    ("MODE_curr" , MODE_curr ());
   DEBUG_GRAF   yLOG_char    ("MODE_prev" , MODE_prev ());
   /*---(fast-track formula)-------------*/
   if (a_display == YVIKEYS_FORMULA) {
      if (MODE_curr () != MODE_SOURCE && MODE_prev () != MODE_SOURCE)  {
         DEBUG_GRAF   yLOG_note    ("formula fast track");
         x_code = 'm';
      }
   }
   /*---(fast-track command)-------------*/
   if (a_display == YVIKEYS_COMMAND) {
      if (strchr (":/", MODE_curr ()) == NULL && strchr (":/", MODE_prev ()) == NULL) {
         DEBUG_GRAF   yLOG_note    ("command fast track");
         x_code = 'c';
      }
   }
   /*---(check focused)------------------*/
   if (x_code == '-' && strchr (MODES_ONELINE, MODE_curr ()) != NULL)  {
      DEBUG_GRAF   yLOG_note    ("current source mode");
      x_edit = 'y';
      x_code = 's';
   }
   /*---(check editing)------------------*/
   else if (x_code == '-' && strchr (MODES_ONELINE, MODE_prev ()) != NULL)  {
      DEBUG_GRAF   yLOG_note    ("sub-mode editing");
      x_edit = 'y';
      switch (MODE_curr ()) {
      case SMOD_SREG       : x_code = 't';  break;
      case UMOD_SRC_REPL   : x_code = 'r';  break;
      case UMOD_SRC_INPT   : x_code = 'i';  break;
      case UMOD_WANDER     : x_code = 'w';  break;
      default              : x_code = '-';  break;
      }
   }
   /*---(just a safety)------------------*/
   else if (x_code == '-') {
      DEBUG_GRAF   yLOG_note    ("catch for mistakes");
      x_code = 'e';
   }
   /*---(set curses color)---------------*/
   attrset     (0);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      DEBUG_GRAF   yLOG_note    ("assign a ncurses color");
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
      DEBUG_GRAF   yLOG_note    ("assign a opengl color");
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
   DEBUG_GRAF   yLOG_char    ("x_edit"    , x_edit);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return x_edit;
}

char
SOURCE__opengl          (tEDIT *a_cur, int a_lef, int a_rig, int a_bot, int a_top, char a_edit)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD];
   char        c1          =  ' ';
   char        c2          =  ' ';
   int         x_bpos      =    0;
   int         x_epos      =    0;
   float       x_beg       =    0;
   float       x_end       =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(fast path for command)----------*/
   if (a_cur->type == EDIT_CMDS && a_edit != 'y') {
      DEBUG_GRAF   yLOG_note    ("fast path to display mode message");
      glPushMatrix    (); {
         glTranslatef ( 0.0f, 1.0f, 0.0f);
         glColor4f   (0.0, 0.0, 0.0, 1.0);
         yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, MODE_message ());
      } glPopMatrix   ();
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_GRAF   yLOG_complex  ("bounds"    , "%3dl to %3dr, %3db to %3dt", a_lef, a_rig, a_bot, a_top);
   /*---(background)---------------------*/
   glColor4f   (0.0, 0.0, 0.0, 1.0);   /* DEBUGGING ONLY */
   glPushMatrix    (); {
      glBegin         (GL_POLYGON); {
         glVertex3f  (a_lef - 2, a_top + 2, 100.0f);
         glVertex3f  (a_rig + 2, a_top + 2, 100.0f);
         glVertex3f  (a_rig + 2, a_bot - 2, 100.0f);
         glVertex3f  (a_lef - 2, a_bot - 2, 100.0f);
      } glEnd   ();
   } glPopMatrix   ();
   SOURCE__color (YVIKEYS_OPENGL);
   glPushMatrix    (); {
      glBegin         (GL_POLYGON); {
         glVertex3f  (a_lef, a_top, 100.0f);
         glVertex3f  (a_rig, a_top, 100.0f);
         glVertex3f  (a_rig, a_bot, 100.0f);
         glVertex3f  (a_lef, a_bot, 100.0f);
      } glEnd   ();
   } glPopMatrix   ();
   /*---(selection)----------------------*/
   if (a_edit == 'y' && yvikeys_sreg_islive () && a_cur->npos > 0) {
      yvikeys_sreg_selected (&x_bpos, &x_epos, NULL);
      x_beg = x_bpos;
      x_end = x_epos;
      if (x_beg < a_cur->bpos)  x_beg = a_cur->bpos;
      if (x_end > a_cur->epos)  x_end = a_cur->epos;
      x_beg += 5;
      x_end += 6;
      x_beg *= myVIKEYS.font_scale;
      x_end *= myVIKEYS.font_scale;
      glColor4f   (0.8, 0.8, 0.8, 1.0);
      glPushMatrix    (); {
         glBegin         (GL_POLYGON); {
            glVertex3f  (a_lef + x_beg, a_top, 110.0f);
            glVertex3f  (a_lef + x_end, a_top, 110.0f);
            glVertex3f  (a_lef + x_end, a_bot, 110.0f);
            glVertex3f  (a_lef + x_beg, a_bot, 110.0f);
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
            glVertex3f  (a_lef + x_beg , a_top, 120.0f);
            glVertex3f  (a_lef + x_end , a_top, 120.0f);
            glVertex3f  (a_lef + x_end , a_bot, 120.0f);
            glVertex3f  (a_lef + x_beg , a_bot, 120.0f);
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
   DEBUG_GRAF   yLOG_info     ("contents"  , a_cur->contents);
   DEBUG_GRAF   yLOG_value    ("apos"      , a_cur->apos);
   DEBUG_GRAF   yLOG_value    ("bpos"      , a_cur->bpos);
   DEBUG_GRAF   yLOG_value    ("cpos"      , a_cur->cpos);
   DEBUG_GRAF   yLOG_value    ("npos"      , a_cur->npos);
   /*> if (a_cur->npos == 0) sprintf (t, "   %c", G_CHAR_NULL);                                                                              <* 
    *> else                  sprintf (t, "%4d%c%-*.*s%c"  , a_cur->npos, c1, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos, c2);   <*/
   if (a_cur->npos == 0) sprintf (t, "   %c", G_CHAR_NULL);
   else                  sprintf (t, "%4d%c%-*.*s%c"  , a_cur->cpos, c1, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos, c2);
   glPushMatrix    (); {
      glTranslatef (a_lef, a_bot + 3, 130.0f);
      glColor4f   (0.0, 0.0, 0.0, 1.0);
      yFONT_print (myVIKEYS.font, myVIKEYS.point, YF_BOTLEF, t);
   } glPopMatrix   ();
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE__curses          (tEDIT *a_cur, int a_left, int a_bott, char a_edit)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_beg       =    0;
   int         x_end       =    0;
   int         x_len       =    0;
   char        c           =  ' ';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(fast path for command)----------*/
   if (a_cur->type == EDIT_CMDS && a_edit != 'y') {
      mvprintw (a_bott, a_left, "%-*.*s", a_cur->wide, a_cur->wide, MODE_message ());
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(base content)-------------------*/
   if (a_cur->npos == 0) mvprintw (a_bott, a_left, "   %c %-*.*s ", G_CHAR_NULL, a_cur->apos, a_cur->apos, " ");
   /*> else                  mvprintw (a_bott, a_left, "%4d %-*.*s ", a_cur->npos, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos);   <*/
   else                  mvprintw (a_bott, a_left, "%4d %-*.*s ", a_cur->cpos, a_cur->apos, a_cur->apos, a_cur->contents + a_cur->bpos);
   /*---(selection)----------------------*/
   if (a_edit == 'y' && yvikeys_sreg_islive () && a_cur->npos > 0) {
      yvikeys_sreg_selected (&x_beg, &x_end, NULL);
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
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
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
   int         x_xmin, x_xmax;
   int         x_ymin, x_ymax;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (a_mode, &x_left, &x_wide, &x_bott, &x_tall, NULL);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%3dl, %3dw, %3db, %3dt", x_left, x_wide, x_bott, x_tall);
   yVIKEYS_view_bounds   (a_mode, &x_xmin, &x_xmax, NULL, &x_ymin, &x_ymax, NULL);
   DEBUG_GRAF   yLOG_complex  ("bounds"    , "%3dx to %3dx, %3dy to %3dy", x_xmin, x_xmax, x_ymin, x_ymax);
   if (myVIKEYS.env == YVIKEYS_CURSES)  a_cur->wide = x_wide;
   else                                 a_cur->wide = x_wide / myVIKEYS.font_scale;
   a_cur->apos = a_cur->wide - 6;
   DEBUG_GRAF   yLOG_char    ("on"        , x_on);
   DEBUG_GRAF   yLOG_value   ("wide"      , a_cur->wide);
   DEBUG_GRAF   yLOG_value   ("apos"      , a_cur->apos);
   DEBUG_GRAF   yLOG_info    ("contents"  , a_cur->contents);
   DEBUG_GRAF   yLOG_value   ("npos"      , a_cur->npos);
   DEBUG_GRAF   yLOG_char    ("type"      , a_cur->type);
   /*---(defense)------------------------*/
   if (x_on != 'y') {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(display)------------------------*/
   x_edit = SOURCE__color (a_mode);
   DEBUG_GRAF   yLOG_char    ("x_edit"    , x_edit);
   if (myVIKEYS.env == YVIKEYS_CURSES) {
      SOURCE__curses (a_cur, x_left, x_bott, x_edit);
   } else {
      SOURCE__opengl (a_cur, x_xmin, x_xmax, x_ymin, x_ymax, x_edit);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_formula             (void)
{
   tEDIT      *x_cur       = NULL;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   x_cur  = &s_src;
   SOURCE_display (x_cur, YVIKEYS_FORMULA);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_command             (void)
{
   tEDIT      *x_cur       = NULL;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   x_cur  = &s_cmd;
   SOURCE_display (x_cur, YVIKEYS_COMMAND);
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
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
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("mode_curr" , MODE_curr ());
   DEBUG_GRAF   yLOG_char    ("mode_prev" , MODE_prev ());
   /*---(check modes)-----------------*/
   if      (strchr (MODES_ONELINE, MODE_curr ()) != NULL )      x_mode = MODE_curr ();
   else if (strchr (MODES_ONELINE, MODE_prev ()) != NULL )      x_mode = MODE_prev ();
   DEBUG_GRAF   yLOG_char    ("x_mode"    , x_mode);
   if (x_mode == '-')  {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check modes)-----------------*/
   --rce;  switch (x_mode) {
   case MODE_SOURCE  :
      DEBUG_GRAF   yLOG_note    ("mode source");
      x_on   = yVIKEYS_view_size     (YVIKEYS_FORMULA, NULL, NULL, NULL, NULL, NULL);
      x_cur  = &s_src;
      break;
   case MODE_COMMAND :
   case MODE_SEARCH  :
      DEBUG_GRAF   yLOG_note    ("mode command/search");
      x_on   = yVIKEYS_view_size     (YVIKEYS_COMMAND, NULL, NULL, NULL, NULL, NULL);
      x_cur  = &s_cmd;
      break;
   default :
      DEBUG_GRAF   yLOG_note    ("default");
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("x_on"      , x_on);
   if (x_on != 'y')  SOURCE_display (x_cur, YVIKEYS_FLOAT);
   /*---(complete)--------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE_menu_prep        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_on        =  '-';
   int         x_wide      =    0;
   tEDIT      *x_cur       = NULL;
   /*---(header)----------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(get wide)--------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_COMMAND, NULL, &x_wide, NULL, NULL, NULL);
   DEBUG_EDIT   yLOG_char    ("on"        , x_on);
   if (x_on != 'y') yVIKEYS_view_size     (YVIKEYS_FLOAT, NULL, &x_wide, NULL, NULL, NULL);
   DEBUG_EDIT   yLOG_char    ("x_wide"    , x_wide);
   /*---(set current)-----------------*/
   x_cur  = &s_cmd;
   /*---(set wide)--------------------*/
   DEBUG_EDIT   yLOG_char    ("env"       , myVIKEYS.env);
   if (myVIKEYS.env == YVIKEYS_CURSES)  x_cur->wide = x_wide;
   else                                 x_cur->wide = x_wide / myVIKEYS.font_scale;
   /*---(set apos)--------------------*/
   x_cur->apos = x_cur->wide - 6;
   DEBUG_EDIT   yLOG_char    ("apos"      , x_cur->apos);
   /*---(complete)--------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}



char
SOURCE_status_words    (char *a_list)
{
   sprintf (a_list, "word %s", s_cur->words);
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
   DEBUG_EDIT   yLOG_char    ("s_live"    , yvikeys_sreg_getlive ());
   --rce;  if (yvikeys_sreg_islive ()) {
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
   yvikeys_sundo_beg (__FUNCTION__);
   for (i = 0; i < x_len; ++i) {
      if (a_major == 'd') {
         if (a_minor == 'h')  --s_cur->cpos;
         yvikeys_sundo_add (a_major, 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
         yvikeys_src_one_delete ();
      } else {
         if (a_minor == 'h')  --s_cur->cpos;
         yvikeys_sundo_add (a_major, 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_CHAR_STORAGE);
         yvikeys_src_one_replace (G_CHAR_STORAGE);
         if (a_minor != 'h')  ++s_cur->cpos;
      }
   }
   yvikeys_sundo_end (__FUNCTION__);
   if (a_major == 'x' && a_minor != 'h')  s_cur->cpos = x_pos;
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
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
      if (yvikeys_sreg_islive ()) {
         DEBUG_USER   yLOG_note    ("escape, means get out of selection");
         yvikeys_sreg_reset (s_cur->cpos);
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
         DEBUG_USER   yLOG_note    ("switch to a text register mode (yYpP)");
         yvikeys_sreg_setreg ('"');
         MODE_enter (SMOD_SREG);
         rc = yvikeys_sreg_smode (G_KEY_SPACE, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (yvikeys_sreg_islive () && strchr ("xXdD", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a text register mode (xXdD)");
         yvikeys_sreg_setreg ('"');
         MODE_enter (SMOD_SREG);
         rc = yvikeys_sreg_smode (G_KEY_SPACE, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr (g_multisrc, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("beginning of multi-key command");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(mode changes)----------------*/
      switch (a_minor) {
      case '@'      :
         DEBUG_USER   yLOG_note    ("macro execution");
         MODE_enter  (SMOD_MACRO   );
         rc = a_minor;
         break;
      case 'q'      :
         IF_MACRO_RECORDING {
            DEBUG_USER   yLOG_note    ("end macro recording");
            rc = yvikeys_macro_recend ();
         } else {
            DEBUG_USER   yLOG_note    ("begin macro recording");
            MODE_enter  (SMOD_MACRO   );
            rc = a_minor;
         }
         break;
      case 'Q'      :
         DEBUG_USER   yLOG_note    ("reset macro recording");
         rc = yvikeys_macro_resetall ();
         break;
      case  'v' :
         if (yvikeys_sreg_islive ())
            s_cur->cpos = yvikeys_sreg_reverse  ();
         else
            yvikeys_sreg_makelive ();
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
         DEBUG_USER   yLOG_note    ("switch to a text register mode (¶)");
         MODE_enter (SMOD_SREG);
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
         /*> DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <*/
         break;
      case  'R' :
         DEBUG_USER   yLOG_note    ("enter replace mode");
         MODE_enter (UMOD_SRC_REPL);
         SRC_REPL_umode ('m', a_minor);
         /*> DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <*/
         rc = 0;
         break;
      case  'i' : case  'a' : case  'I' : case  'A' :
         DEBUG_USER   yLOG_note    ("enter input mode");
         if (a_minor == 'A')  s_cur->cpos = s_cur->npos - 1;
         if (a_minor == 'I')  s_cur->cpos = 0;
         MODE_enter (UMOD_SRC_INPT);
         SRC_INPT_umode ('m', tolower (a_minor));
         /*> DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <*/
         rc = tolower (a_minor);
         break;
      case  'D' :
         rc = SOURCE_delete     ('d', 'l');
         SOURCE__done ();
         break;
      case  'X' :
         rc = SOURCE_delete     ('x', 'l');
         SOURCE__done ();
         break;
      }
      /*---(check for backspace)------------*/
      if (a_minor == G_KEY_BS) {
         DEBUG_USER   yLOG_note    ("handle a backspace");
         rc = SOURCE_delete     ('d', 'h');
         SOURCE__done ();
      }
      if (a_minor == G_KEY_DEL) {
         DEBUG_USER   yLOG_note    ("handle a delete");
         rc = SOURCE_delete     ('d', 'l');
         SOURCE__done ();
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
         yvikeys_sreg_reset (s_cur->cpos);
         rc = yvikeys_sundo_undo (&(s_cur->cpos));
         SOURCE__done ();
         break;
      case 'U' :
         yvikeys_sreg_reset (s_cur->cpos);
         rc = yvikeys_sundo_redo (&(s_cur->cpos));
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
                  rc = tolower (a_minor);
                  DEBUG_USER   yLOG_exit    (__FUNCTION__);
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
      yvikeys_src_one_replace (G_CHAR_PLACE);
      if (x_saved == NULL)  s_cur->contents [s_cur->cpos + 1] = NULL;
      SOURCE__done   ();
      yvikeys_sundo_beg (__FUNCTION__);
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
      rc = yvikeys_src_one_replace (x_saved);
      MODE_exit ();
      yvikeys_sundo_end (__FUNCTION__);
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
         yvikeys_sundo_beg (__FUNCTION__);
         yvikeys_sundo_single ('r', s_cur->cpos, s_cur->contents [s_cur->cpos], a_minor);
         rc = yvikeys_src_one_replace (a_minor);
         yvikeys_sundo_end (__FUNCTION__);
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
         yvikeys_sundo_single ('r', s_cur->cpos, x_saved, chrslashed (a_minor));
         rc = yvikeys_src_one_replace (chrslashed (a_minor));
      } else if (a_minor >= 32 && a_minor < 127) {
         if (a_minor == G_KEY_SPACE)  a_minor = G_CHAR_STORAGE;
         yvikeys_sundo_single ('r', s_cur->cpos, x_saved, a_minor);
         rc = yvikeys_src_one_replace (a_minor);
      } else {
         rc = yvikeys_src_one_replace (x_saved);
      }
      yvikeys_sundo_end (__FUNCTION__);
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
            rc = yvikeys_src_one_replace (x_saved);
            --s_cur->cpos;
            x_saved = s_cur->contents [s_cur->cpos];
            rc = yvikeys_src_one_replace (G_CHAR_PLACE);
         }
      }
      /*---(handle new character)-----------*/
      else {
         DEBUG_USER   yLOG_note    ("replace and move right");
         if (x_prev == '\\') {
            x_prev = '-';
            yvikeys_sundo_single ('R', s_cur->cpos, x_saved, chrslashed (a_minor));
            rc = yvikeys_src_one_replace (chrslashed (a_minor));
         } else if (a_minor >= 32 && a_minor < 127) {
            if (a_minor == G_KEY_SPACE)  a_minor = G_CHAR_STORAGE;
            yvikeys_sundo_single ('R', s_cur->cpos, x_saved, a_minor);
            rc = yvikeys_src_one_replace (a_minor);
         } else {
            rc = yvikeys_src_one_replace (x_saved);
         }
         ++s_cur->cpos;
         x_saved = s_cur->contents [s_cur->cpos];
         if (x_saved == NULL)  s_cur->contents [s_cur->cpos + 1] = NULL;
         rc = yvikeys_src_one_replace (G_CHAR_PLACE);
      }
   }
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
}


char
SRC_INPT__addone        (char a_mode, int a_minor)
{
   DEBUG_USER   yLOG_note    ("add and move remaining chars to the right");
   a_minor = chrvisible (a_minor);
   yvikeys_sundo_single (a_mode, s_cur->cpos, G_CHAR_NULL, a_minor);
   yvikeys_src_one_insert (a_minor);
   ++s_cur->cpos;
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
   static char x_quoting   =  '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_USER   yLOG_char    ("mode"      , x_quoting);
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
      if (a_minor == 'a')  yvikeys_src_one_append (G_CHAR_PLACE);
      else                 yvikeys_src_one_insert (G_CHAR_PLACE);
      SOURCE__done   ();
      yvikeys_sundo_beg (__FUNCTION__);
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
      yvikeys_sundo_single (x_mode, s_cur->cpos, G_CHAR_NULL, chrslashed (a_minor));
      rc = yvikeys_src_one_insert (chrslashed (a_minor));
      ++s_cur->cpos;
   }
   /*---(mode changes)-------------------*/
   else if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      if (x_quoting == 'y' && MODE_prev () != MODE_SOURCE) {
         rc = SRC_INPT__addone (x_mode, a_minor);
      } else {
         DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
         x_quoting = '-';
         x_mode    = '-';
         rc = yvikeys_src_one_delete ();
         --s_cur->cpos;
         yvikeys_sundo_end (__FUNCTION__);
         MODE_exit ();
         DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
         if (a_minor == G_KEY_RETURN && strchr (MODES_ONELINE, MODE_curr ()) != NULL) {
            DEBUG_USER   yLOG_note    ("fast path back to map mode");
            rc = SOURCE_mode (G_KEY_SPACE, a_minor);
         }
      }
   }
   /*---(check for backspace)------------*/
   else if (a_minor == G_KEY_BS) {
      if (x_quoting == 'y' && MODE_prev () != MODE_SOURCE) {
         rc = SRC_INPT__addone (x_mode, a_minor);
      } else {
         DEBUG_USER   yLOG_note    ("handle a backspace");
         if (s_cur->cpos > 0) {
            yvikeys_sundo_add ('d', 'h', s_cur->cpos - 1, s_cur->contents [s_cur->cpos - 1], G_KEY_NULL);
            rc = yvikeys_src_one_backspace ();
         }
      }
   }
   else if (a_minor == G_KEY_DEL) {
      if (x_quoting == 'y' && MODE_prev () != MODE_SOURCE) {
         rc = SRC_INPT__addone (x_mode, a_minor);
      } else {
         DEBUG_USER   yLOG_note    ("handle a delete");
         if (s_cur->cpos < s_cur->npos - 1) {
            ++s_cur->cpos;
            yvikeys_sundo_add ('d', 'l', s_cur->cpos, s_cur->contents [s_cur->cpos], G_KEY_NULL);
            rc = yvikeys_src_one_delete ();
            --s_cur->cpos;
         }
      }
   }
   /*---(wander mode)--------------------*/
   else if (a_minor == G_KEY_BTICK) {
      rc = MODE_enter (UMOD_WANDER);
      yvikeys_map_wander_prep ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle new character)-----------*/
   else if (a_minor == G_KEY_SPACE) {
      rc = SRC_INPT__addone (x_mode, G_CHAR_STORAGE);
   }
   else if ((a_minor > 32 && a_minor < 127) || (a_minor > 127 && a_minor < 256)) {
      rc = SRC_INPT__addone (x_mode, a_minor);
   }
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(check for history)--------------*/
   x_prevmode = MODE_prev ();
   sprintf (x_history, "%c%c%c¤"  , x_prevmode, x_prevmode, x_prevmode);
   if (strcmp (s_cur->contents, x_history) == 0) {
      rc = yvikeys_src_one_delete ();
      --s_cur->cpos;
      yvikeys_sundo_end (__FUNCTION__);
      SOURCE__done ();
      MODE_exit ();
      /*> if (x_prevmode == MODE_SEARCH)  strlcpy (s_cur->contents, "/", LEN_RECD);   <* 
       *> else                            strlcpy (s_cur->contents, ":", LEN_RECD);   <*/
      /*> SOURCE__done   ();                                                          <*/
      yvikeys_hist__bounds ();
      MODE_enter  (UMOD_HISTORY);
      myVIKEYS.cursor = '-';
      DEBUG_USER   yLOG_note    ("change to history mode");
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
   char        t           [LEN_RECD];
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRC unit         : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "position"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC position     : %3dw, %3da, %3db, %3dc, %3de", s_cur->wide, s_cur->apos, s_cur->bpos, s_cur->cpos, s_cur->epos);
   }
   else if (strcmp (a_question, "contents"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC contents     : %3d:%-.40s:", s_cur->npos, s_cur->contents);
   }
   else if (strcmp (a_question, "full"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC full         : %3d:%s:", s_cur->npos, s_cur->contents);
   }
   else if (strcmp (a_question, "words"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC words        : %3d:%-.40s:", s_cur->npos, s_cur->words);
   }
   else if (strcmp (a_question, "source"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC source       : %3d:%-.40s:", s_src.npos, s_src.contents);
   }
   else if (strcmp (a_question, "command"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC command      : %3d:%-.40s:", s_cmd.npos, s_cmd.contents);
   }
   else if (strcmp (a_question, "label"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC label        : %s", s_cur->label);
   }
   else if (strcmp (a_question, "undo"           )   == 0) {
      yvikeys_sundo_status (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SRC undo queue   : %-.50s", t);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




