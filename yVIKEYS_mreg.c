/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



/*===[[ HEADER ]]=============================================================*/

/*
 *   library       : yVIKEYS
 *   module        : regs
 *   purpose       : map mode copy/paste registers
 * 
 */

/*
 *   yVIKEYS_regs manages a series of temporary registers which are used to
 *   aid movement of cells from one spreadsheet location to another using the
 *   yank, delete, and put commands.  in order to support scripting, batch
 *   operations, and power/technical users; a large number of registers will
 *   be created to complicated operations and scripts can be facilitated.
 *
 *   since registers are vital to all applications that allow any form of
 *   editing or modification, a consistent interface will allow users to
 *   quickly become efficient and effective in each tool.  also, consistency
 *   will allow deeper tool usage as training will easily translate across
 *   applications.
 *
 *   gyges registers will be based on the usage and capabilities of vi/vim, but
 *   as mentioned above, generalized to be useful in a broad range of
 *   applications.  the resulting consistent command set is called the vi-keys
 *   mini-language.  because of this desired consistency, though, i will have to
 *   manage a series of broad applicability versus power/depth trade-offs which
 *   will result in better transportability, but perhaps a slightly sub-optimal
 *   feature set for a spreadsheet.
 *
 *   vi/vim has a huge number of registers, many of which are special purpose.
 *   we will focus on delivering the following fifty-five registers to be used
 *   for (y)ank, (d)elete, and (p)ut.
 *      -      default, unnamed register
 *      a-z    named registers, accessable only within this spreadsheet
 *      A-Z    named registers, accessable to all spreadsheet instances
 *      +      window manager clipboard as delimited, 7bit-safe, ascii text
 *      0      working area                  
 *
 *   gyges registers are meant to support power users, scripting, and robust
 *   batch usage.  for everyday they are way overbuilt ;)
 *
 *
 */




/*====================-----------------+------------------====================*/
/*===----                    REGISTER DATA STRUCTURE                   ----===*/
/*====================-----------------+------------------====================*/
/*
 *   the register data structure is about saving data/objects that have been
 *   copied from the main application.  it is critical that all registers
 *   maintain context information and sequencing (where necessary) to properly
 *   reintegrate the data/objects into a new location in the application.
 *
 */

#define     S_REG_MAX   100
#define     LEN_BUF    1000


typedef  struct  cITEM  tITEM;
struct cITEM {
   void       *data;
   tITEM      *b_next;
   tITEM      *b_prev;
};


typedef  struct  cREG   tREG;
struct cREG {
   /*---(#1, ORIGIN TAB)-----------------*/
   /*   stores the tab number of the original selection so that cell          */
   /*   references can be properly adjusted when placed back into a tab.      */
   char        b_all;
   char        z_all;
   /*---(#2, MINIMUM REACH)--------------*/
   /*   stores the column and row of the furthest upper-left cell reference   */
   /*   within a formula of a register cell (for error-checking).             */
   short       x_min;
   short       y_min;
   /*---(#3, MINIMUM BOUND)--------------*/
   /*   stores the column and row of the upper-left corner of the selection   */
   /*   used to source the cells for the register.                            */
   short       x_beg;
   short       y_beg;
   /*---(#4, MAXIMUM BOUND)--------------*/
   /*   stores the column and row of the lower-right corner of the selection  */
   /*   used to source the cells for the register.                            */
   short       x_end;
   short       y_end;
   /*---(#5, MAXIMUM REACH)--------------*/
   /*   stores the column and row of the furthest lower-right cell reference  */
   /*   within a formula of a register cell (for error-checking).             */
   short       x_max;
   short       y_max;
   /*---(#6, CELLS)----------------------*/
   /*   stores the an array of cells within the register in the order that    */
   /*   they need to be placed back into a tab to correctly calculate.        */
   tITEM      *hbuf;                             /* head of items             */
   tITEM      *tbuf;                             /* tail of items             */
   short       nbuf;                             /* total items               */
   char       *labels;                           /* label list (debugging)    */
   /*---(#7, SPECIAL)--------------------*/
   /*   the type flag indicates whether non-selection formula cells with      */
   /*   references into the selection should be adjusted when pasted.         */
   char        type;
   /*---(end)----------------------------*/
};
static      tREG        s_regs       [S_REG_MAX];
static      char        S_REG_LIST   [S_REG_MAX];
static      int         s_nreg       =    0;
static      char        s_creg       =  '-';
static      char       *s_stub       = ",";

#define     S_REG_EMPTY     '-'
#define     S_REG_ACTIVE    'y'

/* destroy a copy held in a register --------------------------*/
static char    (*s_regkill)    (void *a_thing);
/* get a copy and put it into a register ----------------------*/
static void*   (*s_copier)     (char a_type, long a_stamp);
/* clear an area in the host application ----------------------*/
static char    (*s_clearer)    (char a_1st, int b, int x, int y, int z);
/* integrate something into the host application --------------*/
static char    (*s_paster)     (char a_regs, char a_pros, char a_intg, char a_1st, int a_boff, int a_xoff, int a_yoff, int a_zoff, void *a_thing);
/* data export and import -------------------------------------*/
static char    (*s_exim)       (char a_dir, char a_style);



#define   MAX_PASTING           500
typedef  struct cPASTING tPASTING;
struct cPASTING {
   char        abbr;
   char        ref         [LEN_LABEL];
   char        name        [LEN_LABEL];
   char        primary;
   char        clear;       /* merge, clear, ... */
   char        reqs;        /* none, relative, inner, both (rel/inner), every  */
   char        pros;        /* none, relative, all, split */
   char        intg;        /* tbd */
   char        desc        [LEN_DESC ];
};
static const tPASTING   s_pasting [MAX_PASTING] = {
   /*-a- --ref-- ---name-------- -pr-    --c- --r- --p- --i- ---desc--- */
   { '-', "----", ""            , '-',    '-', '-', '-', '-',    "tbd" },
   /*---*/
   { '-', "-nN-", ""            , '-',    '-', 'n', 'N', '-',    "tbd" },
   { '-', "-nR-", ""            , '-',    '-', 'n', 'R', '-',    "tbd" },
   { '-', "-nA-", ""            , '-',    '-', 'n', 'A', '-',    "tbd" },
   { '-', "-nS-", ""            , '-',    '-', 'n', 'S', '-',    "tbd" },
   /*---*/
   { 'n', "-rN-", "normal"      , 'y',    '-', 'r', 'N', '-',    "tbd" },
   { '-', "-rR-", ""            , '-',    '-', 'r', 'R', '-',    "tbd" },
   { '-', "-rA-", ""            , '-',    '-', 'r', 'A', '-',    "tbd" },
   { '-', "-rS-", ""            , '-',    '-', 'r', 'S', '-',    "tbd" },
   /*---*/
   { '-', "-iN-", ""            , '-',    '-', 'i', 'N', '-',    "tbd" },
   { '-', "-iR-", ""            , '-',    '-', 'i', 'R', '-',    "tbd" },
   { '-', "-iA-", ""            , '-',    '-', 'i', 'A', '-',    "tbd" },
   { '-', "-iS-", ""            , '-',    '-', 'i', 'S', '-',    "tbd" },
   /*---*/
   { '-', "-bN-", ""            , '-',    '-', 'b', 'N', '-',    "tbd" },
   { '-', "-bR-", ""            , '-',    '-', 'b', 'R', '-',    "tbd" },
   { '-', "-bA-", ""            , '-',    '-', 'b', 'A', '-',    "tbd" },
   { '-', "-bS-", ""            , '-',    '-', 'b', 'S', '-',    "tbd" },
   /*---*/
   { '-', "-eN-", ""            , '-',    '-', 'e', 'N', '-',    "tbd" },
   { '-', "-eR-", ""            , '-',    '-', 'e', 'R', '-',    "tbd" },
   { '-', "-eA-", ""            , '-',    '-', 'e', 'A', '-',    "tbd" },
   { '-', "-eS-", ""            , '-',    '-', 'e', 'S', '-',    "tbd" },
   /*---------*/
   { '#', "y---", "clear"       , 'y',    'y', '-', '-', '-',    "tbd" },
   /*---*/
   { '-', "ynN-", ""            , '-',    'y', 'n', 'N', '-',    "tbd" },
   { '-', "ynR-", ""            , '-',    'y', 'n', 'R', '-',    "tbd" },
   { '-', "ynA-", ""            , '-',    'y', 'n', 'A', '-',    "tbd" },
   { '-', "ynS-", ""            , '-',    'y', 'n', 'S', '-',    "tbd" },
   /*---*/
   { 'r', "yrN-", "replace"     , 'y',    'y', 'r', 'N', '-',    "tbd" },
   { '-', "yrR-", ""            , '-',    'y', 'r', 'R', '-',    "tbd" },
   { '-', "yrA-", ""            , '-',    'y', 'r', 'A', '-',    "tbd" },
   { '-', "yrS-", ""            , '-',    'y', 'r', 'S', '-',    "tbd" },
   /*---*/
   { 'd', "yiN-", "duplicate"   , 'y',    'y', 'i', 'N', '-',    "tbd" },
   { '-', "yiR-", ""            , '-',    'y', 'i', 'R', '-',    "tbd" },
   { '-', "yiA-", ""            , '-',    'y', 'i', 'A', '-',    "tbd" },
   { '-', "yiS-", ""            , '-',    'y', 'i', 'S', '-',    "tbd" },
   /*---*/
   { 'c', "ybN-", "combo"       , 'y',    'y', 'b', 'N', '-',    "tbd" },
   { 'm', "ybR-", "move"        , 'y',    'y', 'b', 'R', '-',    "tbd" },
   { 'f', "ybA-", "force"       , 'y',    'y', 'b', 'A', '-',    "tbd" },
   { '-', "ybS-", ""            , '-',    'y', 'b', 'S', '-',    "tbd" },
   /*---*/
   { '-', "yeN-", ""            , '-',    'y', 'e', 'N', '-',    "tbd" },
   { '-', "yeR-", ""            , '-',    'y', 'e', 'R', '-',    "tbd" },
   { '-', "yeA-", ""            , '-',    'y', 'e', 'A', '-',    "tbd" },
   { '-', "yeS-", ""            , '-',    'y', 'e', 'S', '-',    "tbd" },
   /*---------*/
};



/*====================------------------------------------====================*/
/*===----                        support functions                     ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char 
yvikeys_mreg__valid     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   rc = strlchr (S_REG_LIST, a_abbr, S_REG_MAX);
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
yvikeys_mreg__index     (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_HIST   yLOG_senter  (__FUNCTION__);
   /*---(check)--------------------------*/
   DEBUG_HIST   yLOG_snote   ("check");
   n  = strlchr (S_REG_LIST, a_abbr, S_REG_MAX);
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
yvikeys_mreg__reset             (void)
{
   /*---(reset register)-----------------*/
   s_creg = '"';
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_mreg__set               (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MREG)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(reset)--------------------------*/
   yvikeys_mreg__reset ();
   /*---(get register number)------------*/
   x_reg  = yvikeys_mreg__index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_creg = a_reg;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> indicate whether cell is in a reg --[ leaf   [ge.430.419.80]*/ /*-[00.0000.209.#]-*/ /*-[--.---.---.--]-*/
yVIKEYS_mreg_inside     (int b, int x, int y, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_reg       = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(get register number)------------*/
   x_reg  = yvikeys_mreg__index  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   --rce;  if (s_regs [x_reg].type != S_REG_ACTIVE) {
      DEBUG_REGS   yLOG_note    ("register inactive");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(misses)-------------------------*/
   if (b != s_regs [x_reg].b_all) {
      DEBUG_REGS   yLOG_note    ("b failed");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (x <  s_regs [x_reg].x_beg || x >  s_regs [x_reg].x_end) {
      DEBUG_REGS   yLOG_note    ("x failed");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (y <  s_regs [x_reg].y_beg || y >  s_regs [x_reg].y_end) {
      DEBUG_REGS   yLOG_note    ("y failed");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (z != s_regs [x_reg].z_all) {
      DEBUG_REGS   yLOG_note    ("z failed");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete------------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 1;
}


/*====================------------------------------------====================*/
/*===----                      memory allocation                       ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char
yvikeys_mreg__new       (char a_reg, void *a_item)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tITEM      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   tITEM      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CMDS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_CMDS   yLOG_point   ("a_item"    , a_item);
   --rce;  if (a_item == NULL) {
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tITEM *) malloc (sizeof (tITEM));
   }
   DEBUG_CMDS   yLOG_value   ("x_tries"   , x_tries);
   DEBUG_CMDS   yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_CMDS   yLOG_note    ("FAILED");
      DEBUG_CMDS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(populate)-----------------------*/
   DEBUG_CMDS   yLOG_note    ("populate");
   x_new->data     = a_item;
   x_new->b_next   = NULL;
   x_new->b_prev   = NULL;
   /*---(tie to master list)-------------*/
   if (s_regs [a_reg].hbuf == NULL) {
      DEBUG_CMDS   yLOG_note    ("nothing in master list, make first");
      s_regs [a_reg].hbuf = x_new;
   } else  {
      DEBUG_CMDS   yLOG_note    ("append to master list");
      s_regs [a_reg].tbuf->b_next = x_new;
      x_new->b_prev  = s_regs [a_reg].tbuf;
   }
   s_regs [a_reg].tbuf = x_new;
   /*---(list)---------------------------*/
   x_curr = s_regs [a_reg].hbuf;
   while  (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("item"      , "%-10p, %-10p, %-10p, %-10p", x_curr, x_curr->data, x_curr->b_prev, x_curr->b_next);
      x_curr = x_curr->b_next;
   }
   /*---(update counts)------------------*/
   ++s_regs [a_reg].nbuf;
   /*---(complete)-----------------------*/
   DEBUG_CMDS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear out a register ---------------[ ------ [ge.B63.253.32]*/ /*-[03.0000.043.3]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__wipe              (char a_reg, char a_scope)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   int         i           = 0;
   char        rc          = 0;
   tITEM      *x_curr      = NULL;
   tITEM      *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   DEBUG_REGS   yLOG_char    ("a_scope"   , a_scope);
   /*---(get register number)------------*/
   x_reg  = yvikeys_mreg__index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(boundaries)---------------------*/
   DEBUG_REGS   yLOG_note    ("reset all values to zero");
   s_regs [x_reg].z_all = 0;
   s_regs [x_reg].x_min  = s_regs [x_reg].y_min  = 0;
   s_regs [x_reg].x_beg  = s_regs [x_reg].y_beg  = 0;
   s_regs [x_reg].x_end  = s_regs [x_reg].y_end  = 0;
   s_regs [x_reg].x_max  = s_regs [x_reg].y_max  = 0;
   s_regs [x_reg].type   = S_REG_EMPTY;
   /*---(cells)--------------------------*/
   DEBUG_REGS   yLOG_note    ("clear all register positions");
   --rce;
   x_curr = s_regs [x_reg].tbuf;
   while (x_curr != NULL) {
      x_next = x_curr->b_prev;
      if (s_regkill != NULL && a_scope != YVIKEYS_INIT && x_curr->data != NULL) {
         rc = s_regkill (x_curr->data);
         if (rc < 0) {
            DEBUG_REGS   yLOG_note    ("found a bad register position");
            DEBUG_REGS   yLOG_value   ("posid"     , i);
            DEBUG_REGS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
      }
      x_curr->data   = NULL;
      x_curr->b_next = NULL;
      x_curr->b_prev = NULL;
      free (x_curr);
      x_curr = x_next;
   }
   s_regs [x_reg].hbuf = NULL;
   s_regs [x_reg].tbuf = NULL;
   s_regs [x_reg].nbuf = 0;
   if (a_scope != YVIKEYS_INIT && s_regs [x_reg].labels != s_stub)
      free (s_regs [x_reg].labels);
   s_regs [x_reg].labels = s_stub;
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                   initialization and wrapup                  ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> clear out all buffers --------------[ ------ [gz.421.121.01]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__purge             (char a_scope)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   char        x_abbr      =  '-';
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_char    ("a_scope"   , a_scope);
   --rce;  switch (a_scope) {
   case YVIKEYS_LOWER  : case YVIKEYS_UPPER  : case YVIKEYS_NUMBER :
   case YVIKEYS_GREEK  : case YVIKEYS_FULL   : case YVIKEYS_INIT   :
      break;
   default :
      DEBUG_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   s_creg = '"';
   DEBUG_SCRP   yLOG_value   ("s_nreg"    , s_nreg);
   for (i = 0; i < s_nreg; ++i) {
      x_abbr = S_REG_LIST [i];
      if (a_scope == YVIKEYS_UPPER  && strchr (gvikeys_upper , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_LOWER  && strchr (gvikeys_lower , x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_NUMBER && strchr (gvikeys_number, x_abbr) == NULL)  continue;
      if (a_scope == YVIKEYS_GREEK  && strchr (gvikeys_greek , x_abbr) == NULL)  continue;
      yvikeys_mreg__wipe (x_abbr, a_scope);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_MREG)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(macro abbrev list)--------------*/
   strlcpy (S_REG_LIST, "\""          , S_REG_MAX);
   strlcat (S_REG_LIST, gvikeys_lower , S_REG_MAX);
   strlcat (S_REG_LIST, gvikeys_upper , S_REG_MAX);
   strlcat (S_REG_LIST, gvikeys_number, S_REG_MAX);
   strlcat (S_REG_LIST, gvikeys_greek , S_REG_MAX);
   strlcat (S_REG_LIST, "-+"          , S_REG_MAX);
   s_nreg   = strlen (S_REG_LIST);
   /*---(registers)----------------------*/
   yvikeys_mreg__purge    (YVIKEYS_INIT);
   s_regkill  = NULL;
   s_copier  = NULL;
   s_clearer = NULL;
   s_paster  = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_MREG);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg_wrap               (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(registers)----------------------*/
   yvikeys_mreg__purge    (YVIKEYS_INIT);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_mreg_config       (void *a_clearer, void *a_copier, void *a_paster, void *a_regkill, void *a_exim)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_REGS  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_MREG)) {
      DEBUG_REGS   yLOG_note    ("init must complete before config");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   DEBUG_REGS  yLOG_point   ("a_regkill" , a_regkill);
   s_regkill    = a_regkill;
   DEBUG_REGS  yLOG_point   ("a_copier"  , a_copier);
   s_copier     = a_copier;
   DEBUG_REGS  yLOG_point   ("a_clearer" , a_clearer);
   s_clearer    = a_clearer;
   DEBUG_REGS  yLOG_point   ("a_paster"  , a_paster);
   s_paster     = a_paster;
   DEBUG_REGS  yLOG_point   ("a_exim"    , a_exim);
   s_exim       = a_exim;
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_MREG, '1');
   /*---(complete)-----------------------*/
   DEBUG_REGS  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           attaching                          ----===*/
/*====================------------------------------------====================*/
static void  o___ATTACHING_______o () { return; }

static char    s_saving   = '-';

char         /*-> attach a cell to a buffer ----------[ ------ [fe.870.378.72]*/ /*-[00.0000.025.7]-*/ /*-[--.---.---.--]-*/
yVIKEYS_mreg_add        (void *a_thing, char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   int         x_reg       =   0;
   char        t           [LEN_LABEL]  = "";
   char        x_labels    [LEN_RECD]   = "";
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_char    ("s_saving"  , s_saving);
   --rce;  if (s_saving != 'y') {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   DEBUG_REGS   yLOG_point   ("a_thing"   , a_thing);
   --rce;  if (a_thing == NULL)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   DEBUG_REGS   yLOG_char    ("s_creg"    , s_creg);
   x_reg  = yvikeys_mreg__index  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(attach)-------------------------*/
   rc = yvikeys_mreg__new (x_reg, a_thing);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   DEBUG_REGS   yLOG_point   ("a_label"   , a_label);
   if (a_label != NULL) {
      DEBUG_REGS   yLOG_info    ("a_label"   , a_label);
      if (s_regs [x_reg].labels != s_stub)  {
         strlcpy (x_labels, s_regs [x_reg].labels, LEN_RECD);
         free (s_regs [x_reg].labels);
         sprintf (t, "%s,", a_label);
      } else {
         sprintf (t, ",%s,", a_label);
      }
      strlcat (x_labels, t, LEN_RECD);
      s_regs [x_reg].labels = strdup (x_labels);
   }
   /*---(counters)-----------------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_regs [x_reg].nbuf);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_mreg_save               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_reg       =    0;
   int         x_nbuf      =    0;
   long        x_stamp     =    0;
   int         b, xb, xe, yb, ye, z;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MREG)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check copier)-------------------*/
   DEBUG_REGS   yLOG_point   ("s_copier"  , s_copier);
   --rce;  if (s_copier == NULL) {
      DEBUG_REGS   yLOG_note    ("copier must be set to operate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(identify register)--------------*/
   DEBUG_REGS   yLOG_char    ("s_creg"    , s_creg);
   x_reg  = yvikeys_mreg__index  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(wipe register)------------------*/
   rc = yvikeys_mreg__wipe  (s_creg, '-');
   DEBUG_REGS   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(save selection)-----------------*/
   rc = yVIKEYS_visu_coords (&b, &xb, &xe, &yb, &ye, &z);
   DEBUG_REGS   yLOG_value   ("visu rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   s_regs [x_reg].b_all   = b;
   s_regs [x_reg].x_beg   = xb;
   s_regs [x_reg].x_end   = xe;
   s_regs [x_reg].y_beg   = yb;
   s_regs [x_reg].y_end   = ye;
   s_regs [x_reg].z_all   = z;
   /*---(call save)----------------------*/
   x_stamp  = rand ();
   s_saving = 'y';
   s_regs [x_reg].type = S_REG_ACTIVE;
   rc = s_copier ('c', x_stamp);
   s_saving = '-';
   DEBUG_REGS   yLOG_value   ("copy rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(check counts)-------------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_regs [x_reg].nbuf);
   /*> if (s_regs [x_reg].nbuf <= 0)  yvikeys_mreg__wipe (s_creg, '-');               <*/
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_mreg__list            (char a_reg, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_reg       =    0;
   /*---(identify register)--------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   strlcpy (a_list, "?"                  , LEN_RECD);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   x_reg  = yvikeys_mreg__index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   strlcpy (a_list, s_regs [x_reg].labels, LEN_RECD);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        major actions                         ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char
yvikeys_mreg__clear          (char a_1st)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y;
   int         b;
   int         x_beg, x_end;
   int         y_beg, y_end;
   int         z;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MREG)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save selection)-----------------*/
   rc = yVIKEYS_visu_coords (&b, &x_beg, &x_end, &y_beg, &y_end, &z);
   DEBUG_REGS   yLOG_value   ("visu rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(check clearer)------------------*/
   DEBUG_REGS   yLOG_point   ("s_clearer" , s_clearer);
   --rce;  if (s_clearer == NULL) {
      DEBUG_REGS   yLOG_note    ("clearer must be set to operate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   for (x = x_beg; x <= x_end; ++x) {
      for (y = y_beg; y <= y_end; ++y) {
         rc = s_clearer (a_1st, b, x, y, z);
         if (rc == 0)  a_1st = '-';
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_mreg_clear        (void) { yvikeys_mreg__clear ('y'); }
char yvikeys_mreg_clear_combo  (void) { yvikeys_mreg__clear ('-'); }

static int  s_boff  = 0;
static int  s_xoff  = 0;
static int  s_yoff  = 0;
static int  s_zoff  = 0;

static int  s_reg   = 0;
static char s_clear = 0;
static char s_reqs  = 0;
static char s_pros  = 0;
static char s_intg  = 0;

char         /*-> prepare for a paste ----------------[ ------ [fe.842.023.21]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__paste_check    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_nbuf      =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MREG)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check paster)-------------------*/
   DEBUG_REGS   yLOG_point   ("s_paster"  , s_paster);
   --rce;  if (s_paster == NULL) {
      DEBUG_REGS   yLOG_note    ("paster must be set to operate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(identify register)--------------*/
   DEBUG_REGS   yLOG_char    ("s_creg"    , s_creg);
   s_reg  = yvikeys_mreg__index  (s_creg);
   DEBUG_REGS   yLOG_value   ("s_reg"     , s_reg);
   --rce;  if (s_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(check for empty)----------------*/
   x_nbuf = s_regs [s_reg].nbuf;
   DEBUG_REGS   yLOG_value   ("x_nbuf"    , x_nbuf);
   /*> --rce;  if (x_nbuf <= 0) {                                                     <* 
    *>    DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return  rce;                                                                <* 
    *> }                                                                              <*/
   /*---(get current position)-----------*/
   yvikeys_map_current (NULL, &s_boff, &s_xoff, &s_yoff, &s_zoff);
   DEBUG_REGS   yLOG_value   ("s_boff"    , s_boff);
   DEBUG_REGS   yLOG_value   ("s_xoff"    , s_xoff);
   DEBUG_REGS   yLOG_value   ("s_yoff"    , s_yoff);
   DEBUG_REGS   yLOG_value   ("s_zoff"    , s_zoff);
   /*---(make into offsets)--------------*/
   s_boff -=  s_regs [s_reg].b_all;
   s_xoff -=  s_regs [s_reg].x_beg;
   s_yoff -=  s_regs [s_reg].y_beg;
   s_zoff -=  s_regs [s_reg].z_all;
   DEBUG_REGS   yLOG_value   ("s_boff (*)", s_boff);
   DEBUG_REGS   yLOG_value   ("s_xoff (*)", s_xoff);
   DEBUG_REGS   yLOG_value   ("s_yoff (*)", s_yoff);
   DEBUG_REGS   yLOG_value   ("s_zoff (*)", s_zoff);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare for a paste ----------------[ ------ [fe.842.023.21]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__paste_settings (char *a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   int         n           =   -1;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   s_clear   = '-';
   s_reqs    = '-';
   s_pros    = '-';
   s_intg    = '-';
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_type"    , a_type);
   --rce;  if (a_type == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("a_type"    , a_type);
   x_len = strllen (a_type, LEN_LABEL);
   DEBUG_REGS   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <  4) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(lookup on ref)------------------*/
   if (x_len == 4) {
      for (i = 0; i < MAX_PASTING; ++i) {
         if (a_type [0] != s_pasting [i].ref [0])   continue;
         if (a_type [1] != s_pasting [i].ref [1])   continue;
         if (a_type [2] != s_pasting [i].ref [2])   continue;
         if (a_type [3] != s_pasting [i].ref [3])   continue;
         n = i;
         break;
      }
   }
   DEBUG_REGS   yLOG_value   ("ref n"     , n);
   /*---(lookup on name)-----------------*/
   if (n < 0) {
      for (i = 0; i < MAX_PASTING; ++i) {
         if (a_type [0] != s_pasting [i].name [0])     continue;
         if (strcmp (a_type, s_pasting [i].name) != 0) continue;
         n = i;
         break;
      }
      DEBUG_REGS   yLOG_value   ("name n"    , n);
   }
   /*---(check for failures)-------------*/
   --rce;  if (n < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark settings)------------------*/
   s_clear = s_pasting [n].clear;
   s_reqs  = s_pasting [n].reqs;
   s_pros  = s_pasting [n].pros;
   s_intg  = s_pasting [n].intg;
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare for a paste ----------------[ ------ [fe.842.023.21]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__paste_clear    (char a_1st)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   /*---(check clearer)------------------*/
   DEBUG_REGS   yLOG_point   ("s_clearer" , s_clearer);
   --rce;  if (s_clearer == NULL) {
      DEBUG_REGS   yLOG_note    ("clearer must be set to operate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   DEBUG_REGS   yLOG_complex ("range"     , "%3dx to %3dx, %3dy to %3dy", s_regs [s_reg].x_beg + s_xoff, s_regs [s_reg].x_end + s_xoff, s_regs [s_reg].y_beg + s_yoff, s_regs [s_reg].y_end + s_yoff);
   for (x = s_regs [s_reg].x_beg; x <= s_regs [s_reg].x_end; ++x) {
      for (y = s_regs [s_reg].y_beg; y <= s_regs [s_reg].y_end; ++y) {
         DEBUG_REGS   yLOG_complex ("target"    , "%c, %3dx, %3dy, %3dz", a_1st, x + s_xoff, y + s_yoff, s_regs [s_reg].z_all + s_zoff);
         rc = s_clearer (a_1st, s_regs [s_reg].b_all + s_boff, x + s_xoff, y + s_yoff, s_regs [s_reg].z_all + s_zoff);
         if (rc == 0)  a_1st = '-';
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_mreg__paste             (char a_1st, char *a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y, z;
   int         c           =    0;
   char        x_1st       =  'y';
   tITEM      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_1st"     , a_1st);
   /*---(prepare)------------------------*/
   rc = yvikeys_mreg__paste_check    ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(settings)-----------------------*/
   rc = yvikeys_mreg__paste_settings (a_type);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clearing)-----------------------*/
   DEBUG_REGS   yLOG_char    ("s_clear"   , s_clear);
   if (s_clear == 'y') {
      yvikeys_mreg__paste_clear (a_1st);
      a_1st = '-';  /* link pasting to clearing */
   }
   if (s_reqs == '-') {
      DEBUG_REGS   yLOG_note    ("requested clear only");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(paste in order)-----------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_regs [s_reg].nbuf);
   x_curr = s_regs [s_reg].hbuf;
   --rce;  while (x_curr != NULL) {
      DEBUG_CMDS   yLOG_complex ("item"      , "%-10p, %-10p, %-10p, %-10p", x_curr, x_curr->data, x_curr->b_prev, x_curr->b_next);
      rc = s_paster (s_reqs, s_pros, s_intg, a_1st, s_boff, s_xoff, s_yoff, s_zoff, x_curr->data);
      if (rc < 0) {
         DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      ++c;
      a_1st = '-';
      x_curr = x_curr->b_next;
   }
   /*---(update)-------------------------*/
   yvikeys_map_reposition  ();
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  yvikeys_mreg_paste       (char *a_type) { yvikeys_mreg__paste ('y', a_type); }
char  yvikeys_mreg_paste_combo (char *a_type) { yvikeys_mreg__paste ('-', a_type); }

char
yvikeys_mreg_visual          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_reg       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = yvikeys_mreg__paste_check    ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(select)-------------------------*/
   yvikeys_visu_exact (
         s_regs [x_reg].b_all + s_boff,
         s_regs [x_reg].x_beg + s_xoff, s_regs [x_reg].x_end + s_xoff,
         s_regs [x_reg].y_beg + s_yoff, s_regs [x_reg].y_end + s_yoff,
         s_regs [x_reg].z_all + s_zoff);
   yVIKEYS_jump (
         s_regs [x_reg].b_all + s_boff,
         s_regs [x_reg].x_end + s_xoff,
         s_regs [x_reg].y_end + s_yoff,
         s_regs [x_reg].z_all + s_zoff);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg_status          (char *a_status)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_list      [LEN_RECD]  = "";
   int         x_reg       =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_status  == NULL)  return rce;
   /*---(identify register)--------------*/
   x_reg  = yvikeys_mreg__index  (s_creg);
   --rce;  if (x_reg < 0) {
      return  rce;
   }
   /*---(status)-------------------------*/
   snprintf (a_status, 500, "reg %c %2d %s ", s_creg, s_regs [x_reg].nbuf, s_regs [x_reg].labels);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_mreg_smode           (int a_major, int a_minor)
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
   --rce;  if (MODE_not (SMOD_MREG)) {
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
   /*---(register selection)-------------*/
   --rce;  if (a_major == '"') {
      if (strchr ("+-", a_minor) != NULL) {
         DEBUG_USER   yLOG_note    ("enter export-import");
         yvikeys_mreg__set    (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      } else if (strchr (S_REG_LIST, a_minor) != 0) {
         yvikeys_mreg__set    (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      } else if (a_minor == '?') {
         /*> my.info_win = G_INFO_REGS;                                               <*/
         /*> REG_set ('"');                                                           <*/
         /*> yVIKEYS_mode_exit ();                                                          <*/
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return  0;
      } else if (a_minor == '!') {
         /*> my.layout_status = G_STATUS_REGS;                                        <*/
         /*> REG_set ('"');                                                           <*/
         /*> yVIKEYS_mode_exit ();                                                    <*/
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return  0;
      }
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(register actions)---------------*/
   --rce;  if (a_major == ' ') {
      /*---(multikey prefixes)-----------*/
      switch (a_minor) {
         /*---(multikey prefixes)-----------*/
      case 'p'  :
         DEBUG_USER   yLOG_note    ("p for paste normal");
         yvikeys_mreg_paste ("normal");
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'P'  :
         DEBUG_USER   yLOG_note    ("P is a multi-key");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      case 'y'  :
         DEBUG_USER   yLOG_note    ("y for yank/copy");
         yvikeys_mreg_save  ();
         yvikeys_visu_clear ();
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'Y'  :
         DEBUG_USER   yLOG_note    ("y for yank/clear");
         yvikeys_mreg_save  ();
         yvikeys_mreg_clear ();
         yvikeys_visu_clear ();
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'x'  :
         DEBUG_USER   yLOG_note    ("x for clear");
         yvikeys_mreg_clear ();
         yvikeys_visu_clear ();
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      }
   }
   /*---(export-import)------------------*/
   --rce;  if (strchr ("+-", a_major) != NULL) {
      DEBUG_USER   yLOG_note    ("calling external export-import function");
      DEBUG_USER   yLOG_point   ("s_exim"    , s_exim);
      if (s_exim != NULL)  rc = s_exim (a_major, a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      yvikeys_visu_clear ();
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(pasting actions)----------------*/
   --rce;  if (a_major == 'P') {
      switch (a_minor) {
      case '_' :  rc = yvikeys_mreg_visual ();            break;
      case '#' :  rc = yvikeys_mreg_paste  ("clear");     break;
      case 'n' :  rc = yvikeys_mreg_paste  ("normal");    break;
      case 'r' :  rc = yvikeys_mreg_paste  ("replace");   break;
      case 'd' :  rc = yvikeys_mreg_paste  ("duplicate"); break;
      case 'c' :  rc = yvikeys_mreg_paste  ("combo");     break;
      case 'm' :  rc = yvikeys_mreg_paste  ("move");      break;
      case 'f' :  rc = yvikeys_mreg_paste  ("force");     break;
      default  :  rc = rce;                               break;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
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
static void  o___UNITTEST________o () { return; }

typedef  struct cTHING  tTHING;
struct cTHING {
   int         x, y, z;
   char        l           [LEN_LABEL];
   tTHING     *n;
};
static tTHING  *s_things  [10][10];
static tTHING  *s_head    = NULL;
static tTHING  *s_tail    = NULL;
static int      s_nthing  =    0;
static int      s_nbase   =    0;
static int      s_nfree   =    0;

tTHING*
yvikeys_mreg__unit_create       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tTHING     *p           = NULL;
   /*---(create)-------------------------*/
   p = (tTHING *) malloc (sizeof (tTHING));
   /*---(link)---------------------------*/
   p->n = NULL;
   if (s_head == NULL) {
      s_head   = s_tail   = p;
      s_nthing = 1;
   } else {
      s_tail->n = p;
      s_tail    = p;
      ++s_nthing;
   }
   /*---(complete)-----------------------*/
   return p;
}

char
yvikeys_mreg__unit_orig         (char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tTHING     *p           = NULL;
   char        t           [LEN_LABEL] = "";
   strlcpy (a_list, ",", LEN_RECD);
   p = s_head;
   s_nbase = 0;
   for (i = 0; i < s_nthing; ++i) {
      if (p->z == 1) {
         sprintf (t, "%s,", p->l);
         strlcat (a_list, t, LEN_RECD);
         ++s_nbase;
      }
      p = p->n;
   }
   return 0;
}

char
yvikeys_mreg__unit_regs         (char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tTHING     *p           = NULL;
   char        t           [LEN_LABEL] = "";
   strlcpy (a_list, ",", LEN_RECD);
   p = s_head;
   s_nfree = 0;
   for (i = 0; i < s_nthing; ++i) {
      if (p->z == 2) {
         sprintf (t, "%s,", p->l);
         strlcat (a_list, t, LEN_RECD);
         ++s_nfree;
      }
      p = p->n;
   }
   return 0;
}

char
yvikeys_mreg__unit_adds         (char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tTHING     *p           = NULL;
   char        t           [LEN_LABEL] = "";
   strlcpy (a_list, ",", LEN_RECD);
   p = s_head;
   for (i = 0; i < s_nthing; ++i) {
      if (p->z == 0) {
         sprintf (t, "%s,", p->l);
         strlcat (a_list, t, LEN_RECD);
      }
      p = p->n;
   }
   return 0;
}

char
yvikeys_mreg__unit_A       (char *a_list)
{
   int    x, y;
   strlcpy (a_list, "", LEN_RECD);
   for (y = 0; y < 4; ++y) {
      strlcat (a_list, " ", LEN_RECD);
      for (x = 0; x < 10; ++x) {
         if      (s_things [x][y] == NULL)  strlcat (a_list, "-", LEN_RECD);
         else if (s_things [x][y]->z == 1)  strlcat (a_list, "X", LEN_RECD);
         else if (s_things [x][y]->z == 0)  strlcat (a_list, "n", LEN_RECD);
      }
   }
}

char
yvikeys_mreg__unit_B            (char *a_list)
{
   int    x, y;
   strlcpy (a_list, "", LEN_RECD);
   for (y = 4; y < 8; ++y) {
      strlcat (a_list, " ", LEN_RECD);
      for (x = 0; x < 10; ++x) {
         if      (s_things [x][y] == NULL)  strlcat (a_list, "-", LEN_RECD);
         else if (s_things [x][y]->z == 1)  strlcat (a_list, "X", LEN_RECD);
         else if (s_things [x][y]->z == 0)  strlcat (a_list, "n", LEN_RECD);
      }
   }
}

char
yvikeys_mreg__unit_purge   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tTHING     *p           = NULL;
   tTHING     *n           = NULL;
   int         x, y;
   /*---(create)-------------------------*/
   p = s_head;
   /*---(wipe)---------------------------*/
   for (i = 0; i < s_nthing; ++i) {
      n = p->n;
      free (p);
      p = n;
   }
   s_nthing = 0;
   s_head   = NULL;
   s_tail   = NULL;
   /*---(clear matrix)-------------------*/
   for (x = 0; x < 10; ++x) {
      for (y = 0; y < 10; ++y) {
         s_things [x][y] = NULL;
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_mreg__unit_regkill (void *a_thing)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   tTHING     *c           = NULL;
   tTHING     *p           = NULL;
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_value   ("s_nthing"  , s_nthing);
   c  = s_head;
   p  = s_head;
   for (i = 0; i < s_nthing; ++i) {
      DEBUG_REGS   yLOG_value   ("pos"       , i);
      DEBUG_REGS   yLOG_info    ("label"     , c->l);
      /*---(not found)-------------------*/
      if (c != a_thing) {
         DEBUG_REGS   yLOG_note    ("wrong one, move to next");
         p = c;
         c = c->n;
         continue;
      }
      /*---(found)-----------------------*/
      if (s_nthing == 1) {
         DEBUG_REGS   yLOG_note    ("only one entry");
         s_head = s_tail = NULL;
      } else if (c == s_head) {
         DEBUG_REGS   yLOG_note    ("change head");
         s_head = c->n;
      } else if (c == s_tail) {
         DEBUG_REGS   yLOG_note    ("change tail");
         s_tail = p;
         p->n  = NULL;
      } else {
         DEBUG_REGS   yLOG_note    ("change middle");
         p->n  = c->n;
      }
      /*---(common)----------------------*/
      free (a_thing);
      --s_nthing;
      break;
   }
   DEBUG_REGS   yLOG_value   ("s_nthing"  , s_nthing);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

tTHING*
yvikeys_mreg__unit_dup          (tTHING *p)
{
   /*---(locals)-----------+-----+-----+-*/
   tTHING     *q           = NULL;
   char        t           [LEN_LABEL];
   /*---(create)-------------------------*/
   q = yvikeys_mreg__unit_create ();
   /*---(copy contents)------------------*/
   q->x = p->x;
   q->y = p->y;
   q->z = 2;
   sprintf (t, "%d%d", p->x, p->y);
   strlcpy (q->l, t, LEN_LABEL);
   /*---(complete)-----------------------*/
   return q;
}

char
yvikeys_mreg__unit_hook         (tTHING *p, int x, int y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_LABEL];
   /*---(check)--------------------------*/
   if (s_things [x][y] != NULL) {
      s_things [x][y] = NULL;
      yvikeys_mreg__unit_regkill (s_things [x][y]);
   }
   /*---(populate)-----------------------*/
   s_things [x][y] = p;
   p->x = x;
   p->y = y;
   p->z = 0;
   sprintf (t, "%d%d", x, y);
   strlcpy (p->l, t, LEN_LABEL);
   /*---(complete)-----------------------*/
   return 0;
}

char yvikeys_mreg__unit_mark_orig    (tTHING *p) { p->z = 1; return 0; }


char
yvikeys_mreg__unit_init    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tTHING     *p           = NULL;
   /*---(purge)--------------------------*/
   yvikeys_mreg__unit_purge ();
   /*---(3 secondary)--------------------*/
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 0, 5);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 3, 6);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 6, 7);
   yvikeys_mreg__unit_mark_orig (p);
   /*---(5 primary)----------------------*/
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 2, 0);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 0, 2);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 3, 2);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 6, 2);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 6, 4);
   yvikeys_mreg__unit_mark_orig (p);
   /*---(4 inner)------------------------*/
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 2, 1);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 2, 3);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 4, 1);
   yvikeys_mreg__unit_mark_orig (p);
   p = yvikeys_mreg__unit_create ();
   yvikeys_mreg__unit_hook   (p, 4, 3);
   yvikeys_mreg__unit_mark_orig (p);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_mreg__unit_copier  (char a_type, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   tTHING     *x_thing     = NULL;
   tTHING     *x_new       = NULL;
   int         b           =    0;
   int         x           =    0;
   int         y           =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   rc = yVIKEYS_first (&b, &x, &y, NULL);
   while (rc >= 0) {
      x_thing = s_things [x][y];
      if (x_thing != NULL) {
         rc = yVIKEYS_visual (b, x, y, 0);
         if (rc == 1) {
            x_new = yvikeys_mreg__unit_dup (x_thing);
            yVIKEYS_mreg_add (x_new, x_new->l);
         }
      }
      rc = yVIKEYS_next  (&b, &x, &y, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_mreg__unit_clearer (char a_1st, int b, int x, int y, int z)
{
   tTHING     *x_thing     = NULL;
   if (s_things [x][y] == NULL)  return 0;
   s_regkill (s_things [x][y]);
   s_things [x][y] = NULL;
   return 0;
}

char
yvikeys_mreg__unit_paster  (char a_regs, char a_pros, char a_intg, char a_1st, int a_boff, int a_xoff, int a_yoff, int a_zoff, void *a_thing)
{
   tTHING     *x_thing;
   int         x, y;
   x_thing = yvikeys_mreg__unit_dup (a_thing);
   x = x_thing->x + a_xoff;
   y = x_thing->y + a_yoff;
   yvikeys_mreg__unit_hook (x_thing, x, y);
   return 0;
}

char
yvikeys_mreg__unit_config  (void)
{
   char        rc          =    0;
   rc = yVIKEYS_mreg_config (yvikeys_mreg__unit_clearer, yvikeys_mreg__unit_copier, yvikeys_mreg__unit_paster, yvikeys_mreg__unit_regkill, NULL);
   return rc;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_mreg__unit      (char *a_question, char x, char y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD ];
   int         x_reg       =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP_REG unit     : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "current"        )   == 0) {
      x_reg = yvikeys_mreg__index (s_creg);
      yvikeys_mreg__list  (s_creg, t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG current  : %c %2d %2d %s", s_creg, x_reg, s_regs [x_reg].nbuf, t);
   }
   else if (strcmp (a_question, "orig"           )   == 0) {
      yvikeys_mreg__unit_orig (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG orig  %2d : %s", s_nbase, t);
   }
   else if (strcmp (a_question, "regs"           )   == 0) {
      yvikeys_mreg__unit_regs (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG regs  %2d : %s", s_nfree, t);
   }
   else if (strcmp (a_question, "adds"           )   == 0) {
      yvikeys_mreg__unit_adds (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG adds  %2d : %s", s_nthing - s_nbase - s_nfree, t);
   }
   else if (strcmp (a_question, "A"              )   == 0) {
      yvikeys_mreg__unit_A    (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "A : %s", t);
   }
   else if (strcmp (a_question, "B"              )   == 0) {
      yvikeys_mreg__unit_B    (t);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "B : %s", t);
   }
   else if (strcmp (a_question, "inside"       )  == 0) {
      yvikeys_mreg__list  (x, t);
      x_reg = yvikeys_mreg__index (x);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG inside   : %c %2d %s", x, s_regs [x_reg].nbuf, t);
   }
   else if (strcmp (a_question, "range"        )  == 0) {
      x_reg = yvikeys_mreg__index (x);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP_REG range    : %c, bx=%4d, by=%4d, ex=%4d, ey=%4d, z =%4d", x, s_regs [x_reg].x_beg, s_regs [x_reg].y_beg, s_regs [x_reg].x_end, s_regs [x_reg].y_end, s_regs [x_reg].z_all);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



