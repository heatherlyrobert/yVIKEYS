/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



static char    (*s_mapper)    (char  a_type);
static char    (*s_locator)   (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z);
static char*   (*s_addresser) (char *a_label, int  a_buf, int  a_x, int  a_y, int  a_z);

static char*   (*s_switcher ) (char  a_label);



char   g_coord    = YVIKEYS_RIGHT;


char   g_vsimple   [LEN_DESC ]   = "_ Kk jJ ~";
char   g_vgoto     [LEN_DESC ]   = "TK tkmjb JB  ";
char   g_vends     [LEN_DESC ]   = "T  tkmjb  B M azud";
char   g_vscroll   [LEN_DESC ]   = " K tkmjb J   ";

char   g_hsimple   [LEN_DESC ]   = "0 Hh lL $";
char   g_hgoto     [LEN_DESC ]   = "SH shcle LE  ";
char   g_hends     [LEN_DESC ]   = "S  shcle  E C azud";
char   g_hscroll   [LEN_DESC ]   = " H shcle L   ";

char   g_hword     [LEN_DESC ]   = "wbe WBE";

char   g_multimap  [LEN_DESC ]   = "cgz e  dx a   ";
char   g_multivisu [LEN_DESC ]   = "cgz e pd  a   ";

char   g_multisrc  [LEN_DESC ]   = "cgz    dx   Ff";
char   g_multiselc [LEN_DESC ]   = "cgz         Ff";

char   g_repeat    [LEN_DESC ]   = "123456789";
char   g_search    [LEN_DESC ]   = "[<>]";


static char *s_map_modes = ":/,\" vMVm' s=+-#F @qQG";



char
yvikeys_map_current     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   char        rce         =  -10;
   char        rc          =    0;
   --rce;  if (a_label != NULL) {
      rc = yvikeys_map_addresser (a_label, g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
      if (rc < 0) return rce;
   }
   if (a_buf != NULL)  *a_buf = g_bmap.gcur;
   if (a_x   != NULL)  *a_x   = g_xmap.gcur;
   if (a_y   != NULL)  *a_y   = g_ymap.gcur;
   if (a_z   != NULL)  *a_z   = g_zmap.gcur;
   return 0;
}

char         /*-> call host to update maps -----------[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yvikeys_map_reposition  (void)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(update)-------------------------*/
   DEBUG_MAP   yLOG_point   ("s_mapper"  , s_mapper);
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_note    ("calling source program mapper");
      s_mapper (YVIKEYS_UPDATE);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear a single map -----------------[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yvikeys__map_clear   (tMAPPED *a_map, char a_which)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_senter  (__FUNCTION__);
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(identity)-----------------------*/
   DEBUG_MAP   yLOG_snote   ("self");
   a_map->which = a_which;
   /*---(lefts)--------------------------*/
   DEBUG_MAP   yLOG_snote   ("mins");
   a_map->umin  = YVIKEYS_EMPTY;
   a_map->gamin = YVIKEYS_EMPTY;
   a_map->glmin = YVIKEYS_EMPTY;
   a_map->gprev = YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   DEBUG_MAP   yLOG_snote   ("maxs");
   a_map->gnext = YVIKEYS_EMPTY;
   a_map->glmax = YVIKEYS_EMPTY;
   a_map->gamax = YVIKEYS_EMPTY;
   a_map->umax  = YVIKEYS_EMPTY;
   /*---(map)----------------------------*/
   DEBUG_MAP   yLOG_snote   ("map");
   for (i = 0; i < LEN_HUGE; ++i) {
      a_map->map [i] = YVIKEYS_EMPTY;
   }
   /*---(indexes)------------------------*/
   DEBUG_MAP   yLOG_snote   ("screen");
   a_map->beg   = 0;
   a_map->cur   = 0;
   a_map->end   = 0;
   a_map->len   = 0;
   a_map->avail = 0;
   a_map->tend  = 0;
   /*---(grids)--------------------------*/
   DEBUG_MAP   yLOG_snote   ("grid");
   a_map->gbeg  = 0;
   a_map->gcur  = 0;
   a_map->gend  = 0;
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys__map_print   (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(headers)------------------------*/
   printf ("-  gmin amin lmin prev    ");
   /*> for (i = 0; i < LEN_HUGE; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , i);                                                       <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   printf ("next lmax amax gmax    aval beg- cur- end- len- tend\n");
   /*---(content)------------------------*/
   printf ("%c  "                        , a_map->which);
   printf ("%4d %4d %4d %4d    "         , a_map->gmin , a_map->gamin , a_map->glmin , a_map->gprev );
   /*> for (i = 0; i < LEN_HUGE; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , a_map->map [i]);                                          <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   /*---(end)----------------------------*/
   printf ("%4d %4d %4d %4d"              , a_map->gnext , a_map->glmax , a_map->gamax , a_map->gmax );
   printf ("   %4d %4d %4d %4d %4d %4d\n", a_map->avail, a_map->beg  , a_map->cur  , a_map->end  , a_map->len  , a_map->tend );
   return 0;
}

char
yvikeys__map_load     (char a_style, tMAPPED *a_map, char a_which)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         j           =    0;
   int         x_spot      =    0;
   /*---(clear)--------------------------*/
   yvikeys__map_clear  (a_map, a_which);
   /*---(col/row setup)------------------*/
   if (a_style == 'f') {
      for (j =  0; j < 16; ++j)  a_map->map [x_spot++] = 1;
      for (j =  0; j <  7; ++j)  a_map->map [x_spot++] = 2;
      for (j =  0; j <  7; ++j)  a_map->map [x_spot++] = 3;
      for (j =  0; j <  2; ++j)  a_map->map [x_spot++] = 4;
      for (j =  0; j < 20; ++j)  a_map->map [x_spot++] = 5;
      for (j =  0; j <  6; ++j)  a_map->map [x_spot++] = 6;
      for (j =  0; j <  6; ++j)  a_map->map [x_spot++] = 7;
      for (j =  0; j <  2; ++j)  a_map->map [x_spot++] = 8;

   }
   else if (strchr ("uads", a_style) != NULL) {
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
   } else if (a_style == '-') {  /* neg to pos grid        */
      for (i = -3; i <= 3; ++i) {
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
      }
   } else if (a_style == 'j') {   /* true grid the skips  */
      for (i = -120; i <= 300; i += 20) {
         a_map->map [x_spot++] = i;
      }
   } else if (a_style == 'w') {   /* get a big grid       */
      for (i = 0; i <= 40; ++i) {
         for (j =  0; j <  4; ++j)  a_map->map [x_spot++] = i;
      }
   } else if (a_style == '1') {
      for (i = 0; i <= 100; ++i)   a_map->map [x_spot++] = i;
   } else if (a_style == '0') {
      a_map->map [x_spot++] = 0;
   }
   /*---(unit min/max)-------------------*/
   a_map->umin  = 0;
   a_map->umax  = x_spot - 1;
   /*---(grid mins)----------------------*/
   a_map->gmin  = 0;
   a_map->gamin = 0;
   a_map->glmin = 0;
   a_map->gprev = 0;
   /*---(grid maxs)----------------------*/
   a_map->gnext = a_map->map [a_map->umax];
   a_map->glmax = a_map->map [a_map->umax];
   a_map->gamax = a_map->map [a_map->umax];
   a_map->gmax  = a_map->map [a_map->umax];
   /*---(final setup)--------------------*/
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
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_map_config        (char a_coord, void *a_mapper, void *a_locator, void *a_addresser)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (MODE_MAP)) {
      DEBUG_MAP    yLOG_note    ("init must complete before config");
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   g_coord      = a_coord;
   s_mapper     = a_mapper;
   s_locator    = a_locator;
   s_addresser  = a_addresser;
   /*---(update status)------------------*/
   STATUS_conf_set   (MODE_MAP, '1');
   /*---(update)-------------------------*/
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_snote   ("call host mapper");
      s_mapper (YVIKEYS_INIT);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> turn label into coordinates --------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
yvikeys_map_locator     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rce         =    0;
   int         rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP_M yLOG_enter   (__FUNCTION__);
   /*---(check locator)------------------*/
   DEBUG_MAP_M yLOG_point   ("locator"    , s_locator);
   --rce;  if (s_locator == NULL) {
      DEBUG_MAP_M yLOG_exitr   (__FUNCTION__,rce);
      return rce;
   }
   /*---(display input)------------------*/
   DEBUG_MAP_M yLOG_point   ("a_label"   , a_label);
   if (a_label != NULL)  DEBUG_MAP_M yLOG_info    ("a_label"   , a_label);
   DEBUG_MAP_M yLOG_point   ("a_buf"     , a_buf);
   if (a_buf   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_buf"    , *a_buf);
   DEBUG_MAP_M yLOG_point   ("a_x"       , a_x);
   if (a_x     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_x"      , *a_x);
   DEBUG_MAP_M yLOG_point   ("a_y"       , a_y);
   if (a_y     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_y"      , *a_y);
   DEBUG_MAP_M yLOG_point   ("a_z"       , a_z);
   if (a_z     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_z"      , *a_z);
   /*---(call locator)-------------------*/
   rc = s_locator (a_label, a_buf, a_x, a_y, a_z);
   DEBUG_MAP   yLOG_value   ("locator"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_MAP_M yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display output)-----------------*/
   if (a_buf != NULL)  DEBUG_MAP_M yLOG_value   ("*a_buf"    , *a_buf);
   if (a_x   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_x"      , *a_x);
   if (a_y   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_y"      , *a_y);
   if (a_z   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_z"      , *a_z);
   /*---(complete)-----------------------*/
   DEBUG_MAP_M yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> turn coordinates into label --------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
yvikeys_map_addresser   (char *a_label, int a_buf, int a_x, int a_y, int a_z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(check addresser)----------------*/
   DEBUG_MAP   yLOG_point   ("addresser" , s_addresser);
   --rce;  if (s_addresser == NULL)  {
      strlcpy (a_label, "-", LEN_LABEL);
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display input)------------------*/
   DEBUG_MAP   yLOG_point   ("a_label"   , a_label);
   DEBUG_MAP   yLOG_value   ("a_buf"     , a_buf);
   DEBUG_MAP   yLOG_value   ("a_x"       , a_x);
   DEBUG_MAP   yLOG_value   ("a_y"       , a_y);
   DEBUG_MAP   yLOG_value   ("a_z"       , a_z);
   /*---(call locator)-------------------*/
   rc = s_addresser (a_label, a_buf, a_x, a_y, a_z);
   DEBUG_MAP   yLOG_value   ("addresser" , rc);
   --rce;  if (rc < 0) {
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display output)-----------------*/
   if (a_label != NULL)  DEBUG_MAP   yLOG_info    ("a_label"   , a_label);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_map_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_MAP)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_note    ("default globals");
   g_coord    = YVIKEYS_OFFICE;
   s_mapper   = NULL;
   DEBUG_PROG   yLOG_note    ("map clearing");
   yvikeys__map_clear (&g_bmap, YVIKEYS_BMAP);
   yvikeys__map_clear (&g_xmap, YVIKEYS_XMAP);
   yvikeys__map_clear (&g_ymap, YVIKEYS_YMAP);
   yvikeys__map_clear (&g_zmap, YVIKEYS_ZMAP);
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_MAP);
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*-> idendify closest grid to position --[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yvikeys__map_closer   (int a_position, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   int         i           =    0;
   int         x_target    =    0;
   int         x_left      = YVIKEYS_EMPTY;
   int         x_right     = YVIKEYS_EMPTY;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_position", a_position);
   /*---(prepare)------------------------*/
   x_target = a_map->map [a_position];
   DEBUG_MAP   yLOG_value   ("x_target"  , x_target);
   DEBUG_MAP   yLOG_value   ("umin"      , a_map->umin);
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   /*---(look right)---------------------*/
   DEBUG_MAP   yLOG_note    ("go right");
   for (i = a_position; i <= a_map->umax; ++i) {
      DEBUG_MAP   yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <= x_target)        continue;
      x_right = i;
      break;
   }
   if (x_right == YVIKEYS_EMPTY)  DEBUG_MAP   yLOG_note    ("x_right not found");
   else                           DEBUG_MAP   yLOG_value   ("x_right"   , x_right - a_position);
   /*---(look left)----------------------*/
   DEBUG_MAP   yLOG_note    ("go left");
   for (i = a_position; i >= a_map->umin; --i) {
      DEBUG_MAP   yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <  x_target) {
         DEBUG_MAP   yLOG_value   ("x_left"    , x_left);
         break;
      }
      x_left  = i;
   }
   if (x_left  == YVIKEYS_EMPTY)  DEBUG_MAP   yLOG_note    ("x_left not found");
   else                           DEBUG_MAP   yLOG_value   ("x_left"    , a_position - x_left);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   rc = x_left;
   if      (x_left  == YVIKEYS_EMPTY && x_right == YVIKEYS_EMPTY)  rc = a_position;
   else if (x_left  == YVIKEYS_EMPTY)                              rc = x_right;
   else if (x_right == YVIKEYS_EMPTY)                              rc = x_left;
   else if (x_right - a_position <  a_position - x_left)           rc = x_right;
   return rc;
}

char
yvikeys__map_move     (int a_target, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_target"  , a_target);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(defense)------------------------*/
   --rce;  if (a_map    == NULL) {
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(make sure index is rational)----*/
   DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
   DEBUG_MAP   yLOG_value   ("umin"      , a_map->umin);
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   if (a_map->cur < a_map->umin)  a_map->cur = a_map->umin;
   if (a_map->cur > a_map->umax)  a_map->cur = a_map->umax;
   DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
   a_map->gcur = a_map->map [a_map->cur];
   DEBUG_MAP   yLOG_value   ("gcur"      , a_map->gcur);
   /*---(shortcut)-----------------------*/
   if (a_map->gcur == a_target) {
      DEBUG_MAP   yLOG_note    ("already in position");
      DEBUG_MAP   yLOG_exit    (__FUNCTION__);
      return 2;
   }
   /*---(check to right)-----------------*/
   if (a_map->gcur <  a_target) {
      DEBUG_MAP   yLOG_note    ("must move to right");
      for (i = a_map->cur; i <= a_map->umax; ++i) {
         if (a_map->map [i] <  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->umax;
   }
   /*---(check to left)------------------*/
   else {
      DEBUG_MAP   yLOG_note    ("must move to left");
      /*---(find the right grid)---------*/
      for (i = a_map->cur; i >= a_map->umin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         /*---(get to leftmost)----------*/
         for (i = a_map->cur; i >= a_map->umin; --i) {
            if (a_map->map [i] != a_target)   break;
            a_map->cur  = i;
            DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         }
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->umin;
   }
   /*---(get to leftmost)----------*/
   a_map->gcur   = a_map->map [a_map->cur];
   for (i = a_map->cur; i >= a_map->umin; --i) {
      if (a_map->map [i] != a_map->gcur)   break;
      a_map->cur  = i;
   }
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return  1;
}

char
yvikeys__screen_small     (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(prepare)------------------------*/
   a_map->beg   = a_map->umin;
   a_map->len   = a_map->umax - a_map->umin + 1;
   a_map->end   = a_map->umax;
   a_map->tend  = a_map->umax;
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 2;
}


char
yvikeys__screen_beg       (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   int         x_tend      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(find closest beg backward)------*/
   for (i = a_map->beg; i > a_map->umin; --i) {
      a_map->beg   = i;
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   DEBUG_MAP   yLOG_value   ("beg"       , a_map->beg);
   /*---(prepare)------------------------*/
   x_tend = a_map->beg + a_map->avail - 1;
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   if (x_tend <  a_map->umax)  a_map->tend  = x_tend;
   DEBUG_MAP   yLOG_value   ("tend"      , a_map->tend);
   /*---(can not fill screen?)-----------*/
   --rce;  if (a_map->tend > a_map->umax)   return rce;
   /*---(find end of last full grid)-----*/
   for (i = a_map->tend; i >= a_map->umin; --i) {
      a_map->end   = i;
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   DEBUG_MAP   yLOG_value   ("end"       , a_map->end);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
yvikeys__screen_end       (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find end of end)----------------*/
   /*> printf ("yvikeys__screen_end\n");                                          <*/
   /*> printf ("end  = %3d\n", a_map->end);                                           <*/
   for (i = a_map->end; i <= a_map->umax; ++i) {
      a_map->end   = i;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      a_map->tend  = i;
      /*> printf ("tend = %3d\n", a_map->tend);                                       <*/
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(check overrun)------------------*/
   /*> printf ("umax = %3d\n", a_map->umax);                                          <*/
   /*> printf ("umin = %3d\n", a_map->umin);                                          <*/
   /*---(run the final)------------------*/
   a_map->beg   = a_map->tend - a_map->avail + 1;
   /*---(find next beg forward)----------*/
   for (i = a_map->beg; i < a_map->umax; ++i) {
      a_map->beg   = i;
      /*> printf ("beg  = %3d\n", a_map->beg);                                        <*/
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   /*---(handle normally)----------------*/
   rc = yvikeys__screen_beg (a_map);
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys__screen         (tMAPPED *a_map)
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
   if (a_map->cur < a_map->umin)  a_map->cur = a_map->umin;
   if (a_map->cur > a_map->umax) {
      a_map->cur = a_map->umax;
      for (i = a_map->cur; i > a_map->umin; --i) {
         a_map->cur   = i;
         x_curr       = a_map->map [i    ];
         x_prev       = a_map->map [i - 1];
         if (x_curr != x_prev)   break;
      }
   }
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   /*---(screen fits all)----------------*/
   if (a_map->umax - a_map->umin <= a_map->avail) {
      /*> printf ("processing a small\n");                                            <*/
      rc = yvikeys__screen_small (a_map);
      return rc;
   }
   /*---(from beginning)-----------------*/
   if (a_map->cur < a_map->beg) {
      myVIKEYS.redraw = 'y';
      /*> printf ("processing a left\n");                                             <*/
      a_map->beg = a_map->cur;
      for (i = a_map->cur; i >= a_map->umin; --i) {
         a_map->beg = i;
         rc = yvikeys__screen_beg (a_map);
         if (rc > 0) break;
      }
   }
   /*---(from ending)--------------------*/
   else if (a_map->cur > a_map->end) {
      myVIKEYS.redraw = 'y';
      /*> printf ("processing a right\n");                                            <*/
      a_map->end   = a_map->cur;
      a_map->tend  = a_map->cur;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      rc = yvikeys__screen_end (a_map);
   }
   /*---(just a refresh)-----------------*/
   else {
      /*> printf ("processing a refresh\n");                                          <*/
      rc = yvikeys__screen_beg (a_map);
   }
   /*---(align grid)---------------------*/
   a_map->len   = a_map->end - a_map->beg + 1;
   a_map->gbeg  = a_map->map [a_map->beg];
   a_map->gcur  = a_map->map [a_map->cur];
   a_map->gend  = a_map->map [a_map->end];
   /*---(complete)-----------------------*/
   return rc;
}

char
yVIKEYS_jump              (int a_buf, int a_x, int a_y, int a_z)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_buf"     , a_buf);
   yvikeys__map_move   (a_buf, &g_bmap);
   yvikeys__screen (&g_bmap);
   DEBUG_MAP   yLOG_value   ("a_x"       , a_x);
   yvikeys__map_move   (a_x, &g_xmap);
   yvikeys__screen (&g_xmap);
   DEBUG_MAP   yLOG_value   ("a_y"       , a_y);
   yvikeys__map_move   (a_y, &g_ymap);
   yvikeys__screen (&g_ymap);
   DEBUG_MAP   yLOG_value   ("a_z"       , a_z);
   yvikeys__map_move   (a_z, &g_zmap);
   yvikeys__screen (&g_zmap);
   yvikeys_map_reposition  ();
   yvikeys_visu_update ();
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys__map_vert     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_char    ("a_major"   , a_major);
   DEBUG_MAP   yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_ymap.gcur;
   DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   x_qtr       = (g_ymap.avail - g_gsizey) / 4.0;
   DEBUG_MAP   yLOG_double  ("x_qtr"     , x_qtr);
   x_gmax  = g_ymap.map [g_ymap.umax - g_gsizey];
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_vsimple" , g_vsimple);
   if (a_major == ' ' && strchr (g_vsimple, a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case '_' : x_grid  = g_ymap.map [g_ymap.umin];  break;
         case 'K' : x_grid -= g_gsizey * 5;  break;
         case 'k' : x_grid -= g_gsizey;      break;
         case 'j' : x_grid += g_gsizey;      break;
         case 'J' : x_grid += g_gsizey * 5;  break;
         case '~' : x_grid  = x_gmax;        break;
         }
      } else {
         switch (a_minor) {
         case '~' : x_grid  = g_ymap.map [g_ymap.umin];  break;
         case 'J' : x_grid -= g_gsizey * 5;  break;
         case 'j' : x_grid -= g_gsizey;      break;
         case 'k' : x_grid += g_gsizey;      break;
         case 'K' : x_grid += g_gsizey * 5;  break;
         case '_' : x_grid  = x_gmax;        break;
         }
      }
   }
   /*---(gotos)--------------------------*/
   DEBUG_MAP   yLOG_info    ("g_vgoto"   , g_vgoto);
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
      DEBUG_MAP   yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_ymap.umin)  x_unit = g_ymap.umin;
      if (x_unit > g_ymap.umax)  x_unit = g_ymap.umax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)", x_unit);
      x_unit  = yvikeys__map_closer (x_unit, &g_ymap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)", x_unit);
      x_grid  = g_ymap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("bjmkt", a_minor) != NULL)  if (x_grid > g_ymap.gend)  x_grid = g_ymap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(ends and edges)-----------------*/
   DEBUG_MAP   yLOG_info    ("g_vends"   , g_vends);
   if (a_major == 'e' && strchr (g_vends  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute ends and edges move");
      DEBUG_MAP   yLOG_value   ("gmin"      , g_ymap.gmin);
      DEBUG_MAP   yLOG_value   ("gamin"     , g_ymap.gamin);
      DEBUG_MAP   yLOG_value   ("glmin"     , g_ymap.glmin);
      DEBUG_MAP   yLOG_value   ("gprev"     , g_ymap.gprev);
      DEBUG_MAP   yLOG_value   ("gnext"     , g_ymap.gnext);
      DEBUG_MAP   yLOG_value   ("glmax"     , g_ymap.glmax);
      DEBUG_MAP   yLOG_value   ("gamax"     , g_ymap.gamax);
      DEBUG_MAP   yLOG_value   ("gmax"      , g_ymap.gmax);
      switch (a_minor) {
      case 'a' : case 'u' :
      case 'T' : x_grid = g_ymap.gamin;     break;
      case 't' : x_grid = g_ymap.glmin;     break;
      case 'k' : x_grid = g_ymap.gprev;     break;
      case 'm' : break;
      case 'M' : break;
      case 'j' : x_grid = g_ymap.gnext;     break;
      case 'b' : x_grid = g_ymap.glmax;     break;
      case 'z' : case 'd' :
      case 'B' : x_grid = g_ymap.gamax;     break;
      }
      if (x_grid < g_ymap.gmin)  x_grid = g_ymap.gcur;
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizey;
   x_grid *= g_gsizey;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   yvikeys__map_move   (x_grid, &g_ymap);
   yvikeys_visu_update ();
   yvikeys__screen (&g_ymap);
   yvikeys_map_reposition  ();
   /*> yvikeys__map_print (&g_xmap);                                                          <*/
   /*> yvikeys__map_print (&g_ymap);                                                          <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys__map_horz     (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_beg       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_char    ("a_major"   , a_major);
   DEBUG_MAP   yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_xmap.gcur;
   DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   DEBUG_MAP   yLOG_value   ("avail"     , g_xmap.avail);
   DEBUG_MAP   yLOG_value   ("gsizex"    , g_gsizex);
   x_qtr       = (g_xmap.avail - g_gsizex) / 4.0;
   DEBUG_MAP   yLOG_double  ("x_qtr"     , x_qtr);
   x_beg       = g_xmap.beg;
   DEBUG_MAP   yLOG_double  ("x_beg"     , x_beg);
   x_gmax      = g_xmap.gmax;
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_hsimple" , g_hsimple);
   if (a_major == ' ' && strchr (g_hsimple, a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute simple move");
      switch (a_minor) {
      case '0' : x_grid  = g_xmap.map [g_xmap.umin];   break;
      case 'H' : x_grid -= g_gsizex * 5;                   break;
      case 'h' : x_grid -= g_gsizex;                       break;
      case 'l' : x_grid += g_gsizex;                       break;
      case 'L' : x_grid += g_gsizex * 5;                   break;
      case '$' : x_grid  = x_gmax;                         break;
      }
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(gotos)--------------------------*/
   DEBUG_MAP   yLOG_info    ("g_hgoto"   , g_hgoto);
   if (a_major == 'g' && strchr (g_hgoto  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute goto move");
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
      DEBUG_MAP   yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_xmap.umin)  x_unit = g_xmap.umin;
      if (x_unit > g_xmap.umax)  x_unit = g_xmap.umax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)" , x_unit);
      x_unit  = yvikeys__map_closer (x_unit, &g_xmap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)" , x_unit);
      x_grid  = g_xmap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("shcle", a_minor) != NULL)  if (x_grid > g_xmap.gend)  x_grid = g_xmap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(ends and edges)-----------------*/
   DEBUG_MAP   yLOG_info    ("g_hends"   , g_hends);
   if (a_major == 'e' && strchr (g_hends  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute ends and edges move");
      DEBUG_MAP   yLOG_value   ("gmin"      , g_xmap.gmin);
      DEBUG_MAP   yLOG_value   ("gamin"     , g_xmap.gamin);
      DEBUG_MAP   yLOG_value   ("glmin"     , g_xmap.glmin);
      DEBUG_MAP   yLOG_value   ("gprev"     , g_xmap.gprev);
      DEBUG_MAP   yLOG_value   ("gnext"     , g_xmap.gnext);
      DEBUG_MAP   yLOG_value   ("glmax"     , g_xmap.glmax);
      DEBUG_MAP   yLOG_value   ("gamax"     , g_xmap.gamax);
      DEBUG_MAP   yLOG_value   ("gmax"      , g_xmap.gmax);
      switch (a_minor) {
      case 'a' : case 'd' :
      case 'S' : x_grid = g_xmap.gamin;     break;
      case 's' : x_grid = g_xmap.glmin;     break;
      case 'h' : x_grid = g_xmap.gprev;     break;
      case 'c' : break;
      case 'C' : break;
      case 'l' : x_grid = g_xmap.gnext;     break;
      case 'e' : x_grid = g_xmap.glmax;     break;
      case 'z' : case 'u' :
      case 'E' : x_grid = g_xmap.gamax;     break;
      }
      if (x_grid < g_xmap.gmin)  x_grid = g_xmap.gcur;
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizex;
   x_grid *= g_gsizex;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   yvikeys__map_move   (x_grid, &g_xmap);
   yvikeys_visu_update ();
   yvikeys__screen (&g_xmap);
   yvikeys_map_reposition  ();
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_map_refresh     (void)
{
   /*---(update)-------------------------*/
   DEBUG_MAP   yLOG_point   ("s_mapper"  , s_mapper);
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_note    ("calling source program mapper");
      s_mapper (YVIKEYS_UPDATE);
   }
   /*---(refresh position)---------------*/
   yvikeys__map_vert (' ', 'r');
   yvikeys__map_horz (' ', 'r');
   clear     ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys__map_mode_chg   (char a_minor)
{
   char        rc          =   -1;
   char        t           [5];
   /*---(common modes)----------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("change"    , a_minor);
   switch (a_minor) {
   case 'G'      :
      DEBUG_USER   yLOG_note    ("entering god-mode");
      MODE_enter  (MODE_GOD     );
      rc = 0;
      break;
   case ':'      :
      SOURCE_start   (":");
      rc = 'a';
      break;
   case '/'      :
      SOURCE_start   ("/");
      rc = 'a';
      break;
   case ','      :
      MODE_enter  (SMOD_BUFFER  );
      rc = a_minor;
      break;
   case '"'      :
      MODE_enter  (SMOD_MREG);
      rc = a_minor;
      break;
   case 'E'      :
      /*> MODE_enter  (SMOD_ERROR   );                                                <* 
       *> DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
       *> rc = a_minor;                                                               <*/
      break;
   case 'F'      :
      DEBUG_USER   yLOG_note    ("calling custom format mode");
      rc = MODE_enter  (XMOD_FORMAT  );
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(selecting and marking)-------*/
   switch (a_minor) {
   case 'v'      :
      if (yvikeys_visu_islive ())   yvikeys_visu_reverse  ();
      else                          yvikeys_visu_makelive ();
      rc = 0;
      break;
   case 'M'      : case 'V'      :
      DEBUG_USER   yLOG_note    ("entering visual selection history sub-mode");
      rc = MODE_enter  (UMOD_VISUAL  );
      if (rc >= 0)  rc = a_minor;
      break;
   case 'm'      : case '\''     :
      DEBUG_USER   yLOG_note    ("entering location mark sub-mode");
      rc = MODE_enter  (UMOD_MARK    );
      if (rc >= 0)  rc = a_minor;
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(source entry)----------------*/
   if (yvikeys_visu_isdead () && strchr ("s=+-#", a_minor) != NULL) {
      if (a_minor == 's')   strlcpy (t, "", LEN_LABEL);
      else                  sprintf (t, "%c", a_minor);
      SOURCE_start   (t);
      rc = 'a';
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(import/export)---------------*/
   if (yvikeys_visu_islive () && strchr ("+-"   , a_minor) != NULL) {
      MODE_enter (SMOD_MREG);
      rc = yvikeys_mreg_smode  ('"', a_minor);
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(macros)----------------------*/
   switch (a_minor) {
   case '@'      :
      IF_MACRO_OFF {
         yvikeys_macro_reset  ();
         MODE_enter  (SMOD_MACRO   );
         rc = a_minor;
      } else {
         yvikeys_macro_reset  ();
         rc = -1;
      }
      break;
   case 'q'      :
      IF_MACRO_OFF {
         MODE_enter  (SMOD_MACRO   );
         rc = a_minor;
      } else {
         rc = yvikeys_macro_recend ();
      }
      break;
   case 'Q'      :
      rc = yvikeys_macro_reset ();
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                       big combinations                       ----===*/
/*====================------------------------------------====================*/
static void  o___COMBO___________o () { return; }

static  int    s_b, s_xb, s_xe, s_yb, s_ye, s_z;   /* starting selection      */
static  int    s_xl, s_yl;                         /* starting selection size */
static  int    s_top, s_lef;                       /* screen top/left         */
static  int    s_xb2, s_xe2, s_yb2, s_ye2;         /* selection for copy      */
static  int    s_xp, s_yp;                         /* top/left for paste      */

char
yvikeys__combo_prep     (char a_clear)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(get current)--------------------*/
   rc = yVIKEYS_visu_coords (&s_b, &s_xb, &s_xe, &s_yb, &s_ye, &s_z);
   /*---(selection size)-----------------*/
   s_xl   = (s_xe - s_xb) + 1;
   s_yl   = (s_ye - s_yb) + 1;
   /*---(clear)--------------------------*/
   if (a_clear == 'y')  rc = yvikeys_mreg_clear ();
   rc = yvikeys_visu_clear ();
   /*---(save top/left)------------------*/
   s_lef  = g_xmap.beg;
   s_top  = g_ymap.beg;
   /*---(copy loc default)---------------*/
   s_xb2  = s_xb;
   s_yb2  = s_yb;
   s_xe2  = s_xe;
   s_ye2  = s_ye;
   /*---(paste loc default)--------------*/
   s_xp   = s_xb;
   s_yp   = s_yb;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys__combo_wrap     (char a_clear)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(defense on boundaries)----------*/
   if (s_xp  <  0          )  s_xp  = 0;
   if (s_xp  >= g_xmap.gmax)  s_xp  = g_xmap.gmax;
   if (s_xb2 <  0          )  s_xb2 = 0;
   if (s_xb2 >= g_xmap.gmax)  s_xb2 = g_xmap.gmax;
   if (s_xe2 <  0          )  s_xe2 = 0;
   if (s_xe2 >= g_xmap.gmax)  s_xe2 = g_xmap.gmax;
   if (s_yp  <  0          )  s_yp  = 0;
   if (s_yp  >= g_ymap.gmax)  s_yp  = g_ymap.gmax;
   if (s_yb2 <  0          )  s_yb2 = 0;
   if (s_yb2 >= g_ymap.gmax)  s_yb2 = g_ymap.gmax;
   if (s_ye2 <  0          )  s_ye2 = 0;
   if (s_ye2 >= g_ymap.gmax)  s_ye2 = g_ymap.gmax;
   /*---(copy/clear)---------------------*/
   rc = yvikeys_visu_exact  (s_b, s_xb2, s_xe2, s_yb2, s_ye2, s_z);
   if (a_clear != 'c')  rc = yvikeys_mreg_save  ();
   if (a_clear == 'y')  rc = yvikeys_mreg_clear_combo ();
   else                 rc = yvikeys_mreg_clear ();
   rc = yvikeys_visu_clear ();
   /*---(paste/move)---------------------*/
   if (a_clear != 'c') {
      rc = yVIKEYS_jump (s_b, s_xp, s_yp, s_z);
      rc = yvikeys_mreg_paste_combo ("combo");
   }
   g_xmap.beg = s_lef;
   g_ymap.beg = s_top;
   rc = yVIKEYS_jump (s_b, s_xb, s_yb, s_z);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> complex delete action --------------[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys__combo_clear    (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        x_len       =    0;
   char        x_pos       =    0;
   char        x_minors    [LEN_LABEL]  = "lhxX��jkyY��iozZ�.";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MAP    yLOG_char    ("visu live" , yvikeys_visu_islive ());
   --rce;  if (yvikeys_visu_islive ()) {
      DEBUG_MAP    yLOG_note    ("function only handles non-selected deletes/clearing");
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MAP    yLOG_char    ("a_major"   , chrvisible (a_major));
   --rce;  if (a_major != 'x') {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MAP    yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_MAP    yLOG_info    ("valid"     , x_minors);
   --rce;  if (a_minor == 0 || strchr (x_minors, a_minor) == NULL) {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get and clear)------------------*/
   rc = yvikeys__combo_prep ('-');
   /*---(full lines)---------------------*/
   switch (a_minor) {
   case 'x' : case 'X' :
      s_yp  = s_yb2 = g_ymap.gmin;
      s_ye2 = g_ymap.gmax;
      break;
   case 'y' : case 'Y' :
      s_xp  = s_xb2 = g_xmap.gmin;
      s_xe2 = g_xmap.gmax;
      break;
   }
   /*---(clear)--------------------------*/
   rc = yvikeys__combo_wrap ('c');
   /*---(move after)---------------------*/
   switch (a_minor) {
   case 'l' : case 'x' :  rc = yvikeys__map_horz (G_KEY_SPACE, 'l');   break;
   case 'h' : case 'X' :  rc = yvikeys__map_horz (G_KEY_SPACE, 'h');   break;
   case 'j' : case 'y' :  rc = yvikeys__map_vert (G_KEY_SPACE, 'j');   break;
   case 'k' : case 'Y' :  rc = yvikeys__map_vert (G_KEY_SPACE, 'k');   break;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> complex delete action --------------[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys__combo_delete   (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        x_pos       =    0;
   char        x_minors    [LEN_LABEL]  = "lhxX�jkyY�iozZ";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MAP    yLOG_char    ("visu live" , yvikeys_visu_islive ());
   DEBUG_MAP    yLOG_char    ("a_major"   , chrvisible (a_major));
   --rce;  if (a_major != 'd') {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MAP    yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_MAP    yLOG_info    ("valid"     , x_minors);
   --rce;  if (a_minor == 0 || strchr (x_minors, a_minor) == NULL) {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get and clear)------------------*/
   rc = yvikeys__combo_prep ('y');
   /*---(horizontal)---------------------*/
   switch (a_minor) {
   case 'l' : case 'x' :
      s_xb2 = s_xe + 1;
      s_xe2 = g_xmap.gmax;
      if (a_minor == 'l')  break;
      s_yp  = s_yb2 = g_ymap.gmin;
      s_ye2 = g_ymap.gmax;
      break;
   case 'h' : case 'X' :
      s_xb2 = g_xmap.gmin;
      s_xe2 = s_xb - 1;
      s_xp  = s_xe - (s_xe2 - s_xb2);
      if (a_minor == 'h')  break;
      s_yp  = s_yb2 = g_ymap.gmin;
      s_ye2 = g_ymap.gmax;
      break;
   }
   /*---(vertical)-----------------------*/
   switch (a_minor) {
   case 'j' : case 'y' :
      s_yb2 = s_ye + 1;
      s_ye2 = g_ymap.gmax;
      if (a_minor == 'j')  break;
      s_xp  = s_xb2 = g_xmap.gmin;
      s_xe2 = g_xmap.gmax;
      break;
   case 'k' : case 'Y' :
      s_yp  = g_ymap.gmin + 1;
      s_ye2 = s_ye - 1;
      s_yb2 = g_ymap.gmin;
      if (a_minor == 'k')  break;
      s_xp  = s_xb2 = g_xmap.gmin;
      s_xe2 = g_xmap.gmax;
      break;
   }
   /*---(zoom)---------------------------*/
   /*---(paste/move)---------------------*/
   rc = yvikeys__combo_wrap ('y');
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> complex append action --------------[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys__combo_append   (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        x_pos       =    0;
   char        x_minors    [LEN_LABEL]  = "lhxX�jkyY�iozZ";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MAP    yLOG_char    ("visu live" , yvikeys_visu_islive ());
   DEBUG_MAP    yLOG_char    ("a_major"   , chrvisible (a_major));
   --rce;  if (a_major != 'a') {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MAP    yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_MAP    yLOG_info    ("valid"     , x_minors);
   --rce;  if (a_minor == 0 || strchr (x_minors, a_minor) == NULL) {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get and clear)------------------*/
   rc = yvikeys__combo_prep ('-');
   /*---(horizontal)---------------------*/
   switch (a_minor) {
   case 'l' : case 'x' :  /* insert to right, and full column version         */
      s_xb2 = s_xb + 1;
      s_xe2 = g_xmap.gmax - s_xl + 1;
      s_xp  = s_xb + s_xl + 1;
      ++s_xb;
      if (a_minor == 'l')  break;
      s_yp  = s_yb2 = g_ymap.gmin;
      s_ye2 = g_ymap.gmax;
      break;
   case 'h' : case 'X' :  /* insert to left, and full column version          */
      s_xb2 = s_xb;
      s_xe2 = g_xmap.gmax - s_xl;
      s_xp  = s_xb + s_xl;
      if (a_minor == 'h')  break;
      s_yp  = s_yb2 = g_ymap.gmin;
      s_ye2 = g_ymap.gmax;
      break;
   }
   /*---(vertical)-----------------------*/
   switch (a_minor) {
   case 'j' : case 'y' :  /* insert below, and full column version            */
      s_yb2 = s_yb + 1;
      s_ye2 = g_ymap.gmax - s_yl + 1;
      s_yp  = s_yb + s_yl + 1;
      ++s_yb;
      if (a_minor == 'j')  break;
      s_xp  = s_xb2 = g_xmap.gmin;
      s_xe2 = g_xmap.gmax;
      break;
   case 'k' : case 'Y' :  /* insert above, and full column version            */
      s_yb2 = s_yb;
      s_ye2 = g_ymap.gmax - s_yl;
      s_yp  = s_yb + s_yl;
      if (a_minor == 'k')  break;
      s_xp  = s_xb2 = g_xmap.gmin;
      s_xe2 = g_xmap.gmax;
      break;
   }
   /*---(zoom)---------------------------*/
   /*---(paste/move)---------------------*/
   rc = yvikeys__combo_wrap ('-');
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       mode key handling                      ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char
yvikeys_map_mode        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_grid      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(nothing to do)------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("enter to begin input");
      SOURCE_start ("�");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape to clear selection");
      yvikeys_visu_clear ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single key)---------------------*/
   DEBUG_USER   yLOG_note    ("review single keys");
   --rce;
   if (a_major == ' ') {
      DEBUG_USER   yLOG_note    ("no or empty major");
      /*---(repeat)----------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("repeating");
         MODE_enter  (UMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(multikey prefixes)-----------*/
      if (yvikeys_visu_isdead () && strchr (g_multimap , a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("prefix of multimap keystring");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      if (yvikeys_visu_islive () && strchr (g_multivisu, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("prefix of visual multimap keystring");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(funky moves)-----------------*/
      if (a_minor == ':') {
         x_grid = REPEAT_use ();
         if (x_grid > 0) {
            rc = yvikeys__map_move   (x_grid, &g_ymap);
            yvikeys__screen (&g_ymap);
            yvikeys_map_reposition  ();
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return rc;;
         }
         /* non-repeat fall thru  */
      }
      if (a_minor == '|') {
         rc = yvikeys__map_move   (REPEAT_use (), &g_xmap);
         yvikeys__screen (&g_xmap);
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(mode changes)----------------*/
      if (strchr (s_map_modes, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("mode changes");
         rc = yvikeys__map_mode_chg (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr ("yYpPx", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a map register mode");
         MODE_enter (SMOD_MREG);
         rc = yvikeys_mreg_smode  (G_KEY_SPACE, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(history)---------------------*/
      if (a_minor == 'u') {
         yvikeys_visu_clear ();
         rc = yvikeys_hist_undo ();
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (a_minor == 'U') {
         yvikeys_visu_clear ();
         rc = yvikeys_hist_redo ();
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(normal)----------------------*/
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vsimple, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_search, a_minor) != 0) {
         rc = yvikeys_srch_next   (a_minor);
         if (rc >= 0) {
            yvikeys__screen (&g_xmap);
            yvikeys__screen (&g_ymap);
            yvikeys__screen (&g_zmap);
            yvikeys_map_reposition  ();
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      DEBUG_USER   yLOG_note    ("no matches found");
   }
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      if (strchr (g_hgoto, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vgoto, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
   }
   /*---(ends family)--------------------*/
   if (a_major == 'e') {
      if (strchr ("azud", a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_hends, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vends, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr ("xyz*!", a_minor) != 0) {
         rc = yvikeys_visu_locking (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
   }
   /*---(paste family)-------------------*/
   if (a_major == 'p') {
      switch (a_minor) {
      case '_' :  rc = yvikeys_mreg_visual ();            break;
      case '#' :  rc = yvikeys_mreg_paste  ("clear");     break;
      case 'n' :  rc = yvikeys_mreg_paste  ("normal");    break;
      case 'r' :  rc = yvikeys_mreg_paste  ("replace");   break;
      case 'd' :  rc = yvikeys_mreg_paste  ("duplicate"); break;
      case 'm' :  rc = yvikeys_mreg_paste  ("move");      break;
      case 'f' :  rc = yvikeys_mreg_paste  ("force");     break;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(delete)-------------------------*/
   if (a_major == 'd') {
      rc = yvikeys__combo_delete  (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(clear)--------------------------*/
   if (a_major == 'x') {
      rc = yvikeys__combo_clear   (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(append)-------------------------*/
   if (a_major == 'a') {
      rc = yvikeys__combo_append  (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(scroll family)------------------*/
   /*> if (a_major == 'z') {                                                          <* 
    *>    rc = KEYS_gz_family  (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(end family)---------------------*/
   /*> if (a_major == 'c') {                                                          <* 
    *>    rc = KEYS_c_family   (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> process keys for wander mode -------[ ------ [ge.FE0.223.65]*/ /*-[05.0000.102.!]-*/ /*-[--.---.---.--]-*/
WANDER_smode            (int a_major, int a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should allow keyboard selection of cells and ranges for formulas
    *   much like excel does
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_grid      =    0;
   char        t           [5];
   char    post = ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_WANDER )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(space)--------------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for control keys)---------*/
   /*> switch (a_minor) {                                                             <* 
    *> case  ',' :                                                                    <* 
    *> case  ')' : post = a_minor;                                                    <* 
    *> case  G_KEY_RETURN  :                                                          <* 
    *> case  G_KEY_ESCAPE  : yvikeys_visu_clear ();                                           <* 
    *>                       LOC_ref (CTAB, CCOL, CROW, 0, wref);                     <* 
    *>                       CTAB = wtab;                                             <* 
    *>                       CCOL = wcol;                                             <* 
    *>                       CROW = wrow;                                             <* 
    *>                       my.cpos = wpos;                                          <* 
    *>                       strcpy (g_contents, wsave);                              <* 
    *>                       if (strcmp (wref2, "") != 0) {                           <* 
    *>                          strcat (g_contents, wref2);                           <* 
    *>                          strcat (g_contents, ":");                             <* 
    *>                       }                                                        <* 
    *>                       strcat (g_contents, wref);                               <* 
    *>                       my.npos = strlen(g_contents);                            <* 
    *>                       if (post != ' ') {                                       <* 
    *>                          g_contents[my.npos]   = post;                         <* 
    *>                          g_contents[++my.npos] = '\0';                         <* 
    *>                       }                                                        <* 
    *>                       my.cpos = my.npos;                                       <* 
    *>                       yVIKEYS_mode_exit ();                                    <* 
    *>                       return  0;   /+ escape -- back to source mode +/         <* 
    *> }                                                                              <*/
   /*---(basic movement)-----------*/
   /*> switch (a_minor) {                                                              <* 
    *> case '_'      : KEYS_row(" _");    break;                                      <* 
    *> case 'K'      : KEYS_row(" K");    break;                                      <* 
    *> case 'k'      : KEYS_row(" k");    break;                                      <* 
    *> case 'j'      : KEYS_row(" j");    break;                                      <* 
    *> case 'J'      : KEYS_row(" J");    break;                                      <* 
    *> case 'G'      : KEYS_row(" G");    break;                                      <* 
    *> case '{'      : KEYS_row(" {");    break;                                      <* 
    *> case '}'      : KEYS_row(" }");    break;                                      <* 
    *> case K_CTRL_B : KEYS_row("^b"); clear(); break;                                <* 
    *> case K_CTRL_F : KEYS_row("^f"); clear(); break;                                <* 
    *> case '0'      : KEYS_col (" 0");     break;                                    <* 
    *> case 'H'      : KEYS_col (" H");     break;                                    <* 
    *> case 'h'      : KEYS_col (" h");     break;                                    <* 
    *> case 'l'      : KEYS_col (" l");     break;                                    <* 
    *> case 'L'      : KEYS_col (" L");     break;                                    <* 
    *> case '$'      : KEYS_col (" $");     break;                                    <* 
    *> case 'b'      : KEYS_col ("es");     break;                                    <* 
    *> case 'e'      : KEYS_col ("ee");     break;                                    <* 
    *> case 'c'      : VISU_col();          break;                                    <* 
    *> case 'r'      : VISU_row();          break;                                    <* 
    *> }                                                                              <*/
   /*> if (a_minor == ':') {                                                          <* 
    *>    LOC_ref    (CTAB, CCOL, CROW, 0, wref2);                                    <* 
    *>    VISU_start (CTAB, CCOL, CROW, VISU_FROM);                                   <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return  0;
}

char       /*----: give current position info --------------------------------*/
yvikeys_map_xstatus     (char *a_list)
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
yvikeys_map_ystatus     (char *a_list)
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
yvikeys_bufs_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_BUFFER)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear callbacks)----------------*/
   DEBUG_PROG   yLOG_note    ("clear callbacks");
   s_switcher = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_BUFFER);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_bufs_config     (void *a_switcher)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_BUFFER)) {
      DEBUG_PROG   yLOG_note    ("init must complete before config");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   s_switcher   = a_switcher;
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_BUFFER, '1');
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for buffer movement ---[ leaf   [ge.F64.217.CA]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_bufs_umode  (uchar a_major, uchar a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate our RBUF capability in vimm
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(request buffer mode)------------*/
   DEBUG_USER   yLOG_value   ("SMOD_BUF"  , MODE_not (SMOD_BUFFER));
   if (a_major != ' ' && a_minor == ',') {
      DEBUG_USER   yLOG_note    ("enter buffer mode");
      if (MODE_not (SMOD_BUFFER))  MODE_enter  (SMOD_BUFFER  );
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_minor;
   }
   /*---(defenses)-----------------------*/
   if (a_major == ',' && MODE_not (SMOD_BUFFER)) {
      DEBUG_USER   yLOG_note    ("force enter buffer mode");
      MODE_enter  (SMOD_BUFFER  );
   }
   --rce;  if (a_major != ',') {
      DEBUG_USER   yLOG_note    ("a_major is wrong)");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (MODE_not (SMOD_BUFFER)) {
      DEBUG_USER   yLOG_note    ("not in buffer mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape, choose nothing");
      /*> my.menu = ' ';                                                              <*/
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for control keys)---------*/
   --rce;
   if (strchr ("[<>]", a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("relative mode");
      rc = s_switcher (a_minor);
      MODE_exit  ();
   } else if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ��", a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("absolute mode");
      rc = s_switcher (a_minor);
      MODE_exit  ();
   } else {
      MODE_exit  ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          unit testing                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

static struct {
   char        label       [LEN_LABEL];
   int         b, x, y, z;
} s_map_unit_deref [100] = {
   { "0a1"        ,   0,   0,   0,   0 },
   { "0b3"        ,   0,   1,   2,   0 },
   { "0c5"        ,   0,   2,   4,   0 },
   { "0d6"        ,   0,   3,   5,   0 },
   { "0f1"        ,   0,   5,   0,   0 },
   { "0f11"       ,   0,   5,  10,   0 },
   { "0f101"      ,   0,   5, 100,   0 },
   { "0g11"       ,   0,   6,  10,   0 },
   { "0h11"       ,   0,   7,  10,   0 },
   { "0i11"       ,   0,   8,  10,   0 },
   { "0k11"       ,   0,  10,  10,   0 },
   { "0p11"       ,   0,  15,  10,   0 },
   { "0p12"       ,   0,  15,  11,   0 },
   { "2b5"        ,   2,   1,   4,   0 },
   { "3d6"        ,   3,   3,   5,   0 },
   { "3p11"       ,   3,  15,  10,   0 },
   { "5b2"        ,   5,   1,   1,   0 },
   { "6d3"        ,   6,   3,   2,   0 },
   { "6g11"       ,   6,   6,  10,   0 },
   { "Ag11"       ,  10,   6,  10,   0 },
   { "Mp11"       ,  22,  15,  10,   0 },
   { "Mp12"       ,  22,  15,  11,   0 },
   { "Za1"        ,  35,   0,   0,   0 },
   { "3:11"       ,   0,   4,  12,   0 },
   { "0c2"        ,   0,   2,   1,   0 },
   { "0e4"        ,   0,   4,   3,   0 },
   { NULL         ,   0,   0,   0,   0 },

};

char
yvikeys__unit_map_ymap  (void)
{
   /*> printf ("running rowmap\n");                                                   <*/
   yvikeys__map_load ('1', &g_ymap, YVIKEYS_YMAP);
   return 0;
}

char
yvikeys__unit_map_xmap  (void)
{
   /*> printf ("running colmap\n");                                                   <*/
   yvikeys__map_load ('w', &g_xmap, YVIKEYS_YMAP);
   return 0;
}

char
yvikeys__unit_map_zmap  (void)
{
   return 0;
}

char
yvikeys__unit_map_map   (char a_type)
{
   char        rc          = 0;
   char        t           [LEN_LABEL]  = "";
   rc = yvikeys__unit_map_addr (t, g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
   yVIKEYS_source (t, "testing");
   return 0;
}

char
yvikeys__unit_map_loc   (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   if (a_z != NULL)  *a_z = 0;
   return str2gyges (a_label, a_buf, a_x, a_y, NULL, NULL, 0, YSTR_CHECK);
}

char 
yvikeys__unit_map_addr  (char *a_label, int a_buf, int a_x, int a_y, int a_z)
{
   return str4gyges (a_buf, a_x, a_y, 0, 0, a_label, YSTR_CHECK);
}

char
yvikeys__unit_quick     (void)
{
   yvikeys__map_load ('1', &g_bmap, YVIKEYS_BMAP);
   yvikeys__map_load ('1', &g_xmap, YVIKEYS_XMAP);
   yvikeys__map_load ('1', &g_ymap, YVIKEYS_YMAP);
   yvikeys__map_load ('1', &g_zmap, YVIKEYS_ZMAP);
   yVIKEYS_map_config (YVIKEYS_OFFICE, yvikeys__unit_map_map, yvikeys__unit_map_loc, yvikeys__unit_map_addr);
   return 0;
}

char*
yVIKEYS__unit           (char *a_question, int a_num)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "yVIKEYS unit     : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "map_curr"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "yVIKEYS map curr : %3dx, %3dy, %3dz", g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__unit_map       (char *a_question, char a_index)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP unit         : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "horz"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP horz pos     : index %3d, grid %3d", g_xmap.cur, g_xmap.gcur);
   }
   else if (strcmp (a_question, "horz_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP horz units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_xmap.avail, g_xmap.beg, g_xmap.cur, g_xmap.end, g_xmap.tend, g_xmap.len);
   }
   else if (strcmp (a_question, "horz_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP horz grids   :        b %3d, c %3d, e %3d", g_xmap.gbeg, g_xmap.gcur, g_xmap.gend);
   }
   else if (strcmp (a_question, "vert_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP vert units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_ymap.avail, g_ymap.beg, g_ymap.cur, g_ymap.end, g_ymap.tend, g_ymap.len);
   }
   else if (strcmp (a_question, "vert_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP vert grids   :        b %3d, c %3d, e %3d", g_ymap.gbeg, g_ymap.gcur, g_ymap.gend);
   }
   else if (strcmp (a_question, "current"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "MAP current      : %3db, %3dx, %3dy, %3dz", g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


