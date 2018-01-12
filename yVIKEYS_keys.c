/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"




char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_line  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "0LlhH$_KkjJG";
   double      x_base      = 0.0;
   /*---(defense)------------------------*/
   DEBUG_YVIKEYS yLOG_point   ("a_base"    , a_base);
   --rce;  if (a_base  == NULL) {
      DEBUG_YVIKEYS yLOG_note    ("base value pointer not set");
      DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YVIKEYS yLOG_double  ("a_base"    , *a_base);
   DEBUG_YVIKEYS yLOG_double  ("a_inc"     , a_inc);
   DEBUG_YVIKEYS yLOG_double  ("a_min"     , a_min);
   DEBUG_YVIKEYS yLOG_double  ("a_max"     , a_max);
   /*---(keys)---------------------------*/
   switch (a_minor) {
   case '0': case '_':
      *a_base  =  a_min;
      break;
   case 'H': case 'K':
      *a_base -=  a_inc * 5;
      break;
   case 'h': case 'k': case '-':
      *a_base -=  a_inc;
      break;
   case 'l': case 'j': case '+':
      *a_base +=  a_inc;
      break;
   case 'L': case 'J':
      *a_base +=  a_inc * 5;
      break;
   case '$': case 'G':
      *a_base  =  a_max;
      break;
   }
   /*---(check limits)-------------------*/
   if (*a_base < a_min)  *a_base = a_min;
   if (*a_base > a_max)  *a_base = a_max;
   /*---(complete)-----------------------*/
   DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_horz  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "0LlhH$";
   /*---(header)-------------------------*/
   DEBUG_YVIKEYS yLOG_enter   (__FUNCTION__);
   DEBUG_YVIKEYS yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_YVIKEYS yLOG_info    ("x_minors"  , x_minors);
   if (strchr (x_minors, a_minor) == 0) {
      DEBUG_YVIKEYS yLOG_note    ("not a valid a_minor movement key");
      DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   rce = yVIKEYS_keys_line (a_minor, a_base, a_inc, a_min, a_max);
   /*---(complete)-----------------------*/
   DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
   return rce;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_vert  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "_KkjJG";
   /*---(header)-------------------------*/
   DEBUG_YVIKEYS yLOG_enter   (__FUNCTION__);
   DEBUG_YVIKEYS yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_YVIKEYS yLOG_info    ("x_minors"  , x_minors);
   if (strchr (x_minors, a_minor) == 0) {
      DEBUG_YVIKEYS yLOG_note    ("not a valid a_minor movement key");
      DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   rce = yVIKEYS_keys_line (a_minor, a_base, a_inc, a_min, a_max);
   /*---(complete)-----------------------*/
   DEBUG_YVIKEYS yLOG_exit    (__FUNCTION__);
   return rce;
}



/*============================----end-of-source---============================*/
