/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



char    (*s_mapper) (char a_type);



char   g_coord    = YVIKEYS_RIGHT;


char   g_vsimple   [LEN_DESC ]   = "_KkjJ~";
char   g_vgoto     [LEN_DESC ]   = "TKtkmjbJB";
char   g_vscroll   [LEN_DESC ]   = "  tkmjb  ";
char   g_hsimple   [LEN_DESC ]   = "0HhlL$";
char   g_hgoto     [LEN_DESC ]   = "SHshcleLE";
char   g_hscroll   [LEN_DESC ]   = "  shcle  ";
char   g_hword     [LEN_DESC ]   = "wbeWBE";
char   g_multimap  [LEN_DESC ]   = "cegz";
char   g_multisrc  [LEN_DESC ]   = "cgz";
char   g_repeat    [LEN_DESC ]   = "123456789";




#define     MAX_VISU       100
#define     VISU_NOT       '-'
#define     VISU_YES       'y'
static char  s_live = VISU_NOT;         /* is the selection active: 0=no, 1=yes          */

typedef     struct cVISU    tVISU;
struct cVISU {
   char        active;
   int         x_root;
   int         y_root;
   int         z_root;
   int         x_beg;
   int         y_beg;
   int         z_beg;
   int         x_end;
   int         y_end;
   int         z_end;
   /*---(end)----------------------------*/
};
static tVISU  s_visu;
static tVISU  s_visu_info [MAX_VISU];
static char   S_VISU_LIST [MAX_VISU] = "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static char s_visu_head   = '-';
static char s_visu_curr   = '-';
static char s_visu_tail   = '-';

#define     VISU_ALL      '*'
#define     VISU_LOWER    'a'
#define     VISU_UPPER    'A'
#define     VISU_SYSTEM   '0'



/*====================------------------------------------====================*/
/*===----                       visual support                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_SUPPORT____o () { return; }

char         /*-> check mark validity ----------------[ leaf   [ge.833.144.30]*/ /*-[01.0000.0A4.F]-*/ /*-[--.---.---.--]-*/
VISU__valid           (char a_visu)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *x_visu      = NULL;
   int         x_index     =   0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   DEBUG_VISU   yLOG_sint    (a_visu);
   /*---(defenses)-----------------------*/
   --rce;  if (a_visu == '\0') {
      DEBUG_VISU   yLOG_snote   ("null is invalid");
      DEBUG_VISU   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   x_visu = strchr (S_VISU_LIST, a_visu);
   DEBUG_VISU   yLOG_spoint  (x_visu);
   --rce;  if (x_visu == NULL) {
      DEBUG_VISU   yLOG_snote   ("not valid");
      DEBUG_VISU   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(convert to index)---------------*/
   x_index = (int) (x_visu - S_VISU_LIST);
   DEBUG_VISU   yLOG_sint    (x_index);
   /*---(check limits)-------------------*/
   x_len = strlen (S_VISU_LIST);
   --rce;  if (x_index >= x_len) {
      DEBUG_VISU   yLOG_snote   ("over max");
      DEBUG_VISU   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return x_index;
}



/*====================------------------------------------====================*/
/*===----                      visual sequence                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_SEQ________o () { return; }

char         /*-> tbd --------------------------------[ leaf   [gz.640.021.20]*/ /*-[01.0000.044.!]-*/ /*-[--.---.---.--]-*/
VISU__range         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(prepare)------------------------*/
   x_len = strlen (S_VISU_LIST);
   DEBUG_VISU   yLOG_sint    (x_len);
   /*---(find next)----------------------*/
   s_visu_head = '-';
   for (i = 0; i < x_len; ++i) {
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_head = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_snote   ("HEAD");
      break;
   }
   /*---(find last)----------------------*/
   s_visu_tail = '-';
   for (i = x_len - 1; i >  0; --i) {
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_tail = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_snote   ("TAIL");
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.043.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
VISU__prev          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("visu_curr" , s_visu_curr);
   x_index = VISU__valid (s_visu_curr);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find previous)------------------*/
   DEBUG_VISU   yLOG_note    ("search for previous mark");
   for (i = x_index - 1; i > 0; --i) {
      if (s_visu_info [i].active == VISU_NOT )  continue;
      DEBUG_VISU   yLOG_value   ("found"     , i);
      x_mark = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_VISU   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

char         /*-> tbd --------------------------------[ ------ [ge.842.053.21]*/ /*-[01.0000.014.!]-*/ /*-[--.---.---.--]-*/
VISU__next          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_mark      =  '-';
   int         x_index     =    0;
   int         i           =    0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("visu_curr" , s_visu_curr);
   x_index = VISU__valid (s_visu_curr);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find next)----------------------*/
   DEBUG_VISU   yLOG_note    ("search for next mark");
   x_len = strlen (S_VISU_LIST);
   for (i = x_index + 1; i < x_len; ++i) {
      if (s_visu_info [i].active == VISU_NOT )  continue;
      DEBUG_VISU   yLOG_value   ("found"     , i);
      x_mark = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_char    ("x_mark"    , x_mark);
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return x_mark;
   }
   DEBUG_VISU   yLOG_note    ("not found");
   --rce;
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}



/*====================------------------------------------====================*/
/*===----                       visual history                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_HISTORY____o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
VISU__unset           (char a_visu)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_visu"    , a_visu);
   /*---(check mark)---------------------*/
   x_index = VISU__valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [x_index].active = VISU_NOT;
   DEBUG_VISU   yLOG_note    ("x");
   s_visu_info [x_index].x_root = 0;
   s_visu_info [x_index].x_beg  = 0;
   s_visu_info [x_index].x_end  = 0;
   DEBUG_VISU   yLOG_note    ("y");
   s_visu_info [x_index].y_root = 0;
   s_visu_info [x_index].y_beg  = 0;
   s_visu_info [x_index].y_end  = 0;
   DEBUG_VISU   yLOG_note    ("z");
   s_visu_info [x_index].z_root = 0;
   s_visu_info [x_index].z_beg  = 0;
   s_visu_info [x_index].z_end  = 0;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range");
   VISU__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
VISU__set             (char a_visu)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_visu"    , a_visu);
   /*---(check mark)---------------------*/
   x_index = VISU__valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [x_index].active = VISU_YES;
   DEBUG_VISU   yLOG_note    ("x");
   s_visu_info [x_index].x_root = s_visu.x_root;
   s_visu_info [x_index].x_beg  = s_visu.x_beg;
   s_visu_info [x_index].x_end  = s_visu.x_end;
   DEBUG_VISU   yLOG_note    ("y");
   s_visu_info [x_index].y_root = s_visu.y_root;
   s_visu_info [x_index].y_beg  = s_visu.y_beg;
   s_visu_info [x_index].y_end  = s_visu.y_end;
   DEBUG_VISU   yLOG_note    ("z");
   s_visu_info [x_index].z_root = s_visu.z_root;
   s_visu_info [x_index].z_beg  = s_visu.z_beg;
   s_visu_info [x_index].z_end  = s_visu.z_end;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range/current");
   s_visu_curr = a_visu;
   VISU__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.A52.153.55]*/ /*-[01.0000.033.8]-*/ /*-[--.---.---.--]-*/
VISU__return          (char a_visu)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_index     =    0;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_char    ("a_visu"    , a_visu);
   /*---(look for sequences)-------------*/
   DEBUG_MARK   yLOG_note    ("check special shortcuts");
   switch (a_visu) {
   case '[' : a_visu = s_visu_head;     break;
   case '<' : a_visu = VISU__prev ();   break;
   case '>' : a_visu = VISU__next ();   break;
   case ']' : a_visu = s_visu_tail;     break;
   }
   --rce;  if (a_visu < 0) {
      DEBUG_MARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   x_index = VISU__valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_visu_info [x_index].active != VISU_YES) {
      DEBUG_MARK   yLOG_note    ("nothing saved under this reference");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_live = VISU_YES;
   DEBUG_VISU   yLOG_note    ("x");
   s_visu.x_root = s_visu_info [x_index].x_root;
   s_visu.x_beg  = s_visu_info [x_index].x_beg;
   s_visu.x_end  = s_visu_info [x_index].x_end;
   DEBUG_VISU   yLOG_note    ("y");
   s_visu.y_root = s_visu_info [x_index].y_root;
   s_visu.y_beg  = s_visu_info [x_index].y_beg;
   s_visu.y_end  = s_visu_info [x_index].y_end;
   DEBUG_VISU   yLOG_note    ("z");
   s_visu.z_root = s_visu_info [x_index].z_root;
   s_visu.z_beg  = s_visu_info [x_index].z_beg;
   s_visu.z_end  = s_visu_info [x_index].z_end;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range/current");
   s_visu_curr = a_visu;
   MAP_jump (s_visu.x_end, s_visu.y_end, s_visu.z_end);
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      location marks                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_BASICS_____o () { return; }

char         /*-> remove marks -----------------------[ ------ [gz.642.141.12]*/ /*-[01.0000.033.3]-*/ /*-[--.---.---.--]-*/
VISU__purge          (char a_scope)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_len       = 0;
   char        x_visu      = 0;
   char       *x_upper     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   char       *x_sys       = "0123456789";
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   DEBUG_VISU   yLOG_value   ("max_visu"  , MAX_VISU);
   x_len = strlen (S_VISU_LIST);
   DEBUG_VISU   yLOG_value   ("x_len"     , x_len);
   for (i = 0; i < x_len; ++i) {
      x_visu = S_VISU_LIST [i];
      if (a_scope == VISU_UPPER  && strchr (x_upper, x_visu) == NULL)  continue;
      if (a_scope == VISU_LOWER  && strchr (x_lower, x_visu) == NULL)  continue;
      if (a_scope == VISU_SYSTEM && strchr (x_sys  , x_visu) == NULL)  continue;
      VISU__unset (x_visu);
   }
   /*---(globals)------------------------*/
   DEBUG_VISU   yLOG_note    ("initialize globals");
   s_visu_head  = '-';
   s_visu_curr  = '-';
   s_visu_tail  = '-';
   VISU__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> initialize all marks ---------------[ shoot  [gz.311.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
VISU_init            (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   VISU__purge  (VISU_ALL);
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual status                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_STATUS_____o () { return; }

char
yVIKEYS_root            (int x, int y, int z)
{
   if (s_live == VISU_NOT)  return 0;
   if (x != s_visu.x_root)  return 0;
   if (y != s_visu.y_root)  return 0;
   if (z != s_visu.z_root)  return 0;
   return 1;
}

char
yVIKEYS_visual          (int x, int y, int z)
{
   if (s_live == VISU_NOT)  return 0;
   if (x <  s_visu.x_beg)   return 0;
   if (x >  s_visu.x_end)   return 0;
   if (y <  s_visu.y_beg)   return 0;
   if (y >  s_visu.y_end)   return 0;
   if (z <  s_visu.z_beg)   return 0;
   if (z >  s_visu.z_end)   return 0;
   return 1;
}

char         /*-> selection active or not ------------[ ------ [gc.B50.00#.D7]*/ /*-[01.0000.104.#]-*/ /*-[--.---.---.--]-*/
VISU_islive        (void)
{
   if (s_live == VISU_YES)  return 1;
   return 0;
}

char
VISU_status           (char *a_list)
{
   if (a_list == NULL)  return -1;
   sprintf (a_list, "visu   %c   %4dx to %4dx,   %4dy to %4dy,   %4dz to %4dz",
         s_live,
         s_visu.x_beg, s_visu.x_end,
         s_visu.y_beg, s_visu.y_end,
         s_visu.z_beg, s_visu.z_end);
   return 0;
}

char
VISU_status_saved     (char *a_list)
{
   int         n           =    0;
   if (a_list == NULL)  return -1;
   n = VISU__valid (s_visu_curr);
   if (n < 0)   n = 0;
   sprintf (a_list, "saved  %c (%c to %c)  %4dx to %4dx,   %4dy to %4dy,   %4dz to %4dz",
         s_visu_curr, s_visu_head, s_visu_tail,
         s_visu_info [n].x_beg, s_visu_info [n].x_end,
         s_visu_info [n].y_beg, s_visu_info [n].y_end,
         s_visu_info [n].z_beg, s_visu_info [n].z_end);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual setting                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_SETTING____o () { return; }

static char
VISU__reverse           (void)
{
   if (s_visu.x_root == s_visu.x_beg) {
      s_visu.x_root = s_visu.x_end;
      s_visu.y_root = s_visu.y_end;
      s_visu.z_root = s_visu.z_end;
      MAP_jump (s_visu.x_beg, s_visu.y_beg, s_visu.z_beg);
   } else {
      s_visu.x_root = s_visu.x_beg;
      s_visu.y_root = s_visu.y_beg;
      s_visu.z_root = s_visu.z_beg;
      MAP_jump (s_visu.x_end, s_visu.y_end, s_visu.z_end);
   }
   return 0;
}

static char  /*-> adjust the visual selection --------[ ------ [ge.760.324.D2]*/ /*-[01.0000.015.X]-*/ /*-[--.---.---.--]-*/
VISU__update       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x, y, z;
   /*---(prepare)------------------------*/
   MAP_current  (&x, &y, &z);
   /*---(not-live)-----------------------*/
   if (s_live == VISU_NOT) {
      s_visu.x_root  = s_visu.x_beg   = s_visu.x_end   = x;
      s_visu.y_root  = s_visu.y_beg   = s_visu.y_end   = y;
      s_visu.z_root  = s_visu.z_beg   = s_visu.z_end   = z;
      return 0;
   }
   /*---(x)------------------------------*/
   if (x < s_visu.x_root) {
      s_visu.x_beg  = x;
      s_visu.x_end  = s_visu.x_root;
   } else {
      s_visu.x_beg  = s_visu.x_root;
      s_visu.x_end  = x;
   }
   /*---(y)------------------------------*/
   if (y < s_visu.y_root) {
      s_visu.y_beg  = y;
      s_visu.y_end  = s_visu.y_root;
   } else {
      s_visu.y_beg  = s_visu.y_root;
      s_visu.y_end  = y;
   }
   /*---(z)------------------------------*/
   if (z < s_visu.z_root) {
      s_visu.z_beg  = z;
      s_visu.z_end  = s_visu.z_root;
   } else {
      s_visu.z_beg  = s_visu.z_root;
      s_visu.z_end  = z;
   }
   /*---(complete)-----------------------*/
   return 0;
}

static char  /*-> clear the selection ----------------[ ------ [gz.742.001.13]*/ /*-[01.0000.743.A]-*/ /*-[--.---.---.--]-*/
VISU__clear         (void)
{
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(backup)-------------------------*/
   VISU__set ('\'');
   s_live  = VISU_NOT;
   VISU__update ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual ranges                          ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_RANGES_____o () { return; }

static char    s_valid = '-';
static int     s_x = 0;
static int     s_y = 0;
static int     s_z = 0;

char         /*-> return selection range borders -----[ leaf   [gc.730.107.C0]*/ /*-[01.0000.204.4]-*/ /*-[--.---.---.--]-*/
VISU_range          (int *a_xbeg, int *a_xend, int *a_ybeg, int *a_yend, int *a_zbeg, int *a_zend)
{
   if (a_xbeg != NULL) *a_xbeg = s_visu.x_beg;
   if (a_xend != NULL) *a_xend = s_visu.x_end;
   if (a_ybeg != NULL) *a_ybeg = s_visu.y_beg;
   if (a_yend != NULL) *a_yend = s_visu.y_end;
   if (a_zbeg != NULL) *a_zbeg = s_visu.z_beg;
   if (a_zend != NULL) *a_zend = s_visu.z_end;
   return 0;
}

char
yVIKEYS_first       (int *a_x, int *a_y, int *a_z)
{
   s_x = s_visu.x_beg;
   s_y = s_visu.y_beg;
   s_z = s_visu.z_beg;
   if (a_x != NULL) *a_x = s_x;
   if (a_y != NULL) *a_y = s_y;
   if (a_z != NULL) *a_z = s_z;
   s_valid = 'y';
   return 0;
}

char
yVIKEYS_next        (int *a_x, int *a_y, int *a_z)
{
   if (s_valid != 'y')  return -1;
   ++s_x;
   if (s_x > s_visu.x_end) {
      s_x = s_visu.x_beg;
      ++s_y;
      if (s_y > s_visu.y_end) {
         s_y = s_visu.y_beg;
         ++s_z;
         if (s_z > s_visu.z_end) {
            s_valid = '-';
         }
      }
   }
   if (a_x != NULL) *a_x = s_x;
   if (a_y != NULL) *a_y = s_y;
   if (a_z != NULL) *a_z = s_z;
   if (s_valid != 'y')  return -1;
   return 0;
}

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
VISU_smode         (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_majors    = "VM";
   static char x_prev      =  '-';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_USER   yLOG_char    ("x_prev"    , x_prev);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_VISUAL )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(exit mode)----------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape means leave");
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(major check)--------------------*/
   DEBUG_USER   yLOG_info    ("x_majors"  , x_majors);
   --rce;  if (strchr (x_majors, a_major) == NULL) {
      DEBUG_USER   yLOG_note    ("major not valid");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(common quick, any major)--------*/
   --rce;  if (strchr("!", a_minor) != NULL) {
      switch (a_minor) {
      case '!' :
         DEBUG_USER   yLOG_note    ("use visual status bar");
         yVIKEYS_cmds_direct (":status visual");
         break;
      }
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(common complex)-----------------*/
   /*> --rce;  if (strchr("?@", a_minor) != NULL) {                                             <* 
    *>    switch (a_minor) {                                                                    <* 
    *>    case '?' :                                                                            <* 
    *>       DEBUG_USER   yLOG_note    ("show mark info window");                               <* 
    *>       /+> my.info_win      = G_INFO_MARK;                                          <+/   <* 
    *>       return a_major;                                                                    <* 
    *>       break;                                                                             <* 
    *> }                                                                                        <*/
   /*---(check for setting)--------------*/
   --rce;  if (a_major == 'M') {
      DEBUG_USER   yLOG_note    ("handle visual selection saving (M)");
      rc = VISU__set (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for returning)------------*/
   --rce;  if (a_major == 'V') {
      DEBUG_USER   yLOG_note    ("handle visual selection return (V)");
      rc = VISU__return (a_minor);
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         MODE_exit ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_prev = a_minor;
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(failure)------------------------*/
   --rce;
   MODE_exit ();
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}











char
MAP_current             (int *a_x, int *a_y, int *a_z)
{
   if (a_x != NULL)  *a_x = g_xmap.gcur;
   if (a_y != NULL)  *a_y = g_ymap.gcur;
   if (a_z != NULL)  *a_z = g_zmap.gcur;
   return 0;
}

char         /*-> call host to update maps -----------[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
MAP_reposition       (void)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(update)-------------------------*/
   DEBUG_MAP   yLOG_point   ("s_mapper"  , s_mapper);
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_note    ("calling source program mapper");
      s_mapper (YVIKEYS_UPDATE);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> clear a single map -----------------[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
MAP__clear           (tMAPPED *a_map, char a_which)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_senter  (__FUNCTION__);
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(identity)-----------------------*/
   DEBUG_MAP   yLOG_snote   ("self");
   a_map->which = a_which;
   /*---(lefts)--------------------------*/
   DEBUG_MAP   yLOG_snote   ("mins");
   a_map->gmin  = YVIKEYS_EMPTY;
   a_map->amin  = YVIKEYS_EMPTY;
   a_map->lmin  = YVIKEYS_EMPTY;
   a_map->prev  = YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   DEBUG_MAP   yLOG_snote   ("maxs");
   a_map->next  = YVIKEYS_EMPTY;
   a_map->lmax  = YVIKEYS_EMPTY;
   a_map->amax  = YVIKEYS_EMPTY;
   a_map->gmax  = YVIKEYS_EMPTY;
   /*---(map)----------------------------*/
   DEBUG_MAP   yLOG_snote   ("map");
   for (i = 0; i < LEN_MAP; ++i) {
      a_map->map [i] = YVIKEYS_EMPTY;
   }
   /*---(indexes)------------------------*/
   DEBUG_MAP   yLOG_snote   ("screen");
   a_map->beg   = 0;
   a_map->cur   = 0;
   a_map->end   = 0;
   a_map->len   = 0;
   a_map->avail = 0;
   a_map->tend  = 0;
   /*---(grids)--------------------------*/
   DEBUG_MAP   yLOG_snote   ("grid");
   a_map->gbeg  = 0;
   a_map->gcur  = 0;
   a_map->gend  = 0;
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
MAP__print           (tMAPPED *a_map)
{
   int         i           =    0;
   /*---(headers)------------------------*/
   printf ("-  gmin amin lmin prev    ");
   /*> for (i = 0; i < LEN_MAP; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , i);                                                       <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   printf ("next lmax amax gmax    aval beg- cur- end- len- tend\n");
   /*---(content)------------------------*/
   printf ("%c  "                        , a_map->which);
   printf ("%4d %4d %4d %4d    "         , a_map->gmin , a_map->amin , a_map->lmin , a_map->prev );
   /*> for (i = 0; i < LEN_MAP; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , a_map->map [i]);                                          <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   /*---(end)----------------------------*/
   printf ("%4d %4d %4d %4d"              , a_map->next , a_map->lmax , a_map->amax , a_map->gmax );
   printf ("   %4d %4d %4d %4d %4d %4d\n", a_map->avail, a_map->beg  , a_map->cur  , a_map->end  , a_map->len  , a_map->tend );
   return 0;
}

char
MAP__load             (char a_style, tMAPPED *a_map, char a_which)
{
   int         i           =    0;
   int         j           =    0;
   int         x_spot      =    0;
   MAP__clear  (a_map, a_which);
   for (i = 0; i < 8; ++i) {
      switch (a_style) {
      case 'u' : /* uniform size grid       */
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
         break;
      case 'a' : /* ascending size grid     */
         for (j =  0; j <= i; ++j)  a_map->map [x_spot++] = i;
         break;
      case 'd' : /* descending size grid    */
         for (j =  0; j <= 7 - i; ++j)  a_map->map [x_spot++] = i;
         break;
      case 's' : /* small                   */
         for (j =  0; j <  3; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = -3; i <= 3; ++i) {
      switch (a_style) {
      case '-' :  /* neg to pos grid        */
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = -120; i <= 300; i += 20) {
      switch (a_style) {
      case 'j' :   /* true grid the skips  */
         a_map->map [x_spot++] = i;
         break;
      }
   }
   for (i = 0; i <= 40; ++i) {
      switch (a_style) {
      case 'w' :   /* get a big grid       */
         for (j =  0; j <  4; ++j)  a_map->map [x_spot++] = i;
         break;
      }
   }
   if (a_style == '1') {
      for (i = 0; i <= 100; ++i)   a_map->map [x_spot++] = i;
   }
   a_map->gmin  = 0;
   a_map->amin  = 0;
   a_map->lmin  = 0;
   a_map->prev  = 0;
   a_map->next  = x_spot - 1;;
   a_map->lmax  = x_spot - 1;;
   a_map->amax  = x_spot - 1;;
   a_map->gmax  = x_spot - 1;;
   switch (a_style) {
   case 'w' :
      a_map->cur   = 44;
      a_map->beg   = 44;
      a_map->len   = 36;
      a_map->end   = 79;
      a_map->avail = 38;
      a_map->tend  = 81;
      break;
   case '1' :
      a_map->cur   =  0;
      a_map->beg   =  0;
      a_map->len   = 30;
      a_map->end   = 29;
      a_map->avail = 30;
      a_map->tend  = 29;
      break;
   default  :
      a_map->cur   = 0;
      a_map->beg   = 0;
      a_map->len   = x_spot;
      a_map->end   = x_spot - 1;
      a_map->avail = x_spot;
      a_map->tend  = x_spot - 1;
      break;
   }
   a_map->gbeg  = a_map->map [a_map->beg];
   a_map->gcur  = a_map->map [a_map->cur];
   a_map->gend  = a_map->map [a_map->end];
   /*> MAP__print  (a_map);                                                           <*/
   return 0;
}

char
yVIKEYS_map_config     (char a_coord, void *a_mapper)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_senter  (__FUNCTION__);
   /*---(globals)------------------------*/
   g_coord    = a_coord;
   DEBUG_MAP   yLOG_schar   (g_coord);
   s_mapper   = a_mapper;
   /*---(update)-------------------------*/
   DEBUG_MAP   yLOG_spoint  (s_mapper);
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_snote   ("call host mapper");
      s_mapper (YVIKEYS_INIT);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
MAP_init               (void)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   DEBUG_PROG   yLOG_note    ("default globals");
   g_coord    = YVIKEYS_OFFICE;
   s_mapper   = NULL;
   DEBUG_PROG   yLOG_note    ("map clearing");
   MAP__clear (&g_xmap, YVIKEYS_XMAP);
   MAP__clear (&g_ymap, YVIKEYS_YMAP);
   MAP__clear (&g_zmap, YVIKEYS_ZMAP);
   MAP__clear (&g_tmap, YVIKEYS_TMAP);
   /*> MAP__print (&g_xmap);                                                          <*/
   /*> MAP__print (&g_ymap);                                                          <*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*-> idendify closest grid to position --[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
MAP__closer           (int a_position, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =    0;
   int         i           =    0;
   int         x_target    =    0;
   int         x_left      = YVIKEYS_EMPTY;
   int         x_right     = YVIKEYS_EMPTY;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_position", a_position);
   /*---(prepare)------------------------*/
   x_target = a_map->map [a_position];
   DEBUG_MAP   yLOG_value   ("x_target"  , x_target);
   DEBUG_MAP   yLOG_value   ("gmin"      , a_map->gmin);
   DEBUG_MAP   yLOG_value   ("gmax"      , a_map->gmax);
   /*---(look right)---------------------*/
   DEBUG_MAP   yLOG_note    ("go right");
   for (i = a_position; i <= a_map->gmax; ++i) {
      DEBUG_MAP   yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <= x_target)        continue;
      x_right = i;
      break;
   }
   if (x_right == YVIKEYS_EMPTY)  DEBUG_MAP   yLOG_note    ("x_right not found");
   else                           DEBUG_MAP   yLOG_value   ("x_right"   , x_right - a_position);
   /*---(look left)----------------------*/
   DEBUG_MAP   yLOG_note    ("go left");
   for (i = a_position; i >= a_map->gmin; --i) {
      DEBUG_MAP   yLOG_value   ("looking"   , i);
      if (a_map->map [i] == YVIKEYS_EMPTY)   break;
      if (a_map->map [i] <  x_target) {
         DEBUG_MAP   yLOG_value   ("x_left"    , x_left);
         break;
      }
      x_left  = i;
   }
   if (x_left  == YVIKEYS_EMPTY)  DEBUG_MAP   yLOG_note    ("x_left not found");
   else                           DEBUG_MAP   yLOG_value   ("x_left"    , a_position - x_left);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   rc = x_left;
   if      (x_left  == YVIKEYS_EMPTY && x_right == YVIKEYS_EMPTY)  rc = a_position;
   else if (x_left  == YVIKEYS_EMPTY)                              rc = x_right;
   else if (x_right == YVIKEYS_EMPTY)                              rc = x_left;
   else if (x_right - a_position <  a_position - x_left)           rc = x_right;
   return rc;
}

char
MAP__move             (int a_target, tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_target"  , a_target);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(defense)------------------------*/
   --rce;  if (a_map    == NULL) {
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> MAP__print  (a_map);                                                           <*/
   /*---(make sure index is rational)----*/
   DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
   DEBUG_MAP   yLOG_value   ("gmin"      , a_map->gmin);
   DEBUG_MAP   yLOG_value   ("gmax"      , a_map->gmax);
   if (a_map->cur < a_map->gmin)  a_map->cur = a_map->gmin;
   if (a_map->cur > a_map->gmax)  a_map->cur = a_map->gmax;
   DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
   a_map->gcur = a_map->map [a_map->cur];
   DEBUG_MAP   yLOG_value   ("gcur"      , a_map->gcur);
   /*---(shortcut)-----------------------*/
   if (a_map->gcur == a_target) {
      DEBUG_MAP   yLOG_note    ("already in position");
      DEBUG_MAP   yLOG_exit    (__FUNCTION__);
      return 2;
   }
   /*---(check to right)-----------------*/
   if (a_map->gcur <  a_target) {
      DEBUG_MAP   yLOG_note    ("must move to right");
      for (i = a_map->cur; i <= a_map->gmax; ++i) {
         if (a_map->map [i] <  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->gmax;
   }
   /*---(check to left)------------------*/
   else {
      DEBUG_MAP   yLOG_note    ("must move to left");
      /*---(find the right grid)---------*/
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         /*---(get to leftmost)----------*/
         for (i = a_map->cur; i >= a_map->gmin; --i) {
            if (a_map->map [i] != a_target)   break;
            a_map->cur  = i;
            DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         }
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->gmin;
   }
   /*---(get to leftmost)----------*/
   a_map->gcur   = a_map->map [a_map->cur];
   for (i = a_map->cur; i >= a_map->gmin; --i) {
      if (a_map->map [i] != a_map->gcur)   break;
      a_map->cur  = i;
   }
   /*> MAP__print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return  1;
}

char
MAP__screen_small         (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(prepare)------------------------*/
   a_map->beg   = a_map->gmin;
   a_map->len   = a_map->gmax - a_map->gmin + 1;
   a_map->end   = a_map->gmax;
   a_map->tend  = a_map->gmax;
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 2;
}


char
MAP__screen_beg           (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   int         x_tend      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(find closest beg backward)------*/
   for (i = a_map->beg; i > a_map->gmin; --i) {
      a_map->beg   = i;
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   DEBUG_MAP   yLOG_value   ("beg"       , a_map->beg);
   /*---(prepare)------------------------*/
   x_tend = a_map->beg + a_map->avail - 1;
   DEBUG_MAP   yLOG_value   ("gmax"      , a_map->gmax);
   if (x_tend <  a_map->gmax)  a_map->tend  = x_tend;
   DEBUG_MAP   yLOG_value   ("tend"      , a_map->tend);
   /*---(can not fill screen?)-----------*/
   --rce;  if (a_map->tend > a_map->gmax)   return rce;
   /*---(find end of last full grid)-----*/
   for (i = a_map->tend; i >= a_map->gmin; --i) {
      a_map->end   = i;
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   DEBUG_MAP   yLOG_value   ("end"       , a_map->end);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 1;
}

char
MAP__screen_end           (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find end of end)----------------*/
   /*> printf ("MAP__screen_end\n");                                          <*/
   /*> printf ("end  = %3d\n", a_map->end);                                           <*/
   for (i = a_map->end; i <= a_map->gmax; ++i) {
      a_map->end   = i;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      a_map->tend  = i;
      /*> printf ("tend = %3d\n", a_map->tend);                                       <*/
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(check overrun)------------------*/
   /*> printf ("gmax = %3d\n", a_map->gmax);                                          <*/
   /*> printf ("gmin = %3d\n", a_map->gmin);                                          <*/
   /*---(run the final)------------------*/
   a_map->beg   = a_map->tend - a_map->avail + 1;
   /*---(find next beg forward)----------*/
   for (i = a_map->beg; i < a_map->gmax; ++i) {
      a_map->beg   = i;
      /*> printf ("beg  = %3d\n", a_map->beg);                                        <*/
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   /*---(handle normally)----------------*/
   rc = MAP__screen_beg (a_map);
   /*---(complete)-----------------------*/
   return rc;
}

char
MAP__screen             (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   int         w           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   int         x_prev      =    0;
   /*---(defense)------------------------*/
   rce;  if (a_map == NULL)     return rce;
   /*---(limits)-------------------------*/
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   if (a_map->cur < a_map->gmin)  a_map->cur = a_map->gmin;
   if (a_map->cur > a_map->gmax) {
      a_map->cur = a_map->gmax;
      for (i = a_map->cur; i > a_map->gmin; --i) {
         a_map->cur   = i;
         x_curr       = a_map->map [i    ];
         x_prev       = a_map->map [i - 1];
         if (x_curr != x_prev)   break;
      }
   }
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   /*---(screen fits all)----------------*/
   if (a_map->gmax - a_map->gmin <= a_map->avail) {
      /*> printf ("processing a small\n");                                            <*/
      rc = MAP__screen_small (a_map);
      return rc;
   }
   /*---(from beginning)-----------------*/
   if (a_map->cur < a_map->beg) {
      myVIKEYS.redraw = 'y';
      /*> printf ("processing a left\n");                                             <*/
      a_map->beg = a_map->cur;
      for (i = a_map->cur; i >= a_map->gmin; --i) {
         a_map->beg = i;
         rc = MAP__screen_beg (a_map);
         if (rc > 0) break;
      }
   }
   /*---(from ending)--------------------*/
   else if (a_map->cur > a_map->end) {
      myVIKEYS.redraw = 'y';
      /*> printf ("processing a right\n");                                            <*/
      a_map->end   = a_map->cur;
      a_map->tend  = a_map->cur;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      rc = MAP__screen_end (a_map);
   }
   /*---(just a refresh)-----------------*/
   else {
      /*> printf ("processing a refresh\n");                                          <*/
      rc = MAP__screen_beg (a_map);
   }
   /*---(align grid)---------------------*/
   a_map->len   = a_map->end - a_map->beg + 1;
   a_map->gbeg  = a_map->map [a_map->beg];
   a_map->gcur  = a_map->map [a_map->cur];
   a_map->gend  = a_map->map [a_map->end];
   /*---(complete)-----------------------*/
   return rc;
}

char
MAP_jump              (int a_x, int a_y, int a_z)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_x"       , a_x);
   MAP__move   (a_x, &g_xmap);
   MAP__screen (&g_xmap);
   DEBUG_MAP   yLOG_value   ("a_y"       , a_y);
   MAP__move   (a_y, &g_ymap);
   MAP__screen (&g_ymap);
   DEBUG_MAP   yLOG_value   ("a_z"       , a_z);
   /*> MAP__move   (a_z, &g_zmap);                                                    <* 
    *> MAP__screen (&g_zmap);                                                         <*/
   MAP_reposition  ();
   VISU__update ();
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
MAP__vert             (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_char    ("a_major"   , a_major);
   DEBUG_MAP   yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_ymap.gcur;
   DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   x_qtr       = (g_ymap.avail - g_gsizey) / 4.0;
   DEBUG_MAP   yLOG_double  ("x_qtr"     , x_qtr);
   x_gmax  = g_ymap.map [g_ymap.gmax - g_gsizey];
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_vsimple" , g_vsimple);
   if (a_major == ' ' && strchr (g_vsimple, a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case '_' : x_grid  = g_ymap.map [g_ymap.gmin];  break;
         case 'K' : x_grid -= g_gsizey * 5;  break;
         case 'k' : x_grid -= g_gsizey;      break;
         case 'j' : x_grid += g_gsizey;      break;
         case 'J' : x_grid += g_gsizey * 5;  break;
         case '~' : x_grid  = x_gmax;        break;
         }
      } else {
         switch (a_minor) {
         case '~' : x_grid  = g_ymap.map [g_ymap.gmin];  break;
         case 'J' : x_grid -= g_gsizey * 5;  break;
         case 'j' : x_grid -= g_gsizey;      break;
         case 'k' : x_grid += g_gsizey;      break;
         case 'K' : x_grid += g_gsizey * 5;  break;
         case '_' : x_grid  = x_gmax;        break;
         }
      }
   }
   /*---(gotos)--------------------------*/
   DEBUG_MAP   yLOG_info    ("g_vgoto"   , g_vgoto);
   if (a_major == 'g' && strchr (g_vgoto  , a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case 'T' : x_unit  = g_ymap.beg - (x_qtr * 4); break;
         case 'K' : x_unit  = g_ymap.beg - (x_qtr * 2); break;
         case 't' : x_unit  = g_ymap.beg;               break;
         case 'k' : x_unit  = g_ymap.beg + (x_qtr * 1); break;
         case 'm' : x_unit  = g_ymap.beg + (x_qtr * 2); break;
         case 'j' : x_unit  = g_ymap.beg + (x_qtr * 3); break;
         case 'b' : x_unit  = g_ymap.beg + (x_qtr * 4); break;
         case 'J' : x_unit  = g_ymap.beg + (x_qtr * 6); break;
         case 'B' : x_unit  = g_ymap.beg + (x_qtr * 8); break;
         }
      } else {
         switch (a_minor) {
         case 'B' : x_unit  = g_ymap.beg - (x_qtr * 4); break;
         case 'J' : x_unit  = g_ymap.beg - (x_qtr * 2); break;
         case 'b' : x_unit  = g_ymap.beg;               break;
         case 'j' : x_unit  = g_ymap.beg + (x_qtr * 1); break;
         case 'm' : x_unit  = g_ymap.beg + (x_qtr * 2); break;
         case 'k' : x_unit  = g_ymap.beg + (x_qtr * 3); break;
         case 't' : x_unit  = g_ymap.beg + (x_qtr * 4); break;
         case 'K' : x_unit  = g_ymap.beg + (x_qtr * 6); break;
         case 'T' : x_unit  = g_ymap.beg + (x_qtr * 8); break;
         }
      }
      DEBUG_MAP   yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_ymap.gmin)  x_unit = g_ymap.gmin;
      if (x_unit > g_ymap.gmax)  x_unit = g_ymap.gmax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)", x_unit);
      x_unit  = MAP__closer (x_unit, &g_ymap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)", x_unit);
      x_grid  = g_ymap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("bjmkt", a_minor) != NULL)  if (x_grid > g_ymap.gend)  x_grid = g_ymap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizey;
   x_grid *= g_gsizey;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   MAP__move   (x_grid, &g_ymap);
   VISU__update ();
   MAP__screen (&g_ymap);
   MAP_reposition  ();
   /*> MAP__print (&g_xmap);                                                          <*/
   /*> MAP__print (&g_ymap);                                                          <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

int
MAP__horz             (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_grid      =    0;
   int         x_unit      =    0;
   float       x_qtr       =    0;
   int         x_beg       =    0;
   int         x_gmax      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_char    ("a_major"   , a_major);
   DEBUG_MAP   yLOG_char    ("a_minor"   , a_minor);
   /*---(prepare)------------------------*/
   x_grid      = g_xmap.gcur;
   DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   DEBUG_MAP   yLOG_value   ("avail"     , g_xmap.avail);
   DEBUG_MAP   yLOG_value   ("gsizex"    , g_gsizex);
   x_qtr       = (g_xmap.avail - g_gsizex) / 4.0;
   DEBUG_MAP   yLOG_double  ("x_qtr"     , x_qtr);
   x_beg       = g_xmap.beg;
   DEBUG_MAP   yLOG_double  ("x_beg"     , x_beg);
   x_gmax  = g_xmap.map [g_xmap.gmax - g_gsizex];
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_hsimple" , g_hsimple);
   if (a_major == ' ' && strchr (g_hsimple, a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute simple move");
      switch (a_minor) {
      case '0' : x_grid  = g_xmap.map [g_xmap.gmin];   break;
      case 'H' : x_grid -= g_gsizex * 5;                   break;
      case 'h' : x_grid -= g_gsizex;                       break;
      case 'l' : x_grid += g_gsizex;                       break;
      case 'L' : x_grid += g_gsizex * 5;                   break;
      case '$' : x_grid  = x_gmax;                         break;
      }
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(gotos)--------------------------*/
   DEBUG_MAP   yLOG_info    ("g_hgoto"   , g_hgoto);
   if (a_major == 'g' && strchr (g_hgoto  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute goto move");
      switch (a_minor) {
      case 'S' : x_unit  = x_beg - (x_qtr * 4);            break;
      case 'H' : x_unit  = x_beg - (x_qtr * 2);            break;
      case 's' : x_unit  = x_beg;                          break;
      case 'h' : x_unit  = x_beg + (x_qtr * 1);            break;
      case 'c' : x_unit  = x_beg + (x_qtr * 2);            break;
      case 'l' : x_unit  = x_beg + (x_qtr * 3);            break;
      case 'e' : x_unit  = x_beg + (x_qtr * 4);            break;
      case 'L' : x_unit  = x_beg + (x_qtr * 6);            break;
      case 'E' : x_unit  = x_beg + (x_qtr * 8);            break;
      }
      DEBUG_MAP   yLOG_value   ("x_unit"    , x_unit);
      if (x_unit < g_xmap.gmin)  x_unit = g_xmap.gmin;
      if (x_unit > g_xmap.gmax)  x_unit = g_xmap.gmax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)" , x_unit);
      x_unit  = MAP__closer (x_unit, &g_xmap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)" , x_unit);
      x_grid  = g_xmap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("shcle", a_minor) != NULL)  if (x_grid > g_xmap.gend)  x_grid = g_xmap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizex;
   x_grid *= g_gsizex;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   MAP__move   (x_grid, &g_xmap);
   VISU__update ();
   MAP__screen (&g_xmap);
   MAP_reposition  ();
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return G_KEY_SPACE;
}

char
yVIKEYS_map_refresh     (void)
{
   MAP__vert (' ', 'r');
   MAP__horz (' ', 'r');
   clear     ();
   return 0;
}

char
MAP_mode                (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_grid      =    0;
   char        t           [5];
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_MAP    )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(space)--------------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return G_KEY_SPACE;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN) {
      MODE_enter  (MODE_SOURCE);
      /*> EDIT_pos    ('0');                                                          <*/
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   if (a_minor == G_KEY_ESCAPE) {
      VISU__clear ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(single key)---------------------*/
   --rce;
   if (a_major == ' ') {
      /*---(repeat)----------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         MODE_enter  (SMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(multikey prefixes)-----------*/
      if (strchr ("gzced"  , a_minor) != 0) {
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(mode switch)-----------------*/
      switch (a_minor) {
      case 'v'      :
         if (s_live == VISU_YES)   VISU__reverse ();
         else                      s_live = VISU_YES;
         break;
      case ':'      :
         x_grid = REPEAT_use ();
         if (x_grid > 0) {
            MAP__move   (x_grid, &g_ymap);
            MAP__screen (&g_ymap);
            MAP_reposition  ();
            rc     = 0;
         } else {
            SOURCE_start   (":");
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return 'a';
         }
         break;
      case '/'      :
         SOURCE_start   ("/");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case 's'      :
         SOURCE_start   ("");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      case '='      : case '+'      : case '-'      : case '#'      :
         sprintf     (t, "%c", a_minor);
         SOURCE_start   (t);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 'a';
         break;
      }
      /*---(submodes)--------------------*/
      switch (a_minor) {
         /*> case '\\'     :                                                             <* 
          *>    DEBUG_USER   yLOG_note    ("selected menu mode");                        <* 
          *>    MODE_enter  (SMOD_MENUS  );                                      <* 
          *>    my.menu = MENU_ROOT;                                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return 0;                                                                <* 
          *>    break;                                                                   <*/
      case '@'      :
         IF_MACRO_OFF {
            MACRO_reset  ();
            MODE_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         MACRO_reset  ();
         DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
         break;
      case 'q'      :
         IF_MACRO_OFF {
            MODE_enter  (SMOD_MACRO   );
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return a_minor;
         }
         MACRO_rec_end ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
      case 'Q'      :
         MACRO_reset ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
         break;
      case 'F'      :
         DEBUG_USER   yLOG_note    ("calling custom format mode");
         MODE_enter  (SMOD_FORMAT  );
         if (rc < 0) {
            DEBUG_USER   yLOG_note    ("mode change refused");
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
            return rc;
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
         break;
         /*> case ','      :                                                             <* 
          *>    MODE_enter  (SMOD_BUFFER  );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;                                                          <* 
          *>    break;                                                                   <*/
         /*> case '"'      :                                                             <* 
          *>    MODE_enter  (SMOD_REGISTER);                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return a_minor;  /+ make sure double quote goes in prev char +/          <* 
          *>    break;                                                                   <*/
      case 'M'      : case 'V'      :
         DEBUG_USER   yLOG_note    ("entering visual selection history sub-mode");
         rc = MODE_enter  (SMOD_VISUAL  );
         if (rc < 0) {
            DEBUG_USER   yLOG_note    ("mode change refused");
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
            return rc;
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;  /* make sure single quote goes in prev char */
         break;
      case 'm'      : case '\''     :
         DEBUG_USER   yLOG_note    ("entering location mark sub-mode");
         rc = MODE_enter  (SMOD_MARK    );
         if (rc < 0) {
            DEBUG_USER   yLOG_note    ("mode change refused");
            DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
            return rc;
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;  /* make sure single quote goes in prev char */
         break;
         /*> case 'E'      :                                                             <* 
          *>    MODE_enter  (SMOD_ERROR   );                                     <* 
          *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
          *>    return SMOD_ERROR;  /+ make sure mode indicator goes also       +/       <* 
          *>    break;                                                                   <*/
      }
      /*---(normal)----------------------*/
      if (a_minor == 'P') {
         /*> MAP__print (&g_xmap);                                                    <*/
         /*> MAP__print (&g_ymap);                                                    <*/
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = MAP__horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_vsimple, a_minor) != 0) {
         rc = MAP__vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      /*---(column movement)-------------*/
      if (a_minor == '|') {
         MAP__move   (REPEAT_use (), &g_xmap);
         MAP__screen (&g_xmap);
         MAP_reposition  ();
         rc     = 0;
      }
      /*> rc = KEYS_regbasic (a_major, a_minor);                                      <* 
       *> if (rc == 0) {                                                              <* 
       *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                <* 
       *>    return 0;                                                                <* 
       *> }                                                                           <*/
      /*---(special)------------------*/
      /*> switch (a_minor) {                                                                                    <* 
       *> case K_CTRL_L : clear ();                       break;                                                <* 
       *> case 'P'      : DEP_writeall (); LOC_col_map (); KEYS_pcol (); KEYS_prow (); HIST_list ();  break;    <* 
       *>                 /+---(formatting)---------------+/                                                    <* 
       *> case '<'      : CELL_align (CHG_INPUT, '<');               break;                                     <* 
       *> case '|'      : CELL_align (CHG_INPUT, '|');               break;                                     <* 
       *> case '>'      : CELL_align (CHG_INPUT, '>');               break;                                     <* 
       *>                 /+---(selection)----------------+/                                                    <* 
       *>                 /+> case 'v'      : VISU_start (CTAB, CCOL, CROW, VISU_FROM);   break;          <+/   <* 
       *>                 /+> case 'V'      : VISU_start (CTAB, CCOL, CROW, VISU_CUM);    break;          <+/   <* 
       *>                 /+> case 'y'      : REG_copy  ();                   break;                      <+/   <* 
       *>                 /+> case 'p'      : REG_paste (G_PASTE_NORM);       break;                      <+/   <* 
       *>                 /+---(modes and multikey)-------+/                                                    <* 
       *> case '@'      : SEQ_calc_full ();               break;                                                <* 
       *>                 /+> case '[' : if (escaped) { sch = ch; special = 1; } else sch = 'x'; break;   <+/   <* 
       *>                 /+---(new stuff)----------------+/                                                    <* 
       *> case 'u'      : HIST_undo ();                   break;                                                <* 
       *> case 'U'      : HIST_redo ();                   break;                                                <* 
       *>                 /+> case 'W'      : REG_bufwrite (my.reg_curr);     break;                      <+/   <* 
       *> case '?'      : my.info_win = G_INFO_CELL;      break;                                                <* 
       *> default       : /+ unknown problem +/                                                                 <* 
       *>                 DEBUG_USER   yLOG_exit    (__FUNCTION__);                                             <* 
       *>                 return rce;                                                                           <* 
       *>                 break;                                                                                <* 
       *> }                                                                                                     <*/
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(special family)------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'c') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 's'      : KEYS_col ("cs");                break;                      <* 
    *>    case 'h'      : KEYS_col ("ch");                break;                      <* 
    *>    case 'l'      : KEYS_col ("cl");                break;                      <* 
    *>    case 'e'      : KEYS_col ("ce");                break;                      <* 
    *>    case 't'      : KEYS_row ("ct");                break;                      <* 
    *>    case 'b'      : KEYS_row ("cb");                break;                      <* 
    *>    default       : return rce;                     break;                      <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*> else if (a_major == 'x') {                                                      <* 
    *>    sch = ' ';                                                                  <* 
    *>    ch  = 24;                                                                   <* 
    *> }                                                                              <*/
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      if (strchr (g_hgoto, a_minor) != 0) {
         rc = MAP__horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
      if (strchr (g_vgoto, a_minor) != 0) {
         rc = MAP__vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return G_KEY_SPACE;
      }
   }
   /*---(scroll family)------------------*/
   /*> if (a_major == 'z') {                                                          <* 
    *>    rc = KEYS_gz_family  (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(end family)---------------------*/
   /*> if (a_major == 'e') {                                                          <* 
    *>    rc = KEYS_e_family   (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(end family)---------------------*/
   /*> if (a_major == 'c') {                                                          <* 
    *>    rc = KEYS_c_family   (a_major, a_minor);                                    <* 
    *>    DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(delete family)-------------------------*/
   /*> --rce;                                                                         <* 
    *> if (a_major == 'd') {                                                           <* 
    *>    switch (a_minor) {                                                           <* 
    *>    case 'd' : --NROW;          break;                                          <* 
    *>    case 'w' : --NCOL;     break;                                               <* 
    *>    default  : return rce;                         break;                       <* 
    *>    }                                                                           <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for wander mode -------[ ------ [ge.FE0.223.65]*/ /*-[05.0000.102.!]-*/ /*-[--.---.---.--]-*/
WANDER_smode            (int a_major, int a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should allow keyboard selection of cells and ranges for formulas
    *   much like excel does
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_grid      =    0;
   char        t           [5];
   char    post = ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (SMOD_WANDER )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(space)--------------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return G_KEY_SPACE;
   }
   /*---(check for control keys)---------*/
   /*> switch (a_minor) {                                                             <* 
    *> case  ',' :                                                                    <* 
    *> case  ')' : post = a_minor;                                                    <* 
    *> case  G_KEY_RETURN  :                                                          <* 
    *> case  G_KEY_ESCAPE  : VISU_clear ();                                           <* 
    *>                       LOC_ref (CTAB, CCOL, CROW, 0, wref);                     <* 
    *>                       CTAB = wtab;                                             <* 
    *>                       CCOL = wcol;                                             <* 
    *>                       CROW = wrow;                                             <* 
    *>                       my.cpos = wpos;                                          <* 
    *>                       strcpy (g_contents, wsave);                              <* 
    *>                       if (strcmp (wref2, "") != 0) {                           <* 
    *>                          strcat (g_contents, wref2);                           <* 
    *>                          strcat (g_contents, ":");                             <* 
    *>                       }                                                        <* 
    *>                       strcat (g_contents, wref);                               <* 
    *>                       my.npos = strlen(g_contents);                            <* 
    *>                       if (post != ' ') {                                       <* 
    *>                          g_contents[my.npos]   = post;                         <* 
    *>                          g_contents[++my.npos] = '\0';                         <* 
    *>                       }                                                        <* 
    *>                       my.cpos = my.npos;                                       <* 
    *>                       yVIKEYS_mode_exit ();                                    <* 
    *>                       return  0;   /+ escape -- back to source mode +/         <* 
    *> }                                                                              <*/
   /*---(basic movement)-----------*/
   /*> switch (a_minor) {                                                              <* 
    *> case '_'      : KEYS_row(" _");    break;                                      <* 
    *> case 'K'      : KEYS_row(" K");    break;                                      <* 
    *> case 'k'      : KEYS_row(" k");    break;                                      <* 
    *> case 'j'      : KEYS_row(" j");    break;                                      <* 
    *> case 'J'      : KEYS_row(" J");    break;                                      <* 
    *> case 'G'      : KEYS_row(" G");    break;                                      <* 
    *> case '{'      : KEYS_row(" {");    break;                                      <* 
    *> case '}'      : KEYS_row(" }");    break;                                      <* 
    *> case K_CTRL_B : KEYS_row("^b"); clear(); break;                                <* 
    *> case K_CTRL_F : KEYS_row("^f"); clear(); break;                                <* 
    *> case '0'      : KEYS_col (" 0");     break;                                    <* 
    *> case 'H'      : KEYS_col (" H");     break;                                    <* 
    *> case 'h'      : KEYS_col (" h");     break;                                    <* 
    *> case 'l'      : KEYS_col (" l");     break;                                    <* 
    *> case 'L'      : KEYS_col (" L");     break;                                    <* 
    *> case '$'      : KEYS_col (" $");     break;                                    <* 
    *> case 'b'      : KEYS_col ("es");     break;                                    <* 
    *> case 'e'      : KEYS_col ("ee");     break;                                    <* 
    *> case 'c'      : VISU_col();          break;                                    <* 
    *> case 'r'      : VISU_row();          break;                                    <* 
    *> }                                                                              <*/
   /*> if (a_minor == ':') {                                                          <* 
    *>    LOC_ref    (CTAB, CCOL, CROW, 0, wref2);                                    <* 
    *>    VISU_start (CTAB, CCOL, CROW, VISU_FROM);                                   <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   return  0;
}

char       /*----: give current position info --------------------------------*/
MAP_xstatus        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "x_axis,  map %4da %4db %4dc %4de %4dt %4dl,  grid %3db %3dc %3de",
         g_xmap.avail, g_xmap.beg  , g_xmap.cur  , g_xmap.end  , g_xmap.tend , g_xmap.len  ,
         g_xmap.gbeg , g_xmap.gcur , g_xmap.gend );
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: give current position info --------------------------------*/
MAP_ystatus        (char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   /*---(walk the list)------------------*/
   sprintf (a_list, "y_axis,  map %4da %4db %4dc %4de %4dt %4dl,  grid %3db %3dc %3de",
         g_ymap.avail, g_ymap.beg  , g_ymap.cur  , g_ymap.end  , g_ymap.tend , g_ymap.len  ,
         g_ymap.gbeg , g_ymap.gcur , g_ymap.gend );
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          unit testing                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char
MAP__unit_ymap          (void)
{
   /*> printf ("running rowmap\n");                                                   <*/
   MAP__load ('1', &g_ymap, YVIKEYS_YMAP);
   return 0;
}

char
MAP__unit_xmap          (void)
{
   /*> printf ("running colmap\n");                                                   <*/
   MAP__load ('w', &g_xmap, YVIKEYS_YMAP);
   return 0;
}

char
MAP__unit_zmap          (void)
{
   return 0;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
MAP__unit               (char *a_question, char a_index)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "MAP unit         : question not understood", LEN_STR);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "horz"           )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz pos     : index %3d, grid %3d", g_xmap.cur, g_xmap.gcur);
   }
   else if (strcmp (a_question, "horz_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_xmap.avail, g_xmap.beg, g_xmap.cur, g_xmap.end, g_xmap.tend, g_xmap.len);
   }
   else if (strcmp (a_question, "horz_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP horz grids   :        b %3d, c %3d, e %3d", g_xmap.gbeg, g_xmap.gcur, g_xmap.gend);
   }
   else if (strcmp (a_question, "vert_unit"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert units   : a %3d, b %3d, c %3d, e %3d, t %3d, l %3d", g_ymap.avail, g_ymap.beg, g_ymap.cur, g_ymap.end, g_ymap.tend, g_ymap.len);
   }
   else if (strcmp (a_question, "vert_grid"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP vert grids   :        b %3d, c %3d, e %3d", g_ymap.gbeg, g_ymap.gcur, g_ymap.gend);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
VISU__unit              (char *a_question, char a_which)
{
   int         n           =    0;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "VISU unit        : question not understood", LEN_STR);
   n = VISU__valid (a_which);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU selection   : %c  %4d to %4dx,  %4d to %4dy,  %4d to %4dz", s_live, s_visu.x_beg, s_visu.x_end, s_visu.y_beg, s_visu.y_end, s_visu.z_beg, s_visu.z_end);
   }
   else if (strcmp (a_question, "cursor"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU cursor      : %c  %4dx,  %4dy,  %4dz", s_live, s_visu.x_beg, s_visu.x_end, s_visu.y_beg, s_visu.y_end, s_visu.z_beg, s_visu.z_end);
   }
   else if (strcmp (a_question, "range"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU range       : %c to %c,  %c", s_visu_head, s_visu_tail, s_visu_curr);
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU saved   (%c) : %c  %4d to %4dx,  %4d to %4dy,  %4d to %4dz", a_which, s_visu_info [n].active, s_visu_info [n].x_beg, s_visu_info [n].x_end, s_visu_info [n].y_beg, s_visu_info [n].y_end, s_visu_info [n].z_beg, s_visu_info [n].z_end);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


