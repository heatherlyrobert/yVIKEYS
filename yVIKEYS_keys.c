/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



typedef struct cLOCAL tLOCAL;
struct cLOCAL {
   /*---(overall)-----------*/
   char        debug;
   int         logger;
};
static tLOCAL its;
#define     DEBUG_VIKEYS   if (its.debug        == 'y')
/*> #define     DEBUG_VIMODE   if (its.debug_mode   == 'y')                           <* 
 *> #define     DEBUG_VISPEE   if (its.debug_speed  == 'y')                           <* 
 *> #define     DEBUG_VISCAL   if (its.debug_scale  == 'y')                           <*/




/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void      o___UTILITY_________________o (void) {;}
char        yVIKEYS_ver     [500];

char*      /* ---- : return library versioning information -------------------*/
yVIKEYS_version    (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc    ]", 15);
#elif  __HEPH__  > 0
   strncpy (t, "[hephaestus ]", 15);
#else
   strncpy (t, "[unknown    ]", 15);
#endif
   snprintf (yVIKEYS_ver, 100, "%s   %s : %s", t, YVIKEYS_VER_NUM, YVIKEYS_VER_TXT);
   return yVIKEYS_ver;
}

char         /*--> set debugging mode --------------------[ ------ [ ------ ]-*/
yVIKEYS_debug      (char a_flag)
{
   /*---(set debug flag)-----------------*/
   if   (a_flag == 'y')  its.debug   = 'y';
   else                  its.debug   = '-';
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_line  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "0LlhH$_KkjJG";
   double      x_base      = 0.0;
   /*---(defense)------------------------*/
   DEBUG_VIKEYS yLOG_point   ("a_base"    , a_base);
   --rce;  if (a_base  == NULL) {
      DEBUG_VIKEYS yLOG_note    ("base value pointer not set");
      DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_VIKEYS yLOG_double  ("a_base"    , *a_base);
   DEBUG_VIKEYS yLOG_double  ("a_inc"     , a_inc);
   DEBUG_VIKEYS yLOG_double  ("a_min"     , a_min);
   DEBUG_VIKEYS yLOG_double  ("a_max"     , a_max);
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
   /*---(complete)-----------------------*/
   DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_horz  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "0LlhH$";
   /*---(header)-------------------------*/
   DEBUG_VIKEYS yLOG_enter   (__FUNCTION__);
   DEBUG_VIKEYS yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_VIKEYS yLOG_info    ("x_minors"  , x_minors);
   if (strchr (x_minors, a_minor) == 0) {
      DEBUG_VIKEYS yLOG_note    ("not a valid a_minor movement key");
      DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   rce = yVIKEYS_keys_line (a_minor, a_base, a_inc, a_min, a_max);
   /*---(complete)-----------------------*/
   DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
   return rce;
}

char         /*--> process keystrokes in progress mode ---[--------[--------]-*/
yVIKEYS_keys_vert  (char a_minor, double *a_base, double a_inc, double a_min, double a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;      /* return code for errors              */
   char        x_minors    [LEN_STR]  = "_KkjJG";
   /*---(header)-------------------------*/
   DEBUG_VIKEYS yLOG_enter   (__FUNCTION__);
   DEBUG_VIKEYS yLOG_char    ("a_minor"   , a_minor);
   /*---(defense)------------------------*/
   DEBUG_VIKEYS yLOG_info    ("x_minors"  , x_minors);
   if (strchr (x_minors, a_minor) == 0) {
      DEBUG_VIKEYS yLOG_note    ("not a valid a_minor movement key");
      DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   rce = yVIKEYS_keys_line (a_minor, a_base, a_inc, a_min, a_max);
   /*---(complete)-----------------------*/
   DEBUG_VIKEYS yLOG_exit    (__FUNCTION__);
   return rce;
}

/*============================----end-of-source---============================*/
