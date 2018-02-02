/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*
 * yVIKEYS_keys manages a perfectly regular "grid" of cells/points/locations.
 * the grid maps to the screen and one cell is the minimum movement.  when a
 * user zooms, the grid is used exactly the same.
 *
 * once yVIKEYS_keys passes control back to the program is can map the outcome
 * to the correct coordinate system in the application.
 *
 * i will initially build it aroung three use-cases...
 *    apelles -- paint package with a purely rectangular workspace
 *    gyges   -- adjustable column widths and row heights
 *    arachne -- 3d space viewer
 * in all these cases, columns are the same, top-to-bottom, and rows are same
 * left to right.  this means, a side-to-side move on row 0, 10, 1000, or 9999
 * results in the exact same destination column regardless of row.
 *
 *
 * 
 * i will build on the gyges model as it is working a sweet.
 *
 * sample column map for gyges with 8 default width columns and a little more
 *    "1111111122222222333333334444444455555555666666667777777788888888>>>"
 * another map if the columns are not consistent width
 *    "1111111111112222333344445555555555555555667788999999999999999999>>>"
 *
 * i need a callback function called s_ender which identifies all edges and
 * ends for a particular cell.  it should be called as soon as 'e' is pressed.
 *
 *
 *
 *
 *
 *
 */


static char s_wrapping     = '-';




int
yVIKEYS_map_ends      (char a_major, char a_minor)
{
}

int
yVIKEYS_map_edges     (char a_major, char a_minor)
{
}

int
yVIKEYS_map_gz_horz   (char a_major, char a_minor)
{
}

int
yVIKEYS_map_gz_vert   (char a_major, char a_minor)
{
}



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
