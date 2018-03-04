/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"


/*
 *     c   = char
 *     L   = label  (10 char)
 *     N   = name   (20 char)
 *     D   = desc   (60 char)
 *     S   = string (no trunc)
 *     d   = 5 decimal
 *
 *
 */

#define     MAX_SECTION   50
typedef struct  cSECTION  tSECTION;
struct cSECTION {
   char        abbr;
   char        name        [LEN_DESC ];
   char        label       [LEN_LABEL];
   char        version;
   char        specs       [LEN_LABEL];
   char        column      [ 9][10];
   char        (*writer) (int , void*,void*,void*,void*,void*,void*,void*,void*,void*);
   char        (*reader) (char, void*,void*,void*,void*,void*,void*,void*,void*,void*);
   int         try;
   int         bad;
};
static tSECTION  s_sections [MAX_SECTION] = {
   /* ---abbr---    ---name------------    --label---   ver   ---specs--    ---1st----    ---2nd----    ---3rd----    ---4th----    ---5th----    ---6th----    ---7th----    ---8th----    ---9th----   writer---------  reader---------  try  bad */
   { SMOD_MARK   , "location marks"     , "loc_mark"  , 'B', "ciii------", "a"         , "x"         , "y"         , "z"         , ""          , ""          , ""          , ""          , ""          , MARK_writer    , MARK_reader    ,   0,   0 },
   { SMOD_VISUAL , "visual marks"       , "visu_mark" , 'A', "ciiiiii---", "a"         , "xbeg"      , "ybeg"      , "zbeg"      , "xend"      , "yend"      , "zend"      , ""          , ""          , VISU_writer    , VISU_reader    ,   0,   0 },
   { MODE_SEARCH , "searches"           , "search"    , 'A', "cS--------", "a"         , "search"    , ""          , ""          , ""          , ""          , ""          , ""          , ""          , SRCH_writer    , SRCH_reader    ,   0,   0 },
   { 0           , ""                   , ""          , '-', "----------", ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , NULL           , NULL           ,   0,   0 },
};
static int  s_nsection   = 0;


static FILE    *s_file;                      /* file pointer                   */
static char     s_fields    [20][LEN_RECD];
static int      s_nfield    =   0;


static char  /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP__sec_columns       (FILE *a_file, char a_index)
{
   /*---(locals)-----------+-----+-----+-*/
   int         i           =    0;
   int         x_label     [LEN_RECD ];
   /*---(prefix)-------------------------*/
   fprintf (a_file, "#---------  ver  ");
   /*---(columns)------------------------*/
   for (i = 0; i < 10; ++i) {
      if (s_sections [a_index].specs [i] == '-')  break;
      sprintf (x_label, "%s%-100.100s", s_sections [a_index].column [i], "-");
      switch (s_sections [a_index].specs [i]) {
      case  'c'  :
         fprintf (a_file, "%-3.3s  "  , x_label);
         break;
      case  'L'  :
         fprintf (a_file, "%-20.20s  ", x_label);
         break;
      case  'D'  :
         fprintf (a_file, "%-60.60s  ", x_label);
         break;
      case  'S'  :
         fprintf (a_file, "%s  "      , x_label);
         break;
      case  'i'  :
         fprintf (a_file, "%-5.5s  "  , x_label);
         break;
      case  'f'  :
         fprintf (a_file, "%-10.10s  ", x_label);
         break;
      }
   }
   /*---(suffix)-------------------------*/
   fprintf (a_file, "\n");
   /*---(complete)-----------------------*/
   return 0;
}

static char
OUTP_writer             (char n, int a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   int        *x_field     [9];
   char        t           [LEN_RECD ];
   /*---(gather next)-----------------*/
   sprintf (myVIKEYS.f_recd, "");
   /*> if (s_sections [n].writer != NULL)  rc = s_sections [n].writer (i, x_field [0], x_field [1], x_field [2], x_field [3], x_field [4], x_field [5], x_field [6], x_field [7], x_field [8]);   <*/
   for (i = 0; i < 9; ++i) x_field [i] = NULL;
   /*> printf ("file   %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   /*> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <*/
   /*> rc = VISU_writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   rc = s_sections [n].writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);
   /*> printf ("after  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   /*> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <*/
   if (rc <= 0)  return rc;
   /*---(columns)---------------------*/
   sprintf (myVIKEYS.f_recd, "%-10.10s  -%c- ", s_sections [n].label, s_sections [n].version);
   /*> sprintf (t, " %d   "   , rc);                                                 <*/
   /*> strlcat (myVIKEYS.f_recd, t, LEN_RECD);                                        <*/
   for (i = 0; i < 9; ++i) {
      if (s_sections [n].specs [i] == '-')  break;
      if (x_field [i] == NULL)  break;
      switch (s_sections [n].specs [i]) {
      case  'c'  :
         sprintf (t, "  %c  "     , *((char   *) x_field [i]));
         break;
      case  'L'  :
         sprintf (t, " %-20.20s " , (char *)     x_field [i]);
         break;
      case  'D'  :
         sprintf (t, " %-60.60s " , (char *)     x_field [i]);
         break;
      case  'S'  :
         sprintf (t, " %s "       , (char *)     x_field [i]);
         break;
      case  'i'  :
         sprintf (t, " %5d "      , *((int    *) x_field [i]));
         break;
      case  'f'  :
         sprintf (t, " %-10.3lf " , *((double *) x_field [i]));
         break;
      default    :
         strlcpy (t, " ", LEN_LABEL);
         break;
      }
      strlcat (myVIKEYS.f_recd, t, LEN_RECD);
   }
   /*---(complete)-----------------------*/
   return 1;
}

char
OUTP__unit_writer       (char a_abbr, char a_item)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_index     =   -1;
   /*---(cleanse)------------------------*/
   sprintf (myVIKEYS.f_recd, "");
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   --rce;  if (n < 0)  return rce;
   /*---(find item)----------------------*/
   switch (s_sections [n].abbr) {
   case SMOD_MARK    :  x_index = MARK_valid (a_item);  break;
   case SMOD_VISUAL  :  x_index = VISU_valid (a_item);  break;
   case MODE_SEARCH  :  x_index = SRCH_valid (a_item);  break;
   }
   --rce;  if (x_index < 0)  return rce;
   /*---(process)------------------------*/
   rc = OUTP_writer (n, x_index);
   /*---(complete)-----------------------*/
   return rc;
}

int 
OUTP_write_all          (FILE *a_file, char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   int         i           =    0;
   int         j           =    0;
   int         n           =   -1;
   int         c           =    0;
   char        x_upper     [LEN_LABEL];
   void       *x_field     [9];
   char        t           [LEN_RECD ];
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   if (n < 0)  return -1;
   /*---(write header)-------------------*/
   strlcpy (x_upper, s_sections [n].name, LEN_LABEL);
   for (i = 0; i < strllen (x_upper, LEN_LABEL); ++i)  x_upper [i] = toupper (x_upper [i]);
   if (a_file != NULL) {
      fprintf (a_file, "#===[[ %-20.20s ]]===============================================================================================#\n",
            x_upper);
      OUTP__sec_columns (a_file, n);
   }
   /*---(write entries)------------------*/
   for (i = 0;  i < 127; ++i) {
      rc = OUTP_writer (n, i);
      if (rc <= 0)  continue;
      if (a_file != NULL)  fprintf (a_file, "%s\n", myVIKEYS.f_recd);
      ++c;
   }
   /*---(write footer)-------------------*/
   if (a_file != NULL) {
      if (c == 0)  fprintf (a_file, "# no %s\n", s_sections [n].name);
      else         fprintf (a_file, "# complete with %d lines\n", c);
   }
   /*---(complete)-----------------------*/
   return c;
}

char         /*-> open file for reading and prep -----[ leaf   [ge.723.023.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
INPT_open          ()
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   DEBUG_INPT  yLOG_info    ("f_loc"     , myVIKEYS.f_loc);
   DEBUG_INPT  yLOG_info    ("f_name"    , myVIKEYS.f_name);
   DEBUG_INPT  yLOG_info    ("f_title"   , myVIKEYS.f_title);
   /*---(open file)----------------------*/
   s_file = fopen (myVIKEYS.f_title, "r");
   DEBUG_INPT  yLOG_point   ("s_file"    , s_file);
   --rce;  if (s_file == NULL) {
      DEBUG_INPT  yLOG_note    ("file could not be openned");
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   DEBUG_INPT  yLOG_note    ("file successfully opened");
   /*---(prepare)------------------*/
   DEBUG_INPT  yLOG_note    ("set file globals");
   myVIKEYS.f_lines = 0;
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> close file for reading and wrap ----[ leaf   [ge.411.011.20]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
INPT_close         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(close file)---------------------*/
   if (s_file == NULL) {
      DEBUG_INPT  yLOG_note    ("no file to close");
   } else {
      DEBUG_INPT  yLOG_note    ("close file");
      fclose  (s_file);
   }
   /*---(screen)-------------------------*/
   DEBUG_INPT   yLOG_note    ("set screen positions correctly");
   /*> KEYS_basics (' ', 'r');                                                        <*/
   /*> KEYS_bcol (BCOL);                                                              <*/
   /*> CURS_col_head();                                                               <*/
   /*> KEYS_brow (BROW);                                                              <*/
   /*> CURS_row_head();                                                               <*/
   /*---(calculate)----------------------*/
   DEBUG_INPT  yLOG_note    ("recalc");
   /*> SEQ_calc_full ();                                                              <*/
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> file reading driver ----------------[ leaf   [ge.632.025.30]*/ /*-[01.0001.013.!]-*/ /*-[--.---.---.--]-*/
INPT_read          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   int         x_len       =    0;               /* string length             */
   /*---(read next good one)-------------*/
   while (1) {
      /*---(read next)-------------------*/
      ++myVIKEYS.f_lines;
      DEBUG_INPT  yLOG_value   ("line"      , myVIKEYS.f_lines);
      fgets (myVIKEYS.f_recd, LEN_RECD, s_file);
      /*---(check eof)-------------------*/
      if (feof (s_file))  {
         DEBUG_INPT  yLOG_note    ("end of file reached");
         return rce;
      }
      /*---(check length)----------------*/
      x_len = strlen (myVIKEYS.f_recd);
      if (x_len <= 0)  {
         DEBUG_INPT  yLOG_note    ("record empty");
         continue;
      }
      /*---(fix)-------------------------*/
      myVIKEYS.f_recd [--x_len] = '\0';
      DEBUG_INPT  yLOG_value   ("length"    , x_len);
      DEBUG_INPT  yLOG_info    ("fixed"     , myVIKEYS.f_recd);
      /*---(filter)----------------------*/
      if (strchr ("# ", myVIKEYS.f_recd [0]) != NULL) {
         DEBUG_INPT  yLOG_note    ("comment/empty line, skipping");
         continue;
      }
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> file reading driver ----------------[ leaf   [ge.851.163.30]*/ /*-[01.0000.013.!]-*/ /*-[--.---.---.--]-*/
INPT_parse              (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        x_recd      [LEN_RECD];
   int         x_len       =    0;
   char       *p           = NULL;               /* strtok pointer            */
   char       *q           = "";               /* strtok delimeters         */
   char       *r           = NULL;               /* strtok context variable   */
   /*---(cleanse)---------------------*/
   for (s_nfield = 0; s_nfield < 20; ++s_nfield) {
      strlcpy (s_fields [s_nfield], "", LEN_RECD);
   }
   s_nfield = 0;
   strlcpy (x_recd, myVIKEYS.f_recd, LEN_RECD);
   /*---(parse fields)----------------*/
   p = strtok_r (x_recd, q, &r);
   while (p != NULL) {
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      DEBUG_INPT  yLOG_bullet  (s_nfield    , p);
      strlcpy   (s_fields [s_nfield++], p, LEN_RECD);
      p = strtok_r (NULL     , q, &r);
   }
   --rce;  if (s_nfield < 1) {
      DEBUG_INPT  yLOG_note    ("no fields found");
      return -rce;
   }
   strlcpy   (myVIKEYS.f_type, s_fields [0], LEN_RECD);
   DEBUG_INPT  yLOG_info    ("type"      , myVIKEYS.f_type);
   if (s_fields [1][0] == '-' && s_fields [1][2] == '-')
      myVIKEYS.f_vers  = s_fields [1][1];
   else
      myVIKEYS.f_vers  = '?';
   DEBUG_INPT  yLOG_char    ("vers"      , myVIKEYS.f_vers);
   /*---(complete)-----------------------*/
   return 0;
}

char
INPT__unit_reader       (char a_abbr)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_index     =   -1;
   /*---(find entry)---------------------*/
   for (i = 0; i < MAX_SECTION; ++i) {
      if (s_sections [i].abbr == 0)       break;
      if (s_sections [i].abbr != a_abbr)  continue;
      n = i;
      break;
   }
   --rce;  if (n < 0)     return rce;
   /*---(parse the record)---------------*/
   rc = INPT_parse ();
   --rce;  if (rc < 0)    return  rce;
   /*---(use the reader)-----------------*/
   if (s_sections [n].writer != NULL) {
      rc = s_sections [n].reader (myVIKEYS.f_vers, s_fields [2], s_fields [3], s_fields [4], s_fields [5], s_fields [6], s_fields [7], s_fields [8], s_fields [9], s_fields [10]);
   }
   /*---(complete)-----------------------*/
   return rc;
}

char         /*-> file reading driver ----------------[ leaf   [ge.C71.072.GA]*/ /*-[02.0000.102.!]-*/ /*-[--.---.---.--]-*/
INTP_read_all      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         i           =    0;
   int         n           =   -1;
   int         x_celltry   = 0;
   int         x_cellbad   = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT  yLOG_enter   (__FUNCTION__);
   /*---(open file)----------------------*/
   rc = INPT_open   ();
   --rce;  if (rc < 0) {
      DEBUG_INPT  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(read lines)---------------------*/
   DEBUG_INPT  yLOG_note    ("read lines");
   while (s_file != NULL) {
      /*---(read and clean)--------------*/
      rc = INPT_read ();
      if (rc < 0)  break;
      rc = INPT_parse ();
      if (rc < 0)  break;
      /*---(find type)-------------------*/
      n = -1;
      for (i = 0; i < s_nsection; ++i) {
         if (s_sections [i].label [0] != myVIKEYS.f_type [0])        continue;
         if (strcmp (s_sections [i].label, myVIKEYS.f_type) != 0)    continue;
         n = i;
         break;
      }
      if (n < 0)  continue;
      /*---(handle)----------------------*/
      ++s_sections [n].try;
      rc = -1;
      if (s_sections [n].writer != NULL) {
         rc = s_sections [n].writer (myVIKEYS.f_vers, s_fields [3], s_fields [4], s_fields [5], s_fields [6], s_fields [7], s_fields [8], s_fields [9], s_fields [10], s_fields [11]);
      }
      if (rc < 0)  ++s_sections [n].bad;
      /*---(done)------------------------*/
   }
   /*---(close file)---------------------*/
   INPT_close ();
   /*---(complete)-----------------------*/
   DEBUG_INPT yLOG_exit    (__FUNCTION__);
   return 0;
}


