/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 *
 *   standard screen grid
 *      normal  1  -  5  -  9       9 for normal use
 *      fine    Z - A * a - z      53 for bigger screens
 *
 *         1      2     3      4     5     6      7     8      9
 *         ZYXWVUTSRQPONMLKJIHGFEDCBA-abcdefghijklmnopqrstuvwxyz
 *       1z             ·            ·            ·             z1
 *        ²   ¬¬¬          ¬¬¬¬¬     ·                          ² 
 *       2s             ·  ¬¬¬¬¬     ·            ·             s2
 *        ²                          ·                          ² 
 *       3m ·  ·  ·  ·  ·  ·  ·  ·  ···  ·  ·  ·  ·  ·  ·  ·  · m3
 *        ²                      ¬¬¬¬¬¬¬¬                       ² 
 *       4f             ·        ¬¬¬¬¬¬¬¬         ·             f4
 *        ²                          ·                          ² 
 *       5*·····················································*5
 *        ²                          ·                          ² 
 *       6F             ·            ·            ·             f6
 *        ²                          ·                          ² 
 *       7M ·  ·  ·  ·  ·  ·  ·  ·  ···  ·  ·  ·  ·  ·  ·  ·  · m7
 *        ²                          ·                          ² 
 *       8S             ·            ·            ·             s8
 *        ²                          ·                          ² 
 *       9Z             ·            ·            ·             Z9
 *         ZYXWVUTSRQPONMLKJIHGFEDCBA-abcdefghijklmnopqrstuvwxyz
 *         1      2     3      4     5     6      7     8      9
 *
 *
 *   targeting
 *     ´ grid        1 - 9 or Z-A*a-z
 *     Ì radial      deg/dist from center
 *     ¡ current     current xy in yVIKEYS 
 *
 *
 */


#define      MAX_NOTES     20
static struct {
   char        xr, yr, size;
   short       x, y;
   uchar       w, h;
   char       *text;
   char        type;
   short       xt, yt;
   short       xb, yb;
   short       xe, ye;
} s_notes [MAX_NOTES];
static char s_nnote = 0;


static char  *s_size       = "-=+).";
static char  *s_norm       = "123456789";
static char  *s_fine       = "ZYXWVUTSRQPONMLKJIHGFEDCBA*abcdefghijklmnopqrstuvwxyz";
static char  *s_targ       = "´ ¡";
static char   s_noting     = 'y';



/*====================------------------------------------====================*/
/*===----                       little supporters                      ----===*/
/*====================------------------------------------====================*/
static void  o___SUPPORT_________o () { return; }

char
yvikeys_note__clear     (char a_init, char n)
{
   /*---(master)-------------------------*/
   if (a_init != 'y' && s_notes [n].xr != '0' && s_notes [n].text != NULL) free (s_notes [n].text);
   s_notes [n].text = NULL;
   s_notes [n].xr   = '0';
   s_notes [n].yr   = '0';
   s_notes [n].size = '=';
   /*---(position)-----------------------*/
   s_notes [n].x    = 0;
   s_notes [n].y    = 0;
   s_notes [n].w    = 0;
   s_notes [n].h    = 0;
   /*---(target)-------------------------*/
   s_notes [n].type = '-';
   s_notes [n].xt   = 0;
   s_notes [n].yt   = 0;
   s_notes [n].xb   = 0;
   s_notes [n].yb   = 0;
   s_notes [n].xe   = 0;
   s_notes [n].ye   = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_note__find      (char xr, char yr)
{
   int         i           =    0;
   for (i = 0; i < s_nnote; ++i) {
      if (s_notes [i].xr != xr)  continue;
      if (s_notes [i].yr != yr)  continue;
      return i;
   }
   return -1;
}

char yvikeys_note__showing   (void) { if (s_noting == 'y') return 1;  return 0; }



/*====================------------------------------------====================*/
/*===----                      changing order                          ----===*/
/*====================------------------------------------====================*/
static void  o___ORDERING________o () { return; }

char
yvikeys_note__remove    (char n)
{
   char        rce         =  -10;
   int         i           =    0;
   --rce;  if (n < 0 || n > MAX_NOTES)  return rce;
   --rce;  if (n >= s_nnote)            return rce;
   if (s_notes [n].text != NULL)  free (s_notes [n].text);
   for (i = n; i < s_nnote; ++i) {
      s_notes [i].xr   = s_notes [i + 1].xr;
      s_notes [i].yr   = s_notes [i + 1].yr;
      s_notes [i].size = s_notes [i + 1].size;
      s_notes [i].x    = s_notes [i + 1].x;
      s_notes [i].y    = s_notes [i + 1].y;
      s_notes [i].w    = s_notes [i + 1].w;
      s_notes [i].h    = s_notes [i + 1].h;
      s_notes [i].text = s_notes [i + 1].text;
      s_notes [i].xt   = s_notes [i + 1].xt;
      s_notes [i].yt   = s_notes [i + 1].yt;
      s_notes [i].xb   = s_notes [i + 1].xb;
      s_notes [i].yb   = s_notes [i + 1].yb;
      s_notes [i].xe   = s_notes [i + 1].xe;
      s_notes [i].ye   = s_notes [i + 1].ye;
      if (i + 1 < MAX_NOTES)  yvikeys_note__clear ('y', i + 1);
   }
   --s_nnote;
   return 0;
}

char
yvikeys_note__totop     (char n)
{
   char        rce         =  -10;
   int         i           =    0;
   --rce;  if (n < 0 || n > MAX_NOTES)  return rce;
   --rce;  if (n >= s_nnote)            return rce;
   s_notes [s_nnote].xr   = s_notes [n].xr;
   s_notes [s_nnote].yr   = s_notes [n].yr;
   s_notes [s_nnote].size = s_notes [n].size;
   s_notes [s_nnote].x    = s_notes [n].x;
   s_notes [s_nnote].y    = s_notes [n].y;
   s_notes [s_nnote].w    = s_notes [n].w;
   s_notes [s_nnote].h    = s_notes [n].h;
   s_notes [s_nnote].text = s_notes [n].text;
   s_notes [s_nnote].xt   = s_notes [n].xt;
   s_notes [s_nnote].yt   = s_notes [n].yt;
   s_notes [s_nnote].xb   = s_notes [n].xb;
   s_notes [s_nnote].yb   = s_notes [n].yb;
   s_notes [s_nnote].xe   = s_notes [n].xe;
   s_notes [s_nnote].ye   = s_notes [n].ye;
   s_notes [n].text = NULL;
   ++s_nnote;
   yvikeys_note__remove (n);
   return 0;
}

char
yvikeys_note__move      (char n, char xr, char yr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        t           [LEN_LABEL] = "";
   char        s, x, y;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n < 0 || n > MAX_NOTES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (n >= s_nnote) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s = s_notes [n].size;
   x = s_notes [n].xr;
   y = s_notes [n].yr;
   rc = yvikeys_note__size   (myVIKEYS.env, n, xr, yr, s);
   if (rc < 0)  yvikeys_note__size   (myVIKEYS.env, n, x, y, s);
   yvikeys_note__retarg (n);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        mass updates                          ----===*/
/*====================------------------------------------====================*/
static void  o___MASS____________o () { return; }

char
yvikeys_note__purge     (char a_init)
{
   int         i           =    0;
   for (i = 0; i < MAX_NOTES; ++i) {
      yvikeys_note__clear (a_init, i);
   }
   s_nnote = 0;
   return 0;
}

char
yvikeys_note_resize     (void)
{
   int         i           =    0;
   for (i = 0; i < s_nnote; ++i) {
      yvikeys_note__size (myVIKEYS.env, i, s_notes [i].xr, s_notes [i].yr, s_notes [i].size);
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     adding/changing content                  ----===*/
/*====================------------------------------------====================*/
static void  o___CONTENT_________o () { return; }

char
yvikeys_note__size      (char a_type, char n, char xr, char yr, char a_size)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_left, x_wide, x_bott, x_tall;
   char       *p           = NULL;
   int         l           =    0;
   int         c           =    0;
   float       a, b;
   int         x, y;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_char    ("a_type"    , a_type);
   /*---(defaults)-----------------------*/
   s_notes [n].size = '=';
   s_notes [n].xr   = '0';
   s_notes [n].x    = 0;
   s_notes [n].yr   = '0';
   s_notes [n].y    = 0;
   /*---(defense)------------------------*/
   --rce;  if (strchr (YVIKEYS_ENVS, a_type) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   --rce;  if (n < 0 || n >= MAX_NOTES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(size)---------------------------*/
   DEBUG_GRAF   yLOG_char    ("a_size"    , a_size);
   --rce;  if (strchr (s_size, a_size) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   switch (a_type) {
   case YVIKEYS_OPENGL :
      yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
      switch (a_size) {
      case '.'  : s_notes [n].w =  30; s_notes [n].h =  30;  break;
      case '-'  : s_notes [n].w = 100; s_notes [n].h =  30;  break;
      case ')'  :
      case '='  : s_notes [n].w = 175; s_notes [n].h =  60;  break;
      case '+'  : s_notes [n].w = 250; s_notes [n].h =  80;  break;
      }
      break;
   case YVIKEYS_CURSES :
      yVIKEYS_view_size   (YVIKEYS_MAIN, &x_left, &x_wide, &x_bott, &x_tall, NULL);
      switch (a_size) {
      case '.'  : s_notes [n].w =   3; s_notes [n].h =   1;  break;
      case '-'  : s_notes [n].w =  10; s_notes [n].h =   1;  break;
      case ')'  :
      case '='  : s_notes [n].w =  25; s_notes [n].h =   3;  break;
      case '+'  : s_notes [n].w =  40; s_notes [n].h =   4;  break;
      }
      break;
   }
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   DEBUG_GRAF   yLOG_complex  ("size"      , "%4dw, %4dh", s_notes [n].w, s_notes [n].h);
   /*---(vertical)-----------------------*/
   DEBUG_GRAF   yLOG_value   ("yr"        , yr);
   --rce;  if ((p = strchr (s_norm, yr)) != NULL) {
      l  = strlen (s_norm) - 1;
      c  = p - s_norm;
      a = 1 - (c * (1.0 / l));
   } else if  ((p = strchr (s_fine, yr)) != NULL) {
      l  = strlen (s_fine) - 1;
      c  = p - s_fine;
      a = c * (1.0 / l);
   } else {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   b = 1 - a;
   if (a_type == YVIKEYS_OPENGL)  {
      y  = x_bott + (x_tall * a) + (s_notes [n].h * b);
   } else {
      y  = x_bott - (x_tall * a) - (s_notes [n].h * b) + 1;
   }
   DEBUG_GRAF   yLOG_complex  ("vert"      , "%2d, %2dc, %4.0fa, %4.0fb, %4dy", l, c, a, b, s_notes [n].y);
   /*---(horizontal)---------------------*/
   DEBUG_GRAF   yLOG_value   ("xr"        , xr);
   --rce;  if ((p = strchr (s_norm, xr)) != NULL) {
      l  = strlen (s_norm) - 1;
      c  = p - s_norm;
      a = c * (1.0 / l);
   } else if  ((p = strchr (s_fine, xr)) != NULL) {
      l  = strlen (s_fine) - 1;
      c  = p - s_fine;
      /*> a  = 1 - (c * (1.0 / l));                                                   <*/
      a  = c * (1.0 / l);
   } else {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x  = x_left + (x_wide * a) - (s_notes [n].w * a);
   DEBUG_GRAF   yLOG_complex  ("horz"      , "%2d, %2dc, %4.0fa, %4dx", l, c, a, s_notes [n].y);
   /*---(save values)--------------------*/
   s_notes [n].size = a_size;
   s_notes [n].xr   = xr;
   s_notes [n].x    = x;
   s_notes [n].yr   = yr;
   s_notes [n].y    = y;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__append    (char n, char xr, char yr, char a_size, char *a_text)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   if (n <  0)  n = s_nnote;
   --rce;  if (n < 0 || n > s_nnote) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if ((a_text != NULL && a_text [0] != '\0') && s_notes [n].text != NULL)  free (s_notes [n].text);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   /*---(size and position)--------------*/
   rc = yvikeys_note__size   (myVIKEYS.env, n, xr, yr, a_size);
   DEBUG_GRAF   yLOG_value   ("size"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(add text)-----------------------*/
   if (a_text != NULL && a_text [0] != '\0')  s_notes [n].text = strdup (a_text);
   if (n == s_nnote) ++s_nnote;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__grid      (char a_type, char n, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_inc , x_bump, y_inc , y_bump;
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_char    ("xt"        , xt);
   --rce;  if (xt == 0 || strchr (x_valid, xt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yt"        , yt);
   --rce;  if (xt == 0 || strchr (x_valid, yt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(default)------------------------*/
   if (xt == '-' || yt == '-' || a_type == YVIKEYS_CURSES) {
      s_notes [n].xb = s_notes [n].yb = 0;
      s_notes [n].xe = s_notes [n].ye = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(total size)---------------------*/
   yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   /*---(horizontal)---------------------*/
   s_notes [n].xb = s_notes [n].x + s_notes [n].w * 0.50;
   x_inc  = x_wide / 25;
   x_bump = x_inc * 0.50;
   s_notes [n].xe = x_left + (tolower (xt) - 'a') * x_inc;
   if (xt != tolower (xt))    s_notes [n].xe += x_bump;
   /*---(vertical)-----------------------*/
   s_notes [n].yb = s_notes [n].y - s_notes [n].h * 0.50;
   y_inc  = x_tall / 25;
   x_bump = y_inc * 0.50;
   s_notes [n].ye = x_bott + x_tall - (tolower (yt) - 'a') * y_inc;
   if (yt != tolower (yt))    s_notes [n].ye -= x_bump;
   /*---(complete)-----------------------*/
   s_notes [n].type = '´';
   /*---(report-out)---------------------*/
   DEBUG_GRAF   yLOG_complex  ("endpoint"  , "%4dx, %4dy, %4dx, %4dy", s_notes [n].xb, s_notes [n].yb, s_notes [n].xe, s_notes [n].ye);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__radial    (char a_type, char n, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_left, x_wide, x_bott, x_tall;
   int         x_inc , x_bump, y_inc , y_bump;
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyz";
   float       x_rad       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("xt"        , xt);
   --rce;  if (xt <= 0 || xt > 360) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yt"        , yt);
   --rce;  if (xt == 0 || strchr (x_valid, yt) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(default)------------------------*/
   if (xt == 0 || yt == '-' || a_type == YVIKEYS_CURSES) {
      s_notes [n].xb = s_notes [n].yb = 0;
      s_notes [n].xe = s_notes [n].ye = 0;
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(total size)---------------------*/
   yVIKEYS_view_bounds (YVIKEYS_MAIN, &x_left, NULL, &x_wide, &x_bott, NULL, &x_tall);
   DEBUG_GRAF   yLOG_complex  ("main"      , "%4dl, %4dw, %4db, %4dt", x_left, x_wide, x_bott, x_tall);
   /*---(vertical)-----------------------*/
   s_notes [n].yb = s_notes [n].y - s_notes [n].h * 0.50;
   x_tall /= 2.0;
   y_inc  = x_tall / 25;
   y_bump = y_inc * 0.25;
   x_rad  = (yt - 'a') * y_inc - y_bump;
   /*---(horizontal)---------------------*/
   s_notes [n].xb = s_notes [n].x + s_notes [n].w * 0.50;
   s_notes [n].xe = x_rad * cos ((90 - xt) * DEG2RAD);
   s_notes [n].ye = x_rad * sin ((90 - xt) * DEG2RAD);
   /*---(complete)-----------------------*/
   s_notes [n].type = ' ';
   /*---(report-out)---------------------*/
   DEBUG_GRAF   yLOG_complex  ("endpoint"  , "%4dx, %4dy, %4dx, %4dy", s_notes [n].xb, s_notes [n].yb, s_notes [n].xe, s_notes [n].ye);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__append_OLD(char xr, char yr, char a_size, char *a_text, char a_type, short xt, short yt)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        n           =    0;
   int         x_len       =    0;
   char        t           [LEN_TERSE] = "";
   char        u           [LEN_RECD]  = "";
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_value   ("s_nnote"   , s_nnote);
   --rce;  if (s_nnote >= MAX_NOTES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("xr"        , xr);
   --rce;  if (xr == 0 || strchr ("123456789", xr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("yr"        , yr);
   --rce;  if (yr == 0 || strchr ("123456789", yr) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_char    ("a_size"    , a_size);
   --rce;  if (a_size == 0 || strchr ("-=+", a_size) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("a_text"    , a_text);
   --rce;  if (a_text == NULL || a_text [0] == '\0') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_text"    , a_text);
   DEBUG_GRAF   yLOG_value   ("xt"        , xt);
   DEBUG_GRAF   yLOG_value   ("yt"        , yt);
   s_notes [s_nnote].xr   = xr;
   s_notes [s_nnote].yr   = yr;
   s_notes [s_nnote].size = a_size;
   s_notes [s_nnote].text = strdup (a_text);
   yvikeys_note__size (myVIKEYS.env, s_nnote, xr, yr, a_size);
   s_notes [s_nnote].xt   = xt;
   s_notes [s_nnote].yt   = yt;
   switch (a_type) {
   case '´' : yvikeys_note__grid   (myVIKEYS.env, s_nnote, xt, yt); break;
   case ' ' : yvikeys_note__radial (myVIKEYS.env, s_nnote, xt, yt); break;
              /*> case '¡' : yvikeys_note__curr   (myVIKEYS.env, s_nnote, xt, yt); break;        <*/
   }
   ++s_nnote;
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__settarg   (char n, char *p)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char        u           [LEN_LABEL] = "";
   uchar      *x_valid     = "-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char        x_type      =  '-';
   short       xt          =  '-';
   short       yt          =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (myVIKEYS.env == YVIKEYS_CURSES) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   strlcpy (u, p, LEN_LABEL);
   x_len = strlen (u);
   DEBUG_GRAF   yLOG_complex ("tail"      , "%2d[%s]", x_len, u);
   rc = -1;
   x_type = u [0];;
   switch (x_type) {
   case '´' :  /* 25 x 25 grid point (a-y)*/
      if (x_len != 3) break;
      DEBUG_GRAF   yLOG_note    ("25 x 25 grid type");
      xt     = u [1];
      yt     = u [2];
      if (strchr (x_valid, xt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("xt fail"   , xt);
         break;
      }
      if (strchr (x_valid, yt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("yt fail"   , yt);
         break;
      }
      rc     = 0;
      DEBUG_GRAF   yLOG_complex ("ready"     , "%c %c %c", x_type, xt, yt);
      yvikeys_note__grid   (myVIKEYS.env, n, xt, yt);
      break;
   case ' ' :  /* radial point */
      DEBUG_GRAF   yLOG_note    ("radial point");
      if (x_len < 3 || x_len > 5)           break;
      yt     = u [x_len];
      u [x_len]  = '\0';
      if (strchr (x_valid, yt) == NULL) {
         DEBUG_GRAF   yLOG_char    ("yt fail"   , yt);
         break;
      }
      xt = atoi (u + 1);
      if (xt <= 0) {
         DEBUG_GRAF   yLOG_value   ("xt fail"   , xt);
         break;
      }
      rc     = 0;
      DEBUG_GRAF   yLOG_complex ("ready"     , "%c %4d %c", x_type, xt, yt);
      break;
   case '¡' :
      DEBUG_GRAF   yLOG_note    ("current point");
      break;
   default  :
      break;
   }
   /*---(update)-------------------------*/
   s_notes [n].type = x_type;
   s_notes [n].xt   = xt;
   s_notes [n].yt   = yt;
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note__retarg    (char n)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_LABEL] = "";
   switch (s_notes [n].type) {
   case '´' :
      sprintf (t, "%c%c%c", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);
      break;
   case ' ' :
      sprintf (t, "%c%d%c", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);
      break;
   }
   yvikeys_note__settarg (n, t);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        main driver                           ----===*/
/*====================------------------------------------====================*/
static void  o___DRIVER__________o () { return; }

char
yvikeys_note            (char *a_all)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =    0;
   int         x_len       =    0;
   char        u           [LEN_RECD]  = "";
   char       *v           = NULL;
   char       *p           = NULL;
   char        x_type      =  '-';
   char        xr          =  '-';
   char        yr          =  '-';
   char        x_size      =  '=';
   short       xt          =  '-';
   short       yt          =  '-';
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_GRAF   yLOG_point   ("a_all"     , a_all);
   --rce;  if (a_all == NULL || a_all [0] == '\0') {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_GRAF   yLOG_info    ("a_all"     , a_all);
   x_len = strlen (a_all);
   DEBUG_GRAF   yLOG_value   ("x_len"     , x_len);
   /*---(singles)------------------------*/
   --rce;  if (x_len == 1) {
      DEBUG_GRAF   yLOG_char    ("single"    , a_all [0]);
      switch (a_all [0])  {
      case '#'  :
         DEBUG_GRAF   yLOG_note    ("selected a full purge");
         rc = yvikeys_note__purge ('-');
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return rc;
         break;
      case '+'  :
         DEBUG_GRAF   yLOG_note    ("display all notes");
         s_noting = 'y';
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      case '-'  :
         DEBUG_GRAF   yLOG_note    ("hide all notes");
         s_noting = '-';
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
      default   :
         DEBUG_GRAF   yLOG_note    ("unknown single char option");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      }
   }
   /*---(setup)--------------------------*/
   xr = a_all [0];
   yr = a_all [1];
   n  = yvikeys_note__find (xr, yr);
   DEBUG_GRAF   yLOG_complex ("note"      , "%c, %c, %2d", xr, yr, n);
   /*---(doubles)------------------------*/
   --rce;  if (x_len == 2) {
      if (n <  0) {
         DEBUG_GRAF   yLOG_note    ("note not found for on-top");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = yvikeys_note__totop (n);
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(triples)------------------------*/
   --rce;  if (x_len == 3) {
      if (n <  0) {
         DEBUG_GRAF   yLOG_note    ("note not found for deletion");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      switch (a_all [2]) {
      case '#' :
         DEBUG_GRAF   yLOG_note    ("delete a note");
         rc = yvikeys_note__remove (n);
         DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
         return rc;
         break;
      default  :
         DEBUG_GRAF   yLOG_note    ("unknown triple char option");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      }
   }
   /*---(prepare)------------------------*/
   x_size = a_all [2];
   strlcpy (u, a_all + 3, LEN_RECD);
   v = a_all + 2;
   p = strrchr (u, ' ');
   DEBUG_GRAF   yLOG_point   ("p"         , p);
   if (p != NULL) {
      DEBUG_GRAF   yLOG_char    ("p[0]"      , p [1]);
      if (strchr (s_targ, p [1]) != NULL)  {
         v = p + 1;;
         p [0]  = '\0';   /* trim from text */
      } else v = NULL;
   } else v = NULL;
   DEBUG_GRAF   yLOG_point   ("v"         , v);
   /*---(triples)------------------------*/
   --rce;  if (x_len == 5 && a_all [2] == '>') {
      if (n <  0) {
         DEBUG_GRAF   yLOG_note    ("note not found for moving");
         DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      rc = yvikeys_note__move (n, u [0], u [1]);
      if (rc >= 0)  yvikeys_note__totop (n);
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(find note)----------------------*/
   DEBUG_GRAF   yLOG_char    ("x_size"    , x_size);
   --rce;  if (strchr (s_size, x_size) == NULL) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (n <  0)  n = s_nnote;
   DEBUG_GRAF   yLOG_value   ("n"         , n);
   /*---(append/change note)-------------*/
   rc = yvikeys_note__size (myVIKEYS.env, n, xr, yr, x_size);
   DEBUG_GRAF   yLOG_char    ("size"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (s_notes [n].text != NULL)  free (s_notes [n].text);
   s_notes [n].text = strdup (u);
   if (n == s_nnote)  ++s_nnote;
   rc = yvikeys_note__totop (n);
   /*---(targeting)----------------------*/
   if (v == NULL) {
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = yvikeys_note__settarg (n, v);
   DEBUG_GRAF   yLOG_char    ("settarg"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_GRAF   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        screen drawing                        ----===*/
/*====================------------------------------------====================*/
static void  o___DRAWING_________o () { return; }

char
yvikeys_note_target     (int xb, int yb, int xe, int ye, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_LABEL];
   int         x_align     =    0;
   int         x_cen, x_mid;
   int         x_len       =    0;
   int         x_edge      =    8;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(base line)----------------------*/
   glColor4f    (0.34f, 0.34f, 0.34f, 1.00f);
   glLineWidth   (4.0);
   glBegin       (GL_LINES); {
      glVertex3f   (xb, yb, z);
      glVertex3f   (xe, ye, z);
   } glEnd();
   /*---(dotted overwrite)---------------*/
   glColor4f    (0.67f, 0.67f, 0.67f, 1.00f);
   glLineWidth   (4.0);
   glEnable      (GL_LINE_STIPPLE);
   glLineStipple (1, 0xf0f0);
   glBegin      (GL_LINES); {
      glVertex3f   (xb, yb, z + 2);
      glVertex3f   (xe, ye, z + 2);
   } glEnd ();
   glDisable     (GL_LINE_STIPPLE);
   /*---(dotted overwrite)---------------*/
   /*> glColor4f    (1.00f, 1.00f, 1.00f, 1.00f);                                     <* 
    *> glLineWidth   (1.0);                                                           <* 
    *> glBegin      (GL_LINES); {                                                     <* 
    *>    glVertex3f   (xb, yb, z + 4);                                               <* 
    *>    glVertex3f   (xe, ye, z + 4);                                               <* 
    *> } glEnd ();                                                                    <*/
   /*---(reset)--------------------------*/
   glLineWidth   (1.0);
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note_text       (char a_type, char a_len, char a_lvl, char *a_text, int x, int y, int w, int h, int z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD];
   int         x_align     =    0;
   int         x_full, x_len;
   int         x_beg, x_next;
   int         x_edge      =    8;
   int         i, j;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   /*---(opengl)-------------------------*/
   if (a_type == YVIKEYS_OPENGL) {
      DEBUG_GRAF   yLOG_complex  ("opengl"    , "%4dx, %4dy, %4dw, %4dh", x, y, w, h);
      x_edge = 20;
      glPushMatrix(); {
         glTranslatef (x + w * 0.5, y - 18, z);
         glColor4f (0.0, 0.0, 0.0, 1.0);
         yFONT_printw (myVIKEYS.font, myVIKEYS.point, YF_TOPCEN, a_text, w - x_edge, h, 1.2);
      } glPopMatrix();
   }
   else {
      if (a_len <  0)           yCOLOR_curs ("m_main");
      else if (a_len != a_lvl)  yCOLOR_curs ("m_curr");
      else                      yCOLOR_curs ("m_prev");
      strlcpy (t, a_text, LEN_RECD);
      x_edge = 1;
      x_beg  = x_next  = 0;
      x_full = strlen (t);
      for (i = 0; i < h; ++i) {
         x_len = strlen (t + x_next);
         x_next = x_beg + w - 3;
         DEBUG_GRAF   yLOG_complex ("current"   , "%1d %2d %2d[%s]", i, x_beg, x_len, t + x_beg);
         for (j = x_next; j > x_beg; --j) {
            DEBUG_GRAF   yLOG_complex ("check"     , "%2d %c", j, t [j]);
            if (strchr ("- ", t [j]) == NULL)  continue;
            t [j] = '\0';
            x_next = j + 1;
            break;
         }
         x_len = strlen (t + x_beg);
         mvprintw (y + i, x + ((w - x_len) / 2), "%s", t + x_beg);
         x_beg = x_next;
         if (x_beg > x_full) break;
      }
   }
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_note_draw       (void)
{
   int         i           =    0;
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   if (yvikeys_note__showing () == 0) {
      DEBUG_GRAF   yLOG_note    ("note showing mode is OFF");
      DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_GRAF   yLOG_note    ("note showing mode is ON");
   for (i = 0; i < s_nnote; ++i) {
      DEBUG_GRAF   yLOG_complex  ("note"      , "%4dl, %4dw, %4db, %4dt, %c", s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, s_notes [i].size);
      if (myVIKEYS.env == YVIKEYS_OPENGL) {
         yvikeys_note_target   (s_notes [i].xb, s_notes [i].yb, s_notes [i].xe, s_notes [i].ye, 200 + (i * 10));
         yvikeys_menu_shadow   (myVIKEYS.env, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 202 + (i * 10));
         if (i == s_nnote - 1)  yvikeys_menu_fill     (myVIKEYS.env, 't', 1, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 204 + (i * 10));
         else                   yvikeys_menu_fill     (myVIKEYS.env, 'n', 0, s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h, 204 + (i * 10));
         yvikeys_note_text     (myVIKEYS.env, 0, 0, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
      } else {
         yvikeys_menu_shadow   (myVIKEYS.env, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 202 + (i * 10));
         if (s_notes [i].size == ')') {
            DEBUG_GRAF   yLOG_note    ("title type");
            yvikeys_menu_fill     (myVIKEYS.env, -1, -1, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 204 + (i * 10));
            yvikeys_note_text     (myVIKEYS.env, -1, -1, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
         } else if (i == s_nnote - 1) {
            DEBUG_GRAF   yLOG_note    ("current type");
            yvikeys_menu_fill     (myVIKEYS.env,  0,  1, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 204 + (i * 10));
            yvikeys_note_text     (myVIKEYS.env,  0,  1, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
         } else {
            DEBUG_GRAF   yLOG_note    ("previous type");
            yvikeys_menu_fill     (myVIKEYS.env,  0,  0, s_notes [i].x, s_notes [i].w, s_notes [i].y, s_notes [i].h, 204 + (i * 10));
            yvikeys_note_text     (myVIKEYS.env,  0,  0, s_notes [i].text, s_notes [i].x, s_notes [i].y, s_notes [i].w, s_notes [i].h, 206 + (i * 10));
         }
      }
   }
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> create a shape mask for notes ------------------------------*/
yVIKEYS_note_mask       (void *a_bounds, void *a_context, int a_left, int a_topp, int a_xmin, int a_ymax)
{
   /*---(locals)-----------+-----+-----+-*/
   Pixmap      *x_bounds;
   GC          *x_context;
   int         i           =    0;
   /*---(quick out)----------------------*/
   if (myVIKEYS.env == YVIKEYS_CURSES)   return 0;
   /*---(header)-------------------------*/
   DEBUG_GRAF   yLOG_enter   (__FUNCTION__);
   DEBUG_GRAF   yLOG_complex  ("args"      , "%p, %p, %3dl, %3dt, %3dx, %3dy", a_bounds, a_context, a_left, a_topp, a_xmin, a_ymax);
   /*---(cast)---------------------------*/
   x_bounds  = (Pixmap *) a_bounds;
   x_context = (GC *) a_context;
   /*---(walk notes)---------------------*/
   for (i = 0; i < s_nnote; ++i) {
      DEBUG_GRAF   yLOG_complex  ("note"      , "%4dl, %4dw, %4db, %4dt", s_notes [i].x, s_notes [i].w, s_notes [i].y - s_notes [i].h, s_notes [i].h);
      XFillRectangle (YX_DISP, *x_bounds, *x_context, a_left + (s_notes [i].x  - a_xmin), a_topp + a_ymax - s_notes [i].y , s_notes [i].w, s_notes [i].h);
      XDrawLine      (YX_DISP, *x_bounds, *x_context, a_left + (s_notes [i].xb - a_xmin), a_topp + a_ymax - s_notes [i].yb, a_left + (s_notes [i].xe - a_xmin), a_ymax - s_notes [i].ye);
   }
   /*---(complete)-----------------------*/
   DEBUG_GRAF   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          unit testing                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_note__unit      (char *a_question, int n)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         c           =    0;
   char        s           [LEN_LABEL];
   char        t           [LEN_HUND]  = "";
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "LAYER unit       : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "count"          ) == 0) {
      for (i = 0; i < MAX_NOTES; ++i) {
         if (s_notes [i].xr == '0')  strcat (t, "·");
         else                        strcat (t, "Ï");
      }
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "NOTE count       : %2d  å%sæ", s_nnote, t);
   }
   else if (strcmp (a_question, "entry"          ) == 0) {
      if (n < 0 || n > MAX_NOTES)     snprintf (yVIKEYS__unit_answer, LEN_FULL, "NOTE entry  (%2s) : - - -    -x    -y   -w  -h  -åæ                   -,   -  - -    -        -,   -", "--");
      else if (s_notes [n].xr == '0') snprintf (yVIKEYS__unit_answer, LEN_FULL, "NOTE entry  (%2d) : - - -    -x    -y   -w  -h  -åæ                   -,   -  - -    -        -,   -", n);
      else {
         if (s_notes [n].text != NULL)  sprintf  (t, "%2då%.15sæ", strlen (s_notes [n].text), s_notes [n].text);
         else                           strcpy   (t, " -åæ");
         switch (s_notes [n].type) {
         case '-' : sprintf (s, "%c -    -   "  , s_notes [n].type); break;
         case '´' : sprintf (s, "%c %c    %c   ", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         case 'Ì' : sprintf (s, "%c %-3d  %c   ", s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         case '¡' : sprintf (s, "%c %-4d,%-4d " , s_notes [n].type, s_notes [n].xt, s_notes [n].yt);   break;
         default  : sprintf (s, "BOOM BOOM  "); break;
         }
         snprintf (yVIKEYS__unit_answer, LEN_FULL, "NOTE entry  (%2d) : %c %c %c %4dx %4dy %3dw %2dh %-19.19s %4d,%4d  %s  %4d,%4d", n, s_notes [n].xr, s_notes [n].yr, s_notes [n].size, s_notes [n].x, s_notes [n].y, s_notes [n].w, s_notes [n].h, t, s_notes [n].xb, s_notes [n].yb, s, s_notes [n].xe, s_notes [n].ye);
      }
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

