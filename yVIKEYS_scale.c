/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



/*===[[ SCALE ]]==============================================================*/
#define     MAX_SCALE   50
static char    s_scale    = -1;             /* scale index                    */
static float   s_inc      =  1;             /* increment in movement          */
static char    s_base     = ' ';            /* scale prefix for labels        */
static float   s_multi    =  1;             /* increment in movement          */

typedef   struct cSCALE  tSCALE;
struct cSCALE {
   char        type;
   char        terse       [LEN_LABEL];
   char        label       [LEN_LABEL];
   char        desc        [LEN_STR  ];
   char        power;
   float       unit;
   char        base;
   float       conv;
   float       multi;
};
static tSCALE s_scale_info [MAX_SCALE] = {
   { 'm' , "T-", "tera"      , "tera  (trillion)"      ,  12 , 1000000000000.0            , 'T' , 1000000000000.0            ,   1.0   },
   { 'm' , "Gh", "giga/H"    , "giga  (100 billion)"   ,  11 ,  100000000000.0            , 'G' ,    1000000000.0            ,   0.1   },
   { 'm' , "Gd", "giga/D"    , "giga  (10 billion)"    ,  10 ,   10000000000.0            , 'G' ,    1000000000.0            ,  10.0   },
   { 'm' , "G-", "giga"      , "giga  (billion)"       ,   9 ,    1000000000.0            , 'G' ,    1000000000.0            ,   1.0   },
   { 'm' , "Mh", "mega/H"    , "mega  (100 million)"   ,   8 ,     100000000.0            , 'M' ,       1000000.0            ,   0.1   },
   { 'm' , "Md", "mega/D"    , "mega  (10 million)"    ,   7 ,      10000000.0            , 'M' ,       1000000.0            ,  10.0   },
   { 'm' , "M-", "mega"      , "mega  (million)"       ,   6 ,       1000000.0            , 'M' ,       1000000.0            ,   1.0   },
   { 'm' , "Kh", "kilo/H"    , "kilo  (100 thousand)"  ,   5 ,        100000.0            , 'K' ,          1000.0            ,   0.1   },
   { 'm' , "Kd", "kilo/D"    , "kilo  (10 thousand)"   ,   4 ,         10000.0            , 'K' ,          1000.0            ,  10.0   },
   { 'm' , "K-", "kilo"      , "kilo  (thousand)"      ,   3 ,          1000.0            , 'K' ,          1000.0            ,   1.0   },
   { 'm' , "H-", "heka"      , "hecto (hundred)"       ,   2 ,           100.0            , 'H' ,           100.0            ,   0.1   },
   { 'm' , "D-", "deca"      , "deca  (ten)"           ,   1 ,            10.0            , 'D' ,            10.0            ,   0.1   },
   { 'm' , "--", "base"      , "base  (unit)"          ,   0 ,             1.0            , '-' ,             1.0            ,   0.1   },
   { 'm' , "d-", "deci"      , "deci  (tenth)"         ,  -1 ,             0.1            , 'd' ,             0.1            ,   0.1   },
   { 'm' , "c-", "cent"      , "centi (hundredth)"     ,  -2 ,             0.01           , 'c' ,             0.010          ,   0.1   },
   { 'm' , "m-", "mill"      , "milli (thousandth)"    ,  -3 ,             0.001          , 'm' ,             0.001          ,   0.1   },
   { 'm' , "mD", "mill/D"    , "milli (10 thousandth)" ,  -4 ,             0.0001         , 'm' ,             0.000001       ,   0.1   },
   { 'm' , "mH", "mill/H"    , "milli (100 thousandth)",  -5 ,             0.00001        , 'm' ,             0.000001       ,  10.0   },
   { 'm' , "u-", "micr"      , "micro (millionth)"     ,  -6 ,             0.000001       , 'u' ,             0.000001       ,   1.0   },
   { 'm' , "uD", "micr/D"    , "micro (10 millionth)"  ,  -7 ,             0.0000001      , 'u' ,             0.000000001    ,   0.1   },
   { 'm' , "uH", "micr/H"    , "micro (100 millionth)" ,  -8 ,             0.00000001     , 'u' ,             0.000000001    ,  10.0   },
   { 'm' , "n-", "nano"      , "nano  (billionth)"     ,  -9 ,             0.000000001    , 'n' ,             0.000000001    ,   1.0   },
   { 'm' , "nD", "nano/D"    , "nano  (10 billionth)"  , -10 ,             0.0000000001   , 'n' ,             0.000000000001 ,   0.1   },
   { 'm' , "nH", "nano/H"    , "nano  (100 billionth)" , -11 ,             0.00000000001  , 'n' ,             0.000000000001 ,  10.0   },
   { 'm' , "p-", "pico"      , "pico  (trillonth)"     , -12 ,             0.000000000001 , 'p' ,             0.000000000001 ,   1.0   },
   { 'm' , "??", "----"      , "end-of-scales"         ,   0 ,             0.0            , '-' ,             0.0            ,   1.0   },
};



/*===[[ SPEED ]]==============================================================*/
#define     MAX_SPEED   50
static char    s_speed    = -1;             /* play speed index               */
static char    s_moving   = '-';            /* playing (y/n)                  */
static float   s_advance  = 0.0;            /* x-advance while moving         */
static double  s_waitns   = 100000000;      /* time to wait while moving      */

typedef   struct cSPEED  tSPEED;
struct cSPEED {
   char        terse       [LEN_LABEL];
   char        desc        [LEN_STR  ];
   float       speed;   
   float       adv_sec;
   float       wait_ns;
};
static tSPEED s_speed_info [MAX_SPEED] = {
   { "-inf"      , "infinity"         ,  -9999.99 ,    -1.2500 ,          0 },
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
   { "+inf"      , "infinity"         ,   9999.99 ,     1.2500 ,          0 },
   { "??????"    , "end-of-list"      ,      0.00 ,     0.0000 ,          0 },
};



/*===[[ UPDATES ]]============================================================*/
#define     MAX_UPDATE  50
static int     s_update   =  0;    /* how many screen updates per second      */
static long    s_average  =  0;

typedef  struct cUPDATE tUPDATE;
struct cUPDATE {
   char        terse       [LEN_LABEL];
   char        desc        [LEN_LABEL];
   float       update;
};
static tUPDATE s_update_info [MAX_UPDATE] = {
   /*---(default)--------------------------*/
   { "every" , "every loop"       ,   0.00 },
   /*---(faster)---------------------------*/
   { "1cs"   , "very fast"        ,   0.01 },
   { "2cs"   , "faster"           ,   0.02 },
   { "5cs"   , "fast"             ,   0.05 },
   /*---(normal)---------------------------*/
   { "10cs"  , "normal"           ,   0.10 },
   /*---(slower)---------------------------*/
   { "25cs"  , "slow"             ,   0.25 },
   { "50cs"  , "slower"           ,   0.50 },
   { "100cs" , "very slow"        ,   1.00 },
   /*---(end)------------------------------*/
   { "---"   , "end-of-list"      ,   0.00 },
   /*---(done)-----------------------------*/
};



/*===[[ UPDATES ]]============================================================*/
#define     NSEC        1000000000
#define     MAX_DELAY   50
static int     s_delay    =  0;    /* how man seconds between main loops      */

typedef  struct cDELAY tDELAY;
struct cDELAY {
   char        terse       [LEN_LABEL];
   char        desc        [LEN_LABEL];
   float       delay;
};
static tDELAY s_delay_info [MAX_DELAY] = {
   /*---(default)---------------------------------*/
   { "keys"   , "keyboard"           ,   0.0      },
   /*---(faster)----------------------------------*/
   { "1us"    , "millionth"          ,   0.000001 },
   { "10us"   , "ten millionths"     ,   0.00001  },
   { "100us"  , "hundred millionths" ,   0.0001   },
   /*---(normal)----------------------------------*/
   { "1ms"    , "thousandth"         ,   0.001    },
   { "5ms"    , "five thousandths"   ,   0.005    },
   { "10ms"   , "hundreth"           ,   0.01     },
   /*---(slower)----------------------------------*/
   { "20ms"   , "fifteth"            ,   0.02     },
   { "50ms"   , "twenteth"           ,   0.05     },
   { "100ms"  , "tenth"              ,   0.1      },
   /*---(glacial)---------------------------------*/
   { "1s"     , "second"             ,   1.0      },
   /*---(end of list)-----------------------------*/
   { "---"    , "end-of-list"        ,   0.0      },
   /*---(done)------------------------------------*/
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
      if (s_scale_info [i].terse [0] != a_code [0])  continue;
      if (s_scale_info [i].terse [1] != a_code [1])  continue;
      x_code = i;
   }
   --rce;  if (x_code < 0) {
      return rce;
   }
   s_scale    = x_code;
   s_inc      = s_scale_info [s_scale].unit;
   s_base     = s_scale_info [s_scale].base;
   s_multi    = s_scale_info [s_scale].multi;
   if (a_inc   != NULL)  *a_inc   = s_inc;
   return 0;
}

char
yVIKEYS_scale_less (double *a_inc)
{
   char        rce         = -10;
   --rce; if (s_scale >= MAX_SCALE - 1) {
      return rce;
   }
   --rce;  if (s_scale_info [s_scale + 1].terse [0] == '?') {
      return rce;
   }
   ++(s_scale);
   s_inc      = s_scale_info [s_scale].unit;
   s_base     = s_scale_info [s_scale].base;
   s_multi    = s_scale_info [s_scale].multi;
   if (a_inc   != NULL)  *a_inc   = s_inc;
   return 0;
}

char
yVIKEYS_scale_more (double *a_inc)
{
   char        rce         = -10;
   --rce; if (s_scale <= 0) {
      return rce;
   }
   --(s_scale);
   s_inc      = s_scale_info [s_scale].unit;
   s_base     = s_scale_info [s_scale].base;
   s_multi    = s_scale_info [s_scale].multi;
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
   --rce;  if (s_scale < 0) {
      return rce;
   }
   /*---(create text line)---------------*/
   snprintf (a_text, LEN_STR, "%s (%s) resolution", s_scale_info [s_scale].desc, s_scale_info [s_scale].terse);
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_scale_base (double *a_multi, char *a_base)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (s_scale < 0) {
      return rce;
   }
   /*---(full out values)----------------*/
   if (a_multi != NULL)  *a_multi = s_multi;
   if (a_base  != NULL)  *a_base  = s_base;
   /*---(complete)-----------------------*/
   return 0;
}



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
      if (s_speed_info [i].terse [0] != a_code [0])      continue;
      if (s_speed_info [i].terse [1] != a_code [1])      continue;
      if (strcmp (s_speed_info [i].terse, a_code) != 0)  continue;
      x_index = i;
   }
   /*---(defense)------------------------*/
   --rce;  if (x_index < 0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   s_speed     = x_index;
   s_advance   = s_speed_info [s_speed].adv_sec;
   s_waitns    = s_speed_info [s_speed].wait_ns;
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
      if (s_speed_info [i].terse [0] != x_code [0])      continue;
      if (s_speed_info [i].terse [1] != x_code [1])      continue;
      if (strcmp (s_speed_info [i].terse, x_code) != 0)  continue;
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
   --rce; if (s_speed < 0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   if (s_moving == '-') {
      s_moving    = 'y';
      s_advance   = s_speed_info [s_speed].adv_sec;
      s_waitns    = s_speed_info [s_speed].wait_ns;
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
   --rce; if (s_speed >= MAX_SPEED - 1) {
      return rce;
   }
   --rce;  if (s_speed_info [s_speed + 1].terse [0] == '?') {
      return rce;
   }
   /*---(set key values)-----------------*/
   ++(s_speed);
   s_advance   = s_speed_info [s_speed].adv_sec;
   s_waitns    = s_speed_info [s_speed].wait_ns;
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
   --rce; if (s_speed <  0) {
      return rce;
   }
   /*---(set key values)-----------------*/
   --(s_speed);
   s_advance   = s_speed_info [s_speed].adv_sec;
   s_waitns    = s_speed_info [s_speed].wait_ns;
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
   --rce;  if (s_speed < 0) {
      return rce;
   }
   /*---(create text line)---------------*/
   snprintf (a_text, LEN_STR, "%s %s %.3fa %.1lfms (%s)", (s_moving == 'y') ? "play" : "stop", s_speed_info [s_speed].terse, s_advance, s_waitns / 1000000, s_speed_info [s_speed].desc);
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
   --rce;  if (s_speed < 0) {
      return rce;
   }
   /*---(update position)----------------*/
   *a_pos += s_advance;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       main loop timing                       ----===*/
/*====================------------------------------------====================*/
static void      o___LOOPING_________________o (void) {;}

char
yvikeys_loop_init       (void)
{
   s_delay         = 0;
   s_update        = 0;
   myVIKEYS.delay  = 0.0;
   myVIKEYS.secs   = 0;
   myVIKEYS.nsec   = 0;
   myVIKEYS.update = 0.0;
   myVIKEYS.loops  = 1;
   yvikeys_loop_delay  ("");
   yvikeys_loop_update ("");
   return 0;
}


char
yvikeys__loop_calc   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_base      =  0.0;
   /*---(initialize)---------------------*/
   myVIKEYS.secs  = 0;
   myVIKEYS.nsec  = 0;
   myVIKEYS.loops = 1;
   /*---(same)---------------------------*/
   if (s_delay == 0 && s_update == 0)   return 0;
   /*---(delay, no update)---------------*/
   if (myVIKEYS.delay > 0.0 && myVIKEYS.update == 0.0) {
      x_base = myVIKEYS.delay;
   }
   /*---(update, no delay)---------------*/
   else if (myVIKEYS.update > 0.0 && myVIKEYS.delay == 0.0) {
      x_base = myVIKEYS.update;
   }
   /*---(fast enough)--------------------*/
   else if (myVIKEYS.delay <= myVIKEYS.update) {
      x_base = myVIKEYS.delay;
   }
   /*---(too slow)-----------------------*/
   else {
      x_base = myVIKEYS.update;
   }
   /*---(update timing)------------------*/
   if (x_base >= 1.0)  myVIKEYS.secs  = trunc (x_base);
   myVIKEYS.nsec  = (x_base - myVIKEYS.secs) * NSEC;
   /*---(update loops)-------------------*/
   if (myVIKEYS.update != 0.0 && myVIKEYS.delay != 0.0)  {
      myVIKEYS.loops = trunc (myVIKEYS.update / myVIKEYS.delay);
   }
   if (myVIKEYS.loops == 0)  myVIKEYS.loops = 1;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_loop_update     (char *a_update)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_prefix    =  ' ';
   int         i           =    0;
   char        x_index     =   -1;
   char        x_max       =   -1;
   /*---(assign prefix)------------------*/
   if (a_update == NULL)   x_prefix = 0;
   else                    x_prefix = a_update [0];
   /*---(determine max)------------------*/
   for (i = 0; i < MAX_UPDATE; ++i) {
      if (strcmp (s_update_info [i].terse, "---"   ) == 0)  break;
      ++x_max;
   }
   /*---(find entry in table)------------*/
   switch (x_prefix) {
   case  0  :
      x_index = s_update;
      rc = -1;
      break;
   case '=' :
      x_index = s_update;
      break;
   case '>' :
      if (s_update <  x_max)  x_index = ++s_update;
      else {
         x_index = x_max;
         rc = -3;
      }
      break;
   case '<' :
      if (s_update >  1    )  x_index = --s_update;
      else {
         x_index = 1;
         rc = -4;
      }
      break;
   default  :
      for (i = 0; i <= x_max; ++i) {
         if (strcmp (s_update_info [i].terse, a_update) != 0)  continue;
         x_index = i;
         break;
      }
      if (x_index == -1) {
         x_index = s_delay;
         rc = -2;
      }
      break;
   }
   /*---(set key values)-----------------*/
   s_update        = x_index;
   myVIKEYS.update = s_update_info [x_index].update;
   /*---(update looping)-----------------*/
   yvikeys__loop_calc   ();
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys_loop_delay      (char *a_delay)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        x_prefix    =  ' ';
   int         i           =    0;
   char        x_index     =   -1;
   char        x_max       =   -1;
   /*---(assign prefix)------------------*/
   if (a_delay  == NULL)   x_prefix = 0;
   else                    x_prefix = a_delay [0];
   /*---(determine max)------------------*/
   for (i = 0; i < MAX_DELAY; ++i) {
      if (strcmp (s_delay_info [i].terse, "---"  ) == 0)  break;
      ++x_max;
   }
   /*---(find entry in table)------------*/
   switch (x_prefix) {
   case  0  :
      x_index = s_delay;
      rc = -1;
      break;
   case '=' :
      x_index = s_delay;
      break;
   case '>' :
      if (s_delay  <  x_max)  x_index = ++s_delay;
      else {
         x_index = x_max;
         rc = -3;
      }
      break;
   case '<' :
      if (s_delay  >  1    )  x_index = --s_delay;
      else {
         x_index = 1;
         rc = -4;
      }
      break;
   default  :
      for (i = 0; i < x_max; ++i) {
         if (strcmp (s_delay_info [i].terse, a_delay) != 0)  continue;
         x_index = i;
         break;
      }
      if (x_index == -1) {
         x_index = s_delay;
         rc = -2;
      }
      break;
   }
   /*---(set key values)-----------------*/
   s_delay         = x_index;
   myVIKEYS.delay  = s_delay_info [x_index].delay;
   /*---(update looping)-----------------*/
   yvikeys__loop_calc   ();
   /*---(complete)-----------------------*/
   return rc;
}

static long long x_rem    = 0;
static long long x_used   = 0;
static float     x_pct    = 0.0;

char
yvikeys_loop_sleep      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   static long x_secs      =    0;
   static long x_nsec      =    0;
   long        x_diff      =    0;
   long        x_targ      =    0;
   /*> long        x_rem       =    0;                                                <*/
   tTSPEC      x_dur;
   static int  x_loop      =    0;
   static long long x_total     =    0;
   /*---(get current time)---------------*/
   clock_gettime  (CLOCK_MONOTONIC_RAW, &x_dur);
   x_diff   = (x_dur.tv_sec  - x_secs) * NSEC;
   x_diff  += (x_dur.tv_nsec - x_nsec);
   /*---(save current)-------------------*/
   x_secs   = x_dur.tv_sec;
   x_nsec   = x_dur.tv_nsec;
   /*---(for timer)----------------------*/
   x_targ   = (myVIKEYS.secs * NSEC) + myVIKEYS.nsec;
   x_rem    = x_targ - x_diff;
   x_used   = x_targ - x_rem;
   x_pct    = (x_used / (float) x_targ) * 100.0;
   /*---(statistics)---------------------*/
   ++x_loop;
   x_total += x_rem;
   s_average = x_rem / x_loop;
   /*> s_average = 0.0;                                                               <*/
   /*---(sleeping)-----------------------*/
   x_dur.tv_sec  = x_rem / NSEC;
   x_dur.tv_nsec = x_rem % NSEC;
   nanosleep      (&x_dur, NULL);
   x_rem   /= 1000;
   x_used  /= 1000;
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current looping info ---------------------------------*/
yvikeys_loop_status     (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(write status)-------------------*/
   sprintf (a_list, "loop, %-5s = %8.6f, %1ds, %10dns, update %-5s = %5.3f, %4d loop, %7lldu %4lldu %6.3f",
         s_delay_info [s_delay].terse, s_delay_info [s_delay].delay, myVIKEYS.secs, myVIKEYS.nsec,
         s_update_info [s_update].terse, s_update_info [s_update].update, myVIKEYS.loops,
         x_rem, x_used, x_pct);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.940.221.74]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
SCALE__unit                (char *a_question, char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     = 0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "SCALE            : question not understood");
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "delay"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "SCALE delay      : %-5s = %8.6f, %1ds, %10dns", s_delay_info [s_delay].terse, s_delay_info [s_delay].delay, myVIKEYS.secs, myVIKEYS.nsec);
   }
   else if (strcmp (a_question, "update"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "SCALE update     : %-5s = %5.3f, %6d loop(s)", s_update_info [s_update].terse, s_update_info [s_update].update, myVIKEYS.loops);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




/*============================----end-of-source---============================*/
