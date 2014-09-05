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



void do_pset(CHAR_DATA *ch, char *argument) {
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;
    sprintf(buf, "%s: Pset %s", ch->name, argument);
    if(ch->level < NO_WATCH) {
        do_watching(ch, buf);
    }
    smash_tilde(argument);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    strcpy(arg4, argument);
    if(arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {
        send_to_char("Syntax: pset <victim> <area> <field> <value>\n\r", ch);
        send_to_char("\n\r",						ch);
        send_to_char("Area being one of:\n\r",			        ch);
        send_to_char("  quest quest+ quest- weapon immune beast\n\r",  ch);
        send_to_char("  blue red yellow green purple class\n\r",        ch);
        send_to_char("  mongoose crane crab viper bull mantis\n\r",	ch);
        send_to_char("  dragon tiger monkey swallow stanceall\n\r",    ch);
        send_to_char("\n\r",						ch);
        send_to_char("Field being one of:\n\r",			ch);
        send_to_char("Weapon:  slice stab slash whip claw blast   \n\r", ch);
        send_to_char("Weapon:  pound crush grep bite pierce suck \n\r",  ch);
        send_to_char("Immune:  slash stab smash animal misc charm\n\r",  ch);
        send_to_char("Immune:  heat cold acid summon voodoo \n\r",	  ch);
        send_to_char("Immune:  hurl backstab shielded kick disarm\n\r",  ch);
        send_to_char("Immune:  steal sleep drain sunlight all\n\r",      ch);
        send_to_char("Class:   Vampire Werewolf Demon Purple\n\r",       ch);
        send_to_char("         Red Blue Green Yellow\n\r",               ch);
        return;
    }
    if((victim = get_char_world(ch, arg1)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(IS_SET(victim->act, PLR_GODLESS) && ch->level < NO_GODLESS) {
        send_to_char("You failed.\n\r", ch);
        return;
    }
    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number(arg3) ? atoi(arg3) : -1;
    /*
     * Set something.
     */
    if(!str_cmp(arg2, "class")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(victim->class != CLASS_NONE) {
            send_to_char("They already have a class.\n\r", ch);
            return;
        }
        if(victim->level < LEVEL_AVATAR) {
            send_to_char("You can only do this to avatars.\n\r", ch);
            return;
        }
        if(ch->level != MAX_LEVEL) {
            send_to_char("Sorry, no can do...\n\r", ch);
            return;
        }
        if(!str_cmp(arg3, "vampire")) {
            victim->class = CLASS_VAMPIRE;
            victim->vampgen = 1;
            update_gen(victim);
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Master Vampire.\n\r", victim);
        } else if(!str_cmp(arg3, "werewolf")) {
            victim->class = CLASS_WEREWOLF;
            victim->vampgen = 1;
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Master Werewolf.\n\r", victim);
        } else if(!str_cmp(arg3, "demon")) {
            victim->class = CLASS_DEMON;
            victim->alignment = -1000;
            SET_BIT(victim->act, PLR_DEMON);
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now a Demon Lord.\n\r", victim);
        } else if(!str_cmp(arg3, "angel")) {
            victim->class = CLASS_ANGEL;
            victim->alignment = 1000;
            SET_BIT(victim->act, PLR_DEMON);
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now an Archangel.\n\r", victim);
        } else if(!str_cmp(arg3, "purple")) {
            if(victim->level < LEVEL_ARCHMAGE) {
                victim->level = LEVEL_ARCHMAGE;
                victim->trust = LEVEL_ARCHMAGE;
            }
            victim->class = CLASS_MAGE;
            victim->pcdata->runes[0] = PURPLE_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Purple Archmage.\n\r", victim);
        } else if(!str_cmp(arg3, "red")) {
            if(victim->level < LEVEL_ARCHMAGE) {
                victim->level = LEVEL_ARCHMAGE;
                victim->trust = LEVEL_ARCHMAGE;
            }
            victim->class = CLASS_MAGE;
            victim->pcdata->runes[0] = RED_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Red Archmage.\n\r", victim);
        } else if(!str_cmp(arg3, "blue")) {
            if(victim->level < LEVEL_ARCHMAGE) {
                victim->level = LEVEL_ARCHMAGE;
                victim->trust = LEVEL_ARCHMAGE;
            }
            victim->class = CLASS_MAGE;
            victim->pcdata->runes[0] = BLUE_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Blue Archmage.\n\r", victim);
        } else if(!str_cmp(arg3, "green")) {
            if(victim->level < LEVEL_ARCHMAGE) {
                victim->level = LEVEL_ARCHMAGE;
                victim->trust = LEVEL_ARCHMAGE;
            }
            victim->class = CLASS_MAGE;
            victim->pcdata->runes[0] = GREEN_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Green Archmage.\n\r", victim);
        } else if(!str_cmp(arg3, "yellow")) {
            if(victim->level < LEVEL_ARCHMAGE) {
                victim->level = LEVEL_ARCHMAGE;
                victim->trust = LEVEL_ARCHMAGE;
            }
            victim->class = CLASS_MAGE;
            victim->pcdata->runes[0] = YELLOW_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Yellow Archmage.\n\r", victim);
        } else if(!str_cmp(arg3, "abomination")) {
            victim->class = CLASS_ABOMINATION;
            victim->vampgen = 3;
            update_gen(victim);
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Abomination Leader.\n\r", victim);
        } else if(!str_cmp(arg3, "skindancer")) {
            if(victim->level < LEVEL_MAGE) {
                victim->level = LEVEL_MAGE;
                victim->trust = LEVEL_MAGE;
            }
            victim->class = CLASS_SKINDANCER;
            victim->vampgen = 3;
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Skindancer Leader.\n\r", victim);
        } else if(!str_cmp(arg3, "lich")) {
            if(victim->level < LEVEL_MAGE) {
                victim->level = LEVEL_MAGE;
                victim->trust = LEVEL_MAGE;
            }
            victim->class = CLASS_LICH;
            victim->vampgen = 3;
            update_gen(victim);
            victim->pcdata->runes[0] = RED_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Lich Leader.\n\r", victim);
        } else if(!str_cmp(arg3, "baali")) {
            victim->class = CLASS_BAALI;
            victim->vampgen = 3;
            update_gen(victim);
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            SET_BIT(victim->act, PLR_DEMON);
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Baali Leader.\n\r", victim);
        } else if(!str_cmp(arg3, "bsd")) {
            victim->class = CLASS_BLACK_SPIRAL_DANCER;
            victim->vampgen = 3;
            victim->pcdata->cland[0] = -1;
            victim->pcdata->cland[1] = -1;
            victim->pcdata->cland[2] = -1;
            SET_BIT(victim->act, PLR_DEMON);
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Black Spiral Dancer Leader.\n\r", victim);
        } else if(!str_cmp(arg3, "nephandi")) {
            if(victim->level < LEVEL_MAGE) {
                victim->level = LEVEL_MAGE;
                victim->trust = LEVEL_MAGE;
            }
            victim->class = CLASS_NEPHANDI;
            SET_BIT(victim->act, PLR_DEMON);
            victim->pcdata->runes[0] = RED_MAGIC;
            victim->pcdata->runes[1] = 2047;
            victim->pcdata->runes[2] = 1023;
            victim->pcdata->runes[3] = 15;
            send_to_char("Ok.\n\r", ch);
            send_to_char("You are now the Nepandi Leader.\n\r", victim);
        } else {
            do_pset(ch, "");
        }
        return;
    }
    if(!str_cmp(arg2, "beast")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 100) {
            send_to_char("Beast range is 0 to 100.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->beast = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "quest")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 1 || value > 15000) {
            send_to_char("Quest range is 1 to 15000.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->pcdata->quest = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "quest+")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 1 || value + victim->pcdata->quest > 15000) {
            send_to_char("Quest range is 1 to 15000.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->pcdata->quest += value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "quest-")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(victim->pcdata->quest - value < 1 || value > 15000) {
            send_to_char("Quest range is 1 to 15000.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->pcdata->quest -= value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "viper")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Viper range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_VIPER] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "crane")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Crane range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_CRANE] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "crab")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Crab range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_CRAB] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "mongoose")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Mongoose range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_MONGOOSE] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "bull")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Bull range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_BULL] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "mantis")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Mantis range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_MANTIS] = value;
            victim->stance[STANCE_CRANE]  = 200;
            victim->stance[STANCE_VIPER]  = 200;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "dragon")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Dragon range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_DRAGON] = value;
            victim->stance[STANCE_CRAB]   = 200;
            victim->stance[STANCE_BULL]   = 200;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "tiger")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Tiger range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_TIGER] = value;
            victim->stance[STANCE_BULL]  = 200;
            victim->stance[STANCE_VIPER] = 200;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "monkey")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Monkey range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_MONKEY] = value;
            victim->stance[STANCE_MONGOOSE]  = 200;
            victim->stance[STANCE_CRANE]  = 200;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "swallow")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Swallow range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->stance[STANCE_SWALLOW] = value;
            victim->stance[STANCE_CRAB]  = 200;
            victim->stance[STANCE_MONGOOSE]  = 200;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "stanceall")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(value < 0 || value > 200) {
            send_to_char("Stance Swallow range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            int i;
            for(i = 1; i <= 10; i++) {
                victim->stance[i] = value;
            }
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "purple")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(IS_MAGE(victim) && (value < 4 || value > 240)) {
            send_to_char("Spell range is 4 to 240.\n\r", ch);
            return;
        } else if(!IS_MAGE(victim) && (value < 4 || value > 200)) {
            send_to_char("Spell range is 4 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->spl[PURPLE_MAGIC] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "red")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(IS_MAGE(victim) && (value < 4 || value > 240)) {
            send_to_char("Spell range is 4 to 240.\n\r", ch);
            return;
        } else if(!IS_MAGE(victim) && (value < 4 || value > 200)) {
            send_to_char("Spell range is 4 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->spl[RED_MAGIC] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "blue")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(IS_MAGE(victim) && (value < 4 || value > 240)) {
            send_to_char("Spell range is 4 to 240.\n\r", ch);
            return;
        } else if(!IS_MAGE(victim) && (value < 4 || value > 200)) {
            send_to_char("Spell range is 4 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->spl[BLUE_MAGIC] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "green")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(IS_MAGE(victim) && (value < 4 || value > 240)) {
            send_to_char("Spell range is 4 to 240.\n\r", ch);
            return;
        } else if(!IS_MAGE(victim) && (value < 4 || value > 200)) {
            send_to_char("Spell range is 4 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->spl[GREEN_MAGIC] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "yellow")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(IS_MAGE(victim) && (value < 4 || value > 240)) {
            send_to_char("Spell range is 4 to 240.\n\r", ch);
            return;
        } else if(!IS_MAGE(victim) && (value < 4 || value > 200)) {
            send_to_char("Spell range is 4 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            victim->spl[YELLOW_MAGIC] = value;
            send_to_char("Ok.\n\r", ch);
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "immune")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        if(arg3 == '\0') {
            send_to_char("pset <victim> immune <immunity>.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            if(!str_cmp(arg3, "voodoo")) {
                if(IS_SET(victim->immune, IMM_VOODOO)) {
                    REMOVE_BIT(victim->immune, IMM_VOODOO);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_VOODOO);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "slash")) {
                if(IS_SET(victim->immune, IMM_SLASH)) {
                    REMOVE_BIT(victim->immune, IMM_SLASH);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_SLASH);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "stab")) {
                if(IS_SET(victim->immune, IMM_STAB)) {
                    REMOVE_BIT(victim->immune, IMM_STAB);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_STAB);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "smash")) {
                if(IS_SET(victim->immune, IMM_SMASH)) {
                    REMOVE_BIT(victim->immune, IMM_SMASH);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_SMASH);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "anmial")) {
                if(IS_SET(victim->immune, IMM_ANIMAL)) {
                    REMOVE_BIT(victim->immune, IMM_ANIMAL);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_ANIMAL);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "misc")) {
                if(IS_SET(victim->immune, IMM_MISC)) {
                    REMOVE_BIT(victim->immune, IMM_MISC);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_MISC);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "charm")) {
                if(IS_SET(victim->immune, IMM_CHARM)) {
                    REMOVE_BIT(victim->immune, IMM_CHARM);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_CHARM);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "heat")) {
                if(IS_SET(victim->immune, IMM_HEAT)) {
                    REMOVE_BIT(victim->immune, IMM_HEAT);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_HEAT);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "cold")) {
                if(IS_SET(victim->immune, IMM_COLD)) {
                    REMOVE_BIT(victim->immune, IMM_COLD);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_COLD);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "lightning")) {
                if(IS_SET(victim->immune, IMM_LIGHTNING)) {
                    REMOVE_BIT(victim->immune, IMM_LIGHTNING);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_LIGHTNING);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "acid")) {
                if(IS_SET(victim->immune, IMM_ACID)) {
                    REMOVE_BIT(victim->immune, IMM_ACID);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_ACID);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "shield")) {
                if(IS_SET(victim->immune, IMM_SHIELDED)) {
                    REMOVE_BIT(victim->immune, IMM_SHIELDED);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_SHIELDED);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "hurl")) {
                if(IS_SET(victim->immune, IMM_HURL)) {
                    REMOVE_BIT(victim->immune, IMM_HURL);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_HURL);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "backstab")) {
                if(IS_SET(victim->immune, IMM_BACKSTAB)) {
                    REMOVE_BIT(victim->immune, IMM_BACKSTAB);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_BACKSTAB);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "kick")) {
                if(IS_SET(victim->immune, IMM_KICK)) {
                    REMOVE_BIT(victim->immune, IMM_KICK);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_KICK);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "disarm")) {
                if(IS_SET(victim->immune, IMM_DISARM)) {
                    REMOVE_BIT(victim->immune, IMM_DISARM);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_DISARM);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "steal")) {
                if(IS_SET(victim->immune, IMM_STEAL)) {
                    REMOVE_BIT(victim->immune, IMM_STEAL);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_STEAL);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "sleep")) {
                if(IS_SET(victim->immune, IMM_SLEEP)) {
                    REMOVE_BIT(victim->immune, IMM_SLEEP);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_SLEEP);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "sunlight")) {
                if(IS_SET(victim->immune, IMM_SUNLIGHT)) {
                    REMOVE_BIT(victim->immune, IMM_SUNLIGHT);
                    send_to_char("Ok Immunity Removed.\n\r", ch);
                    return;
                } else {
                    SET_BIT(victim->immune, IMM_SUNLIGHT);
                    send_to_char("Ok Immunity Added.\n\r", ch);
                    return;
                }
            }
            if(!str_cmp(arg3, "all")) {
                SET_BIT(victim->immune, IMM_DRAIN);
                SET_BIT(victim->immune, IMM_VOODOO);
                SET_BIT(victim->immune, IMM_SLASH);
                SET_BIT(victim->immune, IMM_STAB);
                SET_BIT(victim->immune, IMM_SMASH);
                SET_BIT(victim->immune, IMM_ANIMAL);
                SET_BIT(victim->immune, IMM_MISC);
                SET_BIT(victim->immune, IMM_CHARM);
                SET_BIT(victim->immune, IMM_HEAT);
                SET_BIT(victim->immune, IMM_COLD);
                SET_BIT(victim->immune, IMM_LIGHTNING);
                SET_BIT(victim->immune, IMM_ACID);
                SET_BIT(victim->immune, IMM_HURL);
                SET_BIT(victim->immune, IMM_BACKSTAB);
                SET_BIT(victim->immune, IMM_KICK);
                SET_BIT(victim->immune, IMM_DISARM);
                SET_BIT(victim->immune, IMM_STEAL);
                SET_BIT(victim->immune, IMM_SLEEP);
                send_to_char("Ok All Immunities Added.\n\r", ch);
                return;
            }
            send_to_char("No such immunity exists.\n\r", ch);
            return;
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    if(!str_cmp(arg2, "weapon")) {
        if(IS_NPC(victim)) {
            send_to_char("Not on NPC's.\n\r", ch);
            return;
        }
        argument = one_argument(argument, arg4);
        /*
         * Snarf the value (which need not be numeric).
         */
        value = is_number(arg4) ? atoi(arg4) : -1;
        if(value < 0 || value > 200) {
            send_to_char("Weapon skill range is 0 to 200.\n\r", ch);
            return;
        }
        if(IS_JUDGE(ch)) {
            if(!str_cmp(arg3, "unarmed")) {
                victim->wpn[0] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "slice")) {
                victim->wpn[1] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "stab")) {
                victim->wpn[2] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "slash")) {
                victim->wpn[3] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "whip")) {
                victim->wpn[4] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "claw")) {
                victim->wpn[5] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "blast")) {
                victim->wpn[6] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "pound")) {
                victim->wpn[7] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "crush")) {
                victim->wpn[8] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "grep")) {
                victim->wpn[9] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "bite")) {
                victim->wpn[10] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "pierce")) {
                victim->wpn[11] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "suck")) {
                victim->wpn[12] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            if(!str_cmp(arg3, "all")) {
                victim->wpn[0] = value;
                victim->wpn[1] = value;
                victim->wpn[2] = value;
                victim->wpn[3] = value;
                victim->wpn[4] = value;
                victim->wpn[5] = value;
                victim->wpn[6] = value;
                victim->wpn[7] = value;
                victim->wpn[8] = value;
                victim->wpn[8] = value;
                victim->wpn[9] = value;
                victim->wpn[10] = value;
                victim->wpn[11] = value;
                victim->wpn[12] = value;
                send_to_char("Ok.\n\r", ch);
                return;
            }
            send_to_char("No such weapon skill exists.\n\r", ch);
            return;
        } else {
            send_to_char("Sorry, no can do...\n\r", ch);
        }
        return;
    }
    /*
     * Generate usage message.
     */
    do_pset(ch, "");
    return;
}


