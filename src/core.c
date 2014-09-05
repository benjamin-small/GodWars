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

void do_setstance(CHAR_DATA *ch, char *argument) {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char arg_buf[MAX_STRING_LENGTH];
    int p_l = 0, p_g = 0, p_s = 0;
    int mincost = 250, maxcost = 500;
    int selected = 0, cost = 0, supers = 0;
    int lesser = 0, greater = 0, supreme = 0;
    int max_lesser = 0, max_greater = 0, max_supreme = 0;
    strcpy(arg_buf, argument);
    argument = &arg_buf[0];
    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    strcpy(arg2, argument);
    if(IS_NPC(ch)) {
        return;
    }
    if(ch->stance[6] < 200 || ch->stance[7] < 200 || ch->stance[8] < 200 ||
            ch->stance[9] < 200 || ch->stance[10] < 200) {
        send_to_char("To set your own stance, you must be Grand Master in all traditional stances.\n\r", ch);
        return;
    }
    if((ch->stance[11] > 0 && ch->stance[11] < 200) ||
            (ch->stance[12] > 0 && ch->stance[12] < 200) ||
            (ch->stance[13] > 0 && ch->stance[13] < 200) ||
            (ch->stance[14] > 0 && ch->stance[14] < 200) ||
            (ch->stance[15] > 0 && ch->stance[15] < 200)) {
        send_to_char("You must master your current superstance before working on another!\n\r", ch);
        return;
    }
    if(ch->stance[11] > 0) {
        selected++;
        mincost = 250;
        maxcost = 500;
    }
    if(ch->stance[12] > 0) {
        selected++;
        mincost = 500;
        maxcost = 750;
    }
    if(ch->stance[13] > 0) {
        selected++;
        mincost = 750;
        maxcost = 1000;
    }
    if(ch->stance[14] > 0) {
        selected++;
        mincost = 1000;
        maxcost = 1500;
    }
    if(ch->stance[15] > 0) {
        selected++;
    }
    if(selected >= 5) {
        send_to_char("You have already mastered your five different stances!\n\r", ch);
        return;
    }
    switch(selected) {
    default:
    case QS_11:
        max_lesser = 2;
        max_greater = 0;
        max_supreme = 0;
        break;
    case QS_12:
        max_lesser = 2;
        max_greater = 1;
        max_supreme = 0;
        break;
    case QS_13:
        max_lesser = 3;
        max_greater = 2;
        max_supreme = 1;
        break;
    case QS_14:
        max_lesser = 4;
        max_greater = 2;
        max_supreme = 1;
        break;
    case QS_15:
        max_lesser = 5;
        max_greater = 3;
        max_supreme = 2;
        break;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_LESSER)) {
        lesser++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_LESSER)) {
        lesser++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_LESSER)) {
        lesser++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_POW_LESSER)) {
        lesser++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_RES_LESSER)) {
        lesser++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_GREATER)) {
        greater++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_GREATER)) {
        greater++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_GREATER)) {
        greater++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_POW_GREATER)) {
        greater++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_RES_GREATER)) {
        greater++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_SUPREME)) {
        supreme++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_SUPREME)) {
        supreme++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_SUPREME)) {
        supreme++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_POW_SUPREME)) {
        supreme++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_RES_SUPREME)) {
        supreme++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_PARRY)) {
        supers++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_DODGE)) {
        supers++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_SPEED)) {
        supers++;
    }
    if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_BYPASS)) {
        supers++;
    }
    cost = (((lesser * 50) + (greater * 100) + (supreme * 200) +
             ((supers + supers) * 100))) * 10000;
    mincost *= 10000;
    maxcost *= 10000;
    if(arg1[0] != '\0' && !str_cmp(arg1, "done")) {
        int stance = selected + 11;
        if(cost < mincost || cost > maxcost) {
            sprintf(buf, "Sorry, you must spend between %d and %d exp on your stance.\n\r", mincost, maxcost);
            send_to_char(buf, ch);
            sprintf(buf, "You currently have %d exp allocated to this stance.\n\r", cost);
            send_to_char(buf, ch);
            return;
        }
        if(ch->exp < cost) {
            sprintf(buf, "Sorry, you don't have the required %d exp to buy that stance.\n\r", cost);
            send_to_char(buf, ch);
            return;
        }
        if(stance < 11 || stance > 15) {
            send_to_char("Bug, please inform KaVir.\n\r", ch);
            return;
        }
        if(strlen(ch->pcdata->stancework) < 2) {
            send_to_char("First you must name your stance.\n\r", ch);
            return;
        }
        if(ch->pcdata->stancemove[selected] < 1) {
            send_to_char("First you must specify a type of move for your stance.\n\r", ch);
            return;
        }
        if(strlen(ch->pcdata->stancelist) < 2) {
            free_string(ch->pcdata->stancelist);
            ch->pcdata->stancelist = str_dup(ch->pcdata->stancework);
        } else {
            if(is_name(ch->pcdata->stancework, "viper crane crab mongoose bull")) {
                send_to_char("There is already a basic stance of that name.\n\r", ch);
                return;
            }
            if(is_name(ch->pcdata->stancework, "mantis dragon tiger monkey swallow")) {
                send_to_char("There is already an advanced stance of that name.\n\r", ch);
                return;
            }
            if(is_name(ch->pcdata->stancework, ch->pcdata->stancelist)) {
                send_to_char("You cannot have two stances with the same name.\n\r", ch);
                return;
            }
            strcpy(buf, ch->pcdata->stancelist);
            strcat(buf, " ");
            strcat(buf, ch->pcdata->stancework);
            free_string(ch->pcdata->stancelist);
            ch->pcdata->stancelist = str_dup(buf);
        }
        ch->exp -= cost;
        free_string(ch->pcdata->stancework);
        ch->pcdata->stancework = str_dup("");
        ch->stance[stance] = 1;
        SET_BIT(ch->pcdata->qstats[selected], QS_DONE);
        send_to_char("Congratulations, you now have a new stance!\n\r", ch);
        return;
    }
    if(arg1[0] != '\0' && !str_cmp(arg1, "clear")) {
        ch->pcdata->qstats[selected] = 0;
        send_to_char("Stance options cleared.\n\r", ch);
        return;
    }
    if(arg1[0] == '\0') {
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        send_to_char("                              -= Super Stances =-\n\r", ch);
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        send_to_char("[HIT] Improves your chance to hit and parry.\n\r", ch);
        send_to_char("[DAM] Improves the amount of damage you inflict.\n\r", ch);
        send_to_char("[ARM] Reduces your chance of being hit.\n\r", ch);
        send_to_char("[POW] Increases your maximum damage (damage cap).\n\r", ch);
        send_to_char("[RES] Reduces the maximum damage of those hitting you.\n\r\n\r", ch);
        switch(selected) {
        default:
        case 0:
            send_to_char("You may have two of the above powers, costing 500000 exp each.\n\r", ch);
            break;
        case 1:
            send_to_char("You may have two of the above powers, costing 500000 exp each.\n\r", ch);
            send_to_char("One of the above powers may be a GREATER power, costing 1000000 exp instead.\n\r", ch);
            break;
        case 2:
            send_to_char("You may have three of the above powers, costing 500000 exp each.\n\r", ch);
            send_to_char("Two of the above powers may be a GREATER power, costing 1000000 exp instead.\n\r", ch);
            break;
        case 3:
            send_to_char("You may have four of the above powers, costing 500000 exp each.\n\r", ch);
            send_to_char("Two of the above powers may be a GREATER power, costing 1000000 exp instead.\n\r", ch);
            send_to_char("One of the above powers may be a SUPREME power, costing 2000000 exp instead.\n\r", ch);
            break;
        case 4:
            send_to_char("You may have five of the above powers, costing 500000 exp each.\n\r", ch);
            send_to_char("Three of the above powers may be a GREATER power, costing 1000000 exp instead.\n\r", ch);
            send_to_char("Two of the above powers may be a SUPREME power, costing 2000000 exp instead.\n\r", ch);
            break;
        }
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        send_to_char("The following abilities cost 2000000, 4000000, 6000000, etc - each:\n\r", ch);
        send_to_char("[PARRY] Improved parry ability.        [DODGE]  Improved dodge ability.\n\r", ch);
        send_to_char("[SPEED] Improved combat speed.         [BYPASS] Bypass above two powers.\n\r", ch);
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        send_to_char("Currently selected options: ", ch);
        if(ch->pcdata->qstats[selected] == 0) {
            send_to_char("None. ", ch);
        }
        if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_LESSER)) {
            send_to_char("HIT. ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_GREATER)) {
            send_to_char("HIT (Greater). ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_HIT_SUPREME)) {
            send_to_char("HIT (Supreme). ", ch);
        }
        if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_LESSER)) {
            send_to_char("DAM. ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_GREATER)) {
            send_to_char("DAM (Greater). ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_DAM_SUPREME)) {
            send_to_char("DAM (Supreme). ", ch);
        }
        if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_LESSER)) {
            send_to_char("ARM. ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_GREATER)) {
            send_to_char("ARM (Greater). ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_ARM_SUPREME)) {
            send_to_char("ARM (Supreme). ", ch);
        }
        if(IS_SET(ch->pcdata->qstats[selected], QS_POW_LESSER)) {
            send_to_char("POW. ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_POW_GREATER)) {
            send_to_char("POW (Greater). ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_POW_SUPREME)) {
            send_to_char("POW (Supreme). ", ch);
        }
        if(IS_SET(ch->pcdata->qstats[selected], QS_RES_LESSER)) {
            send_to_char("RES. ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_RES_GREATER)) {
            send_to_char("RES (Greater). ", ch);
        } else if(IS_SET(ch->pcdata->qstats[selected], QS_RES_SUPREME)) {
            send_to_char("RES (Supreme). ", ch);
        }
        send_to_char("\n\r", ch);
        if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_DODGE) ||
                IS_SET(ch->pcdata->qstats[selected], QS_SUPER_PARRY) ||
                IS_SET(ch->pcdata->qstats[selected], QS_SUPER_SPEED) ||
                IS_SET(ch->pcdata->qstats[selected], QS_SUPER_BYPASS)) {
            send_to_char("Special Abilities: ", ch);
            if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_DODGE)) {
                send_to_char("DODGE. ", ch);
            }
            if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_PARRY)) {
                send_to_char("PARRY. ", ch);
            }
            if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_SPEED)) {
                send_to_char("SPEED. ", ch);
            }
            if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_BYPASS)) {
                send_to_char("BYPASS. ", ch);
            }
            send_to_char("\n\r", ch);
        }
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        if(selected == 0) {
            send_to_char("Syntax: setstance <power>.  To undo choice: setstance <power> clear.\n\r", ch);
        } else {
            send_to_char("Syntax: setstance <power> <level>.  To undo choice: setstance <power> clear.\n\r", ch);
        }
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        send_to_char("To name your stance, type: setstance name <name>.  Also see: setstance move.\n\r", ch);
        send_to_char("When you have finished, type: setstance done.  To restart type: setstance clear.\n\r", ch);
        send_to_char("--------------------------------------------------------------------------------\n\r", ch);
        return;
    }
    if(!str_cmp(arg1, "name")) {
        if(arg2[0] == '\0') {
            send_to_char("What do you wish to name your stance?\n\r", ch);
            return;
        } else {
            if(!check_parse_name(arg2)) {
                send_to_char("Stance name must be 3-12 characters long, alphabetic characters only.\n\r", ch);
            } else {
                free_string(ch->pcdata->stancework);
                ch->pcdata->stancework = str_dup(capitalize(arg2));
                send_to_char("Name set.\n\r", ch);
            }
        }
        return;
    }
    if(!str_cmp(arg1, "move")) {
        if(arg2[0] == '\0') {
            send_to_char("Please select a stance movement.  You ??? into your stance, where ??? is:\n\r", ch);
            send_to_char("Arch, Swing, Squat, Twist, Hunch, Spin, Coil, Lunge, Rotate, Slide,\n\r", ch);
            send_to_char("Twirl, Leap, Swoop, Roll, Wriggle.\n\r", ch);
        } else {
            if(!str_cmp(arg2, "arch")) {
                ch->pcdata->stancemove[selected] = 1;
            } else if(!str_cmp(arg2, "swing")) {
                ch->pcdata->stancemove[selected] = 2;
            } else if(!str_cmp(arg2, "squat")) {
                ch->pcdata->stancemove[selected] = 3;
            } else if(!str_cmp(arg2, "twist")) {
                ch->pcdata->stancemove[selected] = 4;
            } else if(!str_cmp(arg2, "hunch")) {
                ch->pcdata->stancemove[selected] = 5;
            } else if(!str_cmp(arg2, "spin")) {
                ch->pcdata->stancemove[selected] = 6;
            } else if(!str_cmp(arg2, "coil")) {
                ch->pcdata->stancemove[selected] = 7;
            } else if(!str_cmp(arg2, "lunge")) {
                ch->pcdata->stancemove[selected] = 8;
            } else if(!str_cmp(arg2, "rotate")) {
                ch->pcdata->stancemove[selected] = 9;
            } else if(!str_cmp(arg2, "slide")) {
                ch->pcdata->stancemove[selected] = 10;
            } else if(!str_cmp(arg2, "twirl")) {
                ch->pcdata->stancemove[selected] = 11;
            } else if(!str_cmp(arg2, "leap")) {
                ch->pcdata->stancemove[selected] = 12;
            } else if(!str_cmp(arg2, "swoop")) {
                ch->pcdata->stancemove[selected] = 13;
            } else if(!str_cmp(arg2, "roll")) {
                ch->pcdata->stancemove[selected] = 14;
            } else if(!str_cmp(arg2, "wriggle")) {
                ch->pcdata->stancemove[selected] = 15;
            } else {
                do_setstance(ch, "move");
                return;
            }
            send_to_char("Move type set.\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg1, "parry")) {
        if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_PARRY)) {
            send_to_char("PARRY ability removed.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], QS_SUPER_PARRY);
        } else {
            send_to_char("PARRY ability set.\n\r", ch);
            SET_BIT(ch->pcdata->qstats[selected], QS_SUPER_PARRY);
        }
        return;
    }
    if(!str_cmp(arg1, "dodge")) {
        if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_DODGE)) {
            send_to_char("DODGE ability removed.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], QS_SUPER_DODGE);
        } else {
            send_to_char("DODGE ability set.\n\r", ch);
            SET_BIT(ch->pcdata->qstats[selected], QS_SUPER_DODGE);
        }
        return;
    }
    if(!str_cmp(arg1, "speed")) {
        if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_SPEED)) {
            send_to_char("SPEED ability removed.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], QS_SUPER_SPEED);
        } else {
            send_to_char("SPEED ability set.\n\r", ch);
            SET_BIT(ch->pcdata->qstats[selected], QS_SUPER_SPEED);
        }
        return;
    }
    if(!str_cmp(arg1, "bypass")) {
        if(IS_SET(ch->pcdata->qstats[selected], QS_SUPER_BYPASS)) {
            send_to_char("BYPASS ability removed.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], QS_SUPER_BYPASS);
        } else {
            send_to_char("BYPASS ability set.\n\r", ch);
            SET_BIT(ch->pcdata->qstats[selected], QS_SUPER_BYPASS);
        }
        return;
    }
    if(!str_cmp(arg1, "hit")) {
        p_l = QS_HIT_LESSER;
        p_g = QS_HIT_GREATER;
        p_s = QS_HIT_SUPREME;
    } else if(!str_cmp(arg1, "dam")) {
        p_l = QS_DAM_LESSER;
        p_g = QS_DAM_GREATER;
        p_s = QS_DAM_SUPREME;
    } else if(!str_cmp(arg1, "arm")) {
        p_l = QS_ARM_LESSER;
        p_g = QS_ARM_GREATER;
        p_s = QS_ARM_SUPREME;
    } else if(!str_cmp(arg1, "pow")) {
        p_l = QS_POW_LESSER;
        p_g = QS_POW_GREATER;
        p_s = QS_POW_SUPREME;
    } else if(!str_cmp(arg1, "res")) {
        p_l = QS_RES_LESSER;
        p_g = QS_RES_GREATER;
        p_s = QS_RES_SUPREME;
    } else {
        do_setstance(ch, "");
        return;
    }
    if(arg2[0] == '\0' || !str_cmp(arg2, "lesser")) {
        if(lesser >= max_lesser) {
            if(selected == 0) {
                send_to_char("You may not have any more powers.\n\r", ch);
            } else {
                send_to_char("You may not have any more lesser powers.\n\r", ch);
            }
        } else {
            send_to_char("Ok.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_g);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_s);
            SET_BIT(ch->pcdata->qstats[selected], p_l);
        }
        return;
    } else if(selected > 0 && !str_cmp(arg2, "greater")) {
        if(greater >= max_greater) {
            send_to_char("You may not have any more greater powers.\n\r", ch);
        } else {
            send_to_char("Ok.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_l);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_s);
            SET_BIT(ch->pcdata->qstats[selected], p_g);
        }
        return;
    } else if(selected > 2 && !str_cmp(arg2, "supreme")) {
        if(supreme >= max_supreme) {
            send_to_char("You may not have any more supreme powers.\n\r", ch);
        } else {
            send_to_char("Ok.\n\r", ch);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_l);
            REMOVE_BIT(ch->pcdata->qstats[selected], p_g);
            SET_BIT(ch->pcdata->qstats[selected], p_s);
        }
        return;
    } else if(!str_cmp(arg2, "clear")) {
        send_to_char("Cleared.\n\r", ch);
        REMOVE_BIT(ch->pcdata->qstats[selected], p_l);
        REMOVE_BIT(ch->pcdata->qstats[selected], p_g);
        REMOVE_BIT(ch->pcdata->qstats[selected], p_s);
        return;
    }
    do_setstance(ch, "");
    return;
}

bool is_undying(CHAR_DATA *ch) {
    if(IS_NPC(ch)) {
        return FALSE;
    }
    if(IS_HERO(ch)) {
        return TRUE;
    }
    /*
        if (IS_VAMPIRE(ch) || IS_WEREWOLF(ch) || IS_DEMON(ch)) return TRUE;
    */
    return FALSE;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA *ch, CHAR_DATA *victim) {
    if(ch->fighting != NULL) {
        bug("Set_fighting: already fighting", 0);
        return;
    }
    if(ch->next_fight != NULL || ch->prev_fight != NULL) {
        bug("Set_fighting: new fight code", 0);
        return;
    }
    if(IS_AFFECTED(ch, AFF_SLEEP)) {
        affect_strip(ch, gsn_sleep);
    }
    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->damcap[DAM_CHANGE] = 1;
    if(first_fight == NULL) {
        first_fight = ch;
    }
    if(last_fight != NULL) {
        last_fight->next_fight = ch;
        ch->prev_fight = last_fight;
    }
    last_fight = ch;
    return;
}



/*
 * Stop fights.
 */
void stop_fighting(CHAR_DATA *ch, bool fBoth) {
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    for(fch = first_fight; fch != NULL; fch = fch_next) {
        fch_next = fch->next_fight;
        if(fch == ch || (fBoth && fch->fighting == ch)) {
            fch->fighting	= NULL;
            fch->position	= POS_STANDING;
            if(fch->prev_fight != NULL && fch->next_fight != NULL) {
                fch->prev_fight->next_fight = fch->next_fight;
                fch->next_fight->prev_fight = fch->prev_fight;
            } else if(fch->prev_fight != NULL) {
                fch->prev_fight->next_fight = NULL;
                last_fight = fch->prev_fight;
            } else if(fch->next_fight != NULL) {
                fch->next_fight->prev_fight = NULL;
                first_fight = fch->next_fight;
            } else {
                first_fight = NULL;
                last_fight = NULL;
            }
            fch->prev_fight = NULL;
            fch->next_fight = NULL;
            update_pos(fch);
        }
    }
    return;
}

bool regenerate_part(OBJ_DATA *obj) {
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    int teeth = 0;
    if(obj->in_obj != NULL) {
        return FALSE;
    }
    if(strlen(obj->questmaker) < 2) {
        return FALSE;
    }
    if(obj->in_room != NULL && (victim = obj->in_room->people) != NULL) {
        if((ch = get_char_room(victim, obj->questmaker)) == NULL) {
            return FALSE;
        }
    } else if((victim = obj->carried_by) != NULL && victim->in_room != NULL) {
        if((ch = get_char_room(victim, obj->questmaker)) == NULL) {
            return FALSE;
        }
    } else {
        return FALSE;
    }
    if((victim = obj->carried_by) != NULL && victim != ch &&
            victim->in_room != NULL) {
        act("$p leaps out of your hands.", victim, obj, NULL, TO_CHAR);
        act("$p leaps out of $n's hands.", victim, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, victim->in_room);
        return FALSE;
    }
    /*
        if ( obj->in_room != NULL && ch->in_room == obj->in_room )
        {
    	act("$p leaps into your hands.",ch,obj,NULL,TO_CHAR);
    	act("$p leaps into $n's hands.",ch,obj,NULL,TO_ROOM);
    	obj_from_room( obj );
    	obj_to_char( obj, ch );
    	return FALSE;
        }
    */
    if(obj->in_room != NULL && (victim = obj->in_room->people) != NULL &&
            ch->in_room != obj->in_room && number_range(1, 3) != 1) {
        if(number_range(1, 4) == 1) {
            act("$p twitches on the floor.", victim, obj, NULL, TO_CHAR);
            act("$p twitches on the floor.", victim, obj, NULL, TO_ROOM);
        }
        return FALSE;
    }
    if(IS_HEAD(ch, LOST_TOOTH_1)) {
        teeth += 1;
    }
    if(IS_HEAD(ch, LOST_TOOTH_2)) {
        teeth += 2;
    }
    if(IS_HEAD(ch, LOST_TOOTH_4)) {
        teeth += 4;
    }
    if(IS_HEAD(ch, LOST_TOOTH_8)) {
        teeth += 8;
    }
    if(IS_HEAD(ch, LOST_TOOTH_16)) {
        teeth += 16;
    }
    if(obj->pIndexData->vnum == OBJ_VNUM_SLICED_ARM) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_ARM)) {
            return FALSE;
        }
        if(IS_ARM_L(ch, LOST_ARM)) {
            if(IS_BLEEDING(ch, BLEEDING_ARM_L)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_ARM);
        } else if(IS_ARM_R(ch, LOST_ARM)) {
            if(IS_BLEEDING(ch, BLEEDING_ARM_R)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_ARM);
        }
        act("$p shoots towards your shoulder.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's shoulder.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the stump of your shoulder.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the stump of $n's shoulder.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SLICED_LEG) {
        if(!IS_LEG_L(ch, LOST_LEG) && !IS_LEG_R(ch, LOST_LEG)) {
            return FALSE;
        }
        if(IS_LEG_L(ch, LOST_LEG)) {
            if(IS_BLEEDING(ch, BLEEDING_LEG_L)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_LEG_L], LOST_LEG);
        } else if(IS_LEG_R(ch, LOST_LEG)) {
            if(IS_BLEEDING(ch, BLEEDING_LEG_R)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_LEG_R], LOST_LEG);
        }
        act("$p shoots towards your hip.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hip.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the stump of your hip.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the stump of $n's hip.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SQUIDGY_EYEBALL) {
        if(!IS_HEAD(ch, LOST_EYE_L) && !IS_HEAD(ch, LOST_EYE_R)) {
            return FALSE;
        }
        if(IS_HEAD(ch, LOST_EYE_L)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_L);
        } else if(IS_HEAD(ch, LOST_EYE_R)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_R);
        }
        act("$p shoots towards your face.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's face.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself into your empty eye socket.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself into $n's empty eye socket.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SLICED_EAR) {
        if(!IS_HEAD(ch, LOST_EAR_L) && !IS_HEAD(ch, LOST_EAR_R)) {
            return FALSE;
        }
        if(IS_HEAD(ch, LOST_EAR_L)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EAR_L);
        } else if(IS_HEAD(ch, LOST_EAR_R)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_EAR_R);
        }
        act("$p shoots towards your head.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's head.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the side of your head.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the side of $n's head.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SLICED_NOSE) {
        if(!IS_HEAD(ch, LOST_NOSE)) {
            return FALSE;
        }
        REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_NOSE);
        act("$p shoots towards your face.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's face.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the front of your face.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the front of $n's face.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_HAND) {
        if(!IS_ARM_L(ch, LOST_ARM) && IS_ARM_L(ch, LOST_HAND)) {
            if(IS_BLEEDING(ch, BLEEDING_HAND_L)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_HAND);
        } else if(!IS_ARM_R(ch, LOST_ARM) && IS_ARM_R(ch, LOST_HAND)) {
            if(IS_BLEEDING(ch, BLEEDING_HAND_R)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_HAND);
        } else {
            return FALSE;
        }
        act("$p shoots towards your wrist.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's wrist.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the stump of your wrist.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the stump of $n's wrist.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_FOOT) {
        if(!IS_LEG_L(ch, LOST_LEG) && IS_LEG_L(ch, LOST_FOOT)) {
            if(IS_BLEEDING(ch, BLEEDING_LEG_L)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_LEG_L], LOST_FOOT);
        } else if(!IS_LEG_R(ch, LOST_LEG) && IS_LEG_R(ch, LOST_FOOT)) {
            if(IS_BLEEDING(ch, BLEEDING_LEG_R)) {
                return FALSE;
            }
            REMOVE_BIT(ch->loc_hp[LOC_LEG_R], LOST_FOOT);
        } else {
            return FALSE;
        }
        act("$p shoots towards your ankle.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's ankle.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto the stump of your ankle.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto the stump of $n's ankle.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_THUMB) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_L(ch, LOST_HAND)
                && IS_ARM_L(ch, LOST_THUMB)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_THUMB);
        } else if(!IS_ARM_R(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_HAND)
                  && IS_ARM_R(ch, LOST_THUMB)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_THUMB);
        } else {
            return FALSE;
        }
        act("$p shoots towards your hand.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hand.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto your hand.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto $n's hand.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_INDEX) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_L(ch, LOST_HAND)
                && IS_ARM_L(ch, LOST_FINGER_I)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_I);
        } else if(!IS_ARM_R(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_HAND)
                  && IS_ARM_R(ch, LOST_FINGER_I)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_I);
        } else {
            return FALSE;
        }
        act("$p shoots towards your hand.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hand.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto your hand.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto $n's hand.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_MIDDLE) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_L(ch, LOST_HAND)
                && IS_ARM_L(ch, LOST_FINGER_M)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_M);
        } else if(!IS_ARM_R(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_HAND)
                  && IS_ARM_R(ch, LOST_FINGER_M)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_M);
        } else {
            return FALSE;
        }
        act("$p shoots towards your hand.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hand.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto your hand.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto $n's hand.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_RING) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_L(ch, LOST_HAND)
                && IS_ARM_L(ch, LOST_FINGER_R)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_R);
        } else if(!IS_ARM_R(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_HAND)
                  && IS_ARM_R(ch, LOST_FINGER_R)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_R);
        } else {
            return FALSE;
        }
        act("$p shoots towards your hand.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hand.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto your hand.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto $n's hand.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(obj->pIndexData->vnum == OBJ_VNUM_SEVERED_LITTLE) {
        if(!IS_ARM_L(ch, LOST_ARM) && !IS_ARM_L(ch, LOST_HAND)
                && IS_ARM_L(ch, LOST_FINGER_L)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_L], LOST_FINGER_L);
        } else if(!IS_ARM_R(ch, LOST_ARM) && !IS_ARM_R(ch, LOST_HAND)
                  && IS_ARM_R(ch, LOST_FINGER_L)) {
            REMOVE_BIT(ch->loc_hp[LOC_ARM_R], LOST_FINGER_L);
        } else {
            return FALSE;
        }
        act("$p shoots towards your hand.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's hand.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself onto your hand.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself onto $n's hand.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    } else if(teeth > 0) {
        if(IS_HEAD(ch, LOST_TOOTH_1)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_1);
        }
        if(IS_HEAD(ch, LOST_TOOTH_2)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_2);
        }
        if(IS_HEAD(ch, LOST_TOOTH_4)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_4);
        }
        if(IS_HEAD(ch, LOST_TOOTH_8)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_8);
        }
        if(IS_HEAD(ch, LOST_TOOTH_16)) {
            REMOVE_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_16);
        }
        teeth -= 1;
        if(teeth >= 16) {
            teeth -= 16;
            SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_16);
        }
        if(teeth >= 8) {
            teeth -= 8;
            SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_8);
        }
        if(teeth >= 4) {
            teeth -= 4;
            SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_4);
        }
        if(teeth >= 2) {
            teeth -= 2;
            SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_2);
        }
        if(teeth >= 1) {
            teeth -= 1;
            SET_BIT(ch->loc_hp[LOC_HEAD], LOST_TOOTH_1);
        }
        act("$p shoots towards your face.", ch, obj, NULL, TO_CHAR);
        act("$p shoots towards $n's face.", ch, obj, NULL, TO_ROOM);
        act("$p presses itself into your mouth.", ch, obj, NULL, TO_CHAR);
        act("$p presses itself into $n's mouth.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return TRUE;
    }
    return FALSE;
}

/*
 * The primary output interface for formatted output.
 */
void act(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type) {
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    /* Next 1 line
    */
    CHAR_DATA *to_old;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    /* Next 2 lines
    */
    CHAR_DATA *familiar = NULL;
    CHAR_DATA *wizard = NULL;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;
    /* Next 1 line
    */
    bool is_fam;
    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if(format == NULL || format[0] == '\0') {
        return;
    }
    to = ch->in_room->people;
    if(type == TO_VICT) {
        if(vch == NULL || vch->in_room == NULL) {
            bug("Act: null vch with TO_VICT.", 0);
            return;
        }
        to = vch->in_room->people;
    }
    for(; to != NULL; to = to->next_in_room) {
        /* Next 2 lines
        */
        is_fam = FALSE;
        to_old = to;
        if(type == TO_CHAR && to != ch) {
            continue;
        }
        if(type == TO_VICT && (to != vch || to == ch)) {
            continue;
        }
        if(type == TO_ROOM && to == ch) {
            continue;
        }
        if(type == TO_NOTVICT && (to == ch || to == vch)) {
            continue;
        }
        /* Next if statement
        */
        if(to->desc == NULL && IS_NPC(to) && (wizard = to->wizard) != NULL) {
            if(!IS_NPC(wizard) && ((familiar = wizard->pcdata->familiar) != NULL)
                    && familiar == to) {
                if(to->in_room == ch->in_room &&
                        wizard->in_room != to->in_room) {
                    to = wizard;
                    SET_BIT(to->extra, EXTRA_CAN_SEE);
                    is_fam = TRUE;
                }
            }
        }
        if(to->desc == NULL || !IS_AWAKE(to)) {
            /* Next if statement
            */
            if(is_fam) {
                REMOVE_BIT(to->extra, EXTRA_CAN_SEE);
                to = to_old;
            }
            continue;
        }
        if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT) {
            is_ok = FALSE;
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!is_ok) {
                /* Next if statement
                */
                if(is_fam) {
                    REMOVE_BIT(to->extra, EXTRA_CAN_SEE);
                    to = to_old;
                }
                continue;
            }
        }
        point	= buf;
        str	= format;
        while(*str != '\0') {
            if(*str != '$') {
                *point++ = *str++;
                continue;
            }
            ++str;
            if(arg2 == NULL && *str >= 'A' && *str <= 'Z') {
                bug("Act: missing arg2 for code %d.", *str);
                i = " <@@@> ";
            } else {
                switch(*str) {
                default:
                    bug("Act: bad code %d.", *str);
                    i = " <@@@> ";
                    break;
                /* Thx alex for 't' idea */
                case 't':
                    i = (char *) arg1;
                    break;
                case 'T':
                    i = (char *) arg2;
                    break;
                case 'n':
                    i = PERS(ch,  to);
                    break;
                case 'N':
                    i = PERS(vch, to);
                    break;
                case 'e':
                    i = he_she  [URANGE(0, ch  ->sex, 2)];
                    break;
                case 'E':
                    i = he_she  [URANGE(0, vch ->sex, 2)];
                    break;
                case 'm':
                    i = him_her [URANGE(0, ch  ->sex, 2)];
                    break;
                case 'M':
                    i = him_her [URANGE(0, vch ->sex, 2)];
                    break;
                case 's':
                    i = his_her [URANGE(0, ch  ->sex, 2)];
                    break;
                case 'S':
                    i = his_her [URANGE(0, vch ->sex, 2)];
                    break;
                case 'p':
                    i = can_see_obj(to, obj1)
                        ? ((obj1->chobj != NULL && obj1->chobj == to)
                           ? "you" : obj1->short_descr)
                        : "something";
                    break;
                case 'P':
                    i = can_see_obj(to, obj2)
                        ? ((obj2->chobj != NULL && obj2->chobj == to)
                           ? "you" : obj2->short_descr)
                        : "something";
                    break;
                case 'd':
                    if(arg2 == NULL || ((char *) arg2)[0] == '\0') {
                        i = "door";
                    } else {
                        one_argument((char *) arg2, fname);
                        i = fname;
                    }
                    break;
                }
            }
            ++str;
            while((*point = *i) != '\0') {
                ++point, ++i;
            }
        }
        *point++ = '\n';
        *point++ = '\r';
        /* Next if statement
        */
        if(is_fam) {
            if(to->in_room != ch->in_room &&
                    familiar != NULL && familiar->in_room == ch->in_room) {
                send_to_char("[ ", to);
            } else {
                REMOVE_BIT(to->extra, EXTRA_CAN_SEE);
                to = to_old;
                continue;
            }
        }
        buf[0]   = UPPER(buf[0]);
        write_to_buffer(to->desc, buf, point - buf);
        /* Next if statement
        */
        if(is_fam) {
            REMOVE_BIT(to->extra, EXTRA_CAN_SEE);
            to = to_old;
        }
    }
    return;
}

void act2(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type) {
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *to;
    /*
        CHAR_DATA *to_old;
    */
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    /*
        CHAR_DATA *familiar = NULL;
        CHAR_DATA *wizard = NULL;
    */
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;
    /*
        bool is_fam;
    */
    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if(format == NULL || format[0] == '\0') {
        return;
    }
    to = ch->in_room->people;
    if(type == TO_VICT) {
        if(vch == NULL || vch->in_room == NULL) {
            bug("Act: null vch with TO_VICT.", 0);
            return;
        }
        to = vch->in_room->people;
    }
    for(; to != NULL; to = to->next_in_room) {
        /*
        	is_fam = FALSE;
        	to_old = to;
        */
        if(type == TO_CHAR && to != ch) {
            continue;
        }
        if(type == TO_VICT && (to != vch || to == ch)) {
            continue;
        }
        if(type == TO_ROOM && to == ch) {
            continue;
        }
        if(type == TO_NOTVICT && (to == ch || to == vch)) {
            continue;
        }
        /*
        	if ( to->desc == NULL && IS_NPC(to) && (wizard = to->wizard) != NULL )
        	{
        	    if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != NULL)
        		&& familiar == to)
        	    {
        		if (to->in_room == ch->in_room &&
        		    wizard->in_room != to->in_room)
        		{
        		    to = wizard;
        		    is_fam = TRUE;
        		}
        	    }
        	}
        */
        if(to->desc == NULL || !IS_AWAKE(to)) {
            /*
            	    if (is_fam) to = to_old;
            */
            continue;
        }
        if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT) {
            is_ok = FALSE;
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!is_ok) {
                /*
                		if (is_fam) to = to_old;
                */
                continue;
            }
        }
        point	= buf;
        str	= format;
        while(*str != '\0') {
            if(*str != '$') {
                *point++ = *str++;
                continue;
            }
            ++str;
            if(arg2 == NULL && *str >= 'A' && *str <= 'Z') {
                /*
                		bug( "Act: missing arg2 for code %d.", *str );
                */
                i = " <@@@> ";
            } else {
                switch(*str) {
                default:
                    i = " ";
                    break;
                case 'n':
                    if(ch != NULL) {
                        i = PERS(ch,  to);
                    } else {
                        i = " ";
                    }
                    break;
                case 'N':
                    if(vch != NULL) {
                        i = PERS(vch,  to);
                    } else {
                        i = " ";
                    }
                    break;
                case 'e':
                    if(ch != NULL) {
                        i = he_she  [URANGE(0, ch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 'E':
                    if(vch != NULL) {
                        i = he_she  [URANGE(0, vch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 'm':
                    if(ch != NULL) {
                        i = him_her [URANGE(0, ch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 'M':
                    if(vch != NULL) {
                        i = him_her [URANGE(0, vch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 's':
                    if(ch != NULL) {
                        i = his_her [URANGE(0, ch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 'S':
                    if(vch != NULL) {
                        i = his_her [URANGE(0, vch  ->sex, 2)];
                    } else {
                        i = " ";
                    }
                    break;
                case 'p':
                    if(obj1 != NULL) {
                        i = can_see_obj(to, obj1)
                            ? ((obj1->chobj != NULL && obj1->chobj == to)
                               ? "you" : obj1->short_descr)
                            : "something";
                    } else {
                        i = " ";
                    }
                    break;
                case 'P':
                    if(obj2 != NULL) {
                        i = can_see_obj(to, obj2)
                            ? ((obj2->chobj != NULL && obj2->chobj == to)
                               ? "you" : obj2->short_descr)
                            : "something";
                    } else {
                        i = " ";
                    }
                    break;
                }
            }
            ++str;
            while((*point = *i) != '\0') {
                ++point, ++i;
            }
        }
        *point++ = '\n';
        *point++ = '\r';
        /*
        	if (is_fam)
        	{
        	    if (to->in_room != ch->in_room && familiar != NULL &&
        		familiar->in_room == ch->in_room)
        		send_to_char("[ ", to);
        	    else
        	    {
        		to = to_old;
        		continue;
        	    }
        	}
        */
        buf[0]   = UPPER(buf[0]);
        write_to_buffer(to->desc, buf, point - buf);
        /*
        	if (is_fam) to = to_old;
        */
    }
    return;
}



void kavitem(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type) {
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };
    char buf[MAX_STRING_LENGTH];
    char kav[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    const char *str;
    const char *i;
    char *point;
    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if(format == NULL || format[0] == '\0') {
        return;
    }
    to = ch->in_room->people;
    if(type == TO_VICT) {
        if(vch == NULL) {
            bug("Act: null vch with TO_VICT.", 0);
            return;
        }
        to = vch->in_room->people;
    }
    for(; to != NULL; to = to->next_in_room) {
        if(to->desc == NULL || !IS_AWAKE(to)) {
            continue;
        }
        if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT) {
            is_ok = FALSE;
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_room != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_room != NULL &&
                    ch->in_room == to->in_room) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                    ch->pcdata->chobj->in_obj != NULL &&
                    !IS_NPC(to) && to->pcdata->chobj != NULL &&
                    to->pcdata->chobj->in_obj != NULL &&
                    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj) {
                is_ok = TRUE;
            } else {
                is_ok = FALSE;
            }
            if(!is_ok) {
                continue;
            }
        }
        if(type == TO_CHAR && to != ch) {
            continue;
        }
        if(type == TO_VICT && (to != vch || to == ch)) {
            continue;
        }
        if(type == TO_ROOM && to == ch) {
            continue;
        }
        if(type == TO_NOTVICT && (to == ch || to == vch)) {
            continue;
        }
        point	= buf;
        str	= format;
        while(*str != '\0') {
            if(*str != '$') {
                *point++ = *str++;
                continue;
            }
            ++str;
            if(arg2 == NULL && *str >= 'A' && *str <= 'Z') {
                i = "";
            } else {
                switch(*str) {
                default:
                    i = "";
                    break;
                case 'n':
                    i = PERS(ch,  to);
                    break;
                case 'e':
                    i = he_she  [URANGE(0, ch  ->sex, 2)];
                    break;
                case 'm':
                    i = him_her [URANGE(0, ch  ->sex, 2)];
                    break;
                case 's':
                    i = his_her [URANGE(0, ch  ->sex, 2)];
                    break;
                case 'p':
                    i = can_see_obj(to, obj1)
                        ? ((obj1->chobj != NULL && obj1->chobj == to)
                           ? "you" : obj1->short_descr)
                        : "something";
                    break;
                case 'o':
                    if(obj1 != NULL) {
                        sprintf(kav, "%s's", obj1->short_descr);
                    }
                    i = can_see_obj(to, obj1)
                        ? ((obj1->chobj != NULL && obj1->chobj == to)
                           ? "your" : kav)
                        : "something's";
                    break;
                }
            }
            ++str;
            while((*point = *i) != '\0') {
                ++point, ++i;
            }
        }
        *point++ = '\n';
        *point++ = '\r';
        buf[0]   = UPPER(buf[0]);
        write_to_buffer(to->desc, buf, point - buf);
    }
    return;
}
