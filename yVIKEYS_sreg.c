/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



#define        S_SREG_MAX      100
static char    S_SREG_LIST     [S_SREG_MAX];



typedef struct  cSREG  tSREG;
struct cSREG {
   /*---(active)------------*/
   char        active;                      /* register in use                */
   char        source;                      /* user, file, import             */
   /*---(source)------------*/
   char        label       [LEN_LABEL];     /* original cell                  */
   int         beg;                         /* original starting point        */
   int         end;                         /* original ending point          */
   int         root;
   /*---(contents)----------*/
   int         len;                         /* length of text                 */
   char        data        [LEN_RECD];      /* text                           */
   /*---(done)--------------*/
};
static      tSREG       s_sreg;
static      tSREG       s_sreg_info  [S_SREG_MAX];
static      int         s_nsreg  =   0;



#define     S_SREG_NOT '-'
#define     S_SREG_YES 'y'



static char  s_csreg   = '"';
static char  s_wsreg   = '"';



#define     S_SREG_NONE     '-'
#define     S_SREG_AUTO     'a'
#define     S_SREG_USER     'u'
#define     S_SREG_IMPORT   'i'
#define     S_SREG_DIRECT   'd'



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___ELEMENTAL_______o () { return; }

char
yvikeys_sreg__wipe      (tSREG *a_dst)
{
   /*---(active)-------------------------*/
   a_dst->active = S_SREG_NOT;
   a_dst->source = S_SREG_NONE;
   /*---(source)-------------------------*/
   strlcpy (a_dst->label, "", LEN_LABEL);
   a_dst->root   =  0;
   a_dst->beg    =  0;
   a_dst->end    =  0;
   /*---(contents)-----------------------*/
   a_dst->len    =  0;
   strlcpy (a_dst->data , "", LEN_RECD);
   /*---(complete)-----------------------*/
   return 0;
}

/*> char                                                                              <* 
 *> yvikeys_sreg__copy      (tSREG *a_dst, tSREG *a_src)                              <* 
 *> {                                                                                 <* 
 *>    /+---(active)-------------------------+/                                       <* 
 *>    a_dst->active = S_SREG_YES;                                                    <* 
 *>    a_dst->source = a_src->source;                                                 <* 
 *>    /+---(source)-------------------------+/                                       <* 
 *>    strlcpy (a_dst->label, a_src->label, LEN_LABEL);                               <* 
 *>    a_dst->root   =  a_src->beg;                                                   <* 
 *>    a_dst->beg    =  a_src->beg;                                                   <* 
 *>    a_dst->end    =  a_src->end;                                                   <* 
 *>    /+---(contents)-----------------------+/                                       <* 
 *>    a_dst->len    =  a_src->len;                                                   <* 
 *>    strlcpy (a_dst->data , a_src->data, LEN_RECD);                                 <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char
yvikeys_sreg__purge     (char a_scope)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  : case YVIKEYS_NUMBER :
   case YVIKEYS_GREEK  : case YVIKEYS_FULL   :
      break;
   default :
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_PROG   yLOG_value   ("s_nsreg"   , s_nsreg);
   for (i = 0; i < s_nsreg; ++i) {
      x_abbr = S_SREG_LIST [i];
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_sreg__wipe (&s_sreg_info [i]);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_SREG)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(macro abbrev list)--------------*/
   strlcpy (S_SREG_LIST, "\""          , S_SREG_MAX);
   strlcat (S_SREG_LIST, gvikeys_lower , S_SREG_MAX);
   strlcat (S_SREG_LIST, gvikeys_number, S_SREG_MAX);
   strlcat (S_SREG_LIST, gvikeys_greek , S_SREG_MAX);
   s_nsreg  = strlen (S_SREG_LIST);
   /*---(registers)----------------------*/
   yvikeys_sreg__purge    (YVIKEYS_FULL);
   /*---(globals)------------------------*/
   yvikeys_sreg_reset     (0);
   s_wsreg = '"';
   /*---(commands)-----------------------*/
   DEBUG_PROG   yLOG_note    ("add commands/status");
   yVIKEYS_view_option (YVIKEYS_STATUS  , "sreg", yvikeys_sreg_status, "details of current source register");
   /*> yVIKEYS_cmds_addX    (YVIKEYS_M_CONFIG, "sreg", "", "a", yvikeys_sreg__direct, "direct definition of source registers");   <*/
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_SREG);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void  o___UTILITY_________o () { return; }

int  
yvikeys_sreg__index     (uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   n  = strlchr (S_SREG_LIST, a_abbr, s_nsreg);
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n  < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return n;
}

char
yvikeys_sreg_setreg     (uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST   yLOG_schar   (a_abbr);
   DEBUG_HIST   yLOG_schar   (s_csreg);
   n = yvikeys_sreg__index  (tolower (a_abbr));
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n < 0) {
      s_csreg = '"';
      DEBUG_HIST   yLOG_schar   (s_csreg);
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(set)----------------------------*/
   s_csreg = a_abbr;
   DEBUG_HIST   yLOG_schar   (s_csreg);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sreg_setwork    (uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST   yLOG_schar   (a_abbr);
   DEBUG_HIST   yLOG_schar   (s_wsreg);
   n = yvikeys_sreg__index  (tolower (a_abbr));
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n < 0) {
      s_wsreg = '"';
      DEBUG_HIST   yLOG_schar   (s_wsreg);
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(set)----------------------------*/
   s_wsreg = a_abbr;
   DEBUG_HIST   yLOG_schar   (s_wsreg);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sreg_clear      (uchar a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_HIST   yLOG_schar   (a_abbr);
   n = yvikeys_sreg__index  (a_abbr);
   DEBUG_HIST   yLOG_sint    (n);
   --rce;  if (n < 0) {
      DEBUG_HIST   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   yvikeys_sreg__wipe (&s_sreg_info [n]);
   /*---(complete)-----------------------*/
   DEBUG_HIST   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_sreg_push       (uchar a_abbr, char *a_data)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   int         x_len       =    0;
   tSREG      *a_dst       = NULL;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (a_data  == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   n = yvikeys_sreg__index  (tolower (a_abbr));
   --rce;  if (n < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_value   ("a_abbr"    , a_abbr);
   DEBUG_SCRP   yLOG_value   ("tolower"   , tolower (a_abbr));
   /*---(wipe)---------------------------*/
   a_dst = &s_sreg_info [n];
   if (a_abbr == tolower (a_abbr))  {
      DEBUG_SCRP   yLOG_note    ("normal, replace mode");
      yvikeys_sreg__wipe (a_dst);
      a_dst->active = S_SREG_YES;
      a_dst->source = S_SREG_DIRECT;
      strlcpy (a_dst->label, "-", LEN_LABEL);
      strlcpy (a_dst->data , a_data , LEN_RECD );
   } else {
      DEBUG_SCRP   yLOG_note    ("append mode");
      strlcat (a_dst->data , a_data , LEN_RECD );
   }
   a_dst->len    = strllen (a_dst->data , LEN_RECD);
   /*> strldchg (a_dst->data, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                 <*/
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_sreg_pop        (uchar a_abbr, char *a_data)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   /*---(init)---------------------------*/
   if (a_data != NULL)  strlcpy (a_data, "", LEN_RECD);
   /*---(defense)------------------------*/
   n = yvikeys_sreg__index  (a_abbr);
   --rce;  if (n < 0)  return rce;
   --rce;  if (s_sreg_info [n].active != S_SREG_YES)  return rce;
   /*---(init)---------------------------*/
   if (a_data != NULL)  strlcpy (a_data, s_sreg_info [n].data, LEN_RECD);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_sreg_fetch      (int *a_len, char *a_data)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   /*---(init)---------------------------*/
   if (a_len  != NULL)  *a_len = 0;
   if (a_data != NULL)  strlcpy (a_data, "", LEN_RECD);
   /*---(defense)------------------------*/
   n = yvikeys_sreg__index  (s_csreg);
   --rce;  if (n < 0)  return rce;
   --rce;  if (s_sreg_info [n].active != S_SREG_YES)  return rce;
   /*---(init)---------------------------*/
   if (a_len  != NULL)  *a_len = s_sreg_info [n].len;
   if (a_data != NULL) {
      strlcpy (a_data, s_sreg_info [n].data, LEN_RECD);
      /*> strldchg (a_data, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);                   <*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_sreg_save       (char *a_label, char *a_data)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =    0;
   tSREG      *a_dst       = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)  return rce;
   --rce;  if (a_data  == NULL)  return rce;
   n = yvikeys_sreg__index  (tolower (s_csreg));
   --rce;  if (n < 0)  return rce;
   /*---(save)---------------------------*/
   a_dst = &s_sreg_info [n];
   if (s_csreg == tolower (s_csreg)) {
      yvikeys_sreg__wipe (a_dst);
      a_dst->active = S_SREG_YES;
      a_dst->source = S_SREG_USER;
      strlcpy (a_dst->label, a_label, LEN_LABEL);
      a_dst->root   = s_sreg.beg;
      a_dst->beg    = s_sreg.beg;
      a_dst->end    = s_sreg.end;
      strlcpy (a_dst->data , a_data , LEN_RECD );
   } else {
      strlcat (a_dst->data , a_data , LEN_RECD );
   }
   a_dst->len    = strllen (a_dst->data , LEN_RECD);
   /*> strldchg (a_dst->data, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                 <*/
   /*---(complete)-----------------------*/
   return 0;
}

/*> char                                                                                        <* 
 *> yvikeys_sreg_append     (char *a_data)                                                      <* 
 *> {                                                                                           <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                 <* 
 *>    char        rce         =  -10;                                                          <* 
 *>    int         n           =    0;                                                          <* 
 *>    tSREG      *a_dst       = NULL;                                                          <* 
 *>    /+---(defense)------------------------+/                                                 <* 
 *>    --rce;  if (a_data  == NULL)  return rce;                                                <* 
 *>    n = yvikeys_sreg__index  (s_csreg);                                                      <* 
 *>    --rce;  if (n < 0)  return rce;                                                          <* 
 *>    /+---(wipe)---------------------------+/                                                 <* 
 *>    a_dst = &s_sreg_info [n];                                                                <* 
 *>    /+---(save)---------------------------+/                                                 <* 
 *>    a_dst->active = S_SREG_YES;                                                              <* 
 *>    a_dst->source = S_SREG_USER;                                                             <* 
 *>    strlcat (a_dst->data , a_data , LEN_RECD );                                              <* 
 *>    a_dst->len    = strllen (a_dst->data , LEN_RECD);                                        <* 
 *>    /+> strldchg (a_dst->data, G_KEY_SPACE, G_CHAR_STORAGE, LEN_RECD);                 <+/   <* 
 *>    /+---(complete)-----------------------+/                                                 <* 
 *>    return 0;                                                                                <* 
 *> }                                                                                           <*/



/*====================------------------------------------====================*/
/*===----                     status and information                   ----===*/
/*====================------------------------------------====================*/
static void  o___STATUS__________o () { return; }

char
yvikeys_sreg__line    (uchar a_abbr, char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         n           =    0;
   tSREG      *x_sreg      = NULL;
   char       *x_data      [LEN_RECD ];
   /*---(defense)------------------------*/
   --rce; if (a_entry == NULL)   return rce;
   strlcpy (a_entry, "", LEN_RECD);
   n = yvikeys_sreg__index (a_abbr);
   if (n  < 0)   return rce;
   x_sreg = &s_sreg_info [n];
   /*---(fill)---------------------------*/
   if (x_sreg->len > 40)  sprintf (x_data , "[%-.40s>", x_sreg->data);
   else                   sprintf (x_data , "[%s]", x_sreg->data);
   sprintf (a_entry, "%c %c %3d %-42.42s %-10.10s %3d %3d %3d %3d %c",
         a_abbr, x_sreg->active, x_sreg->len, x_data,
         x_sreg->label, x_sreg->beg, x_sreg->end, x_sreg->end - x_sreg->beg + 1, x_sreg->root,
         x_sreg->source);
   /*---(complete)-----------------------*/
   if (x_sreg->active == S_SREG_NOT)  return 0;
   return 1;
}

char
yvikeys_sreg_status   (char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        t           [LEN_FULL]   = "";
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)   return rce;
   /*---(get info)-----------------------*/
   rc = yvikeys_sreg__line (s_wsreg, t);
   --rce;  if (rc < 0)            return rce;
   /*---(generate stats)--------------*/
   sprintf (a_entry, "[ sreg %s ]", t);
   /*---(complete)--------------------*/
   return 0;
}

char
yvikeys_sreg_info     (int a_index, char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_abbr      =  '-';
   /*---(defense)------------------------*/
   --rce;  if (a_entry == NULL)    return rce;
   --rce;  if (a_index < 0)        return rce;
   --rce;  if (a_index >= s_nsreg) return rce;
   /*---(get info)-----------------------*/
   x_abbr = S_SREG_LIST [a_index];
   rc = yvikeys_sreg__line (x_abbr, a_entry);
   --rce;  if (rc < 0)             return rce;
   /*---(complete)--------------------*/
   return rc;
}

char         /*-> tbd --------------------------------[ ------ [ge.C53.244.61]*/ /*-[02.0000.204.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg_status_sel (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_len       = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) return rce;
   /*---(write line)------------------*/
   x_len = s_sreg.end - s_sreg.beg + 1;
   /*> sprintf (a_list , "selection %c %3d %3d %3d[%-*.*s]",                          <* 
    *>       s_sreg.active, s_sreg.beg, s_sreg.end, s_sreg.len,                       <* 
    *>       x_len, x_len, s_cur->contents + s_sreg.beg);                             <*/
   /*---(complete)--------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        current selection                     ----===*/
/*====================------------------------------------====================*/
static void  o___SELECTION_______o () { return; }

char         /*-> go back to defaults ----------------[ shoot  [gz.530.011.00]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg_reset      (int a_pos)
{
   yvikeys_sreg__wipe (&s_sreg);
   s_sreg.root  = s_sreg.end   = s_sreg.beg   = a_pos;
   s_csreg = '"';
   return 0;
}

char
yvikeys_sreg_update     (int a_pos)
{
   if (s_sreg.active != S_SREG_YES) {
      s_sreg.root = s_sreg.beg = s_sreg.end = a_pos;
   } else if (a_pos < s_sreg.root) {
      s_sreg.beg = a_pos;
      s_sreg.end = s_sreg.root;
   } else {
      s_sreg.beg = s_sreg.root;
      s_sreg.end = a_pos;
   }
   return 0;
}

int
yvikeys_sreg_reverse    (void)
{
   if      (s_sreg.root == s_sreg.beg) {
      s_sreg.root      = s_sreg.end;
      return s_sreg.beg;
   }
   else if (s_sreg.root == s_sreg.end) {
      s_sreg.root = s_sreg.beg;
      return s_sreg.end;
   }
}

int
yvikeys_sreg_exact      (int a_beg, int a_end, int a_root)
{
   s_sreg.beg  = a_beg;
   s_sreg.end  = a_end;
   s_sreg.root = a_root;
   return 0;
}

char
yvikeys_sreg_selected   (int *a_beg, int *a_end, int *a_root)
{
   if (a_beg  != NULL)  *a_beg  = s_sreg.beg;
   if (a_end  != NULL)  *a_end  = s_sreg.end;
   if (a_root != NULL)  *a_root = s_sreg.root;
   return 0;
}

char
yvikeys_sreg_all        (void)
{
   s_sreg.beg  = 0;
   s_sreg.end  = yvikeys_src_npos() - 1;
   s_sreg.root = 0;
   return 0;
}

char yvikeys_sreg_getlive    (void) { return s_sreg.active; }
char yvikeys_sreg_islive     (void) { if (s_sreg.active == S_SREG_YES)  return 1; return 0; }
char yvikeys_sreg_isdead     (void) { if (s_sreg.active == S_SREG_NOT)  return 1; return 0; }
char yvikeys_sreg_makelive   (void) { s_sreg.active = S_SREG_YES; return 0; }
char yvikeys_sreg_makedead   (void) { s_sreg.active = S_SREG_NOT; return 0; }



/*====================------------------------------------====================*/
/*===----                         mode handling                        ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char
yvikeys_sreg__export    (char a_id)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   n = yvikeys_sreg__index (a_id);
   --rce;  if (n <  0)  {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(write)--------------------------*/
   rc = yvikeys_dump_write (s_sreg_info [n].data);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_sreg__import    (char a_id)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read)---------------------------*/
   rc = yvikeys_dump_read  (0, x_recd, NULL);
   DEBUG_SCRP   yLOG_value   ("read"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("x_recd"    , x_recd);
   /*---(add)----------------------------*/
   rc = yvikeys_sreg_push    (a_id, x_recd);
   DEBUG_SCRP   yLOG_value   ("push"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_sreg__copy      (char a_id, char a_src)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        s           =   -1;
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char        x_direct    [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_id"      , a_id);
   DEBUG_SCRP   yLOG_char    ("a_src"     , a_src);
   s = yvikeys_sreg__index  (a_src);
   DEBUG_SCRP   yLOG_value   ("s"         , s);
   --rce;  if (s <  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(copy)---------------------------*/
   strlcpy (x_recd, s_sreg_info [s].data, LEN_RECD);
   x_len = strlen (x_recd);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <=  0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add)----------------------------*/
   rc = yvikeys_sreg_push    (a_id, x_recd);
   DEBUG_SCRP   yLOG_value   ("push"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_sreg__save      (char a_id, char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD];
   char       *p           = NULL;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(fix quotes)---------------------*/
   strlcpy (x_recd, a_string, LEN_RECD);
   x_len = strlen (x_recd);
   p = x_recd;
   if (x_recd [0] == G_KEY_DQUOTE && x_recd [x_len - 1] == G_KEY_DQUOTE) {
      DEBUG_SCRP   yLOG_note    ("quoted macro format");
      x_recd [--x_len] = G_KEY_NULL;
      p = x_recd + 1;
   }
   /*---(add)----------------------------*/
   rc = yvikeys_sreg_push    (a_id, p);
   DEBUG_SCRP   yLOG_value   ("push"      , rc);
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> enter a macro directly -------------[ ------ [ge.850.137.A4]*/ /*-[02.0000.00#.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg__direct    (char *a_string)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        x_id        =  '-';
   char        x_div       =  '-';
   char       *x_valid     = "*aA0è";
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_SREG)) {
      DEBUG_SCRP   yLOG_note    ("can not execute until operational");
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_point   ("a_string"  , a_string);
   --rce;  if (a_string == NULL) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_SCRP   yLOG_info    ("a_string"  , a_string);
   x_len = strlen (a_string);
   DEBUG_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_id  = a_string [0];
   DEBUG_SCRP   yLOG_char    ("x_id"      , x_id);
   x_div = a_string [1];
   DEBUG_SCRP   yLOG_char    ("x_div"     , x_div);
   /*---(check for purge)----------------*/
   --rce;  if (x_len == 1) {
      rc = yvikeys_sreg__purge  (x_id);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check for shorts)---------------*/
   --rce;  if (x_len == 2) {
      switch (x_div) {
      case '=' : rc = yvikeys_sreg_clear    (x_id);        break;
      case '-' : rc = yvikeys_sreg__export  (x_id);        break;
      case '+' : rc = yvikeys_sreg__import  (x_id);        break;
      default  : rc = yvikeys_sreg__copy    (x_id, x_div); break;
      }
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(assignment)---------------------*/
   --rce;  if (x_len >  2 && x_div == '=') {
      rc = yvikeys_sreg__save (x_id, a_string + 2);
      DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for register actions --[ leaf   [ge.QG5.287.FB]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg_smode      (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =  -1;
   int         x_buf       =  -1;
   int         n           =   0;
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
   --rce;  if (MODE_not (SMOD_SREG)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(escape)-------------------------*/
   if (a_minor == G_KEY_ESCAPE)  {
      DEBUG_USER   yLOG_note    ("escape and return to previous mode");
      yvikeys_sreg_reset (yvikeys_src_cpos ());
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(select register name)-----------*/
   --rce;  if (a_major == '"') {
      n = yvikeys_sreg__index (tolower (a_minor));
      if (n >= 0) {
         DEBUG_USER   yLOG_note    ("select a source register");
         s_csreg = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      } else if (a_minor == '?') {
         DEBUG_USER   yLOG_note    ("show source register inventory");
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
      case '_' :
         s_wsreg = s_csreg;
         yVIKEYS_cmds_direct (":status sreg");
         MODE_exit ();
         break;
      case  '#' :
         DEBUG_USER   yLOG_note    ("wipe source register");
         n = yvikeys_sreg__index  (s_csreg);
         if (n < 0)  return -1;
         yvikeys_sreg__wipe (&s_sreg_info [n]);
         MODE_exit ();
         break;
      case  'y' :
         DEBUG_USER   yLOG_note    ("yank selection source to register");
         yvikeys_src_copy   ();
         yvikeys_sreg_reset (s_sreg.root);
         MODE_exit   ();
         break;
      case  'x' : case  'X' :
         DEBUG_USER   yLOG_note    ("clear selection source");
         yvikeys_src_copy   ();
         yvikeys_src_clear  (a_major, a_minor);
         yvikeys_sreg_reset (s_sreg.end);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'd' : case  'D' :
         DEBUG_USER   yLOG_note    ("delete selection source");
         yvikeys_src_copy   ();
         yvikeys_src_delete (a_major, a_minor);
         yvikeys_sreg_reset (s_sreg.beg);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'r' :
         DEBUG_USER   yLOG_note    ("replace source from register");
         yvikeys_src_replace ();
         yvikeys_sreg_reset (s_sreg.end);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  's' :
         DEBUG_USER   yLOG_note    ("substitute selection source");
         yvikeys_src_delete  (a_major, 'd');
         yivkeys_sundo_chain  ();
         yvikeys_src_paste    ('i');
         yvikeys_sreg_reset (s_sreg.end);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'p' : case  'a' :
         DEBUG_USER   yLOG_note    ("paste after selection source");
         yvikeys_src_paste   ('a');
         yvikeys_sreg_reset (s_sreg.end);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      case  'P' : case  'i' :
         DEBUG_USER   yLOG_note    ("paste before selection source");
         yvikeys_src_paste   ('i');
         yvikeys_sreg_reset (s_sreg.end);
         MODE_exit   ();
         SOURCE__done    ();
         break;
      }
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys_sreg_dump       (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_end = strlen (S_SREG_LIST);
   x_len = 80 - 60 - strlen (myVIKEYS.s_prog);
   fprintf (a_file, "##===[[ yVIKEYS source register dump from %s (:dump sreg) ]]%*.*s##\n", myVIKEYS.s_prog, x_len, x_len, "======================================================================================================================================");
   /*---(walk list)----------------------*/
   for (i = 0; i <= x_end; ++i) {
      if (s_sreg_info [i].len <= 0)  continue;
      fprintf (a_file, "%c=%s\n", S_SREG_LIST [i], s_sreg_info [i].data);
      ++c;
   }
   fprintf (a_file, "##===[[ end-of-dump (%2d recs) ]]==============================================##\n", c);
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}


/*====================------------------------------------====================*/
/*===----                     unit testing functions                   ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_sreg__unit      (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        n           =    0;
   char        t           [LEN_RECD];
   int         x_len       =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "SREG unit        : question not understood", LEN_FULL);
   /*---(simple)-------------------------*/
   if      (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SREG selection   :    %c, %3db, %3de, %3d#, %3dr", s_sreg.active, s_sreg.beg, s_sreg.end, s_sreg.end - s_sreg.beg + 1, s_sreg.root);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "register"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SREG register    : %c  %c", s_csreg, s_wsreg);
      return yVIKEYS__unit_answer;
   }
   else if (strcmp (a_question, "clip"           )   == 0) {
      yvikeys_dump_read (a_reg, t, &x_len);
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "SREG clip   (%2d) : %2d[%.40s]", a_reg, x_len, t);
      return yVIKEYS__unit_answer;
   }
   /*---(complex)------------------------*/
   n = yvikeys_sreg__index  (a_reg);
   if (n < 0) {
      strlcpy  (yVIKEYS__unit_answer, "SREG unit        : not a valid register name", LEN_FULL);
      return yVIKEYS__unit_answer;
   }
   if      (strcmp (a_question, "saved"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SREG saved   (%c) : %c %3d[%.40s]", a_reg, s_sreg_info [n].active, s_sreg_info [n].len, s_sreg_info [n].data);
   }
   if      (strcmp (a_question, "source"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "SREG source  (%c) : %c  %c  %-12.12s  %3db  %3de  %3dr", a_reg, s_sreg_info [n].active, s_sreg_info [n].source, s_sreg_info [n].label, s_sreg_info [n].beg, s_sreg_info [n].end, s_sreg_info [n].root);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}
