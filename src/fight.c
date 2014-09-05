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
bool	check_dodge	args((CHAR_DATA *ch, CHAR_DATA *victim, int dt));
void	check_killer	args((CHAR_DATA *ch, CHAR_DATA *victim));
bool	check_parry	args((CHAR_DATA *ch, CHAR_DATA *victim, int dt));
void	dam_message	args((CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                          int dt));
void	death_cry	args((CHAR_DATA *ch));
int	xp_compute	args((CHAR_DATA *gch, CHAR_DATA *victim));
bool	can_counter	args((CHAR_DATA *ch));
bool	can_bypass	args((CHAR_DATA *ch, CHAR_DATA *victim));
int	number_attacks	args((CHAR_DATA *ch));
int	get_speed	args((CHAR_DATA *ch));
void	update_damcap	args((CHAR_DATA *ch, CHAR_DATA *victim));



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update(void) {
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;
    CHAR_DATA *mount;
    for(ch = first_fight; ch != NULL; ch = ch_next) {
        ch_next	= ch->next_fight;
        if((victim = ch->fighting) == NULL || ch->in_room == NULL) {
            stop_fighting(ch, FALSE);
            continue;
        }
        if(IS_AWAKE(ch) && IS_AWAKE(victim) &&
                ch->in_room == victim->in_room) {
            if(--ch->init <= 0) {
                multi_hit(ch, victim, TYPE_UNDEFINED);
                ch->init = get_speed(ch);
            } else {
                continue;
            }
        } else {
            stop_fighting(ch, FALSE);
        }
        if((victim = ch->fighting) == NULL) {
            stop_fighting(ch, FALSE);
            continue;
        }
        /*
         * Fun for the whole family!
         */
        for(rch = ch->in_room->people; rch != NULL; rch = rch_next) {
            rch_next = rch->next_in_room;
            if(IS_AWAKE(rch) && rch->fighting == NULL) {
                /*
                 * Mount's auto-assist their riders and vice versa.
                 */
                if((mount = rch->mount) != NULL) {
                    if(mount == ch) {
                        multi_hit(rch, victim, TYPE_UNDEFINED);
                    }
                    continue;
                }
                /*
                 * PC's auto-assist others in their group.
                 */
                if(!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM)) {
                    if((!IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM))
                            &&   is_same_group(ch, rch)) {
                        multi_hit(rch, victim, TYPE_UNDEFINED);
                    }
                    continue;
                }
                /*
                 * NPC's assist NPC's of same type or 12.5% chance regardless.
                 */
                if(IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM)) {
                    if(rch->pIndexData == ch->pIndexData
                            ||   number_bits(3) == 0) {
                        CHAR_DATA *vch;
                        CHAR_DATA *target;
                        int number;
                        target = NULL;
                        number = 0;
                        for(vch = ch->in_room->people; vch; vch = vch->next) {
                            if(can_see(rch, vch)
                                    &&   is_same_group(vch, victim)
                                    &&   number_range(0, number) == 0) {
                                target = vch;
                                number++;
                            }
                        }
                        if(target != NULL) {
                            multi_hit(rch, target, TYPE_UNDEFINED);
                        }
                    }
                }
            }
        }
    }
    return;
}



/*
 * Do one group of attacks.
 */
void multi_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt) {
    OBJ_DATA *wield;
    OBJ_DATA *wield1;
    OBJ_DATA *wield2;
    int	      sn;
    int	      level;
    int       unarmed;
    int       wieldorig = 0;
    int       wieldtype = 0;
    if(ch->position < POS_SLEEPING) {
        return;
    }
    if(dt == gsn_headbutt) {
        one_hit(ch, victim, dt, 1);
        return;
    }
    wield1 = get_eq_char(ch, WEAR_WIELD);
    wield2 = get_eq_char(ch, WEAR_HOLD);
    if(wield1 != NULL && wield1->item_type == ITEM_WEAPON) {
        wieldorig  = 1;
    }
    if(wield2 != NULL && wield2->item_type == ITEM_WEAPON) {
        wieldorig += 2;
    }
    wieldtype = wieldorig;
    if(wieldorig == 3) {
        if(number_range(1, 2) == 2) {
            wieldtype = 2;
        } else {
            wieldtype = 1;
        }
    }
    if(wieldtype == 2) {
        wield = wield2;
    } else {
        wield = wield1;
    }
    if(ch->stance[0] > 0 && number_percent() == 1) {
        int stance = ch->stance[0];
        if(ch->stance[stance] >= 200) {
            special_move(ch, victim);
            return;
        }
    }
    unarmed = 0;
    one_hit(ch, victim, dt, wieldtype);
    if(victim == NULL || victim->position != POS_FIGHTING) {
        return;
    }
    if(dt == TYPE_UNDEFINED) {
        dt = TYPE_HIT;
        if(wield != NULL && wield->item_type == ITEM_WEAPON) {
            dt += wield->value[3];
            if(wield->value[0] >= 1) {
                /* Look, just don't ask...   KaVir */
                if(wield->value[0] >= 1000) {
                    sn = wield->value[0] - ((wield->value[0] / 1000) * 1000);
                } else {
                    sn = wield->value[0];
                }
                if(sn != 0 && victim->position == POS_FIGHTING) {
                    (*skill_table[sn].spell_fun)(sn, wield->level, ch, victim);
                }
            }
        }
    }
    if(victim == NULL || victim->position != POS_FIGHTING) {
        return;
    }
    if(ch->fighting != victim || dt == gsn_backstab || dt == gsn_headbutt) {
        return;
    }
    if(!IS_NPC(victim) && !IS_SET(victim->act, PLR_WOLFMAN) &&
            number_percent() <= victim->pcdata->learned[gsn_fastdraw]) {
        OBJ_DATA *obj;
        int wpntype  = 0;
        int wpntype2 = 0;
        if((obj = get_eq_char(victim, WEAR_SCABBARD_R)) != NULL) {
            wpntype = obj->value[3];
            if(wpntype < 0 || wpntype > 12) {
                wpntype = 0;
            }
            if((obj = get_eq_char(victim, WEAR_WIELD)) != NULL) {
                obj_from_char(obj);
                obj_to_room(obj, victim->in_room);
                act("You hurl $p aside.", victim, obj, NULL, TO_CHAR);
                act("$n hurls $p aside.", victim, obj, NULL, TO_ROOM);
            }
            do_draw(victim, "right");
        }
        if((obj = get_eq_char(victim, WEAR_SCABBARD_L)) != NULL) {
            wpntype2 = obj->value[3];
            if(wpntype2 < 0 || wpntype2 > 12) {
                wpntype2 = 0;
            }
            if((obj = get_eq_char(victim, WEAR_HOLD)) != NULL) {
                obj_from_char(obj);
                obj_to_room(obj, victim->in_room);
                act("You hurl $p aside.", victim, obj, NULL, TO_CHAR);
                act("$n hurls $p aside.", victim, obj, NULL, TO_ROOM);
            }
            do_draw(victim, "left");
        }
        if(wpntype  > 0) {
            one_hit(victim, ch, TYPE_UNDEFINED, 1);
        }
        if(victim == NULL || victim->position != POS_FIGHTING) {
            return;
        }
        if(wpntype2 > 0) {
            one_hit(victim, ch, TYPE_UNDEFINED, 2);
        }
        if(victim == NULL || victim->position != POS_FIGHTING) {
            return;
        }
    }
    if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_FANGS) && number_range(1, 5) == 1) {
        one_hit(ch, victim, (TYPE_HIT + 10 + TYPE_ADD_AGG), 0);
    } else if(!IS_NPC(ch) && IS_MOD(ch, MOD_TUSKS) && number_range(1, 3) == 1) {
        one_hit(ch, victim, (TYPE_HIT + 10), 0);
    } else if(!IS_NPC(ch) && IS_MOD(ch, MOD_HORNS) && number_range(1, 5) == 1) {
        multi_hit(ch, victim, gsn_headbutt);
    }
    if(victim == NULL || victim->position != POS_FIGHTING) {
        return;
    }
    if(!IS_NPC(ch) && IS_WEREWOLF(ch) && wieldorig == 0 && get_tribe(ch, TRIBE_BLACK_FURIES) > 1) {
        spell_poison(gsn_poison, 25, ch, victim);
    }
    if(victim->itemaffect < 1) {
        return;
    }
    level = victim->spl[1] >> 2;
    /*
        if (IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) && ch->position == POS_FIGHTING)
        {
    	if ( (sn = skill_lookup( "lightning bolt" ) ) > 0)
    	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
        }
        if ( victim == NULL || victim->position == POS_DEAD ) return;
        if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD) && ch->position == POS_FIGHTING)
        {
    	if ( (sn = skill_lookup( "fireball" ) ) > 0)
    	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
        }
        if ( victim == NULL || victim->position == POS_DEAD ) return;
        if (IS_ITEMAFF(victim, ITEMA_ICESHIELD) && ch->position == POS_FIGHTING)
        {
    	if ( (sn = skill_lookup( "chill touch" ) ) > 0)
    	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
        }
        if ( victim == NULL || victim->position == POS_DEAD ) return;
        if (IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) && ch->position == POS_FIGHTING)
        {
    	if ( (sn = skill_lookup( "acid blast" ) ) > 0)
    	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
        }
        if ( victim == NULL || victim->position == POS_DEAD ) return;
        if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) && ch->position == POS_FIGHTING)
        {
    	if ( (sn = skill_lookup( "chaos blast" ) ) > 0)
    	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
        }
    */
    return;
}


int number_attacks(CHAR_DATA *ch) {
    int count = 1;
    if(ch->in_room == NULL || IS_SET(ch->in_room->added_flags, ROOM2_TIMESLOW)) {
        return count;
    }
    /*
        if ( IS_NPC(ch) )
        {
    	if (ch->level >= 50) count++;
    	if (ch->level >= 100) count++;
    	return count;
        }
    */
    if(IS_STANCE(ch, STANCE_VIPER) && number_percent() >
            ch->stance[STANCE_VIPER] * 0.5) {
        count++;
    } else if(IS_STANCE(ch, STANCE_MANTIS) && number_percent() >
              ch->stance[STANCE_MANTIS] * 0.5) {
        count++;
    } else if(IS_STANCE(ch, STANCE_TIGER) && number_percent() >
              ch->stance[STANCE_TIGER] * 0.5) {
        count++;
    }
    if(!IS_NPC(ch) && get_celerity(ch) > 0) {
        count += get_celerity(ch);
    }
    if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_SILENT_STRIDERS) > 0) {
        if(get_tribe(ch, TRIBE_SILENT_STRIDERS) > 2) {
            count += 2;
        } else {
            count++;
        }
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
        count++;
    }
    if(IS_ITEMAFF(ch, ITEMA_SPEED)) {
        count++;
    }
    return count;
}

int get_speed(CHAR_DATA *ch) {
    int count = number_range(11, 13);
    if(IS_STANCE(ch, STANCE_VIPER) && number_percent() >
            ch->stance[STANCE_VIPER] * 0.5) {
        count--;
    } else if(IS_STANCE(ch, STANCE_MANTIS) && number_percent() >
              ch->stance[STANCE_MANTIS] * 0.5) {
        count--;
    } else if(IS_STANCE(ch, STANCE_TIGER) && number_percent() >
              ch->stance[STANCE_TIGER] * 0.5) {
        count--;
    } else if(!IS_NPC(ch) && ch->stance[0] >= 11 && ch->stance[0] <= 15) {
        int stance = ch->stance[0];
        if(number_percent() > (ch->stance[stance] * 0.5) &&
                IS_SET(ch->pcdata->qstats[stance - 11], QS_SUPER_SPEED)) {
            count--;
        }
    }
    if(!IS_NPC(ch) && get_celerity(ch) > 0) {
        count -= (get_celerity(ch) << 1);
    }
    if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_SILENT_STRIDERS) > 0) {
        if(get_tribe(ch, TRIBE_SILENT_STRIDERS) > 2) {
            count -= 2;
        } else {
            count--;
        }
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
        count -= 3;
    }
    if(IS_ITEMAFF(ch, ITEMA_SPEED)) {
        count--;
    }
    if(count < 2) {
        count = 2;
    }
    if(ch->in_room == NULL || IS_SET(ch->in_room->added_flags, ROOM2_TIMESLOW)) {
        count *= 2;
    }
    return count;
}


/*
 * Hit one guy once.
 */
void one_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt, int handtype) {
    OBJ_DATA *wield;
    OBJ_DATA *gloves;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int level;
    int stance;
    bool right_hand;
    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if(victim == NULL || victim->position == POS_DEAD || ch->in_room != victim->in_room) {
        return;
    }
    /*
     * Figure out the type of damage message.
     */
    if(handtype == 2) {
        wield = get_eq_char(ch, WEAR_HOLD);
        right_hand = FALSE;
    } else {
        wield = get_eq_char(ch, WEAR_WIELD);
        right_hand = TRUE;
    }
    if(dt == TYPE_UNDEFINED) {
        dt = TYPE_HIT;
        if(wield != NULL && wield->item_type == ITEM_WEAPON) {
            dt += wield->value[3];
            if((right_hand && IS_ITEMAFF(ch, ITEMA_RIGHT_AGG)) ||
                    (!right_hand && IS_ITEMAFF(ch, ITEMA_LEFT_AGG))) {
                dt += TYPE_ADD_AGG;
            } else if(IS_WEREWOLF(victim)) {
                if((right_hand && IS_ITEMAFF(ch, ITEMA_RIGHT_SILVER)) ||
                        (!right_hand && IS_ITEMAFF(ch, ITEMA_LEFT_SILVER))) {
                    dt += TYPE_ADD_AGG;
                }
            }
        }
    }
    if(ch->wpn[dt - 1000] > 5) {
        level = (ch->wpn[dt - 1000] / 5);
    } else {
        level = 1;
    }
    if(level > 40) {
        level = 40;
    }
    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if(IS_NPC(ch)) {
        thac0_00 = 20;
        thac0_32 =  0;
    } else {
        thac0_00 = SKILL_THAC0_00;
        thac0_32 = SKILL_THAC0_32;
    }
    thac0     = interpolate(level, thac0_00, thac0_32) - char_hitroll(ch);
    victim_ac = UMAX(-100, char_ac(victim) / 10);
    if(!can_see(ch, victim)) {
        victim_ac -= 4;
    }
    /*
     * The moment of excitement!
     */
    while((diceroll = number_bits(5)) >= 20)
        ;
    if(diceroll == 0
            || (diceroll != 19 && diceroll < thac0 - victim_ac)) {
        /* Miss. */
        damage(ch, victim, 0, dt);
        tail_chain();
        if(!is_safe(ch, victim)) {
            improve_wpn(ch, dt, right_hand);
            improve_stance(ch);
        }
        return;
    }
    /*
     * Hit.
     * Calc damage.
     */
    if(IS_NPC(ch)) {
        dam = number_range(ch->level * 0.5, ch->level * 3 / 2);
        if(wield != NULL) {
            dam += dam * 0.5;
        }
    } else {
        if(IS_POLYAFF(ch, POLY_SERPENT)) {
            dam = number_range(25, 50);
        } else if(wield == NULL) {
            if(IS_MORE(ch, MORE_HAND_FLAME) && IS_VAMPAFF(ch, VAM_CLAWS)) {
                dam = number_range(25, 50);
                dt += TYPE_ADD_AGG;
            } else if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CLAWS) &&
                      get_auspice(ch, AUSPICE_AHROUN) > 0) {
                dam = number_range(25, 50);
                dt += TYPE_ADD_AGG;
            } else if(IS_MORE(ch, MORE_HAND_FLAME)) {
                dam = number_range(15, 30);
                dt += TYPE_ADD_AGG;
            } else if(IS_VAMPAFF(ch, VAM_CLAWS) && wield == NULL) {
                dam = number_range(10, 20);
                dt += TYPE_ADD_AGG;
            } else {
                dam = number_range(1, 4);
            }
        } else if(wield->item_type == ITEM_WEAPON) {
            dam = number_range(wield->value[1], wield->value[2]);
        } else {
            dam = number_range(1, 4);
        }
    }
    /*
     * Bonuses.
     */
    dam += char_damroll(ch);
    if(!IS_AWAKE(victim)) {
        dam *= 2;
    }
    if(dt == gsn_backstab) {
        dam *= number_range(2, 4);
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) &&
            IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT)) {
        dam *= 1.5;
    }
    if(dt >= TYPE_HIT) {
        dam += (dam * ((ch->wpn[dt - 1000] + 1) * 0.01));
    }
    if(!IS_NPC(victim) && IS_WEREWOLF(victim)) {
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            dam *= 0.5;
        }
        if(get_tribe(victim, TRIBE_BONE_GNAWERS) > 4) {
            dam *= 0.5;
        }
        if(get_tribe(victim, TRIBE_SILENT_STRIDERS) > 1) {
            dam *= 0.5;
        }
        if(wield != NULL && IS_SET(wield->spectype, SITEM_SILVER)) {
            dam *= 2;
            dt += TYPE_ADD_AGG;
        } else if((gloves = get_eq_char(ch, WEAR_HANDS)) != NULL
                  && IS_SET(gloves->spectype, SITEM_SILVER)) {
            dam *= 2;
            dt += TYPE_ADD_AGG;
        }
    }
    /*
        if ( !IS_NPC(ch) )
    */
    {
        stance = ch->stance[0];
        if(IS_STANCE(ch, STANCE_NORMAL)) {
            dam *= 1.25;
        } else {
            dam = dambonus(ch, victim, dam, stance);
        }
    }
    if(dam <= 0) {
        dam = 1;
    }
    damage(ch, victim, dam, dt);
    tail_chain();
    if(!is_safe(ch, victim)) {
        improve_wpn(ch, dt, right_hand);
        improve_stance(ch);
    }
    return;
}

bool can_counter(CHAR_DATA *ch) {
    if(IS_STANCE(ch, STANCE_MONKEY)) {
        return TRUE;
    } else if(IS_POLYAFF(ch, POLY_BAT)) {
        return TRUE;
    } else if(IS_POLYAFF(ch, POLY_RAVEN)) {
        return TRUE;
    }
    return FALSE;
}

bool can_bypass(CHAR_DATA *ch, CHAR_DATA *victim) {
    if(IS_STANCE(ch, STANCE_VIPER)) {
        return TRUE;
    } else if(IS_STANCE(ch, STANCE_MANTIS)) {
        return TRUE;
    } else if(IS_STANCE(ch, STANCE_TIGER)) {
        return TRUE;
    } else if(IS_POLYAFF(ch, POLY_SERPENT)) {
        return TRUE;
    } else if(IS_POLYAFF(ch, POLY_WOLF)) {
        return TRUE;
    } else if(!IS_NPC(ch) && ch->stance[0] >= 11 && ch->stance[0] <= 15) {
        int stance = ch->stance[0];
        if(IS_SET(ch->pcdata->qstats[stance - 11], QS_SUPER_BYPASS)) {
            return TRUE;
        }
    }
    return FALSE;
}

void update_damcap(CHAR_DATA *ch, CHAR_DATA *victim) {
    int max_dam = 1000;
    if(IS_NPC(ch)) {
        max_dam += (ch->level << 2);
    } else {
        max_dam += (ch->pcdata->rank * 100);
        switch(ch->class) {
        default:
            break;
        case CLASS_ABOMINATION:
            if(get_disc(ch, DISC_POTENCE) > 0) {
                max_dam += (get_disc(ch, DISC_POTENCE) * 100);
            }
            if(get_tribe(ch, TRIBE_SHADOW_LORDS) > 0) {
                max_dam += 100;
            }
            if(get_tribe(ch, TRIBE_GET_OF_FENRIS) > 1) {
                max_dam += 100;
            }
            if(IS_SET(ch->act, PLR_WOLFMAN)) {
                max_dam += ch->pcdata->wolf;
            }
            break;
        case CLASS_BAALI:
            if(ch->pcdata->wolf > 0) {
                max_dam += ch->beast;
            }
            if(get_disc(ch, DISC_POTENCE) > 0) {
                max_dam += (get_disc(ch, DISC_POTENCE) * 100);
            }
            if(IS_SET(ch->act, PLR_DEMON)) {
                max_dam += 500;
            } else if(IS_EXTRA(ch, EXTRA_PRINCE)) {
                max_dam += 400;
            } else if(IS_EXTRA(ch, EXTRA_SIRE)) {
                max_dam += 300;
            } else {
                max_dam += 200;
            }
            if(!IS_NPC(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT)) {
                max_dam += 200;
            }
            if(ch->in_room != NULL && ch->in_room->vnum == 30000) {
                max_dam += 5000;
            }
            max_dam += (ch->pcdata->demonic * 10);
            break;
        case CLASS_BLACK_SPIRAL_DANCER:
            if(get_tribe(ch, TRIBE_SHADOW_LORDS) > 0) {
                max_dam += 100;
            }
            if(get_tribe(ch, TRIBE_GET_OF_FENRIS) > 1) {
                max_dam += 100;
            }
            if(IS_SET(ch->act, PLR_WOLFMAN)) {
                max_dam += ch->pcdata->wolf;
            }
            if(get_disc(ch, DISC_POTENCE) > 0) {
                max_dam += (get_disc(ch, DISC_POTENCE) * 50);
            }
            if(IS_SET(ch->act, PLR_DEMON)) {
                max_dam += 500;
            } else if(IS_EXTRA(ch, EXTRA_PRINCE)) {
                max_dam += 400;
            } else if(IS_EXTRA(ch, EXTRA_SIRE)) {
                max_dam += 300;
            } else {
                max_dam += 200;
            }
            if(!IS_NPC(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT)) {
                max_dam += 200;
            }
            if(ch->in_room != NULL && ch->in_room->vnum == 30000) {
                max_dam += 5000;
            }
            max_dam += (ch->pcdata->demonic * 10);
            break;
        case CLASS_VAMPIRE:
        case CLASS_LICH:
            if(ch->pcdata->wolf > 0) {
                max_dam += ch->beast;
            }
            if(get_disc(ch, DISC_POTENCE) > 0) {
                max_dam += (get_disc(ch, DISC_POTENCE) * 100);
            }
            break;
        case CLASS_DEMON:
        case CLASS_ANGEL:
        case CLASS_NEPHANDI:
            if(IS_SET(ch->act, PLR_DEMON)) {
                max_dam += 500;
            } else if(IS_EXTRA(ch, EXTRA_PRINCE)) {
                max_dam += 400;
            } else if(IS_EXTRA(ch, EXTRA_SIRE)) {
                max_dam += 300;
            } else {
                max_dam += 200;
            }
            if(!IS_NPC(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT)) {
                max_dam += 200;
            }
            if(IS_ANGEL(ch)) {
                if(ch->in_room != NULL && ch->in_room->vnum == 30050) {
                    max_dam += 5000;
                }
            } else {
                if(ch->in_room != NULL && ch->in_room->vnum == 30000) {
                    max_dam += 5000;
                }
            }
            if(ch->class == CLASS_DEMON || ch->class == CLASS_ANGEL) {
                max_dam += (ch->pcdata->demonic * 20);
            } else {
                max_dam += (ch->pcdata->demonic * 10);
            }
            break;
        case CLASS_WEREWOLF:
        case CLASS_SKINDANCER:
            if(get_tribe(ch, TRIBE_SHADOW_LORDS) > 0) {
                max_dam += 100;
            }
            if(get_tribe(ch, TRIBE_GET_OF_FENRIS) > 1) {
                max_dam += 100;
            }
            if(IS_SET(ch->act, PLR_WOLFMAN)) {
                max_dam += ch->pcdata->wolf;
            }
            if(get_disc(ch, DISC_POTENCE) > 0) {
                max_dam += (get_disc(ch, DISC_POTENCE) * 50);
            }
            break;
        case CLASS_HIGHLANDER:
            if(IS_ITEMAFF(ch, ITEMA_HIGHLANDER)) {
                int wpn = ch->pcdata->disc[0];
                if(wpn == 1 || wpn == 3) {
                    max_dam += ch->wpn[wpn];
                }
            }
            break;
        }
        if(IS_ITEMAFF(ch, ITEMA_ARTIFACT)) {
            max_dam += 500;
        }
        if(IS_ITEMAFF(victim, ITEMA_ARTIFACT)) {
            max_dam -= 500;
        }
        if(victim->stance[0] != STANCE_MONKEY) {
            int stance = ch->stance[0];
            if(ch->stance[0] == STANCE_BULL) {
                max_dam += ch->stance[stance];
            } else if(ch->stance[0] == STANCE_DRAGON) {
                max_dam += ch->stance[stance];
            } else if(ch->stance[0] == STANCE_TIGER) {
                max_dam += ch->stance[stance];
            } else if(!IS_NPC(ch) && ch->stance[0] >= 11 && ch->stance[0] <= 15) {
                if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_LESSER)) {
                    max_dam += ch->stance[stance];
                } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_GREATER)) {
                    max_dam += (ch->stance[stance] * 2);
                } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_SUPREME)) {
                    max_dam += (ch->stance[stance] * 3);
                }
            }
        }
    }
    if(!IS_NPC(victim)) {
        if(get_disc(victim, DISC_FORTITUDE) > 0) {
            max_dam -= (get_disc(victim, DISC_FORTITUDE) * 50);
        }
        switch(victim->class) {
        default:
            break;
        case CLASS_ABOMINATION:
            if(IS_ITEMAFF(ch, ITEMA_SUNSHIELD)) {
                max_dam += 500;
            }
            if(IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_AHROUN) > 3 && IS_VAMPAFF(ch, VAM_CLAWS)) {
                max_dam += 200;
            } else {
                if(IS_ITEMAFF(ch, ITEMA_RIGHT_SILVER)) {
                    max_dam += 100;
                }
                if(IS_ITEMAFF(ch, ITEMA_LEFT_SILVER)) {
                    max_dam += 100;
                }
            }
            break;
        case CLASS_VAMPIRE:
        case CLASS_LICH:
            if(IS_ITEMAFF(ch, ITEMA_SUNSHIELD)) {
                max_dam += 500;
            }
            break;
        case CLASS_DEMON:
        case CLASS_NEPHANDI:
            if(victim->in_room != NULL && victim->in_room->vnum == 30000) {
                max_dam *= 0.2;
            }
            break;
        case CLASS_ANGEL:
            if(victim->in_room != NULL && victim->in_room->vnum == 30050) {
                max_dam *= 0.2;
            }
            break;
        case CLASS_WEREWOLF:
        case CLASS_SKINDANCER:
            if(IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_AHROUN) > 3 && IS_VAMPAFF(ch, VAM_CLAWS)) {
                max_dam += 200;
            } else {
                if(IS_ITEMAFF(ch, ITEMA_RIGHT_SILVER) && get_auspice(victim, AUSPICE_RAGABASH) < 5) {
                    max_dam += 250;
                }
                if(IS_ITEMAFF(ch, ITEMA_LEFT_SILVER) && get_auspice(victim, AUSPICE_RAGABASH) < 5) {
                    max_dam += 250;
                }
            }
            if(get_tribe(victim, TRIBE_BONE_GNAWERS) > 4) {
                max_dam -= 250;
            }
            break;
        }
        if(ch->stance[0] != STANCE_MONKEY) {
            int stance = victim->stance[0];
            if(victim->stance[0] == STANCE_CRAB) {
                max_dam -= victim->stance[stance];
            } else if(victim->stance[0] == STANCE_DRAGON) {
                max_dam -= victim->stance[stance];
            } else if(victim->stance[0] == STANCE_SWALLOW) {
                max_dam -= victim->stance[stance];
            } else if(!IS_NPC(victim) && stance >= 11 && stance <= 15) {
                if(IS_SET(victim->pcdata->qstats[stance - 11], QS_RES_LESSER)) {
                    max_dam += victim->stance[stance];
                } else if(IS_SET(victim->pcdata->qstats[stance - 11], QS_RES_GREATER)) {
                    max_dam += (victim->stance[stance] * 2);
                } else if(IS_SET(victim->pcdata->qstats[stance - 11], QS_RES_SUPREME)) {
                    max_dam += (victim->stance[stance] * 3);
                }
            }
        }
    }
    if(max_dam < 1000) {
        max_dam = 1000;
    }
    ch->damcap[DAM_CAP] = max_dam;
    ch->damcap[DAM_CHANGE] = 0;
    return;
}

/*
 * Inflict damage from a hit.
 */
void damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt) {
    /* For testing
        char buf[MAX_INPUT_LENGTH];
    */
    int max_dam = ch->damcap[DAM_CAP];
    bool agg_dam = FALSE;
    if(victim == NULL || victim->position == POS_DEAD) {
        return;
    }
    /*
     * Stop up any residual loopholes.
     */
    if(ch->damcap[DAM_CHANGE] == 1) {
        update_damcap(ch, victim);
    }
    /* For testing
        sprintf(buf,"Damage now %d.\n\r",dam);
        send_to_char(buf,ch);
    */
    if(dam > max_dam || dam < 0) {
        dam = max_dam;
    }
    if(victim != ch) {
        /*
         * Certain attacks are forbidden.
         * Most other attacks are returned.
         */
        if(is_safe(ch, victim)) {
            return;
        }
        check_killer(ch, victim);
        if(victim->position > POS_STUNNED) {
            if(victim->fighting == NULL) {
                set_fighting(victim, ch);
            }
            victim->position = POS_FIGHTING;
        }
        if(victim->position > POS_STUNNED) {
            if(ch->fighting == NULL) {
                set_fighting(ch, victim);
            }
            /*
             * If victim is charmed, ch might attack victim's master.

            if ( IS_NPC(ch)
            &&   IS_NPC(victim)
            &&   IS_AFFECTED(victim, AFF_CHARM)
            &&   victim->master != NULL
            &&   victim->master->in_room == ch->in_room
            &&   number_bits( 3 ) == 0 )
            {
            stop_fighting( ch, FALSE );
            multi_hit( ch, victim->master, TYPE_UNDEFINED );
            return;
            }

             */
        }
        /*
         * More charm stuff.
         */
        if(victim->master == ch) {
            stop_follower(victim);
        }
        /*
         * Damage modifiers.
         */
        if(IS_AFFECTED(ch, AFF_HIDE)) {
            if(!can_see(victim, ch)) {
                dam *= 1.5;
                send_to_char("You use your concealment to get a surprise attack!\n\r", ch);
            }
            REMOVE_BIT(ch->affected_by, AFF_HIDE);
            act("$n leaps from $s concealment.", ch, NULL, NULL, TO_ROOM);
        }
        if(!IS_IMMORTAL(ch) && !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_OBFUSCATE)) {
            send_to_char("You drop out of Obfuscate!\n\r", ch);
            if(!can_see(victim, ch)) {
                dam *= 1.5;
            }
            REMOVE_BIT(ch->vampaff, VAM_OBFUSCATE);
            act("$n fades into existance.", ch, NULL, NULL, TO_ROOM);
        }
        if(IS_AFFECTED(victim, AFF_SANCTUARY) && dam > 1) {
            dam *= 0.5;
        }
        if(IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1) {
            dam -= dam * 0.25;
        }
        if(dam < 0) {
            dam = 0;
        }
        /*
         * Check if damage is aggravated.
         */
        if(dt >= TYPE_ADD_AGG) {
            agg_dam = TRUE;
            dt -= TYPE_ADD_AGG;
            while(dt >= TYPE_ADD_AGG) {
                dt -= TYPE_ADD_AGG;
            }
        }
        /*
         * Check for disarm, trip, parry, and dodge.
         */
        if(dt >= TYPE_HIT) {
            if(IS_NPC(ch) && number_percent() < 2) {
                disarm(ch, victim);
            }
            /*
            	    if ( IS_NPC(ch) && number_percent( ) < ch->level * 0.5 )
            		trip( ch, victim );
            */
            if(check_parry(ch, victim, dt)) {
                return;
            }
            if(IS_STANCE(victim, STANCE_CRANE) &&
                    victim->stance[STANCE_CRANE] > 100 && !can_counter(ch)
                    && !can_bypass(ch, victim) && check_parry(ch, victim, dt)) {
                return;
            } else if(IS_STANCE(victim, STANCE_MANTIS) &&
                      victim->stance[STANCE_MANTIS] > 100 && !can_counter(ch)
                      && !can_bypass(ch, victim) && check_parry(ch, victim, dt)) {
                return;
            } else if(!IS_NPC(victim) && victim->stance[0] >= 11 &&
                      victim->stance[0] <= 15) {
                int stance = victim->stance[0];
                if(IS_SET(victim->pcdata->qstats[stance - 11], QS_SUPER_PARRY)) {
                    if(victim->stance[stance] > 100 && !can_counter(ch)
                            && !can_bypass(ch, victim) && check_parry(ch, victim, dt)) {
                        return;
                    }
                }
            }
            if(check_dodge(ch, victim, dt)) {
                return;
            }
            if(IS_STANCE(victim, STANCE_MONGOOSE) &&
                    victim->stance[STANCE_MONGOOSE] > 100 && !can_counter(ch)
                    && !can_bypass(ch, victim) && check_dodge(ch, victim, dt)) {
                return;
            } else if(IS_STANCE(victim, STANCE_SWALLOW) &&
                      victim->stance[STANCE_SWALLOW] > 100 && !can_counter(ch)
                      && !can_bypass(ch, victim) && check_dodge(ch, victim, dt)) {
                return;
            } else if(!IS_NPC(victim) && victim->stance[0] >= 11 &&
                      victim->stance[0] <= 15) {
                int stance = victim->stance[0];
                if(IS_SET(victim->pcdata->qstats[stance - 11], QS_SUPER_DODGE)) {
                    if(victim->stance[stance] > 100 && !can_counter(ch)
                            && !can_bypass(ch, victim) && check_dodge(ch, victim, dt)) {
                        return;
                    }
                }
            }
        }
        if(IS_POLYAFF(ch, POLY_BAT)) {
            dt = TYPE_HIT + 10;
            agg_dam = TRUE;
        } else if(IS_POLYAFF(ch, POLY_SERPENT)) {
            dt = gsn_tongue;
            agg_dam = TRUE;
        }
        dam_message(ch, victim, dam, dt);
        if(victim == NULL || victim->position == POS_DEAD) {
            return;
        }
    }
    hurt_person(ch, victim, dam, agg_dam);
    return;
}

void adv_damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam) {
    if(victim->position == POS_DEAD) {
        return;
    }
    if(dam > 1000) {
        dam = 1000;
    }
    if(victim != ch) {
        if(is_safe(ch, victim)) {
            return;
        }
        check_killer(ch, victim);
        if(victim->position > POS_STUNNED) {
            if(victim->fighting == NULL) {
                set_fighting(victim, ch);
            }
            victim->position = POS_FIGHTING;
        }
        if(victim->position > POS_STUNNED) {
            if(ch->fighting == NULL) {
                set_fighting(ch, victim);
            }
            /*
             * If victim is charmed, ch might attack victim's master.
             */
            if(IS_NPC(ch)
                    &&   IS_NPC(victim)
                    &&   IS_AFFECTED(victim, AFF_CHARM)
                    &&   victim->master != NULL
                    &&   victim->master->in_room == ch->in_room
                    &&   number_bits(3) == 0) {
                stop_fighting(ch, FALSE);
                multi_hit(ch, victim->master, TYPE_UNDEFINED);
                return;
            }
        }
        if(victim->master == ch) {
            stop_follower(victim);
        }
        if(IS_AFFECTED(victim, AFF_SANCTUARY) && dam > 1) {
            dam *= 0.5;
        }
        if(IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1) {
            dam -= (dam * 0.25);
        }
        if(dam < 0) {
            dam = 0;
        }
    }
    return;
}

void hurt_person(CHAR_DATA *ch, CHAR_DATA *victim, int dam, bool agg_dam) {
    bool is_npc = FALSE;
    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    if(agg_dam && victim->agg < 100) {
        victim->agg++;
    }
    if(agg_dam) {
        if(victim->position == POS_MORTAL) {
            victim->agg = 100;
        }
        if(victim->agg < 100) {
            victim->agg++;
        }
        if(!IS_NPC(victim)) {
            SET_BIT(victim->act, PLR_LAST_AGG);
        }
    } else if(!IS_NPC(victim)) {
        REMOVE_BIT(victim->act, PLR_LAST_AGG);
    }
    victim->hit -= dam;
    if(!IS_NPC(victim)
            &&   victim->level >= LEVEL_IMMORTAL
            &&   victim->hit < 1) {
        victim->hit = 1;
    }
    update_pos(victim);
    switch(victim->position) {
    case POS_MORTAL:
        if(IS_VAMPIRE(victim)) {
            act("$n is mortally wounded, and spraying blood everywhere.",
                victim, NULL, NULL, TO_ROOM);
            send_to_char("You have been torpored.\n\r", victim);
        } else {
            act("$n is mortally wounded, and spraying blood everywhere.",
                victim, NULL, NULL, TO_ROOM);
            send_to_char(
                "You are mortally wounded, and spraying blood everywhere.\n\r",
                victim);
        }
        break;
    case POS_INCAP:
        act("$n is incapacitated, and bleeding badly.",
            victim, NULL, NULL, TO_ROOM);
        send_to_char(
            "You are incapacitated, and bleeding badly.\n\r",
            victim);
        break;
    case POS_STUNNED:
        act("$n is stunned, but will soon recover.",
            victim, NULL, NULL, TO_ROOM);
        send_to_char("You are stunned, but will soon recover.\n\r",
                     victim);
        break;
    case POS_DEAD:
        act("$n is DEAD!!", victim, 0, 0, TO_ROOM);
        send_to_char("You have been KILLED!!\n\r", victim);
        break;
    default:
        if(dam > victim->max_hit * 0.25) {
            send_to_char("That really did HURT!\n\r", victim);
        }
        if(victim->hit < victim->max_hit * 0.25 && dam > 0) {
            if(!IS_NPC(victim) && IS_VAMPIRE(victim)
                    && number_percent() < victim->beast) {
                vamp_rage(victim);
            } else {
                send_to_char("You sure are BLEEDING!\n\r", victim);
            }
        }
        break;
    }
    /*
     * Sleep spells and extremely wounded folks.
     */
    if(!IS_AWAKE(victim)) {
        stop_fighting(victim, FALSE);
    }
    /*
     * Payoff for killing things.
     */
    if(victim->position == POS_DEAD) {
        group_gain(ch, victim);
        if(!IS_NPC(victim)) {
            sprintf(log_buf, "%s killed by %s at %d",
                    victim->name,
                    (IS_NPC(ch) ? ch->short_descr : ch->name),
                    victim->in_room->vnum);
            log_string(log_buf);
        }
        demon_gain(ch, victim);
        if(!IS_NPC(victim) && IS_NPC(ch)) {
            victim->mdeath++;
        }
        if(IS_NPC(victim)) {
            is_npc = TRUE;
        }
        raw_kill(victim, ch);
        if(IS_SET(ch->act, PLR_AUTOLOOT)) {
            do_get(ch, "all corpse");
        } else {
            do_look(ch, "in corpse");
        }
        if(!IS_NPC(ch) && is_npc) {
            if(IS_SET(ch->act, PLR_AUTOSAC)) {
                do_sacrifice(ch, "corpse");
            }
        }
        return;
    }
    if(victim == ch) {
        return;
    }
    /*
     * Take care of link dead people.
     */
    if(!IS_NPC(victim) && victim->desc == NULL) {
        if(number_range(0, victim->wait) == 0) {
            do_recall(victim, "");
            return;
        }
    }
    /*
     * Wimp out?
     */
    if(IS_NPC(victim) && dam > 0) {
        if((IS_SET(victim->act, ACT_WIMPY) && number_bits(1) == 0
                &&   victim->hit < victim->max_hit * 0.5)
                || (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
                    &&     victim->master->in_room != victim->in_room)) {
            do_flee(victim, "");
        }
    } else if(!IS_NPC(victim)
              &&   victim->hit > 0
              &&   victim->hit <= victim->wimpy
              &&   victim->wait == 0) {
        do_flee(victim, "");
    } else if(IS_AFFECTED(victim, AFF_FEAR)) {
        do_flee(victim, "");
    }
    tail_chain();
    return;
}



bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim) {
    /* Ethereal people can only attack other ethereal people */
    if(ch->in_room != victim->in_room) {
        return TRUE;
    }
    if(!IS_NPC(ch) && get_trust(ch) == LEVEL_BUILDER) {
        send_to_char("Don't fight, build.\n\r", ch);
        return TRUE;
    }
    if(!IS_NPC(victim) && get_trust(victim) == LEVEL_BUILDER) {
        send_to_char("They build, not fight.\n\r", ch);
        return TRUE;
    }
    if(IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL)) {
        send_to_char("You cannot while ethereal.\n\r", ch);
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL)) {
        send_to_char("You cannot fight an ethereal person.\n\r", ch);
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_MIST) || IS_POLYAFF(victim, POLY_MIST)) {
        send_to_char("People in mist form may not enter combat.\n\r", ch);
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_SHADOW) || IS_POLYAFF(victim, POLY_SHADOW)) {
        send_to_char("People in smoke form may not enter combat.\n\r", ch);
        return TRUE;
    }
    /* You cannot attack across planes */
    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        act("You are too insubstantial!", ch, NULL, victim, TO_CHAR);
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        act("$E is too insubstantial!", ch, NULL, victim, TO_CHAR);
        return TRUE;
    }
    if(ch->in_room == NULL || IS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
        send_to_char("Something in the air prevents you from attacking.\n\r", ch);
        return TRUE;
    }
    if(victim == NULL || victim->in_room == NULL) {
        return TRUE;
    }
    if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
        send_to_char("Something in the air prevents you from attacking.\n\r", ch);
        return TRUE;
    }
    if(IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)) {
        send_to_char("Objects cannot fight!\n\r", ch);
        return TRUE;
    } else if(IS_HEAD(victim, LOST_HEAD) || IS_EXTRA(victim, EXTRA_OSWITCH)) {
        send_to_char("You cannot attack an object.\n\r", ch);
        return TRUE;
    }
    if(IS_VAMPAFF(ch, VAM_EARTHMELDED) || IS_VAMPAFF(victim, VAM_EARTHMELDED)) {
        return TRUE;
    }
    /* Thx Josh! */
    if(victim->fighting == ch) {
        return FALSE;
    }
    if(IS_NPC(ch) && IS_NEWBIE(victim) && IS_SET(ch->act, ACT_COMMANDED)) {
        send_to_char("Newbie killing of ANY form is forbidden.\n\r", ch);
        return TRUE;
    }
    if(IS_NPC(victim) && IS_NEWBIE(ch) && IS_SET(victim->act, ACT_COMMANDED)) {
        send_to_char("Newbies cannot attack player-controlled mobs.\n\r", ch);
        return TRUE;
    }
    if(IS_NPC(ch) && IS_MORE(victim, MORE_ANIMAL_MASTER)) {
        send_to_char("You like them too much to attack them.\n\r", ch);
        return TRUE;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        send_to_char("You are unable to attack them.\n\r", ch);
        return TRUE;
    }
    if(IS_ITEMAFF(victim, ITEMA_PEACE)) {
        if(IS_WEREWOLF(victim)) {
            send_to_char("They look FAR too scary to attack!\n\r", ch);
        } else {
            send_to_char("They look FAR too imposing to attack!\n\r", ch);
        }
        return TRUE;
    }
    if(IS_NPC(ch) || IS_NPC(victim)) {
        return FALSE;
    }
    if(ch->trust > LEVEL_BUILDER) {
        send_to_char("You cannot fight if you have implementor powers!\n\r", ch);
        return TRUE;
    }
    if(victim->trust > LEVEL_BUILDER) {
        send_to_char("You cannot fight someone with implementor powers!\n\r", ch);
        return TRUE;
    }
    if(!CAN_PK(ch) || !CAN_PK(victim)) {
        send_to_char("Mortals are not permitted to fight other players.\n\r", ch);
        return TRUE;
    }
    if(ch->desc == NULL || victim->desc == NULL) {
        if(victim->position != POS_FIGHTING && victim->position > POS_STUNNED) {
            send_to_char("They are currently link dead.\n\r", ch);
            return TRUE;
        }
    }
    if(!IS_NPC(ch) && !IS_NPC(victim)) {
        if(IS_NEWBIE(ch)) {
            send_to_char("You cannot attack players while you are a newbie.\n\r", ch);
            return TRUE;
        } else if(IS_NEWBIE(victim)) {
            send_to_char("You cannot attack newbies.\n\r", ch);
            return TRUE;
        }
        if(IS_MORE(ch, MORE_LOYAL) && strlen(ch->pcdata->conding) > 1 &&
                !str_cmp(ch->pcdata->conding, victim->name)) {
            send_to_char("You refuse to attack your master!\n\r", ch);
            return TRUE;
        } else if(strlen(ch->pcdata->love) > 1 &&
                  !str_cmp(ch->pcdata->love, victim->name)) {
            send_to_char("How could you attack someone you love?!\n\r", ch);
            return TRUE;
        }
    }
    return FALSE;
}

bool auto_safe(CHAR_DATA *ch, CHAR_DATA *victim) {
    if(ch->in_room != victim->in_room) {
        return TRUE;
    }
    if(!IS_NPC(ch) && get_trust(ch) == LEVEL_BUILDER) {
        return TRUE;
    }
    if(!IS_NPC(victim) && get_trust(victim) == LEVEL_BUILDER) {
        return TRUE;
    }
    if(IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL)) {
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL)) {
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_MIST) || IS_POLYAFF(victim, POLY_MIST)) {
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_SHADOW) || IS_POLYAFF(victim, POLY_SHADOW)) {
        return TRUE;
    }
    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        return TRUE;
    }
    if(ch->in_room == NULL || IS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
        return TRUE;
    }
    if(victim == NULL || victim->in_room == NULL) {
        return TRUE;
    }
    if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
        return TRUE;
    }
    if(IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)) {
        return TRUE;
    } else if(IS_HEAD(victim, LOST_HEAD) || IS_EXTRA(victim, EXTRA_OSWITCH)) {
        return TRUE;
    }
    if(IS_VAMPAFF(ch, VAM_EARTHMELDED) || IS_VAMPAFF(victim, VAM_EARTHMELDED)) {
        return TRUE;
    }
    if(victim->fighting == ch) {
        return FALSE;
    }
    if(IS_NPC(ch) && IS_NEWBIE(victim) && IS_SET(ch->act, ACT_COMMANDED)) {
        return TRUE;
    }
    if(IS_NPC(victim) && IS_NEWBIE(ch) && IS_SET(victim->act, ACT_COMMANDED)) {
        return TRUE;
    }
    if(IS_NPC(ch) && IS_MORE(victim, MORE_ANIMAL_MASTER)) {
        return TRUE;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        return TRUE;
    }
    if(IS_ITEMAFF(victim, ITEMA_PEACE)) {
        return TRUE;
    }
    if(IS_NPC(ch) || IS_NPC(victim)) {
        return FALSE;
    }
    if(ch->trust > LEVEL_BUILDER) {
        return TRUE;
    }
    if(victim->trust > LEVEL_BUILDER) {
        return TRUE;
    }
    if(!CAN_PK(ch) || !CAN_PK(victim)) {
        return TRUE;
    }
    if(ch->desc == NULL || victim->desc == NULL) {
        if(victim->position != POS_FIGHTING && victim->position > POS_STUNNED) {
            return TRUE;
        }
    }
    if(!IS_NPC(ch) && !IS_NPC(victim)) {
        if(IS_NEWBIE(ch)) {
            return TRUE;
        } else if(IS_NEWBIE(victim)) {
            return TRUE;
        }
        if(IS_MORE(ch, MORE_LOYAL) && strlen(ch->pcdata->conding) > 1 &&
                !str_cmp(ch->pcdata->conding, victim->name)) {
            return TRUE;
        } else if(strlen(ch->pcdata->love) > 1 &&
                  !str_cmp(ch->pcdata->love, victim->name)) {
            return TRUE;
        }
    }
    return FALSE;
}

bool no_attack(CHAR_DATA *ch, CHAR_DATA *victim) {
    if(ch->in_room != victim->in_room) {
        return TRUE;
    }
    /* Ethereal people can only attack other ethereal people */
    if(!IS_NPC(ch) && get_trust(ch) == LEVEL_BUILDER) {
        send_to_char("Don't fight, build.\n\r", ch);
        return TRUE;
    }
    if(!IS_NPC(victim) && get_trust(victim) == LEVEL_BUILDER) {
        send_to_char("They build, not fight.\n\r", ch);
        return TRUE;
    }
    if(IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL)) {
        send_to_char("You cannot while ethereal.\n\r", ch);
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL)) {
        send_to_char("You cannot fight an ethereal person.\n\r", ch);
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_MIST) || IS_POLYAFF(victim, POLY_MIST)) {
        send_to_char("People in mist form may not enter combat.\n\r", ch);
        return TRUE;
    }
    if(IS_POLYAFF(ch, POLY_SHADOW) || IS_POLYAFF(victim, POLY_SHADOW)) {
        send_to_char("People in smoke form may not enter combat.\n\r", ch);
        return TRUE;
    }
    /* You cannot attack across planes */
    if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        act("You are too insubstantial!", ch, NULL, victim, TO_CHAR);
        return TRUE;
    }
    if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE)) {
        act("$E is too insubstantial!", ch, NULL, victim, TO_CHAR);
        return TRUE;
    }
    if(ch->in_room == NULL || IS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
        send_to_char("Something in the air prevents you from attacking.\n\r", ch);
        return TRUE;
    }
    if(IS_ITEMAFF(victim, ITEMA_PEACE)) {
        if(IS_WEREWOLF(victim)) {
            send_to_char("They look FAR too scary to attack!\n\r", ch);
        } else {
            send_to_char("They look FAR too imposing to attack!\n\r", ch);
        }
        return TRUE;
    }
    if(victim == NULL || victim->in_room == NULL) {
        return TRUE;
    }
    if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)) {
        send_to_char("Something in the air prevents you from attacking.\n\r", ch);
        return TRUE;
    }
    if(IS_VAMPAFF(ch, VAM_EARTHMELDED) || IS_VAMPAFF(victim, VAM_EARTHMELDED)) {
        return TRUE;
    }
    if(IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)) {
        send_to_char("Objects cannot fight!\n\r", ch);
        return TRUE;
    } else if(IS_HEAD(victim, LOST_HEAD) || IS_EXTRA(victim, EXTRA_OSWITCH)) {
        send_to_char("You cannot attack objects.\n\r", ch);
        return TRUE;
    }
    if(victim->fighting == ch) {
        return FALSE;
    }
    if(IS_NPC(ch) && IS_NEWBIE(victim) && IS_SET(ch->act, ACT_COMMANDED)) {
        send_to_char("Newbie killing of ANY form is forbidden.\n\r", ch);
        return TRUE;
    }
    if(IS_NPC(victim) && IS_NEWBIE(ch) && IS_SET(victim->act, ACT_COMMANDED)) {
        send_to_char("Newbies cannot attack player-controlled mobs.\n\r", ch);
        return TRUE;
    }
    if(IS_NPC(ch) && IS_MORE(victim, MORE_ANIMAL_MASTER)) {
        send_to_char("You like them too much to attack them.\n\r", ch);
        return TRUE;
    }
    return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer(CHAR_DATA *ch, CHAR_DATA *victim) {
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while(IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL) {
        victim = victim->master;
    }
    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if(IS_NPC(victim)) {
        return;
    }
    /*
     * Charm-o-rama.
     */
    if(IS_SET(ch->affected_by, AFF_CHARM)) {
        if(ch->master == NULL) {
            affect_strip(ch, gsn_charm_person);
            REMOVE_BIT(ch->affected_by, AFF_CHARM);
            return;
        }
        stop_follower(ch);
        return;
    }
    return;
}



/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA *ch, CHAR_DATA *victim, int dt) {
    OBJ_DATA *obj = NULL;
    int chance = 0;
    bool claws = FALSE;
    int num_hands = 0;
    bool no_parry = FALSE;
    if(!IS_AWAKE(victim) || IS_POLYAFF(victim, POLY_SERPENT) ||
            IS_POLYAFF(victim, POLY_BAT) || IS_POLYAFF(victim, POLY_RAVEN)) {
        return FALSE;
    }
    if(IS_NPC(victim)) {
        obj = NULL;
    } else {
        if(get_eq_char(victim, WEAR_WIELD) != NULL) {
            num_hands++;
        }
        if(get_eq_char(victim, WEAR_HOLD)  != NULL) {
            num_hands++;
        }
        if((obj = get_eq_char(victim, WEAR_WIELD)) == NULL
                || obj->item_type != ITEM_WEAPON || number_percent() <= 50) {
            if((obj = get_eq_char(victim, WEAR_HOLD)) == NULL
                    || obj->item_type != ITEM_WEAPON) {
                if((obj = get_eq_char(victim, WEAR_WIELD)) == NULL
                        || obj->item_type != ITEM_WEAPON) {
                    if(num_hands < 2 && IS_VAMPAFF(victim, VAM_CLAWS)) {
                        if((IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_RED_TALONS) > 2)
                                || ((IS_VAMPIRE(victim) || IS_GHOUL(victim)) && get_disc(victim, DISC_PROTEAN) >= 2)) {
                            obj = NULL;
                            claws = TRUE;
                        } else {
                            no_parry = TRUE;
                        }
                    } else {
                        no_parry = TRUE;
                    }
                }
            }
            if(no_parry) {
                return FALSE;
            }
        }
    }
    if(dt < 1000 || dt > 1012) {
        return FALSE;
    }
    if(!IS_NPC(ch) || ch->wpn[0] > 0) {
        chance -= (ch->wpn[dt - 1000] * 0.1);
    } else {
        chance -= (ch->level * 0.2);
    }
    if(!IS_NPC(ch) || victim->wpn[0] > 0) {
        chance += (victim->wpn[dt - 1000] * 0.5);
    } else {
        chance += victim->level;
    }
    if(IS_STANCE(victim, STANCE_CRANE) &&
            victim->stance[STANCE_CRANE] > 0 && !can_counter(ch) &&
            !can_bypass(ch, victim)) {
        chance += (victim->stance[STANCE_CRANE] * 0.25);
    } else if(IS_STANCE(victim, STANCE_MANTIS) &&
              victim->stance[STANCE_MANTIS] > 0 && !can_counter(ch) &&
              !can_bypass(ch, victim)) {
        chance += (victim->stance[STANCE_MANTIS] * 0.25);
    } else if(!IS_NPC(victim) && victim->stance[0] >= 11 &&
              victim->stance[0] <= 15) {
        int stance = victim->stance[0];
        if(IS_SET(victim->pcdata->qstats[stance - 11], QS_SUPER_PARRY)) {
            if(victim->stance[stance] > 100 && !can_counter(ch)
                    && !can_bypass(ch, victim)) {
                chance += (victim->stance[stance] * 0.25);
            }
        }
    }
    chance -= (char_hitroll(ch) * 0.1);
    if(claws) {
        if((IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_RED_TALONS) > 3) ||
                (get_disc(victim, DISC_PROTEAN) >= 5)) {
            chance += (char_hitroll(victim) * 0.1);
        } else if((IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_RED_TALONS) > 2) ||
                  (get_disc(victim, DISC_PROTEAN) >= 4)) {
            chance += (char_hitroll(victim) * 0.075);
        } else if(get_disc(victim, DISC_PROTEAN) >= 3) {
            chance += (char_hitroll(victim) * 0.050);
        } else {
            chance += (char_hitroll(victim) * 0.025);
        }
    } else {
        chance += (char_hitroll(victim) * 0.1);
    }
    if(!IS_NPC(ch)) {
        if(get_celerity(ch) > 0) {
            chance -= get_celerity(ch) * 3;
        } else if(IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
            if(IS_SET(ch->act, PLR_DEMON)) {
                chance -= 30;
            } else if(IS_EXTRA(ch, EXTRA_PRINCE)) {
                chance -= 25;
            } else if(IS_EXTRA(ch, EXTRA_SIRE)) {
                chance -= 20;
            } else {
                chance -= 15;
            }
        }
        if(IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 3) {
            chance -= ((get_auspice(ch, AUSPICE_PHILODOX) - 2) * 5);
        }
    }
    if(!IS_NPC(victim)) {
        if(get_celerity(victim) > 0) {
            chance += get_celerity(victim) * 3;
        } else if(IS_DEMON(victim) && IS_SET(victim->pcdata->disc[C_POWERS], DEM_SPEED)) {
            if(IS_SET(victim->act, PLR_DEMON)) {
                chance += 30;
            } else if(IS_EXTRA(victim, EXTRA_PRINCE)) {
                chance += 25;
            } else if(IS_EXTRA(victim, EXTRA_SIRE)) {
                chance += 20;
            } else {
                chance += 15;
            }
        }
        if(!IS_NPC(victim) && IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_SILENT_STRIDERS) > 0) {
            if(get_tribe(victim, TRIBE_SILENT_STRIDERS) > 2) {
                chance += 10;
            } else {
                chance += 5;
            }
        }
    }
    if(chance > 75) {
        chance = 75;
    } else if(chance < 25) {
        chance = 25;
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
        chance -= 20;
    }
    if(!IS_NPC(victim) && IS_DEMON(victim) && IS_SET(victim->pcdata->disc[C_POWERS], DEM_SPEED)) {
        chance += 20;
    }
    if(number_percent() < 100 && number_percent() >= chance) {
        return FALSE;
    }
    if(claws) {
        if(IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF)) {
            act("You parry $n's blow with your claws.",  ch, NULL, victim, TO_VICT);
        }
        if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF)) {
            act("$N parries your blow with $S claws.", ch, NULL, victim, TO_CHAR);
        }
        return TRUE;
    }
    if(!IS_NPC(victim) && obj != NULL && obj->item_type == ITEM_WEAPON &&
            obj->value[3] >= 0 && obj->value[3] <= 12) {
        if(IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF)) {
            act("You parry $n's blow with $p.",  ch, obj, victim, TO_VICT);
        }
        if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF)) {
            act("$N parries your blow with $p.", ch, obj, victim, TO_CHAR);
        }
        return TRUE;
    }
    if(IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF)) {
        act("You parry $n's blow.",  ch, NULL, victim, TO_VICT);
    }
    if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF)) {
        act("$N parries your blow.", ch, NULL, victim, TO_CHAR);
    }
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA *ch, CHAR_DATA *victim, int dt) {
    int chance = 0;
    int dodge_type = number_range(1, 9);
    if(!IS_AWAKE(victim)) {
        return FALSE;
    }
    if(!IS_NPC(ch) || ch->wpn[0] > 0) {
        chance -= (ch->wpn[dt - 1000] * 0.1);
    } else {
        chance -= (ch->level * 0.2);
    }
    if(!IS_NPC(victim) || victim->wpn[0] > 0) {
        chance += (victim->wpn[0] * 0.5);
    } else {
        chance += victim->level;
    }
    if(IS_POLYAFF(victim, POLY_SERPENT)) {
        chance += 50;
    } else if(IS_POLYAFF(victim, POLY_BAT)) {
        chance += 100;
    } else if(IS_POLYAFF(victim, POLY_RAVEN)) {
        chance += 100;
    } else if(IS_STANCE(victim, STANCE_MONGOOSE) &&
              victim->stance[STANCE_MONGOOSE] > 0 && !can_counter(ch) &&
              !can_bypass(ch, victim)) {
        (chance += victim->stance[STANCE_MONGOOSE] * 0.25);
    } else if(IS_STANCE(victim, STANCE_SWALLOW) &&
              victim->stance[STANCE_SWALLOW] > 0 && !can_counter(ch) &&
              !can_bypass(ch, victim)) {
        (chance += victim->stance[STANCE_SWALLOW] * 0.25);
    } else if(!IS_NPC(victim) && victim->stance[0] >= 11 &&
              victim->stance[0] <= 15) {
        int stance = victim->stance[0];
        if(IS_SET(victim->pcdata->qstats[stance - 11], QS_SUPER_DODGE)) {
            if(victim->stance[stance] > 100 && !can_counter(ch)
                    && !can_bypass(ch, victim)) {
                chance += (victim->stance[stance] * 0.25);
            }
        }
    }
    if(!IS_NPC(ch)) {
        if(get_celerity(ch) > 0) {
            chance -= get_celerity(ch) * 3;
        } else if(IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
            if(IS_SET(ch->act, PLR_DEMON)) {
                chance -= 30;
            } else if(IS_EXTRA(ch, EXTRA_PRINCE)) {
                chance -= 25;
            } else if(IS_EXTRA(ch, EXTRA_SIRE)) {
                chance -= 20;
            } else {
                chance -= 15;
            }
        }
        if(IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_PHILODOX) > 3) {
            chance -= ((get_auspice(ch, AUSPICE_PHILODOX) - 2) * 5);
        }
    }
    if(!IS_NPC(victim)) {
        if(get_celerity(victim) > 0) {
            chance += get_celerity(victim) * 3;
        } else if(IS_DEMON(victim) && IS_SET(victim->pcdata->disc[C_POWERS], DEM_SPEED)) {
            if(IS_SET(victim->act, PLR_DEMON)) {
                chance += 30;
            } else if(IS_EXTRA(victim, EXTRA_PRINCE)) {
                chance += 25;
            } else if(IS_EXTRA(victim, EXTRA_SIRE)) {
                chance += 20;
            } else {
                chance += 15;
            }
        }
        if(!IS_NPC(victim) && IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_SILENT_STRIDERS) > 0) {
            if(get_tribe(victim, TRIBE_SILENT_STRIDERS) > 2) {
                chance += 10;
            } else {
                chance += 5;
            }
        }
        if(IS_WEREWOLF(victim) && get_breed(ch, BREED_LUPUS) > 2) {
            chance += ((get_breed(ch, BREED_LUPUS) - 2) * 5);
        }
    }
    if(chance > 75) {
        chance = 75;
    } else if(chance < 25) {
        chance = 25;
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) && IS_SET(ch->pcdata->disc[C_POWERS], DEM_SPEED)) {
        chance -= 20;
    }
    if(!IS_NPC(victim) && IS_DEMON(victim) && IS_SET(victim->pcdata->disc[C_POWERS], DEM_SPEED)) {
        chance += 20;
    }
    if(number_percent() < 100 && number_percent() >= chance) {
        return FALSE;
    }
    if(IS_POLYAFF(victim, POLY_BAT) || IS_POLYAFF(victim, POLY_SERPENT)) {
        dodge_type = 0;
    }
    if(IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF)) {
        switch(dodge_type) {
        default:
            act("You dodge $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        case 1:
            act("You duck under $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            act("You sidestep $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        case 7:
            act("You jump over $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        case 8:
            act("You leap out of the way of $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        case 9:
            act("You roll under $n's blow.", ch, NULL, victim, TO_VICT);
            break;
        }
    }
    if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF)) {
        switch(dodge_type) {
        default:
            act("$N dodges your blow.", ch, NULL, victim, TO_CHAR);
            break;
        case 1:
            act("$N ducks under your blow.", ch, NULL, victim, TO_CHAR);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            act("$N sidesteps your blow.", ch, NULL, victim, TO_CHAR);
            break;
        case 7:
            act("$N jumps over your blow.", ch, NULL, victim, TO_CHAR);
            break;
        case 8:
            act("$N leaps out of the way of your blow.", ch, NULL, victim, TO_CHAR);
            break;
        case 9:
            act("$N rolls under your blow.", ch, NULL, victim, TO_CHAR);
            break;
        }
    }
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA *victim) {
    CHAR_DATA *mount;
    if(victim->hit > 0) {
        if(victim->position <= POS_STUNNED) {
            bool gm_stance = FALSE;
            victim->position = POS_STANDING;
            if(victim->stance[0] > 0) {
                int stance = victim->stance[0];
                if(victim->stance[stance] >= 200) {
                    gm_stance = TRUE;
                }
            }
            if(victim->max_hit * 0.25 > victim->hit ||
                    !gm_stance) {
                act("$n clambers back to $s feet.", victim, NULL, NULL, TO_ROOM);
                act("You clamber back to your feet.", victim, NULL, NULL, TO_CHAR);
            } else {
                act("$n flips back up to $s feet.", victim, NULL, NULL, TO_ROOM);
                act("You flip back up to your feet.", victim, NULL, NULL, TO_CHAR);
            }
        }
        return;
    } else if((mount = victim->mount) != NULL) {
        if(victim->mounted == IS_MOUNT) {
            act("$n rolls off $N.", mount, NULL, victim, TO_ROOM);
            act("You roll off $N.", mount, NULL, victim, TO_CHAR);
        } else if(victim->mounted == IS_RIDING) {
            act("$n falls off $N.", victim, NULL, mount, TO_ROOM);
            act("You fall off $N.", victim, NULL, mount, TO_CHAR);
        }
        mount->mount    = NULL;
        victim->mount   = NULL;
        mount->mounted  = IS_ON_FOOT;
        victim->mounted = IS_ON_FOOT;
    }
    if(!IS_NPC(victim) && victim->hit <= -11 && is_undying(victim) && !IS_SET(victim->act, PLR_LAST_AGG)) {
        victim->hit = -10;
        if(victim->position == POS_FIGHTING) {
            stop_fighting(victim, TRUE);
        }
        return;
    }
    if(IS_NPC(victim) || victim->hit <= -11) {
        victim->position = POS_DEAD;
        return;
    }
    if(victim->hit <= -6) {
        victim->position = POS_MORTAL;
    } else if(victim->hit <= -3) {
        victim->position = POS_INCAP;
    } else {
        victim->position = POS_STUNNED;
    }
    if(victim->sat != NULL && !CAN_SIT(victim)) {
        victim->sat->sat = NULL;
        victim->sat = NULL;
    }
    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA *ch, CHAR_DATA *maker) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;
    if(IS_NPC(ch)) {
        name		= ch->short_descr;
        corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
        corpse->timer	= number_range(60, 120);
        corpse->value[2] = ch->pIndexData->vnum;
        if(ch->gold > 0) {
            obj_to_obj(create_money(ch->gold), corpse);
            ch->gold = 0;
        }
    } else {
        name		= ch->name;
        corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
        corpse->timer	= number_range(600, 1200);
        /* Why should players keep their gold? */
        if(ch->gold > 0) {
            obj = create_money(ch->gold);
            if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                    (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))) {
                SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
            }
            obj_to_obj(obj, corpse);
            ch->gold = 0;
        }
    }
    free_string(corpse->questmaker);
    if(IS_NPC(maker)) {
        corpse->questmaker = str_dup(maker->short_descr);
    } else {
        corpse->questmaker = str_dup(maker->name);
    }
    free_string(corpse->questowner);
    if(IS_NPC(ch)) {
        corpse->questowner = str_dup(ch->short_descr);
    } else {
        corpse->questowner = str_dup(ch->name);
    }
    if(ch->in_room != NULL) {
        corpse->specpower = ch->in_room->vnum;
    }
    sprintf(buf, corpse->short_descr, name);
    free_string(corpse->short_descr);
    corpse->short_descr = str_dup(buf);
    sprintf(buf, corpse->description, name);
    free_string(corpse->description);
    corpse->description = str_dup(buf);
    for(obj = ch->carrying; obj != NULL; obj = obj_next) {
        obj_next = obj->next_content;
        obj_from_char(obj);
        if(IS_SET(obj->extra_flags, ITEM_VANISH)) {
            extract_obj(obj);
        } else {
            if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
                    (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))) {
                SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
            }
            obj_to_obj(obj, corpse);
        }
    }
    /* If you die in the shadowplane, your corpse should stay there */
    if(IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
        SET_BIT(corpse->extra_flags, ITEM_SHADOWPLANE);
    }
    obj_to_room(corpse, ch->in_room);
    return;
}



void death_cry(CHAR_DATA *ch) {
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    if(IS_NPC(ch)) {
        msg = "You hear something's death cry.";
    } else {
        msg = "You hear someone's death cry.";
    }
    was_in_room = ch->in_room;
    for(door = 0; door <= 5; door++) {
        EXIT_DATA *pexit;
        if((pexit = was_in_room->exit[door]) != NULL
                &&   pexit->to_room != NULL
                &&   pexit->to_room != was_in_room) {
            ch->in_room = pexit->to_room;
            act(msg, ch, NULL, NULL, TO_ROOM);
        }
    }
    ch->in_room = was_in_room;
    return;
}

void make_part(CHAR_DATA *ch, char *argument) {
    char  arg [MAX_INPUT_LENGTH];
    int vnum;
    argument = one_argument(argument, arg);
    vnum = 0;
    if(arg[0] == '\0') {
        return;
    }
    if(!str_cmp(arg, "head")) {
        vnum = OBJ_VNUM_SEVERED_HEAD;
    } else if(!str_cmp(arg, "arm")) {
        vnum = OBJ_VNUM_SLICED_ARM;
    } else if(!str_cmp(arg, "leg")) {
        vnum = OBJ_VNUM_SLICED_LEG;
    } else if(!str_cmp(arg, "heart")) {
        vnum = OBJ_VNUM_TORN_HEART;
    } else if(!str_cmp(arg, "turd")) {
        vnum = OBJ_VNUM_TORN_HEART;
    } else if(!str_cmp(arg, "entrails")) {
        vnum = OBJ_VNUM_SPILLED_ENTRAILS;
    } else if(!str_cmp(arg, "brain")) {
        vnum = OBJ_VNUM_QUIVERING_BRAIN;
    } else if(!str_cmp(arg, "eyeball")) {
        vnum = OBJ_VNUM_SQUIDGY_EYEBALL;
    } else if(!str_cmp(arg, "blood")) {
        vnum = OBJ_VNUM_SPILT_BLOOD;
    } else if(!str_cmp(arg, "face")) {
        vnum = OBJ_VNUM_RIPPED_FACE;
    } else if(!str_cmp(arg, "windpipe")) {
        vnum = OBJ_VNUM_TORN_WINDPIPE;
    } else if(!str_cmp(arg, "cracked_head")) {
        vnum = OBJ_VNUM_CRACKED_HEAD;
    } else if(!str_cmp(arg, "ear")) {
        vnum = OBJ_VNUM_SLICED_EAR;
    } else if(!str_cmp(arg, "nose")) {
        vnum = OBJ_VNUM_SLICED_NOSE;
    } else if(!str_cmp(arg, "tooth")) {
        vnum = OBJ_VNUM_KNOCKED_TOOTH;
    } else if(!str_cmp(arg, "tongue")) {
        vnum = OBJ_VNUM_TORN_TONGUE;
    } else if(!str_cmp(arg, "hand")) {
        vnum = OBJ_VNUM_SEVERED_HAND;
    } else if(!str_cmp(arg, "foot")) {
        vnum = OBJ_VNUM_SEVERED_FOOT;
    } else if(!str_cmp(arg, "thumb")) {
        vnum = OBJ_VNUM_SEVERED_THUMB;
    } else if(!str_cmp(arg, "index")) {
        vnum = OBJ_VNUM_SEVERED_INDEX;
    } else if(!str_cmp(arg, "middle")) {
        vnum = OBJ_VNUM_SEVERED_MIDDLE;
    } else if(!str_cmp(arg, "ring")) {
        vnum = OBJ_VNUM_SEVERED_RING;
    } else if(!str_cmp(arg, "little")) {
        vnum = OBJ_VNUM_SEVERED_LITTLE;
    } else if(!str_cmp(arg, "toe")) {
        vnum = OBJ_VNUM_SEVERED_TOE;
    }
    if(vnum != 0) {
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        char *name;
        name		= IS_NPC(ch) ? ch->short_descr : ch->name;
        obj		= create_object(get_obj_index(vnum), 0);
        if(IS_NPC(ch)) {
            obj->timer = number_range(30, 60);
        } else {
            obj->timer = -1;
        }
        if(!str_cmp(arg, "head") && IS_NPC(ch)) {
            obj->value[1] = ch->pIndexData->vnum;
        } else if(!str_cmp(arg, "head") && !IS_NPC(ch)) {
            ch->pcdata->chobj = obj;
            obj->chobj = ch;
            obj->timer = number_range(30, 60);
        } else if(!str_cmp(arg, "brain") && !IS_NPC(ch) &&
                  IS_AFFECTED(ch, AFF_POLYMORPH) && IS_HEAD(ch, LOST_HEAD)) {
            if(ch->pcdata->chobj != NULL) {
                ch->pcdata->chobj->chobj = NULL;
            }
            ch->pcdata->chobj = obj;
            obj->chobj = ch;
            obj->timer = number_range(30, 60);
        }
        /* For blood :) KaVir */
        if(vnum == OBJ_VNUM_SPILT_BLOOD) {
            obj->timer = 30;
        }
        /* For voodoo dolls - KaVir */
        if(!IS_NPC(ch)) {
            sprintf(buf, obj->name, name);
            free_string(obj->name);
            obj->name = str_dup(buf);
        } else {
            sprintf(buf, obj->name, "mob");
            free_string(obj->name);
            obj->name = str_dup(buf);
        }
        sprintf(buf, obj->short_descr, name);
        free_string(obj->short_descr);
        obj->short_descr = str_dup(buf);
        sprintf(buf, obj->description, name);
        free_string(obj->description);
        obj->description = str_dup(buf);
        free_string(obj->questmaker);
        obj->questmaker = str_dup(ch->name);
        if(IS_AFFECTED(ch, AFF_SHADOWPLANE)) {
            SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
        }
        if(IS_DEMON(ch)) {
            SET_BIT(obj->quest, QUEST_FOLLOWING);
        }
        obj_to_room(obj, ch->in_room);
    }
    return;
}



void raw_kill(CHAR_DATA *victim, CHAR_DATA *ch) {
    CHAR_DATA *mount;
    if(!IS_NPC(victim) && victim->form > 0) {
        death_message(ch, victim);
    }
    stop_fighting(victim, TRUE);
    victim->agg = 0;
    if(IS_NPC(victim) && !IS_NPC(ch)) {
        ch->mkill++;
    }
    if(!IS_NPC(victim) && IS_NPC(ch)) {
        victim->mdeath++;
    }
    if(!IS_NPC(victim) && !IS_NPC(ch) && ch != victim) {
        ch->pkill++;
        victim->pdeath++;
    }
    if(IS_NPC(victim) && (IS_EXTRA(victim, EXTRA_ILLUSION) ||
                          IS_SPIRIT(victim))) {
        act("$n slowly fades away to nothing.", victim, NULL, NULL, TO_ROOM);
    } else if(IS_NPC(victim) && strlen(victim->powertype) > 1 &&
              !str_cmp(victim->powertype, "zombie")) {
        act("$n crumbles to dust as $e hits the floor.", victim, NULL, NULL, TO_ROOM);
    } else if(!IS_NPC(victim) || !IS_EXTRA(victim, EXTRA_ILLUSION)) {
        death_cry(victim);
        make_corpse(victim, ch);
    }
    if((mount = victim->mount) != NULL) {
        if(victim->mounted == IS_MOUNT) {
            act("$n rolls off the corpse of $N.", mount, NULL, victim, TO_ROOM);
            act("You roll off the corpse of $N.", mount, NULL, victim, TO_CHAR);
        } else if(victim->mounted == IS_RIDING) {
            act("$n falls off $N.", victim, NULL, mount, TO_ROOM);
            act("You fall off $N.", victim, NULL, mount, TO_CHAR);
        }
        mount->mount    = NULL;
        victim->mount   = NULL;
        mount->mounted  = IS_ON_FOOT;
        victim->mounted = IS_ON_FOOT;
    }
    victim->position = POS_DEAD;
    if(IS_NPC(victim)) {
        victim->pIndexData->killed++;
        kill_table[URANGE(0, victim->level, MAX_LEVEL - 1)].killed++;
        char_from_room(victim);
        char_to_room(victim, get_room_index(ROOM_VNUM_DISCONNECTION));
        extract_char(victim, TRUE);
        return;
    }
    victim->hit = 1;
    REMOVE_BIT(victim->extra, EXTRA_DYING);
    extract_char(victim, FALSE);
    while(victim->affected) {
        affect_remove(victim, victim->affected);
    }
    if(IS_AFFECTED(victim, AFF_POLYMORPH) && IS_AFFECTED(victim, AFF_ETHEREAL)) {
        victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    } else if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
        victim->affected_by	= AFF_POLYMORPH;
    } else if(IS_AFFECTED(victim, AFF_ETHEREAL)) {
        victim->affected_by	= AFF_ETHEREAL;
    } else {
        victim->affected_by	= 0;
    }
    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    REMOVE_BIT(victim->extra, EXTRA_NO_ACTION);
    victim->pcdata->demonic = 0;
    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->blood[0]     = victim->blood[1];
    victim->armor	 = 100;
    victim->position	 = POS_RESTING;
    victim->hit		 = UMAX(1, victim->hit);
    victim->mana	 = UMAX(1, victim->mana);
    victim->move	 = UMAX(1, victim->move);
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;
    do_call(victim, "all");
    do_autosave(victim, "");
    return;
}


void behead(CHAR_DATA *victim, CHAR_DATA *ch) {
    char buf [MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    if(IS_NPC(victim)) {
        return;
    }
    location = victim->in_room;
    stop_fighting(victim, TRUE);
    make_corpse(victim, ch);
    extract_char(victim, FALSE);
    char_from_room(victim);
    char_to_room(victim, location);
    if(victim == NULL) {
        bug("Behead: Victim no longer exists.", 0);
        return;
    }
    make_part(victim, "head");
    while(victim->affected) {
        affect_remove(victim, victim->affected);
    }
    if(IS_AFFECTED(victim, AFF_POLYMORPH) && IS_AFFECTED(victim, AFF_ETHEREAL)) {
        victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    } else if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
        victim->affected_by	= AFF_POLYMORPH;
    } else if(IS_AFFECTED(victim, AFF_ETHEREAL)) {
        victim->affected_by	= AFF_ETHEREAL;
    } else {
        victim->affected_by	= 0;
    }
    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    REMOVE_BIT(victim->extra, EXTRA_NO_ACTION);
    victim->pcdata->demonic = 0;
    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->blood[0]     = victim->blood[1];
    victim->affected_by	 = 0;
    victim->armor	 = 100;
    victim->position	 = POS_STANDING;
    victim->hit		 = 1;
    victim->mana	 = UMAX(1, victim->mana);
    victim->move	 = UMAX(1, victim->move);
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;
    SET_BIT(victim->loc_hp[0], LOST_HEAD);
    SET_BIT(victim->affected_by, AFF_POLYMORPH);
    sprintf(buf, "the severed head of %s", victim->name);
    free_string(victim->morph);
    victim->morph = str_dup(buf);
    do_call(victim, "all");
    do_autosave(victim, "");
    return;
}



void group_gain(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    CHAR_DATA *mount;
    int xp;
    int members;
    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if((IS_NPC(ch) && (mount = ch->mount) == NULL) || victim == ch) {
        return;
    }
    if(IS_EXTRA(victim, EXTRA_DYING)) {
        return;
    }
    members = 0;
    for(gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
        if(is_same_group(gch, ch)) {
            members++;
        }
    }
    if(members == 0) {
        bug("Group_gain: members.", members);
        members = 1;
    }
    lch = (ch->leader != NULL) ? ch->leader : ch;
    for(gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        if(!is_same_group(gch, ch)) {
            continue;
        }
        xp = xp_compute(gch, victim);
        if(xp == 1) {
            strcpy(buf, "[You receive an experience point]\n\r");
            send_to_char(buf, gch);
        } else if(xp > 1) {
            sprintf(buf, "[You receive %d experience points]\n\r", xp);
            send_to_char(buf, gch);
        }
        if((mount = gch->mount) != NULL) {
            send_to_char(buf, mount);
        }
        gain_exp(gch, xp);
        for(obj = ch->carrying; obj != NULL; obj = obj_next) {
            obj_next = obj->next_content;
            if(obj->wear_loc == WEAR_NONE) {
                continue;
            }
            if((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch))
                    || (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch))
                    || (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch))) {
                act("You are zapped by $p.", ch, obj, NULL, TO_CHAR);
                act("$n is zapped by $p.",   ch, obj, NULL, TO_ROOM);
                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);
            }
        }
    }
    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute(CHAR_DATA *gch, CHAR_DATA *victim) {
    int align, xp, vnum, level, number, extra;
    xp = 300 - URANGE(-3, gch->level - victim->level, 6) * 50;
    align = gch->alignment - victim->alignment;
    if(IS_HERO(gch) || IS_DEMON(gch)) {
        /* Avatars shouldn't be able to change their alignment */
        gch->alignment  = gch->alignment;
    } else if(align >  500) {
        gch->alignment  = UMIN(gch->alignment + (align - 500) / 4,  1000);
    } else if(align < -500) {
        gch->alignment  = UMAX(gch->alignment + (align + 500) / 4, -1000);
    } else {
        gch->alignment -= (gch->alignment >> 2);
    }
    if(IS_NPC(victim) && (vnum = victim->pIndexData->vnum) > 29000) {
        switch(vnum) {
        default:
            break;
        case 29600:
        case 30000:
        case 30001:
        case 30006:
        case 30007:
        case 30008:
        case 30009:
        case 30010:
        case 30012:
        case 30013:
        case 30014:
        case 30015:
        case 30016:
            return 0;
        }
    }
    if(IS_NPC(victim) && (IS_SET(victim->act, ACT_NOEXP) || IS_IMMORTAL(gch))) {
        return 0;
    }
    level  = URANGE(0, victim->level, MAX_LEVEL - 1);
    number = UMAX(1, kill_table[level].number);
    extra  = IS_NPC(victim) ? (victim->pIndexData->killed -
                               kill_table[level].killed / number) : 0;
    xp    -= xp * URANGE(-2, extra, 2) / 8;
    xp     = number_range(xp * 3 / 4, xp * 5 / 4);
    xp     = UMAX(0, xp);
    xp     = (xp * (victim->level + 1)) / (gch->level + 1);
    if(!IS_NPC(gch) && IS_NPC(victim)) {
        if(victim->level > gch->pcdata->score[SCORE_HIGH_LEVEL]) {
            gch->pcdata->score[SCORE_HIGH_LEVEL] = victim->level;
        }
        gch->pcdata->score[SCORE_TOTAL_XP] += xp;
        if(xp > gch->pcdata->score[SCORE_HIGH_XP]) {
            gch->pcdata->score[SCORE_HIGH_XP] = xp;
        }
    }
    return xp;
}



void dam_message(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt) {
    static char * const attack_table[] = {
        "hit",
        "slice",  "stab",  "slash", "whip", "claw",
        "blast",  "pound", "crush", "grep", "bite",
        "pierce", "suck"
    };
    static char * const attack_table2[] = {
        "hits",
        "slices",  "stabs",  "slashes", "whips", "claws",
        "blasts",  "pounds", "crushes", "greps", "bites",
        "pierces", "sucks"
    };
    char buf1[512], buf2[512], buf3[512];
    const char *vs;
    const char *vp;
    const char *attack;
    const char *attack2;
    int damp;
    int bodyloc;
    int recover;
    bool critical = FALSE;
    char punct;
    if(dt >= TYPE_ADD_AGG) {
        dt -= TYPE_ADD_AGG;
        while(dt >= TYPE_ADD_AGG) {
            dt -= TYPE_ADD_AGG;
        }
    }
    if(dam ==   0) {
        vs = " miss";
        vp = " misses";
    } else if(dam <=  25) {
        vs = " lightly";
        vp = " lightly";
    } else if(dam <=  50) {
        vs = "";
        vp = "";
    } else if(dam <= 100) {
        vs = " hard";
        vp = " hard";
    } else if(dam <= 250) {
        vs = " very hard";
        vp = " very hard";
    } else if(dam <= 500) {
        vs = " extremely hard";
        vp = " extremely hard";
    } else                   {
        vs = " incredibly hard";
        vp = " incredibly hard";
    }
    if(IS_NPC(victim)) {
        damp = 0;
    } else {
        damp = -10;
    }
    if((victim->hit - dam > damp) || (dt >= 0 && dt < MAX_SKILL) ||
            (IS_NPC(victim) && IS_SET(victim->act, ACT_NOPARTS))) {
        punct   = (dam <= 250) ? '.' : '!';
        if(dt == TYPE_HIT) {
            if(dam == 0) {
                if(IS_VAMPAFF(ch, VAM_CLAWS) && IS_MORE(ch, MORE_HAND_FLAME)) {
                    sprintf(buf1, "$n's flaming claw%s $N%c",  vp, punct);
                    sprintf(buf2, "Your flaming claw%s $N%c", vs, punct);
                    sprintf(buf3, "$n's flaming claw%s you%c", vp, punct);
                    critical = TRUE;
                } else if(IS_VAMPAFF(ch, VAM_CLAWS)) {
                    sprintf(buf1, "$n's claw%s $N%c",  vp, punct);
                    sprintf(buf2, "Your claw%s $N%c", vs, punct);
                    sprintf(buf3, "$n's claw%s you%c", vp, punct);
                    critical = TRUE;
                } else if(IS_MORE(ch, MORE_HAND_FLAME)) {
                    sprintf(buf1, "$n's flaming hand%s $N%c",  vp, punct);
                    sprintf(buf2, "Your flaming hand%s $N%c", vs, punct);
                    sprintf(buf3, "$n's flaming hand%s you%c", vp, punct);
                    critical = TRUE;
                } else {
                    sprintf(buf1, "$n%s $N%c",  vp, punct);
                    sprintf(buf2, "You%s $N%c", vs, punct);
                    sprintf(buf3, "$n%s you%c", vp, punct);
                }
            } else if(IS_VAMPAFF(ch, VAM_CLAWS) && IS_MORE(ch, MORE_HAND_FLAME)) {
                sprintf(buf1, "$n's flaming claw hits $N%s%c",  vp, punct);
                sprintf(buf2, "Your flaming claw hits $N%s%c", vs, punct);
                sprintf(buf3, "$n's flaming claw hits you%s%c", vp, punct);
                critical = TRUE;
            } else if(IS_VAMPAFF(ch, VAM_CLAWS)) {
                sprintf(buf1, "$n's claw hits $N%s%c",  vp, punct);
                sprintf(buf2, "Your claw hits $N%s%c", vs, punct);
                sprintf(buf3, "$n's claw hits you%s%c", vp, punct);
                critical = TRUE;
            } else if(IS_MORE(ch, MORE_HAND_FLAME)) {
                sprintf(buf1, "$n's flaming hand hits $N%s%c",  vp, punct);
                sprintf(buf2, "Your flaming hand hits $N%s%c", vs, punct);
                sprintf(buf3, "$n's flaming hand hits you%s%c", vp, punct);
                critical = TRUE;
            } else {
                sprintf(buf1, "$n hits $N%s%c",  vp, punct);
                sprintf(buf2, "You hit $N%s%c", vs, punct);
                sprintf(buf3, "$n hits you%s%c", vp, punct);
                critical = TRUE;
            }
        } else {
            if(dt == TYPE_HIT && IS_VAMPAFF(ch, VAM_CLAWS)) {
                attack  = attack_table[dt - TYPE_HIT + 5];
                attack2 = attack_table2[dt - TYPE_HIT + 5];
            } else if(dt >= 0 && dt < MAX_SKILL) {
                attack = skill_table[dt].noun_damage;
                attack2 = skill_table[dt].noun_damage;
            } else if(dt >= TYPE_HIT
                      && dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0])) {
                attack  = attack_table[dt - TYPE_HIT];
                attack2 = attack_table2[dt - TYPE_HIT];
            } else {
                bug("Dam_message: bad dt %d.", dt);
                dt  = TYPE_HIT;
                attack  = attack_table[0];
                attack2 = attack_table2[0];
            }
            if(dam == 0) {
                sprintf(buf1, "$n's %s%s $N%c",  attack, vp, punct);
                sprintf(buf2, "Your %s%s $N%c",  attack, vp, punct);
                sprintf(buf3, "$n's %s%s you%c", attack, vp, punct);
            } else {
                if(dt >= 0 && dt < MAX_SKILL) {
                    sprintf(buf1, "$n's %s strikes $N%s%c",  attack2, vp, punct);
                    sprintf(buf2, "Your %s strikes $N%s%c",  attack, vp, punct);
                    sprintf(buf3, "$n's %s strikes you%s%c", attack2, vp, punct);
                } else {
                    sprintf(buf1, "$n %s $N%s%c",  attack2, vp, punct);
                    sprintf(buf2, "You %s $N%s%c",  attack, vp, punct);
                    sprintf(buf3, "$n %s you%s%c", attack2, vp, punct);
                    critical = TRUE;
                }
            }
            recover = 0;
            if(IS_IMMUNE(victim, IMM_SLASH) &&
                    (attack == "slash" || attack == "slice") && dam > 0) {
                recover = number_range(1, dam);
            }
            if(IS_IMMUNE(victim, IMM_STAB) &&
                    (attack == "stab" || attack == "pierce") && dam > 0) {
                recover = number_range(1, dam);
            }
            if(IS_IMMUNE(victim, IMM_SMASH) &&
                    (attack == "blast" || attack == "pound" || attack == "crush") && dam > 0) {
                recover = number_range(1, dam);
            }
            if(IS_IMMUNE(victim, IMM_ANIMAL) &&
                    (attack == "bite" || attack == "claw") && dam > 0) {
                recover = number_range(1, dam);
            }
            if(IS_IMMUNE(victim, IMM_MISC) &&
                    (attack == "grep" || attack == "suck" || attack == "whip") && dam > 0) {
                recover = number_range(1, dam);
            }
            if(!IS_NPC(victim) && get_disc(victim, DISC_FORTITUDE) > 0 &&
                    (dam - recover) > 0) {
                int per_red = 0;
                switch(get_disc(victim, DISC_FORTITUDE)) {
                default:
                    per_red = 0;
                    break;
                case 1:
                    per_red = number_range(10, 20);
                    break;
                case 2:
                    per_red = number_range(20, 40);
                    break;
                case 3:
                    per_red = number_range(30, 60);
                    break;
                case 4:
                    per_red = number_range(40, 80);
                    break;
                case 5:
                    per_red = number_range(50, 100);
                    break;
                }
                victim->hit += (per_red * (dam - recover) * 0.01);
            } else if(IS_ITEMAFF(victim, ITEMA_RESISTANCE) && (dam - recover) > 0) {
                victim->hit += number_range(1, (dam - recover));
            } else if(!IS_NPC(victim) && IS_DEMON(victim) && (dam - recover) > 0
                      && IS_SET(victim->pcdata->disc[C_POWERS], DEM_TOUGH)) {
                victim->hit += number_range(1, (dam - recover));
            }
            victim->hit += recover;
        }
        act(buf1, ch, NULL, victim, TO_NOTVICT);
        act(buf2, ch, NULL, victim, TO_CHAR);
        act(buf3, ch, NULL, victim, TO_VICT);
        if(IS_NPC(victim) && IS_SET(victim->act, ACT_NOPARTS)) {
            critical = FALSE;
        }
        if(critical) {
            critical_hit(ch, victim, dt, dam);
        }
        return;
    }
    if(dt == TYPE_HIT && !IS_VAMPAFF(ch, VAM_CLAWS) && !IS_VAMPAFF(ch, VAM_FANGS)) {
        damp = number_range(1, 5);
        if(damp == 1) {
            act("You ram your fingers into $N's eye sockets and rip $S face off.", ch, NULL, victim, TO_CHAR);
            act("$n rams $s fingers into $N's eye sockets and rips $S face off.", ch, NULL, victim, TO_NOTVICT);
            act("$n rams $s fingers into your eye sockets and rips your face off.", ch, NULL, victim, TO_VICT);
            victim->form = 21;
            make_part(victim, "face");
        } else if(damp == 2) {
            act("You grab $N by the throat and tear $S windpipe out.", ch, NULL, victim, TO_CHAR);
            act("$n grabs $N by the throat and tears $S windpipe out.", ch, NULL, victim, TO_NOTVICT);
            act("$n grabs you by the throat and tears your windpipe out.", ch, NULL, victim, TO_VICT);
            victim->form = 22;
            make_part(victim, "windpipe");
        } else if(damp == 3) {
            act("You punch your fist through $N's stomach and rip out $S entrails.", ch, NULL, victim, TO_CHAR);
            act("$n punches $s fist through $N's stomach and rips out $S entrails.", ch, NULL, victim, TO_NOTVICT);
            act("$n punches $s fist through your stomach and rips out your entrails.", ch, NULL, victim, TO_VICT);
            victim->form = 23;
            make_part(victim, "entrails");
            if(!IS_BODY(victim, CUT_STOMACH)) {
                SET_BIT(victim->loc_hp[1], CUT_STOMACH);
            }
            if(!IS_BLEEDING(victim, BLEEDING_STOMACH)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_STOMACH);
            }
            SET_BIT(victim->loc_hp[1], LOST_ENTRAILS);
        } else if(damp == 4) {
            if(!IS_BODY(victim, BROKEN_SPINE)) {
                SET_BIT(victim->loc_hp[1], BROKEN_SPINE);
            }
            act("You hoist $N above your head and slam $M down upon your knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_CHAR);
            act("$n hoists $N above $s head and slams $M down upon $s knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_NOTVICT);
            act("$n hoists you above $s head and slams you down upon $s knee.\n\rThere is a loud cracking sound as your spine snaps.", ch, NULL, victim, TO_VICT);
            victim->form = 24;
        } else if(damp == 5) {
            act("You lock your arm around $N's head, and give it a vicious twist.", ch, NULL, victim, TO_CHAR);
            act("$n locks $s arm around $N's head, and gives it a vicious twist.", ch, NULL, victim, TO_NOTVICT);
            act("$n locks $s arm around your head, and gives it a vicious twist.", ch, NULL, victim, TO_VICT);
            if(!IS_BODY(victim, BROKEN_NECK)) {
                act("There is a loud snapping noise as your neck breaks.", victim, NULL, NULL, TO_CHAR);
                act("There is a loud snapping noise as $n's neck breaks.", victim, NULL, NULL, TO_ROOM);
                SET_BIT(victim->loc_hp[1], BROKEN_NECK);
            }
            victim->form = 25;
        }
        return;
    }
    if(dt >= 0 && dt < MAX_SKILL) {
        attack = skill_table[dt].noun_damage;
    } else if(dt >= TYPE_HIT
              && dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0])) {
        attack = attack_table[dt - TYPE_HIT];
    } else {
        bug("Dam_message: bad dt %d.", dt);
        dt  = TYPE_HIT;
        attack  = attack_table[0];
    }
    if(attack == "slash" || attack == "slice") {
        damp = number_range(1, 8);
        if(damp == 1) {
            act("You swing your blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_CHAR);
            act("$n swings $s blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_NOTVICT);
            act("$n swings $s blade in a low arc, rupturing your abdominal cavity.\n\rYour entrails spray out over a wide area.", ch, NULL, victim, TO_VICT);
            victim->form = 26;
            make_part(victim, "entrails");
        } else if(damp == 2) {
            act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
            act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
            act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
            victim->form = 27;
        } else if(damp == 3) {
            if(!IS_BODY(victim, CUT_THROAT)) {
                SET_BIT(victim->loc_hp[1], CUT_THROAT);
            }
            if(!IS_BLEEDING(victim, BLEEDING_THROAT)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
            }
            act("Your blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_CHAR);
            act("$n's blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_NOTVICT);
            act("$n's blow slices open your carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_VICT);
            victim->form = 28;
            make_part(victim, "blood");
        } else if(damp == 4) {
            if(!IS_BODY(victim, CUT_THROAT)) {
                SET_BIT(victim->loc_hp[1], CUT_THROAT);
            }
            if(!IS_BLEEDING(victim, BLEEDING_THROAT)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
            }
            act("You swing your blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
            act("$n swings $s blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
            act("$n swings $s blade across your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
            victim->form = 29;
            make_part(victim, "blood");
        } else if(damp == 5) {
            if(!IS_HEAD(victim, BROKEN_SKULL)) {
                act("You swing your blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s blade down upon your head, splitting it open.\n\rYour brains pour out of your forehead.", ch, NULL, victim, TO_VICT);
                victim->form = 30;
                make_part(victim, "brain");
                SET_BIT(victim->loc_hp[0], BROKEN_SKULL);
            } else {
                act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
                act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
                act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
                victim->form = 31;
                if(!IS_BODY(victim, CUT_CHEST)) {
                    SET_BIT(victim->loc_hp[1], CUT_CHEST);
                }
                if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
                }
            }
        } else if(damp == 6) {
            act("You swing your blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_CHAR);
            act("$n swings $s blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_NOTVICT);
            act("$n swings $s blade between your legs, nearly splitting you in half.", ch, NULL, victim, TO_VICT);
            victim->form = 32;
        } else if(damp == 7) {
            if(!IS_ARM_L(victim, LOST_ARM)) {
                act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
                victim->form = 33;
                make_part(victim, "arm");
                SET_BIT(victim->loc_hp[2], LOST_ARM);
                if(!IS_BLEEDING(victim, BLEEDING_ARM_L)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
                }
                if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
                    REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
                }
            } else if(!IS_ARM_R(victim, LOST_ARM)) {
                act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
                victim->form = 33;
                make_part(victim, "arm");
                SET_BIT(victim->loc_hp[3], LOST_ARM);
                if(!IS_BLEEDING(victim, BLEEDING_ARM_R)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
                }
                if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
                    REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
                }
            } else {
                act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
                act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
                act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
                victim->form = 31;
                if(!IS_BODY(victim, CUT_CHEST)) {
                    SET_BIT(victim->loc_hp[1], CUT_CHEST);
                }
                if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
                }
            }
        } else if(damp == 8) {
            if(!IS_LEG_L(victim, LOST_LEG)) {
                act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
                victim->form = 34;
                make_part(victim, "leg");
                SET_BIT(victim->loc_hp[4], LOST_LEG);
                if(!IS_BLEEDING(victim, BLEEDING_LEG_L)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
                }
                if(IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
                    REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
                }
            } else if(!IS_LEG_R(victim, LOST_LEG)) {
                act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
                victim->form = 34;
                make_part(victim, "leg");
                SET_BIT(victim->loc_hp[5], LOST_LEG);
                if(!IS_BLEEDING(victim, BLEEDING_LEG_R)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
                }
                if(IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
                    REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
                }
            } else {
                act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
                act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
                act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
                victim->form = 31;
                if(!IS_BODY(victim, CUT_CHEST)) {
                    SET_BIT(victim->loc_hp[1], CUT_CHEST);
                }
                if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
                }
            }
        }
    } else if(attack == "stab" || attack == "pierce") {
        damp = number_range(1, 5);
        if(damp == 1) {
            act("You defty invert your weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_CHAR);
            act("$n defty inverts $s weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_NOTVICT);
            act("$n defty inverts $s weapon and plunge it point first into your chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_VICT);
            victim->form = 31;
            make_part(victim, "blood");
            if(!IS_BODY(victim, CUT_CHEST)) {
                SET_BIT(victim->loc_hp[1], CUT_CHEST);
            }
            if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
            }
        } else if(damp == 2) {
            act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
            act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
            act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
            victim->form = 35;
        } else if(damp == 3) {
            act("You thrust your weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_CHAR);
            act("$n thrusts $s weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_NOTVICT);
            act("$n thrusts $s weapon up under your jaw and through your head.", ch, NULL, victim, TO_VICT);
            victim->form = 27;
        } else if(damp == 4) {
            act("You ram your weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_CHAR);
            act("$n rams $s weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_NOTVICT);
            act("$n rams $s weapon through your body, pinning you to the ground.", ch, NULL, victim, TO_VICT);
            victim->form = 31;
            if(!IS_BODY(victim, CUT_CHEST)) {
                SET_BIT(victim->loc_hp[1], CUT_CHEST);
            }
            if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
            }
        } else if(damp == 5) {
            act("You stab your weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_CHAR);
            act("$n stabs $s weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_NOTVICT);
            act("$n stabs $s weapon into your eye and out the back of your head.", ch, NULL, victim, TO_VICT);
            victim->form = 36;
            if(!IS_HEAD(victim, LOST_EYE_L) && number_percent() < 50) {
                SET_BIT(victim->loc_hp[0], LOST_EYE_L);
            } else if(!IS_HEAD(victim, LOST_EYE_R)) {
                SET_BIT(victim->loc_hp[0], LOST_EYE_R);
            } else if(!IS_HEAD(victim, LOST_EYE_L)) {
                SET_BIT(victim->loc_hp[0], LOST_EYE_L);
            }
        }
    } else if(attack == "blast" || attack == "pound" || attack == "crush") {
        damp = number_range(1, 3);
        bodyloc = 0;
        if(damp == 1) {
            act("Your blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_CHAR);
            act("$n's blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_NOTVICT);
            act("$n's blow smashes through your chest, caving in half your ribcage.", ch, NULL, victim, TO_VICT);
            victim->form = 37;
            if(IS_BODY(victim, BROKEN_RIBS_1)) {
                bodyloc += 1;
                REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
            }
            if(IS_BODY(victim, BROKEN_RIBS_2)) {
                bodyloc += 2;
                REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
            }
            if(IS_BODY(victim, BROKEN_RIBS_4)) {
                bodyloc += 4;
                REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
            }
            if(IS_BODY(victim, BROKEN_RIBS_8)) {
                bodyloc += 8;
                REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
            }
            if(IS_BODY(victim, BROKEN_RIBS_16)) {
                bodyloc += 16;
                REMOVE_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
            }
            bodyloc += number_range(1, 3);
            if(bodyloc > 24) {
                bodyloc = 24;
            }
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
        } else if(damp == 2) {
            act("Your blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_CHAR);
            act("$n's blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_NOTVICT);
            act("$n's blow smashes your spine, shattering it in several places.", ch, NULL, victim, TO_VICT);
            victim->form = 38;
            if(!IS_BODY(victim, BROKEN_SPINE)) {
                SET_BIT(victim->loc_hp[1], BROKEN_SPINE);
            }
        } else if(damp == 3) {
            if(!IS_HEAD(victim, BROKEN_SKULL)) {
                act("You swing your weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, leaking out brains.", ch, NULL, victim, TO_CHAR);
                act("$n swings $s weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, covering you with brains.", ch, NULL, victim, TO_NOTVICT);
                act("$n swings $s weapon down upon your head.\n\rYour head cracks open like an overripe melon, spilling your brains everywhere.", ch, NULL, victim, TO_VICT);
                victim->form = 30;
                make_part(victim, "brain");
                SET_BIT(victim->loc_hp[0], BROKEN_SKULL);
            } else {
                act("You hammer your weapon into $N's side, crushing bone.", ch, NULL, victim, TO_CHAR);
                act("$n hammers $s weapon into $N's side, crushing bone.", ch, NULL, victim, TO_NOTVICT);
                act("$n hammers $s weapon into your side, crushing bone.", ch, NULL, victim, TO_VICT);
                victim->form = 37;
            }
        }
    } else if(attack == "bite" || IS_VAMPAFF(ch, VAM_FANGS)) {
        act("You sink your teeth into $N's throat and tear out $S jugular vein.\n\rYou wipe the blood from your chin with one hand.", ch, NULL, victim, TO_CHAR);
        act("$n sink $s teeth into $N's throat and tears out $S jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_NOTVICT);
        act("$n sink $s teeth into your throat and tears out your jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_VICT);
        victim->form = 39;
        make_part(victim, "blood");
        if(!IS_BODY(victim, CUT_THROAT)) {
            SET_BIT(victim->loc_hp[1], CUT_THROAT);
        }
        if(!IS_BLEEDING(victim, BLEEDING_THROAT)) {
            SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
        }
    } else if(attack == "claw" || IS_VAMPAFF(ch, VAM_CLAWS)) {
        damp = number_range(1, 2);
        if(damp == 1) {
            act("You tear out $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
            act("$n tears out $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
            act("$n tears out your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
            victim->form = 40;
            make_part(victim, "blood");
            if(!IS_BODY(victim, CUT_THROAT)) {
                SET_BIT(victim->loc_hp[1], CUT_THROAT);
            }
            if(!IS_BLEEDING(victim, BLEEDING_THROAT)) {
                SET_BIT(victim->loc_hp[6], BLEEDING_THROAT);
            }
        }
        if(damp == 2) {
            if(!IS_HEAD(victim, LOST_EYE_L) && number_percent() < 50) {
                act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
                act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
                act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
                victim->form = 41;
                make_part(victim, "eyeball");
                SET_BIT(victim->loc_hp[0], LOST_EYE_L);
            } else if(!IS_HEAD(victim, LOST_EYE_R)) {
                act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
                act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
                act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
                victim->form = 41;
                make_part(victim, "eyeball");
                SET_BIT(victim->loc_hp[0], LOST_EYE_R);
            } else if(!IS_HEAD(victim, LOST_EYE_L)) {
                act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
                act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
                act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
                victim->form = 41;
                make_part(victim, "eyeball");
                SET_BIT(victim->loc_hp[0], LOST_EYE_L);
            } else {
                act("You claw open $N's chest.", ch, NULL, victim, TO_CHAR);
                act("$n claws open $N's chest.", ch, NULL, victim, TO_NOTVICT);
                act("$n claws open $N's chest.", ch, NULL, victim, TO_VICT);
                victim->form = 42;
                if(!IS_BODY(victim, CUT_CHEST)) {
                    SET_BIT(victim->loc_hp[1], CUT_CHEST);
                }
                if(!IS_BLEEDING(victim, BLEEDING_CHEST)) {
                    SET_BIT(victim->loc_hp[6], BLEEDING_CHEST);
                }
            }
        }
    } else if(attack == "whip") {
        act("You entangle $N around the neck, and squeeze the life out of $S.", ch, NULL, victim, TO_CHAR);
        act("$n entangle $N around the neck, and squeezes the life out of $S.", ch, NULL, victim, TO_NOTVICT);
        act("$n entangles you around the neck, and squeezes the life out of you.", ch, NULL, victim, TO_VICT);
        victim->form = 25;
        if(!IS_BODY(victim, BROKEN_NECK)) {
            SET_BIT(victim->loc_hp[1], BROKEN_NECK);
        }
    } else if(attack == "suck" || attack == "grep") {
        act("You place your weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_CHAR);
        act("$n places $s weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_NOTVICT);
        act("$n places $s weapon on your head and suck out your brains.", ch, NULL, victim, TO_VICT);
        victim->form = 43;
    } else {
        bug("Dam_message: bad dt %d.", dt);
    }
    return;
}


/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm(CHAR_DATA *ch, CHAR_DATA *victim) {
    OBJ_DATA *obj;
    char buf [MAX_STRING_LENGTH];
    /* I'm fed up of being disarmed every 10 seconds - KaVir */
    if(IS_NPC(ch) && victim->level > 2 && number_percent() > 5) {
        return;
    }
    if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM)) {
        return;
    }
    /*
        if ( ( (obj = get_eq_char( ch, WEAR_WIELD ) == NULL) || obj->item_type != ITEM_WEAPON )
        &&   ( (obj = get_eq_char( ch, WEAR_HOLD  ) == NULL) || obj->item_type != ITEM_WEAPON ) )
    	return;
    */
    if(((obj = get_eq_char(victim, WEAR_WIELD)) == NULL) || obj->item_type != ITEM_WEAPON) {
        if(((obj = get_eq_char(victim, WEAR_HOLD)) == NULL) || obj->item_type != ITEM_WEAPON) {
            return;
        }
    }
    sprintf(buf, "$n disarms you!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf, ch, NULL, victim, TO_VICT);
    sprintf(buf, "You disarm $N!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf,  ch, NULL, victim, TO_CHAR);
    sprintf(buf, "$n disarms $N!");
    ADD_COLOUR(ch, buf, WHITE);
    act(buf,  ch, NULL, victim, TO_NOTVICT);
    obj_from_char(obj);
    /* Loyal weapons come back ;)  KaVir */
    if(IS_SET(obj->extra_flags, ITEM_LOYAL) && (!IS_NPC(victim))) {
        act("$p leaps back into your hand!", victim, obj, NULL, TO_CHAR);
        act("$p leaps back into $n's hand!",  victim, obj, NULL, TO_ROOM);
        obj_to_char(obj, victim);
        do_wear(victim, obj->name);
    } else if(IS_NPC(victim)) {
        obj_to_char(obj, victim);
    } else {
        obj_to_room(obj, victim->in_room);
    }
    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf [MAX_STRING_LENGTH];
    if(IS_AFFECTED(victim, AFF_FLYING)) {
        return;
    }
    if(IS_NPC(ch) && victim->level > 2 && number_percent() > 5) {
        return;
    }
    if(!IS_NPC(victim)) {
        if(IS_VAMPIRE(victim) && IS_VAMPAFF(victim, VAM_FLYING)) {
            return;
        }
        if(IS_DEMON(victim) && IS_DEMAFF(victim, DEM_UNFOLDED)) {
            return;
        }
    }
    if(victim->wait == 0) {
        sprintf(buf, "$n trips you and you go down!");
        ADD_COLOUR(ch, buf, WHITE);
        act(buf, ch, NULL, victim, TO_VICT);
        sprintf(buf, "You trip $N and $E goes down!");
        ADD_COLOUR(ch, buf, WHITE);
        act(buf,  ch, NULL, victim, TO_CHAR);
        sprintf(buf, "$n trips $N and $E goes down!");
        ADD_COLOUR(ch, buf, WHITE);
        act(buf,  ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch,     2 * PULSE_VIOLENCE);
        WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
        victim->position = POS_RESTING;
    }
    return;
}



void do_kill(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument(argument, arg);
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(arg[0] == '\0') {
        send_to_char("Kill whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(victim == ch) {
        send_to_char("You cannot kill yourself!\n\r", ch);
        return;
    }
    if(IS_AFFECTED(ch, AFF_FEAR)) {
        send_to_char("No way, you're too scared!\n\r", ch);
        return;
    }
    if(is_safe(ch, victim)) {
        return;
    }
    if(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
        act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }
    if(ch->position == POS_FIGHTING) {
        send_to_char("You do the best you can!\n\r", ch);
        return;
    }
    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
    check_killer(ch, victim);
    if(!IS_NPC(ch) && IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_BLACK_FURIES) > 2
            && victim->position == POS_STANDING) {
        act("You charge into $N, knocking $M from $S feet.", ch, NULL, victim, TO_CHAR);
        act("$n charge into $N, knocking $M from $S feet.", ch, NULL, victim, TO_NOTVICT);
        act("$n charge into you, knocking you from your feet.", ch, NULL, victim, TO_VICT);
        victim->position = POS_STUNNED;
        multi_hit(ch, victim, TYPE_UNDEFINED);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
    }
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}



void do_backstab(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int dam;
    one_argument(argument, arg);
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(arg[0] == '\0') {
        send_to_char("Backstab whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(victim == ch) {
        send_to_char("How can you sneak up on yourself?\n\r", ch);
        return;
    }
    if(is_safe(ch, victim)) {
        return;
    }
    if((obj = get_eq_char(ch, WEAR_WIELD)) == NULL ||
            obj->value[3] != 11) {
        if((obj = get_eq_char(ch, WEAR_HOLD)) == NULL ||
                obj->value[3] != 11) {
            send_to_char("You need to wield a piercing weapon.\n\r", ch);
            return;
        }
    }
    if(victim->fighting != NULL) {
        send_to_char("You can't backstab a fighting person.\n\r", ch);
        return;
    }
    if(victim->hit < victim->max_hit) {
        act("$N is hurt and suspicious ... you can't sneak up.",
            ch, NULL, victim, TO_CHAR);
        return;
    }
    check_killer(ch, victim);
    WAIT_STATE(ch, skill_table[gsn_backstab].beats);
    if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_BACKSTAB)) {
        damage(ch, victim, 0, gsn_backstab);
        return;
    }
    dam = victim->hit;
    if(!IS_AWAKE(victim)
            ||   IS_NPC(ch)
            ||   number_percent() < ch->pcdata->learned[gsn_backstab]) {
        multi_hit(ch, victim, gsn_backstab);
    } else {
        damage(ch, victim, 0, gsn_backstab);
        return;
    }
    if(victim != NULL && victim->position != POS_DEAD && !IS_NPC(victim)) {
        if(IS_DEMON(victim) && dam != victim->hit &&
                IS_SET(victim->pcdata->disc[C_POWERS], DEM_INFERNO)) {
            act("A blast of flames shoot from $N's body and up $p!", ch, obj, victim, TO_CHAR);
            act("A blast of flames shoot from $N's body and up $p!", ch, obj, victim, TO_NOTVICT);
            act("A blast of flames shoot from your body and up $p!", ch, obj, victim, TO_VICT);
            if(IS_SET(obj->quest, QUEST_ARTIFACT) || obj->chobj != NULL) {
                act("You drop $p as it burns your fingers!", ch, obj, NULL, TO_CHAR);
                act("$n drops $p as it burns $s fingers!", ch, obj, NULL, TO_ROOM);
                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);
            } else {
                act("You release $p as it burns to ashes!", ch, obj, NULL, TO_CHAR);
                act("$n releases $p as it burns to ashes!", ch, obj, NULL, TO_ROOM);
                obj_from_char(obj);
                if(obj->points > 0 && !IS_NPC(ch)) {
                    char buf[MAX_STRING_LENGTH];
                    sprintf(buf, "You receive a refund of %d quest points from $p.", obj->points);
                    act(buf, ch, obj, NULL, TO_CHAR);
                    ch->pcdata->quest += obj->points;
                }
                extract_obj(obj);
            }
        }
    }
    return;
}



void do_flee(CHAR_DATA *ch, char *argument) {
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;
    if((victim = ch->fighting) == NULL) {
        if(ch->position == POS_FIGHTING) {
            ch->position = POS_STANDING;
        }
        send_to_char("You aren't fighting anyone.\n\r", ch);
        return;
    }
    if(IS_AFFECTED(ch, AFF_WEBBED)) {
        send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
        return;
    } else if(IS_AFFECTED(ch, AFF_SHADOW)) {
        send_to_char("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
        return;
    } else if(IS_AFFECTED(ch, AFF_JAIL)) {
        send_to_char("You are unable to move with these bands of water entrapping you.\n\r", ch);
        return;
    }
    if(!IS_NPC(ch) && ch->pcdata->wolf >= 0) {
        if(IS_VAMPIRE(ch) && number_percent() <= ch->beast) {
            send_to_char("Your inner beast refuses to let you run!\n\r", ch);
            WAIT_STATE(ch, 12);
            return;
        } else if(IS_WEREWOLF(ch) && number_percent() <= ch->pcdata->wolf * 0.3) {
            send_to_char("Your rage is too great!\n\r", ch);
            WAIT_STATE(ch, 12);
            return;
        }
    }
    was_in = ch->in_room;
    for(attempt = 0; attempt < 6; attempt++) {
        EXIT_DATA *pexit;
        int door;
        door = number_door();
        if((pexit = was_in->exit[door]) == 0
                ||   pexit->to_room == NULL
                ||   IS_SET(pexit->exit_info, EX_CLOSED)
                || (IS_NPC(ch)
                    &&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))) {
            continue;
        }
        move_char(ch, door);
        if((now_in = ch->in_room) == was_in) {
            continue;
        }
        ch->in_room = was_in;
        act("$n has fled!", ch, NULL, NULL, TO_ROOM);
        ch->in_room = now_in;
        if(!IS_NPC(ch)) {
            send_to_char("You flee from combat!  Coward!\n\r", ch);
        }
        stop_fighting(ch, TRUE);
        return;
    }
    send_to_char("You were unable to escape!\n\r", ch);
    return;
}



void do_rescue(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    one_argument(argument, arg);
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(arg[0] == '\0') {
        send_to_char("Rescue whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(victim == ch) {
        send_to_char("What about fleeing instead?\n\r", ch);
        return;
    }
    if(!IS_NPC(ch) && IS_NPC(victim)) {
        send_to_char("They don't need your help!\n\r", ch);
        return;
    }
    if(ch->fighting == victim) {
        send_to_char("Too late.\n\r", ch);
        return;
    }
    if((fch = victim->fighting) == NULL) {
        send_to_char("That person is not fighting right now.\n\r", ch);
        return;
    }
    if(fch->fighting == NULL || fch->fighting != victim) {
        send_to_char("They are not being attacked.\n\r", ch);
        return;
    }
    if(is_safe(ch, fch) || is_safe(ch, victim)) {
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_rescue].beats);
    if(!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_rescue]) {
        send_to_char("You fail the rescue.\n\r", ch);
        return;
    }
    act("You rescue $N!",  ch, NULL, victim, TO_CHAR);
    act("$n rescues you!", ch, NULL, victim, TO_VICT);
    act("$n rescues $N!",  ch, NULL, victim, TO_NOTVICT);
    stop_fighting(ch, FALSE);
    stop_fighting(fch, FALSE);
    stop_fighting(victim, FALSE);
    check_killer(ch, fch);
    set_fighting(ch, fch);
    set_fighting(fch, ch);
    return;
}



void do_kick(CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *boots;
    int dam;
    int stance;
    int hitpoints;
    one_argument(argument, arg);
    if(!IS_NPC(ch)
            &&   ch->level < skill_table[gsn_kick].skill_level) {
        send_to_char(
            "First you should learn to kick.\n\r", ch);
        return;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(arg[0] == '\0' && ch->fighting == NULL) {
        send_to_char("Who do you wish to kick?\n\r", ch);
        return;
    }
    if((victim = ch->fighting) == NULL) {
        if((victim = get_char_room(ch, arg)) == NULL) {
            send_to_char("They aren't here.\n\r", ch);
            return;
        }
    }
    if(ch == victim) {
        send_to_char("Kick YOURSELF?\n\r", ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_kick].beats);
    if(IS_NPC(ch) || number_percent() <= ch->pcdata->learned[gsn_kick]) {
        dam = number_range(1, 4);
    } else {
        dam = 0;
        damage(ch, victim, dam, gsn_kick);
        return;
    }
    dam += char_damroll(ch);
    if(dam == 0) {
        dam = 1;
    }
    if(!IS_AWAKE(victim)) {
        dam *= 2;
    }
    if(!IS_NPC(ch) && IS_DEMON(ch) &&
            IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT)) {
        dam *= 1.5;
    }
    if(!IS_NPC(victim) && IS_WEREWOLF(victim)) {
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            dam *= 0.5;
        }
        if((boots = get_eq_char(ch, WEAR_FEET)) != NULL
                && IS_SET(boots->spectype, SITEM_SILVER)) {
            dam *= 2;
        }
    }
    dam = dam + (dam * ((ch->wpn[0]) * 0.01));
    /*
        if ( !IS_NPC(ch) )
    */
    {
        stance = ch->stance[0];
        if(IS_STANCE(ch, STANCE_NORMAL)) {
            dam *= 1.25;
        } else {
            dam = dambonus(ch, victim, dam, stance);
        }
    }
    if(dam <= 0) {
        dam = 1;
    }
    hitpoints = victim->hit;
    if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_KICK)) {
        if(!IS_NPC(ch) && IS_DEMON(ch) && IS_DEMAFF(ch, DEM_HOOVES)) {
            damage(ch, victim, dam, gsn_kick);
        } else {
            victim->hit += dam;
            damage(ch, victim, dam, gsn_kick);
            victim->hit = hitpoints;
        }
    } else {
        damage(ch, victim, dam, gsn_kick);
    }
    return;
}


int dambonus(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int stance) {
    if(dam < 1) {
        return 0;
    }
    if(stance < 1) {
        return dam;
    }
    if(!IS_NPC(ch) && !can_counter(victim)) {
        if(IS_STANCE(ch, STANCE_MONKEY)) {
            int mindam = dam * 0.25;
            dam *= (ch->stance[STANCE_MONKEY] + 1) / 200;
            if(dam < mindam) {
                dam = mindam;
            }
        } else if(IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100) {
            dam += dam * (ch->stance[STANCE_BULL] * 0.01);
        } else if(IS_STANCE(ch, STANCE_DRAGON) && ch->stance[STANCE_DRAGON] > 100) {
            dam += dam * (ch->stance[STANCE_DRAGON] * 0.01);
        } else if(IS_STANCE(ch, STANCE_TIGER) && ch->stance[STANCE_TIGER] > 100) {
            dam += dam * (ch->stance[STANCE_TIGER] * 0.01);
        } else if(ch->stance[0] > 0 && ch->stance[stance] < 100) {
            dam *= 0.5;
        } else if(!IS_NPC(ch) && stance >= 11 && stance <= 15) {
            if(ch->stance[stance] > 100) {
                if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_LESSER)) {
                    dam += dam * (ch->stance[stance] * 0.01);
                } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_GREATER)) {
                    dam += dam * (ch->stance[stance] * 0.02);
                } else if(IS_SET(ch->pcdata->qstats[stance - 11], QS_POW_SUPREME)) {
                    dam += dam * (ch->stance[stance] * 0.03);
                }
            }
        }
    }
    if(!can_counter(ch) && dam > 1) {
        int victstance = victim->stance[0];
        if(IS_STANCE(victim, STANCE_CRAB) && victim->stance[STANCE_CRAB] > 100) {
            dam /= victim->stance[STANCE_CRAB] * 0.01;
        } else if(IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100) {
            dam /= victim->stance[STANCE_DRAGON] * 0.01;
        } else if(IS_STANCE(victim, STANCE_SWALLOW) && victim->stance[STANCE_SWALLOW] > 100) {
            dam /= victim->stance[STANCE_SWALLOW] * 0.01;
        } else if(!IS_NPC(victim) && victstance >= 11 && victstance <= 15) {
            if(victim->stance[victstance] > 100) {
                if(IS_SET(victim->pcdata->qstats[stance - 11], QS_POW_LESSER)) {
                    dam /= victim->stance[stance] * 0.01;
                } else if(IS_SET(victim->pcdata->qstats[stance - 11], QS_POW_GREATER)) {
                    dam /= victim->stance[stance] * 0.02;
                } else if(IS_SET(victim->pcdata->qstats[stance - 11], QS_POW_SUPREME)) {
                    dam /= victim->stance[stance] * 0.03;
                }
            }
        }
    }
    return dam;
}



void do_punch(CHAR_DATA *ch, char *argument) {
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    int store;
    bool broke = FALSE;
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(ch->level < skill_table[gsn_punch].skill_level) {
        send_to_char("First you should learn to punch.\n\r", ch);
        return;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("Nobody here by that name.\n\r", ch);
        return;
    }
    if(ch == victim) {
        send_to_char("You cannot punch yourself!\n\r", ch);
        return;
    }
    if(victim->hit < victim->max_hit) {
        send_to_char("They are hurt and suspicious.\n\r", ch);
        return;
    }
    if(victim->position < POS_FIGHTING) {
        send_to_char("You can only punch someone who is standing.\n\r", ch);
        return;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(is_safe(ch, victim)) {
        return;
    }
    act("You draw your fist back and aim a punch at $N.", ch, NULL, victim, TO_CHAR);
    act("$n draws $s fist back and aims a punch at you.", ch, NULL, victim, TO_VICT);
    act("$n draws $s fist back and aims a punch at $N.", ch, NULL, victim, TO_NOTVICT);
    WAIT_STATE(ch, skill_table[gsn_punch].beats);
    if(IS_NPC(ch) || number_percent() <= ch->pcdata->learned[gsn_punch]) {
        dam = number_range(1, 4);
    } else {
        dam = 0;
        damage(ch, victim, dam, gsn_punch);
        return;
    }
    dam += char_damroll(ch);
    if(dam == 0) {
        dam = 1;
    }
    if(!IS_AWAKE(victim)) {
        dam *= 2;
    }
    dam = dam + (dam * ch->wpn[0] * 0.01);
    if(dam <= 0) {
        dam = 1;
    }
    if(!IS_NPC(victim) && (
                (IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_GET_OF_FENRIS) > 3)
                || (IS_VAMPIRE(victim) && get_disc(victim, DISC_FORTITUDE) >= 5))) {
        store = victim->hit;
        victim->hit += dam;
        damage(ch, victim, dam, gsn_punch);
        victim->hit = store;
        if(number_percent() <= 25 && !IS_ARM_L(ch, LOST_ARM) &&
                !IS_ARM_L(ch, LOST_HAND)) {
            if(!IS_ARM_L(ch, LOST_FINGER_I) && !IS_ARM_L(ch, BROKEN_FINGER_I)) {
                SET_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_I);
                broke = TRUE;
            }
            if(!IS_ARM_L(ch, LOST_FINGER_M) && !IS_ARM_L(ch, BROKEN_FINGER_M)) {
                SET_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_M);
                broke = TRUE;
            }
            if(!IS_ARM_L(ch, LOST_FINGER_R) && !IS_ARM_L(ch, BROKEN_FINGER_R)) {
                SET_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_R);
                broke = TRUE;
            }
            if(!IS_ARM_L(ch, LOST_FINGER_L) && !IS_ARM_L(ch, BROKEN_FINGER_L)) {
                SET_BIT(ch->loc_hp[LOC_ARM_L], BROKEN_FINGER_L);
                broke = TRUE;
            }
            if(broke) {
                act("The fingers on your left hand shatter under the impact of the blow!", ch, NULL, NULL, TO_CHAR);
                act("The fingers on $n's left hand shatter under the impact of the blow! ", ch, NULL, NULL, TO_ROOM);
            }
        } else if(number_percent() <= 25 && !IS_ARM_R(ch, LOST_ARM) &&
                  !IS_ARM_R(ch, LOST_HAND)) {
            if(!IS_ARM_R(ch, LOST_FINGER_I) && !IS_ARM_R(ch, BROKEN_FINGER_I)) {
                SET_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_I);
                broke = TRUE;
            }
            if(!IS_ARM_R(ch, LOST_FINGER_M) && !IS_ARM_R(ch, BROKEN_FINGER_M)) {
                SET_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_M);
                broke = TRUE;
            }
            if(!IS_ARM_R(ch, LOST_FINGER_R) && !IS_ARM_R(ch, BROKEN_FINGER_R)) {
                SET_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_R);
                broke = TRUE;
            }
            if(!IS_ARM_R(ch, LOST_FINGER_L) && !IS_ARM_R(ch, BROKEN_FINGER_L)) {
                SET_BIT(ch->loc_hp[LOC_ARM_R], BROKEN_FINGER_L);
                broke = TRUE;
            }
            if(broke) {
                act("The fingers on your right hand shatter under the impact of the blow!", ch, NULL, NULL, TO_CHAR);
                act("The fingers on $n's right hand shatter under the impact of the blow! ", ch, NULL, NULL, TO_ROOM);
            }
        }
        stop_fighting(victim, TRUE);
        return;
    }
    damage(ch, victim, dam, gsn_punch);
    if(victim == NULL || victim->position == POS_DEAD || dam < 1) {
        return;
    }
    if(victim->position == POS_FIGHTING) {
        stop_fighting(victim, TRUE);
    }
    if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_NOSE)
            && !IS_HEAD(victim, LOST_NOSE)) {
        act("Your nose shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
        act("$n's nose shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
        SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
    } else if(number_percent() <= 25 && !IS_HEAD(victim, BROKEN_JAW)) {
        act("Your jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_CHAR);
        act("$n's jaw shatters under the impact of the blow!", victim, NULL, NULL, TO_ROOM);
        SET_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
    }
    act("You fall to the ground stunned!", victim, NULL, NULL, TO_CHAR);
    act("$n falls to the ground stunned!", victim, NULL, NULL, TO_ROOM);
    victim->position = POS_STUNNED;
    return;
}


void do_berserk(CHAR_DATA *ch, char *argument) {
    char       arg [MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *mount;
    int number_hit = 0;
    argument = one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(ch->level < skill_table[gsn_berserk].skill_level) {
        send_to_char("You are not wild enough to go berserk.\n\r", ch);
        return;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    WAIT_STATE(ch, skill_table[gsn_berserk].beats);
    if(number_percent() > ch->pcdata->learned[gsn_berserk]) {
        act("You rant and rave, but nothing much happens.", ch, NULL, NULL, TO_CHAR);
        act("$n gets a wild look in $s eyes, but nothing much happens.", ch, NULL, NULL, TO_ROOM);
        return;
    }
    act("You go BERSERK!", ch, NULL, NULL, TO_CHAR);
    act("$n goes BERSERK!", ch, NULL, NULL, TO_ROOM);
    for(vch = char_list; vch != NULL; vch = vch_next) {
        vch_next	= vch->next;
        if(number_hit > 4) {
            continue;
        }
        if(vch->in_room == NULL) {
            continue;
        }
        if(!IS_NPC(vch) && vch->pcdata->chobj != NULL) {
            continue;
        }
        if(ch == vch) {
            continue;
        }
        if(vch->in_room == ch->in_room) {
            if((mount = ch->mount) != NULL) {
                if(mount == vch) {
                    continue;
                }
            }
            if(can_see(ch, vch)) {
                multi_hit(ch, vch, TYPE_UNDEFINED);
                number_hit++;
            }
        }
    }
    do_beastlike(ch, "");
    return;
}



/* Hurl skill by KaVir */
void do_hurl(CHAR_DATA *ch, char *argument) {
    CHAR_DATA       *victim;
    CHAR_DATA       *mount;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    EXIT_DATA       *pexit_rev;
    char            buf       [MAX_INPUT_LENGTH];
    char            direction [MAX_INPUT_LENGTH];
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    int             door;
    int             rev_dir;
    int             dam;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(arg1[0] == '\0') {
        send_to_char("Who do you wish to hurl?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg1)) == NULL) {
        send_to_char("They are not here.\n\r", ch);
        return;
    }
    if(victim == ch) {
        send_to_char("How can you hurl yourself?\n\r", ch);
        return;
    }
    if((mount = victim->mount) != NULL && victim->mounted == IS_MOUNT) {
        send_to_char("But they have someone on their back!\n\r", ch);
        return;
    } else if((mount = victim->mount) != NULL && victim->mounted == IS_RIDING) {
        send_to_char("But they are riding!\n\r", ch);
        return;
    }
    if((victim->hit < victim->max_hit)
            || (victim->position == POS_FIGHTING && victim->fighting != ch)) {
        act("$N is hurt and suspicious, and you are unable to approach $M.",
            ch, NULL, victim, TO_CHAR);
        return;
    }
    if(is_safe(ch, victim)) {
        return;
    }
    if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_HURL)) {
        send_to_char("You are unable to get their feet of the ground.\n\r", ch);
        return;
    }
    if(!IS_NPC(victim) && IS_WEREWOLF(victim) && get_tribe(victim, TRIBE_GET_OF_FENRIS) > 3) {
        send_to_char("You are unable to get their feet of the ground.\n\r", ch);
        return;
    }
    if(IS_NPC(victim) && victim->level > 900) {
        send_to_char("You are unable to get their feet of the ground.\n\r", ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_hurl].beats);
    if(!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_hurl]) {
        send_to_char("You are unable to get their feet of the ground.\n\r", ch);
        multi_hit(victim, ch, TYPE_UNDEFINED);
        return;
    }
    rev_dir = 0;
    if(arg2[0] == '\0') {
        door = number_range(0, 3);
    } else {
        if(!str_cmp(arg2, "n") || !str_cmp(arg2, "north")) {
            door = 0;
        } else if(!str_cmp(arg2, "e") || !str_cmp(arg2, "east")) {
            door = 1;
        } else if(!str_cmp(arg2, "s") || !str_cmp(arg2, "south")) {
            door = 2;
        } else if(!str_cmp(arg2, "w") || !str_cmp(arg2, "west")) {
            door = 3;
        } else {
            send_to_char("You can only hurl people north, south, east or west.\n\r", ch);
            return;
        }
    }
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
            ch->mkill = ch->mkill + 1;
        }
        if(!IS_NPC(victim) && IS_NPC(ch)) {
            victim->mdeath = victim->mdeath + 1;
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
        sprintf(buf, "$n hoists $N in the air and hurls $M %s.", direction);
        act(buf, ch, NULL, victim, TO_NOTVICT);
        sprintf(buf, "You hoist $N in the air and hurl $M %s.", direction);
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
                ch->mkill = ch->mkill + 1;
            }
            if(!IS_NPC(victim) && IS_NPC(ch)) {
                victim->mdeath = victim->mdeath + 1;
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
            ch->mkill = ch->mkill + 1;
        }
        if(!IS_NPC(victim) && IS_NPC(ch)) {
            victim->mdeath = victim->mdeath + 1;
        }
        if(victim->position == POS_DEAD) {
            victim->form = 7;
            raw_kill(victim, ch);
            return;
        }
    }
    return;
}



void do_disarm(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;
    if(!IS_NPC(ch)
            &&   ch->level < skill_table[gsn_disarm].skill_level) {
        send_to_char("You don't know how to disarm opponents.\n\r", ch);
        return;
    }
    if((get_eq_char(ch, WEAR_WIELD) == NULL)
            && (get_eq_char(ch, WEAR_HOLD) == NULL)) {
        send_to_char("You must wield a weapon to disarm.\n\r", ch);
        return;
    }
    if((victim = ch->fighting) == NULL) {
        send_to_char("You aren't fighting anyone.\n\r", ch);
        return;
    }
    if(((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
            && ((obj = get_eq_char(victim, WEAR_HOLD)) == NULL)) {
        send_to_char("Your opponent is not wielding a weapon.\n\r", ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_disarm].beats);
    percent = number_percent() + victim->hitroll - ch->hitroll;
    if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM)) {
        send_to_char("You failed.\n\r", ch);
    } else if(IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3) {
        disarm(ch, victim);
    } else {
        send_to_char("You failed.\n\r", ch);
    }
    return;
}



void do_sla(CHAR_DATA *ch, char *argument) {
    send_to_char("If you want to SLAY, spell it out.\n\r", ch);
    return;
}



void do_slay(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    one_argument(argument, arg);
    if(arg[0] == '\0') {
        send_to_char("Slay whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(ch == victim) {
        send_to_char("Suicide is a mortal sin.\n\r", ch);
        return;
    }
    if(IS_SET(victim->act, PLR_GODLESS)) {
        send_to_char("You failed.\n\r", ch);
        return;
    }
    if(!IS_NPC(victim) && victim->level >= ch->level) {
        send_to_char("You failed.\n\r", ch);
        return;
    }
    act("You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR);
    act("$n slays you in cold blood!", ch, NULL, victim, TO_VICT);
    act("$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT);
    victim->form = 18;
    raw_kill(victim, ch);
    return;
}

/* Had problems with people not dying when POS_DEAD...KaVir */
void killperson(CHAR_DATA *ch, CHAR_DATA *victim) {
    send_to_char("You have been KILLED!!\n\r", victim);
    raw_kill(victim, ch);
    return;
}

/* For decapitating players - KaVir */
void do_decapitate(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    OBJ_DATA  *obj;
    OBJ_DATA  *obj2;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(IS_MORE(ch, MORE_NPC)) {
        send_to_char("NPC's cannot decapitate other players.\n\r", ch);
        return;
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    obj2 = get_eq_char(ch, WEAR_HOLD);
    if((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {
        if((obj2 = get_eq_char(ch, WEAR_HOLD)) == NULL) {
            send_to_char("First you better get a weapon out!\n\r", ch);
            return;
        }
    }
    if((obj != NULL && obj->item_type != ITEM_WEAPON)) {
        if((obj2 != NULL && obj2->item_type != ITEM_WEAPON)) {
            send_to_char("But you are not wielding any weapons!\n\r", ch);
            return;
        }
    }
    if((obj  != NULL && obj->value[3]  != 1 && obj->value[3]  != 3)
            && (obj2 != NULL && obj2->value[3] != 1 && obj2->value[3] != 3)) {
        send_to_char("You need to wield a slashing or slicing weapon to decapitate.\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Decapitate whom?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(ch == victim) {
        send_to_char("That might be a bit tricky...\n\r", ch);
        return;
    }
    if(IS_NPC(victim)) {
        send_to_char("You can only decapitate other players.\n\r", ch);
        return;
    }
    if(!CAN_PK(ch) || IS_NEWBIE(ch)) {
        send_to_char("You are not permitted to decapitate other players.\n\r", ch);
        return;
    }
    if(!CAN_PK(victim) || IS_NEWBIE(victim)) {
        send_to_char("You cannot decapitate newbies.\n\r", ch);
        return;
    }
    if(victim->position > 1) {
        send_to_char("You can only do this to mortally wounded players.\n\r", ch);
        return;
    }
    if(ch->in_room == NULL || IS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
        send_to_char("You cannot decapitate someone in this room.\n\r", ch);
        return;
    }
    act("You bring your weapon down upon $N's neck!",  ch, NULL, victim, TO_CHAR);
    send_to_char("Your head is sliced from your shoulders!\n\r", victim);
    act("$n swings $s weapon down towards $N's neck!",  ch, NULL, victim, TO_NOTVICT);
    act("$n's head is sliced from $s shoulders!", victim, NULL, NULL, TO_ROOM);
    if(strlen(victim->pcdata->love) > 1) {
        if(!str_cmp(ch->name, victim->pcdata->love)) {
            free_string(victim->pcdata->love);
            victim->pcdata->love = str_dup("");
        }
    }
    if((vch = victim->embrace) != NULL) {
        vch->embrace = NULL;
        vch->embraced = ARE_NONE;
    }
    victim->embrace = NULL;
    victim->embraced = ARE_NONE;
    if(IS_DEMON(ch) && !IS_DEMON(victim)) {
        if(ch->race == 0 && victim->race == 0) {
            ch->pcdata->power[POWER_CURRENT] += 1000;
            ch->pcdata->power[POWER_TOTAL] += 1000;
        } else {
            ch->pcdata->power[POWER_CURRENT] += victim->race * 1000;
            ch->pcdata->power[POWER_TOTAL] += victim->race * 1000;
        }
    }
    if(victim->race < 1 && ch->race > 0) {
        ch->paradox[0] += ch->race;
        ch->paradox[1] += ch->race;
        ch->paradox[2] += PARADOX_TICK;
        sprintf(buf, "%s has been decapitated by %s for no status.", victim->name, ch->name);
        do_fatality(ch, buf);
        sprintf(log_buf, "%s decapitated by %s at %d for no status.",
                victim->name, ch->name, victim->in_room->vnum);
        log_string(log_buf);
        victim->pcdata->wolf = 0;
        if(IS_WEREWOLF(victim)) {
            if(IS_SET(victim->act, PLR_WOLFMAN)) {
                do_unwerewolf(victim, "");
            } else if(victim->pcdata->wolfform[5] != FORM_HOMID) {
                do_shift(victim, "");
            }
        }
        if(IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_nightsight(victim, "");
        }
        if(IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_fangs(victim, "");
        }
        if(IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_claws(victim, "");
        }
        mortal_vamp(victim);
        if(IS_MAGE(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
        behead(victim, ch);
        do_beastlike(ch, "");
        ch->pkill = ch->pkill + 1;
        victim->pdeath = victim->pdeath + 1;
        return;
    }
    if((ch->race < 1 && victim->race >= 0) ||
            (ch->race >= 1 && victim->race >= 1)) {
        if(strlen(ch->side) < 2 || strlen(victim->side) < 2 ||
                str_cmp(ch->side, victim->side)) {
            ch->race++;
            if(victim->race > 0) {
                victim->race--;
            }
        } else if(ch->race > 0) {
            ch->race--;
        }
    }
    if(IS_VAMPIRE(victim)) {
        act("An oily black vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR);
        act("An oily black vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as the strength of Caine pulses through your veins!",  ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as the strength of Caine infuses $m!",  ch, NULL, NULL, TO_NOTVICT);
        do_mortalvamp(victim, "");
    } else if(IS_WEREWOLF(victim)) {
        act("An ethereal grey vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR);
        act("A ethereal grey vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as the power of Gaia pulses through your veins!",  ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as the power of Gaia infuses $m!",  ch, NULL, NULL, TO_NOTVICT);
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            do_unwerewolf(victim, "");
        }
    } else if(IS_MAGE(victim)) {
        char *mage_col;
        switch(victim->pcdata->runes[0]) {
        default: /* Defaults to purple */
        case PURPLE_MAGIC:
            mage_col = "purple";
            break;
        case BLUE_MAGIC:
            mage_col = "blue";
            break;
        case RED_MAGIC:
            mage_col = "red";
            break;
        case GREEN_MAGIC:
            mage_col = "green";
            break;
        case YELLOW_MAGIC:
            mage_col = "yellow";
            break;
        }
        sprintf(buf, "A glowing %s vapour pours from $N's corpse into your body.", mage_col);
        act(buf,  ch, NULL, victim, TO_CHAR);
        sprintf(buf, "A glowing %s vapour pours from $N's corpse into $n's body.", mage_col);
        act(buf,  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as arcane magic pulses through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as arcane magic shimmers around $m!", ch, NULL, NULL, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
    } else if(IS_DEMON(victim)) {
        act("Infernal flames shoot from $N's corpse into your body.", ch, NULL, victim, TO_CHAR);
        act("Infernal flames pour from $N's corpse into $n's body.", ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as infernal fire burns through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as infernal fire surrounds $m!", ch, NULL, NULL, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
    } else { /* Highlander or normal avatar */
        act("A misty white vapour pours from $N's corpse into your body.", ch, NULL, victim, TO_CHAR);
        act("A misty white vapour pours from $N's corpse into $n's body.", ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as raw energy pulses through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as sparks of energy crackle around $m!", ch, NULL, NULL, TO_NOTVICT);
    }
    victim->pcdata->wolf = 0;
    if(IS_WEREWOLF(victim)) {
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            do_unwerewolf(victim, "");
        } else if(victim->pcdata->wolfform[5] != FORM_HOMID) {
            do_shift(victim, "");
        }
    }
    if(IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_nightsight(victim, "");
    }
    if(IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_fangs(victim, "");
    }
    if(IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_claws(victim, "");
    }
    mortal_vamp(victim);
    if(IS_MAGE(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        do_unpolymorph(victim, "");
    }
    behead(victim, ch);
    do_beastlike(ch, "");
    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->pcdata->wolf = 0;
    sprintf(buf, "%s has been decapitated by %s.", victim->name, ch->name);
    do_fatality(ch, buf);
    sprintf(log_buf, "%s decapitated by %s at %d.",
            victim->name, ch->name, victim->in_room->vnum);
    log_string(log_buf);
    return;
}

void do_tear(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(!IS_WEREWOLF(ch)) {
        if(!IS_VAMPIRE(ch) || (IS_VAMPIRE(ch) && ch->pcdata->wolf < 1)) {
            send_to_char("Huh?\n\r", ch);
            return;
        }
    }
    if(IS_ITEMAFF(ch, ITEMA_PEACE)) {
        do_majesty(ch, "");
    }
    if(IS_WEREWOLF(ch) && !IS_SET(ch->act, PLR_WOLFMAN)) {
        send_to_char("You can only tear heads off while in Crinos form.\n\r", ch);
        return;
    }
    if(IS_WEREWOLF(ch) && !IS_VAMPAFF(ch, VAM_CLAWS)) {
        send_to_char("You better get your claws out first.\n\r", ch);
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("Who's head do you wish to tear off?\n\r", ch);
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if(ch == victim) {
        send_to_char("That might be a bit tricky...\n\r", ch);
        return;
    }
    if(IS_NPC(victim)) {
        send_to_char("You can only tear the heads off other players.\n\r", ch);
        return;
    }
    if(!CAN_PK(ch) || IS_NEWBIE(ch)) {
        send_to_char("You are not permitted to tear the heads off other players.\n\r", ch);
        return;
    }
    if(!CAN_PK(victim) || IS_NEWBIE(victim)) {
        send_to_char("You cannot tear the heads off newbies.\n\r", ch);
        return;
    }
    if(victim->position > 1) {
        send_to_char("You can only do this to mortally wounded players.\n\r", ch);
        return;
    }
    if(is_safe(ch, victim)) {
        return;
    }
    act("You tear $N's head from $S shoulders!",  ch, NULL, victim, TO_CHAR);
    send_to_char("Your head is torn from your shoulders!\n\r", victim);
    act("$n tears $N's head from $S shoulders!",  ch, NULL, victim, TO_NOTVICT);
    if(strlen(victim->pcdata->love) > 1) {
        if(!str_cmp(ch->name, victim->pcdata->love)) {
            free_string(victim->pcdata->love);
            victim->pcdata->love = str_dup("");
        }
    }
    if((vch = victim->embrace) != NULL) {
        vch->embrace = NULL;
        vch->embraced = ARE_NONE;
    }
    victim->embrace = NULL;
    victim->embraced = ARE_NONE;
    if(IS_DEMON(ch) && !IS_DEMON(victim)) {
        if(ch->race == 0 && victim->race == 0) {
            ch->pcdata->power[POWER_CURRENT] += 1000;
            ch->pcdata->power[POWER_TOTAL] += 1000;
        } else {
            ch->pcdata->power[POWER_CURRENT] += victim->race * 1000;
            ch->pcdata->power[POWER_TOTAL] += victim->race * 1000;
        }
    }
    if(victim->race < 1 && ch->race > 0) {
        ch->paradox[0] += ch->race;
        ch->paradox[1] += ch->race;
        ch->paradox[2] += PARADOX_TICK;
        sprintf(buf, "%s has been decapitated by %s for no status.", victim->name, ch->name);
        do_fatality(ch, buf);
        sprintf(log_buf, "%s decapitated by %s at %d for no status.",
                victim->name, ch->name, victim->in_room->vnum);
        log_string(log_buf);
        victim->pcdata->wolf = 0;
        if(IS_WEREWOLF(victim)) {
            if(IS_SET(victim->act, PLR_WOLFMAN)) {
                do_unwerewolf(victim, "");
            } else if(victim->pcdata->wolfform[5] != FORM_HOMID) {
                do_shift(victim, "");
            }
        }
        if(IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_nightsight(victim, "");
        }
        if(IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_fangs(victim, "");
        }
        if(IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) {
            do_claws(victim, "");
        }
        mortal_vamp(victim);
        if(IS_MAGE(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
        behead(victim, ch);
        do_beastlike(ch, "");
        ch->pkill = ch->pkill + 1;
        victim->pdeath = victim->pdeath + 1;
        return;
    }
    if((ch->race < 1 && victim->race >= 0) ||
            (ch->race >= 1 && victim->race >= 1)) {
        if(strlen(ch->side) < 2 || strlen(victim->side) < 2 ||
                str_cmp(ch->side, victim->side)) {
            ch->race++;
            if(victim->race > 0) {
                victim->race--;
            }
        } else if(ch->race > 0) {
            ch->race--;
        }
    }
    if(IS_VAMPIRE(victim)) {
        act("An oily black vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR);
        act("An oily black vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as the strength of Caine pulses through your veins!",  ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as the strength of Caine infuses $m!",  ch, NULL, NULL, TO_NOTVICT);
        do_mortalvamp(victim, "");
    } else if(IS_WEREWOLF(victim)) {
        act("An ethereal grey vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR);
        act("A ethereal grey vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as the power of Gaia pulses through your veins!",  ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as the power of Gaia infuses $m!",  ch, NULL, NULL, TO_NOTVICT);
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            do_unwerewolf(victim, "");
        }
    } else if(IS_MAGE(victim)) {
        char *mage_col;
        switch(victim->pcdata->runes[0]) {
        default: /* Defaults to purple */
        case PURPLE_MAGIC:
            mage_col = "purple";
            break;
        case BLUE_MAGIC:
            mage_col = "blue";
            break;
        case RED_MAGIC:
            mage_col = "red";
            break;
        case GREEN_MAGIC:
            mage_col = "green";
            break;
        case YELLOW_MAGIC:
            mage_col = "yellow";
            break;
        }
        sprintf(buf, "A glowing %s vapour pours from $N's corpse into your body.", mage_col);
        act(buf,  ch, NULL, victim, TO_CHAR);
        sprintf(buf, "A glowing %s vapour pours from $N's corpse into $n's body.", mage_col);
        act(buf,  ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as arcane magic pulses through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as arcane magic shimmers around $m!", ch, NULL, NULL, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
    } else if(IS_DEMON(victim)) {
        act("Infernal flames shoot from $N's corpse into your body.", ch, NULL, victim, TO_CHAR);
        act("Infernal flames pour from $N's corpse into $n's body.", ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as infernal fire burns through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as infernal fire surrounds $m!", ch, NULL, NULL, TO_NOTVICT);
        if(IS_AFFECTED(victim, AFF_POLYMORPH)) {
            do_unpolymorph(victim, "");
        }
    } else { /* Highlander or normal avatar */
        act("A misty white vapour pours from $N's corpse into your body.", ch, NULL, victim, TO_CHAR);
        act("A misty white vapour pours from $N's corpse into $n's body.", ch, NULL, victim, TO_NOTVICT);
        act("You double over in agony as raw energy pulses through your veins!", ch, NULL, NULL, TO_CHAR);
        act("$n doubles over in agony as sparks of energy crackle around $m!", ch, NULL, NULL, TO_NOTVICT);
    }
    victim->pcdata->wolf = 0;
    if(IS_WEREWOLF(victim)) {
        if(IS_SET(victim->act, PLR_WOLFMAN)) {
            do_unwerewolf(victim, "");
        } else if(victim->pcdata->wolfform[5] != FORM_HOMID) {
            do_shift(victim, "");
        }
    }
    if(IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_nightsight(victim, "");
    }
    if(IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_fangs(victim, "");
    }
    if(IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) {
        do_claws(victim, "");
    }
    mortal_vamp(victim);
    if(IS_MAGE(victim) && IS_AFFECTED(victim, AFF_POLYMORPH)) {
        do_unpolymorph(victim, "");
    }
    behead(victim, ch);
    do_beastlike(ch, "");
    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->pcdata->wolf = 0;
    sprintf(buf, "%s has been decapitated by %s.", victim->name, ch->name);
    do_fatality(ch, buf);
    sprintf(log_buf, "%s decapitated by %s at %d.",
            victim->name, ch->name, victim->in_room->vnum);
    log_string(log_buf);
    return;
}

void do_crack(CHAR_DATA *ch, char *argument) {
    OBJ_DATA  *obj;
    OBJ_DATA  *right;
    OBJ_DATA  *left;
    right = get_eq_char(ch, WEAR_WIELD);
    left = get_eq_char(ch, WEAR_HOLD);
    if(right != NULL && right->pIndexData->vnum == 12) {
        obj = right;
    } else if(left != NULL && left->pIndexData->vnum == 12) {
        obj = left;
    } else {
        send_to_char("You are not holding any heads.\n\r", ch);
        return;
    }
    act("You hurl $p at the floor.", ch, obj, NULL, TO_CHAR);
    act("$n hurls $p at the floor.", ch, obj, NULL, TO_ROOM);
    act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_CHAR);
    if(obj->chobj != NULL) {
        act("$p cracks open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_NOTVICT);
        act("$p crack open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_VICT);
    } else {
        act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_ROOM);
    }
    crack_head(ch, obj, obj->name);
    obj_from_char(obj);
    extract_obj(obj);
}

void crack_head(CHAR_DATA *ch, OBJ_DATA *obj, char *argument) {
    CHAR_DATA *victim;
    MOB_INDEX_DATA *pMobIndex;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(str_cmp(arg2, "mob") && obj->chobj != NULL && !IS_NPC(obj->chobj) &&
            IS_AFFECTED(obj->chobj, AFF_POLYMORPH)) {
        victim = obj->chobj;
        make_part(victim, "cracked_head");
        make_part(victim, "brain");
        sprintf(buf, "the quivering brain of %s", victim->name);
        free_string(victim->morph);
        victim->morph = str_dup(buf);
        return;
    } else if(!str_cmp(arg2, "mob")) {
        if((pMobIndex = get_mob_index(obj->value[1])) == NULL) {
            return;
        }
        victim = create_mobile(pMobIndex);
        char_to_room(victim, ch->in_room);
        make_part(victim, "cracked_head");
        make_part(victim, "brain");
        extract_char(victim, TRUE);
        return;
    } else {
        if((pMobIndex = get_mob_index(30002)) == NULL) {
            return;
        }
        victim = create_mobile(pMobIndex);
        sprintf(buf, capitalize(arg2));
        free_string(victim->short_descr);
        victim->short_descr = str_dup(buf);
        char_to_room(victim, ch->in_room);
        make_part(victim, "cracked_head");
        make_part(victim, "brain");
        extract_char(victim, TRUE);
        return;
    }
    return;
}

/* Voodoo skill by KaVir */

void do_voodoo(CHAR_DATA *ch, char *argument) {
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    char     part1 [MAX_INPUT_LENGTH];
    char     part2 [MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if(arg1[0] == '\0') {
        send_to_char("Who do you wish to use voodoo magic on?\n\r", ch);
        return;
    }
    if((obj = get_eq_char(ch, WEAR_HOLD)) == NULL) {
        send_to_char("You are not holding a voodoo doll.\n\r", ch);
        return;
    }
    if((victim = get_char_world(ch, arg1)) == NULL) {
        send_to_char("They are not here.\n\r", ch);
        return;
    }
    if(IS_NPC(victim)) {
        send_to_char("Not on NPC's.\n\r", ch);
        return;
    }
    sprintf(part2, obj->name);
    sprintf(part1, "%s voodoo doll", victim->name);
    if(str_cmp(part1, part2)) {
        sprintf(buf, "But you are holding %s, not %s!\n\r", obj->short_descr, victim->name);
        send_to_char(buf, ch);
        return;
    }
    if(arg2[0] == '\0') {
        send_to_char("You can 'stab', 'burn' or 'throw' the doll.\n\r", ch);
    } else if(!str_cmp(arg2, "stab")) {
        WAIT_STATE(ch, 12);
        act("You stab a pin through $p.", ch, obj, NULL, TO_CHAR);
        act("$n stabs a pin through $p.", ch, obj, NULL, TO_ROOM);
        if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) {
            return;
        }
        act("You feel an agonising pain in your chest!", victim, NULL, NULL, TO_CHAR);
        act("$n clutches $s chest in agony!", victim, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg2, "burn")) {
        WAIT_STATE(ch, 12);
        act("You set fire to $p.", ch, obj, NULL, TO_CHAR);
        act("$n sets fire to $p.", ch, obj, NULL, TO_ROOM);
        act("$p burns to ashes.", ch, obj, NULL, TO_CHAR);
        act("$p burns to ashes.", ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        extract_obj(obj);
        if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) {
            return;
        }
        if(IS_AFFECTED(victim, AFF_FLAMING)) {
            return;
        }
        SET_BIT(victim->affected_by, AFF_FLAMING);
        act("You suddenly burst into flames!", victim, NULL, NULL, TO_CHAR);
        act("$n suddenly bursts into flames!", victim, NULL, NULL, TO_ROOM);
    } else if(!str_cmp(arg2, "throw")) {
        WAIT_STATE(ch, 12);
        act("You throw $p to the ground.", ch, obj, NULL, TO_CHAR);
        act("$n throws $p to the ground.", ch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, ch->in_room);
        if(!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) {
            return;
        }
        if(victim->position < POS_STANDING) {
            return;
        }
        if(victim->position == POS_FIGHTING) {
            stop_fighting(victim, TRUE);
        }
        act("A strange force picks you up and hurls you to the ground!", victim, NULL, NULL, TO_CHAR);
        act("$n is hurled to the ground by a strange force.", victim, NULL, NULL, TO_ROOM);
        victim->position = POS_RESTING;
        victim->hit = victim->hit - number_range(ch->level, (5 * ch->level));
        update_pos(victim);
        if(victim->position == POS_DEAD && !IS_NPC(victim)) {
            victim->form = 7;
            killperson(ch, victim);
            return;
        }
    } else {
        send_to_char("You can 'stab', 'burn' or 'throw' the doll.\n\r", ch);
    }
    return;
}

void do_firstaid(CHAR_DATA *ch, char *argument) {
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int  loc_dam;
    int  ribs = 0;
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They are not here.\n\r", ch);
        return;
    }
    if(ch->pcdata->firstaid > 0) {
        sprintf(buf, "You cannot perform first aid for another %d seconds.\n\r", ch->pcdata->firstaid);
        send_to_char(buf, ch);
        return;
    }
    if(ch == victim) {
        send_to_char("You cannot perform first aid on yourself.\n\r", ch);
        return;
    }
    if(victim->position > POS_RESTING || victim->position == POS_MEDITATING) {
        send_to_char("You better get them to rest first.\n\r", ch);
        return;
    }
    loc_dam = victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
              victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] +
              victim->loc_hp[6];
    if(victim->hit + 1000 >= victim->max_hit && loc_dam < 1) {
        send_to_char("They are not that badly injured.\n\r", ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_firstaid].beats);
    ch->pcdata->firstaid = 5;
    if(number_percent() > ch->pcdata->learned[gsn_firstaid]) {
        send_to_char("You failed.\n\r", ch);
        return;
    }
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
    if(IS_BLEEDING(victim, BLEEDING_HEAD)) {
        act("You wrap some bandages around $S head.", ch, NULL, victim, TO_CHAR);
        act("$n wraps some bandagess around $N's head.", ch, NULL, victim, TO_NOTVICT);
        act("$n wraps some bandagess around your head.", ch, NULL, victim, TO_VICT);
        act("$n's head stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("Your head stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_HEAD);
    } else if(IS_BLEEDING(victim, BLEEDING_THROAT)) {
        act("You wrap some bandages around $S throat.", ch, NULL, victim, TO_CHAR);
        act("$n wraps some bandagess around $N's throat.", ch, NULL, victim, TO_NOTVICT);
        act("$n wraps some bandagess around your throat.", ch, NULL, victim, TO_VICT);
        act("$n's throat stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("Your throat stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_THROAT);
    } else if(IS_BLEEDING(victim, BLEEDING_ARM_L)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's left arm stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your left arm stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_L);
    } else if(IS_BLEEDING(victim, BLEEDING_ARM_R)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's right arm stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your right arm stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_ARM_R);
    } else if(IS_BLEEDING(victim, BLEEDING_LEG_L)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's left leg stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your left leg stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_L);
    } else if(IS_BLEEDING(victim, BLEEDING_LEG_R)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's right leg stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your right leg stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_LEG_R);
    } else if(IS_BLEEDING(victim, BLEEDING_HAND_L)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's left wrist stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your left wrist stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_L);
    } else if(IS_BLEEDING(victim, BLEEDING_HAND_R)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's right wrist stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your right wrist stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_HAND_R);
    } else if(IS_BLEEDING(victim, BLEEDING_FOOT_L)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's left ankle stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your left ankle stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_L);
    } else if(IS_BLEEDING(victim, BLEEDING_FOOT_R)) {
        act("You press a burning hot iron against $S stump.", ch, NULL, victim, TO_CHAR);
        act("$n press a burning hot iron against $N's stump.", ch, NULL, victim, TO_NOTVICT);
        act("$n press a burning hot iron against your stump.", ch, NULL, victim, TO_VICT);
        act("The stump of $n's right ankle stops bleeding.", victim, NULL, NULL, TO_ROOM);
        act("The stump of your right ankle stops bleeding.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[6], BLEEDING_FOOT_R);
    } else if(ribs > 0) {
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
        ribs -= 1;
        if(ribs >= 16) {
            ribs -= 16;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_16);
        }
        if(ribs >= 8) {
            ribs -= 8;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_8);
        }
        if(ribs >= 4) {
            ribs -= 4;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_4);
        }
        if(ribs >= 2) {
            ribs -= 2;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_2);
        }
        if(ribs >= 1) {
            ribs -= 1;
            SET_BIT(victim->loc_hp[1], BROKEN_RIBS_1);
        }
        act("You wrap a tight bandage around $S chest.", ch, NULL, victim, TO_CHAR);
        act("$n wraps a tight bandage around $N's chest.", ch, NULL, victim, TO_NOTVICT);
        act("$n wraps a tight bandage around your chest.", ch, NULL, victim, TO_VICT);
        act("One of $n's ribs snap back into place.", victim, NULL, NULL, TO_ROOM);
        act("One of your ribs snap back into place.", victim, NULL, NULL, TO_CHAR);
    } else if(IS_HEAD(victim, BROKEN_NOSE) && !IS_HEAD(victim, LOST_NOSE)) {
        act("You give $S nose a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's nose a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your nose a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's nose snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your nose snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_HEAD], BROKEN_NOSE);
    } else if(IS_HEAD(victim, BROKEN_JAW)) {
        act("You give $S jaw a hard whack.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's jaw a hard whack.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your jaw a hard whack.", ch, NULL, victim, TO_VICT);
        act("$n's jaw snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your jaw snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_HEAD], BROKEN_JAW);
    } else if(IS_HEAD(victim, BROKEN_SKULL)) {
        act("You wrap a tight bandage around $S head.", ch, NULL, victim, TO_CHAR);
        act("$n wraps a tight bandage around $N's head.", ch, NULL, victim, TO_NOTVICT);
        act("$n wraps a tight bandage around your head.", ch, NULL, victim, TO_VICT);
        act("The crack in $n's skull closes.", victim, NULL, NULL, TO_ROOM);
        act("The crack in your skull closes.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_HEAD], BROKEN_SKULL);
    } else if(IS_BODY(victim, BROKEN_SPINE)) {
        act("You run your hands firmly down $S back.", ch, NULL, victim, TO_CHAR);
        act("$n runs $s hands firmly down $N's back.", ch, NULL, victim, TO_NOTVICT);
        act("$n runs $s hands firmly down your back.", ch, NULL, victim, TO_VICT);
        act("$n's spine cracks and pops back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your spine cracks and pops back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_BODY], BROKEN_SPINE);
    } else if(IS_BODY(victim, BROKEN_NECK)) {
        act("You give $S neck a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's neck a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your neck a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's neck snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your neck snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_BODY], BROKEN_NECK);
    } else if(IS_ARM_L(victim, BROKEN_ARM) && !IS_ARM_L(victim, LOST_ARM)) {
        act("You give $S arm a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's arm a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your arm a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left arm snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left arm snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_ARM);
    } else if(IS_ARM_R(victim, BROKEN_ARM) && !IS_ARM_R(victim, LOST_ARM)) {
        act("You give $S arm a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's arm a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your arm a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right arm snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right arm snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_ARM);
    } else if(IS_LEG_L(victim, BROKEN_LEG) && !IS_LEG_L(victim, LOST_LEG)) {
        act("You give $S leg a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's leg a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your leg a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left leg snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left leg snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_LEG_L], BROKEN_LEG);
    } else if(IS_LEG_R(victim, BROKEN_LEG) && !IS_LEG_R(victim, LOST_LEG)) {
        act("You give $S leg a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's leg a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your leg a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right leg snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right leg snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_LEG_R], BROKEN_LEG);
    } else if(IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_ARM)
              && !IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_THUMB)) {
        act("You give $S thumb a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's thumb a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your thumb a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left thumb snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left thumb snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_THUMB);
    } else if(IS_ARM_L(victim, BROKEN_FINGER_I) && !IS_ARM_L(victim, LOST_ARM)
              && !IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_FINGER_I)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left index finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left index finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_FINGER_I);
    } else if(IS_ARM_L(victim, BROKEN_FINGER_M) && !IS_ARM_L(victim, LOST_ARM)
              && !IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_FINGER_M)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left middle finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left middle finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_FINGER_M);
    } else if(IS_ARM_L(victim, BROKEN_FINGER_R) && !IS_ARM_L(victim, LOST_ARM)
              && !IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_FINGER_R)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left ring finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left ring finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_FINGER_R);
    } else if(IS_ARM_L(victim, BROKEN_FINGER_L) && !IS_ARM_L(victim, LOST_ARM)
              && !IS_ARM_L(victim, LOST_HAND) && !IS_ARM_L(victim, LOST_FINGER_L)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's left little finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your left little finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_L], BROKEN_FINGER_L);
    } else if(IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_ARM)
              && !IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_THUMB)) {
        act("You give $S thumb a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's thumb a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your thumb a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right thumb snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right thumb snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_THUMB);
    } else if(IS_ARM_R(victim, BROKEN_FINGER_I) && !IS_ARM_R(victim, LOST_ARM)
              && !IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_FINGER_I)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right index finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right index finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_FINGER_I);
    } else if(IS_ARM_R(victim, BROKEN_FINGER_M) && !IS_ARM_R(victim, LOST_ARM)
              && !IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_FINGER_M)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right middle finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right middle finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_FINGER_M);
    } else if(IS_ARM_R(victim, BROKEN_FINGER_R) && !IS_ARM_R(victim, LOST_ARM)
              && !IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_FINGER_R)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right ring finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right ring finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_FINGER_R);
    } else if(IS_ARM_R(victim, BROKEN_FINGER_L) && !IS_ARM_R(victim, LOST_ARM)
              && !IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_FINGER_L)) {
        act("You give $S finger a sharp twist.", ch, NULL, victim, TO_CHAR);
        act("$n gives $N's finger a sharp twist.", ch, NULL, victim, TO_NOTVICT);
        act("$n gives your finger a sharp twist.", ch, NULL, victim, TO_VICT);
        act("$n's right little finger snaps back into place.", victim, NULL, NULL, TO_ROOM);
        act("Your right little finger snaps back into place.", victim, NULL, NULL, TO_CHAR);
        REMOVE_BIT(victim->loc_hp[LOC_ARM_R], BROKEN_FINGER_L);
    } else if(IS_BODY(victim, CUT_THROAT)) {
        if(IS_SET(victim->loc_hp[6], BLEEDING_THROAT)) {
            send_to_char("But their throat is still bleeding!\n\r", ch);
        } else {
            act("You carefully stitch together $S throat.", ch, NULL, victim, TO_CHAR);
            act("$n carefully stitches together $N's throat.", ch, NULL, victim, TO_NOTVICT);
            act("$n carefully stitches together your throat.", ch, NULL, victim, TO_VICT);
            act("The wound in $n's throat closes up.", victim, NULL, NULL, TO_ROOM);
            act("The wound in your throat closes up.", victim, NULL, NULL, TO_CHAR);
            REMOVE_BIT(victim->loc_hp[LOC_BODY], CUT_THROAT);
        }
    } else {
        act("You bend over $N's body and bandage $S wounds.", ch, NULL, victim, TO_CHAR);
        act("$n bends over $N's body and bandages $S wounds.", ch, NULL, victim, TO_NOTVICT);
        act("$n bends over your body and bandages your wounds.", ch, NULL, victim, TO_VICT);
        victim->hit = UMIN(victim->hit + (10 * ch->pcdata->learned[gsn_firstaid]), victim->max_hit);
    }
    update_pos(victim);
    return;
}

void do_repair(CHAR_DATA *ch, char *argument) {
    OBJ_DATA *obj;
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];
    int chance = number_percent();
    one_argument(argument, arg);
    if(IS_NPC(ch)) {
        return;
    }
    if(arg[0] == '\0') {
        send_to_char("What do you want to repair.\n\r", ch);
        return;
    }
    if((obj = get_obj_carry(ch, arg)) == NULL) {
        send_to_char("You are not carrying that object.\n\r", ch);
        return;
    }
    if(ch->pcdata->repair > 0) {
        sprintf(buf, "You cannot perform any repairs for another %d seconds.\n\r", ch->pcdata->repair);
        send_to_char(buf, ch);
        return;
    }
    if(obj->item_type != ITEM_ARMOR) {
        send_to_char("You can only repair armour.\n\r", ch);
        return;
    }
    if(obj->condition >= 100) {
        send_to_char("But it doesn't need repairing!\n\r", ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_repair].beats);
    if(can_see_obj(ch, obj)) {
        act("You careful try to repair $p.", ch, obj, NULL, TO_CHAR);
        act("$n carefully begins to repair $p.", ch, obj, NULL, TO_ROOM);
        ch->pcdata->repair = 5;
        if(chance > ch->pcdata->learned[gsn_repair]) {
            send_to_char("You failed.\n\r", ch);
            return;
        }
        obj->condition += (ch->pcdata->learned[gsn_repair] - chance + 1);
        if(chance <= 5 && ch->pcdata->learned[gsn_repair] >= 100) {
            obj->condition = 100;
            act("You have fully repaired $p.", ch, obj, NULL, TO_CHAR);
            act("$n fully repairs $p.", ch, obj, NULL, TO_ROOM);
            if(obj->resistance > 15) {
                obj->resistance = 15;
                act("You have improved the design of $p, making it stronger.", ch, obj, NULL, TO_CHAR);
                act("$n has improved the design of $p, making it stronger.", ch, obj, NULL, TO_ROOM);
            }
        } else if(obj->condition >= 100) {
            obj->condition = 100;
            act("You have fully repaired $p.", ch, obj, NULL, TO_CHAR);
            act("$n fully repairs $p.", ch, obj, NULL, TO_ROOM);
        } else {
            act("You partially repair $p.", ch, obj, NULL, TO_CHAR);
            act("$n partially repairs $p.", ch, obj, NULL, TO_ROOM);
        }
    } else {
        send_to_char("You are unable to repair it.\n\r", ch);
    }
    return;
}

void death_message(CHAR_DATA *ch, CHAR_DATA *victim) {
    char buf [MAX_STRING_LENGTH];
    char *chname;
    char *chsex;
    if(ch == NULL || victim == NULL || victim->form < 1) {
        return;
    }
    chname = !IS_NPC(ch) ? ch->name : ch->short_descr;
    chsex = ch->sex == SEX_FEMALE ? "her" : "his";
    switch(victim->form) {
    default:
        sprintf(buf, "%s has just died!", chname);
        break;
    case 1:
        sprintf(buf, "%s has just been dissolved by %s's acid blood!", victim->name, chname);
        break;
    case 2:
        sprintf(buf, "%s just committed suicide by tearing %s own heart out!", chname, chsex);
        break;
    case 3:
        sprintf(buf, "%s has just sucked out all of %s's blood!", chname, victim->name);
        break;
    case 4:
        sprintf(buf, "%s has just boiled all of %s's blood!", chname, victim->name);
        break;
    case 5:
        sprintf(buf, "%s has just turned %s's blood into water!", chname, victim->name);
        break;
    case 6:
        sprintf(buf, "%s has just evaporated all of %s's blood!", chname, victim->name);
        break;
    case 7:
        sprintf(buf, "%s has just smashed %s to death!", chname, victim->name);
        break;
    case 8:
        sprintf(buf, "%s has just blown apart %s's mind!", chname, victim->name);
        break;
    case 9:
        sprintf(buf, "%s has just torn out %s's soul!", chname, victim->name);
        break;
    case 10:
        sprintf(buf, "%s has just sucked out %s's life essence!", chname, victim->name);
        break;
    case 11:
        sprintf(buf, "%s has just blown apart %s's eardrums!", chname, victim->name);
        break;
    case 12:
        sprintf(buf, "%s's entrails just poured from %s torn open stomach!", chname, chsex);
        break;
    case 13:
        sprintf(buf, "%s just died from %s wounds!", chname, chsex);
        break;
    case 14:
        sprintf(buf, "%s just burned to death!", chname);
        break;
    case 16:
        sprintf(buf, "%s just died a long, slow, painful death!", chname);
        break;
    case 17:
        sprintf(buf, "%s has just torn apart %s's body!", chname, victim->name);
        break;
    case 18:
        sprintf(buf, "%s has slain %s in cold blood!", chname, victim->name);
        break;
    case 19:
        sprintf(buf, "%s has just shot down %s!", chname, victim->name);
        break;
    case 20:
        sprintf(buf, "%s has struck %s a mortal wound!", chname, victim->name);
        break;
    case 21:
        sprintf(buf, "%s has torn off %s's face!", chname, victim->name);
        break;
    case 22:
        sprintf(buf, "%s has ripped out %s's windpipe!", chname, victim->name);
        break;
    case 23:
        sprintf(buf, "%s has ripped out %s's entrails!", chname, victim->name);
        break;
    case 24:
        sprintf(buf, "%s has broken %s's back!", chname, victim->name);
        break;
    case 25:
        sprintf(buf, "%s has broken %s's neck!", chname, victim->name);
        break;
    case 26:
        sprintf(buf, "%s has disembowelled %s!", chname, victim->name);
        break;
    case 27:
        sprintf(buf, "%s has thrust %s weapon through %s's head!", chname, chsex, victim->name);
        break;
    case 28:
        sprintf(buf, "%s has sliced open %s's carotid artery!", chname, victim->name);
        break;
    case 29:
        sprintf(buf, "%s has sliced open %s's throat!", chname, victim->name);
        break;
    case 30:
        sprintf(buf, "%s has split open %s's head!", chname, victim->name);
        break;
    case 31:
        sprintf(buf, "%s has thrust %s weapon through %s's chest!", chname, chsex, victim->name);
        break;
    case 32:
        sprintf(buf, "%s has swung %s weapon between %s's legs!", chname, chsex, victim->name);
        break;
    case 33:
        sprintf(buf, "%s has sliced off %s's arm!", chname, victim->name);
        break;
    case 34:
        sprintf(buf, "%s has sliced off %s's leg!", chname, victim->name);
        break;
    case 35:
        sprintf(buf, "%s has thrust %s weapon through %s's mouth!", chname, chsex, victim->name);
        break;
    case 36:
        sprintf(buf, "%s has thrust %s weapon through %s's eye!", chname, chsex, victim->name);
        break;
    case 37:
        sprintf(buf, "%s has smashed in %s's chest!", chname, victim->name);
        break;
    case 38:
        sprintf(buf, "%s has shattered %s's spine!", chname, victim->name);
        break;
    case 39:
        sprintf(buf, "%s has torn out %s's throat with %s teeth!", chname, victim->name, chsex);
        break;
    case 40:
        sprintf(buf, "%s has torn out %s's throat with %s claws!", chname, victim->name, chsex);
        break;
    case 41:
        sprintf(buf, "%s has clawed out %s's eyeball!", chname, victim->name);
        break;
    case 42:
        sprintf(buf, "%s has torn open %s's chest with %s claws!", chname, victim->name, chsex);
        break;
    case 43:
        sprintf(buf, "%s has sucked out %s's brains!", chname, victim->name);
        break;
    }
    buf[0] = UPPER(buf[0]);
    do_fatality(ch, buf);
    if(IS_EXTRA(victim, EXTRA_PREGNANT)) {
        sprintf(buf, "%s's unborn child has been slain!", victim->name);
        buf[0] = UPPER(buf[0]);
        do_fatality(ch, buf);
    }
    return;
}

void demon_gain(CHAR_DATA *ch, CHAR_DATA *victim) {
    int vnum;
    if(IS_NPC(victim) && !IS_NPC(ch)) {
        if(IS_DEMON(ch)) {
            if((vnum = victim->pIndexData->vnum) > 29000) {
                switch(vnum) {
                default:
                    ch->pcdata->power[POWER_CURRENT] += victim->level;
                    ch->pcdata->power[POWER_TOTAL] += victim->level;
                    break;
                /* List of mob vnums which don't give dp's */
                case 29600:
                case 30000:
                case 30001:
                case 30006:
                case 30007:
                case 30008:
                case 30009:
                case 30010:
                case 30013:
                case 30014:
                case 30015:
                case 30016:
                    break;
                }
            } else if(IS_NPC(victim) && !IS_SET(victim->act, ACT_NOEXP)) {
                ch->pcdata->power[POWER_CURRENT] += victim->level;
                ch->pcdata->power[POWER_TOTAL] += victim->level;
            }
        }
        if(ch->level == 1 && ch->mkill >= 4) {
            ch->level = 2;
            do_save(ch, "");
        }
    }
    return;
}
