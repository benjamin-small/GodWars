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

/*
 * Local functions.
 */
void	special_hurl	args((CHAR_DATA *ch, CHAR_DATA *victim));




void improve_wpn(CHAR_DATA *ch, int dtype, bool right_hand) {
    OBJ_DATA *wield = NULL;
    char bufskill [20];
    char buf [MAX_INPUT_LENGTH];
    int dice1;
    int dice2;
    int trapper;
    int max_skl = 200;
    bool got_weapon = FALSE;
    dice1 = number_percent();
    dice2 = number_percent();
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_HIGHLANDER(ch)) {
        max_skl = 1000;
    }
    /*
        if (right_hand) wield = get_eq_char( ch, WEAR_WIELD );
        else            wield = get_eq_char( ch, WEAR_HOLD  );
    */
    if(dtype == TYPE_HIT) {
        got_weapon = FALSE;
    } else if(right_hand) {
        if((wield = get_eq_char(ch, WEAR_WIELD)) != NULL &&
                wield->item_type == ITEM_WEAPON) {
            got_weapon = TRUE;
        }
    } else {
        if((wield = get_eq_char(ch, WEAR_HOLD)) != NULL &&
                wield->item_type == ITEM_WEAPON) {
            got_weapon = TRUE;
        }
    }
    if(!got_weapon) {
        wield = NULL;
    }
    if(wield == NULL) {
        dtype = TYPE_HIT;
    }
    if(dtype == TYPE_UNDEFINED) {
        dtype = TYPE_HIT;
        if(wield != NULL && wield->item_type == ITEM_WEAPON) {
            dtype += wield->value[3];
        }
    }
    if(dtype < 1000 || dtype > 1012) {
        return;
    }
    dtype -= 1000;
    if(ch->wpn[dtype] >= max_skl) {
        return;
    }
    trapper = ch->wpn[dtype];
    if((dice1 > ch->wpn[dtype] && dice2 > ch->wpn[dtype]) ||
            (dice1 >= 99 || dice2 >= 99)) {
        ch->wpn[dtype] += 1;
    } else {
        return;
    }
    if(trapper == ch->wpn[dtype]) {
        return;
    }
    if(ch->wpn[dtype] == 1) {
        sprintf(bufskill, "slightly skilled");
    } else if(ch->wpn[dtype] == 26) {
        sprintf(bufskill, "reasonable");
    } else if(ch->wpn[dtype] == 51) {
        sprintf(bufskill, "fairly competent");
    } else if(ch->wpn[dtype] == 76) {
        sprintf(bufskill, "highly skilled");
    } else if(ch->wpn[dtype] == 101) {
        sprintf(bufskill, "very dangerous");
    } else if(ch->wpn[dtype] == 126) {
        sprintf(bufskill, "extremely deadly");
    } else if(ch->wpn[dtype] == 151) {
        sprintf(bufskill, "an expert");
    } else if(ch->wpn[dtype] == 176) {
        sprintf(bufskill, "a master");
    } else if(ch->wpn[dtype] == 200) {
        sprintf(bufskill, "a grand master");
    } else if(ch->wpn[dtype] == 201) {
        sprintf(bufskill, "supremely skilled");
    } else {
        return;
    }
    if(wield == NULL || dtype == 0) {
        sprintf(buf, "You are now %s at unarmed combat.\n\r", bufskill);
    } else {
        sprintf(buf, "You are now %s with %s.\n\r", bufskill, wield->short_descr);
    }
    ADD_COLOUR(ch, buf, WHITE);
    send_to_char(buf, ch);
    if(ch->wpn[dtype] == 200) {
        ch->exp += 10000;
    }
    return;
}

void improve_stance(CHAR_DATA *ch) {
    char buf        [MAX_INPUT_LENGTH];
    char bufskill   [25];
    char stancename [10];
    int dice1, dice2, stance, max;
    dice1 = number_percent();
    dice2 = number_percent();
    if(IS_NPC(ch)) {
        return;
    }
    switch(stance = ch->stance[0]) {
    default:
        return;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        max = 98;
        break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        max = 99;
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        max = 100;
        break;
        break;
    }
    /* Just in case of class changes, etc */
    if(ch->stance[stance] >= 200) {
        ch->stance[stance] = 200;
        return;
    }
    if((dice1 > ch->stance[stance] && dice2 > ch->stance[stance]) ||
            (dice1 >= max || dice2 >= max)) {
        ch->stance[stance] += 1;
    } else {
        return;
    }
    if(stance == ch->stance[stance]) {
        return;
    }
    if(ch->stance[stance] == 1) {
        sprintf(bufskill, "an apprentice of");
    } else if(ch->stance[stance] == 26) {
        sprintf(bufskill, "a trainee of");
    } else if(ch->stance[stance] == 51) {
        sprintf(bufskill, "a student of");
    } else if(ch->stance[stance] == 76) {
        sprintf(bufskill, "fairly experienced in");
    } else if(ch->stance[stance] == 101) {
        sprintf(bufskill, "well trained in");
    } else if(ch->stance[stance] == 126) {
        sprintf(bufskill, "highly skilled in");
    } else if(ch->stance[stance] == 151) {
        sprintf(bufskill, "an expert of");
    } else if(ch->stance[stance] == 176) {
        sprintf(bufskill, "a master of");
    } else if(ch->stance[stance] == 200) {
        sprintf(bufskill, "a grand master of");
    } else {
        return;
    }
    if(stance == STANCE_VIPER) {
        sprintf(stancename, "viper");
    } else if(stance == STANCE_CRANE) {
        sprintf(stancename, "crane");
    } else if(stance == STANCE_CRAB) {
        sprintf(stancename, "crab");
    } else if(stance == STANCE_MONGOOSE) {
        sprintf(stancename, "mongoose");
    } else if(stance == STANCE_BULL) {
        sprintf(stancename, "bull");
    } else if(stance == STANCE_MANTIS) {
        sprintf(stancename, "mantis");
    } else if(stance == STANCE_DRAGON) {
        sprintf(stancename, "dragon");
    } else if(stance == STANCE_TIGER) {
        sprintf(stancename, "tiger");
    } else if(stance == STANCE_MONKEY) {
        sprintf(stancename, "monkey");
    } else if(stance == STANCE_SWALLOW) {
        sprintf(stancename, "swallow");
    } else if(!IS_NPC(ch) && stance >= 11) {
        if(strlen(ch->pcdata->stancename) < 2) {
            return;
        }
        strcpy(stancename, ch->pcdata->stancename);
    } else {
        return;
    }
    sprintf(buf, "You are now %s the %s stance.\n\r", bufskill, stancename);
    ADD_COLOUR(ch, buf, WHITE);
    send_to_char(buf, ch);
    if(ch->stance[stance] == 200) {
        if(stance >= STANCE_MANTIS) {
            ch->exp += 20000;
        } else {
            ch->exp += 10000;
        }
    }
    return;
}

void do_alignment(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_DEMON(ch)) {
        send_to_char("You are unable to change your alignment.\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Please enter 'good', 'neutral', or 'evil'.\n\r", ch);
        return;
    }
    if(!str_cmp(arg, "good")) {
        ch->alignment = 1000;
        send_to_char("Alignment is now angelic.\n\r", ch);
    } else if(!str_cmp(arg, "neutral")) {
        ch->alignment = 0;
        send_to_char("Alignment is now neutral.\n\r", ch);
    } else if(!str_cmp(arg, "evil")) {
        ch->alignment = -1000;
        send_to_char("Alignment is now satanic.\n\r", ch);
    } else {
        send_to_char("Please enter 'good', 'neutral', or 'evil'.\n\r", ch);
    }
    return;
}

void do_side(CHAR_DATA *ch, char *argument) {
    char buf[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(!IS_VAMPIRE(ch)) {
        send_to_char("Huh?\n\r", ch);
        return;
    }
    if(strlen(ch->side) > 1) {
        send_to_char("But you have already chosen a sect!\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Please choose a sect: Camarilla, Sabbat, Anarch.\n\r", ch);
        return;
    }
    if(!str_cmp(arg, "anarch")) {
        if(strlen(ch->clan) > 1) {
            if(!str_cmp(ch->clan, "tzimisce")) {
                send_to_char("Your kind are not welcome among the Anarchs.\n\r", ch);
                return;
            }
        }
        free_string(ch->side);
        ch->side = str_dup("Anarch");
        send_to_char("You are now an Anarch.\n\r", ch);
        sprintf(buf, "%s has sided with the Anarchs!", ch->name);
        do_info(ch, buf);
        return;
    } else if(!str_cmp(arg, "camarilla")) {
        if(strlen(ch->clan) > 1) {
            if(!str_cmp(ch->clan, "baali") || !str_cmp(ch->clan, "tzimisce")) {
                send_to_char("Your kind are not welcome in the Camarilla.\n\r", ch);
                return;
            }
            if(!str_cmp(ch->clan, "lasombra")) {
                SET_BIT(ch->more, MORE_ANTITRIBU);
            }
        }
        free_string(ch->side);
        ch->side = str_dup("Camarilla");
        send_to_char("You are now a member of the Camarilla.\n\r", ch);
        sprintf(buf, "%s has sided with the Camarilla!", ch->name);
        do_info(ch, buf);
        return;
    } else if(!str_cmp(arg, "sabbat")) {
        if(strlen(ch->clan) > 1) {
            if(!str_cmp(ch->clan, "baali")) {
                send_to_char("Your kind are not welcome in the Sabbat.\n\r", ch);
                return;
            }
            if(!str_cmp(ch->clan, "brujah") || !str_cmp(ch->clan, "gangrel") ||
                    !str_cmp(ch->clan, "malkavian") || !str_cmp(ch->clan, "nosferatu") ||
                    !str_cmp(ch->clan, "toreador") || !str_cmp(ch->clan, "tremere") ||
                    !str_cmp(ch->clan, "ventrue") || !str_cmp(ch->clan, "assamite")) {
                SET_BIT(ch->more, MORE_ANTITRIBU);
            }
        }
        free_string(ch->side);
        ch->side = str_dup("Sabbat");
        send_to_char("You are now a member of the Sabbat.\n\r", ch);
        sprintf(buf, "%s has sided with the Sabbat!", ch->name);
        do_info(ch, buf);
        return;
    } else {
        send_to_char("Please choose a sect: Camarilla, Sabbat, Anarch.\n\r", ch);
    }
    return;
}

void do_skill(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    char buf       [MAX_STRING_LENGTH];
    char buf2      [MAX_STRING_LENGTH];
    char bufskill  [25];
    char bufskill2 [25];
    char arg       [MAX_INPUT_LENGTH];
    OBJ_DATA *wield;
    OBJ_DATA *wield2;
    int dtype;
    int dtype2;
    int con_skill;
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(arg[0] == '\0') {
        sprintf(arg, ch->name);
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    /*
        if (IS_NPC(victim))
        {
    	send_to_char( "Not on NPC's.\n\r", ch );
    	return;
        }
    */
    wield = get_eq_char(victim, WEAR_WIELD);
    wield2 = get_eq_char(victim, WEAR_HOLD);
    dtype = TYPE_HIT;
    dtype2 = TYPE_HIT;
    if(wield != NULL && wield->item_type == ITEM_WEAPON) {
        dtype += wield->value[3];
    }
    if(wield2 != NULL && wield2->item_type == ITEM_WEAPON) {
        dtype2 += wield2->value[3];
    }
    dtype -= 1000;
    dtype2 -= 1000;
    if(!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_FAKE_CON)) {
        con_skill = victim->pcdata->fake_skill;
    } else {
        con_skill = victim->wpn[dtype];
    }
    if(con_skill <= 0) {
        sprintf(bufskill, "totally unskilled");
    } else if(con_skill <= 25) {
        sprintf(bufskill, "slightly skilled");
    } else if(con_skill <= 50) {
        sprintf(bufskill, "reasonable");
    } else if(con_skill <= 75) {
        sprintf(bufskill, "fairly competent");
    } else if(con_skill <= 100) {
        sprintf(bufskill, "highly skilled");
    } else if(con_skill <= 125) {
        sprintf(bufskill, "very dangerous");
    } else if(con_skill <= 150) {
        sprintf(bufskill, "extremely deadly");
    } else if(con_skill <= 175) {
        sprintf(bufskill, "an expert");
    } else if(con_skill <= 199) {
        sprintf(bufskill, "a master");
    } else if(con_skill == 200) {
        sprintf(bufskill, "a grand master");
    } else if(con_skill <= 1000) {
        sprintf(bufskill, "supremely skilled");
    } else {
        return;
    }
    if(!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_FAKE_CON)) {
        con_skill = victim->pcdata->fake_skill;
    } else {
        con_skill = victim->wpn[dtype2];
    }
    if(con_skill <= 0) {
        sprintf(bufskill2, "totally unskilled");
    } else if(con_skill <= 25) {
        sprintf(bufskill2, "slightly skilled");
    } else if(con_skill <= 50) {
        sprintf(bufskill2, "reasonable");
    } else if(con_skill <= 75) {
        sprintf(bufskill2, "fairly competent");
    } else if(con_skill <= 100) {
        sprintf(bufskill2, "highly skilled");
    } else if(con_skill <= 125) {
        sprintf(bufskill2, "very dangerous");
    } else if(con_skill <= 150) {
        sprintf(bufskill2, "extremely deadly");
    } else if(con_skill <= 175) {
        sprintf(bufskill2, "an expert");
    } else if(con_skill <= 199) {
        sprintf(bufskill2, "a master");
    } else if(con_skill == 200) {
        sprintf(bufskill2, "a grand master");
    } else if(con_skill <= 1000) {
        sprintf(bufskill2, "supremely skilled");
    } else {
        return;
    }
    if(ch == victim) {
        if(dtype == 0 && dtype2 == 0) {
            sprintf(buf, "You are %s at unarmed combat.", bufskill);
        } else {
            if(dtype != 0) {
                sprintf(buf, "You are %s with %s.", bufskill, wield->short_descr);
            }
            if(dtype2 != 0) {
                sprintf(buf2, "You are %s with %s.", bufskill2, wield2->short_descr);
            }
        }
    } else {
        if(dtype == 0 && dtype2 == 0) {
            sprintf(buf, "$N is %s at unarmed combat.", bufskill);
        } else {
            if(dtype != 0) {
                sprintf(buf, "$N is %s with %s.", bufskill, wield->short_descr);
            }
            if(dtype2 != 0) {
                sprintf(buf2, "$N is %s with %s.", bufskill2, wield2->short_descr);
            }
        }
    }
    if(!(dtype == 0 && dtype2 != 0)) {
        act(buf, ch, NULL, victim, TO_CHAR);
    }
    if(dtype2 != 0) {
        act(buf2, ch, NULL, victim, TO_CHAR);
    }
    skillstance(ch, victim);
    return;
}

void do_throw(CHAR_DATA *ch, char *argument) {
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *victim = NULL;
    OBJ_DATA        *obj;
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    char            buf       [MAX_INPUT_LENGTH];
    char            revdoor   [MAX_INPUT_LENGTH];
    int             door;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
        if((obj = get_eq_char(ch, WEAR_HOLD)) == NULL) {
            send_to_char("You are not holding anything to throw.\n\r", ch);
            return;
        }
    }
    if(arg1[0] == '\0') {
        send_to_char("Which direction do you wish to throw?\n\r", ch);
        return;
    }
    if(!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) {
        door = 0;
        sprintf(arg1, "north");
    } else if(!str_cmp(arg1, "e") || !str_cmp(arg1, "east")) {
        door = 1;
        sprintf(arg1, "east");
    } else if(!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) {
        door = 2;
        sprintf(arg1, "south");
    } else if(!str_cmp(arg1, "w") || !str_cmp(arg1, "west")) {
        door = 3;
        sprintf(arg1, "west");
    } else if(!str_cmp(arg1, "u") || !str_cmp(arg1, "up")) {
        door = 4;
        sprintf(arg1, "up");
    } else if(!str_cmp(arg1, "d") || !str_cmp(arg1, "down")) {
        door = 5;
        sprintf(arg1, "down");
    } else {
        send_to_char("You can only throw north, south, east, west, up or down.\n\r", ch);
        return;
    }
    if(!str_cmp(arg1, "north")) {
        door = 0;
        sprintf(revdoor, "south");
    } else if(!str_cmp(arg1, "east")) {
        door = 1;
        sprintf(revdoor, "west");
    } else if(!str_cmp(arg1, "south")) {
        door = 2;
        sprintf(revdoor, "north");
    } else if(!str_cmp(arg1, "west")) {
        door = 3;
        sprintf(revdoor, "east");
    } else if(!str_cmp(arg1, "up")) {
        door = 4;
        sprintf(revdoor, "down");
    } else if(!str_cmp(arg1, "down")) {
        door = 5;
        sprintf(revdoor, "up");
    } else {
        return;
    }
    location = ch->in_room;
    sprintf(buf, "You hurl $p %s.", arg1);
    act(buf, ch, obj, NULL, TO_CHAR);
    sprintf(buf, "$n hurls $p %s.", arg1);
    act(buf, ch, obj, NULL, TO_ROOM);
    /* First room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p bounces off the %s wall.", arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        act(buf, ch, obj, NULL, TO_CHAR);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p bounces off the %s door.", arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        act(buf, ch, obj, NULL, TO_CHAR);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        return;
    }
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        sprintf(buf, "$p comes flying in from the %s and lands in $N's hands.", revdoor);
        act(buf, ch, obj, victim, TO_NOTVICT);
        sprintf(buf, "$p comes flying in from the %s and lands in your hands.", revdoor);
        act(buf, ch, obj, victim, TO_VICT);
        obj_from_char(obj);
        obj_to_char(obj, victim);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    /* Second room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p comes flying in from the %s and strikes %s wall.", revdoor, arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p comes flying in from the %s and strikes the %s door.", revdoor, arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    sprintf(buf, "$p comes flying in from the %s and carries on %s.", revdoor, arg1);
    act(buf, ch, obj, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        sprintf(buf, "$p comes flying in from the %s and lands in $N's hands.", revdoor);
        act(buf, ch, obj, victim, TO_NOTVICT);
        sprintf(buf, "$p comes flying in from the %s and lands in your hands.", revdoor);
        act(buf, ch, obj, victim, TO_VICT);
        obj_from_char(obj);
        obj_to_char(obj, victim);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    /* Third room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p comes flying in from the %s and strikes %s wall.", revdoor, arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p comes flying in from the %s and strikes the %s door.", revdoor, arg1);
        act(buf, ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    sprintf(buf, "$p comes flying in from the %s and carries on %s.", revdoor, arg1);
    act(buf, ch, obj, NULL, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        sprintf(buf, "$p comes flying in from the %s and lands in $N's hands.", revdoor);
        act(buf, ch, obj, victim, TO_NOTVICT);
        sprintf(buf, "$p comes flying in from the %s and lands in your hands.", revdoor);
        act(buf, ch, obj, victim, TO_VICT);
        obj_from_char(obj);
        obj_to_char(obj, victim);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    sprintf(buf, "$p comes flying in from the %s and drops at your feet.", revdoor);
    act(buf, ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
    /* Move them back */
    char_from_room(ch);
    char_to_room(ch, location);
    do_autosave(ch, "");
    if(victim != NULL && !IS_NPC(victim)) {
        do_autosave(victim, "");
    }
    return;
}

void do_shoot(CHAR_DATA *ch, char *argument) {
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *victim;
    OBJ_DATA        *obj;
    OBJ_DATA        *ammo;
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    char            buf       [MAX_INPUT_LENGTH];
    char            revdoor   [MAX_INPUT_LENGTH];
    int             door;
    bool	    missed;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
            || obj->item_type != ITEM_MISSILE) {
        if((obj = get_eq_char(ch, WEAR_HOLD)) == NULL
                || obj->item_type != ITEM_MISSILE) {
            send_to_char("You are not holding anything to shoot.\n\r", ch);
            return;
        }
    }
    if(obj->value[1] < 1) {
        send_to_char("First you need to load it.\n\r", ch);
        return;
    }
    if(arg1[0] == '\0') {
        send_to_char("Please specify a direction to shoot.\n\r", ch);
        return;
    }
    if(arg2[0] == '\0') {
        send_to_char("Please specify a target to shoot at.\n\r", ch);
        return;
    }
    if(!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) {
        door = 0;
        sprintf(arg1, "north");
    } else if(!str_cmp(arg1, "e") || !str_cmp(arg1, "east")) {
        door = 1;
        sprintf(arg1, "east");
    } else if(!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) {
        door = 2;
        sprintf(arg1, "south");
    } else if(!str_cmp(arg1, "w") || !str_cmp(arg1, "west")) {
        door = 3;
        sprintf(arg1, "west");
    } else if(!str_cmp(arg1, "u") || !str_cmp(arg1, "up")) {
        door = 4;
        sprintf(arg1, "up");
    } else if(!str_cmp(arg1, "d") || !str_cmp(arg1, "down")) {
        door = 5;
        sprintf(arg1, "down");
    } else {
        send_to_char("You can only shoot north, south, east, west, up or down.\n\r", ch);
        return;
    }
    if(!str_cmp(arg1, "north")) {
        door = 0;
        sprintf(revdoor, "south");
    } else if(!str_cmp(arg1, "east")) {
        door = 1;
        sprintf(revdoor, "west");
    } else if(!str_cmp(arg1, "south")) {
        door = 2;
        sprintf(revdoor, "north");
    } else if(!str_cmp(arg1, "west")) {
        door = 3;
        sprintf(revdoor, "east");
    } else if(!str_cmp(arg1, "up")) {
        door = 4;
        sprintf(revdoor, "down");
    } else if(!str_cmp(arg1, "down")) {
        door = 5;
        sprintf(revdoor, "up");
    } else {
        return;
    }
    if(get_obj_index(obj->value[0]) == NULL) {
        return;
    }
    location = ch->in_room;
    ammo = create_object(get_obj_index(obj->value[0]), 0);
    if(number_percent() <= ammo->value[0]) {
        missed = FALSE;
    } else {
        missed = TRUE;
    }
    obj->value[1] -= 1;
    sprintf(buf, "You point $p %s and shoot.", arg1);
    act(buf, ch, obj, NULL, TO_CHAR);
    sprintf(buf, "$n points $p %s and shoots.", arg1);
    act(buf, ch, obj, NULL, TO_ROOM);
    /* First room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p bounces off the %s wall.", arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        act(buf, ch, ammo, NULL, TO_CHAR);
        obj_to_room(ammo, ch->in_room);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p bounces off the %s door.", arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        act(buf, ch, ammo, NULL, TO_CHAR);
        obj_to_room(ammo, ch->in_room);
        return;
    }
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        if(missed) {
            sprintf(buf, "$p flies in from the %s, just missing $N, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p flies in from the %s, just missing you, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_VICT);
        } else {
            sprintf(buf, "$p comes flying in from the %s and strikes $N.", revdoor);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p comes flying in from the %s and strikes you.", revdoor);
            act(buf, ch, ammo, victim, TO_VICT);
            obj_to_char(ammo, victim);
            char_from_room(ch);
            char_to_room(ch, location);
            victim->hit -= number_range(ammo->value[1], ammo->value[2]);
            update_pos(victim);
            if(victim->position == POS_DEAD && !IS_HERO(victim)) {
                send_to_char("You have been KILLED!!\n\r", victim);
                victim->form = 19;
                raw_kill(victim, ch);
            }
            return;
        }
    }
    /* Second room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p comes flying in from the %s and strikes %s wall.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        obj_to_room(ammo, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p comes flying in from the %s and strikes the %s door.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        obj_to_room(ammo, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    if(!missed) {
        sprintf(buf, "$p comes flying in from the %s and carries on %s.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
    }
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        if(missed) {
            sprintf(buf, "$p flies in from the %s, just missing $N, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p flies in from the %s, just missing you, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_VICT);
        } else {
            sprintf(buf, "$p comes flying in from the %s and strikes $N.", revdoor);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p comes flying in from the %s and strikes you.", revdoor);
            act(buf, ch, ammo, victim, TO_VICT);
            obj_to_char(ammo, victim);
            char_from_room(ch);
            char_to_room(ch, location);
            victim->hit -= number_range(ammo->value[1], ammo->value[2]);
            update_pos(victim);
            if(victim->position == POS_DEAD && !IS_HERO(victim)) {
                send_to_char("You have been KILLED!!\n\r", victim);
                victim->form = 19;
                raw_kill(victim, ch);
            }
            return;
        }
    }
    /* Third room */
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$p comes flying in from the %s and strikes %s wall.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        obj_to_room(ammo, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    pexit = ch->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED)) {
        sprintf(buf, "$p comes flying in from the %s and strikes the %s door.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
        obj_to_room(ammo, ch->in_room);
        char_from_room(ch);
        char_to_room(ch, location);
        return;
    }
    if(!missed) {
        sprintf(buf, "$p comes flying in from the %s and carries on %s.", revdoor, arg1);
        act(buf, ch, ammo, NULL, TO_ROOM);
    }
    char_from_room(ch);
    char_to_room(ch, to_room);
    if((victim = get_char_room(ch, arg2)) != NULL) {
        if(missed) {
            sprintf(buf, "$p flies in from the %s, just missing $N, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p flies in from the %s, just missing you, and carrying on %s.", revdoor, arg1);
            act(buf, ch, ammo, victim, TO_VICT);
        } else {
            sprintf(buf, "$p comes flying in from the %s and strikes $N.", revdoor);
            act(buf, ch, ammo, victim, TO_NOTVICT);
            sprintf(buf, "$p comes flying in from the %s and strikes you.", revdoor);
            act(buf, ch, ammo, victim, TO_VICT);
            obj_to_char(ammo, victim);
            char_from_room(ch);
            char_to_room(ch, location);
            victim->hit -= number_range(ammo->value[1], ammo->value[2]);
            update_pos(victim);
            if(victim->position == POS_DEAD && !IS_HERO(victim)) {
                send_to_char("You have been KILLED!!\n\r", victim);
                victim->form = 19;
                raw_kill(victim, ch);
            }
            return;
        }
    }
    sprintf(buf, "$p comes flying in from the %s and drops at your feet.", revdoor);
    act(buf, ch, ammo, NULL, TO_ROOM);
    obj_to_room(ammo, ch->in_room);
    /* Move them back */
    char_from_room(ch);
    char_to_room(ch, location);
    return;
}

void do_reload(CHAR_DATA *ch, char *argument) {
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    OBJ_DATA *ammo;
    OBJ_DATA *missile;
    obj = get_eq_char(ch, WEAR_WIELD);
    obj2 = get_eq_char(ch, WEAR_HOLD);
    if(obj == NULL || obj2 == NULL) {
        send_to_char("You must hold the weapon to reload in one hand and the ammo in the other.\n\r", ch);
        return;
    }
    if(!(obj->item_type  == ITEM_MISSILE && obj2->item_type == ITEM_AMMO)
            &&   !(obj2->item_type == ITEM_MISSILE && obj->item_type  == ITEM_AMMO)) {
        send_to_char("You must hold the weapon to reload in one hand and the ammo in the other.\n\r", ch);
        return;
    }
    if(obj->item_type == ITEM_MISSILE) {
        missile = obj;
        ammo = obj2;
    } else {
        missile = obj2;
        ammo = obj;
    }
    if(missile->value[1] >= missile->value[2]) {
        send_to_char("It is already fully loaded.\n\r", ch);
        return;
    }
    if(missile->value[3] != ammo->value[3]) {
        send_to_char("That weapon doesn't take that sort of ammo.\n\r", ch);
        return;
    }
    if(missile->value[0] != 0 && missile->value[1] != 0 &&
            missile->value[0] != ammo->pIndexData->vnum) {
        send_to_char("You cannot mix ammo types.\n\r", ch);
        return;
    }
    act("You load $p into $P.", ch, ammo, missile, TO_CHAR);
    act("$n loads $p into $P.", ch, ammo, missile, TO_ROOM);
    missile->value[1] += 1;
    missile->value[0] = ammo->pIndexData->vnum;
    obj_from_char(ammo);
    extract_obj(ammo);
    return;
}

void do_unload(CHAR_DATA *ch, char *argument) {
    OBJ_DATA *missile;
    OBJ_DATA *ammo;
    if((missile = get_eq_char(ch, WEAR_WIELD)) == NULL
            || missile->item_type != ITEM_MISSILE) {
        if((missile = get_eq_char(ch, WEAR_HOLD)) == NULL
                || missile->item_type != ITEM_MISSILE) {
            send_to_char("You are not holding anything to unload.\n\r", ch);
            return;
        }
    }
    if(missile->value[1] <= 0) {
        send_to_char("But it is already empty!\n\r", ch);
        return;
    }
    if(get_obj_index(missile->value[0]) == NULL) {
        return;
    }
    ammo = create_object(get_obj_index(missile->value[0]), 0);
    act("You remove $p from $P.", ch, ammo, missile, TO_CHAR);
    act("$n removes $p from $P.", ch, ammo, missile, TO_ROOM);
    missile->value[1] -= 1;
    obj_to_char(ammo, ch);
    return;
}

void do_stance(CHAR_DATA *ch, char *argument) {
    char arg      [MAX_INPUT_LENGTH];
    char buf      [MAX_STRING_LENGTH];
    char buf2     [MAX_STRING_LENGTH];
    char kav      [MAX_INPUT_LENGTH];
    int  selection;
    bool got_adv = FALSE;
    bool add_e = FALSE;
    argument = one_argument(argument, arg);
    if(IS_SET(ch->mounted, IS_RIDING)) {
        send_to_char("Not while mounted.\n\r", ch);
        return;
    }
    if(IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED)) {
        send_to_char("Not while polymorphed.\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        if(ch->stance[0] == -1) {
            ch->stance[0] = 0;
            send_to_char("You drop into a fighting stance.\n\r", ch);
            act("$n drops into a fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else {
            if(!IS_NPC(ch) && ch->stance[0] >= 11) {
                free_string(ch->pcdata->stancename);
                ch->pcdata->stancename = str_dup("");
            }
            ch->stance[0] = -1;
            send_to_char("You relax from your fighting stance.\n\r", ch);
            act("$n relaxes from $s fighting stance.", ch, NULL, NULL, TO_ROOM);
        }
        return;
    }
    if(ch->stance[0] > 0) {
        send_to_char("You cannot change stances until you come up from the one you are currently in.\n\r", ch);
        return;
    }
    if(!str_cmp(arg, "none")) {
        selection = STANCE_NONE;
        send_to_char("You drop into a general fighting stance.\n\r", ch);
        act("$n drops into a general fighting stance.", ch, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg, "viper")) {
        selection = STANCE_VIPER;
        send_to_char("You arch your body into the viper fighting stance.\n\r", ch);
        act("$n arches $s body into the viper fighting stance.", ch, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg, "crane")) {
        selection = STANCE_CRANE;
        send_to_char("You swing your body into the crane fighting stance.\n\r", ch);
        act("$n swings $s body into the crane fighting stance.", ch, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg, "crab")) {
        selection = STANCE_CRAB;
        send_to_char("You squat down into the crab fighting stance.\n\r", ch);
        act("$n squats down into the crab fighting stance. ", ch, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg, "mongoose")) {
        selection = STANCE_MONGOOSE;
        send_to_char("You twist into the mongoose fighting stance.\n\r", ch);
        act("$n twists into the mongoose fighting stance. ", ch, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg, "bull")) {
        selection = STANCE_BULL;
        send_to_char("You hunch down into the bull fighting stance.\n\r", ch);
        act("$n hunches down into the bull fighting stance. ", ch, NULL, NULL, TO_ROOM);
    } else {
        strcpy(buf2, "Advanced stances:");
        if(ch->stance[STANCE_CRANE] >= 200 && ch->stance[STANCE_VIPER] >= 200) {
            strcat(buf2, " Mantis");
            got_adv = TRUE;
        }
        if(ch->stance[STANCE_BULL] >= 200 && ch->stance[STANCE_CRAB] >= 200) {
            strcat(buf2, " Dragon");
            got_adv = TRUE;
        }
        if(ch->stance[STANCE_BULL] >= 200 && ch->stance[STANCE_VIPER] >= 200) {
            strcat(buf2, " Tiger");
            got_adv = TRUE;
        }
        if(ch->stance[STANCE_CRANE] >= 200 && ch->stance[STANCE_MONGOOSE] >= 200) {
            strcat(buf2, " Monkey");
            got_adv = TRUE;
        }
        if(ch->stance[STANCE_CRAB] >= 200 && ch->stance[STANCE_MONGOOSE] >= 200) {
            strcat(buf2, " Swallow");
            got_adv = TRUE;
        }
        strcat(buf2, ".\n\r");
        if(!str_cmp(arg, "mantis") && ch->stance[STANCE_CRANE] >= 200 &&
                ch->stance[STANCE_VIPER] >= 200) {
            selection = STANCE_MANTIS;
            send_to_char("You spin your body into the mantis fighting stance.\n\r", ch);
            act("$n spins $s body into the mantis fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else if(!str_cmp(arg, "dragon") && ch->stance[STANCE_BULL] >= 200 &&
                  ch->stance[STANCE_CRAB] >= 200) {
            selection = STANCE_DRAGON;
            send_to_char("You coil your body into the dragon fighting stance.\n\r", ch);
            act("$n coils $s body into the dragon fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else if(!str_cmp(arg, "tiger") && ch->stance[STANCE_BULL] >= 200 &&
                  ch->stance[STANCE_VIPER] >= 200) {
            selection = STANCE_TIGER;
            send_to_char("You lunge into the tiger fighting stance.\n\r", ch);
            act("$n lunges into the tiger fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else if(!str_cmp(arg, "monkey") && ch->stance[STANCE_CRANE] >= 200 &&
                  ch->stance[STANCE_MONGOOSE] >= 200) {
            selection = STANCE_MONKEY;
            send_to_char("You rotate your body into the monkey fighting stance.\n\r", ch);
            act("$n rotates $s body into the monkey fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else if(!str_cmp(arg, "swallow") && ch->stance[STANCE_CRAB] >= 200 &&
                  ch->stance[STANCE_MONGOOSE] >= 200) {
            selection = STANCE_SWALLOW;
            send_to_char("You slide into the swallow fighting stance.\n\r", ch);
            act("$n slides into the swallow fighting stance.", ch, NULL, NULL, TO_ROOM);
        } else if(!IS_NPC(ch)) {
            if(strlen(ch->pcdata->stancelist) > 1) {
                int namecount = loc_name(arg, ch->pcdata->stancelist);
                if(namecount >= 0 && namecount <= 4) {
                    if(!IS_SET(ch->pcdata->qstats[namecount], QS_DONE)) {
                        send_to_char("You haven't bought that stance yet!\n\r", ch);
                        return;
                    }
                    switch(ch->pcdata->stancemove[namecount]) {
                    default:
                        strcpy(kav, "drop");
                        break;
                    case 1:
                        strcpy(kav, "arch");
                        add_e = TRUE;
                        break;
                    case 2:
                        strcpy(kav, "swing");
                        break;
                    case 3:
                        strcpy(kav, "squat");
                        break;
                    case 4:
                        strcpy(kav, "twist");
                        break;
                    case 5:
                        strcpy(kav, "hunch");
                        add_e = TRUE;
                        break;
                    case 6:
                        strcpy(kav, "spin");
                        break;
                    case 7:
                        strcpy(kav, "coil");
                        break;
                    case 8:
                        strcpy(kav, "lunge");
                        break;
                    case 9:
                        strcpy(kav, "rotate");
                        break;
                    case 10:
                        strcpy(kav, "slide");
                        break;
                    case 11:
                        strcpy(kav, "twirl");
                        break;
                    case 12:
                        strcpy(kav, "leap");
                        break;
                    case 13:
                        strcpy(kav, "swoop");
                        break;
                    case 14:
                        strcpy(kav, "roll");
                        break;
                    case 15:
                        strcpy(kav, "wriggle");
                        break;
                    }
                    sprintf(buf, "You %s into the %s fighting stance.\n\r", kav, arg);
                    send_to_char(buf, ch);
                    sprintf(buf, "$n %s%ss into the %s fighting stance.",
                            kav, add_e ? "e" : "", arg);
                    act(buf, ch, NULL, NULL, TO_ROOM);
                    ch->stance[0] = namecount + 11;
                    if(!IS_NPC(ch) && ch->stance[0] >= 11) {
                        free_string(ch->pcdata->stancename);
                        ch->pcdata->stancename = str_dup(arg);
                    }
                    return;
                }
                send_to_char("Syntax is: stance <style>.\n\r", ch);
                send_to_char("Basic stances: Viper Crane Crab Mongoose Bull.\n\r", ch);
                if(got_adv) {
                    send_to_char(buf2, ch);
                }
                sprintf(buf, "Super stances: %s.\n\r", ch->pcdata->stancelist);
                send_to_char(buf, ch);
                return;
            }
            send_to_char("Syntax is: stance <style>.\n\r", ch);
            send_to_char("Basic stances: Viper Crane Crab Mongoose Bull.\n\r", ch);
            if(got_adv) {
                send_to_char(buf2, ch);
            }
            return;
        } else {
            send_to_char("Syntax is: stance <style>.\n\r", ch);
            send_to_char("Basic stances: Viper Crane Crab Mongoose Bull.\n\r", ch);
            if(got_adv) {
                send_to_char(buf2, ch);
            }
            return;
        }
    }
    ch->stance[0] = selection;
    WAIT_STATE(ch, 12);
    return;
}

void do_fightstyle(CHAR_DATA *ch, char *argument) {
    char            arg1     [MAX_INPUT_LENGTH];
    char            arg2     [MAX_INPUT_LENGTH];
    char            buf      [MAX_INPUT_LENGTH];
    int             selection;
    int             value;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(arg1[0] == '\0' || arg2[0] == '\0') {
        send_to_char("Syntax is: fightstyle <number> <style>.\n\r", ch);
        send_to_char("Style can be selected from the following (enter style in text form):\n\r", ch);
        send_to_char("[ 1]*Trip      [ 2]*Kick      [ 3] Bash      [ 4] Elbow     [ 5] Knee\n\r", ch);
        send_to_char("[ 6] Headbutt  [ 7]*Disarm    [ 8] Bite      [ 9]*Dirt      [10] Grapple\n\r", ch);
        send_to_char("[11] Punch     [12]*Gouge     [13] Rip       [14]*Stamp     [15] Backfist\n\r", ch);
        send_to_char("[16] Jumpkick  [17] Spinkick  [18] Hurl      [19] Sweep     [20] Charge\n\r", ch);
        sprintf(buf, "Selected options: 1:[%d] 2:[%d] 3:[%d] 4:[%d] 5:[%d] 6:[%d] 7:[%d] 8:[%d].\n\r", ch->cmbt[0], ch->cmbt[1],
                ch->cmbt[2], ch->cmbt[3], ch->cmbt[4], ch->cmbt[5], ch->cmbt[6], ch->cmbt[7]);
        send_to_char(buf, ch);
        send_to_char("\n\r* This has been coded (others are not yet in).\n\r", ch);
        return;
    }
    value = is_number(arg1) ? atoi(arg1) : -1;
    if(value < 1 || value > 8) {
        send_to_char("Please enter a value between 1 and 8.\n\r", ch);
        return;
    }
    if(!str_cmp(arg2, "clear")) {
        selection = 0;
    } else if(!str_cmp(arg2, "trip")) {
        selection = 1;
    } else if(!str_cmp(arg2, "kick")) {
        selection = 2;
    } else if(!str_cmp(arg2, "bash")) {
        selection = 3;
    } else if(!str_cmp(arg2, "elbow")) {
        selection = 4;
    } else if(!str_cmp(arg2, "knee")) {
        selection = 5;
    } else if(!str_cmp(arg2, "headbutt")) {
        selection = 6;
    } else if(!str_cmp(arg2, "disarm")) {
        selection = 7;
    } else if(!str_cmp(arg2, "bite")) {
        selection = 8;
    } else if(!str_cmp(arg2, "dirt")) {
        selection = 9;
    } else if(!str_cmp(arg2, "grapple")) {
        selection = 10;
    } else if(!str_cmp(arg2, "punch")) {
        selection = 11;
    } else if(!str_cmp(arg2, "gouge")) {
        selection = 12;
    } else if(!str_cmp(arg2, "rip")) {
        selection = 13;
    } else if(!str_cmp(arg2, "stamp")) {
        selection = 14;
    } else if(!str_cmp(arg2, "backfist")) {
        selection = 15;
    } else if(!str_cmp(arg2, "jumpkick")) {
        selection = 16;
    } else if(!str_cmp(arg2, "spinkick")) {
        selection = 17;
    } else if(!str_cmp(arg2, "hurl")) {
        selection = 18;
    } else if(!str_cmp(arg2, "sweep")) {
        selection = 19;
    } else if(!str_cmp(arg2, "charge")) {
        selection = 20;
    } else {
        send_to_char("Syntax is: fightstyle <number> <style>.\n\r", ch);
        send_to_char("Style can be selected from the following (enter style in text form):\n\r", ch);
        send_to_char("[ 1]*Trip      [ 2]*Kick      [ 3] Bash      [ 4] Elbow     [ 5] Knee\n\r", ch);
        send_to_char("[ 6] Headbutt  [ 7]*Disarm    [ 8] Bite      [ 9]*Dirt      [10] Grapple\n\r", ch);
        send_to_char("[11] Punch     [12]*Gouge     [13] Rip       [14]*Stamp     [15] Backfist\n\r", ch);
        send_to_char("[16] Jumpkick  [17] Spinkick  [18] Hurl      [19] Sweep     [20] Charge\n\r", ch);
        sprintf(buf, "Selected options: 1:[%d] 2:[%d] 3:[%d] 4:[%d] 5:[%d] 6:[%d] 7:[%d] 8:[%d].\n\r", ch->cmbt[0], ch->cmbt[1],
                ch->cmbt[2], ch->cmbt[3], ch->cmbt[4], ch->cmbt[5], ch->cmbt[6], ch->cmbt[7]);
        send_to_char(buf, ch);
        send_to_char("\n\r* This has been coded (others are not yet in).\n\r", ch);
        return;
    }
    ch->cmbt[(value - 1)] = selection;
    sprintf(buf, "Combat option %d now set to %s (%d).\n\r", value, arg2, ch->cmbt[0]);
    send_to_char(buf, ch);
    return;
}

void fightaction(CHAR_DATA *ch, CHAR_DATA *victim, int actype, int dtype, int wpntype) {
    AFFECT_DATA af;
    /*
        char            buf      [MAX_INPUT_LENGTH]; */
    if(IS_NPC(ch)) {
        return;
    }
    /*  if ( IS_AFFECTED(ch,AFF_POLYMORPH) ) return; *** Just in case...KaVir */
    if(actype < 1 || actype > 20) {
        return;
    }
    if(victim == NULL) {
        return;
    }
    /* Trip */
    if(actype == 1) {
        if(number_percent() <= ch->wpn[0]) {
            trip(ch, victim);
        }
        return;
    } else if(actype == 2 && number_percent() < 75) {
        do_kick(ch, "");
        return;
    } else if(actype == 7) {
        if(number_percent() < 25) {
            disarm(ch, victim);
        }
        return;
    } else if(actype == 9) {
        act("You kick a spray of dirt into $N's face.", ch, NULL, victim, TO_CHAR);
        act("$n kicks a spray of dirt into your face.", ch, NULL, victim, TO_VICT);
        act("$n kicks a spray of dirt into $N's face.", ch, NULL, victim, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_BLIND) || number_percent() < 25) {
            return;
        }
        af.type      = 4;
        af.location  = APPLY_HITROLL;
        af.modifier  = -4;
        af.duration  = 1;
        af.bitvector = AFF_BLIND;
        affect_to_char(victim, &af);
        act("$n is blinded!", victim, NULL, NULL, TO_ROOM);
        send_to_char("You are blinded!\n\r", victim);
        return;
    } else if(actype == 12) {
        act("You gouge your fingers into $N's eyes.", ch, NULL, victim, TO_CHAR);
        act("$n gouges $s fingers into your eyes.", ch, NULL, victim, TO_VICT);
        act("$n gouges $s fingers into $N's eyes.", ch, NULL, victim, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_BLIND) || number_percent() < 75) {
            one_hit(ch, victim, dtype, wpntype);
            return;
        }
        af.type      = 4;
        af.location  = APPLY_HITROLL;
        af.modifier  = -4;
        af.duration  = 1;
        af.bitvector = AFF_BLIND;
        affect_to_char(victim, &af);
        act("$n is blinded!", victim, NULL, NULL, TO_ROOM);
        send_to_char("You are blinded!\n\r", victim);
        return;
    } else if(actype == 14) {
        if(victim->move < 1) {
            return;
        }
        act("You leap in the air and stamp on $N's feet.", ch, NULL, victim, TO_CHAR);
        act("$n leaps in the air and stamps on your feet.", ch, NULL, victim, TO_VICT);
        act("$n leaps in the air and stamps on $N's feet.", ch, NULL, victim, TO_NOTVICT);
        victim->move -= number_range(25, 50);
        if(victim->move < 0) {
            victim->move = 0;
        }
        return;
    }
    return;
}

void critical_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt, int dam) {
    OBJ_DATA *obj;
    OBJ_DATA *damaged;
    char buf  [MAX_STRING_LENGTH];
    char buf2 [20];
    int dtype;
    int critical = 0;
    int wpn = 0;
    int wpn2 = 0;
    int count;
    int count2;
    dtype = dt - 1000;
    if(dtype < 0 || dtype > 12) {
        return;
    }
    if(IS_NPC(ch)) {
        critical += (ch->level * 0.2);
    }
    if(!IS_NPC(ch)) {
        critical += (ch->wpn[dtype] * 0.1);
    }
    if(IS_NPC(victim)) {
        critical -= (victim->level * 0.2);
    }
    if(!IS_NPC(victim)) {
        obj = get_eq_char(victim, WEAR_WIELD);
        dtype = TYPE_HIT;
        if(obj != NULL && obj->item_type == ITEM_WEAPON) {
            dtype += obj->value[3];
        }
        wpn = dtype - 1000;
        if(wpn < 0 || wpn > 12) {
            wpn = 0;
        }
        obj = get_eq_char(victim, WEAR_HOLD);
        dtype = TYPE_HIT;
        if(obj != NULL && obj->item_type == ITEM_WEAPON) {
            dtype += obj->value[3];
        }
        wpn2 = dtype - 1000;
        if(wpn2 < 0 || wpn2 > 12) {
            wpn2 = 0;
        }
        if(victim->wpn[wpn] > victim->wpn[wpn2]) {
            critical -= ((victim->wpn[wpn] + 1) / 10);
        } else {
            critical -= ((victim->wpn[wpn2] + 1) / 10);
        }
    }
    if(critical < 1) {
        critical = 1;
    } else if(critical > 5) {
        critical = 5;
    }
    if(number_percent() > critical) {
        return;
    }
    critical = number_range(1, 28);
    if(critical == 1) {
        if(IS_HEAD(victim, LOST_EYE_L) && IS_HEAD(victim, LOST_EYE_R)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FACE)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from loosing an eye.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from loosing an eye.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_HEAD(victim, LOST_EYE_L) && number_percent() < 50) {
            SET_BIT(victim->loc_hp[0], LOST_EYE_L);
        } else if(!IS_HEAD(victim, LOST_EYE_R)) {
            SET_BIT(victim->loc_hp[0], LOST_EYE_R);
        } else if(!IS_HEAD(victim, LOST_EYE_L)) {
            SET_BIT(victim->loc_hp[0], LOST_EYE_L);
        } else {
            return;
        }
        act("Your skillful blow takes out $N's eye!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow takes out $N's eye!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow takes out your eye!", ch, NULL, victim, TO_VICT);
        make_part(victim, "eyeball");
        return;
    } else if(critical == 2) {
        if(IS_HEAD(victim, LOST_EAR_L) && IS_HEAD(victim, LOST_EAR_R)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HEAD)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from loosing an ear.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from loosing an ear.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_HEAD(victim, LOST_EAR_L) && number_percent() < 50) {
            SET_BIT(victim->loc_hp[0], LOST_EAR_L);
        } else if(!IS_HEAD(victim, LOST_EAR_R)) {
            SET_BIT(victim->loc_hp[0], LOST_EAR_R);
        } else if(!IS_HEAD(victim, LOST_EAR_L)) {
            SET_BIT(victim->loc_hp[0], LOST_EAR_L);
        } else {
            return;
        }
        act("Your skillful blow cuts off $N's ear!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's ear!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your ear!", ch, NULL, victim, TO_VICT);
        make_part(victim, "ear");
        return;
    } else if(critical == 3) {
        if(IS_HEAD(victim, LOST_NOSE)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FACE)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from loosing your nose.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from loosing $s nose.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[0], LOST_NOSE);
        act("Your skillful blow cuts off $N's nose!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's nose!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your nose!", ch, NULL, victim, TO_VICT);
        make_part(victim, "nose");
        return;
    } else if(critical == 4) {
        if(IS_HEAD(victim, LOST_NOSE) || IS_HEAD(victim, BROKEN_NOSE)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FACE)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from breaking your nose.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from breaking $s nose.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_HEAD(victim, LOST_NOSE) && !IS_HEAD(victim, BROKEN_NOSE)) {
            SET_BIT(victim->loc_hp[0], BROKEN_NOSE);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's nose!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's nose!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your nose!", ch, NULL, victim, TO_VICT);
        return;
    } else if(critical == 5) {
        if(IS_HEAD(victim, BROKEN_JAW)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FACE)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from breaking your jaw.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from breaking $s jaw.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_HEAD(victim, BROKEN_JAW)) {
            SET_BIT(victim->loc_hp[0], BROKEN_JAW);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's jaw!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's jaw!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your jaw!", ch, NULL, victim, TO_VICT);
        return;
    } else if(critical == 6) {
        if(IS_ARM_L(victim, LOST_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_ARMS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your left arm.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s left arm.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_L(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[2], LOST_ARM);
        } else {
            return;
        }
        if(!IS_BLEEDING(victim, BLEEDING_ARM_L)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
        }
        if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
        }
        act("Your skillful blow cuts off $N's left arm!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's left arm!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your left arm!", ch, NULL, victim, TO_VICT);
        make_part(victim, "arm");
        if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM)) {
            if((obj = get_eq_char(victim, WEAR_ARMS)) != NULL) {
                take_item(victim, obj);
            }
        }
        if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_HANDS)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_WRIST_L)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_FINGER_L)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 7) {
        if(IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_ARMS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your right arm.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s right arm.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_R(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[3], LOST_ARM);
        } else {
            return;
        }
        if(!IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
        }
        if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
        }
        act("Your skillful blow cuts off $N's right arm!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's right arm!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your right arm!", ch, NULL, victim, TO_VICT);
        make_part(victim, "arm");
        if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM)) {
            if((obj = get_eq_char(victim, WEAR_ARMS)) != NULL) {
                take_item(victim, obj);
            }
        }
        if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_HANDS)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_WRIST_R)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_FINGER_R)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 8) {
        if(IS_ARM_L(victim, LOST_ARM) || IS_ARM_L(victim, BROKEN_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_ARMS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking your left arm.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking $s left arm.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_L(victim, BROKEN_ARM) && !IS_ARM_L(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[2], BROKEN_ARM);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's left arm!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's left arm!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your left arm!", ch, NULL, victim, TO_VICT);
        if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
            take_item(victim, obj);
        }
        return;
    } else if(critical == 9) {
        if(IS_ARM_R(victim, LOST_ARM) || IS_ARM_R(victim, BROKEN_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_ARMS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking your right arm.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking $s right arm.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_R(victim, BROKEN_ARM) && !IS_ARM_R(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[3], BROKEN_ARM);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's right arm!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's right arm!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your right arm!", ch, NULL, victim, TO_VICT);
        if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
            take_item(victim, obj);
        }
        return;
    } else if(critical == 10) {
        if(IS_ARM_L(victim, LOST_HAND) || IS_ARM_L(victim, LOST_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your left hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s left hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[2], LOST_HAND);
        } else {
            return;
        }
        if(IS_BLEEDING(victim, BLEEDING_ARM_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
        }
        if(!IS_BLEEDING(victim, BLEEDING_HAND_L)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
        }
        act("Your skillful blow cuts off $N's left hand!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's left hand!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your left hand!", ch, NULL, victim, TO_VICT);
        make_part(victim, "hand");
        if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_HANDS)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_WRIST_L)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_FINGER_L)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 75) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 11) {
        if(IS_ARM_R(victim, LOST_HAND) || IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your right hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s right hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_ARM)) {
            SET_BIT(victim->loc_hp[3], LOST_HAND);
        } else {
            return;
        }
        if(IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
        }
        if(!IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
        }
        act("Your skillful blow cuts off $N's right hand!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's right hand!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your right hand!", ch, NULL, victim, TO_VICT);
        make_part(victim, "hand");
        if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_HANDS)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_WRIST_R)) != NULL) {
            take_item(victim, obj);
        }
        if((obj = get_eq_char(victim, WEAR_FINGER_R)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 75) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 12) {
        if(IS_ARM_L(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_L(victim, LOST_HAND)) {
            return;
        }
        if(IS_ARM_L(victim, LOST_THUMB) && IS_ARM_L(victim, LOST_FINGER_I) &&
                IS_ARM_L(victim, LOST_FINGER_M) && IS_ARM_L(victim, LOST_FINGER_R) &&
                IS_ARM_L(victim, LOST_FINGER_L)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing some fingers from your left hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing some fingers from $s left hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        count = 0;
        count2 = 0;
        if(!IS_ARM_L(victim, LOST_THUMB) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], LOST_THUMB);
            count2 += 1;
            make_part(victim, "thumb");
        }
        if(!IS_ARM_L(victim, LOST_FINGER_I) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], LOST_FINGER_I);
            count += 1;
            make_part(victim, "index");
        }
        if(!IS_ARM_L(victim, LOST_FINGER_M) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], LOST_FINGER_M);
            count += 1;
            make_part(victim, "middle");
        }
        if(!IS_ARM_L(victim, LOST_FINGER_R) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], LOST_FINGER_R);
            count += 1;
            make_part(victim, "ring");
            if((obj = get_eq_char(victim, WEAR_FINGER_L)) != NULL) {
                take_item(victim, obj);
            }
        }
        if(!IS_ARM_L(victim, LOST_FINGER_L) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], LOST_FINGER_L);
            count += 1;
            make_part(victim, "little");
        }
        if(count == 1) {
            sprintf(buf2, "finger");
        } else {
            sprintf(buf2, "fingers");
        }
        if(count > 0 && count2 > 0) {
            sprintf(buf, "Your skillful blow cuts off %d %s and the thumb from $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off %d %s and the thumb from $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off %d %s and the thumb from your left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count > 0) {
            sprintf(buf, "Your skillful blow cuts off %d %s from $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off %d %s from $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off %d %s from your left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count2 > 0) {
            sprintf(buf, "Your skillful blow cuts off the thumb from $N's left hand.");
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off the thumb from $N's left hand.");
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off the thumb from your left hand.");
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        }
        return;
    } else if(critical == 13) {
        if(IS_ARM_L(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_L(victim, LOST_HAND)) {
            return;
        }
        if((IS_ARM_L(victim, LOST_THUMB) || IS_ARM_L(victim, BROKEN_THUMB)) &&
                (IS_ARM_L(victim, LOST_FINGER_I) || IS_ARM_L(victim, BROKEN_FINGER_I)) &&
                (IS_ARM_L(victim, LOST_FINGER_M) || IS_ARM_L(victim, BROKEN_FINGER_M)) &&
                (IS_ARM_L(victim, LOST_FINGER_R) || IS_ARM_L(victim, BROKEN_FINGER_R)) &&
                (IS_ARM_L(victim, LOST_FINGER_L) || IS_ARM_L(victim, BROKEN_FINGER_L))) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking some fingers on your left hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking some fingers on $s left hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        count = 0;
        count2 = 0;
        if(IS_ARM_L(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_L(victim, LOST_HAND)) {
            return;
        }
        if(!IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], BROKEN_THUMB);
            count2 += 1;
        }
        if(!IS_ARM_L(victim, BROKEN_FINGER_I) && !IS_ARM_L(victim, LOST_FINGER_I) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], BROKEN_FINGER_I);
            count += 1;
        }
        if(!IS_ARM_L(victim, BROKEN_FINGER_M) && !IS_ARM_L(victim, LOST_FINGER_M) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], BROKEN_FINGER_M);
            count += 1;
        }
        if(!IS_ARM_L(victim, BROKEN_FINGER_R) && !IS_ARM_L(victim, LOST_FINGER_R) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], BROKEN_FINGER_R);
            count += 1;
        }
        if(!IS_ARM_L(victim, BROKEN_FINGER_L) && !IS_ARM_L(victim, LOST_FINGER_L) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[2], BROKEN_FINGER_L);
            count += 1;
        }
        if(count == 1) {
            sprintf(buf2, "finger");
        } else {
            sprintf(buf2, "fingers");
        }
        if(count > 0 && count2 > 0) {
            sprintf(buf, "Your skillful breaks %d %s and the thumb on $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks %d %s and the thumb on $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks %d %s and the thumb on your left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count > 0) {
            sprintf(buf, "Your skillful blow breaks %d %s on $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks %d %s on $N's left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks %d %s on your left hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count2 > 0) {
            sprintf(buf, "Your skillful blow breaks the thumb on $N's left hand.");
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks the thumb on $N's left hand.");
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks the thumb on your left hand.");
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        }
        return;
    } else if(critical == 14) {
        if(IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_R(victim, LOST_HAND)) {
            return;
        }
        if(IS_ARM_R(victim, LOST_THUMB) && IS_ARM_R(victim, LOST_FINGER_I) &&
                IS_ARM_R(victim, LOST_FINGER_M) && IS_ARM_R(victim, LOST_FINGER_R) &&
                IS_ARM_R(victim, LOST_FINGER_L)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing some fingers from your right hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing some fingers from $s right hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        count = 0;
        count2 = 0;
        if(IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_R(victim, LOST_HAND)) {
            return;
        }
        if(!IS_ARM_R(victim, LOST_THUMB) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], LOST_THUMB);
            count2 += 1;
            make_part(victim, "thumb");
        }
        if(!IS_ARM_R(victim, LOST_FINGER_I) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], LOST_FINGER_I);
            count += 1;
            make_part(victim, "index");
        }
        if(!IS_ARM_R(victim, LOST_FINGER_M) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], LOST_FINGER_M);
            count += 1;
            make_part(victim, "middle");
        }
        if(!IS_ARM_R(victim, LOST_FINGER_R) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], LOST_FINGER_R);
            count += 1;
            make_part(victim, "ring");
            if((obj = get_eq_char(victim, WEAR_FINGER_R)) != NULL) {
                take_item(victim, obj);
            }
        }
        if(!IS_ARM_R(victim, LOST_FINGER_L) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], LOST_FINGER_L);
            count += 1;
            make_part(victim, "little");
        }
        if(count == 1) {
            sprintf(buf2, "finger");
        } else {
            sprintf(buf2, "fingers");
        }
        if(count > 0 && count2 > 0) {
            sprintf(buf, "Your skillful blow cuts off %d %s and the thumb from $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off %d %s and the thumb from $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off %d %s and the thumb from your right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count > 0) {
            sprintf(buf, "Your skillful blow cuts off %d %s from $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off %d %s from $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off %d %s from your right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count2 > 0) {
            sprintf(buf, "Your skillful blow cuts off the thumb from $N's right hand.");
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow cuts off the thumb from $N's right hand.");
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow cuts off the thumb from your right hand.");
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        }
        return;
    } else if(critical == 15) {
        if(IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_R(victim, LOST_HAND)) {
            return;
        }
        if((IS_ARM_R(victim, LOST_THUMB) || IS_ARM_R(victim, BROKEN_THUMB)) &&
                (IS_ARM_R(victim, LOST_FINGER_I) || IS_ARM_R(victim, BROKEN_FINGER_I)) &&
                (IS_ARM_R(victim, LOST_FINGER_M) || IS_ARM_R(victim, BROKEN_FINGER_M)) &&
                (IS_ARM_R(victim, LOST_FINGER_R) || IS_ARM_R(victim, BROKEN_FINGER_R)) &&
                (IS_ARM_R(victim, LOST_FINGER_L) || IS_ARM_R(victim, BROKEN_FINGER_L))) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_HANDS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking some fingers on your right hand.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking some fingers on $s right hand.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        count = 0;
        count2 = 0;
        if(IS_ARM_R(victim, LOST_ARM)) {
            return;
        }
        if(IS_ARM_R(victim, LOST_HAND)) {
            return;
        }
        if(!IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], BROKEN_THUMB);
            count2 += 1;
        }
        if(!IS_ARM_R(victim, BROKEN_FINGER_I) && !IS_ARM_R(victim, LOST_FINGER_I) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], BROKEN_FINGER_I);
            count += 1;
        }
        if(!IS_ARM_R(victim, BROKEN_FINGER_M) && !IS_ARM_R(victim, LOST_FINGER_M) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], BROKEN_FINGER_M);
            count += 1;
        }
        if(!IS_ARM_R(victim, BROKEN_FINGER_R) && !IS_ARM_R(victim, LOST_FINGER_R) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], BROKEN_FINGER_R);
            count += 1;
        }
        if(!IS_ARM_R(victim, BROKEN_FINGER_L) && !IS_ARM_R(victim, LOST_FINGER_L) && number_percent() < 25) {
            SET_BIT(victim->loc_hp[3], BROKEN_FINGER_L);
            count += 1;
        }
        if(count == 1) {
            sprintf(buf2, "finger");
        } else {
            sprintf(buf2, "fingers");
        }
        if(count > 0 && count2 > 0) {
            sprintf(buf, "Your skillful breaks %d %s and the thumb on $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks %d %s and the thumb on $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks %d %s and the thumb on your right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count > 0) {
            sprintf(buf, "Your skillful blow breaks %d %s on $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks %d %s on $N's right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks %d %s on your right hand.", count, buf2);
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        } else if(count2 > 0) {
            sprintf(buf, "Your skillful blow breaks the thumb on $N's right hand.");
            act(buf, ch, NULL, victim, TO_CHAR);
            sprintf(buf, "$n's skillful blow breaks the thumb on $N's right hand.");
            act(buf, ch, NULL, victim, TO_NOTVICT);
            sprintf(buf, "$n's skillful blow breaks the thumb on your right hand.");
            act(buf, ch, NULL, victim, TO_VICT);
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                take_item(victim, obj);
            }
            return;
        }
        return;
    } else if(critical == 16) {
        if(IS_LEG_L(victim, LOST_LEG)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_LEGS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your left leg.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s left leg.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_L(victim, LOST_LEG)) {
            SET_BIT(victim->loc_hp[4], LOST_LEG);
        } else {
            return;
        }
        if(!IS_BLEEDING(victim, BLEEDING_LEG_L)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
        }
        if(IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
        }
        act("Your skillful blow cuts off $N's left leg!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's left leg!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your left leg!", ch, NULL, victim, TO_VICT);
        make_part(victim, "leg");
        if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG)) {
            if((obj = get_eq_char(victim, WEAR_LEGS)) != NULL) {
                take_item(victim, obj);
            }
        }
        if((obj = get_eq_char(victim, WEAR_FEET)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 17) {
        if(IS_LEG_R(victim, LOST_LEG)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_LEGS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your right leg.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s right leg.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_R(victim, LOST_LEG)) {
            SET_BIT(victim->loc_hp[5], LOST_LEG);
        } else {
            return;
        }
        if(!IS_BLEEDING(victim, BLEEDING_LEG_R)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
        }
        if(IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
        }
        act("Your skillful blow cuts off $N's right leg!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's right leg!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your right leg!", ch, NULL, victim, TO_VICT);
        make_part(victim, "leg");
        if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG)) {
            if((obj = get_eq_char(victim, WEAR_LEGS)) != NULL) {
                take_item(victim, obj);
            }
        }
        if((obj = get_eq_char(victim, WEAR_FEET)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 18) {
        if(IS_LEG_L(victim, BROKEN_LEG) || IS_LEG_L(victim, LOST_LEG)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_LEGS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking your left leg.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking $s left leg.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_L(victim, BROKEN_LEG) && !IS_LEG_L(victim, LOST_LEG)) {
            SET_BIT(victim->loc_hp[4], BROKEN_LEG);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's left leg!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's left leg!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your left leg!", ch, NULL, victim, TO_VICT);
        return;
    } else if(critical == 19) {
        if(IS_LEG_R(victim, BROKEN_LEG) || IS_LEG_R(victim, LOST_LEG)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_LEGS)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from breaking your right leg.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from breaking $s right leg.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_R(victim, BROKEN_LEG) && !IS_LEG_R(victim, LOST_LEG)) {
            SET_BIT(victim->loc_hp[5], BROKEN_LEG);
        } else {
            return;
        }
        act("Your skillful blow breaks $N's right leg!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow breaks $N's right leg!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow breaks your right leg!", ch, NULL, victim, TO_VICT);
        return;
    } else if(critical == 20) {
        if(IS_LEG_L(victim, LOST_LEG) || IS_LEG_L(victim, LOST_FOOT)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FEET)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your left foot.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s left foot.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_L(victim, LOST_LEG) && !IS_LEG_L(victim, LOST_FOOT)) {
            SET_BIT(victim->loc_hp[4], LOST_FOOT);
        } else {
            return;
        }
        if(IS_BLEEDING(victim, BLEEDING_LEG_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
        }
        if(!IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
        }
        act("Your skillful blow cuts off $N's left foot!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's left foot!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your left foot!", ch, NULL, victim, TO_VICT);
        make_part(victim, "foot");
        if((obj = get_eq_char(victim, WEAR_FEET)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 75) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 21) {
        if(IS_LEG_R(victim, LOST_LEG) || IS_LEG_R(victim, LOST_FOOT)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FEET)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from loosing your right foot.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from loosing $s right foot.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(!IS_LEG_R(victim, LOST_LEG) && !IS_LEG_R(victim, LOST_FOOT)) {
            SET_BIT(victim->loc_hp[5], LOST_FOOT);
        } else {
            return;
        }
        if(IS_BLEEDING(victim, BLEEDING_LEG_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
        }
        if(!IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
        }
        act("Your skillful blow cuts off $N's right foot!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow cuts off $N's right foot!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow cuts off your right foot!", ch, NULL, victim, TO_VICT);
        make_part(victim, "foot");
        if((obj = get_eq_char(victim, WEAR_FEET)) != NULL) {
            take_item(victim, obj);
        }
        if(!is_undying(victim) && number_percent() > 75) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 22) {
        int bodyloc = 0;
        int broken = number_range(1, 3);
        if(IS_BODY(victim, BROKEN_RIBS_1)) {
            bodyloc += 1;
        }
        if(IS_BODY(victim, BROKEN_RIBS_2)) {
            bodyloc += 2;
        }
        if(IS_BODY(victim, BROKEN_RIBS_4)) {
            bodyloc += 4;
        }
        if(IS_BODY(victim, BROKEN_RIBS_8)) {
            bodyloc += 8;
        }
        if(IS_BODY(victim, BROKEN_RIBS_16)) {
            bodyloc += 16;
        }
        if(bodyloc >= 24) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_BODY)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from breaking some ribs.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from breaking some ribs.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(IS_BODY(victim, BROKEN_RIBS_1)) {
            REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
        }
        if(IS_BODY(victim, BROKEN_RIBS_2)) {
            REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
        }
        if(IS_BODY(victim, BROKEN_RIBS_4)) {
            REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
        }
        if(IS_BODY(victim, BROKEN_RIBS_8)) {
            REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
        }
        if(IS_BODY(victim, BROKEN_RIBS_16)) {
            REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
        }
        if(bodyloc + broken > 24) {
            broken -= 1;
        }
        if(bodyloc + broken > 24) {
            broken -= 1;
        }
        bodyloc += broken;
        if(bodyloc >= 16) {
            bodyloc -= 16;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
        }
        if(bodyloc >= 8) {
            bodyloc -= 8;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
        }
        if(bodyloc >= 4) {
            bodyloc -= 4;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
        }
        if(bodyloc >= 2) {
            bodyloc -= 2;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
        }
        if(bodyloc >= 1) {
            bodyloc -= 1;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
        }
        sprintf(buf, "Your skillful blow breaks %d of $N's ribs!", broken);
        act(buf, ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n's skillful blow breaks %d of $N's ribs!", broken);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "$n's skillful blow breaks %d of your ribs!", broken);
        act(buf, ch, NULL, victim, TO_VICT);
        return;
    } else if(critical == 23) {
        int bodyloc = 0;
        int broken = number_range(1, 3);
        if(IS_HEAD(victim, LOST_TOOTH_1)) {
            bodyloc += 1;
        }
        if(IS_HEAD(victim, LOST_TOOTH_2)) {
            bodyloc += 2;
        }
        if(IS_HEAD(victim, LOST_TOOTH_4)) {
            bodyloc += 4;
        }
        if(IS_HEAD(victim, LOST_TOOTH_8)) {
            bodyloc += 8;
        }
        if(IS_HEAD(victim, LOST_TOOTH_16)) {
            bodyloc += 16;
        }
        if(bodyloc >= 28) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_FACE)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevents you from loosing some teeth.", victim, damaged, NULL, TO_CHAR);
            act("$p prevents $n from loosing some teeth.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p fall broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if(IS_HEAD(victim, LOST_TOOTH_1)) {
            REMOVE_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_1);
        }
        if(IS_HEAD(victim, LOST_TOOTH_2)) {
            REMOVE_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_2);
        }
        if(IS_HEAD(victim, LOST_TOOTH_4)) {
            REMOVE_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_4);
        }
        if(IS_HEAD(victim, LOST_TOOTH_8)) {
            REMOVE_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_8);
        }
        if(IS_HEAD(victim, LOST_TOOTH_16)) {
            REMOVE_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_16);
        }
        if(bodyloc + broken > 28) {
            broken -= 1;
        }
        if(bodyloc + broken > 28) {
            broken -= 1;
        }
        bodyloc += broken;
        if(bodyloc >= 16) {
            bodyloc -= 16;
            SET_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_16);
        }
        if(bodyloc >= 8) {
            bodyloc -= 8;
            SET_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_8);
        }
        if(bodyloc >= 4) {
            bodyloc -= 4;
            SET_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_4);
        }
        if(bodyloc >= 2) {
            bodyloc -= 2;
            SET_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_2);
        }
        if(bodyloc >= 1) {
            bodyloc -= 1;
            SET_BIT(victim->loc_hp[LOC_HEAD], LOST_TOOTH_1);
        }
        sprintf(buf, "Your skillful blow knocks out %d of $N's teeth!", broken);
        act(buf, ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n's skillful blow knocks out %d of $N's teeth!", broken);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "$n's skillful blow knocks out %d of your teeth!", broken);
        act(buf, ch, NULL, victim, TO_VICT);
        if(broken >= 1) {
            make_part(victim, "tooth");
        }
        if(broken >= 2) {
            make_part(victim, "tooth");
        }
        if(broken >= 3) {
            make_part(victim, "tooth");
        }
        return;
    } else if(critical == 24) {
        if(IS_ARM_L(victim, LOST_HAND) || IS_ARM_L(victim, LOST_ARM) ||
                IS_ARM_L(victim, SLIT_WRIST)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_WRIST_L)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from having your left wrist slashed open.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from having $s left wrist slashed open.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[2], SLIT_WRIST);
        if(IS_BLEEDING(victim, BLEEDING_ARM_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
        }
        if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
        }
        if(!IS_BLEEDING(victim, BLEEDING_WRIST_L)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_WRIST_L);
        }
        act("Your skillful blow slashes open $N's left wrist!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow slashes open $N's left wrist!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow slashes open your left wrist!", ch, NULL, victim, TO_VICT);
        if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
            take_item(victim, obj);
        }
        return;
    } else if(critical == 25) {
        if(IS_ARM_R(victim, LOST_HAND) || IS_ARM_R(victim, LOST_ARM) ||
                IS_ARM_R(victim, SLIT_WRIST)) {
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_WRIST_R)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from having your right wrist slashed open.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from having $s right wrist slashed open.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[3], SLIT_WRIST);
        if(IS_BLEEDING(victim, BLEEDING_ARM_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
        }
        if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
            REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
        }
        if(!IS_BLEEDING(victim, BLEEDING_WRIST_R)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_WRIST_R);
        }
        act("Your skillful blow slashes open $N's right wrist!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow slashes open $N's right wrist!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow slashes open your right wrist!", ch, NULL, victim, TO_VICT);
        if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
            take_item(victim, obj);
        }
        return;
    } else if(critical == 26) {
        if((damaged = get_eq_char(victim, WEAR_NECK_1)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from having your throat slashed open.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from having $s throat slashed open.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        if((damaged = get_eq_char(victim, WEAR_NECK_2)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from having your throat slashed open.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from having $s throat slashed open.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[1], CUT_THROAT);
        if(!IS_BLEEDING(victim, BLEEDING_THROAT)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
        }
        act("Your skillful blow slashes open $N's throat!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow slashes open $N's throat!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow slashes open your throat!", ch, NULL, victim, TO_VICT);
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 27) {
        if((damaged = get_eq_char(victim, WEAR_BODY)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from having your chest slashed open.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from having $s chest slashed open.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[1], CUT_CHEST);
        if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
        }
        act("Your skillful blow slashes open $N's chest!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow slashes open $N's chest!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow slashes open your chest!", ch, NULL, victim, TO_VICT);
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    } else if(critical == 28) {
        if((damaged = get_eq_char(victim, WEAR_BODY)) != NULL &&
                damaged->toughness > 0 && !IS_ITEMAFF(ch, ITEMA_VORPAL)) {
            act("$p prevent you from being disembowelled.", victim, damaged, NULL, TO_CHAR);
            act("$p prevent $n from being disembowelled.", victim, damaged, NULL, TO_ROOM);
            if(dam - damaged->toughness < 0) {
                return;
            }
            if((dam - damaged->toughness > damaged->resistance)) {
                damaged->condition -= damaged->resistance;
            } else {
                damaged->condition -= (dam - damaged->toughness);
            }
            if(damaged->condition < 1) {
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_CHAR);
                act("$p falls broken to the ground.", ch, damaged, NULL, TO_ROOM);
                obj_from_char(damaged);
                extract_obj(damaged);
            }
            return;
        }
        SET_BIT(victim->loc_hp[1], CUT_STOMACH);
        if(!IS_BLEEDING(victim, BLEEDING_STOMACH)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_STOMACH);
        }
        act("Your skillful blow disembowels $N!", ch, NULL, victim, TO_CHAR);
        act("$n's skillful blow disembowels $N!", ch, NULL, victim, TO_NOTVICT);
        act("$n's skillful blow disembowels you!", ch, NULL, victim, TO_VICT);
        if(!is_undying(victim) && number_percent() > 50) {
            act("You have struck $N a mortal wound.", ch, NULL, victim, TO_CHAR);
            act("$n has struck $N a mortal wound.", ch, NULL, victim, TO_NOTVICT);
            act("$n has struck you a mortal wound.", ch, NULL, victim, TO_VICT);
            demon_gain(ch, victim);
            group_gain(ch, victim);
            SET_BIT(victim->extra, EXTRA_DYING);
            victim->form = 20;
        }
        return;
    }
    return;
}

void special_move(CHAR_DATA *ch, CHAR_DATA *victim) {
    int dam = number_range(5, 10) + char_damroll(ch);
    if(dam < 10) {
        dam = 10;
    }
    switch(number_range(1, 7)) {
    default:
        return;
    case 1:
        act("You pull your hands into your waist then snap them into $N's stomach.", ch, NULL, victim, TO_CHAR);
        act("$n pulls $s hands into $s waist then snaps them into your stomach.", ch, NULL, victim, TO_VICT);
        act("$n pulls $s hands into $s waist then snaps them into $N's stomach.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_punch);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        act("You double over in agony, and fall to the ground gasping for breath.", victim, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony, and falls to the ground gasping for breath.", victim, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 2:
        act("You spin in a low circle, catching $N behind $S ankle.", ch, NULL, victim, TO_CHAR);
        act("$n spins in a low circle, catching you behind your ankle.", ch, NULL, victim, TO_VICT);
        act("$n spins in a low circle, catching $N behind $S ankle.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_sweep);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_LEG_L(victim, BROKEN_LEG) && !IS_LEG_L(victim, LOST_LEG)) {
            act("Your left leg shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's left leg shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_LEG_L], BROKEN_LEG);
        } else if(number_percent() <= 25 && !IS_LEG_R(victim, BROKEN_LEG) && !IS_LEG_R(victim, LOST_LEG)) {
            act("Your right leg shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's right leg shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_LEG_R], BROKEN_LEG);
        }
        act("You crash to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n crashes to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 3:
        act("You roll between $N's legs and flip to your feet.", ch, NULL, victim, TO_CHAR);
        act("$n rolls between your legs and flips to $s feet.", ch, NULL, victim, TO_VICT);
        act("$n rolls between $N's legs and flips to $s feet.", ch, NULL, victim, TO_NOTVICT);
        act("You spin around and smash your elbow into the back of $N's head.", ch, NULL, victim, TO_CHAR);
        act("$n spins around and smashes $s elbow into the back of your head.", ch, NULL, victim, TO_VICT);
        act("$n spins around and smashes $s elbow into the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_elbow);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        act("You fall to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n falls to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 4:
        act("You somersault over $N's head and land lightly on your toes.", ch, NULL, victim, TO_CHAR);
        act("$n somersaults over your head and lands lightly on $s toes.", ch, NULL, victim, TO_VICT);
        act("$n somersaults over $N's head and lands lightly on $s toes.", ch, NULL, victim, TO_NOTVICT);
        act("You roll back onto your shoulders and kick both feet into $N's back.", ch, NULL, victim, TO_CHAR);
        act("$n rolls back onto $s shoulders and kicks both feet into your back.", ch, NULL, victim, TO_VICT);
        act("$n rolls back onto $s shoulders and kicks both feet into $N's back.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_kick);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_BODY(victim, BROKEN_SPINE)) {
            act("Your spine shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's spine shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_BODY], BROKEN_SPINE);
        }
        act("You fall to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n falls to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        act("You flip back up to your feet.", ch, NULL, NULL, TO_CHAR);
        act("$n flips back up to $s feet.", ch, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 5:
        act("You grab $N by the neck and slam your head into $S face.", ch, NULL, victim, TO_CHAR);
        act("$n grabs $N by the neck and slams $s head into $S face.", ch, NULL, victim, TO_NOTVICT);
        act("$n grabs you by the neck and slams $s head into your face.", ch, NULL, victim, TO_VICT);
        damage(ch, victim, dam, gsn_headbutt);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE)) {
            act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
        } else if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW)) {
            act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
        } else if(number_percent() <= 25 && !IS_BODY(victim, BROKEN_NECK)) {
            act("Your neck shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's neck shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_BODY], BROKEN_NECK);
        }
        act("You grab $N by the waist and hoist $M above your head.", ch, NULL, victim, TO_CHAR);
        act("$n grabs $N by the waist and hoists $M above $s head.", ch, NULL, victim, TO_NOTVICT);
        act("$n grabs you by the waist and hoists you above $s head.", ch, NULL, victim, TO_VICT);
        special_hurl(ch, victim);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        act("You crash to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n crashes to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 6:
        act("You slam your fist into $N's stomach, who doubles over in agony.", ch, NULL, victim, TO_CHAR);
        act("$n slams $s fist into your stomach, and you double over in agony.", ch, NULL, victim, TO_VICT);
        act("$n slams $s fist into $N's stomach, who doubles over in agony.", ch, NULL, victim, TO_NOTVICT);
        act("You grab $N by the head and slam $S face into your knee.", ch, NULL, victim, TO_CHAR);
        act("$n grabs you by the head and slams your face into $s knee.", ch, NULL, victim, TO_VICT);
        act("$n grabs $N by the head and slams $S face into $s knee.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_knee);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE)) {
            act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
        } else if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW)) {
            act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
        } else if(number_percent() <= 25 && !IS_BODY(victim, BROKEN_NECK)) {
            act("Your neck shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's neck shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_BODY], BROKEN_NECK);
        }
        act("You roll onto your back and smash your feet into $N's chest.", ch, NULL, victim, TO_CHAR);
        act("$n rolls onto $s back and smashes $s feet into your chest.", ch, NULL, victim, TO_VICT);
        act("$n rolls onto $s back and smashes $s feet into $N's chest.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_kick);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        act("You crash to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n crashes to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        act("You flip back up to your feet.", ch, NULL, NULL, TO_CHAR);
        act("$n flips back up to $s feet.", ch, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    case 7:
        act("You duck under $N's attack and pound your fist into $S stomach.", ch, NULL, victim, TO_CHAR);
        act("$n ducks under your attack and pounds $s fist into your stomach.", ch, NULL, victim, TO_VICT);
        act("$n ducks under $N's attack and pounds $s fist into $N's stomach.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_punch);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        act("You double over in agony.", victim, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony.", victim, NULL, NULL, TO_ROOM);
        act("You grab $M by the head and smash your knee into $S face.", ch, NULL, victim, TO_CHAR);
        act("$n grabs you by the head and smashes $s knee into your face.", ch, NULL, victim, TO_VICT);
        act("$n grabs $M by the head and smashes $s knee into $N's face.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_knee);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE)) {
            act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
        } else if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW)) {
            act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
        } else if(number_percent() <= 25 && !IS_BODY(victim, BROKEN_NECK)) {
            act("Your neck shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's neck shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_BODY], BROKEN_NECK);
        }
        act("You stamp on the back of $N's leg, forcing $M to drop to one knee.", ch, NULL, victim, TO_CHAR);
        act("$n stamps on the back of your leg, forcing you to drop to one knee.", ch, NULL, victim, TO_VICT);
        act("$n stamps on the back of $N's leg, forcing $M to drop to one knee.", ch, NULL, victim, TO_NOTVICT);
        act("You grab $N by the hair and yank $S head back.", ch, NULL, victim, TO_CHAR);
        act("$n grabs you by the hair and yank your head back.", ch, NULL, victim, TO_VICT);
        act("$n grabs $N by the hair and yank $S head back.", ch, NULL, victim, TO_NOTVICT);
        act("You hammer your elbow down into $N's face.", ch, NULL, victim, TO_CHAR);
        act("$n hammers $s elbow down into your face.", ch, NULL, victim, TO_VICT);
        act("$n hammers $s elbow down into $N's face.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, dam, gsn_elbow);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
        if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE)) {
            act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
        } else if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW)) {
            act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
        } else if(number_percent() <= 25 && !IS_BODY(victim, BROKEN_NECK)) {
            act("Your neck shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
            act("$n's neck shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
            SET_BIT(victim->loc_hp[LOC_BODY], BROKEN_NECK);
        }
        act("You crash to the ground, stunned.", victim, NULL, NULL, TO_CHAR);
        act("$n crashes to the ground, stunned.", victim, NULL, NULL, TO_ROOM);
        stop_fighting(victim, TRUE);
        victim->position = POS_STUNNED;
        break;
    }
    return;
}

void special_hurl(CHAR_DATA *ch, CHAR_DATA *victim) {
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    EXIT_DATA       *pexit_rev;
    char            buf       [MAX_INPUT_LENGTH];
    char            direction [MAX_INPUT_LENGTH];
    int             door;
    int             rev_dir;
    int             dam;
    rev_dir = 0;
    door = number_range(0, 3);
    if(door == 0) {
        sprintf(direction, "north");
        rev_dir = 2;
    }
    if(door == 1) {
        sprintf(direction, "east");
        rev_dir = 3;
    }
    if(door == 2) {
        sprintf(direction, "south");
        rev_dir = 0;
    }
    if(door == 3) {
        sprintf(direction, "west");
        rev_dir = 1;
    }
    if((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL) {
        sprintf(buf, "$n hurls $N into the %s wall.", direction);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "You hurl $N into the %s wall.", direction);
        act(buf, ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n hurls you into the %s wall.", direction);
        act(buf, ch, NULL, victim, TO_VICT);
        dam = number_range(ch->level, (ch->level * 4));
        victim->hit = victim->hit - dam;
        update_pos(victim);
        if(IS_NPC(victim) && !IS_NPC(ch)) {
            ch->mkill++;
        }
        if(!IS_NPC(victim) && IS_NPC(ch)) {
            victim->mdeath++;
        }
        if(victim->position == POS_DEAD) {
            victim->form = 7;
            raw_kill(victim, ch);
            return;
        }
        return;
    }
    pexit = victim->in_room->exit[door];
    if(IS_SET(pexit->exit_info, EX_CLOSED) &&
            !IS_AFFECTED(victim, AFF_PASS_DOOR) &&
            !IS_AFFECTED(victim, AFF_ETHEREAL)) {
        if(IS_SET(pexit->exit_info, EX_LOCKED)) {
            REMOVE_BIT(pexit->exit_info, EX_LOCKED);
        }
        if(IS_SET(pexit->exit_info, EX_CLOSED)) {
            REMOVE_BIT(pexit->exit_info, EX_CLOSED);
        }
        sprintf(buf, "$n hurls $N %s.", direction);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "You hurl $N %s.", direction);
        act(buf, ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n hurls you %s, smashing you through the %s.", direction, pexit->keyword);
        act(buf, ch, NULL, victim, TO_VICT);
        sprintf(buf, "There is a loud crash as $n smashes through the $d.");
        act(buf, victim, NULL, pexit->keyword, TO_ROOM);
        if((to_room   = pexit->to_room) != NULL
                && (pexit_rev = to_room->exit[rev_dir]) != NULL
                &&   pexit_rev->to_room == ch->in_room
                &&   pexit_rev->keyword != NULL) {
            if(IS_SET(pexit_rev->exit_info, EX_LOCKED)) {
                REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
            }
            if(IS_SET(pexit_rev->exit_info, EX_CLOSED)) {
                REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
            }
            if(door == 0) {
                sprintf(direction, "south");
            }
            if(door == 1) {
                sprintf(direction, "west");
            }
            if(door == 2) {
                sprintf(direction, "north");
            }
            if(door == 3) {
                sprintf(direction, "east");
            }
            char_from_room(victim);
            char_to_room(victim, to_room);
            sprintf(buf, "$n comes smashing in through the %s $d.", direction);
            act(buf, victim, NULL, pexit->keyword, TO_ROOM);
            dam = number_range(ch->level, (ch->level * 6));
            victim->hit = victim->hit - dam;
            update_pos(victim);
            if(IS_NPC(victim) && !IS_NPC(ch)) {
                ch->mkill++;
            }
            if(!IS_NPC(victim) && IS_NPC(ch)) {
                victim->mdeath++;
            }
            if(victim->position == POS_DEAD) {
                victim->form = 7;
                raw_kill(victim, ch);
                return;
            }
        }
    } else {
        sprintf(buf, "$n hurls $N %s.", direction);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "You hurl $N %s.", direction);
        act(buf, ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n hurls you %s.", direction);
        act(buf, ch, NULL, victim, TO_VICT);
        if(door == 0) {
            sprintf(direction, "south");
        }
        if(door == 1) {
            sprintf(direction, "west");
        }
        if(door == 2) {
            sprintf(direction, "north");
        }
        if(door == 3) {
            sprintf(direction, "east");
        }
        char_from_room(victim);
        char_to_room(victim, to_room);
        sprintf(buf, "$n comes flying in from the %s.", direction);
        act(buf, victim, NULL, NULL, TO_ROOM);
        dam = number_range(ch->level, (ch->level * 2));
        victim->hit = victim->hit - dam;
        update_pos(victim);
        if(IS_NPC(victim) && !IS_NPC(ch)) {
            ch->mkill++;
        }
        if(!IS_NPC(victim) && IS_NPC(ch)) {
            victim->mdeath++;
        }
        if(victim->position == POS_DEAD) {
            victim->form = 7;
            raw_kill(victim, ch);
            return;
        }
    }
    return;
}

int highest_skill(CHAR_DATA *ch) {
    int highest, loop;
    if(IS_NPC(ch)) {
        return 0;
    }
    highest = 0;
    for(loop = 0; loop <= 12; loop++) {
        if(ch->wpn[loop] > highest) {
            highest = ch->wpn[loop];
        }
    }
    return highest;
}

int highest_stance(CHAR_DATA *ch, bool advanced) {
    int highest, loop, start;
    if(IS_NPC(ch)) {
        return 0;
    }
    highest = 0;
    if(advanced) {
        start = 6;
    } else {
        start = 1;
    }
    for(loop = start; loop <= 11; loop++) {
        if(ch->stance[loop] > highest) {
            highest = ch->stance[loop];
        }
    }
    return highest;
}

void beast_rage(CHAR_DATA *ch) {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    if(IS_NPC(ch) || !IS_VAMPIRE(ch)) {
        return;
    }
    if(ch->pcdata->wolf < 1 && !IS_MORE(ch, MORE_ANGER)) {
        return;
    }
    if(ch->position < POS_SLEEPING) {
        return;
    }
    if(ch->position == POS_FIGHTING) {
        return;
    }
    if(ch->position < POS_FIGHTING) {
        do_stand(ch, "");
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    for(vch = ch->in_room->people; vch != NULL; vch = vch_next) {
        vch_next	= vch->next_in_room;
        if(!IS_NPC(vch) && vch->pcdata->chobj != NULL) {
            continue;
        }
        if(ch == vch) {
            continue;
        }
        if(!can_see(ch, vch)) {
            continue;
        }
        if(auto_safe(ch, vch)) {
            continue;
        }
        if(number_percent() <= 50) {
            continue;
        }
        if(IS_MORE(ch, MORE_ANGER) && vch->position <= POS_MORTAL) {
            continue;
        }
        if(IS_MORE(ch, MORE_ANGER)) {
            act("You decide to vent your anger on $N!", ch, NULL, vch, TO_CHAR);
            act("$n snarls in anger and lunges at $N!", ch, NULL, vch, TO_NOTVICT);
            act("$n snarls in anger and lunges at you!", ch, NULL, vch, TO_VICT);
        } else {
            send_to_char("Your beast demands BLOOD!\n\r", ch);
            act("$n roars with rage as $s inner beast demands BLOOD!", ch, NULL, NULL, TO_ROOM);
        }
        if(!IS_NPC(vch) && vch->position == POS_MORTAL) {
            do_tear(ch, vch->name);
        } else {
            multi_hit(ch, vch, TYPE_UNDEFINED);
        }
        return;
    }
    if(ch->pcdata->wolf > 0) {
        ch->pcdata->wolf--;
    }
    if((ch->pcdata->wolf > 0 || IS_MORE(ch, MORE_ANGER)) &&
            number_percent() <= 25) {
        switch(number_range(1, 8)) {
        default:
        case 1:
            send_to_char("You snarl angrilly.\n\r", ch);
            act("$n snarls angrilly.", ch, NULL, NULL, TO_ROOM);
            break;
        case 2:
            send_to_char("You roar with rage!\n\r", ch);
            act("$n roars with rage!", ch, NULL, NULL, TO_ROOM);
            break;
        case 3:
            send_to_char("You grit your teeth and fume with rage.\n\r", ch);
            act("$n grits $s teeth and fumes with rage.", ch, NULL, NULL, TO_ROOM);
            break;
        case 4:
            send_to_char("You glare around icily.\n\r", ch);
            act("$n glares icily around $m.", ch, NULL, NULL, TO_ROOM);
            break;
        case 5:
            send_to_char("You growl with unbridled hatred.\n\r", ch);
            act("$n growls with unbridled hatred.", ch, NULL, NULL, TO_ROOM);
            break;
        case 6:
            send_to_char("You laugh diabolically.  MUHAHAHAHAHAHA!\n\r", ch);
            act("$n laughs diabolically.  MUHAHAHAHAHAHA!", ch, NULL, NULL, TO_ROOM);
            break;
        case 7:
            send_to_char("You sneer in utter contempt.\n\r", ch);
            act("$n sneers in utter contempt.", ch, NULL, NULL, TO_ROOM);
            break;
        case 8:
            send_to_char("You throw back your head and cackle with insane glee!\n\r", ch);
            act("$n throws back $s head and cackles with insane glee!", ch, NULL, NULL, TO_ROOM);
            break;
        }
    }
    return;
}
