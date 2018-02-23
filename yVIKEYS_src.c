/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char  s_label       [LEN_RECD]  = "";
static char  s_original    [LEN_RECD]  = "";
static char  s_contents    [LEN_RECD]  = "";
static int   s_wide    = 0;
static int   s_apos    = 0;
static int   s_npos    = 0;
static int   s_bpos    = 0;
static int   s_cpos    = 0;
static int   s_epos    = 0;
static char  s_words       [LEN_RECD]  = "";
static char  s_begs        [LEN_RECD]  = "";
static char  s_ends        [LEN_RECD]  = "";


#define     SELC_NOT       '-'
#define     SELC_YES       'y'
static char  s_live    = '-';
static int   s_root    = 0;
static int   s_bsel    = 0;
static int   s_esel    = 0;



#define     MAX_TREG     100
typedef struct  cTEXTREG  tTEXTREG;
struct cTEXTREG {
   char        label       [10];            /* original cell                  */
   int         bpos;                        /* original starting point        */
   int         epos;                        /* original ending point          */
   int         len;                         /* length of text                 */
   char        data        [LEN_RECD];       /* text                           */
   char        source;                      /* user, file, import             */
};
static      tTEXTREG    s_tregs  [MAX_TREG];
static      int         s_ntreg  = 32;
static char             s_ctreg  = '"';
static char             s_wtreg  = '"';


#define     TREG_NONE     '-'
#define     TREG_USER     'u'
#define     TREG_FILE     'f'
#define     TREG_IMPORT   'i'

static char    s_tregnames     [MAX_TREG] = "\"abcdefghijklmnopqrstuvwxyz+-";

static char    (*s_saver) (char *a_contents);

char         SRC_done           (void);
char         TREG_reset         (void);

char
SRC_accept              (void)
{
   s_npos  = s_bpos  = s_cpos  = s_epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   if (s_saver != NULL)  s_saver (s_contents);
   strlcpy (s_original, s_contents, LEN_RECD);
   return 0;
}

char
SRC_reset               (void)
{
   s_npos  = s_bpos  = s_cpos  = s_epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   strlcpy (s_contents, s_original, LEN_RECD);
   return 0;
}

char         /*-> initialize source environment ------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SRC_init                (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   s_saver  = NULL;
   SRC_reset ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initialize source environment ------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_src_config        (void *a_saver)
{
   if (a_saver != NULL)  s_saver  = a_saver;
   return 0;
}

char
SRC_formula                (void)
{
   /*---(locals)-----------+------+----+-*/
   int         x_left, x_bott;
   char        c           =   ' ';
   char        x_curr      =   ' ';
   char        x_beg       =     0;
   char        x_end       =     0;
   char        x_len       =     0;
   char        x_on        =   '-';
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_FORMULA, &x_left, &s_wide, &x_bott, NULL, NULL);
   if (x_on != 'y') {
      if (MODE_curr () == MODE_MAP)  return 0;
      yVIKEYS_view_size     (YVIKEYS_FLOAT  , &x_left, &s_wide, &x_bott, NULL, NULL);
   }
   s_apos = s_wide - 6;
   /*---(length)-------------------------*/
   attrset     (0);
   switch (MODE_curr ()) {
   case MODE_MAP    :   yCOLOR_curs ("map"    );  break;
   case MODE_SOURCE :   yCOLOR_curs ("source" );  x_curr = 'y';  break;
   case SMOD_TEXTREG:   yCOLOR_curs ("source" );  x_curr = 'y';  break;
   case SMOD_REPLACE:   yCOLOR_curs ("replace");  x_curr = 'y';  break;
   case MODE_INPUT  :   yCOLOR_curs ("input"  );  x_curr = 'y';  break;
   case SMOD_WANDER :   yCOLOR_curs ("wander" );  x_curr = 'y';  break;
   default          :   yCOLOR_curs ("map"    );  break;
   }
   mvprintw (x_bott, x_left, "%4d", s_npos);
   /*---(content)------------------------*/
   if (s_npos == 0)  mvprintw (x_bott, x_left + 5, "%-*.*s", s_apos, s_apos, " ");
   else              mvprintw (x_bott, x_left + 5, "%-*.*s", s_apos, s_apos, s_contents + s_bpos);
   /*---(selection)----------------------*/
   if (x_curr == 'y' && s_live == SELC_YES && s_npos > 0) {
      x_beg = s_bsel;
      x_end = s_esel;
      if (x_beg < s_bpos)  x_beg = s_bpos;
      if (x_end > s_epos)  x_end = s_epos;
      x_len = x_end - x_beg + 1;
      attrset  (0);
      yCOLOR_curs ("select" );
      mvprintw (x_bott, x_left + 5 + x_beg - s_bpos, "%-*.*s", x_len, x_len, s_contents + x_beg);
   }
   /*---(current)------------------------*/
   if (x_curr == 'y' && s_npos > 0) {
      attrset  (0);
      yCOLOR_curs ("map"    );
      mvprintw (x_bott, x_left + 5 + s_cpos - s_bpos, "%c", s_contents [s_cpos]);
   }
   /*---(markers)------------------------*/
   attrset  (0);
   yCOLOR_curs ("h_used");
   if (s_bpos > 0)             c = '<';
   else                        c = ']';
   mvprintw (x_bott, x_left + 4, "%c", c);
   if (s_epos != s_npos - 1)   c = '>';
   else                        c = '[';
   mvprintw (x_bott, x_left + s_wide - 1, "%c", c);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.C53.244.61]*/ /*-[02.0000.204.!]-*/ /*-[--.---.---.--]-*/
SRC_select_status       (char *a_list)
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
         x_len, x_len, s_contents + s_bsel);
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> prepare for source mode move -------[ leaf   [gz.412.001.00]*/ /*-[00.0000.313.!]-*/ /*-[--.---.---.--]-*/
SRC_prepare        (void)
{
   /*---(prepare)------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_info    ("s_contents", s_contents);
   s_npos     = strlen (s_contents);
   /*---(display debugging)--------------*/
   DEBUG_USER   yLOG_value   ("s_npos"   , s_npos);
   DEBUG_USER   yLOG_value   ("s_apos"   , s_apos);
   DEBUG_USER   yLOG_value   ("s_bpos"   , s_bpos);
   DEBUG_USER   yLOG_value   ("s_cpos"   , s_cpos);
   DEBUG_USER   yLOG_value   ("s_epos"   , s_epos);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SRC_words              (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char       *x_word      = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-";
   int         x_pos       =    0;
   int         x_not       =    0;
   int         x_yes       =    0;
   /*---(prepare)------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_value   ("s_npos"    , s_npos);
   DEBUG_USER   yLOG_info    ("s_contents", s_contents);
   /*> for (i = 0; i < s_npos; ++i)   s_words [i] = ' ';                              <*/
   for (i = 0; i < s_npos; ++i)   s_words [i] = s_contents [i];
   s_words [s_npos] = 0;
   if (strchr (x_word, s_contents [0]) != NULL)  s_words [0] = '<';
   while (x_pos < s_npos) {
      /*---(skip leading non-words)---------*/
      x_not = 0;
      DEBUG_USER   yLOG_note    ("non-words.............");
      for (x_pos = x_pos; x_pos < s_npos; ++x_pos) {
         if (s_contents [x_pos] == '\0')  break;
         DEBUG_USER   yLOG_value   ("x_pos"     , x_pos);
         DEBUG_USER   yLOG_value   ("s_contents", s_contents [x_pos]);
         if (strchr (x_word, s_contents [x_pos]) == NULL) {
            ++x_not;
            continue;
         }
         break;
      }
      if (x_pos >= s_npos)             break;
      if (s_contents [x_pos] == '\0')  break;
      if (x_not > 0)  {
         DEBUG_USER   yLOG_value   ("beg"       , x_pos);
         s_words [x_pos] = '<';
      }
      /*---(skip words-chars)--------------*/
      x_yes = 0;
      DEBUG_USER   yLOG_note    ("words.................");
      for (x_pos = x_pos; x_pos < s_npos; ++x_pos) {
         if (s_contents [x_pos] == '\0')  break;
         DEBUG_USER   yLOG_value   ("x_pos"     , x_pos);
         DEBUG_USER   yLOG_value   ("s_contents", s_contents [x_pos]);
         if (strchr (x_word, s_contents [x_pos]) != NULL) {
            ++x_yes;
            continue;
         }
         break;
      }
      if (s_contents [x_pos] == '\0') {
         DEBUG_USER   yLOG_value   ("end"       , x_pos);
         if (x_yes > 0)  s_words [x_pos - 1] = '>';
         break;
      }
      if (x_pos >= s_npos)             break;
      if (x_yes > 0) {
         DEBUG_USER   yLOG_value   ("end"       , x_pos);
         if (s_words [x_pos - 1] == '<')  s_words [x_pos - 1] = 'B';
         else                             s_words [x_pos - 1] = '>';
      }
   }
   s_words [s_npos] = 0;
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SRC_words_status       (char *a_list)
{
   sprintf (a_list, "word %s", s_words);
   return 0;
}

char         /*-> complete a source mode move --------[ leaf   [gz.E45.001.A0]*/ /*-[02.0000.513.!]-*/ /*-[--.---.---.--]-*/
SRC_done           (void)
{
   /*---(prepare)------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_info    ("s_contents", s_contents);
   s_npos     = strlen (s_contents);
   /*---(display debugging)--------------*/
   DEBUG_USER   yLOG_value   ("s_npos"   , s_npos);
   DEBUG_USER   yLOG_value   ("s_apos"   , s_apos);
   DEBUG_USER   yLOG_value   ("s_bpos"   , s_bpos);
   DEBUG_USER   yLOG_value   ("s_cpos"   , s_cpos);
   DEBUG_USER   yLOG_value   ("s_epos"   , s_epos);
   /*---(check over/underrun)---------*/
   DEBUG_USER   yLOG_note    ("correct s_npos over/underruns");
   if (s_npos  >= LEN_RECD) {
      s_npos = LEN_RECD - 1;
      s_contents [s_npos] = '\0';
   }
   if (s_npos < 0) {
      s_npos = 0;
   }
   /*---(check min/max)---------------*/
   DEBUG_USER   yLOG_note    ("correct min/max limits");
   if (s_cpos >=  s_npos)     s_cpos = s_npos - 1;
   if (s_cpos <   0      )    s_cpos = 0;
   if (s_bpos <   0      )    s_bpos = 0;
   /*---(small strings)---------------*/
   if (s_npos == 0) {
      DEBUG_USER   yLOG_note    ("check and handle small string");
      s_cpos = s_bpos = s_epos = 0;
   }
   /*---(small strings)---------------*/
   else if (s_npos <= s_apos) {
      DEBUG_USER   yLOG_note    ("check and handle small string");
      s_bpos = 0;
      s_epos = s_npos - 1;
   }
   /*---(long strings)----------------*/
   else {
      DEBUG_USER   yLOG_note    ("check and handle long string");
      s_epos = s_bpos + s_apos - 1;
      /*---(check off left side)------*/
      DEBUG_USER   yLOG_note    ("check off left side of screen");
      if (s_cpos <   s_bpos)  {
         s_bpos = s_cpos;
         s_epos = s_bpos + s_apos - 1;
      }
      /*---(check off right side)-----*/
      DEBUG_USER   yLOG_note    ("check off right side of screen");
      if (s_cpos >   s_epos)  {
         s_epos = s_cpos;
         s_bpos = s_epos - s_apos + 1;
      }
      /*---(check scrolling-----------*/
      DEBUG_USER   yLOG_note    ("make sure right is locked to end");
      if (s_epos >=  s_npos)  {
         s_epos = s_npos - 1;
         s_bpos = s_epos - s_apos + 1;
      }
   }
   /*---(selection update)---------------*/
   if (s_live != SELC_YES) {
      s_root = s_bsel = s_esel = s_cpos;
   } else if (s_cpos < s_root) {
      s_bsel = s_cpos;
      s_esel = s_root;
   } else {
      s_bsel = s_root;
      s_esel = s_cpos;
   }
   /*---(update word breaks)-------------*/
   SRC_words ();
   /*---(display debugging)--------------*/
   DEBUG_USER   yLOG_value   ("s_npos"   , s_npos);
   DEBUG_USER   yLOG_value   ("s_apos"   , s_apos);
   DEBUG_USER   yLOG_value   ("s_bpos"   , s_bpos);
   DEBUG_USER   yLOG_value   ("s_cpos"   , s_cpos);
   DEBUG_USER   yLOG_value   ("s_epos"   , s_epos);
   /*---(complete)--------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_source          (char *a_label, char *a_contents)
{
   if (a_label    == NULL)  strlcpy (s_label   , ""        , LEN_RECD);
   else                     strlcpy (s_label   , a_label   , LEN_RECD);
   if (a_contents == NULL)  strlcpy (s_original, ""        , LEN_RECD);
   else                     strlcpy (s_original, a_contents, LEN_RECD);
   strlcpy (s_contents, s_original, LEN_RECD);
   s_npos = strllen (s_contents, LEN_RECD);
   s_bpos = s_cpos = 0;
   s_epos = s_npos;
   SRC_done ();
   return 0;
}

char         /*-> prepare contents for new edit ------[ leaf   [gz.311.101.00]*/ /*-[00.0000.203.!]-*/ /*-[--.---.---.--]-*/
SRC_start          (char *a_prefix)
{
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   strlcpy (s_contents, a_prefix , LEN_RECD);
   s_npos = strllen (s_contents, LEN_RECD);
   s_bpos = s_cpos = 0;
   s_epos = s_npos;
   SRC_done ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SRC_simple         (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != ' ') {
      DEBUG_USER   yLOG_note    ("a_major was not empty");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER  yLOG_info    ("g_hsimple" , g_hsimple);
   --rce;  if (strchr (g_hsimple, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor was not a valid option");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare ();
   /*---(horizontal moves)---------------*/
   switch (a_minor) {
   case '0' : s_cpos  = 0;                    break;
   case 'H' : s_cpos -= 5;                    break;
   case 'h' : --s_cpos;                       break;
   case 'l' : ++s_cpos;                       break;
   case 'L' : s_cpos += 5;                    break;
   case '$' : s_cpos  = s_npos - 1;           break;
   }
   /*---(wrapup)-------------------------*/
   SRC_done  ();
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SRC_goto           (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'g') {
      DEBUG_USER   yLOG_note    ("a_major was not g (goto)");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER  yLOG_info    ("g_hgoto"  , g_hgoto);
   --rce;  if (strchr (g_hgoto, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor was not a valid option");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare ();
   /*---(goto moves)---------------------*/
   x_qtr = s_apos / 4;
   switch (a_minor) {
   case 'S' : s_cpos  = s_bpos - x_qtr * 4;  break;
   case 'H' : s_cpos  = s_bpos - x_qtr * 2;  break;
   case 's' : s_cpos  = s_bpos;              break;
   case 'h' : s_cpos  = s_bpos + x_qtr * 1;  break;
   case 'c' : s_cpos  = s_bpos + x_qtr * 2;  break;
   case 'l' : s_cpos  = s_bpos + x_qtr * 3;  break;
   case 'e' : s_cpos  = s_epos;              break;
   case 'L' : s_cpos  = s_bpos + x_qtr * 6;  break;
   case 'E' : s_cpos  = s_bpos + x_qtr * 8;  break;
   }
   /*---(wrapup)-------------------------*/
   SRC_done  ();
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SRC_scroll         (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   --rce;  if (a_major != 'z') {
      DEBUG_USER   yLOG_note    ("a_major was not z (scroll)");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER  yLOG_info    ("g_hscroll" , g_hscroll);
   --rce;  if (strchr (g_hscroll, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor was not a valid option");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare ();
   /*---(goto moves)---------------------*/
   x_qtr = s_apos / 4;
   switch (a_minor) {
   case 's' : s_bpos += s_cpos - (s_bpos + x_qtr * 0);  break;
   case 'h' : s_bpos += s_cpos - (s_bpos + x_qtr * 1);  break;
   case 'c' : s_bpos += s_cpos - (s_bpos + x_qtr * 2);  break;
   case 'l' : s_bpos += s_cpos - (s_bpos + x_qtr * 3);  break;
   case 'e' : s_bpos += s_cpos - (s_epos);              break;
   }
   /*---(wrapup)-------------------------*/
   SRC_done  ();
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}


char
SRC_word               (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char       *x_word      = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-";
   int         x_not       =    0;
   int         x_yes       =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_USER  yLOG_info    ("g_hword"   , g_hword);
   --rce;  if (strchr (g_hword, a_minor) == 0) {
      DEBUG_USER   yLOG_note    ("a_minor was not a valid option");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare ();
   /*---(words)--------------------------*/
   if (strchr ("wW", a_minor) != 0) {
      for (i = s_cpos + 1; i < s_npos; ++i) {
         if (strchr ("<B", s_words [i]) == NULL)  continue;
         s_cpos = i;
         break;
      }
   }
   /*---(ends)---------------------------*/
   if (strchr ("eE", a_minor) != 0) {
      for (i = s_cpos + 1; i < s_npos; ++i) {
         if (strchr (">B", s_words [i]) == NULL)  continue;
         s_cpos = i;
         break;
      }
   }
   /*---(beginnings)---------------------*/
   if (strchr ("bB", a_minor) != 0) {
      for (i = s_cpos - 1; i >= 0; --i) {
         if (strchr ("<B", s_words [i]) == NULL)  continue;
         s_cpos = i;
         break;
      }
   }
   /*---(wrapup)-------------------------*/
   SRC_done  ();
   /*---(complete)--------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SRC_charfindrev    (uchar a_ch)
{
   int         i           =    0;
   for (i = s_cpos - 1; i >= 0; --i) {
      if (s_contents [i] != a_ch)  continue;
      s_cpos = i;
      SRC_done ();
      return 0;
   }
   return -1;
}

char
SRC_charfind       (uchar a_ch)
{
   int         i           =    0;
   for (i = s_cpos + 1; i < s_npos; ++i) {
      if (s_contents [i] != a_ch)  continue;
      s_cpos = i;
      SRC_done ();
      return 0;
   }
   return -1;
}

char         /*-> process keys for cell edit mode ----[ leaf   [ge.#R6.23#.HG]*/ /*-[05.0000.102.#]-*/ /*-[--.---.---.--]-*/
SRC_mode      (char a_major, char a_minor)
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
   static char x_char      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_SOURCE )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(universal)----------------------*/
   switch (a_minor) {
   case G_KEY_SPACE  :
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return G_KEY_SPACE;
   case G_KEY_ESCAPE :
      if (s_live == SELC_YES) {
         DEBUG_USER   yLOG_note    ("escape, means get out of selection");
         TREG_reset ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;   /* escape  */
      }
      DEBUG_USER   yLOG_note    ("escape, means forget and return to previous mode");
      SRC_reset  ();
      SRC_done   ();
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   case G_KEY_RETURN :
      DEBUG_USER   yLOG_note    ("enter, means save and return to previous mode");
      SRC_accept ();
      SRC_done   ();
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single letter)------------------*/
   if (a_major == ' ') {
      /*---(repeats)---------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         MODE_enter  (SMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr (g_multisrc, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("captured a multikey prefix a_minor");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(mode changes)----------------*/
      switch (a_minor) {
      case  'v' :
         if (s_live == SELC_YES) {
            DEBUG_USER   yLOG_note    ("flip selection ends");
            if      (s_root == s_bsel) { s_root = s_esel; s_cpos = s_bsel; }
            else if (s_root == s_esel) { s_root = s_bsel; s_cpos = s_esel; }
         } else {
            DEBUG_USER   yLOG_note    ("start selection of text");
            s_live = SELC_YES;
            s_root = s_cpos;
         }
         rc     = 0;
         break;
      case  'f' : case  'F' :
         DEBUG_USER   yLOG_note    ("char find mode");
         return a_minor;
         break;
      case  '#' :
         DEBUG_USER   yLOG_note    ("current char find");
         x_char = s_contents [s_cpos];
         rc = SRC_charfind (x_char);
         break;
      case  'n' :
         DEBUG_USER   yLOG_note    ("char find next");
         rc = SRC_charfind (x_char);
         break;
      case  'N' :
         DEBUG_USER   yLOG_note    ("char find reverse");
         rc = SRC_charfindrev (x_char);
         break;
      case  '"' :
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         MODE_enter (SMOD_TEXTREG);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'r' : case  'R' :
         DEBUG_USER   yLOG_note    ("enter replace mode");
         MODE_enter (SMOD_REPLACE);
         REPL_smode ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'i' : case  'a' :
         DEBUG_USER   yLOG_note    ("enter input mode");
         MODE_enter (MODE_INPUT);
         INPT_mode  ('m', a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      }
      /*---(cut/copy/paste)--------------*/
      if (strchr ("yxdpP", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         s_ctreg = '"';
         MODE_enter (SMOD_TEXTREG);
         rc = TREG_smode (' ', a_minor);
      }
      /*---(basic movement)--------------*/
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = SRC_simple  (a_major, a_minor);
      }
      /*---(word movement)---------------*/
      if (strchr (g_hword, a_minor) != 0) {
         rc = SRC_word    (a_major, a_minor);
      }
      /*---(column movement)-------------*/
      if (a_minor == '|') {
         s_cpos = REPEAT_use ();
         rc     = 0;
         SRC_done ();
      }
   }
   /*---(multi-key)----------------------*/
   else {
      switch (a_major) {
      case 'g' :  rc = SRC_goto     (a_major, a_minor); break;
      case 'z' :  rc = SRC_scroll   (a_major, a_minor); break;
      case 'f' :  x_char = a_minor;
                  rc = SRC_charfind    (x_char);
                  break;
      case 'F' :  x_char = a_minor;
                  rc = SRC_charfindrev (x_char);
                  break;
      default  :  rc = -1;                              break;
      }
   }
   /*---(complete)-----------------------*/
   if (rc < 0) {
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

char         /*-> clear all selections ---------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
TREG_init          (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
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
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> go back to defaults ----------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
TREG_reset         (void)
{
   s_live  = SELC_NOT;
   s_root  = s_esel  = s_bsel  = s_cpos;
   s_ctreg = '"';
   return 0;
}

char
TREG_index         (cchar a_reg)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < s_ntreg; ++i) {
      if (s_tregnames [i] == a_reg)  return  i;
   }
   return -1;
}

char         /*-> tbd --------------------------------[ ------ [ge.C53.244.61]*/ /*-[02.0000.204.!]-*/ /*-[--.---.---.--]-*/
TREG_status             (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
   int         x_len       = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(buffer number)------------------*/
   x_reg  = TREG_index  (s_wtreg);
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

char         /*-> process keys for register actions --[ ------ [gz.320.011.02]*/ /*-[01.0000.113.!]-*/ /*-[--.---.---.--]-*/
TREG_clear         (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =   0;
   /*---(clear)--------------------------*/
   for (i = s_bsel; i <= s_esel; ++i) {
      s_contents [i] = ' ';
   }
   /*---(reposition)---------------------*/
   if (a_dir == 'x')  s_cpos += s_esel - s_bsel + 1;
   else               s_cpos == s_bsel - 1;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register action ---[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
TREG_delete        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_diff      =   0;
   int         i           =   0;
   /*---(set size)-----------------------*/
   x_diff  = s_esel - s_bsel + 1;
   /*---(delete)-------------------------*/
   for (i = s_bsel; i < s_npos; ++i) {
      s_contents [i] = s_contents [i + x_diff];
      if (i + x_diff >= s_npos)  break;
   }
   s_cpos = s_bsel;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [ge.640.051.04]*/ /*-[00.0000.213.!]-*/ /*-[--.---.---.--]-*/
TREG_copy          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_label     [10]        = "";
   char        rce         = -10;
   /*---(identify register)--------------*/
   x_index = TREG_index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set size)-----------------------*/
   x_start = s_contents + s_bsel;
   x_len   = s_esel - s_bsel + 1;
   /*---(copy)---------------------------*/
   strlcpy (s_tregs [x_index].data, x_start, x_len + 1);
   /*---(fill in details)----------------*/
   s_tregs [x_index].len    = x_len;
   s_tregs [x_index].bpos   = s_bsel;
   s_tregs [x_index].epos   = s_esel;
   strlcpy (s_tregs [x_index].label, s_label, 10);
   s_tregs [x_index].source = TREG_USER;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [gz.740.061.21]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
TREG_replace       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   char        x_null      = '-';
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = TREG_index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set the start)------------------*/
   x_start = s_cpos;
   /*---(open)---------------------------*/
   x_gap   = s_tregs [x_index].len;
   if (x_start + x_gap >= s_npos)  x_null = 'y';
   /*---(open)---------------------------*/
   s_npos   = s_tregs [x_index].len;
   for (i  = 0; i < s_npos; ++i) {
      s_contents [i + x_start] = s_tregs [x_index].data [i];
   }
   /*---(final null)---------------------*/
   if (x_null == 'y') {
      s_contents [x_start + x_gap + 1] == '\0';
   }
   /*---(reposition)---------------------*/
   s_cpos += s_tregs [x_index].len - 1;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [gz.640.151.11]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
TREG_paste         (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = TREG_index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set the start)------------------*/
   x_start = s_cpos;
   if (a_dir == '>')   ++x_start;
   /*---(open)---------------------------*/
   x_gap   = s_tregs [x_index].len;
   for (i = s_npos; i >= x_start; --i) {
      s_contents [i + x_gap] = s_contents [i];
      s_contents [i]         = G_CHAR_PLACE;
   }
   /*---(fill)---------------------------*/
   s_npos   = s_tregs [x_index].len;
   for (i  = 0; i < s_npos; ++i) {
      s_contents [i + x_start] = s_tregs [x_index].data [i];
   }
   /*---(reposition)---------------------*/
   s_cpos += s_tregs [x_index].len - 1;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ leaf   [ge.QG5.287.FB]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
TREG_smode              (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
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
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_TEXTREG )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(escape)-------------------------*/
   if (a_minor == G_KEY_ESCAPE)  {
      DEBUG_USER   yLOG_note    ("escape and return to previous mode");
      TREG_reset ();
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for selection)------------*/
   --rce;  if (a_major == '"') {
      if (strchr (s_tregnames, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("select a text register");
         s_ctreg = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*> else if (a_minor == '?') {                                                  <* 
       *>    DEBUG_USER   yLOG_note    ("show text register inventory");              <* 
       *>    my.info_win  = G_INFO_TREG;                                              <* 
       *>    s_ctreg = '"';                                                            <* 
       *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
       *>    return  0;                                                               <* 
       *> }                                                                           <*/
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_major == ' ') {
      switch (a_minor) {
      case  '!' :
         s_wtreg = s_ctreg;
         yVIKEYS_cmds_direct (":status treg");
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      case  '#' :
         DEBUG_USER   yLOG_note    ("wipe text register");
         x_index = TREG_index (s_ctreg);
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
         DEBUG_USER   yLOG_note    ("yank selection text");
         TREG_copy   ();
         MODE_exit   ();
         break;
      case  'x' :
         DEBUG_USER   yLOG_note    ("clear selection text");
         TREG_copy   ();
         TREG_clear  (a_minor);
         MODE_exit   ();
         SRC_done    ();
         break;
      case  'd' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_delete ();
         MODE_exit   ();
         SRC_done    ();
         break;
      case  'r' :
         DEBUG_USER   yLOG_note    ("replace selection text");
         TREG_replace();
         MODE_exit   ();
         SRC_done    ();
         break;
      case  's' :
         DEBUG_USER   yLOG_note    ("substitute selection text");
         TREG_delete ();
         TREG_paste  ('<');
         MODE_exit   ();
         SRC_done    ();
         break;
      case  'p' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         TREG_paste  ('>');
         ++s_cpos;
         MODE_exit   ();
         SRC_done    ();
         break;
      case  'P' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         TREG_paste  ('<');
         MODE_exit   ();
         SRC_done    ();
         break;
         /*> case  'g' :                                                                  <* 
          *>    DEBUG_USER   yLOG_note    ("go to beginning selection position");         <* 
          *>    x_index = REG__reg2index (s_ctreg);                                        <* 
          *>    s_cpos = s_tregs [x_index].bpos;                                       <* 
          *>    SRC_done ();                                                             <* 
          *>    MODE_exit ();                                                             <* 
          *>    break;                                                                    <*/
         /*> case  'G' :                                                                  <* 
          *>    DEBUG_USER   yLOG_note    ("go to ending selection position");            <* 
          *>    x_index = REG__reg2index (s_ctreg);                                        <* 
          *>    s_cpos = s_tregs [x_index].epos;                                       <* 
          *>    SRC_done ();                                                             <* 
          *>    MODE_exit ();                                                             <* 
          *>    break;                                                                    <*/
      }
   }
   TREG_reset ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> replace sub-mode -------------------[ ------ [ge.RG6.25#.E5]*/ /*-[02.0000.112.E]-*/ /*-[--.---.---.--]-*/
REPL_smode    (int a_major, int a_minor)
{  /*---(design notes)-------------------*/
   /*
    *   very limited sub-mode of SOURCE mode.  allows a user to "type over"
    *   existing text including past the end of the line.  there are no
    *   special keys except ESCAPE to return to SOURCE mode.
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_majors    [LEN_RECD]  = "rRm\\";
   static char x_append    = '-';
   static char x_saved = '\0';
   static char x_prev  = '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_REPLACE)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare   ();
   /*---(escaped chars)------------------*/
   if (a_minor == '\\' && x_prev != '\\') {
      DEBUG_USER   yLOG_note    ("found a leading backslash");
      x_prev = '\\';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   if (x_prev == '\\') {
      DEBUG_USER   yLOG_note    ("converting backsplash character");
      x_prev = '-';
      a_minor = chrslashed (a_minor);
      DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      if (x_append == 'y') {
         s_contents [s_cpos] = '\0';
      }
      if (x_saved != '\0') {
         s_contents [s_cpos] = x_saved;
      }
      x_append = '-';
      SRC_done   ();
      MODE_exit ();
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && MODE_curr () == MODE_SOURCE) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         SRC_mode (' ', a_minor);
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle keys)--------------------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , s_cpos);
   DEBUG_USER   yLOG_char    ("curr char" , s_contents [s_cpos]);
   /*---(check for backspace)------------*/
   if (a_major == 'R' && (a_minor == G_KEY_DEL || a_minor == G_KEY_BS)) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (s_cpos > 0) {
         s_contents [s_cpos] = x_saved;
         --(s_cpos);
         x_saved = s_contents [s_cpos];
         s_contents [s_cpos] = G_CHAR_PLACE;
      }
      SRC_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(check for mark)-----------------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      x_saved = s_contents [s_cpos];
      s_contents [s_cpos] = G_CHAR_PLACE;
      if (x_saved == NULL)  s_contents [s_cpos + 1] = NULL;
      SRC_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle normal chars)------------*/
   if (a_major == 'r') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      s_contents [s_cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , s_contents [s_cpos]);
      MODE_exit ();
      SRC_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_major == 'R') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      s_contents [s_cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , s_contents [s_cpos]);
      ++(s_cpos);
      x_saved = s_contents [s_cpos];
      s_contents [s_cpos] = G_CHAR_PLACE;
   }
   /*---(correct current position)-------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , s_cpos);
   DEBUG_USER   yLOG_value   ("curr end"  , s_npos);
   if (s_cpos  >= s_npos) {
      DEBUG_USER   yLOG_note    ("update the end pos");
      s_contents [s_npos    ] = G_CHAR_PLACE;
      s_contents [s_npos + 1] = '\0';
      x_append = 'y';
   }
   /*---(wrap up)------------------------*/
   SRC_done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}

char         /*-> process keys for input mode --------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
INPT_mode          (int  a_major, int  a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate a very basic vi-input mode by handling
    *   all characters, ignoring new line, and popping out with escape
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_majors    [LEN_RECD]   = "IiaAm\\";
   int         i           = 0;             /* loop iterator                  */
   static char x_prev      = '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_INPUT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_USER   yLOG_info    ("x_majors"   , x_majors);
   --rce;  if (strchr (x_majors, a_major) == 0) {
      DEBUG_USER   yLOG_note    ("a_major is not valid");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   SRC_prepare ();
   /*---(check for initial mark)---------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      if (a_minor == 'a' && s_contents [s_cpos] != '\0')  ++(s_cpos);
      DEBUG_USER   yLOG_value   ("total pos" , s_npos);
      DEBUG_USER   yLOG_value   ("new pos"   , s_cpos);
      for (i = s_npos; i >= s_cpos; --i)  s_contents[i + 1] = s_contents[i];
      s_contents [s_cpos] = G_CHAR_PLACE;
      SRC_done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_minor;
   }
   /*---(escaped chars)------------------*/
   if (a_minor == '\\' && x_prev != '\\') {
      DEBUG_USER   yLOG_note    ("found a leading backslash");
      x_prev = '\\';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   if (x_prev == '\\') {
      DEBUG_USER   yLOG_note    ("converting backsplash character");
      x_prev = '-';
      a_minor = chrslashed (a_minor);
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("escape/return, return to source mode");
      for (i = s_cpos; i <= s_npos; ++i)  s_contents[i] = s_contents[i + 1];
      if (a_major == 'a')  --(s_cpos);
      SRC_done   ();
      MODE_exit ();
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && MODE_curr () == MODE_SOURCE) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         SRC_mode (' ', a_minor);
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for backspace)------------*/
   if (a_minor == G_KEY_DEL || a_minor == G_KEY_BS) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (s_cpos > 0) {
         --(s_cpos);
         DEBUG_USER   yLOG_value   ("curr pos"  , s_cpos);
         for (i = s_cpos; i <= s_npos; ++i)  s_contents[i] = s_contents[i + 1];
         SRC_done   ();
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(handle new character)-----------*/
   DEBUG_USER   yLOG_note    ("move remaining chars to the right");
   for (i = s_npos; i >= s_cpos; --i)  s_contents[i + 1] = s_contents[i];
   DEBUG_USER   yLOG_note    ("add the character");
   s_contents [s_cpos] = a_minor;
   ++(s_cpos);
   /*---(wrap up)------------------------*/
   SRC_done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}




char*        /*-> unit test accessor -----------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
SRC__unit               (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_index     =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRC unit         : question not understood", LEN_STR);
   x_index = TREG_index (a_reg);
   if (x_index < 0)  x_index = 0;
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "position"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC position     : %3dw, %3da, %3db, %3dc, %3de", s_wide, s_apos, s_bpos, s_cpos, s_epos);
   }
   else if (strcmp (a_question, "contents"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC contents     : %3d:%-.40s:", s_npos, s_contents);
   }
   else if (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC selection    :   %cl, %3dr, %3db, %3dc, %3de", s_live, s_root, s_bsel, s_cpos, s_esel);
   }
   else if (strcmp (a_question, "textreg"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC textreg      : %2d %3d:%-20.20s: %3d %3d %c %s", x_index, s_tregs [x_index].len, s_tregs [x_index].data, s_tregs [x_index].bpos, s_tregs [x_index].epos, s_tregs [x_index].source, s_tregs [x_index].label);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




