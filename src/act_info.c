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
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *	const	where_name	[] = {
    "[Light         ] ",
    "[On Finger     ] ",
    "[On Finger     ] ",
    "[Around Neck   ] ",
    "[Around Neck   ] ",
    "[On Body       ] ",
    "[On Head       ] ",
    "[On Legs       ] ",
    "[On Feet       ] ",
    "[On Hands      ] ",
    "[On Arms       ] ",
    "[Off Hand      ] ",
    "[Around Body   ] ",
    "[Around Waist  ] ",
    "[Around Wrist  ] ",
    "[Around Wrist  ] ",
    "[Right Hand    ] ",
    "[Left Hand     ] ",
    "[On Face       ] ",
    "[Left Scabbard ] ",
    "[Right Scabbard] ",
    "[Through Chest ] ",
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args((OBJ_DATA *obj, CHAR_DATA *ch,
                                  bool fShort));
void	show_char_to_char_0	args((CHAR_DATA *victim, CHAR_DATA *ch));
void	show_char_to_char_1	args((CHAR_DATA *victim, CHAR_DATA *ch));
void	show_char_to_char	args((CHAR_DATA *list, CHAR_DATA *ch));
bool	check_blind		args((CHAR_DATA *ch));

void	evil_eye		args((CHAR_DATA *victim, CHAR_DATA *ch));
void 	check_left_arm		args((CHAR_DATA *ch, CHAR_DATA *victim));
void 	check_right_arm		args((CHAR_DATA *ch, CHAR_DATA *victim));
void 	check_left_leg		args((CHAR_DATA *ch, CHAR_DATA *victim));
void 	check_right_leg		args((CHAR_DATA *ch, CHAR_DATA *victim));

void	obj_score		args((CHAR_DATA *ch, OBJ_DATA *obj));


char *format_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool fShort) {
    static char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';
    if(IS_SET(obj->quest, QUEST_ARTIFACT)) {
        strcat(buf, "(Artifact) ");
    } else if(IS_SET(obj->quest, QUEST_RELIC)) {
        strcat(buf, "(Relic) ");
    } else if(obj->points > 0) {
        if(IS_SET(obj->quest, QUEST_IMPROVED)) {
            strcat(buf, "(Mythical) ");
        } else {
            strcat(buf, "(Legendary) ");
        }
    }
    if(IS_SET(obj->quest, QUEST_BLOODAGONY)) {
        strcat(buf, "(Bleeding) ");
    }
    if(IS_OBJ_STAT(obj, ITEM_INVIS)) {
        strcat(buf, "(Invis) ");
    }
    if(IS_AFFECTED(ch, AFF_DETECT_EVIL)
            && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
            && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)) {
        strcat(buf, "(Blue Aura) ");
    } else if(IS_AFFECTED(ch, AFF_DETECT_EVIL)
              && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
              && !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)) {
        strcat(buf, "(Red Aura) ");
    } else if(IS_AFFECTED(ch, AFF_DETECT_EVIL)
              && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
              && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL)
              && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)) {
        strcat(buf, "(Yellow Aura) ");
    }
    if(IS_AFFECTED(ch, AFF_DETECT_MAGIC)
            && IS_OBJ_STAT(obj, ITEM_MAGIC)) {
        strcat(buf, "(Magical) ");
    }
    if(IS_OBJ_STAT(obj, ITEM_GLOW)) {
        strcat(buf, "(Glowing) ");
    }
    if(IS_OBJ_STAT(obj, ITEM_HUM)) {
        strcat(buf, "(Humming) ");
    }
    if(IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
            obj->in_room != NULL &&
            !IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
        strcat(buf, "(Umbra) ");
    }
    if(!IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
            obj->in_room != NULL &&
            IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
        strcat(buf, "(Normal plane) ");
    }
    if(!IS_NPC(ch) && IS_VAMPIRE(ch) &&
            IS_OBJ_STAT(obj, ITEM_VANISH) &&
            get_disc(ch, DISC_CHIMERSTRY) > 0) {
        strcat(buf, "(Illusion) ");
    }
    if(!IS_NPC(ch) && IS_WEREWOLF(ch) &&
            IS_SET(obj->spectype, SITEM_SILVER) &&
            get_auspice(ch, AUSPICE_AHROUN) > 1) {
        strcat(buf, "(Silver) ");
    }
    if(fShort) {
        if(obj->short_descr != NULL) {
            strcat(buf, obj->short_descr);
        }
        if(obj->condition < 100) {
            strcat(buf, " (damaged)");
        }
    } else {
        if(obj->description != NULL) {
            strcat(buf, obj->description);
        }
    }
    return buf;
}



int get_celerity(CHAR_DATA *ch) {
    if(IS_NPC(ch)) {
        return 0;
    }
    /* Old werewolves are fast, but Abominations have to burn Celerity */
    if(ch->class == CLASS_WEREWOLF && ch->pcdata->rank >
            get_disc(ch, DISC_CELERITY)) {
        return ch->pcdata->rank;
    }
    if(ch->pcdata->celerity > 0 || (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))) {
        return get_disc(ch, DISC_CELERITY);
    }
    return 0;
}


int get_disc(CHAR_DATA *ch, int disc) {
    int max = 5;
    int total_disc;
    int min_disc = 0;
    if(disc < DISC_ANIMALISM || disc > DISC_MAX) {
        bug("Get_disc: Out of discipline range.", 0);
        return 0;
    }
    if(IS_NPC(ch)) {
        return 0;
    }
    if(ch->pcdata->powers_set[disc] > 0) {
        min_disc = ch->pcdata->powers_set[disc];
    }
    if(IS_DEMON(ch)) {
        switch(disc) {
        default:
            max = 4;
            break;
        case DISC_AUSPEX:
            max = 5;
            break;
        case DISC_DAIMOINON:
            max = (!IS_ANGEL(ch)) ? 5 : 4;
            break;
        case DISC_OBEAH:
            max = (IS_ANGEL(ch))  ? 5 : 4;
            break;
        }
    } else if(ch->class != CLASS_VAMPIRE) {
        max = 4;
    }
    if(min_disc > max) {
        min_disc = max;
    }
    if(ch->pcdata->powers_set[disc] > min_disc) {
        min_disc = ch->pcdata->powers_set[disc];
    }
    if(ch->pcdata->powers_mod[disc] < 1) {
        if(ch->pcdata->powers[disc] > min_disc) {
            if(ch->pcdata->powers[disc] > max) {
                return max;
            } else {
                return ch->pcdata->powers[disc];
            }
        } else {
            return min_disc;
        }
    }
    if(ch->pcdata->powers[disc] < 1) {
        total_disc = 0;
    } else {
        total_disc = ch->pcdata->powers[disc];
    }
    if(ch->pcdata->powers_mod[disc] > 0) {
        total_disc += ch->pcdata->powers_mod[disc];
    }
    if(total_disc > max) {
        total_disc = max;
    }
    if(total_disc < min_disc) {
        return min_disc;
    }
    return total_disc;
}

int get_truedisc(CHAR_DATA *ch, int disc) {
    int total_disc;
    int min_disc = 0;
    if(disc < DISC_ANIMALISM || disc > DISC_MAX) {
        bug("Get_truedisc: Out of discipline range.", 0);
        return 0;
    }
    if(IS_NPC(ch)) {
        return 0;
    }
    if(ch->pcdata->powers_set[disc] > 0) {
        min_disc = ch->pcdata->powers_set[disc];
    }
    if(ch->pcdata->powers_mod[disc] < 1) {
        if(ch->pcdata->powers[disc] > min_disc) {
            return ch->pcdata->powers[disc];
        } else {
            return min_disc;
        }
    }
    if(ch->pcdata->powers[disc] < 1) {
        total_disc = 0;
    } else {
        total_disc = ch->pcdata->powers[disc];
    }
    if(ch->pcdata->powers_mod[disc] > 0) {
        total_disc += ch->pcdata->powers_mod[disc];
    }
    if(total_disc < min_disc) {
        return min_disc;
    }
    return total_disc;
}

int get_breed(CHAR_DATA *ch, int disc) {
    int max = 5;
    if(disc < BREED_HOMID || disc > BREED_METIS) {
        bug("Get_breed: Out of breed range.", 0);
        return 0;
    }
    if(IS_NPC(ch)) {
        return 0;
    }
    if((IS_ABOMINATION(ch) || IS_SKINDANCER(ch)) &&
            ch->pcdata->cland[0] != disc) {
        max = 3;
    }
    if(ch->pcdata->breed[disc] > max) {
        return max;
    } else if(ch->pcdata->breed[disc] < 0) {
        return 0;
    }
    return ch->pcdata->breed[disc];
}

int get_auspice(CHAR_DATA *ch, int disc) {
    int max = 5;
    if(disc < AUSPICE_RAGABASH || disc > AUSPICE_AHROUN) {
        bug("Get_auspice: Out of auspice range.", 0);
        return 0;
    }
    if(IS_NPC(ch)) {
        return 0;
    }
    if((IS_ABOMINATION(ch) || IS_SKINDANCER(ch)) &&
            ch->pcdata->cland[1] != disc) {
        max = 3;
    }
    if(ch->pcdata->auspice[disc] > max) {
        return max;
    } else if(ch->pcdata->auspice[disc] < 0) {
        return 0;
    }
    return ch->pcdata->auspice[disc];
}

int get_tribe(CHAR_DATA *ch, int disc) {
    int max = 5;
    if(disc < TRIBE_BLACK_FURIES || disc > TRIBE_WENDIGOS) {
        bug("Get_tribe: Out of tribe range.", 0);
        return 0;
    }
    if(IS_NPC(ch)) {
        return 0;
    }
    if((IS_ABOMINATION(ch) || IS_SKINDANCER(ch)) &&
            ch->pcdata->cland[2] != disc) {
        max = 3;
    }
    if(ch->pcdata->tribes[disc] > max) {
        return max;
    } else if(ch->pcdata->tribes[disc] < 0) {
        return 0;
    }
    return ch->pcdata->tribes[disc];
}

int char_hitroll(CHAR_DATA *ch) {
    int hr = GET_HITROLL(ch);
    int min;
    if(!IS_NPC(ch)) {
        int stance = ch->stance[0];
        if(stance >= 11 && stance <= 15) {
            if(IS_SET(ch->pcdata->qstats[stance - 11], QS_HIT_LESSER)) {
                hr += ch->stance[stance] * 0.5;
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_HIT_GREATER)) {
                hr += ch->stance[stance];
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_HIT_SUPREME)) {
                hr += ch->stance[stance] * 2;
            }
        }
        if((IS_VAMPIRE(ch) || IS_GHOUL(ch)) && get_celerity(ch) > 0) {
            hr += (get_celerity(ch) * 20);
        }
        if((ch->class == CLASS_VAMPIRE || IS_LICH(ch)) && ch->pcdata->wolf > 0) {
            hr += ch->beast;
        } else if(IS_SET(ch->act, PLR_WOLFMAN) && ch->pcdata->wolf > 0) {
            hr += ch->pcdata->wolf;
        } else if(ch->pcdata->wolfform[1] > FORM_HOMID && ch->pcdata->wolf > 0) {
            hr += ch->pcdata->wolf;
        }
        if(IS_DEMON(ch) && ch->pcdata->demonic > 0) {
            /* Pure demons get double the bonus */
            if(ch->class == CLASS_DEMON || ch->class == CLASS_ANGEL) {
                hr += ((ch->pcdata->demonic * ch->pcdata->demonic) << 1);
            } else {
                hr += (ch->pcdata->demonic * ch->pcdata->demonic);
            }
        }
        if(IS_HIGHLANDER(ch) && IS_ITEMAFF(ch, ITEMA_HIGHLANDER)) {
            int wpn = ch->pcdata->disc[0];
            if(wpn == 1 || wpn == 3) {
                if(ch->wpn[wpn] >= 500) {
                    hr += 500;
                } else {
                    hr += ch->wpn[wpn];
                }
            }
        }
        if(IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_STARGAZERS) > 1) {
            if(get_tribe(ch, TRIBE_STARGAZERS) > 3) {
                hr += 50;
            } else {
                hr += 25;
            }
        }
        if(IS_POLYAFF(ch, POLY_WOLF)) {
            hr += 200;
        } else if(IS_POLYAFF(ch, POLY_SERPENT)) {
            hr += 150;
        } else if(IS_POLYAFF(ch, POLY_ZULO)) {
            hr += 200;
        }
    }
    if(!IS_NPC(ch) && ch->pcdata->wolf > 0) {
        return hr;
    }
    if(!IS_NPC(ch) && ch->pcdata->exercise[EXE_RECOVER] > 0) {
        hr *= 0.75;
    } else if(ch->move < ch->max_move) {
        min = hr * 0.75;
        if(ch->move < 1 || ch->max_move < 1) {
            hr *= 0.75;
        } else {
            hr = hr * ch->move / ch->max_move;
            if(hr < min) {
                hr = min;
            }
        }
    }
    if(ch->hit < ch->max_hit && !IS_MORE(ch, MORE_NOPAIN)) {
        if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 0) {
            return hr;
        }
        min = hr * 0.75;
        if(ch->hit < 1 || ch->max_hit < 1) {
            hr *= 0.75;
        } else {
            hr = hr * ch->hit / ch->max_hit;
            if(hr < min) {
                hr = min;
            }
        }
    }
    return hr;
}

int char_damroll(CHAR_DATA *ch) {
    int dr = GET_DAMROLL(ch);
    int min;
    if(!IS_NPC(ch)) {
        int stance = ch->stance[0];
        if(stance >= 11 && stance <= 15) {
            if(IS_SET(ch->pcdata->qstats[stance - 11], QS_DAM_LESSER)) {
                dr += ch->stance[stance] * 0.5;
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_DAM_GREATER)) {
                dr += ch->stance[stance];
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_DAM_SUPREME)) {
                dr += ch->stance[stance] * 2;
            }
        }
        if(get_disc(ch, DISC_POTENCE) > 0) {
            dr += (get_disc(ch, DISC_POTENCE) * 20);
        }
        if((ch->class == CLASS_VAMPIRE && !IS_WEREWOLF(ch)) && ch->pcdata->wolf > 0) {
            dr += ch->beast;
        } else if(IS_SET(ch->act, PLR_WOLFMAN) && ch->pcdata->wolf > 0) {
            dr += ch->pcdata->wolf;
        } else if(ch->pcdata->wolfform[1] > FORM_HOMID && ch->pcdata->wolf > 0) {
            dr += ch->pcdata->wolf;
        }
        if(IS_DEMON(ch) && ch->pcdata->demonic > 0) {
            /* Pure demons get double the bonus */
            if(ch->class == CLASS_DEMON || ch->class == CLASS_ANGEL) {
                dr += ((ch->pcdata->demonic * ch->pcdata->demonic) << 1);
            } else {
                dr += (ch->pcdata->demonic * ch->pcdata->demonic);
            }
        }
        if(IS_HIGHLANDER(ch) && IS_ITEMAFF(ch, ITEMA_HIGHLANDER)) {
            int wpn = ch->pcdata->disc[0];
            {
                if(ch->wpn[wpn] >= 500) {
                    dr += 500;
                } else {
                    dr += ch->wpn[wpn];
                }
            }
        }
        if(IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_STARGAZERS) > 1) {
            if(get_tribe(ch, TRIBE_STARGAZERS) > 3) {
                dr += 50;
            } else {
                dr += 25;
            }
        }
        if(IS_POLYAFF(ch, POLY_WOLF)) {
            dr += 200;
        } else if(IS_POLYAFF(ch, POLY_SERPENT)) {
            dr += 150;
        } else if(IS_POLYAFF(ch, POLY_ZULO)) {
            dr += 200;
        }
    }
    if(!IS_NPC(ch) && ch->pcdata->wolf > 0) {
        return dr;
    }
    if(!IS_NPC(ch) && ch->pcdata->exercise[EXE_RECOVER] > 0) {
        dr *= 0.75;
    } else if(ch->move < ch->max_move) {
        min = dr * 0.75;
        if(ch->move < 1 || ch->max_move < 1) {
            dr *= 0.75;
        } else {
            dr = dr * ch->move / ch->max_move;
            if(dr < min) {
                dr = min;
            }
        }
    }
    if(ch->hit < ch->max_hit && !IS_MORE(ch, MORE_NOPAIN)) {
        if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 0) {
            return dr;
        }
        min = dr * 0.75;
        if(ch->hit < 1 || ch->max_hit < 1) {
            dr *= 0.75;
        } else {
            dr = dr * ch->hit / ch->max_hit;
            if(dr < min) {
                dr = min;
            }
        }
    }
    return dr;
}

int char_ac(CHAR_DATA *ch) {
    int a_c = GET_AC(ch);
    if(!IS_NPC(ch)) {
        int stance = ch->stance[0];
        if(stance >= 11 && stance <= 15) {
            if(IS_SET(ch->pcdata->qstats[stance - 11], QS_ARM_LESSER)) {
                a_c -= ch->stance[stance];
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_ARM_GREATER)) {
                a_c -= ch->stance[stance] * 2;
            } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_ARM_SUPREME)) {
                a_c -= ch->stance[stance] * 3;
            }
        }
        if(get_disc(ch, DISC_FORTITUDE) > 0) {
            a_c -= (get_disc(ch, DISC_FORTITUDE) * 100);
        } else if(IS_HIGHLANDER(ch) && IS_ITEMAFF(ch, ITEMA_HIGHLANDER)) {
            int wpn = ch->pcdata->disc[0];
            if(wpn == 1 || wpn == 3) {
                a_c -= ch->wpn[wpn];
            }
        }
        if(IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) > 1) {
            a_c -= 250;
        }
        if(IS_POLYAFF(ch, POLY_WOLF)) {
            a_c -= 100;
        } else if(IS_POLYAFF(ch, POLY_SERPENT)) {
            a_c -= 200;
        } else if(IS_POLYAFF(ch, POLY_BAT)) {
            a_c -= 300;
        } else if(IS_POLYAFF(ch, POLY_ZULO)) {
            a_c -= 200;
        }
    }
    if(!IS_NPC(ch) && ch->pcdata->wolf > 0) {
        return a_c;
    }
    if(!IS_NPC(ch) && ch->pcdata->exercise[EXE_RECOVER] > 0) {
        a_c += 250;
    } else if(ch->move < ch->max_move) {
        if(ch->move < 1 || ch->max_move < 1) {
            a_c += 250;
        } else {
            a_c += (ch->move / ch->max_move * 250);
        }
    }
    if(ch->hit < ch->max_hit && !IS_MORE(ch, MORE_NOPAIN)) {
        if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 0) {
            return a_c;
        }
        if(ch->hit < 1 || ch->max_hit < 1) {
            a_c += 250;
        } else {
            a_c += (ch->hit / ch->max_hit * 250);
        }
    }
    return a_c;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing) {
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;
    if(ch->desc == NULL) {
        return;
    }
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for(obj = list; obj != NULL; obj = obj->next_content) {
        count++;
    }
    prgpstrShow	= alloc_mem(count * sizeof(char *));
    prgnShow    = alloc_mem(count * sizeof(int));
    nShow	= 0;
    /*
     * Format the list of objects.
     */
    for(obj = list; obj != NULL; obj = obj->next_content) {
        if(!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch) {
            continue;
        }
        if(obj->description != NULL && strlen(obj->description) < 2) {
            continue;
        }
        if(obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)) {
            pstrShow = format_obj_to_char(obj, ch, fShort);
            fCombine = FALSE;
            if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE)) {
                /*
                 * Look for duplicates, case sensitive.
                 * Matches tend to be near end so run loop backwords.
                 */
                for(iShow = nShow - 1; iShow >= 0; iShow--) {
                    if(!strcmp(prgpstrShow[iShow], pstrShow)) {
                        prgnShow[iShow]++;
                        fCombine = TRUE;
                        break;
                    }
                }
            }
            /*
             * Couldn't combine, or didn't want to.
             */
            if(!fCombine) {
                prgpstrShow [nShow] = str_dup(pstrShow);
                prgnShow    [nShow] = 1;
                nShow++;
            }
        }
    }
    /*
     * Output the formatted list.
     */
    for(iShow = 0; iShow < nShow; iShow++) {
        if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE)) {
            if(prgnShow[iShow] != 1) {
                sprintf(buf, "(%2d) ", prgnShow[iShow]);
                send_to_char(buf, ch);
            } else {
                send_to_char("     ", ch);
            }
        }
        send_to_char(prgpstrShow[iShow], ch);
        send_to_char("\n\r", ch);
        free_string(prgpstrShow[iShow]);
    }
    if(fShowNothing && nShow == 0) {
        if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE)) {
            send_to_char("     ", ch);
        }
        send_to_char("Nothing.\n\r", ch);
    }
    /*
     * Clean up.
     */
    free_mem(prgpstrShow, count * sizeof(char *));
    free_mem(prgnShow,    count * sizeof(int));
    return;
}



void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch) {
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char buf4[MAX_STRING_LENGTH];
    char mount2[MAX_STRING_LENGTH];
    char *vname;
    char *ename;
    CHAR_DATA *mount;
    buf[0] = '\0';
    buf2[0] = '\0';
    buf3[0] = '\0';
    buf4[0] = '\0';
    if(!IS_NPC(victim) && victim->pcdata->chobj != NULL) {
        return;
    }
    if((mount = victim->mount) != NULL && IS_SET(victim->mounted, IS_MOUNT)) {
        return;
    }
    if(IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        strcat(buf, "     ");
    } else {
        if(!IS_NPC(victim) && victim->desc == NULL) {
            strcat(buf, "(Link-Dead) ");
        }
        if(IS_MORE(victim, MORE_SPIRIT)) {
            strcat(buf, "(Spirit World) ");
        }
        if(IS_AFFECTED(victim, AFF_INVISIBLE)) {
            strcat(buf, "(Invis) ");
        }
        if(IS_AFFECTED(victim, AFF_HIDE)) {
            strcat(buf, "(Hide) ");
        }
        if(IS_AFFECTED(victim, AFF_CHARM)) {
            strcat(buf, "(Charmed) ");
        }
        if(IS_AFFECTED(victim, AFF_PASS_DOOR)  ||
                IS_AFFECTED(victim, AFF_ETHEREAL)) {
            strcat(buf, "(Translucent) ");
        }
        if(IS_AFFECTED(victim, AFF_FAERIE_FIRE)) {
            strcat(buf, "(Pink Aura) ");
        }
        if(IS_EVIL(victim)
                &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
            strcat(buf, "(Red Aura) ");
        }
        if(IS_AFFECTED(victim, AFF_SANCTUARY)) {
            strcat(buf, "(White Aura) ");
        }
        if(!IS_NPC(ch) && IS_VAMPIRE(ch) &&
                IS_EXTRA(victim, EXTRA_ILLUSION) &&
                get_disc(ch, DISC_CHIMERSTRY) > 0) {
            strcat(buf, "(Illusion) ");
        }
        if(!IS_NPC(ch) && IS_WEREWOLF(ch) &&
                (IS_ITEMAFF(victim, ITEMA_LEFT_SILVER) ||
                 IS_ITEMAFF(victim, ITEMA_RIGHT_SILVER)) &&
                get_auspice(ch, AUSPICE_AHROUN) > 1) {
            strcat(buf, "(Silver) ");
        }
    }
    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        strcat(buf, "(Normal plane) ");
    } else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        strcat(buf, "(Umbra) ");
    }
    if(!IS_NPC(ch) &&
            (get_disc(ch, DISC_AUSPEX) > 0 || get_auspice(ch, AUSPICE_PHILODOX) > 1)) {
        switch(victim->class) {
        default:
            break;
        case CLASS_VAMPIRE:
            strcat(buf, "(Vampire) ");
            break;
        case CLASS_WEREWOLF:
            strcat(buf, "(Werewolf) ");
            break;
        case CLASS_ABOMINATION:
            strcat(buf, "(Abomination) ");
            break;
        case CLASS_MAGE:
            strcat(buf, "(Mage) ");
            break;
        case CLASS_LICH:
            strcat(buf, "(Lich) ");
            break;
        case CLASS_SKINDANCER:
            strcat(buf, "(Skindancer) ");
            break;
        case CLASS_DEMON:
            strcat(buf, "(Demon) ");
            break;
        case CLASS_BAALI:
            strcat(buf, "(Baali) ");
            break;
        case CLASS_BLACK_SPIRAL_DANCER:
            strcat(buf, "(Black Spiral Dancer) ");
            break;
        case CLASS_NEPHANDI:
            strcat(buf, "(Nephandi) ");
            break;
        case CLASS_HIGHLANDER:
            strcat(buf, "(Highlander) ");
            break;
        case CLASS_GHOUL:
            strcat(buf, "(Ghoul) ");
            break;
        case CLASS_ANGEL:
            strcat(buf, "(Angel) ");
            break;
        }
    }
    if(IS_WEREWOLF(victim) && IS_SET(victim->act, PLR_WOLFMAN) &&
            !IS_POLYAFF(victim, POLY_ZULO) && IS_WEREWOLF(ch)) {
        strcat(buf, "(");
        strcat(buf, victim->name);
        strcat(buf, ") ");
    } else if(!IS_NPC(ch) && !IS_NPC(victim) && IS_VAMPIRE(victim)
              && IS_VAMPAFF(victim, VAM_DISGUISED) && !IS_EXTRA(victim, EXTRA_VICISSITUDE)) {
        if((IS_VAMPIRE(ch) && get_disc(ch, DISC_AUSPEX) >=
                get_disc(victim, DISC_OBFUSCATE))
                || (IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 1)) {
            if(!IS_EXTRA(victim, EXTRA_VICISSITUDE) && !IS_SET(victim->act, PLR_WOLFMAN)) {
                strcat(buf, "(");
                strcat(buf, victim->name);
                strcat(buf, ") ");
            }
        }
    }
    if(IS_AFFECTED(victim, AFF_FLAMING)) {
        if(IS_MORE(victim, MORE_FLAMES)) {
            if(IS_NPC(victim)) {
                sprintf(buf2, "...%s is engulfed in infernal flames!", victim->short_descr);
            } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
                sprintf(buf2, "...%s is engulfed in infernal flames!", victim->morph);
            } else {
                sprintf(buf2, "...%s is engulfed in infernal flames!", victim->name);
            }
        } else {
            if(IS_NPC(victim)) {
                sprintf(buf2, "...%s is engulfed in blazing flames!", victim->short_descr);
            } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
                sprintf(buf2, "...%s is engulfed in blazing flames!", victim->morph);
            } else {
                sprintf(buf2, "...%s is engulfed in blazing flames!", victim->name);
            }
        }
    }
    if(!IS_NPC(victim) && IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        if(IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED)) {
            sprintf(buf3, "...%s is gagged and blindfolded!", victim->morph);
        } else if(IS_EXTRA(victim, GAGGED)) {
            sprintf(buf3, "...%s is gagged!", victim->morph);
        } else if(IS_EXTRA(victim, BLINDFOLDED)) {
            sprintf(buf3, "...%s is blindfolded!", victim->morph);
        }
    }
    if(IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        strcat(buf, victim->morph);
        strcat(buf, " is lying here.\n\r");
        strcat(buf, buf2);
        strcat(buf, buf3);
        strcat(buf, "\n\r");
        buf[5] = UPPER(buf[5]);
        send_to_char(buf, ch);
        return;
    }
    if(IS_EXTRA(victim, TIED_UP)) {
        if(IS_NPC(victim)) {
            sprintf(buf3, "\n\r...%s is tied up", victim->short_descr);
        } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            sprintf(buf3, "\n\r...%s is tied up", victim->morph);
        } else {
            sprintf(buf3, "\n\r...%s is tied up", victim->name);
        }
        if(IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED)) {
            strcat(buf3, ", gagged and blindfolded!");
        } else if(IS_EXTRA(victim, GAGGED)) {
            strcat(buf3, " and gagged!");
        } else if(IS_EXTRA(victim, BLINDFOLDED)) {
            strcat(buf3, " and blindfolded!");
        } else {
            strcat(buf3, "!");
        }
    }
    if(IS_AFFECTED(victim, AFF_WEBBED)) {
        if(IS_NPC(victim)) {
            sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->short_descr);
        } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->morph);
        } else {
            sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->name);
        }
        strcat(buf3, buf4);
    }
    if(IS_AFFECTED(victim, AFF_SHADOW)) {
        if(IS_NPC(victim)) {
            sprintf(buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->short_descr);
        } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            sprintf(buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->morph);
        } else {
            sprintf(buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->name);
        }
        strcat(buf3, buf4);
    }
    if(IS_AFFECTED(victim, AFF_JAIL)) {
        if(IS_NPC(victim)) {
            sprintf(buf4, "\n\r...%s is surrounded with bands of water.", victim->short_descr);
        } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            sprintf(buf4, "\n\r...%s is surrounded with bands of water.", victim->morph);
        } else {
            sprintf(buf4, "\n\r...%s is surrounded with bands of water.", victim->name);
        }
        strcat(buf3, buf4);
    }
    if(victim->embraced == ARE_EMBRACING && victim->embrace != NULL) {
        CHAR_DATA *emb = victim->embrace;
        if(IS_NPC(victim)) {
            vname = str_dup(victim->short_descr);
        } else if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            vname = str_dup(victim->morph);
        } else {
            vname = str_dup(victim->name);
        }
        if(IS_NPC(emb)) {
            ename = str_dup(emb->short_descr);
        } else if(IS_AFFECTED(emb, AFF_POLYMORPH)) {
            ename = str_dup(emb->morph);
        } else {
            ename = str_dup(emb->name);
        }
        if(ch == victim) {
            sprintf(buf4, "\n\r...You are holding %s in a close embrace.", ename);
        } else if(ch == emb) {
            sprintf(buf4, "\n\r...%s is holding you in a close embrace.", vname);
        } else {
            sprintf(buf4, "\n\r...%s is holding %s in a close embrace.", vname, ename);
        }
        strcat(buf3, buf4);
    } else if(victim->embraced == ARE_EMBRACED && victim->embrace != NULL) {
        CHAR_DATA *emb = victim->embrace;
        if(IS_NPC(victim)) {
            vname = str_dup(victim->short_descr);
        } else if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            vname = str_dup(victim->morph);
        } else {
            vname = str_dup(victim->name);
        }
        if(IS_NPC(emb)) {
            ename = str_dup(emb->short_descr);
        } else if(IS_AFFECTED(emb, AFF_POLYMORPH)) {
            ename = str_dup(emb->morph);
        } else {
            ename = str_dup(emb->name);
        }
        if(ch == victim) {
            sprintf(buf4, "\n\r...You are being held by %s in a close embrace.", ename);
        } else if(ch == emb) {
            sprintf(buf4, "\n\r...%s is being held by you in a close embrace.", vname);
        } else {
            sprintf(buf4, "\n\r...%s is being held by %s in a close embrace.", vname, ename);
        }
        strcat(buf3, buf4);
    }
    if(victim->position == POS_STANDING && victim->long_descr[0] != '\0' &&
            strlen(victim->long_descr) > 3 && (mount = victim->mount) == NULL &&
            victim->stance[0] < 1) {
        strcpy(buf4, victim->long_descr);
        buf4[strlen(buf4) - 2] = '\0';
        strcat(buf, buf4);
        strcat(buf, buf2);
        strcat(buf, buf3);
        /*
        	if (buf2[0] != '\0') strcat( buf, "\n\r" );
        */
        strcat(buf, "\n\r");
        buf[0] = UPPER(buf[0]);
        send_to_char(buf, ch);
        if(!IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF)) {
            return;
        }
        if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF)) {
            if(IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD)) {
                act("...$N is surrounded by a crackling shield of lightning.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_FIRESHIELD)) {
                act("...$N is surrounded by a burning shield of fire.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_ICESHIELD)) {
                act("...$N is surrounded by a shimmering shield of ice.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_ACIDSHIELD)) {
                act("...$N is surrounded by a bubbling shield of acid.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD)) {
                act("...$N is surrounded by a swirling shield of chaos.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_REFLECT)) {
                act("...$N is surrounded by a flickering shield of darkness.", ch, NULL, victim, TO_CHAR);
            }
            if(IS_ITEMAFF(victim, ITEMA_SUNSHIELD)) {
                act("...$N is surrounded by a glowing halo of sunlight.", ch, NULL, victim, TO_CHAR);
            }
        }
        return;
    } else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        strcat(buf, victim->morph);
    } else {
        strcat(buf, PERS(victim, ch));
    }
    if((mount = victim->mount) != NULL && victim->mounted == IS_RIDING) {
        if(IS_NPC(mount)) {
            sprintf(mount2, " is here riding %s", mount->short_descr);
        } else {
            sprintf(mount2, " is here riding %s", mount->name);
        }
        strcat(buf, mount2);
        if(victim->position == POS_FIGHTING) {
            strcat(buf, ", fighting ");
            if(victim->fighting == NULL) {
                strcat(buf, "thin air??");
            } else if(victim->fighting == ch) {
                strcat(buf, "YOU!");
            } else if(victim->in_room == victim->fighting->in_room) {
                strcat(buf, PERS(victim->fighting, ch));
                strcat(buf, ".");
            } else {
                strcat(buf, "somone who left??");
            }
        } else {
            strcat(buf, ".");
        }
    } else if(victim->position == POS_STANDING && IS_AFFECTED(victim, AFF_FLYING)) {
        strcat(buf, " is hovering here.");
    } else if(victim->position == POS_STANDING && (!IS_NPC(victim) && (IS_VAMPAFF(victim, VAM_FLYING)))) {
        strcat(buf, " is hovering here.");
    } else {
        switch(victim->position) {
        case POS_DEAD:
            strcat(buf, " is DEAD!!");
            break;
        case POS_MORTAL:
            if(IS_VAMPIRE(victim)) {
                strcat(buf, " is lying here torpored.");
            } else {
                strcat(buf, " is lying here mortally wounded.");
            }
            break;
        case POS_INCAP:
            strcat(buf, " is lying here incapacitated.");
            break;
        case POS_STUNNED:
            strcat(buf, " is lying here stunned.");
            break;
        case POS_SLEEPING:
            if(victim->sat != NULL) {
                sprintf(buf4, " is sleeping on %s here.", victim->sat->short_descr);
                strcat(buf, buf4);
            } else {
                strcat(buf, " is sleeping here.");
            }
            break;
        case POS_RESTING:
            if(victim->sat != NULL) {
                sprintf(buf4, " is resting on %s here.", victim->sat->short_descr);
                strcat(buf, buf4);
            } else {
                strcat(buf, " is resting here.");
            }
            break;
        case POS_MEDITATING:
            if(!IS_NPC(victim) && victim->pcdata->exercise[EXE_TYPE] > 0) {
                switch(victim->pcdata->exercise[EXE_TYPE]) {
                default:
                    strcat(buf, " is exercising here.");
                    break;
                case EXE_PUSHUPS:
                    strcat(buf, " is doing pushups on the floor here.");
                    break;
                case EXE_SKIPPING:
                    strcat(buf, " is doing skipping here.");
                    break;
                case EXE_LEGRAISES:
                    strcat(buf, " is doing leg-raises here.");
                    break;
                }
            } else if(victim->sat != NULL) {
                sprintf(buf4, " is meditating on %s here.", victim->sat->short_descr);
                strcat(buf, buf4);
            } else {
                strcat(buf, " is meditating here.");
            }
            break;
        case POS_SITTING:
            if(victim->sat != NULL) {
                sprintf(buf4, " is sitting on %s here.", victim->sat->short_descr);
                strcat(buf, buf4);
            } else {
                strcat(buf, " is sitting here.");
            }
            break;
        case POS_STANDING:
            if(!IS_NPC(victim) && victim->stance[0] == STANCE_NORMAL) {
                strcat(buf, " is here, crouched in a fighting stance.");
            } else if(victim->stance[0] == STANCE_VIPER) {
                strcat(buf, " is here, crouched in a viper fighting stance.");
            } else if(victim->stance[0] == STANCE_CRANE) {
                strcat(buf, " is here, crouched in a crane fighting stance.");
            } else if(victim->stance[0] == STANCE_CRAB) {
                strcat(buf, " is here, crouched in a crab fighting stance.");
            } else if(victim->stance[0] == STANCE_MONGOOSE) {
                strcat(buf, " is here, crouched in a mongoose fighting stance.");
            } else if(victim->stance[0] == STANCE_BULL) {
                strcat(buf, " is here, crouched in a bull fighting stance.");
            } else if(victim->stance[0] == STANCE_MANTIS) {
                strcat(buf, " is here, crouched in a mantis fighting stance.");
            } else if(victim->stance[0] == STANCE_DRAGON) {
                strcat(buf, " is here, crouched in a dragon fighting stance.");
            } else if(victim->stance[0] == STANCE_TIGER) {
                strcat(buf, " is here, crouched in a tiger fighting stance.");
            } else if(victim->stance[0] == STANCE_MONKEY) {
                strcat(buf, " is here, crouched in a monkey fighting stance.");
            } else if(victim->stance[0] == STANCE_SWALLOW) {
                strcat(buf, " is here, crouched in a swallow fighting stance.");
            } else if(!IS_NPC(victim) && victim->stance[0] >= 11 &&
                      victim->stance[0] <= 15) {
                if(strlen(victim->pcdata->stancename) > 1 && strlen(victim->pcdata->stancename) < 20) {
                    sprintf(buf4, " is here, crouched in a %s fighting stance.", victim->pcdata->stancename);
                    strcat(buf, buf4);
                } else {
                    strcat(buf, " is here, crouched in a strange fighting stance.");
                }
            } else {
                strcat(buf, " is here.");
            }
            break;
        case POS_FIGHTING:
            strcat(buf, " is here, fighting ");
            if(victim->fighting == NULL) {
                strcat(buf, "thin air??");
            } else if(victim->fighting == ch) {
                strcat(buf, "YOU!");
            } else if(victim->in_room == victim->fighting->in_room) {
                strcat(buf, PERS(victim->fighting, ch));
                strcat(buf, ".");
            } else {
                strcat(buf, "somone who left??");
            }
            break;
        }
    }
    if(buf2[0] != '\0') {
        strcat(buf, "\n\r");
    }
    strcat(buf, buf2);
    strcat(buf, buf3);
    strcat(buf, "\n\r");
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF)) {
        return;
    }
    if(IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD)) {
        act("...$N is surrounded by a crackling shield of lightning.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_FIRESHIELD)) {
        act("...$N is surrounded by a burning shield of fire.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_ICESHIELD)) {
        act("...$N is surrounded by a shimmering shield of ice.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_ACIDSHIELD)) {
        act("...$N is surrounded by a bubbling shield of acid.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD)) {
        act("...$N is surrounded by a swirling shield of chaos.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_REFLECT)) {
        act("...$N is surrounded by a flickering shield of darkness.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ITEMAFF(victim, ITEMA_SUNSHIELD)) {
        act("...$N is surrounded by a glowing halo of sunlight.", ch, NULL, victim, TO_CHAR);
    }
    return;
}



void evil_eye(CHAR_DATA *victim, CHAR_DATA *ch) {
    char buf [MAX_STRING_LENGTH];
    int tabletype;
    if(IS_NPC(victim)) {
        return;
    }
    if(victim == ch) {
        return;
    }
    if(IS_IMMORTAL(ch) || IS_IMMORTAL(victim)) {
        return;
    }
    if(!IS_MORE(victim, MORE_EVILEYE)) {
        return;
    }
    if(victim->pcdata->evileye > 0) {
        return;
    }
    victim->pcdata->evileye = 10;
    if(victim->powertype != NULL && strlen(victim->powertype) > 1) {
        sprintf(buf, "\n\r%s\n\r", victim->powertype);
        send_to_char(buf, ch);
    }
    if(IS_SET(victim->spectype, EYE_SELFACTION) &&
            strlen(victim->poweraction) > 1) {
        if(IS_NPC(victim)) {
            SET_BIT(victim->act, ACT_COMMANDED);
        }
        interpret(victim, victim->poweraction);
        if(IS_NPC(victim)) {
            REMOVE_BIT(victim->act, ACT_COMMANDED);
        }
    }
    if(IS_SET(victim->spectype, EYE_ACTION) &&
            strlen(victim->poweraction) > 1) {
        interpret(ch, victim->poweraction);
    }
    if(IS_SET(victim->spectype, EYE_SPELL) && victim->specpower > 0) {
        tabletype = skill_table[victim->specpower].target;
        (*skill_table[victim->specpower].spell_fun)(victim->specpower, victim->spl[tabletype], victim, ch);
    }
    return;
}


void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;
    if(can_see(victim, ch)) {
        act("$n looks at you.", ch, NULL, victim, TO_VICT);
        act("$n looks at $N.",  ch, NULL, victim, TO_NOTVICT);
    }
    if(!IS_NPC(ch) && IS_HEAD(victim, LOST_HEAD)) {
        act("$N is lying here.", ch, NULL, victim, TO_CHAR);
        return;
    }
    if(victim->description[0] != '\0') {
        send_to_char(victim->description, ch);
    } else {
        act("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
    }
    if(victim->max_hit > 0) {
        percent = (100 * victim->hit) / victim->max_hit;
    } else {
        percent = -1;
    }
    strcpy(buf, PERS(victim, ch));
    if(percent >= 100) {
        strcat(buf, " is in perfect health.\n\r");
    } else if(percent >=  90) {
        strcat(buf, " is slightly scratched.\n\r");
    } else if(percent >=  80) {
        strcat(buf, " has a few bruises.\n\r");
    } else if(percent >=  70) {
        strcat(buf, " has some cuts.\n\r");
    } else if(percent >=  60) {
        strcat(buf, " has several wounds.\n\r");
    } else if(percent >=  50) {
        strcat(buf, " has many nasty wounds.\n\r");
    } else if(percent >=  40) {
        strcat(buf, " is bleeding freely.\n\r");
    } else if(percent >=  30) {
        strcat(buf, " is covered in blood.\n\r");
    } else if(percent >=  20) {
        strcat(buf, " is leaking guts.\n\r");
    } else if(percent >=  10) {
        strcat(buf, " is almost dead.\n\r");
    } else {
        strcat(buf, " is DYING.\n\r");
    }
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    if(!IS_NPC(victim) && IS_SET(victim->act, PLR_WOLFMAN)) {
        if(IS_MORE(victim, MORE_BRISTLES)) {
            act("$N's body is covered in thick fur and quill-like spines.", ch, NULL, victim, TO_CHAR);
        } else {
            act("$N's body is covered in thick fur.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(IS_WEREWOLF(victim) && IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) {
        act("$N's eyes are glowing bright green.", ch, NULL, victim, TO_CHAR);
    } else if(IS_ANGEL(victim) && IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) {
        act("$N's eyes are glowing bright blue.", ch, NULL, victim, TO_CHAR);
    } else if(IS_AFFECTED(victim, AFF_INFRARED) || IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) {
        act("$N's eyes are glowing bright red.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_VAMPAFF(victim, VAM_FANGS)) {
        act("$N has a pair of long, pointed fangs.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_AFFECTED(victim, AFF_FLYING)) {
        act("$N is hovering in the air.", ch, NULL, victim, TO_CHAR);
    }
    if(!IS_NPC(victim) && IS_VAMPIRE(victim) &&
            IS_VAMPAFF(victim, VAM_CLAWS) && !IS_ABOMINATION(victim)) {
        if(IS_POLYAFF(victim, POLY_ZULO)) {
            if(IS_ANGEL(ch)) {
                act("$N has long golden talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            } else {
                act("$N has long black talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            }
        } else if(IS_POLYAFF(victim, POLY_WOLF)) {
            act("$N has razor sharp talons extending from $S paws.", ch, NULL, victim, TO_CHAR);
        } else if(IS_POLYAFF(victim, POLY_BAT)) {
            act("$N has tiny, razor sharp claws extending from $S wing tips.", ch, NULL, victim, TO_CHAR);
        } else {
            act("$N has razor sharp claws protruding from under $S finger nails.", ch, NULL, victim, TO_CHAR);
        }
    } else if(!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CLAWS)) {
        if(IS_WEREWOLF(victim) && get_auspice(victim, AUSPICE_AHROUN) > 3) {
            if(get_tribe(victim, TRIBE_BLACK_FURIES) > 1) {
                act("$N has venom-dripping silver talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            } else {
                act("$N has gleaming silver talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            }
        } else {
            if(IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_BLACK_FURIES) > 1) {
                act("$N has venom-dripping talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            } else if(IS_POLYAFF(victim, POLY_ZULO)) {
                act("$N has long black talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            } else {
                act("$N has razor sharp talons extending from $S fingers.", ch, NULL, victim, TO_CHAR);
            }
        }
    }
    if(!IS_NPC(victim) && IS_MORE(victim, MORE_HAND_FLAME)) {
        if(IS_ANGEL(victim)) {
            act("$N's hands are surrounded with a glowing blue nimbus.", ch, NULL, victim, TO_CHAR);
        } else {
            act("$N's hands are engulfed in blazing flames.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(!IS_NPC(victim) && IS_MORE(victim, MORE_OBEAH)) {
        if(victim->pcdata->obeah > 0) {
            act("$N has a brightly glowing eye in the centre of $S forehead.", ch, NULL, victim, TO_CHAR);
        } else {
            act("$N has an eye in the centre of $S forehead.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(!IS_NPC(victim)) {
        if(IS_MOD(victim, MOD_TUSKS)) {
            act("$N has a pair of tusks extending from $S mouth.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_MOD(victim, MOD_HORNS)) {
            act("$N has a pair of pointed horns extending from $S head.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_DEMON(victim) && IS_DEMAFF(victim, DEM_WINGS)) {
            if(IS_ANGEL(victim)) {
                if(IS_DEMAFF(victim, DEM_UNFOLDED)) {
                    act("$N has a pair of white feathered wings spread out behind $S back.", ch, NULL, victim, TO_CHAR);
                } else {
                    act("$N has a pair of white feathered wings folded behind $S back.", ch, NULL, victim, TO_CHAR);
                }
            } else {
                if(IS_DEMAFF(victim, DEM_UNFOLDED)) {
                    act("$N has a pair of batlike wings spread out behind $S back.", ch, NULL, victim, TO_CHAR);
                } else {
                    act("$N has a pair of batlike wings folded behind $S back.", ch, NULL, victim, TO_CHAR);
                }
            }
        } else if(IS_MOD(victim, MOD_WINGS)) {
            if(IS_MOD(victim, MOD_UNFOLDED)) {
                if(IS_ANGEL(victim)) {
                    act("$N has a pair of white feathered wings spread out behind $S back.", ch, NULL, victim, TO_CHAR);
                } else {
                    act("$N has a pair of batlike wings spread out behind $S back.", ch, NULL, victim, TO_CHAR);
                }
            } else {
                if(IS_ANGEL(victim)) {
                    act("$N has a pair of white feathered wings folded behind $S back.", ch, NULL, victim, TO_CHAR);
                } else {
                    act("$N has a pair of batlike wings folded behind $S back.", ch, NULL, victim, TO_CHAR);
                }
            }
        }
        if(IS_MOD(victim, MOD_FINS)) {
            act("$N has a pair of fins extending from the sides of $S body.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_MOD(victim, MOD_TAIL)) {
            act("$N has a long tail hanging from $S backside.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_MOD(victim, MOD_EXOSKELETON)) {
            act("$N is covered with an armoured exoskeleton.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_DEMON(victim) && IS_MORE(victim, MORE_GUARDIAN)) {
            if(IS_ANGEL(victim)) {
                act("$N has a little cherub perched on $S shoulder.", ch, NULL, victim, TO_CHAR);
            } else {
                act("$N has a little imp perched on $S shoulder.", ch, NULL, victim, TO_CHAR);
            }
        }
    }
    found = FALSE;
    for(iWear = 0; iWear < MAX_WEAR; iWear++) {
        if((obj = get_eq_char(victim, iWear)) != NULL
                &&   can_see_obj(ch, obj)) {
            if(!found) {
                send_to_char("\n\r", ch);
                act("$N is using:", ch, NULL, victim, TO_CHAR);
                found = TRUE;
            }
            send_to_char(where_name[iWear], ch);
            if(IS_NPC(ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj != obj) {
                send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
                send_to_char("\n\r", ch);
            } else {
                send_to_char("you\n\r", ch);
            }
        }
    }
    /*
        if ( victim != ch
        &&   !IS_NPC(ch)
        &&   !IS_HEAD(victim, LOST_HEAD)
        &&   number_percent( ) < ch->pcdata->learned[gsn_peek] )
        {
    	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
    	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
        }
    */
    return;
}



void show_char_to_char(CHAR_DATA *list, CHAR_DATA *ch) {
    CHAR_DATA *rch;
    for(rch = list; rch != NULL; rch = rch->next_in_room) {
        if(rch == ch) {
            continue;
        }
        if(IS_IMMORTAL(ch) || IS_IMMORTAL(rch)) {
            if(!IS_NPC(rch)
                    &&   IS_SET(rch->act, PLR_WIZINVIS)
                    &&   get_trust(ch) < get_trust(rch)) {
                continue;
            }
        } else {
            if(!IS_NPC(rch)
                    && (IS_SET(rch->act, PLR_WIZINVIS)
                        ||   IS_ITEMAFF(rch, ITEMA_VANISH))
                    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
                    &&   !IS_ITEMAFF(ch, ITEMA_VISION)) {
                continue;
            }
        }
        if(!IS_NPC(rch)) {
            if(IS_HEAD(rch, LOST_HEAD)
                    || IS_EXTRA(rch, EXTRA_OSWITCH)
                    || IS_VAMPAFF(rch, VAM_EARTHMELDED)) {
                continue;
            }
        }
        if(can_see(ch, rch)) {
            show_char_to_char_0(rch, ch);
        } else if(room_is_dark(ch->in_room)
                  && (IS_AFFECTED(rch, AFF_INFRARED) || IS_VAMPAFF(rch, VAM_NIGHTSIGHT))) {
            if(IS_WEREWOLF(rch) && IS_VAMPAFF(rch, VAM_NIGHTSIGHT)) {
                send_to_char("You see glowing green eyes watching YOU!\n\r", ch);
            } else if(IS_ANGEL(rch) && IS_VAMPAFF(rch, VAM_NIGHTSIGHT)) {
                send_to_char("You see glowing blue eyes watching YOU!\n\r", ch);
            } else {
                send_to_char("You see glowing red eyes watching YOU!\n\r", ch);
            }
        }
    }
    return;
}



bool check_blind(CHAR_DATA *ch) {
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) {
        return TRUE;
    }
    if(IS_ITEMAFF(ch, ITEMA_VISION)) {
        return TRUE;
    }
    if(IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R)) {
        send_to_char("You have no eyes to see with!\n\r", ch);
        return FALSE;
    }
    if(IS_EXTRA(ch, BLINDFOLDED)) {
        send_to_char("You can't see a thing through the blindfold!\n\r", ch);
        return FALSE;
    }
    if(IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT)) {
        send_to_char("You can't see a thing!\n\r", ch);
        return FALSE;
    }
    return TRUE;
}



void do_look(CHAR_DATA *ch, char *argument) {
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *wizard;
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char *pdesc;
    int door;
    bool found;
    if(ch->desc == NULL && (wizard = ch->wizard) == NULL) {
        return;
    }
    if(ch->in_room == NULL) {
        return;
    }
    if(!IS_NPC(ch) && IS_VAMPIRE(ch) &&
            get_disc(ch, DISC_PROTEAN) > 7 &&
            ch->position < POS_SLEEPING) {
        WAIT_STATE(ch, 12);
    } else if(ch->position < POS_SLEEPING) {
        send_to_char("You can't see anything but stars!\n\r", ch);
        return;
    }
    if(ch->position == POS_SLEEPING) {
        send_to_char("You can't see anything, you're sleeping!\n\r", ch);
        return;
    }
    if(!check_blind(ch)) {
        return;
    }
    if(!IS_NPC(ch) && ch->in_room != NULL &&
            IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS) &&
            !IS_ITEMAFF(ch, ITEMA_VISION) && !IS_IMMORTAL(ch)) {
        if(!IS_VAMPIRE(ch) || get_disc(ch, DISC_OBTENEBRATION) == 0) {
            if(!IS_WEREWOLF(ch) || get_tribe(ch, TRIBE_UKTENA) < 1) {
                if(IS_NPC(ch) || !IS_DEMON(ch) || !IS_SET(ch->pcdata->disc[C_POWERS], DEM_SHADOWSIGHT)) {
                    send_to_char("It is pitch black ... \n\r", ch);
                    return;
                }
            }
        }
    }
    if(!IS_NPC(ch)
            &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
            &&   !IS_ITEMAFF(ch, ITEMA_VISION)
            &&   !IS_VAMPAFF(ch, VAM_NIGHTSIGHT)
            &&   !IS_AFFECTED(ch, AFF_SHADOWPLANE)
            &&   !(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
                   &&   !IS_NPC(ch) && ch->pcdata->chobj != NULL
                   &&   ch->pcdata->chobj->in_obj != NULL)
            &&   room_is_dark(ch->in_room)) {
        send_to_char("It is pitch black ... \n\r", ch);
        show_char_to_char(ch->in_room->people, ch);
        return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(arg1[0] == '\0' || !str_cmp(arg1, "auto")) {
        /* 'look' or 'look auto' */
        if(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
                && !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL) {
            act("$p", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
        } else if(IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
            send_to_char("The Umbra\n\r", ch);
        } else {
            sprintf(buf, "%s\n\r", ch->in_room->name);
            send_to_char(buf, ch);
        }
        if(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) {
            do_exits(ch, "auto");
        }
        if(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
                && !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL) {
            act("You are inside $p.", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
            show_list_to_char(ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE);
        } else if((arg1[0] == '\0' || !str_cmp(arg1, "auto")) && IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
            send_to_char("You are standing in complete darkness.\n\r", ch);
        } else if((!IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF)) &&
                  (arg1[0] == '\0' || !str_cmp(arg1, "auto"))) {
            send_to_char(ch->in_room->description, ch);
            if(ch->in_room->blood == 1000) {
                sprintf(buf, "You notice that the room is completely drenched in blood.\n\r");
            } else if(ch->in_room->blood > 750) {
                sprintf(buf, "You notice that there is a very large amount of blood around the room.\n\r");
            } else if(ch->in_room->blood > 500) {
                sprintf(buf, "You notice that there is a large quantity of blood around the room.\n\r");
            } else if(ch->in_room->blood > 250) {
                sprintf(buf, "You notice a fair amount of blood on the floor.\n\r");
            } else if(ch->in_room->blood > 100) {
                sprintf(buf, "You notice several blood stains on the floor.\n\r");
            } else if(ch->in_room->blood > 50) {
                sprintf(buf, "You notice a few blood stains on the floor.\n\r");
            } else if(ch->in_room->blood > 25) {
                sprintf(buf, "You notice a couple of blood stains on the floor.\n\r");
            } else if(ch->in_room->blood > 0) {
                sprintf(buf, "You notice a few drops of blood on the floor.\n\r");
            } else {
                sprintf(buf, "You notice nothing special in the room.\n\r");
            }
            ADD_COLOUR(ch, buf, L_RED);
            if(ch->in_room->blood > 0) {
                send_to_char(buf, ch);
            }
        }
        show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);
        show_char_to_char(ch->in_room->people,   ch);
        return;
    }
    if(!str_cmp(arg1, "i") || !str_cmp(arg1, "in")) {
        /* 'look in' */
        if(arg2[0] == '\0') {
            send_to_char("Look in what?\n\r", ch);
            return;
        }
        if((obj = get_obj_here(ch, arg2)) == NULL) {
            send_to_char("You do not see that here.\n\r", ch);
            return;
        }
        switch(obj->item_type) {
        default:
            send_to_char("That is not a container.\n\r", ch);
            break;
        case ITEM_PORTAL:
            pRoomIndex = get_room_index(obj->value[0]);
            location = ch->in_room;
            if(pRoomIndex == NULL) {
                send_to_char("It doesn't seem to lead anywhere.\n\r", ch);
                return;
            }
            if(obj->value[2] == 1 || obj->value[2] == 3) {
                send_to_char("It seems to be closed.\n\r", ch);
                return;
            }
            char_from_room(ch);
            char_to_room(ch, pRoomIndex);
            found = FALSE;
            for(portal = ch->in_room->contents; portal != NULL; portal = portal_next) {
                portal_next = portal->next_content;
                if((obj->value[0] == portal->value[3])
                        && (obj->value[3] == portal->value[0])) {
                    found = TRUE;
                    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                            !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE)) {
                        REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        do_look(ch, "auto");
                        SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        break;
                    } else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                              IS_SET(portal->extra_flags, ITEM_SHADOWPLANE)) {
                        SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        do_look(ch, "auto");
                        REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        break;
                    } else {
                        do_look(ch, "auto");
                        break;
                    }
                }
            }
            char_from_room(ch);
            char_to_room(ch, location);
            break;
        case ITEM_DRINK_CON:
            if(obj->value[1] <= 0) {
                send_to_char("It is empty.\n\r", ch);
                break;
            }
            if(obj->value[1] < obj->value[0] / 5) {
                sprintf(buf, "There is a little %s liquid left in it.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 4) {
                sprintf(buf, "It contains a small about of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 3) {
                sprintf(buf, "It's about a third full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 2) {
                sprintf(buf, "It's about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0]) {
                sprintf(buf, "It is almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] == obj->value[0]) {
                sprintf(buf, "It's completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else {
                sprintf(buf, "Somehow it is MORE than full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            }
            send_to_char(buf, ch);
            break;
        case ITEM_CONTAINER:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
            if(IS_SET(obj->value[1], CONT_CLOSED)) {
                send_to_char("It is closed.\n\r", ch);
                break;
            }
            act("$p contains:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE);
            break;
        }
        return;
    }
    if((victim = get_char_room(ch, arg1)) != NULL) {
        show_char_to_char_1(victim, ch);
        evil_eye(victim, ch);
        return;
    }
    for(vch = char_list; vch != NULL; vch = vch_next) {
        vch_next	= vch->next;
        if(vch->in_room == NULL) {
            continue;
        }
        if(vch->in_room == ch->in_room) {
            if(!IS_NPC(vch) && !str_cmp(arg1, vch->morph)) {
                show_char_to_char_1(vch, ch);
                evil_eye(vch, ch);
                return;
            }
            continue;
        }
    }
    if(!IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL) {
        obj = get_obj_in_obj(ch, arg1);
        if(obj != NULL) {
            send_to_char(obj->description, ch);
            send_to_char("\n\r", ch);
            return;
        }
    }
    for(obj = ch->carrying; obj != NULL; obj = obj->next_content) {
        if(!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch) {
            continue;
        }
        if(can_see_obj(ch, obj)) {
            pdesc = get_extra_descr(arg1, obj->extra_descr);
            if(pdesc != NULL) {
                send_to_char(pdesc, ch);
                return;
            }
            pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
            if(pdesc != NULL) {
                send_to_char(pdesc, ch);
                return;
            }
        }
        if(is_name(arg1, obj->name)) {
            send_to_char(obj->description, ch);
            send_to_char("\n\r", ch);
            return;
        }
    }
    for(obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
        if(!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch) {
            continue;
        }
        if(can_see_obj(ch, obj)) {
            pdesc = get_extra_descr(arg1, obj->extra_descr);
            if(pdesc != NULL) {
                send_to_char(pdesc, ch);
                return;
            }
            pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
            if(pdesc != NULL) {
                send_to_char(pdesc, ch);
                return;
            }
        }
        if(is_name(arg1, obj->name)) {
            send_to_char(obj->description, ch);
            send_to_char("\n\r", ch);
            return;
        }
    }
    pdesc = get_extra_descr(arg1, ch->in_room->extra_descr);
    if(pdesc != NULL) {
        send_to_char(pdesc, ch);
        return;
    }
    if(!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) {
        door = 0;
    } else if(!str_cmp(arg1, "e") || !str_cmp(arg1, "east")) {
        door = 1;
    } else if(!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) {
        door = 2;
    } else if(!str_cmp(arg1, "w") || !str_cmp(arg1, "west")) {
        door = 3;
    } else if(!str_cmp(arg1, "u") || !str_cmp(arg1, "up")) {
        door = 4;
    } else if(!str_cmp(arg1, "d") || !str_cmp(arg1, "down")) {
        door = 5;
    } else {
        send_to_char("You do not see that here.\n\r", ch);
        return;
    }
    /* 'look direction' */
    if((pexit = ch->in_room->exit[door]) == NULL) {
        send_to_char("Nothing special there.\n\r", ch);
        return;
    }
    /*
        if ( pexit->description != NULL && pexit->description[0] != '\0' )
    	send_to_char( pexit->description, ch );
        else
    	send_to_char( "Nothing special there.\n\r", ch );
    */
    if(pexit->keyword    != NULL
            &&   pexit->keyword[0] != '\0'
            &&   pexit->keyword[0] != ' ') {
        if(IS_SET(pexit->exit_info, EX_CLOSED)) {
            act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
        } else if(IS_SET(pexit->exit_info, EX_ISDOOR)) {
            act("The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR);
            if((pexit = ch->in_room->exit[door]) == NULL) {
                return;
            }
            if((pRoomIndex = pexit->to_room) == NULL) {
                return;
            }
            location = ch->in_room;
            char_from_room(ch);
            char_to_room(ch, pRoomIndex);
            do_look(ch, "auto");
            char_from_room(ch);
            char_to_room(ch, location);
        } else {
            if((pexit = ch->in_room->exit[door]) == NULL) {
                return;
            }
            if((pRoomIndex = pexit->to_room) == NULL) {
                return;
            }
            location = ch->in_room;
            char_from_room(ch);
            char_to_room(ch, pRoomIndex);
            do_look(ch, "auto");
            char_from_room(ch);
            char_to_room(ch, location);
        }
    } else {
        if((pexit = ch->in_room->exit[door]) == NULL) {
            return;
        }
        if((pRoomIndex = pexit->to_room) == NULL) {
            return;
        }
        location = ch->in_room;
        char_from_room(ch);
        char_to_room(ch, pRoomIndex);
        do_look(ch, "auto");
        char_from_room(ch);
        char_to_room(ch, location);
    }
    return;
}



void do_examine(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    one_argument(argument, arg);
    if(arg[0] == '\0') {
        send_to_char("Examine what?\n\r", ch);
        return;
    }
    do_look(ch, arg);
    if((obj = get_obj_here(ch, arg)) != NULL) {
        if(obj->condition >= 100) {
            sprintf(buf, "You notice that %s is in perfect condition.\n\r", obj->short_descr);
        } else if(obj->condition >= 75) {
            sprintf(buf, "You notice that %s is in good condition.\n\r", obj->short_descr);
        } else if(obj->condition >= 50) {
            sprintf(buf, "You notice that %s is in average condition.\n\r", obj->short_descr);
        } else if(obj->condition >= 25) {
            sprintf(buf, "You notice that %s is in poor condition.\n\r", obj->short_descr);
        } else {
            sprintf(buf, "You notice that %s is in awful condition.\n\r", obj->short_descr);
        }
        send_to_char(buf, ch);
        switch(obj->item_type) {
        default:
            break;
        case ITEM_DRINK_CON:
        case ITEM_CONTAINER:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
            send_to_char("When you look inside, you see:\n\r", ch);
            sprintf(buf, "in %s", arg);
            do_look(ch, buf);
        }
    }
    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA *ch, char *argument) {
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;
    buf[0] = '\0';
    fAuto  = !str_cmp(argument, "auto");
    if(!check_blind(ch)) {
        return;
    }
    strcpy(buf, fAuto ? "[Exits:" : "Obvious exits:\n\r");
    found = FALSE;
    for(door = 0; door <= 5; door++) {
        if((pexit = ch->in_room->exit[door]) != NULL
                &&   pexit->to_room != NULL
                &&   !IS_SET(pexit->exit_info, EX_CLOSED)) {
            found = TRUE;
            if(fAuto) {
                strcat(buf, " ");
                strcat(buf, dir_name[door]);
            } else {
                if(IS_SET(world_affects, WORLD_FOG)) {
                    sprintf(buf + strlen(buf), "%-5s - Too foggy to tell.\n\r",
                            capitalize(dir_name[door]));
                } else if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) {
                    sprintf(buf + strlen(buf), "%-5s - %s\n\r",
                            capitalize(dir_name[door]), pexit->to_room->name);
                } else sprintf(buf + strlen(buf), "%-5s - %s\n\r",
                                   capitalize(dir_name[door]),
                                   room_is_dark(pexit->to_room)
                                   ?  "Too dark to tell"
                                   : pexit->to_room->name
                                  );
            }
        }
    }
    if(!found) {
        strcat(buf, fAuto ? " none" : "None.\n\r");
    }
    if(fAuto) {
        strcat(buf, "]\n\r");
    }
    send_to_char(buf, ch);
    return;
}



void do_score(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char ss1[MAX_STRING_LENGTH];
    char ss2[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int a_c = char_ac(ch);
    if(!IS_NPC(ch) && (IS_EXTRA(ch, EXTRA_OSWITCH) || IS_HEAD(ch, LOST_HEAD))) {
        obj_score(ch, ch->pcdata->chobj);
        return;
    }
    sprintf(buf,
            "You are %s%s.  You have been playing for %d hours.\n\r",
            ch->name,
            IS_NPC(ch) ? "" : ch->pcdata->title,
            (get_age(ch) - 17) * 2);
    send_to_char(buf, ch);
    if(!IS_NPC(ch)) {
        birth_date(ch, TRUE);
    }
    if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_PREGNANT)) {
        birth_date(ch, FALSE);
    }
    if(get_trust(ch) != ch->level) {
        sprintf(buf, "You are trusted at level %d.\n\r",
                get_trust(ch));
        send_to_char(buf, ch);
    }
    sprintf(buf,
            "You have %d/%d hit, %d/%d mana, %d/%d movement, %d primal energy.\n\r",
            ch->hit,  ch->max_hit,
            ch->mana, ch->max_mana,
            ch->move, ch->max_move,
            ch->practice);
    send_to_char(buf, ch);
    sprintf(buf,
            "You are carrying %d/%d items with weight %d/%d kg.\n\r",
            ch->carry_number, can_carry_n(ch),
            ch->carry_weight, can_carry_w(ch));
    send_to_char(buf, ch);
    sprintf(buf,
            "Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d.\n\r",
            get_curr_str(ch),
            get_curr_int(ch),
            get_curr_wis(ch),
            get_curr_dex(ch),
            get_curr_con(ch));
    send_to_char(buf, ch);
    sprintf(buf,
            "You have scored %d exp, and have %d gold coins.\n\r",
            ch->exp,  ch->gold);
    send_to_char(buf, ch);
    if(!IS_NPC(ch) && IS_DEMON(ch)) {
        sprintf(buf,
                "You have %d out of %d points of %sic power stored.\n\r",
                ch->pcdata->power[0],  ch->pcdata->power[1],
                IS_ANGEL(ch) ? "angel" : "demon");
        send_to_char(buf, ch);
    }
    sprintf(buf,
            "Autoexit: %s.  Autoloot: %s.  Autosac: %s.\n\r",
            (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
            (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
            (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC)) ? "yes" : "no");
    send_to_char(buf, ch);
    sprintf(buf, "Wimpy set to %d hit points.", ch->wimpy);
    if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_DONE)) {
        strcat(buf, "  You are no longer a virgin.\n\r");
    } else {
        strcat(buf, "\n\r");
    }
    send_to_char(buf, ch);
    if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10) {
        send_to_char("You are drunk.\n\r",   ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0) {
        send_to_char("You are thirsty.\n\r", ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   ==  0) {
        send_to_char("You are hungry.\n\r",  ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 100) {
        send_to_char("You are feeling extremely horny.\n\r",  ch);
    } else if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 50) {
        send_to_char("You are feeling pretty randy.\n\r",  ch);
    } else if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 1) {
        send_to_char("You are feeling rather kinky.\n\r",  ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->stage[1] > 0 && (ch->position == POS_SITTING
            || ch->position == POS_STANDING || ch->position == POS_RESTING)) {
        if(ch->pcdata->partner != NULL && ch->pcdata->partner->in_room != NULL
                && ch->pcdata->partner->in_room == ch->in_room) {
            send_to_char("You are having sexual intercourse.\n\r",  ch);
            if(!IS_NPC(ch) && (ch->pcdata->stage[2] + 25) >= ch->pcdata->stage[1]) {
                send_to_char("You are on the verge of having an orgasm.\n\r",  ch);
            }
        }
    } else switch(ch->position) {
        case POS_DEAD:
            send_to_char("You are DEAD!!\n\r",		ch);
            break;
        case POS_MORTAL:
            if(IS_VAMPIRE(ch)) {
                send_to_char("You are in torpor.\n\r", ch);
            } else {
                send_to_char("You are mortally wounded.\n\r", ch);
            }
            break;
        case POS_INCAP:
            send_to_char("You are incapacitated.\n\r",	ch);
            break;
        case POS_STUNNED:
            send_to_char("You are stunned.\n\r",		ch);
            break;
        case POS_SLEEPING:
            send_to_char("You are sleeping.\n\r",		ch);
            break;
        case POS_RESTING:
            send_to_char("You are resting.\n\r",		ch);
            break;
        case POS_MEDITATING:
            send_to_char("You are meditating.\n\r",	ch);
            break;
        case POS_SITTING:
            send_to_char("You are sitting.\n\r",		ch);
            break;
        case POS_STANDING:
            send_to_char("You are standing.\n\r",		ch);
            break;
        case POS_FIGHTING:
            send_to_char("You are fighting.\n\r",		ch);
            break;
        }
    if(ch->level >= 0) {
        sprintf(buf, "AC: %d.  ", a_c);
        send_to_char(buf, ch);
    }
    send_to_char("You are ", ch);
    if(a_c >=  101) {
        send_to_char("naked!\n\r", ch);
    } else if(a_c >=   80) {
        send_to_char("barely clothed.\n\r",   ch);
    } else if(a_c >=   60) {
        send_to_char("wearing clothes.\n\r",  ch);
    } else if(a_c >=   40) {
        send_to_char("slightly armored.\n\r", ch);
    } else if(a_c >=   20) {
        send_to_char("somewhat armored.\n\r", ch);
    } else if(a_c >=    0) {
        send_to_char("armored.\n\r",          ch);
    } else if(a_c >=  -50) {
        send_to_char("well armored.\n\r",     ch);
    } else if(a_c >= -100) {
        send_to_char("strongly armored.\n\r", ch);
    } else if(a_c >= -250) {
        send_to_char("heavily armored.\n\r",  ch);
    } else if(a_c >= -500) {
        send_to_char("superbly armored.\n\r", ch);
    } else if(a_c >= -749) {
        send_to_char("divinely armored.\n\r", ch);
    } else {
        send_to_char("ultimately armored!\n\r", ch);
    }
    sprintf(buf, "Hitroll: %d.  Damroll: %d.  ", char_hitroll(ch), char_damroll(ch));
    send_to_char(buf, ch);
    if(!IS_NPC(ch) && IS_VAMPIRE(ch)) {
        sprintf(buf, "Blood: %d/%d.  Blood potence: %d.\n\r",
                ch->blood[BLOOD_CURRENT],
                ch->blood[BLOOD_POOL],
                ch->blood[BLOOD_POTENCY]);
        send_to_char(buf, ch);
    } else {
        sprintf(buf, "Blood: %d/%d.\n\r",
                ch->blood[BLOOD_CURRENT],
                ch->blood[BLOOD_POOL]);
        send_to_char(buf, ch);
    }
    if(!IS_NPC(ch) && IS_VAMPIRE(ch)) {
        sprintf(buf, "Beast: %d.  ", ch->beast);
        send_to_char(buf, ch);
        if(ch->beast <  0) {
            send_to_char("You are a cheat!\n\r", ch);
        } else if(ch->beast == 0) {
            send_to_char("You have attained Golconda!\n\r", ch);
        } else if(ch->beast <= 5) {
            send_to_char("You have almost reached Golconda!\n\r", ch);
        } else if(ch->beast <= 10) {
            send_to_char("You are nearing Golconda!\n\r", ch);
        } else if(ch->beast <= 15) {
            send_to_char("You have great control over your beast.\n\r", ch);
        } else if(ch->beast <= 20) {
            send_to_char("Your beast has little influence over your actions.\n\r", ch);
        } else if(ch->beast <= 30) {
            send_to_char("You are in control of your beast.\n\r", ch);
        } else if(ch->beast <= 40) {
            send_to_char("You are able to hold back the beast.\n\r", ch);
        } else if(ch->beast <= 60) {
            send_to_char("You are constantly struggling for control of your beast.\n\r", ch);
        } else if(ch->beast <= 75) {
            send_to_char("Your beast has great control over your actions.\n\r", ch);
        } else if(ch->beast <= 90) {
            send_to_char("The power of the beast overwhelms you.\n\r", ch);
        } else if(ch->beast <= 99) {
            send_to_char("You have almost lost your battle with the beast!\n\r", ch);
        } else {
            send_to_char("The beast has taken over!\n\r", ch);
        }
    }
    if(ch->level >= 0) {
        sprintf(buf, "Alignment: %d.  ", ch->alignment);
        send_to_char(buf, ch);
    }
    send_to_char("You are ", ch);
    if(ch->alignment >  900) {
        send_to_char("angelic.\n\r", ch);
    } else if(ch->alignment >  700) {
        send_to_char("saintly.\n\r", ch);
    } else if(ch->alignment >  350) {
        send_to_char("good.\n\r",    ch);
    } else if(ch->alignment >  100) {
        send_to_char("kind.\n\r",    ch);
    } else if(ch->alignment > -100) {
        send_to_char("neutral.\n\r", ch);
    } else if(ch->alignment > -350) {
        send_to_char("mean.\n\r",    ch);
    } else if(ch->alignment > -700) {
        send_to_char("evil.\n\r",    ch);
    } else if(ch->alignment > -900) {
        send_to_char("demonic.\n\r", ch);
    } else {
        send_to_char("satanic.\n\r", ch);
    }
    if(!IS_NPC(ch) && ch->level >= 0) {
        sprintf(buf, "Status: %d.  ", ch->race);
        send_to_char(buf, ch);
    }
    if(!IS_NPC(ch)) {
        send_to_char("You are ", ch);
        if(ch->level == 1) {
            send_to_char("a Mortal.\n\r", ch);
        } else if(ch->level == 2) {
            send_to_char("a Mortal.\n\r", ch);
        } else if(ch->level == 7) {
            send_to_char("a Builder.\n\r", ch);
        } else if(ch->level == 8) {
            send_to_char("a Quest Maker.\n\r", ch);
        } else if(ch->level == 9) {
            send_to_char("an Enforcer.\n\r", ch);
        } else if(ch->level == 10) {
            send_to_char("a Judge.\n\r", ch);
        } else if(ch->level == 11) {
            send_to_char("a High Judge.\n\r", ch);
        } else if(ch->level == 12) {
            send_to_char("an Implementor.\n\r", ch);
        } else if(ch->race <= 0) {
            send_to_char("an Avatar.\n\r", ch);
        } else if(ch->race <= 4) {
            send_to_char("an Immortal.\n\r", ch);
        } else if(ch->race <= 9) {
            send_to_char("a Godling.\n\r", ch);
        } else if(ch->race <= 14) {
            send_to_char("a Demigod.\n\r", ch);
        } else if(ch->race <= 19) {
            send_to_char("a Lesser God.\n\r", ch);
        } else if(ch->race <= 24) {
            send_to_char("a Greater God.\n\r", ch);
        } else if(ch->race >= 25) {
            send_to_char("a Supreme God.\n\r", ch);
        } else {
            send_to_char("a Bugged Character!\n\r", ch);
        }
    }
    if(!IS_NPC(ch)) {
        if(ch->pkill  == 0) {
            sprintf(ss1, "no players");
        } else if(ch->pkill  == 1) {
            sprintf(ss1, "%d player", ch->pkill);
        } else {
            sprintf(ss1, "%d players", ch->pkill);
        }
        if(ch->pdeath == 0) {
            sprintf(ss2, "no players");
        } else if(ch->pdeath == 1) {
            sprintf(ss2, "%d player", ch->pdeath);
        } else {
            sprintf(ss2, "%d players", ch->pdeath);
        }
        sprintf(buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2);
        send_to_char(buf, ch);
        if(ch->mkill  == 0) {
            sprintf(ss1, "no mobs");
        } else if(ch->mkill  == 1) {
            sprintf(ss1, "%d mob", ch->mkill);
        } else {
            sprintf(ss1, "%d mobs", ch->mkill);
        }
        if(ch->mdeath == 0) {
            sprintf(ss2, "no mobs");
        } else if(ch->mdeath == 1) {
            sprintf(ss2, "%d mob", ch->mdeath);
        } else {
            sprintf(ss2, "%d mobs", ch->mdeath);
        }
        sprintf(buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2);
        send_to_char(buf, ch);
    }
    if(!IS_NPC(ch) && ch->pcdata->quest > 0) {
        if(ch->pcdata->quest == 1) {
            sprintf(buf, "You have a single quest point.\n\r");
        } else {
            sprintf(buf, "You have %d quest points.\n\r", ch->pcdata->quest);
        }
        send_to_char(buf, ch);
    }
    if(IS_AFFECTED(ch, AFF_HIDE)) {
        send_to_char("You are keeping yourself hidden from those around you.\n\r", ch);
    }
    if(!IS_NPC(ch)) {
        if(IS_VAMPIRE(ch) && ch->pcdata->wolf > 0) {
            sprintf(buf, "The beast is in control of your actions:  Affects Hitroll and Damroll by +%d.\n\r", ch->pcdata->wolf);
            send_to_char(buf, ch);
        } else if(ch->pcdata->wolf > 0 && (IS_SET(ch->act, PLR_WOLFMAN) ||
                                           ch->pcdata->wolfform[1] > FORM_HOMID)) {
            sprintf(buf, "You are raging:  Affects Hitroll and Damroll by +%d.\n\r", ch->pcdata->wolf);
            send_to_char(buf, ch);
        } else if(IS_DEMON(ch) && ch->pcdata->demonic > 0) {
            int bonus = (ch->pcdata->demonic * ch->pcdata->demonic);
            if(ch->class == CLASS_DEMON || ch->class == CLASS_ANGEL) {
                bonus <<= 1;
            }
            sprintf(buf, "You are wearing %sic armour:  Affects Hitroll and Damroll by +%d.\n\r",
                    IS_ANGEL(ch) ? "angel" : "demon", bonus);
            send_to_char(buf, ch);
        }
    }
    if(!IS_NPC(ch) && ch->pcdata->atm > 0) {
        sprintf(buf, "You have %d percent magic resistance.\n\r", ch->pcdata->atm);
        send_to_char(buf, ch);
    }
    if(ch->affected != NULL) {
        send_to_char("You are affected by:\n\r", ch);
        for(paf = ch->affected; paf != NULL; paf = paf->next) {
            sprintf(buf, "Spell: '%s'", skill_table[paf->type].name);
            send_to_char(buf, ch);
            if(ch->level >= 0) {
                sprintf(buf,
                        " %s %d for %d hours with bits %s.\n\r",
                        affect_loc_name(paf->location),
                        paf->modifier,
                        paf->duration,
                        affect_bit_name(paf->bitvector));
                send_to_char(buf, ch);
            }
        }
    }
    return;
}



char *	const	day_name	[] = {
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] = {
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time(CHAR_DATA *ch, char *argument) {
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;
    day     = time_info.day + 1;
    if(day > 4 && day <  20) {
        suf = "th";
    } else if(day % 10 ==  1) {
        suf = "st";
    } else if(day % 10 ==  2) {
        suf = "nd";
    } else if(day % 10 ==  3) {
        suf = "rd";
    } else {
        suf = "th";
    }
    sprintf(buf,
            "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rGod Wars started up at %s\rThe system time is %s\r",
            (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
            time_info.hour >= 12 ? "pm" : "am",
            day_name[day % 7],
            day, suf,
            month_name[time_info.month],
            str_boot_time,
            (char *) ctime(&current_time)
           );
    send_to_char(buf, ch);
    return;
}



void do_weather(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    static char * const sky_look[4] = {
        "cloudless",
        "cloudy",
        "rainy",
        "lit by flashes of lightning"
    };
    if(!IS_OUTSIDE(ch)) {
        send_to_char("You can't see the weather indoors.\n\r", ch);
        return;
    }
    if(IS_SET(world_affects, WORLD_ECLIPSE)) {
        send_to_char("The sun is eclipsed by the moon.\n\r", ch);
    }
    if(IS_SET(world_affects, WORLD_FOG)) {
        send_to_char("A thick blanket of fog covers the ground.\n\r", ch);
    }
    if(IS_SET(world_affects, WORLD_RAIN)) {
        sprintf(buf, "The sky is pouring with a heavy rain and %s.\n\r",
                weather_info.change >= 0
                ? "a warm southerly breeze blows"
                : "a cold northern gust blows"
               );
        send_to_char(buf, ch);
        return;
    }
    sprintf(buf, "The sky is %s and %s.\n\r",
            sky_look[weather_info.sky],
            weather_info.change >= 0
            ? "a warm southerly breeze blows"
            : "a cold northern gust blows"
           );
    send_to_char(buf, ch);
    return;
}



void do_help(CHAR_DATA *ch, char *argument) {
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;
    if(argument[0] == '\0') {
        argument = "summary";
    }
    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while(argument[0] != '\0') {
        argument = one_argument(argument, argone);
        if(argall[0] != '\0') {
            strcat(argall, " ");
        }
        strcat(argall, argone);
    }
    for(pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
        if(pHelp->level > get_trust(ch)) {
            continue;
        }
        if(is_name(argall, pHelp->keyword)) {
            if(pHelp->level >= 0 && str_cmp(argall, "imotd")) {
                send_to_char(pHelp->keyword, ch);
                send_to_char("\n\r", ch);
            }
            /*
             * Strip leading '.' to allow initial blanks.
             */
            if(pHelp->text[0] == '.') {
                send_to_char(pHelp->text + 1, ch);
            } else {
                send_to_char(pHelp->text  , ch);
            }
            return;
        }
    }
    send_to_char("No help on that word.\n\r", ch);
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char kav[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char openb[20];
    char closeb[20];
    DESCRIPTOR_DATA *d;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int nTotal;
    bool fClassRestrict;
    bool fImmortalOnly;
    bool isName;
    int loop = 0;
    smash_tilde(argument);
    one_argument(argument, arg);
    if(arg[0] != '\0') {
        sprintf(arg2, "|%s*", argument);
        while(arg2[loop++] != '\0') {
            if(arg2[loop] == ' ') {
                arg2[loop] = '*';
            }
        }
    }
    if(IS_NPC(ch)) {
        return;
    }
    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fImmortalOnly  = FALSE;
    isName         = FALSE;
    /*
     * Parse arguments.
     */
    nNumber = 0;
    for(;;) {
        if(arg[0] == '\0') {
            break;
        }
        if(is_number(arg)) {
            send_to_char("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r", ch);
            return;
        } else {
            /*
             * Look for classes to turn on.
             */
            arg[3] = '\0';
            if(!str_cmp(arg, "imm")
                    || !str_cmp(arg, "ava")) {
                fClassRestrict = TRUE;
                break;
            } else if(!str_cmp(arg, "god")
                      || !str_cmp(arg, "imp")) {
                fImmortalOnly = TRUE;
                break;
            } else {
                isName = TRUE;
                break;
            }
        }
    }
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    nTotal = 0;
    buf[0] = '\0';
    for(d = descriptor_list; d != NULL; d = d->next) {
        CHAR_DATA *wch;
        char const *class;
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if(d->connected != CON_PLAYING) {
            continue;
        }
        if(IS_IMMORTAL(d->character) && !can_see(ch, d->character)) {
            continue;
        }
        nTotal++;
        if(!can_see(ch, d->character) && (!IS_SET(ch->act, PLR_WATCHER))) {
            continue;
        }
        wch   = (d->original != NULL) ? d->original : d->character;
        if(wch->level < iLevelLower
                ||   wch->level > iLevelUpper
                || (fImmortalOnly  && wch->level <  LEVEL_IMMORTAL)
                || (fClassRestrict && wch->level != LEVEL_HERO)) {
            continue;
        }
        if(arg2[0] != '\0' && isName && !IS_NPC(wch)) {
            if(!is_in(wch->name, arg2) && !is_in(wch->pcdata->title, arg2)) {
                continue;
            }
        }
        nMatch++;
        /*
         * Figure out what to print for class.
         */
        class = " ";
        if((IS_HEAD(wch, LOST_HEAD) || IS_EXTRA(wch, EXTRA_OSWITCH)) &&
                wch->pcdata->chobj != NULL) {
            switch(wch->pcdata->chobj->pIndexData->vnum) {
            default:
                class = "An Object    ";
                break;
            case 12:
                class = "A Head       ";
                break;
            case 30005:
                class = "A Brain      ";
                break;
            }
        } else switch(wch->level) {
            default:
                break;
            case MAX_LEVEL -  0:
                if(!str_cmp(wch->name, "KaVir")) {
                    class = "The Coder    ";
                } else {
                    class = "Implementor  ";
                }
                break;
            case MAX_LEVEL -  1:
                class = "High Judge   ";
                break;
            case MAX_LEVEL -  2:
                class = "Judge        ";
                break;
            case MAX_LEVEL -  3:
                class = "Enforcer     ";
                break;
            case MAX_LEVEL -  4:
                class = "Quest Maker  ";
                break;
            case MAX_LEVEL -  5:
                class = "Builder      ";
                break;
            case MAX_LEVEL -  6:
            case MAX_LEVEL -  7:
            case MAX_LEVEL -  8:
            case MAX_LEVEL -  9:
                switch(wch->race) {
                default:
                    class = "Supreme God  ";
                    break;
                case 0:
                    class = "Avatar       ";
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                    class = "Immortal     ";
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    class = "Godling      ";
                    break;
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                    class = "Demigod      ";
                    break;
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                    class = "Lesser God   ";
                    break;
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                    class = "Greater God  ";
                    break;
                }
                break;
            case MAX_LEVEL - 10:
            case MAX_LEVEL - 11:
            case MAX_LEVEL - 12:
                class = "Mortal       ";
                break;
            }
        /*
         * Format it up.
         */
        if(IS_ABOMINATION(wch)) {
            strcpy(openb, "<(");
            strcpy(closeb, ")>");
        } else if(IS_LICH(wch)) {
            strcpy(openb, "<{");
            strcpy(closeb, "}>");
        } else if(IS_SKINDANCER(wch)) {
            strcpy(openb, "{(");
            strcpy(closeb, ")}");
        } else if(IS_BAALI(wch)) {
            strcpy(openb, "<[");
            strcpy(closeb, "]>");
        } else if(IS_BLACK_SPIRAL_DANCER(wch)) {
            strcpy(openb, "([");
            strcpy(closeb, "])");
        } else if(IS_NEPHANDI(wch)) {
            strcpy(openb, "{[");
            strcpy(closeb, "]}");
        } else if(IS_VAMPIRE(wch)) {
            strcpy(openb, "<<");
            strcpy(closeb, ">>");
        } else if(IS_GHOUL(wch)) {
            strcpy(openb, "<");
            strcpy(closeb, ">");
        } else if(IS_WEREWOLF(wch)) {
            strcpy(openb, "((");
            strcpy(closeb, "))");
        } else if(IS_MAGE(wch)) {
            strcpy(openb, "{{");
            strcpy(closeb, "}}");
        } else if(IS_HIGHLANDER(wch)) {
            strcpy(openb, "-=");
            strcpy(closeb, "=-");
        } else {
            strcpy(openb, "[[");
            strcpy(closeb, "]]");
        }
        /*
        	if (IS_VAMPIRE(ch) || IS_WEREWOLF(ch) || IS_MAGE(ch) ||
        	    IS_DEMON(ch) || IS_HIGHLANDER(ch) )
        */
        {
            if(IS_ABOMINATION(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sAbomination Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sAbomination%s", openb, closeb);
                }
            } else if(IS_LICH(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sLich Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sLich%s", openb, closeb);
                }
            } else if(IS_SKINDANCER(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sSkindancer Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sSkindancer%s", openb, closeb);
                }
            } else if(IS_BAALI(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sBaali Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sBaali%s", openb, closeb);
                }
            } else if(IS_BLACK_SPIRAL_DANCER(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sBlack Spiral Dancer Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sBlack Spiral Dancer%s", openb, closeb);
                }
            } else if(IS_NEPHANDI(wch)) {
                if(wch->vampgen <= 3) {
                    sprintf(kav, ". %sNephandi Leader%s", openb, closeb);
                } else {
                    sprintf(kav, ". %sNepandi%s", openb, closeb);
                }
            } else if(IS_ANGEL(wch)) {
                if(IS_SET(wch->act, PLR_DEMON)) {
                    sprintf(kav, ". %sArchangel%s", openb, closeb);
                } else if(IS_EXTRA(wch, EXTRA_PRINCE))
                    sprintf(kav, ". %sAngel Prince%s of %s%s", openb,
                            wch->sex == SEX_FEMALE ? "ss" : "", wch->lord, closeb);
                else if(IS_EXTRA(wch, EXTRA_SIRE)) {
                    sprintf(kav, ". %sGreater Angel of %s%s", openb, wch->lord, closeb);
                } else {
                    sprintf(kav, ". %sLesser Angel of %s%s", openb, wch->lord, closeb);
                }
            } else if(IS_DEMON(wch)) {
                if(IS_SET(wch->act, PLR_DEMON)) {
                    sprintf(kav, ". %sDemon Lord%s", openb, closeb);
                } else if(IS_EXTRA(wch, EXTRA_PRINCE))
                    sprintf(kav, ". %sDemon Prince%s of %s%s", openb,
                            wch->sex == SEX_FEMALE ? "ss" : "", wch->lord, closeb);
                else if(IS_EXTRA(wch, EXTRA_SIRE)) {
                    sprintf(kav, ". %sGreater Demon of %s%s", openb, wch->lord, closeb);
                } else {
                    sprintf(kav, ". %sLesser Demon of %s%s", openb, wch->lord, closeb);
                }
            } else if(IS_GHOUL(wch)) {
                sprintf(kav, ". %sGhoul%s", openb, closeb);
            } else if(IS_VAMPIRE(wch) && wch->vampgen == 1) {
                sprintf(kav, ". %sMaster Vampire%s", openb, closeb);
            } else if(IS_WEREWOLF(wch) && wch->vampgen == 1) {
                sprintf(kav, ". %sMaster Werewolf%s", openb, closeb);
            } else if(IS_VAMPIRE(wch)) {
                bool use_age = FALSE;
                char clanname[80];
                if(strlen(wch->clan) < 2) {
                    use_age = TRUE;
                } else if(wch->vampgen == 2) {
                    sprintf(kav, ". %sFounder of %s%s", openb, wch->clan, closeb);
                } else if(IS_EXTRA(wch, EXTRA_PRINCE))
                    sprintf(kav, ". %s%s Prince%s%s", openb, wch->clan,
                            wch->sex == SEX_FEMALE ? "ss" : "", closeb);
                else {
                    use_age = TRUE;
                }
                if(use_age) {
                    if(strlen(wch->clan) < 2) {
                        strcpy(clanname, "Caitiff");
                    } else {
                        strcpy(clanname, wch->clan);
                    }
                    switch(wch->vampgen) {
                    default:
                        sprintf(kav, ". %s%s Childe%s", openb, clanname, closeb);
                        break;
                    case 6:
                        sprintf(kav, ". %s%s Neonate%s", openb, clanname, closeb);
                        break;
                    case 5:
                        sprintf(kav, ". %s%s Ancilla%s", openb, clanname, closeb);
                        break;
                    case 4:
                        sprintf(kav, ". %s%s Elder%s", openb, clanname, closeb);
                        break;
                    case 3:
                        sprintf(kav, ". %s%s Methuselah%s", openb, clanname, closeb);
                        break;
                    case 2:
                    case 1:
                        sprintf(kav, ". %sAntediluvian%s", openb, closeb);
                        break;
                    }
                }
            } else if(IS_WEREWOLF(wch)) {
                if(strlen(wch->clan) > 1) {
                    if(wch->vampgen == 2) {
                        sprintf(kav, ". %s%s Chief%s", openb, wch->clan, closeb);
                    } else if(IS_EXTRA(wch, EXTRA_PRINCE)) {
                        sprintf(kav, ". %s%s Shaman%s", openb, wch->clan, closeb);
                    } else {
                        sprintf(kav, ". %s%s%s", openb, wch->clan, closeb);
                    }
                } else {
                    sprintf(kav, ". %sRonin%s", openb, closeb);
                }
            } else if(IS_MAGE(wch)) {
                char *mage_col;
                switch(wch->pcdata->runes[0]) {
                default:
                case PURPLE_MAGIC:
                    mage_col = "Purple";
                    break;
                case RED_MAGIC:
                    mage_col = "Red";
                    break;
                case BLUE_MAGIC:
                    mage_col = "Blue";
                    break;
                case GREEN_MAGIC:
                    mage_col = "Green";
                    break;
                case YELLOW_MAGIC:
                    mage_col = "Yellow";
                    break;
                }
                switch(wch->level) {
                default:
                case LEVEL_APPRENTICE:
                    sprintf(kav, ". %s%s Apprentice%s", openb, mage_col, closeb);
                    break;
                case LEVEL_MAGE:
                    sprintf(kav, ". %s%s Mage%s", openb, mage_col, closeb);
                    break;
                case LEVEL_ARCHMAGE:
                    sprintf(kav, ". %s%s Archmage%s", openb, mage_col, closeb);
                    break;
                }
            } else if(IS_HIGHLANDER(wch)) {
                sprintf(kav, ". %sHighlander%s", openb, closeb);
            } else {
                strcpy(kav, ".");
            }
        }
        /*
        	else
        	    strcpy( kav, "." );
        */
        sprintf(buf + strlen(buf), "%-13s %s%s%s\n\r",
                class,
                wch->name,
                wch->pcdata->title,
                kav);
    }
    if(nMatch == 1) {
        sprintf(buf2, "You are the only visible player connected!\n\r");
    } else {
        sprintf(buf2, "There are a total of %d visible players connected.\n\r", nMatch);
    }
    send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    send_to_char(buf, ch);
    send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    send_to_char(buf2, ch);
    if(nTotal > nMatch && !isName) {
        if((nTotal - nMatch) < 2) {
            sprintf(buf2, "There is one invisible player connected.\n\r");
        } else {
            sprintf(buf2, "There are a total of %d invisible players connected.\n\r", (nTotal - nMatch));
        }
        send_to_char(buf2, ch);
    }
    send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    return;
}



void do_inventory(CHAR_DATA *ch, char *argument) {
    char buf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    bool found;
    if(!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD)) {
        send_to_char("You are not a container.\n\r", ch);
        return;
    } else if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH)) {
        if(!IS_NPC(ch) && (obj = ch->pcdata->chobj) == NULL) {
            send_to_char("You are not a container.\n\r", ch);
            return;
        }
        switch(obj->item_type) {
        default:
            send_to_char("You are not a container.\n\r", ch);
            break;
        case ITEM_PORTAL:
            pRoomIndex = get_room_index(obj->value[0]);
            location = ch->in_room;
            if(pRoomIndex == NULL) {
                send_to_char("You don't seem to lead anywhere.\n\r", ch);
                return;
            }
            char_from_room(ch);
            char_to_room(ch, pRoomIndex);
            found = FALSE;
            for(portal = ch->in_room->contents; portal != NULL; portal = portal_next) {
                portal_next = portal->next_content;
                if((obj->value[0] == portal->value[3])
                        && (obj->value[3] == portal->value[0])) {
                    found = TRUE;
                    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                            !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE)) {
                        REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        do_look(ch, "auto");
                        SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        break;
                    } else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                              IS_SET(portal->extra_flags, ITEM_SHADOWPLANE)) {
                        SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        do_look(ch, "auto");
                        REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
                        break;
                    } else {
                        do_look(ch, "auto");
                        break;
                    }
                }
            }
            char_from_room(ch);
            char_to_room(ch, location);
            break;
        case ITEM_DRINK_CON:
            if(obj->value[1] <= 0) {
                send_to_char("You are empty.\n\r", ch);
                break;
            }
            if(obj->value[1] < obj->value[0] / 5) {
                sprintf(buf, "There is a little %s liquid left in you.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 4) {
                sprintf(buf, "You contain a small about of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 3) {
                sprintf(buf, "You're about a third full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0] / 2) {
                sprintf(buf, "You're about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] < obj->value[0]) {
                sprintf(buf, "You are almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else if(obj->value[1] == obj->value[0]) {
                sprintf(buf, "You're completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            } else {
                sprintf(buf, "Somehow you are MORE than full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
            }
            send_to_char(buf, ch);
            break;
        case ITEM_CONTAINER:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
            act("$p contain:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE);
            break;
        }
        return;
    }
    send_to_char("You are carrying:\n\r", ch);
    show_list_to_char(ch->carrying, ch, TRUE, TRUE);
    return;
}



void do_equipment(CHAR_DATA *ch, char *argument) {
    OBJ_DATA *obj;
    int iWear;
    bool found;
    send_to_char("You are using:\n\r", ch);
    found = FALSE;
    for(iWear = 0; iWear < MAX_WEAR; iWear++) {
        if((obj = get_eq_char(ch, iWear)) == NULL) {
            continue;
        }
        send_to_char(where_name[iWear], ch);
        if(can_see_obj(ch, obj)) {
            send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
            send_to_char("\n\r", ch);
        } else {
            send_to_char("something.\n\r", ch);
        }
        found = TRUE;
    }
    if(!found) {
        send_to_char("Nothing.\n\r", ch);
    }
    return;
}



void do_compare(CHAR_DATA *ch, char *argument) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(arg1[0] == '\0') {
        send_to_char("Compare what to what?\n\r", ch);
        return;
    }
    if((obj1 = get_obj_carry(ch, arg1)) == NULL) {
        send_to_char("You do not have that item.\n\r", ch);
        return;
    }
    if(arg2[0] == '\0') {
        for(obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content) {
            if(obj2->wear_loc != WEAR_NONE
                    &&   can_see_obj(ch, obj2)
                    &&   obj1->item_type == obj2->item_type
                    && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0) {
                break;
            }
        }
        if(obj2 == NULL) {
            send_to_char("You aren't wearing anything comparable.\n\r", ch);
            return;
        }
    } else {
        if((obj2 = get_obj_carry(ch, arg2)) == NULL) {
            send_to_char("You do not have that item.\n\r", ch);
            return;
        }
    }
    msg		= NULL;
    value1	= 0;
    value2	= 0;
    if(obj1 == obj2) {
        msg = "You compare $p to itself.  It looks about the same.";
    } else if(obj1->item_type != obj2->item_type) {
        msg = "You can't compare $p and $P.";
    } else {
        switch(obj1->item_type) {
        default:
            msg = "You can't compare $p and $P.";
            break;
        case ITEM_ARMOR:
            value1 = obj1->value[0];
            value2 = obj2->value[0];
            break;
        case ITEM_WEAPON:
            value1 = obj1->value[1] + obj1->value[2];
            value2 = obj2->value[1] + obj2->value[2];
            break;
        }
    }
    if(msg == NULL) {
        if(value1 == value2) {
            msg = "$p and $P look about the same.";
        } else if(value1  > value2) {
            msg = "$p looks better than $P.";
        } else {
            msg = "$p looks worse than $P.";
        }
    }
    act(msg, ch, obj1, obj2, TO_CHAR);
    return;
}



void do_credits(CHAR_DATA *ch, char *argument) {
    do_help(ch, "diku");
    return;
}



void do_wizlist(CHAR_DATA *ch, char *argument) {
    do_help(ch, "wizlist");
    return;
}



void do_where(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;
    one_argument(argument, arg);
    if(IS_SET(world_affects, WORLD_FOG)) {
        send_to_char("It is too foggy to locate anyone.\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Players near you:\n\r", ch);
        found = FALSE;
        for(d = descriptor_list; d != NULL; d = d->next) {
            if(d->connected == CON_PLAYING
                    && (victim = d->character) != NULL
                    &&   !IS_NPC(victim)
                    &&   victim->in_room != NULL
                    &&   victim->in_room->area == ch->in_room->area
                    &&   victim->pcdata->chobj == NULL
                    &&   can_see(ch, victim)) {
                found = TRUE;
                sprintf(buf, "%-28s %s\n\r",
                        victim->name, victim->in_room->name);
                send_to_char(buf, ch);
            }
        }
        if(!found) {
            send_to_char("None\n\r", ch);
        }
    } else {
        found = FALSE;
        for(victim = char_list; victim != NULL; victim = victim->next) {
            if(victim->in_room != NULL
                    &&   victim->in_room->area == ch->in_room->area
                    &&   !IS_AFFECTED(victim, AFF_HIDE)
                    &&   !IS_AFFECTED(victim, AFF_SNEAK)
                    &&   can_see(ch, victim)
                    &&   is_name(arg, victim->name)) {
                found = TRUE;
                sprintf(buf, "%-28s %s\n\r",
                        PERS(victim, ch), victim->in_room->name);
                send_to_char(buf, ch);
                break;
            }
        }
        if(!found) {
            act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
        }
    }
    return;
}




void do_consider(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;
    int overall;
    int con_hit;
    int con_dam;
    int con_ac;
    int con_hp;
    one_argument(argument, arg);
    overall = 0;
    if(arg[0] == '\0') {
        send_to_char("Consider killing whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They're not here.\n\r", ch);
        return;
    }
    act("You examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_CHAR);
    act("$n examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_NOTVICT);
    act("$n examines you closely, looking for your weaknesses.", ch, NULL, victim, TO_VICT);
    do_skill(ch, victim->name);
    if(!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_FAKE_CON)) {
        con_hit = victim->pcdata->fake_hit;
        con_dam = victim->pcdata->fake_dam;
        con_ac = victim->pcdata->fake_ac;
        con_hp = victim->pcdata->fake_hp;
    } else {
        con_hit = char_hitroll(victim);
        con_dam = char_damroll(victim);
        con_ac = char_ac(victim);
        con_hp = victim->hit;
    }
    if(con_hp < 1) {
        con_hp = 1;
    }
    diff = victim->level - ch->level + con_hit - char_hitroll(ch);
    if(diff <= -35) {
        msg = "You are FAR more skilled than $M.";
        overall = overall + 3;
    } else if(diff <= -15) {
        msg = "$E is not as skilled as you are.";
        overall = overall + 2;
    } else if(diff <=  -5) {
        msg = "$E doesn't seem quite as skilled as you.";
        overall = overall + 1;
    } else if(diff <=   5) {
        msg = "You are about as skilled as $M.";
    } else if(diff <=  15) {
        msg = "$E is slightly more skilled than you are.";
        overall = overall - 1;
    } else if(diff <=  35) {
        msg = "$E seems more skilled than you are.";
        overall = overall - 2;
    } else                    {
        msg = "$E is FAR more skilled than you.";
        overall = overall - 3;
    }
    act(msg, ch, NULL, victim, TO_CHAR);
    diff = victim->level - ch->level + con_dam - char_damroll(ch);
    if(diff <= -35) {
        msg = "You are FAR more powerful than $M.";
        overall = overall + 3;
    } else if(diff <= -15) {
        msg = "$E is not as powerful as you are.";
        overall = overall + 2;
    } else if(diff <=  -5) {
        msg = "$E doesn't seem quite as powerful as you.";
        overall = overall + 1;
    } else if(diff <=   5) {
        msg = "You are about as powerful as $M.";
    } else if(diff <=  15) {
        msg = "$E is slightly more powerful than you are.";
        overall = overall - 1;
    } else if(diff <=  35) {
        msg = "$E seems more powerful than you are.";
        overall = overall - 2;
    } else                    {
        msg = "$E is FAR more powerful than you.";
        overall = overall - 3;
    }
    act(msg, ch, NULL, victim, TO_CHAR);
    diff = ch->hit * 100 / con_hp;
    if(diff <=  10) {
        msg = "$E is currently FAR healthier than you are.";
        overall = overall - 3;
    } else if(diff <=  50) {
        msg = "$E is currently much healthier than you are.";
        overall = overall - 2;
    } else if(diff <=  75) {
        msg = "$E is currently slightly healthier than you are.";
        overall = overall - 1;
    } else if(diff <= 125) {
        msg = "$E is currently about as healthy as you are.";
    } else if(diff <= 200) {
        msg = "You are currently slightly healthier than $M.";
        overall = overall + 1;
    } else if(diff <= 500) {
        msg = "You are currently much healthier than $M.";
        overall = overall + 2;
    } else                    {
        msg = "You are currently FAR healthier than $M.";
        overall = overall + 3;
    }
    act(msg, ch, NULL, victim, TO_CHAR);
    diff = con_ac - char_ac(ch);
    if(diff <= -100) {
        msg = "$E is FAR better armoured than you.";
        overall = overall - 3;
    } else if(diff <= -50) {
        msg = "$E looks much better armoured than you.";
        overall = overall - 2;
    } else if(diff <= -25) {
        msg = "$E looks better armoured than you.";
        overall = overall - 1;
    } else if(diff <=  25) {
        msg = "$E seems about as well armoured as you.";
    } else if(diff <=  50) {
        msg = "You are better armoured than $M.";
        overall = overall + 1;
    } else if(diff <=  100) {
        msg = "You are much better armoured than $M.";
        overall = overall + 2;
    } else                    {
        msg = "You are FAR better armoured than $M.";
        overall = overall + 3;
    }
    act(msg, ch, NULL, victim, TO_CHAR);
    diff = overall;
    if(diff <= -11) {
        msg = "Conclusion: $E would kill you in seconds.";
    } else if(diff <=  -7) {
        msg = "Conclusion: You would need a lot of luck to beat $M.";
    } else if(diff <=  -3) {
        msg = "Conclusion: You would need some luck to beat $N.";
    } else if(diff <=   2) {
        msg = "Conclusion: It would be a very close fight.";
    } else if(diff <=   6) {
        msg = "Conclusion: You shouldn't have a lot of trouble defeating $M.";
    } else if(diff <=  10) {
        msg = "Conclusion: $N is no match for you.  You can easily beat $M.";
    } else {
        msg = "Conclusion: $E wouldn't last more than a few seconds against you.";
    }
    act(msg, ch, NULL, victim, TO_CHAR);
    return;
}



void set_title(CHAR_DATA *ch, char *title) {
    char buf[MAX_STRING_LENGTH];
    if(IS_NPC(ch)) {
        bug("Set_title: NPC.", 0);
        return;
    }
    if(isalpha(title[0]) || isdigit(title[0])) {
        buf[0] = ' ';
        strcpy(buf + 1, title);
    } else {
        strcpy(buf, title);
    }
    free_string(ch->pcdata->title);
    ch->pcdata->title = str_dup(buf);
    return;
}



void do_title(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(argument[0] == '\0') {
        send_to_char("Change your title to what?\n\r", ch);
        return;
    }
    if(strlen(argument) > 30) {
        argument[30] = '\0';
    }
    smash_tilde(argument);
    set_title(ch, argument);
    send_to_char("Ok.\n\r", ch);
}



void do_email(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(argument[0] == '\0') {
        if(IS_EXTRA(ch, EXTRA_INVIS_EMAIL)) {
            send_to_char("Email invis OFF (ie it now shows via finger).\n\r", ch);
            REMOVE_BIT(ch->extra, EXTRA_INVIS_EMAIL);
        } else {
            send_to_char("Email invis ON (ie it no longer show via finger).\n\r", ch);
            SET_BIT(ch->extra, EXTRA_INVIS_EMAIL);
        }
        do_autosave(ch, "");
        return;
    }
    if(strlen(argument) > 50) {
        argument[50] = '\0';
    }
    smash_tilde(argument);
    free_string(ch->pcdata->email);
    ch->pcdata->email = str_dup(argument);
    send_to_char("Ok.\n\r", ch);
}



void do_description(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    if(argument[0] != '\0') {
        buf[0] = '\0';
        smash_tilde(argument);
        if(argument[0] == '+') {
            if(ch->description != NULL) {
                strcat(buf, ch->description);
            }
            argument++;
            while(isspace(*argument)) {
                argument++;
            }
        }
        if(strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 2) {
            send_to_char("Description too long.\n\r", ch);
            return;
        }
        strcat(buf, argument);
        strcat(buf, "\n\r");
        free_string(ch->description);
        ch->description = str_dup(buf);
    }
    send_to_char("Your description is:\n\r", ch);
    send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
    return;
}



void do_report(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    char buf[MAX_STRING_LENGTH];
    char hit_str[MAX_INPUT_LENGTH];
    char mana_str[MAX_INPUT_LENGTH];
    char move_str[MAX_INPUT_LENGTH];
    char mhit_str[MAX_INPUT_LENGTH];
    char mmana_str[MAX_INPUT_LENGTH];
    char mmove_str[MAX_INPUT_LENGTH];
    char exp_str[MAX_INPUT_LENGTH];
    sprintf(hit_str, "%d", ch->hit);
    COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
    sprintf(mana_str, "%d", ch->mana);
    COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
    sprintf(move_str, "%d", ch->move);
    COL_SCALE(move_str, ch, ch->move, ch->max_move);
    sprintf(exp_str, "%d", ch->exp);
    COL_SCALE(exp_str, ch, ch->exp, 1000);
    sprintf(mhit_str, "%d", ch->max_hit);
    ADD_COLOUR(ch, mhit_str, L_CYAN);
    sprintf(mmana_str, "%d", ch->max_mana);
    ADD_COLOUR(ch, mmana_str, L_CYAN);
    sprintf(mmove_str, "%d", ch->max_move);
    ADD_COLOUR(ch, mmove_str, L_CYAN);
    sprintf(buf,
            "You report: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
            hit_str,  mhit_str,
            mana_str, mmana_str,
            move_str, mmove_str,
            exp_str);
    send_to_char(buf, ch);
    for(vch = char_list; vch != NULL; vch = vch_next) {
        vch_next	= vch->next;
        if(vch == NULL) {
            continue;
        }
        if(vch == ch) {
            continue;
        }
        if(vch->in_room == NULL) {
            continue;
        }
        if(vch->in_room != ch->in_room) {
            continue;
        }
        sprintf(hit_str, "%d", ch->hit);
        COL_SCALE(hit_str, vch, ch->hit, ch->max_hit);
        sprintf(mana_str, "%d", ch->mana);
        COL_SCALE(mana_str, vch, ch->mana, ch->max_mana);
        sprintf(move_str, "%d", ch->move);
        COL_SCALE(move_str, vch, ch->move, ch->max_move);
        sprintf(exp_str, "%d", ch->exp);
        COL_SCALE(exp_str, vch, ch->exp, 1000);
        sprintf(mhit_str, "%d", ch->max_hit);
        ADD_COLOUR(vch, mhit_str, L_CYAN);
        sprintf(mmana_str, "%d", ch->max_mana);
        ADD_COLOUR(vch, mmana_str, L_CYAN);
        sprintf(mmove_str, "%d", ch->max_move);
        ADD_COLOUR(vch, mmove_str, L_CYAN);
        if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
            sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
                    ch->morph,
                    hit_str,  mhit_str,
                    mana_str, mmana_str,
                    move_str, mmove_str,
                    exp_str);
        else
            sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
                    IS_NPC(ch) ? capitalize(ch->short_descr) : ch->name,
                    hit_str,  mhit_str,
                    mana_str, mmana_str,
                    move_str, mmove_str,
                    exp_str);
        buf[0] = UPPER(buf[0]);
        send_to_char(buf, vch);
    }
    return;
}



void do_practice(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int sn;
    if(IS_NPC(ch)) {
        return;
    }
    if(argument[0] == '\0') {
        int col;
        col    = 0;
        for(sn = 0; sn < MAX_SKILL; sn++) {
            if(skill_table[sn].name == NULL) {
                break;
            }
            if(ch->level < skill_table[sn].skill_level) {
                continue;
            }
            sprintf(buf, "%18s %3d%%  ",
                    skill_table[sn].name, ch->pcdata->learned[sn]);
            send_to_char(buf, ch);
            if(++col % 3 == 0) {
                send_to_char("\n\r", ch);
            }
        }
        if(col % 3 != 0) {
            send_to_char("\n\r", ch);
        }
        sprintf(buf, "You have %d exp left.\n\r", ch->exp);
        send_to_char(buf, ch);
    } else {
        if(!IS_AWAKE(ch)) {
            send_to_char("In your dreams, or what?\n\r", ch);
            return;
        }
        if(ch->exp <= 0) {
            send_to_char("You have no exp left.\n\r", ch);
            return;
        }
        if((sn = skill_lookup(argument)) < 0
                || (!IS_NPC(ch)
                    &&   ch->level < skill_table[sn].skill_level)) {
            send_to_char("You can't practice that.\n\r", ch);
            return;
        }
        if(ch->pcdata->learned[sn] >= SKILL_ADEPT) {
            sprintf(buf, "You are already an adept of %s.\n\r",
                    skill_table[sn].name);
            send_to_char(buf, ch);
        } else if(ch->pcdata->learned[sn] > 0 &&
                  (ch->pcdata->learned[sn] * 2) > ch->exp) {
            sprintf(buf, "You need %d exp to increase %s any more.\n\r",
                    (ch->pcdata->learned[sn] * 2), skill_table[sn].name);
            send_to_char(buf, ch);
        } else if(ch->pcdata->learned[sn] == 0 && ch->exp < 500) {
            sprintf(buf, "You need 500 exp to increase %s.\n\r",
                    skill_table[sn].name);
            send_to_char(buf, ch);
        } else {
            if(ch->pcdata->learned[sn] == 0) {
                ch->exp -= 500;
                ch->pcdata->learned[sn] += get_curr_int(ch);
            } else {
                ch->exp -= (ch->pcdata->learned[sn] * 2);
                ch->pcdata->learned[sn] += get_curr_int(ch);
            }
            if(ch->pcdata->learned[sn] < SKILL_ADEPT) {
                act("You practice $T.",
                    ch, NULL, skill_table[sn].name, TO_CHAR);
            } else {
                ch->pcdata->learned[sn] = SKILL_ADEPT;
                act("You are now an adept of $T.",
                    ch, NULL, skill_table[sn].name, TO_CHAR);
            }
        }
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;
    one_argument(argument, arg);
    if(arg[0] == '\0') {
        wimpy = ch->max_hit / 5;
    } else {
        wimpy = atoi(arg);
    }
    if(wimpy < 0) {
        send_to_char("Your courage exceeds your wisdom.\n\r", ch);
        return;
    }
    if(wimpy > ch->max_hit) {
        send_to_char("Such cowardice ill becomes you.\n\r", ch);
        return;
    }
    ch->wimpy	= wimpy;
    sprintf(buf, "Wimpy set to %d hit points.\n\r", wimpy);
    send_to_char(buf, ch);
    return;
}



void do_socials(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
    col = 0;
    for(iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++) {
        sprintf(buf, "%-12s", social_table[iSocial].name);
        send_to_char(buf, ch);
        if(++col % 6 == 0) {
            send_to_char("\n\r", ch);
        }
    }
    if(col % 6 != 0) {
        send_to_char("\n\r", ch);
    }
    return;
}



void do_nsocials(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
    col = 0;
    for(iSocial = 0; nsocial_table[iSocial].name[0] != '\0'; iSocial++) {
        sprintf(buf, "%-12s", nsocial_table[iSocial].name);
        send_to_char(buf, ch);
        if(++col % 6 == 0) {
            send_to_char("\n\r", ch);
        }
    }
    if(col % 6 != 0) {
        send_to_char("\n\r", ch);
    }
    return;
}

void do_xsocials(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
    col = 0;
    for(iSocial = 0; xsocial_table[iSocial].name[0] != '\0'; iSocial++) {
        sprintf(buf, "%-12s", xsocial_table[iSocial].name);
        send_to_char(buf, ch);
        if(++col % 6 == 0) {
            send_to_char("\n\r", ch);
        }
    }
    if(col % 6 != 0) {
        send_to_char("\n\r", ch);
    }
    return;
}



void do_spells(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int sn;
    int col;
    col = 0;
    for(sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++) {
        sprintf(buf, "%-12s", skill_table[sn].name);
        send_to_char(buf, ch);
        if(++col % 6 == 0) {
            send_to_char("\n\r", ch);
        }
    }
    if(col % 6 != 0) {
        send_to_char("\n\r", ch);
    }
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    col = 0;
    for(cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {
        if(cmd_table[cmd].level == 0
                &&   cmd_table[cmd].level <= get_trust(ch)) {
            sprintf(buf, "%-12s", cmd_table[cmd].name);
            send_to_char(buf, ch);
            if(++col % 6 == 0) {
                send_to_char("\n\r", ch);
            }
        }
    }
    if(col % 6 != 0) {
        send_to_char("\n\r", ch);
    }
    return;
}



void do_channels(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(arg[0] == '\0') {
        if(!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE)) {
            send_to_char("You are silenced.\n\r", ch);
            return;
        }
        send_to_char("Channels:", ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_AUCTION)
                     ? " +AUCTION"
                     : " -auction",
                     ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_CHAT)
                     ? " +CHAT"
                     : " -chat",
                     ch);
#if 0
        send_to_char(!IS_SET(ch->deaf, CHANNEL_HACKER)
                     ? " +HACKER"
                     : " -hacker",
                     ch);
#endif
        if(get_trust(ch) > LEVEL_BUILDER) {
            send_to_char(!IS_SET(ch->deaf, CHANNEL_IMMTALK)
                         ? " +IMMTALK"
                         : " -immtalk",
                         ch);
        }
        if(IS_IMMORTAL(ch)) {
            send_to_char(!IS_SET(ch->deaf, CHANNEL_BUILD)
                         ? " +BUILDTALK"
                         : " -buildtalk",
                         ch);
        }
        send_to_char(!IS_SET(ch->deaf, CHANNEL_MUSIC)
                     ? " +MUSIC"
                     : " -music",
                     ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_QUESTION)
                     ? " +QUESTION"
                     : " -question",
                     ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_SHOUT)
                     ? " +SHOUT"
                     : " -shout",
                     ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_HOWL)
                     ? " +HOWL"
                     : " -howl",
                     ch);
        if(IS_IMMORTAL(ch)) {
            send_to_char(!IS_SET(ch->deaf, CHANNEL_LOG)
                         ? " +LOG"
                         : " -log",
                         ch);
        }
        if(IS_SET(ch->act, PLR_CHAMPION) || IS_SET(ch->act, PLR_DEMON)
                || IS_IMMORTAL(ch)) {
            send_to_char(!IS_SET(ch->deaf, CHANNEL_PRAY)
                         ? " +PRAY"
                         : " -pray",
                         ch);
        }
        send_to_char(!IS_SET(ch->deaf, CHANNEL_INFO)
                     ? " +INFO"
                     : " -info",
                     ch);
        if(IS_VAMPIRE(ch)) {
            send_to_char(!IS_SET(ch->deaf, CHANNEL_VAMPTALK)
                         ? " +VAMP"
                         : " -vamp",
                         ch);
        }
        send_to_char(!IS_SET(ch->deaf, CHANNEL_TELL)
                     ? " +TELL"
                     : " -tell",
                     ch);
        send_to_char(!IS_SET(ch->deaf, CHANNEL_NEWBIE)
                     ? " +NEWBIE"
                     : " -newbie",
                     ch);
        send_to_char(".\n\r", ch);
    } else {
        bool fClear;
        int bit;
        if(arg[0] == '+') {
            fClear = TRUE;
        } else if(arg[0] == '-') {
            fClear = FALSE;
        } else {
            send_to_char("Channels -channel or +channel?\n\r", ch);
            return;
        }
        if(!str_cmp(arg + 1, "auction")) {
            bit = CHANNEL_AUCTION;
        } else if(!str_cmp(arg + 1, "chat")) {
            bit = CHANNEL_CHAT;
        }
#if 0
        else if(!str_cmp(arg + 1, "hacker")) {
            bit = CHANNEL_HACKER;
        }
#endif
        else if(!str_cmp(arg + 1, "immtalk")) {
            bit = CHANNEL_IMMTALK;
        } else if(!str_cmp(arg + 1, "buildtalk")) {
            bit = CHANNEL_BUILD;
        } else if(!str_cmp(arg + 1, "music")) {
            bit = CHANNEL_MUSIC;
        } else if(!str_cmp(arg + 1, "question")) {
            bit = CHANNEL_QUESTION;
        } else if(!str_cmp(arg + 1, "newbie")) {
            bit = CHANNEL_NEWBIE;
        } else if(!str_cmp(arg + 1, "shout")) {
            bit = CHANNEL_SHOUT;
        } else if(!str_cmp(arg + 1, "yell")) {
            bit = CHANNEL_YELL;
        } else if(!str_cmp(arg + 1, "howl")) {
            bit = CHANNEL_HOWL;
        } else if(IS_IMMORTAL(ch) && !str_cmp(arg + 1, "log")) {
            bit = CHANNEL_LOG;
        } else if((IS_SET(ch->act, PLR_CHAMPION) ||
                   IS_SET(ch->act, PLR_DEMON) || IS_IMMORTAL(ch)) &&
                  !str_cmp(arg + 1, "pray")) {
            bit = CHANNEL_PRAY;
        } else if(!str_cmp(arg + 1, "info")) {
            bit = CHANNEL_INFO;
        } else if(!str_cmp(arg + 1, "vamp")) {
            bit = CHANNEL_VAMPTALK;
        } else if(!str_cmp(arg + 1, "tell")) {
            bit = CHANNEL_TELL;
        } else {
            send_to_char("Set or clear which channel?\n\r", ch);
            return;
        }
        if(fClear) {
            REMOVE_BIT(ch->deaf, bit);
        } else {
            SET_BIT(ch->deaf, bit);
        }
        send_to_char("Ok.\n\r", ch);
    }
    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    if(IS_NPC(ch)) {
        return;
    }
    one_argument(argument, arg);
    if(arg[0] == '\0') {
        send_to_char("[ Keyword  ] Option\n\r", ch);
        send_to_char(IS_SET(ch->act, PLR_ANSI)
                     ? "[+ANSI     ] You have ansi colour on.\n\r"
                     : "[-ansi     ] You have ansi colour off.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_AUTOEXIT)
                     ? "[+AUTOEXIT ] You automatically see exits.\n\r"
                     : "[-autoexit ] You don't automatically see exits.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_AUTOLOOT)
                     ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
                     : "[-autoloot ] You don't automatically loot corpses.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_AUTOSAC)
                     ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
                     : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_BLANK)
                     ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
                     : "[-blank    ] You have no blank line before your prompt.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_BRIEF)
                     ? "[+BRIEF    ] You see brief descriptions.\n\r"
                     : "[-brief    ] You see long descriptions.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_COMBINE)
                     ? "[+COMBINE  ] You see object lists in combined format.\n\r"
                     : "[-combine  ] You see object lists in single format.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_PROMPT)
                     ? "[+PROMPT   ] You have a prompt.\n\r"
                     : "[-prompt   ] You don't have a prompt.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_TELNET_GA)
                     ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
                     : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
                     , ch);
        send_to_char(IS_SET(ch->act, PLR_SILENCE)
                     ? "[+SILENCE  ] You are silenced.\n\r"
                     : ""
                     , ch);
        send_to_char(!IS_SET(ch->act, PLR_NO_EMOTE)
                     ? ""
                     : "[-emote    ] You can't emote.\n\r"
                     , ch);
        send_to_char(!IS_SET(ch->act, PLR_NO_TELL)
                     ? ""
                     : "[-tell     ] You can't use 'tell'.\n\r"
                     , ch);
    } else {
        bool fSet;
        int bit;
        if(arg[0] == '+') {
            fSet = TRUE;
        } else if(arg[0] == '-') {
            fSet = FALSE;
        } else {
            send_to_char("Config -option or +option?\n\r", ch);
            return;
        }
        if(!str_cmp(arg + 1, "ansi")) {
            bit = PLR_ANSI;
        } else if(!str_cmp(arg + 1, "autoexit")) {
            bit = PLR_AUTOEXIT;
        } else if(!str_cmp(arg + 1, "autoloot")) {
            bit = PLR_AUTOLOOT;
        } else if(!str_cmp(arg + 1, "autosac")) {
            bit = PLR_AUTOSAC;
        } else if(!str_cmp(arg + 1, "blank")) {
            bit = PLR_BLANK;
        } else if(!str_cmp(arg + 1, "brief")) {
            bit = PLR_BRIEF;
        } else if(!str_cmp(arg + 1, "combine")) {
            bit = PLR_COMBINE;
        } else if(!str_cmp(arg + 1, "prompt")) {
            bit = PLR_PROMPT;
        } else if(!str_cmp(arg + 1, "telnetga")) {
            bit = PLR_TELNET_GA;
        } else {
            send_to_char("Config which option?\n\r", ch);
            return;
        }
        if(fSet) {
            SET_BIT(ch->act, bit);
        } else {
            REMOVE_BIT(ch->act, bit);
        }
        send_to_char("Ok.\n\r", ch);
    }
    return;
}

void do_ansi(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_ANSI)) {
        do_config(ch, "-ansi");
    } else {
        do_config(ch, "+ansi");
    }
    return;
}

void do_autoexit(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_AUTOEXIT)) {
        do_config(ch, "-autoexit");
    } else {
        do_config(ch, "+autoexit");
    }
    return;
}

void do_autoloot(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_AUTOLOOT)) {
        do_config(ch, "-autoloot");
    } else {
        do_config(ch, "+autoloot");
    }
    return;
}

void do_autosac(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_AUTOSAC)) {
        do_config(ch, "-autosac");
    } else {
        do_config(ch, "+autosac");
    }
    return;
}

void do_blank(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_BLANK)) {
        do_config(ch, "-blank");
    } else {
        do_config(ch, "+blank");
    }
    return;
}

void do_brief(CHAR_DATA *ch, char *argument) {
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_SET(ch->act, PLR_BRIEF)) {
        do_config(ch, "-brief");
    } else {
        do_config(ch, "+brief");
    }
    return;
}

void do_diagnose(CHAR_DATA *ch, char *argument) {
    char buf  [MAX_STRING_LENGTH];
    char arg  [MAX_INPUT_LENGTH];
    int teeth = 0;
    int ribs = 0;
    CHAR_DATA *victim;
    argument = one_argument(argument, arg);
    if(arg == '\0') {
        send_to_char("Who do you wish to diagnose?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("Nobody here by that name.\n\r", ch);
        return;
    }
    if(IS_VAMPIRE(victim)) {
        victim->loc_hp[6] = 0;
    }
    act("$n examines $N carefully, diagnosing $S injuries.", ch, NULL, victim, TO_NOTVICT);
    act("$n examines you carefully, diagnosing your injuries.", ch, NULL, victim, TO_VICT);
    act("Your diagnoses of $N reveals the following...", ch, NULL, victim, TO_CHAR);
    send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    if((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
            victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] +
            victim->loc_hp[6]) == 0) {
        act("$N has no apparent injuries.", ch, NULL, victim, TO_CHAR);
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        return;
    }
    /* Check head */
    if(IS_HEAD(victim, LOST_EYE_L) && IS_HEAD(victim, LOST_EYE_R)) {
        act("$N has lost both of $S eyes.", ch, NULL, victim, TO_CHAR);
    } else if(IS_HEAD(victim, LOST_EYE_L)) {
        act("$N has lost $S left eye.", ch, NULL, victim, TO_CHAR);
    } else if(IS_HEAD(victim, LOST_EYE_R)) {
        act("$N has lost $S right eye.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_EAR_L) && IS_HEAD(victim, LOST_EAR_R)) {
        act("$N has lost both of $S ears.", ch, NULL, victim, TO_CHAR);
    } else if(IS_HEAD(victim, LOST_EAR_L)) {
        act("$N has lost $S left ear.", ch, NULL, victim, TO_CHAR);
    } else if(IS_HEAD(victim, LOST_EAR_R)) {
        act("$N has lost $S right ear.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_NOSE)) {
        act("$N has lost $S nose.", ch, NULL, victim, TO_CHAR);
    } else if(IS_HEAD(victim, BROKEN_NOSE)) {
        act("$N has got a broken nose.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, BROKEN_JAW)) {
        act("$N has got a broken jaw.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_HEAD)) {
        act("$N has had $S head cut off.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_HEAD)) {
            act("...Blood is spurting from the stump of $S neck.", ch, NULL, victim, TO_CHAR);
        }
    } else {
        if(IS_BODY(victim, BROKEN_NECK)) {
            act("$N has got a broken neck.", ch, NULL, victim, TO_CHAR);
        }
        if(IS_BODY(victim, CUT_THROAT)) {
            act("$N has had $S throat cut open.", ch, NULL, victim, TO_CHAR);
            if(IS_BLEEDING(victim, BLEEDING_THROAT)) {
                act("...Blood is pouring from the wound.", ch, NULL, victim, TO_CHAR);
            }
        }
    }
    if(IS_BODY(victim, CUT_CHEST)) {
        act("$N has a deep wound in $S chest.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_CHEST)) {
            act("...Blood is pouring from the wound.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(IS_BODY(victim, CUT_STOMACH)) {
        act("$N has been disembowelled.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_STOMACH)) {
            act("...Blood is pouring from the wound.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(IS_HEAD(victim, BROKEN_SKULL)) {
        act("$N has got a broken skull.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_TOOTH_1)) {
        teeth += 1;
    }
    if(IS_HEAD(victim, LOST_TOOTH_2)) {
        teeth += 2;
    }
    if(IS_HEAD(victim, LOST_TOOTH_4)) {
        teeth += 4;
    }
    if(IS_HEAD(victim, LOST_TOOTH_8)) {
        teeth += 8;
    }
    if(IS_HEAD(victim, LOST_TOOTH_16)) {
        teeth += 16;
    }
    if(teeth > 0) {
        sprintf(buf, "$N has had %d teeth knocked out.", teeth);
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_TONGUE)) {
        act("$N has had $S tongue ripped out.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_HEAD(victim, LOST_HEAD)) {
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        return;
    }
    /* Check body */
    if(IS_BODY(victim, BROKEN_RIBS_1)) {
        ribs += 1;
    }
    if(IS_BODY(victim, BROKEN_RIBS_2)) {
        ribs += 2;
    }
    if(IS_BODY(victim, BROKEN_RIBS_4)) {
        ribs += 4;
    }
    if(IS_BODY(victim, BROKEN_RIBS_8)) {
        ribs += 8;
    }
    if(IS_BODY(victim, BROKEN_RIBS_16)) {
        ribs += 16;
    }
    if(ribs > 0) {
        sprintf(buf, "$N has got %d broken ribs.", ribs);
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    if(IS_BODY(victim, BROKEN_SPINE)) {
        act("$N has got a broken spine.", ch, NULL, victim, TO_CHAR);
    }
    /* Check arms */
    check_left_arm(ch, victim);
    check_right_arm(ch, victim);
    check_left_leg(ch, victim);
    check_right_leg(ch, victim);
    send_to_char("--------------------------------------------------------------------------------\n\r", ch);
    return;
}

void check_left_arm(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;
    if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM)) {
        act("$N has lost both of $S arms.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_ARM_L) && IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_ARM_L)) {
            act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_ARM_L(victim, LOST_ARM)) {
        act("$N has lost $S left arm.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_ARM_L)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM)) {
        act("$N arms are both broken.", ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_L(victim, BROKEN_ARM)) {
        act("$N's left arm is broken.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND) &&
            !IS_ARM_R(victim, LOST_ARM)) {
        act("$N has lost both of $S hands.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_HAND_L) && IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
            act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_ARM_L(victim, LOST_HAND)) {
        act("$N has lost $S left hand.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_ARM_L(victim, SLIT_WRIST) && IS_ARM_R(victim, SLIT_WRIST) &&
            !IS_ARM_R(victim, LOST_ARM) && !IS_ARM_R(victim, LOST_HAND)) {
        act("$N has had both of $S wrists slashed open.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_WRIST_L) && IS_BLEEDING(victim, BLEEDING_WRIST_R)) {
            act("...Blood is spurting from both wrists.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_WRIST_L)) {
            act("...Blood is spurting from the left wrist.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_WRIST_R)) {
            act("...Blood is spurting from the right wrist.", ch, NULL, victim, TO_CHAR);
        }
    } else if(IS_ARM_L(victim, SLIT_WRIST)) {
        act("$N has had $S left wrist slashed open.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_WRIST_L)) {
            act("...Blood is spurting from the wrist.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(IS_ARM_L(victim, LOST_FINGER_I)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, LOST_FINGER_M)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, LOST_FINGER_R)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, LOST_FINGER_L)) {
        fingers += 1;
    }
    if(fingers == 1) {
        sprintf(finger, "finger");
    } else {
        sprintf(finger, "fingers");
    }
    if(fingers > 0 && IS_ARM_L(victim, LOST_THUMB)) {
        sprintf(buf, "$N has lost %d %s and $S thumb from $S left hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(fingers > 0) {
        sprintf(buf, "$N has lost %d %s from $S left hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_L(victim, LOST_THUMB)) {
        sprintf(buf, "$N has lost the thumb from $S left hand.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    fingers = 0;
    if(IS_ARM_L(victim, BROKEN_FINGER_I) && !IS_ARM_L(victim, LOST_FINGER_I)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, BROKEN_FINGER_M) && !IS_ARM_L(victim, LOST_FINGER_M)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, BROKEN_FINGER_R) && !IS_ARM_L(victim, LOST_FINGER_R)) {
        fingers += 1;
    }
    if(IS_ARM_L(victim, BROKEN_FINGER_L) && !IS_ARM_L(victim, LOST_FINGER_L)) {
        fingers += 1;
    }
    if(fingers == 1) {
        sprintf(finger, "finger");
    } else {
        sprintf(finger, "fingers");
    }
    if(fingers > 0 && IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB)) {
        sprintf(buf, "$N has broken %d %s and $S thumb on $S left hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(fingers > 0) {
        sprintf(buf, "$N has broken %d %s on $S left hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB)) {
        sprintf(buf, "$N has broken the thumb on $S left hand.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    return;
}

void check_right_arm(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;
    if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM)) {
        return;
    }
    if(IS_ARM_R(victim, LOST_ARM)) {
        act("$N has lost $S right arm.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(!IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM)) {
        act("$N's right arm is broken.", ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, BROKEN_ARM)) {
        act("$N's right arm is broken.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND)) {
        return;
    }
    if(IS_ARM_R(victim, LOST_HAND)) {
        act("$N has lost $S right hand.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_ARM_R(victim, SLIT_WRIST) && !IS_ARM_L(victim, SLIT_WRIST)) {
        act("$N has had $S right wrist slashed open.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            act("...Blood is spurting from the wrist.", ch, NULL, victim, TO_CHAR);
        }
    }
    if(IS_ARM_R(victim, LOST_FINGER_I)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, LOST_FINGER_M)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, LOST_FINGER_R)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, LOST_FINGER_L)) {
        fingers += 1;
    }
    if(fingers == 1) {
        sprintf(finger, "finger");
    } else {
        sprintf(finger, "fingers");
    }
    if(fingers > 0 && IS_ARM_R(victim, LOST_THUMB)) {
        sprintf(buf, "$N has lost %d %s and $S thumb from $S right hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(fingers > 0) {
        sprintf(buf, "$N has lost %d %s from $S right hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_R(victim, LOST_THUMB)) {
        sprintf(buf, "$N has lost the thumb from $S right hand.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    fingers = 0;
    if(IS_ARM_R(victim, BROKEN_FINGER_I) && !IS_ARM_R(victim, LOST_FINGER_I)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, BROKEN_FINGER_M) && !IS_ARM_R(victim, LOST_FINGER_M)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, BROKEN_FINGER_R) && !IS_ARM_R(victim, LOST_FINGER_R)) {
        fingers += 1;
    }
    if(IS_ARM_R(victim, BROKEN_FINGER_L) && !IS_ARM_R(victim, LOST_FINGER_L)) {
        fingers += 1;
    }
    if(fingers == 1) {
        sprintf(finger, "finger");
    } else {
        sprintf(finger, "fingers");
    }
    if(fingers > 0 && IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB)) {
        sprintf(buf, "$N has broken %d %s and $S thumb on $S right hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(fingers > 0) {
        sprintf(buf, "$N has broken %d %s on $S right hand.", fingers, finger);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB)) {
        sprintf(buf, "$N has broken the thumb on $S right hand.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    return;
}

void check_left_leg(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;
    if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG)) {
        act("$N has lost both of $S legs.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_LEG_L) && IS_BLEEDING(victim, BLEEDING_LEG_R)) {
            act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_LEG_L)) {
            act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_LEG_R)) {
            act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_LEG_L(victim, LOST_LEG)) {
        act("$N has lost $S left leg.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_LEG_L)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG)) {
        act("$N legs are both broken.", ch, NULL, victim, TO_CHAR);
    } else if(IS_LEG_L(victim, BROKEN_LEG)) {
        act("$N's left leg is broken.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT)) {
        act("$N has lost both of $S feet.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_FOOT_L) && IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
            act("...Blood is spurting from both stumps.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
            act("...Blood is spurting from the left stump.", ch, NULL, victim, TO_CHAR);
        } else if(IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
            act("...Blood is spurting from the right stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_LEG_L(victim, LOST_FOOT)) {
        act("$N has lost $S left foot.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_LEG_L(victim, LOST_TOE_A)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, LOST_TOE_B)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, LOST_TOE_C)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, LOST_TOE_D)) {
        toes += 1;
    }
    if(toes == 1) {
        sprintf(toe, "toe");
    } else {
        sprintf(toe, "toes");
    }
    if(toes > 0 && IS_LEG_L(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has lost %d %s and $S big toe from $S left foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(toes > 0) {
        sprintf(buf, "$N has lost %d %s from $S left foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_LEG_L(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has lost the big toe from $S left foot.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    toes = 0;
    if(IS_LEG_L(victim, BROKEN_TOE_A) && !IS_LEG_L(victim, LOST_TOE_A)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, BROKEN_TOE_B) && !IS_LEG_L(victim, LOST_TOE_B)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, BROKEN_TOE_C) && !IS_LEG_L(victim, LOST_TOE_C)) {
        toes += 1;
    }
    if(IS_LEG_L(victim, BROKEN_TOE_D) && !IS_LEG_L(victim, LOST_TOE_D)) {
        toes += 1;
    }
    if(toes == 1) {
        sprintf(toe, "toe");
    } else {
        sprintf(toe, "toes");
    }
    if(toes > 0 && IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has broken %d %s and $S big toe from $S left foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(toes > 0) {
        sprintf(buf, "$N has broken %d %s on $S left foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has broken the big toe on $S left foot.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    return;
}

void check_right_leg(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;
    if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG)) {
        return;
    }
    if(IS_LEG_R(victim, LOST_LEG)) {
        act("$N has lost $S right leg.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_LEG_R)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(!IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG)) {
        act("$N's right leg is broken.", ch, NULL, victim, TO_CHAR);
    }
    if(IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT)) {
        return;
    }
    if(IS_LEG_R(victim, LOST_FOOT)) {
        act("$N has lost $S right foot.", ch, NULL, victim, TO_CHAR);
        if(IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
            act("...Blood is spurting from the stump.", ch, NULL, victim, TO_CHAR);
        }
        return;
    }
    if(IS_LEG_R(victim, LOST_TOE_A)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, LOST_TOE_B)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, LOST_TOE_C)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, LOST_TOE_D)) {
        toes += 1;
    }
    if(toes == 1) {
        sprintf(toe, "toe");
    } else {
        sprintf(toe, "toes");
    }
    if(toes > 0 && IS_LEG_R(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has lost %d %s and $S big toe from $S right foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(toes > 0) {
        sprintf(buf, "$N has lost %d %s from $S right foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_LEG_R(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has lost the big toe from $S right foot.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    toes = 0;
    if(IS_LEG_R(victim, BROKEN_TOE_A) && !IS_LEG_R(victim, LOST_TOE_A)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, BROKEN_TOE_B) && !IS_LEG_R(victim, LOST_TOE_B)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, BROKEN_TOE_C) && !IS_LEG_R(victim, LOST_TOE_C)) {
        toes += 1;
    }
    if(IS_LEG_R(victim, BROKEN_TOE_D) && !IS_LEG_R(victim, LOST_TOE_D)) {
        toes += 1;
    }
    if(toes == 1) {
        sprintf(toe, "toe");
    } else {
        sprintf(toe, "toes");
    }
    if(toes > 0 && IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has broken %d %s and $S big toe on $S right foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(toes > 0) {
        sprintf(buf, "$N has broken %d %s on $S right foot.", toes, toe);
        act(buf, ch, NULL, victim, TO_CHAR);
    } else if(IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG)) {
        sprintf(buf, "$N has broken the big toe on $S right foot.");
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    return;
}

void obj_score(CHAR_DATA *ch, OBJ_DATA *obj) {
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int itemtype;
    sprintf(buf, "You are %s.\n\r", obj->short_descr);
    send_to_char(buf, ch);
    sprintf(buf, "Type %s, Extra flags %s.\n\r", item_type_name(obj->item_type),
            extra_bit_name(obj->extra_flags));
    send_to_char(buf, ch);
    sprintf(buf, "You weigh %d pounds and are worth %d gold coins.\n\r", obj->weight, obj->cost);
    send_to_char(buf, ch);
    if(obj->questmaker != NULL && strlen(obj->questmaker) > 1 &&
            obj->questowner != NULL && strlen(obj->questowner) > 1) {
        sprintf(buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker, obj->questowner);
        send_to_char(buf, ch);
    } else if(obj->questmaker != NULL && strlen(obj->questmaker) > 1) {
        sprintf(buf, "You were created by %s.\n\r", obj->questmaker);
        send_to_char(buf, ch);
    } else if(obj->questowner != NULL && strlen(obj->questowner) > 1) {
        sprintf(buf, "You are owned by %s.\n\r", obj->questowner);
        send_to_char(buf, ch);
    }
    switch(obj->item_type) {
    case ITEM_SCROLL:
    case ITEM_POTION:
        sprintf(buf, "You contain level %d spells of:", obj->value[0]);
        send_to_char(buf, ch);
        if(obj->value[1] >= 0 && obj->value[1] < MAX_SKILL) {
            send_to_char(" '", ch);
            send_to_char(skill_table[obj->value[1]].name, ch);
            send_to_char("'", ch);
        }
        if(obj->value[2] >= 0 && obj->value[2] < MAX_SKILL) {
            send_to_char(" '", ch);
            send_to_char(skill_table[obj->value[2]].name, ch);
            send_to_char("'", ch);
        }
        if(obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
            send_to_char(" '", ch);
            send_to_char(skill_table[obj->value[3]].name, ch);
            send_to_char("'", ch);
        }
        send_to_char(".\n\r", ch);
        break;
    case ITEM_QUEST:
        sprintf(buf, "Your quest point value is %d.\n\r", obj->value[0]);
        send_to_char(buf, ch);
        break;
    case ITEM_WAND:
    case ITEM_STAFF:
        sprintf(buf, "You have %d(%d) charges of level %d",
                obj->value[1], obj->value[2], obj->value[0]);
        send_to_char(buf, ch);
        if(obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
            send_to_char(" '", ch);
            send_to_char(skill_table[obj->value[3]].name, ch);
            send_to_char("'", ch);
        }
        send_to_char(".\n\r", ch);
        break;
    case ITEM_WEAPON:
        sprintf(buf, "You inflict %d to %d damage in combat (average %d).\n\r",
                obj->value[1], obj->value[2],
                (obj->value[1] + obj->value[2]) / 2);
        send_to_char(buf, ch);
        if(obj->value[0] >= 1000) {
            itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
        } else {
            itemtype = obj->value[0];
        }
        if(itemtype > 0) {
            if(obj->level < 10) {
                sprintf(buf, "You are a minor spell weapon.\n\r");
            } else if(obj->level < 20) {
                sprintf(buf, "You are a lesser spell weapon.\n\r");
            } else if(obj->level < 30) {
                sprintf(buf, "You are an average spell weapon.\n\r");
            } else if(obj->level < 40) {
                sprintf(buf, "You are a greater spell weapon.\n\r");
            } else if(obj->level < 50) {
                sprintf(buf, "You are a major spell weapon.\n\r");
            } else {
                sprintf(buf, "You are a supreme spell weapon.\n\r");
            }
            send_to_char(buf, ch);
        }
        if(itemtype == 1) {
            sprintf(buf, "You are dripping with corrosive acid.\n\r");
        } else if(itemtype == 4) {
            sprintf(buf, "You radiate an aura of darkness.\n\r");
        } else if(itemtype == 30) {
            sprintf(buf, "You are the bane of all evil.\n\r");
        } else if(itemtype == 34) {
            sprintf(buf, "You drink the souls of your victims.\n\r");
        } else if(itemtype == 37) {
            sprintf(buf, "You have been tempered in hellfire.\n\r");
        } else if(itemtype == 48) {
            sprintf(buf, "You crackle with sparks of lightning.\n\r");
        } else if(itemtype == 53) {
            sprintf(buf, "You are dripping with a dark poison.\n\r");
        } else if(itemtype > 0) {
            sprintf(buf, "You have been imbued with the power of %s.\n\r", skill_table[itemtype].name);
        }
        if(itemtype > 0) {
            send_to_char(buf, ch);
        }
        if(obj->value[0] >= 1000) {
            itemtype = obj->value[0] / 1000;
        } else {
            break;
        }
        if(itemtype == 4 || itemtype == 1) {
            sprintf(buf, "You radiate an aura of darkness.\n\r");
        } else if(itemtype == 27 || itemtype == 2) {
            sprintf(buf, "You allow your wielder to see invisible things.\n\r");
        } else if(itemtype == 39 || itemtype == 3) {
            sprintf(buf, "You grant your wielder the power of flight.\n\r");
        } else if(itemtype == 45 || itemtype == 4) {
            sprintf(buf, "You allow your wielder to see in the dark.\n\r");
        } else if(itemtype == 46 || itemtype == 5) {
            sprintf(buf, "You render your wielder invisible to the human eye.\n\r");
        } else if(itemtype == 52 || itemtype == 6) {
            sprintf(buf, "You allow your wielder to walk through solid doors.\n\r");
        } else if(itemtype == 54 || itemtype == 7) {
            sprintf(buf, "You protect your wielder from evil.\n\r");
        } else if(itemtype == 57 || itemtype == 8) {
            sprintf(buf, "You protect your wielder in combat.\n\r");
        } else if(itemtype == 9) {
            sprintf(buf, "You allow your wielder to walk in complete silence.\n\r");
        } else if(itemtype == 10) {
            sprintf(buf, "You surround your wielder with a shield of lightning.\n\r");
        } else if(itemtype == 11) {
            sprintf(buf, "You surround your wielder with a shield of fire.\n\r");
        } else if(itemtype == 12) {
            sprintf(buf, "You surround your wielder with a shield of ice.\n\r");
        } else if(itemtype == 13) {
            sprintf(buf, "You surround your wielder with a shield of acid.\n\r");
        } else if(itemtype == 14) {
            sprintf(buf, "You protect your wielder from attacks from DarkBlade clan guardians.\n\r");
        } else if(itemtype == 15) {
            sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
        } else if(itemtype == 16) {
            sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
        } else if(itemtype == 17) {
            sprintf(buf, "You enable your wielder to move at supernatural speed.\n\r");
        } else if(itemtype == 18) {
            sprintf(buf, "You can slice through armour without difficulty.\n\r");
        } else if(itemtype == 19) {
            sprintf(buf, "You protect your wielder from player attacks.\n\r");
        } else if(itemtype == 20) {
            sprintf(buf, "You surround your wielder with a shield of darkness.\n\r");
        } else if(itemtype == 21) {
            sprintf(buf, "You grant your wielder superior protection.\n\r");
        } else if(itemtype == 22) {
            sprintf(buf, "You grant your wielder supernatural vision.\n\r");
        } else if(itemtype == 23) {
            sprintf(buf, "You make your wielder fleet-footed.\n\r");
        } else if(itemtype == 24) {
            sprintf(buf, "You conceal your wielder from sight.\n\r");
        } else if(itemtype == 25) {
            sprintf(buf, "You invoke the power of your wielders beast.\n\r");
        } else {
            sprintf(buf, "You are bugged...please report it.\n\r");
        }
        if(itemtype > 0) {
            send_to_char(buf, ch);
        }
        break;
    case ITEM_ARMOR:
        sprintf(buf, "Your armor class is %d.\n\r", obj->value[0]);
        send_to_char(buf, ch);
        if(obj->value[3] < 1) {
            break;
        }
        if(obj->value[3] == 4 || obj->value[3] == 1) {
            sprintf(buf, "You radiate an aura of darkness.\n\r");
        } else if(obj->value[3] == 27 || obj->value[3] == 2) {
            sprintf(buf, "You allow your wearer to see invisible things.\n\r");
        } else if(obj->value[3] == 39 || obj->value[3] == 3) {
            sprintf(buf, "You grant your wearer the power of flight.\n\r");
        } else if(obj->value[3] == 45 || obj->value[3] == 4) {
            sprintf(buf, "You allow your wearer to see in the dark.\n\r");
        } else if(obj->value[3] == 46 || obj->value[3] == 5) {
            sprintf(buf, "You render your wearer invisible to the human eye.\n\r");
        } else if(obj->value[3] == 52 || obj->value[3] == 6) {
            sprintf(buf, "You allow your wearer to walk through solid doors.\n\r");
        } else if(obj->value[3] == 54 || obj->value[3] == 7) {
            sprintf(buf, "You protect your wearer from evil.\n\r");
        } else if(obj->value[3] == 57 || obj->value[3] == 8) {
            sprintf(buf, "You protect your wearer in combat.\n\r");
        } else if(obj->value[3] == 9) {
            sprintf(buf, "You allow your wearer to walk in complete silence.\n\r");
        } else if(obj->value[3] == 10) {
            sprintf(buf, "You surround your wearer with a shield of lightning.\n\r");
        } else if(obj->value[3] == 11) {
            sprintf(buf, "You surround your wearer with a shield of fire.\n\r");
        } else if(obj->value[3] == 12) {
            sprintf(buf, "You surround your wearer with a shield of ice.\n\r");
        } else if(obj->value[3] == 13) {
            sprintf(buf, "You surround your wearer with a shield of acid.\n\r");
        } else if(obj->value[3] == 14) {
            sprintf(buf, "You protect your wearer from attacks from DarkBlade clan guardians.\n\r");
        } else if(obj->value[3] == 15) {
            sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
        } else if(obj->value[3] == 16) {
            sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
        } else if(obj->value[3] == 17) {
            sprintf(buf, "You enable your wearer to move at supernatural speed.\n\r");
        } else if(obj->value[3] == 18) {
            sprintf(buf, "You can slice through armour without difficulty.\n\r");
        } else if(obj->value[3] == 19) {
            sprintf(buf, "You protect your wearer from player attacks.\n\r");
        } else if(obj->value[3] == 20) {
            sprintf(buf, "You surround your wearer with a shield of darkness.\n\r");
        } else if(obj->value[3] == 21) {
            sprintf(buf, "You grant your wearer superior protection.\n\r");
        } else if(obj->value[3] == 22) {
            sprintf(buf, "You grant your wearer supernatural vision.\n\r");
        } else if(obj->value[3] == 23) {
            sprintf(buf, "You make your wearer fleet-footed.\n\r");
        } else if(obj->value[3] == 24) {
            sprintf(buf, "You conceal your wearer from sight.\n\r");
        } else if(obj->value[3] == 25) {
            sprintf(buf, "You invoke the power of your wearers beast.\n\r");
        } else {
            sprintf(buf, "You are bugged...please report it.\n\r");
        }
        if(obj->value[3] > 0) {
            send_to_char(buf, ch);
        }
        break;
    }
    for(paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
        if(paf->location != APPLY_NONE && paf->modifier != 0) {
            sprintf(buf, "You %s %d.\n\r",
                    affect_loc_name(paf->location), paf->modifier);
            send_to_char(buf, ch);
        }
    }
    for(paf = obj->affected; paf != NULL; paf = paf->next) {
        if(paf->location != APPLY_NONE && paf->modifier != 0) {
            sprintf(buf, "You %s %d.\n\r",
                    affect_loc_name(paf->location), paf->modifier);
            send_to_char(buf, ch);
        }
    }
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt(CHAR_DATA *ch, char *argument) {
    char buf [ MAX_STRING_LENGTH ];
    buf[0] = '\0';
    if(IS_NPC(ch)) {
        return;
    }
    /*
       ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );
    */
    if(argument[0] == '\0') {
        do_help(ch, "prompt");
        return;
    }
    if(!strcmp(argument, "on")) {
        if(IS_EXTRA(ch, EXTRA_PROMPT)) {
            send_to_char("But you already have customised prompt on!\n\r", ch);
        } else {
            send_to_char("Ok.\n\r", ch);
            SET_BIT(ch->extra, EXTRA_PROMPT);
        }
        return;
    } else if(!strcmp(argument, "off")) {
        if(!IS_EXTRA(ch, EXTRA_PROMPT)) {
            send_to_char("But you already have customised prompt off!\n\r", ch);
        } else {
            send_to_char("Ok.\n\r", ch);
            REMOVE_BIT(ch->extra, EXTRA_PROMPT);
        }
        return;
    } else if(!strcmp(argument, "clear")) {
        free_string(ch->prompt);
        ch->prompt = str_dup("");
        return;
    } else {
        if(strlen(argument) > 50) {
            argument[50] = '\0';
        }
        smash_tilde(argument);
        strcat(buf, argument);
    }
    free_string(ch->prompt);
    ch->prompt = str_dup(buf);
    send_to_char("Ok.\n\r", ch);
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_cprompt(CHAR_DATA *ch, char *argument) {
    char buf [ MAX_STRING_LENGTH ];
    buf[0] = '\0';
    if(IS_NPC(ch)) {
        return;
    }
    /*
       ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );
    */
    if(argument[0] == '\0') {
        do_help(ch, "cprompt");
        return;
    }
    if(!strcmp(argument, "clear")) {
        free_string(ch->cprompt);
        ch->cprompt = str_dup("");
        return;
    } else {
        if(strlen(argument) > 50) {
            argument[50] = '\0';
        }
        smash_tilde(argument);
        strcat(buf, argument);
    }
    free_string(ch->cprompt);
    ch->cprompt = str_dup(buf);
    send_to_char("Ok.\n\r", ch);
    return;
}

void do_peek(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Peek at whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They are not here.\n\r", ch);
        return;
    }
    if(number_percent() <= ch->pcdata->learned[gsn_peek]) {
        send_to_char("You peek at their inventory:\n\r", ch);
        show_list_to_char(victim->carrying, ch, TRUE, TRUE);
    } else {
        send_to_char("You peek at their inventory:\n\r", ch);
        send_to_char("     Nothing.\n\r", ch);
    }
    WAIT_STATE(ch, 12);
    return;
}

void do_affects(CHAR_DATA *ch, char *argument) {
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    if(ch->affected != NULL) {
        send_to_char("You are affected by:\n\r", ch);
        for(paf = ch->affected; paf != NULL; paf = paf->next) {
            sprintf(buf, "Affect: '%s'", skill_table[paf->type].name);
            send_to_char(buf, ch);
            if(ch->level >= 0) {
                sprintf(buf,
                        " %s %d for %d hours with bits %s.\n\r",
                        affect_loc_name(paf->location),
                        paf->modifier,
                        paf->duration,
                        affect_bit_name(paf->bitvector));
                send_to_char(buf, ch);
            }
        }
    } else {
        send_to_char("You don't have any special affects on you.\n\r", ch);
    }
    return;
}

