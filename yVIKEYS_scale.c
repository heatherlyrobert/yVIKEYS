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
#define     MAX_SCALE   50



/*===[[ FILE-WIDE VARIABLES ]]================================================*/
static char    s_index    = -1;             /* scale index                    */
static float   s_inc      =  1;             /* increment in movement          */
static char    s_base     = ' ';            /* scale prefix for labels        */
static float   s_multi    =  1;             /* increment in movement          */



/*===[[ STRUCTURE ]]==========================================================*/
typedef   struct cSCALE  tSCALE;
struct cSCALE {
   char        type;
   char        code        [LEN_LABEL];
   char        label       [LEN_LABEL];
   char        desc        [LEN_STR  ];
   char        power;
   float       unit;
   char        base;
   float       conv;
   float       multi;
};
static tSCALE s_scale_info [MAX_SCALE] = {
   { 'm' , "T-", "tera"          , "terasecond"        ,  12 , 1000000000000.0            , 'T' , 1000000000000.0            ,   1.0   },
   { 'm' , "GH", "gigah"         , "100 gigasecond"    ,  11 ,  100000000000.0            , 'T' ,    1000000000.0            ,   0.1   },
   { 'm' , "GD", "gigad"         , "10 gigasecond"     ,  10 ,   10000000000.0            , 'G' ,    1000000000.0            ,  10.0   },
   { 'm' , "G-", "giga"          , "gigasecond"        ,   9 ,    1000000000.0            , 'G' ,    1000000000.0            ,   1.0   },
   { 'm' , "MH", "megah"         , "100 megasecond"    ,   6 ,     100000000.0            , 'G' ,       1000000.0            ,   0.1   },
   { 'm' , "MD", "megad"         , "10 megasecond"     ,   6 ,      10000000.0            , 'M' ,       1000000.0            ,  10.0   },
   { 'm' , "M-", "mega"          , "megasecond"        ,   6 ,       1000000.0            , 'M' ,       1000000.0            ,   1.0   },
   { 'm' , "KH", "kiloh"         , "100 kilosecond"    ,   5 ,        100000.0            , 'M' ,          1000.0            ,   0.1   },
   { 'm' , "KD", "kilod"         , "10 kilosecond"     ,   4 ,         10000.0            , 'K' ,          1000.0            ,  10.0   },
   { 'm' , "K-", "kilo"          , "kilosecond"        ,   3 ,          1000.0            , 'K' ,          1000.0            ,   1.0   },
   { 'm' , "H-", "hecto"         , "hectosecond"       ,   2 ,           100.0            , 'K' ,           100.0            ,   0.1   },
   { 'm' , "D-", "deca"          , "decasecond"        ,   1 ,            10.0            , 'H' ,            10.0            ,   0.1   },
   { '-' , "-5", "secs5"         , "5 second"          ,   0 ,             5.0            , 'D' ,             5.0            ,   0.5   },
   { '-' , "-2", "secs2"         , "2 second"          ,   0 ,             2.0            , 'D' ,             2.0            ,   0.2   },
   { 'm' , "--", "secs"          , "second"            ,   0 ,             1.0            , 'D' ,             1.0            ,   0.1   },
   { '-' , "d5", "deci5"         , "5 decisecond"      ,  -1 ,             0.5            , 's' ,             0.5            ,   0.5   },
   { '-' , "d2", "deci2"         , "2 decisecond"      ,  -1 ,             0.2            , 's' ,             0.2            ,   0.2   },
   { 'm' , "d-", "deci"          , "decisecond"        ,  -1 ,             0.1            , 's' ,             0.1            ,   0.1   },
   { '-' , "c5", "centi5"        , "5 centisecond"     ,  -1 ,             0.05           , 'd' ,             0.050          ,   0.5   },
   { '-' , "c2", "centi2"        , "2 centisecond"     ,  -1 ,             0.02           , 'd' ,             0.020          ,   0.2   },
   { 'm' , "c-", "centi"         , "centisecond"       ,  -2 ,             0.01           , 'd' ,             0.010          ,   0.1   },
   { '-' , "m5", "milli"         , "5 millisecond"     ,  -3 ,             0.005          , 'c' ,             0.005          ,   0.5   },
   { '-' , "m2", "milli"         , "2 millisecond"     ,  -3 ,             0.002          , 'c' ,             0.002          ,   0.2   },
   { 'm' , "m-", "milli"         , "millisecond"       ,  -3 ,             0.001          , 'c' ,             0.001          ,   0.1   },
   { 'm' , "uH", "microh"        , "100 microsecond"   ,  -4 ,             0.0001         , 'm' ,             0.000001       ,   0.1   },
   { 'm' , "uD", "microd"        , "10 microsecond"    ,  -5 ,             0.00001        , 'u' ,             0.000001       ,  10.0   },
   { 'm' , "u-", "micro"         , "microsecond"       ,  -6 ,             0.000001       , 'u' ,             0.000001       ,   1.0   },
   { 'm' , "nH", "nanoh"         , "100 nanosecond"    ,  -7 ,             0.0000001      , 'u' ,             0.000000001    ,   0.1   },
   { 'm' , "nD", "nanod"         , "10 nanosecond"     ,  -8 ,             0.00000001     , 'n' ,             0.000000001    ,  10.0   },
   { 'm' , "n-", "nano"          , "nanosecond"        ,  -9 ,             0.000000001    , 'n' ,             0.000000001    ,   1.0   },
   { 'm' , "pH", "picoh"         , "100 picosecond"    , -10 ,             0.0000000001   , 'n' ,             0.000000000001 ,   0.1   },
   { 'm' , "pD", "picod"         , "10 picosecond"     , -11 ,             0.00000000001  , 'p' ,             0.000000000001 ,  10.0   },
   { 'm' , "p-", "pico"          , "picosecond"        , -12 ,             0.000000000001 , 'p' ,             0.000000000001 ,   1.0   },
   { 'm' , "??", "----"          , "end-of-scales"     ,   0 ,             0.0            , '-' ,             0.0            ,   1.0   },
};



/*====================------------------------------------====================*/
/*===----                       progress scale                         ----===*/
/*====================------------------------------------====================*/
static void      o___SCALE___________________o (void) {;}

char
yVIKEYS_scale_set  (char *a_code, double *a_inc)
{
   char        rce         = -10;
   int         i           = 0;
   char        x_code      = -1;
   --rce;  if (strlen (a_code) != 2) {
      return rce;
   }
   for (i = 0; i < MAX_SCALE; ++i) {
      if (s_scale_info [i].code [0] != a_code [0])  continue;
      if (s_scale_info [i].code [1] != a_code [1])  continue;
      x_code = i;
   }
   --rce;  if (x_code < 0) {
      return rce;
   }
   s_index    = x_code;
   s_inc      = s_scale_info [s_index].unit;
   s_base     = s_scale_info [s_index].base;
   s_multi    = s_scale_info [s_index].multi;
   if (a_inc   != NULL)  *a_inc   = s_inc;
   return 0;
}

char
yVIKEYS_scale_less (double *a_inc)
{
   char        rce         = -10;
   --rce; if (s_index >= MAX_SCALE - 1) {
      return rce;
   }
   --rce;  if (s_scale_info [s_index + 1].code [0] == '?') {
      return rce;
   }
   ++(s_index);
   s_inc      = s_scale_info [s_index].unit;
   s_base     = s_scale_info [s_index].base;
   s_multi    = s_scale_info [s_index].multi;
   if (a_inc   != NULL)  *a_inc   = s_inc;
   return 0;
}

char
yVIKEYS_scale_more (double *a_inc)
{
   char        rce         = -10;
   --rce; if (s_index <= 0) {
      return rce;
   }
   --(s_index);
   s_inc      = s_scale_info [s_index].unit;
   s_base     = s_scale_info [s_index].base;
   s_multi    = s_scale_info [s_index].multi;
   if (a_inc   != NULL)  *a_inc   = s_inc;
   return 0;
}

char
yVIKEYS_scale_desc (char *a_text)
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
   snprintf (a_text, LEN_STR, "%s (%s) resolution", s_scale_info [s_index].desc, s_scale_info [s_index].code);
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_scale_base (double *a_multi, char *a_base)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (s_index < 0) {
      return rce;
   }
   /*---(full out values)----------------*/
   if (a_multi != NULL)  *a_multi = s_multi;
   if (a_base  != NULL)  *a_base  = s_base;
   /*---(complete)-----------------------*/
   return 0;
}

/*============================----end-of-source---============================*/
