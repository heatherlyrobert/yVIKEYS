/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



char    (*s_mapper) (char a_type);



char   g_coord    = YVIKEYS_RIGHT;


char   g_vsimple [LEN_DESC ]   = "_KkjJ~";
char   g_vgoto   [LEN_DESC ]   = "TKtkmjbJB";
char   g_vscroll [LEN_DESC ]   = "  tkmjb  ";
char   g_hsimple [LEN_DESC ]   = "0HhlL$";
char   g_hgoto   [LEN_DESC ]   = "SHshcleLE";
char   g_hscroll [LEN_DESC ]   = "  shcle  ";
char   g_multi   [LEN_DESC ]   = "cdegz";




char
MAP__clear           (tMAPPED *a_map, char a_which)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(identity)-----------------------*/
   a_map->which = a_which;
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
MAP__print           (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(headers)------------------------*/
   printf ("-  gmin amin lmin prev    ");
   /*> for (i = 0; i < LEN_MAP; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , i);                                                       <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   printf ("next lmax amax gmax    aval beg- cur- end- len- tend\n");
   /*---(content)------------------------*/
   printf ("%c  "                        , a_map->which);
   printf ("%4d %4d %4d %4d    "         , a_map->gmin , a_map->amin , a_map->lmin , a_map->prev );
   /*> for (i = 0; i < LEN_MAP; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , a_map->map [i]);                                          <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   /*---(end)----------------------------*/
   printf ("%4d %4d %4d %4d"              , a_map->next , a_map->lmax , a_map->amax , a_map->gmax );
   printf ("   %4d %4d %4d %4d %4d %4d\n", a_map->avail, a_map->beg  , a_map->cur  , a_map->end  , a_map->len  , a_map->tend );
   return 0;
}

char
MAP__load             (char a_style, tMAPPED *a_map, char a_which)
{
   int         i           =    0;
   int         j           =    0;
   int         x_spot      =    0;
   MAP__clear  (a_map, a_which);
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
   /*> MAP__print  (a_map);                                                           <*/
   return 0;
}

char
yVIKEYS_map_config     (char a_coord, void *a_mapper)
{
   g_coord    = a_coord;
   s_mapper   = a_mapper;
   if (s_mapper != NULL)  s_mapper (YVIKEYS_INIT);
   return 0;
}

char
MAP_init               (void)
{
   g_coord    = YVIKEYS_OFFICE;
   s_mapper   = NULL;
   MAP__clear (&g_xmap, YVIKEYS_XMAP);
   MAP__clear (&g_ymap, YVIKEYS_YMAP);
   MAP__clear (&g_zmap, YVIKEYS_ZMAP);
   MAP__clear (&g_tmap, YVIKEYS_TMAP);
   /*> MAP__print (&g_xmap);                                                          <*/
   /*> MAP__print (&g_ymap);                                                          <*/
   return 0;
}

int          /*-> idendify closest grid to position --[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
MAP__closer           (int a_position, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   int         i           =    0;
   int         x_target    =    0;
   int         x_left      = YVIKEYS_EMPTY;
   int         x_right     = YVIKEYS_EMPTY;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_value   ("a_position", a_position);
   /*---(prepare)------------------------*/
   x_target = a_map->map [a_position];
   DEBUG_USER  yLOG_value   ("x_target"  , x_target);
   DEBUG_USER  yLOG_value   ("gmin"      , a_map->gmin);
   DEBUG_USER  yLOG_value   ("gmax"      , a_map->gmax);
   /*---(look right)---------------------*/
   DEBUG_USER  yLOG_note    ("go right");
   for (i = a_position; i <= a_map->gmax; ++i) {
      DEBUG_USER  yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <= x_target)        continue;
      x_right = i;
      break;
   }
   if (x_right == YVIKEYS_EMPTY)  DEBUG_USER  yLOG_note    ("x_right not found");
   else                           DEBUG_USER  yLOG_value   ("x_right"   , x_right - a_position);
   /*---(look left)----------------------*/
   DEBUG_USER  yLOG_note    ("go left");
   for (i = a_position; i >= a_map->gmin; --i) {
      DEBUG_USER  yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <  x_target) {
         DEBUG_USER  yLOG_value   ("x_left"    , x_left);
         break;
      }
      x_left  = i;
   }
   if (x_left  == YVIKEYS_EMPTY)  DEBUG_USER  yLOG_note    ("x_left not found");
   else                           DEBUG_USER  yLOG_value   ("x_left"    , a_position - x_left);
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   rc = x_left;
   if      (x_left  == YVIKEYS_EMPTY && x_right == YVIKEYS_EMPTY)  rc = a_position;
   else if (x_left  == YVIKEYS_EMPTY)                              rc = x_right;
   else if (x_right == YVIKEYS_EMPTY)                              rc = x_left;
   else if (x_right - a_position <  a_position - x_left)           rc = x_right;
   return rc;
}

char
MAP__move             (int a_target, tMAPPED *a_map)
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
   /*> MAP__print  (a_map);                                                           <*/
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
   /*> MAP__print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return  1;
}

char
MAP__screen_small         (tMAPPED *a_map)
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
MAP__screen_beg           (tMAPPED *a_map)
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
MAP__screen_end           (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find end of end)----------------*/
   /*> printf ("MAP__screen_end\n");                                          <*/
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
   rc = MAP__screen_beg (a_map);
   /*---(complete)-----------------------*/
   return rc;
}

char
MAP__screen             (tMAPPED *a_map)
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
      rc = MAP__screen_small (a_map);
      return rc;
   }
   /*---(from beginning)-----------------*/
   if (a_map->cur < a_map->beg) {
      /*> printf ("processing a left\n");                                             <*/
      a_map->beg = a_map->cur;
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         a_map->beg = i;
         rc = MAP__screen_beg (a_map);
         if (rc > 0) break;
      }
   }
   /*---(from ending)--------------------*/
   else if (a_map->cur > a_map->end) {
      /*> printf ("processing a right\n");                                            <*/
      a_map->end   = a_map->cur;
      a_map->tend  = a_map->cur;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      rc = MAP__screen_end (a_map);
   }
   /*---(just a refresh)-----------------*/
   else {
      /*> printf ("processing a refresh\n");                                          <*/
      rc = MAP__screen_beg (a_map);
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
MAP__vert             (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_major"   , a_major);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_ymap.gcur;
   DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
   x_qtr       = (g_ymap.avail - g_gsizey) / 4.0;
   DEBUG_USER  yLOG_double  ("x_qtr"     , x_qtr);
   x_gmax  = g_ymap.map [g_ymap.gmax - g_gsizey];
   DEBUG_USER  yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_USER  yLOG_info    ("g_vsimple" , g_vsimple);
   if (a_major == ' ' && strchr (g_vsimple, a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case '_' : x_grid  = g_ymap.map [g_ymap.gmin];  break;
         case 'K' : x_grid -= g_gsizey * 5;  break;
         case 'k' : x_grid -= g_gsizey;      break;
         case 'j' : x_grid += g_gsizey;      break;
         case 'J' : x_grid += g_gsizey * 5;  break;
         case '~' : x_grid  = x_gmax;        break;
         }
      } else {
         switch (a_minor) {
         case '~' : x_grid  = g_ymap.map [g_ymap.gmin];  break;
         case 'J' : x_grid -= g_gsizey * 5;  break;
         case 'j' : x_grid -= g_gsizey;      break;
         case 'k' : x_grid += g_gsizey;      break;
         case 'K' : x_grid += g_gsizey * 5;  break;
         case '_' : x_grid  = x_gmax;        break;
         }
      }
   }
   /*---(gotos)--------------------------*/
   DEBUG_USER  yLOG_info    ("g_vgoto"   , g_vgoto);
   if (a_major == 'g' && strchr (g_vgoto  , a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case 'T' : x_unit  = g_ymap.beg - (x_qtr * 4); break;
         case 'K' : x_unit  = g_ymap.beg - (x_qtr * 2); break;
         case 't' : x_unit  = g_ymap.beg;               break;
         case 'k' : x_unit  = g_ymap.beg + (x_qtr * 1); break;
         case 'm' : x_unit  = g_ymap.beg + (x_qtr * 2); break;
         case 'j' : x_unit  = g_ymap.beg + (x_qtr * 3); break;
         case 'b' : x_unit  = g_ymap.beg + (x_qtr * 4); break;
         case 'J' : x_unit  = g_ymap.beg + (x_qtr * 6); break;
         case 'B' : x_unit  = g_ymap.beg + (x_qtr * 8); break;
         }
      } else {
         switch (a_minor) {
         case 'B' : x_unit  = g_ymap.beg - (x_qtr * 4); break;
         case 'J' : x_unit  = g_ymap.beg - (x_qtr * 2); break;
         case 'b' : x_unit  = g_ymap.beg;               break;
         case 'j' : x_unit  = g_ymap.beg + (x_qtr * 1); break;
         case 'm' : x_unit  = g_ymap.beg + (x_qtr * 2); break;
         case 'k' : x_unit  = g_ymap.beg + (x_qtr * 3); break;
         case 't' : x_unit  = g_ymap.beg + (x_qtr * 4); break;
         case 'K' : x_unit  = g_ymap.beg + (x_qtr * 6); break;
         case 'T' : x_unit  = g_ymap.beg + (x_qtr * 8); break;
         }
      }
      DEBUG_USER  yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_ymap.gmin)  x_unit = g_ymap.gmin;
      if (x_unit > g_ymap.gmax)  x_unit = g_ymap.gmax;
      DEBUG_USER  yLOG_value   ("x_unit (1)", x_unit);
      x_unit  = MAP__closer (x_unit, &g_ymap);
      DEBUG_USER  yLOG_value   ("x_unit (2)", x_unit);
      x_grid  = g_ymap.map [x_unit];
      DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("bjmkt", a_minor) != NULL)  if (x_grid > g_ymap.gend)  x_grid = g_ymap.gend;
      DEBUG_USER  yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizey;
   x_grid *= g_gsizey;
   DEBUG_USER  yLOG_value   ("x_grid new", x_grid);
   MAP__move   (x_grid, &g_ymap);
   MAP__screen (&g_ymap);
   if (s_mapper != NULL) {
      s_mapper (YVIKEYS_UPDATE);
   }
   /*> MAP__print (&g_xmap);                                                          <*/
   /*> MAP__print (&g_ymap);                                                          <*/
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

int
MAP__horz             (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_beg       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER  yLOG_enter   (__FUNCTION__);
   DEBUG_USER  yLOG_char    ("a_major"   , a_major);
   DEBUG_USER  yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_xmap.gcur;
   DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
   DEBUG_USER  yLOG_value   ("avail"     , g_xmap.avail);
   DEBUG_USER  yLOG_value   ("gsizex"    , g_gsizex);
   x_qtr       = (g_xmap.avail - g_gsizex) / 4.0;
   DEBUG_USER  yLOG_double  ("x_qtr"     , x_qtr);
   x_beg       = g_xmap.beg;
   DEBUG_USER  yLOG_double  ("x_beg"     , x_beg);
   x_gmax  = g_xmap.map [g_xmap.gmax - g_gsizex];
   DEBUG_USER  yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_USER  yLOG_info    ("g_hsimple" , g_hsimple);
   if (a_major == ' ' && strchr (g_hsimple, a_minor) != NULL) {
      DEBUG_USER  yLOG_note    ("execute simple move");
      switch (a_minor) {
      case '0' : x_grid  = g_xmap.map [g_xmap.gmin];   break;
      case 'H' : x_grid -= g_gsizex * 5;                   break;
      case 'h' : x_grid -= g_gsizex;                       break;
      case 'l' : x_grid += g_gsizex;                       break;
      case 'L' : x_grid += g_gsizex * 5;                   break;
      case '$' : x_grid  = x_gmax;                         break;
      }
      DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(gotos)--------------------------*/
   DEBUG_USER  yLOG_info    ("g_hgoto"   , g_hgoto);
   if (a_major == 'g' && strchr (g_hgoto  , a_minor) != NULL) {
      DEBUG_USER  yLOG_note    ("execute goto move");
      switch (a_minor) {
      case 'S' : x_unit  = x_beg - (x_qtr * 4);            break;
      case 'H' : x_unit  = x_beg - (x_qtr * 2);            break;
      case 's' : x_unit  = x_beg;                          break;
      case 'h' : x_unit  = x_beg + (x_qtr * 1);            break;
      case 'c' : x_unit  = x_beg + (x_qtr * 2);            break;
      case 'l' : x_unit  = x_beg + (x_qtr * 3);            break;
      case 'e' : x_unit  = x_beg + (x_qtr * 4);            break;
      case 'L' : x_unit  = x_beg + (x_qtr * 6);            break;
      case 'E' : x_unit  = x_beg + (x_qtr * 8);            break;
      }
      DEBUG_USER  yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_xmap.gmin)  x_unit = g_xmap.gmin;
      if (x_unit > g_xmap.gmax)  x_unit = g_xmap.gmax;
      DEBUG_USER  yLOG_value   ("x_unit (1)" , x_unit);
      x_unit  = MAP__closer (x_unit, &g_xmap);
      DEBUG_USER  yLOG_value   ("x_unit (2)" , x_unit);
      x_grid  = g_xmap.map [x_unit];
      DEBUG_USER  yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("shcle", a_minor) != NULL)  if (x_grid > g_xmap.gend)  x_grid = g_xmap.gend;
      DEBUG_USER  yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizex;
   x_grid *= g_gsizex;
   DEBUG_USER  yLOG_value   ("x_grid new", x_grid);
   MAP__move   (x_grid, &g_xmap);
   MAP__screen (&g_xmap);
   if (s_mapper != NULL) {
      s_mapper (YVIKEYS_UPDATE);
      /*> MAP__print (&g_xmap);                                                       <*/
      /*> MAP__print (&g_ymap);                                                       <*/
   }
   /*---(complete)-----------------------*/
   DEBUG_USER  yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

char
yVIKEYS_map_refresh     (void)
{
   MAP__vert (' ', 'r');
   MAP__horz (' ', 'r');
   clear     ();
   return 0;
}

char
MAP_mode                (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
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
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ENTER) {
      MODE_enter  (MODE_SOURCE);
      /*> EDIT_pos    ('0');                                                          <*/
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
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
         MODE_enter  (SMOD_REPEAT);
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
          *>    MODE_enter  (MODE_VISUAL);                                       <* 
          *>    VISU_start  (CTAB, CCOL, CROW, VISU_FROM);                               <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
         /*> case 'V'      :                                                             <* 
          *>    MODE_enter  (MODE_VISUAL);                                       <* 
          *>    VISU_restore ();                                                         <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
      case ':'      :
         MODE_enter  (MODE_COMMAND);
         CMDS_start ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case '/'      :
         MODE_enter  (MODE_SEARCH);
         SRCH_start ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
         /*> case 's'      :                                                             <* 
          *>    EDIT_start  ("");                                                        <* 
          *>    MODE_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'i');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'i';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '='      :                                                             <* 
          *>    EDIT_start  ("=");                                                       <* 
          *>    MODE_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '#'      :                                                             <* 
          *>    EDIT_start  ("#");                                                       <* 
          *>    MODE_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '+'      :                                                             <* 
          *>    EDIT_start  ("+");                                                       <* 
          *>    MODE_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
         /*> case '-'      :                                                             <* 
          *>    EDIT_start  ("-");                                                       <* 
          *>    MODE_enter  (MODE_INPUT  );                                      <* 
          *>    MODE_input ('m', 'a');                                                   <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 'a';                                                              <* 
          *>    break;                                                                   <*/
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
         /*> case '\\'     :                                                             <* 
          *>    DEBUG_USER   yLOG_note    ("selected menu mode");                        <* 
          *>    MODE_enter  (SMOD_MENUS  );                                      <* 
          *>    my.menu = MENU_ROOT;                                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
      case '@'      :
         IF_MACRO_OFF {
            MACRO_reset  ();
            MODE_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         MACRO_reset  ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      case 'q'      :
         IF_MACRO_OFF {
            MODE_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         MACRO_rec_end ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
      case 'Q'      :
         MACRO_reset ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
         /*> case 'F'      :                                                             <* 
          *>    MODE_enter  (SMOD_FORMAT  );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
         /*> case ','      :                                                             <* 
          *>    MODE_enter  (SMOD_BUFFER  );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;                                                          <* 
          *>    break;                                                                   <*/
         /*> case '"'      :                                                             <* 
          *>    MODE_enter  (SMOD_REGISTER);                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;  /+ make sure double quote goes in prev char +/          <* 
          *>    break;                                                                   <*/
         /*> case 'm'      :                                                             <* 
          *> case '\''     :                                                             <* 
          *>    MODE_enter  (SMOD_MARK    );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;  /+ make sure single quote goes in prev char +/          <* 
          *>    break;                                                                   <*/
         /*> case 'E'      :                                                             <* 
          *>    MODE_enter  (SMOD_ERROR   );                                     <* 
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
         /*> MAP__print (&g_xmap);                                                    <*/
         /*> MAP__print (&g_ymap);                                                    <*/
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = MAP__horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_vsimple, a_minor) != 0) {
         rc = MAP__vert   (a_major, a_minor);
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
      if (strchr (g_hgoto, a_minor) != 0) {
         rc = MAP__horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_vgoto, a_minor) != 0) {
         rc = MAP__vert   (a_major, a_minor);
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

char       /*----: give current position info --------------------------------*/
MAP_xstatus        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "x_axis,  map %4da %4db %4dc %4de %4dt %4dl,  grid %3db %3dc %3de",
         g_xmap.avail, g_xmap.beg  , g_xmap.cur  , g_xmap.end  , g_xmap.tend , g_xmap.len  ,
         g_xmap.gbeg , g_xmap.gcur , g_xmap.gend );
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current position info --------------------------------*/
MAP_ystatus        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "y_axis,  map %4da %4db %4dc %4de %4dt %4dl,  grid %3db %3dc %3de",
         g_ymap.avail, g_ymap.beg  , g_ymap.cur  , g_ymap.end  , g_ymap.tend , g_ymap.len  ,
         g_ymap.gbeg , g_ymap.gcur , g_ymap.gend );
   /*---(complete)-----------------------*/
   return 0;
}





char
MAP__unit_ymap          (void)
{
   /*> printf ("running rowmap\n");                                                   <*/
   MAP__load ('1', &g_ymap, YVIKEYS_YMAP);
   return 0;
}

char
MAP__unit_xmap          (void)
{
   /*> printf ("running colmap\n");                                                   <*/
   MAP__load ('w', &g_xmap, YVIKEYS_YMAP);
   return 0;
}

char
MAP__unit_zmap          (void)
{
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
MAP__unit               (char *a_question, char a_index)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP unit         : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "horz"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz pos     : index %3d, grid %3d", g_xmap.cur, g_xmap.gcur);
   }
   else if (strcmp (a_question, "horz_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_xmap.avail, g_xmap.beg, g_xmap.cur, g_xmap.end, g_xmap.tend, g_xmap.len);
   }
   else if (strcmp (a_question, "horz_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz grids   :        b %3d, c %3d, e %3d", g_xmap.gbeg, g_xmap.gcur, g_xmap.gend);
   }
   else if (strcmp (a_question, "vert_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_ymap.avail, g_ymap.beg, g_ymap.cur, g_ymap.end, g_ymap.tend, g_ymap.len);
   }
   else if (strcmp (a_question, "vert_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert grids   :        b %3d, c %3d, e %3d", g_ymap.gbeg, g_ymap.gcur, g_ymap.gend);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}



