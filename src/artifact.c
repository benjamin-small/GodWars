/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  God Wars Mud improvements copyright (C) 1994, 1995, 1996 by Richard    *
 *  Woolcock.  This mud is NOT to be copied in whole or in part, or to be  *
 *  run without the permission of Richard Woolcock.  Nobody else has       *
 *  permission to authorise the use of this code.                          *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*int clan_table_search args( (CHAR_DATA *ch) );*/
/*void format_string_spaces args ( (char *argument,int spaces) );*/

void artifact_table_write() {
    int art_pos;
    FILE *fp;
    if((fp = fopen(ART_LIST, "w")) != NULL) {
        for(art_pos = 0; art_pos < MAX_ART; art_pos++) {
            fprintf(fp, "%d %s~\n",
                    artifact_table[art_pos].object_vnum,
                    artifact_table[art_pos].player_name);
        }
        fflush(fp);
        fclose(fp);
    } else {
        /*bug("Cannot Open ARTIFACT file :- WRITE ARTIFACT TABLE", 0);*/
    }
    return;
}

void artifact_table_read() {
    int art_pos;
    FILE *fp;
    if((fp = fopen(ART_LIST, "r")) != NULL) {
        for(art_pos = 0; art_pos < MAX_ART; art_pos++) {
            artifact_table[art_pos].object_vnum = fread_number(fp);
            artifact_table[art_pos].player_name = fread_string(fp);
        }
    } else {
        /*bug("Cannot Open ARTIFACT Table:- Loading Default Table", 0);*/
        return;
    }
    fflush(fp);
    fclose(fp);
    return;
}

void do_show_artifacts(CHAR_DATA *ch, char *argument) {
    char buf [MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int art_pos;
    for(art_pos = 0; art_pos < MAX_ART; art_pos++) {
        if((pObjIndex =
                    get_obj_index(artifact_table[art_pos].object_vnum)) == NULL) {
            continue;
        }
        if(strlen(artifact_table[art_pos].player_name) < 3 ||
                !str_cmp((artifact_table[art_pos].player_name), "none")) {
            sprintf(buf, "%s, which has not yet been discovered.\n\r",
                    pObjIndex->short_descr);
        } else {
            sprintf(buf, "%s, which has been discovered by %s.\n\r",
                    pObjIndex->short_descr,
                    artifact_table[art_pos].player_name);
        }
        buf[0] = UPPER(buf[0]);
        send_to_char(buf, ch);
    }
}

struct artifact_type artifact_table[MAX_ART] = {
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0},
    {"none", 0}
};
