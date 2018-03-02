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
   char        (*writer) (void*,void*,void*,void*,void*,void*,void*,void*,void*);
   char        (*reader) (void*,void*,void*,void*,void*,void*,void*,void*,void*);
};
tSECTION  s_sections [MAX_SECTION] = {
   /* ---abbr---    ---name------------    --label---   ver   ---specs--    ---1st----    ---2nd----    ---3rd----    ---4th----    ---5th----    ---6th----    ---7th----    ---8th----    ---9th----  writer  reader*/
   { SMOD_MARK   , "location marks"     , "loc_mark"  , 'A', "cliii-----", "label"     , "x"         , "y"         , "z"         , ""          , ""          , ""          , ""          , ""          , NULL , NULL  },
   { SMOD_VISUAL , "visual marks"       , "visu_mark" , 'A', "ciiiiii---", "a"         , "xbeg"      , "ybeg"      , "zbeg"      , "xend"      , "yend"      , "zend"      , ""          , ""          , NULL , NULL  },
   { 0           , ""                   , ""          , '-', "----------", ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , ""          , NULL , NULL  },
};
int       s_nsection   = 0;

char         /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
FILE__sec_columns       (FILE *a_file, char a_index)
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
         fprintf (a_file, "%-10.10s  ", x_label);
         break;
      case  'N'  :
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

char         /*-> write file tab information ---------[ leaf   [ge.420.213.30]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
FILE__sec_footer        (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no visual marks\n");
   else               fprintf (a_file, "#---------  ver  -  --x--  --y--  --z--  --x--  --y--  --z-- \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char
FILE_writer             (char n, int a_entry)
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
   rc = VISU_writer (a_entry, &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);
   /*> printf ("after  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n", &x_field [0], &x_field [1], &x_field [2], &x_field [3], &x_field [4], &x_field [5], &x_field [6], &x_field [7], &x_field [8]);   <*/
   /*> printf ("value  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p  %14p\n",  x_field [0],  x_field [1],  x_field [2],  x_field [3],  x_field [4],  x_field [5],  x_field [6],  x_field [7],  x_field [8]);   <*/
   if (rc <= 0)  return rc;
   /*---(columns)---------------------*/
   sprintf (myVIKEYS.f_recd, "%-10.10s  -%c- ", s_sections [n].label, s_sections [n].version);
   /*> sprintf (t, " %d   "   , rc);                                                 <*/
   strlcat (myVIKEYS.f_recd, t, LEN_RECD);
   for (i = 0; i < 9; ++i) {
      if (s_sections [n].specs [i] == '-')  break;
      if (x_field [i] == NULL)  break;
      switch (s_sections [n].specs [i]) {
      case  'c'  :
         sprintf (t, "  %c  "     , *((char   *) x_field [i]));
         break;
      case  'L'  :
         sprintf (t, " %-10.10s " , (char *)     x_field [i]);
         break;
      case  'N'  :
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
      }
      strlcat (myVIKEYS.f_recd, t, LEN_RECD);
   }
   /*---(complete)-----------------------*/
   return 1;
}

char
FILE_write_all          (FILE *a_file, char a_abbr)
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
   fprintf (a_file, "#===[[ %-20.20s ]]===============================================================================================#\n",
         x_upper);
   FILE__sec_columns (a_file, n);
   /*---(write entries)------------------*/
   for (i = 0; i < 100; ++i) {
      rc = FILE_writer (n, i);
      if (rc <  0)  break;
      if (rc == 0)  continue;
      if (a_file != NULL)  fprintf (a_file, "%s\n", myVIKEYS.f_recd);
      ++c;
   }
   /*---(write footer)-------------------*/
   if (c == 0)  fprintf (a_file, "# no %s\n", s_sections [n].name);
   else         fprintf (a_file, "# complete with %d lines\n", c);
   /*---(complete)-----------------------*/
   return 0;
}


