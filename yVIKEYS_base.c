/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 * metis  tn4∑∑  main loop must kick into immediate mode when close group is found
 *
 */




static char  s_keys_log   [LEN_FULL];
static char  s_keys_multi [LEN_FULL];
static char  s_keys_mode  [LEN_FULL];
static char  s_keys_error [LEN_FULL];
static int   s_nkey      = 0;
static int   s_akey      = 0;
static int   s_gpos      = 0;
static char  s_keys_last  [LEN_LABEL];
static int   s_acks      = 0;
static int   s_spaces    = 0;
static int   s_noops     = 0;


typedef struct cDUMP tDUMP;
static struct cDUMP {
   char       *name;
   char        (*provider) (FILE*);           /* function pointer               */
   tDUMP      *next;
};
static tDUMP      *s_hdump     = NULL;
static tDUMP      *s_tdump     = NULL;
static int         s_ndump     =    0;



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
   snprintf (yVIKEYS_ver, 100, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return yVIKEYS_ver;
}

char         /*-> initialize all of yvikeys ----------[ leaf   [gn.530.341.50]*/ /*-[02.0000.000.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_init         (char a_mode)
{
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*----(first)-------------------------*/
   STATUS_init  ();
   srand (time (NULL));
   /*----(early)-------------------------*/
   yPARSE_init  ('y', NULL, '-');
   yPARSE_delimiters  ("");
   MODE_init            (a_mode);
   yvikeys_view_init    ();
   yvikeys_file_init    ();
   /*----(middling)----------------------*/
   yvikeys_menu_init    ();
   yvikeys_cmds_init    ();
   yvikeys_srch_init    ();
   /*----(later)-------------------------*/
   yvikeys_keys_init    ();
   yvikeys_map_init     ();
   SOURCE_init  ();
   yvikeys_macro_init   ();
   /*----(latest)------------------------*/
   yvikeys_mreg_init    ();
   yvikeys_sreg_init    ();
   yvikeys_sundo_init   ();
   yvikeys_mark_init    ();
   yvikeys_hint_init    ();
   yvikeys_visu_init    ();
   REPEAT_init  ();
   yvikeys_bufs_init    ();
   yvikeys_hist_init    ();
   yvikeys_loop_init    ();
   yvikeys_menu_final   ();
   yvikeys_dump_init    ();
   yvikeys_layer_init   ();
   yvikeys_sizes_init   ();
   STATUS_init_set   (UMOD_SENDKEYS);
   /*----(globals)-----------------------*/
   myVIKEYS.loud      = '-';
   myVIKEYS.done      = '-';
   myVIKEYS.trouble   = '-';
   myVIKEYS.info_win  = '-';
   myVIKEYS.log_keys  = 'y';
   myVIKEYS.cursor    = 'y';
   myVIKEYS.status_w  = 'L';
   myVIKEYS.font      = -1;
   myVIKEYS.fancy     = -1;
   /*----(complete)----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yVIKEYS_wrap         (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   if (myVIKEYS.font  >= 0)  yFONT_free (myVIKEYS.font);
   if (myVIKEYS.fancy >= 0)  yFONT_free (myVIKEYS.fancy);
   yPARSE_wrap ();
   DEBUG_PROG   yLOG_note    ("parse done");
   VIEW_wrap   ();
   DEBUG_PROG   yLOG_note    ("view done");
   STATUS_wrap ();
   DEBUG_PROG   yLOG_note    ("status done");
   yvikeys_mreg_wrap   ();
   DEBUG_PROG   yLOG_note    ("mreg done");
   yvikeys_menu_wrap   ();
   DEBUG_PROG   yLOG_note    ("menu done");
   yvikeys_dump_purge  ();
   DEBUG_PROG   yLOG_note    ("dump done");
   yvikeys_hist_wrap   ();
   DEBUG_PROG   yLOG_note    ("hist done");
   yvikeys_layer_wrap  ();
   DEBUG_PROG   yLOG_note    ("layer done");
   yvikeys_cmds_wrap    ();
   DEBUG_PROG   yLOG_note    ("commands done");
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char yVIKEYS_quit            (void) { if (myVIKEYS.done    == 'y') return 1; return 0; }
char yVIKEYS_error           (void) { if (myVIKEYS.trouble != '-') return 1; return 0; }

char
yVIKEYS_simple          (cchar *a_title, cchar *a_ver, cchar a_env, cint a_wide, cint a_tall, char a_align, char a_mode, uchar *a_fixed, uchar *a_fancy, void *a_drawer)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         x_left      =    0;
   int         x_bott      =    0;
   char        t           [LEN_DESC]  = "";
   char       *p           = NULL;
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   rc = yVIKEYS_init (a_mode);
   DEBUG_PROG   yLOG_value   ("init"      , rc);
   rc = yVIKEYS_view_config (a_title, a_ver, a_env, a_wide, a_tall, 0);
   strlcpy (t, a_title, LEN_DESC);
   p = strchr (t, '-');
   if (p != NULL)  p [0] = '\0';
   strlcpy (myVIKEYS.s_prog  , t      , LEN_LABEL);
   strlcpy (myVIKEYS.s_vernum, a_ver  , LEN_LABEL);
   DEBUG_PROG   yLOG_value   ("config"    , rc);
   rc = yVIKEYS_view_fonts  (a_fixed, a_fancy);
   DEBUG_PROG   yLOG_value   ("fonts"     , rc);
   rc = yVIKEYS_view_setup    (YVIKEYS_MAIN , YVIKEYS_FLAT, a_align, 0, 0, 0, 0, 0, 0, 0, a_drawer);
   DEBUG_PROG   yLOG_value   ("setup"     , rc);
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                        data dumping                          ----===*/
/*====================------------------------------------====================*/
static void  o___DUMPING_________o () { return; }

char
yVIKEYS_dump_add        (char *a_name, void *a_provider)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   tDUMP      *x_new       = NULL;
   char        x_tries     =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_spoint  (a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return  rce;
   }
   x_len = strlen (a_name);
   DEBUG_PROG   yLOG_sint    (x_len);
   --rce;  if (x_len < 3) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return  rce;
   }
   DEBUG_PROG   yLOG_spoint  (a_provider);
   --rce;  if (a_provider == NULL) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return  rce;
   }
   /*---(allocate)-----------------------*/
   while (x_new == NULL && x_tries < 10)  {
      ++x_tries;
      x_new = (tDUMP *) malloc (sizeof (tDUMP));
   }
   DEBUG_PROG   yLOG_sint    (x_tries);
   DEBUG_PROG   yLOG_spoint  (x_new);
   --rce;  if (x_new == NULL) {
      DEBUG_PROG   yLOG_snote   ("FAILED");
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return  rce;
   }
   /*---(tie to linked list)-------------*/
   DEBUG_PROG   yLOG_snote   ("link");
   x_new->next   = NULL;
   if (s_hdump == NULL)  s_hdump       = x_new;
   else                  s_tdump->next = x_new;
   s_tdump       = x_new;
   /*---(populate)-----------------------*/
   DEBUG_PROG   yLOG_snote   ("populate");
   x_new->name     = strdup (a_name);
   x_new->provider = a_provider;
   /*---(complete)-----------------------*/
   ++s_ndump;
   DEBUG_PROG   yLOG_sint    (s_ndump);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_dump_exec       (char *a_name)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tDUMP      *x_curr      = NULL;
   FILE       *f           = NULL;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(execute)------------------------*/
   x_curr = s_hdump;
   while (x_curr != NULL) {
      if (a_name [0] == x_curr->name [0])  {
         if (strcmp (a_name, x_curr->name) == 0) {
            /*---(open file)-------------*/
            f = fopen (FILE_CLIP, "w");
            DEBUG_PROG   yLOG_point   ("f"         , f);
            if (f == NULL) {
               DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
            }
            rc = x_curr->provider (f);
            DEBUG_PROG   yLOG_value   ("rc"        , rc);
            fclose (f);
            DEBUG_PROG   yLOG_exit    (__FUNCTION__);
            return rc;
         }
      }
      x_curr = x_curr->next;
   }
   DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
   return rce;
}

/*> char                                                                              <* 
 *> yvikeys_clip_dump       (char *a_what)                                            <* 
 *> {                                                                                 <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                       <* 
 *>    char        rce         =  -10;                                                <* 
 *>    FILE       *f           = NULL;                                                <* 
 *>    /+---(defense)------------------------+/                                       <* 
 *>    --rce;  if (a_what == NULL)  return rce;                                       <* 
 *>    /+---(open file)----------------------+/                                       <* 
 *>    f = fopen (FILE_CLIP, "w");                                                    <* 
 *>    --rce;  if (f == NULL)       return rce;                                       <* 
 *>    /+---(dump)---------------------------+/                                       <* 
 *>    if      (strcmp (a_what, "keys"      ) == 0)  KEYS_dump          (f);          <* 
 *>    else if (strcmp (a_what, "status"    ) == 0)  STATUS_dump        (f);          <* 
 *>    else if (strcmp (a_what, "macros"    ) == 0)  yvikeys_macro_dump (f);          <* 
 *>    else if (strcmp (a_what, "sreg"      ) == 0)  yvikeys_sreg_dump  (f);          <* 
 *>    /+---(close)--------------------------+/                                       <* 
 *>    fclose (f);                                                                    <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/

char
yvikeys_dump_write      (cchar *a_recd)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   FILE       *f           = NULL;
   char        rc          =    0;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_spoint  ( a_recd);
   --rce;  if (a_recd == NULL) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(open)---------------------------*/
   f = fopen (FILE_CLIP, "w");
   DEBUG_SCRP   yLOG_spoint  (f);
   --rce;  if (f == NULL) {
      DEBUG_PROG   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(write)--------------------------*/
   x_len = fprintf (f, "%s\n", a_recd);
   DEBUG_SCRP   yLOG_sint    (x_len);
   /*---(close)--------------------------*/
   rc    = fclose (f);
   DEBUG_SCRP   yLOG_sint    (rc);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_dump_read       (int a_line, char *a_recd, int *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   FILE       *f           = NULL;
   char        x_recd      [LEN_RECD] = "";
   int         x_len       =    0;
   int         c           =    0;
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_SCRP   yLOG_spoint  (a_recd);
   --rce;  if (a_recd == NULL) {
      DEBUG_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(defaults)-----------------------*/
   if (a_len != NULL)  *a_len = 0;
   strlcpy (a_recd, "", LEN_RECD);
   /*---(open)---------------------------*/
   f = fopen (FILE_CLIP, "r");
   DEBUG_SCRP   yLOG_spoint  (f);
   --rce;  if (f == NULL) {
      DEBUG_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(read)---------------------------*/
   --rce;  while (c <= a_line)  {
      fgets (x_recd, LEN_RECD, f);
      if (feof (f)) {
         DEBUG_SCRP   yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      ++c;
   }
   DEBUG_SCRP   yLOG_sint    (c);
   /*---(close)--------------------------*/
   rc = fclose (f);
   DEBUG_SCRP   yLOG_sint    (rc);
   /*---(clean)--------------------------*/
   x_len = strlen (x_recd);
   if (x_len > 0) {
      DEBUG_SCRP   yLOG_sint    (x_len);
      DEBUG_SCRP   yLOG_schar   (chrvisible (x_recd [x_len - 1]));
      if (x_recd [x_len - 1] == '\n')  x_recd [--x_len] = '\0';
      if (x_len > 0) {
         DEBUG_SCRP   yLOG_schar   (chrvisible (x_recd [x_len - 1]));
         if (x_recd [x_len - 1] == '≥' )  x_recd [--x_len] = '\0';
      }
   }
   /*---(copy)---------------------------*/
   strlcpy (a_recd, x_recd, LEN_RECD);
   if (a_len != NULL)  *a_len = x_len;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yvikeys_dump_init       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   DEBUG_SCRP   yLOG_enter   (__FUNCTION__);
   s_hdump = s_tdump = NULL;
   s_ndump = 0;
   rc = yVIKEYS_dump_add ("keys"        , yvikeys_keys_dump   );
   rc = yVIKEYS_dump_add ("status"      , STATUS_dump         );
   rc = yVIKEYS_dump_add ("macros"      , yvikeys_macro_dump  );
   rc = yVIKEYS_dump_add ("sreg"        , yvikeys_sreg_dump   );
   DEBUG_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yvikeys_dump_purge      (void)
{
   tDUMP      *x_next      = NULL;
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   x_next = s_hdump;
   while (x_next != NULL) {
      DEBUG_SCRP   yLOG_sint    (s_ndump);
      x_next  = x_next->next;
      free (s_hdump->name);
      free (s_hdump);
      s_hdump = x_next;
      --s_ndump;
   }
   s_hdump = s_tdump = NULL;
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        key logging                           ----===*/
/*====================------------------------------------====================*/
static void  o___LOGGING_________o () { return; }

char
yvikeys_keys__roll      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_off       = LEN_FULL / 2;
   char        t           [LEN_FULL];
   /*---(header)-------------------------*/
   DEBUG_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(log)----------------------------*/
   DEBUG_SCRP   yLOG_snote   ("log");
   strlcpy (t, s_keys_log   + x_off, LEN_FULL);
   strlcpy (s_keys_log  , t        , LEN_FULL);
   /*---(mode)---------------------------*/
   DEBUG_SCRP   yLOG_snote   ("mode");
   strlcpy (t, s_keys_mode  + x_off, LEN_FULL);
   strlcpy (s_keys_mode , t        , LEN_FULL);
   /*---(error)--------------------------*/
   DEBUG_SCRP   yLOG_snote   ("error");
   strlcpy (t, s_keys_error + x_off, LEN_FULL);
   strlcpy (s_keys_error, t        , LEN_FULL);
   /*---(multi)--------------------------*/
   DEBUG_SCRP   yLOG_snote   ("multi");
   strlcpy (t, s_keys_multi + x_off, LEN_FULL);
   strlcpy (s_keys_multi, t        , LEN_FULL);
   /*---(position)-----------------------*/
   s_nkey -= x_off;
   /*---(complete)-----------------------*/
   DEBUG_SCRP   yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ leaf   [gz.430.151.10]*/ /*-[00.0000.104.!]-*/ /*-[--.---.---.--]-*/
yvikeys_keys_status     (char *a_msg)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD];
   int         x_key       = 0;
   int         x_len       = 0;             /* string length                  */
   int         i           = 0;             /* iterator -- keys               */
   int         x_start     = 0;             /* iterator -- keys               */
   x_len = strlen (s_keys_log) - 1;
   x_start = x_len - 39;
   if (x_start < 0) x_start = 0;
   snprintf (a_msg, 500, "keys    %-5d %s§", s_akey, s_keys_log + x_start);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_keys__multi     (int a_pos)
{
   char        x_mode      =  ' ';
   /*---(defense)------------------------*/
   if (a_pos < 0 || a_pos >  s_nkey)                             return 0;
   if (s_keys_log [a_pos] == G_KEY_NULL )                        return 0;
   if (s_keys_log [a_pos] == G_KEY_SPACE)                        return 0;
   if (s_keys_log [a_pos] == G_CHAR_SPACE)                       return 0;
   /*---(key for mode)-------------------*/
   x_mode = s_keys_mode [a_pos];
   if (a_pos > 0 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 1];
   if (a_pos > 1 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 2];
   if (a_pos > 2 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 3];
   if (a_pos > 3 && x_mode == UMOD_REPEAT)  x_mode = s_keys_mode [a_pos - 4];
   switch (x_mode) {
   case MODE_SOURCE  : case MODE_COMMAND : case MODE_SEARCH  : case SMOD_HINT  :
      if (strchr (g_multisrc, s_keys_log [a_pos]) != NULL)       return 1;
      break;
   case MODE_MAP     :
      if (strchr (g_multimap, s_keys_log [a_pos]) != NULL)       return 1;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_keys__logger    (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   int         x_key       =    0;
   int         x_multi     =  '-';
   /*---(key)----------------------------*/
   x_key = chrvisible (a_key);
   s_keys_log   [s_nkey]     = x_key;
   s_keys_log   [s_nkey + 1] = 0;
   /*---(mode)---------------------------*/
   s_keys_mode  [s_nkey]     = MODE_curr ();
   s_keys_mode  [s_nkey + 1] = 0;
   /*---(error)--------------------------*/
   s_keys_error [s_nkey]     = '-';
   s_keys_error [s_nkey + 1] = 0;
   /*---(multi)--------------------------*/
   x_multi = yvikeys_keys__multi (s_nkey);
   switch (s_keys_multi [s_nkey - 1]) {
   case 'p' :
      s_keys_multi [s_nkey] = 's';
      break;
   case 's' : case '-' : default  :
      if (x_multi)  s_keys_multi [s_nkey] = 'p';
      else          s_keys_multi [s_nkey] = '-';
      break;
   }
   s_keys_multi [s_nkey + 1] = 0;
   /*---(update count)-------------------*/
   ++s_akey;
   ++s_nkey;
   ++s_gpos;
   /*---(roll)---------------------------*/
   if (s_nkey >= LEN_FULL - 2)  yvikeys_keys__roll ();
   /*---(macro)--------------------------*/
   IF_MACRO_RECORDING {
      yvikeys_macro_reckey (x_key);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_set_error       (void)
{
   s_keys_error [s_nkey - 1] = 'E';
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [gz.420.121.11]*/ /*-[01.0000.102.!]-*/ /*-[--.---.---.--]-*/
yvikeys_keys_unique     (void)
{  /*    return 1 if not a repeat sequence, 0 if repeating   */
   /*    five mode options :    - -    - p    p s    s -    s p
   */
   /*---(locals)-----------+-----+-----+-*/
   uchar       m1, m2, m3, m4;
   uchar       c1, c2, c3, c4;
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_senter  (__FUNCTION__);
   DEBUG_LOOP   yLOG_sint    (s_nkey);
   /*---(basic defense)------------------*/
   if (s_nkey < 2) {
      DEBUG_LOOP   yLOG_snote   ("too few keys, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   /*---(prepare)------------------------*/
   c3 = s_keys_log   [s_nkey - 2];
   m3 = s_keys_multi [s_nkey - 2];
   c4 = s_keys_log   [s_nkey - 1];
   m4 = s_keys_multi [s_nkey - 1];
   /*---(filter)-------------------------*/
   DEBUG_LOOP   yLOG_sint    (m3);
   if (strchr ("ps-", m3) == NULL) {
      DEBUG_LOOP   yLOG_snote   ("m3 is weird, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (m4);
   if (strchr ("ps-", m4) == NULL) {
      DEBUG_LOOP   yLOG_snote   ("m4 is weird, unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   if (m3 == 's' && m4 == '-') {
      DEBUG_LOOP   yLOG_snote   ("coming off combo, obviously unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   if (m4 == 'p') {
      DEBUG_LOOP   yLOG_snote   ("starting combo, obviously unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (c3);
   if (c3 == G_CHAR_SPACE) {
      DEBUG_LOOP   yLOG_snote   ("c3 is space, so unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   DEBUG_LOOP   yLOG_sint    (c4);
   if (c4 == G_CHAR_SPACE) {
      DEBUG_LOOP   yLOG_snote   ("c4 is space, so unique");
      DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
      return 1;
   }
   /*---(single)-------------------------*/
   if (m3 == '-' && m4 == '-' && c3 == c4)       return 0;
   /*---(filter)-------------------------*/
   if (s_nkey < 4)                               return 1;
   /*---(prepare)------------------------*/
   c1 = s_keys_log   [s_nkey - 4];
   m1 = s_keys_multi [s_nkey - 4];
   c2 = s_keys_log   [s_nkey - 3];
   m2 = s_keys_multi [s_nkey - 3];
   /*---(filter)-------------------------*/
   if (strchr ("ps-", m1) == NULL)               return 0;
   if (strchr ("ps-", m2) == NULL)               return 0;
   if (m1 != 'p')                                return 1;
   if (m2 != 's')                                return 1;
   /*---(double)-------------------------*/
   if (c4 == c2 && c3 == c1)                     return 0;
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_sexitr  (__FUNCTION__, 1);
   return 1;
}

int  yVIKEYS_keys_nkey       (void) { return s_nkey; }
int  yvikeys_keys_gpos       (void) { return s_gpos; }

char
yvikeys_keys_keygpos    (void)
{
   if (s_gpos < 0)         return 0;
   if (s_gpos >= s_nkey)   return 0;
   return s_keys_log [s_gpos];
}

char
yvikeys_keys_repeating  (void)
{
   if (s_gpos < s_nkey)  return 1;
   return 0;
}

char*
yVIKEYS_keys_last       (void)
{
   sprintf (s_keys_last, "(%c) %c", s_keys_mode [s_nkey - 1], s_keys_log [s_nkey - 1]);
   return s_keys_last;
}

char
yvikeys_keys_dump       (FILE *a_file)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   /*---(header)-------------------------*/
   fprintf (a_file, "yVIKEYS, key logging report                                                               (:dump keys)\n");
   fprintf (a_file, "len = %d\n", s_nkey);
   fprintf (a_file, "          ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "%-10d", i);
   fprintf (a_file, "\n");
   fprintf (a_file, "type----  ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "-123456789");
   fprintf (a_file, "\n");
   fprintf (a_file, "keys      %s\n", s_keys_log);
   fprintf (a_file, "mode      %s\n", s_keys_mode);
   fprintf (a_file, "multi     %s\n", s_keys_multi);
   fprintf (a_file, "error     %s\n", s_keys_error);
   fprintf (a_file, "type----  ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "-123456789");
   fprintf (a_file, "\n");
   fprintf (a_file, "          ");
   for (i = 0; i < (s_nkey / 10) + 1; ++i)  fprintf (a_file, "%-10d", i);
   fprintf (a_file, "\n");
   /*---(complete)-----------------------*/
   return 0;
}

char
yvikeys_keys_init       (void)
{
   strlcpy (s_keys_log  , "", LEN_FULL);
   strlcpy (s_keys_mode , "", LEN_FULL);
   strlcpy (s_keys_multi, "", LEN_FULL);
   strlcpy (s_keys_error, "", LEN_FULL);
   s_nkey = 0;
   s_gpos = 0;
   yvikeys_view_keys ("-- -");
   return 0;
}

char
yvikeys_keys_repos      (int a_pos)
{
   s_gpos = a_pos;
}

static char  s_win   = '.';

char
yvikeys_sendkeys_prep   (void)
{
   s_win = '-';
   return 0;
}

char         /*--> process keystrokes in sendkeys mode ---[--------[--------]-*/
SENDKEYS_umode          (uchar a_major, uchar a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       t           [LEN_TERSE] = "";
   static char x_1st       =  ' ';
   static char x_2nd       =  ' ';
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , chrvisible (a_minor));
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , MODE_curr ());
   --rce;  if (MODE_not (UMOD_SENDKEYS)) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(major mode changes)-------------*/
   if (a_minor == G_KEY_RETURN || a_minor == G_KEY_ESCAPE) {
      DEBUG_USER   yLOG_note    ("enter/escape, leave progress mode");
      MODE_exit  ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_minor == '\\') {
      DEBUG_USER   yLOG_note    ("found a backslash prefix");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return a_minor;
   }
   if (strchr ("”‘", a_minor) != NULL) {
      DEBUG_USER   yLOG_note    ("found a control, alt, hyper, super prefix key");
      if (x_2nd != ' ')  x_1st = a_minor;
      else               x_2nd = a_minor;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check window)-------------------*/
   if (s_win == '-') {
      s_win = a_minor;
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(send key to current win)--------*/
   if (a_major == '\\')  a_minor = chrslashed (a_minor);
   if (a_minor == ' ')   a_minor = '≤';
   DEBUG_USER   yLOG_value   ("a_minor"   , a_minor);
   if      (x_1st != ' ')  sprintf (t, "%c %c%c%c", s_win, x_1st, x_2nd, a_minor);
   else if (x_2nd != ' ')  sprintf (t, "%c %c%c"  , s_win, x_2nd, a_minor);
   else                    sprintf (t, "%c %c"    , s_win, a_minor);
   DEBUG_USER   yLOG_info    ("t"         , t);
   rc = yX11_yvikeys_sendkeys   (t);
   x_1st = x_2nd = ' ';
   /*---(complete)------------------------------*/
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         main loop                            ----===*/
/*====================------------------------------------====================*/
static void  o___MAIN____________o () { return; }

char             /* [------] process the command line arguments --------------*/
yVIKEYS_args       (int a_argc, char *a_argv[])
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* loop iterator -- arguments     */
   char       *a           = NULL;          /* current argument               */
   int         len         = 0;             /* argument length                */
   int         x_total     = 0;
   int         x_args      = 0;
   /*---(begin)--------------------------*/
   DEBUG_TOPS   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (i = 1; i < a_argc; ++i) {
      /*---(read)------------------------*/
      a   = a_argv [i];
      len = strlen(a);
      ++x_total;
      /*---(filter)----------------------*/
      if (a[0] == '@')  continue;
      ++x_args;
      DEBUG_ARGS  yLOG_complex ("argument"  , "%2d of %2d, %s", i, a_argc, a);
      /*---(configuration)---------------*/
      if      (strcmp (a, "--nav"          ) == 0)    yVIKEYS_cmds_direct   (":nav show");
      else if (strcmp (a, "--nonav"        ) == 0)    yVIKEYS_cmds_direct   (":nav hide");
      else if (strcmp (a, "--progress"     ) == 0)    yVIKEYS_cmds_direct   (":progress show");
      else if (strcmp (a, "--noprogress"   ) == 0)    yVIKEYS_cmds_direct   (":progress hide");
      else if (strcmp (a, "--play"         ) == 0)    yVIKEYS_cmds_direct   (":play");
      else if (strcmp (a, "--stop"         ) == 0)    yVIKEYS_cmds_direct   (":stop");
      else if (strcmp (a, "--status"       ) == 0)    yVIKEYS_cmds_direct   (":status show");
      else if (strcmp (a, "--nostatus"     ) == 0)    yVIKEYS_cmds_direct   (":status hide");
      else if (strcmp (a, "--script"       ) == 0) {
         DEBUG_TOPS   yLOG_note    ("found --script option");
         if (i < a_argc)  strlcpy (myVIKEYS.m_script, a_argv [++i], LEN_DESC);
      }
   }
   DEBUG_TOPS   yLOG_info    ("m_script"  , myVIKEYS.m_script);
   /*---(complete)-----------------------*/
   DEBUG_TOPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

uchar        /*-> gather main loop keyboard input ----[ ------ [gc.D44.233.C7]*/ /*-[02.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_input      (char a_runmode, uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   uchar       x_ch        = ' ';
   uchar       x_play      = ' ';
   char        x_used      =   0;
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_char    ("a_runmode" , a_runmode);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   DEBUG_LOOP   yLOG_char    ("macro rec" , yvikeys_macro_rmode ());
   /*---(fixes)--------------------------*/
   if (myVIKEYS.env == YVIKEYS_OPENGL) {
      if (a_key == G_KEY_ENTER)  a_key = G_KEY_RETURN; /* X11 sends incorrently  */
      if (a_key == G_KEY_DEL  )  a_key = G_KEY_BS;     /* X11 sends incorrectly  */
   }
   /*---(normal)-------------------------*/
   IF_MACRO_NOT_PLAYING {
      /*---(repeating)---------*/
      if (yvikeys_keys_repeating ()) {
         DEBUG_LOOP   yLOG_note    ("normal mode, group repeating older keys");
         x_ch = s_keys_log [s_gpos];
         ++s_gpos;
      }
      /*---(not-repeating)-----*/
      else {
         x_ch  = a_key;
         DEBUG_LOOP   yLOG_char    ("log_keys"  , myVIKEYS.log_keys);
         if (x_ch != 0 && myVIKEYS.log_keys == 'y') {
            DEBUG_LOOP   yLOG_note    ("normal mode, new keystroke and recording");
            yvikeys_keys__logger (x_ch);
         } else {
            DEBUG_LOOP   yLOG_note    ("normal mode, NO recording");
         }
      }
      /*---(done)--------------*/
   }
   /*---(run, delay, or playback)--------*/
   else IF_MACRO_PLAYING {
      /*---(playback)----------*/
      DEBUG_LOOP   yLOG_note    ("handle playback control");
      x_play = a_key;
      DEBUG_LOOP   yLOG_value   ("x_play"    , x_play);
      x_used = yvikeys_macro_exeplay (x_play);
      DEBUG_LOOP   yLOG_value   ("x_used"    , x_used);
      if (x_used < 0) {
         DEBUG_LOOP   yLOG_note    ("terminated, do not execute next key");
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      if (x_used > 0) {
         DEBUG_LOOP   yLOG_note    ("playback key, no impact on macro");
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      /*---(not-repeating)-----*/
      DEBUG_LOOP   yLOG_note    ("macro running, delay, or playback");
      x_ch = (uchar) yvikeys_macro_exekey ();
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      IF_MACRO_OFF {
         DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
         return x_ch;
      }
      /*---(advance)-----------*/
      DEBUG_LOOP   yLOG_note    ("advance");
      /*> yvikeys_macro_exeadv (x_play);                                              <*/
      yvikeys_macro_exeadv (0);
      /*---(done)--------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return x_ch;
}

char
BASE__________stub      (char a_major, char a_minor)
{
   MODE_exit  ();
   return a_minor;
}

uchar        /*-> process main loop keyboard input ---[ leaf   [gc.GD1.132.IM]*/ /*-[05.0000.111.R]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_handle     (uchar a_key)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          = 0;             /* generic return code            */
   static char x_major     = ' ';           /* saved keystroke from last loop */
   static char x_savemode  = '-';           /* to identify mode changes       */
   int         x_repeat    = 0;             /* store current repeat count     */
   char        x_nomode    = '-';           /* flag illegal mode              */
   uchar       x_key       = ' ';
   char        x_keys      [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_value   ("a_key"     , a_key);
   /*---(defense)------------------------*/
   if (a_key == 0) {
      DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_LOOP   yLOG_note    ("REAL KEY");
   /*---(prepare)------------------------*/
   myVIKEYS.trouble   = '-';
   x_key = chrworking (a_key);
   DEBUG_LOOP   yLOG_value   ("x_key"     , x_key);
   /*---(handle count)-------------------*/
   if (MODE_curr () == UMOD_REPEAT) {
      rc = REPEAT_umode (x_major, x_key);
      if (rc >  0)  x_major = G_KEY_SPACE;
   }
   /*---(main loop)----------------------*/
   while (1) {
      DEBUG_LOOP   yLOG_char    ("MODE bef"  , MODE_curr ());
      /*---(handle keystroke)------------*/
      switch (MODE_curr ()) {
      case MODE_GOD      : rc = GOD_mode              (x_major , x_key);  break;
      case MODE_PROGRESS : rc = PROGRESS_mode         (x_major , x_key);  break;
      case MODE_MAP      : rc = yvikeys_map_mode      (x_major , x_key);  break;
      case MODE_SOURCE   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_SRC_INPT : rc = SRC_INPT_umode        (x_major , x_key);  break;
      case SMOD_SREG     : rc = yvikeys_sreg_smode    (x_major , x_key);  break;
      case UMOD_SRC_REPL : rc = SRC_REPL_umode        (x_major , x_key);  break;
      case UMOD_SRC_UNDO : rc = BASE__________stub    (x_major , x_key);  break;
      case UMOD_MAP_UNDO : rc = BASE__________stub    (x_major , x_key);  break;
      case MODE_COMMAND  : rc = SOURCE_mode           (x_major , x_key);  break;
      case SMOD_HINT     : rc = SOURCE_mode           (x_major , x_key);  break;
      case MODE_SEARCH   : rc = SOURCE_mode           (x_major , x_key);  break;
      case UMOD_HISTORY  : rc = yvikeys_hist_umode    (x_major , x_key);  break;
      case UMOD_VISUAL   : rc = yvikeys_visu_umode    (x_major , x_key);  break;
      case SMOD_ERROR    : rc = BASE__________stub    (x_major , x_key);  break;
      case XMOD_FORMAT   : rc = FORMAT_xmode          (x_major , x_key);  break;
      case XMOD_PALETTE  : rc = PALETTE_xmode         (x_major , x_key);  break;
      case XMOD_UNITS    : rc = UNITS_xmode           (x_major , x_key);  break;
      case SMOD_BUFFER   : rc = yvikeys_bufs_umode    (x_major , x_key);  break;
      case UMOD_WANDER   : rc = yvikeys_map_wander    (x_major , x_key);  break;
      case SMOD_MREG     : rc = yvikeys_mreg_smode    (x_major , x_key);  break;
      case UMOD_MARK     : rc = yvikeys_mark_smode    (x_major , x_key);  break;
      case SMOD_MENUS    : rc = yvikeys_menu_smode    (x_major , x_key);  break;
      case SMOD_MACRO    : rc = yvikeys_macro_smode   (x_major , x_key);  break;
      case UMOD_SENDKEYS : rc = SENDKEYS_umode        (x_major , x_key);  break;
      case UMOD_REPEAT   :                                                break;
      default            : rc = -1;  x_nomode = 'y';                      break;
      }
      DEBUG_USER   yLOG_value   ("rc"        , rc);
      DEBUG_LOOP   yLOG_char    ("MODE aft"  , MODE_curr ());
      /*---(translate unprintable)-------*/
      x_repeat = REPEAT_count ();
      DEBUG_USER   yLOG_value   ("x_repeat"  , x_repeat);
      /*> snprintf (x_keys,   9, "%2d %c%c"  , x_repeat, x_major, chrvisible (x_key));   <*/
      snprintf (x_keys,   9, " %2x%c%c"  , x_key, x_major, chrvisible (x_key));
      /*---(loop repeats)----------------*/
      if (rc == 0 && x_repeat > 0 && MODE_curr () != UMOD_REPEAT) {
         DEBUG_USER   yLOG_note    ("repeating");
         REPEAT_decrement ();
         continue;
      }
      /*---(loop repeats)----------------*/
      if (rc <= 0 && MODE_curr () != UMOD_REPEAT) {
         DEBUG_USER   yLOG_note    ("complete repeat");
         REPEAT_done ();
      }
      break;
      /*---(done)------------------------*/
   }
   /*---(setup for next keystroke)-------*/
   if      (rc == 0)    x_major = G_KEY_SPACE;
   else if (rc >  0)    x_major = rc;
   else               { x_major = G_KEY_SPACE;  yvikeys_set_error ();  myVIKEYS.trouble = 'y';  REPEAT_reset (); }
   yvikeys_view_keys (x_keys);
   /*---(save current mode)--------------*/
   x_savemode = MODE_curr ();
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}

uchar        /*-> process input string in main loop --[ ------ [ge.C74.153.42]*/ /*-[02.0000.00#.D]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main_string  (uchar *a_keys)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;
   int         i           =    0;
   int         j           =   -1;
   int         x_len       =    0;
   uchar       x_ch        =  ' ';     /* current keystroke                   */
   char        x_keys      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_LOOP   yLOG_enter   (__FUNCTION__);
   DEBUG_LOOP   yLOG_point   ("a_keys"    , a_keys);
   --rce;  if (a_keys == NULL) {
      DEBUG_LOOP   yLOG_note    ("a_keys is null");
      DEBUG_LOOP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strlcpy    (x_keys, a_keys  , LEN_RECD);
   strlencode (x_keys, ySTR_MAX, LEN_RECD);
   DEBUG_LOOP   yLOG_info    ("x_keys"    , x_keys);
   x_len = strlen (a_keys);
   DEBUG_LOOP   yLOG_value   ("x_len"     , x_len);
   --rce;
   for (i = 0; i < x_len; ++i) {
      DEBUG_LOOP   yLOG_value   ("LOOP"      , i);
      /*---(get next char)---------------*/
      if (i != j) {
         DEBUG_LOOP   yLOG_note    ("new keystroke");
         DEBUG_LOOP   yLOG_value   ("a_keys[i]" , a_keys[i]);
         DEBUG_LOOP   yLOG_char    ("a_keys[i]" , chrvisible (a_keys[i]));
         x_ch = chrworking (a_keys [i]);
      } else {
         DEBUG_LOOP   yLOG_note    ("repeat loop");
         x_ch = 0;
      }
      j = i;
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      DEBUG_LOOP   yLOG_char    ("x_ch"      , chrvisible (x_ch));
      /*---(handle input)----------------*/
      x_ch = yVIKEYS_main_input (RUN_TEST, x_ch);
      DEBUG_LOOP   yLOG_value   ("x_ch"      , x_ch);
      if (x_ch  == G_KEY_ACK)     ++s_acks;
      if (x_ch  == G_KEY_SPACE)   ++s_spaces;
      if (x_ch  == 0)             ++s_noops;
      /*---(handle keystroke)------------*/
      rc = yVIKEYS_main_handle (x_ch);
      DEBUG_LOOP   yLOG_value   ("rc"        , rc);
      /*---(check for macro)-------------*/
      DEBUG_LOOP   yLOG_char    ("macro exe" , yvikeys_macro_emode ());
      DEBUG_LOOP   yLOG_value   ("macro cnt" , yvikeys_macro_count ());
      DEBUG_LOOP   yLOG_value   ("s_nkey"    , s_nkey);
      DEBUG_LOOP   yLOG_value   ("s_gpos"    , s_gpos);
      IF_MACRO_MOVING  {
         DEBUG_LOOP   yLOG_note    ("macro running and used step, back up loop counter");
         --i;
      }
      if (yvikeys_keys_repeating ()) {
         DEBUG_LOOP   yLOG_note    ("group using older keystrokes, back up loop counter");
         --i;
      }
      /*---(done)------------------------*/
   }
   DEBUG_LOOP   yLOG_note    ("main loop done");
   /*---(complete)-----------------------*/
   DEBUG_LOOP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> handle main loop for ncurses -------[ ------ [gn.842.232.99]*/ /*-[01.0000.000.!]-*/ /*-[--.---.---.--]-*/
yVIKEYS_main            (char *a_delay, char *a_update, void *a_altinput ())
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         x_loop      =    0;
   int         x_ch        =  ' ';
   uchar       x_key       =  ' ';
   char        x_draw      =  '-';
   char        x_group     =  '-';
   char        t           [LEN_DESC]  = "";
   /*---(prepare)------------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   yvikeys_loop_set     (a_delay, a_update);
   yVIKEYS_view_all (0.0);
   /*---(CLA for script)-----------------*/
   if (strcmp (myVIKEYS.m_script, "") != 0) {
      sprintf (t, ":script %s", myVIKEYS.m_script);
      DEBUG_TOPS   yLOG_info    ("t"         , t);
      rc = yVIKEYS_cmds_direct (t);
      DEBUG_TOPS   yLOG_value   ("cmds"      , rc);
   }
   /*---(main-loop)----------------------*/
   while (1) {
      /*---(get input)-------------------*/
      x_ch = yvikeys_loop_getch ();
      DEBUG_GRAF  yLOG_value   ("x_ch"      , x_ch);
      yvikeys_loop_beg   ();
      /*---(specialty actions)-----------*/
      if (x_ch == KEY_RESIZE)  yVIKEYS_resize (0, 0, 0);
      if (x_ch == -102)        yVIKEYS_resize (0, 0, 0);
      if (x_ch < 0)  x_key = 0;
      else           x_key = x_ch;
      /*---(keyboard input)--------------*/
      DEBUG_GRAF  yLOG_value   ("x_key"     , x_key);
      x_key = yVIKEYS_main_input  (RUN_USER, x_key);
      if (x_key == G_KEY_ACK)  continue;   /* for macros to skip spaces, etc  */
      /*---(handling)--------------------*/
      yVIKEYS_main_handle (x_key);
      if (yVIKEYS_quit ())  break;
      /*---(alternate input)-------------*/
      if (a_altinput != NULL)   a_altinput ();
      /*---(repeating)-------------------*/
      /*> if (x_key == ')' && yvikeys_macro_emode () == MACRO_STOP) {                           <* 
       *>    DEBUG_GRAF  yLOG_note    ("kick into grouping (non-macro) fast execution mode");   <* 
       *>    yvikeys_macro_set2blitz ();                                                        <* 
       *>    x_group = 'y';                                                                     <* 
       *>    continue;                                                                          <* 
       *> }                                                                                     <* 
       *> if (!yvikeys_keys_repeating () && x_group == 'y') {                                   <* 
       *>    DEBUG_GRAF  yLOG_note    ("end grouping (non-macro) fast execution mode");         <* 
       *>    yvikeys_macro_set2run ();                                                          <* 
       *>    x_group = '-';                                                                     <* 
       *> }                                                                                     <*/
      /*---(showing)---------------------*/
      ++x_loop;
      x_draw = '-';
      if ((x_loop % myVIKEYS.loops) == 0) {
         x_draw = 'y';
         yvikeys_loop_prog ();
         yVIKEYS_view_all (0.0);
      }
      /*---(sleeping)--------------------*/
      yvikeys_loop_sleep (x_key, x_draw);
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char          yVIKEYS__unit_answer [LEN_FULL];

char       /*----: set up program urgents/debugging --------------------------*/
yvikeys__unit_quiet    (void)
{
   int         x_narg       = 1;
   char       *x_args [20]  = {"yVIKEYS_unit" };
   /*> yURG_logger   (x_narg, x_args);                                                <*/
   /*> yURG_urgs     (x_narg, x_args);                                                <*/
   yVIKEYS_init (MODE_MAP);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yvikeys__unit_loud     (void)
{
   int         x_narg       = 1;
   char       *x_args [20]  = {"yVIKEYS_unit" };
   yURG_logger   (x_narg, x_args);
   yURG_urgs     (x_narg, x_args);
   yURG_name  ("kitchen"      , YURG_ON);
   yURG_name  ("yurg"         , YURG_ON);
   yURG_name  ("edit"         , YURG_ON);
   yURG_name  ("regs"         , YURG_ON);
   yURG_name  ("mark"         , YURG_ON);
   yURG_name  ("mode"         , YURG_ON);
   yURG_name  ("map"          , YURG_ON);
   yURG_name  ("map_mas"      , YURG_ON);
   yURG_name  ("cmds"         , YURG_ON);
   yURG_name  ("srch"         , YURG_ON);
   yURG_name  ("hist"         , YURG_ON);
   yURG_name  ("graf"         , YURG_ON);
   yURG_name  ("ystr"         , YURG_ON);
   yURG_name  ("yparse"       , YURG_ON);
   DEBUG_YVIKEYS yLOG_info     ("yVIKEYS"    , yVIKEYS_version   ());
   yVIKEYS_init (MODE_MAP);
   return 0;
}

char       /*----: stop logging ----------------------------------------------*/
yvikeys__unit_end      (void)
{
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   yVIKEYS_wrap ();
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   yLOGS_end    ();
   return 0;
}

char        yvikeys_dump__fake       (FILE *f) { return 0; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
yvikeys_dump__unit      (char *a_question, char a_index)
{
   /*---(locals)-----------+-----------+-*/
   char        t           [LEN_RECD ] = "";
   int         x_fore      = 0;
   tDUMP      *x_curr      = NULL;
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "DUMP unit        : question not understood", LEN_FULL);
   /*---(questions)----------------------*/
   if      (strcmp (a_question, "count"          )   == 0) {
      x_curr = s_hdump;  while (x_curr  != NULL) { ++x_fore; x_curr = x_curr->next; }
      snprintf (yVIKEYS__unit_answer, LEN_RECD, "DUMP counts      : num=%4d, fore=%4d", s_ndump, x_fore);
   }
   else if (strcmp (a_question, "entry"          )   == 0) {
      x_curr = s_hdump;  while (x_curr  != NULL && x_fore < a_index) { ++x_fore; x_curr = x_curr->next; }
      if (x_curr == NULL)   snprintf (yVIKEYS__unit_answer, LEN_RECD, "DUMP entry  (%2d) : no entry", a_index);
      else                  snprintf (yVIKEYS__unit_answer, LEN_RECD, "DUMP entry  (%2d) : %2d:%-15.15s  %p", a_index, strlen (x_curr->name), x_curr->name, x_curr->provider);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}

char yvikeys__unit_reset      (void)  { s_acks = 0; s_spaces = 0; s_noops = 0; return 0; }

char*        /*-> tbd --------------------------------[ leaf   [gs.520.202.40]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
KEYS__unit              (char *a_question, char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   char        t           [LEN_RECD ] = "";
   int         x_beg       =    0;
   char        x_open      =  '[';
   /*---(preprare)-----------------------*/
   strlcpy  (yVIKEYS__unit_answer, "KEYS unit        : question not understood", LEN_FULL);
   /*---(dependency list)----------------*/
   if (s_nkey > 40) {
      x_beg  = s_nkey - 40;
      x_open = '<';
   }
   if      (strcmp (a_question, "log"            )   == 0) {
      sprintf (t, "%s", s_keys_log   + x_beg);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS log         : %3d %3d %c%-.40s]", s_akey, s_nkey, x_open, t);
   }
   else if (strcmp (a_question, "mode"           )   == 0) {
      sprintf (t, "%s", s_keys_mode  + x_beg);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS mode        : %3d %3d %c%-.40s]", s_akey, s_nkey, x_open, t);
   }
   else if (strcmp (a_question, "multi"          )   == 0) {
      sprintf (t, "%s", s_keys_multi + x_beg);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS multi       : %3d %3d %c%-.40s]", s_akey, s_nkey, x_open, t);
   }
   else if (strcmp (a_question, "error"          )   == 0) {
      sprintf (t, "%s", s_keys_error + x_beg);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS error       : %3d %3d %c%-.40s]", s_akey, s_nkey, x_open, t);
   }
   else if (strcmp (a_question, "full"           )   == 0) {
      sprintf (t, "%s", s_keys_error + x_beg);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS full        : %3d %3d [%s]", s_akey, s_nkey, s_keys_log);
   }
   else if (strcmp (a_question, "status"         )   == 0) {
      yvikeys_keys_status (t);
      strltrim (t, ySTR_BOTH, LEN_RECD);
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "%-.60s", t);
   }
   else if (strcmp (a_question, "pos"          )  == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS pos         : %3dn %3dp", s_nkey, s_gpos);
   }
   else if (strcmp (a_question, "acks"         )  == 0) {
      snprintf (yVIKEYS__unit_answer, LEN_FULL, "KEYS acks        : %3da %3ds %3dz", s_acks, s_spaces, s_noops);
   }
   /*---(complete)-----------------------*/
   return yVIKEYS__unit_answer;
}


