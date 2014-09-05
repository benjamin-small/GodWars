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

void	adv_spell_damage	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_affect	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_action	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	birth_write		args( ( CHAR_DATA *ch, char *argument ) );
bool	birth_ok		args( ( CHAR_DATA *ch, char *argument ) );


#define MAX_CHAIN(ch) ((ch)->class == CLASS_MAGE ? 5 : \
                       (ch)->class == CLASS_SKINDANCER ? 4 : 3)

/* Need to get rid of those flames somehow - KaVir */
void do_smother( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Smother whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot smother yourself.\n\r", ch );
	return;
    }

    if (!IS_AFFECTED(victim, AFF_FLAMING))
    {
	send_to_char( "But they are not on fire!\n\r", ch );
	return;
    }

    if ( number_percent() > 10 )
    {
	act( "You try to smother the flames around $N but fail!",  ch, NULL, victim, TO_CHAR    );
	act( "$n tries to smother the flames around you but fails!", ch, NULL, victim, TO_VICT    );
	act( "$n tries to smother the flames around $N but fails!",  ch, NULL, victim, TO_NOTVICT );
	if ( number_percent() > 98 && !IS_AFFECTED(ch,AFF_FLAMING) )
	{
	    act( "A spark of flame from $N's body sets you on fire!",  ch, NULL, victim, TO_CHAR    );
	    act( "A spark of flame from your body sets $n on fire!", ch, NULL, victim, TO_VICT    );
	    act( "A spark of flame from $N's body sets $n on fire!",  ch, NULL, victim, TO_NOTVICT );
	    SET_BIT(ch->affected_by, AFF_FLAMING);
	}
	return;
    }

    act( "You manage to smother the flames around $M!",  ch, NULL, victim, TO_CHAR    );
    act( "$n manages to smother the flames around you!", ch, NULL, victim, TO_VICT    );
    act( "$n manages to smother the flames around $N!",  ch, NULL, victim, TO_NOTVICT );
    REMOVE_BIT(victim->affected_by, AFF_FLAMING);
    REMOVE_BIT(victim->more, MORE_FLAMES);
    return;
}

/* Loads of Vampire powers follow.  KaVir */

void do_fangs( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_DEMON(ch) && !IS_VAMPIRE(ch) && !IS_WEREWOLF(ch))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_FANGS))
	{
	    send_to_char("You haven't been granted the gift of fangs.\n\r",ch);
	    return;
	}
    }
    else if (!IS_VAMPIRE(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (IS_VAMPIRE(ch) && ch->pcdata->wolf > 0)
    {
	send_to_char("Your beast won't let you retract your fangs.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_FANGS) )
    {
	send_to_char("Your fangs slide back into your gums.\n\r",ch);
	act("$n's fangs slide back into $s gums.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_FANGS);
	return;
    }
    send_to_char("Your fangs extend out of your gums.\n\r",ch);
    act("A pair of razor sharp fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_FANGS);
    return;
}

void do_shift( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];
/*
    int toform = 0;
*/
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
/*
    if ( arg[0] == '\0' )
    {
	send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );
	return;
    }
    if      ( !str_cmp(arg,"homid" ) ) toform = 1;
    else if ( !str_cmp(arg,"glabro") ) toform = 2;
    else if ( !str_cmp(arg,"crinos") ) toform = 3;
    else if ( !str_cmp(arg,"hispo" ) ) toform = 4;
    else if ( !str_cmp(arg,"lupus" ) ) toform = 5;
    else
    {
	send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );
	return;
    }
    if ( ch->pcdata->wolfform[0] < 1 || ch->pcdata->wolfform[0] > 5) 
	ch->pcdata->wolfform[0] = 1;
    if ( ch->pcdata->wolfform[0] == toform )
    {
	send_to_char( "You are already in that form.\n\r", ch );
	return;
    }
    ch->pcdata->wolfform[1] = toform;
*/
    if ( ch->pcdata->wolfform[1] <= FORM_HOMID )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only change from homid form.\n\r", ch );
	    return;
	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	if (!IS_VAMPAFF(ch,VAM_FANGS)) SET_BIT(ch->vampaff, VAM_FANGS);
	if (!IS_VAMPAFF(ch,VAM_CLAWS)) SET_BIT(ch->vampaff, VAM_CLAWS);
	send_to_char( "You transform into lupus form.\n\r", ch );
	act( "$n transforms into a wolf.", ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
	SET_BIT(ch->polyaff, POLY_WOLF);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s the wolf", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	ch->pcdata->wolfform[1] = FORM_LUPUS;
	return;
    }
    else
    {
	if ( ch->pcdata->wolfform[1] != FORM_LUPUS )
	{
	    send_to_char( "You can only change from lupus form.\n\r", ch );
	    return;
	}
	if (IS_VAMPAFF(ch,VAM_CLAWS)) REMOVE_BIT(ch->vampaff, VAM_CLAWS);
	if (IS_VAMPAFF(ch,VAM_FANGS)) REMOVE_BIT(ch->vampaff, VAM_FANGS);
	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	send_to_char( "You transform into homid form.\n\r", ch );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->polyaff, POLY_WOLF);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	ch->pcdata->wolfform[1] = FORM_HOMID;
	if (ch->pcdata->wolf >= 100) ch->pcdata->wolf = 99;
	return;
    }
    return;
}

void do_claws( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_SET(ch->act, PLR_DEMON) || IS_SET(ch->act, PLR_CHAMPION))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_CLAWS))
	{
	    send_to_char("You haven't been granted the gift of claws.\n\r",ch);
	    return;
	}
    }
    else if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ((IS_VAMPIRE(ch) || IS_GHOUL(ch)) && get_disc(ch,DISC_PROTEAN) < 2)
    {
	send_to_char("You require level 2 Protean to use Claws.\n\r",ch);
	return;
    }

    if ((IS_VAMPIRE(ch) || IS_GHOUL(ch)) && !IS_ABOMINATION(ch) && ch->pcdata->wolf > 0)
    {
	send_to_char("Your beast won't let you retract your claws.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_CLAWS) )
    {
	if (IS_VAMPIRE(ch) || IS_GHOUL(ch)) 
	{
	    send_to_char("Your claws slide back under your nails.\n\r",ch);
	    act("$n's claws slide back under $s nails.", ch, NULL, NULL, TO_ROOM);
	}
	else if (IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_AHROUN) > 3)
	{
	    send_to_char("Your gleaming silver talons slide back into your fingers.\n\r",ch);
	    act("$n's gleaming silver talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("Your talons slide back into your fingers.\n\r",ch);
	    act("$n's talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	}
	REMOVE_BIT(ch->vampaff, VAM_CLAWS);
	return;
    }

    if (IS_VAMPIRE(ch) || IS_GHOUL(ch)) 
    {
	send_to_char("Sharp claws extend from under your finger nails.\n\r",ch);
	act("Sharp claws extend from under $n's finger nails.", ch, NULL, NULL, TO_ROOM);
    }
    else if (IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_AHROUN) > 3) 
    {
	send_to_char("Gleaming silver talons extend from your fingers.\n\r",ch);
	act("Gleaming silver talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
	act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->vampaff, VAM_CLAWS);
    return;
}

void do_nightsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_DEMON(ch))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_EYES))
	{
	    send_to_char("You haven't been granted the gift of nightsight.\n\r",ch);
	    return;
	}
    }
    else if (IS_WEREWOLF(ch))
    {
	if (get_breed(ch, BREED_METIS) < 1 && get_disc(ch, DISC_PROTEAN) < 1 && 
	    !IS_POLYAFF(ch, POLY_ZULO))
	{
	    send_to_char("You require the level 1 Metis power to use Eyes of the Cat.\n\r",ch);
	    return;
	}
    }
    else if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (((!IS_WEREWOLF(ch) || IS_ABOMINATION(ch)) && !IS_DEMON(ch)) && 
	get_disc(ch,DISC_PROTEAN) < 1)
    {
	send_to_char("You require level 1 Protean to use Gleam of Red Eyes.\n\r",ch);
	return;
    }
    else if ((IS_VAMPIRE(ch) || IS_GHOUL(ch)) && ch->pcdata->wolf > 0)
    {
	send_to_char("Not while your beast is in control.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	if (IS_WEREWOLF(ch) && get_disc(ch, DISC_PROTEAN) < 1)
	{
	    send_to_char("The green glow in your eyes fades.\n\r",ch);
	    act("The green glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	}
	else if (IS_ANGEL(ch))
	{
	    send_to_char("The blue glow in your eyes fades.\n\r",ch);
	    act("The blue glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	}
	else
	{
	    send_to_char("The red glow in your eyes fades.\n\r",ch);
	    act("The red glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	}
	REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	return;
    }
    if (IS_WEREWOLF(ch) && get_disc(ch, DISC_PROTEAN) < 1)
    {
	send_to_char("Your eyes start glowing green.\n\r",ch);
	act("$n's eyes start glowing green.", ch, NULL, NULL, TO_ROOM);
    }
    else if (IS_ANGEL(ch))
    {
	send_to_char("Your eyes start glowing blue.\n\r",ch);
	act("$n's eyes start glowing blue.", ch, NULL, NULL, TO_ROOM);
    }
    else
    {
	send_to_char("Your eyes start glowing red.\n\r",ch);
	act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
    }
    SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    return;
}

void do_majesty( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_SHADOW_LORDS) > 2)
    {
	do_icy(ch,"");
	return;
    }

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_disc(ch,DISC_PRESENCE) < 5)
    {
	send_to_char("You require level 5 Presence to use Majesty.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch,ITEMA_PEACE) )
    {
	send_to_char("You lower your Majesty.\n\r",ch);
	act("$n no longer seems so imposing.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->itemaffect, ITEMA_PEACE);
	ch->pcdata->majesty = 60;
	return;
    }
    if (ch->pcdata->majesty > 0)
    {
	if (ch->pcdata->majesty > 1)
	    sprintf(buf,"You cannot raise your Majesty for another %d seconds.\n\r",ch->pcdata->majesty);
	else
	    sprintf(buf,"You cannot raise your Majesty for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }
    send_to_char("Your raise your Majesty.\n\r",ch);
    act("$n suddenly appears very imposing.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->itemaffect, ITEMA_PEACE);
    return;
}

void do_awe( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_disc(ch,DISC_PRESENCE) < 1)
    {
	send_to_char("You require level 1 Presence to use Awe.\n\r",ch);
	return;
    }

    if (IS_EXTRA(ch,EXTRA_AWE) )
    {
	send_to_char("You are no longer awe-inspiring.\n\r",ch);
	act("$n no longer appears so awe-inspiring.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->extra, EXTRA_AWE);
	return;
    }
    send_to_char("Your are now awe-inspiring.\n\r",ch);
    act("$n suddenly appears very awe-inspiring.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->extra, EXTRA_AWE);
    return;
}

void do_shadowsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_DEMON(ch))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_SHADOWSIGHT))
	{
	    send_to_char("You haven't been granted the gift of shadowsight.\n\r",ch);
	    return;
	}
    }
    else if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
    {
	send_to_char("You can no longer see between planes.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_SHADOWSIGHT);
	return;
    }
    send_to_char("You can now see between planes.\n\r",ch);
    SET_BIT(ch->affected_by, AFF_SHADOWSIGHT);
    return;
}

void do_bite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    bool      can_sire = FALSE;
    int       blood, sn, improve;
    int       ch_gen = ch->vampgen;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) || (ch->vampgen < 1))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (is_affected(ch, gsn_potency)) ch_gen++;

    if (ch->vampgen == 2 && strlen(ch->clan) < 2)
    {
	send_to_char("First you need to get a clan name.\n\r",ch);
	return;
    }


    if (ch->pcdata->cland[0] == -1 ||
	ch->pcdata->cland[1] == -1 ||
	ch->pcdata->cland[2] == -1)
    {
	send_to_char("First you need to buy your disciplines.\n\r",ch);
	return;
    }

    if (ch_gen > 6 || true_generation(ch,ch->lord) > 6) 
	can_sire = FALSE;
    else 
	can_sire = TRUE;

    if (!can_sire)
    {
	send_to_char("You are not able to create any childer.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to drink your blood.\n\r",ch);
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You are already a vampire!\n\r", ch );
	return;
    }

    if (IS_MAGE(victim))
    {
	send_to_char("They refuse to drink your blood.\n\r",ch);
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only make avatars into vampires.\n\r", ch );
	return;
    }

    if (IS_WEREWOLF(victim))
    {
	send_to_char( "You are unable to create werevamps!\n\r", ch );
	return;
    }

    if (IS_VAMPIRE(victim))
    {
	send_to_char( "But they are already a vampire!\n\r", ch );
	return;
    }

    if (IS_DEMON(victim))
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (IS_HIGHLANDER(victim))
    {
	send_to_char( "You cannot turn highlanders into vampires.\n\r", ch );
	return;
    }

    if ( ch->blood[BLOOD_CURRENT] < 10 )
    {
	send_to_char("You don't have enough blood.\n\r",ch);
	return;
    }

    blood = number_range(5,10) * ch->blood[BLOOD_POTENCY];
    ch->blood[BLOOD_CURRENT] -= blood;
    victim->blood[BLOOD_CURRENT] += blood;

    if (ch->beast > 0 && ch->beast < 100) ch->beast += 1;
    act("You feed $N some blood from your open wrist.",ch,NULL,victim,TO_CHAR);
    act("$n feeds $N some blood from $s open wrist.",ch,NULL,victim,TO_NOTVICT);
    act("$n feeds you some blood from $s open wrist.", ch, NULL, victim, TO_VICT);

    /* For Abominations */
    if (IS_WEREWOLF(ch) && get_tribe(ch,TRIBE_GET_OF_FENRIS) > 0)
    {
	act("You scream in agony as the blood burns through your body!", victim, NULL, NULL, TO_CHAR);
	act("$n screams in agony and falls to the ground...DEAD!", victim, NULL, NULL, TO_ROOM);
	group_gain(ch,victim);
	victim->form = 1;
	killperson(ch,victim);
	return;
    }

    if (IS_MAGE(victim))
    {
	send_to_char("You have lost your avatar!\n\r",victim);
	victim->trust = 0;
	victim->level = 3;
    }

    victim->class = CLASS_VAMPIRE;
    if ( victim->vampgen != 0 )
    {
	do_autosave(ch,"");
	do_autosave(victim,"");
	send_to_char( "Your vampiric status has been restored.\n\r", victim );
	return;
    }

    send_to_char( "You are now a vampire.\n\r", victim );    
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    victim->vampgen = ch_gen + 1;
    update_gen(victim);
    free_string(victim->lord);
    if (ch->vampgen == 1)
	victim->lord=str_dup(ch->name);
    else
    {
	sprintf(buf,"%s %s",ch->lord,ch->name);
	victim->lord=str_dup(buf);
    }

    if (strlen(ch->clan) > 1 && (IS_EXTRA(ch, EXTRA_PRINCE) || 
	IS_EXTRA(ch, EXTRA_SIRE)))
    {
	free_string(victim->clan);
	victim->clan=str_dup(ch->clan);
	if (strlen(ch->side) > 1)
	{
	    free_string(victim->side);
	    victim->side=str_dup(ch->side);
	}
    }
    REMOVE_BIT(ch->extra,EXTRA_SIRE);
    if (IS_MORE(ch, MORE_ANTITRIBU)) SET_BIT(victim->more, MORE_ANTITRIBU);

    if ( ch->vampgen == 1 )
    {
	/* Second generation vampires chose all their disciplines */
	victim->pcdata->cland[0] = -1;
	victim->pcdata->cland[1] = -1;
	victim->pcdata->cland[2] = -1;
    }
    else
    {
	/* They get the clan discipline */
	improve = victim->pcdata->cland[0] = ch->pcdata->cland[0];
	if ( improve >= 0 ) victim->pcdata->powers[improve] = -1;

	/* They get their sires personal discipline */
	improve = victim->pcdata->cland[1] = ch->pcdata->cland[2];
	if ( improve >= 0 ) victim->pcdata->powers[improve] = -1;

	/* They get to chose a discipline of their own */
	victim->pcdata->cland[2] = -1;
    }

    for ( sn = 0; sn <= DISC_MAX; sn++ )
    {
	victim->pcdata->powers[sn] = 0;
    }

    do_autosave(victim,"");
    return;
}

void do_claw( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    bool      can_sire = FALSE;
    int       gifts;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) || (ch->vampgen < 1))
    {
	do_claws(ch,"");
	return;
    }

    if (ch->vampgen == 1 || ch->vampgen == 2) can_sire = TRUE;

    if (ch->vampgen > 3) can_sire = FALSE;
    else can_sire = TRUE;

    if (!can_sire)
    {
	send_to_char("You are unable to spread your gift.\n\r",ch);
	return;
    }

    if ( ch->vampgen == 2 )
    {
	if ( strlen(ch->clan) < 2 )
	{
	    send_to_char( "First you need to create a tribe.\n\r", ch );
	    return;
	}
	if ( ch->pcdata->cland[2] == -1 )
	{
	    send_to_char( "First you need to select a tribe gift.\n\r", ch );
	    return;
	}
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Claw whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot claw yourself.\n\r", ch );
	return;
    }

    if (IS_MAGE(victim))
    {
	send_to_char( "You cannot claw mages.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "You cannot claw immortals.\n\r", ch );
	return;
    }

    if (IS_VAMPIRE(victim))
    {
	send_to_char( "You are unable to create werevamps!\n\r", ch );
	return;
    }

    if (IS_DEMON(victim))
    {
	send_to_char( "But they have no soul!\n\r", ch );
	return;
    }

    if (IS_GHOUL(victim))
    {
	send_to_char( "They have been tainted with Kindred blood.\n\r", ch );
	return;
    }

    if (IS_HIGHLANDER(victim))
    {
	send_to_char( "You cannot turn highlanders into werewolves.\n\r", ch );
	return;
    }

    if (IS_WEREWOLF(victim))
    {
	send_to_char( "But they are already a werewolf!\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "You cannot claw an unwilling person.\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("First you better get your claws out!\n\r",ch);
	return;
    }

    act("You plunge your claws into $N.", ch, NULL, victim, TO_CHAR);
    act("$n plunges $s claws into $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n plunges $s claws into your chest.", ch, NULL, victim, TO_VICT);
    victim->class = CLASS_WEREWOLF;
    victim->level = LEVEL_AVATAR;

    victim->pcdata->cland[0] = gifts = number_range(0,2);
    victim->pcdata->breed[gifts] = -1;

    victim->pcdata->cland[1] = gifts = number_range(0,4);
    victim->pcdata->auspice[gifts] = -1;

    if ( ch->vampgen > 2 )
    {
	victim->pcdata->cland[2] = gifts = ch->pcdata->cland[2];
	victim->pcdata->tribes[gifts] = -1;
    }
    else victim->pcdata->cland[2] = -1;

    send_to_char( "You are now a werewolf.\n\r", victim );
    victim->vampgen = ch->vampgen + 1;
    free_string(victim->lord);
    if (ch->vampgen == 1)
	victim->lord=str_dup(ch->name);
    else
    {
	sprintf(buf,"%s %s",ch->lord,ch->name);
	victim->lord=str_dup(buf);
    }

    if (strlen(ch->clan) > 1 && (IS_EXTRA(ch, EXTRA_PRINCE) || 
	IS_EXTRA(ch, EXTRA_SIRE)))
    {
	free_string(victim->clan);
	victim->clan=str_dup(ch->clan);
    }
    if (IS_EXTRA(ch,EXTRA_SIRE)) REMOVE_BIT(ch->extra,EXTRA_SIRE);
    do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}

void do_clanname( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );
    if ( IS_NPC(ch) ) return;

    if ( (!IS_VAMPIRE(ch) && !IS_WEREWOLF(ch)) 
	|| ch->vampgen != 1 )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	if (IS_VAMPIRE(ch))
	    send_to_char( "Who's clan do you wish to name?\n\r", ch );
	else
	    send_to_char( "Who do you wish to give a tribe to?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) ) return;
    if ( victim->vampgen != 2 )
    {
	if (IS_VAMPIRE(ch))
	    send_to_char( "Only the Antediluvians may have clans.\n\r", ch );
	else
	    send_to_char( "Only greater werewolves may own a tribe.\n\r", ch );
	return;
    }
    if ( str_cmp(victim->clan,"") )
    {
	if (IS_VAMPIRE(ch))
	    send_to_char( "But they already have a clan!\n\r", ch );
	else
	    send_to_char( "But they already have a tribe!\n\r", ch );
	return;
    }
    smash_tilde( argument );
    if ( strlen(argument) < 3 || strlen(argument) > 13 )
    {
	if (IS_VAMPIRE(ch))
	    send_to_char( "Clan name should be between 3 and 13 letters long.\n\r", ch );
	else
	    send_to_char( "Tribe name should be between 3 and 13 letters long.\n\r", ch );
	return;
    }
    free_string( victim->clan );
    victim->clan = str_dup( argument );
    if (IS_VAMPIRE(ch))
    	send_to_char( "Clan name set.\n\r", ch );
    else
    	send_to_char( "Tribe name set.\n\r", ch );
    return;
}

void do_clandisc( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       improve, sn, col;

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_VAMPIRE(ch) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                               -= Disciplines =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if ( ch->vampgen <= 2 )
	{
	    /* Clan discipline */
	    improve = ch->pcdata->cland[0];
	    sprintf( buf, "Clan Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );

	    /* Personal discipline 1 */
	    improve = ch->pcdata->cland[1];
	    sprintf( buf, "Personal Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );

	    /* Personal discipline 2 */
	    improve = ch->pcdata->cland[2];
	    sprintf( buf, "Personal Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );
	}
	else
	{
	    /* Clan discipline */
	    improve = ch->pcdata->cland[0];
	    sprintf( buf, "Clan Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );

	    /* Sires discipline */
	    improve = ch->pcdata->cland[1];
	    sprintf( buf, "Sires Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );

	    /* Personal discipline */
	    improve = ch->pcdata->cland[2];
	    sprintf( buf, "Personal Discipline: %s.\n\r", disc_name(improve) );
	    send_to_char( buf, ch );
	}
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if ( ch->pcdata->cland[2] >= 0 ) return;
	send_to_char("Please select another discipline from the list below.\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	col    = 0;
	for ( sn = 0; sn <= DISC_MAX; sn++ )
	{
	    if ( ch->pcdata->powers[sn] != 0 ) continue;
	    sprintf( buf, "%-15s           ", disc_name( sn ) );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	}
	if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"animalism")) 
	improve = DISC_ANIMALISM;
    else if (!str_cmp(arg,"auspex")) 
	improve = DISC_AUSPEX;
    else if (!str_cmp(arg,"celerity")) 
	improve = DISC_CELERITY;
    else if (!str_cmp(arg,"chimerstry")) 
	improve = DISC_CHIMERSTRY;
    else if (!str_cmp(arg,"daimoinon")) 
	improve = DISC_DAIMOINON;
    else if (!str_cmp(arg,"dominate")) 
	improve = DISC_DOMINATE;
    else if (!str_cmp(arg,"fortitude")) 
	improve = DISC_FORTITUDE;
    else if (!str_cmp(arg,"melpominee")) 
	improve = DISC_MELPOMINEE;
    else if (!str_cmp(arg,"necromancy")) 
	improve = DISC_NECROMANCY;
    else if (!str_cmp(arg,"obeah")) 
	improve = DISC_OBEAH;
    else if (!str_cmp(arg,"obfuscate")) 
	improve = DISC_OBFUSCATE;
    else if (!str_cmp(arg,"obtenebration")) 
	improve = DISC_OBTENEBRATION;
    else if (!str_cmp(arg,"potence")) 
	improve = DISC_POTENCE;
    else if (!str_cmp(arg,"presence")) 
	improve = DISC_PRESENCE;
    else if (!str_cmp(arg,"protean")) 
	improve = DISC_PROTEAN;
    else if (!str_cmp(arg,"quietus")) 
	improve = DISC_QUIETUS;
    else if (!str_cmp(arg,"serpentis")) 
	improve = DISC_SERPENTIS;
    else if (!str_cmp(arg,"thanatosis")) 
	improve = DISC_THANATOSIS;
    else if (!str_cmp(arg,"thaumaturgy")) 
	improve = DISC_THAUMATURGY;
    else if (!str_cmp(arg,"vicissitude")) 
	improve = DISC_VICISSITUDE;
    else
    {
	send_to_char("You know of no such discipline.\n\r",ch);
	return;
    }

    if (ch->pcdata->cland[0] == improve ||
	ch->pcdata->cland[1] == improve ||
	ch->pcdata->cland[2] == improve)
    {
	send_to_char("You already know that discipline.\n\r",ch);
	return;
    }

    if ( ch->vampgen < 3 || ch->class != CLASS_VAMPIRE )
    {
	if ( ch->pcdata->cland[0] < 0 )
	    ch->pcdata->cland[0] = improve;
	else if ( ch->pcdata->cland[1] < 0 )
	    ch->pcdata->cland[1] = improve;
	else if ( ch->pcdata->cland[2] < 0 )
	    ch->pcdata->cland[2] = improve;
	else
	{
	    send_to_char("All of your discipline slots are full.\n\r",ch);
	    return;
	}
	ch->pcdata->powers[improve] = -1;
	send_to_char("Ok.\n\r",ch);
    }
    else
    {
	if ( ch->pcdata->cland[2] < 0 )
	    ch->pcdata->cland[2] = improve;
	else
	{
	    send_to_char("Your discipline slot is full.\n\r",ch);
	    return;
	}
	ch->pcdata->powers[improve] = -1;
	send_to_char("Ok.\n\r",ch);
    }

    return;
}

void do_mask( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBFUSCATE) < 2)
    {
	send_to_char("You require level 2 Obfuscate to use Mask of a Thousand Faces.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_EXTRA(ch, EXTRA_VICISSITUDE))
    {
	send_to_char( "You are unable to change your appearance.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You can only mask avatars or lower.\n\r", ch );
	return;
    }
    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	if (!IS_POLYAFF(ch, POLY_MASK))
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	sprintf(buf,"$n's form shimmers and transforms into a clone of %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->polyaff, POLY_MASK);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your form shimmers and transforms into a clone of $n.",ch,NULL,NULL,TO_CHAR);
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_MASK))
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	act("Your form shimmers and transforms into a clone of $N.",ch,NULL,victim,TO_CHAR);
	act("$n's form shimmers and transforms into a clone of you.",ch,NULL,victim,TO_VICT);
	act("$n's form shimmers and transforms into a clone of $N.",ch,NULL,victim,TO_NOTVICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	SET_BIT(ch->polyaff, POLY_MASK);
	return;
    }
    act("Your form shimmers and transforms into a clone of $N.",ch,NULL,victim,TO_CHAR);
    act("$n's form shimmers and transforms into a clone of you.",ch,NULL,victim,TO_VICT);
    act("$n's form shimmers and transforms into a clone of $N.",ch,NULL,victim,TO_NOTVICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->polyaff, POLY_MASK);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}

void do_change( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_PROTEAN) < 4)
    {
	send_to_char("You require level 4 Protean to Change Shape.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg,"bat") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	SET_BIT(ch->polyaff, POLY_BAT);
	clear_stats(ch);
	act( "You transform into bat form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a bat.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->vampaff, VAM_FLYING);
	SET_BIT(ch->vampaff, VAM_SONIC);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s the vampire bat", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"wolf") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	act( "You transform into wolf form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a dire wolf.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->polyaff, POLY_WOLF);
	clear_stats(ch);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	sprintf(buf, "%s the dire wolf", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"mist") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	act( "You transform into mist form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a white mist.", ch, NULL, NULL, TO_ROOM );
	if (IS_EXTRA(ch, TIED_UP))
	{
	    act("The ropes binding you fall through your ethereal form.",ch,NULL,NULL,TO_CHAR);
	    act("The ropes binding $n fall through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    REMOVE_BIT(ch->extra, TIED_UP);
	    REMOVE_BIT(ch->extra, GAGGED);
	    REMOVE_BIT(ch->extra, BLINDFOLDED);
	}
	if (is_affected(ch, gsn_web))
	{
	    act("The webbing entrapping $n falls through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The webbing entrapping you falls through your ethereal form.\n\r",ch);
	    affect_strip(ch, gsn_web);
	}
	if (IS_AFFECTED(ch, AFF_WEBBED))
	{
	    act("The webbing entrapping $n falls through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The webbing entrapping you falls through your ethereal form.\n\r",ch);
	    REMOVE_BIT(ch->affected_by, AFF_WEBBED);
	}
	if (is_affected(ch, gsn_tendrils))
	{
	    act("The tendrils of darkness entrapping $n fall through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The tendrils of darkness entrapping you fall through your ethereal form.\n\r",ch);
	    affect_strip(ch, gsn_tendrils);
	}
	if (IS_AFFECTED(ch, AFF_SHADOW))
	{
	    act("The tendrils of darkness entrapping $n fall through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The tendrils of darkness entrapping you fall through your ethereal form.\n\r",ch);
	    REMOVE_BIT(ch->affected_by, AFF_SHADOW);
	}
	if (is_affected(ch, gsn_jail))
	{
	    act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	    affect_strip(ch, gsn_jail);
	}
	if (IS_AFFECTED(ch, AFF_JAIL))
	{
	    act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	    REMOVE_BIT(ch->affected_by, AFF_JAIL);
	}
	SET_BIT(ch->polyaff, POLY_MIST);
	clear_stats(ch);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->affected_by, AFF_ETHEREAL);
	sprintf(buf, "%s the white mist", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"human") )
    {
	if (!IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You are already in human form.\n\r", ch );
	    return;
	}
	if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_BAT))
	{
	    REMOVE_BIT(ch->vampaff, VAM_FLYING);
	    REMOVE_BIT(ch->vampaff, VAM_SONIC);
	    REMOVE_BIT(ch->polyaff, POLY_BAT);
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_WOLF))
	    REMOVE_BIT(ch->polyaff, POLY_WOLF);
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_MIST))
	{
	    REMOVE_BIT(ch->polyaff, POLY_MIST);
	    REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	}
	else
	{
	    /* In case they try to change to human from a non-vamp form */
	    send_to_char( "You seem to be stuck in this form.\n\r", ch );
	    return;
	}
	act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_CHANGED);
	clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    else
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
    return;
}

void do_vampire( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_IMMUNE(ch, IMM_VAMPIRE))
    {
	send_to_char("You lower your defences.\n\r",ch);
	SET_BIT(ch->immune, IMM_VAMPIRE);
	return;
    }
    send_to_char("You raise your defences.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_VAMPIRE);
    return;
}

void do_shadowplane( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
    	if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	{
	    send_to_char("You fade into the Umbra.\n\r",ch);
	    act("$n slowly fades out of existance.",ch,NULL,NULL,TO_ROOM);
	    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    do_look(ch,"auto");
	    return;
    	}
    	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    	send_to_char("You fade back into the real world.\n\r",ch);
	act("$n slowly fades into existance.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
    {
    	act( "You toss $p to the ground and it vanishes into the real world.", ch, obj, NULL, TO_CHAR );
    	act( "$n tosses $p to the ground and it vanishes.", ch, obj, NULL, TO_ROOM );
	obj_from_char(obj);
	obj_to_room(obj,ch->in_room);
    }
    else
    {
    	act( "You toss $p to the ground and it vanishes into the Umbra.", ch, obj, NULL, TO_CHAR );
    	act( "$n tosses $p to the ground and it vanishes.", ch, obj, NULL, TO_ROOM );
	obj_from_char(obj);
	obj_to_room(obj,ch->in_room);
	SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
    }
    return;
}

void do_introduce( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_VAMPIRE(ch) && ch->vampgen > 0 && ch->vampgen < 14)
	do_tradition(ch,ch->lord);
    else if (IS_WEREWOLF(ch) && IS_HERO(ch) &&
	ch->vampgen > 0 && ch->vampgen < 5)
	do_bloodline(ch,ch->lord);
    else send_to_char("Huh?\n\r",ch);
    return;
}

void do_bloodline( CHAR_DATA *ch, char *argument )
{
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      buf  [MAX_STRING_LENGTH];
    char      buf2 [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if (IS_NPC(ch)) return;

    if (arg1 != '\0') arg1[0] = UPPER(arg1[0]);
    if (arg2 != '\0') arg2[0] = UPPER(arg2[0]);
    if (arg3 != '\0') arg3[0] = UPPER(arg3[0]);

    if (!str_cmp(arg1,"kavir")) strcpy(arg1,"KaVir");

    if ( strlen(ch->clan) < 2 && ch->vampgen != 1 )
    {
	strcpy(buf,"In the name of Gaia, I announce my Garou heritage.");
	do_say(ch,buf);
	sprintf(buf,"My name is %s, I am a Ronin of no tribe.", ch->name);
	do_say(ch,buf);
	return;
    }
    if ( strlen(ch->clan) > 16)
	strcpy(buf,"In the name of the Wyrm, I announce my Garou heritage.");
    else
	strcpy(buf,"In the name of Gaia, I announce my Garou heritage.");
    do_say(ch,buf);
    if (ch->vampgen != 1)
    {
	if      (ch->vampgen == 4) sprintf(buf2,"%s",arg3);
	else if (ch->vampgen == 3) sprintf(buf2,"%s",arg2);
	else if (ch->vampgen == 2) sprintf(buf2,"%s",arg1);
	if (ch->vampgen == 1)
	{
	    if ( strlen(ch->clan) > 16)
		sprintf(buf,"My name is %s, chosen Champion of the Wyrm.", ch->name);
	    else
		sprintf(buf,"My name is %s, chosen Champion of Gaia.", ch->name);
	}
	if (ch->vampgen == 2)
	    sprintf(buf,"My name is %s, Chieftain of the %s tribe, pup of %s.", ch->name,ch->clan,buf2);
	else
	    sprintf(buf,"My name is %s, of the %s tribe, pup of %s.", ch->name,ch->clan,buf2);
	do_say(ch,buf);
    }
    if ( arg3[0] != '\0' )
    {
	sprintf(buf,"My name is %s, of the %s tribe, pup of %s.",
	arg3,ch->clan,arg2);
	do_say(ch,buf);
    }
    if ( arg2[0] != '\0' )
    {
	if ( arg1[0] != '\0' )
	    sprintf(buf,"My name is %s, Chieftain of the %s tribe, pup of %s.",arg2,ch->clan,arg1);
	else
	    sprintf(buf,"My name is %s, of the %s tribe, pup of %s.",arg2,ch->clan,arg1);
	do_say(ch,buf);
    }
    if ( ch->vampgen == 1 )
    {
	if ( strlen(ch->clan) > 16)
	    sprintf(buf,"My name is %s, chosen Champion of the Wyrm.", ch->name);
	else
	    sprintf(buf,"My name is %s, chosen Champion of Gaia.", ch->name);
    }
    else
    {
	if ( strlen(ch->clan) > 16)
	    sprintf(buf,"My name is %s, chosen Champion of the Wyrm.", arg1);
	else
	    sprintf(buf,"My name is %s, chosen Champion of Gaia.", arg1);
    }
    do_say(ch,buf);
    return;
}

void do_tradition( CHAR_DATA *ch, char *argument )
{
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      arg4 [MAX_INPUT_LENGTH];
    char      arg5 [MAX_INPUT_LENGTH];
    char      arg6 [MAX_INPUT_LENGTH];
    char      buf  [MAX_STRING_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];
    char      buf3 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );
    argument = one_argument( argument, arg6 );

    if (IS_NPC(ch)) return;

    if (arg1 != '\0') arg1[0] = UPPER(arg1[0]);
    if (arg2 != '\0') arg2[0] = UPPER(arg2[0]);
    if (arg3 != '\0') arg3[0] = UPPER(arg3[0]);
    if (arg4 != '\0') arg4[0] = UPPER(arg4[0]);
    if (arg5 != '\0') arg5[0] = UPPER(arg5[0]);
    if (arg6 != '\0') arg6[0] = UPPER(arg6[0]);

    if (!str_cmp(arg1,"kavir")) strcpy(arg1,"KaVir");

    if (!IS_VAMPIRE(ch) || (ch->vampgen < 1) || (ch->vampgen > 7))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( !str_cmp(ch->clan,"") && ch->vampgen == 2 )
    {
	send_to_char( "Not until you have a clan.\n\r", ch );
	return;
    }

    if      (ch->vampgen == 7) {sprintf(buf2,"Seventh");sprintf(buf3,"Fledgling");}
    else if (ch->vampgen == 6) {sprintf(buf2,"Sixth");sprintf(buf3,"Neonate");}
    else if (ch->vampgen == 5) {sprintf(buf2,"Fifth");sprintf(buf3,"Ancilla");}
    else if (ch->vampgen == 4) {sprintf(buf2,"Fourth");sprintf(buf3,"Elder");}
    else if (ch->vampgen == 3) {sprintf(buf2,"Third");sprintf(buf3,"Methuselah");}
    else if (ch->vampgen == 2) {sprintf(buf2,"Second");sprintf(buf3,"Antediluvian");}
    if (ch->vampgen == 1)
	sprintf(buf,"As is the tradition, I recite the lineage of %s, Sire of all Kindred.",ch->name);
    else
	sprintf(buf,"As is the tradition, I recite the lineage of %s, %s of the %s Generation.",ch->name,buf3,buf2);
    do_say(ch,buf);
    if ( ch->vampgen != 1 )
    {
	if      (ch->vampgen == 7) sprintf(buf2,"%s",arg6);
	else if (ch->vampgen == 6) sprintf(buf2,"%s",arg5);
	else if (ch->vampgen == 5) sprintf(buf2,"%s",arg4);
	else if (ch->vampgen == 4) sprintf(buf2,"%s",arg3);
	else if (ch->vampgen == 3) sprintf(buf2,"%s",arg2);
	else if (ch->vampgen == 2) sprintf(buf2,"%s",arg1);

	if (ch->vampgen == 2)
	    sprintf(buf,"I am %s, founder of clan %s.  My sire is %s.", ch->name,ch->clan,buf2);
	else
	    sprintf(buf,"I am %s of clan %s.  My sire is %s.", ch->name,ch->clan,buf2);
	do_say(ch,buf);
    }
    if ( arg6[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg6,ch->clan,arg5);
	do_say(ch,buf);
    }
    if ( arg5[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg5,ch->clan,arg4);
	do_say(ch,buf);
    }
    if ( arg4[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg4,ch->clan,arg3);
	do_say(ch,buf);
    }
    if ( arg3[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg3,ch->clan,arg2);
	do_say(ch,buf);
    }
    if ( arg2[0] != '\0' )
    {
	if ( arg1[0] != '\0' )
	    sprintf(buf,"I am %s, founder of %s.  My sire is %s.",arg2,ch->clan,arg1);
	else
	    sprintf(buf,"I am %s of clan %s.  My sire is %s.",arg2,ch->clan,arg1);
	do_say(ch,buf);
    }
    if ( ch->vampgen == 1 )
	sprintf(buf,"I am %s, cursed by God to drink only blood and eat only ashes.", ch->name);
    else
	sprintf(buf,"I am %s.  All Kindred are my childer.",arg1);
    do_say(ch,buf);
    if ( ch->vampgen == 7 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg6,arg5,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 6 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg5,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 5 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 4 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg3,arg2,arg1);
    if ( ch->vampgen == 3 ) sprintf(buf,"My name is %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg2,arg1);
    if ( ch->vampgen == 2 ) sprintf(buf,"My name is %s, childe of %s.  Recognize my lineage.",ch->name,arg1);
    if ( ch->vampgen == 1 ) sprintf(buf,"My name is %s.  Recognize my lineage.",ch->name);
    do_say(ch,buf);
    return;
}

void do_darkheart( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_disc(ch,DISC_SERPENTIS) < 4)
    {
	send_to_char("You require level 4 Serpentis to use Dark Heart.\n\r",ch);
	return;
    }
    if (IS_IMMUNE(ch,IMM_STAKE) )
    {
	send_to_char("But you've already torn your heart out!\n\r",ch);
	return;
    }
    send_to_char("You rip your heart from your body and toss it to the ground.\n\r",ch);
    act("$n rips $s heart out and tosses it to the ground.", ch, NULL, NULL, TO_ROOM);
    make_part( ch, "heart" );
    if (!IS_VAMPIRE(ch)) ch->hit = -10;
    else ch->hit = ch->hit - number_range(10,20);
    update_pos(ch);
    if (!IS_VAMPIRE(ch) || (ch->position == POS_DEAD && !IS_HERO(ch)))
    {
	ch->form = 2;
	killperson(ch,ch);
	return;
    }
    SET_BIT(ch->immune, IMM_STAKE);
    return;
}

void do_truesight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_WEREWOLF(ch) || (get_breed(ch, BREED_LUPUS) < 1 && get_disc(ch,DISC_AUSPEX) < 1))
    {
	if (IS_WEREWOLF(ch))
	{
	    send_to_char("You require the level 1 Lupis power to use Heightened Senses.\n\r",ch);
	    return;
	}
        if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_HIGHLANDER(ch) && !IS_DEMON(ch))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
        }
	if (get_disc(ch,DISC_AUSPEX) < 1 && (IS_VAMPIRE(ch) || IS_GHOUL(ch) || IS_DEMON(ch)))
	{
	    if ( IS_DEMON(ch) )
		send_to_char("You require Basic Vision to use Heightened Senses.\n\r",ch);
	    else
		send_to_char("You require level 1 Auspex to use Heightened Senses.\n\r",ch);
	    return;
	}
    }
  
    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses return to normal.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses increase to incredible proportions.\n\r", ch );
    }
    return;
}

void do_soulmask( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int value, max_stat;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if ( IS_NPC(ch) )
	return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_OBFUSCATE) < 5)
    {
	send_to_char("You require level 5 Obfuscate to use Soul Mask.\n\r",ch);
	return;
    }

    if (arg1[0] != '\0' && !str_cmp(arg1,"on"))
    {
	if (IS_EXTRA(ch, EXTRA_FAKE_CON))
	{send_to_char("You already have Soul Mask on.\n\r",ch); return;}
	SET_BIT(ch->extra, EXTRA_FAKE_CON);
	send_to_char("Your Soul Mask is now ON.\n\r",ch);
	return;
    }
    if (arg1[0] != '\0' && !str_cmp(arg1,"off"))
    {
	if (!IS_EXTRA(ch, EXTRA_FAKE_CON))
	{send_to_char("You already have Soul Mask off.\n\r",ch); return;}
	REMOVE_BIT(ch->extra, EXTRA_FAKE_CON);
	send_to_char("Your Soul Mask is now OFF.\n\r",ch);
	return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                -= Soul Mask =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("You have the following stats:\n\r",ch);
	sprintf(buf,"Weapon skill: %d, Stance: %d.\n\r",
		ch->pcdata->fake_skill, ch->pcdata->fake_stance);
	send_to_char(buf, ch);
	sprintf(buf,"Hitroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_hit, char_hitroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Damroll: %d, Actual: %d.\n\r",
		ch->pcdata->fake_dam, char_damroll(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Armour: %d, Actual: %d.\n\r",
		ch->pcdata->fake_ac, char_ac(ch));
	send_to_char(buf, ch);
	sprintf(buf,"Hp: %d/%d, Actual: %d/%d.\n\r",
		ch->pcdata->fake_hp, ch->pcdata->fake_max_hp, 
		ch->hit, ch->max_hit);
	send_to_char(buf, ch);
	sprintf(buf,"Vitality: %d/%d, Actual: %d/%d.\n\r",
		ch->pcdata->fake_move, ch->pcdata->fake_max_move, 
		ch->move, ch->max_move);
	send_to_char(buf, ch);
	sprintf(buf,"Str: %d/%d, Int: %d/%d, Wis: %d/%d, Dex: %d/%d, Con: %d/%d.\n\r",
		ch->pcdata->fake_stats[STAT_STR], get_curr_str(ch),
		ch->pcdata->fake_stats[STAT_INT], get_curr_int(ch),
		ch->pcdata->fake_stats[STAT_WIS], get_curr_wis(ch),
		ch->pcdata->fake_stats[STAT_DEX], get_curr_dex(ch),
		ch->pcdata->fake_stats[STAT_CON], get_curr_con(ch));
	send_to_char(buf, ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("You can set: Skill, Stance, Str, Int, Wis, Dex, Con, Hit, Dam, Ac,\n\r",ch);
	send_to_char("Hp, Max_Hp, Vitality, Max_Vitality.\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }
    value = is_number( arg2 ) ? atoi( arg2 ) : -10000;

    switch (ch->vampgen)
    {
	default: max_stat = 5;  break;
	case 1:  max_stat = 10; break;
	case 2:
	case 3:
	case 4:  max_stat = 9;  break;
	case 5:  max_stat = 8;  break;
	case 6:  max_stat = 7;  break;
	case 7:  max_stat = 6;  break;
    }

    if (!str_cmp(arg1,"skill"))
    {
	if (value < 0 || value > 200)
	{
	    send_to_char("Please enter a value between 0 and 200.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_skill = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"stance"))
    {
	if (value < 0 || value > 200)
	{
	    send_to_char("Please enter a value between 0 and 200.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_stance = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"hit") || !str_cmp(arg1,"hitroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hit = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"dam") || !str_cmp(arg1,"damroll"))
    {
	if (value < 0 || value > 1000)
	{
	    send_to_char("Please enter a value between 0 and 1000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_dam = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"ac") || !str_cmp(arg1,"armour") || !str_cmp(arg1,"armor"))
    {
	send_to_char("Please use ac+ or ac- to choose a positive or negative value.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"ac+") || !str_cmp(arg1,"armour+") || !str_cmp(arg1,"armor+"))
    {
	if (value < 0 || value > 100)
	{
	    send_to_char("Please enter a value between 0 and 100.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_ac = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"ac-") || !str_cmp(arg1,"armour-") || !str_cmp(arg1,"armor-"))
    {
	if (value < 1 || value > 2000)
	{
	    send_to_char("Please enter a value between 1 and 2000.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_ac = 0 - value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"hp") || !str_cmp(arg1,"hits"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	if (value > ch->pcdata->fake_max_hp)
	{
	    send_to_char("You cannot have more hp than your max_hp.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_hp = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"max_hp") || !str_cmp(arg1,"max_hits"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	if (value < ch->pcdata->fake_hp)
	{
	    send_to_char("You cannot have less max_hp than your hp.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_max_hp = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"vitality"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	if (value > ch->pcdata->fake_max_move)
	{
	    send_to_char("You cannot have more vitality than your max_vatality.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_move = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"max_vitality"))
    {
	if (value < 1 || value > 30000)
	{
	    send_to_char("Please enter a value between 1 and 30000.\n\r",ch);
	    return;
	}
	if (value < ch->pcdata->fake_max_move)
	{
	    send_to_char("You cannot have less max_vitality than your vatality.\n\r",ch);
	    return;
	}
	ch->pcdata->fake_max_move = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"str") || !str_cmp(arg1,"strength"))
    {
	if (value < 3 || value > max_stat)
	{
	    sprintf(buf, "Please enter a value between 3 and %d.\n\r", max_stat);
	    send_to_char(buf, ch);
	    return;
	}
	ch->pcdata->fake_stats[STAT_STR] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"int") || !str_cmp(arg1,"intelligence"))
    {
	if (value < 3 || value > max_stat)
	{
	    sprintf(buf, "Please enter a value between 3 and %d.\n\r", max_stat);
	    send_to_char(buf, ch);
	    return;
	}
	ch->pcdata->fake_stats[STAT_INT] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"wis") || !str_cmp(arg1,"wisdom"))
    {
	if (value < 3 || value > max_stat)
	{
	    sprintf(buf, "Please enter a value between 3 and %d.\n\r", max_stat);
	    send_to_char(buf, ch);
	    return;
	}
	ch->pcdata->fake_stats[STAT_WIS] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"dex") || !str_cmp(arg1,"dexterity"))
    {
	if (value < 3 || value > max_stat)
	{
	    sprintf(buf, "Please enter a value between 3 and %d.\n\r", max_stat);
	    send_to_char(buf, ch);
	    return;
	}
	ch->pcdata->fake_stats[STAT_DEX] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"con") || !str_cmp(arg1,"constitution"))
    {
	if (value < 3 || value > max_stat)
	{
	    sprintf(buf, "Please enter a value between 3 and %d.\n\r", max_stat);
	    send_to_char(buf, ch);
	    return;
	}
	ch->pcdata->fake_stats[STAT_CON] = value;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    send_to_char("You can set: Skill, Stance, Str, Int, Wis, Dex, Con, Hit, Dam, Ac,\n\r",ch);
    send_to_char("Hp, Max_Hp, Vitality, Max_Vitality.\n\r",ch);
    return;
}

void do_scry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *victimroom;
    DESCRIPTOR_DATA *d;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    bool      found = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_DEMON(ch))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_SCRY))
	{
	    send_to_char("You haven't been granted the gift of scry.\n\r",ch);
	    return;
	}
    }
    else if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ITEMAFF(ch, ITEMA_VISION) && !IS_WEREWOLF(ch) && !IS_DEMON(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    else if ( get_auspice(ch, AUSPICE_PHILODOX) < 3 && 
	get_disc(ch, DISC_AUSPEX) < 5 && !IS_ITEMAFF(ch, ITEMA_VISION) )
    {
	if ( IS_ABOMINATION(ch) )
	    send_to_char("You require level 3 Philodox or level 5 Auspex to Scry.\n\r",ch);
	else if ( IS_WEREWOLF(ch) )
	    send_to_char("You require the level 3 Philodox power to use Scent of Beyond.\n\r",ch);
	else if ( IS_DEMON(ch) )
	    send_to_char("You require Supreme Vision to use Scry.\n\r",ch);
	else
	    send_to_char("You require level 5 Auspex to use Scry.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("You close your eyes and mentally survey the surrounding area.\n\r",ch);
	act("$n closes $s eyes and a look of concentration crosses $s face.",ch,NULL,NULL,TO_ROOM);
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected != CON_PLAYING ) continue;
	    if ( (victim = d->character) == NULL ) continue;
	    if ( IS_NPC(victim) || victim->in_room == NULL ) continue;
	    if ( victim == ch ) continue;
	    if ( !can_see(ch,victim) ) continue;
	    if ( IS_AFFECTED(victim, AFF_POLYMORPH) )
		sprintf(buf,"%s: ", victim->morph);
	    else
		sprintf(buf,"%s: ", victim->name);
	    send_to_char(buf,ch);
	    if (!IS_IMMUNE(victim,IMM_SHIELDED) )
		sprintf(buf,"%s.\n\r", victim->in_room->name);
	    else
		sprintf(buf,"(shielded).\n\r");
	    send_to_char(buf,ch);
	    found = TRUE;
	}
	if (!found) send_to_char("You cannot seem to locate anyone.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "You are unable to locate them.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && IS_IMMUNE(victim,IMM_SHIELDED) && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char("You are unable to locate them.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Scry on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    chroom = ch->in_room;
    victimroom = victim->in_room;

    char_from_room(ch);
    char_to_room(ch,victimroom);
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else
	do_look(ch,"auto");
    char_from_room(ch);
    char_to_room(ch,chroom);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Scry on you.",ch,NULL,victim,TO_VICT);
    }

    return;
}

void do_readaura( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       con_hit, con_dam, con_ac, con_hp, con_mana, con_move;
    int       con_str, con_int, con_dex, con_wis, con_con;
    int       con_max_hp, con_max_mana, con_max_move;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_DEMON(ch) && 
	!IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_AUSPEX) < 2 && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	if ( IS_DEMON(ch) )
	    send_to_char("You require Intermediate Vision to Read somethings Aura.\n\r",ch);
	else
	    send_to_char("You require level 2 Auspex to Read somethings Aura.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Read the aura on what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "Read the aura on what?\n\r", ch );
	    return;
	}
	spell_identify( skill_lookup( "identify" ), ch->level, ch, obj );
	return;
    }

    if (!IS_NPC(victim) && IS_IMMUNE(victim,IMM_SHIELDED) && !IS_ITEMAFF(ch, ITEMA_VISION))
    {
	send_to_char("You are unable to read their aura.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Read Aura on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("$n examines $N intently.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you intently.",ch,NULL,victim,TO_VICT);

    if (!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_FAKE_CON))
    {
	con_hit = victim->pcdata->fake_hit;
	con_dam = victim->pcdata->fake_dam;
	con_ac = victim->pcdata->fake_ac;
	con_str = victim->pcdata->fake_stats[STAT_STR];
	con_int = victim->pcdata->fake_stats[STAT_INT];
	con_wis = victim->pcdata->fake_stats[STAT_WIS];
	con_dex = victim->pcdata->fake_stats[STAT_DEX];
	con_con = victim->pcdata->fake_stats[STAT_CON];
	con_hp = victim->pcdata->fake_hp;
	con_mana = victim->pcdata->fake_mana;
	con_move = victim->pcdata->fake_move;
	con_max_hp = victim->pcdata->fake_max_hp;
	con_max_mana = victim->pcdata->fake_max_mana;
	con_max_move = victim->pcdata->fake_max_move;
    }
    else
    {
	con_hit = char_hitroll(victim);
	con_dam = char_damroll(victim);
	con_ac = char_ac(victim);
	con_str = get_curr_str(victim);
	con_int = get_curr_int(victim);
	con_wis = get_curr_wis(victim);
	con_dex = get_curr_dex(victim);
	con_con = get_curr_con(victim);
	con_hp = victim->hit;
	con_mana = victim->mana;
	con_move = victim->move;
	con_max_hp = victim->max_hit;
	con_max_mana = victim->max_mana;
	con_max_move = victim->max_move;
    }

    if (IS_NPC(victim)) sprintf(buf, "%s is an NPC.\n\r",victim->short_descr);
    else 
    {
	if      (victim->level == 12) sprintf(buf, "%s is an Implementor.\n\r", victim->name);
	else if (victim->level == 11) sprintf(buf, "%s is a High Judge.\n\r", victim->name);
	else if (victim->level == 10) sprintf(buf, "%s is a Judge.\n\r", victim->name);
	else if (victim->level == 9 ) sprintf(buf, "%s is an Enforcer.\n\r", victim->name);
	else if (victim->level == 8 ) sprintf(buf, "%s is a Quest Maker.\n\r", victim->name);
	else if (victim->level == 7 ) sprintf(buf, "%s is a Builder.\n\r", victim->name);
	else if (victim->level >= 3 ) sprintf(buf, "%s is an Avatar.\n\r", victim->name);
	else sprintf(buf, "%s is a Mortal.\n\r", victim->name);
    }
    send_to_char(buf,ch);
    sprintf( buf, "Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\n\r",
	con_str, con_int, con_wis, con_dex, con_con);
    send_to_char( buf, ch );
    if (IS_MAGE(victim))
    {
	sprintf(buf,"Hp: %d/%d, Mana: %d/%d, Vitality: %d/%d.\n\r",
	    con_hp, con_max_hp, con_mana, con_max_mana, con_move, con_max_move);
    }
    else
    {
	sprintf(buf,"Hp: %d/%d, Vitality: %d/%d.\n\r",
	    con_hp, con_max_hp, con_move, con_max_move);
    }
    send_to_char(buf,ch);
    sprintf(buf,"Hitroll: %d, Damroll: %d, AC: %d.\n\r", 
	con_hit, con_dam, con_ac);
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Status: %d, ",victim->race);
	send_to_char(buf,ch);
    }
    sprintf(buf,"Alignment: %d.\n\r",victim->alignment);
    send_to_char(buf,ch);
    if (!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_PREGNANT))
	act("$N is pregnant.",ch,NULL,victim,TO_CHAR);
    if (!IS_NPC(victim) && IS_VAMPIRE(victim) && victim->pcdata->diableries[DIAB_EVER] >= 5)
	act("$N's soul is permanently stained with multiple diablerie.",ch,NULL,victim,TO_CHAR);
    else if (!IS_NPC(victim) && IS_VAMPIRE(victim) && victim->pcdata->diableries[DIAB_TIME] > 0)
	act("$N has recently committed diablerie.",ch,NULL,victim,TO_CHAR);
    if (!IS_NPC(victim) && IS_MORE(victim, MORE_INFORM))
    {
	if (ch->pcdata->lie == LIE_TRUE)
	    act("$N has recently lied.",ch,NULL,victim,TO_CHAR);
	else
	    act("$N has recently spoken truthfully.",ch,NULL,victim,TO_CHAR);
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Read Aura on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_mortalvamp( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    int       disc;
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_VAMPIRE(ch) || IS_GHOUL(ch))
    {
	for ( disc = DISC_ANIMALISM; disc <= DISC_VICISSITUDE; disc++ )
	    ch->pcdata->powers_mod[disc] += 10;
	if (IS_EXTRA(ch, EXTRA_NO_ACTION))
	{
	    if (IS_MORE(ch, MORE_PROJECTION)) do_projection(ch,"");
	}
    	if (IS_VAMPAFF(ch,VAM_EARTHMELDED) ) do_earthmeld(ch,"");
	if (IS_MORE(ch, MORE_MUMMIFIED)) do_mummify(ch,"");
	if (IS_MORE(ch, MORE_PLASMA)) do_plasmaform(ch,"");
	if (IS_MORE(ch, MORE_ASHES)) do_ashes(ch,"");
	if (IS_EXTRA(ch, EXTRA_AWE)) do_awe(ch,"");
	if (IS_MORE(ch, MORE_GUARDIAN)) do_spiritguardian(ch,"");
	if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");
	if (get_disc(ch,DISC_AUSPEX) >= 3) do_unveil(ch,"self");
    	if (IS_POLYAFF(ch,POLY_ZULO) ) do_zuloform(ch,"");
    	if (IS_POLYAFF(ch,POLY_MASK) ) do_mask(ch,"self");
    	if (IS_POLYAFF(ch,POLY_TRUE) ) do_truedisguise(ch,"self");
    	if (IS_POLYAFF(ch,POLY_HAGS) ) do_mask(ch,"self");
    	if (IS_POLYAFF(ch,POLY_CHANGELING) ) do_mask(ch,"self");
    	if (IS_POLYAFF(ch,POLY_FLESHCRAFT) ) do_mask(ch,"self");
    	if (IS_POLYAFF(ch,POLY_BONECRAFT) ) do_mask(ch,"self");
    	if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    	if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
    	if (IS_POLYAFF(ch,POLY_SHADOW) ) do_shadowbody(ch,"");
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	{
	    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	    REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	    REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
	    REMOVE_BIT(ch->polyaff, POLY_SPIRIT);
	    free_string( ch->morph );
	    ch->morph = str_dup( "" );
	    free_string( ch->long_descr );
	    ch->morph = str_dup( "" );
	}
    	if (IS_IMMUNE(ch,IMM_SHIELDED) ) do_shield(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) ) do_shadowplane(ch,"");
    	if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
	if (IS_MORE(ch, MORE_HAND_FLAME)) do_hand(ch,"");
    	if (IS_VAMPAFF(ch,VAM_SPIRITEYES) ) do_spiriteyes(ch,"");
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) ) do_shadowsight(ch,"");
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) ) do_truesight(ch,"");
    	if (IS_VAMPAFF(ch,VAM_OBFUSCATE) ) do_vanish(ch,"");
	if (IS_MORE(ch, MORE_SILENCE)) do_silencedeath(ch,"");
	if (IS_MORE(ch, MORE_DARKNESS)) do_shroudnight(ch,"");
	if (IS_MORE(ch, MORE_REINA)) do_reina(ch,"");
	if (IS_MORE(ch, MORE_OBEAH)) {ch->pcdata->obeah = 0; do_obeah(ch,"");}
	if (IS_MORE(ch, MORE_NEUTRAL)) do_neutral(ch,"");
	ch->pcdata->wolf = 0;
	for ( disc = DISC_ANIMALISM; disc <= DISC_VICISSITUDE; disc++ )
	    ch->pcdata->powers_mod[disc] -= 10;
    }
    return;
}

void do_shield( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (get_disc(ch,DISC_OBFUSCATE) < 4 && get_tribe(ch, TRIBE_SILVER_FANGS) < 4)
    {
	if ( IS_ABOMINATION(ch) )
	    send_to_char("You require level 4 Obfuscate or Silver Fangs to Shield your aura.\n\r",ch);
	else if ( IS_WEREWOLF(ch) )
	    send_to_char("You require level 4 Silver Fangs to Shield your aura.\n\r",ch);
	else 
	    send_to_char("You require level 4 Obfuscate to Shield your aura.\n\r",ch);
	return;
    }

    if (!IS_IMMUNE(ch,IMM_SHIELDED) )
    {
    	send_to_char("You shield your aura from those around you.\n\r",ch);
    	SET_BIT(ch->immune, IMM_SHIELDED);
	return;
    }
    send_to_char("You stop shielding your aura.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_SHIELDED);
    return;
}

void do_serpent( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH]; 

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_SERPENTIS) < 3)
    {
	send_to_char("You require level 3 Serpentis to transform into Serpent Form.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH))
    {
	if (!IS_POLYAFF(ch,POLY_SERPENT))
	{
	    send_to_char( "You cannot polymorph from this form.\n\r", ch );
	    return;
	}
	act( "You transform back into human.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->polyaff, POLY_SERPENT);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	clear_stats(ch);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	return;
    }
    SET_BIT(ch->polyaff, POLY_SERPENT);
    clear_stats(ch);
    if (ch->stance[0] != -1) do_stance(ch,"");
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    act( "You transform into a huge serpent.", ch, NULL, NULL, TO_CHAR );
    act( "$n transforms into a huge serpent.", ch, NULL, NULL, TO_ROOM );
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf, "%s the huge serpent", ch->name);
    free_string( ch->morph );
    ch->morph = str_dup( buf );
    return;
}

void poison_weapon( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( obj->item_type != ITEM_WEAPON )
    {
	send_to_char( "You can only poison weapons.\n\r", ch );
	return;
    }
    if ( obj->value[0] != 0 )
    {
	send_to_char( "This weapon cannot be poisoned.\n\r", ch );
	return;
    }
    act("You run your long serpentine tongue along $p, poisoning it.",ch,obj,NULL,TO_CHAR);
    act("$n runs $s long serpentine tongue along $p, poisoning it.",ch,obj,NULL,TO_ROOM);
    obj->value[0] = 53;
    return;
}

void do_regenerate( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    int        regen;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->pcdata->wolf > 0 && !IS_ABOMINATION(ch))
    {
	send_to_char("You cannot control your regenerative powers while the beast is so strong.\n\r",ch);
	return;
    }

    if ( ch->pcdata->regenerate > 0 )
    {
	send_to_char("You stop your body regenerating.\n\r",ch);
	ch->pcdata->regenerate = 0;
	return;
    }

    if (ch->blood[BLOOD_CURRENT] < 10)
    {
	send_to_char("You have insufficient blood to regenerate.\n\r",ch);
	return;
    }

    if (ch->hit >= ch->max_hit && ch->move >= ch->max_move)
    {
	send_to_char("Your body doesn't require regeneration.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' ) regen = 1;
    else if ( ( regen = is_number(arg) ? atoi(arg) : 1 ) < 1 || regen > 10 )
    {
	send_to_char("Please enter a number between 1 and 10.\n\r",ch);
	return;
    }

    ch->pcdata->regenerate = regen;
    send_to_char("You begin regenerating yourself.\n\r",ch);
    return;
}

void werewolf_regen( CHAR_DATA *ch )
{
    bool mesg = TRUE;
    int regen_hp, regen_max;

    if (IS_NPC(ch)) return;

    if (ch->hit < 1 ) 
    {
	ch->hit += number_range(1,3);
	update_pos(ch);
    }
    else
    {
	if (ch->hit >= ch->max_hit) 
	{
	    mesg = FALSE;
	    ch->move = UMIN(ch->move + (ch->max_move * 0.05), ch->max_move);
	    ch->mana = UMIN(ch->mana + (ch->max_mana * 0.05), ch->max_mana);
	}
	else if (ch->max_hit > 0)
	{
	    regen_hp = ch->max_hit * 0.05;
	    if (ch->agg < 0) ch->agg = 0; else if (ch->agg > 100) ch->agg = 100;

	    if (ch->agg > 0 && (ch->hit + regen_hp) >= (ch->max_hit * ((100 - ch->agg) * 0.01)))
	    {
		if ((ch->hit + 1) < (ch->max_hit * ((100 - ch->agg) * 0.01)))
		{
		    ch->hit = ch->max_hit * ((100 - ch->agg) * 0.01);
		    send_to_char("Your body has regenerated all non-aggravated damage.\n\r",ch);
		}
		else if (IS_VAMPIRE(ch))
		{
		    if (ch->position == POS_FIGHTING)
		    {
			send_to_char("You are unable to heal aggravated wounds while fighting.\n\r",ch);
			ch->pcdata->regenerate = 0;
			return;
		    }
		    else if (ch->pcdata->willpower[0] < 1)
		    {
			send_to_char("You have insufficient willpower to heal your aggravated wounds.\n\r",ch);
			ch->pcdata->regenerate = 0;
			return;
		    }
		    ch->pcdata->willpower[0] -= 1;
		    ch->agg -= 10;
		    if (ch->agg < 0) ch->agg = 0;
		    if (ch->agg >= 100) ch->hit = 1;
		    else 
		    {
			regen_max = ch->max_hit * ((100 - ch->agg) * 0.01);
			regen_hp = ch->max_hit * 0.05;
			if (regen_hp > regen_max) regen_hp = regen_max;
			ch->hit += regen_hp;
		    }
		}
		else
		{
		    if (ch->position > POS_RESTING) return;
		    else if (ch->pcdata->willpower[0] < 1)
		    {
			send_to_char("You have insufficient willpower to heal your aggravated wounds.\n\r",ch);
			return;
		    }
		    ch->pcdata->willpower[0] -= 1;
		    ch->agg -= 10;
		    if (ch->agg < 0) ch->agg = 0;
		    if (ch->agg >= 100) ch->hit = 1;
		    else 
		    {
			regen_max = ch->max_hit * ((100 - ch->agg) * 0.01);
			regen_hp = ch->max_hit * 0.05;
			if (regen_hp > regen_max) regen_hp = regen_max;
			ch->hit += regen_hp;
		    }
		}
	    }
	    else ch->hit += ch->max_hit * 0.05;
	}
	ch->move = UMIN(ch->move + (ch->max_move * 0.05), ch->max_move);

	/* Still not sure if mana should regenerate so fast... */
	ch->mana = UMIN(ch->mana + (ch->max_mana * 0.05), ch->max_mana);

	if ( ch->hit >= ch->max_hit && 
	    ch->mana >= ch->max_mana && ch->move >= ch->max_move )
	{
	    ch->hit = ch->max_hit;
	    ch->mana = ch->max_mana;
	    ch->move = ch->max_move;
	    if ( mesg )
		send_to_char("Your body has completely regenerated itself.\n\r",ch);
	    if (IS_VAMPIRE(ch)) ch->pcdata->regenerate = 0;
	}
    }
    return;
}

void reg_mend( CHAR_DATA *ch )
{
    int ribs = 0;
    int teeth = 0;

    if (IS_BODY(ch,BROKEN_RIBS_1 )) ribs += 1;
    if (IS_BODY(ch,BROKEN_RIBS_2 )) ribs += 2;
    if (IS_BODY(ch,BROKEN_RIBS_4 )) ribs += 4;
    if (IS_BODY(ch,BROKEN_RIBS_8 )) ribs += 8;
    if (IS_BODY(ch,BROKEN_RIBS_16)) ribs += 16;
    if (IS_HEAD(ch,LOST_TOOTH_1  )) teeth += 1;
    if (IS_HEAD(ch,LOST_TOOTH_2  )) teeth += 2;
    if (IS_HEAD(ch,LOST_TOOTH_4  )) teeth += 4;
    if (IS_HEAD(ch,LOST_TOOTH_8  )) teeth += 8;
    if (IS_HEAD(ch,LOST_TOOTH_16 )) teeth += 16;

    if (ribs > 0)
    {
    	if (IS_BODY(ch,BROKEN_RIBS_1 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_1);
    	if (IS_BODY(ch,BROKEN_RIBS_2 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_2);
	if (IS_BODY(ch,BROKEN_RIBS_4 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_4);
	if (IS_BODY(ch,BROKEN_RIBS_8 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_8);
	if (IS_BODY(ch,BROKEN_RIBS_16))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_16);
	ribs -= 1;
	if (ribs >= 16) {ribs -= 16;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_16);}
	if (ribs >= 8 ) {ribs -= 8;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_8);}
	if (ribs >= 4 ) {ribs -= 4;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_4);}
	if (ribs >= 2 ) {ribs -= 2;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_2);}
	if (ribs >= 1 ) {ribs -= 1;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_1);}
	act("One of $n's ribs snap back into place.",ch,NULL,NULL,TO_ROOM);
	act("One of your ribs snap back into place.",ch,NULL,NULL,TO_CHAR);
    }
    else if (IS_HEAD(ch,LOST_EYE_L))
    {
	act("An eyeball appears in $n's left eye socket.",ch,NULL,NULL,TO_ROOM);
	act("An eyeball appears in your left eye socket.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EYE_L);
    }
    else if (IS_HEAD(ch,LOST_EYE_R))
    {
	act("An eyeball appears in $n's right eye socket.",ch,NULL,NULL,TO_ROOM);
	act("An eyeball appears in your right eye socket.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EYE_R);
    }
    else if (IS_HEAD(ch,LOST_EAR_L))
    {
	act("An ear grows on the left side of $n's head.",ch,NULL,NULL,TO_ROOM);
	act("An ear grows on the left side of your head.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EAR_L);
    }
    else if (IS_HEAD(ch,LOST_EAR_R))
    {
	act("An ear grows on the right side of $n's head.",ch,NULL,NULL,TO_ROOM);
	act("An ear grows on the right side of your head.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_EAR_R);
    }
    else if (IS_HEAD(ch,LOST_NOSE))
    {
	act("A nose grows on the front of $n's face.",ch,NULL,NULL,TO_ROOM);
	act("A nose grows on the front of your face.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_NOSE);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (teeth > 0)
    {
    	if (IS_HEAD(ch,LOST_TOOTH_1 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_1);
    	if (IS_HEAD(ch,LOST_TOOTH_2 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_2);
    	if (IS_HEAD(ch,LOST_TOOTH_4 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_4);
    	if (IS_HEAD(ch,LOST_TOOTH_8 ))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_8);
    	if (IS_HEAD(ch,LOST_TOOTH_16))
	    REMOVE_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_16);
	teeth -= 1;
	if (teeth >= 16) {teeth -= 16;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_16);}
	if (teeth >= 8 ) {teeth -= 8;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_8);}
	if (teeth >= 4 ) {teeth -= 4;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_4);}
	if (teeth >= 2 ) {teeth -= 2;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_2);}
	if (teeth >= 1 ) {teeth -= 1;
	    SET_BIT(ch->loc_hp[LOC_HEAD],LOST_TOOTH_1);}
	act("A missing tooth grows in your mouth.",ch,NULL,NULL,TO_CHAR);
	act("A missing tooth grows in $n's mouth.",ch,NULL,NULL,TO_ROOM);
    }
    else if (IS_HEAD(ch,BROKEN_NOSE))
    {
	act("$n's nose snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your nose snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (IS_HEAD(ch,BROKEN_JAW))
    {
	act("$n's jaw snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your jaw snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_JAW);
    }
    else if (IS_HEAD(ch,BROKEN_SKULL))
    {
	act("$n's skull knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your skull knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_SKULL);
    }
    else if (IS_BODY(ch,BROKEN_SPINE))
    {
	act("$n's spine knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your spine knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_SPINE);
    }
    else if (IS_BODY(ch,BROKEN_NECK))
    {
	act("$n's neck snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your neck snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_NECK);
    }
    else if (IS_ARM_L(ch,LOST_ARM))
    {
	act("An arm grows from the stump of $n's left shoulder.",ch,NULL,NULL,TO_ROOM);
	act("An arm grows from the stump of your left shoulder.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_ARM);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_ARM);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_HAND);
    }
    else if (IS_ARM_R(ch,LOST_ARM))
    {
	act("An arm grows from the stump of $n's right shoulder.",ch,NULL,NULL,TO_ROOM);
	act("An arm grows from the stump of your right shoulder.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_ARM);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_ARM);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_HAND);
    }
    else if (IS_LEG_L(ch,LOST_LEG))
    {
	act("A leg grows from the stump of $n's left hip.",ch,NULL,NULL,TO_ROOM);
	act("A leg grows from the stump of your left hip.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],LOST_LEG);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],BROKEN_LEG);
	SET_BIT(ch->loc_hp[LOC_LEG_L],LOST_FOOT);
    }
    else if (IS_LEG_R(ch,LOST_LEG))
    {
	act("A leg grows from the stump of $n's right hip.",ch,NULL,NULL,TO_ROOM);
	act("A leg grows from the stump of your right hip.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],LOST_LEG);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],BROKEN_LEG);
	SET_BIT(ch->loc_hp[LOC_LEG_R],LOST_FOOT);
    }
    else if (IS_ARM_L(ch,BROKEN_ARM))
    {
	act("$n's left arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_ARM);
    }
    else if (IS_ARM_R(ch,BROKEN_ARM))
    {
	act("$n's right arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_ARM);
    }
    else if (IS_LEG_L(ch,BROKEN_LEG))
    {
	act("$n's left leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],BROKEN_LEG);
    }
    else if (IS_LEG_R(ch,BROKEN_LEG))
    {
	act("$n's right leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],BROKEN_LEG);
    }
    else if (IS_ARM_L(ch,LOST_HAND))
    {
	act("A hand grows from the stump of $n's left wrist.",ch,NULL,NULL,TO_ROOM);
	act("A hand grows from the stump of your left wrist.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_HAND);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_THUMB);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_I);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_M);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_R);
	SET_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_L);
    }
    else if (IS_ARM_R(ch,LOST_HAND))
    {
	act("A hand grows from the stump of $n's right wrist.",ch,NULL,NULL,TO_ROOM);
	act("A hand grows from the stump of your right wrist.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_HAND);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_THUMB);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_I);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_M);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_R);
	SET_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_L);
    }
    else if (IS_LEG_L(ch,LOST_FOOT))
    {
	act("A foot grows from the stump of $n's left ankle.",ch,NULL,NULL,TO_ROOM);
	act("A foot grows from the stump of your left ankle.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],LOST_FOOT);
    }
    else if (IS_LEG_R(ch,LOST_FOOT))
    {
	act("A foot grows from the stump of $n's right ankle.",ch,NULL,NULL,TO_ROOM);
	act("A foot grows from the stump of your right ankle.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],LOST_FOOT);
    }
    else if (IS_ARM_L(ch,LOST_THUMB))
    {
	act("A thumb slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A thumb slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_THUMB);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_THUMB);
    }
    else if (IS_ARM_L(ch,BROKEN_THUMB))
    {
	act("$n's left thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_THUMB);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_I))
    {
	act("An index finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("An index finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_I);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_I))
    {
	act("$n's left index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_M))
    {
	act("A middle finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A middle finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_M);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_M))
    {
	act("$n's left middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_R))
    {
	act("A ring finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A ring finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_R);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_R))
    {
	act("$n's left ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);
    }
    else if (IS_ARM_L(ch,LOST_FINGER_L))
    {
	act("A little finger slides out of $n's left hand.",ch,NULL,NULL,TO_ROOM);
	act("A little finger slides out of your left hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],LOST_FINGER_L);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_L))
    {
	act("$n's left little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);
    }
    else if (IS_ARM_R(ch,LOST_THUMB))
    {
	act("A thumb slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A thumb slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_THUMB);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_THUMB);
    }
    else if (IS_ARM_R(ch,BROKEN_THUMB))
    {
	act("$n's right thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_THUMB);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_I))
    {
	act("An index finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("An index finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_I);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_I))
    {
	act("$n's right index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_M))
    {
	act("A middle finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A middle finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_M);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_M))
    {
	act("$n's right middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);
    }
    else if (IS_ARM_R(ch,LOST_FINGER_R))
    {
	act("A ring finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A ring finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_R);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_R))
    {
	act("$n's right ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);
    }
    else if ( IS_ARM_R(ch,LOST_FINGER_L))
    {
	act("A little finger slides out of $n's right hand.",ch,NULL,NULL,TO_ROOM);
	act("A little finger slides out of your right hand.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],LOST_FINGER_L);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);
    }
    else if ( IS_ARM_R(ch,BROKEN_FINGER_L))
    {
	act("$n's right little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);
    }
    else if (IS_BODY(ch,CUT_CHEST) && !IS_BLEEDING(ch, BLEEDING_CHEST))
    {
	act("The wound in $n's chest closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your chest closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_CHEST);
    }
    else if (IS_BODY(ch,CUT_STOMACH) && !IS_BLEEDING(ch, BLEEDING_STOMACH))
    {
	act("The wound in $n's stomach closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your stomach closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_STOMACH);
    }
    else if (IS_BODY(ch,CUT_THROAT) && !IS_BLEEDING(ch, BLEEDING_THROAT))
    {
	act("The wound in $n's throat closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your throat closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_THROAT);
    }
    else if (IS_ARM_L(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_L))
    {
	act("The wound in $n's left wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your left wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],SLIT_WRIST);
    }
    else if (IS_ARM_R(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_R))
    {
	act("The wound in $n's right wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your right wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],SLIT_WRIST);
    }
    return;
}

void vamp_rage( CHAR_DATA *ch )
{
    if (IS_NPC(ch) || !IS_VAMPIRE(ch)) return;
    send_to_char("You scream with rage as the beast within consumes you!\n\r",ch);
    act("$n screams with rage as $s inner beast consumes $m!.", ch, NULL, NULL, TO_ROOM);
    if (ch->beast > 0) do_beastlike(ch,"");
    if ( ch->pcdata->wolf >= (ch->beast * 0.5) ) return;
    if (!IS_VAMPAFF(ch, VAM_NIGHTSIGHT) && get_disc(ch,DISC_PROTEAN) > 0) 
	do_nightsight(ch,"");
    if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
    if (!IS_VAMPAFF(ch, VAM_CLAWS) && get_disc(ch,DISC_PROTEAN) > 1) 
	do_claws(ch,"");
    send_to_char("You bare yours fangs and growl as your inner beast consumes you.\n\r",ch);
    act("$n bares $s fangs and growls as $s inner beast consumes $m.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->wolf += number_range(10,20);
    WAIT_STATE(ch,12);
    return;
}

void do_humanity( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_VAMPIRE(ch) && ch->beast > 0
	&& ch->beast < 100 && number_range(1, 500) <= ch->beast)
    {
	if (ch->beast == 1)
	{
	    send_to_char("You have attained Golconda!\n\r", ch);
	    if (!IS_IMMUNE(ch, IMM_SUNLIGHT))
		SET_BIT(ch->immune, IMM_SUNLIGHT);
	}
	else
	    send_to_char("You feel slightly more in control of your beast.\n\r", ch);
	ch->beast -= 1;
    }
    return;
}

void do_beastlike( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_VAMPIRE(ch) && !IS_ABOMINATION(ch) && ch->beast < 100
	&& ch->beast > 0 && number_range(1, 500) <= ch->beast)
    {
	if (get_disc(ch, DISC_OBEAH) != 0) return;
	if (ch->beast < 99)
	    send_to_char("You feel your beast take more control over your actions.\n\r", ch);
	else
	    send_to_char("Your beast has fully taken over control of your actions!\n\r", ch);
	ch->beast += 1;
	if (get_disc(ch,DISC_PROTEAN) > 0 && 
	    !IS_VAMPAFF(ch, VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
	if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	if (get_disc(ch,DISC_PROTEAN) > 1 && 
	    !IS_VAMPAFF(ch, VAM_CLAWS) ) do_claws(ch,"");
    }
    return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_ARM_L(ch, SLIT_WRIST) && !IS_ARM_R(ch, SLIT_WRIST))
    {
	send_to_char("First you need to SLIT your wrist.\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Who do you wish to feed?\n\r",ch);
	return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if ((IS_GHOUL(victim) || victim->class == CLASS_NONE || IS_MAGE(victim)) && 
	victim->blood[0] <= 25)
    {
	if (!IS_HERO(victim))
	    send_to_char("Blood does them no good at all.\n\r",ch);
	else
	    do_bite(ch, victim->name);
	return;
    }
    else if (!IS_VAMPIRE(victim) && !IS_GHOUL(victim) && 
	victim->class != CLASS_NONE)
    {
	send_to_char("Blood does them no good at all.\n\r",ch);
	return;
    }
    if (ch == victim)
    {
	send_to_char("There is no point in feeding yourself blood!\n\r",ch);
	return;
    }
    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to drink your blood.\n\r",ch);
	return;
    }
    if ( ch->blood[BLOOD_CURRENT] < (10 * ch->blood[BLOOD_POTENCY]))
    {
	send_to_char("You don't have enough blood.\n\r",ch);
	return;
    }

    act("You feed $N some blood from your open wrist.",ch,NULL,victim,TO_CHAR);
    act("$n feeds $N some blood from $s open wrist.",ch,NULL,victim,TO_NOTVICT);

    if (victim->position < POS_RESTING)
	send_to_char("You feel some blood poured down your throat.\n\r",victim);
    else
	act("$n feeds you some blood from $s open wrist.", ch, NULL, victim, TO_VICT);

    blood = number_range(5,10) * ch->blood[BLOOD_POTENCY];
    ch->blood[BLOOD_CURRENT] -= blood;
    victim->blood[BLOOD_CURRENT] += blood;
    if (victim->blood[BLOOD_CURRENT] >= victim->blood[BLOOD_POOL])
    {
	victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
	if (IS_VAMPIRE(victim)) 
	    send_to_char("Your blood lust is sated.\n\r",victim);
    }
    if (IS_HERO(victim) && (victim->class == CLASS_NONE || IS_GHOUL(victim)))
    {
	power_ghoul(ch, victim);
	if (!IS_NPC(victim) && victim->pcdata->dpoints < (ch->blood[2] * 1000))
	{
	    if (!IS_VAMPIRE(ch)) victim->pcdata->dpoints += (blood * 60);
	    if (victim->pcdata->dpoints > (ch->blood[2] * 100))
		victim->pcdata->dpoints = (ch->blood[2] * 100);
	}
    }
    return;
}

void do_upkeep( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_ANGEL(ch))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                               -= Angel upkeep =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	if (ch->pcdata->disc[C_POWERS] < 1 && get_disc(ch, DISC_AUSPEX) < 1)
	    send_to_char("You have no angelic powers.\n\r",ch);
	switch ( get_disc(ch, DISC_AUSPEX) )
	{
	    default: break;
	    case 1: send_to_char("You have Basic Angelic Vision.\n\r",ch); break;
	    case 2: send_to_char("You have Intermediate Angelic Vision.\n\r",ch); break;
	    case 3: send_to_char("You have Advanced Angelic Vision.\n\r",ch); break;
	    case 4: send_to_char("You have Expert Angelic Vision.\n\r",ch); break;
	    case 5: send_to_char("You have Supreme Angelic Vision.\n\r",ch); break;
	}
	switch ( get_disc(ch, DISC_OBEAH) )
	{
	    default: break;
	    case 1: send_to_char("You have Basic Obeah.\n\r",ch); break;
	    case 2: send_to_char("You have Intermediate Obeah.\n\r",ch); break;
	    case 3: send_to_char("You have Advanced Obeah.\n\r",ch); break;
	    case 4: send_to_char("You have Expert Obeah.\n\r",ch); break;
	    case 5: send_to_char("You have Supreme Obeah.\n\r",ch); break;
	}
	if (IS_POLYAFF(ch, POLY_ZULO))
	    sprintf(buf,"You are in Angel Form.\n\r");
	if (IS_MORE(ch, MORE_GUARDIAN))
	    sprintf(buf,"You have a little cherub perched on your shoulder.\n\r");
    	if ( IS_DEMPOWER(ch,DEM_FANGS) )
	{
	    if (IS_VAMPAFF(ch, VAM_FANGS)) send_to_char("You have a pair of long pointed fangs extending from your gums.\n\r",ch);
	    else send_to_char("You have a pair of long pointed fangs, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_CLAWS) )
	{
	    if (IS_VAMPAFF(ch, VAM_CLAWS)) send_to_char("You have a pair of razor sharp claws extending from your fingers.\n\r",ch);
	    else send_to_char("You have a pair of razor sharp claws, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_MOD(ch,MOD_HORNS) )
	    send_to_char("You have a pair of curved horns extending from your forehead.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_HOOVES) )
	{
	    if (IS_DEMAFF(ch, DEM_HOOVES)) send_to_char("You have hooves instead of feet.\n\r",ch);
	    else send_to_char("You are able to transform your feet into hooves at will.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_EYES) )
	{
	    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) send_to_char("Your eyes are glowing bright blue, allowing you to see in the dark.\n\r",ch);
	    else send_to_char("You are able to see in the dark, although that power is not currently activated.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_WINGS) )
	{
	    if (!IS_DEMAFF(ch, DEM_WINGS)) send_to_char("You have the ability to extend wings from your back.\n\r",ch);
	    else if (IS_DEMAFF(ch, DEM_UNFOLDED)) send_to_char("You have a pair of white feathered wings unfolded behind your back.\n\r",ch);
	    else send_to_char("You have a pair of white feathered wings folded behind your back.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_MIGHT) )
	    send_to_char("Your muscles ripple with supernatural strength.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TOUGH) )
	    send_to_char("Your skin reflects blows with supernatural toughness.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SPEED) )
	    send_to_char("You move with supernatural speed and grace.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TRAVEL) )
	    send_to_char("You are able to travel to other angels at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SCRY) )
	    send_to_char("You are able to scry over great distances at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SHADOWSIGHT) )
	{
	    if (IS_AFFECTED(ch, AFF_SHADOWSIGHT)) send_to_char("You are able see things in the Umbra.\n\r",ch);
	    else send_to_char("You are able to view the Umbra, although you are not currently doing so.\n\r",ch);
	}
    	if (IS_VAMPAFF(ch, VAM_OBFUSCATE))
	    send_to_char("You are concealed with Obfuscate.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_EYESPY) )
	    send_to_char("You have the ability to create Eyespy's.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_FIREWALL) )
	    send_to_char("You have the ability to summon Firewalls.\n\r",ch);
	if (IS_MORE(ch,MORE_HAND_FLAME) )
	    send_to_char("Your hands are infused with the Might of God.\n\r",ch);
    }
    else if (IS_DEMON(ch))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if (IS_NEPHANDI(ch))
	    send_to_char("                              -= Nephandi upkeep =-\n\r",ch);
	else if (IS_BAALI(ch))
	    send_to_char("                               -= Baali upkeep =-\n\r",ch);
	else if (IS_BLACK_SPIRAL_DANCER(ch))
	    send_to_char("                         -= Black Spiral Dancer upkeep =-\n\r",ch);
	else
	    send_to_char("                               -= Demon upkeep =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);

	if (ch->pcdata->disc[C_POWERS] < 1 && get_disc(ch, DISC_AUSPEX) < 1)
	    send_to_char("You have no demonic powers.\n\r",ch);
	switch ( get_disc(ch, DISC_AUSPEX) )
	{
	    default: break;
	    case 1: send_to_char("You have Basic Demonic Vision.\n\r",ch); break;
	    case 2: send_to_char("You have Intermediate Demonic Vision.\n\r",ch); break;
	    case 3: send_to_char("You have Advanced Demonic Vision.\n\r",ch); break;
	    case 4: send_to_char("You have Expert Demonic Vision.\n\r",ch); break;
	    case 5: send_to_char("You have Supreme Demonic Vision.\n\r",ch); break;
	}
	switch ( get_disc(ch, DISC_DAIMOINON) )
	{
	    default: break;
	    case 1: send_to_char("You have Basic Daimoinon.\n\r",ch); break;
	    case 2: send_to_char("You have Intermediate Daimoinon.\n\r",ch); break;
	    case 3: send_to_char("You have Advanced Daimoinon.\n\r",ch); break;
	    case 4: send_to_char("You have Expert Daimoinon.\n\r",ch); break;
	    case 5: send_to_char("You have Supreme Daimoinon.\n\r",ch); break;
	}
	if (IS_POLYAFF(ch, POLY_ZULO))
	    sprintf(buf,"You are in Demon Form.\n\r");
	if (IS_MORE(ch, MORE_GUARDIAN))
	    sprintf(buf,"You have a little imp perched on your shoulder.\n\r");
    	if ( IS_DEMPOWER(ch,DEM_FANGS) )
	{
	    if (IS_VAMPAFF(ch, VAM_FANGS)) send_to_char("You have a pair of long pointed fangs extending from your gums.\n\r",ch);
	    else send_to_char("You have a pair of long pointed fangs, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_CLAWS) )
	{
	    if (IS_VAMPAFF(ch, VAM_CLAWS)) send_to_char("You have a pair of razor sharp claws extending from your fingers.\n\r",ch);
	    else send_to_char("You have a pair of razor sharp claws, but they are not currently extended.\n\r",ch);
	}
    	if ( IS_MOD(ch,MOD_HORNS) )
	    send_to_char("You have a pair of curved horns extending from your forehead.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_HOOVES) )
	{
	    if (IS_DEMAFF(ch, DEM_HOOVES)) send_to_char("You have hooves instead of feet.\n\r",ch);
	    else send_to_char("You are able to transform your feet into hooves at will.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_EYES) )
	{
	    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) send_to_char("Your eyes are glowing bright red, allowing you to see in the dark.\n\r",ch);
	    else send_to_char("You are able to see in the dark, although that power is not currently activated.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_WINGS) )
	{
	    if (!IS_DEMAFF(ch, DEM_WINGS)) send_to_char("You have the ability to extend wings from your back.\n\r",ch);
	    else if (IS_DEMAFF(ch, DEM_UNFOLDED)) send_to_char("You have a pair of large leathery wings unfolded behind your back.\n\r",ch);
	    else send_to_char("You have a pair of large leathery wings folded behind your back.\n\r",ch);
	}
    	if ( IS_DEMPOWER(ch,DEM_MIGHT) )
	    send_to_char("Your muscles ripple with supernatural strength.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TOUGH) )
	    send_to_char("Your skin reflects blows with supernatural toughness.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SPEED) )
	    send_to_char("You move with supernatural speed and grace.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_TRAVEL) )
	    send_to_char("You are able to travel to other demons at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SCRY) )
	    send_to_char("You are able to scry over great distances at will.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_SHADOWSIGHT) )
	{
	    if (IS_AFFECTED(ch, AFF_SHADOWSIGHT)) send_to_char("You are able see things in the Umbra.\n\r",ch);
	    else send_to_char("You are able to view the Umbra, although you are not currently doing so.\n\r",ch);
	}
    	if (IS_VAMPAFF(ch, VAM_OBFUSCATE))
	    send_to_char("You are concealed with Obfuscate.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_EYESPY) )
	    send_to_char("You have the ability to create Eyespy's.\n\r",ch);
    	if ( IS_DEMPOWER(ch,DEM_FIREWALL) )
	    send_to_char("You have the ability to summon Firewalls.\n\r",ch);
	if (IS_MORE(ch,MORE_HAND_FLAME) )
	    send_to_char("Your hands are engulfed in burning flames.\n\r",ch);
    }
    else if (IS_VAMPIRE(ch) || IS_GHOUL(ch))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if (IS_ABOMINATION(ch))
	    send_to_char("                            -= Abomination upkeep =-\n\r",ch);
	else if (IS_LICH(ch))
	    send_to_char("                               -= Lich upkeep =-\n\r",ch);
	else if (IS_SKINDANCER(ch))
	    send_to_char("                            -= Skindancer upkeep =-\n\r",ch);
	else
	    send_to_char("                              -= Vampire upkeep =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("You must spend 1 blood per tick or you may well enter Frenzy.\n\r",ch);

    	if (ch->pcdata->regenerate > 0 )
	{
	    sprintf(buf,"You will attempt to regenerate %d more time%s\n\r", 
		ch->pcdata->regenerate, 
		ch->pcdata->regenerate > 1 ? "s." : ".");
	    send_to_char(buf,ch);
	}

    	if (ch->pcdata->celerity > 0 )
	{
	    sprintf(buf,"Your are infused with %d %s of Celerity.\n\r", 
		ch->pcdata->celerity, 
		ch->pcdata->celerity == 1 ? "second" : "seconds");
	    send_to_char(buf,ch);
	}

    	if (strlen(ch->pcdata->conding) > 1 )
	{
	    if (IS_MORE(ch, MORE_LOYAL))
		sprintf(buf,"You have been conditioned by %s and are loyal.\n\r", ch->pcdata->conding);
	    else
		sprintf(buf,"You have been conditioned by %s.\n\r", ch->pcdata->conding);
	    send_to_char(buf,ch);
	}
    	if (strlen(ch->pcdata->love) > 1 )
	{
	    sprintf(buf,"You are in love with %s.\n\r", ch->pcdata->love);
	    send_to_char(buf,ch);
	}
	if (get_disc(ch,DISC_OBEAH) != 0)
	{
	    if (IS_MORE(ch, MORE_OBEAH))
	    {
		if (ch->pcdata->obeah > 0)
		    send_to_char("Your third eye is open and glowing brightly.\n\r",ch);
		else
		    send_to_char("Your third eye is open.\n\r",ch);
	    }
	    else
		send_to_char("Your third eye is closed.\n\r",ch);
	}
	if (get_disc(ch,DISC_PROTEAN) > 5)
	    send_to_char("Your Flesh is as hard as Marble.\n\r",ch);
	if (get_disc(ch,DISC_DAIMOINON) > 5)
	    send_to_char("You are immune to heat and fire.\n\r",ch);
	if (get_disc(ch,DISC_VICISSITUDE) > 5)
	    send_to_char("Your blood is a deadly acid.\n\r",ch);
	if (IS_MORE(ch, MORE_EVILEYE))
	    send_to_char("Your Evil Eye is on.\n\r",ch);
	if (IS_MORE(ch, MORE_ANIMAL_MASTER))
	    send_to_char("You appear very attractive to animals.\n\r",ch);
    	if (IS_VAMPAFF(ch, VAM_OBFUSCATE))
	    send_to_char("You are concealed with Obfuscate.\n\r",ch);
	if (IS_EXTRA(ch, EXTRA_FAKE_CON))
	    send_to_char("Your Soul Mask is up.\n\r",ch);
    	if (IS_ITEMAFF(ch,ITEMA_PEACE) )
	    send_to_char("Your Majesty is up.\n\r",ch);
    	if (IS_EXTRA(ch,EXTRA_AWE) )
	    send_to_char("You appear very awe-inspiring.\n\r",ch);
    	if (IS_MORE(ch,MORE_GUARDIAN) )
	    send_to_char("Your Spirit Guardian is with you.\n\r",ch);
    	if (IS_MORE(ch,MORE_COURAGE) )
	    send_to_char("You are feeling very brave.\n\r",ch);
    	if (IS_MORE(ch,MORE_ANGER) )
	    send_to_char("You are feeling very angry.\n\r",ch);
    	if (IS_MORE(ch,MORE_CALM) )
	    send_to_char("You are feeling very lazy.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	{
	    if (IS_POLYAFF(ch, POLY_ZULO))
		sprintf(buf,"You are in Zulo Form.\n\r");
	    else if (IS_POLYAFF(ch, POLY_SPIRIT))
		sprintf(buf,"You are in Spiritform.\n\r");
	    else if (IS_EXTRA(ch, EXTRA_VICISSITUDE))
		sprintf(buf,"Your features look like %s.\n\r",ch->morph);
	    else if (IS_POLYAFF(ch, POLY_TRUE))
		sprintf(buf,"You are True Disguised as %s.\n\r",ch->morph);
	    else if (IS_POLYAFF(ch, POLY_CHANGELING))
		sprintf(buf,"You are Changed to look like %s.\n\r",ch->morph);
	    else if (IS_POLYAFF(ch, POLY_FLESHCRAFT))
		sprintf(buf,"You are Fleshcrafted to look like %s.\n\r",ch->morph);
	    else if (IS_POLYAFF(ch, POLY_BONECRAFT))
		sprintf(buf,"You are Bonecrafted to look like %s.\n\r",ch->morph);
	    else
		sprintf(buf,"You are Masked as %s.\n\r",ch->morph);
	    send_to_char(buf,ch);
	}
    	if (IS_IMMUNE(ch,IMM_SHIELDED) )
	    send_to_char("You aura is Shielded.\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	    send_to_char("You are in the Umbra.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_FANGS) )
	    send_to_char("You have your Fangs extended.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_CLAWS) )
	    send_to_char("You have your Claws extended.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
	    send_to_char("You are using Gleam of Red Eyes.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_SPIRITEYES) )
	    send_to_char("You are using Spirit Eyes.\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
	    send_to_char("You are using Umbral Vision.\n\r",ch);
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) )
	    send_to_char("You are using Heightened Senses.\n\r",ch);
	if (IS_MORE(ch,MORE_HAND_FLAME) )
	    send_to_char("Your hands are engulfed in burning flames.\n\r",ch);
    	if (IS_IMMUNE(ch,IMM_STAKE) )
	    send_to_char("Your heart has been removed from your chest, rendering you immune to staking.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_CHANGED) )
	{
	    if      (IS_POLYAFF(ch,POLY_BAT))  sprintf(buf2,"Bat" );
	    else if (IS_POLYAFF(ch,POLY_WOLF)) sprintf(buf2,"Wolf");
	    else                               sprintf(buf2,"Mist");
	    sprintf(buf,"You are in %s form.\n\r",buf2);
	    send_to_char(buf,ch);
	}
	if      ( IS_POLYAFF(ch,POLY_SHADOW) )
	    send_to_char("You are in Shadow form.\n\r",ch);
	else if ( IS_POLYAFF(ch,POLY_SERPENT) )
	    send_to_char("You are in Serpent form.\n\r",ch);
	else if ( IS_MORE(ch,MORE_PLASMA) )
	    send_to_char("You are in Plasma form.\n\r",ch);
	else if ( IS_MORE(ch,MORE_ASHES) )
	    send_to_char("You are in Ashes form.\n\r",ch);
	else if ( IS_MORE(ch,MORE_MUMMIFIED) )
	    send_to_char("Your body is mummified.\n\r",ch);
	if (!IS_ABOMINATION(ch))
	{
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    return;
	}
	if (IS_POLYAFF(ch,POLY_BAT))
	    send_to_char("You are in Lupus form.\n\r",ch);
	if (IS_MORE(ch,MORE_BRISTLES) )
	    send_to_char("Your body is covered in long quill-like spines.\n\r",ch);
	if (IS_ITEMAFF(ch,ITEMA_SUNSHIELD) )
	    send_to_char("You are surrounded with a halo of sunlight.\n\r",ch);
	if (get_auspice(ch, AUSPICE_RAGABASH) > 0)
	    send_to_char("Your light feet leave no tracks.\n\r",ch);
	if (get_auspice(ch, AUSPICE_RAGABASH) > 4)
	    send_to_char("Luna has blessed you with immunity to silver.\n\r",ch);
	if (get_auspice(ch, AUSPICE_PHILODOX) > 0)
	    send_to_char("You are able to shrug off the effects of pain.\n\r",ch);
	if (get_auspice(ch, AUSPICE_AHROUN) > 0 && IS_VAMPAFF(ch, VAM_CLAWS))
	    send_to_char("Your claws are as sharp as razors.\n\r",ch);
	if (get_auspice(ch, AUSPICE_AHROUN) > 3 && IS_VAMPAFF(ch, VAM_CLAWS))
	    send_to_char("Your claws are pure silver.\n\r",ch);
	if (get_tribe(ch, TRIBE_BLACK_FURIES) > 1 && IS_VAMPAFF(ch, VAM_CLAWS))
	    send_to_char("Your claws are dripping with black venom.\n\r",ch);
	if (get_tribe(ch, TRIBE_GET_OF_FENRIS) > 3)
	    send_to_char("Punches bounce off your body with no affect.\n\r",ch);
	if (get_tribe(ch, TRIBE_RED_TALONS) > 2)
	{
	    if (get_tribe(ch, TRIBE_RED_TALONS) > 3)
		send_to_char("Your claws can parry weapons in combat as well as any other weapon can.\n\r",ch);
	    else
		send_to_char("Your claws can parry weapons in combat.\n\r",ch);
	}
	if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 0)
	{
	    if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 2)
		send_to_char("Your move at a Speed Beyond Thought.\n\r",ch);
	    else
		send_to_char("Your move at the Speed of Thought.\n\r",ch);
	}
	if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 1)
	    send_to_char("Your body has the resistance of Messanger's Fortitude.\n\r",ch);
	if ( ch->pcdata->absorb[ABS_MOB_VNUM] > 0 )
	    send_to_char("You have a Flesh Bonded animal absorbed into your body.\n\r",ch);
    }
    else if (IS_WEREWOLF(ch))
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                              -= Werewolf upkeep =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    	if (strlen(ch->pcdata->conding) > 1 )
	{
	    if (IS_MORE(ch, MORE_LOYAL))
		sprintf(buf,"You have been conditioned by %s and are loyal.\n\r", ch->pcdata->conding);
	    else
		sprintf(buf,"You have been conditioned by %s.\n\r", ch->pcdata->conding);
	    send_to_char(buf,ch);
	}
    	if (strlen(ch->pcdata->love) > 1 )
	{
	    sprintf(buf,"You are in love with %s.\n\r", ch->pcdata->love);
	    send_to_char(buf,ch);
	}
	if (get_auspice(ch,AUSPICE_AHROUN) > 4)
	    send_to_char("You are immune to heat and fire.\n\r",ch);
	if (get_tribe(ch,TRIBE_GET_OF_FENRIS) > 0)
	    send_to_char("Your blood is a deadly acid.\n\r",ch);
	if (IS_MORE(ch, MORE_ANIMAL_MASTER))
	    send_to_char("You appear very attractive to animals.\n\r",ch);
    	if (IS_ITEMAFF(ch,ITEMA_PEACE) )
	    send_to_char("You are using the Icy Chill of Despair.\n\r",ch);
    	if (IS_EXTRA(ch,EXTRA_AWE) )
	    send_to_char("You seem very persuasive.\n\r",ch);
    	if (IS_MORE(ch,MORE_GUARDIAN) )
	    send_to_char("Your Spirit Guardian is with you.\n\r",ch);
    	if (IS_VAMPAFF(ch, VAM_OBFUSCATE))
	    send_to_char("You are concealed with Obfuscate.\n\r",ch);
    	if (IS_MORE(ch,MORE_COURAGE) )
	    send_to_char("You are feeling very brave.\n\r",ch);
    	if (IS_MORE(ch,MORE_ANGER) )
	    send_to_char("You are feeling very angry.\n\r",ch);
    	if (IS_MORE(ch,MORE_CALM) )
	    send_to_char("You are feeling very lazy.\n\r",ch);
    	if (IS_IMMUNE(ch,IMM_SHIELDED) )
	    send_to_char("You aura is Shielded.\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	    send_to_char("You are in the Umbra.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_FANGS) )
	    send_to_char("You have your Fangs extended.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_CLAWS) )
	    send_to_char("You have your Claws extended.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
	    send_to_char("You are using Eyes of the Cat.\n\r",ch);
    	if (IS_VAMPAFF(ch,VAM_SPIRITEYES) )
	    send_to_char("You are using Scent from Beyond.\n\r",ch);
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
	    send_to_char("You are using Umbral Vision.\n\r",ch);
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) )
	    send_to_char("You are using Heightened Senses.\n\r",ch);
	if (IS_MORE(ch,MORE_HAND_FLAME) )
	    send_to_char("Your hands are engulfed in burning flames.\n\r",ch);

	if (IS_POLYAFF(ch,POLY_BAT))
	    send_to_char("You are in Lupus form.\n\r",ch);
	if (IS_MORE(ch,MORE_BRISTLES) )
	    send_to_char("Your body is covered in long quill-like spines.\n\r",ch);
	if (IS_ITEMAFF(ch,ITEMA_SUNSHIELD) )
	    send_to_char("You are surrounded with a halo of sunlight.\n\r",ch);
	if (get_auspice(ch, AUSPICE_RAGABASH) > 0)
	    send_to_char("Your light feet leave no tracks.\n\r",ch);
	if (get_auspice(ch, AUSPICE_RAGABASH) > 4)
	    send_to_char("Luna has blessed you with immunity to silver.\n\r",ch);
	if (get_auspice(ch, AUSPICE_PHILODOX) > 0)
	    send_to_char("You are able to shrug off the effects of pain.\n\r",ch);
	if (get_auspice(ch, AUSPICE_AHROUN) > 0 && IS_VAMPAFF(ch, VAM_CLAWS) )
	    send_to_char("Your claws are as sharp as razors.\n\r",ch);
	if (get_auspice(ch, AUSPICE_AHROUN) > 0 && IS_VAMPAFF(ch, VAM_CLAWS) )
	    send_to_char("Your claws are pure silver.\n\r",ch);
	if (get_tribe(ch, TRIBE_BLACK_FURIES) > 1 && IS_VAMPAFF(ch, VAM_CLAWS))
	    send_to_char("Your claws are dripping with black venom.\n\r",ch);
	if (get_tribe(ch, TRIBE_GET_OF_FENRIS) > 3)
	    send_to_char("Punches bounce off your body with no affect.\n\r",ch);
	if (get_tribe(ch, TRIBE_RED_TALONS) > 2)
	{
	    if (get_tribe(ch, TRIBE_RED_TALONS) > 3)
		send_to_char("Your claws can parry weapons in combat as well as any other weapon can.\n\r",ch);
	    else
		send_to_char("Your claws can parry weapons in combat.\n\r",ch);
	}
	if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 0)
	{
	    if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 2)
		send_to_char("Your move at a Speed Beyond Thought.\n\r",ch);
	    else
		send_to_char("Your move at the Speed of Thought.\n\r",ch);
	}
	if (get_tribe(ch, TRIBE_SILENT_STRIDERS) > 1)
	    send_to_char("Your body has the resistance of Messanger's Fortitude.\n\r",ch);
	if ( ch->pcdata->absorb[ABS_MOB_VNUM] > 0 )
	    send_to_char("You have a Flesh Bonded animal absorbed into your body.\n\r",ch);
    }
    else
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_vclan( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_VAMPIRE(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->clan) < 2 )
    {
	send_to_char("But you don't belong to any clan!\n\r",ch);
	return;
    }

    sprintf( buf, "The %s clan:\n\r", ch->clan );
    send_to_char( buf, ch );
    send_to_char("[      Name      ] [ Gen ] [ Hits  % ] [ Mana  % ] [ Move  % ] [  Exp  ] [Blood]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_VAMPIRE(gch) ) continue;
	if ( strlen(gch->clan) < 2) continue;
	if ( !str_cmp(ch->clan,gch->clan) )
	{
	    if (strlen(ch->side) > 1 && strlen(gch->side) > 1 && 
		str_cmp(ch->side, gch->side))
	    {
		sprintf( buf,
		   "[%-16s] [ ??? ] [?????????] [?????????] [?????????] [???????] [ ??? ]\n\r",
		    capitalize( gch->name ));
		send_to_char( buf, ch );
	    }
	    else
	    {
		sprintf( buf,
		   "[%-16s] [ %-2d  ] [%-6d%3d] [%-6d%3d] [%-6d%3d] [%7d] [ %3d ]\n\r",
		    capitalize( gch->name ),
		    gch->vampgen,
		    gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		    gch->mana, (gch->mana * 100 / gch->max_mana),
		    gch->move, (gch->move * 100 / gch->max_move),
		    gch->exp, gch->blood[BLOOD_CURRENT] );
		send_to_char( buf, ch );
	    }
	}
    }
    return;
}

void do_tribe( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char clan[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    char chage[10];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( strlen(ch->clan) > 16)
    {
	send_to_char("You need no tribe, foul spawn of the wyrm.\n\r",ch);
	return;
    }
    send_to_char("[      Name      ] [      Tribe      ] [  Age  ] [ Hits  % ] [ Vital % ] [ Exp ]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_WEREWOLF(gch) ) continue;
	if ( IS_IMMORTAL(gch) && !IS_IMMORTAL(ch) && IS_SET(gch->act, PLR_WIZINVIS) ) continue;
	if ( strlen(gch->clan) > 16) continue;
	if ( gch->clan != NULL && strlen(gch->clan) > 1 )
	    sprintf(clan,gch->clan);
	else if ( gch->vampgen == 1 )
	    sprintf(clan,"All");
	else
	    sprintf(clan,"None");
	{
	    if ( ch->vampgen <= 2 ) strcpy(chage,"Ancient");
	    else switch (ch->pcdata->rank)
	    {
		default:
		case 0: strcpy(chage,"  Pup"); break;
		case 1: strcpy(chage," Youth"); break;
		case 2: strcpy(chage," Adult"); break;
		case 3: strcpy(chage," Elder"); break;
		case 4: strcpy(chage,"Ancient"); break;
	    }
	    sprintf( buf,
	    "[%-16s] [%-17s] [%-7s] [%-6d%3d] [%-6d%3d] [%-5d]\n\r",
		capitalize( gch->name ), clan, chage,
		gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		gch->move, (gch->move * 100 / gch->max_move),
		gch->exp);
		send_to_char( buf, ch );
	}
    }
    return;
}


void do_werewolf( CHAR_DATA *ch, char *argument )
{
    char       buf[MAX_INPUT_LENGTH];
    char       arg[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
/*
    OBJ_DATA  *obj;
*/
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_WEREWOLF(ch)) return;
    if (IS_SET(ch->act, PLR_WOLFMAN)) return;
    if ( ch->pcdata->wolfform[1] != FORM_HOMID ) return;

    if (IS_POLYAFF(ch,POLY_MASK) ) do_mask(ch,"self");
    if (IS_POLYAFF(ch,POLY_TRUE) ) do_truedisguise(ch,"self");
    if (IS_POLYAFF(ch,POLY_HAGS) ) do_mask(ch,"self");
    if (IS_POLYAFF(ch,POLY_CHANGELING) ) do_mask(ch,"self");
    if (IS_POLYAFF(ch,POLY_FLESHCRAFT) ) do_mask(ch,"self");
    if (IS_POLYAFF(ch,POLY_BONECRAFT) ) do_mask(ch,"self");
    if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
    if (IS_POLYAFF(ch,POLY_SHADOW) ) do_shadowbody(ch,"");
    if (IS_VAMPAFF(ch,VAM_DISGUISED) )
    {
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
	REMOVE_BIT(ch->polyaff, POLY_SPIRIT);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	free_string( ch->long_descr );
	ch->morph = str_dup( "" );
    }
    SET_BIT(ch->act, PLR_WOLFMAN);

    send_to_char("You throw back your head and howl with rage!\n\r",ch);
    act("$n throws back $s head and howls with rage!.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Coarse dark hair sprouts from your body.\n\r",ch);
    act("Coarse dark hair sprouts from $n's body.",ch,NULL,NULL,TO_ROOM);
/*
    if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT))
    {
	send_to_char("Your eyes start glowing red.\n\r",ch);
	act("$n's eyes start glowing red.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    }
*/
    if (!IS_VAMPAFF(ch,VAM_FANGS))
    {
	send_to_char("A pair of long fangs extend from your mouth.\n\r",ch);
	act("A pair of long fangs extend from $n's mouth.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_FANGS);
    }
    if (!IS_VAMPAFF(ch,VAM_CLAWS))
    {
	if (get_auspice(ch, AUSPICE_AHROUN) > 3)
	{
	    send_to_char("Gleaming silver talons extend from your fingers.\n\r",ch);
	    act("Gleaming silver talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
	    act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
	}
	SET_BIT(ch->vampaff, VAM_CLAWS);
    }
    if (!IS_MOD(ch, MOD_TAIL)) SET_BIT(ch->pcdata->bodymods, MOD_TAIL);
/*
    if ((obj = get_eq_char(ch,WEAR_WIELD)) != NULL && !IS_SET(obj->spectype, SITEM_WOLFWEAPON))
    {
	act("$p drops from your right hand.",ch,obj,NULL,TO_CHAR);
	act("$p drops from $n's right hand.",ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_char(obj, ch);
    }
    if ((obj = get_eq_char(ch,WEAR_HOLD)) != NULL && !IS_SET(obj->spectype, SITEM_WOLFWEAPON))
    {
	act("$p drops from your left hand.",ch,obj,NULL,TO_CHAR);
	act("$p drops from $n's left hand.",ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_char(obj, ch);
    }
*/
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    if (IS_DEMON(ch) && IS_POLYAFF(ch, POLY_ZULO))
	sprintf(buf,"An enormous hairy demonic beast" );
    else if (IS_POLYAFF(ch, POLY_ZULO))
	sprintf(buf,"An enormous black hairy monster" );
    else
	sprintf(buf,"A huge werewolf" );
    free_string(ch->morph);
    if (!IS_ABOMINATION(ch))
    {
	ch->blood[BLOOD_CURRENT] *= 2;
	ch->blood[BLOOD_POOL] *= 2;
    }
    ch->morph = str_dup(buf);
    ch->pcdata->wolf += 25;
    if (get_tribe(ch, TRIBE_GET_OF_FENRIS) > 4) ch->pcdata->wolf = 300;
    else if (ch->pcdata->wolf > 300) ch->pcdata->wolf = 300;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( ch == vch )
	{
	    act("You throw back your head and howl with rage!", ch, NULL, NULL, TO_CHAR);
	    continue;
	}
	if ( !IS_NPC(vch) && vch->pcdata->chobj != NULL )
	    continue;
	if (!IS_NPC(vch))
	{
	    if (vch->in_room == ch->in_room)
		act("$n throws back $s head and howls with rage!", ch, NULL, vch, TO_VICT);
	    else if (vch->in_room->area == ch->in_room->area)
		send_to_char("You hear a fearsome howl close by!\n\r", vch);
	    else
		send_to_char("You hear a fearsome howl far off in the distance!\n\r", vch);
	    if (!CAN_PK(vch)) continue;
	}
	if ( vch->in_room == ch->in_room && can_see( ch, vch) )
	{
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	    if (vch == NULL || vch->position <= POS_STUNNED) continue;
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	    if (vch == NULL || vch->position <= POS_STUNNED) continue;
	    multi_hit( ch, vch, TYPE_UNDEFINED );
	}
    }
    return;
}

void do_unwerewolf( CHAR_DATA *ch, char *argument )
{
    char       arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_WEREWOLF(ch)) return;
    if (!IS_SET(ch->act, PLR_WOLFMAN)) return;
    if ( ch->pcdata->wolfform[1] != FORM_HOMID ) return;
    REMOVE_BIT(ch->act, PLR_WOLFMAN);
    free_string(ch->morph);
    if (IS_DEMON(ch) && IS_POLYAFF(ch, POLY_ZULO))
	ch->morph = str_dup("a great demonic beast");
    else if (IS_POLYAFF(ch, POLY_ZULO))
	ch->morph = str_dup("a huge black monster");
    else
    {
	ch->morph = str_dup("");
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
    }
    if (!IS_ABOMINATION(ch))
    {
	ch->blood[BLOOD_CURRENT] *= 0.5;
	ch->blood[BLOOD_POOL] *= 0.5;
    }

    if (IS_VAMPAFF(ch,VAM_CLAWS) && !IS_POLYAFF(ch, POLY_ZULO))
    {
	if (get_auspice(ch, AUSPICE_AHROUN) > 3)
	{
	    send_to_char("Your gleaming silver talons slide back into your fingers.\n\r",ch);
	    act("$n's gleaming silver talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("Your talons slide back into your fingers.\n\r",ch);
	    act("$n's talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	}
	REMOVE_BIT(ch->vampaff, VAM_CLAWS);
    }
    if (IS_VAMPAFF(ch,VAM_FANGS) && !IS_POLYAFF(ch, POLY_ZULO))
    {
	send_to_char("Your fangs slide back into your mouth.\n\r",ch);
	act("$n's fangs slide back into $s mouth.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_FANGS);
    }
/*
    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) && !IS_POLYAFF(ch, POLY_ZULO))
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    }
*/
    if (IS_MOD(ch, MOD_TAIL)) REMOVE_BIT(ch->pcdata->bodymods, MOD_TAIL);
    send_to_char("Your coarse hair shrinks back into your body.\n\r",ch);
    act("$n's coarse hair shrinks back into $s body.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->wolf -= 25;
    if (ch->pcdata->wolf < 0) ch->pcdata->wolf = 0;
    return;
}

void do_favour( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) ) return;

    if ( !IS_VAMPIRE(ch) && !IS_WEREWOLF(ch) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax is: favour <target> <favour>\n\r", ch );
	if ( ch->vampgen == 2 )
	{
	    if ( IS_WEREWOLF(ch) )
		send_to_char( "Favours you can grant: Induct Outcast Sire Shaman.\n\r", ch );
	    else
		send_to_char( "Favours you can grant: Induct Outcast Sire Prince.\n\r", ch );
	}
	else if (IS_EXTRA(ch, EXTRA_PRINCE))
	    send_to_char( "Favours you can grant: Induct Outcast Sire.\n\r", ch );
	else
	    send_to_char("None.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Not on yourself!\n\r", ch );
	return;
    }

    if ( ch->class != victim->class )
    {
	send_to_char( "You cannot do that, fool!\n\r", ch );
	return;
    }

    if ( !str_cmp(arg2,"sire") )
    {
	if (ch->vampgen != 2 && !IS_EXTRA(ch, EXTRA_PRINCE))
	{
	    send_to_char("You are unable to grant this sort of favour.\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim,EXTRA_SIRE)) 
	{
	    if ( IS_WEREWOLF(ch) )
	    {
		act("You remove $N's permission to claw a pup!",ch,NULL,victim,TO_CHAR);
		act("$n has removed $N's permission to claw a pup!",ch,NULL,victim,TO_NOTVICT);
		act("$n has remove your permission to claw a pup!",ch,NULL,victim,TO_VICT);
	    }
	    else
	    {
		act("You remove $N's permission to sire a childe!",ch,NULL,victim,TO_CHAR);
		act("$n has removed $N's permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
		act("$n has remove your permission to sire a childe!",ch,NULL,victim,TO_VICT);
	    }
	    REMOVE_BIT(victim->extra,EXTRA_SIRE);
	    return;
	}
	if ( IS_WEREWOLF(ch) )
	{
	    act("You grant $N permission to claw a pup!",ch,NULL,victim,TO_CHAR);
	    act("$n has granted $N permission to claw a pup!",ch,NULL,victim,TO_NOTVICT);
	    act("$n has granted you permission to claw a pup!",ch,NULL,victim,TO_VICT);
	}
	else
	{
	    act("You grant $N permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	    act("$n has granted $N permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	    act("$n has granted you permission to sire a childe!",ch,NULL,victim,TO_VICT);
	}
	SET_BIT(victim->extra,EXTRA_SIRE);
	return;
    }
    else if ( !str_cmp(arg2,"prince") && IS_VAMPIRE(ch) )
    {
	if (ch->vampgen != 2)
	{
	    send_to_char("You are unable to grant this sort of favour.\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim,EXTRA_PRINCE)) 
	{
	    act("You remove $N's Prince status!",ch,NULL,victim,TO_CHAR);
	    act("$n has removed $N's Prince status!",ch,NULL,victim,TO_NOTVICT);
	    act("$n has removed your Prince status!",ch,NULL,victim,TO_VICT);
	    REMOVE_BIT(victim->extra,EXTRA_PRINCE);
	    return;
	}
	act("You grant $N Prince status!",ch,NULL,victim,TO_CHAR);
	act("$n has granted $N Prince status!",ch,NULL,victim,TO_NOTVICT);
	act("$n has granted you Prince status!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->extra,EXTRA_PRINCE);
	return;
    }
    else if ( !str_cmp(arg2,"shaman") && IS_WEREWOLF(ch) )
    {
	if (ch->vampgen != 2)
	{
	    send_to_char("You are unable to grant this sort of favour.\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim,EXTRA_PRINCE)) 
	{
	    act("You remove $N's Shaman status!",ch,NULL,victim,TO_CHAR);
	    act("$n has removed $N's Shaman status!",ch,NULL,victim,TO_NOTVICT);
	    act("$n has removed your Shaman status!",ch,NULL,victim,TO_VICT);
	    REMOVE_BIT(victim->extra,EXTRA_PRINCE);
	    return;
	}
	act("You grant $N Shaman status!",ch,NULL,victim,TO_CHAR);
	act("$n has granted $N Shaman status!",ch,NULL,victim,TO_NOTVICT);
	act("$n has granted you Shaman status!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->extra,EXTRA_PRINCE);
	return;
    }
    else if ( !str_cmp(arg2,"induct") )
    {
	if (ch->vampgen != 2 && !IS_EXTRA(ch, EXTRA_PRINCE))
	{
	    send_to_char("You are unable to grant this sort of favour.\n\r",ch);
	    return;
	}
	if (!IS_IMMUNE(victim,IMM_VAMPIRE))
	{
	    send_to_char( "You cannot induct an unwilling person.\n\r", ch );
	    return;
	}
	if (ch->vampgen >= victim->vampgen)
	{
	    if ( IS_WEREWOLF(ch) )
		send_to_char("You can only induct those of weaker heritage than yourself.\n\r",ch);
	    else
		send_to_char("You can only induct those of higher generation than yourself.\n\r",ch);
	    return;
	}
	if ( IS_WEREWOLF(ch) )
	{
	    act("You induct $N into your tribe!",ch,NULL,victim,TO_CHAR);
	    act("$n inducts $N into $s tribe!",ch,NULL,victim,TO_NOTVICT);
	    act("$n inducts you into $s tribe!",ch,NULL,victim,TO_VICT);
	}
	else
	{
	    act("You induct $N into your clan!",ch,NULL,victim,TO_CHAR);
	    act("$n inducts $N into $s clan!",ch,NULL,victim,TO_NOTVICT);
	    act("$n inducts you into $s clan!",ch,NULL,victim,TO_VICT);
	}
	free_string(victim->clan);
	victim->clan = str_dup( ch->clan );
	return;
    }
    else if ( !str_cmp(arg2,"outcast") )
    {
	if (ch->vampgen != 2 && !IS_EXTRA(ch, EXTRA_PRINCE))
	{
	    send_to_char("You are unable to grant this sort of favour.\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim, EXTRA_PRINCE) && ch->vampgen != 2)
	{
	    if ( IS_WEREWOLF(ch) )
		send_to_char( "You cannot outcast another Shaman.\n\r", ch );
	    else
		send_to_char( "You cannot outcast another Prince.\n\r", ch );
	    return;
	}
	if (ch->vampgen >= victim->vampgen)
	{
	    if ( IS_WEREWOLF(ch) )
		send_to_char("You can only outcast those of weaker heritage than yourself.\n\r",ch);
	    else
		send_to_char("You can only outcast those of higher generation than yourself.\n\r",ch);
	    return;
	}
	if ( IS_WEREWOLF(ch) )
	{
	    act("You outcast $N from your tribe!",ch,NULL,victim,TO_CHAR);
	    act("$n outcasts $N from $s tribe!",ch,NULL,victim,TO_NOTVICT);
	    act("$n outcasts you from $s tribe!",ch,NULL,victim,TO_VICT);
	}
	else
	{
	    act("You outcast $N from your clan!",ch,NULL,victim,TO_CHAR);
	    act("$n outcasts $N from $s clan!",ch,NULL,victim,TO_NOTVICT);
	    act("$n outcasts you from $s clan!",ch,NULL,victim,TO_VICT);
	}
	free_string(victim->clan);
	victim->clan = str_dup( "" );
	REMOVE_BIT(victim->extra,EXTRA_SIRE);
	REMOVE_BIT(victim->extra,EXTRA_PRINCE);
	return;
    }
    else 
	send_to_char( "You are unable to grant that sort of favour.\n\r", ch );
    return;
}

void do_chant( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *book;
    OBJ_DATA *page;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE; /* Target object/victim may be anywhere */
    int spellno = 1;
    int spellcount = 0;
    int spelltype;
    int sn = 0;
    int level;
    extern int chainspell;

    chainspell = 0;

    one_argument( argument, arg );

    if ( ( book = get_eq_char(ch, WEAR_WIELD) ) == NULL || 
	book->item_type != ITEM_BOOK)
    {
	if ( ( book = get_eq_char(ch, WEAR_HOLD) ) == NULL || 
	    book->item_type != ITEM_BOOK)
	{
	    send_to_char( "First you must hold a spellbook.\n\r", ch );
	    return;
	}
    }
    if (IS_SET(book->value[1], CONT_CLOSED))
    {
	send_to_char( "First you better open the book.\n\r", ch );
	return;
    }
    if (book->value[2] < 1)
    {
	send_to_char( "There are no spells on the index page!\n\r", ch );
	return;
    }
    if ( ( page = get_page( book, book->value[2] ) ) == NULL )
    {
	send_to_char( "The current page seems to have been torn out!\n\r", ch );
	return;
    }
    spellcount = ((page->value[1] * 10000) + (page->value[2] * 10) + 
	page->value[3]);
    act("You chant the arcane words from $p.",ch,book,NULL,TO_CHAR);
    act("$n chants some arcane words from $p.",ch,book,NULL,TO_ROOM);
    if (IS_SET(page->quest, QUEST_MASTER_RUNE))
    {
	ch->spectype = 0;
	if (IS_SET(page->spectype, ADV_FAILED) || 
	    !IS_SET(page->spectype, ADV_FINISHED) || page->points < 1)
	    send_to_char( "The spell failed.\n\r", ch );
	else if (IS_SET(page->spectype, ADV_DAMAGE))
	    adv_spell_damage(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_AFFECT))
	    adv_spell_affect(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_ACTION))
	    adv_spell_action(ch,book,page,argument);
	else send_to_char( "The spell failed.\n\r", ch );
	return;
    }
    switch ( spellcount )
    {
    default:
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    case 40024:
	/* EARTH + DESTRUCTION + AREA */
	sn = skill_lookup( "earthquake" );
	spellno = 3;
	break;
    case 10022:
	/* FIRE + DESTRUCTION + TARGETING */
	sn = skill_lookup( "fireball" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 640322:
	/* LIFE + ENHANCEMENT + TARGETING */
	sn = skill_lookup( "heal" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 1280044:
	/* DEATH + SUMMONING + AREA */
	sn = skill_lookup( "guardian" );
	spellno = 3;
	break;
    case 2565128:
	/* MIND + INFORMATION + OBJECT */
	sn = skill_lookup( "identify" );
	object_target = TRUE;
	global_target = TRUE;
	break;
    case 5120328:
	/* SPIRIT + ENHANCEMENT + OBJECT */
	sn = skill_lookup( "enchant weapon" );
	object_target = TRUE;
	break;
    }
    if ( arg[0] == '\0' && (victim_target == TRUE || object_target == TRUE))
    {
	send_to_char( "Please specify a target.\n\r", ch );
	return;
    }
    if (victim_target && sn > 0)
    {
	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    return;
	}
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (object_target && sn > 0)
    {
	if ( !global_target && ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You are not carrying that object.\n\r", ch );
	    return;
	}
	else if ( global_target && ( obj = get_obj_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "You cannot find any object like that.\n\r", ch );
	    return;
	}
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (sn > 0)
    {
	spelltype = (skill_table[sn].target);
	if (spelltype == TAR_OBJ_INV)
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	WAIT_STATE(ch, skill_table[sn].beats);
    }
    else send_to_char( "Nothing happens.\n\r", ch );
    return;
}

OBJ_DATA *get_page( OBJ_DATA *book, int page_num )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;

    if (page_num < 1) return NULL;
    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == page_num) return page;
    }
    return NULL;
}

void adv_spell_damage( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *page_next;
    ROOM_INDEX_DATA *old_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int min = page->value[1];
    int max = page->value[2];
    int dam, max_dam = 1000;
    int level;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool reversed = FALSE;
    extern int chainspell;

    if ( ++chainspell > MAX_CHAIN(ch) ) return;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (min < 1 || max < 1)
    {send_to_char("The spell failed.\n\r",ch);return;}

    switch ( ch->class )
    {
	case CLASS_MAGE:       max_dam = 3000; break;
	case CLASS_SKINDANCER: max_dam = 2500; break;
	case CLASS_NEPHANDI:   max_dam = 2000; break;
	case CLASS_LICH:       max_dam = 1500; break;
    }
    max_dam += ( ch->spl[RED_MAGIC] << 2 );

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->victpoweron) > 0 || str_cmp(page->victpoweron,"(null)"))
    {
	if (strlen(page->victpoweroff) > 0 || str_cmp(page->victpoweroff,"(null)"))
	    cast_message = TRUE;
    }

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) reversed      = TRUE;

    if (victim_target)
    {
	CHAR_DATA *victim;

	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	if (ch->in_room == victim->in_room)
	    dam = number_range(min,max) + level;
	else
	    dam = number_range(min,max);
	if (ch->spectype < max_dam)
	{
	    ch->spectype += dam;
	    if (!IS_NPC(victim) && get_disc(victim,DISC_FORTITUDE) > 0 && 
		dam > 1 && !reversed)
		dam = number_range(1,dam);
	    else if (IS_ITEMAFF(victim, ITEMA_RESISTANCE) && dam > 1 && !reversed)
		dam = number_range(1,dam);
	    else if (!IS_NPC(victim) && (IS_SET(victim->act, PLR_DEMON) || 
		IS_SET(victim->act, PLR_CHAMPION)) && !reversed &&
		IS_SET(victim->pcdata->disc[C_POWERS], DEM_TOUGH) && dam > 1)
		dam = number_range(1,dam);
	    else if (!IS_NPC(victim) && IS_WEREWOLF(victim) && 
		victim->pcdata->disc[TOTEM_BOAR] > 2 && dam > 1 && !reversed)
		dam *= 0.5;
	    if (!IS_NPC(victim) && (IS_SET(victim->act,PLR_DEMON) || 
		IS_SET(victim->act,PLR_CHAMPION)) && victim->in_room != NULL && 
		victim->in_room->vnum == ROOM_VNUM_HELL && !reversed)
	    { if (dam < 5) dam = 1; else dam *= 0.2; }
	}
	else dam = 0;
	old_room = ch->in_room;
	if (victim->in_room != NULL && victim->in_room != ch->in_room)
	{
	    char_from_room(ch);
	    char_to_room(ch,victim->in_room);
	}
	if (!reversed)
	{
	    if (is_safe(ch,victim))
	    {
		char_from_room(ch);
		char_to_room(ch,old_room);
		return;
	    }
	}
	else if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    return;
	}
	char_from_room(ch);
	char_to_room(ch,old_room);
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,victim,TO_ROOM);
	}
	if (reversed)
	{
	    victim->hit += dam;
	    if (victim->hit > victim->max_hit) victim->hit = victim->max_hit;
	}
	else
	{
	    adv_damage(ch,victim,dam);
	    sprintf(buf,"Your %s strikes $N incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's %s strikes $N incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's %s strikes you incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_VICT);
	    if (!IS_NPC(victim) && number_percent() <= victim->pcdata->atm)
		act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, victim, TO_VICT );
	    else
		hurt_person(ch,victim,dam,FALSE);
	}
	WAIT_STATE(ch, 12);
	ch->mana -= mana_cost;
    }
    else if (area_affect)
    {
	CHAR_DATA *vch = ch;
	CHAR_DATA *vch_next;

	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	dam = number_range(min,max) + (level>>1);
	if (ch->spectype < max_dam)
	{
	    ch->spectype += dam;
	}
	else dam = 0;
	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !reversed)
	{
	    send_to_char("You cannot fight in this room.\n\r",ch);
	    return;
	}
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,vch,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,vch,TO_ROOM);
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next	= vch->next;
	    if ( vch->in_room == NULL ) continue;
	    if ( ch == vch && not_caster ) continue;
	    if ( !IS_NPC(vch) && no_players ) continue;
	    if (!reversed)
	    {
		if (is_safe(ch,vch)) continue;
	    }
	    if (IS_ITEMAFF(vch, ITEMA_REFLECT))
	    {
		send_to_char( "You are unable to focus your spell on them.\n\r", ch );
		continue;
	    }
	    if ( vch->in_room == ch->in_room )
	    {
		if (reversed)
		{
		    vch->hit += dam;
		    if (vch->hit > vch->max_hit) vch->hit = vch->max_hit;
		}
		else
		{
		    adv_damage(ch,vch,dam);
		    sprintf(buf,"Your %s strikes $N incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_CHAR);
		    sprintf(buf,"$n's %s strikes $N incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_NOTVICT);
		    sprintf(buf,"$n's %s strikes you incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_VICT);
		    if (!IS_NPC(vch) && number_percent() <= vch->pcdata->atm)
			act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, vch, TO_VICT );
		    else if (IS_ITEMAFF(vch, ITEMA_RESISTANCE) && dam > 1)
			hurt_person(ch,vch,number_range(1,dam), FALSE);
		    else hurt_person(ch,vch,dam,FALSE);
		}
		continue;
	    }
	}
	WAIT_STATE(ch, 12);
	ch->mana -= mana_cost;
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}
    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_affect( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    CHAR_DATA *victim = ch;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int apply_bit = page->value[1];
    int bonuses = page->value[2];
    int affect_bit = page->value[3];
    int sn;
    int level = page->level;
    bool any_affects = FALSE;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;
    extern int chainspell;

    if ( ++chainspell > MAX_CHAIN(ch) ) return;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) 
	{is_reversed = TRUE;bonuses = 0 - bonuses;}

    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if (page->toughness < PURPLE_MAGIC || page->toughness > YELLOW_MAGIC)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    if (page->toughness == PURPLE_MAGIC)
    {
	if ( ( sn = skill_lookup( "purple sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == RED_MAGIC)
    {
	if ( ( sn = skill_lookup( "red sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == BLUE_MAGIC)
    {
	if ( ( sn = skill_lookup( "blue sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == GREEN_MAGIC)
    {
	if ( ( sn = skill_lookup( "green sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == YELLOW_MAGIC)
    {
	if ( ( sn = skill_lookup( "yellow sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}

    if (!victim_target && victim != ch)
	{send_to_char("The spell failed.\n\r",ch);return;}
    if (not_caster && ch == victim)
	{send_to_char("The spell failed.\n\r",ch);return;}
    else if (no_players && !IS_NPC(victim))
	{send_to_char("The spell failed.\n\r",ch);return;}

    if ( is_affected( victim, sn ) )
    {
	send_to_char("They are already affected by a spell of that colour.\n\r",ch);
	return;
    }

    if ( apply_bit == 0 )
    {
	enhance_stat(sn,level,ch,victim,APPLY_NONE,bonuses,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_STR) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_STR,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DEX) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DEX,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_INT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_INT,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_WIS) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_WIS,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_CON) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_CON,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MANA) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MANA,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HIT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HIT,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MOVE) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MOVE,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_AC) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_AC,0 - (bonuses * 5),affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HITROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HITROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DAMROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DAMROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_SAVING_SPELL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_SAVING_SPELL,bonuses*0.2,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if (!any_affects) {send_to_char("The spell failed.\n\r",ch);return;}

    if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
    if (message_one)  act2(c_1,ch,NULL,victim,TO_VICT);
    if (message_two)  act2(c_2,ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE(ch,12);
    ch->mana -= mana_cost;

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_action( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg1 [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj2 = NULL;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *victim2 = NULL;
    ROOM_INDEX_DATA *old_room = ch->in_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int action_bit = page->value[1];
    int action_type = page->value[2];
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool second_victim = FALSE;
    bool second_object = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;
    extern int chainspell;

    if ( ++chainspell > MAX_CHAIN(ch) ) return;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_VICTIM)) second_victim = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_OBJECT)) second_object = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) is_reversed   = TRUE;

    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
 	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if ( arg2[0] == '\0' && (second_victim || second_object) )
    {
	send_to_char("Please specify a target.\n\r",ch);
	return;
    }
    else if (second_victim && victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_room( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL ||
	    victim2->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_here( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if (victim != NULL)
    {
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    if (victim2 != NULL)
    {
	if (victim2->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    switch ( action_bit )
    {
    default:
	send_to_char("The spell failed.\n\r",ch);
	return;
    case ACTION_MOVE:
	if (!victim_target && !second_victim && !object_target && !second_object)
	{
	    if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
	}
	else if ( arg1[0] == '\0' )
	{
	    send_to_char("Please specify a target.\n\r",ch);
	    return;
	}
	else if (victim_target && !second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL ||
		victim->in_room == ch->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,NULL,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,ch->in_room);
		if (message_two) act2(c_2,victim,NULL,NULL,TO_ROOM);
		do_look(victim,"");
	    }
	    else
	    {
		if (ch->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,victim->in_room);
		if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
		do_look(ch,"");
	    }
	}
	else if (!victim_target && !second_victim && object_target && !second_object)
	{
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == ch->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		obj_from_room(obj);
		obj_to_room(obj,ch->in_room);
	    }
	    else
	    {
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		do_look(ch,"");
	    }
	    if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
	}
	else if (victim_target && second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (victim2 == NULL || victim2->in_room == NULL ||
		victim2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim2,NULL,victim,TO_CHAR);
		if (message_one) act2(c_1,victim2,NULL,victim,TO_ROOM);
		char_from_room(victim2);
		char_to_room(victim2,victim->in_room);
		if (message_two) act2(c_2,victim2,NULL,victim,TO_ROOM);
		do_look(victim2,"");
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,victim2,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,victim2,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,victim2->in_room);
		if (message_two) act2(c_2,victim,NULL,victim2,TO_ROOM);
		do_look(victim,"");
	    }
	}
	else if (victim_target && !second_victim && !object_target && second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		obj_from_room(obj2);
		obj_to_room(obj2,victim->in_room);
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim);
		char_to_room(victim,obj2->in_room);
		do_look(victim,"");
	    }
	    if (message_two) act2(c_2,victim,obj2,NULL,TO_ROOM);
	}
	else if (!victim_target && !second_victim && object_target && second_object)
	{
	    if (obj == NULL || obj->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == obj->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (is_reversed)
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj2,NULL,TO_ROOM);
		obj_from_room(obj2);
		obj_to_room(obj2,obj->in_room);
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		if (message_two) act2(c_2,ch,obj2,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	    else
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
		obj_from_room(obj);
		obj_to_room(obj,obj2->in_room);
		char_from_room(ch);
		char_to_room(ch,obj2->in_room);
		if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	}
	else if (!victim_target && second_victim && object_target && !second_object)
	{
	    if (victim2 == NULL || victim2->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == victim2->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim2,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim2);
		char_to_room(victim2,obj->in_room);
		do_look(victim2,"");
	    }
	    else
	    {
		obj_from_room(obj);
		obj_to_room(obj,victim2->in_room);
	    }
	    if (message_two) act2(c_2,victim2,obj,NULL,TO_ROOM);
	}
	else {send_to_char("The spell failed.\n\r",ch);return;}
	break;
    case ACTION_MOB:
	if ( action_type < 1)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_NPC(ch) || ch->pcdata->followers > 4)
	{send_to_char("The spell failed.\n\r",ch);return;}
	switch ( action_type )
	{
	    default: break; /* No problem */
	    case MOB_VNUM_HOUND:
	    case MOB_VNUM_GUARDIAN:
	    case MOB_VNUM_MOUNT:
	    case MOB_VNUM_FROG:
	    case MOB_VNUM_RAVEN:
	    case MOB_VNUM_CAT:
	    case MOB_VNUM_DOG:
	    case MOB_VNUM_EYE:
	    case MOB_VNUM_MOLERAT:
	    case MOB_VNUM_YAK:
	    case MOB_VNUM_ASTRAL:
	    case MOB_VNUM_CLONE:
	    case 30002: /* Small gremlin */
	    case 30003: /* Medium gremlin */
	    case 30004: /* Large gremlin */
	    case 30005: /* Cute fluffy creature */
		send_to_char("The spell failed.\n\r",ch);
		return;
	}
	if (( victim = create_mobile( get_mob_index( action_type ))) == NULL )
	{send_to_char("The spell failed.\n\r",ch);return;}

	if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
	if (message_one) act2(c_1,ch,NULL,victim,TO_ROOM);
	ch->pcdata->followers++;
	char_to_room( victim, ch->in_room );
	SET_BIT(victim->act, ACT_NOEXP);
	free_string(victim->lord);
	victim->lord = str_dup(ch->name);
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    extract_char(victim, TRUE);
	    return;
	}
	break;
    case ACTION_OBJECT:
	if ( action_type < 1)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (( obj = create_object( get_obj_index( action_type ),0 )) == NULL )
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
	free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	obj_to_room( obj, ch->in_room );
	if (IS_SET(obj->quest, QUEST_ARTIFACT) || 
	    IS_SET(obj->spectype, SITEM_DEMONIC) ||
	    IS_SET(obj->extra_flags, ITEM_VANISH) ||
	    IS_SET(obj->quest, QUEST_RELIC))
	{
	    act("$p explodes, hurling you to the ground!",ch,obj,NULL,TO_CHAR);
	    act("$p explodes, hurling $n to the ground!",ch,obj,NULL,TO_ROOM);
	    extract_obj(obj);
	    ch->position = POS_STUNNED;
	    return;
	}
	break;
    }
    WAIT_STATE(ch,12);
    ch->mana -= mana_cost;

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void do_familiar( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    return;

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to make your familiar?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Become your own familiar?\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
	familiar->wizard = NULL;
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    send_to_char("Ok.\n\r",ch);

    return;
}

void do_fcommand( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && ch->level < LEVEL_APPRENTICE)
    {
	if (!IS_DEMON(ch))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }

    if ( ( victim = ch->pcdata->familiar ) == NULL )
    {
	if (IS_VAMPIRE(ch) || IS_GHOUL(ch))
	    send_to_char( "Huh?\n\r", ch );
	else if (ch->level >= LEVEL_APPRENTICE)
	    send_to_char( "But you don't have a familiar!\n\r", ch );
	else
	    send_to_char( "But you don't have an eye spy!\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	if (IS_VAMPIRE(ch) || IS_GHOUL(ch))
	    send_to_char( "Please specify a command.\n\r", ch );
	else if (ch->level >= LEVEL_APPRENTICE)
	    send_to_char( "What do you wish to make your familiar do?\n\r", ch );
	else
	    send_to_char( "What do you wish to make your eye spy do?\n\r", ch );
	return;
    }

    if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
    interpret(victim,argument);
    if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);

    if (is_in(argument,"|kick*")) WAIT_STATE(ch, 8);
    return;
}

void do_flex( CHAR_DATA *ch, char *argument )
{
    int eff_str = get_curr_str(ch) * 3;
    if (IS_NPC(ch)) eff_str = ch->level;
    act("You flex your bulging muscles.",ch,NULL,NULL,TO_CHAR);
    act("$n flexes $s bulging muscles.",ch,NULL,NULL,TO_ROOM);

    if (!IS_NPC(ch) && get_disc(ch,DISC_POTENCE) > 0)
	eff_str += get_disc(ch,DISC_POTENCE) * 10;

    if (eff_str < 100)
    {
	if (!IS_NPC(ch) && IS_DEMON(ch))
	{
	    if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT))
		return;
	}
	else if (!IS_WEREWOLF(ch) || ch->pcdata->disc[TOTEM_BEAR] < 1)
	    return;
    }

    if ( IS_EXTRA(ch, TIED_UP) )
    {
	act("The ropes restraining you snap.",ch,NULL,NULL,TO_CHAR);
	act("The ropes restraining $n snap.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->extra, TIED_UP);
    }
    if (is_affected(ch, gsn_web))
    {
	act("The webbing entrapping $n breaks away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you breaks away.\n\r",ch);
	affect_strip(ch, gsn_web);
    }
    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	act("The webbing entrapping $n breaks away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you breaks away.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_WEBBED);
    }
    if (is_affected(ch, gsn_tendrils))
    {
	act("The tendrils of darkness entrapping $n break away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you break away.\n\r",ch);
	affect_strip(ch, gsn_tendrils);
    }
    if (IS_AFFECTED(ch, AFF_SHADOW))
    {
	act("The tendrils of darkness entrapping $n break away.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you break away.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_SHADOW);
    }
    if (is_affected(ch, gsn_jail))
    {
	act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	affect_strip(ch, gsn_jail);
    }
    if (IS_AFFECTED(ch, AFF_JAIL))
    {
	act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_JAIL);
    }
    WAIT_STATE(ch,12);
    return;
}

void do_rage( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_SET(ch->act, PLR_WOLFMAN) && ch->pcdata->wolf < 150)
    {
	send_to_char("You start snarling angrilly.\n\r",ch);
	act("$n starts snarling angrilly.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->wolf += number_range(10,20);
	if (ch->pcdata->wolf >= 100) do_werewolf(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
    send_to_char("But you are already in a rage!\n\r",ch);
    return;
}

void do_calm( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_WEREWOLF(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_SET(ch->act, PLR_WOLFMAN))
    {
	send_to_char("You take a deep breath and calm yourself.\n\r",ch);
	act("$n takes a deep breath and tries to calm $mself.",ch,NULL,NULL,TO_ROOM);
	if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) > 0) ch->pcdata->wolf = 0;
	else ch->pcdata->wolf -= number_range(10,20);
	if (ch->pcdata->wolf < 100) do_unwerewolf(ch,"");
	WAIT_STATE(ch,12);
	return;
    }
    send_to_char("But you are not in crinos form!\n\r",ch);
    return;
}

void do_web( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_WEREWOLF(ch) || ch->pcdata->disc[TOTEM_SPIDER] < 2)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot web yourself.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot web an ethereal person.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "web" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = ch->spl[spelltype] * 0.25;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return;
}

void do_entrancement( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_PRESENCE) < 3)
    {
	send_to_char("You require level 3 Presence to use Entrancement.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if ( IS_NPC(victim) && ch->pcdata->followers > 4 )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( IS_AFFECTED(victim, AFF_CHARM) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( !IS_AFFECTED(victim, AFF_CHARM) )
    {
	act("You stare deeply into $N's eyes.",ch,NULL,victim,TO_CHAR);
	act("$n stares deeply into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
	act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);
    }

    if ( ( sn = skill_lookup( "charm person" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = get_disc(ch,DISC_PRESENCE) * 10;

    if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) &&
	get_disc(victim,DISC_PRESENCE) >= get_disc(ch,DISC_PRESENCE) )
    {
	act("$N shrugs off your attempt to entrance $M.",ch,NULL,victim,TO_CHAR);
	act("$N shrugs off $n's attempt to entrance $M.",ch,NULL,victim,TO_NOTVICT);
	act("You shrug off $n's attempt to entrance you.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Entrancement on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0)
    {
	act("$N shrugs off your attempt to entrance $M.",ch,NULL,victim,TO_CHAR);
	act("$N shrugs off $n's attempt to entrance $M.",ch,NULL,victim,TO_NOTVICT);
	act("You shrug off $n's attempt to entrance you.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Entrancement on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Entrancement.\n\r",victim);
	act("$N shrugs off your attempt to entrance $M.",ch,NULL,victim,TO_CHAR);
	act("$N shrugs off $n's attempt to entrance $M.",ch,NULL,victim,TO_NOTVICT);
	act("You shrug off $n's attempt to entrance you.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Entrancement on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );

    if ( !IS_AFFECTED(victim, AFF_CHARM) )
    {
	act("$N shrugs off your attempt to entrance $M.",ch,NULL,victim,TO_CHAR);
	act("$N shrugs off $n's attempt to entrance $M.",ch,NULL,victim,TO_NOTVICT);
	act("You shrug off $n's attempt to entrance you.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Entrancement on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("$N gazes lovingly back at $n.",ch,NULL,victim,TO_NOTVICT);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Entrancement on you.",ch,NULL,victim,TO_VICT);
    }

    if (IS_NPC(victim))
    {
	free_string(victim->lord);
	victim->lord = str_dup(ch->name);
	ch->pcdata->followers++;
    }
    return;
}

void do_summoning( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_PRESENCE) < 4)
    {
	send_to_char("You require level 4 Presence to use Summoning.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   ch->in_room == NULL
    ||   IS_NPC(victim) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->fighting != NULL
    ||   !IS_IMMUNE(victim, IMM_SUMMON) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Summoning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
    act( "$N has summoned you!", victim, NULL, ch,   TO_CHAR );
    do_look( victim, "auto" );
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Summoning on you.",ch,NULL,victim,TO_VICT);
    }
    if ( (mount = victim->mount) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, get_room_index(victim->in_room->vnum) );
    do_look( mount, "auto" );
    return;
}

void do_pray( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf [MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    
    act("You mutter a few prayers.",ch,NULL,NULL,TO_CHAR);
    act("$n mutters a quick prayer.",ch,NULL,NULL,TO_ROOM);

    if (IS_NPC(ch)) return;

    if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_ALTAR && 
	ch->class == CLASS_NONE)
    {
	send_to_char("KaVir has restored you.\n\r",ch);
	ch->agg  = 0;
	ch->hit  = ch->max_hit;
	ch->mana = ch->max_mana;
	ch->move = ch->max_move;
	ch->loc_hp[0] = 0;
	ch->loc_hp[1] = 0;
	ch->loc_hp[2] = 0;
	ch->loc_hp[3] = 0;
	ch->loc_hp[4] = 0;
	ch->loc_hp[5] = 0;
	ch->loc_hp[6] = 0;
	update_pos( ch );
    }

    if (!IS_IMMORTAL(ch) && !IS_DEMON(ch)) return;

    if (argument[0] == '\0' && (IS_IMMORTAL(ch) || IS_SET(ch->act, PLR_DEMON)))
    {
	send_to_char("What do you wish to pray?\n\r",ch);
	return;
    }
    else if ( argument[0] == '\0' )
    {
	if (ch->pcdata->power[POWER_CURRENT] < 1)
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	if ( ( victim = get_char_world(ch, ch->lord) ) == NULL )
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	act("You hear $n's prayers in your mind.",ch,NULL,victim,TO_VICT);
	send_to_char("You feel energy pour into your body.\n\r",victim);
	if (ch->pcdata->power[POWER_CURRENT] == 1)
	    sprintf(buf,"You receive a single point of energy.\n\r");
	else
	    sprintf(buf,"You receive %d points of energy.\n\r",
		ch->pcdata->power[POWER_CURRENT]);
	send_to_char(buf,victim);
	act("$n is briefly surrounded by a halo of energy.",victim,NULL,NULL,TO_ROOM);
	victim->pcdata->power[POWER_CURRENT] += ch->pcdata->power[POWER_CURRENT];
	victim->pcdata->power[POWER_TOTAL] += ch->pcdata->power[POWER_CURRENT];
	ch->pcdata->power[POWER_CURRENT] = 0;
	return;
    }
    if (IS_SET(ch->deaf, CHANNEL_PRAY))
    {
	send_to_char("But you're not even on the channel!\n\r",ch);
	return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (victim = d->character) == NULL ) continue;
	if ( IS_NPC(victim) || !IS_DEMON(victim) ) continue;
	if ( IS_SET(victim->deaf, CHANNEL_PRAY) ) continue;
	if ( victim == ch )
	    act("You pray '$T'.", ch, NULL, argument, TO_CHAR);
	else
	{
	    sprintf(buf,"$n prays '%s'.", argument);
	    act(buf, ch, NULL, victim, TO_VICT);
	}
    }
    return;
}

void do_birth( CHAR_DATA *ch, char *argument )
{
    char buf2[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_EXTRA(ch, EXTRA_PREGNANT))
    {
	send_to_char("But you are not even pregnant!\n\r",ch);
	return;
    }

    if (!IS_EXTRA(ch, EXTRA_LABOUR))
    {
	send_to_char("You're not ready to give birth yet.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	if (ch->pcdata->genes[4] == SEX_MALE)
	    send_to_char( "What do you wish to name your little boy?\n\r", ch );
	else if (ch->pcdata->genes[4] == SEX_FEMALE)
	    send_to_char( "What do you wish to name your little girl?\n\r", ch );
	else
	    send_to_char( "What do you wish to name your child?\n\r", ch );
	return;
    }

    if (!check_parse_name( argument ))
    {
	send_to_char( "Thats an illegal name.\n\r", ch );
	return;
    }

    if ( char_exists(FILE_PLAYER,argument) )
    {
	send_to_char( "That player already exists.\n\r", ch );
	return;
    }

    strcpy(buf2,ch->pcdata->cparents);
    strcat(buf2," ");
    strcat(buf2,argument);
    if (!birth_ok(ch, buf2))
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    argument[0] = UPPER(argument[0]);
    birth_write( ch, argument );
    ch->pcdata->genes[9] += 1;
    REMOVE_BIT(ch->extra, EXTRA_PREGNANT);
    REMOVE_BIT(ch->extra, EXTRA_LABOUR);
    do_autosave(ch,"");
    return;
}

bool birth_ok( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char mum [MAX_INPUT_LENGTH];
    char dad [MAX_INPUT_LENGTH];
    char child [MAX_INPUT_LENGTH];

    argument = one_argument( argument, mum );
    argument = one_argument( argument, dad );
    argument = one_argument( argument, child );

    if (dad[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    if (child[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    dad[0] = UPPER(dad[0]);
    if (!str_cmp(dad,"Kavir")) strcpy(dad,"KaVir");
    child[0] = UPPER(child[0]);
    if (ch->pcdata->genes[4] == SEX_MALE)
    {
	send_to_char("You give birth to a little boy!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's son, named %s!",ch->name,dad,child);
	do_info(ch,buf);
	return TRUE; 
    }
    else if (ch->pcdata->genes[4] == SEX_FEMALE)
    {
	send_to_char("You give birth to a little girl!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's daughter, named %s!",ch->name,dad,child);
	do_info(ch,buf);
	return TRUE; 
    }
    return FALSE;
}


bool char_exists( int file_dir, char *argument )
{
    FILE *fp;
    char buf [MAX_STRING_LENGTH];
    bool found = FALSE;

    fflush( fpReserve );
    fclose( fpReserve );
    switch ( file_dir )
    {
	default:
	    return FALSE;
	    break;
	case FILE_PLAYER:
	    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( argument ) );
	    break;
    }
    if ( ( fp = fopen( buf, "r" ) ) != NULL )
    {
	found = TRUE;
	fclose( fp );
	fflush( fp );
	fpReserve = fopen( NULL_FILE, "r" );
    }
    return found;
}

void birth_write( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    char buf [MAX_STRING_LENGTH];
    char *strtime;

    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    fflush( fpReserve );
    fclose( fpReserve );
    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( argument ) );
    if ( ( fp = fopen(buf, "w") ) == NULL) 
    {
	bug( "Birth_write: fopen", 0 );
	perror( buf );
    }
    else
    {  
	fprintf( fp, "#PLAYER\n");
	fprintf( fp, "Name         %s~\n",capitalize( argument ));
	fprintf( fp, "Createtime   %s~\n",str_dup( strtime )	);
	fprintf( fp, "Sex          %d\n",ch->pcdata->genes[4]	);
	fprintf( fp, "Race         0\n"				);
	fprintf( fp, "Immune       %d\n",ch->pcdata->genes[3]	);
	fprintf( fp, "Level        2\n"				);
	fprintf( fp, "Room         %d\n",ch->in_room->vnum 	);
	fprintf( fp, "PkPdMkMd     0 0 5 0\n"			);
	fprintf( fp, "HpManaMove   %d %d %d %d %d %d\n",
	    ch->pcdata->genes[0], ch->pcdata->genes[0], ch->pcdata->genes[1], 
	    ch->pcdata->genes[1], ch->pcdata->genes[2], ch->pcdata->genes[2]);
	fprintf( fp, "Act          1600\n"   			);
	fprintf( fp, "Extra        32768\n"   			);
	fprintf( fp, "Position     7\n"				);
	fprintf( fp, "Password     %s~\n",ch->pcdata->pwd	);
	fprintf( fp, "Title         the mortal~\n"		);
	fprintf( fp, "Parents      %s~\n",ch->pcdata->cparents	);
	fprintf( fp, "AttrPerm     %d %d %d %d %d\n",
	    ch->pcdata->perm_str,
	    ch->pcdata->perm_int,
	    ch->pcdata->perm_wis,
	    ch->pcdata->perm_dex,
	    ch->pcdata->perm_con );
	fprintf( fp, "End\n\n" );
	fprintf( fp, "#END\n" );
    }
    fflush( fp );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_teach( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_MAGE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->level == LEVEL_APPRENTICE)
    {
	send_to_char("You don't know enough to teach another.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Teach whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot teach yourself.\n\r", ch );
	return;
    }

    if (IS_MAGE(victim))
    {
	send_to_char( "They are already a mage.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only teach avatars.\n\r", ch );
	return;
    }

    if (IS_VAMPIRE(victim))
    {
	send_to_char( "You are unable to teach vampires!\n\r", ch );
	return;
    }

    if (IS_WEREWOLF(victim))
    {
	send_to_char( "You are unable to teach werewolves!\n\r", ch );
	return;
    }

    if (IS_DEMON(victim))
    {
	send_to_char( "You are unable to teach demons!\n\r", ch );
	return;
    }

    if (IS_HIGHLANDER(victim))
    {
	send_to_char( "You are unable to teach highlanders.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "You cannot teach an unwilling person.\n\r", ch );
	return;
    }

    if (ch->exp < 100000)
    {
	send_to_char("You cannot afford the 100000 exp required to teach them.\n\r",ch);
	return;
    }

    if (victim->exp < 100000)
    {
	send_to_char("They cannot afford the 100000 exp required to learn from you.\n\r",ch);
	return;
    }

    ch->exp -= 100000;
    victim->exp -= 100000;

    act("You teach $N the basics of magic.", ch, NULL, victim, TO_CHAR);
    act("$n teaches $N the basics of magic.", ch, NULL, victim, TO_NOTVICT);
    act("$n teaches you the basics of magic.", ch, NULL, victim, TO_VICT);
    victim->level = LEVEL_APPRENTICE;
    victim->trust = LEVEL_APPRENTICE;
    victim->class = CLASS_MAGE;
    send_to_char( "You are now an apprentice.\n\r", victim );
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    victim->pcdata->runes[0] = ch->pcdata->runes[0];
    do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}

void do_earthmeld( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && get_disc(ch, DISC_PROTEAN) < 3)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (get_disc(ch,DISC_PROTEAN) < 3)
    {
	send_to_char("You require level 3 Protean to use Earthmeld.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_EARTHMELDED) )
    {
	REMOVE_BIT(ch->vampaff, VAM_EARTHMELDED);
	send_to_char("You rise up from the ground.\n\r",ch);
	act("$n rises up from the ground.", ch, NULL, NULL, TO_ROOM);
	return;
    }
    send_to_char("You sink into the ground.\n\r",ch);
    act("$n sinks into the ground.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_EARTHMELDED);
    REMOVE_BIT(ch->affected_by, AFF_FLAMING);
    REMOVE_BIT(ch->more, MORE_FLAMES);
    return;
}

void do_powers( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    int sn;
    bool any_powers = FALSE;
    bool any_commands = FALSE;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	if (IS_WEREWOLF(ch))
	    do_wwpowers(ch, argument);
	else
	    send_to_char("Huh?\n\r",ch);
	return;
    }

    for ( sn = 0; sn <= DISC_MAX; sn++ )
    {
	if ( get_disc(ch,sn) > 0 ) 
	{
	    any_powers = TRUE;
	    switch (sn)
	    {
		default: break;
		case DISC_ANIMALISM:
		case DISC_AUSPEX:
		case DISC_CHIMERSTRY:
		case DISC_CELERITY:
		case DISC_DAIMOINON:
		case DISC_DOMINATE:
		case DISC_MELPOMINEE:
		case DISC_NECROMANCY:
		case DISC_OBEAH:
		case DISC_OBFUSCATE:
		case DISC_OBTENEBRATION:
		case DISC_PRESENCE:
		case DISC_PROTEAN:
		case DISC_QUIETUS:
		case DISC_SERPENTIS:
		case DISC_THANATOSIS:
		case DISC_THAUMATURGY:
		case DISC_VICISSITUDE:
		    any_commands = TRUE;
		    break;
	    }
	}
    }
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char("                            -= Discipline Commands =-\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    if (get_disc(ch,DISC_ANIMALISM) > 0)
    {
	strcpy(buf,"Animalism: Pigeon");
	if (get_disc(ch,DISC_ANIMALISM) > 1) strcat(buf," Beckon");
	if (get_disc(ch,DISC_ANIMALISM) > 2) strcat(buf," Song");
	if (get_disc(ch,DISC_ANIMALISM) > 3) strcat(buf," Share");
	if (get_disc(ch,DISC_ANIMALISM) > 4) strcat(buf," Beast");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_AUSPEX) > 0)
    {
	strcpy(buf,"Auspex: Heighten");
	if (get_disc(ch,DISC_AUSPEX) > 1) strcat(buf," Readaura");
	if (get_disc(ch,DISC_AUSPEX) > 2) strcat(buf," Unveil");
	if (get_disc(ch,DISC_AUSPEX) > 3) strcat(buf," Astral");
	if (get_disc(ch,DISC_AUSPEX) > 4) strcat(buf," Scry");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_CELERITY) > 0)
	send_to_char("Celerity: Celerity.\n\r", ch);
    if (get_disc(ch,DISC_CHIMERSTRY) > 0)
    {
	strcpy(buf,"Chimerstry: Mirror");
	if (get_disc(ch,DISC_CHIMERSTRY) > 1) strcat(buf," Formillusion");
	if (get_disc(ch,DISC_CHIMERSTRY) > 2) strcat(buf," Clone");
	if (get_disc(ch,DISC_CHIMERSTRY) > 3) strcat(buf," Falseappearance");
	if (get_disc(ch,DISC_CHIMERSTRY) > 4) strcat(buf," Control");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_DAIMOINON) > 0)
    {
	strcpy(buf,"Daimoinon: Guardian");
	if (get_disc(ch,DISC_DAIMOINON) > 1) strcat(buf," Fear");
	if (get_disc(ch,DISC_DAIMOINON) > 2) strcat(buf," Portal");
	if (get_disc(ch,DISC_DAIMOINON) > 3) strcat(buf," Curse");
	if (get_disc(ch,DISC_DAIMOINON) > 4) strcat(buf," Twist");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_DOMINATE) > 0)
    {
	strcpy(buf,"Dominate: Command");
	if (get_disc(ch,DISC_DOMINATE) > 1) strcat(buf," Condition");
	if (get_disc(ch,DISC_DOMINATE) > 2) strcat(buf," Possession");
	if (get_disc(ch,DISC_DOMINATE) > 3) strcat(buf," Loyalty");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_MELPOMINEE) > 0)
    {
	strcpy(buf,"Melpominee: Scream");
	if (get_disc(ch,DISC_MELPOMINEE) > 1) strcat(buf," Reina");
	if (get_disc(ch,DISC_MELPOMINEE) > 2) strcat(buf," Courage");
	if (get_disc(ch,DISC_MELPOMINEE) > 3) strcat(buf," Love");
	if (get_disc(ch,DISC_MELPOMINEE) > 4) strcat(buf," Anger");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_NECROMANCY) > 0)
    {
	strcpy(buf,"Necromancy: Thanatopsis");
	if (get_disc(ch,DISC_NECROMANCY) > 1) strcat(buf," Preserve");
	if (get_disc(ch,DISC_NECROMANCY) > 2) strcat(buf," Spiritgate");
	if (get_disc(ch,DISC_NECROMANCY) > 3) strcat(buf," Spiritguardian");
	if (get_disc(ch,DISC_NECROMANCY) > 4) strcat(buf," Zombie");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_OBEAH) != 0)
    {
	strcpy(buf,"Obeah: Obeah Panacea");
	if (get_disc(ch,DISC_OBEAH) > 1) strcat(buf," Anesthetic");
	if (get_disc(ch,DISC_OBEAH) > 2) strcat(buf," Neutral");
	if (get_disc(ch,DISC_OBEAH) > 3) strcat(buf," Unburden");
	if (get_disc(ch,DISC_OBEAH) > 4) strcat(buf," Renew");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_OBFUSCATE) > 0)
    {
	strcpy(buf,"Obfuscate: Vanish");
	if (get_disc(ch,DISC_OBFUSCATE) > 1) strcat(buf," Mask");
	if (get_disc(ch,DISC_OBFUSCATE) > 3) strcat(buf," Shield");
	if (get_disc(ch,DISC_OBFUSCATE) > 4) strcat(buf," Soulmask");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_OBTENEBRATION) > 0)
    {
	strcpy(buf,"Obtenebration: Shroud");
	if (get_disc(ch,DISC_OBTENEBRATION) > 1) strcat(buf," Arms");
	if (get_disc(ch,DISC_OBTENEBRATION) > 2) strcat(buf," Shadowbody");
	if (get_disc(ch,DISC_OBTENEBRATION) > 3) strcat(buf," Lamprey");
	if (get_disc(ch,DISC_OBTENEBRATION) > 4) strcat(buf," Shadowstep");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_PRESENCE) > 0)
    {
	strcpy(buf,"Presence: Awe");
	if (get_disc(ch,DISC_PRESENCE) > 1) strcat(buf," Dreadgaze");
	if (get_disc(ch,DISC_PRESENCE) > 2) strcat(buf," Entrance");
	if (get_disc(ch,DISC_PRESENCE) > 3) strcat(buf," Summoning");
	if (get_disc(ch,DISC_PRESENCE) > 4) strcat(buf," Majesty");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_PROTEAN) > 0)
    {
	strcpy(buf,"Protean: Nightsight");
	if (get_disc(ch,DISC_PROTEAN) > 1) strcat(buf," Claws");
	if (get_disc(ch,DISC_PROTEAN) > 2) strcat(buf," Earthmeld");
	if (get_disc(ch,DISC_PROTEAN) > 3) strcat(buf," Change");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_QUIETUS) > 0)
    {
	strcpy(buf,"Quietus: Silence");
	if (get_disc(ch,DISC_QUIETUS) > 1) strcat(buf," Weakness");
	if (get_disc(ch,DISC_QUIETUS) > 2) strcat(buf," Infirmity");
	if (get_disc(ch,DISC_QUIETUS) > 3) strcat(buf," Bloodagony");
	if (get_disc(ch,DISC_QUIETUS) > 4) strcat(buf," Spit");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_SERPENTIS) > 0)
    {
	strcpy(buf,"Serpentis: Tongue");
	if (get_disc(ch,DISC_SERPENTIS) > 1) strcat(buf," Mummify");
	if (get_disc(ch,DISC_SERPENTIS) > 2) strcat(buf," Serpent");
	if (get_disc(ch,DISC_SERPENTIS) > 3) strcat(buf," Darkheart");
	if (get_disc(ch,DISC_SERPENTIS) > 4) strcat(buf," Breath");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_THANATOSIS) > 0)
    {
	strcpy(buf,"Thanatosis: Hagswrinkles");
	if (get_disc(ch,DISC_THANATOSIS) > 1) strcat(buf," Putrefaction");
	if (get_disc(ch,DISC_THANATOSIS) > 2) strcat(buf," Ashes");
	if (get_disc(ch,DISC_THANATOSIS) > 3) strcat(buf," Withering");
	if (get_disc(ch,DISC_THANATOSIS) > 4) strcat(buf," Drainlife");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }

    if (get_disc(ch,DISC_THAUMATURGY) > 0)
    {
	strcpy(buf,"Thaumaturgy: Taste");
	if (get_disc(ch,DISC_THAUMATURGY) > 1) strcat(buf," Bloodrage");
	if (get_disc(ch,DISC_THAUMATURGY) > 2) strcat(buf," Potency");
	if (get_disc(ch,DISC_THAUMATURGY) > 3) strcat(buf," Theft");
	if (get_disc(ch,DISC_THAUMATURGY) > 4) strcat(buf," Cauldron");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (get_disc(ch,DISC_VICISSITUDE) > 0)
    {
	strcpy(buf,"Vicissitude: Changeling");
	if (get_disc(ch,DISC_VICISSITUDE) > 1) strcat(buf," Fleshcraft");
	if (get_disc(ch,DISC_VICISSITUDE) > 2) strcat(buf," Bonecraft");
	if (get_disc(ch,DISC_VICISSITUDE) > 3) strcat(buf," Zuloform");
	if (get_disc(ch,DISC_VICISSITUDE) > 4) strcat(buf," Plasmaform");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
    }
    if (!any_powers)
	send_to_char("You have no disciplines.\n\r",ch);
    else if (!any_commands)
	send_to_char("All your discipline powers are automatic.\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    return;
}

void do_dreadgaze( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_PRESENCE) < 2)
    {
	send_to_char("You require level 2 Presence to use Dread Gaze.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Dread Gaze on?\n\r", ch );
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

    if (is_safe(ch,victim)) return;

    if ( IS_AFFECTED(victim, AFF_FEAR) )
    {
	send_to_char( "They are already pretty scared.\n\r", ch );
	return;
    }

    act("You bare your fangs and snarl at $N.",ch,NULL,victim,TO_CHAR);
    act("$n bares $s fangs and snarls at $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n bares $s fangs and snarls at you.",ch,NULL,victim,TO_VICT);

    if ( IS_MORE(victim, MORE_COURAGE) )
    {
	send_to_char( "They are far too brave to be scared so easily.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dread Gaze on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) && 
	get_disc(victim,DISC_PRESENCE) >= get_disc(ch,DISC_PRESENCE))
    {
	send_to_char( "Your Dread Gaze has no effect on them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dread Gaze on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0)
    {
	send_to_char( "Your Dread Gaze has no effect on them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dread Gaze on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char( "Your Dread Gaze has no effect on them.\n\r", ch );
	send_to_char("You burn some willpower to resist Dread Gaze.\n\r",victim);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Dread Gaze on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "fear" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = get_disc(ch,DISC_PRESENCE) * 5;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Dread Gaze on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_vanish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBFUSCATE) < 1)
    {
	send_to_char("You require level 1 Obfuscate to Vanish from sight.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (get_disc(ch,DISC_OBFUSCATE) >= 3 && !str_cmp(arg,"all"))
    {
	send_to_char("You Cloak the Gathering.\n\r",ch);
	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( vch == ch || vch->leader == ch )
	    {
		SET_BIT( vch->vampaff, VAM_OBFUSCATE );
		act( "You fade away into the shadows.", vch, NULL, NULL, TO_CHAR );
		act( "$n fades away into the shadows.", vch, NULL, NULL, TO_ROOM );
	    }
	}
    }
    else
    {
	SET_BIT( ch->vampaff, VAM_OBFUSCATE );
	act( "You fade away into the shadows.", ch, NULL, NULL, TO_CHAR );
	act( "$n fades away into the shadows.", ch, NULL, NULL, TO_ROOM );
    }
    WAIT_STATE( ch, 12 );
    return;
}

void do_discipline( CHAR_DATA *ch, char *argument )
{
    char arg  [MAX_INPUT_LENGTH];
    char buf  [MAX_INPUT_LENGTH];
    char disc [25];
    int  improve;
    int  cost;
    int  max_d = (IS_ABOMINATION(ch) || IS_LICH(ch)) ? 4 : 5;
    int  sn, col;
    bool got_disc = FALSE;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("                               -= Disciplines =-\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	col    = 0;
	for ( sn = 0; sn <= DISC_MAX; sn++ )
	{
	    if ( ch->pcdata->powers[sn] == 0 ) continue;
	    switch (sn)
	    {
		default: 
		    strcpy(disc,"None"); 
		    break;
		case DISC_ANIMALISM: 
		    strcpy(disc,"Animalism"); 
		    break;
		case DISC_AUSPEX: 
		    strcpy(disc,"Auspex"); 
		    break;
		case DISC_CELERITY: 
		    strcpy(disc,"Celerity"); 
		    break;
		case DISC_CHIMERSTRY: 
		    strcpy(disc,"Chimerstry"); 
		    break;
		case DISC_DAIMOINON: 
		    strcpy(disc,"Daimoinon"); 
		    break;
		case DISC_DOMINATE: 
		    strcpy(disc,"Dominate"); 
		    break;
		case DISC_FORTITUDE: 
		    strcpy(disc,"Fortitude"); 
		    break;
		case DISC_MELPOMINEE: 
		    strcpy(disc,"Melpominee"); 
		    break;
		case DISC_NECROMANCY: 
		    strcpy(disc,"Necromancy"); 
		    break;
		case DISC_OBEAH: 
		    strcpy(disc,"Obeah"); 
		    break;
		case DISC_OBFUSCATE: 
		    strcpy(disc,"Obfuscate"); 
		    break;
		case DISC_OBTENEBRATION: 
		    strcpy(disc,"Obtenebration"); 
		    break;
		case DISC_POTENCE: 
		    strcpy(disc,"Potence"); 
		    break;
		case DISC_PRESENCE: 
		    strcpy(disc,"Presence"); 
		    break;
		case DISC_PROTEAN: 
		    strcpy(disc,"Protean"); 
		    break;
		case DISC_QUIETUS: 
		    strcpy(disc,"Quietus"); 
		    break;
		case DISC_SERPENTIS: 
		    strcpy(disc,"Serpentis"); 
		    break;
		case DISC_THANATOSIS: 
		    strcpy(disc,"Thanatosis"); 
		    break;
		case DISC_THAUMATURGY: 
		    strcpy(disc,"Thaumaturgy"); 
		    break;
		case DISC_VICISSITUDE: 
		    strcpy(disc,"Vicissitude"); 
		    break;
	    }
	    if (ch->class == CLASS_VAMPIRE && (ch->pcdata->cland[0] == sn ||
		ch->pcdata->cland[1] == sn || ch->pcdata->cland[2] == sn))
		capitalize( disc );

	    sprintf( buf, "%-15s: %-2d       ",
		disc, get_disc(ch, sn) );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	    got_disc = TRUE;
	}
	if ( col % 3 != 0 ) send_to_char( "\n\r", ch );
	if (!got_disc) send_to_char("You currently have no disciplines.\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("To improve a discipline rating, type: discipline <name>\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	return;
    }

    if (!str_cmp(arg,"animalism")) improve = DISC_ANIMALISM;
    else if (!str_cmp(arg,"auspex")) improve = DISC_AUSPEX;
    else if (!str_cmp(arg,"celerity")) improve = DISC_CELERITY;
    else if (!str_cmp(arg,"chimerstry")) improve = DISC_CHIMERSTRY;
    else if (!str_cmp(arg,"daimoinon")) improve = DISC_DAIMOINON;
    else if (!str_cmp(arg,"dominate")) improve = DISC_DOMINATE;
    else if (!str_cmp(arg,"fortitude")) improve = DISC_FORTITUDE;
    else if (!str_cmp(arg,"melpominee")) improve = DISC_MELPOMINEE;
    else if (!str_cmp(arg,"necromancy")) improve = DISC_NECROMANCY;
    else if (!str_cmp(arg,"obeah")) improve = DISC_OBEAH;
    else if (!str_cmp(arg,"obfuscate")) improve = DISC_OBFUSCATE;
    else if (!str_cmp(arg,"obtenebration")) improve = DISC_OBTENEBRATION;
    else if (!str_cmp(arg,"potence")) improve = DISC_POTENCE;
    else if (!str_cmp(arg,"presence")) improve = DISC_PRESENCE;
    else if (!str_cmp(arg,"protean")) improve = DISC_PROTEAN;
    else if (!str_cmp(arg,"quietus")) improve = DISC_QUIETUS;
    else if (!str_cmp(arg,"serpentis")) improve = DISC_SERPENTIS;
    else if (!str_cmp(arg,"thanatosis")) improve = DISC_THANATOSIS;
    else if (!str_cmp(arg,"thaumaturgy")) improve = DISC_THAUMATURGY;
    else if (!str_cmp(arg,"vicissitude")) improve = DISC_VICISSITUDE;
    else
    {
	send_to_char("You know of no such discipline.\n\r",ch);
	return;
    }

    if (ch->pcdata->powers[improve] == 0)
    {
	send_to_char("You know of no such discipline.\n\r",ch);
	return;
    }

    if (ch->pcdata->powers[improve] >= max_d)
    {
	send_to_char("You are unable to improve your ability in that discipline any further.\n\r",ch);
	return;
    }

    if ( IS_ABOMINATION(ch) || IS_LICH(ch) )
	cost = 15;
    else if (ch->pcdata->cland[0] == improve || 
	ch->pcdata->cland[1] == improve || 
	ch->pcdata->cland[2] == improve)
    {
	if (ch->pcdata->powers[improve] == -1) 
	    cost = 5;
	else
	    cost = ( ch->pcdata->powers[improve] + 1 ) * 5;
    }
    else
    {
	if (ch->pcdata->powers[improve] == -1) 
	    cost = 10;
	else
	    cost = ( ch->pcdata->powers[improve] + 1 ) * 10;
    }

    if (ch->practice < cost)
    {
	sprintf(buf,"You need another %d primal to increase that discipline.\n\r", 
	    cost - ch->practice );
	send_to_char(buf,ch);
	return;
    }

    if (ch->pcdata->powers[improve] < 1)
	ch->pcdata->powers[improve] = 1;
    else
	ch->pcdata->powers[improve]++;

    ch->practice -= cost;
    send_to_char("You improve your discipline.\n\r",ch);

    return;
}

void do_teachdisc( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char disc [25];
    CHAR_DATA *victim;
    int improve, sn, col;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_NPC))
    {
	send_to_char("NPC's cannot teach disciplines.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: teach <person> <discipline>\n\r", ch );
	send_to_char( "The following list contains your disciplines and the level you can teach to.\n\r", ch );
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );

	col    = 0;
	for ( sn = 0; sn <= DISC_MAX; sn++ )
	{
	    if ( ch->pcdata->powers[sn] == 0 ) continue;
	    switch (sn)
	    {
		default: 
		    strcpy(disc,"None"); 
		    break;
		case DISC_ANIMALISM: 
		    strcpy(disc,"Animalism"); 
		    break;
		case DISC_AUSPEX: 
		    strcpy(disc,"Auspex"); 
		    break;
		case DISC_CELERITY: 
		    strcpy(disc,"Celerity"); 
		    break;
		case DISC_CHIMERSTRY: 
		    strcpy(disc,"Chimerstry"); 
		    break;
		case DISC_DAIMOINON: 
		    strcpy(disc,"Daimoinon"); 
		    break;
		case DISC_DOMINATE: 
		    strcpy(disc,"Dominate"); 
		    break;
		case DISC_FORTITUDE: 
		    strcpy(disc,"Fortitude"); 
		    break;
		case DISC_MELPOMINEE: 
		    strcpy(disc,"Melpominee"); 
		    break;
		case DISC_NECROMANCY: 
		    strcpy(disc,"Necromancy"); 
		    break;
		case DISC_OBEAH: 
		    strcpy(disc,"Obeah"); 
		    break;
		case DISC_OBFUSCATE: 
		    strcpy(disc,"Obfuscate"); 
		    break;
		case DISC_OBTENEBRATION: 
		    strcpy(disc,"Obtenebration"); 
		    break;
		case DISC_POTENCE: 
		    strcpy(disc,"Potence"); 
		    break;
		case DISC_PRESENCE: 
		    strcpy(disc,"Presence"); 
		    break;
		case DISC_PROTEAN: 
		    strcpy(disc,"Protean"); 
		    break;
		case DISC_QUIETUS: 
		    strcpy(disc,"Quietus"); 
		    break;
		case DISC_SERPENTIS: 
		    strcpy(disc,"Serpentis"); 
		    break;
		case DISC_THANATOSIS: 
		    strcpy(disc,"Thanatosis"); 
		    break;
		case DISC_THAUMATURGY: 
		    strcpy(disc,"Thaumaturgy"); 
		    break;
		case DISC_VICISSITUDE: 
		    strcpy(disc,"Vicissitude"); 
		    break;
	    }
	    sprintf( buf, "%-15s: %-2d       ",
		disc, ch->pcdata->powers[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
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

    if (!str_cmp(arg2,"animalism")) 
	improve = DISC_ANIMALISM;
    else if (!str_cmp(arg2,"auspex")) 
	improve = DISC_AUSPEX;
    else if (!str_cmp(arg2,"celerity")) 
	improve = DISC_CELERITY;
    else if (!str_cmp(arg2,"chimerstry")) 
	improve = DISC_CHIMERSTRY;
    else if (!str_cmp(arg2,"daimoinon")) 
	improve = DISC_DAIMOINON;
    else if (!str_cmp(arg2,"dominate")) 
	improve = DISC_DOMINATE;
    else if (!str_cmp(arg2,"fortitude")) 
	improve = DISC_FORTITUDE;
    else if (!str_cmp(arg2,"melpominee")) 
	improve = DISC_MELPOMINEE;
    else if (!str_cmp(arg2,"necromancy")) 
	improve = DISC_NECROMANCY;
    else if (!str_cmp(arg2,"obeah")) 
	improve = DISC_OBEAH;
    else if (!str_cmp(arg2,"obfuscate")) 
	improve = DISC_OBFUSCATE;
    else if (!str_cmp(arg2,"obtenebration")) 
	improve = DISC_OBTENEBRATION;
    else if (!str_cmp(arg2,"potence")) 
	improve = DISC_POTENCE;
    else if (!str_cmp(arg2,"presence")) 
	improve = DISC_PRESENCE;
    else if (!str_cmp(arg2,"protean")) 
	improve = DISC_PROTEAN;
    else if (!str_cmp(arg2,"quietus")) 
	improve = DISC_QUIETUS;
    else if (!str_cmp(arg2,"serpentis")) 
	improve = DISC_SERPENTIS;
    else if (!str_cmp(arg2,"thanatosis")) 
	improve = DISC_THANATOSIS;
    else if (!str_cmp(arg2,"thaumaturgy")) 
	improve = DISC_THAUMATURGY;
    else if (!str_cmp(arg2,"vicissitude")) 
	improve = DISC_VICISSITUDE;
    else
    {
	send_to_char("You know of no such discipline.\n\r",ch);
	return;
    }

    if (ch->pcdata->powers[improve] == 0)
    {
	send_to_char("You know of no such discipline.\n\r",ch);
	return;
    }

    if (ch->pcdata->powers[improve] < 3)
    {
	send_to_char("You require level 3 in a discipline before you can teach it.\n\r",ch);
	return;
    }

    if (!IS_VAMPIRE(victim))
    {
	send_to_char("They are unable to learn disciplines.\n\r",ch);
	return;
    }

    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to learn any disciplines.\n\r",ch);
	return;
    }

    if (victim->pcdata->powers[improve] != 0)
    {
	send_to_char("They already know that discipline.\n\r",ch);
	return;
    }

    victim->pcdata->powers[improve] = -1;
    sprintf(buf,"You teach $N how to use the %s discipline.",capitalize(arg2));
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"$n teaches you how to use the %s discipline.",capitalize(arg2));
    act(buf,ch,NULL,victim,TO_VICT);
    return;
}

void do_pigeon( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 1)
    {
	send_to_char("You require level 1 Animalism to call a Carrier Pigeon.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Transport which object?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Transport who whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (victim == ch)
    {
	send_to_char( "You cannot send things to yourself!\n\r", ch );
	return;
    }

    if (victim->in_room == NULL || victim->in_room == ch->in_room)
    {
	send_to_char( "But they are already here!\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
	return;
    }

    if (obj->weight > 100)
    {
	send_to_char( "That item weighs too much for the pigeon to carry.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_TRANSPORT))
    {
	send_to_char( "You are unable to transport anything to them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( IS_AFFECTED(ch,AFF_ETHEREAL) || IS_AFFECTED(victim,AFF_ETHEREAL) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_number + 1 > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a pigeon.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    send_to_char( "A pigeon flies down from the sky and lands on your shoulder.\n\r", ch );
    act("A pigeon flies down from the sky and lands on $n's shoulder.",ch,NULL,NULL,TO_ROOM);
    act("You place $p in the pigeon's beak.",ch,obj,NULL,TO_CHAR);
    act("$n places $p in the pigeon's beak.",ch,obj,NULL,TO_ROOM);
    send_to_char( "The pigeon takes off from your shoulder and flies away.\n\r", ch );
    act("The pigeon takes off from $n's shoulder and flies away.",ch,NULL,NULL,TO_ROOM);

    obj_from_char(obj);
    obj_to_char(obj,victim);

    send_to_char( "A pigeon flies down from the sky and lands on your shoulder.\n\r", victim );
    act("A pigeon flies down from the sky and lands on $n's shoulder.",victim,NULL,NULL,TO_ROOM);
    act("You take $p from the pigeon's beak.",victim,obj,NULL,TO_CHAR);
    act("$n takes $p from the pigeon's beak.",victim,obj,NULL,TO_ROOM);
    send_to_char( "The pigeon takes off from your shoulder and flies away.\n\r", victim );
    act("The pigeon takes off from $n's shoulder and flies away.",victim,NULL,NULL,TO_ROOM);

    do_autosave(ch,"");
    do_autosave(victim,"");
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just sent $p to you via a pigeon.",ch,obj,victim,TO_VICT);
    }
    return;
}

void do_beckon( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 2)
    {
	if (get_breed(ch, BREED_LUPUS) < 2)
	{
	    if (IS_ABOMINATION(ch))
		send_to_char("You require level 2 Animalism or level 2 Lupus power to Beckon an animal.\n\r",ch);
	    else if (IS_WEREWOLF(ch))
		send_to_char("You require the level 2 Lupus power to Beckon an animal.\n\r",ch);
	    else
		send_to_char("You require level 2 Animalism to Beckon an animal.\n\r",ch);
	    return;
	}
    }

    if (arg[0] == '\0')
    {
	send_to_char("Please select the type of animal you which to summon.\n\r",ch);
	send_to_char("Choose from: Horse, Frog, Raven, Cat, Dog, Yak.\n\r",ch);
	return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    strcpy(buf, "$N trots into the room." );
    if (!str_cmp(arg,"stallion") || !str_cmp(arg,"horse") || 
	!str_cmp(arg,"mount"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_MOUNT ) );
	victim->level = 50;
	victim->armor = -500;
	victim->hitroll = 50;
	victim->damroll = 50;
	victim->hit = 5000;
	victim->max_hit = 5000;
	free_string(victim->name);
	victim->name = str_dup("mount black horse stallion");
	sprintf(buf,"%s's black stallion",ch->name);
	free_string(victim->short_descr);
	victim->short_descr = str_dup(buf);
	free_string(victim->long_descr);
	victim->long_descr = str_dup("A beautiful black stallion stands here.\n\r");
	strcpy(buf, "$N trots into the room." );
    }
    else if (!str_cmp(arg,"frog"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_FROG ) );
	strcpy(buf, "$N hops into the room." );
    }
    else if (!str_cmp(arg,"raven"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_RAVEN ) );
	strcpy(buf, "$N flies into the room." );
    }
    else if (!str_cmp(arg,"dog"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_DOG ) );
    }
    else if (!str_cmp(arg,"cat"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_CAT ) );
    }
    else if (!str_cmp(arg,"molerat"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_MOLERAT ) );
    }
    else if (!str_cmp(arg,"yak"))
    {
	ch->pcdata->followers++;
	victim=create_mobile( get_mob_index( MOB_VNUM_YAK ) );
    }
    else
    {
	send_to_char("Please select the type of animal you which to summon.\n\r",ch);
	send_to_char("Choose from: Horse, Frog, Raven, Cat, Dog, Yak.\n\r",ch);
	return;
    }
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    char_to_room( victim, ch->in_room );
    send_to_char("You whistle loudly.\n\r",ch);
    act( "$n whistles loudly.", ch, NULL, victim, TO_ROOM );
    act( buf, ch, NULL, victim, TO_CHAR );
    act( buf, ch, NULL, victim, TO_ROOM );
    return;
}

void do_beastwithin( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 5)
    {
	if (get_auspice(ch, AUSPICE_GALLIARD) < 3)
	{
	    if (IS_ABOMINATION(ch))
		send_to_char("You require level 5 Animalism or level 3 Galliard to use Beast Within.\n\r",ch);
	    else if (IS_WEREWOLF(ch))
		send_to_char("You require the level 3 Galliard power to use Beast Within.\n\r",ch);
	    else
		send_to_char("You require level 5 Animalism to use Beast Within.\n\r",ch);
	    return;
	}
    }

    if ( arg[0] == '\0' ) victim = ch;
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim) || (!IS_VAMPIRE(victim) && !IS_WEREWOLF(victim)))
    {
	send_to_char("But they have no beast to call forth!\n\r",ch);
	return;
    }

    if (IS_WEREWOLF(victim) && IS_SET(victim->act, PLR_WEREWOLF))
    {
	send_to_char("Their beast is already controlling them.\n\r",ch);
	return;
    }

    if ( !IS_WEREWOLF(victim) && victim->pcdata->wolf > 0 )
    {
	if (ch == victim)
	    send_to_char("Your beast is already controlling you.\n\r",ch);
	else
	    send_to_char("Their beast is already controlling them.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Beast Within on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( !IS_WEREWOLF(victim) && (victim->beast < 20 || get_disc(victim,DISC_ANIMALISM) > 6) )
    {
	if (ch == victim)
	    send_to_char("You have too much control over your beast to use Beast Within.\n\r",ch);
	else
	    send_to_char("They have too much control over their beast to use Beast Within on.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Beast Within on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (victim->pcdata->resist[WILL_ANIMALISM] > 0)
    {
	send_to_char("You failed.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Beast Within on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_WILLPOWER(victim, RES_ANIMALISM) && 
	victim->pcdata->willpower[0] >= 10)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_ANIMALISM] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_ANIMALISM);
	send_to_char("You failed.\n\r",ch);
	send_to_char("You burn some willpower to resist Beast Within.\n\r",victim);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Beast Within on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_VAMPIRE(victim) && get_disc(victim,DISC_PROTEAN) > 0 && 
	!IS_VAMPAFF(victim, VAM_NIGHTSIGHT) ) do_nightsight(victim,"");
    if (IS_VAMPIRE(victim) && !IS_VAMPAFF(victim, VAM_FANGS)) do_fangs(victim,"");
    if (IS_VAMPIRE(victim) && get_disc(victim,DISC_PROTEAN) > 1 && 
	!IS_VAMPAFF(victim, VAM_CLAWS) ) do_claws(victim,"");
    if (IS_WEREWOLF(victim))
    {
	victim->pcdata->wolf = number_range(150,200);
	do_werewolf(victim,"");
    }
    else
    {
	if (victim->beast > 0) do_beastlike(victim,"");
	send_to_char("You bare yours fangs and growl as your inner beast consumes you.\n\r",victim);
	act("$n bares $s fangs and growls as $s inner beast consumes $m.",victim,NULL,NULL,TO_ROOM);
 	victim->pcdata->wolf += number_range(10,20);
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Beast Within on you.",ch,NULL,victim,TO_VICT);
    }
    WAIT_STATE(ch,12);
    return;
}

void do_serenity( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 3)
    {
	if (get_tribe(ch, TRIBE_CHILDREN_OF_GAIA) < 4)
	{
	    if (IS_ABOMINATION(ch))
		send_to_char("You require level 3 Animalism or level 4 Children of Gaia to use Serenity.\n\r",ch);
	    else if (IS_WEREWOLF(ch))
		send_to_char("You require the level 4 Children of Gaia power to use Serenity.\n\r",ch);
	    else
		send_to_char("You require level 3 Animalism to use Song of Serenity.\n\r",ch);
	    return;
	}
    }

    if ( arg[0] == '\0' ) victim = ch;
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim) || (!IS_VAMPIRE(victim) && !IS_SET(victim->act, PLR_WOLFMAN)))
    {
	send_to_char("They have no beast to suppress!\n\r",ch);
	return;
    }

    if ( victim->pcdata->wolf < 1 )
    {
	if (ch == victim)
	    send_to_char("But your beast is not controlling you.\n\r",ch);
	else
	    send_to_char("But their beast is not controlling them.\n\r",ch);

	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Song of Serenity on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( get_disc(victim,DISC_ANIMALISM) > 6 )
    {
	if (ch == victim)
	    send_to_char("You have too much control over your beast to use Song of Serenity.\n\r",ch);
	else
	    send_to_char("They have too much control over their beast to use Song of Serenity on.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Song of Serenity on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (victim->pcdata->resist[WILL_ANIMALISM] > 0 || IS_ITEMAFF(victim, ITEMA_RAGER))
    {
	send_to_char("You failed.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Song of Serenity on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_WILLPOWER(victim, RES_ANIMALISM) && 
	victim->pcdata->willpower[0] >= 10)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_ANIMALISM] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_ANIMALISM);
	send_to_char("You failed.\n\r",ch);
	send_to_char("You burn some willpower to resist Song of Serenity.\n\r",victim);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Song of Serenity on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    send_to_char("You take a deep breath and force back your inner beast.\n\r",victim);
    act("$n takes a deep breath and forces back $s inner beast.",victim,NULL,NULL,TO_ROOM);
    victim->pcdata->wolf = 0;
    if (IS_WEREWOLF(victim)) do_unwerewolf(victim,"");
    if (IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_nightsight(victim,"");
    if (IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_fangs(victim,"");
    if (IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_claws(victim,"");
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Song of Serenity on you.",ch,NULL,victim,TO_VICT);
    }
    WAIT_STATE(ch,12);
    return;
}

void do_sharespirit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg [MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 4)
    {
	send_to_char("You require level 4 Animalism to Share Spirits with an animal.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to share spirits with?\n\r", ch );
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
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

    if ( ( familiar = victim->wizard ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    switch ( victim->pIndexData->vnum )
    {
	default:
	    send_to_char( "Nothing happens.\n\r", ch );
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

    act("You merge your spirit with that of $N.",ch,NULL,victim,TO_CHAR);
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    return;
}

void do_projection( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *familiar;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_DEMON(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_AUSPEX) < 4)
    {
	if ( IS_DEMON(ch) )
	    send_to_char("You require Expert Vision to Astrally Project yourself.\n\r",ch);
	else
	    send_to_char("You require level 4 Auspex to Astrally Project yourself.\n\r",ch);
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	if (IS_EXTRA(ch, EXTRA_NO_ACTION))
	{
	    extract_char(familiar, TRUE);
	    send_to_char( "Your astral projection vanishes.\n\r", ch );
	    REMOVE_BIT(ch->extra, EXTRA_NO_ACTION);
	    REMOVE_BIT(ch->more, MORE_PROJECTION);
	}
	else send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;
    victim=create_mobile( get_mob_index( MOB_VNUM_ASTRAL ) );
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    victim->level = 50;
    victim->hit = ch->hit;
    victim->max_hit = ch->max_hit;
    victim->mana = ch->mana;
    victim->max_mana = ch->max_mana;
    victim->move = ch->move;
    victim->max_move = ch->max_move;
    victim->hitroll = char_hitroll(ch);
    victim->damroll = char_damroll(ch);
    victim->armor = char_ac(ch);

    free_string(victim->name);
    victim->name = str_dup(ch->name);
    free_string(victim->short_descr);
    victim->short_descr = str_dup(ch->name);
    sprintf(buf,"%s is here.\n\r",ch->name);
    free_string(victim->long_descr);
    victim->long_descr = str_dup(buf);
    char_to_room(victim,ch->in_room);

    send_to_char("You astrally project your spirit into the room.\n\r",ch);
    act("$n closes $s eyes and a look of concentration crosses $s face.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    SET_BIT(victim->more, MORE_SPIRIT);
    SET_BIT(victim->vampaff, VAM_SPIRITEYES);
    SET_BIT(ch->extra, EXTRA_NO_ACTION);
    SET_BIT(ch->more, MORE_PROJECTION);
    return;
}

void do_unveil( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    bool stop_unv = FALSE;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_DEMON(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_AUSPEX) < 3)
    {
	if ( IS_DEMON(ch) )
	    send_to_char("You require Advanced Vision to Unveil the Shrouded Mind.\n\r",ch);
	else
	    send_to_char("You require level 3 Auspex to Unveil the Shrouded Mind.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unveil the Shrouded Mind of whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (IS_IMMORTAL(victim))
    {
	if (ch != victim) send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( victim->desc == NULL )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
	    {
		d->snoop_by = NULL;
		stop_unv = TRUE;
	    }
	}
	if (stop_unv)
	    send_to_char( "You cease trying to Unveil any minds.\n\r", ch );
	else if (get_truedisc(ch,DISC_AUSPEX) < 20)
	    send_to_char( "How can you Unveil your own shrouded mind?\n\r", ch );
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "You failed.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_SET(victim->act, PLR_WATCHER))
    {
	send_to_char( "You failed.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_IMMUNE(victim, IMM_SHIELDED))
    {
	send_to_char( "You failed.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ch->desc != NULL )
    {
	for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "Nothing happens.\n\r", ch );
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
		}
		return;
	    }
	}
    }

    if (victim->pcdata->resist[WILL_AUSPEX] > 0)
    {
	send_to_char("You failed.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_WILLPOWER(victim, RES_AUSPEX) && 
	victim->pcdata->willpower[0] >= 10)
    {
	victim->pcdata->willpower[0] -= 10;
	victim->pcdata->resist[WILL_AUSPEX] = 60;
	if (!IS_WILLPOWER(victim, RES_TOGGLE))
	    REMOVE_BIT(victim->pcdata->resist[0], RES_AUSPEX);
	send_to_char("You failed.\n\r",ch);
	send_to_char("You burn some willpower to resist Unveil the Shrouded Mind.\n\r",victim);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "You attempt to Unveil their Shrouded Mind.\n\r", ch );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Unveil the Shrouded Mind on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_embrace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       dam = number_range(500,1000);

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ((victim = ch->embrace) != NULL && victim->embrace != NULL &&
	victim->embrace == ch)
    {
	if (ch->embraced == ARE_EMBRACED)
	{
	    send_to_char("You are already being embraced.\n\r",ch);
	    return;
	}
	if (number_percent() < ch->beast && ch->blood[0] < ch->blood[1])
	{
	    send_to_char("You are unable to stop drinking.\n\r",ch);
	    WAIT_STATE(ch, 12);
	    return;
	}
	act("You retract your fangs from $N's neck.",ch,NULL,victim,TO_CHAR);
	act("$n retracts $s fangs from your neck.",ch,NULL,victim,TO_VICT);
	act("$n retracts $s fangs from $N's neck.",ch,NULL,victim,TO_NOTVICT);

	act("You release your hold on $N.",ch,NULL,victim,TO_CHAR);
	act("$n releases $s hold on you.",ch,NULL,victim,TO_VICT);
	act("$n releases $s hold on $N.",ch,NULL,victim,TO_NOTVICT);

	ch->embrace = NULL;
	ch->embraced = ARE_NONE;
	victim->embrace = NULL;
	victim->embraced = ARE_NONE;
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to embrace?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot embrace yourself, fool!\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if (victim->position == POS_FIGHTING)
    {
	send_to_char( "They are too busy fighting at the moment.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && victim->position == POS_STUNNED)
    {
	send_to_char( "Not while they are stunned.\n\r", ch );
	return;
    }

    if (victim->embrace != NULL)
    {
	send_to_char( "They are already being embraced.\n\r", ch );
	return;
    }

    act("You step towards $N, with your arms outstretched.",ch,NULL,victim,TO_CHAR);
    act("$n step towards you, with $s arms outstretched.",ch,NULL,victim,TO_VICT);
    act("$n step towards $N, with $s arms outstretched.",ch,NULL,victim,TO_NOTVICT);
    if (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL &&
	victim->master == ch)
	send_to_char( "You stand helpless before your master.\n\r", victim );
    else if (IS_IMMUNE(victim, IMM_VAMPIRE))
	act("You stand defenceless before $m",ch,NULL,victim,TO_VICT);
    else if (victim->position < POS_MEDITATING)
	act("You leap onto $S defenseless body.",ch,NULL,victim,TO_CHAR);
    else
    {
	act("$N leaps out of your reach!",ch,NULL,victim,TO_CHAR);
	act("You leap out of $s reach!",ch,NULL,victim,TO_VICT);
	act("$N leap out of $n's reach!",ch,NULL,victim,TO_NOTVICT);
	return;
    }

    act("You grab hold of $N in a close embrace.",ch,NULL,victim,TO_CHAR);
    act("$n grabs hold of you in a close embrace.",ch,NULL,victim,TO_VICT);
    act("$n grabs hold of $N in a close embrace.",ch,NULL,victim,TO_NOTVICT);

    if (IS_MORE(victim, MORE_BRISTLES))
    {
	damage( victim, ch, dam, gsn_spines );
	stop_fighting(victim, TRUE);
    }
    if (ch->position < POS_STANDING) return;

    if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs (ch,"");

    act("You sink your fangs into $N's neck.",ch,NULL,victim,TO_CHAR);
    act("$n sinks $s fangs into your neck.",ch,NULL,victim,TO_VICT);
    act("$n sinks $s fangs into $N's neck.",ch,NULL,victim,TO_NOTVICT);

    ch->embrace = victim;
    ch->embraced = ARE_EMBRACING;
    victim->embrace = ch;
    victim->embraced = ARE_EMBRACED;

    return;
}

void do_guardian( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int sn;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && (!IS_DEMON(ch) || IS_ANGEL(ch)))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 1)
    {
	send_to_char("You require level 1 Daimoinon to call forth a Guardian Demon.\n\r",ch);
	return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    ch->pcdata->followers++;

    victim=create_mobile( get_mob_index( MOB_VNUM_GUARDIAN ) );
    victim->level = 50;
    victim->hit = 5000;
    victim->max_hit = 5000;
    victim->hitroll = 50;
    victim->damroll = 50;
    victim->armor = -500;
    victim->alignment = -1000;

    free_string(victim->lord);
    victim->lord = str_dup(ch->name);

    strcpy(buf,"Come forth, creature of darkness, and do my bidding!");
    do_say( ch, buf );

    send_to_char( "A demon bursts from the ground and bows before you.\n\r",ch );
    act( "$N bursts from the ground and bows before $n.", ch, NULL, victim, TO_ROOM );

    char_to_room( victim, ch->in_room );

    if ( ( sn = skill_lookup( "guardian" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    add_follower( victim, ch );
    af.type      = sn;
    af.duration  = 6666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    return;
}

void do_fear( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && (!IS_DEMON(ch) || IS_ANGEL(ch)))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 2)
    {
	send_to_char("You require level 2 Daimoinon to use Consuming Fear.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Consuming Fear on?\n\r", ch );
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
	    act("$n just failed to use Consuming Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( IS_MORE(victim, MORE_COURAGE) )
    {
	send_to_char( "They are far too brave to be scared so easily.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Consuming Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) && 
	get_disc(victim,DISC_DAIMOINON) >= 
	get_disc(ch,DISC_DAIMOINON))
    {
	send_to_char( "Your Consuming Fear has no effect on them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Consuming Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Consuming Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    level = get_disc(ch,DISC_DAIMOINON) * 5;

    if ( saves_spell( level, victim ) && victim->position >= POS_FIGHTING )
    {
	act("$E ignores you.",ch,NULL,victim,TO_CHAR);
	act("You ignore $m.",ch,NULL,victim,TO_VICT);
	act("$N ignores $n.",ch,NULL,victim,TO_NOTVICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Consuming Fear on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    send_to_char("You scream in terror as you are Consumed with Infernal Fear!\n\r",victim);
    act("$n screams in terror as $e is Consumed with Infernal Fear.",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Consuming Fear on you.",ch,NULL,victim,TO_VICT);
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

void do_tasteblood( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    char      disc [40];
    char      vampend [5];
    int       col, sn;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THAUMATURGY) < 1)
    {
	send_to_char("You require level 1 Thaumaturgy to use Taste of Blood.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use this power on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    act("You examine $N intently.",ch,NULL,victim,TO_CHAR);
    act("$n examines $N intently.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you intently.",ch,NULL,victim,TO_VICT);

    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    if (IS_NPC(victim) || !IS_VAMPIRE(victim))
    {
	sprintf(buf,"Name: %-15s     Blood: %-15d    Blood Pool: %d\n\r",
	    IS_NPC(victim) ? victim->short_descr : victim->name,
	    victim->blood[0],victim->blood[1]);
	send_to_char(buf,ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	if (!IS_GHOUL(victim)) return;
    }

    switch (victim->vampgen)
    {
	default: strcpy(vampend,"th"); break;
	case 1:  strcpy(vampend,"st"); break;
	case 2:  strcpy(vampend,"nd"); break;
	case 3:  strcpy(vampend,"rd"); break;
	case 4:  strcpy(vampend,"th"); break;
	case 5:  strcpy(vampend,"th"); break;
	case 6:  strcpy(vampend,"th"); break;
	case 7:  strcpy(vampend,"th"); break;
    }

    if (IS_VAMPIRE(victim))
    {
	sprintf(buf,"Name: %-15s     Clan: %-15s     Generation: %d%s\n\r",
	    victim->name,
	    strlen(victim->clan) < 2 ? "Caitiff" : victim->clan,
	    victim->vampgen, vampend);
	send_to_char( buf, ch );
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	sprintf(buf,"Blood: %-15d    Blood Pool: %-10d    Blood Potence: %d\n\r",
	    victim->blood[0],victim->blood[1],victim->blood[2]);
	send_to_char(buf,ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    }

    col    = 0;
    for ( sn = 0; sn <= DISC_MAX; sn++ )
    {
	if ( get_disc(victim,sn) == 0 ) continue;

	switch (sn)
	{
	    default: strcpy(disc,"None"); break;
	    case DISC_ANIMALISM: strcpy(disc,"Animalism"); break;
	    case DISC_AUSPEX: strcpy(disc,"Auspex"); break;
	    case DISC_CELERITY: strcpy(disc,"Celerity"); break;
	    case DISC_CHIMERSTRY: strcpy(disc,"Chimerstry"); break;
	    case DISC_DAIMOINON: strcpy(disc,"Daimoinon"); break;
	    case DISC_DOMINATE: strcpy(disc,"Dominate"); break;
	    case DISC_FORTITUDE: strcpy(disc,"Fortitude"); break;
	    case DISC_MELPOMINEE: strcpy(disc,"Melpominee"); break;
	    case DISC_NECROMANCY: strcpy(disc,"Necromancy"); break;
	    case DISC_OBEAH: strcpy(disc,"Obeah"); break;
	    case DISC_OBFUSCATE: strcpy(disc,"Obfuscate"); break;
	    case DISC_OBTENEBRATION: strcpy(disc,"Obtenebration"); break;
	    case DISC_POTENCE: strcpy(disc,"Potence"); break;
	    case DISC_PRESENCE: strcpy(disc,"Presence"); break;
	    case DISC_PROTEAN: strcpy(disc,"Protean"); break;
	    case DISC_QUIETUS: strcpy(disc,"Quietus"); break;
	    case DISC_SERPENTIS: strcpy(disc,"Serpentis"); break;
	    case DISC_THANATOSIS: strcpy(disc,"Thanatosis"); break;
	    case DISC_THAUMATURGY: strcpy(disc,"Thaumaturgy"); break;
	    case DISC_VICISSITUDE: strcpy(disc,"Vicissitude"); break;
	}

	if ( get_disc(victim,sn) < 0 )
	{
	    sprintf( buf, "%-15s: 0        ",
		disc );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	}
	else
	{
	    sprintf( buf, "%-15s: %-2d       ",
		disc, get_disc(victim,sn) );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 ) send_to_char( "\n\r", ch );
	}
    }

    if ( col % 3 != 0 ) send_to_char( "\n\r", ch );

    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Taste of Blood on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_bloodrage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THAUMATURGY) < 2)
    {
	send_to_char("You require level 2 Thaumaturgy to use Blood Rage.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Who do you wish to use this power on?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char( "Do you wish to make them regenerate or boost a stat?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim) || !IS_VAMPIRE(victim))
    {
	send_to_char( "You can only use this power on vampires.\n\r", ch );
	return;
    }

    act("You focus and concentrate on $N's blood.",ch,NULL,victim,TO_CHAR);
    do_regenerate(victim,"");
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Blood Rage on you.",ch,NULL,victim,TO_VICT);
    }

    return;
}

void do_potency( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int sn;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THAUMATURGY) < 3)
    {
	send_to_char("You require level 3 Thaumaturgy to use Blood Potency.\n\r",ch);
	return;
    }

    if (is_affected(ch, gsn_potency) || IS_GHOUL(ch))
    {
	send_to_char("You are unable to concentrate the potency of your blood any further.\n\r",ch);
	return;
    }

    if (ch->blood[0] < ((14 - ch->vampgen) * 10))
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->blood[0] -= ((14 - ch->vampgen) * 10);

    if ( ( sn = skill_lookup( "blood potency" ) ) < 0 ) return;

    af.type      = sn;
    af.duration  = (get_disc(ch,DISC_THAUMATURGY) * number_range(5,10));
    af.location  = APPLY_BLOOD_MAX;
    af.modifier  = 100;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location  = APPLY_BLOOD_POT;
    af.modifier  = 1;
    affect_to_char( ch, &af );

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    send_to_char("You concentrate the potency of your blood.\n\r",ch);

    WAIT_STATE( ch, 12 );
    return;
}

void do_theftvitae( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       blood = number_range(5,10);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THAUMATURGY) < 4)
    {
	send_to_char("You require level 4 Thaumaturgy to use Theft of Vitae.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who's Vitae do you wish to steal?\n\r", ch );
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

    if (ch == victim)
    {
	send_to_char( "Now that would be completely pointless.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Theft of Vitae on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    blood *= victim->blood[BLOOD_POTENCY];
    if ( victim->blood[BLOOD_CURRENT] < blood )
	blood = victim->blood[BLOOD_CURRENT];

    act("A stream of blood shoots from $N's body into yours.",ch,NULL,victim,TO_CHAR);
    act("A stream of blood shoots from your body into $n's.",ch,NULL,victim,TO_VICT);
    act("A stream of blood shoots from $N's body into $n's.",ch,NULL,victim,TO_NOTVICT);

    victim->blood[BLOOD_CURRENT] -= blood;
    ch->blood[BLOOD_CURRENT] += blood;
    if (ch->blood[BLOOD_CURRENT] >= ch->blood[BLOOD_POOL])
    {
	ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
	if (IS_VAMPIRE(ch)) send_to_char("Your blood lust is sated.\n\r",ch);
    }
    if (IS_NPC(victim) || !IS_VAMPIRE(victim))
    {
	if (victim->blood[0] > 0 && victim->blood[0] <= 50)
	{
	    act("$n sways weakly from bloodloss.",victim,NULL,NULL,TO_ROOM);
	    send_to_char("You feel weak from bloodloss.\n\r",victim);
	}
	else if (victim->blood[0] < 1)
	{
	    act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	    group_gain(ch,victim);
	    victim->form = 3;
	    killperson(ch,victim);
	    WAIT_STATE( ch, 12 );
	    return;
	}
    }
    else
    {
	power_ghoul(victim,ch);
	if (!IS_NPC(ch) && ch->pcdata->dpoints < (victim->blood[2] * 1000))
	{
	    if (!IS_VAMPIRE(ch)) ch->pcdata->dpoints += (blood * 60);
	    if (ch->pcdata->dpoints > (victim->blood[2] * 1000))
		ch->pcdata->dpoints = (victim->blood[2] * 1000);
	}
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Theft of Vitae on you.",ch,NULL,victim,TO_VICT);
    }
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void update_gen( CHAR_DATA *ch )
{
    if (IS_NPC(ch) || !IS_VAMPIRE(ch)) return;
    switch (ch->vampgen)
    {
	default:
	    ch->pcdata->willpower[0] = 50;
	    ch->pcdata->willpower[1] = 50;
	    ch->blood[BLOOD_POOL] = 100;
	    ch->blood[BLOOD_POTENCY] = 1;
	    break;
	case 1:
	    ch->pcdata->willpower[0] = 200;
	    ch->pcdata->willpower[1] = 200;
	    ch->blood[BLOOD_POOL] = 1000;
	    ch->blood[BLOOD_POTENCY] = 10;
	case 2:
	    ch->pcdata->willpower[0] = 100;
	    ch->pcdata->willpower[1] = 100;
	    ch->blood[BLOOD_POOL] = 600;
	    ch->blood[BLOOD_POTENCY] = 6;
	    break;
	case 3:
	    ch->pcdata->willpower[0] = 90;
	    ch->pcdata->willpower[1] = 90;
	    ch->blood[BLOOD_POOL] = 500;
	    ch->blood[BLOOD_POTENCY] = 5;
	    break;
	case 4:
	    ch->pcdata->willpower[0] = 80;
	    ch->pcdata->willpower[1] = 80;
	    ch->blood[BLOOD_POOL] = 400;
	    ch->blood[BLOOD_POTENCY] = 4;
	    break;
	case 5:
	    ch->pcdata->willpower[0] = 70;
	    ch->pcdata->willpower[1] = 70;
	    ch->blood[BLOOD_POOL] = 300;
	    ch->blood[BLOOD_POTENCY] = 3;
	    break;
	case 6:
	    ch->pcdata->willpower[0] = 60;
	    ch->pcdata->willpower[1] = 60;
	    ch->blood[BLOOD_POOL] = 200;
	    ch->blood[BLOOD_POTENCY] = 2;
	    break;
	case 7:
	    ch->pcdata->willpower[0] = 50;
	    ch->pcdata->willpower[1] = 50;
	    ch->blood[BLOOD_POOL] = 100;
	    ch->blood[BLOOD_POTENCY] = 1;
	    break;
    }
    return;
}

void do_cauldronblood( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       blood = number_range(10,20);
    int       damage;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THAUMATURGY) < 5)
    {
	send_to_char("You require level 5 Thaumaturgy to use Cauldron of Blood.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who's blood do you want to boil?\n\r", ch );
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
	send_to_char( "Stupidity is not a virtue.\n\r", ch );
	return;
    }

    if ( ch->blood[BLOOD_CURRENT] < 20 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->blood[BLOOD_CURRENT] -= 20;

    if (is_safe(ch,victim)) return;

    if ( victim->blood[BLOOD_CURRENT] < blood )
	blood = victim->blood[BLOOD_CURRENT];
    damage = (blood * victim->max_hit * 0.01);
    if ( !IS_NPC(victim) && get_disc(victim,DISC_FORTITUDE) > 0)
    {
	int per_red = 0;
	switch( get_disc(victim,DISC_FORTITUDE) )
	{
	default: per_red = 0;                   break;
	case 1:  per_red = number_range(5,10);  break;
	case 2:  per_red = number_range(10,20); break;
	case 3:  per_red = number_range(15,30); break;
	case 4:  per_red = number_range(20,40); break;
	case 5:  per_red = number_range(25,50); break;
	case 6:  per_red = number_range(30,60); break;
	case 7:  per_red = number_range(35,70); break;
	case 8:  per_red = number_range(40,80); break;
	case 9:  per_red = number_range(45,90); break;
	case 10: per_red = number_range(50,100);break;
	}
	damage -= (damage * per_red * 0.01);
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You scream in agony as your blood begins to boil!\n\r",victim);
    act("$n screams in agony as $s blood begins to boil!",victim,NULL,NULL,TO_ROOM);

    victim->blood[BLOOD_CURRENT] -= blood;
    if (IS_NPC(victim) || (!IS_VAMPIRE(victim) && !IS_GHOUL(victim)) || 
	get_disc(victim,DISC_DAIMOINON) < 6)
	victim->hit -= damage;
    if (!IS_VAMPIRE(victim))
    {
	if ((IS_NPC(victim) && victim->hit < 1) || 
	    (!IS_NPC(victim) && victim->hit < -10) || (victim->blood[0] < 1))
	{
	    act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	    group_gain(ch,victim);
	    victim->form = 4;
	    killperson(ch,victim);
	    WAIT_STATE( ch, 12 );
	    return;
	}
	update_pos(victim);
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	WAIT_STATE( ch, 12 );
    }
    else
    {
	if ((IS_NPC(victim) && victim->hit < 1) || 
	    (!IS_NPC(victim) && victim->hit < -10))
	{
	    act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	    group_gain(ch,victim);
	    victim->form = 4;
	    killperson(ch,victim);
	    WAIT_STATE( ch, 12 );
	    return;
	}
	update_pos(victim);
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	WAIT_STATE( ch, 12 );
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Cauldron of Blood on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_possession( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg [MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DOMINATE) < 3)
    {
	send_to_char("You require level 3 Dominate to use Possession.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to possess?\n\r", ch );
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
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

    if (IS_ITEMAFF(victim, ITEMA_SHADES))
    {
	if (!IS_SET(ch->act,PLR_HOLYLIGHT) && 
	    get_disc(ch,DISC_DOMINATE) < 6)
	{
	    send_to_char( "You are unable to make eye contact with them.\n\r", ch );
	    return;
	}
    }

    act("You stare deeply into $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n stares deeply into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);

    if ( ( familiar = victim->wizard ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (victim->level > (get_disc(ch,DISC_DOMINATE) * 10))
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    act("You possess $N's body.",ch,NULL,victim,TO_CHAR);
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    return;
}

void do_mirrorimage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 1)
    {
	send_to_char("You require level 1 Chimerstry to create a Mirror Image.\n\r",ch);
	return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;
    victim=create_mobile( get_mob_index( MOB_VNUM_CLONE ) );
    char_to_room(victim,get_room_index(ROOM_VNUM_DISCONNECTION));
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    free_string(victim->name);
    victim->name = str_dup(ch->name);
    free_string(victim->short_descr);
    victim->short_descr = str_dup(ch->name);
    free_string(victim->long_descr);
    victim->long_descr = str_dup("");
    if (ch->class == CLASS_VAMPIRE) victim->class = CLASS_VAMPIRE;
    if (ch->class == CLASS_GHOUL) victim->class = CLASS_GHOUL;
    victim->alignment = ch->alignment;
    if (strlen(ch->description) > 1)
    {
	free_string(victim->description);
	victim->description = str_dup(ch->description);
    }
    SET_BIT(victim->act, ACT_NOPARTS);
    SET_BIT(victim->extra, EXTRA_ILLUSION);

    {
	OBJ_DATA *obj_next;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE )
	    {
		if (( obj2 = create_object( obj->pIndexData, 0 )) != NULL )
		{
		    SET_BIT(obj2->extra_flags, ITEM_VANISH);
		    obj_to_char(obj2, victim);
		    wear_obj( victim, obj2, obj->wear_loc );
		}
	    }
	}
    }
    victim->level = 50;
    victim->hit = ch->hit;
    victim->max_hit = ch->max_hit;
    victim->mana = ch->mana;
    victim->max_mana = ch->max_mana;
    victim->move = ch->move;
    victim->max_move = ch->max_move;
    victim->hitroll = char_hitroll(ch);
    victim->damroll = char_damroll(ch);
    victim->armor = char_ac(ch);
    char_from_room(victim);
    char_to_room(victim,ch->in_room);

    send_to_char("A mirror image of you shimmers into existance.\n\r",ch);
    act("A mirror image of $n appears in the room.",ch,NULL,NULL,TO_ROOM);
    return;
}

void do_formillusion( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *original;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 2)
    {
	if (get_tribe(ch, TRIBE_FIANNA) < 3)
	{
	    if (IS_ABOMINATION(ch))
		send_to_char("You require level 2 Chimerstry or level 3 Fianna power to use Illusionary Form.\n\r",ch);
	    else if (IS_WEREWOLF(ch))
		send_to_char("You require the level 3 Fianna power to create an Illusionary Form.\n\r",ch);
	    else
		send_to_char("You require level 2 Chimerstry to create an Illusionary Form.\n\r",ch);
	    return;
	}
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to Form an Illusion of?\n\r", ch );
	return;
    }

    if ( ( original = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( original->level > 200 )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    ch->pcdata->followers++;
    victim=create_mobile( get_mob_index( MOB_VNUM_CLONE ) );
    char_to_room(victim,get_room_index(ROOM_VNUM_DISCONNECTION));
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    free_string(victim->name);
    victim->name = str_dup(original->name);
    free_string(victim->short_descr);
    if (IS_NPC(original))
	victim->short_descr = str_dup(original->short_descr);
    else
	victim->short_descr = str_dup(original->name);
    free_string(victim->long_descr);
    if (IS_NPC(original))
	victim->long_descr = str_dup(original->long_descr);
    else
	victim->long_descr = str_dup("");

    if (original->class == CLASS_VAMPIRE) victim->class = CLASS_VAMPIRE;
    if (original->class == CLASS_GHOUL) victim->class = CLASS_GHOUL;

    victim->alignment = original->alignment;
    if (strlen(original->description) > 1)
    {
	free_string(victim->description);
	victim->description = str_dup(original->description);
    }
    SET_BIT(victim->act, ACT_NOPARTS);
    SET_BIT(victim->extra, EXTRA_ILLUSION);

    {
	OBJ_DATA *obj_next;
	for ( obj = original->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE )
	    {
		if (( obj2 = create_object( obj->pIndexData, 0 )) != NULL )
		{
		    SET_BIT(obj2->extra_flags, ITEM_VANISH);
		    obj_to_char(obj2, victim);
		    wear_obj( victim, obj2, obj->wear_loc );
		}
	    }
	}
    }
    if (IS_NPC(original)) victim->level = original->level;
    else victim->level = 50;
    victim->hit = original->hit;
    victim->max_hit = original->max_hit;
    victim->mana = original->mana;
    victim->max_mana = original->max_mana;
    victim->move = original->move;
    victim->max_move = original->max_move;
    victim->hitroll = char_hitroll(original);
    victim->damroll = char_damroll(original);
    victim->armor = char_ac(original);
    char_from_room(victim);
    char_to_room(victim,original->in_room);

    send_to_char("A mirror image of you shimmers into existance.\n\r",original);
    act("A mirror image of $n appears in the room.",original,NULL,NULL,TO_ROOM);
    return;
}

void do_cloneobject( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_WEREWOLF(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 2)
    {
	if (get_tribe(ch, TRIBE_FIANNA) < 4)
	{
	    if (IS_ABOMINATION(ch))
		send_to_char("You require level 3 Chimerstry or level 4 Fianna power to use Clone Object.\n\r",ch);
	    else if (IS_WEREWOLF(ch))
		send_to_char("You require the level 4 Fianna power to use Clone Object.\n\r",ch);
	    else
		send_to_char("You require level 3 Chimerstry to use Clone Object.\n\r",ch);
	    return;
	}
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What object do you wish to make an illusionary clone of?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }

    if (strlen(obj->questmaker) > 1)
    {
	send_to_char( "You are unable to clone that object.\n\r", ch );
	return;
    }

    if (IS_SET(obj->extra_flags, ITEM_VANISH))
    {
	send_to_char( "You are unable to make illusions of illusions!\n\r", ch );
	return;
    }

    if (IS_SET(obj->quest, QUEST_RELIC) || IS_SET(obj->quest, QUEST_ARTIFACT))
    {
	send_to_char( "You are unable to clone that type of object.\n\r", ch );
	return;
    }

    if (strlen(obj->questowner) > 1)
    {
	send_to_char( "You are unable to clone a claimed item.\n\r", ch );
	return;
    }

    if ( ch->carry_number + 1 > can_carry_n( ch ) )
    {
	send_to_char( "You cannot carry any more objects.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You cannot carry that much weight.\n\r", ch );
	return;
    }

    if (( obj2 = create_object( obj->pIndexData, obj->level )) != NULL )
    {
	SET_BIT(obj2->extra_flags, ITEM_VANISH);
	obj2->timer = (get_disc(ch,DISC_CHIMERSTRY) * 5);
	obj2->cost = 0;
	switch (obj2->item_type)
	{
	    default:
		obj2->value[0] = obj->value[0];
		obj2->value[1] = obj->value[1];
		obj2->value[2] = obj->value[2];
		obj2->value[3] = obj->value[3];
		break;
	    case ITEM_WAND:
	    case ITEM_STAFF:
	    case ITEM_SCROLL:
	    case ITEM_POTION:
	    case ITEM_PILL:
		obj2->value[0] = 0;
		obj2->value[1] = 0;
		obj2->value[2] = 0;
		obj2->value[3] = 0;
		break;
	}
	obj_to_char(obj2, ch);
	act("An exact duplicate of $p appears in your hands.",ch,obj,NULL,TO_CHAR);
	act("An exact duplicate of $p appears in $n's hands.",ch,obj,NULL,TO_ROOM);
    }
    else
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
    return;
}

void do_rename( CHAR_DATA *ch, char *argument )
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

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 4)
    {
	send_to_char("You require level 4 Chimerstry to give an object a False Appearance.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: False <object> rename  <short description>\n\r", ch );
	send_to_char( "Syntax: False <object> keyword <extra keyword/s>\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
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
    send_to_char( "Syntax: False <object> rename  <short description>\n\r", ch );
    send_to_char( "Syntax: False <object> keyword <extra keyword/s>\n\r", ch );
    return;
}

void do_control( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;
    char arg [MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 5)
    {
	send_to_char("You require level 5 Chimerstry to Control an Illusion.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What illusion do you want to control?\n\r", ch );
	return;
    }

    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
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

    if ( ( familiar = victim->wizard ) != NULL ) 
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (!IS_EXTRA(victim, EXTRA_ILLUSION))
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    act("You use your power over illusions to control $N.",ch,NULL,victim,TO_CHAR);
    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    return;
}

void do_truedisguise( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_CHIMERSTRY) < 6)
    {
	send_to_char("You require level 6 Chimerstry to use True Disguise.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_EXTRA(ch, EXTRA_VICISSITUDE))
    {
	send_to_char( "You are unable to change your appearance.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You cannot disguise yourself as them.\n\r", ch );
	return;
    }
    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	if (!IS_POLYAFF(ch, POLY_TRUE))
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	sprintf(buf,"$n's form shifts into an image of %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->polyaff, POLY_TRUE);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your form shifts into an image of $n.",ch,NULL,NULL,TO_CHAR);
    	free_string( ch->long_descr );
    	ch->long_descr = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_TRUE))
	{
	    send_to_char("Nothing happens.\n\r",ch);
	    return;
	}
	act("Your form shifts into an image of $N.",ch,NULL,victim,TO_CHAR);
	act("$n's form shifts into an image of you.",ch,NULL,victim,TO_VICT);
	act("$n's form shifts into an image of $N.",ch,NULL,victim,TO_NOTVICT);
    	free_string( ch->morph );
	if (IS_NPC(victim))
	{
	    ch->morph = str_dup( victim->short_descr );
	    free_string( ch->long_descr );
	    ch->long_descr = str_dup( victim->long_descr );
	}
	else
	{
	    ch->morph = str_dup( victim->name );
	    free_string( ch->long_descr );
	    ch->long_descr = str_dup( "" );
	}
	return;
    }
    act("Your form shifts into an image of $N.",ch,NULL,victim,TO_CHAR);
    act("$n's form shifts into an image of you.",ch,NULL,victim,TO_VICT);
    act("$n's form shifts into an image of $N.",ch,NULL,victim,TO_NOTVICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->polyaff, POLY_TRUE);
    free_string( ch->morph );
    if (IS_NPC(victim))
    {
	ch->morph = str_dup( victim->short_descr );
	free_string( ch->long_descr );
	ch->long_descr = str_dup( victim->long_descr );
    }
    else
    {
	ch->morph = str_dup( victim->name );
	free_string( ch->long_descr );
	ch->long_descr = str_dup( "" );
    }
    return;
}

void do_omnipotence( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *eye;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_AUSPEX) < 9)
    {
	send_to_char("You require level 9 Auspex to use Omnipotence.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	if ( ( eye = ch->pcdata->familiar ) != NULL && 
	    eye->pIndexData->vnum == MOB_VNUM_EYE) 
	{
	    send_to_char( "You are no longer using your Omnipotence.\n\r",ch );
	    ch->pcdata->familiar = NULL;
	    eye->wizard = NULL;
	    extract_char(eye, TRUE);
	    return;
	}
	send_to_char( "Who do you wish to use Omnipotence on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Now that would be completely pointless.\n\r", ch );
	return;
    }

    if (IS_IMMORTAL(victim) || IS_IMMUNE(victim,IMM_SHIELDED))
    {
	send_to_char( "You are unable to locate them.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( ( eye = ch->pcdata->familiar ) != NULL )
    {
	if ( eye->pIndexData->vnum == MOB_VNUM_EYE ) 
	{
	    send_to_char( "You have switched the target of your Omnipotence.\n\r",ch );
	    char_from_room( eye );
	    char_to_room( eye, victim->in_room );
	    if ( eye->master != NULL ) stop_follower( eye );
	    eye->master        = victim;
	    eye->leader        = NULL;
	}
	else
	    send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (ch->pcdata->followers > 4 || victim->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;

    eye=create_mobile( get_mob_index( MOB_VNUM_EYE ) );
    eye->level = 1;
    eye->hit = 1;
    eye->max_hit = 1;
    eye->hitroll = 0;
    eye->damroll = 0;
    eye->armor = 100;
    eye->alignment = 0;

    free_string(eye->name);
    eye->name = str_dup("eye eyeball");
    free_string(eye->short_descr);
    eye->short_descr = str_dup("a small spectral eye");
    free_string(eye->long_descr);
    eye->long_descr = str_dup("A small spectral eye floats here.\n\r");
    free_string(eye->lord);
    eye->lord = str_dup(ch->name);

    send_to_char( "You are now using your Omnipotence.\n\r",ch );

    char_to_room( eye, victim->in_room );
    ch->pcdata->familiar = eye;
    eye->wizard = ch;

    if ( eye->master != NULL )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    eye->master        = victim;
    eye->leader        = NULL;
    SET_BIT(eye->more, MORE_SPIRIT);
    SET_BIT(eye->vampaff, VAM_SPIRITEYES);
    SET_BIT(eye->act, ACT_NOEXP);
    SET_BIT(eye->act, ACT_NOPARTS);
    SET_BIT(eye->affected_by, AFF_ETHEREAL);
    SET_BIT(eye->affected_by, AFF_FLYING);
    return;
}

void do_obeah( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch, DISC_OBEAH) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( IS_MORE(ch,MORE_OBEAH) )
    {
	if (ch->pcdata->obeah > 0)
	{
	    send_to_char("You cannot close your eye until it stops glowing!\n\r",ch);
	    return;
	}
	send_to_char("You close the eye on your forehead.\n\r",ch);
	act("$n closes the eye on $s forehead.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->more, MORE_OBEAH);
	return;
    }
    send_to_char("Your third eye opens in your forehead.\n\r",ch);
    act("A third eye opens in $n's forehead.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->more, MORE_OBEAH);
    return;
}

void part_reg( CHAR_DATA *ch )
{
    int ribs = 0;

    if (IS_BODY(ch,BROKEN_RIBS_1 )) ribs += 1;
    if (IS_BODY(ch,BROKEN_RIBS_2 )) ribs += 2;
    if (IS_BODY(ch,BROKEN_RIBS_4 )) ribs += 4;
    if (IS_BODY(ch,BROKEN_RIBS_8 )) ribs += 8;
    if (IS_BODY(ch,BROKEN_RIBS_16)) ribs += 16;

    if (ribs > 0)
    {
    	if (IS_BODY(ch,BROKEN_RIBS_1 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_1);
    	if (IS_BODY(ch,BROKEN_RIBS_2 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_2);
	if (IS_BODY(ch,BROKEN_RIBS_4 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_4);
	if (IS_BODY(ch,BROKEN_RIBS_8 ))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_8);
	if (IS_BODY(ch,BROKEN_RIBS_16))
	    REMOVE_BIT(ch->loc_hp[1],BROKEN_RIBS_16);
	ribs -= 1;
	if (ribs >= 16) {ribs -= 16;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_16);}
	if (ribs >= 8 ) {ribs -= 8;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_8);}
	if (ribs >= 4 ) {ribs -= 4;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_4);}
	if (ribs >= 2 ) {ribs -= 2;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_2);}
	if (ribs >= 1 ) {ribs -= 1;
	    SET_BIT(ch->loc_hp[1],BROKEN_RIBS_1);}
	act("One of $n's ribs snap back into place.",ch,NULL,NULL,TO_ROOM);
	act("One of your ribs snap back into place.",ch,NULL,NULL,TO_CHAR);
    }
    else if (IS_HEAD(ch,BROKEN_NOSE) && !IS_HEAD(ch, LOST_NOSE))
    {
	act("$n's nose snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your nose snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (IS_HEAD(ch,BROKEN_JAW))
    {
	act("$n's jaw snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your jaw snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_JAW);
    }
    else if (IS_HEAD(ch,BROKEN_SKULL))
    {
	act("$n's skull knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your skull knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_HEAD],BROKEN_SKULL);
    }
    else if (IS_BODY(ch,BROKEN_SPINE))
    {
	act("$n's spine knits itself back together.",ch,NULL,NULL,TO_ROOM);
	act("Your spine knits itself back together.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_SPINE);
    }
    else if (IS_BODY(ch,BROKEN_NECK))
    {
	act("$n's neck snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your neck snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],BROKEN_NECK);
    }
    else if (IS_ARM_L(ch,BROKEN_ARM) && !IS_ARM_L(ch,LOST_ARM))
    {
	act("$n's left arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_ARM);
    }
    else if (IS_ARM_R(ch,BROKEN_ARM) && !IS_ARM_R(ch,LOST_ARM))
    {
	act("$n's right arm snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right arm snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_ARM);
    }
    else if (IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG))
    {
	act("$n's left leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_L],BROKEN_LEG);
    }
    else if (IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG))
    {
	act("$n's right leg snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right leg snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_LEG_R],BROKEN_LEG);
    }
    else if (IS_ARM_L(ch,BROKEN_THUMB) && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND) && !IS_ARM_L(ch,LOST_THUMB))
    {
	act("$n's left thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_THUMB);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_I) && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND) && !IS_ARM_L(ch,LOST_FINGER_I))
    {
	act("$n's left index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_M) && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND) && !IS_ARM_L(ch,LOST_FINGER_M))
    {
	act("$n's left middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_R) && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND) && !IS_ARM_L(ch,LOST_FINGER_R))
    {
	act("$n's left ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);
    }
    else if (IS_ARM_L(ch,BROKEN_FINGER_L) && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND) && !IS_ARM_L(ch,LOST_FINGER_L))
    {
	act("$n's left little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your left little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);
    }
    else if (IS_ARM_R(ch,BROKEN_THUMB) && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND) && !IS_ARM_R(ch,LOST_THUMB))
    {
	act("$n's right thumb snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right thumb snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_THUMB);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_I) && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND) && !IS_ARM_R(ch,LOST_FINGER_I))
    {
	act("$n's right index finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right index finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_I) && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND) && !IS_ARM_R(ch,LOST_FINGER_I))
    {
	act("$n's right middle finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right middle finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_R) && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND) && !IS_ARM_R(ch,LOST_FINGER_R))
    {
	act("$n's right ring finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right ring finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);
    }
    else if (IS_ARM_R(ch,BROKEN_FINGER_L) && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND) && !IS_ARM_R(ch,LOST_FINGER_L))
    {
	act("$n's right little finger snaps back into place.",ch,NULL,NULL,TO_ROOM);
	act("Your right little finger snaps back into place.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);
    }
    else if (IS_ARM_L(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_L))
    {
	act("The wound in $n's left wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your left wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],SLIT_WRIST);
    }
    else if (IS_ARM_R(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_R))
    {
	act("The wound in $n's right wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your right wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],SLIT_WRIST);
    }
    else if (IS_BODY(ch,CUT_CHEST) && !IS_BLEEDING(ch, BLEEDING_CHEST))
    {
	act("The wound in $n's chest closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your chest closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_CHEST);
    }
    else if (IS_BODY(ch,CUT_STOMACH) && !IS_BLEEDING(ch, BLEEDING_STOMACH))
    {
	act("The wound in $n's stomach closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your stomach closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_STOMACH);
    }
    else if (IS_BODY(ch,CUT_THROAT) && !IS_BLEEDING(ch, BLEEDING_THROAT))
    {
	act("The wound in $n's throat closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your throat closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_BODY],CUT_THROAT);
    }
    else if (IS_ARM_L(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_L))
    {
	act("The wound in $n's left wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your left wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_L],SLIT_WRIST);
    }
    else if (IS_ARM_R(ch,SLIT_WRIST) && !IS_BLEEDING(ch, BLEEDING_WRIST_R))
    {
	act("The wound in $n's right wrist closes up.",ch,NULL,NULL,TO_ROOM);
	act("The wound in your right wrist closes up.",ch,NULL,NULL,TO_CHAR);
	REMOVE_BIT(ch->loc_hp[LOC_ARM_R],SLIT_WRIST);
    }
    return;
}

void do_slit( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    argument = one_argument( argument, arg );

    if (arg[0] == '\0')
    {
	send_to_char("Syntax: Slit <left/right>.\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"l") || !str_cmp(arg,"left"))
    {
	if ( ( obj = get_eq_char(ch, WEAR_WIELD) ) != NULL )
	{
	    if ( obj->item_type != ITEM_WEAPON || 
		(obj->value[3] != 1 && obj->value[3] != 3 && obj->value[3] != 5) )
	    {
		send_to_char("You need to hold a sharp item in your right hand.\n\r",ch);
		return;
	    }
	}
	else if ( !IS_VAMPAFF(ch, VAM_CLAWS) && !IS_VAMPAFF(ch, VAM_FANGS) )
	{
	    send_to_char("You need to hold a sharp item in your right hand, or use claws or fangs.\n\r",ch);
	    return;
	}

	if (IS_ARM_L(ch, LOST_ARM))
	{
	    send_to_char("But you've already lost your whole arm!\n\r",ch);
	    return;
	}
	if (IS_ARM_L(ch, LOST_HAND))
	{
	    send_to_char("But you've already lost your whole hand!\n\r",ch);
	    return;
	}
	if (IS_ARM_L(ch, SLIT_WRIST))
	{
	    send_to_char("Your left wrist has already been slit.\n\r",ch);
	    return;
	}
	if (obj != NULL)
	{
	    act("You slit open your left wrist with $p.",ch,obj,NULL,TO_CHAR);
	    act("$n slits open $s left wrist with $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_VAMPAFF(ch, VAM_CLAWS))
	{
	    send_to_char("You slit open your left wrist with your claws.\n\r",ch);
	    act("$n slits open $s left wrist with $s claws.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("You rip open your left wrist with your fangs.\n\r",ch);
	    act("$n rips open $s left wrist with $s fangs.",ch,NULL,NULL,TO_ROOM);
	}
	SET_BIT(ch->loc_hp[LOC_ARM_L], SLIT_WRIST);
	SET_BIT(ch->loc_hp[6], BLEEDING_WRIST_L);
	return;
    }
    else if (!str_cmp(arg,"r") || !str_cmp(arg,"right"))
    {
	if ( ( obj = get_eq_char(ch, WEAR_HOLD) ) != NULL )
	{
	    if ( obj->item_type != ITEM_WEAPON || 
		(obj->value[3] != 1 && obj->value[3] != 3 && obj->value[3] != 5) )
	    {
		send_to_char("You need to hold a sharp item in your left hand.\n\r",ch);
		return;
	    }
	}
	else if ( !IS_VAMPAFF(ch, VAM_CLAWS) && !IS_VAMPAFF(ch, VAM_FANGS) )
	{
	    send_to_char("You need to hold a sharp item in your left hand, or use claws or fangs.\n\r",ch);
	    return;
	}

	if (IS_ARM_R(ch, LOST_ARM))
	{
	    send_to_char("But you've already lost your whole arm!\n\r",ch);
	    return;
	}
	if (IS_ARM_R(ch, LOST_HAND))
	{
	    send_to_char("But you've already lost your whole hand!\n\r",ch);
	    return;
	}
	if (IS_ARM_R(ch, SLIT_WRIST))
	{
	    send_to_char("Your right wrist has already been slit.\n\r",ch);
	    return;
	}
	if (obj != NULL)
	{
	    act("You slit open your right wrist with $p.",ch,obj,NULL,TO_CHAR);
	    act("$n slits open $s right wrist with $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_VAMPAFF(ch, VAM_CLAWS))
	{
	    send_to_char("You slit open your right wrist with your claws.\n\r",ch);
	    act("$n slits open $s right wrist with $s claws.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("You rip open your right wrist with your fangs.\n\r",ch);
	    act("$n rips open $s right wrist with $s fangs.",ch,NULL,NULL,TO_ROOM);
	}
	SET_BIT(ch->loc_hp[LOC_ARM_R], SLIT_WRIST);
	SET_BIT(ch->loc_hp[6], BLEEDING_WRIST_R);
	return;
    }
    else send_to_char("Syntax: Slit <left/right>.\n\r",ch);
    return;
}

char *disc_name( int disc_no )
{
    static char disc[20];

    switch ( disc_no )
    {
	default: 
	    strcpy(disc,"None"); 
	    break;
	case DISC_ANIMALISM: 
	    strcpy(disc,"Animalism"); 
	    break;
	case DISC_AUSPEX: 
	    strcpy(disc,"Auspex"); 
	    break;
	case DISC_CELERITY: 
	    strcpy(disc,"Celerity"); 
	    break;
	case DISC_CHIMERSTRY: 
	    strcpy(disc,"Chimerstry"); 
	    break;
	case DISC_DAIMOINON: 
	    strcpy(disc,"Daimoinon"); 
	    break;
	case DISC_DOMINATE: 
	    strcpy(disc,"Dominate"); 
	    break;
	case DISC_FORTITUDE: 
	    strcpy(disc,"Fortitude"); 
	    break;
	case DISC_MELPOMINEE: 
	    strcpy(disc,"Melpominee"); 
	    break;
	case DISC_NECROMANCY: 
	    strcpy(disc,"Necromancy"); 
	    break;
	case DISC_OBEAH: 
	    strcpy(disc,"Obeah"); 
	    break;
	case DISC_OBFUSCATE: 
	    strcpy(disc,"Obfuscate"); 
	    break;
	case DISC_OBTENEBRATION: 
	    strcpy(disc,"Obtenebration"); 
	    break;
	case DISC_POTENCE: 
	    strcpy(disc,"Potence"); 
	    break;
	case DISC_PRESENCE: 
	    strcpy(disc,"Presence"); 
	    break;
	case DISC_PROTEAN: 
	    strcpy(disc,"Protean"); 
	    break;
	case DISC_QUIETUS: 
	    strcpy(disc,"Quietus"); 
	    break;
	case DISC_SERPENTIS: 
	    strcpy(disc,"Serpentis"); 
	    break;
	case DISC_THANATOSIS: 
	    strcpy(disc,"Thanatosis"); 
	    break;
	case DISC_THAUMATURGY: 
	    strcpy(disc,"Thaumaturgy"); 
	    break;
	case DISC_VICISSITUDE: 
	    strcpy(disc,"Vicissitude"); 
	    break;
    }
    return &disc[0];
}
