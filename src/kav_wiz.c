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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void quest_clone args( ( CHAR_DATA *ch, OBJ_DATA *obj ) ); 

void clear_stats( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int disc;

    if (IS_NPC(ch)) return;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->wear_loc != WEAR_NONE )
	{obj_from_char(obj); obj_to_char(obj,ch);}
    }
    while ( ch->affected )
	affect_remove( ch, ch->affected );
    ch->armor			= 100;
    ch->hitroll			= 0;
    ch->damroll			= 0;
    ch->saving_throw		= 0;
    ch->pcdata->demonic		= 0;
    ch->pcdata->atm		= 0;
    ch->pcdata->followers	= 0;
    ch->pcdata->powers_set_wear	= 0;

    if (ch->polyaff > 0)
    {
	if (IS_POLYAFF(ch, POLY_BAT)   || IS_POLYAFF(ch, POLY_WOLF)    ||
	    IS_POLYAFF(ch, POLY_MIST)  || IS_POLYAFF(ch, POLY_SERPENT) ||
	    IS_POLYAFF(ch, POLY_RAVEN) || IS_POLYAFF(ch, POLY_FISH)    ||
	    IS_POLYAFF(ch, POLY_FROG)  || IS_POLYAFF(ch, POLY_SHADOW)  )
	{
	    do_autosave( ch, "" );
	    return;
	}
    }
    if (IS_VAMPIRE(ch) && ch->polyaff < 1)
    {
	for ( disc = DISC_ANIMALISM; disc <= DISC_VICISSITUDE; disc++ )
	{
	    ch->pcdata->powers_mod[disc] = 0;
	    ch->pcdata->powers_set[disc] = 0;
	}
    }

    do_autosave( ch, "" );
    return;
}



/* In case I need to remove my pfiles, or wanna turn mortal for a bit */
void do_relevel( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!str_cmp(ch->name,"Put your name here"))
    {
	ch->level = MAX_LEVEL;
	ch->trust = MAX_LEVEL;
	send_to_char("Done.\n\r",ch);
    }
    else
	send_to_char("Huh?\n\r",ch);
    return;
}

void do_propose( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are already married!\n\r",ch);
	else
	    send_to_char("But you are already engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who do you wish to propose marriage to?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (ch->mkill < 5 || victim->mkill < 5)
    {
	send_to_char("Not until you have both saved.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are already married!\n\r",ch);
	else
	    send_to_char("But they are already engaged!\n\r",ch);
	return;
    }

    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||
	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )
    {
	ch->pcdata->propose = victim;
	act("You propose marriage to $M.",ch,NULL,victim,TO_CHAR);
	act("$n gets down on one knee and proposes to $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n asks you quietly 'Will you marry me?'",ch,NULL,victim,TO_VICT);
	return;
    }
    send_to_char("I don't think that would be a very good idea...\n\r",ch);
    return;
}

void do_accept( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are already married!\n\r",ch);
	else
	    send_to_char("But you are already engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who's proposal of marriage do you wish to accept?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (ch->mkill < 5 || victim->mkill < 5)
    {
	send_to_char("Not until you have both saved.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are already married!\n\r",ch);
	else
	    send_to_char("But they are already engaged!\n\r",ch);
	return;
    }

    if ( victim->pcdata->propose != ch )
    {
	send_to_char("But they haven't proposed to you!\n\r",ch);
	return;
    }

    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||
	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )
    {
	victim->pcdata->propose = NULL;
	ch->pcdata->propose = NULL;
	free_string(victim->pcdata->marriage);
	victim->pcdata->marriage = str_dup( ch->name );
	free_string(ch->pcdata->marriage);
	ch->pcdata->marriage = str_dup( victim->name );
	act("You accept $S offer of marriage.",ch,NULL,victim,TO_CHAR);
	act("$n accepts $N's offer of marriage.",ch,NULL,victim,TO_NOTVICT);
	act("$n accepts your offer of marriage.",ch,NULL,victim,TO_VICT);
	do_autosave(ch,"");
	do_autosave(victim,"");
	sprintf(buf,"%s and %s are now engaged!",ch->name,victim->name);
	do_info(ch,buf);
	return;
    }
    send_to_char("I don't think that would be a very good idea...\n\r",ch);
    return;
}

void do_breakup( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	{
	    send_to_char("You'll have to get divorced.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("But you are not even engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who do you wish to break up with?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	{
	    send_to_char("They'll have to get divorced.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("But they are not even engaged!\n\r",ch);
	return;
    }

    if (!str_cmp(ch->name, victim->pcdata->marriage) &&
	!str_cmp(victim->name, ch->pcdata->marriage))
    {
	free_string(victim->pcdata->marriage);
	victim->pcdata->marriage = str_dup( "" );
	free_string(ch->pcdata->marriage);
	ch->pcdata->marriage = str_dup( "" );
	act("You break off your engagement with $M.",ch,NULL,victim,TO_CHAR);
	act("$n breaks off $n engagement with $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n breaks off $s engagement with you.",ch,NULL,victim,TO_VICT);
	do_autosave(ch,"");
	do_autosave(victim,"");
	sprintf(buf,"%s and %s have broken up!",ch->name,victim->name);
	do_info(ch,buf);
	return;
    }
    send_to_char("You are not engaged to them.\n\r",ch);
    return;
}

void do_marry( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: marry <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (IS_SET(victim1->act, PLR_GODLESS) && ch->level < NO_GODLESS)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if (IS_SET(victim2->act, PLR_GODLESS) && ch->level < NO_GODLESS)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	SET_BIT(victim1->extra, EXTRA_MARRIED);
	SET_BIT(victim2->extra, EXTRA_MARRIED);
	do_autosave(victim1,"");
	do_autosave(victim2,"");
	sprintf(buf,"%s and %s are now married!",victim1->name,victim2->name);
	do_info(ch,buf);
	return;
    }
    send_to_char("But they are not yet engaged!\n\r",ch);
    return;
}

void do_divorce( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: divorse <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (IS_SET(victim1->act, PLR_GODLESS) && ch->level < NO_GODLESS)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if (IS_SET(victim2->act, PLR_GODLESS) && ch->level < NO_GODLESS)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	if (!IS_EXTRA(victim1,EXTRA_MARRIED) || !IS_EXTRA(victim2,EXTRA_MARRIED))
	{
	    send_to_char("But they are not married!\n\r",ch);
	    return;
	}
	REMOVE_BIT(victim1->extra, EXTRA_MARRIED);
	REMOVE_BIT(victim2->extra, EXTRA_MARRIED);
	free_string(victim1->pcdata->marriage);
	victim1->pcdata->marriage = str_dup( "" );
	free_string(victim2->pcdata->marriage);
	victim2->pcdata->marriage = str_dup( "" );
	do_autosave(victim1,"");
	do_autosave(victim2,"");
	sprintf(buf,"%s and %s are now divorced!",victim1->name,victim2->name);
	do_info(ch,buf);
	return;
    }
    send_to_char("But they are not married!\n\r",ch);
    return;
}

void do_improve( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    int value;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Improve <attribute> <bonus>\n\r",ch);
	send_to_char("Attribute being one of: hit, dam, ac.\n\r",ch);
	send_to_char("Hit: 30qp each, Dam: 30qp each, Ac: 10qp each.\n\r",ch);
	return;
    }
    if ( arg2[0] != '\0' ) value = atoi( arg2 );
    else
    {
	send_to_char("Improve <attribute> <bonus>\n\r",ch);
	send_to_char("Attribute being one of: hit, dam, ac.\n\r",ch);
	send_to_char("Hit: 30qp each, Dam: 30qp each, Ac: 10qp each.\n\r",ch);
	return;
    }
    if (value < 1)
    {
	send_to_char("Improve <attribute> <bonus>\n\r",ch);
	send_to_char("Attribute being one of: hit, dam, ac.\n\r",ch);
	send_to_char("Hit: 30qp each, Dam: 30qp each, Ac: 10qp each.\n\r",ch);
	return;
    }

    if (!str_cmp(arg1,"hit") || !str_cmp(arg1,"hitroll") || !str_cmp(arg1,"hr"))
    {
	if (ch->pcdata->qstats[QS_HIT] + value > 150)
	{
	    send_to_char("You are limited to 150 hitroll.\n\r",ch);
	    return;
	}
	if ((value * 30) > ch->pcdata->quest)
	{
	    send_to_char("You have insufficient quest points.\n\r",ch);
	    return;
	}
	ch->pcdata->qstats[QS_HIT] += value;
	ch->pcdata->quest -= (value*30);
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"dam") || !str_cmp(arg1,"damroll") || !str_cmp(arg1,"dr"))
    {
	if (ch->pcdata->qstats[QS_DAM] + value > 150)
	{
	    send_to_char("You are limited to 150 damroll.\n\r",ch);
	    return;
	}
	if ((value * 30) > ch->pcdata->quest)
	{
	    send_to_char("You have insufficient quest points.\n\r",ch);
	    return;
	}
	ch->pcdata->qstats[QS_DAM] += value;
	ch->pcdata->quest -= (value*30);
	send_to_char("Ok.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"armor") || !str_cmp(arg1,"armour") || !str_cmp(arg1,"ac"))
    {
	if (ch->pcdata->qstats[QS_AC] + value > 500)
	{
	    send_to_char("You are limited to 500 armour class.\n\r",ch);
	    return;
	}
	if ((value * 10) > ch->pcdata->quest)
	{
	    send_to_char("You have insufficient quest points.\n\r",ch);
	    return;
	}
	ch->pcdata->qstats[QS_AC] += value;
	ch->pcdata->quest -= (value*10);
	send_to_char("Ok.\n\r",ch);
	return;
    }
    send_to_char("Improve <attribute> <bonus>\n\r",ch);
    send_to_char("Attribute being one of: hit, dam, ac.\n\r",ch);
    send_to_char("Hit: 30qp each, Dam: 30qp each, Ac: 10qp each.\n\r",ch);
    return;
}

void do_quest( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    char arg3 [MAX_STRING_LENGTH];
    char buf  [MAX_STRING_LENGTH]; 
    char endchar[10];
    OBJ_DATA *obj = NULL;
    OBJ_INDEX_DATA *pObjIndex = NULL;
    int value;
    int add;
    int remove;

    if (IS_NPC(ch)) {send_to_char("Not while switched.\n\r",ch); return;}

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] != '\0' && !str_cmp( arg1, "create" ) )
    {
	if (ch->pcdata->quest < 1)
	{
	    send_to_char("But you don't have any quest points!\n\r",ch);
	    return;
	}
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not allowed to create new objects.\n\r",ch);
	    return;
	}
    	else if ( arg2[0] == '\0' )
	{
	    send_to_char("Syntax: quest create <object> <field>\n\rObject being one of: Light (10 QP), Weapon <type> (50 QP), Armor (30 QP),\n\rContainer (10 QP), Boat (10 QP), Fountain <type> (10 QP), Stake (10 QP).\n\r",ch);
	    return;
	}

    	     if (!str_cmp(arg2,"light"    )) {add = ITEM_LIGHT;     value = 10;}
    	else if (!str_cmp(arg2,"weapon"   )) {add = ITEM_WEAPON;    value = 50;}
    	else if (!str_cmp(arg2,"armor"    )) {add = ITEM_ARMOR;     value = 20;}
    	else if (!str_cmp(arg2,"armour"   )) {add = ITEM_ARMOR;     value = 20;}
    	else if (!str_cmp(arg2,"container")) {add = ITEM_CONTAINER; value = 10;}
    	else if (!str_cmp(arg2,"boat"     )) {add = ITEM_BOAT;      value = 10;}
    	else if (!str_cmp(arg2,"fountain" )) {add = ITEM_FOUNTAIN;  value = 10;}
    	else if (!str_cmp(arg2,"stake"    )) {add = ITEM_STAKE;     value = 10;}
	else
	{
	    send_to_char("Syntax: quest create <object> <field>\n\rObject being one of: Light (1 QP), Weapon <type> (5 QP), Armor (1 QP),\n\rContainer (1 QP), Boat (1 QP), Fountain <type> (1 QP), Stake (1 QP).\n\r",ch);
	    return;
	}
	if (ch->pcdata->quest < value)
	{
	    sprintf(buf,"You don't have the required %d quest points.\n\r",value);
	    send_to_char(buf,ch);
	    return;
	}
    	if ( ( pObjIndex = get_obj_index( OBJ_VNUM_PROTOPLASM ) ) == NULL )
    	{
	    send_to_char( "Error...missing object, please inform KaVir.\n\r", ch );
	    return;
    	}
    	obj = create_object( pObjIndex, 25 );
    	obj->weight = 1;
    	obj->cost   = 1000;
    	obj->item_type = add;
	if (add == ITEM_WEAPON)
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify weapon type: Slice, Stab, Slash, Whip, Claw, Blast, Pound,\n\rCrush, Pierce, or Suck.\n\r",ch);
		if (obj != NULL) extract_obj(obj);
		return;
	    }
	    else if (!str_cmp(arg3,"slice" )) obj->value[3] = 1;
	    else if (!str_cmp(arg3,"stab"  )) obj->value[3] = 2;
	    else if (!str_cmp(arg3,"slash" )) obj->value[3] = 3;
	    else if (!str_cmp(arg3,"whip"  )) obj->value[3] = 4;
	    else if (!str_cmp(arg3,"claw"  )) obj->value[3] = 5;
	    else if (!str_cmp(arg3,"blast" )) obj->value[3] = 6;
	    else if (!str_cmp(arg3,"pound" )) obj->value[3] = 7;
	    else if (!str_cmp(arg3,"crush" )) obj->value[3] = 8;
	    else if (!str_cmp(arg3,"pierce")) obj->value[3] = 11;
	    else if (!str_cmp(arg3,"suck"  )) obj->value[3] = 12;
	    else {
		send_to_char("Please specify weapon type: Slice, Stab, Slash, Whip, Claw, Blast, Pound,\n\rCrush, Pierce, or Suck.\n\r",ch);
		if (obj != NULL) extract_obj(obj);
		return;}
	    obj->value[1] = 10;
	    obj->value[2] = 20;
	    obj->level    = 50;
	}
	else if (add == ITEM_FOUNTAIN)
	{
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify fountain contents: Water, Beer, Wine, Ale, Darkale, Whisky,\n\rFirebreather, Specialty, Slime, Milk, Tea, Coffee, Blood, Saltwater.\n\r",ch);
		if (obj != NULL) extract_obj(obj);
		return;
	    }
	    else if (!str_cmp(arg3,"water"        )) obj->value[2] = 0;
	    else if (!str_cmp(arg3,"beer"         )) obj->value[2] = 1;
	    else if (!str_cmp(arg3,"wine"         )) obj->value[2] = 2;
	    else if (!str_cmp(arg3,"ale"          )) obj->value[2] = 3;
	    else if (!str_cmp(arg3,"darkale"      )) obj->value[2] = 4;
	    else if (!str_cmp(arg3,"whisky"       )) obj->value[2] = 5;
	    else if (!str_cmp(arg3,"firebreather" )) obj->value[2] = 7;
	    else if (!str_cmp(arg3,"specialty"    )) obj->value[2] = 8;
	    else if (!str_cmp(arg3,"slime"        )) obj->value[2] = 9;
	    else if (!str_cmp(arg3,"milk"         )) obj->value[2] = 10;
	    else if (!str_cmp(arg3,"tea"          )) obj->value[2] = 11;
	    else if (!str_cmp(arg3,"coffee"       )) obj->value[2] = 12;
	    else if (!str_cmp(arg3,"blood"        )) obj->value[2] = 13;
	    else if (!str_cmp(arg3,"saltwater"    )) obj->value[2] = 14;
	    else {
		send_to_char("Please specify fountain contents: Water, Beer, Wine, Ale, Darkale, Whisky,\n\rFirebreather, Specialty, Slime, Milk, Tea, Coffee, Blood, Saltwater.\n\r",ch);
		if (obj != NULL) extract_obj(obj);
		return;}
	    obj->value[0] = 1000;
	    obj->value[1] = 1000;
	}
	else if (add == ITEM_CONTAINER)
	    obj->value[0] = 999;
	else if (add == ITEM_LIGHT)
	    obj->value[2] = -1;
	else if (add == ITEM_ARMOR)
	    obj->value[0] = 15;
	ch->pcdata->quest -= value;
    	obj_to_char(obj,ch);
	SET_BIT(obj->quest,QUEST_FREENAME);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
    	if (obj->questowner != NULL) free_string(obj->questowner);
    	obj->questowner = str_dup(ch->name);
    	act( "You reach up into the air and draw out a ball of protoplasm.", ch, obj, NULL, TO_CHAR );
    	act( "$n reaches up into the air and draws out a ball of protoplasm.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "- - - - - - - - - - ----====[ QUEST ITEM COSTS ]====---- - - - - - - - - - -\n\r",	 ch );
	send_to_char( "Rename: Renames the object for 1 QP.\n\r",ch );
	send_to_char( "Keyword: Add one or more object keywords.\n\r",ch );
	send_to_char( "Create: Creates a new personalized object, costing between 10 and 50 QP.\n\r",ch );
	send_to_char( "Clone: Clones the object for original's cost (minimum of 5 QP).\n\r",ch );
	send_to_char( "Transporter: Future transportation to that room.  Costs 50 QP.\n\r",ch );
	send_to_char( "Str, Dex, Int, Wis, Con... max =   3 each, at  20 quest points per +1 stat.\n\r", ch);
	send_to_char( "Hp, Mana, Move............ max =  25 each, at   5 quest points per point.\n\r", ch);
	send_to_char( "Hitroll, Damroll.......... max =   5 each, at  30 quest points per point.\n\r", ch);
	send_to_char( "Ac........................ max = -25,      at  10 points per point.\n\r", ch);
	send_to_char( "Protection: Sets AC on armour at 1 QP per point.\n\r", ch );
	send_to_char( "Min/Max: Sets min/max damage on weapon at 1 QP per point.\n\r", ch );
	send_to_char( "Weapon: Sets weapon type for 10 QP.\n\r", ch );
	send_to_char( "Extra (add/remove): Glow(1/1), Hum(1/1), Invis(1/1), Anti-Good(1/10),\n\r                    Anti-Neutral(1/10), Anti-Evil(1/10), Loyal(10/1).\n\r",ch );
	send_to_char( "Power: Spell power for spell weapons.  Costs 1 QP per power point.\n\r",ch );
	send_to_char( "Spell: Spell weapons or affect.  Costs 50 QP.\n\r",ch );
	send_to_char( "Wear: Select location, costs 20 QP's.  Type 'quest <obj> wear' to see locations.\n\r",ch );
	send_to_char( "Special: Set activate/twist/press/pull flags.\n\r",ch );
	send_to_char( "You-in/You-out/Other-in/Other-out: Renames for transporter actions.\n\r",ch );
	send_to_char( "You-wear/You-remove/You-use: What you see when you wear/remove/use.\n\r",ch );
	send_to_char( "Other-wear/Other-remove/Other-use: What others see when you wear/remove/use.\n\r",ch );
	send_to_char( "Weight: Set objects weight to 1.  Costs 10 QP.\n\r",ch );
	send_to_char( "- - - - - - - - - - ----====[ QUEST ITEM COSTS ]====---- - - - - - - - - - -\n\r",	 ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
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

    /*
     * Snarf the value (which need not be numeric).
     */
    if ( arg3[0] != '\0' ) value = atoi( arg3 );
    else value = 0;

    if ( !str_cmp( arg2, "protection" ) )
    {
	if ( arg3[0] == '\0' )
		{send_to_char("How much armor class?\n\r", ch); return;}
	if (obj->item_type != ITEM_ARMOR)
	{
	    send_to_char("That item is not armor.\n\r", ch);
	    return;
	}
	else if (obj->item_type == ITEM_ARMOR && (value+obj->value[0]) > 15)
	{
	    if (obj->value[0] < 15)
	    	sprintf(buf,"The armor class can be increased by %d.\n\r",(15 - obj->value[0]));
	    else
	    	sprintf(buf,"The armor class cannot be increased any further.\n\r");
	    send_to_char(buf, ch);
	    return;
	}
	else if (value > ch->pcdata->quest)
	{
	    send_to_char("You don't have enough quest points.\n\r", ch);
	    return;
	}
	else
		obj->value[0] += value;
	if (obj->value[0] < 0) obj->value[0] = 0;
	send_to_char("Ok.\n\r",ch);
	if (value < 1) value = 1;
	ch->pcdata->quest -= value;
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "min" ) )
    {
	if ( arg3[0] == '\0' )
		{send_to_char("How much min damage?\n\r", ch); return;}
	if (obj->item_type != ITEM_WEAPON)
	{
	    send_to_char("That item is not a weapon.\n\r", ch);
	    return;
	}
	else if (obj->item_type == ITEM_WEAPON && (value+obj->value[1]) > 10)
	{
	    if (obj->value[1] < 10)
	    	sprintf(buf,"The minimum damage can be increased by %d.\n\r",(10 - obj->value[1]));
	    else
	    	sprintf(buf,"The minimum damage cannot be increased any further.\n\r");
	    send_to_char(buf, ch);
	    return;
	}
	else if (value > ch->pcdata->quest)
	{
	    send_to_char("You don't have enough quest points.\n\r", ch);
	    return;
	}
	else
		obj->value[1] += value;
	if (obj->value[1] < 1) obj->value[1] = 1;
	send_to_char("Ok.\n\r",ch);
	if (value < 1) value = 1;
	ch->pcdata->quest -= value;
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "max" ) )
    {
	if ( arg3[0] == '\0' )
		{send_to_char("How much max damage?\n\r", ch); return;}
	if (obj->item_type != ITEM_WEAPON)
	{
	    send_to_char("That item is not a weapon.\n\r", ch);
	    return;
	}
	else if (obj->item_type == ITEM_WEAPON && (value+obj->value[2]) > 20)
	{
	    if (obj->value[2] < 20)
	    	sprintf(buf,"The maximum damage can be increased by %d.\n\r",(20 - obj->value[2]));
	    else
	    	sprintf(buf,"The maximum damage cannot be increased any further.\n\r");
	    send_to_char(buf, ch);
	    return;
	}
	else if (value > ch->pcdata->quest)
	{
	    send_to_char("You don't have enough quest points.\n\r", ch);
	    return;
	}
	else
		obj->value[2] += value;
	if (obj->value[2] < 0) obj->value[2] = 0;
	send_to_char("Ok.\n\r",ch);
	if (value < 1) value = 1;
	ch->pcdata->quest -= value;
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "weapon" ) )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not allowed to change weapon types.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_WEAPON)
	{
	    if (IS_SET(obj->quest, QUEST_RELIC))
	    {
		send_to_char("Not on a relic.\n\r",ch);
		return;
	    }
	    if (ch->pcdata->quest < 10)
	    {
	    	send_to_char("You don't have enough quest points.\n\r", ch);
	    	return;
	    }
	    if (arg3[0] == '\0')
	    {
		send_to_char("Please specify weapon type: Slice, Stab, Slash, Whip, Claw, Blast, Pound,\n\rCrush, Pierce, or Suck.\n\r",ch);
		return;
	    }
	    else if (!str_cmp(arg3,"slice" )) value = 1;
	    else if (!str_cmp(arg3,"stab"  )) value = 2;
	    else if (!str_cmp(arg3,"slash" )) value = 3;
	    else if (!str_cmp(arg3,"whip"  )) value = 4;
	    else if (!str_cmp(arg3,"claw"  )) value = 5;
	    else if (!str_cmp(arg3,"blast" )) value = 6;
	    else if (!str_cmp(arg3,"pound" )) value = 7;
	    else if (!str_cmp(arg3,"crush" )) value = 8;
	    else if (!str_cmp(arg3,"pierce")) value = 11;
	    else if (!str_cmp(arg3,"suck"  )) value = 12;
	    else
	    {
		send_to_char("Please specify weapon type: Slice, Stab, Slash, Whip, Claw, Blast, Pound,\n\rCrush, Pierce, or Suck.\n\r",ch);
		return;
	    }
	    if (obj->value[3] == value)
	    {
		send_to_char("It is already that weapon type.\n\r",ch);
		return;
	    }
	    obj->value[3] = value;
	    ch->pcdata->quest -= 10;
	    send_to_char("Ok.\n\r",ch);
	    if (obj->questmaker != NULL) free_string(obj->questmaker);
	    obj->questmaker = str_dup(ch->name);
	}
	else
	{
	    send_to_char("That item is not a weapon.\n\r", ch);
	    return;
	}
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if ( arg3[0] == '\0' )
	{
	    send_to_char("Enter one of: Glow, Hum, Invis, Anti-good, Anti-evil, Anti-neutral, Loyal,\n\rSilver.\n\r",ch);
	    return;
	}
	if      ( !str_cmp( arg3, "glow"         )) {value = ITEM_GLOW; add = 1; remove = 1;}
	else if ( !str_cmp( arg3, "hum"          )) {value = ITEM_HUM; add = 1; remove = 1;}
	else if ( !str_cmp( arg3, "invis"        )) {value = ITEM_INVIS; add = 1; remove = 1;}
	else if ( !str_cmp( arg3, "anti-good"    )) {value = ITEM_ANTI_GOOD; add = 1; remove = 10;}
	else if ( !str_cmp( arg3, "anti-evil"    )) {value = ITEM_ANTI_EVIL; add = 1; remove = 10;}
	else if ( !str_cmp( arg3, "anti-neutral" )) {value = ITEM_ANTI_NEUTRAL; add = 1; remove = 10;}
	else if ( !str_cmp( arg3, "loyal"        )) {value = ITEM_LOYAL; add = 10; remove = 1;}
	else if ( !str_cmp( arg3, "silver"       )) {value = SITEM_SILVER; add = 100; remove = 0;}
	else
	{
	    send_to_char("Enter one of: Glow, Hum, Invis, Anti-good, Anti-evil, Anti-neutral, Loyal,\n\rSilver.\n\r",ch);
	    return;
	}

	if (!str_cmp( arg3, "silver" ))
	{
	    if (IS_SET(obj->spectype, SITEM_SILVER))
	    {
		send_to_char("That item is already silver.\n\r",ch);
		return;
	    }
	    else if (ch->pcdata->quest < add)
	    {
		sprintf(buf,"Sorry, you need %d quest points to set that flag.\n\r",add);
		send_to_char(buf,ch);return;
	    }
	    ch->pcdata->quest -= add;
	    SET_BIT(obj->spectype, SITEM_SILVER);
	}
	else if (IS_SET(obj->extra_flags,value))
	{
	    if (ch->pcdata->quest < remove)
	    {
		sprintf(buf,"Sorry, you need %d quest points to remove that flag.\n\r",remove);
		send_to_char(buf,ch);return;
	    }
	    ch->pcdata->quest -= remove;
	    REMOVE_BIT(obj->extra_flags, value);
	}
	else
	{
	    if (ch->pcdata->quest < add)
	    {
		sprintf(buf,"Sorry, you need %d quest points to set that flag.\n\r",add);
		send_to_char(buf,ch);return;
	    }
	    ch->pcdata->quest -= add;
	    SET_BIT(obj->extra_flags, value);
	}
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not allowed to change object wear locations.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if ( arg3[0] == '\0' )
	{
	    send_to_char("Wear location can be from: Finger, Neck, Body, Head, Legs, Hands, Arms,\n\rAbout, Waist, Wrist, Hold, Face.\n\r",ch);
	    return;
	}
	if      ( !str_cmp( arg3, "finger" )) value = ITEM_WEAR_FINGER;
	else if ( !str_cmp( arg3, "neck"   )) value = ITEM_WEAR_NECK;
	else if ( !str_cmp( arg3, "body"   )) value = ITEM_WEAR_BODY;
	else if ( !str_cmp( arg3, "head"   )) value = ITEM_WEAR_HEAD;
	else if ( !str_cmp( arg3, "legs"   )) value = ITEM_WEAR_LEGS;
	else if ( !str_cmp( arg3, "feet"   )) value = ITEM_WEAR_FEET;
	else if ( !str_cmp( arg3, "hands"  )) value = ITEM_WEAR_HANDS;
	else if ( !str_cmp( arg3, "arms"   )) value = ITEM_WEAR_ARMS;
	else if ( !str_cmp( arg3, "about"  )) value = ITEM_WEAR_ABOUT;
	else if ( !str_cmp( arg3, "waist"  )) value = ITEM_WEAR_WAIST;
	else if ( !str_cmp( arg3, "wrist"  )) value = ITEM_WEAR_WRIST;
	else if ( !str_cmp( arg3, "hold"   )) value = ITEM_WIELD;
	else if ( !str_cmp( arg3, "face"   )) value = ITEM_WEAR_FACE;
	else {
	    send_to_char("Wear location can be from: Finger, Neck, Body, Head, Legs, Hands, Arms, 
About, Waist, Wrist, Hold, Face.\n\r",ch); return;}
	if (IS_SET(obj->wear_flags,ITEM_TAKE) ) value += 1;
	if (obj->wear_flags == value || obj->wear_flags == (value+1) )
	{
	    act("But $p is already worn in that location!",ch,obj,NULL,TO_CHAR);
	    return;
	}
	else if ( (value != ITEM_WIELD && value != (ITEM_WIELD+1) )
		&& obj->item_type == ITEM_WEAPON)
	{
	    act("You can only HOLD a weapon.",ch,obj,NULL,TO_CHAR);
	    return;
	}
	else if (ch->pcdata->quest < 20 && !(obj->pIndexData->vnum == 30037 && obj->wear_flags == 1))
	{
	    send_to_char("It costs 20 quest points to change a location.\n\r", ch);
	    return;
	}
	if (!(obj->pIndexData->vnum == 30037 && obj->wear_flags == 1)) 
	    ch->pcdata->quest -= 20;
	obj->wear_flags = value;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "replacespell" ) )
    {
	int weapon = 0;
	int affect = 0;
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    if (!IS_SET(obj->spectype, SITEM_DEMONIC))
	    {
		send_to_char("Not on a relic.\n\r",ch);
		return;
	    }
	    if (obj->specpower < 1)
	    {
		send_to_char("You are unable to infuse any further powers within this demonic item.\n\r",ch);
		return;
	    }
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if ( arg3[0] == '\0' )
	{
	    send_to_char("Spell weapons: Acid, Dark, Holy, Vampiric, Flaming, Electrified, Poisonous.\n\r",ch);
	    send_to_char("Spell affects: Blind, Seeinvis, Fly, Infravision, Invis, Passdoor, Protection,\n\rSanct, Sneak, Shockshield, Fireshield, Iceshield, Acidshield.\n\r",ch);
	    return;
	}
	if      ( !str_cmp( arg3, "acid"        )) weapon = 1;
	else if ( !str_cmp( arg3, "dark"        )) weapon = 4;
	else if ( !str_cmp( arg3, "holy"        )) weapon = 30;
	else if ( !str_cmp( arg3, "vampiric"    )) weapon = 34;
	else if ( !str_cmp( arg3, "flaming"     )) weapon = 37;
	else if ( !str_cmp( arg3, "electrified" )) weapon = 48;
	else if ( !str_cmp( arg3, "poisonous"   )) weapon = 53;
	else if ( !str_cmp( arg3, "infravision" )) affect = 1;
	else if ( !str_cmp( arg3, "seeinvis"    )) affect = 2;
	else if ( !str_cmp( arg3, "fly"         )) affect = 3;
	else if ( !str_cmp( arg3, "blind"       )) affect = 4;
	else if ( !str_cmp( arg3, "invis"       )) affect = 5;
	else if ( !str_cmp( arg3, "passdoor"    )) affect = 6;
	else if ( !str_cmp( arg3, "protection"  )) affect = 7;
	else if ( !str_cmp( arg3, "sanct"       )) affect = 8;
	else if ( !str_cmp( arg3, "sneak"       )) affect = 9;
	else if ( !str_cmp( arg3, "shockshield" )) affect = 10;
	else if ( !str_cmp( arg3, "fireshield"  )) affect = 11;
	else if ( !str_cmp( arg3, "iceshield"   )) affect = 12;
	else if ( !str_cmp( arg3, "acidshield"  )) affect = 13;
	else
	{
	    send_to_char("Spell weapons: Dark, Holy, Vampiric, Flaming, Electrified, Poisonous.\n\r",ch);
	    send_to_char("Spell affects: Blind, Seeinvis, Fly, Infravision, Invis, Passdoor, Protection,\n\rSanct, Sneak, Shockshield, Fireshield, Iceshield, Acidshield.\n\r",ch);
	    return;
	}

	if (obj->item_type != ITEM_WEAPON && weapon > 0)
	{
	    send_to_char("You can only put that power on a weapon.\n\r", ch);
	    return;
	}
	else if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR && affect > 0)
	{
	    send_to_char("You can only put that power on a weapon or a piece of armour.\n\r", ch);
	    return;
	}
	else if (ch->pcdata->quest < 50 )
	{
	    send_to_char("It costs 50 quest points to create a spell weapon or affect.\n\r", ch);
	    return;
	}
	if (weapon > 0)
	{
	    if (obj->value[0] >= 1000)
		obj->value[0] = ((obj->value[0] / 1000) * 1000);
	    else obj->value[0] = 0;
	    obj->value[0] += weapon;
	}
	else if (affect > 0)
	{
	    if (obj->item_type == ITEM_WEAPON)
	    {
	    	if (obj->value[0] >= 1000)
		    obj->value[0] -= ((obj->value[0] / 1000) * 1000);
	    	obj->value[0] += (affect*1000);
	    }
	    else if (obj->item_type == ITEM_ARMOR)
	    	obj->value[3] = affect;
	}
	ch->pcdata->quest -= 50;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "spell" ) )
    {
	int weapon = 0;
	int affect = 0;
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    if (!IS_SET(obj->spectype, SITEM_DEMONIC))
	    {
		send_to_char("Not on a relic.\n\r",ch);
		return;
	    }
	    if (obj->specpower < 1)
	    {
		send_to_char("You are unable to infuse any further powers within this demonic item.\n\r",ch);
		return;
	    }
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if ( arg3[0] == '\0' )
	{
	    send_to_char("Spell weapons: Acid, Dark, Holy, Vampiric, Flaming, Electrified, Poisonous.\n\r",ch);
	    send_to_char("Spell affects: Blind, Seeinvis, Fly, Infravision, Invis, Passdoor, Protection,\n\rSanct, Sneak, Shockshield, Fireshield, Iceshield, Acidshield.\n\r",ch);
	    return;
	}
	if      ( !str_cmp( arg3, "acid"        )) weapon = 1;
	else if ( !str_cmp( arg3, "dark"        )) weapon = 4;
	else if ( !str_cmp( arg3, "holy"        )) weapon = 30;
	else if ( !str_cmp( arg3, "vampiric"    )) weapon = 34;
	else if ( !str_cmp( arg3, "flaming"     )) weapon = 37;
	else if ( !str_cmp( arg3, "electrified" )) weapon = 48;
	else if ( !str_cmp( arg3, "poisonous"   )) weapon = 53;
	else if ( !str_cmp( arg3, "infravision" )) affect = 1;
	else if ( !str_cmp( arg3, "seeinvis"    )) affect = 2;
	else if ( !str_cmp( arg3, "fly"         )) affect = 3;
	else if ( !str_cmp( arg3, "blind"       )) affect = 4;
	else if ( !str_cmp( arg3, "invis"       )) affect = 5;
	else if ( !str_cmp( arg3, "passdoor"    )) affect = 6;
	else if ( !str_cmp( arg3, "protection"  )) affect = 7;
	else if ( !str_cmp( arg3, "sanct"       )) affect = 8;
	else if ( !str_cmp( arg3, "sneak"       )) affect = 9;
	else if ( !str_cmp( arg3, "shockshield" )) affect = 10;
	else if ( !str_cmp( arg3, "fireshield"  )) affect = 11;
	else if ( !str_cmp( arg3, "iceshield"   )) affect = 12;
	else if ( !str_cmp( arg3, "acidshield"  )) affect = 13;
	else
	{
	    send_to_char("Spell weapons: Dark, Holy, Vampiric, Flaming, Electrified, Poisonous.\n\r",ch);
	    send_to_char("Spell affects: Blind, Seeinvis, Fly, Infravision, Invis, Passdoor, Protection,\n\rSanct, Sneak, Shockshield, Fireshield, Iceshield, Acidshield.\n\r",ch);
	    return;
	}

	if (obj->item_type != ITEM_WEAPON && weapon > 0)
	{
	    send_to_char("You can only put that power on a weapon.\n\r", ch);
	    return;
	}
	else if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR && affect > 0)
	{
	    send_to_char("You can only put that power on a weapon or a piece of armour.\n\r", ch);
	    return;
	}
	else if (ch->pcdata->quest < 50 )
	{
	    send_to_char("It costs 50 quest points to create a spell weapon or affect.\n\r", ch);
	    return;
	}
	if (weapon > 0)
	{
	    if ( obj->value[0] - ((obj->value[0] / 1000) * 1000) != 0 )
	    {
		send_to_char("That item already has a spell weapon power.  If you wish to replace the \n\rcurrent spell power, use the format: quest <object> replacespell <spell>.\n\r",ch);
		return;
	    }
	    if (obj->value[0] >= 1000)
		obj->value[0] = ((obj->value[0] / 1000) * 1000);
	    else obj->value[0] = 0;
	    obj->value[0] += weapon;
	}
	else if (affect > 0)
	{
	    if (obj->item_type == ITEM_WEAPON)
	    {
	    	if ( obj->value[0] >= 1000 )
	    	{
		    send_to_char("That item already has a spell affect power.  If you wish to replace the \n\rcurrent spell power, use the format: quest <object> replacespell <spell>.\n\r",ch);
		    return;
	    	}
	    	if (obj->value[0] >= 1000)
		    obj->value[0] -= ((obj->value[0] / 1000) * 1000);
	    	obj->value[0] += (affect*1000);
	    }
	    else if (obj->item_type == ITEM_ARMOR)
	    {
	    	if ( obj->value[3] > 0 )
	    	{
		    send_to_char("That item already has a spell affect power.  If you wish to replace the \n\rcurrent spell power, use the format: quest <object> replacespell <spell>.\n\r",ch);
		    return;
	    	}
	    	obj->value[3] = affect;
	    }
	}
	ch->pcdata->quest -= 50;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "power" ) )
    {
	if ( arg3[0] == '\0' )
	{
	    send_to_char("Please specify the amount of power.\n\r",ch);
	    return;
	}
	if (obj->item_type != ITEM_WEAPON)
	{
	    send_to_char("Only weapons have a spell power.\n\r",ch);
	    return;
	}
	else if (obj->level >= 50)
	{
	    send_to_char("This weapon can hold no more spell power.\n\r",ch);
	    return;
	}
	else if (value + obj->level > 50)
	{
	    sprintf(buf,"You can only add %d more spell power to this weapon.\n\r",(50 - obj->level));
	    send_to_char(buf,ch);
	    return;
	}
	else if (value > ch->pcdata->quest)
	{
	    send_to_char("You don't have enough quest points to increase the spell power.\n\r",ch);
	    return;
	}
	obj->level += value;
	if (obj->level < 0) obj->level = 0;
	if (value < 1) value = 1;
	ch->pcdata->quest -= value;
	send_to_char("Ok.\n\r",ch);
	if (obj->questmaker != NULL) free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
    {
	if (obj->weight < 2)
	{
	    send_to_char("You cannot reduce the weight of this item any further.\n\r",ch);
	    return;
	}
	else if (ch->pcdata->quest < 10)
	{
	    send_to_char("It costs 10 quest point to remove the weight of an object.\n\r",ch);
	    return;
	}
	obj->weight = 1;
	ch->pcdata->quest -= 10;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "transporter" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (IS_SET(obj->spectype, SITEM_TELEPORTER))
	{ send_to_char("This item is already a transporter.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_TRANSPORTER))
	{ send_to_char("This item is already a teleporter.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_SPELL))
	{ send_to_char("This item is already a spell caster.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_OBJECT))
	{ send_to_char("This item is already an object creator.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_MOBILE))
	{ send_to_char("This item is already a creature creator.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_ACTION))
	{ send_to_char("This item is already a commanding device.\n\r",ch);
	  return;}
	else if (ch->pcdata->quest < 50)
	{
	    send_to_char("It costs 50 quest point to create a transporter.\n\r",ch);
	    return;
	}
	SET_BIT(obj->spectype, SITEM_ACTIVATE);
	SET_BIT(obj->spectype, SITEM_TELEPORTER);
	obj->specpower = ch->in_room->vnum;
	ch->pcdata->quest -= 50;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
    	if (obj->chpoweron != NULL) free_string(obj->chpoweron);
    	obj->chpoweron = str_dup("You transform into a fine mist and seep into the ground.");
    	if (obj->victpoweron != NULL) free_string(obj->victpoweron);
    	obj->victpoweron = str_dup("$n transforms into a fine mist and seeps into the ground.");
    	if (obj->chpoweroff != NULL) free_string(obj->chpoweroff);
    	obj->chpoweroff = str_dup("You seep up from the ground and reform your body.");
    	if (obj->victpoweroff != NULL) free_string(obj->victpoweroff);
    	obj->victpoweroff = str_dup("A fine mist seeps up from the ground and reforms into $n.");
    	if (obj->chpoweruse != NULL) free_string(obj->chpoweruse);
    	obj->chpoweruse = str_dup("You activate $p.");
    	if (obj->victpoweruse != NULL) free_string(obj->victpoweruse);
    	obj->victpoweruse = str_dup("$n activates $p.");
	return;
    }

    if ( !str_cmp( arg2, "retransporter" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (!IS_SET(obj->spectype, SITEM_TELEPORTER))
	{ send_to_char("This item is not a transporter.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_TRANSPORTER))
	{ send_to_char("This item is already a teleporter.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_SPELL))
	{ send_to_char("This item is already a spell caster.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_OBJECT))
	{ send_to_char("This item is already an object creator.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_MOBILE))
	{ send_to_char("This item is already a creature creator.\n\r",ch);
	  return;}
    	else if (IS_SET(obj->spectype, SITEM_ACTION))
	{ send_to_char("This item is already a commanding device.\n\r",ch);
	  return;}
	else if (ch->pcdata->quest < 50)
	{
	    send_to_char("It costs 50 quest point to create a transporter.\n\r",ch);
	    return;
	}
	SET_BIT(obj->spectype, SITEM_ACTIVATE);
	SET_BIT(obj->spectype, SITEM_TELEPORTER);
	obj->specpower = ch->in_room->vnum;
	ch->pcdata->quest -= 50;
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "clone" ) )
    {
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
        if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	value = obj->points;
	if (value < 5 || obj->item_type == ITEM_PAGE) value = 5;
	if (obj->item_type == ITEM_QUEST || obj->item_type == ITEM_QUESTCARD)
	{
	    send_to_char("I think not...\n\r",ch);
	    return;
	}
	else if (obj->questowner == NULL || str_cmp(obj->questowner,ch->name))
	{
	    send_to_char("You can only clone something you own.\n\r",ch);
	    return;
	}
	else if (ch->pcdata->quest < value)
	{
	    sprintf(buf,"It will cost you %d quest points to clone %s.\n\r",
		value,obj->short_descr);
	    send_to_char(buf,ch);
	    return;
	}
	ch->pcdata->quest -= value;
	quest_clone(ch,obj);
	return;
    }

    if ( arg3[0] == '\0' )
    {
	do_quest(ch,"");
	return;
    }

    if (obj->item_type != ITEM_BOOK)
    {
    	if      ( !str_cmp( arg2, "hitroll" ) || !str_cmp( arg2, "hit" ) )
	    {oset_affect(ch,obj,value,APPLY_HITROLL,TRUE);return;}
    	else if ( !str_cmp( arg2, "damroll" ) || !str_cmp( arg2, "dam" ) )
	    {oset_affect(ch,obj,value,APPLY_DAMROLL,TRUE);return;}
    	else if ( !str_cmp( arg2, "armor" ) || !str_cmp( arg2, "ac" ) )
	    {oset_affect(ch,obj,value,APPLY_AC,TRUE);return;}
    	else if ( !str_cmp( arg2, "hitpoints" ) || !str_cmp( arg2, "hp" ) )
	    {oset_affect(ch,obj,value,APPLY_HIT,TRUE);return;}
    	else if ( !str_cmp( arg2, "mana" ) )
	    {oset_affect(ch,obj,value,APPLY_MANA,TRUE);return;}
    	else if ( !str_cmp( arg2, "move" ) || !str_cmp( arg2, "movement" ) )
	    {oset_affect(ch,obj,value,APPLY_MOVE,TRUE);return;}
    	else if ( !str_cmp( arg2, "str" ) || !str_cmp( arg2, "strength" ) )
	    {oset_affect(ch,obj,value,APPLY_STR,TRUE);return;}
    	else if ( !str_cmp( arg2, "dex" ) || !str_cmp( arg2, "dexterity" ) )
	    {oset_affect(ch,obj,value,APPLY_DEX,TRUE);return;}
    	else if ( !str_cmp( arg2, "int" ) || !str_cmp( arg2, "intelligence" ) )
	    {oset_affect(ch,obj,value,APPLY_INT,TRUE);return;}
    	else if ( !str_cmp( arg2, "wis" ) || !str_cmp( arg2, "wisdom" ) )
	    {oset_affect(ch,obj,value,APPLY_WIS,TRUE);return;}
    	else if ( !str_cmp( arg2, "con" ) || !str_cmp( arg2, "constitution" ) )
	    {oset_affect(ch,obj,value,APPLY_CON,TRUE);return;}
    }

    if ( !str_cmp( arg2, "rename" ) )
    {
	value = 1;
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not allowed to rename objects.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest,QUEST_FREENAME))
	{
	    value = 0;
	    REMOVE_BIT(obj->quest,QUEST_FREENAME);
	}
	if (ch->pcdata->quest < value)
	{
	    send_to_char("It costs 1 quest point to rename an object.\n\r",ch);
	    return;
	}
	if (strlen(arg3) < 5)
	{
	    send_to_char("Name should be at least 5 characters long.\n\r",ch);
	    return;
	}
	ch->pcdata->quest -= value;
	strcpy(arg3,capitalize(arg3));
	arg3[0] = LOWER(arg3[0]);
	endchar[0] = (arg3[strlen(arg3)-1]);
	endchar[1] = '\0';
	if (!str_cmp(endchar,".")) arg3[strlen(arg3)-1] = '\0';
	free_string( obj->name );
	obj->name = str_dup( arg3 );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	if (is_in(arg3,"|pair of*"))
	    sprintf(buf, "%s are lying here.",arg3);
	else
	    sprintf(buf, "%s is lying here.",arg3);
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
	value = 1;
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not allowed to rename objects.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (!IS_SET(obj->quest,QUEST_NAME) && 
	    (IS_SET(obj->quest,QUEST_SHORT) || IS_SET(obj->quest,QUEST_LONG)))
	{
	    SET_BIT(obj->quest,QUEST_NAME);
	    value = 0;
	}
	else if (IS_SET(obj->quest,QUEST_NAME))
	{
	    REMOVE_BIT(obj->quest,QUEST_SHORT);
	    REMOVE_BIT(obj->quest,QUEST_LONG);
	}
	else
	    SET_BIT(obj->quest,QUEST_NAME);
	if (IS_SET(obj->quest,QUEST_FREENAME))
	{
	    value = 0;
	    REMOVE_BIT(obj->quest,QUEST_FREENAME);
	}
	if (ch->pcdata->quest < value)
	{
	    send_to_char("It costs 1 quest point to rename an object.\n\r",ch);
	    return;
	}
	if (strlen(arg3) < 3)
	{
	    send_to_char("Keywords should be at least 3 characters long.\n\r",ch);
	    return;
	}
	ch->pcdata->quest -= value;
	strcpy(arg3,capitalize(arg3));
	arg3[0] = LOWER(arg3[0]);
	endchar[0] = (arg3[strlen(arg3)-1]);
	endchar[1] = '\0';
	if (!str_cmp(endchar,".")) arg3[strlen(arg3)-1] = '\0';
	strcpy(buf,obj->name);
	if (strlen(buf) > 80)
	{
	    send_to_char("You cannot fit any more keywords on this object.\n\r",ch);
	    return;
	}
	strcat(buf," ");
	strcat(buf,arg3);
	free_string( obj->name );
	obj->name = str_dup( buf );
	send_to_char("Ok.\n\r",ch);
    	if (obj->questmaker != NULL) free_string(obj->questmaker);
    	obj->questmaker = str_dup(ch->name);
	return;
    }

    if ( !str_cmp( arg2, "name" ) || !str_cmp( arg2, "short" ) || 
	!str_cmp( arg2, "long" ))
    {
	send_to_char("If you wish to rename an item, please use the format:\n\r",ch);
	send_to_char("quest <object> rename <short description>\n\r",ch);
	send_to_char("The short description being the name you see when you are wearing it.\n\r",ch);
	send_to_char("Example: quest sword rename a black longsword\n\r",ch);
	return;
    }

    if (!str_cmp(arg2,"special"))
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if (arg3[0] == '\0')
	{
	    send_to_char("Please enter ACTIVATE, TWIST, PRESS or PULL.\n\r",ch);
	    return;
	}
	if (str_cmp(arg3,"activate") && str_cmp(arg3,"twist") &&
	    str_cmp(arg3,"press"   ) && str_cmp(arg3,"pull" ))
	{
	    send_to_char("Please enter ACTIVATE, TWIST, PRESS or PULL.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->spectype, SITEM_ACTIVATE))
	    REMOVE_BIT(obj->spectype, SITEM_ACTIVATE);
	if (IS_SET(obj->spectype, SITEM_TWIST))
	    REMOVE_BIT(obj->spectype, SITEM_TWIST);
	if (IS_SET(obj->spectype, SITEM_PRESS))
	    REMOVE_BIT(obj->spectype, SITEM_PRESS);
	if (IS_SET(obj->spectype, SITEM_PULL))
	    REMOVE_BIT(obj->spectype, SITEM_PULL);
	if (!str_cmp(arg3,"activate")) SET_BIT(obj->spectype, SITEM_ACTIVATE);
	if (!str_cmp(arg3,"twist"   )) SET_BIT(obj->spectype, SITEM_TWIST);
	if (!str_cmp(arg3,"press"   )) SET_BIT(obj->spectype, SITEM_PRESS);
	if (!str_cmp(arg3,"pull"    )) SET_BIT(obj->spectype, SITEM_PULL);
	arg3[0] = UPPER(arg3[0]);
	sprintf(buf,"%s flag set.\n\r",arg3);
	send_to_char(buf,ch);
	return;
    }

    if (!str_cmp(arg2,"you-out") || !str_cmp(arg2,"you-wear"))
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"you-out") && !IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("That item is not a transporter.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"you-wear") && IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("Not on a transporter.\n\r",ch);
	    return;
	}
	if (obj->chpoweron != NULL) strcpy(buf,obj->chpoweron);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweron );
	    obj->chpoweron = str_dup( "(null)" );
	}
	else if (obj->chpoweron != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweron );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweron = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweron );
	    obj->chpoweron = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg2,"other-out") || !str_cmp(arg2,"other-wear") )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"other-out") && !IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("That item is not a transporter.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"other-wear") && IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("Not on a transporter.\n\r",ch);
	    return;
	}
	if (obj->victpoweron != NULL) strcpy(buf,obj->victpoweron);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweron );
	    obj->victpoweron = str_dup( "(null)" );
	}
	else if (obj->victpoweron != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweron );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweron = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweron );
	    obj->victpoweron = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg2,"you-in") || !str_cmp(arg2,"you-remove") )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"you-in") && !IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("That item is not a transporter.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"you-remove") && IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("Not on a transporter.\n\r",ch);
	    return;
	}
	if (obj->chpoweroff != NULL) strcpy(buf,obj->chpoweroff);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweroff );
	    obj->chpoweroff = str_dup( "(null)" );
	}
	else if (obj->chpoweroff != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweroff );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweroff = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweroff );
	    obj->chpoweroff = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg2,"other-in") || !str_cmp(arg2,"other-remove") )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"other-in") && !IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("That item is not a transporter.\n\r",ch);
	    return;
	}
    	if (!str_cmp(arg2,"other-remove") && IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    send_to_char("Not on a transporter.\n\r",ch);
	    return;
	}
	if (obj->victpoweroff != NULL) strcpy(buf,obj->victpoweroff);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweroff );
	    obj->victpoweroff = str_dup( "(null)" );
	}
	else if (obj->victpoweroff != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweroff );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweroff = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweroff );
	    obj->victpoweroff = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg2,"you-use") )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if (obj->chpoweruse != NULL) strcpy(buf,obj->chpoweruse);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->chpoweruse );
	    obj->chpoweruse = str_dup( "(null)" );
	}
	else if (obj->chpoweruse != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->chpoweruse );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->chpoweruse = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->chpoweruse );
	    obj->chpoweruse = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg2,"other-use") )
    {
	if (!IS_EXTRA(ch,EXTRA_TRUSTED))
	{
	    send_to_char("You are not permitted to change an object in this way.\n\r",ch);
	    return;
	}
	if (IS_SET(obj->quest, QUEST_RELIC))
	{
	    send_to_char("Not on a relic.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_BOOK)
	{
	    send_to_char("Not on a book.\n\r",ch);
	    return;
	}
	if (obj->victpoweruse != NULL) strcpy(buf,obj->victpoweruse);
	else {send_to_char("There is a problem with this item, please inform an implementor.\n\r",ch);return;}
	if (!str_cmp(arg3,"clear"))
	{
	    free_string( obj->victpoweruse );
	    obj->victpoweruse = str_dup( "(null)" );
	}
	else if (obj->victpoweruse != NULL && buf[0] != '\0' && str_cmp(buf,"(null)"))
	{
	    if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	    {
		send_to_char("Line too long.\n\r",ch);
		return;
	    }
	    else
	    {
	    	free_string( obj->victpoweruse );
	    	strcat( buf, "\n\r" );
	    	strcat( buf, arg3 );
	    	obj->victpoweruse = str_dup( buf );
	    }
	}
	else
	{
	    free_string( obj->victpoweruse );
	    obj->victpoweruse = str_dup( arg3 );
	}
	send_to_char("Ok.\n\r",ch);
	return;
    }
    send_to_char("No such option.\n\r",ch);
    return;
}

void quest_clone( CHAR_DATA *ch, OBJ_DATA *obj )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj2;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;

    pObjIndex = get_obj_index( obj->pIndexData->vnum);
    obj2 = create_object( pObjIndex, obj->level );
    /* Copy any changed parts of the object. */
    free_string(obj2->name);
    obj2->name=str_dup(obj->name);
    free_string(obj2->short_descr);
    obj2->short_descr=str_dup(obj->short_descr);
    free_string(obj2->description);
    obj2->description=str_dup(obj->description);
    if (obj2->questmaker != NULL) free_string(obj2->questmaker);
	obj2->questmaker=str_dup(ch->name);
    if (obj->chpoweron != NULL && obj->points >= 50)
    {
    	free_string(obj2->chpoweron);
    	obj2->chpoweron=str_dup(obj->chpoweron);
    }
    if (obj->chpoweroff != NULL && obj->points >= 50)
    {
    	free_string(obj2->chpoweroff);
    	obj2->chpoweroff=str_dup(obj->chpoweroff);
    }
    if (obj->chpoweruse != NULL && obj->points >= 50)
    {
    	free_string(obj2->chpoweruse);
    	obj2->chpoweruse=str_dup(obj->chpoweruse);
    }
    if (obj->victpoweron != NULL && obj->points >= 50)
    {
    	free_string(obj2->victpoweron);
    	obj2->victpoweron=str_dup(obj->victpoweron);
    }
    if (obj->victpoweroff != NULL && obj->points >= 50)
    {
    	free_string(obj2->victpoweroff);
    	obj2->victpoweroff=str_dup(obj->victpoweroff);
    }
    if (obj->victpoweruse != NULL && obj->points >= 50)
    {
    	free_string(obj2->victpoweruse);
    	obj2->victpoweruse=str_dup(obj->victpoweruse);
    }
    if (obj->points >= 50)
    {
	obj2->spectype	 = obj->spectype;
	obj2->specpower	 = obj->specpower;
    }
    obj2->item_type	 = obj->item_type;
    obj2->extra_flags	 = obj->extra_flags;
    obj2->wear_flags	 = obj->wear_flags;
    obj2->weight	 = obj->weight;
    obj2->condition	 = obj->condition;
    obj2->toughness	 = obj->toughness;
    obj2->resistance	 = obj->resistance;
    obj2->quest		 = obj->quest;
    obj2->points	 = obj->points;
    obj2->cost		 = obj->cost;
    obj2->value[0]	 = obj->value[0];
    obj2->value[1]	 = obj->value[1];
    obj2->value[2]	 = obj->value[2];
    obj2->value[3]	 = obj->value[3];
    /*****************************************/
    obj_to_char(obj2,ch);
    if (obj->affected != NULL)
    {
    	for ( paf = obj->affected; paf != NULL; paf = paf->next )
    	{
	    if (affect_free == NULL)
	        paf2 = alloc_perm( sizeof(*paf2) );
	    else
	    {
		paf2 = affect_free;
		affect_free = affect_free->next;
	    }
	    paf2->type  	= 0;
	    paf2->duration	= paf->duration;
	    paf2->location	= paf->location;
	    paf2->modifier	= paf->modifier;
	    paf2->bitvector	= 0;
	    paf2->next  	= obj2->affected;
	    obj2->affected	= paf2;
    	}
    }
    act( "$p splits into two identical objects.", ch, obj, NULL, TO_CHAR );
    return;
}

void oset_affect( CHAR_DATA *ch, OBJ_DATA *obj, int value, int affect, bool is_quest )
{
    char buf [MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int quest;
    int range;
    int cost;
    int max;

    if (IS_NPC(ch))
    {
	send_to_char("Switch back, smart ass.\n\r", ch);
	return;
    }

    if (value == 0)
    {
	send_to_char("Please enter a positive or negative amount.\n\r", ch);
	return;
    }

    if (!IS_JUDGE(ch) && (obj->questowner == NULL))
    {
	send_to_char("First you must set the owners name on the object.\n\r", ch);
	return;
    }

    if (!IS_JUDGE(ch) && (obj->questmaker == NULL ||
	str_cmp(ch->name,obj->questmaker)) && !is_quest )
    {
	send_to_char("That item has already been oset by someone else.\n\r", ch);
	return;
    }

    if      ( affect == APPLY_STR     ) {range = 3;  cost = 20 ; quest = QUEST_STR;}
    else if ( affect == APPLY_DEX     ) {range = 3;  cost = 20 ; quest = QUEST_DEX;}
    else if ( affect == APPLY_INT     ) {range = 3;  cost = 20 ; quest = QUEST_INT;}
    else if ( affect == APPLY_WIS     ) {range = 3;  cost = 20 ; quest = QUEST_WIS;}
    else if ( affect == APPLY_CON     ) {range = 3;  cost = 20 ; quest = QUEST_CON;}
    else if ( affect == APPLY_HIT     ) {range = 25; cost =  5 ; quest = QUEST_HIT;}
    else if ( affect == APPLY_MANA    ) {range = 25; cost =  5 ; quest = QUEST_MANA;}
    else if ( affect == APPLY_MOVE    ) {range = 25; cost =  5 ; quest = QUEST_MOVE;}
    else if ( affect == APPLY_HITROLL ) {range = 5;  cost = 30 ; quest = QUEST_HITROLL;}
    else if ( affect == APPLY_DAMROLL ) {range = 5;  cost = 30 ; quest = QUEST_DAMROLL;}
    else if ( affect == APPLY_AC      ) {range = 25; cost = 10 ; quest = QUEST_AC;}
    else return;

    if (IS_SET(obj->quest, QUEST_IMPROVED))
	max = 1250 + (ch->race * 20);
    else if ( obj->pIndexData->vnum == OBJ_VNUM_PROTOPLASM )
	{range *= 2; max = 750 + (ch->race * 10);}
    else
	max = 400 + (ch->race * 10);
    if (obj->item_type == ITEM_WEAPON) {max *= 2;range *= 2;}
    if (IS_SET(obj->spectype, SITEM_DEMONIC))
    {
	if (obj->specpower == ARM_GREY  ) max += 100;
	if (obj->specpower == ARM_PURPLE) max += 250;
	if (obj->specpower == ARM_RED   ) max += 750;
	if (obj->specpower == ARM_BRASS ) max += 1500;
    }
    if ( !IS_JUDGE(ch) && ((value > 0 && value > range) ||
	(value < 0 && value < (range-range-range))) )
    {
	send_to_char("That is not within the acceptable range...\n\r", ch);
	send_to_char("Str, Dex, Int, Wis, Con... max =   3 each, at  20 quest points per +1 stat.\n\r", ch);
	send_to_char("Hp, Mana, Move............ max =  25 each, at   5 quest point per point.\n\r", ch);
	send_to_char("Hitroll, Damroll.......... max =   5 each, at  30 quest points per point.\n\r", ch);
	send_to_char("Ac........................ max = -25,      at  10 points per point.\n\r", ch);
	send_to_char("\n\rNote: Created items can have upto 2 times the above maximum.\n\r", ch);
	send_to_char("Also: Weapons may have upto 2 (4 for created) times the above maximum.\n\r", ch);
	return;
    }

    if ( quest >= QUEST_HIT && value < 0 ) cost *= (value - (value*2));
    else cost *= value;

    if ( cost < 0 ) cost = 0;

    if (!IS_JUDGE(ch) && IS_SET(obj->quest, quest) )
    {
	send_to_char("That affect has already been set on this object.\n\r", ch);
	return;
    }

    if (!IS_JUDGE(ch) && (obj->points + cost > max))
    {
	sprintf(buf,"You are limited to %d quest points per item.\n\r",max);
	send_to_char(buf, ch);
	return;
    }

    if (is_quest && ch->pcdata->quest < cost)
    {
	sprintf(buf,"That costs %d quest points, while you only have %d.\n\r",cost, ch->pcdata->quest);
	send_to_char(buf, ch);
	return;
    }

    if (!IS_SET(obj->quest, quest) ) SET_BIT(obj->quest,quest);

    if (is_quest) ch->pcdata->quest -= cost;
    obj->points += cost;
    if (obj->questmaker != NULL) free_string(obj->questmaker);
    obj->questmaker = str_dup(ch->name);

    if ( affect_free == NULL )
    {
	paf		= alloc_perm( sizeof(*paf) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= -1;
    paf->duration	= -1;
    paf->location	= affect;
    paf->modifier	= value;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    send_to_char("Ok.\n\r",ch);
    return;
}

void do_clist( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       buf[MAX_STRING_LENGTH];
    
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if (IS_NPC(wch)) continue;
	if (wch->pcdata->coterie == NULL) continue;
	if (wch->pcdata->leader == NULL) continue;

	if (strlen(wch->pcdata->coterie) < 2 || 
	    strlen(wch->pcdata->leader) < 2)
	{
	    sprintf(buf,"[%s] No coterie.\n\r",wch->name);
	    send_to_char(buf,ch);
	}
	else
	{
	    sprintf(buf,"[%s] The %s coterie, lead by %s.\n\r", wch->name, 
		wch->pcdata->coterie, wch->pcdata->leader);
	    send_to_char(buf,ch);
	}
    }
    return;
}
