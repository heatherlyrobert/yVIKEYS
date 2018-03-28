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
   int         otab;
   /*---(#2, MINIMUM REACH)--------------*/
   /*   stores the column and row of the furthest upper-left cell reference   */
   /*   within a formula of a register cell (for error-checking).             */
   int         minc;
   int         minr;
   /*---(#3, MINIMUM BOUND)--------------*/
   /*   stores the column and row of the upper-left corner of the selection   */
   /*   used to source the cells for the register.                            */
   int         begc;
   int         begr;
   /*---(#4, MAXIMUM BOUND)--------------*/
   /*   stores the column and row of the lower-right corner of the selection  */
   /*   used to source the cells for the register.                            */
   int         endc;
   int         endr;
   /*---(#5, MAXIMUM REACH)--------------*/
   /*   stores the column and row of the furthest lower-right cell reference  */
   /*   within a formula of a register cell (for error-checking).             */
   int         maxc;
   int         maxr;
   /*---(#6, CELLS)----------------------*/
   /*   stores the an array of cells within the register in the order that    */
   /*   they need to be placed back into a tab to correctly calculate.        */
   int         nbuf;                             /* total spots               */
   void       *buf         [LEN_BUF];            /* cell pointers             */
   char        notes       [LEN_BUF];            /* source coding             */
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


/*---(destroys object in application)------------*/
static char    (*s_killer)  (void *a_thing);
/*---(clear, copy, cut, etc)---------------------*/
static void*   (*s_copier)  (char a_type, int a_x1, int a_x2, int a_y1, int a_y2, int a_z);
/*---(place, adjust, integrate)------------------*/
static char    (*s_paster)  (char a_type, int a_x , int a_y , int a_z , void *a_thing);



/*====================------------------------------------====================*/
/*===----                        support functions                     ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char         /*-> find a register index by abbr ------[ leaf   [fn.540.134.80]*/ /*-[00.0000.0L3.O]-*/ /*-[--.---.---.--]-*/
REGS__by_abbr           (char a_reg)
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
REGS__abbr2tab          (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        x_tab       =    0;
   /*---(get register index)-------------*/
   x_tab = REGS__by_abbr (a_reg);
   if (x_tab < 0)  return x_tab;
   /*---(complete)-----------------------*/
   return x_tab + 128;
}

char
REGS__set               (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   /*---(get register number)------------*/
   x_reg  = REGS__by_abbr  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
REGS__reset             (void)
{
   /*---(reset register)-----------------*/
   s_creg = '"';
   /*---(complete)-----------------------*/
   return 0;
}


/*====================------------------------------------====================*/
/*===----                   initialization and wrapup                  ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> clear out a register ---------------[ ------ [ge.B63.253.32]*/ /*-[03.0000.043.3]-*/ /*-[--.---.---.--]-*/
REGS__wipe              (char a_reg, char a_init)
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
   x_reg  = REGS__by_abbr  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(boundaries)---------------------*/
   DEBUG_REGS   yLOG_note    ("reset all values to zero");
   s_regs [x_reg].otab  = 0;
   s_regs [x_reg].minc  = s_regs [x_reg].minr  = 0;
   s_regs [x_reg].begc  = s_regs [x_reg].begr  = 0;
   s_regs [x_reg].endc  = s_regs [x_reg].endr  = 0;
   s_regs [x_reg].maxc  = s_regs [x_reg].maxr  = 0;
   s_regs [x_reg].type  = '-';
   s_regs [x_reg].nbuf  = 0;
   s_regs [x_reg].real  = 0;
   /*---(cells)--------------------------*/
   DEBUG_REGS   yLOG_note    ("clear all register positions");
   --rce;
   for (i = 0; i < LEN_BUF; ++i) {
      x_thing = s_regs [x_reg].buf [i];
      if (s_killer != NULL && a_init != 'y' && x_thing != NULL) {
         rc = s_killer (x_thing);
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
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear out all buffers --------------[ ------ [gz.421.121.01]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
REGS__purge             (char a_init)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   s_creg = '"';
   s_nreg = strlen (s_regnames);
   for (i = 0; i < s_nreg; ++i) {
      REGS__wipe (s_regnames[i], a_init);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
REGS_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_REGISTER)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(registers)----------------------*/
   REGS__purge    ('y');
   s_killer = NULL;
   s_copier = NULL;
   s_paster = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_REGISTER);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear all selections ---------------[ shoot  [gz.311.001.02]*/ /*-[00.0000.102.1]-*/ /*-[--.---.---.--]-*/
REGS_wrap               (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(registers)----------------------*/
   REGS__purge    ('y');
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_regs_config       (void *a_killer, void *a_copier, void *a_paster)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_REGS  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("init must complete before config");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   DEBUG_REGS  yLOG_point   ("a_killer"  , a_killer);
   s_killer     = a_killer;
   DEBUG_REGS  yLOG_point   ("a_copier"  , a_copier);
   s_copier     = a_copier;
   DEBUG_REGS  yLOG_point   ("a_paster"  , a_paster);
   s_paster     = a_paster;
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_REGISTER, '1');
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
yVIKEYS_regs_add        (void *a_thing, char a_note)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       =   0;
   int         x_nbuf      =   0;
   /*---(defense)------------------------*/
   --rce;  if (s_saving != 'y')                  return  rce;
   --rce;  if (a_thing  == NULL)                 return  rce;
   x_reg  = REGS__by_abbr  (s_creg);
   --rce;  if (x_reg < 0)                        return  rce;
   --rce;  if (s_regs [x_reg].nbuf >= LEN_BUF)   return  rce;
   /*---(attach cell to buffer)----------*/
   x_nbuf = s_regs [x_reg].nbuf;
   s_regs [x_reg].buf   [x_nbuf] = a_thing;
   s_regs [x_reg].notes [x_nbuf] = a_note;
   ++s_regs [x_reg].nbuf;
   ++s_regs [x_reg].real;
   /*---(complete)-----------------------*/
   return 0;
}

char
REGS_save               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_reg       =    0;
   int         x_nbuf      =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
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
   x_reg  = REGS__by_abbr  (s_creg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(wipe register)------------------*/
   rc = REGS__wipe  (s_creg, '-');
   DEBUG_REGS   yLOG_value   ("wipe rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(save selection)-----------------*/
   rc = VISU_range (&s_regs [x_reg].otab,
         &s_regs [x_reg].begc, &s_regs [x_reg].begr,
         &s_regs [x_reg].endc, &s_regs [x_reg].endr);
   DEBUG_REGS   yLOG_value   ("visu rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(call save)----------------------*/
   s_saving = 'y';
   rc = s_copier ('c',
         s_regs [x_reg].begc, s_regs [x_reg].endc,
         s_regs [x_reg].begr, s_regs [x_reg].endr,
         s_regs [x_reg].otab);
   s_saving = '-';
   DEBUG_REGS   yLOG_value   ("copy rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return  rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        major actions                         ----===*/
/*====================------------------------------------====================*/
static void  o___ACTIONS_________o () { return; }

char
REGS_clear              (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REGS_delete             (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REGS_copy               (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REGS_cut                (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REGS_paste              (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_operational (SMOD_REGISTER)) {
      DEBUG_REGS   yLOG_note    ("can not execute until operational");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}


char
REGS__unit_killer       (void *a_thing)
{
   return 0;
}

char
REGS__unit_copier       (char a_type, int a_x1, int a_x2, int a_y1, int a_y2, int a_z)
{
   return 0;
}

char
REGS__unit_paster       (char a_type, int a_x , int a_y , int a_z , void *a_thing)
{
   return 0;
}



