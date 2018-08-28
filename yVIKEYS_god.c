/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



char         /*--> process keystrokes in normal mode -----[--------[--------]-*/
GOD_mode           (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   char        x_minors    [LEN_STR]  = "ypdx";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_GOD     )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE || a_minor == G_KEY_RETURN || a_minor == G_KEY_ENTER ) {
      DEBUG_USER   yLOG_note    ("escape/return, nothing to do");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      /*---(repeat)----------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("repeating");
         MODE_enter  (UMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      switch (a_minor) {
      /* crab  /horz  */  case 'h': case 'l': case 'H': case 'L':
      /* boom  /vert  */  case 'j': case 'k': case 'J': case 'K':
      /* dolly /zoom  */  case 'i': case 'I': case 'o': case 'O':
      /* x-axis/pitch */  case 'p': case 'P': case 'v': case 'V':
      /* y-axis/yaw   */  case 'y': case 'Y': case 't': case 'T':
      /* z-axis/roll  */  case 'r': case 'R': case 'w': case 'W':
         yGOD_key (a_minor);
         break;
      case ',':
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case ':'      :
         SOURCE_start   (":");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         rc = 'a';
         break;
      }
   }
   /*---(buffer/area)--------------------*/
   if (a_major == ',') {
      switch (a_minor) {
      case 'p':
         MODE_enter  (MODE_PROGRESS);
         /*> TICK_draw ();                                                            <*/
         break;
      }
   }
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
PROGRESS_mode           (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   int         x_leg       = 0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_PROGRESS)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      /*---(modes)-----------------------*/
      switch (a_minor) {
      case ',':
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      case '^':
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
         break;
      }
      /*---(unit test data)--------------*/
      /*> if (a_minor == '#') {                                                       <* 
       *>    KINE_unitcond ();                                                        <* 
       *> }                                                                           <*/
      /*---(vertical movement)-----------*/
      /*> x_leg = my.p_leg;                                                           <* 
       *> if (strchr ("_KkjJG", a_minor) != 0) {                                      <* 
       *>    yVIKEYS_keys_vert    (a_minor, &my.p_leg, 1.0, 0.0, 5.0);                <* 
       *> }                                                                           <* 
       *> if (x_leg != my.p_leg) TICK_draw ();                                        <*/
      /*---(zoom and retreat)------------*/
      switch (a_minor) {
      case '+':
         yVIKEYS_scale_less    (&myVIKEYS.p_inc);
         /*> TICK_draw ();                                                            <*/
         break;
      case '-':
         yVIKEYS_scale_more    (&myVIKEYS.p_inc);
         /*> TICK_draw ();                                                            <*/
         break;
      }
      /*---(play and stop)---------------*/
      switch (a_minor) {
      case '>':
         yVIKEYS_speed_more   (&myVIKEYS.p_waitns);
         /*> TICK_draw ();                                                            <*/
         break;
      case '<':
         yVIKEYS_speed_less   (&myVIKEYS.p_waitns);
         /*> TICK_draw ();                                                            <*/
         break;
      case '.':
         yVIKEYS_speed_play   (&myVIKEYS.p_waitns);  /* toggles */
         /*> TICK_draw ();                                                            <*/
         break;
      }
      /*---(horizontal movement)---------*/
      /*> if (strchr ("0HhlL$", a_minor) != 0) {                                                        <* 
       *>    yVIKEYS_keys_horz    (a_minor, &myVIKEYS.p_cursec, myVIKEYS.p_inc, 0.0, myVIKEYS.p_len);   <* 
       *> }                                                                                             <*/
      /*---(other)-----------------------*/
      switch (a_minor) {
      case '?':
         myVIKEYS.p_debug = 'y';
         /*> TICK_show ();                                                     <*/
         break;
         /*> printf ("myVIKEYS.p_width         = %10.3f\n", myVIKEYS.w_wide * 1.0);                         <* 
          *> printf ("full size          = %10.3f\n", myVIKEYS.w_wide * 2.0);                         <* 
          *> printf ("x_half             = %10.3f\n", myVIKEYS.w_wide * 1.0);                         <* 
          *> printf ("myVIKEYS.p_cursec             = %10.3f\n", myVIKEYS.p_cursec);                                   <* 
          *> printf ("x_inc              = %10.3f\n", 10.0);                                     <* 
          *> printf ("myVIKEYS.p_inc           = %10.3f\n", myVIKEYS.p_inc);                                 <* 
          *> printf ("x_sec              = %10.3f\n", 10.0 / myVIKEYS.p_inc);                          <* 
          *> printf ("# secs on screen   = %10.3f\n", (myVIKEYS.w_wide * 2.0) / (10.0 / myVIKEYS.p_inc));   <* 
          *> printf (" * myVIKEYS.p_cursec          = %10.3f\n", myVIKEYS.p_cursec * (10.0 / myVIKEYS.p_inc));               <*/
         /*> +*---(calculate offset)---------------*+                                                 <* 
          *> x_right     = myVIKEYS.w_wide;                                                                <* 
          *> x_cur       = myVIKEYS.p_cursec * (x_inc / myVIKEYS.p_inc) / 2.0f;   /+ texture displayed at 0.5x +/      <* 
          *> x_half      = x_right;                                                                   <* 
          *> /+---(set beginning and end)----------+/                                                 <* 
          *> x_beg       = x_cur - x_half;                                                            <* 
          *> /+> x_beg       = myVIKEYS.w_wide;                                                      <+/   <* 
          *> if (x_beg < 0.0)  x_beg = 0.0;                                                           <* 
          *> x_end       = x_beg + (x_half * 2.0f);                                                   <* 
          *> /+---(scale number to texture)--------+/                                                 <* 
          *> x_beg      /= myVIKEYS.p_texw;                                                                 <* 
          *> x_end      /= myVIKEYS.p_texw;                                                                 <*/
      }
   }
   /*---(alignment)----------------------*/
   if (a_major == '^') {
      switch (a_minor) {
      case '0': myVIKEYS.p_curpos = '0'; break;
      case 's': myVIKEYS.p_curpos = 's'; break;
      case 'h': myVIKEYS.p_curpos = 'h'; break;
      case 'c': myVIKEYS.p_curpos = 'c'; break;
      case 'l': myVIKEYS.p_curpos = 'l'; break;
      case 'e': myVIKEYS.p_curpos = 'e'; break;
      case '$': myVIKEYS.p_curpos = '$'; break;
      }
   }
   /*---(buffer/area)--------------------*/
   if (a_major == ',') {
      switch (a_minor) {
      case ',':
      case 'a':
         MODE_exit  ();
         /*> TICK_draw ();                                                            <*/
         break;
      /*> case 'p':                                                                   <* 
       *>    myVIKEYS.scrn = SCRN_NORM;                                               <* 
       *>    TICK_draw ();                                                            <* 
       *>    break;                                                                   <* 
       *> case 'P':                                                                   <* 
       *>    myVIKEYS.scrn = SCRN_PROG;                                               <* 
       *>    TICK_draw ();                                                            <* 
       *>    break;                                                                   <*/
      }
   }
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}
