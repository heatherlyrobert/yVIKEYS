/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */
#define     MAX_SPEED   50



/*===[[ FILE-WIDE VARIABLES ]]================================================*/
static char    s_index    = -1;             /* play speed index               */
static char    s_moving   = '-';            /* playing (y/n)                  */
static float   s_advance  = 0.0;            /* x-advance while moving         */
static double  s_waitns   = 100000000;      /* time to wait while moving      */



/*===[[ STRUCTURE ]]==========================================================*/
typedef   struct cSPEED  tSPEED;
struct cSPEED {
   char        code        [LEN_LABEL];
   char        desc        [LEN_STR  ];
   float       speed;   
   float       adv_sec;
   float       wait_ns;
};
static tSPEED s_speed_info [MAX_SPEED] = {
   { "-50.0x"    , "blur"             ,    -50.00 ,    -1.2500 ,    2000000 },
   { "-20.0x"    , "super fast"       ,    -20.00 ,    -0.5000 ,    2000000 },
   { "-10.0x"    , "very fast"        ,    -10.00 ,    -0.2500 ,    2000000 },
   { "-5.00x"    , "faster"           ,     -5.00 ,    -0.1250 ,    2000000 },
   { "-2.00x"    , "double"           ,     -2.00 ,    -0.0500 ,    2000000 },
   { "-1.00x"    , "normal"           ,     -1.00 ,    -0.0250 ,    2000000 },
   { "-0.75x"    , "three-quarters"   ,     -0.75 ,    -0.0150 ,    4500000 },
   { "-0.50x"    , "half"             ,     -0.50 ,    -0.0100 ,    5000000 },
   { "-0.25x"    , "quarter"          ,     -0.25 ,    -0.0060 ,    5000000 },
   { "-0.10x"    , "slow"             ,     -0.10 ,    -0.0025 ,    4500000 },
   { "-0.05x"    , "super slow"       ,     -0.05 ,    -0.0010 ,    5000000 },
   { "-0.02x"    , "ultra slow"       ,     -0.02 ,    -0.0005 ,    5000000 },
   { "+0.00x"    , "stopped"          ,      0.00 ,     0.0000 ,   10000000 },
   { "+0.02x"    , "ultra slow"       ,      0.02 ,     0.0005 ,    5000000 },
   { "+0.05x"    , "super slow"       ,      0.05 ,     0.0010 ,    5000000 },
   { "+0.10x"    , "slow"             ,      0.10 ,     0.0025 ,    4500000 },
   { "+0.25x"    , "quarter"          ,      0.25 ,     0.0060 ,    5000000 },
   { "+0.50x"    , "half"             ,      0.50 ,     0.0100 ,    5000000 },
   { "+0.75x"    , "three-quarters"   ,      0.75 ,     0.0150 ,    4500000 },
   { "+1.00x"    , "normal"           ,      1.00 ,     0.0250 ,    2000000 },
   { "+2.00x"    , "double"           ,      2.00 ,     0.0500 ,    2000000 },
   { "+5.00x"    , "faster"           ,      5.00 ,     0.1250 ,    2000000 },
   { "+10.0x"    , "very fast"        ,     10.00 ,     0.2500 ,    2000000 },
   { "+20.0x"    , "super fast"       ,     20.00 ,     0.5000 ,    2000000 },
   { "+50.0x"    , "blur"             ,     50.00 ,     1.2500 ,    2000000 },
   { "??????"    , "end-of-list"      ,      0.00 ,     0.0000 ,          0 },
};



/*====================------------------------------------====================*/
/*===----                       progress speed                         ----===*/
/*====================------------------------------------====================*/
static void      o___SPEED___________________o (void) {;}

char
yVIKEYS_speed_set  (char *a_code, double *a_waitns)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   char        x_index     = -1;
   /*---(defense)------------------------*/
   --rce;  if (strlen (a_code) != 6) {
      return rce;
   }
   /*---(find entry in table)------------*/
   for (i = 0; i < MAX_SPEED; ++i) {
      if (s_speed_info [i].code [0] != a_code [0])      continue;
      if (s_speed_info [i].code [1] != a_code [1])      continue;
      if (strcmp (s_speed_info [i].code, a_code) != 0)  continue;
      x_index = i;
   }
   /*---(defense)------------------------*/
   --rce;  if (x_index < 0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   s_index     = x_index;
   s_advance   = s_speed_info [s_index].adv_sec;
   s_waitns    = s_speed_info [s_index].wait_ns;
   if (a_waitns != NULL)  *a_waitns = s_waitns;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_stop (double *a_waitns)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   char       *x_code      = "+0.00x";
   char        x_index     = -1;
   /*---(find stopped values)------------*/
   for (i = 0; i < MAX_SPEED; ++i) {
      if (s_speed_info [i].code [0] != x_code [0])      continue;
      if (s_speed_info [i].code [1] != x_code [1])      continue;
      if (strcmp (s_speed_info [i].code, x_code) != 0)  continue;
      x_index = i;
   }
   /*---(defense)------------------------*/
   --rce;  if (x_index < 0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   s_moving    = '-';
   s_waitns    = s_speed_info [x_index].wait_ns;
   if (a_waitns != NULL)  *a_waitns = s_waitns;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_play (double *a_waitns)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce; if (s_index < 0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   if (s_moving == '-') {
      s_moving    = 'y';
      s_advance   = s_speed_info [s_index].adv_sec;
      s_waitns    = s_speed_info [s_index].wait_ns;
   } else {
      yVIKEYS_speed_stop (a_waitns);
   }
   if (a_waitns != NULL)  *a_waitns = s_waitns;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_more (double *a_waitns)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce; if (s_index >= MAX_SPEED - 1) {
      return rce;
   }
   --rce;  if (s_speed_info [s_index + 1].code [0] == '?') {
      return rce;
   }
   /*---(set key values)-----------------*/
   ++(s_index);
   s_advance   = s_speed_info [s_index].adv_sec;
   s_waitns    = s_speed_info [s_index].wait_ns;
   if (a_waitns != NULL)  *a_waitns = s_waitns;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_less (double *a_waitns)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce; if (s_index <  0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   --(s_index);
   s_advance   = s_speed_info [s_index].adv_sec;
   s_waitns    = s_speed_info [s_index].wait_ns;
   if (a_waitns != NULL)  *a_waitns = s_waitns;
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_desc (char *a_text)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (a_text == NULL) {
      return rce;
   }
   --rce;  if (s_index < 0) {
      return rce;
   }
   /*---(create text line)---------------*/
   snprintf (a_text, LEN_STR, "%s %s %.3fa %.1lfms (%s)", (s_moving == 'y') ? "play" : "stop", s_speed_info [s_index].code, s_advance, s_waitns / 1000000, s_speed_info [s_index].desc);
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_speed_adv  (double *a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (s_moving != 'y') {
      return rce;
   }
   --rce;  if (a_pos  == NULL) {
      return rce;
   }
   --rce;  if (s_index < 0) {
      return rce;
   }
   /*---(update position)----------------*/
   *a_pos += s_advance;
   /*---(complete)-----------------------*/
   return 0;
}




/*============================----end-of-source---============================*/
