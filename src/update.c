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
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );



void gain_exp( CHAR_DATA *ch, int gain )
{
    CHAR_DATA *mount = NULL;
    CHAR_DATA *master = NULL;

    if ( IS_NPC(ch) && (mount = ch->mount) != NULL && !IS_NPC(mount))
    {
	if ( (master = ch->master) == NULL || master != mount )
	{
	    mount->exp += gain;
	}
    }
    if ( !IS_NPC(ch) && !IS_IMMORTAL(ch) )
    {
	ch->exp += gain;
    }
    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int conamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_VAMPIRE(ch)) return 0;
	gain = number_range( 10, 20 );

	if ((conamount = (get_curr_con(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING: gain *= conamount * 0.5;	break;
		case POS_SLEEPING:   gain *= conamount;		break;
		case POS_RESTING:    gain *= conamount * 0.5;	break;
	    }
	}

	if ( ch->pcdata->condition[COND_FULL]   == 0 && !IS_HERO(ch) )
	    gain *= 0.5;

	if ( ch->pcdata->condition[COND_THIRST] == 0 && !IS_HERO(ch) )
	    gain *= 0.5;

    }

    if ( IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain *= 0.25;

    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int intamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_VAMPIRE(ch)) return 0;
	gain = number_range( 10, 20 );

	if ((intamount = (get_curr_int(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING: gain *= intamount * ch->level;	break;
		case POS_SLEEPING:   gain *= intamount;			break;
		case POS_RESTING:    gain *= intamount * 0.5;		break;
	    }
	}

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain *= 0.5;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain *= 0.25;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;
    int dexamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	if (IS_VAMPIRE(ch)) return 0;
	gain = number_range( 10, 20 );

	if ((dexamount = (get_curr_dex(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_MEDITATING: gain *= dexamount * 0.5;	break;
		case POS_SLEEPING:   gain *= dexamount;		break;
		case POS_RESTING:    gain *= dexamount * 0.5;	break;
	    }
	}

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain *= 0.5;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain *= 0.25;

    return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) )
	return;

    if (!IS_NPC(ch) && IS_HERO(ch) && !IS_VAMPIRE(ch) 
	&& iCond != COND_DRUNK)
	return;

    condition				= ch->pcdata->condition[iCond];
    if (!IS_NPC(ch) && !IS_VAMPIRE(ch) )
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );
    else
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 100 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_VAMPIRE(ch))
	    {
		send_to_char( "You are REALLY hungry.\n\r",  ch );
		act( "You hear $n's stomach rumbling.", ch, NULL, NULL, TO_ROOM );
	    }
	    break;

	case COND_THIRST:
	    if (!IS_VAMPIRE(ch)) 
		send_to_char( "You are REALLY thirsty.\n\r", ch );
/*
	    else if (ch->hit > 0)
	    {
		send_to_char( "You are DYING from lack of blood!\n\r", ch );
		act( "$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		ch->hit -= number_range(2,5);
		if (number_percent() <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
*/
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }
    else if ( ch->pcdata->condition[iCond] < 10 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_VAMPIRE(ch)) 
		send_to_char( "You feel hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    if (!IS_VAMPIRE(ch)) 
		send_to_char( "You feel thirsty.\n\r", ch );
/*
	    else
	    {
		send_to_char( "You crave blood.\n\r", ch );
		if (number_range(1,1000) <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (number_percent() > (ch->pcdata->condition[COND_THIRST]+75)
		    && !IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
*/
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    OBJ_DATA *stake;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( ch->in_room == NULL ) continue;
/*
	if ( ch->hunting != NULL && ch->hunting != '\0' && 
	    strlen(ch->hunting) > 1 )
	{
	    check_hunt( ch );
	    continue;
	}
*/
	if ( ch->sat != NULL )
	{
	    if (!CAN_SIT(ch) || ch->in_room == NULL || 
		ch->sat->in_room == NULL || ch->sat->in_room != ch->in_room)
	    {
		ch->sat->sat = NULL;
		ch->sat = NULL;
	    }
	}
	if (IS_EXTRA(ch, EXTRA_DYING))
	{
	    act("$n falls to the ground and dies from $s wounds.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("You fall to the ground and die from your wounds.\n\r",ch);
	    ch->form = 16;
	    raw_kill(ch,ch);
	    continue;
	}
	if ( !IS_NPC(ch) )
	{
	    if (ch->desc == NULL || !IS_PLAYING(ch->desc)) continue;
	    if (IS_EXTRA(ch, EXTRA_OSWITCH) && IS_MORE(ch, MORE_STAKED) && ( stake = get_eq_char( ch, WEAR_STAKE ) ) == NULL)
	    {
		OBJ_DATA *obj;

		REMOVE_BIT(ch->more, MORE_STAKED);
		ch->pcdata->obj_vnum = 0;
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
		free_string(ch->morph);
		ch->morph = str_dup("");
		send_to_char( "You recover from torpor.\n\r", ch );
		if ( ( obj = ch->pcdata->chobj ) != NULL )
		{
		    obj->chobj = NULL;
		    if (obj->carried_by != NULL)
		    {
			if (ch != obj->carried_by)
			{
			    act("You drop $N onto the floor.", obj->carried_by, NULL, ch, TO_CHAR);
			    act("$n drops $N onto the floor.", obj->carried_by, NULL, ch, TO_NOTVICT);
			    act("$n drops you onto the floor.", obj->carried_by, NULL, ch, TO_VICT);
			}
		    }
		}
		ch->pcdata->chobj = NULL;
		act("$n clambers to $s feet.",ch,NULL,NULL,TO_ROOM);
		act("You clamber back to your feet.",ch,NULL,NULL,TO_CHAR);
		extract_obj(obj);
		if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
		    char_from_room(ch);
		    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
		}
	    }
	    if (ch->position < POS_SLEEPING && ch->pcdata->wolf > 0)
	    {
		ch->pcdata->wolf = 0;
		if (IS_WEREWOLF(ch)) do_unwerewolf(ch,"");
		if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT) && !IS_POLYAFF(ch, POLY_ZULO)) 
		    do_nightsight(ch,"");
		if (IS_VAMPAFF(ch, VAM_FANGS) && !IS_POLYAFF(ch, POLY_ZULO)) 
		    do_fangs(ch,"");
		if (IS_VAMPAFF(ch, VAM_CLAWS) && !IS_POLYAFF(ch, POLY_ZULO)) 
		    do_claws(ch,"");
		send_to_char("You feel your beast release its hold.\n\r",ch);
	    }
	    else if (ch->pcdata->song > 0)
	    {
		OBJ_DATA *obj;
		if ( ( obj = ch->pcdata->memorised ) == NULL )
		{
		    ch->pcdata->memorised = NULL;
		    ch->pcdata->song = 0;
		}
		else sing_song(ch,obj);
	    }
	    else if (ch->pcdata->condition[COND_DRUNK] > 10 && number_range(1,10) == 1)
	    {
		send_to_char("You hiccup loudly.\n\r",ch);
		act("$n hiccups.",ch,NULL,NULL,TO_ROOM);
	    }
	    if (ch->embraced == ARE_EMBRACING)
	    {
		CHAR_DATA *vch;
		int amount = number_range(5,15);
		if (ch->position < POS_STANDING)
		{
		    if ( ( vch = ch->embrace ) != NULL )
		    {
			vch->embrace = NULL;
			vch->embraced = ARE_NONE;
		    }
		    ch->embrace = NULL;
		    ch->embraced = ARE_NONE;
		}
		else if ( ( vch = ch->embrace ) == NULL )
		    ch->embraced = ARE_NONE;
		else if (vch->position == POS_FIGHTING)
		{
		    vch->embrace = NULL;
		    vch->embraced = ARE_NONE;
		    ch->embrace = NULL;
		    ch->embraced = ARE_NONE;
		}
		else if (vch->in_room == NULL || ch->in_room == NULL || 
		    ch->in_room != vch->in_room || is_safe(ch,vch))
		{
		    ch->embrace = NULL;
		    ch->embraced = ARE_NONE;
		    vch->embrace = NULL;
		    vch->embraced = ARE_NONE;
		}
		else if ( vch->level > 100 && vch->position > POS_STUNNED)
		{
		    if (vch->position < POS_FIGHTING)
		    {
			act("You clamber to your feet.",vch,NULL,NULL,TO_CHAR);
			act("$n clambers to $s feet.",vch,NULL,NULL,TO_ROOM);
			vch->position = POS_STANDING;
		    }
		    act("$N rips your head away from $S throat and hurls you across the room.",ch,NULL,vch,TO_CHAR);
		    act("You rip $n's head away from your throat and hurl $m across the room.",ch,NULL,vch,TO_VICT);
		    act("$N rips $n's head away from $S throat and hurls $m across the room.",ch,NULL,vch,TO_NOTVICT);
		    act("You crash to the ground, stunned.",ch,NULL,NULL,TO_CHAR);
		    act("$n crashes to the ground, stunned.",ch,NULL,NULL,TO_ROOM);
		    stop_fighting(ch, TRUE);
		    ch->position = POS_STUNNED;
		    ch->embrace = NULL;
		    ch->embraced = ARE_NONE;
		    vch->embrace = NULL;
		    vch->embraced = ARE_NONE;
		}
		else
		{
		    act("You shudder with ecstacy as you drain $N's lifeblood from $S throat.",ch,NULL,vch,TO_CHAR);
		    act("The gentle motion of $n's lips on your throat sends electric shivers through your body.",ch,NULL,vch,TO_VICT);
		    act("$n drinks deeply from $N's throat.",ch,NULL,vch,TO_NOTVICT);
		    if (vch->position == POS_STUNNED) update_pos(vch);
		    if (!IS_NPC(vch) && ((IS_WEREWOLF(vch) &&
			get_tribe(vch, TRIBE_GET_OF_FENRIS) > 0) || 
			IS_DEMON(vch)))
		    {
			int dam = (number_range(25,50) * amount * vch->blood[BLOOD_POTENCY]);
			act("Your lips blister and burn as $N's blood enters your mouth!",ch,NULL,vch,TO_CHAR);
			ch->hit -= dam;
			ch->agg += dam * 0.005;
			if (ch->agg > 100) ch->agg = 100;
			if (ch->hit < -10) ch->hit = -10;
			update_pos(ch);
			if (ch->position <= POS_STUNNED)
			{
			    act("$n keels over and collapses on the floor.",ch,NULL,NULL,TO_ROOM);
			    ch->embrace = NULL;
			    ch->embraced = ARE_NONE;
			    vch->embrace = NULL;
			    vch->embraced = ARE_NONE;
			}
		    }
		    if (IS_MORE(vch, MORE_BRISTLES))
		    {
			int dam = number_range(1000,5000);
			damage(vch, ch, dam, gsn_spines);
			stop_fighting(vch, TRUE);
			update_pos(ch);
			if (ch->position <= POS_STUNNED)
			{
			    act("$n keels over and collapses on the floor.",ch,NULL,NULL,TO_ROOM);
			    ch->embrace = NULL;
			    ch->embraced = ARE_NONE;
			    vch->embrace = NULL;
			    vch->embraced = ARE_NONE;
			}
		    }
		    if (vch->blood[BLOOD_CURRENT] > amount &&
			(vch->blood[BLOOD_CURRENT] - amount) <= 50)
		    {
			ch->blood[BLOOD_CURRENT] += (amount * vch->blood[BLOOD_POTENCY]);
			vch->blood[BLOOD_CURRENT] -= amount;
			if (!IS_VAMPIRE(vch))
			{
			    act("$N sways weakly in your arms.",ch,NULL,vch,TO_CHAR);
			    act("You sway weakly in $n's arms.",ch,NULL,vch,TO_VICT);
			    act("$N sways weakly in $n's arms.",ch,NULL,vch,TO_NOTVICT);
			}
			if (vch->blood[BLOOD_CURRENT] <= 25)
			{
			    if (IS_VAMPIRE(vch))
			    {
				act("You can feel that $N is very low on blood.",ch,NULL,vch,TO_CHAR);
				send_to_char("You are very low on blood.\n\r",vch);
			    }
			    else
			    {
				act("You can feel that $N is on the verge of death.",ch,NULL,vch,TO_CHAR);
				send_to_char("You feel very faint.\n\r",vch);
			    }
			}
		    }
		    else if (vch->blood[BLOOD_CURRENT] - amount <= 0)
		    {
			ch->blood[BLOOD_CURRENT] += vch->blood[BLOOD_CURRENT];
			vch->blood[BLOOD_CURRENT] = 0;
			act("Your body falls limply to the ground.",vch,NULL,NULL,TO_CHAR);
			act("$n's body falls limply to the ground.",vch,NULL,NULL,TO_ROOM);
			if (!IS_NPC(vch) && IS_VAMPIRE(vch))
			{
			    act("You suck $N's soul out of $S body.",ch,NULL,vch,TO_CHAR);
			    act("$n sucks your soul out of your body.",ch,NULL,vch,TO_VICT);
			    send_to_char("You have been Diablerised!\n\r",vch);
			    sprintf( buf, "%s has been diablerised by %s.", vch->name, ch->name);
			    do_info(ch, buf);
			    if (ch->beast == 0)
			    {
				send_to_char("You have lost Golconda!\n\r", ch);
				sprintf(buf, "%s has lost Golconda!", ch->name);
				do_info(ch,buf);
				if (IS_IMMUNE(ch, IMM_SUNLIGHT))
				    REMOVE_BIT(ch->immune, IMM_SUNLIGHT);
				ch->beast = 80;
				ch->truebeast = 90;
			    }
			    ch->pcdata->diableries[DIAB_TIME] += 10000;
			    ch->pcdata->diableries[DIAB_EVER]++;
			    ch->beast += 20;
			    ch->truebeast += 10;
			    if (ch->beast > 100) ch->beast = 100; 
			    if (ch->truebeast > 100) ch->truebeast = 100; 
			    if (is_affected(vch,gsn_potency)) 
				affect_strip(vch, gsn_potency);
			    ch->race++;
			    vch->race--;
			    if ( vch->exp > 0 )
			    {
				ch->exp += vch->exp;
				vch->exp = 0;
			    }
			}
			ch->embrace = NULL;
			ch->embraced = ARE_NONE;
			vch->embrace = NULL;
			vch->embraced = ARE_NONE;
			group_gain(ch,vch);
			vch->form = 0;
			killperson(ch,vch);
		    }
		    else
		    {
			ch->blood[BLOOD_CURRENT] += (amount * vch->blood[BLOOD_POTENCY]);
			vch->blood[BLOOD_CURRENT] -= amount;
		    }
		    if (ch->blood[BLOOD_CURRENT] >= ch->blood[BLOOD_POOL])
			ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
		}
	    }
	    if (ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
	    {
		CHAR_DATA *vch;
		if (ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
		{
		    ch->pcdata->stage[2]++;
		    if ( ( vch = ch->pcdata->partner ) != NULL &&
			!IS_NPC(vch) && vch->pcdata->partner == ch &&
			((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) ||
			(ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
		    {
			if (ch->in_room != vch->in_room) continue;
			if (vch->pcdata->stage[2] >= 225 &&
			    ch->pcdata->stage[2] >= 225 &&
			    vch->pcdata->stage[2] < 240 &&
			    ch->pcdata->stage[2] < 240)
			{
			    ch->pcdata->stage[2] = 240;
			    vch->pcdata->stage[2] = 240;
			}
			if (ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
			{
			    act("You thrust deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_CHAR);
			    act("$n thrusts deeply between your warm, damp thighs.",ch,NULL,vch,TO_VICT);
			    act("$n thrusts deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_NOTVICT);
			    if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
				ch->pcdata->stage[2] = vch->pcdata->stage[2];
			}
			else if (ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
			{
			    act("You squeeze your legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_CHAR);
			    act("$n squeezes $s legs tightly around you, moaning loudly.",ch,NULL,vch,TO_VICT);
			    act("$n squeezes $s legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_NOTVICT);
			    if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
				ch->pcdata->stage[2] = vch->pcdata->stage[2];
			}
		    }
		    if (ch->pcdata->stage[2] >= 250)
		    {
			if ( ( vch = ch->pcdata->partner ) != NULL &&
			    !IS_NPC(vch) && vch->pcdata->partner == ch &&
			    ch->in_room == vch->in_room)
			{
			    vch->pcdata->stage[2] = 250;
			    if (ch->sex == SEX_MALE)
			    {
				stage_update(ch,vch,2);
				stage_update(vch,ch,2);
			    }
			    else
			    {
				stage_update(vch,ch,2);
				stage_update(ch,vch,2);
			    }
			    ch->pcdata->stage[0] = 0;
			    vch->pcdata->stage[0] = 0;
			    if (!IS_EXTRA(ch, EXTRA_EXP))
			    {
				send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",ch);
				SET_BIT(ch->extra, EXTRA_EXP);
				ch->exp += 100000;
			    }
			    if (!IS_EXTRA(vch, EXTRA_EXP))
			    {
				send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",vch);
				SET_BIT(vch->extra, EXTRA_EXP);
				vch->exp += 100000;
			    }
			}
		    }
		}
		else
		{
		    if (ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 &&
			ch->position != POS_RESTING) 
		    {
			if (ch->pcdata->stage[0] > 1)
			    ch->pcdata->stage[0] -= 1;
			else
			    ch->pcdata->stage[0] = 0;
		    }
		    else if (ch->pcdata->stage[2]>0 && ch->pcdata->stage[0] < 1)
		    {
			if (ch->pcdata->stage[2] > 10)
			    ch->pcdata->stage[2] -= 10;
			else
			    ch->pcdata->stage[2] = 0;
			if (ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
			    send_to_char("You feel fully recovered.\n\r",ch);
		    }
		}
	    }

	    if (IS_ABOMINATION(ch))
	    {
		if (ch->position == POS_FIGHTING && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    if (ch->pcdata->wolf < 300)
		    {
			if (get_tribe(ch,TRIBE_GET_OF_FENRIS) > 4)
			    ch->pcdata->wolf += number_range(15,30);
			else
			    ch->pcdata->wolf += number_range(5,10);
		    }
		    if (!IS_SET(ch->act, PLR_WOLFMAN) && 
			ch->pcdata->wolf >= 100 && ch->pcdata->wolfform[1] == FORM_HOMID)
			do_werewolf(ch,"");
		}
		else if (ch->pcdata->wolf > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    ch->pcdata->wolf--;
		    if (ch->pcdata->wolf < 100 && ch->pcdata->wolfform[1] == FORM_HOMID)
			do_unwerewolf(ch,"");
		}
		if (IS_MORE(ch, MORE_ANGER)) beast_rage(ch);
		if (ch->pcdata->regenerate > 0)
		{
		    if (ch->blood[BLOOD_CURRENT] < 10 && ch->hit > 0)
		    {
			send_to_char("You have insufficient blood to regenerate any further.\n\r",ch);
			ch->pcdata->regenerate = 0;
			continue;
		    }
		    ch->pcdata->regenerate--;
		    ch->blood[BLOOD_CURRENT] -= 10;
		    werewolf_regen(ch);
		    if (ch->loc_hp[6] > 0) ch->loc_hp[0] = 0;
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
			part_reg(ch);
		}
	    }
	    else if (IS_VAMPIRE(ch))
	    {
		if ( ch->position == POS_FIGHTING && ch->pcdata->wolf > 0 
		    && ch->pcdata->wolf < (ch->beast * 0.5) 
		    && !IS_ITEMAFF(ch, ITEMA_RAGER) )
		    ch->pcdata->wolf++;
		else if (ch->pcdata->wolf > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    if (number_percent() <= ch->beast)
			beast_rage(ch);
		    else
			ch->pcdata->wolf--;
		    if (ch->pcdata->wolf < 1)
			send_to_char("You feel the beast release it's hold on you.\n\r",ch);
		}
		if (IS_MORE(ch, MORE_ANGER)) beast_rage(ch);
		if (ch->pcdata->wolf < 1)
		{
		    if (ch->pcdata->regenerate > 0)
		    {
			if (ch->blood[BLOOD_CURRENT] < 10 && ch->hit > 0)
			{
			    send_to_char("You have insufficient blood to regenerate any further.\n\r",ch);
			    continue;
			}
			ch->pcdata->regenerate--;
			ch->blood[BLOOD_CURRENT] -= 10;
			werewolf_regen(ch);
			if (ch->loc_hp[6] > 0) ch->loc_hp[0] = 0;
			if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
			    part_reg(ch);
		    }
		    continue;
		}
		if (ch->move > 1) ch->move--;
	    }
	    else if (IS_WEREWOLF(ch))
	    {
		if (ch->position == POS_FIGHTING && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    if (ch->pcdata->wolf < 300)
		    {
			if (get_tribe(ch,TRIBE_GET_OF_FENRIS) > 4)
			    ch->pcdata->wolf += number_range(15,30);
			else
			    ch->pcdata->wolf += number_range(5,10);
		    }
		    if (!IS_SET(ch->act, PLR_WOLFMAN) && 
			ch->pcdata->wolf >= 100 && ch->pcdata->wolfform[1] == FORM_HOMID)
			do_werewolf(ch,"");
		}
		else if (ch->pcdata->wolf > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
		{
		    ch->pcdata->wolf--;
		    if (ch->pcdata->wolf < 100 && ch->pcdata->wolfform[1] == FORM_HOMID)
			do_unwerewolf(ch,"");
		}
		if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
		    werewolf_regen(ch);
		if ( IS_BLACK_SPIRAL_DANCER(ch) )
		{
		    if ( ch->in_room != NULL && ch->in_room->vnum == 30000 )
		    {
			if ( ch->hit > 0 )
			{
			    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
				werewolf_regen(ch);
			    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
				werewolf_regen(ch);
			    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
				werewolf_regen(ch);
			    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
				werewolf_regen(ch);
			}
			if (ch->loc_hp[6] > 0)
			{
			    int sn = skill_lookup( "clot" );
			    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
			}
			else
			{
			    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
				ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
			    reg_mend(ch);
			}
		    }
		    else
		    {
			if (ch->loc_hp[6] > 0)
			{
			    int sn = skill_lookup( "clot" );
			    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
			}
			else if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
			{
			    int sn = skill_lookup( "mend" );
			    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
			}
		    }
		    continue;
	        }
		if ( ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL] )
		{
		    ch->blood[BLOOD_CURRENT] += number_range(5,10);
		    if (ch->blood[BLOOD_CURRENT] > ch->blood[BLOOD_POOL])
			ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
		}
		if (IS_WEREWOLF(ch) && ch->position == POS_SLEEPING
		    && ch->pcdata->disc[TOTEM_BEAR] > 3 && ch->hit > 0)
		{
		    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			werewolf_regen(ch);
		    if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			werewolf_regen(ch);
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	    else if (IS_DEMON(ch) && IS_HERO(ch))
	    {
		if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
		    werewolf_regen(ch);
		if ( ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL] )
		    ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
		if ( ch->in_room != NULL && 
		    ch->in_room->vnum == ((IS_ANGEL(ch)) ? 30050 : 30000) )
		{
		    if ( ch->hit > 0 )
		    {
			if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			    werewolf_regen(ch);
			if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			    werewolf_regen(ch);
			if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			    werewolf_regen(ch);
			if ( ch->hit < ch->max_hit || ch->move < ch->max_move )
			    werewolf_regen(ch);
		    }
		    if (ch->loc_hp[6] > 0)
		    {
			int sn = skill_lookup( "clot" );
			(*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		    }
		    else
		    {
			if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
			reg_mend(ch);
		    }
		    continue;
		}
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
		    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		{
		    int sn = skill_lookup( "mend" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
	    }
	    else if (IS_MAGE(ch) && ch->mana < ch->max_mana)
	    {
		if (ch->position == POS_MEDITATING)
		    ch->mana += number_range(100,500);
		else
		    ch->mana += number_range(10,50);
	    }
	    if (IS_ITEMAFF(ch, ITEMA_REGENERATE) || (!IS_NPC(ch) && 
		IS_HIGHLANDER(ch)))
	    {
		if ( !IS_VAMPIRE(ch) && ch->blood[BLOOD_CURRENT] < 
		    ch->blood[BLOOD_POOL] )
		{
		    ch->blood[BLOOD_CURRENT] += number_range(5,10);
		    if (ch->blood[BLOOD_CURRENT] > ch->blood[BLOOD_POOL])
			ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
		}
		if ( ch->hit < ch->max_hit || ch->move < ch->max_move ||
		     ch->mana < ch->max_mana )
		    werewolf_regen(ch);
		if (ch->loc_hp[6] > 0)
		{
		    int sn = skill_lookup( "clot" );
		    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
		}
		else
		{
		    if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
			ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
		    reg_mend(ch);
		}
	    }
	    continue;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) || IS_EXTRA(ch, EXTRA_ILLUSION) || 
	    ch->wizard != NULL) 
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	    if (ch == NULL) continue;
	}

	if ( ch->position == POS_FIGHTING && IS_SET(ch->act, ACT_SKILLED) &&
	    ch->stance[0] < 1)
	{
	    switch (number_range(1,5))
	    {
		default:
		case 1: do_stance(ch,"viper"); break;
		case 2: do_stance(ch,"crane"); break;
		case 3: do_stance(ch,"mongoose"); break;
		case 4: do_stance(ch,"bull"); break;
		case 5: do_stance(ch,"crab"); break;
	    }
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_CORPSE_PC ) continue;
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n picks $p up.", ch, obj_best, NULL, TO_ROOM );
		act( "You pick $p up.", ch, obj_best, NULL, TO_CHAR );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL)
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( ch->hunting == NULL || strlen(ch->hunting) < 2 )
	&& ( (!IS_SET(ch->act, ACT_STAY_AREA) && ch->level < 900)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door );
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 3 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_AFFECTED(ch, AFF_WEBBED)
	&&   ch->level < 900
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = pexit->to_room->people;
		  rch != NULL;
		  rch  = rch->next_in_room )
	    {
		if ( !IS_NPC(rch) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
		move_char( ch, door );
	}

    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch = NULL;
    int diff, recover;
    bool char_up;
    bool mes_all = FALSE;

    strcpy(buf,"Buf - Please inform KaVir.\n\r");

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&& ( ch = d->character) != NULL
	&&  !IS_NPC(ch)
	&&  !IS_EXTRA(ch, EXTRA_OSWITCH)
	&&  !IS_HEAD(ch, LOST_HEAD))
	{
	    if (IS_WEREWOLF(ch) && ch->pcdata->gnosis[0] < ch->pcdata->gnosis[1])
	    {
		ch->pcdata->gnosis[0] += number_range(5,10);
		if (ch->pcdata->gnosis[0] > ch->pcdata->gnosis[1]) 
		    ch->pcdata->gnosis[0] = ch->pcdata->gnosis[1];
	    }
	    recover = number_range(5,10) + (ch->pcdata->willpower[1] * 0.1);
	    if (ch->pcdata->willpower[0] < ch->pcdata->willpower[1])
		ch->pcdata->willpower[0] += recover;
	    if (ch->pcdata->willpower[0] > ch->pcdata->willpower[1])
		ch->pcdata->willpower[0] = ch->pcdata->willpower[1];
	    if (IS_MORE(ch, MORE_INFORM)) REMOVE_BIT(ch->more, MORE_INFORM);
	    if (IS_MORE(ch, MORE_COURAGE) && number_percent() < 50)
	    {
		send_to_char("You no longer feel quite so brave.\n\r",ch);
		REMOVE_BIT(ch->more, MORE_COURAGE);
	    }
	    if (IS_MORE(ch, MORE_CALM) && number_percent() < 50)
	    {
		send_to_char("You no longer feel so apathetic.\n\r",ch);
		REMOVE_BIT(ch->more, MORE_CALM);
	    }
	    if (IS_MORE(ch, MORE_ANGER) && number_percent() < 50 && ch->pcdata->wolf < 1)
	    {
		send_to_char("You no longer feel so angry.\n\r",ch);
		REMOVE_BIT(ch->more, MORE_ANGER);
	    }
	}
    }
    if (IS_SET(world_affects, WORLD_ECLIPSE))
    {
	strcpy(buf,"The sun emerges from behind the moon.\n\r");
	REMOVE_BIT(world_affects, WORLD_ECLIPSE);
	mes_all = TRUE;
    }
    else if (IS_SET(world_affects, WORLD_FOG) && number_range(1,5) == 1)
    {
	strcpy(buf,"The fog disperses.\n\r");
	REMOVE_BIT(world_affects, WORLD_FOG);
	mes_all = TRUE;
    }
    else if (IS_SET(world_affects, WORLD_RAIN) && number_range(1,5) == 1)
    {
	strcpy(buf,"It is no longer raining quite so heavily.\n\r");
	REMOVE_BIT(world_affects, WORLD_RAIN);
	mes_all = TRUE;
    }
    if (mes_all)
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( ch = d->character) != NULL
	    &&  !IS_NPC(ch)
	    &&  !IS_EXTRA(ch, EXTRA_OSWITCH)
	    &&  !IS_HEAD(ch, LOST_HEAD)
	    &&  ch->in_room != NULL
	    &&  ch->in_room->sector_type != SECT_INSIDE
	    &&  IS_OUTSIDE(ch))
		send_to_char(buf,ch);
	}
    }
    buf[0] = '\0';
    switch ( ++time_info.hour )
    {
    case  4:
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( ch = d->character) != NULL
	    &&  !IS_NPC(ch)
	    &&  !IS_EXTRA(ch, EXTRA_OSWITCH)
	    &&  !IS_HEAD(ch, LOST_HEAD)
	    &&  IS_VAMPIRE(ch))
		send_to_char( "You feel weary as the night begins to come to an end.\n\r",ch);
	}
	break;

    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    char_up = FALSE;
	    if ( d->connected == CON_PLAYING
	    && ( ch = d->character) != NULL
	    &&  !IS_NPC(ch)
	    &&  !IS_EXTRA(ch, EXTRA_OSWITCH)
	    &&  !IS_HEAD(ch, LOST_HEAD))
	    {
		send_to_char( "You hear a clock in the distance strike midnight.\n\r",ch);
		if (IS_MORE(ch, MORE_NOPAIN))
		{
		    REMOVE_BIT(ch->more, MORE_NOPAIN);
		    send_to_char("You no longer feel so oblivious to pain.\n\r",ch);
		}
		if ( IS_VAMPIRE(ch) )
		{
		    if (ch->beast > ch->truebeast && ch->beast > 0 && 
			ch->beast < 100 && number_range(1,5) == 1)
		    {
			if (--ch->beast == 0)
			{
			    send_to_char("You have attained Golconda!\n\r", ch);
			    if (!IS_IMMUNE(ch, IMM_SUNLIGHT))
				SET_BIT(ch->immune, IMM_SUNLIGHT);
			    ch->exp += 100000000;
			    send_to_char("You recieve 100000000 experience points]\n\r",ch);
			}
		    }
		    if (ch->hit < ch->max_hit)
			{ ch->hit = ch->max_hit; char_up = TRUE; }
		    if (ch->move < ch->max_move)
			{ ch->move = ch->max_move; char_up = TRUE; }
		    ch->loc_hp[0] = 0;
		    ch->loc_hp[1] = 0;
		    ch->loc_hp[2] = 0;
		    ch->loc_hp[3] = 0;
		    ch->loc_hp[4] = 0;
		    ch->loc_hp[5] = 0;
		    ch->loc_hp[6] = 0;
		    if (char_up) send_to_char( "You feel the strength of the kindred flow through your veins!\n\r", ch );
		    update_pos(ch);
		}
	    }
	}
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if (IS_SET(world_affects, WORLD_RAIN))
	{
	    if ( weather_info.mmhg <  970 )
		weather_info.mmhg += 10;
	    else if ( weather_info.mmhg > 1010 )
		weather_info.mmhg -= 10;
	    strcat( buf, "The rain pours down heavily from the sky.\n\r" );
	    break;
	}
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    bool is_obj;
    bool drop_out = FALSE;
    int sn;
    time_t save_time;

    save_time	= current_time;
    ch_save	= NULL;
    ch_quit	= NULL;
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;
	drop_out = FALSE;

	if (!IS_NPC(ch) && (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH)))
	    is_obj = TRUE;
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	    {is_obj = TRUE;SET_BIT(ch->extra, EXTRA_OSWITCH);}
	else
	    is_obj = FALSE;
	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC(ch)
	&& ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
	&&   ch->level >= 2
	&&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}
/*
	if (!IS_NPC(ch) && IS_WEREWOLF(ch) && !is_obj &&
	    ch->pcdata->disc[TOTEM_BEAR] > 3 && ch->position == POS_SLEEPING)
	{
	    if ( ch->hit  < ch->max_hit  ) ch->hit  = ch->max_hit;
	    if ( ch->move < ch->max_move ) ch->move = ch->max_move;
	}
*/
	if ( ch->position > POS_STUNNED && !is_obj)
	{
	    if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch);

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain(ch);

	    if ( ch->move < ch->max_move )
		ch->move += move_gain(ch);
	}

	if ( ch->position == POS_STUNNED && !is_obj)
	{
            ch->hit = ch->hit + number_range(2,4);
	    update_pos( ch );
	}

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL && !is_obj)
	{
	    OBJ_DATA *obj;

	    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    || ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 ) )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if ( ++ch->timer >= 12 )
	    {
		if ( ch->was_in_room == NULL && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL)
		    {
			OBJ_DATA *chobj = ch->pcdata->chobj;
			if ( chobj->in_room != NULL )
			    obj_from_room( chobj );
			else if ( chobj->carried_by != NULL )
			    obj_from_char( chobj );
			else if ( chobj->in_obj != NULL )
			    obj_from_obj( chobj );
			obj_to_room( chobj, get_room_index( ROOM_VNUM_LIMBO ) );
		    }
		    do_autosave( ch, "" );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->timer > 30 ) ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -1 );
	    if (!IS_VAMPIRE(ch))
	    {
	    	gain_condition( ch, COND_FULL, -1 );
	    	gain_condition( ch, COND_THIRST, -1 );
	    }
	    else
	    {
		if (ch->blood[BLOOD_CURRENT] > 0) ch->blood[BLOOD_CURRENT]--;
		if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] < 1)
		{
		    send_to_char( "You are DYING from lack of blood!\n\r", ch );
		    act( "$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		    if (ch->hit > 0) ch->hit -= number_range(2,5);
		    update_pos(ch);
		    if (number_percent() <= ch->beast && ch->beast > 0) vamp_rage(ch);
		    if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
		}
		else if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] <= 20)
		{
		    send_to_char( "You crave blood.\n\r", ch );
		    if (number_range(1,1000) <= ch->beast && ch->beast > 0) vamp_rage(ch);
		    if (number_percent() > (ch->blood[BLOOD_CURRENT]+75)
			&& !IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
		}
/*
		blood = -1;
		if (ch->beast > 0)
		{
		    if (IS_VAMPAFF(ch, VAM_CLAWS)) blood -= number_range(1,3);
		    if (IS_VAMPAFF(ch, VAM_FANGS)) blood -= 1;
		    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) blood -= 1;
		    if (IS_VAMPAFF(ch, AFF_SHADOWSIGHT)) blood -= number_range(1,3);
		    if (IS_SET(ch->act, PLR_HOLYLIGHT)) blood -= number_range(1,5);
		    if (IS_VAMPAFF(ch, VAM_DISGUISED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, VAM_CHANGED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, IMM_SHIELDED)) blood -= number_range(1,3);
		    if (IS_POLYAFF(ch, POLY_SERPENT)) blood -= number_range(1,3);
		    if (ch->beast == 100) blood *= 2;
		}
		gain_condition( ch, COND_THIRST, blood );
*/
	    }
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off && !is_obj)
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
	  
		affect_remove( ch, paf );
	    }
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( ch->loc_hp[6] < 1 && !is_obj && ch->in_room != NULL && !IS_VAMPIRE(ch))
	{ if (ch->blood[0] < 100) ch->blood[0]++; }
	if ( ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL && !IS_VAMPIRE(ch))
	{
	    if (IS_BLEEDING(ch,BLEEDING_HEAD))
	    {
		act( "A spray of blood shoots from the stump of $n's neck.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your neck.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 10)
		    ch->blood[BLOOD_CURRENT] -= 10;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_THROAT))
	    {
		act( "Blood pours from the slash in $n's throat.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Blood pours from the slash in your throat.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 3)
		    ch->blood[BLOOD_CURRENT] -= 3;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_CHEST))
	    {
		act( "Blood pours from the deep wound in $n's chest.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Blood pours from the deep wound in your chest.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_STOMACH))
	    {
		act( "Blood pours from $n's torn-open stomach.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Blood pours from your torn-open stomach.\n\r", ch );
		if (number_percent() > 50 && !IS_BODY(ch, LOST_ENTRAILS))
		{
		    act( "$n's entrails pour out onto the floor.", ch, NULL, NULL, TO_ROOM );
		    send_to_char( "Your entrails pour out onto the floor.\n\r", ch );
		    SET_BIT(ch->loc_hp[LOC_BODY], LOST_ENTRAILS);
		    make_part(ch,"entrails");
		    if (!is_undying(ch))
		    {
			act("$n collapses weakly and dies from $s wounds.",ch,NULL,NULL,TO_ROOM);
			send_to_char("You collapse weakly from your wounds.",ch);
			ch->form = 12;
			killperson(ch,ch);
			drop_out = TRUE;
			continue;
		    }
		}
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_L))
	    {
		act( "A spray of blood shoots from the stump of $n's left arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left arm.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_L))
	    {
		act( "A spray of blood shoots from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left wrist.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_WRIST_L))
	    {
		act( "A spray of blood shoots from $n's slashed left wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from your slashed left wrist.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_R))
	    {
		act( "A spray of blood shoots from the stump of $n's right arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right arm.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_R))
	    {
		act( "A spray of blood shoots from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right wrist.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_WRIST_R))
	    {
		act( "A spray of blood shoots from $n's slashed right wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from your slashed right wrist.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_L))
	    {
		act( "A spray of blood shoots from the stump of $n's left leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left leg.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_L))
	    {
		act( "A spray of blood shoots from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left ankle.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_R))
	    {
		act( "A spray of blood shoots from the stump of $n's right leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right leg.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 5)
		    ch->blood[BLOOD_CURRENT] -= 5;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_R))
	    {
		act( "A spray of blood shoots from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right ankle.\n\r", ch );
		if (ch->blood[BLOOD_CURRENT] > 2)
		    ch->blood[BLOOD_CURRENT] -= 2;
		else
		    ch->blood[BLOOD_CURRENT] = 0;
	    }
	    if (!is_undying(ch) && number_percent() > 90)
	    {
		act("$n collapses weakly and dies from $s wounds.",ch,NULL,NULL,TO_ROOM);
		send_to_char("You collapse weakly to the floor from your wounds.",ch);
		ch->form = 13;
		killperson(ch,ch);
		drop_out = TRUE;
		continue;
	    }
	    if (ch->blood[BLOOD_CURRENT] == 0 && !is_undying(ch))
	    {
		act("$n collapses weakly and dies from blood loss.",ch,NULL,NULL,TO_ROOM);
		send_to_char("You collapse weakly to the floor from blood loss.",ch);
		ch->form = 13;
		killperson(ch,ch);
		drop_out = TRUE;
		continue;
	    }
	    else if (ch->blood[BLOOD_CURRENT] <= 20 && !IS_VAMPIRE(ch))
	    {
		act("$n sways weakly.",ch,NULL,NULL,TO_ROOM);
		send_to_char("You sway weakly from blood loss.\n\r",ch);
	    }
	    ch->in_room->blood += number_range(1,10);
	    if (ch->in_room->blood > 1000) ch->in_room->blood = 1000;
	}
	if ( IS_AFFECTED(ch, AFF_FLAMING) && !is_obj && !drop_out && ch->in_room != NULL )
	{
	    int dam = number_range(250,500);

	    if (IS_MORE(ch, MORE_FLAMES)) dam *= 2;
	    if (IS_NPC(ch) || (!IS_IMMUNE(ch, IMM_HEAT) &&
		get_disc(ch, DISC_DAIMOINON) < 6))
	    {
	    	act( "$n's flesh burns and crisps.", ch, NULL, NULL, TO_ROOM );
	    	send_to_char( "Your flesh burns and crisps.\n\r", ch );
		if ( !IS_NPC(ch) && (IS_VAMPIRE(ch) || IS_GHOUL(ch)) &&
		    get_disc(ch,DISC_FORTITUDE) > 0)
		{
		    int per_red = 0;
		    switch( get_disc(ch,DISC_FORTITUDE) )
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
		    ch->hit += (per_red * dam * 0.01);
		}
		ch->hit -= dam;
		update_pos(ch);
		if (ch == NULL) continue;
		if (ch->position == POS_DEAD)
		{
		    ch->form = 14;
		    killperson(ch,ch);
		    drop_out = TRUE;
		    continue;
		}
	    }
	}
/*
	else if ( IS_VAMPIRE(ch) && (!IS_AFFECTED(ch,AFF_SHADOWPLANE)) &&
	    (!IS_NPC(ch) && !IS_IMMUNE(ch,IMM_SUNLIGHT)) && ch->in_room != NULL &&
	    (!ch->in_room->sector_type == SECT_INSIDE) && !is_obj &&
	    (!room_is_dark(ch->in_room)) && (weather_info.sunlight != SUN_DARK) )
	{
	    act( "$n's flesh smolders in the sunlight!", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your flesh smolders in the sunlight!\n\r", ch );
	    ** This one's to keep Zarkas quiet ;) **
	    if (IS_POLYAFF(ch, POLY_SERPENT))
	    	ch->hit = ch->hit - number_range(2,4);
	    else
	    	ch->hit = ch->hit - number_range(5,10);
	    update_pos(ch);
	    if ((!IS_NPC(ch) && ch->hit<-10) || (IS_NPC(ch) && ch->hit<0))
	    {
		ch->form = 15;
		killperson(ch,ch);
		drop_out = TRUE;
		continue;
	    }
	}
*/
	if ( IS_AFFECTED(ch, AFF_ROT) && !is_obj && !drop_out )
	{
	    act( "Pieces of rotting flesh fall from $n's body.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Pieces of rotting flesh fall from your body.\n\r", ch );
	    damage( ch, ch, number_range(10,100), gsn_rot );
	    if (ch == NULL || ch->position == POS_DEAD) continue;
	    else if (get_curr_con(ch) > 0 && 
		( sn = skill_lookup( "rotting flesh" ) ) >= 0 )
	    {
		AFFECT_DATA af;
		af.type      = sn;
		af.location  = APPLY_CON;
		af.modifier  = -1;
		af.duration  = 0;
		af.bitvector = AFF_ROT;
		affect_join( ch, &af );
	    }
	}
	else if ( IS_AFFECTED(ch, AFF_POISON) && !is_obj && !drop_out )
	{
	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    damage( ch, ch, 2, gsn_poison );
	    if (ch == NULL || ch->position == POS_DEAD) continue;
	}
	else if ( !IS_NPC( ch ) && ch->paradox[1] > 0 )
	{
	    if ( ch->paradox[1] > 50 ) paradox( ch );
	    else if ( ch->paradox[2] == 0 && ch->paradox[1] > 0 )
	    {
		ch->paradox[1] --;
		ch->paradox[2] = PARADOX_TICK;
	    }
	    else ch->paradox[3] --;
	}
	else if ( ch->position == POS_INCAP && !is_obj && !drop_out )
	{
	    if (IS_HERO(ch))
                ch->hit = ch->hit + number_range(2,4);
	    else
                ch->hit = ch->hit - number_range(1,2);
	    update_pos( ch );
            if (ch->position > POS_INCAP)
            {
                act( "$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Your wounds stop bleeding and seal up.\n\r", ch );
            }
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "You clamber back to your feet.\n\r", ch );
            }
	}
	else if ( ch->position == POS_MORTAL && !is_obj && !drop_out )
	{
	    drop_out = FALSE;
	    if (IS_HERO(ch))
                ch->hit = ch->hit + number_range(2,4);
	    else
	    {
                ch->hit = ch->hit - number_range(1,2);
		if ((!IS_NPC(ch) && ch->hit<-10) || (IS_NPC(ch) && ch->hit<0))
		{
/* kavirp */
		    ch->form = 16;
		    killperson(ch,ch);
		    continue;
		}
	    }
	    if (!drop_out)
	    {
	    	update_pos( ch );
            	if (ch->position == POS_INCAP)
            	{
                    act( "$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Your wounds begin to close, and your bones pop back into place.\n\r", ch );
            	}
	    }
	}
	else if ( ch->position == POS_DEAD && !is_obj && !drop_out )
	{
/*
	    update_pos(ch);
*/
	    if (!IS_NPC(ch)) 
	    {
		ch->form = 16;
		killperson(ch,ch);
		continue;
	    }
	}
	drop_out = FALSE;
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save != NULL || ch_quit != NULL )
    {
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
	    ch_next = ch->next;
	    if ( ch == ch_save )
		do_autosave( ch, "" );
	    if ( ch == ch_quit )
		do_quit( ch, "" );
	}
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	if (obj->item_type != ITEM_PAGE && IS_SET(obj->quest, QUEST_FOLLOWING))
	{
	    if ( regenerate_part( obj ) ) continue;
	}

	if (obj->item_type != ITEM_PAGE && IS_SET(obj->spectype, SITEM_MESSAGE))
	{
	    if (strlen(obj->chpoweruse) > 1 && strlen(obj->victpoweruse) > 1 && 
		number_range(1,30) == 1)
	    {
		if ( obj->carried_by != NULL )
		{
		    act( obj->chpoweruse, obj->carried_by, obj, NULL, TO_CHAR );
		    act( obj->victpoweruse, obj->carried_by, obj, NULL, TO_ROOM );
		}
		else if ( obj->in_room != NULL 
		&&      ( rch = obj->in_room->people ) != NULL )
		{
		    act( obj->chpoweruse, rch, obj, NULL, TO_ROOM );
		    act( obj->chpoweruse, rch, obj, NULL, TO_CHAR );
		}
		if (IS_SET(obj->quest, QUEST_BLOODAGONY))
		{
		    if (--obj->specpower < 1)
		    {
			free_string(obj->chpoweruse);
			obj->chpoweruse = str_dup( "" );
			free_string(obj->victpoweruse);
			obj->victpoweruse = str_dup( "" );
			obj->specpower = 0;
			REMOVE_BIT(obj->spectype, SITEM_MESSAGE);
			REMOVE_BIT(obj->quest, QUEST_BLOODAGONY);
			if ( ( rch = obj->carried_by ) != NULL )
			{
			    act("All the blood has dripped off $p.",rch,obj,NULL,TO_CHAR);
			    if (obj->wear_loc == WEAR_WIELD)
				REMOVE_BIT(rch->itemaffect, ITEMA_RIGHT_AGG);
			    else if (obj->wear_loc == WEAR_HOLD)
				REMOVE_BIT(rch->itemaffect, ITEMA_LEFT_AGG);
			}
		    }
		}
	    }
	}

	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	if (IS_SET(obj->extra_flags, ITEM_VANISH))
	    message = "$p slowly fades away to nothing.";
	else if (IS_SET(obj->spectype, SITEM_DEMONIC))
	    message = "$p vanishes in a blast of flames.";
	else switch ( obj->item_type )
	{
	default:              message = "$p vanishes.";         	break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         	break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; 	break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; 	break;
	case ITEM_FOOD:       message = "$p decomposes.";		break;
	case ITEM_TRASH:      message = "$p crumbles into dust.";	break;
	case ITEM_EGG:        message = "$p cracks open.";		break;
	case ITEM_WEAPON:     message = "$p crumbles into dust.";	break;
	case ITEM_WALL:       message = "$p sinks back into the ground.";break;
	case ITEM_STAKE:      message = "$p crumbles into dust.";       break;
	}

	if ( obj->carried_by != NULL )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

	/* If the item is an egg, we need to create a mob and shell!
	 * KaVir
	 */
	if ( obj->item_type == ITEM_EGG )
	{
	    CHAR_DATA      *creature;
	    OBJ_DATA       *egg;
	    if ( get_mob_index( obj->value[0] ) != NULL )
	    {
		if (obj->carried_by != NULL && obj->carried_by->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->carried_by->in_room);
		}
		else if (obj->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->in_room);
		}
		else
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,get_room_index(ROOM_VNUM_HELL));
/*
		    obj->timer = 1;
		    continue;
*/
		}
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, creature->in_room );
	    	act( "$n clambers out of $p.", creature, obj, NULL, TO_ROOM );
	    }
	    else if (obj->in_room != NULL)
	    {
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, obj->in_room );
	    }
	}
	extract_obj( obj );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't want the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim = NULL;

    OBJ_DATA *obj = NULL;
    OBJ_DATA *chobj = NULL;
    ROOM_INDEX_DATA *objroom = NULL;
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
/*
	if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata->stage[1] > 0
	&& ( victim = ch->pcdata->partner ) != NULL
	&&   !IS_NPC(victim)
	&&   ch->in_room != NULL
	&&   victim->in_room != NULL
	&&   victim->in_room != ch->in_room )
	{
	    ch->pcdata->stage[1] = 0;
	    victim->pcdata->stage[1] = 0;
	}
*/
	if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata != NULL
	&& ( obj = ch->pcdata->chobj ) != NULL )
	{
	    if (obj->in_room != NULL)
	    	objroom = obj->in_room;
	    else if (obj->in_obj != NULL)
	    	objroom = get_room_index(ROOM_VNUM_IN_OBJECT);
	    else if (obj->carried_by != NULL)
	    {
		if (obj->carried_by != ch && obj->carried_by->in_room != NULL)
		    objroom = obj->carried_by->in_room;
		else continue;
	    }
	    else continue;
	    if (ch->in_room != objroom && objroom != NULL)
	    {
	    	char_from_room(ch);
	    	char_to_room(ch,objroom);
		do_look(ch,"auto");
	    }
	}
	else if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata != NULL
	&&   (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0) )
	{
	    if (ch->pcdata->obj_vnum != 0)
	    {
		bind_char(ch);
		continue;
	    }
	    if (IS_HEAD(ch,LOST_HEAD))
	    {
	    	REMOVE_BIT(ch->loc_hp[0],LOST_HEAD);
	    	send_to_char("You are able to regain a body.\n\r",ch);
	    	ch->position = POS_RESTING;
	    	ch->hit = 1;
	    } else {
	    	send_to_char("You return to your body.\n\r",ch);
		REMOVE_BIT(ch->extra,EXTRA_OSWITCH);}
	    REMOVE_BIT(ch->affected_by,AFF_POLYMORPH);
	    free_string(ch->morph);
	    ch->morph = str_dup("");
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
	    if ( ( chobj = ch->pcdata->chobj ) != NULL )
		chobj->chobj = NULL;
	    ch->pcdata->chobj = NULL;
	    do_look(ch,"auto");
	}
	continue;
    }

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->next;
	if ( IS_NPC(wch)
	|| ( wch->desc != NULL && wch->desc->connected != CON_PLAYING )
	||   wch->position <= POS_STUNNED
	||   wch->level >= LEVEL_IMMORTAL
	||   wch->pcdata == NULL
	|| ( ( chobj = wch->pcdata->chobj ) != NULL )
	||   wch->in_room == NULL )
	    continue;

	if ( !IS_NPC(wch) && ( IS_WEREWOLF(wch) ||
	    IS_ITEMAFF(wch, ITEMA_STALKER) ) )
	{
	    if ( wch->hunting != NULL && wch->hunting != '\0' && 
		strlen(wch->hunting) > 1 && get_auspice(wch, AUSPICE_RAGABASH) > 2)
	    {
		ROOM_INDEX_DATA *old_room = wch->in_room;
		check_hunt( wch );
		if (wch->in_room == old_room)
		{
		    free_string(wch->hunting);
		    wch->hunting = str_dup( "" );
		    continue;
		}
		check_hunt( wch );
		if (wch->in_room == old_room)
		{
		    free_string(wch->hunting);
		    wch->hunting = str_dup( "" );
		}
		continue;
	    }
	}

	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   no_attack(ch, wch)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&& ( vch->desc == NULL || IS_PLAYING(vch->desc) )
		&&   !no_attack(ch, vch)
		&&   vch->pcdata != NULL
		&& ( ( chobj = vch->pcdata->chobj ) == NULL )
		&&   vch->level < LEVEL_IMMORTAL
		&&   vch->position > POS_STUNNED
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
	    {
/*
		bug( "Aggr_update: null victim attempt by mob %d.", ch->pIndexData->vnum );
*/
		continue;
	    }

	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_gain_exp;
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;

    if ( --pulse_gain_exp <= 0 )
    {
	char buf [MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch = NULL;
	CHAR_DATA *ch_next = NULL;
	int sn;
	extern int bootcount;
	pulse_gain_exp	= PULSE_GAIN_EXP;
	if (bootcount > 0)
	{
	    bootcount--;
	    switch (bootcount)
	    {
		default:  strcpy(buf," "); break;
		case 600: strcpy(buf,"Reboot in ten minutes");    break;
		case 300: strcpy(buf,"Reboot in five minutes");   break;
		case 120: strcpy(buf,"Reboot in two minutes");    break;
		case 60:  strcpy(buf,"Reboot in one minute");     break;
		case 30:  strcpy(buf,"Reboot in thirty seconds"); break;
		case 10:  strcpy(buf,"Reboot in ten seconds");    break;
		case 0:   strcpy(buf,"Reboot NOW");    break;
	    }
	    if (strlen(buf) > 2) sysmes(buf);
	    if (bootcount <= 0)
	    {
		extern bool merc_down;
		for ( ch = char_list; ch != NULL; ch = ch_next )
		{
		    ch_next = ch->next;
		    if (!IS_NPC(ch)) do_autosave(ch,"");
		}
		sprintf(log_buf,"Timed reboot." );
		log_string( log_buf );
		merc_down = TRUE;
	    }
	}
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   (ch = d->character) != NULL
	    &&  !IS_NPC(ch))
	    {
		ch->exp += ch->beast ? 1 : 10;
		if (ch->position == POS_FIGHTING && ch->move > 0 && !IS_HERO(ch)) ch->move--;
		if (ch->pcdata->bpoints > 0)
		{
		    ch->pcdata->bpoints--;
		    if (ch->pcdata->bpoints < 1)
			send_to_char("You are once more able to use Blood Agony.\n\r",ch);
		}
		if (ch->pcdata->dpoints > 0)
		{
		    ch->pcdata->dpoints--;
		    if (ch->pcdata->dpoints < 1)
		    {
			if (IS_GHOUL(ch))
			{
			    bool lost_disc = FALSE;
			    mortal_vamp(ch);
			    for ( sn = 0; sn <= DISC_MAX; sn++ )
			    {
				if (ch->pcdata->powers[sn] > 0)
				    lost_disc = TRUE;
				ch->pcdata->powers[sn] = 0;
			    }
			    if (lost_disc) send_to_char("You have lost your blood powers.\n\r",ch);
			}
			else send_to_char("You are once more able to sire a childe.\n\r",ch);
		    }
		}
		if (ch->pcdata->silence > 0)
		{
		    ch->pcdata->silence--;
		    if (ch->pcdata->silence < 1)
		    {
			if (IS_MORE(ch, MORE_SILENCE))
			{
			    act("The room is no longer silent.",ch,NULL,NULL,TO_CHAR);
			    act("The room is no longer silent.",ch,NULL,NULL,TO_ROOM);
			    REMOVE_BIT(ch->in_room->added_flags, ROOM2_SILENCE); 
			    REMOVE_BIT(ch->more, MORE_SILENCE);
			    ch->pcdata->silence = 60;
			}
			else send_to_char("Your ability to call the Silence of Death has returned.\n\r",ch);
		    }
		}
		if (ch->pcdata->necromancy > 0)
		{
		    ch->pcdata->necromancy--;
		    if (ch->pcdata->necromancy < 1)
			send_to_char("Your powers of Necromancy have replenished themselves.\n\r",ch);
		}
		if (ch->pcdata->rpoints > 0)
		{
		    ch->pcdata->rpoints--;
		    if (ch->pcdata->rpoints < 1)
			send_to_char("You have regained sufficient strength to use another ritual.\n\r",ch);
		}
		if (ch->pcdata->wpoints > 0)
		{
		    ch->pcdata->wpoints--;
		    if (ch->pcdata->wpoints < 1)
			send_to_char("Your powers of weather control have replenished themselves.\n\r",ch);
		}
		if (ch->pcdata->spirit > 0)
		{
		    ch->pcdata->spirit--;
		    if (ch->pcdata->spirit < 1)
			send_to_char("Your powers over the spirits have replenished themselves.\n\r",ch);
		}
		if (ch->pcdata->conjure > 0)
		{
		    ch->pcdata->conjure--;
		    if (ch->pcdata->conjure < 1)
			send_to_char("Your powers of conjuration have replenished themselves.\n\r",ch);
		}
		if (ch->pcdata->celerity > 0)
		{
		    ch->pcdata->celerity--;
		    if (ch->pcdata->celerity < 1)
		    {
			if (IS_WILLPOWER(ch, RES_CELERITY) && ch->position == POS_FIGHTING)
			{
			    if (ch->blood[BLOOD_CURRENT] < 10)
			    {
				send_to_char("Not enough blood to renew Celerity.\n\r",ch);
				send_to_char("Your movements slow down.\n\r",ch);
				act("$n is no longer moving with supernatural speed.", ch, NULL, NULL, TO_ROOM);
			    }
			    else
			    {
				ch->blood[BLOOD_CURRENT] -= 10;
				ch->pcdata->celerity += 60;
				send_to_char("You burn blood to continue using your Celerity.\n\r",ch);
			    }
			}
			else
			{
			    send_to_char("Your movements slow down.\n\r",ch);
			    act("$n is no longer moving with supernatural speed.", ch, NULL, NULL, TO_ROOM);
			}
		    }
		}
		if (ch->pcdata->obeah > 0 && !IS_MORE(ch, MORE_NEUTRAL))
		{
		    ch->pcdata->obeah--;
		    if (ch->pcdata->obeah < 1)
		    {
			if (IS_WEREWOLF(ch))
			    send_to_char("Your healing powers have returned.\n\r",ch);
			else
			{
			    send_to_char("Your third eye stops glowing.\n\r",ch);
			    act("The eye in $n's forehead stops glowing.",ch,NULL,NULL,TO_ROOM);
			}
		    }
		}
		if (ch->pcdata->darkness > 0)
		{
		    ch->pcdata->darkness--;
		    if (ch->pcdata->darkness < 1)
		    {
			if (IS_MORE(ch, MORE_DARKNESS))
			{
			    act("The darkness in the room gradually fades away to nothing.",ch,NULL,NULL,TO_CHAR);
			    act("The darkness in the room gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
			    REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS); 
			    REMOVE_BIT(ch->more, MORE_DARKNESS);
			    ch->pcdata->darkness = 60;
			}
			else send_to_char("Your ability to call the Shroud of Night has returned.\n\r",ch);
		    }
		}
		if (ch->pcdata->reina > 0)
		{
		    ch->pcdata->reina--;
		    if (ch->pcdata->reina < 1)
		    {
			if (IS_MORE(ch, MORE_REINA))
			{
			    send_to_char("The peaceful harmony in the air gradually fades away to nothing.\n\r",ch);
			    act("The peaceful harmony in the air gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
			    REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE); 
			    REMOVE_BIT(ch->more, MORE_REINA);
			    ch->pcdata->reina = 60;
			}
			else send_to_char("You have recovered your voice.\n\r",ch);
		    }
		}
		if (ch->pcdata->majesty > 0)
		{
		    ch->pcdata->majesty--;
		    if (ch->pcdata->majesty < 1)
		    {
			if (IS_WEREWOLF(ch))
			    send_to_char("You can now use your Icy Chill of Despair again if you wish.\n\r",ch);
			else
			    send_to_char("You can now raise your Majesty again if you wish.\n\r",ch);
		    }
		}
		if (ch->pcdata->temporis > 0)
		{
		    ch->pcdata->temporis--;
		    if (ch->pcdata->temporis < 1)
			send_to_char("Your powers of time control have replenished themselves.\n\r",ch);
		}
		if (ch->pcdata->firstaid > 0) ch->pcdata->firstaid--;
		if (ch->pcdata->repair   > 0) ch->pcdata->repair--;
		if (ch->pcdata->evileye  > 0) ch->pcdata->evileye--;
		if (ch->pcdata->diableries[DIAB_TIME] > 0) 
		    ch->pcdata->diableries[DIAB_TIME]--;
		if (ch->pcdata->resist[WILL_ANIMALISM] > 0)
		{
		    ch->pcdata->resist[WILL_ANIMALISM]--;
		    if (ch->pcdata->resist[WILL_ANIMALISM] < 1)
		    {
			send_to_char("You are no longer immune to Beast Within or Song of Serenity.\n\r",ch);
			if (!IS_WILLPOWER(ch, RES_TOGGLE)) 
			    REMOVE_BIT(ch->pcdata->resist[0], RES_ANIMALISM);
		    }
		}
		if (ch->pcdata->resist[WILL_AUSPEX] > 0)
		{
		    ch->pcdata->resist[WILL_AUSPEX]--;
		    if (ch->pcdata->resist[WILL_AUSPEX] < 1)
		    {
			send_to_char("You are no longer immune to having your mind Unveiled.\n\r",ch);
			if (!IS_WILLPOWER(ch, RES_TOGGLE)) 
			    REMOVE_BIT(ch->pcdata->resist[0], RES_AUSPEX);
		    }
		}
		if (ch->pcdata->resist[WILL_DOMINATE] > 0)
		{
		    ch->pcdata->resist[WILL_DOMINATE]--;
		    if (ch->pcdata->resist[WILL_DOMINATE] < 1)
		    {
			send_to_char("You are no longer immune to being Dominated.\n\r",ch);
			if (!IS_WILLPOWER(ch, RES_TOGGLE)) 
			    REMOVE_BIT(ch->pcdata->resist[0], RES_DOMINATE);
		    }
		}
		if (ch->pcdata->resist[WILL_PRESENCE] > 0)
		{
		    ch->pcdata->resist[WILL_PRESENCE]--;
		    if (ch->pcdata->resist[WILL_PRESENCE] < 1)
		    {
			send_to_char("You are no longer immune to Dread Gaze and Entrancement.\n\r",ch);
			if (!IS_WILLPOWER(ch, RES_TOGGLE)) 
			    REMOVE_BIT(ch->pcdata->resist[0], RES_PRESENCE);
		    }
		}
		if (!IS_HERO(ch) && ch->hit < 1 && ch->position != POS_FIGHTING)
		{
		    char_from_room(ch);
		    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
		    act("The healer makes a few gestures and $n appears in the room.",ch,NULL,NULL,TO_ROOM);
		    ch->hit = 1;
		    update_pos(ch);
		    send_to_char("You feel a little better.\n\r",ch);
		    act("The healer says 'Are you ok $n?  Lucky I was here!'.",ch,NULL,NULL,TO_ROOM);
		    act("The healer says 'Are you ok $n?  Lucky I was here!'.",ch,NULL,NULL,TO_CHAR);
		}
	    }
	}
    }

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_mobile   <= (PULSE_MOBILE>>1) )
    {
	if ( pulse_mobile   <= 0 )
	{
	    pulse_mobile	= PULSE_MOBILE;
	    mobile_update	( );
	    obj_update		( );
	}
	else if ( pulse_mobile   == (PULSE_MOBILE>>1) )
	    obj_update		( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
	aggr_update( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
    }

    tail_chain( );
    return;
}

void paradox( CHAR_DATA *ch )
{
    char            buf[MAX_STRING_LENGTH]; 

    send_to_char("The sins of your past strike back!\n\r",  ch);
    send_to_char("The paradox has come for your soul!\n\r",  ch);
    sprintf( buf, "%s is struck by a paradox.\n\r", ch->name);
    do_info(ch, buf);
    ch->paradox[1] = 0;
    ch->paradox[2] = 0;

    ch->hit     =  -10;
    update_pos( ch );
    do_escape(ch, "");
    SET_BIT(ch->extra, TIED_UP);
    SET_BIT(ch->extra, GAGGED);
    SET_BIT(ch->extra, BLINDFOLDED);
    return;
}
