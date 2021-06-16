/*============================----beg-of-source---============================*/
/*---(headers)---------------------------*/
#include    "yVIKEYS.h"
#include    "yVIKEYS_priv.h"



char yvikeys_tags_purge     (void) { yvikeys_srch__purge (MODE_SEARCH); }
char yVIKEYS_tags_found     (char *a_label, int b, int x, int y, int z) { yvikeys_srch__found (SMOD_TAGS  , a_label, b, x, y, z); }
char yvikeys_tags_by_cursor (char a_move, tFIND **r_curr)  { return yvikeys_srch__by_cursor (SMOD_TAGS  , a_move, r_curr); }
char yvikeys_tags_by_index  (int a_index, tFIND **r_curr)  { return yvikeys_srch__by_index (SMOD_TAGS  , a_index, r_curr); }
char yvikeys_tags_list      (char *a_list)                 { return yvikeys_srch__list (SMOD_TAGS  , a_list); }
int  yVIKEYS_tags_count     (void)                         { return yvikeys_srch__count (SMOD_TAGS  ); }
char yvikeys_tags_curr      (char *a_label)                { return yvikeys_srch__curr      (SMOD_TAGS, a_label); }
