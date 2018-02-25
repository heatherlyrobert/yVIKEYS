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



/*> static char  s_label       [LEN_RECD]  = "-";                                     <* 
 *> static char  s_original    [LEN_RECD]  = "";                                      <* 
 *> static char  s_contents    [LEN_RECD]  = "";                                      <* 
 *> static int   s_wide    = 0;                                                       <* 
 *> static int   s_apos    = 0;                                                       <* 
 *> static int   s_npos    = 0;                                                       <* 
 *> static int   s_bpos    = 0;                                                       <* 
 *> static int   s_cpos    = 0;                                                       <* 
 *> static int   s_epos    = 0;                                                       <* 
 *> static char  s_words       [LEN_RECD]  = "";                                      <*/


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



/*====================------------------------------------====================*/
/*===----                       support functions                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char         /*-> accept all source changes ----------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE__accept          (void)
{
   s_cur->npos  = s_cur->bpos  = s_cur->cpos  = s_cur->epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   switch (MODE_curr ()) {
   case MODE_SOURCE  :
      if (s_saver != NULL)  s_saver (s_cur->contents);
      break;
   case MODE_COMMAND :
      yVIKEYS_cmds_direct (s_cur->contents);
      break;
   case MODE_SEARCH  :
      yVIKEYS_srch_direct (s_cur->contents);
      break;
   }
   strlcpy (s_cur->original, s_cur->contents, LEN_RECD);
   return 0;
}

char         /*-> reject all source changes ----------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
SOURCE__reset           (void)
{
   s_cur->npos  = s_cur->bpos  = s_cur->cpos  = s_cur->epos  = 0;
   s_root  = s_bsel  = s_esel  = 0;
   s_live  = SELC_NOT;
   s_ctreg = s_wtreg = '"';
   strlcpy (s_cur->contents, s_cur->original, LEN_RECD);
   return 0;
}

char         /*-> prepare for source mode move -------[ leaf   [gz.412.001.00]*/ /*-[00.0000.313.!]-*/ /*-[--.---.---.--]-*/
SOURCE__prep            (void)
{
   /*---(prepare)------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_info    ("contents"  , s_cur->contents);
   s_cur->npos     = strlen (s_cur->contents);
   /*---(display debugging)--------------*/
   DEBUG_USER   yLOG_value   ("npos"     , s_cur->npos);
   DEBUG_USER   yLOG_value   ("apos"     , s_cur->apos);
   DEBUG_USER   yLOG_value   ("bpos"     , s_cur->bpos);
   DEBUG_USER   yLOG_value   ("cpos"     , s_cur->cpos);
   DEBUG_USER   yLOG_value   ("epos"     , s_cur->epos);
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
SOURCE__words          (void)
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
   DEBUG_USER   yLOG_value   ("s_npos"    , s_cur->npos);
   DEBUG_USER   yLOG_info    ("s_contents", s_cur->contents);
   for (i = 0; i < s_cur->npos; ++i)   s_cur->words [i] = s_cur->contents [i];
   s_cur->words [s_cur->npos] = 0;
   if (strchr (x_word, s_cur->contents [0]) != NULL)  s_cur->words [0] = '<';
   while (x_pos < s_cur->npos) {
      /*---(skip leading non-words)---------*/
      x_not = 0;
      DEBUG_USER   yLOG_note    ("non-words.............");
      for (x_pos = x_pos; x_pos < s_cur->npos; ++x_pos) {
         if (s_cur->contents [x_pos] == '\0')  break;
         DEBUG_USER   yLOG_value   ("x_pos"     , x_pos);
         DEBUG_USER   yLOG_value   ("s_contents", s_cur->contents [x_pos]);
         if (strchr (x_word, s_cur->contents [x_pos]) == NULL) {
            ++x_not;
            continue;
         }
         break;
      }
      if (x_pos >= s_cur->npos)             break;
      if (s_cur->contents [x_pos] == '\0')  break;
      if (x_not > 0)  {
         DEBUG_USER   yLOG_value   ("beg"       , x_pos);
         s_cur->words [x_pos] = '<';
      }
      /*---(skip words-chars)--------------*/
      x_yes = 0;
      DEBUG_USER   yLOG_note    ("words.................");
      for (x_pos = x_pos; x_pos < s_cur->npos; ++x_pos) {
         if (s_cur->contents [x_pos] == '\0')  break;
         DEBUG_USER   yLOG_value   ("x_pos"     , x_pos);
         DEBUG_USER   yLOG_value   ("s_contents", s_cur->contents [x_pos]);
         if (strchr (x_word, s_cur->contents [x_pos]) != NULL) {
            ++x_yes;
            continue;
         }
         break;
      }
      if (s_cur->contents [x_pos] == '\0') {
         DEBUG_USER   yLOG_value   ("end"       , x_pos);
         if (x_yes > 0)  s_cur->words [x_pos - 1] = '>';
         break;
      }
      if (x_pos >= s_cur->npos)             break;
      if (x_yes > 0) {
         DEBUG_USER   yLOG_value   ("end"       , x_pos);
         if (s_cur->words [x_pos - 1] == '<')  s_cur->words [x_pos - 1] = 'B';
         else                             s_cur->words [x_pos - 1] = '>';
      }
   }
   s_cur->words [s_cur->npos] = 0;
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> complete a source mode move --------[ leaf   [gz.E45.001.A0]*/ /*-[02.0000.513.!]-*/ /*-[--.---.---.--]-*/
SOURCE__done            (void)
{
   /*---(prepare)------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_info    ("s_contents", s_cur->contents);
   s_cur->npos     = strlen (s_cur->contents);
   /*---(display debugging)--------------*/
   DEBUG_USER   yLOG_value   ("s_npos"   , s_cur->npos);
   DEBUG_USER   yLOG_value   ("s_apos"   , s_cur->apos);
   DEBUG_USER   yLOG_value   ("s_bpos"   , s_cur->bpos);
   DEBUG_USER   yLOG_value   ("s_cpos"   , s_cur->cpos);
   DEBUG_USER   yLOG_value   ("s_epos"   , s_cur->epos);
   /*---(check over/underrun)---------*/
   DEBUG_USER   yLOG_note    ("correct s_npos over/underruns");
   if (s_cur->npos  >= LEN_RECD) {
      s_cur->npos = LEN_RECD - 1;
      s_cur->contents [s_cur->npos] = '\0';
   }
   if (s_cur->npos < 0) {
      s_cur->npos = 0;
   }
   /*---(check min/max)---------------*/
   DEBUG_USER   yLOG_note    ("correct min/max limits");
   if (s_cur->cpos >=  s_cur->npos)     s_cur->cpos = s_cur->npos - 1;
   if (s_cur->cpos <   0      )    s_cur->cpos = 0;
   if (s_cur->bpos <   0      )    s_cur->bpos = 0;
   /*---(small strings)---------------*/
   if (s_cur->npos == 0) {
      DEBUG_USER   yLOG_note    ("check and handle small string");
      s_cur->cpos = s_cur->bpos = s_cur->epos = 0;
   }
   /*---(small strings)---------------*/
   else if (s_cur->npos <= s_cur->apos) {
      DEBUG_USER   yLOG_note    ("check and handle small string");
      s_cur->bpos = 0;
      s_cur->epos = s_cur->npos - 1;
   }
   /*---(long strings)----------------*/
   else {
      DEBUG_USER   yLOG_note    ("check and handle long string");
      s_cur->epos = s_cur->bpos + s_cur->apos - 1;
      /*---(check off left side)------*/
      DEBUG_USER   yLOG_note    ("check off left side of screen");
      if (s_cur->cpos <   s_cur->bpos)  {
         s_cur->bpos = s_cur->cpos;
         s_cur->epos = s_cur->bpos + s_cur->apos - 1;
      }
      /*---(check off right side)-----*/
      DEBUG_USER   yLOG_note    ("check off right side of screen");
      if (s_cur->cpos >   s_cur->epos)  {
         s_cur->epos = s_cur->cpos;
         s_cur->bpos = s_cur->epos - s_cur->apos + 1;
      }
      /*---(check scrolling-----------*/
      DEBUG_USER   yLOG_note    ("make sure right is locked to end");
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
   DEBUG_USER   yLOG_value   ("s_npos"   , s_cur->npos);
   DEBUG_USER   yLOG_value   ("s_apos"   , s_cur->apos);
   DEBUG_USER   yLOG_value   ("s_bpos"   , s_cur->bpos);
   DEBUG_USER   yLOG_value   ("s_cpos"   , s_cur->cpos);
   DEBUG_USER   yLOG_value   ("s_epos"   , s_cur->epos);
   /*---(complete)--------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
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
   default  :
      s_cur = &s_src;
      MODE_enter  (MODE_SOURCE );
      break;
   }
   /*---(populate globals)---------------*/
   strlcpy (s_cur->contents, a_prefix , LEN_RECD);
   s_cur->npos = strllen (s_cur->contents, LEN_RECD);
   s_cur->bpos = s_cur->cpos = 0;
   s_cur->epos = s_cur->npos;
   SOURCE__done ();
   /*---(go info input)------------------*/
   MODE_enter  (SMOD_INPUT  );
   INPUT_smode  ('m', 'a');
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
TEXTREG__reset         (void)
{
   s_live  = SELC_NOT;
   s_root  = s_esel  = s_bsel  = s_cur->cpos;
   s_ctreg = '"';
   return 0;
}

char
TEXTREG__index         (cchar a_reg)
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
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   DEBUG_PROG   yLOG_note    ("clear s_saver pointer");
   s_saver  = NULL;
   DEBUG_PROG   yLOG_note    ("clear command/search values");
   s_cur = &s_cmd;
   s_cur->type  = EDIT_CMDS;
   strlcpy (s_cur->original, "" , LEN_RECD );
   strlcpy (s_cur->label   , "-", LEN_LABEL);
   SOURCE__reset ();
   DEBUG_PROG   yLOG_note    ("clear source values");
   s_cur = &s_src;
   s_cur->type  = EDIT_NORM;
   strlcpy (s_cur->original, "" , LEN_RECD );
   strlcpy (s_cur->label   , "-", LEN_LABEL);
   SOURCE__reset ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
TEXTREG_init       (void)
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

char         /*-> initialize source environment ------[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_src_config        (void *a_saver)
{
   if (a_saver != NULL)  s_saver  = a_saver;
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
SOURCE_formula             (void)
{
   /*---(locals)-----------+------+----+-*/
   int         x_left, x_bott;
   char        c           =   ' ';
   char        x_edit      =   ' ';
   char        x_beg       =     0;
   char        x_end       =     0;
   char        x_len       =     0;
   char        x_on        =   '-';
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_FORMULA, &x_left, &s_src.wide, &x_bott, NULL, NULL);
   if (x_on != 'y') {
      if (MODE_prev () != MODE_SOURCE)                   return 0;
      if (strchr (MODES_EDITING, MODE_curr ()) == NULL)  return 0;
      yVIKEYS_view_size     (YVIKEYS_FLOAT  , &x_left, &s_src.wide, &x_bott, NULL, NULL);
   }
   s_src.apos = s_src.wide - 6;
   /*---(base color)---------------------*/
   attrset     (0);
   if (MODE_curr () == MODE_SOURCE) {
      yCOLOR_curs ("source" );
      x_edit = 'y';
   } else if (MODE_prev () == MODE_SOURCE) {
      x_edit = 'y';
      switch (MODE_curr ()) {
      case SMOD_TEXTREG:   yCOLOR_curs ("textreg");  break;
      case SMOD_REPLACE:   yCOLOR_curs ("replace");  break;
      case SMOD_INPUT  :   yCOLOR_curs ("input"  );  break;
      case SMOD_WANDER :   yCOLOR_curs ("wander" );  break;
      default          :   yCOLOR_curs ("map"    );  break;
      }
   } else {
      yCOLOR_curs ("map"    );
   }
   /*---(length)-------------------------*/
   mvprintw (x_bott, x_left, "%4d", s_src.npos);
   /*---(base content)-------------------*/
   if (s_src.npos == 0)  mvprintw (x_bott, x_left + 5, "%-*.*s", s_src.apos, s_src.apos, " ");
   else                  mvprintw (x_bott, x_left + 5, "%-*.*s", s_src.apos, s_src.apos, s_src.contents + s_src.bpos);
   /*---(selection)----------------------*/
   if (x_edit == 'y' && s_live == SELC_YES && s_src.npos > 0) {
      x_beg = s_bsel;
      x_end = s_esel;
      if (x_beg < s_src.bpos)  x_beg = s_src.bpos;
      if (x_end > s_src.epos)  x_end = s_src.epos;
      x_len = x_end - x_beg + 1;
      attrset  (0);
      yCOLOR_curs ("select" );
      mvprintw (x_bott, x_left + 5 + x_beg - s_src.bpos, "%-*.*s", x_len, x_len, s_src.contents + x_beg);
   }
   /*---(current)------------------------*/
   if (x_edit == 'y' && s_src.npos > 0) {
      attrset  (0);
      yCOLOR_curs ("map"    );
      mvprintw (x_bott, x_left + 5 + s_src.cpos - s_src.bpos, "%c", s_src.contents [s_src.cpos]);
   }
   /*---(markers)------------------------*/
   attrset  (0);
   yCOLOR_curs ("h_used");
   if      (s_src.npos == 0)                c = ' ';
   else if (s_src.bpos > 0)                 c = '<';
   else                                     c = ']';
   mvprintw (x_bott, x_left + 4, "%c", c);
   if      (s_src.npos == 0)                c = ' ';
   else if (s_src.epos != s_src.npos - 1)   c = '>';
   else                                     c = '[';
   mvprintw (x_bott, x_left + s_src.wide - 1, "%c", c);
   /*---(complete)-----------------------*/
   return 0;
}

char
SOURCE_command             (void)
{
   /*---(locals)-----------+------+----+-*/
   int         x_left, x_bott;
   char        c           =   ' ';
   char        x_edit      =   ' ';
   char        x_beg       =     0;
   char        x_end       =     0;
   char        x_len       =     0;
   char        x_on        =   '-';
   /*---(get sizes)----------------------*/
   x_on = yVIKEYS_view_size     (YVIKEYS_COMMAND, &x_left, &s_cmd.wide, &x_bott, NULL, NULL);
   if (x_on != 'y') {
      if (strchr (":/", MODE_prev ()) == NULL)           return 0;
      if (strchr (MODES_EDITING, MODE_curr ()) == NULL)  return 0;
      yVIKEYS_view_size     (YVIKEYS_FLOAT  , &x_left, &s_cmd.wide, &x_bott, NULL, NULL);
   }
   s_cmd.apos = s_cmd.wide - 4;
   /*---(base color)---------------------*/
   attrset     (0);
   if (strchr (":/", MODE_curr ()) != NULL)  {
      yCOLOR_curs ("source" );
      x_edit = 'y';
   } else if (strchr (":/", MODE_prev ()) != NULL) {
      x_edit = 'y';
      switch (MODE_curr ()) {
      case SMOD_TEXTREG:   yCOLOR_curs ("source" );  break;
      case SMOD_REPLACE:   yCOLOR_curs ("replace");  break;
      case SMOD_INPUT  :   yCOLOR_curs ("input"  );  break;
      case SMOD_WANDER :   yCOLOR_curs ("wander" );  break;
      default          :   yCOLOR_curs ("command");  break;
      }
   } else {
      yCOLOR_curs ("command");
      mvprintw (x_bott, x_left, "%-*.*s", s_cmd.wide, s_cmd.wide, " ");
      attrset (0);
      return 0;
   }
   /*---(length)-------------------------*/
   mvprintw (x_bott, x_left, "%4d", s_cmd.npos);
   /*---(base content)-------------------*/
   if (s_cmd.npos == 0)  mvprintw (x_bott, x_left + 5, "%-*.*s", s_cmd.apos, s_cmd.apos, " ");
   else                  mvprintw (x_bott, x_left + 5, "%-*.*s", s_cmd.apos, s_cmd.apos, s_cmd.contents + s_cmd.bpos);
   /*---(selection)----------------------*/
   if (x_edit == 'y' && s_live == SELC_YES && s_cmd.npos > 0) {
      x_beg = s_bsel;
      x_end = s_esel;
      if (x_beg < s_cmd.bpos)  x_beg = s_cmd.bpos;
      if (x_end > s_cmd.epos)  x_end = s_cmd.epos;
      x_len = x_end - x_beg + 1;
      attrset  (0);
      yCOLOR_curs ("select" );
      mvprintw (x_bott, x_left + 5 + x_beg - s_cmd.bpos, "%-*.*s", x_len, x_len, s_cmd.contents + x_beg);
   }
   /*---(current)------------------------*/
   if (x_edit == 'y' && s_cmd.npos > 0) {
      attrset  (0);
      yCOLOR_curs ("map"    );
      mvprintw (x_bott, x_left + 5 + s_cmd.cpos - s_cmd.bpos, "%c", s_cmd.contents [s_cmd.cpos]);
   }
   /*---(markers)------------------------*/
   attrset  (0);
   yCOLOR_curs ("h_used");
   if      (s_cmd.npos == 0)                c = ' ';
   else if (s_cmd.bpos > 0)                 c = '<';
   else                                     c = ']';
   mvprintw (x_bott, x_left + 4, "%c", c);
   if      (s_cmd.npos == 0)                c = ' ';
   else if (s_cmd.epos != s_cmd.npos - 1)   c = '>';
   else                                     c = '[';
   mvprintw (x_bott, x_left + s_cmd.wide - 1, "%c", c);
   /*---(complete)-----------------------*/
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
TEXTREG_status          (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
   int         x_len       = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(buffer number)------------------*/
   x_reg  = TEXTREG__index  (s_wtreg);
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
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SOURCE__goto       (int a_major, int a_minor)
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
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.DB2.162.62]*/ /*-[01.0000.403.!]-*/ /*-[--.---.---.--]-*/
SOURCE__scroll     (char a_major, char a_minor)
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
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return 0;
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
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
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



/*====================------------------------------------====================*/
/*===----                      text reg actions                        ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char         /*-> process keys for register actions --[ ------ [gz.320.011.02]*/ /*-[01.0000.113.!]-*/ /*-[--.---.---.--]-*/
TEXTREG__clear        (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =   0;
   /*---(clear)--------------------------*/
   for (i = s_bsel; i <= s_esel; ++i) {
      s_cur->contents [i] = ' ';
   }
   /*---(reposition)---------------------*/
   if (a_dir == 'x')  s_cur->cpos += s_esel - s_bsel + 1;
   else               s_cur->cpos == s_bsel - 1;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register action ---[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
TEXTREG__delete        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_diff      =   0;
   int         i           =   0;
   /*---(set size)-----------------------*/
   x_diff  = s_esel - s_bsel + 1;
   /*---(delete)-------------------------*/
   for (i = s_bsel; i < s_cur->npos; ++i) {
      s_cur->contents [i] = s_cur->contents [i + x_diff];
      if (i + x_diff >= s_cur->npos)  break;
   }
   s_cur->cpos = s_bsel;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [ge.640.051.04]*/ /*-[00.0000.213.!]-*/ /*-[--.---.---.--]-*/
TEXTREG__copy          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_label     [10]        = "";
   char        rce         = -10;
   /*---(identify register)--------------*/
   x_index = TEXTREG__index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set size)-----------------------*/
   x_start = s_cur->contents + s_bsel;
   x_len   = s_esel - s_bsel + 1;
   /*---(copy)---------------------------*/
   strlcpy (s_tregs [x_index].data, x_start, x_len + 1);
   /*---(fill in details)----------------*/
   s_tregs [x_index].len    = x_len;
   s_tregs [x_index].bpos   = s_bsel;
   s_tregs [x_index].epos   = s_esel;
   strlcpy (s_tregs [x_index].label, s_cur->label, LEN_LABEL);
   s_tregs [x_index].source = TREG_USER;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [gz.740.061.21]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
TEXTREG__replace        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   char        x_null      = '-';
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = TEXTREG__index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set the start)------------------*/
   x_start = s_cur->cpos;
   /*---(open)---------------------------*/
   x_gap   = s_tregs [x_index].len;
   if (x_start + x_gap >= s_cur->npos)  x_null = 'y';
   /*---(open)---------------------------*/
   s_cur->npos   = s_tregs [x_index].len;
   for (i  = 0; i < s_cur->npos; ++i) {
      s_cur->contents [i + x_start] = s_tregs [x_index].data [i];
   }
   /*---(final null)---------------------*/
   if (x_null == 'y') {
      s_cur->contents [x_start + x_gap + 1] == '\0';
   }
   /*---(reposition)---------------------*/
   s_cur->cpos += s_tregs [x_index].len - 1;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> process keys for register actions --[ ------ [gz.640.151.11]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
TEXTREG__paste          (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = TEXTREG__index (s_ctreg);
   if (x_index < 0)  return -1;
   /*---(set the start)------------------*/
   x_start = s_cur->cpos;
   if (a_dir == '>')   ++x_start;
   /*---(open)---------------------------*/
   x_gap   = s_tregs [x_index].len;
   for (i = s_cur->npos; i >= x_start; --i) {
      s_cur->contents [i + x_gap] = s_cur->contents [i];
      s_cur->contents [i]         = G_CHAR_PLACE;
   }
   /*---(fill)---------------------------*/
   s_cur->npos   = s_tregs [x_index].len;
   for (i  = 0; i < s_cur->npos; ++i) {
      s_cur->contents [i + x_start] = s_tregs [x_index].data [i];
   }
   /*---(reposition)---------------------*/
   s_cur->cpos += s_tregs [x_index].len - 1;
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
         TEXTREG__reset ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;   /* escape  */
      }
      DEBUG_USER   yLOG_note    ("escape, means forget and return to previous mode");
      SOURCE__reset  ();
      SOURCE__done   ();
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   case G_KEY_RETURN :
      DEBUG_USER   yLOG_note    ("enter, means save and return to previous mode");
      SOURCE__accept ();
      SOURCE__done   ();
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
            if      (s_root == s_bsel) { s_root = s_esel; s_cur->cpos = s_bsel; }
            else if (s_root == s_esel) { s_root = s_bsel; s_cur->cpos = s_esel; }
         } else {
            DEBUG_USER   yLOG_note    ("start selection of text");
            s_live = SELC_YES;
            s_root = s_cur->cpos;
         }
         rc     = 0;
         break;
      case  'f' : case  'F' :
         DEBUG_USER   yLOG_note    ("char find mode");
         return a_minor;
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
         MODE_enter (SMOD_TEXTREG);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'r' : case  'R' :
         DEBUG_USER   yLOG_note    ("enter replace mode");
         MODE_enter (SMOD_REPLACE);
         REPLACE_smode ('m', ' ');
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case  'i' : case  'a' :
         DEBUG_USER   yLOG_note    ("enter input mode");
         MODE_enter (SMOD_INPUT);
         INPUT_smode ('m', a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      }
      /*---(cut/copy/paste)--------------*/
      if (strchr ("yxdpP", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a text register mode");
         s_ctreg = '"';
         MODE_enter (SMOD_TEXTREG);
         rc = TEXTREG_smode (' ', a_minor);
      }
      /*---(basic movement)--------------*/
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = SOURCE__simple  (a_major, a_minor);
      }
      /*---(word movement)---------------*/
      if (strchr (g_hword, a_minor) != 0) {
         rc = SOURCE__word    (a_major, a_minor);
      }
      /*---(column movement)-------------*/
      if (a_minor == '|') {
         s_cur->cpos = REPEAT_use ();
         rc     = 0;
         SOURCE__done ();
      }
   }
   /*---(multi-key)----------------------*/
   else {
      switch (a_major) {
      case 'g' :  rc = SOURCE__goto      (a_major, a_minor); break;
      case 'z' :  rc = SOURCE__scroll    (a_major, a_minor); break;
      case 'f' :  x_char = a_minor;
                  rc = SOURCE__charfind  (x_char);
                  break;
      case 'F' :  x_char = a_minor;
                  rc = SOURCE__charfindrev (x_char);
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

char         /*-> process keys for register actions --[ leaf   [ge.QG5.287.FB]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
TEXTREG_smode           (int a_major, int a_minor)
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
      TEXTREG__reset ();
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
         x_index = TEXTREG__index (s_ctreg);
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
         TEXTREG__copy   ();
         MODE_exit   ();
         break;
      case  'x' :
         DEBUG_USER   yLOG_note    ("clear selection text");
         TEXTREG__copy   ();
         TEXTREG__clear  (a_minor);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'd' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TEXTREG__copy   ();
         TEXTREG__delete ();
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'r' :
         DEBUG_USER   yLOG_note    ("replace selection text");
         TEXTREG__replace ();
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  's' :
         DEBUG_USER   yLOG_note    ("substitute selection text");
         TEXTREG__delete ();
         TEXTREG__paste   ('<');
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'p' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         TEXTREG__paste   ('>');
         ++s_cur->cpos;
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'P' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         TEXTREG__paste   ('<');
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
   TEXTREG__reset ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> replace sub-mode -------------------[ ------ [ge.RG6.25#.E5]*/ /*-[02.0000.112.E]-*/ /*-[--.---.---.--]-*/
REPLACE_smode    (int a_major, int a_minor)
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
   SOURCE__prep   ();
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
         s_cur->contents [s_cur->cpos] = '\0';
      }
      if (x_saved != '\0') {
         s_cur->contents [s_cur->cpos] = x_saved;
      }
      x_append = '-';
      SOURCE__done   ();
      MODE_exit ();
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && strchr (MODES_ONELINE, MODE_curr ()) != NULL) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         SOURCE_mode (' ', a_minor);
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle keys)--------------------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , s_cur->cpos);
   DEBUG_USER   yLOG_char    ("curr char" , s_cur->contents [s_cur->cpos]);
   /*---(check for backspace)------------*/
   if (a_major == 'R' && (a_minor == G_KEY_DEL || a_minor == G_KEY_BS)) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (s_cur->cpos > 0) {
         s_cur->contents [s_cur->cpos] = x_saved;
         --(s_cur->cpos);
         x_saved = s_cur->contents [s_cur->cpos];
         s_cur->contents [s_cur->cpos] = G_CHAR_PLACE;
      }
      SOURCE__done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(check for mark)-----------------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      x_saved = s_cur->contents [s_cur->cpos];
      s_cur->contents [s_cur->cpos] = G_CHAR_PLACE;
      if (x_saved == NULL)  s_cur->contents [s_cur->cpos + 1] = NULL;
      SOURCE__done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle normal chars)------------*/
   if (a_major == 'r') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      s_cur->contents [s_cur->cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , s_cur->contents [s_cur->cpos]);
      MODE_exit ();
      SOURCE__done   ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_major == 'R') {
      DEBUG_USER   yLOG_note    ("replace the marked character");
      s_cur->contents [s_cur->cpos] = a_minor;
      DEBUG_USER   yLOG_char    ("new  char" , s_cur->contents [s_cur->cpos]);
      ++(s_cur->cpos);
      x_saved = s_cur->contents [s_cur->cpos];
      s_cur->contents [s_cur->cpos] = G_CHAR_PLACE;
   }
   /*---(correct current position)-------*/
   DEBUG_USER   yLOG_value   ("curr pos"  , s_cur->cpos);
   DEBUG_USER   yLOG_value   ("curr end"  , s_cur->npos);
   if (s_cur->cpos  >= s_cur->npos) {
      DEBUG_USER   yLOG_note    ("update the end pos");
      s_cur->contents [s_cur->npos    ] = G_CHAR_PLACE;
      s_cur->contents [s_cur->npos + 1] = '\0';
      x_append = 'y';
   }
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
}

char         /*-> process keys for input mode --------[ ------ [ge.TQ5.25#.F9]*/ /*-[03.0000.122.R]-*/ /*-[--.---.---.--]-*/
INPUT_smode             (int  a_major, int  a_minor)
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
   --rce;  if (MODE_not (SMOD_INPUT )) {
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
   SOURCE__prep ();
   /*---(check for initial mark)---------*/
   if (a_major == 'm') {
      DEBUG_USER   yLOG_note    ("mark replacement position and save existing");
      if (a_minor == 'a' && s_cur->contents [s_cur->cpos] != '\0')  ++(s_cur->cpos);
      DEBUG_USER   yLOG_value   ("total pos" , s_cur->npos);
      DEBUG_USER   yLOG_value   ("new pos"   , s_cur->cpos);
      for (i = s_cur->npos; i >= s_cur->cpos; --i)  s_cur->contents[i + 1] = s_cur->contents[i];
      s_cur->contents [s_cur->cpos] = G_CHAR_PLACE;
      SOURCE__done   ();
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
      for (i = s_cur->cpos; i <= s_cur->npos; ++i)  s_cur->contents[i] = s_cur->contents[i + 1];
      if (a_major == 'a')  --(s_cur->cpos);
      SOURCE__done   ();
      MODE_exit ();
      DEBUG_USER   yLOG_value   ("mode"     , MODE_curr ());
      if (a_minor == G_KEY_RETURN && strchr (MODES_ONELINE, MODE_curr ()) != NULL) {
         DEBUG_USER   yLOG_note    ("fast path back to map mode");
         SOURCE_mode (' ', a_minor);
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for backspace)------------*/
   if (a_minor == G_KEY_DEL || a_minor == G_KEY_BS) {
      DEBUG_USER   yLOG_note    ("handle a backspace/delete");
      if (s_cur->cpos > 0) {
         --(s_cur->cpos);
         DEBUG_USER   yLOG_value   ("curr pos"  , s_cur->cpos);
         for (i = s_cur->cpos; i <= s_cur->npos; ++i)  s_cur->contents[i] = s_cur->contents[i + 1];
         SOURCE__done   ();
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_major;
   }
   /*---(handle new character)-----------*/
   DEBUG_USER   yLOG_note    ("move remaining chars to the right");
   for (i = s_cur->npos; i >= s_cur->cpos; --i)  s_cur->contents[i + 1] = s_cur->contents[i];
   DEBUG_USER   yLOG_note    ("add the character");
   s_cur->contents [s_cur->cpos] = a_minor;
   ++(s_cur->cpos);
   /*---(wrap up)------------------------*/
   SOURCE__done   ();
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_major;
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
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SRC unit         : question not understood", LEN_STR);
   x_index = TEXTREG__index (a_reg);
   if (x_index < 0)  x_index = 0;
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "position"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC position     : %3dw, %3da, %3db, %3dc, %3de", s_cur->wide, s_cur->apos, s_cur->bpos, s_cur->cpos, s_cur->epos);
   }
   else if (strcmp (a_question, "contents"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "SRC contents     : %3d:%-.40s:", s_cur->npos, s_cur->contents);
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
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




