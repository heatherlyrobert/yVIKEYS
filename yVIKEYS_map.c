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
   a_map->gmin = EMPTY_SPOT;
   a_map->amin = EMPTY_SPOT;
   a_map->lmin = EMPTY_SPOT;
   a_map->prev = EMPTY_SPOT;
   a_map->beg  = EMPTY_SPOT;
   a_map->end  = EMPTY_SPOT;
   a_map->next = EMPTY_SPOT;
   a_map->lmax = EMPTY_SPOT;
   a_map->amax = EMPTY_SPOT;
   a_map->gmax = EMPTY_SPOT;
   for (i = 0; i < LEN_MAP; ++i) {
      a_map->map [i] = EMPTY_SPOT;
   }
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
   a_map->gmin = 0;
   a_map->amin = 0;
   a_map->lmin = 0;
   a_map->prev = 0;
   a_map->beg  = 0;
   a_map->end  = x_spot - 1;;
   a_map->next = x_spot - 1;;
   a_map->lmax = x_spot - 1;;
   a_map->amax = x_spot - 1;;
   a_map->gmax = x_spot - 1;;
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
yVIKEYS__map_move     (int *a_index, int *a_grid, int a_target, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_index  == NULL)  return rce;
   --rce;  if (a_grid   == NULL)  return rce;
   --rce;  if (a_map    == NULL)  return rce;
   /*---(shortcut)-----------------------*/
   if (*a_grid == a_target)     return 2;
   /*---(check to right)-----------------*/
   if (*a_grid <  a_target) {
      /*> printf ("must search to right\n");                                          <*/
      for (i = *a_index; i <= a_map->gmax; ++i) {
         /*> printf ("   checking index %4d\n", i);                                   <*/
         if (a_map->map [i] <  a_target)   continue;
         *a_index  = i;
         *a_grid = a_map->map [*a_index];
         return 0;
      }
      *a_index  = a_map->gmax;
   }
   /*---(check to left)------------------*/
   else {
      /*> printf ("must search to left\n");                                           <*/
      for (i = *a_index; i >= a_map->gmin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         *a_index  = i;
         *a_grid = a_map->map [*a_index];
         return 0;
      }
      *a_index  = a_map->gmin;
   }
   /*---(default)------------------------*/
   *a_grid = a_map->map [*a_index];
   /*---(complete)-----------------------*/
   return  1;
}

int
yVIKEYS_map_horz      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_col       = s_colmap.map [s_horz];
   int         x_save      = x_col;
   int         i           =    0;
   /*---(simple)-------------------------*/
   if (a_major == ' ') {
      switch (a_minor) {
      case 'H' : x_col -= 5;   break;
      case 'h' : x_col -= 1;   break;
      case 'l' : x_col += 1;   break;
      case 'L' : x_col += 5;   break;
      }
   }
   /*---(back to index)------------------*/
   /*---(complete)-----------------------*/
   return a_major;
}

/*> int                                                                               <* 
 *> yVIKEYS_map_vert      (char a_major, char a_minor)                                <* 
 *> {                                                                                 <* 
 *>    int         x_horz      = s_crow;                                              <* 
 *>    int         x_crow      = s_crow;                                              <* 
 *>    if (a_major == ' ') {                                                          <* 
 *>       switch (a_minor) {                                                          <* 
 *>       case 'K' : x_crow -= 5;   break;                                            <* 
 *>       case 'k' : x_crow -= 1;   break;                                            <* 
 *>       case 'j' : x_crow += 1;   break;                                            <* 
 *>       case 'J' : x_crow += 5;   break;                                            <* 
 *>       }                                                                           <* 
 *>    }                                                                              <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/



