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



char    (*s_cmapper) (char a_type);
char    (*s_rmapper) (char a_type);

static char  *s_vsimple  = "_KkjJ~";
static char  *s_vgoto    = "TKtkmjbJB";
static char  *s_hsimple  = "0HhlL$";
static char  *s_hgoto    = "SHshcleLE";

char
yVIKEYS__map_clear    (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(lefts)--------------------------*/
   a_map->gmin  = YVIKEYS_EMPTY;
   a_map->amin  = YVIKEYS_EMPTY;
   a_map->lmin  = YVIKEYS_EMPTY;
   a_map->prev  = YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   a_map->next  = YVIKEYS_EMPTY;
   a_map->lmax  = YVIKEYS_EMPTY;
   a_map->amax  = YVIKEYS_EMPTY;
   a_map->gmax  = YVIKEYS_EMPTY;
   /*---(map)----------------------------*/
   for (i = 0; i < LEN_MAP; ++i) {
      a_map->map [i] = YVIKEYS_EMPTY;
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
      if (a_map->map [i] == YVIKEYS_EMPTY)  break;
      printf ("%4d "  , i);
   }
   printf ("   end- next lmax amax gmax\n");
   /*---(content)------------------------*/
   printf ("%4d %4d %4d %4d %4d    "  , a_map->gmin, a_map->amin, a_map->lmin, a_map->prev, a_map->beg);
   for (i = 0; i < LEN_MAP; ++i) {
      if (a_map->map [i] == YVIKEYS_EMPTY)  break;
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
   if (a_style == '1') {
      for (i = 0; i <= 100; ++i)   a_map->map [x_spot++] = i;
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
   case '1' :
      a_map->cur   =  0;
      a_map->beg   =  0;
      a_map->len   = 30;
      a_map->end   = 29;
      a_map->avail = 30;
      a_map->tend  = 29;
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
   if (s_cmapper != NULL)  s_cmapper ('i');
   if (s_rmapper != NULL)  s_rmapper ('i');
   return 0;
}

char
yVIKEYS__map_move     (int a_target, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_target"  , a_target);
   DEBUG_USER  yLOG_point   ("a_map"     , a_map);
   /*---(defense)------------------------*/
   --rce;  if (a_map    == NULL) {
      DEBUG_USER  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make sure index is rational)----*/
   DEBUG_USER  yLOG_value   ("cur"       , a_map->cur);
   DEBUG_USER  yLOG_value   ("gmin"      , a_map->gmin);
   DEBUG_USER  yLOG_value   ("gmax"      , a_map->gmax);
   if (a_map->cur < a_map->gmin)  a_map->cur = a_map->gmin;
   if (a_map->cur > a_map->gmax)  a_map->cur = a_map->gmax;
   DEBUG_USER  yLOG_value   ("cur"       , a_map->cur);
   a_map->gcur = a_map->map [a_map->cur];
   DEBUG_USER  yLOG_value   ("gcur"      , a_map->gcur);
   /*---(shortcut)-----------------------*/
   if (a_map->gcur == a_target) {
      DEBUG_USER  yLOG_note    ("already in position");
      DEBUG_USER  yLOG_exit    (__FUNCTION__);
      return 2;
   }
   /*---(check to right)-----------------*/
   if (a_map->gcur <  a_target) {
      DEBUG_USER  yLOG_note    ("must move to right");
      for (i = a_map->cur; i <= a_map->gmax; ++i) {
         if (a_map->map [i] <  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         DEBUG_USER  yLOG_value   ("cur"       , a_map->cur);
         DEBUG_USER  yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->gmax;
   }
   /*---(check to left)------------------*/
   else {
      DEBUG_USER  yLOG_note    ("must move to left");
      /*---(find the right grid)---------*/
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         /*---(get to leftmost)----------*/
         for (i = a_map->cur; i >= a_map->gmin; --i) {
            if (a_map->map [i] != a_target)   break;
            a_map->cur  = i;
            DEBUG_USER  yLOG_value   ("cur"       , a_map->cur);
         }
         DEBUG_USER  yLOG_exit    (__FUNCTION__);
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
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return  1;
}

char
yVIKEYS__map_screen_small (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_point   ("a_map"     , a_map);
   /*---(prepare)------------------------*/
   a_map->beg   = a_map->gmin;
   a_map->len   = a_map->gmax - a_map->gmin + 1;
   a_map->end   = a_map->gmax;
   a_map->tend  = a_map->gmax;
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
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
   int         x_tend      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_point   ("a_map"     , a_map);
   /*---(find closest beg backward)------*/
   for (i = a_map->beg; i > a_map->gmin; --i) {
      a_map->beg   = i;
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   DEBUG_USER  yLOG_value   ("beg"       , a_map->beg);
   /*---(prepare)------------------------*/
   x_tend = a_map->beg + a_map->avail - 1;
   DEBUG_USER  yLOG_value   ("gmax"      , a_map->gmax);
   if (x_tend <  a_map->gmax)  a_map->tend  = x_tend;
   DEBUG_USER  yLOG_value   ("tend"      , a_map->tend);
   /*---(can not fill screen?)-----------*/
   --rce;  if (a_map->tend > a_map->gmax)   return rce;
   /*---(find end of last full grid)-----*/
   for (i = a_map->tend; i >= a_map->gmin; --i) {
      a_map->end   = i;
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   DEBUG_USER  yLOG_value   ("end"       , a_map->end);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
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
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_major"   , a_major);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = s_rowmap.gcur;
   DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
   x_qtr       = s_rowmap.avail / 4.0;
   DEBUG_USER  yLOG_double  ("x_qtr"     , x_qtr);
   /*---(simple)-------------------------*/
   DEBUG_USER  yLOG_info    ("s_vsimple" , s_vsimple);
   if (a_major == ' ' && strchr (s_vsimple, a_minor) != NULL) {
      switch (a_minor) {
      case '_' : x_grid -= 1000000;   break;
      case 'K' : x_grid -= 5;         break;
      case 'k' : x_grid -= 1;         break;
      case 'j' : x_grid += 1;         break;
      case 'J' : x_grid += 5;         break;
      case '~' : x_grid += 1000000;   break;
      }
   }
   /*---(gotos)--------------------------*/
   DEBUG_USER  yLOG_info    ("s_vgoto"   , s_vgoto);
   if (a_major == 'g' && strchr (s_vgoto  , a_minor) != NULL) {
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
   }
   /*---(check screen)-------------------*/
   DEBUG_USER  yLOG_value   ("x_grid new", x_grid);
   yVIKEYS__map_move (x_grid, &s_rowmap);
   yVIKEYS__map_screen (&s_rowmap);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

int
yVIKEYS_map_horz      (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_major"   , a_major);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = s_colmap.gcur;
   DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
   x_qtr       = s_colmap.avail / 4.0;
   DEBUG_USER  yLOG_double  ("x_qtr"     , x_qtr);
   /*---(simple)-------------------------*/
   DEBUG_USER  yLOG_info    ("s_hsimple" , s_hsimple);
   if (a_major == ' ' && strchr (s_hsimple, a_minor) != NULL) {
      switch (a_minor) {
      case '0' : x_grid -= 1000000;   break;
      case 'H' : x_grid -= 5;         break;
      case 'h' : x_grid -= 1;         break;
      case 'l' : x_grid += 1;         break;
      case 'L' : x_grid += 5;         break;
      case '$' : x_grid += 1000000;   break;
      }
   }
   /*---(gotos)--------------------------*/
   DEBUG_USER  yLOG_info    ("s_hgoto"   , s_hgoto);
   if (a_major == 'g' && strchr (s_hgoto  , a_minor) != NULL) {
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
   }
   /*---(check screen)-------------------*/
   DEBUG_USER  yLOG_value   ("x_grid new", x_grid);
   yVIKEYS__map_move (x_grid, &s_colmap);
   yVIKEYS__map_screen (&s_colmap);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

char
yVIKEYS_map_mode        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(space)--------------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return G_KEY_SPACE;
   }
   /*---(major mode changes)-------------*/
   /*> if (a_minor == G_KEY_RETURN) {                                                 <* 
    *>    yVIKEYS_mode_enter  (MODE_SOURCE);                                          <* 
    *>    EDIT_pos    ('0');                                                          <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return  0;                                                                  <* 
    *> }                                                                              <*/
   /*> if (a_minor == G_KEY_ESCAPE) {                                                 <* 
    *>    VISU_clear ();                                                              <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return  0;                                                                  <* 
    *> }                                                                              <*/
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      /*---(multiplier)------------------*/
      if (strchr ("123456789"  , a_minor) != 0) {
         yVIKEYS_mode_enter  (SMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr ("gzced"  , a_minor) != 0) {
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(mode switch)-----------------*/
      switch (a_minor) {
         /*> case 'v'      :                                                             <* 
          *>    yVIKEYS_mode_enter  (MODE_VISUAL);                                       <* 
          *>    VISU_start  (CTAB, CCOL, CROW, VISU_FROM);                               <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
         /*> case 'V'      :                                                             <* 
          *>    yVIKEYS_mode_enter  (MODE_VISUAL);                                       <* 
          *>    VISU_restore ();                                                         <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
      case ':'      :
         yVIKEYS_mode_enter  (MODE_COMMAND);
         yVIKEYS_cmds_start ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case '/'      :
         yVIKEYS_mode_enter  (MODE_SEARCH);
         yVIKEYS_srch_start ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
         /*> case 's'      :                                                             <* 
          *>    EDIT_start  ("");                                                        <* 
          *>    yVIKEYS_mode_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'i');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'i';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '='      :                                                             <* 
          *>    EDIT_start  ("=");                                                       <* 
          *>    yVIKEYS_mode_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '#'      :                                                             <* 
          *>    EDIT_start  ("#");                                                       <* 
          *>    yVIKEYS_mode_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '+'      :                                                             <* 
          *>    EDIT_start  ("+");                                                       <* 
          *>    yVIKEYS_mode_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '-'      :                                                             <* 
          *>    EDIT_start  ("-");                                                       <* 
          *>    yVIKEYS_mode_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
         /*> case '\\'     :                                                             <* 
          *>    DEBUG_USER   yLOG_note    ("selected menu mode");                        <* 
          *>    yVIKEYS_mode_enter  (SMOD_MENUS  );                                      <* 
          *>    my.menu = MENU_ROOT;                                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
      case '@'      :
         IF_MACRO_OFF {
            yVIKEYS_macro_reset  ();
            yVIKEYS_mode_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         yVIKEYS_macro_reset  ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      case 'q'      :
         IF_MACRO_OFF {
            yVIKEYS_mode_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         yVIKEYS_macro_rec_end ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
      case 'Q'      :
         yVIKEYS_macro_reset ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
         /*> case 'F'      :                                                             <* 
          *>    yVIKEYS_mode_enter  (SMOD_FORMAT  );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
         /*> case ','      :                                                             <* 
          *>    yVIKEYS_mode_enter  (SMOD_BUFFER  );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;                                                          <* 
          *>    break;                                                                   <*/
         /*> case '"'      :                                                             <* 
          *>    yVIKEYS_mode_enter  (SMOD_REGISTER);                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;  /+ make sure double quote goes in prev char +/          <* 
          *>    break;                                                                   <*/
         /*> case 'm'      :                                                             <* 
          *> case '\''     :                                                             <* 
          *>    yVIKEYS_mode_enter  (SMOD_MARK    );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;  /+ make sure single quote goes in prev char +/          <* 
          *>    break;                                                                   <*/
         /*> case 'E'      :                                                             <* 
          *>    yVIKEYS_mode_enter  (SMOD_ERROR   );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return SMOD_ERROR;  /+ make sure mode indicator goes also       +/       <* 
          *>    break;                                                                   <*/
      }
      /*---(normal)----------------------*/
      /*> if (a_minor == 6) {                                                         <* 
       *>    rc = KEYS_gz_family  ('g', 'B');                                         <* 
       *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
       *>    return 0;                                                                <* 
       *> }                                                                           <*/
      /*> if (a_minor == 2) {                                                         <* 
       *>    rc = KEYS_gz_family  ('g', 'T');                                         <* 
       *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
       *>    return 0;                                                                <* 
       *> }                                                                           <*/
      if (a_minor == 'P') {
         yVIKEYS__map_print (&s_colmap);
         yVIKEYS__map_print (&s_rowmap);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr ("0HhlL$", a_minor) != 0) {
         rc = yVIKEYS_map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr ("_KkjJ~", a_minor) != 0) {
         rc = yVIKEYS_map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      /*> rc = KEYS_regbasic (a_major, a_minor);                                      <* 
       *> if (rc == 0) {                                                              <* 
       *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
       *>    return 0;                                                                <* 
       *> }                                                                           <*/
      /*---(special)------------------*/
      /*> switch (a_minor) {                                                                                    <* 
       *> case K_CTRL_L : clear ();                       break;                                                <* 
       *> case 'P'      : DEP_writeall (); LOC_col_map (); KEYS_pcol (); KEYS_prow (); HIST_list ();  break;    <* 
       *>                 /+---(formatting)---------------+/                                                    <* 
       *> case '<'      : CELL_align (CHG_INPUT, '<');               break;                                     <* 
       *> case '|'      : CELL_align (CHG_INPUT, '|');               break;                                     <* 
       *> case '>'      : CELL_align (CHG_INPUT, '>');               break;                                     <* 
       *>                 /+---(selection)----------------+/                                                    <* 
       *>                 /+> case 'v'      : VISU_start (CTAB, CCOL, CROW, VISU_FROM);   break;          <+/   <* 
       *>                 /+> case 'V'      : VISU_start (CTAB, CCOL, CROW, VISU_CUM);    break;          <+/   <* 
       *>                 /+> case 'y'      : REG_copy  ();                   break;                      <+/   <* 
       *>                 /+> case 'p'      : REG_paste (G_PASTE_NORM);       break;                      <+/   <* 
       *>                 /+---(modes and multikey)-------+/                                                    <* 
       *> case '@'      : SEQ_calc_full ();               break;                                                <* 
       *>                 /+> case '[' : if (escaped) { sch = ch; special = 1; } else sch = 'x'; break;   <+/   <* 
       *>                 /+---(new stuff)----------------+/                                                    <* 
       *> case 'u'      : HIST_undo ();                   break;                                                <* 
       *> case 'U'      : HIST_redo ();                   break;                                                <* 
       *>                 /+> case 'W'      : REG_bufwrite (my.reg_curr);     break;                      <+/   <* 
       *> case '?'      : my.info_win = G_INFO_CELL;      break;                                                <* 
       *> default       : /+ unknown problem +/                                                                 <* 
       *>                 DEBUG_USER   yLOG_exit    (__FUNCTION__);                                             <* 
       *>                 return rce;                                                                           <* 
       *>                 break;                                                                                <* 
       *> }                                                                                                     <*/
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(special family)------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'c') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 's'      : KEYS_col ("cs");                break;                      <* 
    *>    case 'h'      : KEYS_col ("ch");                break;                      <* 
    *>    case 'l'      : KEYS_col ("cl");                break;                      <* 
    *>    case 'e'      : KEYS_col ("ce");                break;                      <* 
    *>    case 't'      : KEYS_row ("ct");                break;                      <* 
    *>    case 'b'      : KEYS_row ("cb");                break;                      <* 
    *>    default       : return rce;                     break;                      <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*> else if (a_major == 'x') {                                                      <* 
    *>    sch = ' ';                                                                  <* 
    *>    ch  = 24;                                                                   <* 
    *> }                                                                              <*/
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      if (strchr (s_hgoto, a_minor) != 0) {
         rc = yVIKEYS_map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (s_vgoto, a_minor) != 0) {
         rc = yVIKEYS_map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
   }
   /*---(scroll family)------------------*/
   /*> if (a_major == 'z') {                                                          <* 
    *>    rc = KEYS_gz_family  (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(end family)---------------------*/
   /*> if (a_major == 'e') {                                                          <* 
    *>    rc = KEYS_e_family   (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(end family)---------------------*/
   /*> if (a_major == 'c') {                                                          <* 
    *>    rc = KEYS_c_family   (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(delete family)-------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'd') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 'd' : --NROW;          break;                                          <* 
    *>    case 'w' : --NCOL;     break;                                               <* 
    *>    default  : return rce;                         break;                       <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS__unit_rowmap    (void)
{
   /*> printf ("running rowmap\n");                                                   <*/
   yVIKEYS__map_load ('1', &s_rowmap);
   return 0;
}

char
yVIKEYS__unit_colmap    (void)
{
   /*> printf ("running colmap\n");                                                   <*/
   yVIKEYS__map_load ('w', &s_colmap);
   return 0;
}

char
yVIKEYS__unit_dismap    (void)
{
   return 0;
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
   else if (strcmp (a_question, "vert_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", s_rowmap.avail, s_rowmap.beg, s_rowmap.cur, s_rowmap.end, s_rowmap.tend, s_rowmap.len);
   }
   else if (strcmp (a_question, "vert_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert grids   :        b %3d, c %3d, e %3d", s_rowmap.gbeg, s_rowmap.gcur, s_rowmap.gend);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



