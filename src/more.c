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
void	show_disc		args( ( CHAR_DATA *ch ) );



void do_mindblast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       damage;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_AUSPEX) < 8)
    {
	send_to_char("You require level 8 Auspex to use a Mind Blast.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to Mind Blast?\n\r", ch );
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
	    act("$n just failed to use Mind Blast on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (IS_IMMUNE(victim,IMM_SHIELDED) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Mind Blast on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Mind Blast on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    damage = victim->max_hit * 0.1;

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You clutch your head as your mind seems to explode!\n\r",victim);
    act("$n clutches $s head in agony!",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Mind Blast on you.",ch,NULL,victim,TO_VICT);
    }

    victim->hit -= damage;
    if ((IS_NPC(victim) && victim->hit < 1) || 
	(!IS_NPC(victim) && victim->hit < -10))
    {
	act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	demon_gain(ch,victim);
	group_gain(ch,victim);
	victim->form = 8;
	killperson(ch,victim);
	WAIT_STATE( ch, 12 );
	return;
    }
    stop_fighting(victim,TRUE);
    update_pos(victim);
    if (victim->position > POS_STUNNED)
	victim->position = POS_STUNNED;
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void do_willpower( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) )
	return;

    if (arg[0] == '\0')
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                -= Will Power =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if (IS_WILLPOWER(ch, RES_ANIMALISM))
	    send_to_char("Animalism: You automatically try to resist Beast Within and Song of Serenity.\n\r",ch);
	else
	    send_to_char("Animalism: You don't try to resist Beast Within and Song of Serenity.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_AUSPEX))
	    send_to_char("Auspex: You automatically try to resist Unveil the Shrouded Mind.\n\r",ch);
	else
	    send_to_char("Auspex: You don't try to resist Unveil the Shrouded Mind.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_DOMINATE))
	    send_to_char("Dominate: You automatically try to resist Dominate attempts.\n\r",ch);
	else
	    send_to_char("Dominate: You don't try to resist Dominate attempts.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_PRESENCE))
	    send_to_char("Presence: You automatically try to resist Dread Gaze and Entrancement.\n\r",ch);
	else
	    send_to_char("Presence: You don't try to resist Dread Gaze and Entrancement.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_TOGGLE))
	    send_to_char("Toggle: You automatically keep burning willpower to resist attempts.\n\r",ch);
	else
	    send_to_char("Toggle: You will only burn willpower to resist each first attempt.\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("You may change any of the above by typing: willpower <value>\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("Remaining resistance in seconds:\n\r", ch );
	sprintf( buf,"Animalism: %d.  Auspex: %d.  Presence: %d.  Dominate: %d.\n\r",
	ch->pcdata->resist[WILL_ANIMALISM], ch->pcdata->resist[WILL_AUSPEX],
	ch->pcdata->resist[WILL_PRESENCE],  ch->pcdata->resist[WILL_DOMINATE]);
	send_to_char( buf, ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"animalism"))
    {
	if (IS_WILLPOWER(ch, RES_ANIMALISM))
	{
	    send_to_char("You will no longer try to resist Beast Within and Song of Serenity.\n\r",ch);
	    REMOVE_BIT(ch->pcdata->resist[0], RES_ANIMALISM);
	}
	else
	{
	    send_to_char("You will now automatically try to resist Beast Within and Song of Serenity.\n\r",ch);
	    SET_BIT(ch->pcdata->resist[0], RES_ANIMALISM);
	}
    }
    else if (!str_cmp(arg,"auspex"))
    {
	if (IS_WILLPOWER(ch, RES_AUSPEX))
	{
	    send_to_char("You will no longer try to resist Unveil the Shrouded Mind.\n\r",ch);
	    REMOVE_BIT(ch->pcdata->resist[0], RES_AUSPEX);
	}
	else
	{
	    send_to_char("You will now automatically try to resist Unveil the Shrouded Mind.\n\r",ch);
	    SET_BIT(ch->pcdata->resist[0], RES_AUSPEX);
	}
    }
    else if (!str_cmp(arg,"dominate"))
    {
	if (IS_WILLPOWER(ch, RES_DOMINATE))
	{
	    send_to_char("You will no longer try to resist Domination attempts.\n\r",ch);
	    REMOVE_BIT(ch->pcdata->resist[0], RES_DOMINATE);
	}
	else
	{
	    send_to_char("You will now automatically try to resist Domination attempts.\n\r",ch);
	    SET_BIT(ch->pcdata->resist[0], RES_DOMINATE);
	}
    }
    else if (!str_cmp(arg,"presence"))
    {
	if (IS_WILLPOWER(ch, RES_PRESENCE))
	{
	    send_to_char("You will no longer try to resist Dread Gaze and Entrancement.\n\r",ch);
	    REMOVE_BIT(ch->pcdata->resist[0], RES_PRESENCE);
	}
	else
	{
	    send_to_char("You will now automatically try to resist Dread Gaze and Entrancement.\n\r",ch);
	    SET_BIT(ch->pcdata->resist[0], RES_PRESENCE);
	}
    }
    else if (!str_cmp(arg,"toggle"))
    {
	if (IS_WILLPOWER(ch, RES_TOGGLE))
	{
	    send_to_char("You will now only burn willpower to resist each first attempt.\n\r",ch);
	    REMOVE_BIT(ch->pcdata->resist[0], RES_TOGGLE);
	}
	else
	{
	    send_to_char("You will now automatically keep burning willpower to resist attempts.\n\r",ch);
	    SET_BIT(ch->pcdata->resist[0], RES_TOGGLE);
	}
    }
    else
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                -= Will Power =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if (IS_WILLPOWER(ch, RES_ANIMALISM))
	    send_to_char("Animalism: You automatically try to resist Beast Within and Song of Serenity.\n\r",ch);
	else
	    send_to_char("Animalism: You don't try to resist Beast Within and Song of Serenity.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_AUSPEX))
	    send_to_char("Auspex: You automatically try to resist Unveil the Shrouded Mind.\n\r",ch);
	else
	    send_to_char("Auspex: You don't try to resist Unveil the Shrouded Mind.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_DOMINATE))
	    send_to_char("Dominate: You automatically try to resist Dominate attempts.\n\r",ch);
	else
	    send_to_char("Dominate: You don't try to resist Dominate attempts.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_PRESENCE))
	    send_to_char("Presence: You automatically try to resist Dread Gaze and Entrancement.\n\r",ch);
	else
	    send_to_char("Presence: You don't try to resist Dread Gaze and Entrancement.\n\r",ch);
	if (IS_WILLPOWER(ch, RES_TOGGLE))
	    send_to_char("Toggle: You automatically keep burning willpower to resist attempts.\n\r",ch);
	else
	    send_to_char("Toggle: You will only burn willpower to resist each first attempt.\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("You may change any of the above by typing: willpower <value>\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("Remaining resistance in seconds:\n\r", ch );
	sprintf( buf,"Animalism: %d.  Auspex: %d.  Presence: %d.  Dominate: %d.\n\r",
	ch->pcdata->resist[WILL_ANIMALISM], ch->pcdata->resist[WILL_AUSPEX],
	ch->pcdata->resist[WILL_PRESENCE],  ch->pcdata->resist[WILL_DOMINATE]);
	send_to_char( buf, ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }
    return;
}

void do_changeling( CHAR_DATA *ch, char *argument )
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

    if (get_disc(ch,DISC_VICISSITUDE) < 1)
    {
	send_to_char("You require level 1 Vicissitude to use Changeling.\n\r",ch);
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

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
	send_to_char( "Not while in Zulo Form.\n\r", ch );
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
	send_to_char( "You can only change to look like avatars or lower.\n\r", ch );
	return;
    }
    if (victim->sex != ch->sex)
    {
	send_to_char( "You can only change to look like people of the same sex as you.\n\r", ch );
	return;
    }
    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	if (!IS_POLYAFF(ch, POLY_CHANGELING))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	sprintf(buf,"$n's features twist and distort until $e looks like %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->polyaff, POLY_CHANGELING);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your features twist and distort until you looks like $n.",ch,NULL,NULL,TO_CHAR);
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_CHANGELING))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
	act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
	act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	return;
    }
    act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
    act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
    act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
    SET_BIT(ch->polyaff, POLY_CHANGELING);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}

void do_fleshcraft( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    one_argument( argument, arg1 );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_VICISSITUDE) < 2)
    {
	send_to_char("You require level 2 Vicissitude to use Fleshcraft.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (!str_cmp(arg1,"mod") || !str_cmp(arg1,"modify"))
    {
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
	    send_to_char("Syntax: Fleshcraft Modify <target> <modification>\n\r",ch);
	    return;
	}
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (IS_NPC(victim))
	{
	    send_to_char( "You cannot fleshcraft an unwilling person.\n\r", ch );
	    return;
	}
	if (!IS_IMMUNE(victim,IMM_VAMPIRE) && ch != victim)
	{
	    send_to_char( "You cannot fleshcraft an unwilling person.\n\r", ch );
	    return;
	}
	if (!str_cmp(arg3,"fins"))
	{
	    if (IS_MOD(victim, MOD_FINS))
	    {
		if (ch == victim)
		{
		    send_to_char("You push your fins back into your body!\n\r",ch);
		    act("$n pushes $s fins back into $s body!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's fins back into $S body!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your fins back into your body!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's fins back into $S body!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_FINS);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull a pair of fins out of your body!\n\r",ch);
		    act("$n pulls a pair of fins out of $s body!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull a pair of fins out of $N's body!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls a pair of fins out of your body!",ch,NULL,victim,TO_VICT);
		    act("$n pulls a pair of fins out of $N's body!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_FINS);
	    }
	}
	else if (!str_cmp(arg3,"tail"))
	{
	    if (IS_MOD(victim, MOD_TAIL))
	    {
		if (ch == victim)
		{
		    send_to_char("You push your tail back into your body!\n\r",ch);
		    act("$n pushes $s tail back into $s body!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's tail back into $S body!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your tail back into your body!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's tail back into $S body!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_TAIL);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull a tail out of your backside!\n\r",ch);
		    act("$n pulls a tail out of $s backside!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull a tail out of $N's backside!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls a tail out of your backside!",ch,NULL,victim,TO_VICT);
		    act("$n pulls a tail out of $N's backside!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_TAIL);
	    }
	}
	else if (!str_cmp(arg3,"wings"))
	{
	    if (IS_MOD(victim, MOD_WINGS))
	    {
		if (IS_MOD(victim, MOD_UNFOLDED)) do_wings(victim,"");
		if (ch == victim)
		{
		    send_to_char("You push your wings back into your shoulders!\n\r",ch);
		    act("$n pushes $s wings back into $s shoulders!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's wings back into $S shoulders!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your wings back into your shoulders!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's wings back into $S shoulders!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_WINGS);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull a pair of wings out of your shoulders!\n\r",ch);
		    act("$n pulls a pair of wings out of $s shoulders!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull a pair of wings out of $N's shoulders!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls a pair of wings out of your shoulders!",ch,NULL,victim,TO_VICT);
		    act("$n pulls a pair of wings out of $N's shoulders!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_WINGS);
	    }
	}
	else send_to_char( "You cannot do that.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
	send_to_char( "Not while in Zulo Form.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
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
	if (!IS_POLYAFF(ch, POLY_FLESHCRAFT))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	sprintf(buf,"$n's features twist and distort until $e looks like %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->polyaff, POLY_FLESHCRAFT);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your features twist and distort until you looks like $n.",ch,NULL,NULL,TO_CHAR);
    	free_string( ch->long_descr );
    	ch->long_descr = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_FLESHCRAFT))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
	act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
	act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
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
    act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
    act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
    act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
    SET_BIT(ch->polyaff, POLY_FLESHCRAFT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
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

void do_bonecraft( CHAR_DATA *ch, char *argument )
{
    char      buf [MAX_STRING_LENGTH];
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    one_argument( argument, arg1 );
    smash_tilde(argument);

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_VICISSITUDE) < 3)
    {
	send_to_char("You require level 3 Vicissitude to use Bonecraft.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (!str_cmp(arg1,"mod") || !str_cmp(arg1,"modify"))
    {
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
	    send_to_char("Syntax: Bonecraft Modify <target> <modification>\n\r",ch);
	    return;
	}
	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (IS_NPC(victim))
	{
	    send_to_char( "You cannot bonecraft an unwilling person.\n\r", ch );
	    return;
	}
	if (!IS_IMMUNE(victim,IMM_VAMPIRE) && ch != victim)
	{
	    send_to_char( "You cannot bonecraft an unwilling person.\n\r", ch );
	    return;
	}
	if (!str_cmp(arg3,"tusks"))
	{
	    if (IS_MOD(victim, MOD_TUSKS))
	    {
		if (ch == victim)
		{
		    send_to_char("You push your tusks back into your jaw bone!\n\r",ch);
		    act("$n pushes $s tusks back $s jaw bone!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's tusks back into $S jaw bone!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your tusks back into your jaw bone!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's tusks back into $S jaw bone!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_TUSKS);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull a pair of tusks out of your jaw bone!\n\r",ch);
		    act("$n pulls a pair of tusks out of $s jaw bone!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull a pair of tusks out of $N's jaw bone!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls a pair of tusks out of your jaw bone!",ch,NULL,victim,TO_VICT);
		    act("$n pulls a pair of tusks out of $N's jaw bone!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_TUSKS);
	    }
	}
	else if (!str_cmp(arg3,"horns"))
	{
	    if (IS_MOD(victim, MOD_HORNS))
	    {
		if (ch == victim)
		{
		    send_to_char("You push your horns back into the bone of your skull!\n\r",ch);
		    act("$n pushes $s horns back into the bone of $s skull!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's horns back into the bone of $S skull!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your horns back into the bone of your skull!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's horns back into the bone of $S skull!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_HORNS);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull a pair of horns out of the bone of your skull!\n\r",ch);
		    act("$n pulls a pair of horns out of the bone of $s skull!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull a pair of horns out of the bone of $N's skull!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls a pair of horns out of the bone of your skull!",ch,NULL,victim,TO_VICT);
		    act("$n pulls a pair of horns out of the bone of $N's skull!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_HORNS);
	    }
	}
	else if (!str_cmp(arg3,"exoskeleton"))
	{
	    if (IS_MOD(victim, MOD_EXOSKELETON))
	    {
		if (ch == victim)
		{
		    send_to_char("You push your exoskeleton back under your skin!\n\r",ch);
		    act("$n pushes $s exoskeleton back under $s skin!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You push $N's exoskeleton back under $S skin!",ch,NULL,victim,TO_CHAR);
		    act("$n pushes your exoskeleton back under your skin!",ch,NULL,victim,TO_VICT);
		    act("$n pushes $N's exoskeleton back under $S skin!",ch,NULL,victim,TO_NOTVICT);
		}
		REMOVE_BIT(victim->pcdata->bodymods, MOD_EXOSKELETON);
	    }
	    else
	    {
		if (ch == victim)
		{
		    send_to_char("You pull out an exoskeleton from under your skin!\n\r",ch);
		    act("$n pulls out an exoskeleton from under $s skin!",ch,NULL,NULL,TO_ROOM);
		}
		else
		{
		    act("You pull out an exoskeleton from under $N's skin!",ch,NULL,victim,TO_CHAR);
		    act("$n pulls out an exoskeleton from under your skin!",ch,NULL,victim,TO_VICT);
		    act("$n pulls out an exoskeleton from under $N's skin!",ch,NULL,victim,TO_NOTVICT);
		}
		SET_BIT(victim->pcdata->bodymods, MOD_EXOSKELETON);
	    }
	}
	else send_to_char( "You cannot do that.\n\r", ch );
	return;
    }

    if ( strlen(argument) > 20 )
    {
	send_to_char( "Please limit your name to 20 characters or less.\n\r", ch );
	return;
    }

    if ( str_cmp(argument,"self") && !check_parse_name( argument ) )
    {
	send_to_char( "Illegal name.\n\r", ch );
	return;
    }

    if (is_in(argument,"|kavir*pandora*arioch*trial*"))
    {
	send_to_char( "Illegal name.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
	send_to_char( "Not while in Zulo Form.\n\r", ch );
	return;
    }

    if ( !str_cmp(argument,ch->name) || !str_cmp(argument,"self") )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	if (!IS_POLYAFF(ch, POLY_BONECRAFT))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	sprintf(buf,"$n's features twist and distort until $e looks like %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->polyaff, POLY_BONECRAFT);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your features twist and distort until you look like $n.",ch,NULL,NULL,TO_CHAR);
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_BONECRAFT))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	sprintf(buf,"Your features twist and distort until you look like %s.\n\r",argument);
	send_to_char(buf,ch);
	sprintf(buf,"$n's features twist and distort until $e looks like %s.",argument);
	act(buf,ch,NULL,NULL,TO_ROOM);
    	free_string( ch->morph );
	ch->morph = str_dup( argument );
	return;
    }
    sprintf(buf,"Your features twist and distort until you look like %s.\n\r",argument);
    send_to_char(buf,ch);
    sprintf(buf,"$n's features twist and distort until $e looks like %s.",argument);
    act(buf,ch,NULL,NULL,TO_ROOM);
    SET_BIT(ch->polyaff, POLY_BONECRAFT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    free_string( ch->morph );
    ch->morph = str_dup( argument );
    return;
}

void do_zuloform( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_VICISSITUDE) < 4)
    {
	send_to_char("You require level 4 Vicissitude to change into Zulo Form.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
/*
	if (IS_MOD(ch, MOD_UNFOLDED) && IS_MOD(ch, MOD_WINGS)) do_wings(ch,"");
*/
    	free_string( ch->morph );
	if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	    ch->morph = str_dup( "a huge werewolf" );
	else
	{
	    ch->morph = str_dup( "" );
	    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	    REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	    REMOVE_BIT(ch->vampaff, VAM_CLAWS);
	}

	REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
	REMOVE_BIT(ch->polyaff, POLY_ZULO);
/*
	REMOVE_BIT(ch->pcdata->bodymods, MOD_WINGS);
*/
	send_to_char("Your body shrinks and distorts back to its normal form.\n\r",ch);
	if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	    act("An enormous black hairy monster's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	else
	    act("A huge black monster's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
    {
	send_to_char("Your body grows and distorts into an enormous black hairy monster.\n\r",ch);
	act("$n's body grows and distorts into an enormous black hairy monster.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("Your body grows and distorts into a huge black monster.\n\r",ch);
	act("$n's body grows and distorts into a huge black monster.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->vampaff, VAM_CLAWS);
    SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    SET_BIT(ch->polyaff, POLY_ZULO);
/*
    if (get_disc(ch,DISC_VICISSITUDE) > 5)
	SET_BIT(ch->pcdata->bodymods, MOD_WINGS);
*/
    free_string( ch->morph );
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	ch->morph = str_dup( "an enormous black hairy monster" );
    else
	ch->morph = str_dup( "a huge black monster" );
    return;
}

void do_plasmaform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    if (IS_NPC(ch)) return;

    if ((!IS_VAMPIRE(ch) && !IS_GHOUL(ch)) || IS_HEAD(ch, LOST_HEAD))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_VICISSITUDE) < 5)
    {
	send_to_char("You require level 5 Vicissitude to change into Plasma Form.\n\r",ch);
	return;
    }

    if (!IS_MORE(ch, MORE_PLASMA) && IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("Not from this form.\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) != NULL && IS_EXTRA(ch, EXTRA_OSWITCH))
    {
	ch->pcdata->obj_vnum = 0;
	obj->chobj = NULL;
	ch->pcdata->chobj = NULL;
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
	REMOVE_BIT(ch->more, MORE_PLASMA);
	free_string(ch->morph);
	ch->morph = str_dup("");
	act("$p transforms into $n.",ch,obj,NULL,TO_ROOM);
	act("Your reform your human body.",ch,obj,NULL,TO_CHAR);
	extract_obj(obj);
	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
	{
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_HELL));
	}
	return;
    }
    if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
    if ((obj = create_object(get_obj_index(OBJ_VNUM_SPILT_BLOOD),0)) == NULL)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    obj_to_room(obj,ch->in_room);
    SET_BIT(obj->quest, QUEST_VANISH);
    act("$n transforms into $p and splashes to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and splash to the ground.",ch,obj,NULL,TO_CHAR);
    if (IS_EXTRA(ch, TIED_UP))
    {
	act("The ropes binding you fall through your liquid form.",ch,NULL,NULL,TO_CHAR);
	act("The ropes binding $n fall through $s liquid form.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->extra, TIED_UP);
	REMOVE_BIT(ch->extra, GAGGED);
	REMOVE_BIT(ch->extra, BLINDFOLDED);
    }
    if (is_affected(ch, gsn_web))
    {
	act("The webbing entrapping $n falls through $s liquid form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you falls through your liquid form.\n\r",ch);
	affect_strip(ch, gsn_web);
    }
    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	act("The webbing entrapping $n falls through $s liquid form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you falls through your liquid form.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_WEBBED);
    }
    if (is_affected(ch, gsn_tendrils))
    {
	act("The tendrils of darkness entrapping $n fall through $s liquid form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you fall through your liquid form.\n\r",ch);
	affect_strip(ch, gsn_tendrils);
    }
    if (IS_AFFECTED(ch, AFF_SHADOW))
    {
	act("The tendrils of darkness entrapping $n fall through $s liquid form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you fall through your liquid form.\n\r",ch);
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
    ch->pcdata->obj_vnum = OBJ_VNUM_SPILT_BLOOD;
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    SET_BIT(ch->more, MORE_PLASMA);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}

void do_portal( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];
    int        duration;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && (!IS_DEMON(ch) || IS_ANGEL(ch)))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 3)
    {
	send_to_char("You require level 3 Daimoinon to create an Infernal Portal.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0')
    {
	send_to_char( "Who do you wish to create a portal to?\n\r", ch );
	return;
    }

    victim = get_char_world( ch, arg );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   ch->in_room == NULL
    ||   IS_NPC(victim) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !IS_IMMUNE(victim, IMM_SUMMON)
    ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->in_room->vnum == ch->in_room->vnum)
    {
	send_to_char( "You failed.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to create an Infernal Portal to you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    duration = number_range(2,3) * 15;

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->value[0] = victim->in_room->vnum;
    obj->value[3] = ch->in_room->vnum;
    obj->timer = duration;
    free_string(obj->short_descr);
    obj->short_descr = str_dup("an infernal portal");
    free_string(obj->description);
    obj->description = str_dup("An infernal portal of burning flames floats here.");
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, ch->in_room );

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->value[0] = ch->in_room->vnum;
    obj->value[3] = victim->in_room->vnum;
    obj->timer = duration;
    free_string(obj->short_descr);
    obj->short_descr = str_dup("an infernal portal");
    free_string(obj->description);
    obj->description = str_dup("An infernal portal of burning flames floats here.");
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, victim->in_room );

    act( "$p bursts from the ground in front of $n.", ch, obj, NULL, TO_ROOM );
    act( "$p bursts from the ground appears in front of you.", ch, obj, NULL, TO_CHAR );
    act( "$p bursts from the ground appears in front of $n.", victim, obj, NULL, TO_ROOM );
    act( "$p bursts from the ground appears in front of you.", ch, obj, victim, TO_VICT );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just created an Infernal Portal to you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_soultwist( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       damage;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && (!IS_DEMON(ch) || IS_ANGEL(ch)))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 5)
    {
	send_to_char("You require level 5 Daimoinon to use Soul Twist.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Soul Twist on?\n\r", ch );
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
	    act("$n just failed to use Soul Twist on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_EVIL(ch))
    {
	send_to_char( "You are not evil enough to use Soul Twist.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Soul Twist on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (victim->alignment < 1)
    {
	send_to_char( "They are not good enough to use Soul Twist on.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Soul Twist on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    damage = victim->alignment;

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You scream in agony as your very soul seems to be torn apart!\n\r",victim);
    act("$n clutches $s chest and screams in agony!",victim,NULL,NULL,TO_ROOM);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Soul Twist on you.",ch,NULL,victim,TO_VICT);
    }

    victim->hit -= damage;
    if ((IS_NPC(victim) && victim->hit < 1) || 
	(!IS_NPC(victim) && victim->hit < -10))
    {
	act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	demon_gain(ch,victim);
	group_gain(ch,victim);
	victim->form = 9;
	killperson(ch,victim);
	WAIT_STATE( ch, 12 );
	return;
    }
    update_pos(victim);
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void do_curse( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    int       critical = number_range(1,4);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && (!IS_DEMON(ch) || IS_ANGEL(ch)))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 4)
    {
	send_to_char("You require level 4 Daimoinon to use Curse.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to curse?\n\r", ch );
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
	    act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (number_percent() > (14 - ch->vampgen)*get_disc(ch,DISC_DAIMOINON))
    {
	send_to_char("Nothing happens.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[2],LOST_ARM);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	send_to_char("Your scream in agony as your left arm falls off!\n\r",victim);
	act("$n screams in agony as $s left arm falls off!",victim,NULL,NULL,TO_ROOM);
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
	    act("$n just used Curse on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[3],LOST_ARM);
	if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	send_to_char("Your scream in agony as your right arm falls off!\n\r",victim);
	act("$n screams in agony as $s right arm falls off!",victim,NULL,NULL,TO_ROOM);
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
	    act("$n just used Curse on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[4],LOST_LEG);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	send_to_char("Your scream in agony as your left leg falls off!\n\r",victim);
	act("$n screams in agony as $s left leg falls off!",victim,NULL,NULL,TO_ROOM);
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
	    act("$n just used Curse on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[5],LOST_LEG);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	send_to_char("Your scream in agony as your right leg falls off!\n\r",victim);
	act("$n screams in agony as $s right leg falls off!",victim,NULL,NULL,TO_ROOM);
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
	    act("$n just used Curse on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    send_to_char("Nothing happens.\n\r",ch);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just failed to use Curse on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_conditioning( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DOMINATE) < 2)
    {
	send_to_char("You require level 2 Dominate to use Conditioning.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You can't condition yourself!\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if (IS_ITEMAFF(victim, ITEMA_SHADES) || IS_ITEMAFF(ch, ITEMA_SHADES) || 
	!can_see(victim,ch))
    {
	if (!IS_SET(ch->act,PLR_HOLYLIGHT) && get_disc(ch,DISC_DOMINATE) < 6)
	{
	    send_to_char( "You are unable to make eye contact with them.\n\r", ch );
	    return;
	}
    }

    act("You stare deeply into $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n stares deeply into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);

    if (strlen(victim->pcdata->conding) > 2 && 
	!str_cmp(victim->pcdata->conding,ch->name))
    {
	free_string(victim->pcdata->conding);
	victim->pcdata->conding = str_dup( "" );
	REMOVE_BIT(victim->more, MORE_LOYAL);
	act("You remove $N's conditioning.",ch,NULL,victim,TO_CHAR);
	act("$n removes your conditioning.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Conditioning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && IS_VAMPIRE(victim) && victim->vampgen < ch->vampgen)
    {
	act("You shake off $N's attempt to Condition you.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's attempt to Condition $m.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your attempt to Condition $m.",victim,NULL,ch,TO_VICT);
	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Conditioning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_DOMINATE] > 0)
    {
	act("You shake off $N's attempt to Condition you.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's attempt to Condition $m.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your attempt to Condition $m.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Conditioning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && ((victim->pcdata->willpower[0] >= 2 && 
	IS_MORE(victim, MORE_LOYAL)) || victim->pcdata->willpower[0] >= 5) &&
	strlen(victim->pcdata->conding) > 1)
    {
	if (IS_MORE(victim, MORE_LOYAL)) victim->pcdata->willpower[0] -= 2;
	else victim->pcdata->willpower[0] -= 5;
	victim->pcdata->resist[WILL_DOMINATE] = 60;
	send_to_char("You burn some willpower to resist Conditioning.\n\r",victim);
	act("You shake off $N's attempt to Condition you.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's attempt to Condition $m.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your attempt to Condition $m.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Conditioning on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Conditioning.\n\r",victim);
	act("You shake off $N's attempt to Condition you.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's attempt to Condition $m.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your attempt to Condition $m.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Conditioning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (strlen(victim->pcdata->conding) > 1)
    {
	free_string(victim->pcdata->conding);
	victim->pcdata->conding = str_dup( "" );
	REMOVE_BIT(victim->more, MORE_LOYAL);
	act("You remove $N's conditioning.",ch,NULL,victim,TO_CHAR);
	act("$n removes your conditioning.",ch,NULL,victim,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Conditioning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    free_string(victim->pcdata->conding);
    victim->pcdata->conding = str_dup( ch->name );
    REMOVE_BIT(victim->more, MORE_LOYAL);
    act("You condition $N's mind.",ch,NULL,victim,TO_CHAR);
    act("$n conditions your mind.",ch,NULL,victim,TO_VICT);
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Conditioning on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

int true_generation( CHAR_DATA *ch, char *argument )
{
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      arg4 [MAX_INPUT_LENGTH];
    char      arg5 [MAX_INPUT_LENGTH];
    char      arg6 [MAX_INPUT_LENGTH];
    char      arg7 [MAX_INPUT_LENGTH];
    char      arg8 [MAX_INPUT_LENGTH];
    char      arg9 [MAX_INPUT_LENGTH];
    char      arg10[MAX_INPUT_LENGTH];
    char      arg11[MAX_INPUT_LENGTH];
    char      arg12[MAX_INPUT_LENGTH];
    int       vampgen = 1;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );
    argument = one_argument( argument, arg6 );
    argument = one_argument( argument, arg7 );
    argument = one_argument( argument, arg8 );
    argument = one_argument( argument, arg9 );
    argument = one_argument( argument, arg10 );
    argument = one_argument( argument, arg11 );
    argument = one_argument( argument, arg12 );

    if (IS_NPC(ch)) return 0;

    if (arg1[0] != '\0')  {arg1[0] = UPPER(arg1[0]);vampgen = 2;}
    if (arg2[0] != '\0')  {arg2[0] = UPPER(arg2[0]);vampgen = 3;}
    if (arg3[0] != '\0')  {arg3[0] = UPPER(arg3[0]);vampgen = 4;}
    if (arg4[0] != '\0')  {arg4[0] = UPPER(arg4[0]);vampgen = 5;}
    if (arg5[0] != '\0')  {arg5[0] = UPPER(arg5[0]);vampgen = 6;}
    if (arg6[0] != '\0')  {arg6[0] = UPPER(arg6[0]);vampgen = 7;}
    if (arg7[0] != '\0')  {arg7[0] = UPPER(arg7[0]);vampgen = 8;}
    if (arg8[0] != '\0')  {arg8[0] = UPPER(arg8[0]);vampgen = 9;}
    if (arg9[0] != '\0')  {arg9[0] = UPPER(arg9[0]);vampgen = 10;}
    if (arg10[0] != '\0') {arg10[0] = UPPER(arg10[0]);vampgen = 11;}
    if (arg11[0] != '\0') {arg11[0] = UPPER(arg11[0]);vampgen = 12;}
    if (arg12[0] != '\0') {arg12[0] = UPPER(arg12[0]);vampgen = 13;}

    return vampgen;
}

void do_status( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (ch->race > 0)
    {
	send_to_char("You already have a reputation.\n\r",ch);
	return;
    }

    if (ch->race < 0)
    {
	send_to_char("Your reputation is too bad already.\n\r",ch);
	return;
    }

    if (ch->exp < 1)
    {
	send_to_char("It costs 1 exp to make yourself Known.\n\r",ch);
	return;
    }

    ch->race = 1;
    ch->exp--;
    send_to_char("You are now Known.\n\r",ch);
    return;
}

void do_tongue( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int dam;
    int stance;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_SERPENTIS) < 1)
    {
	send_to_char("You require level 1 Serpentis to use Tongue of the Serpent.\n\r",ch);
	return;
    }

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use Tongue of the Serpent on?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    if ( ( obj = get_obj_carry( ch, arg ) ) != NULL )
	    {
		if ( obj->item_type != ITEM_WEAPON )
		{
		    send_to_char( "You can only poison weapons with your serpentine tongue.\n\r", ch );
		    return;
		}
		if ( obj->value[0] != 0 )
		{
		    send_to_char( "This weapon cannot be poisoned.\n\r", ch );
		    return;
		}
		act("You run your long serpentine tongue along $p, poisoning it.",ch,obj,NULL,TO_CHAR);
		act("$n runs $s long serpentine tongue along $p, poisoning it.",ch,obj,NULL,TO_ROOM);
		obj->level = 5 * get_disc(ch,DISC_SERPENTIS);
		obj->value[0] = 53;
		return;
	    }
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
	    act("$n just failed to use Tongue of the Serpent on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    WAIT_STATE( ch, 6 );

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

    if ( !IS_NPC(ch) ) dam = dam + (dam * ((ch->wpn[0]) * 0.01));

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

    damage( ch, victim, dam, gsn_tongue );
    return;
}

void do_mummify( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_SERPENTIS) < 2)
    {
	send_to_char("You require level 2 Serpentis to Mummify yourself.\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) != NULL && IS_EXTRA(ch, EXTRA_OSWITCH))
    {
	REMOVE_BIT(ch->more, MORE_MUMMIFIED);
	ch->pcdata->obj_vnum = 0;
	obj->chobj = NULL;
	ch->pcdata->chobj = NULL;
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
	free_string(ch->morph);
	ch->morph = str_dup("");
	act("$p clambers to $s feet.",ch,obj,NULL,TO_ROOM);
	act("Your body unshrivels and returns to normal.",ch,obj,NULL,TO_CHAR);
	extract_obj(obj);
	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
	{
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_HELL));
	}
	return;
    }
    if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
    if ((obj = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC),0)) == NULL)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    mummify_corpse(ch,obj);
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    obj_to_room(obj,ch->in_room);
    SET_BIT(obj->quest, QUEST_VANISH);
    act("$n shrivels up and falls to the ground.",ch,NULL,NULL,TO_ROOM);
    act("You shrivel up and fall to the ground.",ch,NULL,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = OBJ_VNUM_CORPSE_PC;
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->more, MORE_MUMMIFIED);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}

void mummify_corpse( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (obj->pIndexData->vnum != OBJ_VNUM_CORPSE_PC)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    sprintf(buf, obj->short_descr, ch->name);
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );

    sprintf(buf, obj->description, ch->name);
    free_string(obj->description);
    obj->description = str_dup( buf );

    SET_BIT(obj->quest, QUEST_VANISH);

    return;
}

void staked_corpse( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (obj->pIndexData->vnum != OBJ_VNUM_CORPSE_PC)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    sprintf(buf, "corpse body %s", ch->name);
    free_string(obj->name);
    obj->name = str_dup( buf );

    sprintf(buf, obj->short_descr, ch->name);
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );

    sprintf(buf, "The body of %s is lying here with a stake through its chest.", ch->name);
    free_string(obj->description);
    obj->description = str_dup( buf );

    obj->weight = 150;
    obj->wear_flags = ITEM_TAKE;
    SET_BIT(obj->quest, QUEST_VANISH);

    return;
}

void do_baal( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int sn, blood, potence, celerity, fortitude;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 9)
    {
	send_to_char("You require level 9 Daimoinon to become Baal's Avatar.\n\r",ch);
	return;
    }

    if (is_affected(ch, gsn_demon))
    {
	send_to_char("You are already possessed by the spirit of Baal.\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_POTENCE) < 5)
	potence = 5 - get_disc(ch,DISC_POTENCE);
    else if (get_disc(ch,DISC_POTENCE) < 10) potence = 1;
    else potence = 0;

    if (get_disc(ch,DISC_CELERITY) < 5)
	celerity = 5 - get_disc(ch,DISC_CELERITY);
    else if (get_disc(ch,DISC_CELERITY) < 10) celerity = 1;
    else celerity = 0;

    if (get_disc(ch,DISC_FORTITUDE) < 5)
	fortitude = 5 - get_disc(ch,DISC_FORTITUDE);
    else if (get_disc(ch,DISC_FORTITUDE) < 10) fortitude = 1;
    else fortitude = 0;

    blood = (potence + celerity + fortitude) * 10;

    if (blood < 1)
    {
	send_to_char("The spirit of Baal doesn't answer your call.\n\r",ch);
	return;
    }

    if (ch->blood[0] < blood)
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->blood[0] -= blood;

    if ( ( sn = skill_lookup( "demonic possession" ) ) < 0 ) return;

    af.type      = sn;
    af.duration  = get_disc(ch,DISC_DAIMOINON);
    af.bitvector = 0;
    if (potence > 0)
    {
	af.location  = APPLY_POTENCE;
	af.modifier  = potence;
	affect_to_char( ch, &af );
    }
    if (celerity > 0)
    {
	af.location  = APPLY_CELERITY;
	af.modifier  = celerity;
	affect_to_char( ch, &af );
    }
    if (fortitude > 0)
    {
	af.location  = APPLY_FORTITUDE;
	af.modifier  = fortitude;
	affect_to_char( ch, &af );
    }

    act( "$n's eyes burn with unholy strength!", ch, NULL, NULL, TO_ROOM );
    send_to_char("Your eyes glow as you are infused with some of the power of Baal!\n\r",ch);

    WAIT_STATE( ch, 12 );
    return;
}

void do_servant( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int sn;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 8)
    {
	send_to_char("You require level 8 Daimoinon to summon a Demonic Servant.\n\r",ch);
	return;
    }

    if (ch->pcdata->followers > 4)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    ch->pcdata->followers++;
    victim=create_mobile( get_mob_index( MOB_VNUM_GUARDIAN ) );
    char_to_room(victim,get_room_index(ROOM_VNUM_DISCONNECTION));
    free_string(victim->lord);
    victim->lord = str_dup(ch->name);
    free_string(victim->name);
    victim->name = str_dup("demon servant");
    free_string(victim->short_descr);
    victim->short_descr = str_dup("a demonic servant");
    free_string(victim->long_descr);
    victim->long_descr = str_dup("A demonic servant stands here, awaiting orders.\n\r");
    victim->sex = 1;
    victim->alignment = -1000;
    free_string(victim->description);
    victim->description = str_dup("He looks extremely powerful.\n\r");

    victim->level = 100;
    victim->hit = 30000;
    victim->max_hit = 30000;
    victim->mana = 0;
    victim->max_mana = 1000;
    victim->move = 1000;
    victim->max_move = 1000;
    victim->hitroll = char_hitroll(ch) - 20;
    victim->damroll = char_damroll(ch) - 20;
    victim->armor = char_ac(ch) + 100;
    char_from_room(victim);
    char_to_room(victim,ch->in_room);

    strcpy(buf,"Come forth, creature of darkness, and do my bidding!");
    do_say( ch, buf );

    send_to_char( "A demon bursts from the ground and bows before you.\n\r",ch );
    act( "$N bursts from the ground and bows before $n.", ch, NULL, victim, TO_ROOM );

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

void do_controlbeast( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int  cost = (100 - ch->truebeast);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->pcdata->wolf > 0)
    {
	send_to_char("Not while the beast is in control!\n\r",ch);
	return;
    }

    if (ch->beast > 99 || ch->truebeast > 99)
    {
	send_to_char("The beast has already taken over.\n\r",ch);
	return;
    }

    if (ch->pcdata->diableries[DIAB_EVER] > 0)
    {
	send_to_char("Those who drink of the hearts blood may not attain Golconda.\n\r",ch);
	return;
    }

    if (ch->beast < 1)
    {
	send_to_char("But you have already attained Golconda!\n\r",ch);
	return;
    }

    if (ch->truebeast < 1)
    {
	send_to_char("You are unable to exert any further control over your beast.\n\r",ch);
	return;
    }

    if (ch->practice < cost )
    {
	sprintf(buf,"You need %d primal to try and control your beast.\n\r",cost);
	send_to_char(buf,ch);
	return;
    }

    if (arg[0] == '\0' || str_cmp(arg,"buy"))
    {
	sprintf(buf,"To lower your beast for the cost of %d primal, type: 'controlbeast buy'.\n\r",cost);
	send_to_char(buf,ch);
	return;
    }

    ch->practice -= cost;
    ch->truebeast--;
    send_to_char("You exert some control over your beast.\n\r",ch);
    return;
}

void do_shroudnight( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (IS_ABOMINATION(ch))
    {
	if (get_disc(ch,DISC_OBTENEBRATION) < 1 && get_tribe(ch,TRIBE_UKTENA) < 1)
	{
	    send_to_char("You require either level 1 Uktena or level 1 Obtenebration to summon darkness.\n\r",ch);
	    return;
	}
    }
    else if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	if (!IS_WEREWOLF(ch))
	{
	    send_to_char( "Huh?\n\r", ch );
	    return;
	}
	if (get_tribe(ch,TRIBE_UKTENA) < 1)
 	{
	    send_to_char("You require the level 1 power of the Uktena to call the Shroud of Night.\n\r",ch);
	    return;
	}
    }
    else if (!IS_WEREWOLF(ch) && get_disc(ch,DISC_OBTENEBRATION) < 1)
    {
	send_to_char("You require level 1 Obtenebration to use Shroud of Night.\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_DARKNESS))
    {
	REMOVE_BIT(ch->more, MORE_DARKNESS);
	REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
	send_to_char("You banish the darkness from the room!\n\r", ch);
	act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
	act("$n banishes the darkness from the room!",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->darkness = 60;
	return;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
    {
	send_to_char("The room is already in darkness.\n\r", ch);
	return;
    }

    if (ch->pcdata->darkness > 0)
    {
	if (ch->pcdata->darkness > 1)
	    sprintf(buf,"You cannot call upon the Shroud of Night for another %d seconds.\n\r",ch->pcdata->darkness);
	else
	    sprintf(buf,"You cannot call upon the Shroud of Night for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    SET_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
    SET_BIT(ch->more, MORE_DARKNESS);
    ch->pcdata->darkness = 60;
    send_to_char("You call forth the Shroud of Night to darken the room!\n\r", ch);
    act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    act("The room is filled with darkness!",ch,NULL,NULL,TO_ROOM);
    return;
}  

void do_silencedeath( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_QUIETUS) < 1)
    {
	send_to_char("You require level 1 Quietus to use Silence of Death.\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_SILENCE))
    {
	REMOVE_BIT(ch->more, MORE_SILENCE);
	REMOVE_BIT(ch->in_room->added_flags, ROOM2_SILENCE);
	send_to_char("You banish the silence from the room!\n\r", ch);
	act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
	act("The room is no longer silent.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->silence = 60;
	return;
    }

    if (IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	send_to_char("The room is already silent.\n\r", ch);
	return;
    }

    if (ch->pcdata->silence > 0)
    {
	if (ch->pcdata->silence > 1)
	    sprintf(buf,"You cannot call upon the Silence of Death for another %d seconds.\n\r",ch->pcdata->silence);
	else
	    sprintf(buf,"You cannot call upon the Silence of Death for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    SET_BIT(ch->in_room->added_flags, ROOM2_SILENCE);
    SET_BIT(ch->more, MORE_SILENCE);
    ch->pcdata->silence = 60;
    send_to_char("You call forth the Silence of Death!\n\r", ch);
    act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    act("The room suddenly goes completely silent!",ch,NULL,NULL,TO_ROOM);
    return;
}  

void do_weakness( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];
    int sn;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_QUIETUS) < 2)
    {
	send_to_char("You require level 2 Quietus to use Weakness.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0')
    {
	send_to_char("Who do you wish to use Weakness on?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "Now that would just be plain stupid...\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "weakness" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Weakness on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    send_to_char("You feel weaker!\n\r",victim);
    act("$n looks weaker.",victim,NULL,NULL,TO_ROOM);

    af.type      = sn;
    af.location  = APPLY_STR;
    af.modifier  = 0 - get_disc(ch,DISC_QUIETUS);
    af.duration  = number_range(1,2);
    af.bitvector = 0;
    affect_join( victim, &af );
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Weakness on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_infirmity( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];
    int sn;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_QUIETUS) < 3)
    {
	send_to_char("You require level 3 Quietus to use Infirmity.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0')
    {
	send_to_char("Who do you wish to use Infirmity on?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "Now that would just be plain stupid...\n\r", ch );
	return;
    }

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Infirmity on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "weakness" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    send_to_char("You feel weaker!\n\r",victim);
    send_to_char("You feel slower!\n\r",victim);
    send_to_char("You feel less healthy!\n\r",victim);
    act("$n looks weaker.",victim,NULL,NULL,TO_ROOM);
    act("$n looks slower.",victim,NULL,NULL,TO_ROOM);
    act("$n looks less healthy.",victim,NULL,NULL,TO_ROOM);

    af.type      = sn;
    af.location  = APPLY_STR;
    af.modifier  = 0 - get_disc(ch,DISC_QUIETUS);
    af.duration  = number_range(1,2);
    af.bitvector = 0;
    affect_join( victim, &af );

    if ( ( sn = skill_lookup( "slowness" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.location  = APPLY_DEX;
    af.modifier  = 0 - get_disc(ch,DISC_QUIETUS);
    af.duration  = number_range(1,2);
    af.bitvector = 0;
    affect_join( victim, &af );

    if ( ( sn = skill_lookup( "infirmity" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.location  = APPLY_CON;
    af.modifier  = 0 - get_disc(ch,DISC_QUIETUS);
    af.duration  = number_range(1,2);
    af.bitvector = 0;
    affect_join( victim, &af );
    WAIT_STATE( ch, 12 );
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Infirmity on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_bloodagony( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_QUIETUS) < 4)
    {
	send_to_char("You require level 4 Quietus to use Blood Agony.\n\r",ch);
	return;
    }

    if (ch->pcdata->bpoints > 0)
    {
	if (ch->pcdata->bpoints > 1)
	    sprintf(buf,"You cannot use Blood Agony for another %d seconds.\n\r",ch->pcdata->bpoints);
	else
	    sprintf(buf,"You cannot use Blood Agony for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( ch->blood[BLOOD_CURRENT] < 10 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What weapon do you wish to use Blood Agony on?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You are not carrying that object.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_WEAPON )
    {
	send_to_char( "You may only use Blood Agony on a weapon.\n\r", ch );
	return;
    }

    if ( IS_SET(obj->quest, QUEST_BLOODAGONY) )
    {
	send_to_char( "This weapon is already covered in blood.\n\r", ch );
	return;
    }

    if ( IS_SET(obj->quest, QUEST_BLOODAGONY)
    ||   IS_SET(obj->quest, QUEST_ARTIFACT)
    ||   IS_SET(obj->quest, QUEST_RELIC)
    ||   obj->chobj != NULL )
    {
	send_to_char("You are unable to smear any blood on this weapon.\n\r",ch);
	return;
    }

    act("You cut open your wrist and smear some blood onto $p.",ch,obj,NULL,TO_CHAR);
    act("$n cuts open $s wrist and smears some blood onto $p.",ch,obj,NULL,TO_ROOM);
    SET_BIT(obj->quest, QUEST_BLOODAGONY);
    ch->blood[BLOOD_CURRENT] -= 10;
    ch->pcdata->bpoints = 60;

    if ( obj->spectype == 0 || obj->spectype == SITEM_SILVER )
    {
	SET_BIT(obj->spectype, SITEM_MESSAGE);
	free_string(obj->chpoweruse);
	obj->chpoweruse = str_dup( "A few drops of blood drip from $p." );
	free_string(obj->victpoweruse);
	obj->victpoweruse = str_dup( "A few drops of blood drip from $p carried by $n." );
	obj->specpower = ch->specpower = get_disc(ch, DISC_QUIETUS) << 2;
    }

    return;
}

void do_spit( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj_lose;
    OBJ_DATA *obj_next;
    int dam;
    int stance;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_QUIETUS) < 5)
    {
	send_to_char("You require level 5 Quietus to use Taste of Death.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use Taste of Death on?\n\r",ch);
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

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Taste of Death on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    WAIT_STATE( ch, 6 );

    dam = number_range(10,20);

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) && get_disc(ch,DISC_POTENCE) > 0)
	dam += dam * get_disc(ch,DISC_POTENCE) * 0.5;
    else if ( !IS_NPC(ch) && IS_DEMON(ch) && 
	IS_SET(ch->pcdata->disc[C_POWERS], DEM_MIGHT) )
	dam *= 1.5;

    if ( !IS_NPC(ch) ) dam = dam + (dam * ((ch->wpn[0]) * 0.01));
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

    damage( ch, victim, dam, gsn_spit + TYPE_ADD_AGG );

    if ( number_percent( ) < get_disc(ch,DISC_QUIETUS) )
    {
	for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
	{
	    obj_next = obj_lose->next_content;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->condition > 0 )
		{
		    act( "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR );
		    obj_lose->condition -= 25;
		    if ( obj_lose->condition < 1 )
			extract_obj( obj_lose );
		}
		break;

	    case ITEM_SCROLL:
	    case ITEM_WAND:
	    case ITEM_STAFF:
	    case ITEM_TRASH:
	    case ITEM_POTION:
	    case ITEM_FURNITURE:
	    case ITEM_FOOD:
	    case ITEM_BOAT:
	    case ITEM_PILL:
	    case ITEM_CORPSE_NPC:
	    case ITEM_DRINK_CON:
	    case ITEM_CONTAINER:
		act( "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }
    return;
}

void do_shadowbody( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH]; 

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_OBTENEBRATION) < 3)
    {
	send_to_char("You require level 3 Obtenebration to transform into your Shadow Body.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH))
    {
	if (IS_POLYAFF(ch, POLY_SHADOW))
	{
	    REMOVE_BIT(ch->polyaff, POLY_SHADOW);
	    REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	}
	else
	{
	    send_to_char( "You seem to be stuck in this form.\n\r", ch );
	    return;
	}
	act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
    }
    else
    {
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	act( "You transform into a black smoke.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a black smoke.", ch, NULL, NULL, TO_ROOM );
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
	SET_BIT(ch->polyaff, POLY_SHADOW);
	clear_stats(ch);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->affected_by, AFF_ETHEREAL);
	sprintf(buf, "%s the black smoke", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
    }
    return;
}

void do_arms( CHAR_DATA *ch, char *argument )
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
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_OBTENEBRATION) < 2)
    {
	send_to_char("You require level 2 Obtenebration to use Arms of the Abyss.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use the Arms of the Abyss on?\n\r",ch);
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
	if (is_affected(ch, gsn_tendrils))
	{
	    act("The tendrils of darkness entrapping $n fall away.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The tendrils of darkness entrapping you fall away.\n\r",ch);
	    affect_strip(ch, gsn_tendrils);
	}
	else if (IS_AFFECTED(ch, AFF_SHADOW))
	{
	    act("The tendrils of darkness entrapping $n fall away.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The tendrils of darkness entrapping you fall away.\n\r",ch);
	    REMOVE_BIT(ch->affected_by, AFF_SHADOW);
	}
	else send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot use Arms of the Abyss on an ethereal person.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Arms of the Abyss on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Arms of the Abyss on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "tendrils" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = get_disc(ch,DISC_OBTENEBRATION) * 5;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
    return;
}

void power_ghoul( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int sn, disc;
    bool enhanced = FALSE;
    bool gain_disc;

    if ( IS_NPC(ch) || IS_NPC(victim) || !IS_HERO(victim) || !IS_VAMPIRE(ch)) 
	return;
    if (victim->class == CLASS_NONE) victim->class = CLASS_GHOUL;
    else if (victim->class != CLASS_GHOUL) return;
    for ( sn = 0; sn <= DISC_MAX; sn++ )
    {
	gain_disc = TRUE;
	switch ( sn )
	{
	    case DISC_ANIMALISM:
	    case DISC_AUSPEX:
	    case DISC_CELERITY:
	    case DISC_DOMINATE:
	    case DISC_FORTITUDE:
	    case DISC_OBFUSCATE:
	    case DISC_POTENCE:
	    case DISC_PRESENCE:
	    case DISC_PROTEAN:
	    case DISC_SERPENTIS:
	    case DISC_THAUMATURGY:
	    case DISC_VICISSITUDE:
		break;
	    default:
		gain_disc = FALSE;
		break;
	}
	if (!gain_disc) continue;
	if (ch->pcdata->powers[sn] > 0)
	    disc = (1 + ch->pcdata->powers[sn]) * 0.5;
	else disc = 0;

	if (disc > victim->pcdata->powers[sn])
	{
	    victim->pcdata->powers[sn] = disc;
	    enhanced = TRUE;
	}
    }

    if (enhanced)
	send_to_char("You feel the potency of the blood run through your body.\n\r",victim);

    return;
}

void do_lamprey( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int level, dam;
    int blood = number_range(1,10);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_OBTENEBRATION) < 4)
    {
	send_to_char("You require level 4 Obtenebration to Call the Lamprey.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to Call the Lamprey on?\n\r",ch);
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
	send_to_char( "That would not be a good idea.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot Call the Lamprey on an ethereal person.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Call the Lamprey on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Call the Lamprey on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    level = get_disc(ch,DISC_OBTENEBRATION) * 5;
    dam = blood * get_disc(ch,DISC_OBTENEBRATION) * level;
    damage(ch,victim,dam,gsn_tendrils);

    if (victim == NULL || victim->position == POS_DEAD) return;

    if ( victim->blood[BLOOD_CURRENT] < blood )
	blood = victim->blood[BLOOD_CURRENT];

    victim->blood[BLOOD_CURRENT] -= blood;
    ch->blood[BLOOD_CURRENT] += (blood * victim->blood[BLOOD_POTENCY]);
    if (ch->blood[BLOOD_CURRENT] >= ch->blood[BLOOD_POOL])
    {
	ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
	if (IS_VAMPIRE(ch)) send_to_char("Your blood lust is sated.\n\r",ch);
    }
    if (IS_NPC(victim) || !IS_VAMPIRE(victim))
    {
	power_ghoul(victim,ch);
	if (!IS_NPC(ch) && ch->pcdata->dpoints < (victim->blood[2] * 1000))
	{
	    ch->pcdata->dpoints += (blood * 60);
	    if (ch->pcdata->dpoints > (victim->blood[2] * 100))
		ch->pcdata->dpoints = (victim->blood[2] * 100);
	}
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
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void do_thanatopsis( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_NECROMANCY) < 1)
    {
	send_to_char("You require level 1 Necromancy to perform Thanatopsis on a corpse.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to perform Thanatopsis on?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You cannot seem to find that object.\n\r", ch );
	return;
    }

    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
    {
	send_to_char( "That isn't a corpse.\n\r", ch );
	return;
    }

    act("You examine $p carefully.",ch,obj,NULL,TO_CHAR);
    act("$n examines $p carefully.",ch,obj,NULL,TO_ROOM);

    if (strlen(obj->questmaker) < 1 || strlen(obj->questowner) < 1)
    {
	send_to_char( "You are unable to discover anything about the killer.\n\r", ch );
	return;
    }

    sprintf(buf,"It seems that %s was killed by %s.\n\r",obj->questowner,obj->questmaker);
    send_to_char(buf,ch);

    if ( ( location = ch->in_room ) == NULL )
    {
	send_to_char("Bug - Please inform KaVir.\n\r",ch);
	return;
    }
    if ( ( pRoomIndex = get_room_index(obj->specpower) ) == NULL )
    {
	sprintf(buf,"You are unsure where %s was killed.\n\r",obj->questowner);
	send_to_char(buf,ch);
    }
    else
    {
	if (location == pRoomIndex)
	    sprintf(buf,"It seems that %s was killed right here.\n\r", obj->questowner);
	else
	{
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);
	    sprintf(buf,"It seems that %s was killed at %s.\n\r", obj->questowner, ch->in_room->name);
	    char_from_room(ch);
	    char_to_room(ch,location);
	}
	send_to_char(buf,ch);
    }

    return;
}

void do_spiritgate( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *corpse;
    OBJ_DATA  *obj;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char       arg [MAX_INPUT_LENGTH];
    int        duration, destination;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_NECROMANCY) < 3)
    {
	send_to_char("You require level 3 Necromancy to create a Spirit Gate.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0')
    {
	send_to_char( "Which corpse do you wish to use to make a Spirit Gate?\n\r", ch );
	return;
    }

    if ( ( corpse = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You cannot seem to find that object.\n\r", ch );
	return;
    }

    if (corpse->item_type == ITEM_CORPSE_PC)
    {
	send_to_char( "Not on that type of corpse.\n\r", ch );
	return;
    }

    if (corpse->item_type != ITEM_CORPSE_NPC)
    {
	send_to_char( "That isn't a corpse.\n\r", ch );
	return;
    }

    duration = number_range(2,3) * 15;

    if ( ( location = ch->in_room ) == NULL )
    {
	send_to_char("Bug - Please inform KaVir.\n\r",ch);
	return;
    }
    if ( ( pRoomIndex = get_room_index(corpse->specpower) ) == NULL )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }
    if (location == pRoomIndex)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    destination = ch->in_room->vnum;

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->value[0] = corpse->specpower;
    obj->value[3] = destination;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("spirit gate portal");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a spirit gate");
    free_string(obj->description);
    obj->description = str_dup("A glowing spirit gate floats here.");
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, location );
    act( "$p shimmers into existance in front of $n.", ch, obj, NULL, TO_ROOM );
    act( "$p shimmers into existance in front of you.", ch, obj, NULL, TO_CHAR );

    char_from_room(ch);
    char_to_room(ch,pRoomIndex);

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->value[0] = destination;
    obj->value[3] = corpse->specpower;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("spirit gate portal");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a spirit gate");
    free_string(obj->description);
    obj->description = str_dup("A glowing spirit gate floats here.");
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
	obj->extra_flags = ITEM_SHADOWPLANE;
    obj_to_room( obj, ch->in_room );
    act( "$p shimmers into existance.", ch, obj, NULL, TO_ROOM );

    char_from_room(ch);
    char_to_room(ch,location);

    act("$p crumbles to dust.",ch,corpse,NULL,TO_CHAR);
    act("$n crumbles to dust.",ch,corpse,NULL,TO_ROOM);
    extract_obj(corpse);
    return;
}

void do_preservation( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_NECROMANCY) < 2)
    {
	send_to_char("You require level 2 Necromancy to perform Preservation.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to perform Preservation on?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You cannot seem to find that object.\n\r", ch );
	return;
    }

    if ( obj->chobj != NULL )
    {
	send_to_char( "You are unable to preserve it.\n\r", ch );
	return;
    }

    if (obj->item_type == ITEM_CORPSE_PC)
	send_to_char( "You cannot preserve that type of corpse.\n\r", ch );
    else if (obj->item_type == ITEM_FOOD)
    {
	act("$p shrivels up.",ch,obj,NULL,TO_CHAR);
	act("$p shrivels up.",ch,obj,NULL,TO_ROOM);
	if (obj->value[0] > 1) obj->value[0] *= 0.5;
	ch->carry_weight -= obj->weight;
	if (obj->weight > 1) obj->weight *= 0.5;
	ch->carry_weight += obj->weight;
	obj->timer = -1;
    }
    else if (obj->item_type == ITEM_CORPSE_NPC)
    {
	if (strlen(obj->questowner) < 1)
	{
	    act("You are unable to preserve $p.",ch,obj,NULL,TO_CHAR);
	    return;
	}

	if (obj->timer < 1)
	{
	    send_to_char( "It is already preserved.\n\r", ch );
	    return;
	}
	act("$p shrivels up.",ch,obj,NULL,TO_CHAR);
	act("$p shrivels up.",ch,obj,NULL,TO_ROOM);

	free_string(obj->short_descr);
	sprintf(buf, "the shriveled up corpse of %s",obj->questowner);
	obj->short_descr = str_dup(buf);

	free_string(obj->description);
	sprintf(buf, "The shriveled up corpse of %s is lying here.",obj->questowner);
	obj->description = str_dup(buf);

	ch->carry_weight -= obj->weight;
	if (obj->weight > 1) obj->weight *= 0.5;
	ch->carry_weight += obj->weight;
	obj->timer = -1;
    }
    else
	send_to_char( "You cannot preserve that object.\n\r", ch );
    return;
}

void do_zombie( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA		*obj;
    OBJ_DATA		*obj_content;
    OBJ_DATA		*obj_next;
    CHAR_DATA		*victim;
    MOB_INDEX_DATA	*pMobIndex;
    AFFECT_DATA af;
    int sn;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_NECROMANCY) < 5)
    {
	send_to_char("You require level 5 Necromancy to transform a corpse into a Zombie.\n\r",ch);
	return;
    }

    if (ch->pcdata->necromancy > 0)
    {
	if (ch->pcdata->necromancy > 1)
	    sprintf(buf,"You cannot animate a Zombie for another %d seconds.\n\r",ch->pcdata->necromancy);
	else
	    sprintf(buf,"You cannot animate a Zombie for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to animate as a Zombie?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You cannot seem to find that object.\n\r", ch );
	return;
    }

    if (obj->item_type != ITEM_CORPSE_NPC)
    {
	send_to_char( "You cannot animate that as a Zombie.\n\r", ch );
	return;
    }

    if (obj->in_room == NULL)
    {
	send_to_char( "You better drop it first...don't want the little bugger animating in your pants.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if ( ( pMobIndex = get_mob_index( obj->value[2] ) ) == NULL )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );

    if (victim->level > 200) victim->level = 200;
    victim->hitroll = victim->level;
    victim->damroll = victim->level;

    char_to_room(victim,ch->in_room);
    sprintf(buf,"The zombie of %s stands here.\n\r",victim->short_descr);
    free_string(victim->long_descr);
    victim->long_descr = str_dup(buf);
    sprintf(buf,"zombie %s",victim->name);
    free_string(victim->name);
    victim->name = str_dup(buf);
    sprintf(buf,"the zombie of %s",victim->short_descr);
    free_string(victim->short_descr);
    victim->short_descr = str_dup(buf);
    act("$n clambers to $s feet.",victim,NULL,NULL,TO_ROOM);
    free_string(victim->powertype);
    victim->powertype = str_dup("zombie");
    for ( obj_content = obj->contains; obj_content != NULL; obj_content = obj_next )
    {
	obj_next = obj_content->next_content;
	obj_from_obj(obj_content);
	obj_to_char(obj_content,victim);
    }
    extract_obj(obj);
    do_wear(victim,"all");

    ch->pcdata->necromancy = 60;

    if (ch->pcdata->followers > 4) return;

    free_string(victim->lord);
    victim->lord = str_dup(ch->name);

    ch->pcdata->followers++;

    if ( ( sn = skill_lookup( "charm person" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    add_follower( victim, ch );
    af.type      = sn;
    af.duration  = 5000;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );

    return;
}

void do_hagswrinkles( CHAR_DATA *ch, char *argument )
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

    if (get_disc(ch,DISC_THANATOSIS) < 1)
    {
	send_to_char("You require level 1 Thanatosis to use Hags Wrinkles.\n\r",ch);
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

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
	send_to_char( "Not while in Zulo Form.\n\r", ch );
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
	if (!IS_POLYAFF(ch, POLY_HAGS))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	sprintf(buf,"$n's features twist and distort until $e looks like %s.",ch->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->polyaff, POLY_HAGS);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	act("Your features twist and distort until you looks like $n.",ch,NULL,NULL,TO_CHAR);
    	free_string( ch->long_descr );
    	ch->long_descr = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	if (!IS_POLYAFF(ch, POLY_HAGS))
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
	act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
	act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
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
    act("Your features twist and distort until you looks like $N.",ch,NULL,victim,TO_CHAR);
    act("$n's features twist and distort until $e looks like you.",ch,NULL,victim,TO_VICT);
    act("$n's features twist and distort until $e looks like $N.",ch,NULL,victim,TO_NOTVICT);
    SET_BIT(ch->polyaff, POLY_HAGS);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
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

void do_ashes( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    if (IS_NPC(ch)) return;

    if ((!IS_VAMPIRE(ch) && !IS_GHOUL(ch)) || IS_HEAD(ch, LOST_HEAD))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_THANATOSIS) < 3)
    {
	send_to_char("You require level 3 Thanatosis to change into a pile of Ashes.\n\r",ch);
	return;
    }

    if (!IS_MORE(ch, MORE_ASHES) && IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("Not from this form.\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) != NULL && IS_EXTRA(ch, EXTRA_OSWITCH))
    {
	ch->pcdata->obj_vnum = 0;
	obj->chobj = NULL;
	ch->pcdata->chobj = NULL;
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
	REMOVE_BIT(ch->more, MORE_ASHES);
	free_string(ch->morph);
	ch->morph = str_dup("");
	act("$p transforms into $n.",ch,obj,NULL,TO_ROOM);
	act("Your reform your human body.",ch,obj,NULL,TO_CHAR);
	extract_obj(obj);
	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
	{
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_HELL));
	}
	return;
    }
    if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
    if ((obj = create_object(get_obj_index(OBJ_VNUM_ASHES),0)) == NULL)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    obj_to_room(obj,ch->in_room);
    SET_BIT(obj->quest, QUEST_VANISH);
    act("$n transforms into $p and falls to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and fall to the ground.",ch,obj,NULL,TO_CHAR);
    if (IS_EXTRA(ch, TIED_UP))
    {
	act("The ropes binding you fall through your form.",ch,NULL,NULL,TO_CHAR);
	act("The ropes binding $n fall through $s form.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->extra, TIED_UP);
	REMOVE_BIT(ch->extra, GAGGED);
	REMOVE_BIT(ch->extra, BLINDFOLDED);
    }
    if (is_affected(ch, gsn_web))
    {
	act("The webbing entrapping $n falls through $s form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you falls through your form.\n\r",ch);
	affect_strip(ch, gsn_web);
    }
    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	act("The webbing entrapping $n falls through $s form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The webbing entrapping you falls through your form.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_WEBBED);
    }
    if (is_affected(ch, gsn_tendrils))
    {
	act("The tendrils of darkness entrapping $n fall through $s form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you fall through your form.\n\r",ch);
	affect_strip(ch, gsn_tendrils);
    }
    if (IS_AFFECTED(ch, AFF_SHADOW))
    {
	act("The tendrils of darkness entrapping $n fall through $s form.",ch,NULL,NULL,TO_ROOM);
	send_to_char("The tendrils of darkness entrapping you fall through your form.\n\r",ch);
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
    ch->pcdata->obj_vnum = OBJ_VNUM_ASHES;
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    SET_BIT(ch->more, MORE_ASHES);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}

void do_withering( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    int       critical = number_range(1,4);

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THANATOSIS) < 4)
    {
	send_to_char("You require level 4 Thanatosis to use Withering.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Withering on?\n\r", ch );
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
	    act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (number_percent() > (14 - ch->vampgen)*get_disc(ch,DISC_THANATOSIS))
    {
	send_to_char("Nothing happens.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
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
	    act("$n just used Withering on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
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
	    act("$n just used Withering on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
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
	    act("$n just used Withering on you.",ch,NULL,victim,TO_VICT);
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
		act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
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
	    act("$n just used Withering on you.",ch,NULL,victim,TO_VICT);
	}
	if (IS_NPC(victim)) do_kill(victim,ch->name);
	return;
    }
    send_to_char("Nothing happens.\n\r",ch);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just failed to use Withering on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_drainlife( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    int       damage;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );


    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THANATOSIS) < 5)
    {
	send_to_char("You require level 5 Thanatosis to use Drain Life from someone.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who's lifeforce do you wish to Drain?\n\r", ch );
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

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Drain Life on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (ch->hit >= ch->max_hit)
    {
	send_to_char( "But you are already at full lifeforce!\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("You drain some of their lifeforce.\n\r",ch);
    send_to_char("You feel your lifeforce being drained away!\n\r",victim);

    damage = number_range(1,10) * (get_disc(ch,DISC_THANATOSIS) * 5);
    if (victim->hit <= damage) damage = victim->hit - 1;
    if (ch->hit + damage > ch->max_hit) damage -= (ch->max_hit - ch->hit);
    ch->hit += damage;
    victim->hit -= damage;
    if (victim->hit < 1)
    {
	act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	if (!IS_NPC(victim)) update_pos(victim);
	else 
	{
	    demon_gain(ch,victim);
	    group_gain(ch,victim);
	    victim->form = 10;
	    killperson(ch,victim);
	}
	return;
    }

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Drain Life on you.",ch,NULL,victim,TO_VICT);
    }
    if (IS_NPC(victim)) do_kill(victim,ch->name);
    WAIT_STATE( ch, 12 );
    return;
}

void do_putrefaction( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg [MAX_INPUT_LENGTH];
    int sn, level;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_THANATOSIS) < 2)
    {
	send_to_char("You require level 2 Thanatosis to use Putrefaction.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0')
    {
	send_to_char( "Who do you wish to use Putrefaction on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(victim, AFF_ROT))
    {
	send_to_char("Their flesh is already rotting.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) && IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char("They have no flesh to rot.\n\r",ch);
	return;
    }

    if (is_safe(ch, victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Putrefaction on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "rotting flesh" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    send_to_char("Your flesh begins to rot!\n\r",victim);
    act("$n's flesh begins to rot!",victim,NULL,NULL,TO_ROOM);

    level = get_disc(ch,DISC_THANATOSIS);

    af.type      = sn;
    af.location  = APPLY_CON;
    af.modifier  = 0 - level;
    af.duration  = number_range(level,(2*level));
    af.bitvector = AFF_ROT;
    affect_join( victim, &af );
    WAIT_STATE( ch, 12 );
    return;
}

void do_scream( CHAR_DATA *ch, char *argument )
{
    char      arg[MAX_INPUT_LENGTH];
    char      buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        damage;
    bool       no_dam = FALSE;

    one_argument( argument, arg );

    if (IS_NPC(ch)) no_dam = TRUE;
    else
    {
	if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch)) no_dam = TRUE;
	else if (get_disc(ch,DISC_MELPOMINEE) < 1) no_dam = TRUE;

	if (IS_WEREWOLF(ch) && get_tribe(ch, TRIBE_GET_OF_FENRIS) > 2) 
	    no_dam = FALSE;
    }

    if (!IS_NPC(ch) && ch->pcdata->reina > 0)
    {
	if (ch->pcdata->reina > 1)
	    sprintf(buf,"Your voice will not recover for another %d seconds.\n\r",ch->pcdata->reina);
	else
	    sprintf(buf,"Your voice will not recover for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You scream loudly!\n\r", ch );
	act("$n screams loudly!",ch,NULL,NULL,TO_ROOM);
	if (no_dam) return;
	if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");
	ch->pcdata->reina = 10;
	damage = get_disc(ch,DISC_MELPOMINEE) * 10;
	for ( victim = ch->in_room->people; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;

	    if ( victim == ch ) continue;
	    if (IS_IMMORTAL(victim)) continue;
	    if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) && 
		get_disc(victim,DISC_MELPOMINEE) > 0) continue;
	    if (is_safe(ch,victim))
	    {
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just used Scream on you.",ch,NULL,victim,TO_VICT);
		}
		return;
	    }
	    send_to_char("You clutch your head in agony as your eardrums seem to explode!\n\r",victim);
	    act("$n clutches $s head in agony!",victim,NULL,NULL,TO_ROOM);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Scream on you.",ch,NULL,victim,TO_VICT);
	    }
	    victim->hit -= damage;
	    if ((IS_NPC(victim) && victim->hit < 1) || 
		(!IS_NPC(victim) && victim->hit < -10))
	    {
		act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
		demon_gain(ch,victim);
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
		    send_to_char("You fall to the ground, stunned!\n\r",victim);
		    act("$n falls to the ground, stunned!",victim,NULL,NULL,TO_ROOM);
		}
		else if (IS_NPC(victim)) do_kill(victim,ch->name);
	    }
	}
	WAIT_STATE( ch, 12 );
	return;
    }
    else if ( ( victim = get_char_room( ch, arg ) ) != NULL )
    {
	if ( victim == ch )
	{
	    send_to_char( "You scream loudly at yourself!\n\r", ch );
	    act("$n screams loudly at $mself!",ch,NULL,NULL,TO_ROOM);
	    return;
	}
	else
	{
	    act("You scream loudly at $N!",ch,NULL,victim,TO_CHAR);
	    act("$n screams loudly at you!",ch,NULL,victim,TO_VICT);
	    act("$n screams loudly at $N!",ch,NULL,victim,TO_NOTVICT);
	}
	if (no_dam) return;
	if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");
	if (IS_IMMORTAL(victim)) return;
	if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) && 
	    get_disc(victim,DISC_MELPOMINEE) > 0) return;
	if (is_safe(ch,victim))
	{
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Scream on you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}

	damage = get_disc(ch,DISC_MELPOMINEE) * number_range(10,20);

	ch->pcdata->reina = 10;
	send_to_char("You clutch your head in agony as your eardrums seem to explode!\n\r",victim);
	act("$n clutches $s head in agony!",victim,NULL,NULL,TO_ROOM);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Scream on you.",ch,NULL,victim,TO_VICT);
	}
	victim->hit -= damage;
	if ((IS_NPC(victim) && victim->hit < 1) || 
	    (!IS_NPC(victim) && victim->hit < -10))
	{
	    act("$n's body falls limply to the ground.",victim,NULL,NULL,TO_ROOM);
	    demon_gain(ch,victim);
	    group_gain(ch,victim);
	    victim->form = 11;
	    killperson(ch,victim);
	    WAIT_STATE( ch, 12 );
	    return;
	}
	update_pos(victim);
	if (number_range(1,20) <= get_disc(ch,DISC_MELPOMINEE))
	{
	    victim->position = POS_STUNNED;
	    send_to_char("You fall to the ground, stunned!\n\r",victim);
	    act("$n falls to the ground, stunned!",victim,NULL,NULL,TO_ROOM);
	}
	else if (IS_NPC(victim)) do_kill(victim,ch->name);
	WAIT_STATE( ch, 12 );
	return;
    }
    else
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    return;
}

void do_spiritform( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_PROTEAN) < 7)
    {
	send_to_char("You require level 7 Protean to transform into Spirit Form.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH))
    {
	if (!IS_POLYAFF(ch,POLY_SPIRIT))
	{
	    send_to_char( "You cannot change from this form.\n\r", ch );
	    return;
	}
	act( "Your body regains its substance.", ch, NULL, NULL, TO_CHAR );
	act( "$n's ethereal body regains its substance.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	REMOVE_BIT(ch->polyaff, POLY_SPIRIT);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->vampaff, VAM_FLYING);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	return;
    }
    act( "You body becomes translucent and insubstancial.", ch, NULL, NULL, TO_CHAR );
    act( "$n's body becomes translucent and insubstancial.", ch, NULL, NULL, TO_ROOM );
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
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->affected_by, AFF_ETHEREAL);
    SET_BIT(ch->polyaff, POLY_SPIRIT);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->vampaff, VAM_FLYING);
    free_string( ch->morph );
    ch->morph = str_dup( ch->name );
    return;
}

void do_spiritguardian( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	if (IS_WEREWOLF(ch))
	{
	    if (get_auspice(ch, AUSPICE_THEURGE) < 4)
	    {
		send_to_char("You require the level 4 Theurge power to summon your Guardian Spirit.\n\r",ch);
		return;
	    }
	}
	else
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }
    else if (get_disc(ch,DISC_NECROMANCY) < 4)
    {
	send_to_char("You require level 4 Necromancy to summon your Spirit Guardian.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_GUARDIAN) )
    {
	send_to_char("You dismiss your Spirit Guardian.\n\r",ch);
	guardian_message( ch );
	send_to_char("I shall return when you call me, Master.\n\r",ch);
	REMOVE_BIT(ch->more, MORE_GUARDIAN);
	return;
    }
    send_to_char("You call your Spirit Guardian.\n\r",ch);
    guardian_message( ch );
    send_to_char("I have arrived, Master.\n\r",ch);
    SET_BIT(ch->more, MORE_GUARDIAN);
    return;
}

void do_succulence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA af;
    CHAR_DATA *victim;
    int sn;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 6)
    {
	send_to_char("You require level 6 Animalism to use Succulence.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Succulence on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Not on yourself.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim))
    {
	send_to_char( "Not on players.\n\r", ch );
	return;
    }

    if (is_affected(victim, gsn_potency))
    {
	send_to_char("You are unable to make them any more succulent.\n\r",ch);
	return;
    }

    if ( ( sn = skill_lookup( "blood potency" ) ) < 0 ) 
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.duration  = get_disc(ch,DISC_ANIMALISM) * 10;
    af.location  = APPLY_BLOOD_POT;
    af.modifier  = victim->blood[BLOOD_POTENCY];
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_BLOOD_MAX;
    af.modifier  = victim->blood[BLOOD_POOL];
    affect_to_char( victim, &af );

    victim->blood[BLOOD_CURRENT] *= 2;

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
    send_to_char("You increase the quality of their blood.\n\r",ch);
    return;
}

void do_loyalty( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_DOMINATE) < 4)
    {
	send_to_char("You require level 4 Dominate to use Loyalty.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to make loyal?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You make yourself loyal to yourself!\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's\n\r", ch );
	return;
    }

    if (strlen(victim->pcdata->conding) < 2 || 
	str_cmp(victim->pcdata->conding,ch->name))
    {
	send_to_char( "They are not conditioned to you.\n\r", ch );
	return;
    }

    act("You stare deeply into $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n stares deeply into $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n stares deeply into your eyes.",ch,NULL,victim,TO_VICT);

    if (IS_MORE(victim,MORE_LOYAL) )
    {
	act("You remove $N's loyalty to you.", ch, NULL, victim, TO_CHAR);
	act("You no longer feel quite so loyal to $n.", ch, NULL, victim, TO_VICT);
	REMOVE_BIT(victim->more, MORE_LOYAL);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Loyalty on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }
    act("You make $N completely loyal to you.", ch, NULL, victim, TO_CHAR);
    act("You suddenly feel completely loyal to $n.", ch, NULL, victim, TO_VICT);
    SET_BIT(victim->more, MORE_LOYAL);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Loyalty on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_love( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_MELPOMINEE) < 4)
    {
	send_to_char("You require level 4 Melpominee to sing the Song of Love.\n\r",ch);
	return;
    }

    if (ch->pcdata->reina > 0)
    {
	if (ch->pcdata->reina > 1)
	    sprintf(buf,"Your voice will not recover for another %d seconds.\n\r",ch->pcdata->reina);
	else
	    sprintf(buf,"Your voice will not recover for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use the Song of Love on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You're already in love with yourself.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    send_to_char("You sing the Song of Love.\n\r", ch);
    act("$n sings a beautiful, haunting song.", ch, NULL, NULL, TO_ROOM );
    ch->pcdata->reina = 60;

    if (strlen(victim->pcdata->love) > 2)
    {
	if (!str_cmp(victim->pcdata->love,ch->name))
	{
	    act("You remove $N's love for you!", ch, NULL, victim, TO_CHAR);
	    act("You are no longer in love with $n!", ch, NULL, victim, TO_VICT);
	    free_string(victim->pcdata->love);
	    victim->pcdata->love = str_dup( "" );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Love on you.",ch,NULL,victim,TO_VICT);
	    }
	}
	else
	{
	    send_to_char( "They are already in love with someone else.\n\r", ch );
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to use Love on you.",ch,NULL,victim,TO_VICT);
	    }
	}
	return;
    }

    if (!IS_NPC(victim) && (IS_VAMPIRE(victim) || IS_GHOUL(victim)) &&
	get_disc(victim,DISC_PRESENCE) >= get_disc(ch,DISC_PRESENCE) )
    {
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Love on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0)
    {
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Love on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Love.\n\r",victim);
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Love on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("You make $N fall in love with you!", ch, NULL, victim, TO_CHAR);
    act("You fall in love with $n!", ch, NULL, victim, TO_VICT);
    act("$N gazes lovingly into $n's eyes!", ch, NULL, victim, TO_NOTVICT);

    free_string(victim->pcdata->love);
    victim->pcdata->love = str_dup( ch->name );

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Love on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_frenzy( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_ANIMALISM) < 7)
    {
	send_to_char("You require level 7 Animalism to use Frenzy Trigger.\n\r",ch);
	return;
    }

    if ( ch->pcdata->wolf > 0 )
    {
	send_to_char("You take a deep breath and force back your inner beast.\n\r",ch);
	act("$n takes a deep breath and forces back $s inner beast.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->wolf = 0;
	if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT) && 
	    get_disc(ch,DISC_PROTEAN) > 0) 
	    do_nightsight(ch,"");
	if (IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	if (IS_VAMPAFF(ch, VAM_CLAWS) && 
	    get_disc(ch,DISC_PROTEAN) > 1) 
	    do_claws(ch,"");
	WAIT_STATE(ch,12);
	return;
    }

    if (!IS_VAMPAFF(ch, VAM_NIGHTSIGHT) && 
	get_disc(ch,DISC_PROTEAN) > 0) 
	do_nightsight(ch,"");
    if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
    if (!IS_VAMPAFF(ch, VAM_CLAWS) && 
	get_disc(ch,DISC_PROTEAN) > 1) 
	do_claws(ch,"");
    send_to_char("You bare yours fangs and growl as your inner beast consumes you.\n\r",ch);
    act("$n bares $s fangs and growls as $s inner beast consumes $m.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->wolf += number_range(10,20);
    WAIT_STATE(ch,12);
    return;
}

/* A coterie is a group of vampire players who work together. */
void do_coterie( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *gch;
    char      arg [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    char      *strtime;
    int       discipline, disc, level, value;

    /* Remove the first 'word' from the entered string, and copy it into 
     * arg.
     */
    argument = one_argument( argument, arg );

    /* Removes the tildes from argument for file-saving purposes. */
    smash_tilde( argument );

    /* This command cannot be used by Non-Player Characters (mobiles). */
    if (IS_NPC(ch)) return;

    /* Only vampires should be able to form coteries. */
    if (!IS_VAMPIRE(ch)) 
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    /* If no parameters or the 'info' parameter are added, then the player
     * should see a list of all the members of their coterie if they have 
     * one, or a list of coterie options if they don't.
     */
    if ( arg[0] == '\0' || !str_cmp(arg, "info") )
    {
	/* If they person belongs to a coterie, then this should list all 
         * the coterie members.
         */
	if (strlen(ch->pcdata->coterie) > 1 && strlen(ch->pcdata->leader) > 1 &&
	    strlen(ch->pcdata->cot_time) > 1)
	{
	    /* Show the name of the coterie, and who founded it. */
	    sprintf( buf, "The %s coterie, lead by %s:\n\r", 
		ch->pcdata->coterie, ch->pcdata->leader );
	    send_to_char( buf, ch );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    /* Display the following information about the coterie members:
             * Name:  The name of the member.
             * Rank:  Their position within the coterie - Leader, champion, etc.
             * Clan:  What vampire clan they belong to.
             * Gen:   What generation of vampire they are.
             * Hits:  What condition of physical damage the vampire is in.
             * Exp:   How many experience points the vampire has.
             * Status:How many players the vampire has killed.
             */
	    send_to_char("[      Name      ] [  Rank  ] [    Clan    ] [ Gen ] [ Hits ] [ Exp ] [ Status ]\n\r", ch );

	    /* Check each player in the char_list for a match. */
	    for ( gch = char_list; gch != NULL; gch = gch->next )
	    {
		if ( IS_NPC(gch) ) continue;
		if ( !IS_VAMPIRE(gch) ) continue;
		if ( strlen(gch->pcdata->coterie) < 2) continue;
		if ( strlen(gch->pcdata->cot_time) < 2) continue;
		if ( strlen(gch->pcdata->leader) < 2) continue;
		if ( str_cmp(ch->pcdata->leader,gch->pcdata->leader) ) continue;
		if ( str_cmp(ch->pcdata->cot_time,gch->pcdata->cot_time) ) continue;
		if ( !str_cmp(ch->pcdata->coterie,gch->pcdata->coterie) )
		{
		    /* Players with no clan are called Caitiff.  The Serpents
                     * of the Light and Daughters of Cacophony have too long 
                     * names to display on the screen, so I abbreviate them.
                     */
		    if (strlen(gch->clan) < 2) 
			strcpy(arg,"Caitiff");
		    else if (!str_cmp(gch->clan,"Serpents of the Light"))
			strcpy(arg,"Serpents");
		    else if (!str_cmp(gch->clan,"Daughters of Cacophony"))
			strcpy(arg,"Daughters");
		    else
			strcpy(arg,gch->clan);
		    sprintf( buf,
		    "[%-16s] [%8s] [%-12s] [  %-2d ] [%-6d] [%-5d] [   %-3d  ]\n\r",
			capitalize( gch->name ),
			IS_MORE(gch, MORE_LEADER) ? " Leader " : 
			IS_MORE(gch, MORE_CHAMPION) ? "Champion" : " Member ",
			arg,
			gch->vampgen, gch->hit, gch->exp, gch->race );
			send_to_char( buf, ch );
		}
	    }
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    send_to_char( "For more information type 'coterie help'.\n\r", ch );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    return;
	}
	/* If the player doesn't belong to a coterie, then display the 
	 * information about how they can create one.
	 */
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                              -= Coterie Options =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("CHAMPION : Make a member into a champion.\n\r",ch);
	send_to_char("FOUND    : Founds a new Coterie.\n\r",ch);
	send_to_char("INDUCT   : Inducts a member into your coterie.\n\r",ch);
	send_to_char("INFO     : Lists information on your coterie.\n\r",ch);
	send_to_char("LEAVE    : Leaves your current Coterie.\n\r",ch);
	send_to_char("OUTCAST  : Outcasts a member from your coterie.\n\r",ch);
	send_to_char("READY    : Coterie rules are completed.\n\r",ch);
	send_to_char("RULES    : Lists the coterie rules.\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }

    /* This allows a member to leave the coterie they currently belong to. */
    if ( !str_cmp(arg, "leave") )
    {
	/* Check if the person is actually a coterie member. */
	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}

	/* Check each player in the char_list for a match. */
	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( IS_NPC(gch) ) continue;
	    if ( !IS_VAMPIRE(gch) ) continue;
	    if ( strlen(gch->pcdata->coterie) < 2) continue;
	    if ( strlen(gch->pcdata->leader) < 2) continue;
	    if ( strlen(gch->pcdata->cot_time) < 2) continue;
	    if ( ch == gch ) continue;
	    if ( str_cmp(ch->pcdata->leader,gch->pcdata->leader) ) continue;
	    if ( str_cmp(ch->pcdata->cot_time,gch->pcdata->cot_time) ) continue;
	    if ( !str_cmp(ch->pcdata->coterie,gch->pcdata->coterie) )
	    {
		/* If the leader leaves their own coterie, then it will 
		 * be disbanded, and all other online members kicked out.
		 */
		if (IS_MORE(ch, MORE_LEADER))
		{
		    act( "Info -> $n has disbanded the coterie.", ch, NULL, gch, TO_VICT );
		    free_string(gch->pcdata->coterie);
		    gch->pcdata->coterie = str_dup( "" );
		    free_string(gch->pcdata->leader);
		    gch->pcdata->leader = str_dup( "" );
		}
		else
		    act( "Info -> $n has left the coterie.", ch, NULL, gch, TO_VICT );
	    }
	}
	if (IS_MORE(ch, MORE_LEADER))
	    send_to_char("You disband the coterie.\n\r",ch);
	else
	    send_to_char("You leave the coterie.\n\r",ch);
	REMOVE_BIT(ch->more, MORE_CHAMPION);
	free_string(ch->pcdata->coterie);
	ch->pcdata->coterie = str_dup( "" );
	free_string(ch->pcdata->leader);
	ch->pcdata->leader = str_dup( "" );
	REMOVE_BIT(ch->more, MORE_LEADER);
	return;
    }

    /* This allows someone to found a new coterie. */
    if ( !str_cmp(arg, "found") )
    {
	/* You cannot already belong to a coterie. */
	if (strlen(ch->pcdata->coterie) > 1)
	{
	    send_to_char( "But you are already the member of a coterie!\n\r", ch );
	    return;
	}
	if (argument[0] == '\0')
	{
	    send_to_char( "What do you wish to call your coterie?\n\r", ch );
	    return;
	}
	if (strlen(argument) < 5 || strlen(argument) > 20 )
	{
	    send_to_char( "Your coterie name should be between 5 and 20 characters long.\n\r", ch );
	    return;
	}
	/* Calculate and store the time the coterie was created.  Two 
	 * coteries of the same name created by the same person at different 
	 * times are considered different coteries to prevent a single coterie 
	 * having different rules for different members.
	 */
	strtime = ctime( &current_time );
	strtime[strlen(strtime)-1] = '\0';
	free_string(ch->pcdata->cot_time);
	ch->pcdata->cot_time = str_dup( strtime );

	send_to_char("You have founded a new coterie!\n\r",ch);
	free_string(ch->pcdata->coterie);
	ch->pcdata->coterie = str_dup( argument );
	free_string(ch->pcdata->leader);
	ch->pcdata->leader = str_dup( ch->name );
	SET_BIT(ch->more, MORE_LEADER);
	REMOVE_BIT(ch->more, MORE_CHAMPION);
	free_string(ch->pcdata->cot_sides);
	if (strlen(ch->side) < 2)
	    ch->pcdata->cot_sides = str_dup( "None" );
	else
	    ch->pcdata->cot_sides = str_dup( ch->side );
	free_string(ch->pcdata->cot_clans);
	if (strlen(ch->clan) < 2)
	    ch->pcdata->cot_clans = str_dup( "Caitiff" );
	else
	    ch->pcdata->cot_clans = str_dup( ch->clan );

	/* Wipe any discipline limitations/requirements from any previous 
	 * coteries the player might have belonged to.
	 */
	for (value = COT_BANNED; value <= COT_MAX; value++)
	{ch->pcdata->cot_min[value] = 0;}

	ch->pcdata->cot_min[COT_GENERATION] = 13;
	return;
    }

    if ( !str_cmp(arg, "induct") )
    {
	/* You cannot induct someone unless you actually belong to a 
	 * coterie.
	 */
	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->cot_clans) < 2 || 
	    strlen(ch->pcdata->cot_sides) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION))
	{
	    send_to_char( "Only a coterie leader or champion can induct new members.\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_CHAMPION) && IS_MORE(ch, MORE_LEADER))
	{
	    send_to_char( "Your coterie has not yet been completely set up.\n\r", ch );
	    return;
	}
	if (argument[0] == '\0')
	{
	    send_to_char( "Who do you wish to induct?\n\r", ch );
	    return;
	}
	if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	/* Only player vampires are allowed in the coterie. */
	if (IS_NPC(victim))
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if (!IS_VAMPIRE(victim))
	{
	    send_to_char( "You can only induct vampires.\n\r", ch );
	    return;
	}

	/* If not all clans are allowed in the coterie, then we better 
	 * check that the clan of the recruit is permitted.
	 */
	if (!is_name("All",ch->pcdata->cot_clans))
	{
	    /* Caitiff are not stored as a string value, but rather as a 
	     * lack of one.
	     * Check appropriate clan is permitted.
	     */
	    if (strlen(victim->clan) < 2 && 
		!is_name("Caitiff",ch->pcdata->cot_clans))
	    {
		send_to_char( "You are not allowed Caitiff in your coterie.\n\r", ch );
		return;
	    }
	    else if (!str_cmp(victim->clan,"Serpents of the Light") &&
		!is_name("serpents", ch->pcdata->cot_clans))
	    {
		send_to_char( "You are not allowed that clan in your coterie.\n\r", ch );
		return;
	    }
	    else if (!str_cmp(victim->clan,"Daughters of Cacophony") &&
		!is_name("daughters", ch->pcdata->cot_clans))
	    {
		send_to_char( "You are not allowed that clan in your coterie.\n\r", ch );
		return;
	    }
	    else if (!is_name(victim->clan,ch->pcdata->cot_clans))
	    {
		send_to_char( "You are not allowed that clan in your coterie.\n\r", ch );
		return;
	    }
	}
	/* If not all sects are allowed in the coterie, then we better 
	 * check that the sect of the recruit is permitted.
	 */
	if (!is_name("All",ch->pcdata->cot_sides))
	{
	    if (strlen(victim->side) < 2 && 
		!is_name("None",ch->pcdata->cot_sides))
	    {
		send_to_char( "You are not allowed Kindred of no sect in your coterie.\n\r", ch );
		return;
	    }
	    else if (!is_name(victim->side,ch->pcdata->cot_sides))
	    {
		send_to_char( "You are not allowed Kindred of that sect in your coterie.\n\r", ch );
		return;
	    }
	}

	/* You cannot induct someone who is already in the coterie! */
	if ( strlen(victim->pcdata->coterie) > 1 && 
	     strlen(victim->pcdata->leader) > 1 &&
	     strlen(victim->pcdata->cot_time) > 1 &&
	    !str_cmp(ch->pcdata->coterie,victim->pcdata->coterie) &&
	    !str_cmp(ch->pcdata->cot_time,victim->pcdata->cot_time) &&
	    !str_cmp(ch->pcdata->leader, victim->pcdata->leader) )
	{
	    send_to_char( "They are already a member of your coterie.\n\r", ch );
	    return;
	}

	/* People already in other coteries have to leave before joining 
	 * another one.
	 */
	if (strlen(victim->pcdata->coterie) > 1)
	{
	    send_to_char( "You cannot induct an unwilling person.\n\r", ch );
	    return;
	}
	/* Unwilling people cannot be forced to join. */
	if (!IS_IMMUNE(victim,IMM_VAMPIRE))
	{
	    send_to_char( "You cannot induct an unwilling person.\n\r", ch );
	    return;
	}

	/* Check they have the required disciplines (powers).
	 * Go through each required level of power (banned, basic, 
	 * intermediate, advanced, and expert) and for each one, check 
	 * for each discipline.
	 */
	for (level = COT_BANNED; level <= COT_EXPERT; level++)
	{
	    for (value = DISC_ANIMALISM; value <= DISC_VICISSITUDE; value++)
	    {
		/* The FOR loop requires DISC_ values, which are in the 
		 * range 0 to 19.  However the disciplines are stored in the 
		 * players coterie information as bits, so we have to work out 
		 * which discipline number relates to which bit.
		 */
		switch (value)
		{
		    default: disc = POW_NONE; break;
		    case DISC_ANIMALISM: disc = POW_ANIMALISM; break;
		    case DISC_AUSPEX: disc = POW_AUSPEX; break;
		    case DISC_CELERITY: disc = POW_CELERITY; break;
		    case DISC_CHIMERSTRY: disc = POW_CHIMERSTRY; break;
		    case DISC_DAIMOINON: disc = POW_DAIMOINON; break;
		    case DISC_DOMINATE: disc = POW_DOMINATE; break;
		    case DISC_FORTITUDE: disc = POW_FORTITUDE; break;
		    case DISC_MELPOMINEE: disc = POW_MELPOMINEE; break;
		    case DISC_NECROMANCY: disc = POW_NECROMANCY; break;
		    case DISC_OBEAH: disc = POW_OBEAH; break;
		    case DISC_OBFUSCATE: disc = POW_OBFUSCATE; break;
		    case DISC_OBTENEBRATION: disc = POW_OBTENEBRATION; break;
		    case DISC_POTENCE: disc = POW_POTENCE; break;
		    case DISC_PRESENCE: disc = POW_PRESENCE; break;
		    case DISC_PROTEAN: disc = POW_PROTEAN; break;
		    case DISC_QUIETUS: disc = POW_QUIETUS; break;
		    case DISC_SERPENTIS: disc = POW_SERPENTIS; break;
		    case DISC_THANATOSIS: disc = POW_THANATOSIS; break;
		    case DISC_THAUMATURGY: disc = POW_THAUMATURGY; break;
		    case DISC_VICISSITUDE: disc = POW_VICISSITUDE; break;
		}
		/* Now we check if they have the required discipline at 
		 * the specified level, and display a message accordingly.
		 */
		switch (level)
		{
		    default: break;
		    case COT_BANNED:
			if (IS_SET(ch->pcdata->cot_min[level], disc) && 
			    victim->pcdata->powers[value] != 0)
			{
			    send_to_char( "They have a banned discipline.\n\r", ch );
			    return;
			}
			break;
		    case COT_BASIC:
			if (IS_SET(ch->pcdata->cot_min[level], disc) && 
			    victim->pcdata->powers[value] < 1)
			{
			    send_to_char( "They don't have the required disciplines.\n\r", ch );
			    return;
			}
			break;
		    case COT_INTERMEDIATE:
			if (IS_SET(ch->pcdata->cot_min[level], disc) && 
			    victim->pcdata->powers[value] < 3)
			{
			    send_to_char( "They don't have the required disciplines.\n\r", ch );
			    return;
			}
			break;
		    case COT_ADVANCED:
			if (IS_SET(ch->pcdata->cot_min[level], disc) && 
			    victim->pcdata->powers[value] < 5)
			{
			    send_to_char( "They don't have the required disciplines.\n\r", ch );
			    return;
			}
			break;
		    case COT_EXPERT:
			if (IS_SET(ch->pcdata->cot_min[level], disc) && 
			    victim->pcdata->powers[value] < 6)
			{
			    send_to_char( "They don't have the required disciplines.\n\r", ch );
			    return;
			}
			break;
		}
	    }
	}

	/* Some coteries have a minimum hit point entry requirement. */
	if (victim->max_hit < ch->pcdata->cot_min[COT_HP])
	{
	    send_to_char( "They have insufficient hit points to join your coterie.\n\r", ch );
	    return;
	}

	/* Some coteries have a maximum generation entry requirement. */
	if (victim->vampgen > ch->pcdata->cot_min[COT_GENERATION])
	{
	    send_to_char( "They are not low enough generation to join your coterie.\n\r", ch );
	    return;
	}

	/* Some coteries have a minimum weapon skill entry requirement. */
	if (highest_skill(victim) < ch->pcdata->cot_min[COT_SKILL])
	{
	    send_to_char( "They are not good enough with weapons to join your coterie.\n\r", ch );
	    return;
	}

	/* Some coteries have a minimum fighting stance entry requirement. */
	if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE) && 
	    highest_stance(victim, TRUE) < ch->pcdata->cot_min[COT_STANCE])
	{
	    send_to_char( "They are not good enough with advanced stances to join your coterie.\n\r", ch );
	    return;
	}

	/* Some coteries require an advanced stance entry requirement. */
	if (!IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE) && 
	    highest_stance(victim, FALSE) < ch->pcdata->cot_min[COT_STANCE])
	{
	    send_to_char( "They are not good enough with stances to join your coterie.\n\r", ch );
	    return;
	}

	/* Some coteries require Golconda before you may enter.
	 * Golconda is a form of inner peace which most vampires strive for.
	*/
	if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_GOLCONDA) && 
	    victim->beast > 0)
	{
	    send_to_char( "But they don't have Golconda!\n\r", ch );
	    return;
	}

	/* Some coteries don't allow diablerists. 
	 * A diablerist is a vampire who drinks the blood of another vampire.
	 */
	if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_DIABLERIE) && 
	    victim->pcdata->diableries[DIAB_TIME] > 0)
	{
	    send_to_char( "But they're a diablerist!\n\r", ch );
	    return;
	}

	act("You induct $N into your coterie.",ch,NULL,victim,TO_CHAR);
	act("$n inducts you into $s coterie.",ch,NULL,victim,TO_VICT);

	/* Check each player in the char_list for a match. */
	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( IS_NPC(gch) ) continue;
	    if ( !IS_VAMPIRE(gch) ) continue;
	    if ( strlen(gch->pcdata->coterie) < 2) continue;
	    if ( strlen(gch->pcdata->cot_time) < 2) continue;
	    if ( strlen(gch->pcdata->leader) < 2) continue;
	    if ( str_cmp(ch->pcdata->leader,gch->pcdata->leader) ) continue;
	    if ( str_cmp(ch->pcdata->cot_time,gch->pcdata->cot_time) ) continue;
	    if ( !str_cmp(ch->pcdata->coterie,gch->pcdata->coterie) )
		act( "Info -> $N has been inducted to the coterie!", gch, NULL, victim, TO_CHAR );
	}

	/* Clear all their coterie values and replace them with the 
	 * values of a coterie member.
	 */
	free_string(victim->pcdata->coterie);
	victim->pcdata->coterie = str_dup( ch->pcdata->coterie );
	free_string(victim->pcdata->cot_time);
	victim->pcdata->cot_time = str_dup( ch->pcdata->cot_time );
	free_string(victim->pcdata->leader);
	victim->pcdata->leader = str_dup( ch->pcdata->leader );
	free_string(victim->pcdata->cot_clans);
	victim->pcdata->cot_clans = str_dup( ch->pcdata->cot_clans );
	free_string(victim->pcdata->cot_sides);
	victim->pcdata->cot_sides = str_dup( ch->pcdata->cot_sides );
	REMOVE_BIT(victim->more, MORE_CHAMPION);
	REMOVE_BIT(victim->more, MORE_LEADER);

	/* The new member has the same discipline requirements as the 
	 * person who inducted them.
	 */
	for (value = COT_BANNED; value <= COT_MAX; value++)
	{victim->pcdata->cot_min[value] = ch->pcdata->cot_min[value];}
	return;
    }
    if ( !str_cmp(arg, "outcast") )
    {
	/* You cannot outcast someone unless you actually belong to their 
	 * coterie, and even then only a select few can do it.
	 */
	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION))
	{
	    send_to_char( "Only a coterie leader or champion can outcast members.\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_CHAMPION) && IS_MORE(ch, MORE_LEADER))
	{
	    send_to_char( "Your coterie has not yet been completely set up.\n\r", ch );
	    return;
	}

	/* Find the person to be outcast, and check that they are valid. */
	if (argument[0] == '\0')
	{
	    send_to_char( "Who do you wish to outcast?\n\r", ch );
	    return;
	}
	if ( ( victim = get_char_world( ch, argument ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
	if (IS_NPC(victim))
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if (!IS_VAMPIRE(victim))
	{
	    send_to_char( "You can only induct vampires.\n\r", ch );
	    return;
	}
	if (victim == ch)
	{
	    send_to_char( "You cannot outcast yourself.\n\r", ch );
	    return;
	}
	if (strlen(victim->pcdata->coterie) < 2 || 
	    strlen(victim->pcdata->cot_time) < 2 || 
	    strlen(victim->pcdata->leader) < 2)
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (str_cmp(ch->pcdata->leader,victim->pcdata->leader))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (str_cmp(ch->pcdata->coterie,victim->pcdata->coterie))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (str_cmp(ch->pcdata->cot_time,victim->pcdata->cot_time))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (IS_MORE(victim, MORE_LEADER) || IS_MORE(victim, MORE_CHAMPION))
	{
	    send_to_char( "You can only outcast regular members.\n\r", ch );
	    return;
	}

	act("You outcast $N from the coterie.",ch,NULL,victim,TO_CHAR);
	act("$n outcasts you from the coterie.",ch,NULL,victim,TO_VICT);

	/* Clear the person's coterie values, as they are no longer a 
	 * coterie member.
	 */
	free_string(victim->pcdata->coterie);
	victim->pcdata->coterie = str_dup( "" );
	free_string(victim->pcdata->cot_time);
	victim->pcdata->cot_time = str_dup( "None" );
	free_string(victim->pcdata->leader);
	victim->pcdata->leader = str_dup( "" );
	free_string(victim->pcdata->cot_clans);
	victim->pcdata->cot_clans = str_dup( "All" );
	free_string(victim->pcdata->cot_sides);
	victim->pcdata->cot_sides = str_dup( "All" );
	REMOVE_BIT(victim->more, MORE_CHAMPION);
	for (value = COT_BANNED; value <= COT_MAX; value++)
	{victim->pcdata->cot_min[value] = 0;}

	/* Inform remaining members that their coterie has lost a member. */
	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( IS_NPC(gch) ) continue;
	    if ( !IS_VAMPIRE(gch) ) continue;
	    if ( strlen(gch->pcdata->coterie) < 2) continue;
	    if ( strlen(gch->pcdata->cot_time) < 2) continue;
	    if ( strlen(gch->pcdata->leader) < 2) continue;
	    if ( str_cmp(ch->pcdata->leader,gch->pcdata->leader) ) continue;
	    if ( str_cmp(ch->pcdata->cot_time,gch->pcdata->cot_time) ) continue;
	    if ( !str_cmp(ch->pcdata->coterie,gch->pcdata->coterie) )
		act( "Info -> $N has been kicked out of the coterie!", gch, NULL, victim, TO_CHAR );
	}
	return;
    }

    /* This allows a coterie leader to make someone into a champion - a 
     * person who has the ability to recruit new members.  A sort of 
     * sub-leader.
     */
    if ( !str_cmp(arg, "champion") )
    {
	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->cot_clans) < 2 || 
	    strlen(ch->pcdata->cot_sides) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_LEADER))
	{
	    send_to_char( "Only a coterie leader can make someone a champion.\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_CHAMPION))
	{
	    send_to_char( "Your coterie has not yet been completely set up.\n\r", ch );
	    return;
	}

	/* Find the person to be made a champion, and check they are valid. */
	if (argument[0] == '\0')
	{
	    send_to_char( "Who do you wish to make into a champion?\n\r", ch );
	    return;
	}
	if ( ( victim = get_char_world( ch, argument ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
	if (IS_NPC(victim))
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if (!IS_VAMPIRE(victim))
	{
	    send_to_char( "Only on vampires.\n\r", ch );
	    return;
	}
	if (victim == ch)
	{
	    send_to_char( "You cannot make yourself a champion!\n\r", ch );
	    return;
	}

	/* You can only make members of your own coterie into a champion. */
	if (strlen(victim->pcdata->coterie) < 2 || 
	    strlen(victim->pcdata->cot_time) < 2 || 
	    strlen(victim->pcdata->leader) < 2)
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (str_cmp(ch->pcdata->leader,victim->pcdata->leader))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}
	if (str_cmp(ch->pcdata->coterie,victim->pcdata->coterie))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}

	if (str_cmp(ch->pcdata->cot_time,victim->pcdata->cot_time))
	{
	    send_to_char( "They are not a member of your coterie.\n\r", ch );
	    return;
	}

	/* Inform all members of the coterie that the person has been 
	 * made/unmade a champion.
	 */
	if (!IS_MORE(victim, MORE_CHAMPION))
	{
	    act("You make $N a coterie champion.",ch,NULL,victim,TO_CHAR);
	    act("$n makes you a coterie champion.",ch,NULL,victim,TO_VICT);
	    sprintf(buf,"Info -> $N has been made a champion!");
	    SET_BIT(victim->more, MORE_CHAMPION);
	}
	else
	{
	    act("You remove $N's coterie champion position.",ch,NULL,victim,TO_CHAR);
	    act("$n removes your coterie champion position.",ch,NULL,victim,TO_VICT);
	    sprintf(buf,"Info -> $N is no longer a coterie champion!");
	    REMOVE_BIT(victim->more, MORE_CHAMPION);
	}

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( IS_NPC(gch) ) continue;
	    if ( !IS_VAMPIRE(gch) ) continue;
	    if ( strlen(gch->pcdata->coterie) < 2) continue;
	    if ( strlen(gch->pcdata->cot_time) < 2) continue;
	    if ( strlen(gch->pcdata->leader) < 2) continue;
	    if ( str_cmp(ch->pcdata->leader,gch->pcdata->leader) ) continue;
	    if ( str_cmp(ch->pcdata->cot_time,gch->pcdata->cot_time) ) continue;
	    if ( !str_cmp(ch->pcdata->coterie,gch->pcdata->coterie) )
		act( buf, gch, NULL, victim, TO_CHAR );
	}
	return;
    }

    /* This is used when the coterie rules are complete and the coterie 
     * is ready to take on new members.
     */
    if ( !str_cmp(arg, "ready") )
    {
	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}
	if (!IS_MORE(ch, MORE_LEADER))
	{
	    send_to_char( "Only a coterie leader can say when the coterie is ready.\n\r", ch );
	    return;
	}
	if (IS_MORE(ch, MORE_CHAMPION))
	{
	    send_to_char( "But your coterie has already been completed!\n\r", ch );
	    return;
	}
	SET_BIT(ch->more, MORE_CHAMPION);
	send_to_char( "Your coterie is now ready to accept new members!\n\r", ch );
	return;
    }

    /* This allows the coterie leader to define the rules of the coterie. */
    if ( !str_cmp(arg, "rules") )
    {
	/* Cut off the next two 'words' in the original input string and 
	 * copy their contents into arg2 and arg3.
	 */
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if (strlen(ch->pcdata->coterie) < 2 || 
	    strlen(ch->pcdata->cot_time) < 2 || 
	    strlen(ch->pcdata->leader) < 2)
	{
	    send_to_char( "But you are not a member of any coterie!\n\r", ch );
	    return;
	}

	/* If the coterie has been set as READY, then you cannot change 
	 * the rules - This command will now only display existing rules.
	 */
	if (IS_MORE(ch, MORE_LEADER) && IS_MORE(ch, MORE_CHAMPION))
	{
	    show_disc(ch);
	    return;
	}

	/* If you are not a leader, or no parameters are supplied, then 
	 * just list the current rules.
	 */
	if (arg2[0] == '\0' || !IS_MORE(ch, MORE_LEADER))
	{
	    show_disc(ch);
	    return;
	}

	/* Specify which clans are permitted in the coterie. */
	if (!str_cmp(arg2,"clans"))
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify which clans are allowed in your coterie (clear to start over).\n\r",ch);
		return;
	    }
	    if (!str_cmp(arg3,"all"))
	    {
		send_to_char("Ok.\n\r",ch);
		free_string(ch->pcdata->cot_clans);
		ch->pcdata->cot_clans = str_dup( "All" );
		return;
	    }
	    /* This method is required as not all inputs are simple 
	     * capitalised.
	     */
	    else if (!str_cmp(arg3,"assamite") ) strcpy(arg3, "Assamite");
	    else if (!str_cmp(arg3,"assamites")) strcpy(arg3, "Assamite");
	    else if (!str_cmp(arg3,"baali")    ) strcpy(arg3, "Baali");
	    else if (!str_cmp(arg3,"brujah")   ) strcpy(arg3, "Brujah");
	    else if (!str_cmp(arg3,"caitiff")  ) strcpy(arg3, "Caitiff");
	    else if (!str_cmp(arg3,"daughters")) strcpy(arg3, "Daughters");
	    else if (!str_cmp(arg3,"gangrel")  ) strcpy(arg3, "Gangrel");
	    else if (!str_cmp(arg3,"giovanni") ) strcpy(arg3, "Giovanni");
	    else if (!str_cmp(arg3,"lasombra") ) strcpy(arg3, "Lasombra");
	    else if (!str_cmp(arg3,"malkavian")) strcpy(arg3, "Malkavian");
	    else if (!str_cmp(arg3,"nosferatu")) strcpy(arg3, "Nosferatu");
	    else if (!str_cmp(arg3,"panders")  ) strcpy(arg3, "Panders");
	    else if (!str_cmp(arg3,"ravnos")   ) strcpy(arg3, "Ravnos");
	    else if (!str_cmp(arg3,"samedi")   ) strcpy(arg3, "Samedi");
	    else if (!str_cmp(arg3,"serpents") ) strcpy(arg3, "Serpents");
	    else if (!str_cmp(arg3,"setite")   ) strcpy(arg3, "Setite");
	    else if (!str_cmp(arg3,"setites")  ) strcpy(arg3, "Setite");
	    else if (!str_cmp(arg3,"toreador") ) strcpy(arg3, "Toreador");
	    else if (!str_cmp(arg3,"tremere")  ) strcpy(arg3, "Tremere");
	    else if (!str_cmp(arg3,"tzimisce") ) strcpy(arg3, "Tzimisce");
	    else if (!str_cmp(arg3,"ventrue")  ) strcpy(arg3, "Ventrue");
	    else if (!str_cmp(arg3,"clear")  )
	    {
		free_string(ch->pcdata->cot_clans);
		if (strlen(ch->clan) < 2)
		    ch->pcdata->cot_clans = str_dup( "Caitiff" );
		else
		    ch->pcdata->cot_clans = str_dup( ch->clan );
		send_to_char("Ok.\n\r",ch);
		return;
	    }
	    else
	    {
		send_to_char( "That is not a clan or a bloodline.\n\r", ch );
		return;
	    }
	    if (is_name(arg3,ch->pcdata->cot_clans) || 
		is_name("all",ch->pcdata->cot_clans))
	    {
		send_to_char( "That clan is already allowed to join the coterie.\n\r", ch );
		return;
	    }
	    if (strlen(ch->pcdata->cot_clans) > MAX_INPUT_LENGTH)
	    {
		bug( "do_coterie: Too many clans.", 0 );
		return;
	    }
	    strcpy(buf,ch->pcdata->cot_clans);
	    strcat(buf," ");
	    strcat(buf,arg3);
	    free_string(ch->pcdata->cot_clans);
	    ch->pcdata->cot_clans = str_dup(buf);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify which sects are permitted in the coterie. */
	else if (!str_cmp(arg2,"sects"))
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify which sects are allowed in your coterie (clear to start over).\n\r",ch);
		return;
	    }
	    if (!str_cmp(arg3,"all"))
	    {
		send_to_char("Ok.\n\r",ch);
		free_string(ch->pcdata->cot_sides);
		ch->pcdata->cot_sides = str_dup( "All" );
		return;
	    }
	    else if (!str_cmp(arg3,"anarch")    ) strcpy(arg3, "Anarch");
	    else if (!str_cmp(arg3,"camarilla") ) strcpy(arg3, "Camarilla");
	    else if (!str_cmp(arg3,"sabbat")    ) strcpy(arg3, "Sabbat");
	    else if (!str_cmp(arg3,"clear")     )
	    {
		free_string(ch->pcdata->cot_sides);
		if (strlen(ch->side) < 2)
		    ch->pcdata->cot_sides = str_dup( "None" );
		else
		    ch->pcdata->cot_sides = str_dup( ch->side );
		send_to_char("Ok.\n\r",ch);
		return;
	    }
	    else
	    {
		send_to_char( "That is not a sect.\n\r", ch );
		return;
	    }
	    if (is_name(arg3,ch->pcdata->cot_sides) || 
		is_name("all",ch->pcdata->cot_sides))
	    {
		send_to_char( "That sect is already allowed to join the coterie.\n\r", ch );
		return;
	    }
	    if (strlen(ch->pcdata->cot_sides) > MAX_INPUT_LENGTH)
	    {
		bug( "do_coterie: Too many sects.", 0 );
		return;
	    }
	    strcpy(buf,ch->pcdata->cot_sides);
	    strcat(buf," ");
	    strcat(buf,arg3);
	    free_string(ch->pcdata->cot_sides);
	    ch->pcdata->cot_sides = str_dup(buf);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify how many hit points new members must have. */
	else if (!str_cmp(arg2,"hp"))
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify the minimum number of hit points required.\n\r",ch);
		return;
	    }

	    /* If arg3 isn't a number, then it shouldn't be valid. */
	    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	    if (value < 0)
	    {
		send_to_char("Thats not a number!\n\r",ch);
		return;
	    }
	    else if (value > 10000)
	    {
		send_to_char("Please enter a number between 0 and 10000.\n\r",ch);
		return;
	    }

	    /* You must be able to fulfill your own requirements. */
	    if (ch->hit < value)
	    {
		sprintf(buf, "But you're only generation %d!\n\r",ch->vampgen);
		send_to_char(buf,ch);
		return;
	    }
	    ch->pcdata->cot_min[COT_HP] = value;
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify what generation new members should be. */
	else if (!str_cmp(arg2,"gen") || !str_cmp(arg2,"generation"))
	{
	    int min_gen = 13;

	    if (ch->pcdata->cot_min[COT_EXPERT] > 0) min_gen = 7;
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify the highest generation permitted.\n\r",ch);
		return;
	    }

	    /* If arg3 isn't a number, then it shouldn't be valid. */
	    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	    if (value < 0)
	    {
		send_to_char("Thats not a number!\n\r",ch);
		return;
	    }
	    else if (value < ch->vampgen || value > min_gen)
	    {
		if (ch->vampgen == min_gen)
		    sprintf(buf, "Your coterie may only contain vampires who are generation %d.\n\r",ch->vampgen);
		else
		    sprintf(buf, "Please enter a number between %d and %d.\n\r",ch->vampgen,min_gen);
		send_to_char(buf,ch);
		return;
	    }

	    /* You must be able to fulfill your own requirements. */
	    if (ch->vampgen > value)
	    {
		sprintf(buf, "But you're only generation %d!\n\r",ch->vampgen);
		send_to_char(buf,ch);
		return;
	    }
	    ch->pcdata->cot_min[COT_GENERATION] = value;
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify what weapon skill new members should have. */
	else if (!str_cmp(arg2,"skill"))
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify the minimum amount of weapon skill required.\n\r",ch);
		return;
	    }

	    /* If arg3 isn't a number, then it shouldn't be valid. */
	    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	    if (value < 0)
	    {
		send_to_char("Thats not a number!\n\r",ch);
		return;
	    }
	    else if (value > 200)
	    {
		send_to_char("Please enter a number between 0 and 200.\n\r",ch);
		return;
	    }

	    /* highest_skill is an int function which works out which of your 
	     * weapon skills is the highest.
	     */
	    if (highest_skill(ch) < value)
	    {
		send_to_char("But you're not even that skilled yourself!\n\r",ch);
		return;
	    }
	    ch->pcdata->cot_min[COT_SKILL] = value;
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify what fighting stance ability new members should have. */
	else if (!str_cmp(arg2,"stance"))
	{
	    bool advanced = FALSE;

	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify the minimum amount of stance ability required.\n\r",ch);
		return;
	    }

	    /* If arg3 isn't a number, then it shouldn't be valid. */
	    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

	    if (value < 0)
	    {
		send_to_char("Thats not a number!\n\r",ch);
		return;
	    }
	    else if (value > 200)
	    {
		send_to_char("Please enter a number between 0 and 200.\n\r",ch);
		return;
	    }
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE))
		advanced = TRUE;

	    /* highest_stance is an int function which works out which of your 
	     * fighting stances is the highest.  If the requirement is an 
	     * advanced stance, then the second function parameter (advanced) 
	     * will be TRUE, and the function will only check advanced fighting 
	     * stances.
	     */
	    if (highest_stance(ch, advanced) < value)
	    {
		send_to_char("But you're not even that skilled yourself!\n\r",ch);
		return;
	    }
	    ch->pcdata->cot_min[COT_STANCE] = value;
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify discipline requirements of new members. */
	else if (!str_cmp(arg2,"banned") || 
	    !str_cmp(arg2,"basic") || !str_cmp(arg2,"intermediate") || 
	    !str_cmp(arg2,"advanced") || !str_cmp(arg2,"expert"))
	{
	    if      (!str_cmp(arg2, "banned"))       level = COT_BANNED;
	    else if (!str_cmp(arg2, "basic"))        level = COT_BASIC;
	    else if (!str_cmp(arg2, "intermediate")) level = COT_INTERMEDIATE;
	    else if (!str_cmp(arg2, "advanced"))     level = COT_ADVANCED;
	    else                                     level = COT_EXPERT;
	    if (arg3[0] == '\0')
	    {
		if (level == COT_BANNED)
		    sprintf(buf, "Please specify which disciplines are banned.\n\r");
		else
		    sprintf(buf, "Please specify which disciplines are required at %s level.\n\r", capitalize(arg2));
		send_to_char(buf,ch);
		return;
	    }

	    /* From the input, calculate the values of:
	     * discipline: A bit used for the coterie requirements values.
	     * disc: An int, range 0-19, used for the players discipline array.
	     */
	    if      (!str_cmp(arg3,"animalism") ) 
		{discipline = POW_ANIMALISM; disc = DISC_ANIMALISM;}
	    else if (!str_cmp(arg3,"auspex")) 
		{discipline = POW_AUSPEX; disc = DISC_AUSPEX;}
	    else if (!str_cmp(arg3,"celerity")) 
		{discipline = POW_CELERITY; disc = DISC_CELERITY;}
	    else if (!str_cmp(arg3,"chimerstry")) 
		{discipline = POW_CHIMERSTRY; disc = DISC_CHIMERSTRY;}
	    else if (!str_cmp(arg3,"daimoinon")) 
		{discipline = POW_DAIMOINON; disc = DISC_DAIMOINON;}
	    else if (!str_cmp(arg3,"dominate")) 
		{discipline = POW_DOMINATE; disc = DISC_DOMINATE;}
	    else if (!str_cmp(arg3,"fortitude")) 
		{discipline = POW_FORTITUDE; disc = DISC_FORTITUDE;}
	    else if (!str_cmp(arg3,"melpominee")) 
		{discipline = POW_MELPOMINEE; disc = DISC_MELPOMINEE;}
	    else if (!str_cmp(arg3,"necromancy")) 
		{discipline = POW_NECROMANCY; disc = DISC_NECROMANCY;}
	    else if (!str_cmp(arg3,"obeah")) 
		{discipline = POW_OBEAH; disc = DISC_OBEAH;}
	    else if (!str_cmp(arg3,"obfuscate")) 
		{discipline = POW_OBFUSCATE; disc = DISC_OBFUSCATE;}
	    else if (!str_cmp(arg3,"obtenebration")) 
		{discipline = POW_OBTENEBRATION; disc = DISC_OBTENEBRATION;}
	    else if (!str_cmp(arg3,"potence")) 
		{discipline = POW_POTENCE; disc = DISC_POTENCE;}
	    else if (!str_cmp(arg3,"presence")) 
		{discipline = POW_PRESENCE; disc = DISC_PRESENCE;}
	    else if (!str_cmp(arg3,"protean")) 
		{discipline = POW_PROTEAN; disc = DISC_PROTEAN;}
	    else if (!str_cmp(arg3,"quietus")) 
		{discipline = POW_QUIETUS; disc = DISC_QUIETUS;}
	    else if (!str_cmp(arg3,"serpentis")) 
		{discipline = POW_SERPENTIS; disc = DISC_SERPENTIS;}
	    else if (!str_cmp(arg3,"thanatosis")) 
		{discipline = POW_THANATOSIS; disc = DISC_THANATOSIS;}
	    else if (!str_cmp(arg3,"thaumaturgy")) 
		{discipline = POW_THAUMATURGY; disc = DISC_THAUMATURGY;}
	    else if (!str_cmp(arg3,"vicissitude")) 
		{discipline = POW_VICISSITUDE; disc = DISC_VICISSITUDE;}
	    else if (!str_cmp(arg3,"clear")  )
	    {
		ch->pcdata->cot_min[level] = POW_NONE;
		send_to_char("Ok.\n\r",ch);
		return;
	    }
	    else
	    {
		send_to_char( "There is no such discipline.\n\r", ch );
		return;
	    }

	    if (level == COT_EXPERT && disc == DISC_THAUMATURGY)
	    {
		send_to_char( "There is no expert Thaumaturgy!\n\r", ch );
		return;
	    }

	    /* You cannot ban a discipline from the coterie if you 
	     * already know it!
	     */
	    if (level == COT_BANNED && ch->pcdata->powers[disc] != 0)
	    {
		sprintf(buf,"But you already know the %s discipline!\n\r",
		    capitalize(arg3));
		send_to_char( buf, ch );
		return;
	    }
	    /* You cannot set discipline requirements that you don't have 
	     * yourself.
	     */
	    if ((level == COT_BASIC && ch->pcdata->powers[disc] < 1) ||
		(level == COT_INTERMEDIATE && ch->pcdata->powers[disc] < 3) ||
		(level == COT_ADVANCED && ch->pcdata->powers[disc] < 5) ||
		(level == COT_EXPERT && ch->pcdata->powers[disc] < 6))
	    {
		arg2[0] = UPPER(arg2[0]); arg3[0] = UPPER(arg3[0]);
		sprintf(buf,"But you don't have %s level %s!\n\r", arg2, arg3);
		send_to_char( buf, ch );
		return;
	    }
/*
	    if (IS_SET(ch->pcdata->cot_min[COT_BANNED], discipline))
	    {
		REMOVE_BIT(ch->pcdata->cot_min[COT_BANNED], discipline);
		send_to_char( "That discipline is no longer banned.\n\r", ch );
		return;
	    }
	    if (IS_SET(ch->pcdata->cot_min[COT_BASIC], discipline) ||
		IS_SET(ch->pcdata->cot_min[COT_INTERMEDIATE], discipline) ||
		IS_SET(ch->pcdata->cot_min[COT_ADVANCED], discipline) ||
		IS_SET(ch->pcdata->cot_min[COT_EXPERT], discipline))
	    {
		send_to_char( "That discipline is already a coterie requirement.\n\r", ch );
		return;
	    }
*/
	    if (IS_SET(ch->pcdata->cot_min[COT_BANNED], discipline))
		REMOVE_BIT(ch->pcdata->cot_min[COT_BANNED], discipline);
	    if (IS_SET(ch->pcdata->cot_min[COT_BASIC], discipline))
		REMOVE_BIT(ch->pcdata->cot_min[COT_BASIC], discipline);
	    if (IS_SET(ch->pcdata->cot_min[COT_INTERMEDIATE], discipline))
		REMOVE_BIT(ch->pcdata->cot_min[COT_INTERMEDIATE], discipline);
	    if (IS_SET(ch->pcdata->cot_min[COT_ADVANCED], discipline))
		REMOVE_BIT(ch->pcdata->cot_min[COT_ADVANCED], discipline);
	    if (IS_SET(ch->pcdata->cot_min[COT_EXPERT], discipline))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXPERT], discipline);
	    /* Make sure a discipline cannot be both banned and required. */
	    REMOVE_BIT(ch->pcdata->cot_min[COT_BANNED], discipline);
	    REMOVE_BIT(ch->pcdata->cot_min[COT_BASIC], discipline);
	    REMOVE_BIT(ch->pcdata->cot_min[COT_INTERMEDIATE], discipline);
	    REMOVE_BIT(ch->pcdata->cot_min[COT_ADVANCED], discipline);
	    REMOVE_BIT(ch->pcdata->cot_min[COT_EXPERT], discipline);
	    SET_BIT(ch->pcdata->cot_min[level], discipline);
	    if (level == COT_EXPERT && ch->pcdata->cot_min[COT_GENERATION] > 7 )
		ch->pcdata->cot_min[COT_GENERATION] = 7;
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Specify if new members should have an advanced fighting stance. */
	else if (!str_cmp(arg2,"adv_stance"))
	{
	    value = ch->pcdata->cot_min[COT_STANCE];

	    /* Check that you have one yourself! */
	    if (highest_stance(ch, TRUE) < 1)
	    {
		send_to_char("But you don't even have any advanced stances!\n\r",ch);
		return;
	    }
	    /* Make sure your advanced stance is good enough. */
	    if (highest_stance(ch, TRUE) < value)
	    {
		send_to_char("But you're not even that skilled yourself!\n\r",ch);
		return;
	    }
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE);
	    else
		SET_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Toggle if new members should have Golconda. */
	else if (!str_cmp(arg2,"golconda"))
	{
	    /* Golconda means ch->beast is 0.  You've got to have 
	     * Golconda yourself if you want other members to have it.
	     */
	    if (ch->beast > 0)
	    {
		send_to_char("But you don't even have Golconda yourself!\n\r",ch);
		return;
	    }
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_GOLCONDA))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_GOLCONDA);
	    else
		SET_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_GOLCONDA);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Toggle if new members must not have committed diablerie. */
	else if (!str_cmp(arg2,"no_diablerie"))
	{
	    if (ch->pcdata->diableries[DIAB_EVER] > 0)
	    {
		send_to_char("But you've already committed diablerie!\n\r",ch);
		return;
	    }
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_DIABLERIE))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_DIABLERIE);
	    else
		SET_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_DIABLERIE);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Toggle if new members must not kill other sect members. */
	else if (!str_cmp(arg2,"no_kill_sect"))
	{
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_SECT))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_SECT);
	    else
		SET_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_SECT);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* Toggle if new members must not kill other coterie members. */
	else if (!str_cmp(arg2,"no_kill_coterie"))
	{
	    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_COTERIE))
		REMOVE_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_COTERIE);
	    else
		SET_BIT(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_COTERIE);
	    send_to_char("Ok.\n\r",ch);
	    return;
	}
	/* If you didn't enter any of the above, then list all the 
	 * current rules as a default.
	 */
	else show_disc(ch);
	return;
    }

    /* The 'coterie' command on its own lists the coterie options, unless 
     * you already belong to a coterie in which case you won't have reached 
     * this point.
     */
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("                              -= Coterie Options =-\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("CHAMPION : Make a member into a champion.\n\r",ch);
    send_to_char("FOUND    : Founds a new Coterie.\n\r",ch);
    send_to_char("INDUCT   : Inducts a member into your coterie.\n\r",ch);
    send_to_char("INFO     : Lists information on your coterie.\n\r",ch);
    send_to_char("LEAVE    : Leaves your current Coterie.\n\r",ch);
    send_to_char("OUTCAST  : Outcasts a member from your coterie.\n\r",ch);
    send_to_char("READY    : Coterie rules are completed.\n\r",ch);
    send_to_char("RULES    : Lists the coterie rules.\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_ctalk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    
    if ( IS_NPC(ch) ) return;

    if ( !IS_VAMPIRE(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( strlen(ch->pcdata->coterie) < 2 || strlen(ch->pcdata->leader) < 2 )
    {
	send_to_char("But you don't belong to a coterie!\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char("Coterie-talk what?\n\r",ch);
	return;
    }

    if ( IS_MORE(ch, MORE_LEADER) )
	act("-=$n=- '$t'.", ch, argument, NULL, TO_CHAR);
    else
	act("-$n- '$t'.", ch, argument, NULL, TO_CHAR);

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected != CON_PLAYING ) continue;
	if ( (victim = d->character) == NULL ) continue;
	if ( IS_NPC(victim) ) continue;
	if ( !IS_VAMPIRE(victim) ) continue;
	if ( strlen(victim->pcdata->coterie) < 2 ) continue;
	if ( strlen(victim->pcdata->leader) < 2 ) continue;
	if ( str_cmp(ch->pcdata->coterie, victim->pcdata->coterie ) ) continue;
	if ( str_cmp(ch->pcdata->leader, victim->pcdata->leader ) ) continue;
	if ( IS_MORE(ch, MORE_LEADER) )
	    act("-=$n=- '$t'.", ch, argument, victim, TO_VICT);
	else
	    act("-$n- '$t'.", ch, argument, victim, TO_VICT);
    }
    return;
}

void do_shadowstep( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char( "Who do you wish to shadowstep to?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBTENEBRATION) < 5)
    {
	send_to_char("You require level 5 Obtenebration to use Shadow Step.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   ch->in_room == NULL
    ||   (IS_NPC(victim) && !room_is_dark(victim->in_room))
    ||   victim->level > (get_disc(ch,DISC_OBTENEBRATION) * 10))
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( victim->in_room == ch->in_room )
    {
	send_to_char( "But you are already there!\n\r", ch );
	return;
    }

    if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->position != POS_STANDING
    ||   (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON) ) )
    {
	send_to_char( "You are unable to focus on their location.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Summoning on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    send_to_char("You step into the shadows.\n\r",ch);
    act("$n steps into the shadows and vanishes from sight.",ch,NULL,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    do_look(ch,"");
    send_to_char("You step out of the shadows.\n\r",ch);
    act("$N steps from the shadows.",victim,NULL,ch,TO_CHAR);
    act("$N steps from the shadows.",victim,NULL,ch,TO_ROOM);
    return;
}

void do_hand( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch))
    {
	if (!IS_WEREWOLF(ch) || get_auspice(ch, AUSPICE_AHROUN) < 5)
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }

    if (IS_MORE(ch,MORE_HAND_FLAME) )
    {
	if (IS_ANGEL(ch))
	{
	    send_to_char("The glowing blue nimbus around your hands fades away.\n\r",ch);
	    act("The glowing blue nimbus around $n's hands fades away.", ch, NULL, NULL, TO_ROOM);
	}
	else
	{
	    send_to_char("The flames around your hands are extinguished.\n\r",ch);
	    act("The flames around $n's hands die away.", ch, NULL, NULL, TO_ROOM);
	}
	REMOVE_BIT(ch->more, MORE_HAND_FLAME);
	if ( ch->in_room != NULL && ch->in_room->light > 0 ) 
	    --ch->in_room->light;
	return;
    }
    if (IS_ANGEL(ch))
    {
	send_to_char("Your hands begin to glow with a blue nimbus.\n\r",ch);
	act("$n's hands begin to blue with a blue nimbus.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("Your hands erupts into flames.\n\r",ch);
	act("$n's hands erupts into flames.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->more, MORE_HAND_FLAME);
    if ( ch->in_room != NULL ) ++ch->in_room->light;
    return;
}

void do_flamebolt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam, hp, sn;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch) || IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who do you wish to shoot a Flame Bolt at?\n\r", ch );
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

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Engulf on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Now that would be just plain stupid.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "flame bolt" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->atm >= number_percent())
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    dam = number_range(500,1000);
    hp = victim->hit;
    if (!IS_NPC(victim) && IS_VAMPIRE(victim) )
    	damage( ch, victim, (dam*2), (sn + TYPE_ADD_AGG) );
    else
    	damage( ch, victim, dam, (sn + TYPE_ADD_AGG) );
    WAIT_STATE(ch, 6);
    if (victim == NULL || victim->position == POS_DEAD) return;
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_HEAT) )
	{victim->hit = hp;victim->agg--;}
    else if (!IS_NPC(victim) && IS_VAMPIRE(victim) && get_disc(victim,DISC_DAIMOINON) > 5)
	{victim->hit = hp;victim->agg--;}
    else if (!IS_NPC(victim) && IS_WEREWOLF(victim) && get_auspice(victim,AUSPICE_AHROUN) > 4)
	{victim->hit = hp;victim->agg--;}
    update_pos(victim);
    return;
}

void do_engulf( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam, hp, sn;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch) || IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who do you wish to Engulf?\n\r", ch );
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

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Engulf on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Now that would be just plain stupid.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "flame bolt" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->atm >= number_percent())
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    dam = number_range(500,1000);
    hp = victim->hit;


    if (!IS_NPC(victim) && IS_VAMPIRE(victim) )
    	damage( ch, victim, (dam*2), (sn + TYPE_ADD_AGG) );
    else
    	damage( ch, victim, dam, (sn + TYPE_ADD_AGG) );
    WAIT_STATE(ch, 6);
    if (victim == NULL || victim->position == POS_DEAD) return;
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_HEAT) )
	{victim->hit = hp;victim->agg--;}
    else if (!IS_NPC(victim) && IS_VAMPIRE(victim) && get_disc(victim,DISC_DAIMOINON) > 5)
	{victim->hit = hp;victim->agg--;}
    else if (!IS_NPC(victim) && IS_WEREWOLF(victim) && get_auspice(victim,AUSPICE_AHROUN) > 4)
	{victim->hit = hp;victim->agg--;}
    else if (!IS_AFFECTED(victim, AFF_FLAMING))
    {
	send_to_char("You are engulfed in flames!\n\r",victim);
	act("$n is engulfed in flames!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->affected_by, AFF_FLAMING);
    }
    update_pos(victim);
    return;
}

void do_evileye( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int value;
 
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
 
    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( arg1[0] != '\0' && !str_cmp( arg1, "on" ) )
    {
	if (IS_MORE(ch, MORE_EVILEYE))
	{
	    send_to_char("You already have Evil Eye on.\n\r",ch);
	    return;
	}
	send_to_char("Your Evil Eye is now ON.\n\r",ch);
	SET_BIT(ch->more, MORE_EVILEYE);
	return;
    }

    if ( arg1[0] != '\0' && !str_cmp( arg1, "off" ) )
    {
	if (!IS_MORE(ch, MORE_EVILEYE))
	{
	    send_to_char("You already have Evil Eye off.\n\r",ch);
	    return;
	}
	send_to_char("Your Evil Eye is now OFF.\n\r",ch);
	REMOVE_BIT(ch->more, MORE_EVILEYE);
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                 -= Evil Eye =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
        send_to_char( "Format is: evileye <option> <value>\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
        send_to_char( "Option ON switches your Evil Eye on.\n\r", ch );
        send_to_char( "Option OFF switches your Evil Eye off.\n\r", ch );
        send_to_char( "Option ACTION is a text string action performed by you or the viewer.\n\r", ch );
        send_to_char( "Option MESSAGE is a text string shown to the person looking at you.\n\r", ch );
        send_to_char( "Option TOGGLE has values: Spell, Self, Other.\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if ( strlen(ch->poweraction) < 2 )
	    sprintf(buf,"Current Action: None.\n\r");
	else
	    sprintf(buf,"Current Action: %s.\n\r",ch->poweraction);
	send_to_char(buf,ch);
	if ( strlen(ch->powertype) < 2 )
	    sprintf(buf,"Current Message: None.\n\r");
	else
	    sprintf(buf,"Current Message: %s.\n\r",ch->powertype);
	send_to_char(buf,ch);
	send_to_char("Current Toggle flags:",ch);
	if (IS_SET(ch->spectype,EYE_SPELL)) send_to_char(" Spell",ch);
	if (IS_SET(ch->spectype,EYE_SELFACTION)) send_to_char(" Self",ch);
	if (IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" Other",ch);
	if (!IS_SET(ch->spectype,EYE_SPELL) &&
	    !IS_SET(ch->spectype,EYE_SELFACTION) &&
	    !IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" None",ch);
	send_to_char(".\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }
    value = is_number( arg2 ) ? atoi( arg2 ) : -1;
    if ( !str_cmp( arg1, "action" ) )
    {
	if (is_in(arg2,"|quit*rem*drop*gift*claim*consent*propose*accept*vampi*ma *maj*sid *side *reca*/*l *lo *loo *look *exa*ha *hav *have *haven*"))
	{
	    send_to_char("Illegal action type.\n\r",ch);
	    return;
	}
	if (strlen(arg2) > 80)
	{
	    send_to_char("Action too long, please limit it to 80 characters.\n\r",ch);
	    return;
	}
	if (!str_cmp(arg2,"clear"))
	{
	    free_string( ch->poweraction );
	    ch->poweraction = str_dup( "" );
	    send_to_char("Action cleared.\n\r",ch);
	    return;
	}
	free_string( ch->poweraction );
	ch->poweraction = str_dup( arg2 );
	send_to_char("Action set.\n\r",ch);
	return;
    }
    else if ( !str_cmp( arg1, "message" ) )
    {
	if (strlen(arg2) > 80)
	{
	    send_to_char("Message too long, please limit it to 80 characters.\n\r",ch);
	    return;
	}
	if (!str_cmp(arg2,"clear"))
	{
	    free_string( ch->powertype );
	    ch->powertype = str_dup( "" );
	    send_to_char("Message cleared.\n\r",ch);
	    return;
	}
	free_string( ch->powertype );
	ch->powertype = str_dup( arg2 );
	send_to_char("Message set.\n\r",ch);
	return;
    }
    else if ( !str_cmp( arg1, "toggle" ) )
    {
	if ( !str_cmp( arg2, "spell" ) && IS_SET(ch->spectype,EYE_SPELL))
	    REMOVE_BIT(ch->spectype,EYE_SPELL);
	else if ( !str_cmp( arg2, "spell" ) && !IS_SET(ch->spectype,EYE_SPELL))
	    SET_BIT(ch->spectype,EYE_SPELL);
	else if ( !str_cmp( arg2, "self" ) && IS_SET(ch->spectype,EYE_SELFACTION))
	    REMOVE_BIT(ch->spectype,EYE_SELFACTION);
	else if ( !str_cmp( arg2, "self" ) && !IS_SET(ch->spectype,EYE_SELFACTION))
	    SET_BIT(ch->spectype,EYE_SELFACTION);
	else if ( !str_cmp( arg2, "other" ) && IS_SET(ch->spectype,EYE_ACTION))
	    REMOVE_BIT(ch->spectype,EYE_ACTION);
	else if ( !str_cmp( arg2, "other" ) && !IS_SET(ch->spectype,EYE_ACTION))
	    SET_BIT(ch->spectype,EYE_ACTION);
	else {
	    send_to_char("TOGGLE flag should be one of: spell, self, other.\n\r",ch);
	    return;}
	sprintf(buf,"%s flag toggled.\n\r",capitalize(arg2));
	send_to_char(buf,ch);
	return;
    }
    else
    {
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	send_to_char("                                 -= Evil Eye =-\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
        send_to_char( "Format is: evileye <option> <value>\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
        send_to_char( "Option ACTION is a text string action performed by you or the viewer.\n\r", ch );
        send_to_char( "Option MESSAGE is a text string shown to the person looking at you.\n\r", ch );
        send_to_char( "Option TOGGLE has values: Spell, Self, Other.\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	if ( strlen(ch->poweraction) < 2 )
	    sprintf(buf,"Current Action: None.\n\r");
	else
	    sprintf(buf,"Current Action: %s.\n\r",ch->poweraction);
	send_to_char(buf,ch);
	if ( strlen(ch->powertype) < 2 )
	    sprintf(buf,"Current Message: None.\n\r");
	else
	    sprintf(buf,"Current Message: %s.\n\r",ch->powertype);
	send_to_char(buf,ch);
	send_to_char("Current Toggle flags:",ch);
	if (IS_SET(ch->spectype,EYE_SPELL)) send_to_char(" Spell",ch);
	if (IS_SET(ch->spectype,EYE_SELFACTION)) send_to_char(" Self",ch);
	if (IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" Other",ch);
	if (!IS_SET(ch->spectype,EYE_SPELL) &&
	    !IS_SET(ch->spectype,EYE_SELFACTION) &&
	    !IS_SET(ch->spectype,EYE_ACTION)) send_to_char(" None",ch);
	send_to_char(".\n\r",ch);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    }
    return;
}

void do_spiriteyes( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_NECROMANCY) < 1)
    {
	send_to_char("You require level 1 Necromancy to use Spirit Eyes.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_SPIRITEYES) )
    {
	send_to_char("You can no longer see into the shadowlands.\n\r",ch);
	REMOVE_BIT(ch->vampaff, VAM_SPIRITEYES);
	return;
    }
    send_to_char("You can now see into the shadowlands.\n\r",ch);
    SET_BIT(ch->vampaff, VAM_SPIRITEYES);
    return;
}

void do_stake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    OBJ_DATA  *stake;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stake whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
	    if ( obj->item_type == ITEM_CORPSE_NPC )
	    {
		send_to_char("They have no stake in them to remove.\n\r",ch);
		return;
	    }
	    if ( obj->item_type != ITEM_CORPSE_PC )
	    {
		if (obj->chobj == NULL || obj->chobj != ch)
		    send_to_char("How can you remove a stake from THAT?\n\r",ch);
		else
		    send_to_char("There is no stake for you to remove.\n\r",ch);
		return;
	    }
	    if ( ( victim = obj->chobj ) == NULL )
	    {
		send_to_char("There is no stake for you to remove.\n\r",ch);
		return;
	    }
	    if ( ( stake = get_eq_char( victim, WEAR_STAKE ) ) == NULL )
	    {
		send_to_char("There is no stake for you to remove.\n\r",ch);
		return;
	    }
	    if (!IS_EXTRA(victim, EXTRA_OSWITCH) || !IS_MORE(victim, MORE_STAKED))
	    {
		send_to_char("There is no stake for you to remove.\n\r",ch);
		return;
	    }
	    else
	    {
		REMOVE_BIT(victim->more, MORE_STAKED);
		victim->pcdata->obj_vnum = 0;
		obj->chobj = NULL;
		victim->pcdata->chobj = NULL;
		REMOVE_BIT(victim->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(victim->extra, EXTRA_OSWITCH);
		free_string(victim->morph);
		victim->morph = str_dup("");
		if (victim == ch)
		{
		    act("You pull $p out of your heart.", ch, stake, victim, TO_CHAR);
		    act("$n pulls $p out of $s chest.", ch, stake, victim, TO_ROOM);
		}
		else
		{
		    act("You pull $p out of $N's heart.", ch, stake, victim, TO_CHAR);
		    act("$n pulls $p out of $N's chest.", ch, stake, victim, TO_NOTVICT);
		    send_to_char( "You feel the stake pulled from your heart.\n\r", victim );
		}
		obj_from_char(stake);
		obj_to_char(stake,ch);
		stake->timer = -1;
		if (obj->carried_by != NULL)
		{
		    if (victim != obj->carried_by)
		    {
			act("You drop $N onto the floor.", obj->carried_by, NULL, victim, TO_CHAR);
			act("$n drops $N onto the floor.", obj->carried_by, NULL, victim, TO_NOTVICT);
			act("$n drops you onto the floor.", obj->carried_by, NULL, victim, TO_VICT);
		    }
		}
		act("$n clambers to $s feet.",victim,NULL,NULL,TO_ROOM);
		act("You clamber back to your feet.",victim,NULL,NULL,TO_CHAR);
		extract_obj(obj);
		if (victim->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
		    char_from_room(victim);
		    char_to_room(victim,get_room_index(ROOM_VNUM_HELL));
		}
	    }
	    return;
	}
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ( stake = get_eq_char( victim, WEAR_STAKE ) ) != NULL )
    {
	if (victim == ch)
	{
	    act("You pull $p out of your heart.", ch, stake, victim, TO_CHAR);
	    act("$n pulls $p out of $s chest.", ch, stake, victim, TO_ROOM);
	}
	else if (!IS_IMMUNE(victim,IMM_VAMPIRE))
	    send_to_char("They refuse to let you near them.\n\r",ch);
	else
	{
	    act("You pull $p out of $N's heart.", ch, stake, victim, TO_CHAR);
	    act("$n pulls $p out of $N's chest.", ch, stake, victim, TO_NOTVICT);
	    act("$n pulls $p out of your chest.", ch, stake, victim, TO_VICT);
	}
	obj_from_char(stake);
	obj_to_char(stake,ch);
	return;
    }

    stake = get_eq_char( ch, WEAR_HOLD );
    if ( stake == NULL || stake->item_type != ITEM_STAKE )
    {
	stake = get_eq_char( ch, WEAR_WIELD );
	if ( stake == NULL || stake->item_type != ITEM_STAKE )
	{
	    send_to_char( "How can you stake someone down without holding a stake?\n\r", ch );
	    return;
	}
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot stake yourself.\n\r", ch );
	return;
    }

    if (!IS_VAMPIRE(victim))
    {
	send_to_char( "You can only stake vampires.\n\r", ch );
	return;
    }

    if (victim->position > POS_MORTAL)
    {
	send_to_char( "You can only stake a vampire who is in torpor.\n\r", ch );
	return;
    }

    if (get_eq_char(victim, WEAR_STAKE) != NULL)
    {
	send_to_char( "But they already have a stake through their chest!\n\r", ch );
	return;
    }

    if ( is_safe(ch, victim) ) return;

    act("You raise $p over $N's chest.", ch, stake, victim, TO_CHAR);
    act("$n raises $p over $N's chest.", ch, stake, victim, TO_NOTVICT);

    if (IS_MORE(victim, MORE_DEFLECTION))
    {
	act("$p explodes into tiny fragments in your hand.", ch, stake, NULL, TO_CHAR);
	act("$p explodes into tiny fragments in $n's hand.", ch, stake, victim, TO_ROOM);
	REMOVE_BIT(victim->more, MORE_DEFLECTION);
	extract_obj(stake);
	send_to_char("You fall to the ground, stunned.\n\r",ch);
	act("$n falls to the ground, stunned.", ch, NULL, NULL, TO_ROOM);
	ch->position = POS_STUNNED;
	return;
    }

    act("You plunge $p into $N's heart.", ch, stake, victim, TO_CHAR);
    act("$n plunges $p into $N's heart.", ch, stake, victim, TO_NOTVICT);

    if ( ( vch = victim->embrace ) != NULL )
    {
	vch->embrace = NULL;
	vch->embraced = ARE_NONE;
    }
    victim->embrace = NULL;
    victim->embraced = ARE_NONE;

    obj_from_char(stake);
    obj_to_char(stake,victim);
    equip_char(victim, stake, WEAR_STAKE);

    if (IS_IMMUNE(victim,IMM_STAKE))
    {
	send_to_char( "You feel a stake plunged through your chest.\n\r", victim );
	return;
    }
    stake->timer = (0.05 * victim->beast) + 1;
    send_to_char( "You feel a stake plunged through your heart.\n\r", victim );

    victim->pcdata->wolf = 0;
    if (IS_WEREWOLF(victim)) do_unwerewolf(victim,"");
    if (IS_VAMPAFF(victim, VAM_NIGHTSIGHT) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_nightsight(victim,"");
    if (IS_VAMPAFF(victim, VAM_FANGS) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_fangs(victim,"");
    if (IS_VAMPAFF(victim, VAM_CLAWS) && !IS_POLYAFF(victim, POLY_ZULO)) 
	do_claws(victim,"");

    mortal_vamp(victim);

    if (IS_AFFECTED(ch,AFF_POLYMORPH)) clear_stats(victim);
    if ((obj = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC),0)) == NULL)
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    victim->hit = 1;
    victim->position = POS_STANDING;
    staked_corpse(victim,obj);
    if (victim->mounted == IS_RIDING) do_dismount(victim,"");
    obj_to_room(obj,victim->in_room);
    victim->pcdata->obj_vnum = OBJ_VNUM_CORPSE_PC;
    obj->chobj = victim;
    victim->pcdata->chobj = obj;
    SET_BIT(victim->more, MORE_STAKED);
    SET_BIT(victim->affected_by, AFF_POLYMORPH);
    SET_BIT(victim->extra, EXTRA_OSWITCH);
    free_string(victim->morph);
    victim->morph = str_dup(obj->short_descr);
    return;
}

void ritual_ward( CHAR_DATA *ch, int direction, int ward )
{
    OBJ_DATA  *obj;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    char       arg [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    int        duration = number_range(1,2) * 15;
    int        rev;

    if (IS_NPC(ch)) return;

    if (ward != EX_WARD_GHOULS  &&
	ward != EX_WARD_LUPINES &&
	ward != EX_WARD_KINDRED &&
	ward != EX_WARD_SPIRITS )
    {
	send_to_char( "Bug - Please inform KaVir.\n\r", ch );
	return;
    }

         if (direction == DIR_NORTH) rev = DIR_SOUTH;
    else if (direction == DIR_SOUTH) rev = DIR_NORTH;
    else if (direction == DIR_EAST)  rev = DIR_WEST;
    else if (direction == DIR_WEST)  rev = DIR_EAST;
    else if (direction == DIR_UP)    rev = DIR_DOWN;
    else if (direction == DIR_DOWN)  rev = DIR_UP;
    else
    {
	send_to_char( "Bug - Please inform KaVir.\n\r", ch );
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

    if (IS_SET(pexit->exit_info, EX_WARD_GHOULS)  || 
	IS_SET(pexit->exit_info, EX_WARD_LUPINES) ||
	IS_SET(pexit->exit_info, EX_WARD_KINDRED) || 
	IS_SET(pexit->exit_info, EX_WARD_SPIRITS) )
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    obj = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    obj->item_type = ITEM_WARD;
    obj->value[0] = direction;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("ward");
    free_string(obj->short_descr);
    if      (ward == EX_WARD_GHOULS) 
	obj->short_descr = str_dup("a Ward vs Ghouls");
    else if (ward == EX_WARD_LUPINES) 
	obj->short_descr = str_dup("a Ward vs Lupines");
    else if (ward == EX_WARD_KINDRED) 
	obj->short_descr = str_dup("a Ward vs Kindred");
    else
	obj->short_descr = str_dup("a Ward vs Spirits");
    if (direction == DIR_UP)
	sprintf(buf,"A small ward is drawn in the centre of the ceiling.");
    else if (direction == DIR_DOWN)
	sprintf(buf,"A small ward is drawn in the centre of the floor.");
    else
	sprintf(buf,"A small ward is drawn on the floor to the %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, ward);

    if (direction == DIR_UP)
    {
	act( "You carefully draw $p in the centre of the ceiling.", ch, obj, NULL, TO_CHAR );
	act( "$n carefully draws $p in the centre of the ceiling.", ch, obj, NULL, TO_ROOM );
    }
    else if (direction == DIR_DOWN)
    {
	act( "You carefully draw $p in the centre of the floor.", ch, obj, NULL, TO_CHAR );
	act( "$n carefully draws $p in the centre of the floor.", ch, obj, NULL, TO_ROOM );
    }
    else
    {
	sprintf(buf,"You carefully draw $p on the floor to the %s.",arg);
	act( buf, ch, obj, NULL, TO_CHAR );
	sprintf(buf,"$n carefully draws $p on the floor to the %s.",arg);
	act( buf, ch, obj, NULL, TO_ROOM );
    }
    WAIT_STATE(ch,12);

    if (location == to_room) return;

    char_from_room(ch);
    char_to_room(ch, to_room);

    if ( ( pexit = ch->in_room->exit[rev] ) == NULL ) return;
    if (IS_SET(pexit->exit_info, ward)) return;

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
    obj->item_type = ITEM_WARD;
    obj->value[0] = rev;
    obj->timer = duration;
    free_string(obj->name);
    obj->name = str_dup("ward");
    free_string(obj->short_descr);
    if      (ward == EX_WARD_GHOULS) 
	obj->short_descr = str_dup("a Ward vs Ghouls");
    else if (ward == EX_WARD_LUPINES) 
	obj->short_descr = str_dup("a Ward vs Lupines");
    else if (ward == EX_WARD_KINDRED) 
	obj->short_descr = str_dup("a Ward vs Kindred");
    else
	obj->short_descr = str_dup("a Ward vs Spirits");
    free_string(obj->description);
    obj->description = str_dup("");
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, ward);

    char_from_room(ch);
    char_to_room(ch, location);

    return;
}

void show_disc( CHAR_DATA *ch )
{
    char      buf [MAX_STRING_LENGTH];
    int level;

    if (IS_NPC(ch) || !IS_VAMPIRE(ch)) return;

    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("                               -= Coterie Rules =-\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    if (IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION))
    {
	send_to_char( "Syntax: coterie rules <option>\n\r", ch );
	send_to_char( "Option can be any one of the following: \n\r", ch );
	send_to_char( "Clans Sects Banned Basic Intermediate Advanced Expert Hp Generation Skill \n\r", ch );
	send_to_char( "Stance Adv_Stance Golconda No_Diablerie No_Kill_Sect No_Kill_Coterie.\n\r", ch );
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    }
    sprintf( buf, "Sects allowed to join: %s.\n\r",ch->pcdata->cot_sides);
    send_to_char(buf,ch);
    sprintf( buf, "Clans allowed to join: %s.\n\r",ch->pcdata->cot_clans);
    send_to_char(buf,ch);

    for (level = COT_BANNED; level <= COT_EXPERT; level++)
    {
	switch (level)
	{
	    default: break;
	    case COT_BANNED:
		if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) 
		|| ch->pcdata->cot_min[level] > POW_NONE)
		    send_to_char("Banned disciplines:",ch);
		break;
	    case COT_BASIC:
		if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) 
		|| ch->pcdata->cot_min[level] > POW_NONE)
		    send_to_char("Basic disciplines required (Level 1+):",ch);
		break;
	    case COT_INTERMEDIATE:
		if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) 
		|| ch->pcdata->cot_min[level] > POW_NONE)
		    send_to_char("Intermediate disciplines required (Level 3+):",ch);
		break;
	    case COT_ADVANCED:
		if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) 
		|| ch->pcdata->cot_min[level] > POW_NONE)
		    send_to_char("Advanced disciplines required (Level 5+):",ch);
		break;
	    case COT_EXPERT:
		if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) 
		|| ch->pcdata->cot_min[level] > POW_NONE)
		    send_to_char("Expert disciplines required (Level 6+):",ch);
		break;
	}
	if (IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION) && 
	    ch->pcdata->cot_min[level] <= POW_NONE)
	{
	    send_to_char(" None.\n\r",ch);
	    continue;
	}
	if (IS_SET(ch->pcdata->cot_min[level], POW_ANIMALISM))
	    send_to_char(" Animalism",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_AUSPEX))
	    send_to_char(" Auspex",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_CELERITY))
	    send_to_char(" Celerity",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_CHIMERSTRY))
	    send_to_char(" Chimerstry",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_DAIMOINON))
	    send_to_char(" Daimoinon",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_DOMINATE))
	    send_to_char(" Dominate",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_FORTITUDE))
	    send_to_char(" Fortitude",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_MELPOMINEE))
	    send_to_char(" Melpominee",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_NECROMANCY))
	    send_to_char(" Necromancy",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_OBEAH))
	    send_to_char(" Obeah",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_OBFUSCATE))
	    send_to_char(" Obfuscate",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_OBTENEBRATION))
	    send_to_char(" Obtenebration",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_POTENCE))
	    send_to_char(" Potence",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_PRESENCE))
	    send_to_char(" Presence",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_PROTEAN))
	    send_to_char(" Protean",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_QUIETUS))
	    send_to_char(" Quietus",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_SERPENTIS))
	    send_to_char(" Serpentis",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_THANATOSIS))
	    send_to_char(" Thanatosis",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_THAUMATURGY))
	    send_to_char(" Thaumaturgy",ch);
	if (IS_SET(ch->pcdata->cot_min[level], POW_VICISSITUDE))
	    send_to_char(" Vicissitude",ch);
	if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	    ch->pcdata->cot_min[level] > POW_NONE)
	send_to_char(".\n\r",ch);
    }
    if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	ch->pcdata->cot_min[COT_HP] > 0 )
    {
	sprintf(buf, "Minimum number of hit points required: %d.\n\r",
	    ch->pcdata->cot_min[COT_HP]);
	send_to_char(buf,ch);
    }
    if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	ch->pcdata->cot_min[COT_GENERATION] < 13 )
    {
	sprintf(buf, "Highest generation permitted: %d.\n\r",
	    ch->pcdata->cot_min[COT_GENERATION]);
	send_to_char(buf,ch);
    }
    if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	ch->pcdata->cot_min[COT_SKILL] > 0 )
    {
	sprintf(buf, "Minimum weapon skill required: %d.\n\r",
	    ch->pcdata->cot_min[COT_SKILL]);
	send_to_char(buf,ch);
    }
    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_ADV_STANCE))
    {
	if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	    ch->pcdata->cot_min[COT_STANCE] > 0 )
	{
	    sprintf(buf, "Minimum advanced stance level required: %d.\n\r",
		ch->pcdata->cot_min[COT_STANCE]);
	    send_to_char(buf,ch);
	}
	else send_to_char("You must have an advanced stance to join.\n\r",ch);
    }
    else if ((IS_MORE(ch, MORE_LEADER) && !IS_MORE(ch, MORE_CHAMPION)) || 
	ch->pcdata->cot_min[COT_STANCE] > 0 )
    {
	sprintf(buf, "Minimum stance level required: %d.\n\r",
	    ch->pcdata->cot_min[COT_STANCE]);
	send_to_char(buf,ch);
    }
    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_GOLCONDA))
	send_to_char("You must have (and keep) Golconda.\n\r",ch);
    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_DIABLERIE))
	send_to_char("You must never commit diablerie.\n\r",ch);
    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_SECT))
	send_to_char("You must never decapitate someone of your sect.\n\r",ch);
    if (IS_SET(ch->pcdata->cot_min[COT_EXTRA], COT_NO_KILL_COTERIE))
	send_to_char("You must never decapitate someone within the coterie.\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_reina( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *rch;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_MELPOMINEE) < 2)
    {
	send_to_char("You require level 2 Melpominee to sing the Song of Reina.\n\r",ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_NEUTRAL))
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_REINA))
    {
	REMOVE_BIT(ch->more, MORE_REINA);
	REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE);
	send_to_char("The peaceful harmony in the air gradually fades away to nothing.\n\r", ch);
	act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
	act("The peaceful harmony in the air gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->reina = 60;
	return;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
    {
	send_to_char("You are unable to sing the Song of Reina here.\n\r", ch);
	return;
    }

    if (ch->pcdata->reina > 0)
    {
	if (ch->pcdata->reina > 1)
	    sprintf(buf,"Your voice will not recover for another %d seconds.\n\r",ch->pcdata->reina);
	else
	    sprintf(buf,"Your voice will not recover for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    SET_BIT(ch->in_room->room_flags, ROOM_SAFE);
    SET_BIT(ch->more, MORE_REINA);
    ch->pcdata->reina = 60;
    send_to_char("You sing the Song of Reina.\n\r", ch);
    act("$n sings a beautiful, haunting song.", ch, NULL, NULL, TO_ROOM );
    send_to_char("The air is filled with peaceful harmony.\n\r", ch);
    act("The air is filled with peaceful harmony.",ch,NULL,NULL,TO_ROOM);
    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }
    return;
}

void do_jail( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use Jail of Water on?\n\r",ch);
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
	if (is_affected(ch, gsn_jail))
	{
	    act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	    affect_strip(ch, gsn_jail);
	}
	else if (IS_AFFECTED(ch, AFF_JAIL))
	{
	    act("The band of water entrapping $n splash to the ground.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The bands of water entrapping you splash to the ground.\n\r",ch);
	    REMOVE_BIT(ch->affected_by, AFF_JAIL);
	}
	else send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot use Jail of Water on an ethereal person.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Jail of Water on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Jail of Water on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ( sn = skill_lookup( "jail" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = number_range(10,50);
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 24 );
    return;
}

void do_flames( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam, hp, sn;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch,DISC_DAIMOINON) < 7)
    {
	send_to_char("You require level 7 Daimoinon to use Flames of the Nether World.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char( "Who do you wish to strike with the Flames of the Nether World?\n\r", ch );
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

    if ( is_safe(ch, victim) )
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Flames of the Nether World on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Now that would be just plain stupid.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( "flame bolt" ) ) < 0 )
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }

    act( "A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );

    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->atm >= number_percent())
    {
	send_to_char("Nothing happens.\n\r",ch);
	WAIT_STATE(ch, 6);
	return;
    }

    dam = number_range(50,100) * get_disc(ch,DISC_DAIMOINON);
    hp = victim->hit;
    if (!IS_NPC(victim) && IS_VAMPIRE(victim) )
    	damage( ch, victim, (dam*2), sn );
    else
    	damage( ch, victim, dam, sn );
    WAIT_STATE(ch, 6);
    if (victim == NULL || victim->position == POS_DEAD) return;
    else if (!IS_MORE(victim, MORE_FLAMES))
    {
	send_to_char("You are engulfed in infernal flames!\n\r",victim);
	act("$n is engulfed in infernal flames!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->affected_by, AFF_FLAMING);
	SET_BIT(victim->more, MORE_FLAMES);
    }
    update_pos(victim);
    return;
}

void do_courage( CHAR_DATA *ch, char *argument )
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

    if (get_disc(ch,DISC_MELPOMINEE) < 3)
    {
	send_to_char("You require level 3 Melpominee to sing the Song of Courage.\n\r",ch);
	return;
    }

    if (ch->pcdata->reina > 0)
    {
	if (ch->pcdata->reina > 1)
	    sprintf(buf,"Your voice will not recover for another %d seconds.\n\r",ch->pcdata->reina);
	else
	    sprintf(buf,"Your voice will not recover for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use Courage on?\n\r", ch );
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

    send_to_char("You sing the Song of Courage.\n\r", ch);
    act("$n sings a beautiful, haunting song.", ch, NULL, NULL, TO_ROOM );
    ch->pcdata->reina = 60;

    if (IS_MORE(victim, MORE_COURAGE))
    {
	if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0 && ch != victim)
	{
	    send_to_char("They resist you.\n\r",ch);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to remove Courage from you.",ch,NULL,victim,TO_VICT);
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
	    send_to_char("You burn some willpower to resist Anger.\n\r",victim);
	    send_to_char("They resist you.\n\r",ch);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just failed to remove Courage from you.",ch,NULL,victim,TO_VICT);
	    }
	    return;
	}
	if (ch != victim) send_to_char("You remove their Courage.\n\r",ch);
	send_to_char("You no longer feel quite so brave.\n\r",victim);
	WAIT_STATE( ch, 12 );
	return;
    }

    if (ch != victim) send_to_char("You grant them great Courage.\n\r",ch);
    send_to_char("You suddenly feel very brave.\n\r",victim);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Courage on you.",ch,NULL,victim,TO_VICT);
    }
    SET_BIT(ch->more, MORE_COURAGE);
    WAIT_STATE( ch, 12 );
    return;
}

void do_anger( CHAR_DATA *ch, char *argument )
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

    if (get_disc(ch,DISC_MELPOMINEE) < 5)
    {
	send_to_char("You require level 5 Melpominee to sing the Song of Anger.\n\r",ch);
	return;
    }

    if (ch->pcdata->reina > 0)
    {
	if (ch->pcdata->reina > 1)
	    sprintf(buf,"Your voice will not recover for another %d seconds.\n\r",ch->pcdata->reina);
	else
	    sprintf(buf,"Your voice will not recover for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you wish to use the Song of Anger on?\n\r", ch );
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

    send_to_char("You sing the Song of Anger.\n\r", ch);
    act("$n sings a beautiful, haunting song.", ch, NULL, NULL, TO_ROOM );
    ch->pcdata->reina = 60;

    if (IS_MORE(victim, MORE_ANGER))
    {
	send_to_char("You no longer feel so angry.\n\r",victim);
	act("$n regains $s temper.",victim,NULL,NULL,TO_ROOM);
	REMOVE_BIT(victim->more, MORE_ANGER);
	WAIT_STATE( ch, 12 );
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_PRESENCE] > 0 && ch != victim)
    {
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Anger on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Anger.\n\r",victim);
	send_to_char("They resist you.\n\r",ch);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Anger on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Anger on you.",ch,NULL,victim,TO_VICT);
    }

    if (IS_MORE(victim, MORE_CALM))
    {
	send_to_char("You no longer feel so calm.\n\r",victim);
	act("$n no longer seems quite so calm.",victim,NULL,NULL,TO_ROOM);
	REMOVE_BIT(victim->more, MORE_CALM);
	WAIT_STATE( ch, 12 );
	return;
    }
    send_to_char("You suddenly feel very angry!\n\r",victim);
    act("$n snarls with anger.",victim,NULL,NULL,TO_ROOM);
    SET_BIT(victim->more, MORE_ANGER);
    WAIT_STATE( ch, 12 );
    return;
}

void do_panacea( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ANGEL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBEAH) < 1)
    {
	send_to_char("You require level 1 Obeah to use Panacea.\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0)
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your Obeah powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your Obeah powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to heal with Panacea?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_EVIL(victim))
    {
	send_to_char( "They are too evil to benefit from the powers of Obeah.\n\r", ch );
	return;
    }

    if (!IS_MORE(ch, MORE_OBEAH)) do_obeah(ch,"");

    if (victim->loc_hp[6] > 0)
    {
	int sn = skill_lookup( "clot" );
	act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
	ch->pcdata->obeah = 5;
	(*skill_table[sn].spell_fun) (sn,ch->level,ch,victim);
    }
    else
    {
	if ((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
	    victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5]) != 0)
	{
	    act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
	    ch->pcdata->obeah = 10;
	    reg_mend(victim);
	}
	else if (victim->hit < victim->max_hit)
	{
	    victim->hit += get_disc(ch, DISC_OBEAH) * 1000;
	    victim->agg -= get_disc(ch, DISC_OBEAH);
	    if (victim->agg < 0) victim->agg = 0;
	    act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
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
	act("$n just used Panacea on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_anesthetic( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ANGEL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBEAH) < 2)
    {
	send_to_char("You require level 2 Obeah to use Anesthetic Touch.\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0)
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your Obeah powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your Obeah powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to use Anesthetic Touch on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_EVIL(victim))
    {
	send_to_char( "They are too evil to benefit from the powers of Obeah.\n\r", ch );
	return;
    }

    if (!IS_MORE(ch, MORE_OBEAH)) do_obeah(ch,"");

    act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
    ch->pcdata->obeah = 30;

    if (IS_MORE(victim, MORE_CALM))
    {
	send_to_char("You remove their immunity to pain.\n\r",ch);
	send_to_char("You no longer feel so oblivious to pain.\n\r",victim);
	REMOVE_BIT(victim->more, MORE_NOPAIN);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just removed Anesthetic Touch from you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }
    send_to_char("You make them immune to pain.\n\r",ch);
    send_to_char("You feel completely oblivious to pain.\n\r",victim);
    SET_BIT(victim->more, MORE_NOPAIN);

    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Anesthetic Touch on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_neutral( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_INPUT_LENGTH];
    CHAR_DATA *rch;

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ANGEL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBEAH) < 3)
    {
	send_to_char("You require level 3 Obeah to create a Neutral Guard.\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0 && !IS_MORE(ch, MORE_NEUTRAL))
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your Obeah powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your Obeah powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if (ch->in_room == NULL)
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_REINA))
    {
	send_to_char("Nothing happens.\n\r",ch);
	return;
    }

    if (IS_MORE(ch, MORE_NEUTRAL))
    {
	REMOVE_BIT(ch->more, MORE_NEUTRAL);
	REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE);
	act("A look of concentration crosses $n's face.", ch, NULL, NULL, TO_ROOM );
	send_to_char("The peaceful harmony in the air gradually fades away to nothing.\n\r", ch);
	act("The peaceful harmony in the air gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
	ch->pcdata->obeah = 30;
	return;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
    {
	send_to_char("You are unable to create a Neutral Guard here.\n\r", ch);
	return;
    }

    SET_BIT(ch->in_room->room_flags, ROOM_SAFE);
    SET_BIT(ch->more, MORE_NEUTRAL);
    act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
    ch->pcdata->obeah = 30;
    send_to_char("The air is filled with peaceful harmony.\n\r", ch);
    act("The air is filled with peaceful harmony.",ch,NULL,NULL,TO_ROOM);
    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
    }
    return;
}

void do_unburden( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ANGEL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBEAH) < 4)
    {
	send_to_char("You require level 4 Obeah to Unburden the Bestial Soul.\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0)
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your Obeah powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your Obeah powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who's Soul do you wish to Unburden?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (ch == victim)
    {
	send_to_char( "You may not do this too yourself.\n\r", ch );
	return;
    }

    if (!IS_VAMPIRE(victim) || IS_ABOMINATION(victim) || IS_LICH(victim))
    {
	send_to_char( "You can only Unburden the Soul of a Kindred.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE))
    {
	send_to_char( "They must lower their defences before you can try to Unburden their Soul.\n\r", ch );
	return;
    }

    if (victim->position > POS_RESTING)
    {
	send_to_char( "You better get them to sit down first.\n\r", ch );
	return;
    }

    if (victim->beast == 100)
    {
	send_to_char( "Their Soul is too Bestial to Unburden.\n\r", ch );
	return;
    }

    if (IS_EVIL(victim))
    {
	send_to_char( "They are too evil to benefit from the powers of Obeah.\n\r", ch );
	return;
    }

    if (!IS_MORE(ch, MORE_OBEAH)) do_obeah(ch,"");

    act( "You place your hands against $N's chest.", ch, NULL, victim, TO_CHAR );
    act( "$n places $s hands against your chest.", ch, NULL, victim, TO_VICT );
    act( "$n places $s hands against $N's chest.", ch, NULL, victim, TO_NOTVICT );
    if (victim->beast > victim->truebeast && victim->beast > 1)
    {
	act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
	victim->beast -= get_disc(ch, DISC_OBEAH);
	if ( victim->beast < 1 ) victim->beast = 1;
	ch->pcdata->obeah = 30;
	if (victim->beast < victim->truebeast) victim->beast = victim->truebeast;
	send_to_char("You feel their beast weaken it's grip!\n\r",ch);
	send_to_char("You feel your beast weaken it's grip!\n\r",victim);
    }
    else
    {
	send_to_char("You are unable to help them any further.\n\r",ch);
	return;
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Unburden the Bestial Soul on you.",ch,NULL,victim,TO_VICT);
    }
    send_to_char("Your hands drop to your sides.\n\r",ch);
    act( "$n's hands drop to $s sides.", ch, NULL, NULL, TO_ROOM );
    return;
}

void do_renew( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_STRING_LENGTH];
    int       sn, check, oldcheck;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch) && !IS_ANGEL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_OBEAH) < 5)
    {
	send_to_char("You require level 5 Obeah to use Renewed Vigor.\n\r",ch);
	return;
    }

    if (ch->position == POS_FIGHTING)
    {
	send_to_char("No way!  You are still fighting!\n\r",ch);
	return;
    }

    if (ch->pcdata->obeah > 0)
    {
	if (ch->pcdata->obeah > 1)
	    sprintf(buf,"You cannot call upon your Obeah powers for another %d seconds.\n\r",ch->pcdata->obeah);
	else
	    sprintf(buf,"You cannot call upon your Obeah powers for another 1 second.\n\r");
	send_to_char(buf,ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who do you want to use Renewed Vigor on?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if (IS_EVIL(victim))
    {
	send_to_char( "They are too evil to benefit from the powers of Obeah.\n\r", ch );
	return;
    }

    if (!IS_MORE(ch, MORE_OBEAH)) do_obeah(ch,"");

    if ((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] + 
	 victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] + 
	 victim->loc_hp[6]) == 0 && victim->hit == victim->max_hit)
    {
	send_to_char("They don't require any healing.\n\r",ch);
	return;
    }

    act( "$n's third eye begins to glow with an intense white light.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Your third eye begins to glow with an intense white light.\n\r", ch );
    ch->pcdata->obeah = 60;

    if ( (check = victim->loc_hp[6]) > 0)
    {
	oldcheck = 0;
	sn = skill_lookup( "clot" );
	while ( oldcheck != (check = victim->loc_hp[6]) )
	{
	    oldcheck = check;
	    (*skill_table[sn].spell_fun) (sn,ch->level,ch,victim);
	}
    }
    if ((check = (victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
	victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5])) > 0)
    {
	oldcheck = 0;
	while ( oldcheck != (check = victim->loc_hp[0] + victim->loc_hp[1] + 
		victim->loc_hp[2] + victim->loc_hp[3] + victim->loc_hp[4] + 
		victim->loc_hp[5]) )
	{
	    oldcheck = check;
	    reg_mend(victim);
	}
    }
    if (victim->hit < victim->max_hit)
    {
	victim->hit = victim->max_hit;
	victim->agg = 0;
	send_to_char("You have been completely healed!\n\r",victim);
	act("$n's wounds have been completely healed!",victim,NULL,NULL,TO_ROOM);
	update_pos(victim);
    }
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Renewed Vigor on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}

void do_lick( CHAR_DATA *ch, char *argument )
{
    char      arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "You lick your lips and smile.\n\r", ch );
	act("$n licks $s lips and smiles.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    else if ( ( victim = get_char_room( ch, arg ) ) != NULL )
    {
	if ( victim == ch )
	{
	    send_to_char( "You lick yourself.\n\r", ch );
	    act("$n licks $mself - YUCK.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    act("You lick $M.",ch,NULL,victim,TO_CHAR);
	    act("$n licks you.",ch,NULL,victim,TO_VICT);
	    act("$n licks $N.",ch,NULL,victim,TO_NOTVICT);
	}
	return;
    }
    else
    {
	if (IS_VAMPIRE(ch))
	{
	    if (!str_cmp(arg,"l") || !str_cmp(arg,"left"))
	    {
		if (!IS_ARM_L(ch, SLIT_WRIST))
		{
		    send_to_char("Your left wrist doesn't need licking shut.\n\r",ch);
		    return;
		}
		act("You lick shut the cut in your left wrist.",ch,NULL,NULL,TO_CHAR);
		act("$n licks shut the cut in $s left wrist.",ch,NULL,NULL,TO_ROOM);
		REMOVE_BIT(ch->loc_hp[6], BLEEDING_WRIST_L);
		REMOVE_BIT(ch->loc_hp[LOC_ARM_L], SLIT_WRIST);
		return;
	    }
	    else if (!str_cmp(arg,"r") || !str_cmp(arg,"right"))
	    {
		if (!IS_ARM_R(ch, SLIT_WRIST))
		{
		    send_to_char("Your right wrist doesn't need licking shut.\n\r",ch);
		    return;
		}
		act("You lick shut the cut in your right wrist.",ch,NULL,NULL,TO_CHAR);
		act("$n licks shut the cut in $s right wrist.",ch,NULL,NULL,TO_ROOM);
		REMOVE_BIT(ch->loc_hp[6], BLEEDING_WRIST_R);
		REMOVE_BIT(ch->loc_hp[LOC_ARM_R], SLIT_WRIST);
		return;
	    }
	}
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    return;
}

void do_breath( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj_lose;
    OBJ_DATA *obj_next;
    int dam, sn;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
    {
	send_to_char( "Bug...please inform KaVir.\n\r", ch );
	return;
    }

    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (get_disc(ch,DISC_SERPENTIS) < 5)
    {
	send_to_char("You require level 5 Serpentis to use Breath of the Basilisk.\n\r",ch);
	return;
    }

    if (IS_ITEMAFF(ch, ITEMA_PEACE)) do_majesty(ch,"");

    if (arg[0] == '\0' && ch->fighting == NULL)
    {
	send_to_char("Who do you wish to use Breath of the Basilisk on?\n\r",ch);
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

    if (is_safe(ch,victim))
    {
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Breath of the Basilisk on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    dam = number_range(500,1000) * get_disc(ch, DISC_SERPENTIS) * get_disc(ch, DISC_SERPENTIS);

    if ( !IS_AWAKE(victim) )
	dam *= 2;

    damage( ch, victim, dam, sn + TYPE_ADD_AGG );

    WAIT_STATE( ch, 6 );

    if ( number_percent( ) < (get_disc(ch,DISC_SERPENTIS) << 2) )
    {
	for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
	{
	    obj_next = obj_lose->next_content;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->condition > 0 )
		{
		    act( "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR );
		    obj_lose->condition -= 25;
		    if ( obj_lose->condition < 1 )
			extract_obj( obj_lose );
		}
		break;

	    case ITEM_SCROLL:
	    case ITEM_WAND:
	    case ITEM_STAFF:
	    case ITEM_TRASH:
	    case ITEM_POTION:
	    case ITEM_FURNITURE:
	    case ITEM_FOOD:
	    case ITEM_BOAT:
	    case ITEM_PILL:
	    case ITEM_CORPSE_NPC:
	    case ITEM_DRINK_CON:
	    case ITEM_CONTAINER:
		act( "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }
    return;
}

void do_celerity( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( (IS_NPC(ch) || (!IS_VAMPIRE(ch) && !IS_GHOUL(ch)))
	&& get_disc(ch, DISC_CELERITY) < 1 )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (get_disc(ch, DISC_CELERITY) < 1)
    {
	send_to_char("But you don't have any Celerity!\n\r",ch);
	return;
    }

    if (arg[0] != '\0')
    {
	if (!str_cmp(arg,"renew"))
	{
	    if (IS_WILLPOWER(ch, RES_CELERITY))
	    {
		send_to_char("You will no longer automatically renew Celerity when you are in a fight.\n\r",ch);
		REMOVE_BIT(ch->pcdata->resist[0], RES_CELERITY);
	    }
	    else
	    {
		send_to_char("You will now automatically renew Celerity when you are in a fight.\n\r",ch);
		SET_BIT(ch->pcdata->resist[0], RES_CELERITY);
	    }
	}
	else
	    send_to_char("Type 'celerity' to activate, or 'celerity renew' to toggle autorenew.\n\r",ch);
	return;
    }
    else
    {
	if ( ch->blood[BLOOD_CURRENT] < 10 )
	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
	}

	if ( ch->pcdata->celerity > 60 )
	{
	    send_to_char("You are unable to burn any more blood for celerity.\n\r",ch);
	    return;
	}
	else
	{
	    if ( ch->pcdata->celerity > 0 )
		send_to_char("You burn some more blood to keep your celerity running longer.\n\r",ch);
	    else
	    {
		send_to_char("You start moving with supernatural speed!\n\r",ch);
		act("$n starts moving with supernatural speed!",ch,NULL,NULL,TO_ROOM);
	    }
	    ch->pcdata->celerity += 60;
	    ch->blood[BLOOD_CURRENT] -= 10;
	    return;
	}
    }
    return;
}

void guardian_message( CHAR_DATA *ch )
{
    if ( IS_DEMON(ch) )
    {
	if ( IS_ANGEL(ch) )
	{
	    send_to_char("Your little cherub whispers softly into your ear:\n\r",ch);
	    act( "A little cherub on $n's shoulder whispers something into $n's ear.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    send_to_char("Your pet imp whispers softly into your ear:\n\r",ch);
	    act( "A tiny imp on $n's shoulder whispers something into $n's ear.",ch,NULL,NULL,TO_ROOM);
	}
    }
    else
	send_to_char("Your guardian spirit sends you a telepathic message:\n\r",ch);
    return;
}
