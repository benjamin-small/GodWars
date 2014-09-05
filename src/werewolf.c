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

/* Local functions */

char *	gift_breed_name		args( ( int gift_no ) );
char *	gift_auspice_name	args( ( int gift_no ) );
char *	gift_tribe_name		args( ( int gift_no ) );


void do_persuasion( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_breed(ch,BREED_HOMID) < 1)
    {
	send_to_char("You require the level 1 Homid power to become persuasive.\n\r",ch);
	return;
    }

    if (IS_EXTRA(ch,EXTRA_AWE) )
    {
	send_to_char("You are no longer so persuasive.\n\r",ch);
	act("$n no longer appears so persuasive.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->extra, EXTRA_AWE);
	return;
    }
    send_to_char("Your are now persuasive.\n\r",ch);
    act("$n suddenly appears very persuasive.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->extra, EXTRA_AWE);
    return;
}

void do_staredown( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_breed(ch,BREED_HOMID) < 2)
    {
	send_to_char("You require the level 2 Homid power to staredown someone.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_icy(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Staredown on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You're pretty scared of yourself!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_FEAR) )
    {
	send_to_char( "They are already pretty scared.\n\r", ch );
	return;
    }

    act("You lock eyes with $N.",ch,NULL,victim,TO_CHAR);
    act("$n locks eyes with $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n locks eyes with you.",ch,NULL,victim,TO_VICT);

    if ( IS_MORE(victim, MORE_COURAGE) )
    {
	send_to_char( "They are far too brave to be scared so easily.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Staredown on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0)
    {
	send_to_char( "Your Staredown has no effect on them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Staredown on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && IS_WILLPOWER(victim, RES_PRESENCE) && 
	victim->pcdata->willpower[0] >= 10)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_PRESENCE] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_PRESENCE);
	send_to_char( "Your Staredown has no effect on them.\n\r", ch );
	send_to_char("You burn some willpower to resist Staredown.\n\r",victim);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Staredown on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "fear" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = get_breed(ch,BREED_HOMID) * 10;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Staredown on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_witherlimb( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    int       critical = number_range(1,4);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_breed(ch,BREED_METIS) < 4)
    {
	send_to_char("You require the level 4 Metis power to Wither Limbs.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_icy(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Wither Limb on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "Now that would be really silly...\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    WAIT_STATE( ch, 12 );

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (number_percent() > 10 * get_breed(ch,BREED_METIS))
    {
	send_to_char("Nothing happens.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (critical == 1)
    {
	if (IS_ARM_L(victim,LOST_ARM)) 
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[2],LOST_ARM);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	send_to_char("Your left arm withers up and falls off!\n\r",victim);
	act("$n's left arm withers up and falls off!",victim,NULL,NULL,TO_ROOM);
	make_part(victim,"arm");
	if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	{
	    if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	    take_item(victim,obj);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    else if (critical == 2)
    {
	if (IS_ARM_R(victim,LOST_ARM))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[3],LOST_ARM);
	if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	send_to_char("Your right arm withers up and falls off!\n\r",victim);
	act("$n's right arm withers up and falls off!",victim,NULL,NULL,TO_ROOM);
	make_part(victim,"arm");
	if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	{
	    if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	    take_item(victim,obj);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    else if (critical == 3)
    {
	if (IS_LEG_L(victim,LOST_LEG))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[4],LOST_LEG);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	send_to_char("Your left leg withers up and falls off!\n\r",victim);
	act("$n's left leg withers up and falls off!",victim,NULL,NULL,TO_ROOM);
	make_part(victim,"leg");
	if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	{
	    if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    else if (critical == 4)
    {
	if (IS_LEG_R(victim,LOST_LEG))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[5],LOST_LEG);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	send_to_char("Your right leg withers up and falls off!\n\r",victim);
	act("$n's right leg withers up and falls off!",victim,NULL,NULL,TO_ROOM);
	make_part(victim,"leg");
	if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	{
	    if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Wither Limb on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    send_to_char("Nothing happens.\n\r",ch);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just failed to use Wither Limb on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_blur( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_auspice(ch, AUSPICE_RAGABASH) < 2)
    {
	send_to_char("You require the level 2 Ragabash power to use Blur of the Milky Eye.\n\r",ch);
	return;
    }

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	send_to_char( "You slowly fade into existance.\n\r", ch );
	act("$n slowly fades into existance.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char( "You slowly fade out of existance.\n\r", ch );
	act("$n slowly fades out of existance.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->act, PLR_WIZINVIS);
    }
    return;
}

void do_truefear( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_auspice(ch, AUSPICE_AHROUN) < 3)
    {
	send_to_char("You require the level 3 Ahroun power to use True Fear.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_icy(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use True Fear on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You're pretty scared of yourself!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_FEAR) )
    {
	send_to_char( "They are already pretty scared.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use True Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( IS_MORE(victim, MORE_COURAGE) )
    {
	send_to_char( "They are far too brave to be scared so easily.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use True Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use True Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    level = get_auspice(ch,AUSPICE_AHROUN) * 10;

    if ( saves_spell( level, victim ) && victim->position >= POS_FIGHTING )
    {
	act("$E ignores you.",ch,NULL,victim,TO_CHAR);
	act("You ignore $m.",ch,NULL,victim,TO_VICT);
	act("$N ignores $n.",ch,NULL,victim,TO_NOTVICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use True Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    send_to_char("You scream in utter terror!\n\r",victim);
    act("$n screams in utter terror!",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used True Fear on you.",ch,NULL,victim,TO_VICT);
    }

    if ( ( sn = skill_lookup( "fear" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.location  = APPLY_HITROLL;
    af.modifier  = 0 - get_disc(ch,DISC_DAIMOINON);
    af.duration  = number_range(1,2);
    af.bitvector = AFF_FEAR;
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = 0 - get_disc(ch,DISC_DAIMOINON);
    affect_to_char( victim, &af );

    WAIT_STATE( ch, 12 );
    return;
}

void do_scentbeyond( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_auspice(ch,AUSPICE_THEURGE) < 2)
    {
	send_to_char("You require the level 2 power of Theurge to use Scent from Beyond.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_SPIRITEYES) )
    {
	send_to_char("You can no longer sense things in the shadowlands.\n\r",ch);
	REMOVE_BIT(ch->vampaff, VAM_SPIRITEYES);
	return;
    }
    send_to_char("You can now sense things in the shadowlands.\n\r",ch);
    SET_BIT(ch->vampaff, VAM_SPIRITEYES);
    return;
}

void do_disquiet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_breed(ch,BREED_HOMID) < 4)
    {
	send_to_char("You require the level 4 Homid power to use Disquiet.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Disquiet on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (IS_MORE(victim, MORE_CALM))
    {
	send_to_char("You no longer feel so apathetic.\n\r",victim);
	REMOVE_BIT(victim->more, MORE_CALM);
	WAIT_STATE( ch, 12 );
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0 && ch != victim)
    {
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Disquiet on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && IS_WILLPOWER(victim, RES_PRESENCE) && 
	victim->pcdata->willpower[0] >= 10 && ch != victim)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_PRESENCE] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_PRESENCE);
	send_to_char("You burn some willpower to resist Disquiet.\n\r",victim);
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Disquiet on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Disquiet on you.",ch,NULL,victim,TO_VICT);
    }

    if (IS_MORE(victim, MORE_ANGER))
    {
	send_to_char("You no longer feel so angry.\n\r",victim);
	act("$n regains $s temper.",victim,NULL,NULL,TO_ROOM);
	REMOVE_BIT(victim->more, MORE_ANGER);
	WAIT_STATE( ch, 12 );
	return;
    }

    send_to_char("You suddenly feel very apathetic!\n\r",victim);
    act("$n yawns and looks around with a bored expression.",victim,NULL,NULL,TO_ROOM);
    SET_BIT(victim->more, MORE_CALM);
    WAIT_STATE( ch, 12 );
    return;
}

void do_aeolus( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch, TRIBE_BLACK_FURIES) < 1)
    {
	send_to_char("You require the level 1 power of the Black Furies to use the Curse of Aeolus.\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (ch->pcdata->wpoints > 0)
    {
	if (ch->pcdata->wpoints > 1)
	    sprintf(buf,"You cannot call the Curse of Aeolus for another %d seconds.\n\r",ch->pcdata->wpoints);
	else
	    sprintf(buf,"You cannot call the Curse of Aeolus for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( ch->in_room->sector_type == SECT_INSIDE || !IS_OUTSIDE(ch) )
    {
	send_to_char("You can only use this power outside.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    if (IS_SET(world_affects, WORLD_FOG))
    {
	send_to_char("You banish the fog.\n\r",ch);
	REMOVE_BIT(world_affects, WORLD_FOG);
	ch->pcdata->wpoints = 60;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and the fog disperses.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("The fog disperses.\n\r", victim);
	}
    }
    else
    {
	send_to_char("You summon a thick blanket of fog.\n\r",ch);
	SET_BIT(world_affects, WORLD_FOG);
	ch->pcdata->wpoints = 60;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and calls forth a thick blanket of fog.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("A thick blanket of fog rises from the ground.\n\r", victim);
	}
    }
    return;
}

void do_porcupine( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_breed(ch,BREED_METIS) < 5)
    {
	send_to_char("You require the level 5 Homid power to use the Gift of the Porcupine.\n\r",ch);
	return;
    }

    if (!IS_SET(ch->act, PLR_WOLFMAN))
    {
	send_to_char("This power can only be activated in Crinos form.\n\r",ch);
	return;
    }

    if (IS_MORE(ch,MORE_BRISTLES) )
    {
	send_to_char("Your long quill-like spines retract into your body.\n\r",ch);
	act("$n's long quill-like spines retract into your body.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->more, MORE_BRISTLES);
	return;
    }
    send_to_char("Long quill-like spines extend from your body.\n\r",ch);
    act("Long quill-like spines extend from $n's body.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->more, MORE_BRISTLES);
    return;
}

void do_obedience( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char action[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch_next;
    int level;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (get_tribe(ch,TRIBE_SHADOW_LORDS) < 5)
    {
	send_to_char( "You require the level 5 power of the Shadow Lords to use Obedience on people.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Make whom to do what?\n\r", ch );
	return;
    }

    strcpy(action,argument);

    if (is_in(action,"|quit*rem*drop*gift*claim*consent*propose*vampi*ma *maj*sid *side *reca*/*ha *hav*"))
    {
	send_to_char( "I think not.\n\r", ch );
	return;
    }

    level = get_tribe(ch,TRIBE_SHADOW_LORDS) * 5;

    if (!str_cmp(arg,"all"))
    {
	sprintf( buf, "I command everyone to %s", action );
	do_say(ch,buf);
	WAIT_STATE(ch,24);
	for ( victim = ch->in_room->people; victim != NULL; victim = vch_next )
	{
	    vch_next = victim->next_in_room;
	    WAIT_STATE(ch,24);
	    if ( victim == ch ) continue;
	    if ( IS_IMMORTAL(victim) ) continue;

	    if ( IS_NPC(victim) && victim->level >= level )
	    {
	    	act("$s ignores you.",victim,NULL,ch,TO_VICT);
		continue;
	    }

	    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_DOMINATE] > 0)
	    {
		act("You ignore $N.",victim,NULL,ch,TO_CHAR);
		act("$n ignores $N.",victim,NULL,ch,TO_NOTVICT);
		act("$n ignores you.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Obedience on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }
	
	    if (!IS_NPC(victim) && IS_WILLPOWER(victim, RES_DOMINATE) && 
		victim->pcdata->willpower[0] >= 20)
	    {
		victim->pcdata->willpower[0] -= 20;
		victim->pcdata->resist[WILL_DOMINATE] = 60;
		if (!IS_WILLPOWER(victim, RES_TOGGLE))
		    REMOVE_BIT(victim->pcdata->resist[0], RES_DOMINATE);
		send_to_char("You burn some willpower to resist Obedience.\n\r",victim);
		act("You ignore $N.",victim,NULL,ch,TO_CHAR);
		act("$n ignores $N.",victim,NULL,ch,TO_NOTVICT);
		act("$n ignores you.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Obedience on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }

	    act("You obey $N.",victim,NULL,ch,TO_CHAR);
	    if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
	    interpret( victim, action );
	    if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Obedience on you.",ch,NULL,victim,TO_VICT);
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Now that would be just plain stupid.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "I think not...\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
	sprintf( buf, "I command %s to %s", victim->short_descr, action );
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
	sprintf( buf, "I command %s to %s", victim->morph, action );
    else
	sprintf( buf, "I command %s to %s", victim->name, action );
    do_say(ch,buf);
    WAIT_STATE(ch,24);

    if ( IS_NPC(victim) && victim->level >= level )
    {
	act("You ignores $N.",victim,NULL,ch,TO_CHAR);
	act("$n ignores $N.",victim,NULL,ch,TO_NOTVICT);
	act("$n ignores you.",victim,NULL,ch,TO_VICT);
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_DOMINATE] > 0)
    {
	act("You ignores $N.",victim,NULL,ch,TO_CHAR);
	act("$n ignores $N.",victim,NULL,ch,TO_NOTVICT);
	act("$n ignores you.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Obedience on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && IS_WILLPOWER(victim, RES_DOMINATE) && 
	victim->pcdata->willpower[0] >= 10)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_DOMINATE] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_DOMINATE);
	send_to_char("You burn some willpower to resist Obedience.\n\r",victim);
	act("You ignores $N.",victim,NULL,ch,TO_CHAR);
	act("$n ignores $N.",victim,NULL,ch,TO_NOTVICT);
	act("$n ignores you.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Obedience on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("You obey $N.",victim,NULL,ch,TO_CHAR);
    if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
    interpret( victim, action );
    if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Obedience on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_paralyzing( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_tribe(ch,TRIBE_SHADOW_LORDS) < 4)
    {
	send_to_char( "You require the level 4 power of the Shadow Lords to use Paralyzing Stare.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "I think not...\n\r", ch );
	return;
    }

    act("You stare deeply into $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n stares deeply into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Paralyzing Stare on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("You stare back into $N's eyes.",victim,NULL,ch,TO_CHAR);
    act("$n stares back into $N's eyes.",victim,NULL,ch,TO_NOTVICT);
    act("$n stares back into your eyes.",victim,NULL,ch,TO_VICT);

    send_to_char( "You paralyze them with your stare!\n\r", ch );
    send_to_char( "You are momentarily paralyzed!\n\r", victim );

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Paralyzing stare on you.",ch,NULL,victim,TO_VICT);
    }

    WAIT_STATE( ch, 24 );
    WAIT_STATE( victim, 24 );
    return;
}

void do_icy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_tribe(ch,TRIBE_SHADOW_LORDS) < 3)
    {
	send_to_char( "You require the level 3 power of the Shadow Lords to use Icy Chill of Despair.\n\r", ch );
	return;
    }

    if (IS_ITEMAFF(ch,ITEMA_PEACE) )
    {
	send_to_char("You stop using the Icy Chill of Despair.\n\r",ch);
	act("$n no longer seems so scary.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->itemaffect, ITEMA_PEACE);
	ch->pcdata->majesty = 120;
	return;
    }
    if (ch->pcdata->majesty > 0)
    {
	if (ch->pcdata->majesty > 1)
	    sprintf(buf,"You cannot use the Icy Chill of Despair for another %d seconds.\n\r",ch->pcdata->majesty);
	else
	    sprintf(buf,"You cannot use the Icy Chill of Despair for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }
    send_to_char("Your start using the Icy Chill of Despair.\n\r",ch);
    act("$n suddenly appears really scary.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->itemaffect, ITEMA_PEACE);
    return;
}

void do_clap( CHAR_DATA *ch, char *argument )
{
    char      arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        damage;
    bool       no_dam = FALSE;

    one_argument( argument, arg );

    if (IS_NPC(ch)) no_dam = TRUE;
    else
    {
	if (!IS_WEREWOLF(ch)) no_dam = TRUE;
	else if (get_tribe(ch,TRIBE_SHADOW_LORDS) < 2) no_dam = TRUE;
    }

    {
	send_to_char( "You clap your hands together.\n\r", ch );
	act("$n claps $s hands together.",ch,NULL,NULL,TO_ROOM);
	if (no_dam) return;
	if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_icy(ch,"");
	damage = get_tribe(ch, TRIBE_SHADOW_LORDS) * number_range(5,10);
	for ( victim = ch->in_room->people; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;

	    if ( victim == ch ) continue;
	    if (IS_IMMORTAL(victim)) continue;
	    if (is_safe(ch,victim))
	    {
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just used Clap of Thunder on you.",ch,NULL,victim,TO_VICT);
		}
		return;
	    }
	    act("The room reverberates with a huge roll of thunder!",victim,NULL,NULL,TO_ROOM);
	    send_to_char("The room reverberates with a huge roll of thunder!\n\r",victim);
	    send_to_char("You clutch your head in agony as your eardrums seem to explode!\n\r",victim);
	    act("$n clutches $s head in agony!",victim,NULL,NULL,TO_ROOM);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Clap of Thunder on you.",ch,NULL,victim,TO_VICT);
	    }
	    victim->hit -= damage;
	    if ((IS_NPC(victim) && victim->hit < 1) || 
		(!IS_NPC(victim) && victim->hit < -10))
	    {
		act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
		group_gain(ch,victim);
		victim->form = 11;
		killperson(ch,victim);
	    }
	    else
	    {
		update_pos(victim);

		if (number_range(1,20) <= get_disc(ch,DISC_MELPOMINEE))
		{
		    victim->position = POS_STUNNED;
		    send_to_char("You fall to the ground, stunned!\n\r",ch);
		    act("$n falls to the ground, stunned!",victim,NULL,NULL,TO_ROOM);
		}
		else if (IS_NPC(victim)) do_kill(victim,ch->name);
	    }
	}
	WAIT_STATE( ch, 24 );
	return;
    }
    return;
}

void do_fetishdoll( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       buf [MAX_INPUT_LENGTH];
    char       arg [MAX_INPUT_LENGTH];
    char     part1 [MAX_INPUT_LENGTH];
    char     part2 [MAX_INPUT_LENGTH];
    int       worn;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch,TRIBE_UKTENA) < 5)
    {
	send_to_char( "You require the level 5 power of the Uktena to create a Fetish Doll.\n\r", ch );
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char( "Who do you wish to make a Fetish Doll of?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nobody by that name is playing.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
	{
	    send_to_char( "You are not holding any body parts.\n\r", ch );
	    return;
	}
	else worn = WEAR_HOLD;
    }
    else worn = WEAR_WIELD;

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if      (obj->value[2] == 12) sprintf(part1,"head %s",victim->name);
    else if (obj->value[2] == 13) sprintf(part1,"heart %s",victim->name);
    else if (obj->value[2] == 14) sprintf(part1,"arm %s",victim->name);
    else if (obj->value[2] == 15) sprintf(part1,"leg %s",victim->name);
    else if (obj->value[2] == 30004) sprintf(part1,"entrails %s",victim->name);
    else if (obj->value[2] == 30005) sprintf(part1,"brain %s",victim->name);
    else if (obj->value[2] == 30006) sprintf(part1,"eye eyeball %s",victim->name);
    else if (obj->value[2] == 30012) sprintf(part1,"face %s",victim->name);
    else if (obj->value[2] == 30013) sprintf(part1,"windpipe %s",victim->name);
    else if (obj->value[2] == 30014) sprintf(part1,"cracked head %s",victim->name);
    else if (obj->value[2] == 30025) sprintf(part1,"ear %s",victim->name);
    else if (obj->value[2] == 30026) sprintf(part1,"nose %s",victim->name);
    else if (obj->value[2] == 30027) sprintf(part1,"tooth %s",victim->name);
    else if (obj->value[2] == 30028) sprintf(part1,"tongue %s",victim->name);
    else if (obj->value[2] == 30029) sprintf(part1,"hand %s",victim->name);
    else if (obj->value[2] == 30030) sprintf(part1,"foot %s",victim->name);
    else if (obj->value[2] == 30031) sprintf(part1,"thumb %s",victim->name);
    else if (obj->value[2] == 30032) sprintf(part1,"index finger %s",victim->name);
    else if (obj->value[2] == 30033) sprintf(part1,"middle finger %s",victim->name);
    else if (obj->value[2] == 30034) sprintf(part1,"ring finger %s",victim->name);
    else if (obj->value[2] == 30035) sprintf(part1,"little finger %s",victim->name);
    else if (obj->value[2] == 30036) sprintf(part1,"toe %s",victim->name);
    else
    {
	sprintf(buf,"%s isn't a part of %s!\n\r",obj->name,victim->name);
	send_to_char( buf, ch );
	return;
    }

    sprintf(part2,obj->name);

    if ( str_cmp(part1,part2) )
    {
	sprintf(buf,"But you are holding %s, not %s!\n\r",obj->short_descr,victim->name);
	send_to_char( buf, ch );
	return;
    }

    act("$p vanishes from your hand in a puff of smoke.", ch, obj, NULL, TO_CHAR);
    act("$p vanishes from $n's hand in a puff of smoke.", ch, obj, NULL, TO_ROOM);
    obj_from_char(obj);
    extract_obj(obj);

    obj = create_object( get_obj_index( OBJ_VNUM_VOODOO_DOLL ), 0 );

    sprintf(buf,"%s voodoo doll",victim->name);
    free_string(obj->name);
    obj->name=str_dup(buf);

    sprintf(buf,"a voodoo doll of %s",victim->name);
    free_string(obj->short_descr);
    obj->short_descr=str_dup(buf);

    sprintf(buf,"A voodoo doll of %s lies here.",victim->name);
    free_string(obj->description);
    obj->description=str_dup(buf);

    obj_to_char(obj,ch);
    equip_char(ch,obj,worn);

    act("$p appears in your hand.", ch, obj, NULL, TO_CHAR);
    act("$p appears in $n's hand.", ch, obj, NULL, TO_ROOM);

    return;
}

void do_invisibility( CHAR_DATA *ch, char *argument )
{
    int sn;

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_tribe(ch,TRIBE_UKTENA) < 4)
    {
	send_to_char("You require the level 4 power of the Uktena to use Invisibility.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_INVISIBLE))
    {
	send_to_char("But you are already invisible!\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if ( ( sn = skill_lookup( "invis" ) ) < 0 ) return;
    (*skill_table[sn].spell_fun) ( sn, 50, ch, ch );
    WAIT_STATE( ch, 12 );
    return;
}

void do_wallgranite( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    char       arg [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    int        duration = number_range(15,30);
    int        direction, rev;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_auspice(ch,AUSPICE_PHILODOX) < 5)
    {
	send_to_char("You require the level 5 power of Philodox create a Wall of Granite.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0')
    {
	send_to_char( "Which direction do you wish to make the wall?\n\r", ch );
	return;
    }

         if (!str_cmp(arg,"north") || !str_cmp(arg,"n")) 
	{direction = DIR_NORTH; rev = DIR_SOUTH;}
    else if (!str_cmp(arg,"south") || !str_cmp(arg,"s")) 
	{direction = DIR_SOUTH; rev = DIR_NORTH;}
    else if (!str_cmp(arg,"east")  || !str_cmp(arg,"e")) 
	{direction = DIR_EAST;  rev = DIR_WEST;}
    else if (!str_cmp(arg,"west")  || !str_cmp(arg,"w")) 
	{direction = DIR_WEST;  rev = DIR_EAST;}
    else if (!str_cmp(arg,"up")    || !str_cmp(arg,"u")) 
	{direction = DIR_UP;    rev = DIR_DOWN;}
    else if (!str_cmp(arg,"down")  || !str_cmp(arg,"d")) 
	{direction = DIR_DOWN;  rev = DIR_UP;}
    else
    {
	send_to_char( "Select direction: North, South, East, West, Up or Down.\n\r", ch );
	return;
    }

    switch (direction)
    {
	default:        strcpy(arg,"north"); break;
	case DIR_NORTH: strcpy(arg,"north"); break;
	case DIR_SOUTH: strcpy(arg,"south"); break;
	case DIR_EAST:  strcpy(arg,"east");  break;
	case DIR_WEST:  strcpy(arg,"west");  break;
	case DIR_UP:    strcpy(arg,"up");    break;
	case DIR_DOWN:  strcpy(arg,"down");  break;
    }

    if ( ( pexit = ch->in_room->exit[direction] ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( ( location = ch->in_room ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( ( to_room = pexit->to_room ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_SET(pexit->exit_info, EX_WALL_WATER) || 
	IS_SET(pexit->exit_info, EX_WALL_GRANITE) || 
	IS_SET(pexit->exit_info, EX_WALL_FLAME))
    {
	send_to_char("There is already a wall blocking that exit.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->item_type = ITEM_WALL;
    obj->value[0] = direction;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("wall stone granite");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of granite");
    sprintf(buf,"A wall of solid granite blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_GRANITE);

    if (direction == DIR_UP)
	sprintf(buf,"Stone erupts from the ground and forms a wall above you.");
    else if (direction == DIR_DOWN)
	sprintf(buf,"Stone erupts from the ground and forms a wall below you.");
    else
	sprintf(buf,"Stone erupts from the ground and forms a wall to the %s.",arg);
    act( buf, ch, NULL, NULL, TO_CHAR );
    act( buf, ch, NULL, NULL, TO_ROOM );
    WAIT_STATE(ch,12);

    if (location == to_room) return;

    char_from_room(ch);
    char_to_room(ch, to_room);

    if ( ( pexit = ch->in_room->exit[rev] ) == NULL ) return;
    if (IS_SET(pexit->exit_info, EX_WALL_WATER) || 
	IS_SET(pexit->exit_info, EX_WALL_GRANITE) || 
	IS_SET(pexit->exit_info, EX_WALL_FLAME)) return;

    switch (rev)
    {
	default:        strcpy(arg,"north"); break;
	case DIR_NORTH: strcpy(arg,"north"); break;
	case DIR_SOUTH: strcpy(arg,"south"); break;
	case DIR_EAST:  strcpy(arg,"east");  break;
	case DIR_WEST:  strcpy(arg,"west");  break;
	case DIR_UP:    strcpy(arg,"up");    break;
	case DIR_DOWN:  strcpy(arg,"down");  break;
    }

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->item_type = ITEM_WALL;
    obj->value[0] = rev;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("wall stone granite");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of granite");
    sprintf(buf,"A wall of solid granite blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_GRANITE);

    if (rev == DIR_UP)
	sprintf(buf,"Stone erupts from the ground and forms a wall above you.");
    else if (rev == DIR_DOWN)
	sprintf(buf,"Stone erupts from the ground and forms a wall below you.");
    else
	sprintf(buf,"Stone erupts from the ground and forms a wall to the %s.",arg);
    act( buf, ch, NULL, NULL, TO_ROOM );

    char_from_room(ch);
    char_to_room(ch, location);

    return;
}

void do_camouflage( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_tribe(ch,TRIBE_WENDIGOS) < 1)
    {
	send_to_char("You require the level 1 power of the Wendigos to use Camouflage.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_HIDE))
    {
	send_to_char( "You are already camouflaging yourself.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (!IS_AFFECTED(ch, AFF_HIDE)) 
    {
	act("$n seems to blend into the shadows.",ch,NULL,NULL,TO_ROOM);
	send_to_char("You Camouflage yourself.\n\r",ch);	
	SET_BIT(ch->affected_by, AFF_HIDE);
    }
    WAIT_STATE( ch, 12 );
    return;
}

void do_reshape( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg1[MAX_INPUT_LENGTH];
    char      arg2[MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    char      endchar[15];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_breed(ch, BREED_HOMID) < 3)
    {
	send_to_char("You require the level 3 Homid power to use Reshape Object.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: Reshape <object> rename  <short description>\n\r", ch );
	send_to_char( "Syntax: Reshape <object> keyword <extra keyword/s>\n\r", ch );
	send_to_char( "Syntax: Reshape <object> flags   <glow/hum>\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }

    if (obj->item_type == ITEM_QUEST || obj->item_type == ITEM_AMMO ||
        obj->item_type == ITEM_EGG   || obj->item_type == ITEM_VOODOO ||
        obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE ||
        obj->item_type == ITEM_TOOL  || obj->item_type == ITEM_SYMBOL ||
	obj->item_type == ITEM_PAGE  || IS_SET(obj->quest, QUEST_ARTIFACT))
    {
	send_to_char( "You cannot quest-change that item.\n\r", ch );
	return;
    }

    if (!IS_IMMORTAL(ch) && (obj->questowner == NULL || str_cmp(ch->name,obj->questowner)))
    {
	send_to_char("You can only change an item you own.\n\r", ch);
	return;
    }

    if ( !str_cmp( arg2, "rename" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (strlen(argument) < 5)
	{
	    send_to_char("Name should be at least 5 characters long.\n\r",ch);
	    return;
	}
	strcpy(argument,capitalize(argument));
	argument[0] = LOWER(argument[0]);
	endchar[0] = (argument[strlen(argument)-1]);
	endchar[1] = '\0';
	if (!str_cmp(endchar,".")) argument[strlen(argument)-1] = '\0';
	free_string( obj->name );
	obj->name = str_dup( argument );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( argument );
	if (is_in(argument,"|pair of*"))
	    sprintf(buf, "%s are lying here.",argument);
	else
	    sprintf(buf, "%s is lying here.",argument);
	buf[0] = UPPER(buf[0]);
	free_string( obj->description );
	obj->description = str_dup( buf );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "keyword" ) || !str_cmp( arg2, "keywords" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (strlen(argument) < 3)
	{
	    send_to_char("Keywords should be at least 3 characters long.\n\r",ch);
	    return;
	}
	strcpy(argument,capitalize(argument));
	argument[0] = LOWER(argument[0]);
	endchar[0] = (argument[strlen(argument)-1]);
	endchar[1] = '\0';
	if (!str_cmp(endchar,".")) argument[strlen(argument)-1] = '\0';
	strcpy(buf,obj->name);
	if (strlen(buf) > 80)
	{
	    send_to_char("You cannot fit any more keywords on this object.\n\r",ch);
	    return;
	}
	strcat(buf," ");
	strcat(buf,argument);
	free_string( obj->name );
	obj->name = str_dup( buf );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "flags" ) )
    {
	if (!str_cmp(argument,"glow"))
	{
	    if (IS_SET(obj->extra_flags, ITEM_GLOW))
		REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
	    else
		SET_BIT(obj->extra_flags, ITEM_GLOW);
	}
	else if (!str_cmp(argument,"hum"))
	{
	    if (IS_SET(obj->extra_flags, ITEM_HUM))
		REMOVE_BIT(obj->extra_flags, ITEM_HUM);
	    else
		SET_BIT(obj->extra_flags, ITEM_HUM);
	}
	else
	{
	    send_to_char( "Syntax: Reshape <object> flags   <glow/hum>\n\r", ch );
	    return;
	}
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    send_to_char( "Syntax: Reshape <object> rename  <short description>\n\r", ch );
    send_to_char( "Syntax: Reshape <object> keyword <extra keyword/s>\n\r", ch );
    send_to_char( "Syntax: Reshape <object> flags   <glow/hum>\n\r", ch );
    return;
}

void do_teachgift( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char disc [25];
    CHAR_DATA *victim;
    int improve, sn, col, gtype;

    smash_tilde(argument);
    argument = one_argument( argument, arg1 );
    strcpy(arg2,argument);

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: teach <person> <gift>\n\r", ch );
	send_to_char( "The following list contains your gifts and the level you can teach to.\n\r", ch );
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );

	col    = 0;
	for ( sn = 0; sn <= BREED_MAX; sn++ )
	{
	    if ( ch->pcdata->breed[sn] == 0 ) continue;
	    switch (sn)
	    {
		default:          strcpy(disc,"None");  break;
		case BREED_HOMID: strcpy(disc,"Homid"); break;
		case BREED_LUPUS: strcpy(disc,"Lupus"); break;
		case BREED_METIS: strcpy(disc,"Metis"); break;
	    }
	}
	for ( sn = 0; sn <= AUSPICE_MAX; sn++ )
	{
	    if ( ch->pcdata->auspice[sn] == 0 ) continue;
	    switch (sn)
	    {
		default:               strcpy(disc,"None"); break;
		case AUSPICE_RAGABASH: strcpy(disc,"Ragabash"); break;
		case AUSPICE_THEURGE:  strcpy(disc,"Theurge"); break;
		case AUSPICE_PHILODOX: strcpy(disc,"Philodox"); break;
		case AUSPICE_GALLIARD: strcpy(disc,"Galliard"); break;
		case AUSPICE_AHROUN:   strcpy(disc,"Ahroun"); break;
	    }
	}
	for ( sn = 0; sn <= TRIBE_MAX; sn++ )
	{
	    if ( ch->pcdata->tribes[sn] == 0 ) continue;
	    switch (sn)
	    {
		default:                     strcpy(disc,"None"); break;
		case TRIBE_BLACK_FURIES:     strcpy(disc,"Black Furies"); break;
		case TRIBE_BONE_GNAWERS:     strcpy(disc,"Bone Gnawers"); break;
		case TRIBE_CHILDREN_OF_GAIA: strcpy(disc,"Children of Gaia"); break;
		case TRIBE_FIANNA:           strcpy(disc,"Fianna"); break;
		case TRIBE_GET_OF_FENRIS:    strcpy(disc,"Get of Fenris"); break;
		case TRIBE_GLASS_WALKERS:    strcpy(disc,"Glass Walkers"); break;
		case TRIBE_RED_TALONS:       strcpy(disc,"Red Talons"); break;
		case TRIBE_SHADOW_LORDS:     strcpy(disc,"Shadow Lords"); break;
		case TRIBE_SILENT_STRIDERS:  strcpy(disc,"Silent Striders"); break;
		case TRIBE_SILVER_FANGS:     strcpy(disc,"Silver Fangs"); break;
		case TRIBE_STARGAZERS:       strcpy(disc,"Stargazers"); break;
		case TRIBE_UKTENA:           strcpy(disc,"Uktena"); break;
		case TRIBE_WENDIGOS:         strcpy(disc,"Wendigos"); break;
	    }
	}
	if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot teach yourself!\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (!str_cmp(arg2,"homid")) 
	{improve = BREED_HOMID;gtype = 0;}
    else if (!str_cmp(arg2,"lupus")) 
	{improve = BREED_LUPUS;gtype = 0;}
    else if (!str_cmp(arg2,"metis")) 
	{improve = BREED_METIS;gtype = 0;}
    else if (!str_cmp(arg2,"ragabash")) 
	{improve = AUSPICE_RAGABASH;gtype = 1;}
    else if (!str_cmp(arg2,"theurge")) 
	{improve = AUSPICE_THEURGE;gtype = 1;}
    else if (!str_cmp(arg2,"philodox")) 
	{improve = AUSPICE_PHILODOX;gtype = 1;}
    else if (!str_cmp(arg2,"galliard")) 
	{improve = AUSPICE_GALLIARD;gtype = 1;}
    else if (!str_cmp(arg2,"ahroun")) 
	{improve = AUSPICE_AHROUN;gtype = 1;}
    else if (!str_cmp(arg2,"black furies")) 
	{improve = TRIBE_BLACK_FURIES;gtype = 2;}
    else if (!str_cmp(arg2,"bone gnawers")) 
	{improve = TRIBE_BONE_GNAWERS;gtype = 2;}
    else if (!str_cmp(arg2,"children of gaia")) 
	{improve = TRIBE_CHILDREN_OF_GAIA;gtype = 2;}
    else if (!str_cmp(arg2,"fianna")) 
	{improve = TRIBE_FIANNA;gtype = 2;}
    else if (!str_cmp(arg2,"get of fenris")) 
	{improve = TRIBE_GET_OF_FENRIS;gtype = 2;}
    else if (!str_cmp(arg2,"get of fenris")) 
	{improve = TRIBE_GET_OF_FENRIS;gtype = 2;}
    else if (!str_cmp(arg2,"glass walkers")) 
	{improve = TRIBE_GLASS_WALKERS;gtype = 2;}
    else if (!str_cmp(arg2,"red talons")) 
	{improve = TRIBE_RED_TALONS;gtype = 2;}
    else if (!str_cmp(arg2,"shadow lords")) 
	{improve = TRIBE_SHADOW_LORDS;gtype = 2;}
    else if (!str_cmp(arg2,"silent striders")) 
	{improve = TRIBE_SILENT_STRIDERS;gtype = 2;}
    else if (!str_cmp(arg2,"silver fangs")) 
	{improve = TRIBE_SILVER_FANGS;gtype = 2;}
    else if (!str_cmp(arg2,"stargazers")) 
	{improve = TRIBE_STARGAZERS;gtype = 2;}
    else if (!str_cmp(arg2,"uktena")) 
	{improve = TRIBE_UKTENA;gtype = 2;}
    else if (!str_cmp(arg2,"wendigos")) 
	{improve = TRIBE_WENDIGOS;gtype = 2;}
    else
    {
	send_to_char("You know of no such gift.\n\r",ch);
	return;
    }

    switch (gtype)
    {
	default:
	    if (ch->pcdata->breed[improve] == 0)
	    {
		send_to_char("You know of no such breed power.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->breed[improve] < 3)
	    {
		send_to_char("You require level 3 in a gift before you can teach it.\n\r",ch);
		return;
	    }
	    break;
	case 1:
	    if (ch->pcdata->auspice[improve] == 0)
	    {
		send_to_char("You know of no such auspice power.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->auspice[improve] < 3)
	    {
		send_to_char("You require level 3 in a gift before you can teach it.\n\r",ch);
		return;
	    }
	    break;
	case 2:
	    if (ch->pcdata->tribes[improve] == 0)
	    {
		send_to_char("You know of no such tribe power.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->tribes[improve] < 3)
	    {
		send_to_char("You require level 3 in a gift before you can teach it.\n\r",ch);
		return;
	    }
	    break;
    }

    if (!IS_WEREWOLF(victim))
    {
	send_to_char("They are unable to learn gifts.\n\r",ch);
	return;
    }

    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to learn any gifts.\n\r",ch);
	return;
    }

    switch (gtype)
    {
	default:
	    if (victim->pcdata->breed[improve] != 0)
	    {
		send_to_char("They already know that gift.\n\r",ch);
		return;
	    }
	    victim->pcdata->breed[improve] = -1;
	    break;
	case 1:
	    if (victim->pcdata->auspice[improve] != 0)
	    {
		send_to_char("They already know that gift.\n\r",ch);
		return;
	    }
	    victim->pcdata->auspice[improve] = -1;
	    break;
	case 2:
	    if (victim->pcdata->tribes[improve] != 0)
	    {
		send_to_char("They already know that gift.\n\r",ch);
		return;
	    }
	    victim->pcdata->tribes[improve] = -1;
	    break;
    }

    sprintf(buf,"You teach $N how to use the %s gift.",capitalize(arg2));
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"$n teaches you how to use the %s gift.",capitalize(arg2));
    act(buf,ch,NULL,victim,TO_VICT);
    return;
}

void do_halosun( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch,TRIBE_CHILDREN_OF_GAIA) < 5)
    {
	send_to_char("You require the level 5 Children of Gaia power to use the Halo of the Sun.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch,ITEMA_SUNSHIELD) )
    {
	send_to_char("Your Halo of the Sun gradually fades away.\n\r",ch);
	act("$n's Halo of the Sun gradually fades away.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->itemaffect, ITEMA_SUNSHIELD);
	return;
    }
    send_to_char("You are surrounded by a halo of sunlight!\n\r",ch);
    act("$n is surrounded by a halo of sunlight!", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->itemaffect, ITEMA_SUNSHIELD);
    return;
}

void do_mother( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_auspice(ch,AUSPICE_THEURGE) < 1)
    {
	send_to_char("You require the level 1 Theurge power to use Mother's Touch.\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0)
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your healing powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your healing powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to heal with Mother's Touch?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_EVIL(victim))
    {
	send_to_char( "They are too evil to benefit from your healing powers.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You cannot use this power upon yourself.\n\r", ch );
	return;
    }

    if (victim->loc_hp[6] > 0)
    {
	int sn = skill_lookup( "clot" );
	act( "You lay your hands upon $N.", ch, NULL, victim, TO_CHAR );
	act( "$n's lays $s hands upon $N.", ch, NULL, victim, TO_NOTVICT );
	act( "$n's lays $s hands upon you.", ch, NULL, victim, TO_VICT );
	ch->pcdata->obeah = 10;
	(*skill_table[sn].spell_fun) (sn,ch->level,ch,victim);
    }
    else
    {
	if ((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
	    victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5]) != 0)
	{
	    act( "You lay your hands upon $N.", ch, NULL, victim, TO_CHAR );
	    act( "$n's lays $s hands upon $N.", ch, NULL, victim, TO_NOTVICT );
	    act( "$n's lays $s hands upon you.", ch, NULL, victim, TO_VICT );
	    ch->pcdata->obeah = 20;
	    reg_mend(victim);
	}
	else if (victim->hit < victim->max_hit)
	{
	    victim->hit += get_auspice(ch, AUSPICE_THEURGE) * 1000;
	    victim->agg -= get_auspice(ch, AUSPICE_THEURGE) * 2;
	    if (victim->agg < 0) victim->agg = 0;
	    act( "You lay your hands upon $N.", ch, NULL, victim, TO_CHAR );
	    act( "$n's lays $s hands upon $N.", ch, NULL, victim, TO_NOTVICT );
	    act( "$n's lays $s hands upon you.", ch, NULL, victim, TO_VICT );
	    ch->pcdata->obeah = 30;
	    update_pos(victim);
	    if (victim->hit >= victim->max_hit)
	    {
		victim->hit = victim->max_hit;
		send_to_char("You have been completely healed!\n\r",victim);
		act("$n's wounds have been completely healed!",victim,NULL,NULL,TO_ROOM);
	    }
	    else
	    {
		send_to_char("Your wounds begin to heal!\n\r",victim);
		act("$n's wounds begin to heal!",victim,NULL,NULL,TO_ROOM);
	    }
	}
	else
	{
	    send_to_char("They don't require any healing.\n\r",ch);
	    return;
	}
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Mother's Touch on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_bodywrack( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       damage;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_tribe(ch,TRIBE_BLACK_FURIES) < 5)
    {
	send_to_char("You require the level 5 Black Furies power to use Body Wrack.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Body Wrack on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Body Wrack on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    damage = number_range(500,1000);

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You scream in agony as your body seems to be torn apart!\n\r",victim);
    act("$n doubles over and screams in agony!",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Body Wrack on you.",ch,NULL,victim,TO_VICT);
    }

    victim->hit -= damage;
    if ((IS_NPC(victim) && victim->hit < 1) || 
	(!IS_NPC(victim) && victim->hit < -10))
    {
	act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	group_gain(ch,victim);
	victim->form = 17;
	killperson(ch,victim);
	WAIT_STATE( ch, 12 );
	return;
    }
    update_pos(victim);
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void do_wwpowers( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    bool any_powers = FALSE;
    bool any_commands = FALSE;

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    for ( sn = 0; sn <= BREED_MAX; sn++ )
    {
	if ( get_breed(ch,sn) > 0 ) 
	    any_powers = TRUE;
    }
    if (!any_commands) for ( sn = 0; sn <= AUSPICE_MAX; sn++ )
    {
	if ( get_auspice(ch,sn) > 0 ) 
	    any_powers = TRUE;
    }
    if (!any_commands) for ( sn = 0; sn <= TRIBE_MAX; sn++ )
    {
	if ( get_tribe(ch,sn) > 0 ) 
	    any_powers = TRUE;
    }
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char("                               -= Gift Commands =-\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    if (get_breed(ch, BREED_HOMID) > 0)
    {
	strcpy(buf,"Homid: Persuasion");
	if (get_breed(ch, BREED_HOMID) > 1) strcat(buf," Staredown");
	if (get_breed(ch, BREED_HOMID) > 2) strcat(buf," Reshape");
	if (get_breed(ch, BREED_HOMID) > 3) strcat(buf," Disquiet");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_breed(ch, BREED_LUPUS) > 0)
    {
	strcpy(buf,"Lupus: Heighten");
	if (get_breed(ch, BREED_LUPUS) > 1) strcat(buf," Beckon");
	if (get_breed(ch, BREED_LUPUS) > 2) strcat(buf," Master");
	if (get_breed(ch, BREED_LUPUS) > 3) strcat(buf," Masssummons");
	if (get_breed(ch, BREED_LUPUS) > 4) strcat(buf," Fleshbond");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_breed(ch, BREED_METIS) > 0)
    {
	strcpy(buf,"Metis: Nightsight");
	if (get_breed(ch, BREED_METIS) > 1) strcat(buf," Burrow");
	if (get_breed(ch, BREED_METIS) > 3) strcat(buf," Witherlimb");
	if (get_breed(ch, BREED_METIS) > 4) strcat(buf," Porcupine");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_auspice(ch, AUSPICE_RAGABASH) > 0)
    {
	strcpy(buf,"Ragabash:");
	if (get_auspice(ch, AUSPICE_RAGABASH) > 1) strcat(buf," Vanish");
	if (get_auspice(ch, AUSPICE_RAGABASH) > 3) strcat(buf," Tail");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_auspice(ch, AUSPICE_THEURGE) > 0)
    {
	strcpy(buf,"Theurge: Mother");
	if (get_auspice(ch, AUSPICE_THEURGE) > 1) strcat(buf," Scent");
	if (get_auspice(ch, AUSPICE_THEURGE) > 3) strcat(buf," Guardian");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_auspice(ch, AUSPICE_PHILODOX) > 0)
    {
	strcpy(buf,"Philodox:");
	if (get_auspice(ch, AUSPICE_PHILODOX) > 2) strcat(buf," Scry");
	if (get_auspice(ch, AUSPICE_PHILODOX) > 4) strcat(buf," Wallgranite");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_auspice(ch, AUSPICE_GALLIARD) > 0)
    {
	strcpy(buf,"Galliard: Wyld");
	if (get_auspice(ch, AUSPICE_GALLIARD) > 2) strcat(buf," Beastwithin");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_auspice(ch, AUSPICE_AHROUN) > 0)
    {
	strcpy(buf,"Ahroun:");
	if (get_auspice(ch, AUSPICE_AHROUN) > 2) strcat(buf," Truefear");
	if (get_auspice(ch, AUSPICE_AHROUN) > 4) strcat(buf," Hand");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_BLACK_FURIES) > 0)
    {
	strcpy(buf,"Black Furies: Aeolus");
	if (get_auspice(ch, TRIBE_BLACK_FURIES) > 4) strcat(buf," Bodywrack");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_BONE_GNAWERS) > 0)
    {
	strcpy(buf,"Bone Gnawers:");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) > 0)
    {
	strcpy(buf,"Children of Gaia: Calm");
	if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) > 3) strcat(buf," Serenity");
	if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) > 4) strcat(buf," Halo");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_FIANNA) > 0)
    {
	strcpy(buf,"Fianna:");
	if (get_tribe(ch, TRIBE_FIANNA) > 2) strcat(buf," Illusionaryform");
	if (get_tribe(ch, TRIBE_FIANNA) > 3) strcat(buf," Cloneobject");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_GET_OF_FENRIS) > 0)
    {
	strcpy(buf,"Get of Fenris:");
	if (get_tribe(ch, TRIBE_GET_OF_FENRIS) > 2) strcat(buf," Scream");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_GLASS_WALKERS) > 0)
    {
	strcpy(buf,"Glass Walkers:");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_RED_TALONS) > 0)
    {
	strcpy(buf,"Red Talons:");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_SHADOW_LORDS) > 0)
    {
	strcpy(buf,"Shadow Lords:");
	if (get_tribe(ch, TRIBE_SHADOW_LORDS) > 1) strcat(buf," Clap");
	if (get_tribe(ch, TRIBE_SHADOW_LORDS) > 2) strcat(buf," Icy");
	if (get_tribe(ch, TRIBE_SHADOW_LORDS) > 3) strcat(buf," Paralyze");
	if (get_tribe(ch, TRIBE_SHADOW_LORDS) > 4) strcat(buf," Obedience");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 0)
    {
	strcpy(buf,"Silent Striders:");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_SILVER_FANGS) > 0)
    {
	strcpy(buf,"Silver Fangs:");
	if (get_tribe(ch, TRIBE_SILVER_FANGS) > 3) strcat(buf," Mindblock");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_STARGAZERS) > 0)
    {
	strcpy(buf,"Stargazers:");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_UKTENA) > 0)
    {
	strcpy(buf,"Uktena: Shroud");
	if (get_tribe(ch, TRIBE_UKTENA) > 3) strcat(buf," Invisibility");
	if (get_tribe(ch, TRIBE_UKTENA) > 4) strcat(buf," Fetishdoll");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_tribe(ch, TRIBE_WENDIGOS) > 0)
    {
	strcpy(buf,"Wendigos: Camouflage");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (!any_powers) send_to_char("You have no gifts.\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    return;
}

void do_burrow( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_breed(ch,BREED_METIS) < 2)
    {
	send_to_char("You require the level 2 Metis power to Burrow into the ground.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_EARTHMELDED) )
    {
	REMOVE_BIT(ch->vampaff, VAM_EARTHMELDED);
	send_to_char("You burrow up out of the ground.\n\r",ch);
	act("$n burrows up out of the ground.", ch, NULL, NULL, TO_ROOM);
	return;
    }
    send_to_char("You burrow down into the ground.\n\r",ch);
    act("$n burrows down into the ground.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_EARTHMELDED);
    REMOVE_BIT(ch->affected_by, AFF_FLAMING);
    REMOVE_BIT(ch->more, MORE_FLAMES);
    return;
}

void do_wyld( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *vch;
    CHAR_DATA *victim;
    char buf [MAX_STRING_LENGTH];
    
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) && ch->in_room == NULL)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_auspice(ch,AUSPICE_GALLIARD) < 1)
    {
	send_to_char("You require the level 1 Galliard power to use the Call of the Wyld.\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL && ch->hit > 0 )
    {
	send_to_char("You can only call for assistance in times of great need.\n\r",ch);
	return;
    }

    if (ch->pcdata->reina > 0)
    {
	send_to_char("Your voice has not yet recovered.\n\r",ch);
	return;
    }

    ch->pcdata->reina = 5;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (vch = d->character) == NULL ) continue;
	if ( vch->in_room == NULL) continue;
	if ( IS_NPC(vch) ) continue;
	if ( ch->beast >= 40 && vch->beast < 40 ) continue;
	if ( ch->beast < 40 && vch->beast >= 40 ) continue;
	if ( vch == ch )
	    act("You call for assistance!", ch, NULL, argument, TO_CHAR);
	if (ch != vch && (IS_WEREWOLF(vch) || IS_IMMORTAL(vch)))
	{
	    if (victim != NULL)
	    {
		sprintf(buf,"CALL OF THE WYLD:[$n is under attack by %s at %s]", 
		    IS_NPC(victim) ? victim->short_descr : victim->name,
                    ch->in_room->name);
		act(buf,ch,NULL,vch, TO_VICT);
	    }
	    else if (ch->hit < 1)
	    {
		sprintf(buf,"CALL OF THE WYLD:[$n is badly wounded at %s]", 
                    ch->in_room->name);
		act(buf,ch,NULL,vch, TO_VICT);
	    }
	    else
	    {
		sprintf(buf,"CALL OF THE WYLD:[$n needs assistance at %s]", 
                    ch->in_room->name);
		act(buf,ch,NULL,vch, TO_VICT);
	    }
	}
    }
    return;
}

void do_dazzle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) && ch->in_room == NULL)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch,TRIBE_CHILDREN_OF_GAIA) < 3)
    {
	send_to_char("You require the level 3 Children of Gaia power to use Dazzle.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to Dazzle?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (victim->position != POS_FIGHTING && victim->hit < victim->max_hit)
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dazzle on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_IMMUNE(victim,IMM_SHIELDED) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dazzle on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dazzle on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You cover your eyes as you are dazzled!\n\r",victim);
    act("$n covers $s eyes, looking dazzled!",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Dazzle on you.",ch,NULL,victim,TO_VICT);
    }

    stop_fighting(victim,TRUE);
    update_pos(victim);
    if (victim->position > POS_STUNNED)
	victim->position = POS_STUNNED;
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 24 );
    return;
}

void do_tail( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    int stance;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_auspice(ch, AUSPICE_RAGABASH) < 4)
    {
	send_to_char("You require the level 4 Ragabash power to use Tail of the Monkey.\n\r",ch);
	return;
    }

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use Tail of the Monkey on?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Tail of the Monkey on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    WAIT_STATE( ch, 4 );

    dam = number_range(10,20);

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) && get_disc(ch,DISC_POTENCE) > 0)
	dam += dam * get_disc(ch,DISC_POTENCE) * 0.5;
    else if ( !IS_NPC(ch) && (IS_SET(ch->act, PLR_CHAMPION) ||
	IS_SET(ch->act, PLR_DEMON)) && 
	IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT) )
	dam *= 1.5;

    dam = dam + (dam * ((ch->wpn[0]) * 0.01));

/*
    if ( !IS_NPC(ch) )
*/
    {
	stance = ch->stance[0];
	if ( IS_STANCE(ch, STANCE_NORMAL) ) dam *= 1.25;
	else dam = dambonus(ch,victim,dam,stance);
    }

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, gsn_tail );
    return;
}

void do_rain( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) < 2)
    {
	send_to_char("You require the level 2 power of the Children of Gaia to call upon Gaia's Tears.\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (ch->pcdata->wpoints > 0)
    {
	if (ch->pcdata->wpoints > 1)
	    sprintf(buf,"You cannot call your weather controlling powers for another %d seconds.\n\r",ch->pcdata->wpoints);
	else
	    sprintf(buf,"You cannot call your weather controlling powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( ch->in_room->sector_type == SECT_INSIDE || !IS_OUTSIDE(ch) )
    {
	send_to_char("You can only control the weather outside.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    if (IS_SET(world_affects, WORLD_RAIN))
    {
	send_to_char("You banish the rain.\n\r",ch);
	REMOVE_BIT(world_affects, WORLD_RAIN);
	weather_info.sky = SKY_CLOUDY;
	ch->pcdata->wpoints = 60;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and the rain stops.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("It stops raining.\n\r", victim);
	}
    }
    else
    {
	send_to_char("You summon a heavy shower of rain.\n\r",ch);
	SET_BIT(world_affects, WORLD_RAIN);
	ch->pcdata->wpoints = 60;
	weather_info.sky = SKY_RAINING;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and calls forth a heavy shower of rain.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("It starts to rain heavily.\n\r", victim);
	}
    }
    return;
}

void do_eclipse( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) < 4)
    {
	send_to_char("You require the level 4 power of the Children of Gaia to call an Eclipse\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (ch->pcdata->wpoints > 0)
    {
	if (ch->pcdata->wpoints > 1)
	    sprintf(buf,"You cannot call your weather controlling powers for another %d seconds.\n\r",ch->pcdata->wpoints);
	else
	    sprintf(buf,"You cannot call your weather controlling powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( ch->in_room->sector_type == SECT_INSIDE || !IS_OUTSIDE(ch) )
    {
	send_to_char("You can only control the weather outside.\n\r",ch);
	return;
    }

    if (weather_info.sunlight != SUN_LIGHT)
    {
	send_to_char("You can only create an eclipse when the sun is high in the sky.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    if (IS_SET(world_affects, WORLD_ECLIPSE))
    {
	send_to_char("You end the eclipse.\n\r",ch);
	REMOVE_BIT(world_affects, WORLD_ECLIPSE);
	ch->pcdata->wpoints = 120;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and the sun emerges from behind the moon.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("The sun emerges from behind the moon.\n\r", victim);
	}
    }
    else
    {
	send_to_char("You create an eclipse.\n\r",ch);
	SET_BIT(world_affects, WORLD_ECLIPSE);
	ch->pcdata->wpoints = 120;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) ) continue;
	    if ( victim->in_room == NULL ) continue;
	    if ( ch == victim ) continue;
	    if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	    if ( ch->in_room == victim->in_room )
		act("$n holds up his arms and calls forth an eclipse of the sun.", ch, NULL, victim, TO_VICT);
	    else
		send_to_char("The sun is eclipsed by the moon.\n\r", victim);
	}
    }
    return;
}

void do_lightning( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    int dam, hp, sn;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) < 3)
    {
	send_to_char("You require the level 3 power of the Children of Gaia to call the Lightning\n\r",ch);
	return;
    }

    if (ch->pcdata->wpoints > 0)
    {
	if (ch->pcdata->wpoints > 1)
	    sprintf(buf,"You cannot call your weather controlling powers for another %d seconds.\n\r",ch->pcdata->wpoints);
	else
	    sprintf(buf,"You cannot call your weather controlling powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( ch->in_room->sector_type == SECT_INSIDE || !IS_OUTSIDE(ch) )
    {
	send_to_char("You can only control the weather outside.\n\r",ch);
	return;
    }

    if (weather_info.sky != SKY_LIGHTNING && 
	weather_info.sky != SKY_RAINING &&
	!IS_SET(world_affects, WORLD_RAIN) )
    {
	send_to_char("The weather is not bad enough to call for a lightning strike.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who do you wish to strike with lightning?\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }

    if ( ch == victim )
    {
	send_to_char( "Now that would be just plain stupid.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "lightning breath" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    ch->pcdata->wpoints = 60;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (victim = d->character) == NULL ) continue;
	if ( IS_NPC(victim) ) continue;
	if ( victim->in_room == NULL ) continue;
	if ( victim->in_room->sector_type == SECT_INSIDE ) continue;
	send_to_char( "Lightning flashes in the sky.\n\r", ch );
    }

    send_to_char( "A stream of lightning arcs down from the sky.\n\r", ch );
    act("A stream of lightning arcs down from the sky.", ch, NULL, NULL, TO_ROOM);

    dam = number_range(500,1000);
    hp = victim->hit;
    damage( ch, victim, dam, sn );
    WAIT_STATE(ch,6);
    if (victim == NULL || victim->position == POS_DEAD) return;
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_LIGHTNING) )
	victim->hit = hp;
    update_pos(victim);
    return;
}

void do_wall( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    char       arg [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    int        duration = number_range(15,30);
    int        direction, rev;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) < 2)
    {
	send_to_char("You require the level 2 power of the Children of Gaia to create a Wall of Water.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0')
    {
	send_to_char( "Which direction do you wish to make the wall?\n\r", ch );
	return;
    }

         if (!str_cmp(arg,"north") || !str_cmp(arg,"n")) 
	{direction = DIR_NORTH; rev = DIR_SOUTH;}
    else if (!str_cmp(arg,"south") || !str_cmp(arg,"s")) 
	{direction = DIR_SOUTH; rev = DIR_NORTH;}
    else if (!str_cmp(arg,"east")  || !str_cmp(arg,"e")) 
	{direction = DIR_EAST;  rev = DIR_WEST;}
    else if (!str_cmp(arg,"west")  || !str_cmp(arg,"w")) 
	{direction = DIR_WEST;  rev = DIR_EAST;}
    else if (!str_cmp(arg,"up")    || !str_cmp(arg,"u")) 
	{direction = DIR_UP;    rev = DIR_DOWN;}
    else if (!str_cmp(arg,"down")  || !str_cmp(arg,"d")) 
	{direction = DIR_DOWN;  rev = DIR_UP;}
    else
    {
	send_to_char( "Select direction: North, South, East, West, Up or Down.\n\r", ch );
	return;
    }

    switch (direction)
    {
	default:        strcpy(arg,"north"); break;
	case DIR_NORTH: strcpy(arg,"north"); break;
	case DIR_SOUTH: strcpy(arg,"south"); break;
	case DIR_EAST:  strcpy(arg,"east");  break;
	case DIR_WEST:  strcpy(arg,"west");  break;
	case DIR_UP:    strcpy(arg,"up");    break;
	case DIR_DOWN:  strcpy(arg,"down");  break;
    }

    if ( ( pexit = ch->in_room->exit[direction] ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( ( location = ch->in_room ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( ( to_room = pexit->to_room ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_SET(pexit->exit_info, EX_WALL_WATER) || 
	IS_SET(pexit->exit_info, EX_WALL_GRANITE) || 
	IS_SET(pexit->exit_info, EX_WALL_FLAME))
    {
	send_to_char("There is already a wall blocking that exit.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->item_type = ITEM_WALL;
    obj->value[0] = direction;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("wall flowing water");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of flowing water");
    sprintf(buf,"A wall of flowing water blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_WATER);

    if (direction == DIR_UP)
	sprintf(buf,"Water pours from the ground and forms a wall above you.");
    else if (direction == DIR_DOWN)
	sprintf(buf,"Water pours from the ground and forms a wall below you.");
    else
	sprintf(buf,"Water pours from the ground and forms a wall to the %s.",arg);
    act( buf, ch, NULL, NULL, TO_CHAR );
    act( buf, ch, NULL, NULL, TO_ROOM );
    WAIT_STATE(ch,12);

    if (location == to_room) return;

    char_from_room(ch);
    char_to_room(ch, to_room);

    if ( ( pexit = ch->in_room->exit[rev] ) == NULL ) return;
    if (IS_SET(pexit->exit_info, EX_WALL_WATER) || 
	IS_SET(pexit->exit_info, EX_WALL_GRANITE) || 
	IS_SET(pexit->exit_info, EX_WALL_FLAME)) return;

    switch (rev)
    {
	default:        strcpy(arg,"north"); break;
	case DIR_NORTH: strcpy(arg,"north"); break;
	case DIR_SOUTH: strcpy(arg,"south"); break;
	case DIR_EAST:  strcpy(arg,"east");  break;
	case DIR_WEST:  strcpy(arg,"west");  break;
	case DIR_UP:    strcpy(arg,"up");    break;
	case DIR_DOWN:  strcpy(arg,"down");  break;
    }

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->item_type = ITEM_WALL;
    obj->value[0] = rev;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("wall flowing water");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of flowing water");
    sprintf(buf,"A wall of flowing water blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_WATER);

    if (rev == DIR_UP)
	sprintf(buf,"Water pours from the ground and forms a wall above you.");
    else if (rev == DIR_DOWN)
	sprintf(buf,"Water pours from the ground and forms a wall below you.");
    else
	sprintf(buf,"Water pours from the ground and forms a wall to the %s.",arg);
    act( buf, ch, NULL, NULL, TO_ROOM );

    char_from_room(ch);
    char_to_room(ch, location);

    return;
}

void do_animal( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_breed(ch, BREED_LUPUS) < 3)
    {
	send_to_char("You require the level 3 Lupus power to Master the animals.\n\r",ch);
	return;
    }

    if (IS_MORE(ch,MORE_ANIMAL_MASTER) )
    {
	send_to_char("You are no longer so attractive to animals.\n\r",ch);
	REMOVE_BIT(ch->more, MORE_ANIMAL_MASTER);
	return;
    }
    send_to_char("You make yourself attractive to animals.\n\r",ch);
    SET_BIT(ch->more, MORE_ANIMAL_MASTER);
    return;
}

void do_fleshbond( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char arg[MAX_INPUT_LENGTH];
    int mob_vnum;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_breed(ch, BREED_LUPUS) < 5)
    {
	send_to_char("You require the level 5 Lupus power to Flesh Bond an animal to yourself.\n\r",ch);
	return;
    }

    mob_vnum = ch->pcdata->absorb[ABS_MOB_VNUM];

    if (mob_vnum > 0)
    {
	if ( ( victim = create_mobile( get_mob_index( mob_vnum ) ) ) == NULL )
	{
	    send_to_char("Bug - Please Inform KaVir.\n\r",ch);
	    return;
	}
	if ( ( ( ch->hit - ch->pcdata->absorb[ABS_MOB_HP] ) < 1 ) || 
	    ( ( ch->max_hit - ch->pcdata->absorb[ABS_MOB_MAX_HP]) < 1 ) )
	{
	    send_to_char("You would be unable to survive the removal of the Flesh Bonded creature.\n\r",ch);
	    return;
	}
	ch->hit -= ch->pcdata->absorb[ABS_MOB_HP];
	ch->max_hit -= ch->pcdata->absorb[ABS_MOB_MAX_HP];
	victim->hit = ch->pcdata->absorb[ABS_MOB_HP];
	victim->max_hit = ch->pcdata->absorb[ABS_MOB_MAX_HP];
	ch->pcdata->absorb[ABS_MOB_VNUM] = 0;
	ch->pcdata->absorb[ABS_MOB_HP] = 0;
	ch->pcdata->absorb[ABS_MOB_MAX_HP] = 0;
	char_to_room(victim,ch->in_room);
	act("You pull $N out of your body.",ch,NULL,victim,TO_CHAR);
	act("$n pulls $N out of $s body.",ch,NULL,victim,TO_ROOM);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What animal do you wish to Flesh Bond?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "I think not...\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    switch ( victim->pIndexData->vnum )
    {
	default:
	    send_to_char( "You are unable to Flesh Bond with them.\n\r", ch );
	    return;
	case 3062:
	case 3066:
	case 5333:
	case 30006:
	case 30007:
	case 30008:
	case 30009:
	case 30010:
	case 30013:
	case 30014:
	    break;
    }

    act("You absorb $N into your body.",ch,NULL,victim,TO_CHAR);
    act("$n absorbs $N into $s body.",ch,NULL,victim,TO_NOTVICT);
    act("$n absorbs you into $s body.",ch,NULL,victim,TO_VICT);

    for ( obj = victim->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	obj_from_char(obj);
	obj_to_room(obj,victim->in_room);
	act("$p falls to the ground.",victim,obj,NULL,TO_ROOM);
    }
    while ( victim->affected )
	affect_remove( victim, victim->affected );

    ch->pcdata->absorb[ABS_MOB_VNUM] = victim->pIndexData->vnum;
    ch->pcdata->absorb[ABS_MOB_HP] = victim->hit;
    ch->pcdata->absorb[ABS_MOB_MAX_HP] = victim->max_hit;
    ch->hit += victim->hit;
    ch->max_hit += victim->max_hit;
    extract_char(victim, TRUE);
    return;
}

void do_masssummons( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    int count = 0;

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_breed(ch, BREED_LUPUS) < 4)
    {
	send_to_char("You require the level 4 Lupus power to use Mass Summons.\n\r",ch);
	return;
    }

    if ( ch->in_room == NULL )
    {
	send_to_char( "Bug - Please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if (count >= 5) return;
	if ( !IS_NPC(victim) || victim->in_room == NULL || 
	    victim->in_room == ch->in_room) continue;
	switch ( victim->pIndexData->vnum )
	{
	    default: continue;
	    case 30006:
	    case 30007:
	    case 30008:
	    case 30009:
	    case 30010:
	    case 30013:
	    case 30014:
		break;
	}
	count++;
	act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
	char_from_room( victim );
	char_to_room( victim, ch->in_room );
	do_look( victim, "auto" );
	WAIT_STATE( ch, 12 );
	if ( (mount = victim->mount) != NULL )
	{
	    act( "$n arrives suddenly on $N.", mount, NULL, victim, TO_NOTVICT );
	    act( "$N has summoned you!", mount, NULL, ch, TO_CHAR );
	}
	else
	    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
    }
    if (count < 1) send_to_char("Nothing happens.\n\r",ch);
    return;
}

void do_gifts( CHAR_DATA *ch, char *argument )
{
    char arg  [MAX_INPUT_LENGTH];
    char buf  [MAX_INPUT_LENGTH];
    char disc [25];
    int  improve, imp_cost;
    int  sn, col;
    int  gt = 0;
    int  gift = 0;
    int  ww_max = (ch->class != CLASS_WEREWOLF) ? 3 : 5;

    bool got_disc = FALSE;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("                                  -= Gifts =-\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	col    = 0;
	for ( sn = BREED_HOMID; sn <= BREED_METIS; sn++ )
	{
	    if ( ch->pcdata->breed[sn] == 0 ) continue;
	    got_disc = TRUE;
	    switch (sn)
	    {
		default: strcpy(disc,"None"); break;
		case BREED_HOMID: strcpy(disc,"Homid"); break;
		case BREED_LUPUS: strcpy(disc,"Lupus"); break;
		case BREED_METIS: strcpy(disc,"Metis"); break;
	    }
	    if (ch->pcdata->cland[0] == sn) strcpy(disc,upper_cap(disc));
	    if (ch->pcdata->breed[sn] != 0 )
	    {
		sprintf( buf, "%-16s: %-2d      ",
		    disc, get_breed(ch,sn) );
		if (ch->pcdata->cland[0] == sn) ADD_COLOUR(ch, buf, WHITE);
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	}

	for ( sn = AUSPICE_RAGABASH; sn <= AUSPICE_AHROUN; sn++ )
	{
	    if ( ch->pcdata->auspice[sn] == 0 ) continue;
	    got_disc = TRUE;
	    switch (sn)
	    {
		default: strcpy(disc,"None"); break;
		case AUSPICE_RAGABASH: strcpy(disc,"Ragabash"); break;
		case AUSPICE_THEURGE:  strcpy(disc,"Theurge");  break;
		case AUSPICE_PHILODOX: strcpy(disc,"Philodox"); break;
		case AUSPICE_GALLIARD: strcpy(disc,"Galliard"); break;
		case AUSPICE_AHROUN:   strcpy(disc,"Ahroun");   break;
	    }
	    if (ch->pcdata->cland[1] == sn) strcpy(disc,upper_cap(disc));
	    if (ch->pcdata->auspice[sn] != 0 )
	    {
		sprintf( buf, "%-16s: %-2d      ", disc, get_auspice(ch,sn) );
		if (ch->pcdata->cland[1] == sn) ADD_COLOUR(ch, buf, WHITE);
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	}

	for ( sn = TRIBE_BLACK_FURIES; sn <= TRIBE_WENDIGOS; sn++ )
	{
	    if ( ch->pcdata->tribes[sn] == 0 ) continue;
	    got_disc = TRUE;
	    switch (sn)
	    {
		default: strcpy(disc,"None"); break;
		case TRIBE_BLACK_FURIES:     strcpy(disc,"Black Furies"); break;
		case TRIBE_BONE_GNAWERS:     strcpy(disc,"Bone Gnawers"); break;
		case TRIBE_CHILDREN_OF_GAIA: strcpy(disc,"Children of Gaia"); break;
		case TRIBE_FIANNA:           strcpy(disc,"Fianna"); break;
		case TRIBE_GET_OF_FENRIS:    strcpy(disc,"Get of Fenris"); break;
		case TRIBE_GLASS_WALKERS:    strcpy(disc,"Glass Walkers"); break;
		case TRIBE_RED_TALONS:       strcpy(disc,"Red Talons"); break;
		case TRIBE_SHADOW_LORDS:     strcpy(disc,"Shadow Lords"); break;
		case TRIBE_SILENT_STRIDERS:  strcpy(disc,"Silent Striders"); break;
		case TRIBE_SILVER_FANGS:     strcpy(disc,"Silver Fangs"); break;
		case TRIBE_STARGAZERS:       strcpy(disc,"Stargazers"); break;
		case TRIBE_UKTENA:           strcpy(disc,"Uktena"); break;
		case TRIBE_WENDIGOS:         strcpy(disc,"Wendigos"); break;
	    }
	    if (ch->pcdata->cland[2] == sn) strcpy(disc,upper_cap(disc));
	    if (ch->pcdata->tribes[sn] != 0)
	    {
		sprintf( buf, "%-16s: %-2d      ", disc, get_tribe(ch,sn) );
		if (ch->pcdata->cland[2] == sn) ADD_COLOUR(ch, buf, WHITE);
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	}

	if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	if (!got_disc) send_to_char("You currently have no gifts.\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("To improve a gift rating, type: gifts <name>\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	return;
    }

    if      (!str_cmp(argument,"homid")) 
	{improve = BREED_HOMID; gt = 0;}
    else if (!str_cmp(argument,"lupus")) 
	{improve = BREED_LUPUS; gt = 0;}
    else if (!str_cmp(argument,"metis")) 
	{improve = BREED_METIS; gt = 0;}
    else if (!str_cmp(argument,"ragabash")) 
	{improve = AUSPICE_RAGABASH; gt = 1;}
    else if (!str_cmp(argument,"theurge")) 
	{improve = AUSPICE_THEURGE; gt = 1;}
    else if (!str_cmp(argument,"philodox")) 
	{improve = AUSPICE_PHILODOX; gt = 1;}
    else if (!str_cmp(argument,"galliard")) 
	{improve = AUSPICE_GALLIARD; gt = 1;}
    else if (!str_cmp(argument,"ahroun")) 
	{improve = AUSPICE_AHROUN; gt = 1;}
    else if (!str_cmp(argument,"black furies")) 
	{improve = TRIBE_BLACK_FURIES; gt = 2;}
    else if (!str_cmp(argument,"bone gnawers")) 
	{improve = TRIBE_BONE_GNAWERS; gt = 2;}
    else if (!str_cmp(argument,"children of gaia")) 
	{improve = TRIBE_CHILDREN_OF_GAIA; gt = 2;}
    else if (!str_cmp(argument,"fianna")) 
	{improve = TRIBE_FIANNA; gt = 2;}
    else if (!str_cmp(argument,"get of fenris")) 
	{improve = TRIBE_GET_OF_FENRIS; gt = 2;}
    else if (!str_cmp(argument,"glass walkers")) 
	{improve = TRIBE_GLASS_WALKERS; gt = 2;}
    else if (!str_cmp(argument,"red talons")) 
	{improve = TRIBE_RED_TALONS; gt = 2;}
    else if (!str_cmp(argument,"shadow lords")) 
	{improve = TRIBE_SHADOW_LORDS; gt = 2;}
    else if (!str_cmp(argument,"silent striders")) 
	{improve = TRIBE_SILENT_STRIDERS; gt = 2;}
    else if (!str_cmp(argument,"silver fangs")) 
	{improve = TRIBE_SILVER_FANGS; gt = 2;}
    else if (!str_cmp(argument,"stargazers")) 
	{improve = TRIBE_STARGAZERS; gt = 2;}
    else if (!str_cmp(argument,"uktena")) 
	{improve = TRIBE_UKTENA; gt = 2;}
    else if (!str_cmp(argument,"wendigos")) 
	{improve = TRIBE_WENDIGOS; gt = 2;}
    else
    {
	send_to_char("You know of no such gift.\n\r",ch);
	return;
    }

    switch (gt)
    {
	default: break;
	case 0: gift = ch->pcdata->breed[improve];
		break;
	case 1: gift = ch->pcdata->auspice[improve];
		break;
	case 2: gift = ch->pcdata->tribes[improve];
		break;
    }

    if ( ch->pcdata->cland[gt] == improve ) ww_max = 5;

    if (gift == 0)
    {
	send_to_char("You know of no such gift.\n\r",ch);
	return;
    }

    if (gift >= ww_max)
    {
	send_to_char("You are unable to improve your ability in that gift any further.\n\r",ch);
	return;
    }

    if (gift < 1) gift = 1;

    if (ch->class != CLASS_WEREWOLF)
    {
	if ((gt == 0 || gt == 1) && ch->pcdata->cland[gt] == improve) 
	    imp_cost = gift * 10;
	else
	    imp_cost = gift * 15;
    }
    else if (ch->pcdata->cland[2] == -1 && gt == 2)
	imp_cost = gift * 9;
    else if (ch->pcdata->cland[gt] == improve)
	imp_cost = gift * 5;
    else
	imp_cost = gift * 10;

    if (ch->practice < imp_cost)
    {
	sprintf(buf,"You need another %d primal to increase that gift.\n\r", 
	    imp_cost - ch->practice );
	send_to_char(buf,ch);
	return;
    }

    if (gift < 1) gift = 1;
    else gift++;

    switch (gt)
    {
	default : break;
	case 0: ch->pcdata->breed[improve] = gift;   break;
	case 1: ch->pcdata->auspice[improve] = gift; break;
	case 2: ch->pcdata->tribes[improve] = gift;   break;
    }

    ch->practice -= imp_cost;
    send_to_char("You improve your gift.\n\r",ch);

    return;
}

void do_tribegifts( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       improve, sn, col, gtype;

    strcpy( arg, argument );

    if ( IS_NPC(ch) ) return;

    if ( !IS_WEREWOLF(ch) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                  -= Gifts =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	/* Breed gift */
	improve = ch->pcdata->cland[0];
	sprintf( buf, "Breed Gift: %s.\n\r", gift_breed_name(improve) );
	send_to_char( buf, ch );

	/* Auspice gift */
	improve = ch->pcdata->cland[1];
	sprintf( buf, "Auspice Gift: %s.\n\r", gift_auspice_name(improve) );
	send_to_char( buf, ch );

	/* Tribe gift */
	improve = ch->pcdata->cland[2];
	sprintf( buf, "Tribe Gift: %s.\n\r", gift_tribe_name(improve) );
	send_to_char( buf, ch );

	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	if ( ch->pcdata->cland[0] == -1 )
	{
	    send_to_char("Please select a breed gift from the list below.\n\r",ch);
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    col    = 0;
	    for ( sn = 0; sn <= BREED_MAX; sn++ )
	    {
		sprintf( buf, "%-15s           ", gift_breed_name( sn ) );
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	    if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    return;
	}

	if ( ch->pcdata->cland[1] == -1 )
	{
	    send_to_char("Please select an auspice gift from the list below.\n\r",ch);
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    col    = 0;
	    for ( sn = 0; sn <= AUSPICE_MAX; sn++ )
	    {
		sprintf( buf, "%-15s           ", gift_auspice_name( sn ) );
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	    if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    return;
	}

	if ( ch->pcdata->cland[2] == -1 && strlen(ch->clan) > 1 )
	{
	    send_to_char("Please select a tribe gift from the list below.\n\r",ch);
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    col    = 0;
	    for ( sn = 0; sn <= TRIBE_MAX; sn++ )
	    {
		sprintf( buf, "%-15s           ", gift_tribe_name( sn ) );
		send_to_char( buf, ch );
		if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    }
	    if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    return;
	}
	return;
    }

    if (!str_cmp(arg,"homid")) 
	{improve = BREED_HOMID;gtype = 0;}
    else if (!str_cmp(arg,"lupus")) 
	{improve = BREED_LUPUS;gtype = 0;}
    else if (!str_cmp(arg,"metis")) 
	{improve = BREED_METIS;gtype = 0;}
    else if (!str_cmp(arg,"ragabash")) 
	{improve = AUSPICE_RAGABASH;gtype = 1;}
    else if (!str_cmp(arg,"theurge")) 
	{improve = AUSPICE_THEURGE;gtype = 1;}
    else if (!str_cmp(arg,"philodox")) 
	{improve = AUSPICE_PHILODOX;gtype = 1;}
    else if (!str_cmp(arg,"galliard")) 
	{improve = AUSPICE_GALLIARD;gtype = 1;}
    else if (!str_cmp(arg,"ahroun")) 
	{improve = AUSPICE_AHROUN;gtype = 1;}
    else if (!str_cmp(arg,"black furies")) 
	{improve = TRIBE_BLACK_FURIES;gtype = 2;}
    else if (!str_cmp(arg,"bone gnawers")) 
	{improve = TRIBE_BONE_GNAWERS;gtype = 2;}
    else if (!str_cmp(arg,"children of gaia")) 
	{improve = TRIBE_CHILDREN_OF_GAIA;gtype = 2;}
    else if (!str_cmp(arg,"fianna")) 
	{improve = TRIBE_FIANNA;gtype = 2;}
    else if (!str_cmp(arg,"get of fenris")) 
	{improve = TRIBE_GET_OF_FENRIS;gtype = 2;}
    else if (!str_cmp(arg,"get of fenris")) 
	{improve = TRIBE_GET_OF_FENRIS;gtype = 2;}
    else if (!str_cmp(arg,"glass walkers")) 
	{improve = TRIBE_GLASS_WALKERS;gtype = 2;}
    else if (!str_cmp(arg,"red talons")) 
	{improve = TRIBE_RED_TALONS;gtype = 2;}
    else if (!str_cmp(arg,"shadow lords")) 
	{improve = TRIBE_SHADOW_LORDS;gtype = 2;}
    else if (!str_cmp(arg,"silent striders")) 
	{improve = TRIBE_SILENT_STRIDERS;gtype = 2;}
    else if (!str_cmp(arg,"silver fangs")) 
	{improve = TRIBE_SILVER_FANGS;gtype = 2;}
    else if (!str_cmp(arg,"stargazers")) 
	{improve = TRIBE_STARGAZERS;gtype = 2;}
    else if (!str_cmp(arg,"uktena")) 
	{improve = TRIBE_UKTENA;gtype = 2;}
    else if (!str_cmp(arg,"wendigos")) 
	{improve = TRIBE_WENDIGOS;gtype = 2;}
    else
    {
	send_to_char("You know of no such gift.\n\r",ch);
	return;
    }

    switch (gtype)
    {
	default:
	    if (ch->pcdata->cland[0] != -1)
	    {
		send_to_char("You already have a breed gift.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->breed[improve] != 0)
	    {
		send_to_char("You already know that gift.\n\r",ch);
		return;
	    }
	    ch->pcdata->cland[0] = improve;
	    ch->pcdata->breed[improve] = -1;
	    break;
	case 1:
	    if (ch->pcdata->cland[1] != -1)
	    {
		send_to_char("You already have an auspice gift.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->auspice[improve] != 0)
	    {
		send_to_char("You already know that gift.\n\r",ch);
		return;
	    }
	    ch->pcdata->cland[1] = improve;
	    ch->pcdata->auspice[improve] = -1;
	    break;
	case 2:
	    if (strlen(ch->clan) < 2 && ch->vampgen > 2)
	    {
		send_to_char("Ronin are not permitted a tribe gift.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->cland[2] != -1)
	    {
		send_to_char("You already have a tribe gift.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->tribes[improve] != 0)
	    {
		send_to_char("You already know that gift.\n\r",ch);
		return;
	    }
	    ch->pcdata->cland[2] = improve;
	    ch->pcdata->tribes[improve] = -1;
	    break;
    }
    send_to_char("Ok.\n\r",ch);
    return;
}

char *gift_breed_name( int gift_no )
{
    static char disc[6];

    switch ( gift_no )
    {
	default: 
	    strcpy(disc,"None"); 
	    break;
	case BREED_HOMID: 
	    strcpy(disc,"Homid"); 
	    break;
	case BREED_LUPUS: 
	    strcpy(disc,"Lupus"); 
	    break;
	case BREED_METIS: 
	    strcpy(disc,"Metis"); 
	    break;
    }
    return &disc[0];
}

char *gift_auspice_name( int gift_no )
{
    static char disc[9];

    switch ( gift_no )
    {
	default: 
	    strcpy(disc,"None"); 
	    break;
	case AUSPICE_RAGABASH: 
	    strcpy(disc,"Ragabash"); 
	    break;
	case AUSPICE_THEURGE: 
	    strcpy(disc,"Theurge"); 
	    break;
	case AUSPICE_PHILODOX: 
	    strcpy(disc,"Philodox"); 
	    break;
	case AUSPICE_GALLIARD: 
	    strcpy(disc,"Galliard"); 
	    break;
	case AUSPICE_AHROUN: 
	    strcpy(disc,"Ahroun"); 
	    break;
    }
    return &disc[0];
}

char *gift_tribe_name( int gift_no )
{
    static char disc[17];

    switch ( gift_no )
    {
	default: 
	    strcpy(disc,"None"); 
	    break;
	case TRIBE_BLACK_FURIES:
	    strcpy(disc,"Black Furies"); 
	    break;
	case TRIBE_BONE_GNAWERS:
	    strcpy(disc,"Bone Gnawers"); 
	    break;
	case TRIBE_CHILDREN_OF_GAIA:
	    strcpy(disc,"Children of Gaia"); 
	    break;
	case TRIBE_FIANNA:
	    strcpy(disc,"Fianna"); 
	    break;
	case TRIBE_GET_OF_FENRIS:
	    strcpy(disc,"Get of Fenris"); 
	    break;
	case TRIBE_GLASS_WALKERS:
	    strcpy(disc,"Glass Walkers"); 
	    break;
	case TRIBE_RED_TALONS:
	    strcpy(disc,"Red Talons"); 
	    break;
	case TRIBE_SHADOW_LORDS:
	    strcpy(disc,"Shadow Lords"); 
	    break;
	case TRIBE_SILENT_STRIDERS:
	    strcpy(disc,"Silent Striders"); 
	    break;
	case TRIBE_SILVER_FANGS:
	    strcpy(disc,"Silver Fangs"); 
	    break;
	case TRIBE_STARGAZERS:
	    strcpy(disc,"Stargazers"); 
	    break;
	case TRIBE_UKTENA:
	    strcpy(disc,"Uktena"); 
	    break;
	case TRIBE_WENDIGOS:
	    strcpy(disc,"Wendigos"); 
	    break;
    }
    return &disc[0];
}


