/*============================----beg-of-source---============================*/



/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    <yLOG.h>               /* heatherly program logger                */


/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>             /* clibc  standard input/output             */
#include    <stdlib.h>            /* clibc  standard general purpose          */
#include    <string.h>            /* clibc  standard string handling          */



/*===[[ HEADER GUARD ]]===================================*/
#ifndef yVIKEYS_PRIV
#define yVIKEYS_PRIV yes


/*===[[ RATIONAL LIMITS ]]====================================================*/
/*   LEN_ is a length or size of something
 *   MIN_ is a minimum count
 *   DEF_ is a default count
 *   MAX_ is a maximum count
 *
 */
/*---(string lengths)-----------------*/
#define     LEN_STR     200
/*---(struct array size)--------------*/



typedef  struct  cMODE_INFO  tMODE_INFO;
struct cMODE_INFO {
   char        abbr;                   /* single character abbreviation       */
   char        major;                  /* major mode (y/n)                    */
   char        show;                   /* show a message line (y/n)           */
   char        three       [ 5];       /* very short name                     */
   char        terse       [10];       /* short name                          */
   char        desc        [50];       /* description of mode                 */
   char        allow       [25];       /* allowed mode transitions            */
   int         count;                  /* number of times used                */
   char        mesg        [LEN_STR];  /* informative message for display     */
};

#endif
/*============================----end-of-source---============================*/
