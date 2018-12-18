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

#define     MAX_REG     100
#define     LEN_BUF    1000

typedef  struct  cREG   tREG;
struct cREG {
   /*---(#1, ORIGIN TAB)-----------------*/
   /*   stores the tab number of the original selection so that cell          */
   /*   references can be properly adjusted when placed back into a tab.      */
   int         z_all;
   /*---(#2, MINIMUM REACH)--------------*/
   /*   stores the column and row of the furthest upper-left cell reference   */
   /*   within a formula of a register cell (for error-checking).             */
   int         x_min;
   int         y_min;
   /*---(#3, MINIMUM BOUND)--------------*/
   /*   stores the column and row of the upper-left corner of the selection   */
   /*   used to source the cells for the register.                            */
   int         x_beg;
   int         y_beg;
   /*---(#4, MAXIMUM BOUND)--------------*/
   /*   stores the column and row of the lower-right corner of the selection  */
   /*   used to source the cells for the register.                            */
   int         x_end;
   int         y_end;
   /*---(#5, MAXIMUM REACH)--------------*/
   /*   stores the column and row of the furthest lower-right cell reference  */
   /*   within a formula of a register cell (for error-checking).             */
   int         x_max;
   int         y_max;
   /*---(#6, CELLS)----------------------*/
   /*   stores the an array of cells within the register in the order that    */
   /*   they need to be placed back into a tab to correctly calculate.        */
   int         nbuf;                             /* total spots               */
   void       *buf         [LEN_BUF ];           /* cell pointers             */
   char        notes       [LEN_BUF ];           /* source coding             */
   char        labels      [LEN_RECD];           /* label list                */
   int         real;                             /* filled spots              */
   /*---(#7, SPECIAL)--------------------*/
   /*   the type flag indicates whether non-selection formula cells with      */
   /*   references into the selection should be adjusted when pasted.         */
   char        type;
   /*---(end)----------------------------*/
};
static      tREG        s_regs       [MAX_REG];
static      int         s_nreg       =    0;
static      char        s_creg       =  '-';
static      char        s_regnames   [MAX_REG] = "\"abcdefghijklmnopqrstuvwxyz-+0123456789";

#define     S_REG_EMPTY     '-'
#define     S_REG_ACTIVE    'y'

/* destroy a copy held in a register --------------------------*/
static char    (*s_regkill)    (void *a_thing);
/* get a copy and put it into a register ----------------------*/
static void*   (*s_copier)     (char a_type, long a_stamp);
/* clear an area in the host application ----------------------*/
static char    (*s_clearer)    (char a_1st, int x, int y, int z);
/* integrate something into the host application --------------*/
static char    (*s_paster)     (char a_regs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, void *a_thing);
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
static tPASTING   s_pasting [MAX_PASTING] = {
   /*-a- --ref-- ---name-------- -pr-    --c- --r- --p- --i- ---desc--- */
   { '-', "----", ""            , '-',    '-', '-', '-', '-',    "tbd" },
   /*---*/
   { '-', "-nN-", ""            , '-',    '-', 'n', 'N', '-',    "tbd" },
   { '-', "-nR-", ""            , '-',    '-', 'n', 'R', '-',    "tbd" },
   { '-', "-nA-", ""            , '-',    '-', 'n', 'A', '-',    "tbd" },
   { '-', "-nS-", ""            , '-',    '-', 'n', 'S', '-',    "tbd" },
   /*---*/
   { '-', "-rN-", "normal"      , 'y',    '-', 'r', 'N', '-',    "tbd" },
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
   { '-', "y---", "clear"       , 'y',    'y', '-', '-', '-',    "tbd" },
   /*---*/
   { '-', "ynN-", ""            , '-',    'y', 'n', 'N', '-',    "tbd" },
   { '-', "ynR-", ""            , '-',    'y', 'n', 'R', '-',    "tbd" },
   { '-', "ynA-", ""            , '-',    'y', 'n', 'A', '-',    "tbd" },
   { '-', "ynS-", ""            , '-',    'y', 'n', 'S', '-',    "tbd" },
   /*---*/
   { '-', "yrN-", "replace"     , 'y',    'y', 'r', 'N', '-',    "tbd" },
   { '-', "yrR-", ""            , '-',    'y', 'r', 'R', '-',    "tbd" },
   { '-', "yrA-", ""            , '-',    'y', 'r', 'A', '-',    "tbd" },
   { '-', "yrS-", ""            , '-',    'y', 'r', 'S', '-',    "tbd" },
   /*---*/
   { '-', "yiN-", "duplicate"   , 'y',    'y', 'i', 'N', '-',    "tbd" },
   { '-', "yiR-", ""            , '-',    'y', 'i', 'R', '-',    "tbd" },
   { '-', "yiA-", ""            , '-',    'y', 'i', 'A', '-',    "tbd" },
   { '-', "yiS-", ""            , '-',    'y', 'i', 'S', '-',    "tbd" },
   /*---*/
   { '-', "ybN-", ""            , '-',    'y', 'b', 'N', '-',    "tbd" },
   { '-', "ybR-", "move"        , 'y',    'y', 'b', 'R', '-',    "tbd" },
   { '-', "ybA-", "force"       , 'y',    'y', 'b', 'A', '-',    "tbd" },
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

char         /*-> find a register index by abbr ------[ leaf   [fn.540.134.80]*/ /*-[00.0000.0L3.O]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_by_abbr           (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char       *x_loc       = NULL;
   static char x_prev      =    0;
   static int  x_index     =   -1;
   /*---(short-cut)----------------------*/
   if (a_reg == x_prev)  return x_index;
   x_prev    =    0;
   x_index   =   -1;
   /*---(defense)------------------------*/
   --rce;  if (a_reg == 0)                            return rce;
   x_loc     = strchr (s_regnames, a_reg);
   --rce;  if (x_loc == NULL)                         return rce;
   /*---(buffer number)------------------*/
   x_index   = (int) (x_loc - s_regnames);
   /*---(defense on range)---------------*/
   --rce; if (x_index <  0      )                     return rce;
   --rce; if (x_index >= MAX_REG)                     return rce;
   /*---(save)---------------------------*/
   x_prev    = a_reg;
   /*---(complete)-----------------------*/
   return x_index;
}

int          /*-> convert a register abbr to tab -----[ ------ [fn.420.132.11]*/ /*-[00.0000.016.7]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_abbr2tab          (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        x_tab       =    0;
   /*---(get register index)-------------*/
   x_tab = yvikeys__regs_by_abbr (a_reg);
   if (x_tab < 0)  return x_tab;
   /*---(complete)-----------------------*/
   return x_tab + 128;
}

char
yvikeys__regs_reset             (void)
{
   /*---(reset register)-----------------*/
   s_creg = '"';
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys__regs_set               (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   /*---(reset)--------------------------*/
   yvikeys__regs_reset ();
   /*---(get register number)------------*/
   x_reg  = yvikeys__regs_by_abbr  (a_reg);
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
yVIKEYS_regs_inside     (int x, int y, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_reg       = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(get register number)------------*/
   x_reg  = yvikeys__regs_by_abbr  (s_creg);
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
/*===----                   initialization and wrapup                  ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> clear out a register ---------------[ ------ [ge.B63.253.32]*/ /*-[03.0000.043.3]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_wipe              (char a_reg, char a_init)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   int         i           = 0;
   void       *x_thing     = NULL;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   DEBUG_REGS   yLOG_char    ("a_init"    , a_init);
   /*---(get register number)------------*/
   x_reg  = yvikeys__regs_by_abbr  (a_reg);
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
   s_regs [x_reg].nbuf   = 0;
   s_regs [x_reg].real   = 0;
   /*---(cells)--------------------------*/
   DEBUG_REGS   yLOG_note    ("clear all register positions");
   --rce;
   for (i = 0; i < LEN_BUF; ++i) {
      x_thing = s_regs [x_reg].buf [i];
      if (s_regkill != NULL && a_init != 'y' && x_thing != NULL) {
         rc = s_regkill (x_thing);
         if (rc < 0) {
            DEBUG_REGS   yLOG_note    ("found a bad register position");
            DEBUG_REGS   yLOG_value   ("posid"     , i);
            DEBUG_REGS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
      }
      s_regs [x_reg].buf  [i] = NULL;
      s_regs [x_reg].notes[i] = '-';
   }
   strlcpy (s_regs [x_reg].labels, ",", LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear out all buffers --------------[ ------ [gz.421.121.01]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_purge             (char a_init)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   s_creg = '"';
   s_nreg = strlen (s_regnames);
   for (i = 0; i < s_nreg; ++i) {
      yvikeys__regs_wipe (s_regnames[i], a_init);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
yvikeys_regs_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_MAP_REG)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(registers)----------------------*/
   yvikeys__regs_purge    ('y');
   s_regkill  = NULL;
   s_copier  = NULL;
   s_clearer = NULL;
   s_paster  = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_MAP_REG);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
yvikeys_regs_wrap               (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(registers)----------------------*/
   yvikeys__regs_purge    ('y');
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_regs_config       (void *a_clearer, void *a_copier, void *a_paster, void *a_regkill, void *a_exim)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_REGS  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_MAP_REG)) {
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
   STATUS_conf_set   (SMOD_MAP_REG, '1');
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
yVIKEYS_regs_add        (void *a_thing, char *a_label, char a_note)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       =   0;
   int         x_nbuf      =   0;
   char        t           [LEN_LABEL]  = "";
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
   x_reg  = yvikeys__regs_by_abbr  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   x_nbuf = s_regs [x_reg].nbuf;
   DEBUG_REGS   yLOG_value   ("x_nbuf"    , x_nbuf);
   --rce;  if (x_nbuf >= LEN_BUF) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(attach)-------------------------*/
   s_regs [x_reg].buf   [x_nbuf] = a_thing;
   s_regs [x_reg].notes [x_nbuf] = a_note;
   DEBUG_REGS   yLOG_point   ("a_label"   , a_label);
   if (a_label != NULL) {
      DEBUG_REGS   yLOG_info    ("a_label"   , a_label);
      sprintf (t, "%s,", a_label);
      strlcat (s_regs [x_reg].labels, t, LEN_RECD);
   }
   /*---(counters)-----------------------*/
   ++s_regs [x_reg].nbuf;
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_regs [x_reg].nbuf);
   ++s_regs [x_reg].real;
   DEBUG_REGS   yLOG_value   ("real"      , s_regs [x_reg].real);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_regs_save               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_reg       =    0;
   int         x_nbuf      =    0;
   long        x_stamp     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MAP_REG)) {
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
   x_reg  = yvikeys__regs_by_abbr  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(wipe register)------------------*/
   rc = yvikeys__regs_wipe  (s_creg, '-');
   DEBUG_REGS   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(save selection)-----------------*/
   rc = VISU_range (
         &s_regs [x_reg].x_beg, &s_regs [x_reg].x_end,
         &s_regs [x_reg].y_beg, &s_regs [x_reg].y_end,
         &s_regs [x_reg].z_all);
   DEBUG_REGS   yLOG_value   ("visu rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
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
   if (s_regs [x_reg].nbuf <= 0)  yvikeys__regs_wipe (s_creg, '-');
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__regs_list            (char a_reg, char *a_list)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_reg       =    0;
   /*---(identify register)--------------*/
   strlcpy (a_list, "?"                  , LEN_RECD);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   x_reg  = yvikeys__regs_by_abbr  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   strlcpy (a_list, s_regs [x_reg].labels, LEN_RECD);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        major actions                         ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char
yvikeys_regs_clear           (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y;
   int         x_beg, x_end;
   int         y_beg, y_end;
   int         z;
   char        x_1st       =  'y';
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MAP_REG)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save selection)-----------------*/
   rc = VISU_range (&x_beg, &x_end, &y_beg, &y_end, &z);
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
         rc = s_clearer (x_1st, x, y, z);
         if (rc == 0)  x_1st = '-';
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

static int  s_xoff  = 0;
static int  s_yoff  = 0;
static int  s_zoff  = 0;
static int  s_reg   = 0;
static char s_clear = 0;
static char s_reqs  = 0;
static char s_pros  = 0;
static char s_intg  = 0;

char         /*-> prepare for a paste ----------------[ ------ [fe.842.023.21]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_paste_check    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_nbuf      =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_MAP_REG)) {
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
   s_reg  = yvikeys__regs_by_abbr  (s_creg);
   DEBUG_REGS   yLOG_value   ("s_reg"     , s_reg);
   --rce;  if (s_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(check for empty)----------------*/
   x_nbuf = s_regs [s_reg].nbuf;
   DEBUG_REGS   yLOG_value   ("x_nbuf"    , x_nbuf);
   --rce;  if (x_nbuf <= 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(get current position)-----------*/
   yvikeys_map_current (&s_xoff, &s_yoff, &s_zoff);
   DEBUG_REGS   yLOG_value   ("s_xoff"    , s_xoff);
   DEBUG_REGS   yLOG_value   ("s_yoff"    , s_yoff);
   DEBUG_REGS   yLOG_value   ("s_zoff"    , s_zoff);
   /*---(make into offsets)--------------*/
   s_xoff -=  s_regs [s_reg].x_beg;
   s_yoff -=  s_regs [s_reg].y_beg;
   s_zoff -=  s_regs [s_reg].z_all;
   DEBUG_REGS   yLOG_value   ("s_xoff (*)", s_xoff);
   DEBUG_REGS   yLOG_value   ("s_yoff (*)", s_yoff);
   DEBUG_REGS   yLOG_value   ("s_zoff (*)", s_zoff);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> prepare for a paste ----------------[ ------ [fe.842.023.21]*/ /*-[01.0000.015.!]-*/ /*-[--.---.---.--]-*/
yvikeys__regs_paste_settings (char *a_type)
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
yvikeys__regs_paste_clear    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y;
   char        x_1st       =  'y';
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
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
         DEBUG_REGS   yLOG_complex ("target"    , "%c, %3dx, %3dy, %3dz", x_1st, x + s_xoff, y + s_yoff, s_regs [s_reg].z_all + s_zoff);
         rc = s_clearer (x_1st, x + s_xoff, y + s_yoff, s_regs [s_reg].z_all + s_zoff);
         if (rc == 0)  x_1st = '-';
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_regs_paste              (char *a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x, y, z;
   int         i           =    0;
   char        x_1st       =  'y';
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = yvikeys__regs_paste_check    ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(settings)-----------------------*/
   rc = yvikeys__regs_paste_settings (a_type);
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clearing)-----------------------*/
   if (s_clear == 'y') {
      yvikeys__regs_paste_clear ();
      x_1st = '-';  /* link pasting to clearing */
   }
   if (s_reqs == '-') {
      DEBUG_REGS   yLOG_note    ("requested clear only");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(paste in order)-----------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_regs [s_reg].nbuf);
   --rce;  for (i = 0; i < s_regs [s_reg].nbuf; ++i) {
      DEBUG_REGS   yLOG_value   ("i"         , i);
      rc = s_paster (s_reqs, s_pros, s_intg, x_1st, s_xoff, s_yoff, s_zoff, s_regs [s_reg].buf [i]);
      if (rc < 0) {
         DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_1st = '-';
   }
   /*---(update)-------------------------*/
   yvikeys_map_reposition  ();
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_regs_visual          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_reg       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = yvikeys__regs_paste_check    ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(select)-------------------------*/
   VISU_exact (
         s_regs [x_reg].x_beg + s_xoff, s_regs [x_reg].x_end + s_xoff,
         s_regs [x_reg].y_beg + s_yoff, s_regs [x_reg].y_end + s_yoff,
         s_regs [x_reg].z_all + s_zoff);
   yVIKEYS_jump (
         s_regs [x_reg].x_end + s_xoff,
         s_regs [x_reg].y_end + s_yoff,
         s_regs [x_reg].z_all + s_zoff);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
yvikeys_regs_status          (char *a_status)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_list      [LEN_RECD]  = "";
   int         x_reg       =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_status  == NULL)  return rce;
   /*---(identify register)--------------*/
   x_reg  = yvikeys__regs_by_abbr  (s_creg);
   --rce;  if (x_reg < 0) {
      return  rce;
   }
   /*---(status)-------------------------*/
   snprintf (a_status, 500, "reg %c %2d %s ", s_creg, s_regs [x_reg].nbuf, s_regs [x_reg].labels);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_regs_smode           (int a_major, int a_minor)
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
   --rce;  if (MODE_not (SMOD_MAP_REG)) {
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
         yvikeys__regs_set    (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      } else if (strchr (s_regnames, a_minor) != 0) {
         yvikeys__regs_set    (a_minor);
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
         yvikeys_regs_paste ("normal");
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
         yvikeys_regs_save  ();
         VISU_clear   ();
         MODE_exit ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case 'Y'  :
         DEBUG_USER   yLOG_note    ("y for yank/clear");
         yvikeys_regs_save  ();
         yvikeys_regs_clear ();
         VISU_clear   ();
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
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(pasting actions)----------------*/
   --rce;  if (a_major == 'P') {
      switch (a_minor) {
      case '_' :  rc = yvikeys_regs_visual ();            break;
      case '#' :  rc = yvikeys_regs_paste  ("clear");     break;
      case 'n' :  rc = yvikeys_regs_paste  ("normal");    break;
      case 'r' :  rc = yvikeys_regs_paste  ("replace");   break;
      case 'd' :  rc = yvikeys_regs_paste  ("duplicate"); break;
      case 'm' :  rc = yvikeys_regs_paste  ("move");      break;
      case 'f' :  rc = yvikeys_regs_paste  ("force");     break;
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
yvikeys__unit_regs_create       (void)
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
yvikeys__unit_regs_base         (char *a_list)
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
yvikeys__unit_regs_free         (char *a_list)
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
yvikeys__unit_regs_list         (char *a_list)
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
yvikeys__unit_regs_A       (char *a_list)
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
yvikeys__unit_regs_B            (char *a_list)
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
yvikeys__unit_regs_purge   (void)
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
yvikeys__unit_regs_regkill (void *a_thing)
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
yvikeys__unit_regs_dup          (tTHING *p)
{
   /*---(locals)-----------+-----+-----+-*/
   tTHING     *q           = NULL;
   char        t           [LEN_LABEL];
   /*---(create)-------------------------*/
   q = yvikeys__unit_regs_create ();
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
yvikeys__unit_regs_hook         (tTHING *p, int x, int y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_LABEL];
   /*---(check)--------------------------*/
   if (s_things [x][y] != NULL) {
      s_things [x][y] = NULL;
      yvikeys__unit_regs_regkill (s_things [x][y]);
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

char yvikeys__unit_regs_makebase     (tTHING *p) { p->z = 1; return 0; }


char
yvikeys__unit_regs_init    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tTHING     *p           = NULL;
   /*---(purge)--------------------------*/
   yvikeys__unit_regs_purge ();
   /*---(3 secondary)--------------------*/
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 0, 5);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 3, 6);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 6, 7);
   yvikeys__unit_regs_makebase (p);
   /*---(5 primary)----------------------*/
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 2, 0);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 0, 2);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 3, 2);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 6, 2);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 6, 4);
   yvikeys__unit_regs_makebase (p);
   /*---(4 inner)------------------------*/
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 2, 1);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 2, 3);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 4, 1);
   yvikeys__unit_regs_makebase (p);
   p = yvikeys__unit_regs_create ();
   yvikeys__unit_regs_hook   (p, 4, 3);
   yvikeys__unit_regs_makebase (p);
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys__unit_regs_copier  (char a_type, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   tTHING     *x_thing     = NULL;
   tTHING     *x_new       = NULL;
   int         x           =    0;
   int         y           =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   rc = yVIKEYS_first (&x, &y, NULL);
   while (rc >= 0) {
      x_thing = s_things [x][y];
      if (x_thing != NULL) {
         rc = yVIKEYS_visual (x, y, 0);
         if (rc == 1) {
            x_new = yvikeys__unit_regs_dup (x_thing);
            yVIKEYS_regs_add (x_new, x_new->l, '-');
         }
      }
      rc = yVIKEYS_next  (&x, &y, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys__unit_regs_clearer (char a_1st, int x, int y, int z)
{
   tTHING     *x_thing     = NULL;
   if (s_things [x][y] == NULL)  return 0;
   s_regkill (s_things [x][y]);
   s_things [x][y] = NULL;
   return 0;
}

char
yvikeys__unit_regs_paster  (char a_regs, char a_pros, char a_intg, char a_1st, int a_xoff, int a_yoff, int a_zoff, void *a_thing)
{
   tTHING     *x_thing;
   int         x, y;
   x_thing = yvikeys__unit_regs_dup (a_thing);
   x = x_thing->x + a_xoff;
   y = x_thing->y + a_yoff;
   yvikeys__unit_regs_hook (x_thing, x, y);
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__unit_regs      (char *a_question, char x, char y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD ];
   int         x_reg       =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP_REG unit     : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "current"        )   == 0) {
      x_reg = yvikeys__regs_by_abbr (s_creg);
      yvikeys__regs_list  (s_creg, t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG current  : %c %2d %2d %s", s_creg, x_reg, s_regs [x_reg].nbuf, t);
   }
   else if (strcmp (a_question, "base"           )   == 0) {
      yvikeys__unit_regs_base (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG base  %2d : %s", s_nbase, t);
   }
   else if (strcmp (a_question, "free"           )   == 0) {
      yvikeys__unit_regs_free (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG free  %2d : %s", s_nfree, t);
   }
   else if (strcmp (a_question, "list"           )   == 0) {
      yvikeys__unit_regs_list (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG list  %2d : %s", s_nthing - s_nbase - s_nfree, t);
   }
   else if (strcmp (a_question, "A"              )   == 0) {
      yvikeys__unit_regs_A    (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "A : %s", t);
   }
   else if (strcmp (a_question, "B"              )   == 0) {
      yvikeys__unit_regs_B    (t);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "B : %s", t);
   }
   else if (strcmp (a_question, "inside"       )  == 0) {
      yvikeys__regs_list  (x, t);
      x_reg = yvikeys__regs_by_abbr (x);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG inside   : %c %2d %s", x, s_regs [x_reg].nbuf, t);
   }
   else if (strcmp (a_question, "range"        )  == 0) {
      x_reg = yvikeys__regs_by_abbr (x);
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP_REG range    : %c, bx=%4d, by=%4d, ex=%4d, ey=%4d, z =%4d", x, s_regs [x_reg].x_beg, s_regs [x_reg].y_beg, s_regs [x_reg].x_end, s_regs [x_reg].y_end, s_regs [x_reg].z_all);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



