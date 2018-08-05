/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


static char    (*s_undo) (void);
static char    (*s_redo) (void);

char
yvikeys_hist_init       (void)
{
   s_undo         = NULL;
   s_redo         = NULL;
   STATUS_init_set   (UMOD_HISTORY);
   return 0;
}

char         /*-> initialize history environment -----[ shoot  [gz.210.001.01]*/ /*-[00.0000.102.4]-*/ /*-[--.---.---.--]-*/
yVIKEYS_hist_config     (void *a_undo, void *a_redo)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_EDIT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (!STATUS_check_needs  (UMOD_HISTORY)) {
      DEBUG_EDIT   yLOG_note    ("init must complete before config");
      DEBUG_EDIT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save pointer)-------------------*/
   if (a_undo != NULL)  s_undo  = a_undo;
   if (a_redo != NULL)  s_redo  = a_redo;
   /*---(update status)------------------*/
   STATUS_conf_set   (UMOD_HISTORY, '1');
   /*---(complete)-----------------------*/
   DEBUG_EDIT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_hist_undo       (void)
{
   if (s_undo != NULL)  return s_undo ();
   return -1;
}

char
yvikeys_hist_redo       (void)
{
   if (s_redo != NULL)  return s_redo ();
   return -1;
}


