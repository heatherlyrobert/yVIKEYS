/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*---(horizontal)---------------------*/
static int     s_horz    =    0;
static int     s_ccol    =    0;
/*---(vertical)-----------------------*/
static int     s_vert    =    0;
static int     s_crow    =    0;


static int     s_bcol    = 0;
static int     s_ecol    = 0;
static int     s_acol    = 0;


#define    EMPTY_SPOT   -666

char    (*s_cmapper) (int a_horz, int a_ccol, int a_bcol, int a_ecol);
char    (*s_rmapper) (int a_vert, int a_crow, int a_brow, int a_erow);


char
yVIKEYS__map_clear    (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(lefts)--------------------------*/
   a_map->gmin  = EMPTY_SPOT;
   a_map->amin  = EMPTY_SPOT;
   a_map->lmin  = EMPTY_SPOT;
   a_map->prev  = EMPTY_SPOT;
   /*---(rights)-------------------------*/
   a_map->next  = EMPTY_SPOT;
   a_map->lmax  = EMPTY_SPOT;
   a_map->amax  = EMPTY_SPOT;
   a_map->gmax  = EMPTY_SPOT;
   /*---(map)----------------------------*/
   for (i = 0; i < LEN_MAP; ++i) {
      a_map->map [i] = EMPTY_SPOT;
   }
   /*---(indexes)------------------------*/
   a_map->beg   = 0;
   a_map->cur   = 0;
   a_map->end   = 0;
   a_map->len   = 0;
   a_map->avail = 0;
   a_map->tend  = 0;
   /*---(grids)--------------------------*/
   a_map->gbeg  = 0;
   a_map->gcur  = 0;
   a_map->gend  = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS__map_print   (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(headers)------------------------*/
   printf ("gmin amin lmin prev beg-    ");
   for (i = 0; i < LEN_MAP; ++i) {
      if (a_map->map [i] == EMPTY_SPOT)  break;
      printf ("%4d "  , i);
   }
   printf ("   end- next lmax amax gmax\n");
   /*---(content)------------------------*/
   printf ("%4d %4d %4d %4d %4d    "  , a_map->gmin, a_map->amin, a_map->lmin, a_map->prev, a_map->beg);
   for (i = 0; i < LEN_MAP; ++i) {
      if (a_map->map [i] == EMPTY_SPOT)  break;
      printf ("%4d "  , a_map->map [i]);
   }
   printf ("   %4d %4d %4d %4d %4d\n", a_map->end, a_map->next, a_map->lmax, a_map->amax, a_map->gmax);
   return 0;
}

char
yVIKEYS__map_load     (char a_style, tMAPPED *a_map)
{
   int         i           =    0;
   int         j           =    0;
   int         x_spot      =    0;
   yVIKEYS__map_clear  (a_map);
   for (i = 0; i < 8; ++i) {
      switch (a_style) {
      case 'u' : /* uniform size grid       */
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
         break;
      case 'a' : /* ascending size grid     */
         for (j =  0; j <= i; ++j)  a_map->map [x_spot++] = i;
         break;
      case 'd' : /* descending size grid    */
         for (j =  0; j <= 7 - i; ++j)  a_map->map [x_spot++] = i;
         break;
      case 's' : /* small                   */
         for (j =  0; j <  3; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = -3; i <= 3; ++i) {
      switch (a_style) {
      case '-' :  /* neg to pos grid        */
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = -120; i <= 300; i += 20) {
      switch (a_style) {
      case 'j' :   /* true grid the skips  */
         a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = 0; i <= 40; ++i) {
      switch (a_style) {
      case 'w' :   /* get a big grid       */
         for (j =  0; j <  4; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   a_map->gmin  = 0;
   a_map->amin  = 0;
   a_map->lmin  = 0;
   a_map->prev  = 0;
   a_map->next  = x_spot - 1;;
   a_map->lmax  = x_spot - 1;;
   a_map->amax  = x_spot - 1;;
   a_map->gmax  = x_spot - 1;;
   switch (a_style) {
   case 'w' :
      a_map->cur   = 44;
      a_map->beg   = 44;
      a_map->len   = 36;
      a_map->end   = 79;
      a_map->avail = 38;
      a_map->tend  = 81;
      break;
   default  :
      a_map->cur   = 0;
      a_map->beg   = 0;
      a_map->len   = x_spot;
      a_map->end   = x_spot - 1;
      a_map->avail = x_spot;
      a_map->tend  = x_spot - 1;
      break;
   }
   a_map->gbeg  = a_map->map [a_map->beg];
   a_map->gcur  = a_map->map [a_map->cur];
   a_map->gend  = a_map->map [a_map->end];
   /*> yVIKEYS__map_print  (a_map);                                                   <*/
   return 0;
}

char
yVIKEYS_map_init       (void *a_col_mapper, void *a_row_mapper)
{
   s_cmapper  = a_col_mapper;
   s_rmapper  = a_row_mapper;
   return 0;
}


/*> char         /+-> move left or right by columns ------[ ------ [ge.JA4.264.92]+/ /+-[04.0000.214.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> MOVE_gz_horz       (char a_major, char a_minor)                                                                                <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-------------------------+/                                                                                    <* 
 *>    char        rce         = -10;           /+ return code for error          +/                                               <* 
 *>    int         i           = 0;             /+ iterator -- horizontal pos     +/                                               <* 
 *>    int         x_target    = 0;                                                                                                <* 
 *>    int         x_cum       = 0;                                                                                                <* 
 *>    int         x_col       = 0;                                                                                                <* 
 *>    char        x_minors    [LEN_RECD]  = "shcle";                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_USER   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_USER   yLOG_char    ("a_major"   , a_major);                                                                          <* 
 *>    DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);                                                                          <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    --rce;  if (a_major != 'g' && a_major != 'z') {                                                                             <* 
 *>       DEBUG_USER   yLOG_note    ("can only process g and z family moves");                                                     <* 
 *>       DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (strchr (x_minors, a_minor) == 0) {                                                                              <* 
 *>       DEBUG_USER   yLOG_note    ("can only process g and z horizontal (shcle) moves");                                         <* 
 *>       DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(set target)---------------------+/                                                                                    <* 
 *>    --rce;                                                                                                                      <* 
 *>    DEBUG_USER   yLOG_value   ("my.x_avail", my.x_avail);                                                                       <* 
 *>    x_target = my.x_left;                                                                                                       <* 
 *>    switch (a_minor) {                                                                                                          <* 
 *>    case 's' :  x_target +=  0;                        break;                                                                   <* 
 *>    case 'h' :  x_target += (my.x_avail / 4);          break;                                                                   <* 
 *>    case 'c' :  x_target += (my.x_avail / 2);          break;                                                                   <* 
 *>    case 'l' :  x_target += (my.x_avail / 4) * 3;      break;                                                                   <* 
 *>    case 'e' :  x_target +=  my.x_avail;               break;                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_USER   yLOG_value   ("x_target"  , x_target);                                                                         <* 
 *>    /+---(process gotos)------------------+/                                                                                    <* 
 *>    --rce;                                                                                                                      <* 
 *>    if (a_major == 'g') {                                                                                                       <* 
 *>       DEBUG_USER   yLOG_note    ("handle a g=goto type");                                                                      <* 
 *>       switch (a_minor) {                                                                                                       <* 
 *>       case 's' :                                                                                                               <* 
 *>          CCOL = BCOL; break;                                                                                                   <* 
 *>       case 'e' :                                                                                                               <* 
 *>          CCOL = ECOL; break;                                                                                                   <* 
 *>       case 'h' : case 'c' : case 'l' :                                                                                         <* 
 *>          for (i = BCOL; i <= ECOL; ++i) {                                                                                      <* 
 *>             DEBUG_USER   yLOG_complex ("checking"  , "col %3d at %3d", i, LOC_col_xpos (CTAB, i));                             <* 
 *>             if (LOC_col_xpos (CTAB, i) <= x_target)  {                                                                         <* 
 *>                x_col = i;                                                                                                      <* 
 *>                continue;                                                                                                       <* 
 *>             }                                                                                                                  <* 
 *>             DEBUG_USER   yLOG_note    ("just passed");                                                                         <* 
 *>             break;                                                                                                             <* 
 *>          }                                                                                                                     <* 
 *>          CCOL = x_col;                                                                                                         <* 
 *>          break;                                                                                                                <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(process scrolls)----------------+/                                                                                    <* 
 *>    else if (a_major == 'z') {                                                                                                  <* 
 *>       DEBUG_USER   yLOG_note    ("handle a z=scroll type");                                                                    <* 
 *>       switch (a_minor) {                                                                                                       <* 
 *>       case 's' :                                                                                                               <* 
 *>          BCOL = CCOL; break;                                                                                                   <* 
 *>       case 'e' :                                                                                                               <* 
 *>          ECOL = CCOL; break;                                                                                                   <* 
 *>       case 'h' : case 'c' : case 'l' :                                                                                         <* 
 *>          BCOL = CCOL;                                                                                                          <* 
 *>          x_cum   = x_target - (LOC_col_width (CTAB, CCOL) / 2);                                                                <* 
*>          x_col   = 0;                                                                                                          <* 
*>          for (i = CCOL - 1; i >= 0; --i) {                                                                                     <* 
   *>             x_cum -= LOC_col_width (CTAB, i);                                                                                  <* 
      *>             DEBUG_USER   yLOG_complex ("checking"  , "col %3d wid %3d cum %3d", i, LOC_col_width (CTAB, i), x_cum);            <* 
      *>             if (x_cum > 0) {                                                                                                   <* 
         *>                x_col = i;                                                                                                      <* 
            *>                continue;                                                                                                       <* 
            *>             }                                                                                                                  <* 
            *>             DEBUG_USER   yLOG_note    ("just passed");                                                                         <* 
            *>             break;                                                                                                             <* 
            *>          }                                                                                                                     <* 
            *>          BCOL = x_col;                                                                                                         <* 
            *>       }                                                                                                                        <* 
            *>    }                                                                                                                           <* 
            *>    /+---(update current colunn)----------+/                                                                                    <* 
            *>    DEBUG_USER   yLOG_value   ("my.bcol"   , BCOL);                                                                             <* 
            *>    DEBUG_USER   yLOG_value   ("tab->ccol" , CCOL);                                                                             <* 
            *>    DEBUG_USER   yLOG_value   ("my.ecol"   , ECOL);                                                                             <* 
            *>    /+---(complete)-----------------------+/                                                                                    <* 
            *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                                                   <* 
            *>    return 0;                                                                                                                   <* 
            *> }                                                                                                                              <*/


int
yVIKEYS_map_fixer     (void)
{
   /*---(absolute boundaries)------------*/
   if (s_ccol > s_colmap.gmax)   s_ccol = s_colmap.gmax;
   if (s_ccol < s_colmap.gmin)   s_ccol = s_colmap.gmin;
   if (s_crow > s_colmap.gmax)   s_ccol = s_colmap.gmax;
   if (s_crow < s_colmap.gmin)   s_ccol = s_colmap.gmin;
}

char
yVIKEYS__map_move     (int a_target, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_map    == NULL)  return rce;
   /*---(make sure index is rational)----*/
   if (a_map->cur < a_map->gmin)  a_map->cur = a_map->gmin;
   if (a_map->cur > a_map->gmax)  a_map->cur = a_map->gmax;
   a_map->gcur = a_map->map [a_map->cur];
   /*---(shortcut)-----------------------*/
   if (a_map->gcur == a_target)     return 2;
   /*---(check to right)-----------------*/
   if (a_map->gcur <  a_target) {
      /*> printf ("must search to right\n");                                          <*/
      for (i = a_map->cur; i <= a_map->gmax; ++i) {
         /*> printf ("   checking index %4d\n", i);                                   <*/
         if (a_map->map [i] <  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         return 0;
      }
      a_map->cur  = a_map->gmax;
   }
   /*---(check to left)------------------*/
   else {
      /*> printf ("must search to left\n");                                           <*/
      /*---(find the right grid)---------*/
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         /*---(get to leftmost)----------*/
         for (i = a_map->cur; i >= a_map->gmin; --i) {
            if (a_map->map [i] != a_target)   break;
            a_map->cur  = i;
         }
         return 0;
      }
      a_map->cur  = a_map->gmin;
   }
   /*---(get to leftmost)----------*/
   a_map->gcur   = a_map->map [a_map->cur];
   for (i = a_map->cur; i >= a_map->gmin; --i) {
      if (a_map->map [i] != a_map->gcur)   break;
      a_map->cur  = i;
   }
   /*---(complete)-----------------------*/
   return  1;
}

char
yVIKEYS__map_screen_small (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   /*---(prepare)------------------------*/
   a_map->beg   = a_map->gmin;
   a_map->len   = a_map->gmax - a_map->gmin + 1;
   a_map->tend  = a_map->gmax;
   /*---(find real end)------------------*/
   for (i = a_map->tend; i >= a_map->gmin; --i) {
      a_map->end   = i;
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(complete)-----------------------*/
   return 2;
}


char
yVIKEYS__map_screen_beg   (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find closest beg backward)------*/
   /*> printf ("yVIKEYS__map_screen_beg\n");                                          <*/
   for (i = a_map->beg; i > a_map->gmin; --i) {
      a_map->beg   = i;
      /*> printf ("beg  = %3d\n", a_map->beg);                                        <*/
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   /*---(prepare)------------------------*/
   if (a_map->tend != a_map->gmax)  a_map->tend  = a_map->beg + a_map->avail - 1;
   /*---(can not fill screen?)-----------*/
   --rce;  if (a_map->tend > a_map->gmax)   return rce;
   /*---(find end of last full grid)-----*/
   for (i = a_map->tend; i >= a_map->gmin; --i) {
      a_map->end   = i;
      /*> printf ("tend = %3d\n", a_map->tend);                                       <*/
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(complete)-----------------------*/
   return 1;
}

char
yVIKEYS__map_screen_end   (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find end of end)----------------*/
   /*> printf ("yVIKEYS__map_screen_end\n");                                          <*/
   /*> printf ("end  = %3d\n", a_map->end);                                           <*/
   for (i = a_map->end; i <= a_map->gmax; ++i) {
      a_map->end   = i;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      a_map->tend  = i;
      /*> printf ("tend = %3d\n", a_map->tend);                                       <*/
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(check overrun)------------------*/
   /*> printf ("gmax = %3d\n", a_map->gmax);                                          <*/
   /*> printf ("gmin = %3d\n", a_map->gmin);                                          <*/
   /*---(run the final)------------------*/
   a_map->beg   = a_map->tend - a_map->avail + 1;
   /*---(find next beg forward)----------*/
   for (i = a_map->beg; i < a_map->gmax; ++i) {
      a_map->beg   = i;
      /*> printf ("beg  = %3d\n", a_map->beg);                                        <*/
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   /*---(handle normally)----------------*/
   rc = yVIKEYS__map_screen_beg (a_map);
   /*---(complete)-----------------------*/
   return rc;
}

char
yVIKEYS__map_screen     (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   int         w           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   int         x_prev      =    0;
   /*---(defense)------------------------*/
   rce;  if (a_map == NULL)     return rce;
   /*---(limits)-------------------------*/
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   if (a_map->cur < a_map->gmin)  a_map->cur = a_map->gmin;
   if (a_map->cur > a_map->gmax) {
      a_map->cur = a_map->gmax;
      for (i = a_map->cur; i > a_map->gmin; --i) {
         a_map->cur   = i;
         x_curr       = a_map->map [i    ];
         x_prev       = a_map->map [i - 1];
         if (x_curr != x_prev)   break;
      }
   }
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   /*---(screen fits all)----------------*/
   if (a_map->gmax - a_map->gmin <= a_map->avail) {
      /*> printf ("processing a small\n");                                            <*/
      rc = yVIKEYS__map_screen_small (a_map);
      return rc;
   }
   /*---(from beginning)-----------------*/
   if (a_map->cur < a_map->beg) {
      /*> printf ("processing a left\n");                                             <*/
      a_map->beg = a_map->cur;
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         a_map->beg = i;
         rc = yVIKEYS__map_screen_beg (a_map);
         if (rc > 0) break;
      }
   }
   /*---(from ending)--------------------*/
   else if (a_map->cur > a_map->end) {
      /*> printf ("processing a right\n");                                            <*/
      a_map->end   = a_map->cur;
      a_map->tend  = a_map->cur;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      rc = yVIKEYS__map_screen_end (a_map);
   }
   /*---(just a refresh)-----------------*/
   else {
      /*> printf ("processing a refresh\n");                                          <*/
      rc = yVIKEYS__map_screen_beg (a_map);
   }
   /*---(align grid)---------------------*/
   a_map->len   = a_map->end - a_map->beg + 1;
   a_map->gbeg  = a_map->map [a_map->beg];
   a_map->gcur  = a_map->map [a_map->cur];
   a_map->gend  = a_map->map [a_map->end];
   /*---(complete)-----------------------*/
   return rc;
}

int
yVIKEYS_map_vert      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      = s_rowmap.gcur;
   int         x_unit      =    0;
   int         x_qtr       = s_rowmap.avail / 4;
   char       *x_simple    = "_KkjJ~";
   char       *x_goto      = "TKtkmjbJB";
   /*---(simple)-------------------------*/
   if (a_major == ' ' && strchr (x_simple, a_minor) != NULL) {
      switch (a_minor) {
      case '_' : x_grid -= 1000000;   break;
      case 'K' : x_grid -= 5;         break;
      case 'k' : x_grid -= 1;         break;
      case 'j' : x_grid += 1;         break;
      case 'J' : x_grid += 5;         break;
      case '~' : x_grid += 1000000;   break;
      }
      yVIKEYS__map_move (x_grid, &s_rowmap);
   }
   /*---(gotos)--------------------------*/
   if (a_major == 'g' && strchr (x_goto  , a_minor) != NULL) {
      switch (a_minor) {
      case 'T' : x_unit  = s_rowmap.beg - (x_qtr * 4); break;
      case 'K' : x_unit  = s_rowmap.beg - (x_qtr * 2); break;
      case 't' : x_unit  = s_rowmap.beg;               break;
      case 'k' : x_unit  = s_rowmap.beg + (x_qtr * 1); break;
      case 'm' : x_unit  = s_rowmap.beg + (x_qtr * 2); break;
      case 'j' : x_unit  = s_rowmap.beg + (x_qtr * 3); break;
      case 'b' : x_unit  = s_rowmap.end;               break;
      case 'J' : x_unit  = s_rowmap.beg + (x_qtr * 6); break;
      case 'B' : x_unit  = s_rowmap.beg + (x_qtr * 8); break;
      }
      if (x_unit < s_rowmap.gmin)  x_unit = s_rowmap.gmin;
      if (x_unit > s_rowmap.gmax)  x_unit = s_rowmap.gmax;
      x_grid  = s_rowmap.map [x_unit];
      yVIKEYS__map_move (x_grid, &s_rowmap);
   }
   /*---(check screen)-------------------*/
   yVIKEYS__map_screen (&s_rowmap);
   /*---(complete)-----------------------*/
   return a_major;
}

int
yVIKEYS_map_horz      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      = s_colmap.gcur;
   int         x_unit      =    0;
   int         x_qtr       = s_colmap.avail / 4;
   char       *x_simple    = "0HhlL$";
   char       *x_goto      = "SHshcleLE";
   /*---(simple)-------------------------*/
   if (a_major == ' ' && strchr (x_simple, a_minor) != NULL) {
      switch (a_minor) {
      case '0' : x_grid -= 1000000;   break;
      case 'H' : x_grid -= 5;         break;
      case 'h' : x_grid -= 1;         break;
      case 'l' : x_grid += 1;         break;
      case 'L' : x_grid += 5;         break;
      case '$' : x_grid += 1000000;   break;
      }
      yVIKEYS__map_move (x_grid, &s_colmap);
   }
   /*---(gotos)--------------------------*/
   if (a_major == 'g' && strchr (x_goto  , a_minor) != NULL) {
      switch (a_minor) {
      case 'S' : x_unit  = s_colmap.beg - (x_qtr * 4); break;
      case 'H' : x_unit  = s_colmap.beg - (x_qtr * 2); break;
      case 's' : x_unit  = s_colmap.beg;               break;
      case 'h' : x_unit  = s_colmap.beg + (x_qtr * 1); break;
      case 'c' : x_unit  = s_colmap.beg + (x_qtr * 2); break;
      case 'l' : x_unit  = s_colmap.beg + (x_qtr * 3); break;
      case 'e' : x_unit  = s_colmap.end;               break;
      case 'L' : x_unit  = s_colmap.beg + (x_qtr * 6); break;
      case 'E' : x_unit  = s_colmap.beg + (x_qtr * 8); break;
      }
      if (x_unit < s_colmap.gmin)  x_unit = s_colmap.gmin;
      if (x_unit > s_colmap.gmax)  x_unit = s_colmap.gmax;
      x_grid  = s_colmap.map [x_unit];
      yVIKEYS__map_move (x_grid, &s_colmap);
   }
   /*---(check screen)-------------------*/
   yVIKEYS__map_screen (&s_colmap);
   /*---(complete)-----------------------*/
   return a_major;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yVIKEYS__map_unit       (char *a_question, char a_index)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP unit         : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "horz"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz pos     : index %3d, grid %3d", s_colmap.cur, s_colmap.gcur);
   }
   else if (strcmp (a_question, "horz_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", s_colmap.avail, s_colmap.beg, s_colmap.cur, s_colmap.end, s_colmap.tend, s_colmap.len);
   }
   else if (strcmp (a_question, "horz_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz grids   :        b %3d, c %3d, e %3d", s_colmap.gbeg, s_colmap.gcur, s_colmap.gend);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



