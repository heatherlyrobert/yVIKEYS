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
#define     DEBUG_VIKEYS   if (its.debug == 'y')




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



/*> char         /+--> process keystrokes in progress mode ---[--------[--------]-+/                                  <* 
 *> yVIKEYS_bounds     (double a_left, double a_right, double a_top, double a_bottom, double a_close, double a_far)   <* 
 *> {                                                                                                                 <* 
 *> }                                                                                                                 <*/

/*> char         /+--> process keystrokes in progress mode ---[--------[--------]-+/                            <* 
 *> yVIKEYS_basics     (char a_major, char a_minor, double *a_base, double a_inc, double a_min, double a_max)   <* 
 *> {                                                                                                           <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                 <* 
 *>    char        rce         = -10;      /+ return code for errors              +/                            <* 
 *>    char        x_minors    [LEN_STR]  = "0LlhH$_KkjJG";                                                     <* 
 *>    /+---(header)-------------------------+/                                                                 <* 
 *>    DEBUG_USER   yLOG_enter   (__FUNCTION__);                                                                <* 
 *>    DEBUG_USER   yLOG_char    ("a_major"   , a_major);                                                       <* 
 *>    DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);                                                       <* 
 *>    DEBUG_USER   yLOG_double  ("a_base"    , *a_base);                                                       <* 
 *>    DEBUG_USER   yLOG_double  ("a_inc"     , a_inc);                                                         <* 
 *>    DEBUG_USER   yLOG_double  ("a_min"     , a_min);                                                         <* 
 *>    DEBUG_USER   yLOG_double  ("a_max"     , a_max);                                                         <* 
 *>    /+---(defense)------------------------+/                                                                 <* 
 *>    --rce;  if (a_major != ' ') {                                                                            <* 
 *>       DEBUG_USER   yLOG_note    ("not a valid a_major prefix key");                                         <* 
 *>       DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                             <* 
 *>       return rce;                                                                                           <* 
 *>    }                                                                                                        <* 
 *>    --rce;  if (strchr (x_minors, a_minor) == 0) {                                                           <* 
 *>       DEBUG_USER   yLOG_note    ("not a valid a_minor movement key");                                       <* 
 *>       DEBUG_USER   yLOG_exit    (__FUNCTION__);                                                             <* 
 *>       return rce;                                                                                           <* 
 *>    }                                                                                                        <* 
 *>                                                                                                             <* 
 *>    /+---(basic horizontal)---------------+/                                                                 <* 
 *>    switch (a_minor) {                                                                                       <* 
 *>    case '0'      : MOVE_horz ('0');       break;                                                            <* 
 *>    case 'H'      : MOVE_horz ('H');       break;                                                            <* 
 *>    case 'h'      : MOVE_horz ('h');       break;                                                            <* 
 *>    case 'l'      : MOVE_horz ('l');       break;                                                            <* 
 *>    case 'L'      : MOVE_horz ('L');       break;                                                            <* 
 *>    case '$'      : MOVE_horz ('$');       break;                                                            <* 
 *>    }                                                                                                        <* 
 *>    /+---(basic vertical)-----------------+/                                                                 <* 
 *>    switch (a_minor) {                                                                                       <* 
 *>    case '_'      : MOVE_vert ('_');       break;                                                            <* 
 *>    case 'K'      : MOVE_vert ('K');       break;                                                            <* 
 *>    case 'k'      : MOVE_vert ('k');       break;                                                            <* 
 *>    case 'j'      : MOVE_vert ('j');       break;                                                            <* 
 *>    case 'J'      : MOVE_vert ('J');       break;                                                            <* 
 *>    case 'G'      : MOVE_vert ('G');       break;                                                            <* 
 *>    }                                                                                                        <* 
 *>    /+---(special)------------------------+/                                                                 <* 
 *>    switch (a_minor) {                                                                                       <* 
 *>    case 'r'      : break;                                                                                   <* 
 *>    }                                                                                                        <* 
 *>                                                                                                             <* 
 *>    switch (a_minor) {                                                                                       <* 
 *>    case '0': my_pos  =  0.0;               break;                                                           <* 
 *>    case 'L': my_pos +=  my.p_inc * 5;      break;                                                           <* 
 *>    case 'l': my_pos +=  my.p_inc;          break;                                                           <* 
 *>    case 'h': my_pos -=  my.p_inc;          break;                                                           <* 
 *>    case 'H': my_pos -=  my.p_inc * 5;      break;                                                           <* 
 *>    case '$': my_pos  =  my.p_len;          break;                                                           <* 
 *>    }                                                                                                        <* 
 *> }                                                                                                           <*/

/*============================----end-of-source---============================*/
