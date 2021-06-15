/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*
 * ---------- EXTERNAL INTERFACE ----------
 *
 *  yVIKEYS_hint_config -- must be called before use
 *
 *  yVIKEYS_hint_found  -- register selected/active hints
 *
 *  yVIKEYS_hinting     -- see whether to show hints or not
 *
 *
 * ---------- REQUIRED SUPPORT ----------
 *
 *  hinter              -- function to process hints
 *
 *  unhinter            -- function to clear hints
 *
 *
 * ---------- FUTURE IDEAS ----------
 *
 *  custom labels...
 *     -- any short text refers to any location
 *     -- read/written to files
 *     -- seems useful, but not sure i need them
 *
 *
 */



char    (*s_hinter)     (char *a_hint)               = NULL;
char    (*s_unhinter)   (int b, int x, int y, int z) = NULL;
static char    s_hinting     = '-';
static char    s_hintstr     [LEN_LABEL] = "-";
static char    s_hint        [LEN_LABEL] = "-";



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char         /*-> initialize hints -------------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yvikeys_hint_init            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_HINT)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_HINT);
   /*---(clear all)----------------------*/
   yvikeys_hint_wrap  ();
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_hint_config     (void *a_hinter, void *a_unhinter)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_MARK  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_HINT)) {
      DEBUG_MARK   yLOG_note    ("init must complete before config");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update hinter)------------------*/
   DEBUG_MARK   yLOG_point   ("a_hinter"  , a_hinter);
   --rce;  if (a_hinter == NULL) {
      DEBUG_MARK   yLOG_note    ("without hinter callback, hinting can not function");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_hinter = a_hinter;
   DEBUG_MARK   yLOG_point   ("s_hinter"  , s_hinter);
   /*---(update unhinter)----------------*/
   DEBUG_MARK   yLOG_point   ("a_unhinter", a_unhinter);
   --rce;  if (a_unhinter == NULL) {
      DEBUG_MARK   yLOG_note    ("without unhinter callback, hinting can not function");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_unhinter = a_unhinter;
   DEBUG_MARK   yLOG_point   ("s_unhinter"  , s_unhinter);
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_HINT, '1');
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yvikeys_hint_purge   (void) { yvikeys_srch__purge (SMOD_HINT); }

char
yvikeys_hint_wrap            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(update status)------------------*/
   yvikeys_hint_purge ();
   s_hinter    = NULL;
   s_unhinter  = NULL;
   strcpy (s_hintstr, "-");
   strcpy (s_hint   , "-");
   s_hinting   = '-';
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    searching and cursoring                   ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

int   yvikeys_hint_count     (void)                         { return yvikeys_srch__count     (SMOD_HINT); }
char  yvikeys_hint_by_cursor (char a_move, tFIND **r_curr)  { return yvikeys_srch__by_cursor (SMOD_HINT, a_move, r_curr); }
char  yvikeys_hint_by_index  (int a_index, tFIND **r_curr)  { return yvikeys_srch__by_index  (SMOD_HINT, a_index, r_curr); }
char  yvikeys_hint_list      (char *a_list)                 { return yvikeys_srch__list      (SMOD_HINT, a_list); }
char  yvikeys_hint_curr      (char *a_label)                { return yvikeys_srch__curr      (SMOD_HINT, a_label); }



/*====================------------------------------------====================*/
/*===----                      mark command line                       ----===*/
/*====================------------------------------------====================*/
static void  o___COMMAND_________o () { return; }

char yVIKEYS_hint_found   (char *a_label, int b, int x, int y, int z) { yvikeys_srch__found (SMOD_HINT  , a_label, b, x, y, z); }

char
yvikeys_hint_direct     (char *a_hint)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   tFIND      *x_curr      = NULL;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_MARK   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MARK   yLOG_note    ("before status check");
   --rce;  if (!STATUS_operational (SMOD_HINT)) {
      DEBUG_MARK   yLOG_note    ("can not configure until operational");
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_note    ("after status check");
   /*---(defense)------------------------*/
   DEBUG_MARK   yLOG_point   ("a_hint"    , a_hint);
   --rce;  if (a_hint == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MARK   yLOG_info    ("a_hint"    , a_hint);
   x_len = strlen (a_hint);
   DEBUG_MARK   yLOG_value   ("x_len"     , x_len);
   --rce;  if (a_hint [0] != ';') {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   strlcpy (x_label  , a_hint, LEN_LABEL);
   strlcpy (s_hintstr, a_hint, LEN_LABEL);
   /*---(clearing)-----------------------*/
   if (strcmp (a_hint, ";") == 0) {
      DEBUG_MARK   yLOG_note    ("calling purge");
      yvikeys_hint_purge ();
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(common things)------------------*/
   switch (a_hint [1]) {
   case '+'  :
      DEBUG_MARK   yLOG_note    ("turning hinting on");
      s_hinting = 'y';
      strlcpy (x_label, "update", LEN_LABEL);
      break;
   case '-'  :
      DEBUG_MARK   yLOG_note    ("turning hinting off");
      s_hinting = '-';
      strlcpy (x_label, "update", LEN_LABEL);
      break;
   case '[' : case '<' : case '.' : case '>' : case ']' :
      rc = yvikeys_hint_by_cursor (a_hint [1], NULL);
      DEBUG_MARK   yLOG_exit    (__FUNCTION__);
      return rc;
      break;
   }
   /*---(check hinter)-------------------*/
   DEBUG_MARK   yLOG_point   ("s_hinter"  , s_hinter);
   --rce;  if (s_hinter == NULL) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear before calling)-----------*/
   DEBUG_MARK   yLOG_note    ("purge before handling");
   if (strncmp (a_hint, ";;", 2) == 0) {
      strlcpy (s_hint   , a_hint, LEN_LABEL);
      yvikeys_hint_purge ();
   }
   /*---(execute)------------------------*/
   rc = s_hinter (x_label);
   --rce;  if (rc < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(go to head)---------------------*/
   if (strncmp (a_hint, ";;", 2) == 0) {
      yvikeys_hint_by_cursor (YDLST_HEAD, NULL);
   }
   /*---(complete)-----------------------*/
   DEBUG_MARK   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        screen display                        ----===*/
/*====================------------------------------------====================*/
static void  o___DISPLAY_________o () { return; }

char yVIKEYS_hinting (void)  { if (s_hinting == 'y')  return 1;  else return 0; };

char
yvikeys_hint_status          (char *a_list)
{
   char        x_list      [LEN_RECD]  = "";
   char        x_label     [LEN_LABEL] = "";
   yvikeys_hint_list (x_list);
   yvikeys_hint_curr (x_label);
   snprintf (a_list, LEN_FULL, "hinting  %c  %-5.5s  %-5.5s  %2d  %-5.5s  %s", s_hinting, s_hintstr, s_hint, yvikeys_hint_count (), x_label, x_list);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char
yvikeys__unit_hinter    (char *a_hint)
{
   if (a_hint == NULL)  return 0;
   /*---(clearing)-----------------------*/
   if      (strcmp (";;aa", a_hint) == 0) {
      DEBUG_MARK   yLOG_note    ("hinter -- ;;aa version");
      yVIKEYS_hint_found ("aa"       ,   0,   1,   1,   0);
      /*> yVIKEYS_jump (0,  1,  1, 0);                                                <*/
   } else if (strcmp (";;ae", a_hint) == 0) {
      DEBUG_MARK   yLOG_note    ("hinter -- ;;ae version");
      yVIKEYS_hint_found ("ae"       ,   0,  10,  10,   0);
      /*> yVIKEYS_jump (0, 10, 10, 0);                                                <*/
   } else if (strcmp (";;bc", a_hint) == 0) {
      DEBUG_MARK   yLOG_note    ("hinter -- ;;bc version");
      yVIKEYS_hint_found ("ae"       ,   0,  10,  10,   0);
      /*> yVIKEYS_jump (0, 10, 10, 0);                                                <*/
      yVIKEYS_hint_found ("bc"       ,   0,   1,   2,   0);
      yVIKEYS_hint_found ("pp"       ,   0,   1,   4,   0);
      yVIKEYS_hint_found ("zj"       ,   0,  25,  14,   0);
   } else if (strcmp (";;zp", a_hint) == 0) {
      DEBUG_MARK   yLOG_note    ("hinter -- ;;zp version");
      yVIKEYS_hint_found ("zp"       ,   2,   1,   4,   0);
      /*> yVIKEYS_jump (2,  1,  4, 0);                                                <*/
   } else if (strcmp ("update", a_hint) == 0) {
      ;;
   } else {
      DEBUG_MARK   yLOG_note    ("hinter -- no match");
      return -10;
   }
   return 0;
}

char
yvikeys__unit_unhinter  (int b, int x, int y, int z)
{
   return 0;
}

char*        /*-> unit test accessor -----------------[ light  [us.940.221.74]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_hint__unit           (char *a_question, char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   char        x_list      [LEN_RECD];
   int         x_index     = 0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "MARK             : question not understood");
   /*---(defense)------------------------*/
   /*> switch (a_mark) {                                                                                                                                                                                                                                                                               <* 
    *> case  0   :                                                                                                                                                                                                                                                                                     <* 
    *>    x_index = 0;  break;                                                                                                                                                                                                                                                                         <* 
    *> default   :                                                                                                                                                                                                                                                                                     <* 
    *>    x_index = yvikeys_mark__index (a_mark);                                                                                                                                                                                                                                                      <* 
    *>    if (x_index < 0) {                                                                                                                                                                                                                                                                           <* 
    *>       snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK invalid     : %c not defined", a_mark);                                                                                                                                                                                                   <* 
    *>    }                                                                                                                                                                                                                                                                                            <* 
    *> }                                                                                                                                                                                                                                                                                               <* 
    *> /+---(questions)----------------------+/                                                                                                                                                                                                                                                        <* 
    *> if      (strcmp (a_question, "list"        )   == 0) {                                                                                                                                                                                                                                          <* 
    *>    yvikeys_mark_listplus (x_list);                                                                                                                                                                                                                                                              <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK list        : %-.80s", x_list);                                                                                                                                                                                                              <* 
    *> }                                                                                                                                                                                                                                                                                               <* 
    *> else if (strcmp (a_question, "info"        )   == 0) {                                                                                                                                                                                                                                          <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK info        : %c %c %-12.12s %4db %4dx %4dy %4dz", a_mark, s_mark_info [x_index].source, s_mark_info [x_index].label, s_mark_info [x_index].b_pos, s_mark_info [x_index].x_pos, s_mark_info [x_index].y_pos, s_mark_info [x_index].z_pos);   <* 
    *> }                                                                                                                                                                                                                                                                                               <* 
    *> else if (strcmp (a_question, "range"       )   == 0) {                                                                                                                                                                                                                                          <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK range       : %c to %c", s_mark_head, s_mark_tail);                                                                                                                                                                                          <* 
    *> }                                                                                                                                                                                                                                                                                               <*/
   /*> else if (strcmp (a_question, "mark_entry"  )   == 0) {                         <* 
    *>    MARK_entry (a_mark, x_list);                                                <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK entry       :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*> else if (strcmp (a_question, "mark_status" )   == 0) {                         <* 
    *>    yvikeys_mark_status (x_list);                                                       <* 
    *>    snprintf (yVIKEYS__unit_answer, LEN_RECD, "MARK status      :%s:", x_list);          <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


