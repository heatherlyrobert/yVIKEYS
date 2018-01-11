/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"




char
yVIKEYS_repeat_init     (void)
{
   its.repeat       = 0;
   its.repeat_macro = 0;
   return 0;
}

char         /*-> accumulate multiplier --------------[ ------ [ge.A43.214.63]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_repeat_umode    (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr ());
   --rce;  if (yVIKEYS_mode_not (SMOD_REPEAT )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ESCAPE) {
      yVIKEYS_mode_exit ();
      its.repeat = 0;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for major)-----------------------*/
   if (its.repeat ==  0 && strchr ("123456789", a_major) != NULL) {
      DEBUG_USER   yLOG_note    ("assign starting repeat");
      its.repeat  = a_major - '0';
   }
   /*---(check for minor)-----------------------*/
   if (strchr ("0123456789",  a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("increment repeat");
      its.repeat *= 10;
      its.repeat += a_minor - '0';
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(adjust)-------------------------*/
   --its.repeat;
   if (its.repeat <  0) its.repeat =  0;
   if (its.repeat > 99) its.repeat = 99;
   /*---(complete)-----------------------*/
   yVIKEYS_mode_exit ();
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return a_minor;
}


char yVIKEYS_repeat_norm     (void) { if (its.repeat < 1) return 0; else return 1; } 
char yVIKEYS_repeat_dec      (void) { --its.repeat; }
char yVIKEYS_repeat_macro    (void) { if (its.repeat_macro < 1) return 0; else return 1; }
int  yVIKEYS_repeat_value    (void) { return its.repeat; }

