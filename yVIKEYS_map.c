/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



static char    (*s_mapper)    (char  a_type);
static char    (*s_locator)   (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z);
static char*   (*s_addresser) (char *a_label, int  a_buf, int  a_x, int  a_y, int  a_z);

static char*   (*s_switcher ) (char  a_label);



char   g_coord    = YVIKEYS_RIGHT;


char   g_vsimple   [LEN_DESC ]   = "_ Kk jJ ~";
char   g_vgoto     [LEN_DESC ]   = "TK tkmjb JB  ";
char   g_vends     [LEN_DESC ]   = "T  tkmjb  B M azud";
char   g_vscroll   [LEN_DESC ]   = " K tkmjb J   ";

char   g_hsimple   [LEN_DESC ]   = "0 Hh lL $";
char   g_hgoto     [LEN_DESC ]   = "SH shcle LE  ";
char   g_hends     [LEN_DESC ]   = "S  shcle  E C azud";
char   g_hscroll   [LEN_DESC ]   = " H shcle L   ";

char   g_hword     [LEN_DESC ]   = "wbe WBE";

char   g_multimap  [LEN_DESC ]   = "cgz e  dxia   ";
char   g_multivisu [LEN_DESC ]   = "cgz e p  ia   ";

char   g_multisrc  [LEN_DESC ]   = "cgz    dx   Ff";
char   g_multiselc [LEN_DESC ]   = "cgz         Ff";

char   g_repeat    [LEN_DESC ]   = "123456789";
char   g_search    [LEN_DESC ]   = "[<>]";


static char *s_map_modes = ":/,\" vMVm' s=+-#F @qQG";



#define     MAX_VISU       100
#define     VISU_NOT       '-'
#define     VISU_YES       'y'
static char  s_live = VISU_NOT;         /* is the selection active: 0=no, 1=yes          */

typedef     struct cVISU    tVISU;
struct cVISU {
   /*---(flag)---------------------------*/
   char        active;
   /*---(root)---------------------------*/
   int         b_all;
   int         x_root;
   int         y_root;
   int         z_all;
   /*---(begin)--------------------------*/
   int         x_beg;
   int         y_beg;
   /*---(end)----------------------------*/
   int         x_end;
   int         y_end;
   /*---(labels)-------------------------*/
   char        b_label     [LEN_LABEL];
   char        e_label     [LEN_LABEL];
   /*---(special)------------------------*/
   char        x_lock;
   char        y_lock;
   /*---(end)----------------------------*/
};
static tVISU  s_visu;
static tVISU  s_visu_info [MAX_VISU];
char          S_VISU_LIST [MAX_VISU] = "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static int    s_nvisu     =   0;

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
VISU_valid             (char a_visu)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char       *x_visu      = NULL;
   int         x_index     =   0;
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
   --rce;  if (x_index >= s_nvisu) {
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
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_senter  (__FUNCTION__);
   /*---(find next)----------------------*/
   s_visu_head = '-';
   for (i = 0; i < s_nvisu; ++i) {
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_head = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
      DEBUG_VISU   yLOG_snote   ("HEAD");
      break;
   }
   /*---(find last)----------------------*/
   s_visu_tail = '-';
   for (i = s_nvisu - 1; i >  0; --i) {
      if (s_visu_info [i].active == VISU_NOT ) continue;
      s_visu_tail = S_VISU_LIST [i];
      DEBUG_VISU   yLOG_schar   (S_VISU_LIST [i]);
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
   x_index = VISU_valid (s_visu_curr);
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
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_char    ("visu_curr" , s_visu_curr);
   x_index = VISU_valid (s_visu_curr);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find next)----------------------*/
   DEBUG_VISU   yLOG_note    ("search for next mark");
   for (i = x_index + 1; i < s_nvisu; ++i) {
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
   x_index = VISU_valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [x_index].active = VISU_NOT;
   DEBUG_VISU   yLOG_note    ("buf");
   s_visu_info [x_index].b_all  = 0;
   DEBUG_VISU   yLOG_note    ("x");
   s_visu_info [x_index].x_root = 0;
   s_visu_info [x_index].x_beg  = 0;
   s_visu_info [x_index].x_end  = 0;
   DEBUG_VISU   yLOG_note    ("y");
   s_visu_info [x_index].y_root = 0;
   s_visu_info [x_index].y_beg  = 0;
   s_visu_info [x_index].y_end  = 0;
   DEBUG_VISU   yLOG_note    ("z");
   s_visu_info [x_index].z_all  = 0;
   DEBUG_VISU   yLOG_note    ("labels");
   strlcpy (s_visu_info [x_index].b_label, "", LEN_LABEL);
   strlcpy (s_visu_info [x_index].e_label, "", LEN_LABEL);
   DEBUG_VISU   yLOG_note    ("locks");
   s_visu_info [x_index].x_lock = '-';
   s_visu_info [x_index].y_lock = '-';
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
   DEBUG_VISU   yLOG_char    ("s_live"    , s_live);
   if (s_live != VISU_YES) {
      DEBUG_VISU   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   x_index = VISU_valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   DEBUG_VISU   yLOG_note    ("set as active");
   s_visu_info [x_index].active = VISU_YES;
   DEBUG_VISU   yLOG_note    ("buf");
   s_visu_info [x_index].b_all  = s_visu.b_all;
   DEBUG_VISU   yLOG_note    ("x");
   s_visu_info [x_index].x_root = s_visu.x_root;
   s_visu_info [x_index].x_beg  = s_visu.x_beg;
   s_visu_info [x_index].x_end  = s_visu.x_end;
   DEBUG_VISU   yLOG_note    ("y");
   s_visu_info [x_index].y_root = s_visu.y_root;
   s_visu_info [x_index].y_beg  = s_visu.y_beg;
   s_visu_info [x_index].y_end  = s_visu.y_end;
   DEBUG_VISU   yLOG_note    ("z");
   s_visu_info [x_index].z_all  = s_visu.z_all;
   DEBUG_VISU   yLOG_note    ("labels");
   strlcpy (s_visu_info [x_index].b_label, s_visu.b_label, LEN_LABEL);
   strlcpy (s_visu_info [x_index].e_label, s_visu.e_label, LEN_LABEL);
   DEBUG_VISU   yLOG_note    ("locks");
   s_visu_info [x_index].x_lock = s_visu.x_lock;
   s_visu_info [x_index].y_lock = s_visu.y_lock;
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
   DEBUG_VISU   yLOG_note    ("check special shortcuts");
   switch (a_visu) {
   case '[' : a_visu = s_visu_head;     break;
   case '<' : a_visu = VISU__prev ();   break;
   case '>' : a_visu = VISU__next ();   break;
   case ']' : a_visu = s_visu_tail;     break;
   }
   --rce;  if (a_visu < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   x_index = VISU_valid (a_visu);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (s_visu_info [x_index].active != VISU_YES) {
      DEBUG_VISU   yLOG_note    ("nothing saved under this reference");
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
   s_visu.z_all  = s_visu_info [x_index].z_all;
   DEBUG_VISU   yLOG_note    ("labels");
   strlcpy (s_visu.b_label, s_visu_info [x_index].b_label, LEN_LABEL);
   strlcpy (s_visu.e_label, s_visu_info [x_index].e_label, LEN_LABEL);
   DEBUG_VISU   yLOG_note    ("locks");
   s_visu.x_lock = s_visu_info [x_index].x_lock;
   s_visu.y_lock = s_visu_info [x_index].y_lock;
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range/current");
   yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   if (a_visu != '\'') {
      s_visu_curr = a_visu;
      VISU__set ('\'');
   }
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
   char        x_visu      = 0;
   char       *x_upper     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   char       *x_sys       = "0123456789";
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   DEBUG_VISU   yLOG_value   ("max_visu"  , MAX_VISU);
   DEBUG_VISU   yLOG_value   ("nvisu"     , s_nvisu);
   for (i = 0; i < s_nvisu; ++i) {
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
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (UMOD_VISUAL)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear)--------------------------*/
   s_nvisu = strllen (S_VISU_LIST, 100);
   VISU__purge  (VISU_ALL);
   s_live = VISU_NOT;
   /*---(read/write)---------------------*/
   yVIKEYS_file_add (UMOD_VISUAL , VISU_writer, VISU_reader);
   /*---(update status)------------------*/
   STATUS_init_set   (UMOD_VISUAL);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
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
   if (z != s_visu.z_all)   return 0;
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
   if (z != s_visu.z_all)   return 0;
   return 1;
}

char         /*-> selection active or not ------------[ ------ [gc.B50.00#.D7]*/ /*-[01.0000.104.#]-*/ /*-[--.---.---.--]-*/
VISU_islive        (void)
{
   if (s_live == VISU_YES)  return 1;
   return 0;
}

char         /*-> selection active or not ------------[ ------ [gc.B50.00#.D7]*/ /*-[01.0000.104.#]-*/ /*-[--.---.---.--]-*/
VISU_onecell       (void)
{
   if (s_live != VISU_YES)            return 0;
   if (s_visu.x_beg != s_visu.x_end)  return 0;
   if (s_visu.y_beg != s_visu.y_end)  return 0;
   return 1;
}

char         /*-> tbd --------------------------------[ ------ [ge.420.132.11]*/ /*-[00.0000.114.!]-*/ /*-[--.---.---.--]-*/
VISU_infowin       (char *a_entry, int a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         a           =    0;
   int         n           =    0;
   char        t           [LEN_RECD]  = "";
   /*---(status)-------------------------*/
   strlcpy (a_entry, "                                                                              ", LEN_RECD);
   if (a_index <   0)  return 0;
   if (a_index >= 26)  return 0;
   a = 'a' + a_index;
   n = VISU_valid (a);
   if (s_visu_info [n].active == VISU_NOT )  sprintf (t, " %c  -           -                    ", a);
   else                                      sprintf (t, " %c  %-10.10s  %-10.10s           "        , a, s_visu_info [n].b_label, s_visu_info [n].e_label);
   strlcpy (a_entry, t, LEN_RECD);
   a = 'A' + a_index;
   n = VISU_valid (a);
   if (s_visu_info [n].active == VISU_NOT )  sprintf (t, " %c  -           -                    ", a);
   else                                      sprintf (t, " %c  %-10.10s  %-10.10s           "        , a, s_visu_info [n].b_label, s_visu_info [n].e_label);
   strlcat (a_entry, t, LEN_RECD);
   /*---(complete)-----------------------*/
   return 0;
}

char
VISU_status           (char *a_list)
{
   if (a_list == NULL)  return -1;
   sprintf (a_list, "visu   %c   %4dx to %4dx,   %4dy to %4dy,   %4dz",
         s_live,
         s_visu.x_beg, s_visu.x_end,
         s_visu.y_beg, s_visu.y_end,
         s_visu.z_all);
   return 0;
}

char
VISU_status_saved     (char *a_list)
{
   int         n           =    0;
   if (a_list == NULL)  return -1;
   n = VISU_valid (s_visu_curr);
   if (n < 0)   n = 0;
   sprintf (a_list, "saved  %c (%c to %c)  %4dx to %4dx,   %4dy to %4dy,   %4dz",
         s_visu_curr, s_visu_head, s_visu_tail,
         s_visu_info [n].x_beg, s_visu_info [n].x_end,
         s_visu_info [n].y_beg, s_visu_info [n].y_end,
         s_visu_info [n].z_all);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       visual setting                         ----===*/
/*====================------------------------------------====================*/
static void  o___VISU_SETTING____o () { return; }

static char
VISU__reverse           (void)
{
   /*---(if x locked)--------------------*/
   if (s_visu.x_lock == 'y') {
      if (s_visu.y_root == s_visu.y_beg) {
         s_visu.y_root = s_visu.y_end;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_root, s_visu.y_beg, s_visu.z_all);
      }
      else  {
         s_visu.y_root = s_visu.y_beg;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_root, s_visu.y_end, s_visu.z_all);
      }
      return 0;
   }
   /*---(if y locked)--------------------*/
   if (s_visu.y_lock == 'y') {
      if (s_visu.x_root == s_visu.x_beg) {
         s_visu.x_root = s_visu.x_end;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_beg, s_visu.y_root, s_visu.z_all);
      }
      else  {
         s_visu.x_root = s_visu.x_beg;
         yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_root, s_visu.z_all);
      }
      return 0;
   }
   /*---(root at beg)--------------------*/
   if (s_visu.x_root == s_visu.x_beg && s_visu.y_root == s_visu.y_beg) {
      s_visu.x_root = s_visu.x_end;
      s_visu.y_root = s_visu.y_end;
      yVIKEYS_jump (s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
      return 0;
   }
   /*---(root at end)--------------------*/
   s_visu.x_root = s_visu.x_beg;
   s_visu.y_root = s_visu.y_beg;
   yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   /*---(complete)-----------------------*/
   return 0;
}

static char  /*-> adjust the visual selection --------[ ------ [ge.760.324.D2]*/ /*-[01.0000.015.X]-*/ /*-[--.---.---.--]-*/
VISU__update       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         b, x, y, z;
   /*---(prepare)------------------------*/
   yvikeys_map_current  (NULL, &b, &x, &y, &z);
   /*---(not-live)-----------------------*/
   if (s_live == VISU_NOT) {
      s_visu.x_root  = s_visu.x_beg   = s_visu.x_end   = x;
      s_visu.y_root  = s_visu.y_beg   = s_visu.y_end   = y;
      s_visu.z_all   = z;
      yvikeys_map_addresser (s_visu.b_label, b, x, y, z);
      strlcpy  (s_visu.e_label, s_visu.b_label, LEN_LABEL);
      return 0;
   }
   /*---(x)------------------------------*/
   if (s_visu.x_lock != 'y') {
      if (x < s_visu.x_root) {
         s_visu.x_beg  = x;
         s_visu.x_end  = s_visu.x_root;
      } else {
         s_visu.x_beg  = s_visu.x_root;
         s_visu.x_end  = x;
      }
   }
   /*---(y)------------------------------*/
   if (s_visu.y_lock != 'y') {
      if (y < s_visu.y_root) {
         s_visu.y_beg  = y;
         s_visu.y_end  = s_visu.y_root;
      } else {
         s_visu.y_beg  = s_visu.y_root;
         s_visu.y_end  = y;
      }
   }
   /*---(set labels)---------------------*/
   yvikeys_map_addresser (s_visu.b_label, s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
   yvikeys_map_addresser (s_visu.e_label, s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
   /*---(save to recent)-----------------*/
   VISU__set ('\'');
   /*---(complete)-----------------------*/
   return 0;
}

char       /*-> clear the selection ----------------[ ------ [gz.742.001.13]*/ /*-[01.0000.743.A]-*/ /*-[--.---.---.--]-*/
VISU_clear          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x, y;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(backup)-------------------------*/
   s_live  = VISU_NOT;
   if (s_visu.x_lock == 'y')  x = s_visu.x_root;
   else                       x = s_visu.x_beg;
   if (s_visu.y_lock == 'y')  y = s_visu.y_root;
   else                       y = s_visu.y_beg;
   yVIKEYS_jump (s_visu.b_all, x, y, s_visu.z_all);
   s_visu.x_lock = '-';
   s_visu.y_lock = '-';
   VISU__update ();
   /*---(complete)-----------------------*/
   DEBUG_VISU   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> apply a specific selection ---------[ ------ [ge.E54.125.43]*/ /*-[01.0000.213.T]-*/ /*-[--.---.---.--]-*/
VISU_exact         (int a_xbeg, int a_xend, int a_ybeg, int a_yend, int a_z)
{  /*---(design notes)--------------------------------------------------------*/
   /* if the two ends of the range are legal, this function will change the   */
   /* current selection to the boundaries passed as arguments.                */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   char        rc          = 0;
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   DEBUG_VISU   yLOG_value   ("a_xbeg"    , a_xbeg);
   DEBUG_VISU   yLOG_value   ("a_xend"    , a_xend);
   DEBUG_VISU   yLOG_value   ("a_ybeg"    , a_ybeg);
   DEBUG_VISU   yLOG_value   ("a_yend"    , a_yend);
   DEBUG_VISU   yLOG_value   ("a_z"       , a_z   );
   /*---(prepare)------------------------*/
   DEBUG_VISU   yLOG_note    ("clear existing ranges");
   VISU_clear ();
   /*---(set range)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range live");
   s_live  = VISU_YES;
   /*---(locations)----------------------*/
   DEBUG_VISU   yLOG_note    ("set range coordinates");
   s_visu.x_root = s_visu.x_beg  = a_xbeg;
   s_visu.y_root = s_visu.y_beg  = a_ybeg;
   s_visu.x_end  = a_xend;
   s_visu.y_end  = a_yend;
   s_visu.z_all  = a_z;
   /*---(set labels)---------------------*/
   yvikeys_map_addresser (s_visu.b_label, s_visu.b_all, s_visu.x_beg, s_visu.y_beg, s_visu.z_all);
   yvikeys_map_addresser (s_visu.e_label, s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
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
VISU_range          (int *a_xbeg, int *a_xend, int *a_ybeg, int *a_yend, int *a_z)
{
   if (a_xbeg != NULL) *a_xbeg = s_visu.x_beg;
   if (a_xend != NULL) *a_xend = s_visu.x_end;
   if (a_ybeg != NULL) *a_ybeg = s_visu.y_beg;
   if (a_yend != NULL) *a_yend = s_visu.y_end;
   if (a_z    != NULL) *a_z    = s_visu.z_all;
   return 0;
}

char
yVIKEYS_first       (int *a_x, int *a_y, int *a_z)
{
   s_x = s_visu.x_beg;
   s_y = s_visu.y_beg;
   s_z = s_visu.z_all;
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
         s_valid = '-';
      }
   }
   if (a_x != NULL) *a_x = s_x;
   if (a_y != NULL) *a_y = s_y;
   if (a_z != NULL) *a_z = s_z;
   if (s_valid != 'y')  return -1;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       file handling                          ----===*/
/*====================------------------------------------====================*/
static void  o___FILE____________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
VISU_writer            (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_beg       =    0;
   char        x_end       =    0;
   int         i           =    0;
   char        c           =    0;
   /*---(prepare)----------------s-------*/
   yVIKEYS_unit_reset ();
   if (a_abbr == 0) {
      x_beg = 1;
      x_end = s_nvisu - 1;
   } else {
      x_beg = x_end = VISU_valid (a_abbr);
      if (x_beg <= 0)  return rce;
   }
   /*---(find marked entries)------------*/
   for (i = x_beg; i <= x_end; ++i) {
      if (s_visu_info [i].active != VISU_YES)  continue;
      yVIKEYS_file_write (UMOD_VISUAL, &(S_VISU_LIST [i]), &s_visu_info [i].x_beg, &s_visu_info [i].y_beg, &s_visu_info [i].x_end, &s_visu_info [i].y_end, &s_visu_info [i].z_all, NULL, NULL, NULL);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
VISU_reader           (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_index     =    0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_VISU   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_VISU   yLOG_char    ("version"   , n);
   --rce;  if (n != 'A') {
      DEBUG_VISU   yLOG_note    ("illegal version");
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check mark)---------------------*/
   DEBUG_VISU   yLOG_value   ("mark"      , a[0]);
   x_index = VISU_valid (a[0]);
   DEBUG_VISU   yLOG_value   ("x_index"   , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_VISU   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_VISU   yLOG_char    ("mark"      , a[0]);
   /*---(begin)--------------------------*/
   DEBUG_VISU   yLOG_note    ("assign values to beginning");
   s_visu_info [x_index].x_beg = atoi (b);
   DEBUG_VISU   yLOG_value   ("x_beg"     , s_visu_info [x_index].x_beg);
   s_visu_info [x_index].y_beg = atoi (c);
   DEBUG_VISU   yLOG_value   ("y_beg"     , s_visu_info [x_index].y_beg);
   /*---(end)----------------------------*/
   DEBUG_VISU   yLOG_note    ("assign values to ending");
   s_visu_info [x_index].x_end = atoi (d);
   DEBUG_VISU   yLOG_value   ("x_end"     , s_visu_info [x_index].x_end);
   s_visu_info [x_index].y_end = atoi (e);
   DEBUG_VISU   yLOG_value   ("y_end"     , s_visu_info [x_index].y_end);
   /*---(z)------------------------------*/
   s_visu_info [x_index].z_all = atoi (f);
   DEBUG_VISU   yLOG_value   ("z_all"     , s_visu_info [x_index].z_all);
   /*---(root)---------------------------*/
   s_visu_info [x_index].x_root = s_visu_info [x_index].x_beg;
   s_visu_info [x_index].y_root = s_visu_info [x_index].y_beg;
   /*---(status)-------------------------*/
   s_visu_info [x_index].active = VISU_YES;
   /*---(address)------------------------*/
   yvikeys_map_addresser (s_visu_info [x_index].b_label, s_visu_info [x_index].b_all, s_visu_info [x_index].x_beg, s_visu_info [x_index].y_beg, s_visu_info [x_index].z_all);
   DEBUG_VISU   yLOG_info    ("b_label"   , s_visu_info [x_index].b_label);
   yvikeys_map_addresser (s_visu_info [x_index].e_label, s_visu_info [x_index].b_all, s_visu_info [x_index].x_end, s_visu_info [x_index].y_end, s_visu_info [x_index].z_all);
   DEBUG_VISU   yLOG_info    ("e_label"   , s_visu_info [x_index].e_label);
   /*---(update range)-------------------*/
   DEBUG_VISU   yLOG_note    ("update the range");
   VISU__range ();
   /*---(complete)-----------------------*/
   DEBUG_VISU  yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                       visual mode                            ----===*/
/*====================------------------------------------====================*/
static void  o___MODE____________o () { return; }

char         /*-> process keys for marks -------------[ leaf   [ge.UD8.24#.JA]*/ /*-[03.0000.102.E]-*/ /*-[--.---.---.--]-*/
VISU_smode         (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =   -1;
   char       *x_majors    = "VM";
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   myVIKEYS.info_win = '-';
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_VISUAL )) {
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
   --rce;  if (a_minor == '?') {
      DEBUG_USER   yLOG_note    ("show mark info window");
      myVIKEYS.info_win = 'y';
      return a_major;
   }
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
yvikeys_map_current     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   char        rce         =  -10;
   char        rc          =    0;
   --rce;  if (a_label != NULL) {
      rc = yvikeys_map_addresser (a_label, g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
      if (rc < 0) return rce;
   }
   if (a_buf != NULL)  *a_buf = g_bmap.gcur;
   if (a_x   != NULL)  *a_x   = g_xmap.gcur;
   if (a_y   != NULL)  *a_y   = g_ymap.gcur;
   if (a_z   != NULL)  *a_z   = g_zmap.gcur;
   return 0;
}

char         /*-> call host to update maps -----------[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yvikeys_map_reposition  (void)
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
yvikeys__map_clear   (tMAPPED *a_map, char a_which)
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
   a_map->umin  = YVIKEYS_EMPTY;
   a_map->gamin = YVIKEYS_EMPTY;
   a_map->glmin = YVIKEYS_EMPTY;
   a_map->gprev = YVIKEYS_EMPTY;
   /*---(rights)-------------------------*/
   DEBUG_MAP   yLOG_snote   ("maxs");
   a_map->gnext = YVIKEYS_EMPTY;
   a_map->glmax = YVIKEYS_EMPTY;
   a_map->gamax = YVIKEYS_EMPTY;
   a_map->umax  = YVIKEYS_EMPTY;
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
yvikeys__map_print   (tMAPPED *a_map)
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
   printf ("%4d %4d %4d %4d    "         , a_map->gmin , a_map->gamin , a_map->glmin , a_map->gprev );
   /*> for (i = 0; i < LEN_MAP; ++i) {                                                <* 
    *>    if (a_map->map [i] == YVIKEYS_EMPTY)  break;                                <* 
    *>    printf ("%4d "  , a_map->map [i]);                                          <* 
    *> }                                                                              <* 
    *> printf ("   ");                                                                <*/
   /*---(end)----------------------------*/
   printf ("%4d %4d %4d %4d"              , a_map->gnext , a_map->glmax , a_map->gamax , a_map->gmax );
   printf ("   %4d %4d %4d %4d %4d %4d\n", a_map->avail, a_map->beg  , a_map->cur  , a_map->end  , a_map->len  , a_map->tend );
   return 0;
}

char
yvikeys__map_load     (char a_style, tMAPPED *a_map, char a_which)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         j           =    0;
   int         x_spot      =    0;
   /*---(clear)--------------------------*/
   yvikeys__map_clear  (a_map, a_which);
   /*---(col/row setup)------------------*/
   if (a_style == 'f') {
      for (j =  0; j < 16; ++j)  a_map->map [x_spot++] = 1;
      for (j =  0; j <  7; ++j)  a_map->map [x_spot++] = 2;
      for (j =  0; j <  7; ++j)  a_map->map [x_spot++] = 3;
      for (j =  0; j <  2; ++j)  a_map->map [x_spot++] = 4;
      for (j =  0; j < 20; ++j)  a_map->map [x_spot++] = 5;
      for (j =  0; j <  6; ++j)  a_map->map [x_spot++] = 6;
      for (j =  0; j <  6; ++j)  a_map->map [x_spot++] = 7;
      for (j =  0; j <  2; ++j)  a_map->map [x_spot++] = 8;

   }
   else if (strchr ("uads", a_style) != NULL) {
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
   } else if (a_style == '-') {  /* neg to pos grid        */
      for (i = -3; i <= 3; ++i) {
         for (j =  0; j <  8; ++j)  a_map->map [x_spot++] = i;
      }
   } else if (a_style == 'j') {   /* true grid the skips  */
      for (i = -120; i <= 300; i += 20) {
         a_map->map [x_spot++] = i;
      }
   } else if (a_style == 'w') {   /* get a big grid       */
      for (i = 0; i <= 40; ++i) {
         for (j =  0; j <  4; ++j)  a_map->map [x_spot++] = i;
      }
   } else if (a_style == '1') {
      for (i = 0; i <= 100; ++i)   a_map->map [x_spot++] = i;
   } else if (a_style == '0') {
      a_map->map [x_spot++] = 0;
   }
   /*---(unit min/max)-------------------*/
   a_map->umin  = 0;
   a_map->umax  = x_spot - 1;
   /*---(grid mins)----------------------*/
   a_map->gmin  = 0;
   a_map->gamin = 0;
   a_map->glmin = 0;
   a_map->gprev = 0;
   /*---(grid maxs)----------------------*/
   a_map->gnext = a_map->map [a_map->umax];
   a_map->glmax = a_map->map [a_map->umax];
   a_map->gamax = a_map->map [a_map->umax];
   a_map->gmax  = a_map->map [a_map->umax];
   /*---(final setup)--------------------*/
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
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   return 0;
}

char
yVIKEYS_map_config        (char a_coord, void *a_mapper, void *a_locator, void *a_addresser)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (MODE_MAP)) {
      DEBUG_MAP    yLOG_note    ("init must complete before config");
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   g_coord      = a_coord;
   s_mapper     = a_mapper;
   s_locator    = a_locator;
   s_addresser  = a_addresser;
   /*---(update status)------------------*/
   STATUS_conf_set   (MODE_MAP, '1');
   /*---(update)-------------------------*/
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_snote   ("call host mapper");
      s_mapper (YVIKEYS_INIT);
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> turn label into coordinates --------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
yvikeys_map_locator     (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rce         =    0;
   int         rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP_M yLOG_enter   (__FUNCTION__);
   /*---(check locator)------------------*/
   DEBUG_MAP_M yLOG_point   ("locator"    , s_locator);
   --rce;  if (s_locator == NULL) {
      DEBUG_MAP_M yLOG_exitr   (__FUNCTION__,rce);
      return rce;
   }
   /*---(display input)------------------*/
   DEBUG_MAP_M yLOG_point   ("a_label"   , a_label);
   if (a_label != NULL)  DEBUG_MAP_M yLOG_info    ("a_label"   , a_label);
   DEBUG_MAP_M yLOG_point   ("a_buf"     , a_buf);
   if (a_buf   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_buf"    , *a_buf);
   DEBUG_MAP_M yLOG_point   ("a_x"       , a_x);
   if (a_x     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_x"      , *a_x);
   DEBUG_MAP_M yLOG_point   ("a_y"       , a_y);
   if (a_y     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_y"      , *a_y);
   DEBUG_MAP_M yLOG_point   ("a_z"       , a_z);
   if (a_z     != NULL)  DEBUG_MAP_M yLOG_value   ("*a_z"      , *a_z);
   /*---(call locator)-------------------*/
   rc = s_locator (a_label, a_buf, a_x, a_y, a_z);
   DEBUG_MAP   yLOG_value   ("locator"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_MAP_M yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display output)-----------------*/
   if (a_buf != NULL)  DEBUG_MAP_M yLOG_value   ("*a_buf"    , *a_buf);
   if (a_x   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_x"      , *a_x);
   if (a_y   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_y"      , *a_y);
   if (a_z   != NULL)  DEBUG_MAP_M yLOG_value   ("*a_z"      , *a_z);
   /*---(complete)-----------------------*/
   DEBUG_MAP_M yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*-> turn coordinates into label --------[ ------ [gc.722.112.13]*/ /*-[01.0000.304.#]-*/ /*-[--.---.---.--]-*/
yvikeys_map_addresser   (char *a_label, int a_buf, int a_x, int a_y, int a_z)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   /*---(check addresser)----------------*/
   DEBUG_MAP   yLOG_point   ("addresser" , s_addresser);
   --rce;  if (s_addresser == NULL)  {
      strlcpy (a_label, "-", LEN_LABEL);
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display input)------------------*/
   DEBUG_MAP   yLOG_point   ("a_label"   , a_label);
   DEBUG_MAP   yLOG_value   ("a_buf"     , a_buf);
   DEBUG_MAP   yLOG_value   ("a_x"       , a_x);
   DEBUG_MAP   yLOG_value   ("a_y"       , a_y);
   DEBUG_MAP   yLOG_value   ("a_z"       , a_z);
   /*---(call locator)-------------------*/
   rc = s_addresser (a_label, a_buf, a_x, a_y, a_z);
   DEBUG_MAP   yLOG_value   ("addresser" , rc);
   --rce;  if (rc < 0) {
      DEBUG_MAP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(display output)-----------------*/
   if (a_label != NULL)  DEBUG_MAP   yLOG_info    ("a_label"   , a_label);
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return rc;
}

char
yvikeys_map_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (MODE_MAP)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_note    ("default globals");
   g_coord    = YVIKEYS_OFFICE;
   s_mapper   = NULL;
   DEBUG_PROG   yLOG_note    ("map clearing");
   yvikeys__map_clear (&g_bmap, YVIKEYS_BMAP);
   yvikeys__map_clear (&g_xmap, YVIKEYS_XMAP);
   yvikeys__map_clear (&g_ymap, YVIKEYS_YMAP);
   yvikeys__map_clear (&g_zmap, YVIKEYS_ZMAP);
   /*---(update status)------------------*/
   STATUS_init_set   (MODE_MAP);
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*-> idendify closest grid to position --[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yvikeys__map_closer   (int a_position, tMAPPED *a_map)
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
   DEBUG_MAP   yLOG_value   ("umin"      , a_map->umin);
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   /*---(look right)---------------------*/
   DEBUG_MAP   yLOG_note    ("go right");
   for (i = a_position; i <= a_map->umax; ++i) {
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
   for (i = a_position; i >= a_map->umin; --i) {
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
yvikeys__map_move     (int a_target, tMAPPED *a_map)
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
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(make sure index is rational)----*/
   DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
   DEBUG_MAP   yLOG_value   ("umin"      , a_map->umin);
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   if (a_map->cur < a_map->umin)  a_map->cur = a_map->umin;
   if (a_map->cur > a_map->umax)  a_map->cur = a_map->umax;
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
      for (i = a_map->cur; i <= a_map->umax; ++i) {
         if (a_map->map [i] <  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->umax;
   }
   /*---(check to left)------------------*/
   else {
      DEBUG_MAP   yLOG_note    ("must move to left");
      /*---(find the right grid)---------*/
      for (i = a_map->cur; i >= a_map->umin; --i) {
         if (a_map->map [i] >  a_target)   continue;
         a_map->cur  = i;
         a_map->gcur = a_map->map [a_map->cur];
         /*---(get to leftmost)----------*/
         for (i = a_map->cur; i >= a_map->umin; --i) {
            if (a_map->map [i] != a_target)   break;
            a_map->cur  = i;
            DEBUG_MAP   yLOG_value   ("cur"       , a_map->cur);
         }
         DEBUG_MAP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      a_map->cur  = a_map->umin;
   }
   /*---(get to leftmost)----------*/
   a_map->gcur   = a_map->map [a_map->cur];
   for (i = a_map->cur; i >= a_map->umin; --i) {
      if (a_map->map [i] != a_map->gcur)   break;
      a_map->cur  = i;
   }
   /*> yvikeys__map_print  (a_map);                                                           <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return  1;
}

char
yvikeys__screen_small     (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_curr      =    0;
   int         x_next      =    0;
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_point   ("a_map"     , a_map);
   /*---(prepare)------------------------*/
   a_map->beg   = a_map->umin;
   a_map->len   = a_map->umax - a_map->umin + 1;
   a_map->end   = a_map->umax;
   a_map->tend  = a_map->umax;
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 2;
}


char
yvikeys__screen_beg       (tMAPPED *a_map)
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
   for (i = a_map->beg; i > a_map->umin; --i) {
      a_map->beg   = i;
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   DEBUG_MAP   yLOG_value   ("beg"       , a_map->beg);
   /*---(prepare)------------------------*/
   x_tend = a_map->beg + a_map->avail - 1;
   DEBUG_MAP   yLOG_value   ("umax"      , a_map->umax);
   if (x_tend <  a_map->umax)  a_map->tend  = x_tend;
   DEBUG_MAP   yLOG_value   ("tend"      , a_map->tend);
   /*---(can not fill screen?)-----------*/
   --rce;  if (a_map->tend > a_map->umax)   return rce;
   /*---(find end of last full grid)-----*/
   for (i = a_map->tend; i >= a_map->umin; --i) {
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
yvikeys__screen_end       (tMAPPED *a_map)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_curr      =    0;
   int         x_prev      =    0;
   int         x_next      =    0;
   /*---(find end of end)----------------*/
   /*> printf ("yvikeys__screen_end\n");                                          <*/
   /*> printf ("end  = %3d\n", a_map->end);                                           <*/
   for (i = a_map->end; i <= a_map->umax; ++i) {
      a_map->end   = i;
      /*> printf ("end  = %3d\n", a_map->end);                                        <*/
      a_map->tend  = i;
      /*> printf ("tend = %3d\n", a_map->tend);                                       <*/
      x_curr       = a_map->map [i    ];
      x_next       = a_map->map [i + 1];
      if (x_curr != x_next)   break;
   }
   /*---(check overrun)------------------*/
   /*> printf ("umax = %3d\n", a_map->umax);                                          <*/
   /*> printf ("umin = %3d\n", a_map->umin);                                          <*/
   /*---(run the final)------------------*/
   a_map->beg   = a_map->tend - a_map->avail + 1;
   /*---(find next beg forward)----------*/
   for (i = a_map->beg; i < a_map->umax; ++i) {
      a_map->beg   = i;
      /*> printf ("beg  = %3d\n", a_map->beg);                                        <*/
      x_curr       = a_map->map [i    ];
      x_prev       = a_map->map [i - 1];
      if (x_curr != x_prev)   break;
   }
   /*---(handle normally)----------------*/
   rc = yvikeys__screen_beg (a_map);
   /*---(complete)-----------------------*/
   return rc;
}

char
yvikeys__screen         (tMAPPED *a_map)
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
   if (a_map->cur < a_map->umin)  a_map->cur = a_map->umin;
   if (a_map->cur > a_map->umax) {
      a_map->cur = a_map->umax;
      for (i = a_map->cur; i > a_map->umin; --i) {
         a_map->cur   = i;
         x_curr       = a_map->map [i    ];
         x_prev       = a_map->map [i - 1];
         if (x_curr != x_prev)   break;
      }
   }
   /*> printf ("cur  = %3d\n", a_map->cur);                                           <*/
   /*---(screen fits all)----------------*/
   if (a_map->umax - a_map->umin <= a_map->avail) {
      /*> printf ("processing a small\n");                                            <*/
      rc = yvikeys__screen_small (a_map);
      return rc;
   }
   /*---(from beginning)-----------------*/
   if (a_map->cur < a_map->beg) {
      myVIKEYS.redraw = 'y';
      /*> printf ("processing a left\n");                                             <*/
      a_map->beg = a_map->cur;
      for (i = a_map->cur; i >= a_map->umin; --i) {
         a_map->beg = i;
         rc = yvikeys__screen_beg (a_map);
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
      rc = yvikeys__screen_end (a_map);
   }
   /*---(just a refresh)-----------------*/
   else {
      /*> printf ("processing a refresh\n");                                          <*/
      rc = yvikeys__screen_beg (a_map);
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
yVIKEYS_jump              (int a_buf, int a_x, int a_y, int a_z)
{
   /*---(header)-------------------------*/
   DEBUG_MAP   yLOG_enter   (__FUNCTION__);
   DEBUG_MAP   yLOG_value   ("a_buf"     , a_buf);
   yvikeys__map_move   (a_buf, &g_bmap);
   yvikeys__screen (&g_bmap);
   DEBUG_MAP   yLOG_value   ("a_x"       , a_x);
   yvikeys__map_move   (a_x, &g_xmap);
   yvikeys__screen (&g_xmap);
   DEBUG_MAP   yLOG_value   ("a_y"       , a_y);
   yvikeys__map_move   (a_y, &g_ymap);
   yvikeys__screen (&g_ymap);
   DEBUG_MAP   yLOG_value   ("a_z"       , a_z);
   yvikeys__map_move   (a_z, &g_zmap);
   yvikeys__screen (&g_zmap);
   yvikeys_map_reposition  ();
   VISU__update ();
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys__map_vert     (char a_major, char a_minor)
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
   x_gmax  = g_ymap.map [g_ymap.umax - g_gsizey];
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_vsimple" , g_vsimple);
   if (a_major == ' ' && strchr (g_vsimple, a_minor) != NULL) {
      if (g_coord == YVIKEYS_OFFICE) {
         switch (a_minor) {
         case '_' : x_grid  = g_ymap.map [g_ymap.umin];  break;
         case 'K' : x_grid -= g_gsizey * 5;  break;
         case 'k' : x_grid -= g_gsizey;      break;
         case 'j' : x_grid += g_gsizey;      break;
         case 'J' : x_grid += g_gsizey * 5;  break;
         case '~' : x_grid  = x_gmax;        break;
         }
      } else {
         switch (a_minor) {
         case '~' : x_grid  = g_ymap.map [g_ymap.umin];  break;
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
      if (x_unit < g_ymap.umin)  x_unit = g_ymap.umin;
      if (x_unit > g_ymap.umax)  x_unit = g_ymap.umax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)", x_unit);
      x_unit  = yvikeys__map_closer (x_unit, &g_ymap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)", x_unit);
      x_grid  = g_ymap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("bjmkt", a_minor) != NULL)  if (x_grid > g_ymap.gend)  x_grid = g_ymap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(ends and edges)-----------------*/
   DEBUG_MAP   yLOG_info    ("g_vends"   , g_vends);
   if (a_major == 'e' && strchr (g_vends  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute ends and edges move");
      DEBUG_MAP   yLOG_value   ("gmin"      , g_ymap.gmin);
      DEBUG_MAP   yLOG_value   ("gamin"     , g_ymap.gamin);
      DEBUG_MAP   yLOG_value   ("glmin"     , g_ymap.glmin);
      DEBUG_MAP   yLOG_value   ("gprev"     , g_ymap.gprev);
      DEBUG_MAP   yLOG_value   ("gnext"     , g_ymap.gnext);
      DEBUG_MAP   yLOG_value   ("glmax"     , g_ymap.glmax);
      DEBUG_MAP   yLOG_value   ("gamax"     , g_ymap.gamax);
      DEBUG_MAP   yLOG_value   ("gmax"      , g_ymap.gmax);
      switch (a_minor) {
      case 'a' : case 'u' :
      case 'T' : x_grid = g_ymap.gamin;     break;
      case 't' : x_grid = g_ymap.glmin;     break;
      case 'k' : x_grid = g_ymap.gprev;     break;
      case 'm' : break;
      case 'M' : break;
      case 'j' : x_grid = g_ymap.gnext;     break;
      case 'b' : x_grid = g_ymap.glmax;     break;
      case 'z' : case 'd' :
      case 'B' : x_grid = g_ymap.gamax;     break;
      }
      if (x_grid < g_ymap.gmin)  x_grid = g_ymap.gcur;
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizey;
   x_grid *= g_gsizey;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   yvikeys__map_move   (x_grid, &g_ymap);
   VISU__update ();
   yvikeys__screen (&g_ymap);
   yvikeys_map_reposition  ();
   /*> yvikeys__map_print (&g_xmap);                                                          <*/
   /*> yvikeys__map_print (&g_ymap);                                                          <*/
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

int
yvikeys__map_horz     (char a_major, char a_minor)
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
   x_gmax      = g_xmap.gmax;
   DEBUG_MAP   yLOG_value   ("x_gmax"    , x_gmax);
   /*---(simple)-------------------------*/
   DEBUG_MAP   yLOG_info    ("g_hsimple" , g_hsimple);
   if (a_major == ' ' && strchr (g_hsimple, a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute simple move");
      switch (a_minor) {
      case '0' : x_grid  = g_xmap.map [g_xmap.umin];   break;
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
      if (x_unit < g_xmap.umin)  x_unit = g_xmap.umin;
      if (x_unit > g_xmap.umax)  x_unit = g_xmap.umax;
      DEBUG_MAP   yLOG_value   ("x_unit (1)" , x_unit);
      x_unit  = yvikeys__map_closer (x_unit, &g_xmap);
      DEBUG_MAP   yLOG_value   ("x_unit (2)" , x_unit);
      x_grid  = g_xmap.map [x_unit];
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
      if (strchr ("shcle", a_minor) != NULL)  if (x_grid > g_xmap.gend)  x_grid = g_xmap.gend;
      DEBUG_MAP   yLOG_value   ("x_grid (1)", x_grid);
   }
   /*---(ends and edges)-----------------*/
   DEBUG_MAP   yLOG_info    ("g_hends"   , g_hends);
   if (a_major == 'e' && strchr (g_hends  , a_minor) != NULL) {
      DEBUG_MAP   yLOG_note    ("execute ends and edges move");
      DEBUG_MAP   yLOG_value   ("gmin"      , g_xmap.gmin);
      DEBUG_MAP   yLOG_value   ("gamin"     , g_xmap.gamin);
      DEBUG_MAP   yLOG_value   ("glmin"     , g_xmap.glmin);
      DEBUG_MAP   yLOG_value   ("gprev"     , g_xmap.gprev);
      DEBUG_MAP   yLOG_value   ("gnext"     , g_xmap.gnext);
      DEBUG_MAP   yLOG_value   ("glmax"     , g_xmap.glmax);
      DEBUG_MAP   yLOG_value   ("gamax"     , g_xmap.gamax);
      DEBUG_MAP   yLOG_value   ("gmax"      , g_xmap.gmax);
      switch (a_minor) {
      case 'a' : case 'd' :
      case 'S' : x_grid = g_xmap.gamin;     break;
      case 's' : x_grid = g_xmap.glmin;     break;
      case 'h' : x_grid = g_xmap.gprev;     break;
      case 'c' : break;
      case 'C' : break;
      case 'l' : x_grid = g_xmap.gnext;     break;
      case 'e' : x_grid = g_xmap.glmax;     break;
      case 'z' : case 'u' :
      case 'E' : x_grid = g_xmap.gamax;     break;
      }
      if (x_grid < g_xmap.gmin)  x_grid = g_xmap.gcur;
      DEBUG_MAP   yLOG_value   ("x_grid"    , x_grid);
   }
   /*---(check screen)-------------------*/
   if (x_grid > x_gmax)  x_grid = x_gmax;
   x_grid /= g_gsizex;
   x_grid *= g_gsizex;
   DEBUG_MAP   yLOG_value   ("x_grid new", x_grid);
   yvikeys__map_move   (x_grid, &g_xmap);
   VISU__update ();
   yvikeys__screen (&g_xmap);
   yvikeys_map_reposition  ();
   /*---(complete)-----------------------*/
   DEBUG_MAP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_map_refresh     (void)
{
   /*---(update)-------------------------*/
   DEBUG_MAP   yLOG_point   ("s_mapper"  , s_mapper);
   if (s_mapper != NULL) {
      DEBUG_MAP   yLOG_note    ("calling source program mapper");
      s_mapper (YVIKEYS_UPDATE);
   }
   /*---(refresh position)---------------*/
   yvikeys__map_vert (' ', 'r');
   yvikeys__map_horz (' ', 'r');
   clear     ();
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys__map_mode_chg   (char a_minor)
{
   char        rc          =   -1;
   char        t           [5];
   /*---(common modes)----------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("change"    , a_minor);
   switch (a_minor) {
   case 'G'      :
      DEBUG_USER   yLOG_note    ("entering visual selection history sub-mode");
      MODE_enter  (MODE_GOD     );
      rc = 0;
      break;
   case ':'      :
      SOURCE_start   (":");
      rc = 'a';
      break;
   case '/'      :
      SOURCE_start   ("/");
      rc = 'a';
      break;
   case ','      :
      MODE_enter  (SMOD_BUFFER  );
      rc = a_minor;
      break;
   case '"'      :
      MODE_enter  (SMOD_MREG);
      rc = a_minor;
      break;
   case 'E'      :
      /*> MODE_enter  (SMOD_ERROR   );                                                <* 
       *> DEBUG_USER   yLOG_exit    (__FUNCTION__);                                   <* 
       *> rc = a_minor;                                                               <*/
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(selecting and marking)-------*/
   switch (a_minor) {
   case 'v'      :
      if (s_live == VISU_YES)   VISU__reverse ();
      else                      s_live = VISU_YES;
      rc = 0;
      break;
   case 'M'      : case 'V'      :
      DEBUG_USER   yLOG_note    ("entering visual selection history sub-mode");
      rc = MODE_enter  (UMOD_VISUAL  );
      if (rc >= 0)  rc = a_minor;
      break;
   case 'm'      : case '\''     :
      DEBUG_USER   yLOG_note    ("entering location mark sub-mode");
      rc = MODE_enter  (UMOD_MARK    );
      if (rc >= 0)  rc = a_minor;
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(source entry)----------------*/
   switch (a_minor) {
   case 's'      : case '='      : case '+'      : case '-'      : case '#'      :
      if (a_minor == 's')   strlcpy (t, "", LEN_LABEL);
      else                  sprintf     (t, "%c", a_minor);
      SOURCE_start   (t);
      rc = 'a';
      break;
   case 'F'      :
      DEBUG_USER   yLOG_note    ("calling custom format mode");
      rc = MODE_enter  (XMOD_FORMAT  );
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(macros)----------------------*/
   switch (a_minor) {
   case '@'      :
      IF_MACRO_OFF {
         yvikeys_macro_reset  ();
         MODE_enter  (SMOD_MACRO   );
         rc = a_minor;
      } else {
         yvikeys_macro_reset  ();
         rc = -1;
      }
      break;
   case 'q'      :
      IF_MACRO_OFF {
         MODE_enter  (SMOD_MACRO   );
         rc = a_minor;
      } else {
         rc = yvikeys_macro_recend ();
      }
      break;
   case 'Q'      :
      rc = yvikeys_macro_reset ();
      break;
   }
   if (rc >= 0) {
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exitr   (__FUNCTION__, rc);
   return rc;
}

char         /*-> complex delete action --------------[ ------ [gz.430.031.02]*/ /*-[01.0000.213.!]-*/ /*-[--.---.---.--]-*/
yvikeys__map_delete    (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        i           =    0;
   char        x_len       =    0;
   char        x_pos       =    0;
   char        x_minors    [LEN_LABEL]  = "hljk";
   /*---(header)-------------------------*/
   DEBUG_MAP    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_MAP    yLOG_char    ("s_live"    , s_live);
   --rce;  if (s_live != VISU_NOT) {
      DEBUG_MAP    yLOG_note    ("function only handles non-selected deletes/clearing");
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_MAP    yLOG_char    ("a_minor"   , chrvisible (a_minor));
   DEBUG_MAP    yLOG_info    ("valid"     , x_minors);
   --rce;  if (a_minor == 0 || strchr (x_minors, a_minor) == NULL) {
      DEBUG_MAP    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(action)-------------------------*/
   if (a_major == 'x') {
      yvikeys_regs_save  ();
      yvikeys_regs_clear ();
      VISU_clear    ();
   }
   /*---(move after)---------------------*/
   switch (a_minor) {
   case 'h' : rc = yvikeys__map_horz (G_KEY_SPACE, a_minor);         break;
   case 'l' : rc = yvikeys__map_horz (G_KEY_SPACE, a_minor);         break;
   case 'j' : rc = yvikeys__map_vert (G_KEY_SPACE, a_minor);         break;
   case 'k' : rc = yvikeys__map_vert (G_KEY_SPACE, a_minor);         break;
   }
   /*---(complete)-----------------------*/
   DEBUG_MAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_map_mode        (char a_major, char a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_grid      =    0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (MODE_MAP    )) {
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
   if (a_minor == G_KEY_RETURN) {
      DEBUG_USER   yLOG_note    ("enter to begin input");
      SOURCE_start ("¦");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape to clear selection");
      VISU_clear ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(single key)---------------------*/
   DEBUG_USER   yLOG_note    ("review single keys");
   --rce;
   if (a_major == ' ') {
      DEBUG_USER   yLOG_note    ("no or empty major");
      /*---(repeat)----------------------*/
      if (strchr (g_repeat, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("repeating");
         MODE_enter  (UMOD_REPEAT);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(multikey prefixes)-----------*/
      if (s_live == VISU_NOT && strchr (g_multimap , a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("prefix of multimap keystring");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      if (s_live == VISU_YES && strchr ("e" , a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("prefix of visual multimap keystring");
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return a_minor;
      }
      /*---(funky moves)-----------------*/
      if (a_minor == ':') {
         x_grid = REPEAT_use ();
         if (x_grid > 0) {
            rc = yvikeys__map_move   (x_grid, &g_ymap);
            yvikeys__screen (&g_ymap);
            yvikeys_map_reposition  ();
            DEBUG_USER   yLOG_exit    (__FUNCTION__);
            return rc;;
         }
         /* non-repeat fall thru  */
      }
      if (a_minor == '|') {
         rc = yvikeys__map_move   (REPEAT_use (), &g_xmap);
         yvikeys__screen (&g_xmap);
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(mode changes)----------------*/
      if (strchr (s_map_modes, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("mode changes");
         rc = yvikeys__map_mode_chg (a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr ("yYpP", a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("switch to a map register mode");
         MODE_enter (SMOD_MREG);
         rc = yvikeys_regs_smode  (G_KEY_SPACE, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(history)---------------------*/
      if (a_minor == 'u') {
         VISU_clear ();
         rc = yvikeys_hist_undo ();
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (a_minor == 'U') {
         VISU_clear ();
         rc = yvikeys_hist_redo ();
         yvikeys_map_reposition  ();
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(normal)----------------------*/
      if (strchr (g_hsimple, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vsimple, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_search, a_minor) != 0) {
         rc = SRCH_next   (a_minor);
         if (rc >= 0) {
            yvikeys__screen (&g_xmap);
            yvikeys__screen (&g_ymap);
            yvikeys__screen (&g_zmap);
            yvikeys_map_reposition  ();
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      DEBUG_USER   yLOG_note    ("no matches found");
   }
   /*---(goto family)--------------------*/
   if (a_major == 'g') {
      if (strchr (g_hgoto, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vgoto, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
   }
   /*---(ends family)--------------------*/
   if (a_major == 'e') {
      if (strchr ("azud", a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_hends, a_minor) != 0) {
         rc = yvikeys__map_horz   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr (g_vends, a_minor) != 0) {
         rc = yvikeys__map_vert   (a_major, a_minor);
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      if (strchr ("xyz*!", a_minor) != 0) {
         switch (a_minor) {
         case 'x'  :
            DEBUG_USER   yLOG_note    ("x for x_axis/col selection");
            if (s_visu.y_lock == 'y') {
               s_visu.y_lock = '-';
               if (s_visu.y_root <= g_ymap.gcur) {
                  s_visu.y_beg  = s_visu.y_root;
                  s_visu.y_end  = g_ymap.gcur;
               } else {
                  s_visu.y_end  = s_visu.y_root;
                  s_visu.y_beg  = g_ymap.gcur;
               }
            } else {
               s_visu.y_lock = 'y';
               s_visu.y_beg  = g_ymap.gmin;
               s_visu.y_end  = g_ymap.gmax;
            }
            break;
         case 'y'  :
            DEBUG_USER   yLOG_note    ("y for y-axis/row selection");
            if (s_visu.x_lock == 'y') {
               s_visu.x_lock = '-';
               if (s_visu.x_root <= g_xmap.gcur) {
                  s_visu.x_beg  = s_visu.x_root;
                  s_visu.x_end  = g_xmap.gcur;
               } else {
                  s_visu.x_end  = s_visu.x_root;
                  s_visu.x_beg  = g_xmap.gcur;
               }
            } else {
               s_visu.x_lock = 'y';
               s_visu.x_beg  = g_xmap.gmin;
               s_visu.x_end  = g_xmap.gmax;
            }
            break;
         case '!'  :
            DEBUG_USER   yLOG_note    ("! for screen selection");
            s_visu.y_lock = '-';
            s_visu.y_root = g_ymap.gbeg;
            s_visu.y_beg  = g_ymap.gbeg;
            s_visu.y_end  = g_ymap.gend;
            s_visu.x_lock = '-';
            s_visu.x_root = g_xmap.gbeg;
            s_visu.x_beg  = g_xmap.gbeg;
            s_visu.x_end  = g_xmap.gend;
            yVIKEYS_jump (s_visu.b_all, s_visu.x_end, s_visu.y_end, s_visu.z_all);
            break;
         case '*'  :
            DEBUG_USER   yLOG_note    ("* for all on current z selection");
            s_visu.y_lock = 'y';
            s_visu.y_beg  = g_ymap.gmin;
            s_visu.y_end  = g_ymap.gmax;
            s_visu.x_lock = 'y';
            s_visu.x_beg  = g_xmap.gmin;
            s_visu.x_end  = g_xmap.gmax;
            break;
         }
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return rc;
      }
   }
   /*---(paste family)-------------------*/
   if (a_major == 'p') {
      switch (a_minor) {
      case '_' :  rc = yvikeys_regs_visual ();            break;
      case '#' :  rc = yvikeys_regs_paste  ("clear");     break;
      case 'n' :  rc = yvikeys_regs_paste  ("normal");    break;
      case 'r' :  rc = yvikeys_regs_paste  ("replace");   break;
      case 'd' :  rc = yvikeys_regs_paste  ("duplicate"); break;
      case 'm' :  rc = yvikeys_regs_paste  ("move");      break;
      case 'f' :  rc = yvikeys_regs_paste  ("force");     break;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(delete)-------------------------*/
   if (a_major == 'x') {
      rc = yvikeys__map_delete  (a_major, a_minor);
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rc;
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
   return rc;
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
   --rce;  if (MODE_not (UMOD_WANDER )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(space)--------------------------*/
   if (a_minor == G_KEY_SPACE ) {
      DEBUG_USER   yLOG_note    ("space, nothing to do");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
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
yvikeys_map_xstatus     (char *a_list)
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
yvikeys_map_ystatus     (char *a_list)
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

char
yvikeys_bufs_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_prep  (SMOD_BUFFER)) {
      DEBUG_PROG   yLOG_note    ("status is not ready for init");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(clear callbacks)----------------*/
   DEBUG_PROG   yLOG_note    ("clear callbacks");
   s_switcher = NULL;
   /*---(update status)------------------*/
   STATUS_init_set   (SMOD_BUFFER);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_bufs_config     (void *a_switcher)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (SMOD_BUFFER)) {
      DEBUG_PROG   yLOG_note    ("init must complete before config");
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(globals)------------------------*/
   s_switcher   = a_switcher;
   /*---(update status)------------------*/
   STATUS_conf_set   (SMOD_BUFFER, '1');
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> process keys for buffer movement ---[ leaf   [ge.F64.217.CA]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_bufs_umode  (uchar a_major, uchar a_minor)
{
   /*---(design notes)-------------------*/
   /*
    *   this should imitate our RBUF capability in vimm
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(request buffer mode)------------*/
   DEBUG_USER   yLOG_value   ("SMOD_BUF"  , MODE_not (SMOD_BUFFER));
   if (a_major != ' ' && a_minor == ',') {
      DEBUG_USER   yLOG_note    ("enter buffer mode");
      if (MODE_not (SMOD_BUFFER))  MODE_enter  (SMOD_BUFFER  );
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_minor;
   }
   /*---(defenses)-----------------------*/
   if (a_major == ',' && MODE_not (SMOD_BUFFER)) {
      DEBUG_USER   yLOG_note    ("force enter buffer mode");
      MODE_enter  (SMOD_BUFFER  );
   }
   --rce;  if (a_major != ',') {
      DEBUG_USER   yLOG_note    ("a_major is wrong)");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (MODE_not (SMOD_BUFFER)) {
      DEBUG_USER   yLOG_note    ("not in buffer mode");
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mode changes)-------------------*/
   if (a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("escape, choose nothing");
      /*> my.menu = ' ';                                                              <*/
      MODE_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for control keys)---------*/
   --rce;
   if (strchr ("[<>]", a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("relative mode");
      rc = s_switcher (a_minor);
      MODE_exit  ();
   } else if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ®¯", a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("absolute mode");
      rc = s_switcher (a_minor);
      MODE_exit  ();
   } else {
      MODE_exit  ();
      DEBUG_USER   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          unit testing                        ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

static struct {
   char        label       [LEN_LABEL];
   int         b, x, y, z;
} s_map_unit_deref [100] = {
   { "0a1"        ,   0,   0,   0,   0 },
   { "0b3"        ,   0,   1,   2,   0 },
   { "0c5"        ,   0,   2,   4,   0 },
   { "0f11"       ,   0,   5,  10,   0 },
   { "0g11"       ,   0,   6,  10,   0 },
   { "0k11"       ,   0,  10,  10,   0 },
   { "0p11"       ,   0,  15,  10,   0 },
   { "0p12"       ,   0,  15,  11,   0 },
   { "2b5"        ,   2,   1,   4,   0 },
   { "3d6"        ,   3,   3,   5,   0 },
   { "5b2"        ,   5,   1,   1,   0 },
   { "6d3"        ,   6,   3,   2,   0 },
   { "6g11"       ,   7,   6,  10,   0 },
   { "Ag11"       ,  10,   6,  10,   0 },
   { "Mp11"       ,  22,  15,  10,   0 },
   { "Mp12"       ,  22,  15,  11,   0 },
   { "Za1"        ,  35,   0,   0,   0 },
   { "3:11"       ,   0,   4,  12,   0 },
   { "0c2"        ,   0,   2,   1,   0 },
   { "0e4"        ,   0,   4,   3,   0 },
   { NULL         ,   0,   0,   0,   0 },

};

char
yvikeys__unit_map_ymap  (void)
{
   /*> printf ("running rowmap\n");                                                   <*/
   yvikeys__map_load ('1', &g_ymap, YVIKEYS_YMAP);
   return 0;
}

char
yvikeys__unit_map_xmap  (void)
{
   /*> printf ("running colmap\n");                                                   <*/
   yvikeys__map_load ('w', &g_xmap, YVIKEYS_YMAP);
   return 0;
}

char
yvikeys__unit_map_zmap  (void)
{
   return 0;
}

char
yvikeys__unit_map_map   (char a_type)
{
   char        rc          = 0;
   char        t           [LEN_LABEL]  = "";
   rc = yvikeys__unit_map_addr (t, g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
   yVIKEYS_source (t, "testing");
   return 0;
}

char
yvikeys__unit_map_loc   (char *a_label, int *a_buf, int *a_x, int *a_y, int *a_z)
{
   int         i           =    0;
   int         n           =   -1;
   DEBUG_MAP   yLOG_senter  (__FUNCTION__);
   DEBUG_MAP   yLOG_snote   (a_label);
   for (i = 0; i < 100; ++i) {
      if (s_map_unit_deref [i].label == NULL)                 break;
      if (s_map_unit_deref [i].label [0] != a_label [0])      continue;
      if (strcmp (s_map_unit_deref [i].label, a_label) != 0)  continue;
      n = i;
      break;
   }
   DEBUG_MAP   yLOG_sint    (n);
   *a_buf = -1;
   *a_x   = -1;
   *a_y   = -1;
   *a_z   = -1;
   if (n < 0) {
      DEBUG_MAP   yLOG_sexitr  (__FUNCTION__, n);
      return n;
   }
   *a_buf = s_map_unit_deref [i].b;
   *a_x   = s_map_unit_deref [i].x;
   *a_y   = s_map_unit_deref [i].y;
   *a_z   = s_map_unit_deref [i].z;
   DEBUG_MAP   yLOG_sint    (*a_buf);
   DEBUG_MAP   yLOG_sint    (*a_x);
   DEBUG_MAP   yLOG_sint    (*a_y);
   DEBUG_MAP   yLOG_sint    (*a_z);
   /*> yVIKEYS_source (a_label, "testing");                                           <*/
   DEBUG_MAP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char 
yvikeys__unit_map_addr  (char *a_label, int a_buf, int a_x, int a_y, int a_z)
{
   int         i           =    0;
   int         n           =   -1;
   DEBUG_MAP   yLOG_senter  (__FUNCTION__);
   DEBUG_MAP   yLOG_sint    (a_buf);
   DEBUG_MAP   yLOG_sint    (a_x);
   DEBUG_MAP   yLOG_sint    (a_y);
   DEBUG_MAP   yLOG_sint    (a_z);
   for (i = 0; i < 100; ++i) {
      if (s_map_unit_deref [i].label == NULL )     break;
      if (s_map_unit_deref [i].b     != a_buf)     continue;
      if (s_map_unit_deref [i].x     != a_x  )     continue;
      if (s_map_unit_deref [i].y     != a_y  )     continue;
      if (s_map_unit_deref [i].z     != a_z  )     continue;
      n = i;
      break;
   }
   DEBUG_MAP   yLOG_sint    (n);
   if (n < 0) {
      strlcpy (a_label, "-", LEN_LABEL);
      DEBUG_MAP   yLOG_sexitr  (__FUNCTION__, n);
      return -1;
   }
   strlcpy (a_label, s_map_unit_deref [n].label, LEN_LABEL);
   DEBUG_MAP   yLOG_snote   (a_label);
   DEBUG_MAP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys__unit_quick     (void)
{
   yvikeys__map_load ('1', &g_bmap, YVIKEYS_BMAP);
   yvikeys__map_load ('1', &g_xmap, YVIKEYS_XMAP);
   yvikeys__map_load ('1', &g_ymap, YVIKEYS_YMAP);
   yvikeys__map_load ('1', &g_zmap, YVIKEYS_ZMAP);
   yVIKEYS_map_config (YVIKEYS_OFFICE, yvikeys__unit_map_map, yvikeys__unit_map_loc, yvikeys__unit_map_addr);
   return 0;
}

char*
yVIKEYS__unit           (char *a_question, int a_num)
{
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "yVIKEYS unit     : question not understood", LEN_STR);
   /*---(questions)----------------------*/
   if (strcmp (a_question, "map_curr"       )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "yVIKEYS map curr : %3dx, %3dy, %3dz", g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys__unit_map       (char *a_question, char a_index)
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
   else if (strcmp (a_question, "current"        )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "MAP current      : %3db, %3dx, %3dy, %3dz", g_bmap.gcur, g_xmap.gcur, g_ymap.gcur, g_zmap.gcur);
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
   n = VISU_valid (a_which);
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "selection"      )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU selection   : %c  %4d to %4dx,  %4d to %4dy,  %4d to %4dz", s_live, s_visu.x_beg, s_visu.x_end, s_visu.y_beg, s_visu.y_end, s_visu.z_all, s_visu.z_all);
   }
   else if (strcmp (a_question, "cursor"         )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU cursor      : %c  %4dx,  %4dy,  %4dz", s_live, s_visu.x_beg, s_visu.x_end, s_visu.y_beg, s_visu.y_end, s_visu.z_all, s_visu.z_all);
   }
   else if (strcmp (a_question, "range"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU range       : %c to %c,  %c", s_visu_head, s_visu_tail, s_visu_curr);
   }
   else if (strcmp (a_question, "saved"          )   == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_STR, "VISU saved   (%c) : %c  %4d to %4dx,  %4d to %4dy,  %4d to %4dz", a_which, s_visu_info [n].active, s_visu_info [n].x_beg, s_visu_info [n].x_end, s_visu_info [n].y_beg, s_visu_info [n].y_end, s_visu_info [n].z_all, s_visu_info [n].z_all);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


