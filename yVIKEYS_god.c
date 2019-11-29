/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"

/*===[[ METIS ]]==============================================================*/
/*
 * metis  dn4··  progress mode should integrate into mapper (either map or progress)
 * metis  dn4··  progress mode should use vert and horz move functions from map mode
 * metis  dw2··  add snap shot command for both progress and god view
 *
 *
 *
 */


/*===[[ SCALE ]]==============================================================*/
#define     MAX_SCALE   100
static float   s_inc      =  1;             /* increment in movement          */
static char    s_base     = ' ';            /* scale prefix for labels        */
static float   s_multi    =  1;             /* increment in movement          */

typedef   struct cSCALE  tSCALE;
struct cSCALE {
   char        type;
   char        terse       [LEN_LABEL];
   char        label       [LEN_LABEL];
   char        desc        [LEN_FULL  ];
   char        power;
   float       unit;
};
static tSCALE s_scale_info [MAX_SCALE] = {
   { 'm' , "T"  , "tera"      , "tera  (1 trillion)"    ,  12 , 1000000000000.0             },
   { 'm' , "GH+", "giga/H+"   , "giga  (300 billion)"   ,  11 ,  320000000000.0             },
   { 'm' , "GH" , "giga/H"    , "giga  (100 billion)"   ,  11 ,  100000000000.0             },
   { 'm' , "GD+", "giga/D+"   , "giga  (30 billion)"    ,  10 ,   32000000000.0             },
   { 'm' , "GD" , "giga/D"    , "giga  (10 billion)"    ,  10 ,   10000000000.0             },
   { 'm' , "G+" , "giga+"     , "giga  (3 billion)"     ,   9 ,    3200000000.0             },
   { 'm' , "G"  , "giga"      , "giga  (1 billion)"     ,   9 ,    1000000000.0             },
   { 'm' , "MH+", "mega/H+"   , "mega  (300 million)"   ,   8 ,     320000000.0             },
   { 'm' , "MH" , "mega/H"    , "mega  (100 million)"   ,   8 ,     100000000.0             },
   { 'm' , "MD+", "mega/D+"   , "mega  (30 million)"    ,   7 ,      32000000.0             },
   { 'm' , "MD" , "mega/D"    , "mega  (10 million)"    ,   7 ,      10000000.0             },
   { 'm' , "M+" , "mega+"     , "mega  (3 million)"     ,   6 ,       3200000.0             },
   { 'm' , "M"  , "mega"      , "mega  (1 million)"     ,   6 ,       1000000.0             },
   { 'm' , "KH+", "kilo/H+"   , "kilo  (300 thousand)"  ,   5 ,        320000.0             },
   { 'm' , "KH" , "kilo/H"    , "kilo  (100 thousand)"  ,   5 ,        100000.0             },
   { 'm' , "KD+", "kilo/D+"   , "kilo  (30 thousand)"   ,   4 ,         32000.0             },
   { 'm' , "KD" , "kilo/D"    , "kilo  (10 thousand)"   ,   4 ,         10000.0             },
   { 'm' , "K+" , "kilo+"     , "kilo  (3 thousand)"    ,   3 ,          3200.0             },
   { 'm' , "K"  , "kilo"      , "kilo  (1 thousand)"    ,   3 ,          1000.0             },
   { 'm' , "H+" , "heka+"     , "hecto (3 hundred)"     ,   2 ,           320.0             },
   { 'm' , "H"  , "heka"      , "hecto (1 hundred)"     ,   2 ,           100.0             },
   { 'm' , "D+" , "deca+"     , "deca  (thirty)"        ,   1 ,            32.0             },
   { 'm' , "D"  , "deca"      , "deca  (ten)"           ,   1 ,            10.0             },
   { 'm' , "+"  , "base+"     , "norm  (three)"         ,   0 ,             3.2             },
   { 'm' , "~"  , "base"      , "norm  (one)"           ,   0 ,             1.0             },
   { 'm' , "-"  , "base+"     , "norm  (third)"         ,  -1 ,             0.32            },
   { 'm' , "d"  , "deci"      , "deci  (tenth)"         ,  -1 ,             0.1             },
   { 'm' , "d-" , "deci-"     , "deci  (thirteth)"      ,  -2 ,             0.032           },
   { 'm' , "c"  , "cent"      , "centi (1 hundredth)"   ,  -2 ,             0.01            },
   { 'm' , "c-" , "cent-"     , "centi (3 hundredth)"   ,  -3 ,             0.0032          },
   { 'm' , "m"  , "mill"      , "milli (1 thousandth)"  ,  -3 ,             0.001           },
   { 'm' , "m-" , "mill-"     , "milli (3 thousandth)"  ,  -4 ,             0.00032         },
   { 'm' , "md" , "mill/d"    , "milli (10 thousandth)" ,  -4 ,             0.0001          },
   { 'm' , "md-", "mill/d-"   , "milli (30 thousandth)" ,  -5 ,             0.000032        },
   { 'm' , "mh" , "mill/h"    , "milli (100 thousandth)",  -5 ,             0.00001         },
   { 'm' , "mh-", "mill/h-"   , "milli (300 thousandth)",  -6 ,             0.0000032       },
   { 'm' , "u"  , "micr"      , "micro (1 millionth)"   ,  -6 ,             0.000001        },
   { 'm' , "u-" , "micr-"     , "micro (3 millionth)"   ,  -7 ,             0.00000032      },
   { 'm' , "ud" , "micr/d"    , "micro (10 millionth)"  ,  -7 ,             0.0000001       },
   { 'm' , "ud-", "micr/d-"   , "micro (30 millionth)"  ,  -8 ,             0.000000032     },
   { 'm' , "uh" , "micr/h"    , "micro (100 millionth)" ,  -8 ,             0.00000001      },
   { 'm' , "uh-", "micr/h-"   , "micro (300 millionth)" ,  -9 ,             0.0000000032    },
   { 'm' , "n"  , "nano"      , "nano  (1 billionth)"   ,  -9 ,             0.000000001     },
   { 'm' , "n-" , "nano-"     , "nano  (3 billionth)"   , -10 ,             0.00000000032   },
   { 'm' , "nd" , "nano/d"    , "nano  (10 billionth)"  , -10 ,             0.0000000001    },
   { 'm' , "nd-", "nano/d-"   , "nano  (30 billionth)"  , -11 ,             0.000000000032  },
   { 'm' , "nh" , "nano/h"    , "nano  (100 billionth)" , -11 ,             0.00000000001   },
   { 'm' , "nh-", "nano/h-"   , "nano  (300 billionth)" , -12 ,             0.0000000000032 },
   { 'm' , "p"  , "pico"      , "pico  (1 trillonth)"   , -12 ,             0.000000000001  },
   { 'm' , "---", "----"      , "end-of-scales"         ,   0 ,             0.0             },
};



/*===[[ SPEED ]]==============================================================*/
#define     MAX_SPEED   50
typedef   struct cSPEED  tSPEED;
struct cSPEED {
   char        terse       [LEN_LABEL];
   char        desc        [LEN_FULL  ];
   float       speed;   
};
static tSPEED s_speed_info [MAX_SPEED] = {
   { "-50.0x"    , "blur"             ,    -50.00 },
   { "-20.0x"    , "super fast"       ,    -20.00 },
   { "-10.0x"    , "very fast"        ,    -10.00 },
   { "-5.00x"    , "faster"           ,     -5.00 },
   { "-2.00x"    , "double"           ,     -2.00 },
   { "-1.00x"    , "normal"           ,     -1.00 },
   { "-0.75x"    , "three-fourth"     ,     -0.75 },
   { "-0.50x"    , "half"             ,     -0.50 },
   { "-0.25x"    , "quarter"          ,     -0.25 },
   { "-0.10x"    , "slow"             ,     -0.10 },
   { "-0.05x"    , "super slow"       ,     -0.05 },
   { "-0.02x"    , "ultra slow"       ,     -0.02 },
   { "+0.00x"    , "stopped"          ,      0.00 },
   { "+0.02x"    , "ultra slow"       ,      0.02 },
   { "+0.05x"    , "super slow"       ,      0.05 },
   { "+0.10x"    , "slow"             ,      0.10 },
   { "+0.25x"    , "quarter"          ,      0.25 },
   { "+0.50x"    , "half"             ,      0.50 },
   { "+0.75x"    , "three-fourth"     ,      0.75 },
   { "+1.00x"    , "normal"           ,      1.00 },
   { "+2.00x"    , "double"           ,      2.00 },
   { "+5.00x"    , "faster"           ,      5.00 },
   { "+10.0x"    , "very fast"        ,     10.00 },
   { "+20.0x"    , "super fast"       ,     20.00 },
   { "+50.0x"    , "blur"             ,     50.00 },
   { "---"       , "end-of-list"      ,      0.00 },
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
   { "every" , "every loop"         ,   0.00     },
   /*---(faster)---------------------------*/
   { "10ms"  , "very fast"          ,   0.01     },
   { "20ms"  , "faster"             ,   0.02     },
   { "50ms"  , "fast"               ,   0.05     },
   /*---(normal)---------------------------*/
   { "100ms" , "normal"             ,   0.10     },
   /*---(slower)---------------------------*/
   { "250ms" , "slow"               ,   0.25     },
   { "500ms" , "slower"             ,   0.50     },
   { "1s"    , "very slow"          ,   1.00     },
   /*---(macro debugging)------------------*/
   { "2s"    , "stop action"        ,   2.00     },
   { "5s"    , "occasional peeks"   ,   5.00     },
   { "100s"  , "flying blind"       , 100.00     },
   /*---(end)------------------------------*/
   { "---"   , "end-of-list"        ,   0.00     },
   /*---(done)-----------------------------*/
};



/*===[[ DELAYS ]]=============================================================*/
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
   { "keys"  , "keyboard"           ,   0.0      },
   /*---(faster)----------------------------------*/
   { "1us"   , "millionth"          ,   0.000001 },
   { "10us"  , "ten millionths"     ,   0.00001  },
   { "100us" , "hundred millionths" ,   0.0001   },
   { "500us" , "500 millionths"     ,   0.0005   },
   /*---(normal)----------------------------------*/
   { "1ms"   , "thousandth"         ,   0.001    },
   { "5ms"   , "five thousandths"   ,   0.005    },
   { "10ms"  , "hundreth"           ,   0.01     },
   /*---(slower)----------------------------------*/
   { "20ms"  , "fifteth"            ,   0.02     },
   { "50ms"  , "twenteth"           ,   0.05     },
   { "100ms" , "tenth"              ,   0.1      },
   /*---(glacial)---------------------------------*/
   { "1s"    , "second"             ,   1.0      },
   /*---(end of list)-----------------------------*/
   { "---"   , "end-of-list"        ,   0.0      },
   /*---(done)------------------------------------*/
};



/*====================------------------------------------====================*/
/*===----                         shared functions                     ----===*/
/*====================------------------------------------====================*/
static void      o___SHARED__________________o (void) {;}

char
yvikeys__loop_calc   (void)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_base      =  0.0;
   /*---(initialize)---------------------*/
   myVIKEYS.secs  = 0;
   myVIKEYS.nsec  = 0;
   myVIKEYS.loops = 1;
   /*---(keyboard only)------------------*/
   if      (myVIKEYS.delay  == 0.0 && myVIKEYS.update == 0.0) {
      x_base = myVIKEYS.delay;
   }
   /*---(delay, no update)---------------*/
   else if (myVIKEYS.delay  >  0.0 && myVIKEYS.update == 0.0) {
      x_base = myVIKEYS.delay;
   }
   /*---(update, no delay)---------------*/
   else if (myVIKEYS.update  > 0.0 && myVIKEYS.delay  == 0.0) {
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
   /*---(flag blocking)------------------*/
   if (x_base == 0.0)        myVIKEYS.blocking = 'y';
   else                      myVIKEYS.blocking = '-';
   /*---(progress advance)---------------*/
   myVIKEYS.p_inc  = s_scale_info [myVIKEYS.p_scale].unit / 10.0;
   /*> printf ("x_base   = %f\n", x_base);                                            <*/
   x_base         *= myVIKEYS.loops;
   /*> printf ("x_base   = %f\n", x_base);                                            <*/
   if (x_base == 0.0) {
      myVIKEYS.p_adv  = 0.0;
   } else {
      myVIKEYS.p_adv  = (float) (s_scale_info [myVIKEYS.p_scale].unit * s_speed_info [myVIKEYS.p_speed].speed);
      /*> printf ("p_adv    = %lf\n", myVIKEYS.p_adv);                                <*/
      myVIKEYS.p_adv *= x_base;
      /*> printf ("p_adv    = %lf\n", myVIKEYS.p_adv);                                <*/
      /*> printf ("p_adv    = %28.14lf\n", myVIKEYS.p_adv);                           <*/
   }
   /*---(redraw)-------------------------*/
   myVIKEYS.p_redraw = 'y';
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_prog_redraw   (void)
{
   char        rc          =    0;
   if (myVIKEYS.p_redraw == 'y')  rc = 1;
   myVIKEYS.p_redraw = '-';
   return rc;
}

char
yVIKEYS_prog_script     (float a_beg, float a_end, int a_lines)
{
   myVIKEYS.p_beg   = a_beg;
   DEBUG_USER   yLOG_double  ("p_beg"     , myVIKEYS.p_beg);
   myVIKEYS.p_end   = a_end;
   DEBUG_USER   yLOG_double  ("p_end"     , myVIKEYS.p_end);
   myVIKEYS.p_len   = a_end - a_beg;
   DEBUG_USER   yLOG_double  ("p_len"     , myVIKEYS.p_len);
   myVIKEYS.p_cur   = a_beg;
   DEBUG_USER   yLOG_double  ("p_cur"     , myVIKEYS.p_cur);
   myVIKEYS.p_lines = a_lines;
   DEBUG_USER   yLOG_value   ("p_lines"   , myVIKEYS.p_lines);
   return 0;
}

char
yVIKEYS_prog_cur        (char *a_pos, float *a_sec, float *a_scale, float *a_inc, int *a_line)
{
   if (a_pos   != NULL)  *a_pos   = myVIKEYS.p_pos;
   if (a_sec   != NULL)  *a_sec   = myVIKEYS.p_cur;
   if (a_scale != NULL)  *a_scale = s_scale_info [myVIKEYS.p_scale].unit;
   if (a_inc   != NULL)  *a_inc   = myVIKEYS.p_inc;
   if (a_line  != NULL)  *a_line  = myVIKEYS.p_line;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         mode logic                           ----===*/
/*====================------------------------------------====================*/
static void      o___MODES___________________o (void) {;}

char         /*--> process keystrokes in normal mode -----[--------[--------]-*/
GOD_mode           (char a_major, char a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   char        x_minors    [LEN_FULL]  = "ypdx";
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
      case '\\'     :
         DEBUG_USER   yLOG_note    ("entering menu sub-mode");
         MODE_enter  (SMOD_MENUS   );
         yvikeys_menu_start  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
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
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_PROGRESS)) {
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
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("enter/escape, leave progress mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      DEBUG_USER   yLOG_note    ("review single keys");
      /*---(modes)-----------------------*/
      switch (a_minor) {
      case ':'      :
         DEBUG_USER   yLOG_note    ("start the command mode");
         SOURCE_start   (":");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '\\'     :
         DEBUG_USER   yLOG_note    ("entering menu sub-mode");
         MODE_enter  (SMOD_MENUS   );
         yvikeys_menu_start  ();
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
      if (strchr ("_KkjJ~", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("handle the horizontal keys");
         DEBUG_USER   yLOG_value   ("p_line"    , myVIKEYS.p_line);
         DEBUG_USER   yLOG_value   ("p_nline"   , myVIKEYS.p_lines);
         switch (a_minor) {
         case '_' :  myVIKEYS.p_line   = 0;                     break;
         case 'K' :  myVIKEYS.p_line  -= 5;                     break;
         case 'k' :  myVIKEYS.p_line  -= 1;                     break;
         case 'j' :  myVIKEYS.p_line  += 1;                     break;
         case 'J' :  myVIKEYS.p_line  += 5;                     break;
         case '~' :  myVIKEYS.p_line   = myVIKEYS.p_lines;      break;
         }
         if (myVIKEYS.p_line < 0                )  myVIKEYS.p_line = 0;
         if (myVIKEYS.p_line >= myVIKEYS.p_lines)  myVIKEYS.p_line = myVIKEYS.p_lines - 1;
         DEBUG_USER   yLOG_value   ("p_line"    , myVIKEYS.p_line);
      }
      /*---(zoom and retreat)------------*/
      switch (a_minor) {
      case 'i':
         yvikeys_scale     (MODE_PROGRESS, "<");
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = 'y';
         break;
      case 'o':
         yvikeys_scale     (MODE_PROGRESS, ">");
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = 'y';
         break;
      }
      /*---(play and stop)---------------*/
      switch (a_minor) {
      case '<':
         yvikeys_speed     (MODE_PROGRESS, "<");
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = 'y';
         break;
      case '>':
         yvikeys_speed     (MODE_PROGRESS, ">");
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = 'y';
         break;
      case ',':
         myVIKEYS.p_play = 'y';
         if (myVIKEYS.p_adv == 0.0)  yvikeys_speed (MODE_PROGRESS, "+1.00x");
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = 'y';
         break;
      case '.':
         myVIKEYS.p_play   = '-';
         myVIKEYS.p_redraw = '-';
         yvikeys__loop_calc ();
         break;
      }
      /*---(horizontal movement)---------*/
      if (strchr ("0HhlL$", a_minor) != 0) {
         DEBUG_USER   yLOG_double  ("p_beg"     , myVIKEYS.p_beg);
         DEBUG_USER   yLOG_double  ("p_end"     , myVIKEYS.p_end);
         DEBUG_USER   yLOG_double  ("p_inc"     , myVIKEYS.p_inc);
         DEBUG_USER   yLOG_double  ("p_cur"     , myVIKEYS.p_cur);
         switch (a_minor) {
         case '0' :  myVIKEYS.p_cur  = myVIKEYS.p_beg;        break;
         case 'H' :  myVIKEYS.p_cur -= myVIKEYS.p_inc * 5.0;  break;
         case 'h' :  myVIKEYS.p_cur -= myVIKEYS.p_inc      ;  break;
         case 'l' :  myVIKEYS.p_cur += myVIKEYS.p_inc      ;  break;
         case 'L' :  myVIKEYS.p_cur += myVIKEYS.p_inc * 5.0;  break;
         case '$' :  myVIKEYS.p_cur  = myVIKEYS.p_end;        break;
         }
         DEBUG_USER   yLOG_double  ("p_cur"     , myVIKEYS.p_cur);
      }
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
      if (strchr ("0shcle$", a_minor) != NULL) {
         myVIKEYS.p_pos = a_minor;
         yvikeys__loop_calc ();
         myVIKEYS.p_redraw = '-';
      } else {
         return -1;
      }
   }
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       progress scale                         ----===*/
/*====================------------------------------------====================*/
static void      o___SCALE___________________o (void) {;}

char
yvikeys_scale           (char a_mode, char *a_scale)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_scale     =    0;
   char        rc          =    0;
   char        x_prefix    =  ' ';
   int         i           =    0;
   char        x_index     =   -1;
   char        x_max       =   -1;
   char        x_def       =   -1;
   /*---(types)--------------------------*/
   /*> printf ("beg : myVIKEYS.p_scale = %d\n", myVIKEYS.p_scale);                    <*/
   /*> printf ("nxt : a_mode  = %c\n", a_mode);                                       <*/
   /*> printf ("nxt : a_scale = %s\n", a_scale);                                      <*/
   switch (a_mode) {
   case MODE_PROGRESS : x_scale = myVIKEYS.p_scale;   break;
   }
   /*---(assign prefix)------------------*/
   if (a_scale == NULL)    x_prefix = 0;
   else                    x_prefix = a_scale [0];
   /*---(determine max)------------------*/
   for (i = 0; i < MAX_UPDATE; ++i) {
      if (strcmp (s_scale_info [i].terse, "---"   ) == 0)  break;
      if (strcmp (s_scale_info [i].terse, "~"     ) == 0)  x_def = i;
      ++x_max;
   }
   /*> printf ("    : x_def = %d, x_max = %d\n", x_def, x_max);                       <*/
   /*---(find entry in table)------------*/
   switch (x_prefix) {
   case  0  :
      x_index = x_scale;
      rc      = -1;
      break;
   case '0' :
      x_index = x_def;
      break;
   case '=' :
      x_index = x_scale;
      break;
   case '<' :
      if (x_scale <  x_max)  x_index = ++x_scale;
      else {
         x_index = x_max;
         rc      = -3;
      }
      break;
   case '>' :
      if (x_scale >  0    )  x_index = --x_scale;
      else {
         x_index = 0;
         rc      = -4;
      }
      break;
   default  :
      for (i = 0; i <= x_max; ++i) {
         if (strcmp (s_scale_info [i].terse, a_scale) != 0)  continue;
         x_index = i;
         break;
      }
      if (x_index == -1) {
         x_index = x_scale;
         rc      = -2;
      }
      break;
   }
   /*---(set key values)-----------------*/
   switch (a_mode) {
   case MODE_PROGRESS : myVIKEYS.p_scale = x_index;   break;
   }
   /*> printf ("end : myVIKEYS.p_scale = %d\n", myVIKEYS.p_scale);                    <*/
   /*---(update looping)-----------------*/
   yvikeys__loop_calc   ();
   if (x_scale == x_index)  myVIKEYS.p_redraw = '-';
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys_scale_prog      (char *a_scale)
{
   return yvikeys_scale  (MODE_PROGRESS, a_scale);
}

char
yVIKEYS_scale_brief (char   *a_text)
{
   if (a_text != NULL)  snprintf (a_text, LEN_FULL, "%s", s_scale_info [myVIKEYS.p_scale].label);
   return 0;
}

char
yVIKEYS_scale_desc (char   *a_text)
{
   if (a_text != NULL)  snprintf (a_text, LEN_FULL, "%s %s", s_scale_info [myVIKEYS.p_scale].terse, s_scale_info [myVIKEYS.p_scale].desc);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       progress speed                         ----===*/
/*====================------------------------------------====================*/
static void      o___SPEED___________________o (void) {;}

char
yvikeys_speed           (char a_mode, char *a_speed)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_speed     =    0;
   char        rc          =    0;
   char        x_prefix    =  ' ';
   int         i           =    0;
   char        x_index     =   -1;
   char        x_max       =   -1;
   char        x_def       =   -1;
   /*---(types)--------------------------*/
   /*> printf ("beg : myVIKEYS.p_speed = %d\n", myVIKEYS.p_speed);                    <*/
   /*> printf ("nxt : a_mode  = %c\n", a_mode);                                       <*/
   /*> printf ("nxt : a_speed = %s\n", a_speed);                                      <*/
   switch (a_mode) {
   case MODE_PROGRESS : x_speed = myVIKEYS.p_speed;   break;
   }
   /*---(assign prefix)------------------*/
   if (a_speed == NULL)    x_prefix = 0;
   else                    x_prefix = a_speed [0];
   /*---(determine max)------------------*/
   for (i = 0; i < MAX_UPDATE; ++i) {
      if (strcmp (s_speed_info [i].terse, "---"   ) == 0)  break;
      if (strcmp (s_speed_info [i].terse, "+0.00x") == 0)  x_def = i;
      ++x_max;
   }
   /*> printf ("    : x_def = %d, x_max = %d\n", x_def, x_max);                       <*/
   /*---(find entry in table)------------*/
   switch (x_prefix) {
   case  0  :
      x_index = x_speed;
      rc      = -1;
      break;
   case '0' :
      x_index = x_def;
      break;
   case '=' :
      x_index = x_speed;
      break;
   case '>' :
      if (x_speed <  x_max)  x_index = ++x_speed;
      else {
         x_index = x_max;
         rc      = -3;
      }
      break;
   case '<' :
      if (x_speed >  0    )  x_index = --x_speed;
      else {
         x_index = 0;
         rc      = -4;
      }
      break;
   default  :
      for (i = 0; i <= x_max; ++i) {
         if (strcmp (s_speed_info [i].terse, a_speed) != 0)  continue;
         x_index = i;
         break;
      }
      if (x_index == -1) {
         x_index = x_speed;
         rc      = -2;
      }
      break;
   }
   /*---(set key values)-----------------*/
   switch (a_mode) {
   case MODE_PROGRESS : myVIKEYS.p_speed = x_index;   break;
   }
   /*> printf ("end : myVIKEYS.p_speed = %d\n", myVIKEYS.p_speed);                    <*/
   /*---(update looping)-----------------*/
   yvikeys__loop_calc   ();
   if (x_speed == x_index)  myVIKEYS.p_redraw = '-';
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys_speed_prog      (char *a_speed)
{
   return yvikeys_speed  (MODE_PROGRESS, a_speed);
}

char
yVIKEYS_speed_brief (char *a_text)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (a_text == NULL) {
      return rce;
   }
   --rce;  if (myVIKEYS.p_speed < 0) {
      return rce;
   }
   /*---(create text line)---------------*/
   snprintf (a_text, LEN_FULL, "%s", s_speed_info [myVIKEYS.p_speed].terse);
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
   --rce;  if (myVIKEYS.p_speed < 0) {
      return rce;
   }
   /*---(create text line)---------------*/
   snprintf (a_text, LEN_FULL, "%s (%s)", s_speed_info [myVIKEYS.p_speed].terse, s_speed_info [myVIKEYS.p_speed].desc);
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
   /*---(delay/updapte)------------------*/
   s_delay             = 0;
   s_update            = 0;
   myVIKEYS.macro_skip = 1;
   myVIKEYS.delay      = 0.0;
   myVIKEYS.secs       = 0;
   myVIKEYS.nsec       = 0;
   myVIKEYS.update     = 0.0;
   myVIKEYS.loops      = 1;
   myVIKEYS.blocking   = ' ';
   yvikeys_loop_delay  ("");
   yvikeys_loop_update ("");
   /*---(progress)-----------------------*/
   myVIKEYS.p_play     = '-';
   myVIKEYS.p_pos      = 'c';
   myVIKEYS.p_scale    = 0;
   myVIKEYS.p_speed    = 0;
   myVIKEYS.p_adv      = 0.0;
   myVIKEYS.p_inc      = 0.0;
   yvikeys_scale       (MODE_PROGRESS, "0");
   yvikeys_speed       (MODE_PROGRESS, "0");
   /*---(commands)-----------------------*/
   yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "delay"       , ""    , "ss"   , yvikeys_loop_set     , "adjust the main loop wait and screen update timings"         );
   yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "p_scale"     , ""    , "s"    , yvikeys_scale_prog   , "adjust the progress scale"                                   );
   yVIKEYS_cmds_add (YVIKEYS_M_CONFIG, "p_speed"     , ""    , "s"    , yvikeys_speed_prog   , "adjust the progress speed"                                   );
   /*---(complete)-----------------------*/
   return 0;
}

int
yvikeys_loop_getch      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_ch        =    0;
   XKeyEvent  *key_event;
   int         the_bytes;
   char        the_keys    [5];
   /*---(opengl)-------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      if (myVIKEYS.blocking == 'y') {
         XNextEvent(YX_DISP, &YX_EVNT);
      }
      else {
         if (XPending(YX_DISP))   XNextEvent(YX_DISP, &YX_EVNT);
         else                  return 0;
      }
      switch(YX_EVNT.type) {
      case KeyPress:
         key_event  = (XKeyEvent *) &YX_EVNT;
         the_bytes = XLookupString((XKeyEvent *) &YX_EVNT, the_keys, 5, NULL, NULL);
         if (the_bytes < 1) break;
         /*---(handle)----------------*/
         x_ch  = the_keys [0];
         break;
      }
   }
   /*---(ncurses)------------------------*/
   else if (myVIKEYS.env == YVIKEYS_CURSES) {
      if (myVIKEYS.blocking == 'y')   nodelay  (stdscr, FALSE);
      else                            nodelay  (stdscr, TRUE );
      x_ch = getch ();
   }
   /*---(complete)-----------------------*/
   return x_ch;
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
   case '0' :
      x_index = 0;
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
         x_index = s_update;
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
   case '0' :
      x_index = 0;
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

static char   s_save_delay   [LEN_LABEL] = "";
static char   s_save_update  [LEN_LABEL] = "";

char
yvikeys_loop_set        (char *a_delay, char *a_update)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(set values)---------------------*/
   rc = yvikeys_loop_delay   (a_delay);
   if (rc >= 0)  strlcpy (s_save_delay , a_delay , LEN_LABEL);
   rc = yvikeys_loop_update  (a_update);
   if (rc >= 0)  strlcpy (s_save_update, a_update, LEN_LABEL);
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys_loop_normal     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(return to normal)---------------*/
   DEBUG_LOOP   yLOG_senter  (__FUNCTION__);
   DEBUG_LOOP   yLOG_schar   (yvikeys_macro_emode ());
   DEBUG_LOOP   yLOG_snote   (s_save_delay);
   rc = yvikeys_loop_delay   (s_save_delay);
   myVIKEYS.macro_skip = 1;
   DEBUG_LOOP   yLOG_sint    (myVIKEYS.macro_skip);
   DEBUG_LOOP   yLOG_snote   (s_save_update);
   rc = yvikeys_loop_update  (s_save_update);
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
yvikeys_loop_macro      (char a_delay, char a_update)
{
   /*---(no change for playback)---------*/
   IF_MACRO_PLAYBACK {
      yvikeys_loop_normal ();
      return 0;
   }
   /*---(change to macro speed)----------*/
   DEBUG_LOOP   yLOG_senter  (__FUNCTION__);
   DEBUG_LOOP   yLOG_schar   (yvikeys_macro_emode ());
   DEBUG_LOOP   yLOG_sint    (a_delay);
   DEBUG_LOOP   yLOG_schar   (a_delay);
   /*---(run mode)-----------------------*/
   IF_MACRO_RUN   a_delay = '0';
   /*---(delay mode)---------------------*/
   switch (a_delay) {
   case '0' :  yvikeys_loop_delay ("1us"  );   myVIKEYS.macro_skip =  1;   break;
   case '1' :  yvikeys_loop_delay ("100us");   myVIKEYS.macro_skip =  1;   break;
   case '2' :  yvikeys_loop_delay ("1ms"  );   myVIKEYS.macro_skip =  1;   break;
   case '3' :  yvikeys_loop_delay ("10ms" );   myVIKEYS.macro_skip =  1;   break;
   case '4' :  yvikeys_loop_delay ("10ms" );   myVIKEYS.macro_skip =  5;   break;
   case '5' :  yvikeys_loop_delay ("100ms");   myVIKEYS.macro_skip =  1;   break;
   case '6' :  yvikeys_loop_delay ("100ms");   myVIKEYS.macro_skip =  5;   break;
   case '7' :  yvikeys_loop_delay ("100ms");   myVIKEYS.macro_skip = 10;   break;
   case '8' :  yvikeys_loop_delay ("100ms");   myVIKEYS.macro_skip = 20;   break;
   case '9' :  yvikeys_loop_delay ("100ms");   myVIKEYS.macro_skip = 30;   break;
   }
   DEBUG_LOOP   yLOG_sint    (myVIKEYS.macro_skip);
   /*---(change update basis)------------*/
   DEBUG_LOOP   yLOG_sint    (a_update);
   DEBUG_LOOP   yLOG_schar   (a_update);
   switch (a_update) {
   case MACRO_NORMAL :  yvikeys_loop_update ("100ms");  break;
   case MACRO_SLOWER :  yvikeys_loop_update ("500ms");  break;
   case MACRO_BLINKS :  yvikeys_loop_update ("2s");     break;
   case MACRO_PEEKS  :  yvikeys_loop_update ("5s");     break;
   case MACRO_BLIND  :  yvikeys_loop_update ("100s");   break;
   }
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

static long long s_loop_targ   = 0;
static long long s_loop_used   = 0;
static long long s_loop_rem    = 0;

static long long s_loop_slept  = 0;
static long long s_loop_miss   = 0;

static long long s_loop_prev   = 0;
static long long s_loop_beg    = 0;
static long long s_loop_end    = 0;
static long long s_loop_dur    = 0;


static long long x_used   = 0;
static float     x_pct    = 0.0;

static long      s_avg    = 0.0;
static long      s_draw   = 0.0;
static long      s_keys   = 0.0;
static long      s_idle   = 0.0;

char
yvikeys_loop_beg        (void)
{
   /*---(locals)-----------+-----+-----+-*/
   tTSPEC      x_dur;
   /*---(get beginning time)-------------*/
   clock_gettime  (CLOCK_MONOTONIC_RAW, &x_dur);
   s_loop_beg   = x_dur.tv_sec * NSEC;
   s_loop_beg  += x_dur.tv_nsec;
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_loop_prog       (void)
{
   if (myVIKEYS.p_play == 'y') {
      myVIKEYS.p_cur += myVIKEYS.p_adv;
   }
   if (myVIKEYS.p_cur <  myVIKEYS.p_beg) {
      myVIKEYS.p_cur  = myVIKEYS.p_beg;
      myVIKEYS.p_play = '-';
   }
   if (myVIKEYS.p_cur >  myVIKEYS.p_end) {
      myVIKEYS.p_cur  = myVIKEYS.p_end;
      myVIKEYS.p_play = '-';
   }
   return 0;
}

char
yvikeys_loop_sleep      (uchar a_key, char a_draw)
{
   /*---(locals)-----------+-----+-----+-*/
   static long x_secs      =    0;
   static long x_nsec      =    0;
   tTSPEC      x_dur;
   static int  x_loops     =    0;
   static int  x_draws     =    0;
   static int  x_keys      =    0;
   static int  x_idles     =    0;
   static long long x_total =    0;
   static long long x_draw  =    0;
   static long long x_key   =    0;
   static long long x_idle  =    0;
   char        x_type       =  '-';
   /*---(get ending time)----------------*/
   clock_gettime  (CLOCK_MONOTONIC_RAW, &x_dur);
   s_loop_end   = x_dur.tv_sec * NSEC;
   s_loop_end  += x_dur.tv_nsec;
   /*---(classify)-----------------------*/
   if      (a_draw == 'y')  x_type = 'g';
   else if (a_key  !=  0 )  x_type = 'k';
   else                     x_type = '-';
   /*---(calc sleep diffs)---------------*/
   /*> s_loop_slept = s_loop_beg - s_loop_prev;                                       <*/
   /*> s_loop_miss  = s_loop_beg - s_loop_prev;                                       <*/
   /*---(calc run diffs)-----------------*/
   s_loop_targ  = (myVIKEYS.secs * NSEC) + myVIKEYS.nsec;
   s_loop_used  = s_loop_end  - s_loop_beg;
   s_loop_rem   = s_loop_targ - s_loop_used;
   if (myVIKEYS.blocking == 'y')  s_loop_rem = 0;
   /*> s_loop_prev  = s_loop_end;                                                     <*/
   /*---(for timer)----------------------*/
   x_pct    = (s_loop_used / (float) s_loop_targ) * 100.0;
   /*---(statistics)---------------------*/
   ++x_loops;
   x_total += s_loop_used;
   s_avg    = x_total / (float) x_loops;
   switch (x_type) {
   case 'g' :
      ++x_draws;
      x_draw += s_loop_used;
      s_draw  = x_draw / (float) x_draws;
      break;
   case 'k' :
      ++x_keys;
      x_key  += s_loop_used;
      s_keys  = x_key  / (float) x_keys;
      break;
   default  :
      ++x_idles;
      x_idle += s_loop_used;
      s_idle  = x_idle / (float) x_idles;
      break;
   }
   /*> s_avg     = x_total / x_loops;                                                 <*/
   /*---(sleeping)-----------------------*/
   if (myVIKEYS.blocking != 'y') {
      x_dur.tv_sec  = s_loop_rem / NSEC;
      x_dur.tv_nsec = s_loop_rem % NSEC;
      nanosleep      (&x_dur, NULL);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current looping info ---------------------------------*/
yvikeys_prog_status     (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(write status)-------------------*/
   sprintf (a_list, "progress, play=%c, %8.6fa, %8.6fi", myVIKEYS.p_play, myVIKEYS.p_adv, myVIKEYS.p_inc);
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current looping info ---------------------------------*/
yvikeys_delay_status    (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(write status)-------------------*/
   sprintf (a_list, "delay, %-5s = %8.6f, %1ds, %10dns, update %-5s = %5.3fs, %4d loop(s), %c",
         s_delay_info [s_delay].terse, s_delay_info [s_delay].delay, myVIKEYS.secs, myVIKEYS.nsec,
         s_update_info [s_update].terse, s_update_info [s_update].update, myVIKEYS.loops, myVIKEYS.blocking);
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current looping info ---------------------------------*/
yvikeys_main_status     (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(write status)-------------------*/
   sprintf (a_list, "main, %8lldt, %8lldr, %8lldu, %6ldt, %6ldd, %6ldk, %6ldi",
         s_loop_targ, s_loop_rem, s_loop_used, s_avg, s_draw, s_keys, s_idle);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ light  [us.940.221.74]*/ /*-[02.0000.00#.#]-*/ /*-[--.---.---.--]-*/
GOD__unit                  (char *a_question, char a_mark)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     = 0;
   /*---(preprare)-----------------------*/
   strcpy  (yVIKEYS__unit_answer, "SCALE            : question not understood");
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "delay"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "LOOP delay       : %-5s = %8.6f, %1ds, %10dns   %c", s_delay_info [s_delay].terse, s_delay_info [s_delay].delay, myVIKEYS.secs, myVIKEYS.nsec, myVIKEYS.blocking);
   }
   else if (strcmp (a_question, "update"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "LOOP update      : %-5s = %5.3f, %6d loop(s)", s_update_info [s_update].terse, s_update_info [s_update].update, myVIKEYS.loops);
   }
   else if (strcmp (a_question, "p_scale"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "PROG scale       : %-5s = %.3e", s_scale_info [myVIKEYS.p_scale].terse, s_scale_info [myVIKEYS.p_scale].unit);
   }
   else if (strcmp (a_question, "p_speed"     )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "PROG speed       : %-7s = %8.2f", s_speed_info [myVIKEYS.p_speed].terse, s_speed_info [myVIKEYS.p_speed].speed);
   }
   else if (strcmp (a_question, "p_play"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "PROG play        : %c, %10.6lfa, %10.6lfi", myVIKEYS.p_play, myVIKEYS.p_adv, myVIKEYS.p_inc);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}




/*============================----end-of-source---============================*/
