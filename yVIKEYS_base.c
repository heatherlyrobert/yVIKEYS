/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"





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



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char          yVIKEYS__unit_answer [LEN_STR];

char       /*----: set up program urgents/debugging --------------------------*/
yVIKEYS__unit_quiet    (void)
{
   ySTR_debug ('-');
   its.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_QUIET);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yVIKEYS__unit_loud     (void)
{
   ySTR_debug ('y');
   its.logger = yLOG_begin ("yVIKEYS" , yLOG_SYSTEM, yLOG_NOISE);
   yURG_name  ("user"         , YURG_ON);
   yURG_name  ("yvikeys"      , YURG_ON);
   yURG_name  ("yvikeys_mode" , YURG_ON);
   yURG_name  ("yvikeys_keys" , YURG_ON);
   yURG_name  ("yvikeys_scale", YURG_ON);
   DEBUG_YVIKEYS yLOG_info     ("yVIKEYS"    , yVIKEYS_version   ());
   return 0;
}

char       /*----: stop logging ----------------------------------------------*/
yVIKEYS__unit_end      (void)
{
   yLOG_end     ();
   return 0;
}




