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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "imc.h"
#include "imc-mercbase.h"



/*
 * Local functions.
 */
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );
char * oldelang		args( ( CHAR_DATA *ch, char *argument ) );
char * badlang		args( ( CHAR_DATA *ch, char *argument ) );
char * darktongue	args( ( CHAR_DATA *ch, char *argument ) );
char * drunktalk	args( ( CHAR_DATA *ch, char *argument ) );
char * beasttalk	args( ( CHAR_DATA *ch, char *argument ) );
char * beastchat	args( ( CHAR_DATA *ch, char *argument ) );
char * socialc		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );
char * socialv		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );
char * socialn		args( ( CHAR_DATA *ch, char *argument, char *you, char *them ) );



bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( ch->level == LEVEL_IMPLEMENTOR )
	return TRUE;

    if ( IS_IMMORTAL(ch) && is_name( "imm", pnote->to_list ) )
	return TRUE;

    if ( IS_IMMORTAL(ch) && is_name( "immortal", pnote->to_list ) )
	return TRUE;

    if ( is_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    if ( ch->pnote != NULL )
	return;

    if ( note_free == NULL )
    {
	pnote	  = alloc_perm( sizeof(*ch->pnote) );
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    FILE *fp;
    NOTE_DATA *prev;
    char *to_list;

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " " );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for ( prev = note_list; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( prev == NULL )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->next	= note_free;
    note_free	= pnote;

    /*
     * Rewrite entire list.
     */
    fflush( fpReserve );
    fclose( fpReserve );
    if ( ( fp = fopen( NOTE_FILE, "w" ) ) == NULL )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
		pnote->sender,
		pnote->date,
		pnote->to_list,
		pnote->subject,
		pnote->text
		);
	}
	fflush( fp );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



void do_note( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTE_DATA *pnote;
    int vnum;
    int anum;

    if ( IS_NPC(ch) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( !str_cmp( arg, "list" ) )
    {
	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r",
		    vnum, pnote->sender, pnote->subject );
		send_to_char( buf, ch );
		vnum++;
	    }
	}
	return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\n\r", ch );
	    return;
	}

	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
		    vnum - 1,
		    pnote->sender,
		    pnote->subject,
		    pnote->date,
		    pnote->to_list
		    );
		send_to_char( buf, ch );
		send_to_char( pnote->text, ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch );
	strcpy( buf, ch->pnote->text );
	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 4 )
	{
	    send_to_char( "Note too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "subject" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "to" ) )
    {
	if ( strchr(argument, '@') != NULL && !IS_IMMORTAL(ch) )
	{
	    send_to_char( "Players may not send notes to other muds.\n\r",ch);
	    return;
	}
	note_attach( ch );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote != NULL )
	{
	    free_string( ch->pnote->text );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date );
	    free_string( ch->pnote->sender );
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
	    ch->pnote->sender,
	    ch->pnote->subject,
	    ch->pnote->to_list
	    );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_cmp( arg, "post" ) )
    {
	FILE *fp;
	char *strtime;

	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->pnote->date			= str_dup( strtime );

	/* Handle IMC notes */
	if ( strchr( ch->pnote->to_list, '@' ) != NULL )
	{
	    imc_post_mail( ch, ch->pnote->sender, ch->pnote->to_list,
			       ch->pnote->date,   ch->pnote->subject,
			       ch->pnote->text);
	}

	if ( note_list == NULL )
	{
	    note_list	= ch->pnote;
	}
	else
	{
	    for ( pnote = note_list; pnote->next != NULL; pnote = pnote->next )
		;
	    pnote->next	= ch->pnote;
	}
	pnote		= ch->pnote;
	ch->pnote	= NULL;

	fflush( fpReserve );
	fclose( fpReserve );
	if ( ( fp = fopen( NOTE_FILE, "a" ) ) == NULL )
	{
	    perror( NOTE_FILE );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
		pnote->sender,
		pnote->date,
		pnote->to_list,
		pnote->subject,
		pnote->text
		);
	    fflush( fp );
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	do_autosave(ch,"");
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
	if ( !is_number( argument ) )
	{
	    send_to_char( "Note remove which number?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = note_list; pnote != NULL; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && vnum++ == anum )
	    {
		note_remove( ch, pnote );
		send_to_char( "Ok.\n\r", ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    send_to_char( "Huh?  Type 'help note' for usage.\n\r", ch );
    return;
}



/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int position;

    if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	    send_to_char( "Not in Demon Form.\n\r", ch );
	else
	    send_to_char( "Not in Zulo Form.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER(buf[0]);
	return;
    }

    if ( IS_HEAD(ch, LOST_TONGUE) )
    {
	sprintf( buf, "You can't %s without a tongue!\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    if ( IS_EXTRA(ch, GAGGED) )
    {
	sprintf( buf, "You can't %s with a gag on!\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

/*
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
*/
    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_SILENCE) )
    {
	sprintf( buf, "You can't %s.\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    if (ch->in_room != NULL && IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	sprintf( buf, "You can't %s.\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {
    default:

	sprintf(buf,"You %s '$T'.", verb);
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	    act( buf, ch, NULL, drunktalk(ch, argument), TO_CHAR );
	else if (!IS_NPC(ch) && ch->pcdata->wolf > 0 && IS_VAMPIRE(ch) && 
	    !IS_SET(world_affects, WORLD_NICE) && !IS_ABOMINATION(ch))
	    act( buf, ch, NULL, beastchat(ch, argument), TO_CHAR );
	else
	    act( buf, ch, NULL, argument, TO_CHAR );
/*
	sprintf( buf, "You %s '%s'.\n\r", verb, argument );
	send_to_char( buf, ch );
*/

	sprintf( buf, "$n %ss '$t'.",     verb );
	sprintf( buf2, "$n %ss '$t'.",     verb );
	break;

    case CHANNEL_IMMTALK:
	sprintf( buf, "$n: $t." );
	sprintf( buf2, "$n: $t." );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

    case CHANNEL_BUILD:
	sprintf( buf, "$n= $t." );
	sprintf( buf2, "$n= $t." );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;

    case CHANNEL_VAMPTALK:
	if (!IS_NPC(ch) && IS_ABOMINATION(ch))
	{
	    sprintf( buf, "<($n)> $t." );
	    sprintf( buf2, "<($n)> $t." );
	}
	else if (!IS_NPC(ch) && IS_LICH(ch))
	{
	    sprintf( buf, "<{$n}> $t." );
	    sprintf( buf2, "<{$n}> $t." );
	}
	else if (!IS_NPC(ch) && IS_GHOUL(ch))
	{
	    sprintf( buf, "<$n> $t." );
	    sprintf( buf2, "<$n> $t." );
	}
	else
	{
	    sprintf( buf, "<<$n>> $t." );
	    sprintf( buf2, "<<$n>> $t." );
	}
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	&&   vch != ch
	&&  !IS_SET(och->deaf, channel) )
	{
	    if ( channel == CHANNEL_BUILD && !IS_IMMORTAL(och) )
		continue;
	    if ( channel == CHANNEL_IMMTALK && get_trust(och) <= LEVEL_BUILDER )
		continue;
	    if ( channel == CHANNEL_VAMPTALK && (!IS_NPC(och) && !IS_VAMPIRE(och)))
		continue;
	    if ( channel == CHANNEL_YELL
	    &&   vch->in_room->area != ch->in_room->area )
		continue;

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
	    if (!IS_NPC(vch) && IS_VAMPIRE(vch) && 
		strlen(ch->side) > 1 && strlen(vch->side) > 1 &&
		str_cmp(ch->side,vch->side) )
	    {
	    	act( buf2, ch, argument, vch, TO_VICT );
	    	vch->position	= position;
		continue;
	    }
	    act( buf, ch, argument, vch, TO_VICT );
	    vch->position	= position;
	}
    }

    return;
}



void do_auction( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_AUCTION, "auction" );
    return;
}



void do_chat( CHAR_DATA *ch, char *argument )
{
    if (IS_IMMORTAL(ch) && IS_EXTRA(ch, EXTRA_PSAY))
    {
	send_to_char( "Please switch Ptalk OFF first.\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_CHAT, "chat" );
    return;
}



void do_newbie( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_NEWBIE, "newbie" );
    return;
}

/*
 * Alander's new channels.
 */
void do_music( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}



void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "question" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_YELL, "yell" );
    return;
}



void do_buildtalk( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_BUILD, "buildtalk" );
    return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}



void do_vamptalk( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch) || !IS_VAMPIRE(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    talk_channel( ch, argument, CHANNEL_VAMPTALK, "vamptalk" );
    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
    char buf    [MAX_STRING_LENGTH];
    char name   [80];
    char poly   [MAX_STRING_LENGTH];
    char speak  [15];
    char speaks [15];
    char endbit [5];
    char secbit [5];
    CHAR_DATA *to;
    CHAR_DATA *vch;
    bool is_ok;

    if (IS_IMMORTAL(ch) && IS_EXTRA(ch, EXTRA_PSAY))
    {
	send_to_char( "Please switch Ptalk OFF first.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOPARTS) )
    {
	send_to_char( "You are unable to speak.\n\r", ch );
	return;
    }
    if ( IS_HEAD(ch, LOST_TONGUE) )
    {
	send_to_char( "You can't speak without a tongue!\n\r", ch );
	return;
    }
    if ( IS_EXTRA(ch, GAGGED) )
    {
	send_to_char( "You can't speak with a gag on!\n\r", ch );
	return;
    }

    if (ch->in_room != NULL && IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	send_to_char( "Your words make no sound!\n\r", ch );
	return;
    }

    if (strlen(argument) > MAX_INPUT_LENGTH)
    {
	send_to_char( "Line too long.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    /* Allow the manipulation of a const argument */
    strcpy( buf, argument );
    argument = &buf[0];

    endbit[0] = argument[strlen(argument)-1];
    endbit[1] = '\0';

    if (strlen(argument) > 1) secbit[0] = argument[strlen(argument)-2];
	else secbit[0] = '\0';
    secbit[1] = '\0';

    if (IS_BODY(ch,CUT_THROAT))
    {
	sprintf(speak,"rasp");
	sprintf(speaks,"rasps");
    }
    else if (!IS_NPC(ch) && 
	(IS_SET(ch->act,PLR_WOLFMAN) || IS_POLYAFF(ch, POLY_WOLF) 
	|| (IS_VAMPIRE(ch) && !IS_ABOMINATION(ch) && ch->pcdata->wolf > 0)))
    {
	if (number_percent() > 50)
	{
	    sprintf(speak,"growl");
	    sprintf(speaks,"growls");
	}
	else
	{
	    sprintf(speak,"snarl");
	    sprintf(speaks,"snarls");
	}
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
    {
	sprintf(speak,"squeak");
	sprintf(speaks,"squeaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
    {
	sprintf(speak,"hiss");
	sprintf(speaks,"hisses");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
    {
	sprintf(speak,"croak");
	sprintf(speaks,"croaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
    {
	sprintf(speak,"squark");
	sprintf(speaks,"squarks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	{
	    sprintf(speak,"boom");
	    sprintf(speaks,"booms");
	}
	else
	{
	    sprintf(speak,"roar");
	    sprintf(speaks,"roars");
	}
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
    {
	sprintf(speak,"croak");
	sprintf(speaks,"croaks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
    {
	sprintf(speak,"squark");
	sprintf(speaks,"squarks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
    {
	sprintf(speak,"purr");
	sprintf(speaks,"purrs");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
    {
	sprintf(speak,"bark");
	sprintf(speaks,"barks");
    }
    else if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
	sprintf(speak,"slur");
	sprintf(speaks,"slurs");
    }
    else if (IS_MORE(ch, MORE_SINGING))
    {
	sprintf(speak,"sing");
	sprintf(speaks,"sings");
	REMOVE_BIT(ch->more, MORE_SINGING);
    }
    else if (!IS_NPC(ch) && (ch->pcdata->exercise[EXE_COUNT] > 10 || 
	ch->pcdata->exercise[EXE_RECOVER] > 10))
    {
	sprintf(speak,"pant");
	sprintf(speaks,"pants");
    }
/*
    else if (IS_MORE(ch, MORE_INFORM))
    {
	sprintf(speak,"announce");
	sprintf(speaks,"announces");
    }
*/
    else if (!str_cmp(endbit,"!"))
    {
	sprintf(speak,"exclaim");
	sprintf(speaks,"exclaims");
    }
    else if (!str_cmp(endbit,"?"))
    {
	sprintf(speak,"ask");
	sprintf(speaks,"asks");
    }
    else if (secbit[0] != '\0' && str_cmp(secbit,".") && !str_cmp(endbit,"."))
    {
	sprintf(speak,"state");
	sprintf(speaks,"states");
    }
    else if (secbit[0] != '\0' && !str_cmp(secbit,".") && !str_cmp(endbit,"."))
    {
	sprintf(speak,"mutter");
	sprintf(speaks,"mutters");
    }
    else if (secbit[0] != '\0' && (!str_cmp(secbit,":") || 
	!str_cmp(secbit,"=") || !str_cmp(secbit,";")) && !str_cmp(endbit,")"))
    {
	sprintf(speak,"grin");
	sprintf(speaks,"grins");
    }
    else if (secbit[0] != '\0' && !str_cmp(secbit,":") && !str_cmp(endbit,"("))
    {
	sprintf(speak,"frown");
	sprintf(speaks,"frowns");
    }
    else
    {
	sprintf(speak,"say");
	sprintf(speaks,"says");
    }
    sprintf(poly,"You %s '$T'.", speak);
    if (!IS_NPC(ch))
    {
	if (IS_POLYAFF(ch, POLY_ZULO))
	    act( poly, ch, NULL, darktongue(ch, argument), TO_CHAR );
	else if (ch->pcdata->condition[COND_DRUNK] > 10)
	    act( poly, ch, NULL, drunktalk(ch, argument), TO_CHAR );
	else if (ch->pcdata->wolf > 0 && IS_VAMPIRE(ch) && 
	    !IS_SET(world_affects, WORLD_NICE) && !IS_ABOMINATION(ch))
	    act( poly, ch, NULL, beasttalk(ch, argument), TO_CHAR );
	else if (IS_SPEAKING(ch,DIA_OLDE))
	    act( poly, ch, NULL, oldelang(ch, argument), TO_CHAR );
	else if (IS_SPEAKING(ch,DIA_BAD))
	    act( poly, ch, NULL, badlang(ch, argument), TO_CHAR );
	else
	    act( poly, ch, NULL, argument, TO_CHAR );
    }
    else
	act( poly, ch, NULL, argument, TO_CHAR );

    sprintf(poly,"$n %s '$T'.", speaks);

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
	act( poly, ch, NULL, argument, TO_ROOM );
	if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
	{
	    if ( ( vch = ch->desc->snoop_by->character ) == NULL )
		ch->desc->snoop_by = NULL;
	    else if ( ch->in_room == NULL || vch->in_room == NULL )
		bug( "In_room bug - KaVir.", 0 );
	    else if (!IS_NPC(vch) && IS_MORE(ch, MORE_INFORM))
	    {
		if (ch->pcdata->lie == LIE_TRUE)
		    act("$n is lying.",ch,NULL,vch,TO_VICT);
		else
		    act("$n is telling the truth.",ch,NULL,vch,TO_VICT);
	    }
	}
    	room_text( ch, strlower(argument) );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
	else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s '%s'.\n\r", name,speaks,argument);
	send_to_char(poly,to);
    }

    room_text( ch, strlower(argument) );
    return;
}

void do_psay( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    char arg3 [MAX_STRING_LENGTH];

    char poly1  [MAX_STRING_LENGTH];
    char poly2  [MAX_STRING_LENGTH];
    char speak1  [15];
    char speaks1 [15];
    char endbit1 [5];
    char secbit1 [5];
    char speak2  [15];
    char speaks2 [15];
    char endbit2 [5];
    char secbit2 [5];

    CHAR_DATA *victim;

    smash_tilde(argument);
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if (!IS_EXTRA(ch, EXTRA_PSAY))
    {
	send_to_char( "Please switch Ptalk ON first.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOPARTS) )
    {
	send_to_char( "You are unable to speak.\n\r", ch );
	return;
    }
    if ( IS_HEAD(ch, LOST_TONGUE) )
    {
	send_to_char( "You can't speak without a tongue!\n\r", ch );
	return;
    }
    if ( IS_EXTRA(ch, GAGGED) )
    {
	send_to_char( "You can't speak with a gag on!\n\r", ch );
	return;
    }

    if (ch->in_room != NULL && IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	send_to_char( "Your words make no sound!\n\r", ch );
	return;
    }

    if (strlen(arg2) > MAX_INPUT_LENGTH || strlen(arg3) > MAX_INPUT_LENGTH)
    {
	send_to_char( "Line too long.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: Psay <person> <their message> <everyone elses message>\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 3 || strlen(arg3) < 3 )
    {
	send_to_char( "Syntax: Psay <person> <their message> <everyone elses message>\n\r", ch );
	return;
    }

    if ( argument[0] != '\0' )
    {
	send_to_char( "Please use quotes around what you wish to say.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    endbit1[0] = arg2[strlen(arg2)-1];
    endbit1[1] = '\0';

    endbit2[0] = arg3[strlen(arg3)-1];
    endbit2[1] = '\0';

    if (strlen(arg2) > 3) secbit1[0] = arg2[strlen(arg2)-2];
	else secbit1[0] = '\0';
    secbit1[1] = '\0';
    if (strlen(arg3) > 3) secbit2[0] = arg3[strlen(arg3)-2];
	else secbit2[0] = '\0';
    secbit2[1] = '\0';

    if (IS_BODY(ch,CUT_THROAT))
    {
	sprintf(speak1,"rasp");
	sprintf(speaks1,"rasps");
    }
    else if (!IS_NPC(ch) && 
	(IS_SET(ch->act,PLR_WOLFMAN) || IS_POLYAFF(ch, POLY_WOLF) 
	|| (IS_VAMPIRE(ch) && !IS_ABOMINATION(ch) && ch->pcdata->wolf > 0)))
    {
	if (number_percent() > 50)
	{
	    sprintf(speak1,"growl");
	    sprintf(speaks1,"growls");
	}
	else
	{
	    sprintf(speak1,"snarl");
	    sprintf(speaks1,"snarls");
	}
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
    {
	sprintf(speak1,"squeak");
	sprintf(speaks1,"squeaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
    {
	sprintf(speak1,"hiss");
	sprintf(speaks1,"hisses");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
    {
	sprintf(speak1,"croak");
	sprintf(speaks1,"croaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
    {
	sprintf(speak1,"squark");
	sprintf(speaks1,"squarks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	{
	    sprintf(speak1,"boom");
	    sprintf(speaks1,"booms");
	}
	else
	{
	    sprintf(speak1,"roar");
	    sprintf(speaks1,"roars");
	}
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
    {
	sprintf(speak1,"croak");
	sprintf(speaks1,"croaks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
    {
	sprintf(speak1,"squark");
	sprintf(speaks1,"squarks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
    {
	sprintf(speak1,"purr");
	sprintf(speaks1,"purrs");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
    {
	sprintf(speak1,"bark");
	sprintf(speaks1,"barks");
    }
    else if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
	sprintf(speak1,"slur");
	sprintf(speaks1,"slurs");
    }
    else if (IS_MORE(ch, MORE_SINGING))
    {
	sprintf(speak1,"sing");
	sprintf(speaks1,"sings");
    }
    else if (!IS_NPC(ch) && (ch->pcdata->exercise[EXE_COUNT] > 10 || 
	ch->pcdata->exercise[EXE_RECOVER] > 10))
    {
	sprintf(speak1,"pant");
	sprintf(speaks1,"pants");
    }
    else if (!str_cmp(endbit1,"!"))
    {
	sprintf(speak1,"exclaim");
	sprintf(speaks1,"exclaims");
    }
    else if (!str_cmp(endbit1,"?"))
    {
	sprintf(speak1,"ask");
	sprintf(speaks1,"asks");
    }
    else if (secbit1[0] != '\0' && str_cmp(secbit1,".") && !str_cmp(endbit1,"."))
    {
	sprintf(speak1,"state");
	sprintf(speaks1,"states");
    }
    else if (secbit1[0] != '\0' && !str_cmp(secbit1,".") && !str_cmp(endbit1,"."))
    {
	sprintf(speak1,"mutter");
	sprintf(speaks1,"mutters");
    }
    else if (secbit1[0] != '\0' && (!str_cmp(secbit1,":") || 
	!str_cmp(secbit1,"=") || !str_cmp(secbit1,";")) && !str_cmp(endbit1,")"))
    {
	sprintf(speak1,"grin");
	sprintf(speaks1,"grins");
    }
    else if (secbit1[0] != '\0' && !str_cmp(secbit1,":") && !str_cmp(endbit1,"("))
    {
	sprintf(speak1,"frown");
	sprintf(speaks1,"frowns");
    }
    else
    {
	sprintf(speak1,"say");
	sprintf(speaks1,"says");
    }

    if (IS_BODY(ch,CUT_THROAT))
    {
	sprintf(speak2,"rasp");
	sprintf(speaks2,"rasps");
    }
    else if (!IS_NPC(ch) && 
	(IS_SET(ch->act,PLR_WOLFMAN) || IS_POLYAFF(ch, POLY_WOLF) 
	|| (IS_VAMPIRE(ch) && !IS_ABOMINATION(ch) && ch->pcdata->wolf > 0)))
    {
	if (number_percent() > 50)
	{
	    sprintf(speak2,"growl");
	    sprintf(speaks2,"growls");
	}
	else
	{
	    sprintf(speak2,"snarl");
	    sprintf(speaks2,"snarls");
	}
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
    {
	sprintf(speak2,"squeak");
	sprintf(speaks2,"squeaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
    {
	sprintf(speak2,"hiss");
	sprintf(speaks2,"hisses");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
    {
	sprintf(speak2,"croak");
	sprintf(speaks2,"croaks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
    {
	sprintf(speak2,"squark");
	sprintf(speaks2,"squarks");
    }
    else if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	{
	    sprintf(speak2,"boom");
	    sprintf(speaks2,"booms");
	}
	else
	{
	    sprintf(speak2,"roar");
	    sprintf(speaks2,"roars");
	}
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
    {
	sprintf(speak2,"croak");
	sprintf(speaks2,"croaks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
    {
	sprintf(speak2,"squark");
	sprintf(speaks2,"squarks");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
    {
	sprintf(speak2,"purr");
	sprintf(speaks2,"purrs");
    }
    else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
    {
	sprintf(speak2,"bark");
	sprintf(speaks2,"barks");
    }
    else if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
	sprintf(speak2,"slur");
	sprintf(speaks2,"slurs");
    }
    else if (IS_MORE(ch, MORE_SINGING))
    {
	sprintf(speak2,"sing");
	sprintf(speaks2,"sings");
	REMOVE_BIT(ch->more, MORE_SINGING);
    }
    else if (!IS_NPC(ch) && (ch->pcdata->exercise[EXE_COUNT] > 10 || 
	ch->pcdata->exercise[EXE_RECOVER] > 10))
    {
	sprintf(speak2,"pant");
	sprintf(speaks2,"pants");
    }
    else if (!str_cmp(endbit2,"!"))
    {
	sprintf(speak2,"exclaim");
	sprintf(speaks2,"exclaims");
    }
    else if (!str_cmp(endbit2,"?"))
    {
	sprintf(speak2,"ask");
	sprintf(speaks2,"asks");
    }
    else if (secbit2[0] != '\0' && str_cmp(secbit2,".") && !str_cmp(endbit2,"."))
    {
	sprintf(speak2,"state");
	sprintf(speaks2,"states");
    }
    else if (secbit2[0] != '\0' && !str_cmp(secbit2,".") && !str_cmp(endbit2,"."))
    {
	sprintf(speak2,"mutter");
	sprintf(speaks2,"mutters");
    }
    else if (secbit2[0] != '\0' && (!str_cmp(secbit2,":") || 
	!str_cmp(secbit2,"=") || !str_cmp(secbit2,";")) && !str_cmp(endbit2,")"))
    {
	sprintf(speak2,"grin");
	sprintf(speaks2,"grins");
    }
    else if (secbit2[0] != '\0' && !str_cmp(secbit2,":") && !str_cmp(endbit2,"("))
    {
	sprintf(speak2,"frown");
	sprintf(speaks2,"frowns");
    }
    else
    {
	sprintf(speak2,"say");
	sprintf(speaks2,"says");
    }

    sprintf(poly1,"To %s: You %s '$T'.", victim->name, speak1);
    sprintf(poly2,"To others: You %s '$T'.", speak2);

    act( poly1, ch, NULL, arg2, TO_CHAR );
    act( poly2, ch, NULL, arg3, TO_CHAR );

    sprintf(poly1,"$n %s '$t'.", speaks1);
    sprintf(poly2,"$n %s '$t'.", speaks2);

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
	act( poly1, ch, arg2, victim, TO_VICT );
	act( poly2, ch, arg3, victim, TO_NOTVICT );
    	room_text( ch, strlower(arg2) );
	return;
    }
    room_text( ch, strlower(arg2) );
    return;
}

void room_text( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA     *vch;
    CHAR_DATA     *vch_next;
    CHAR_DATA     *mob;
    OBJ_DATA      *obj;
    ROOMTEXT_DATA *rt;
    bool           mobfound;
    bool           hop;

    char           arg  [MAX_INPUT_LENGTH];
    char           arg1 [MAX_INPUT_LENGTH];
    char           arg2 [MAX_INPUT_LENGTH];

    for ( rt = ch->in_room->roomtext; rt != NULL; rt = rt->next )
    {
	if (!strcmp(argument,rt->input)
	 || is_in(argument, rt->input)
	 || all_in(argument, rt->input)) 
	{
	    if ( rt->name != NULL         && rt->name != '\0'
	    &&   str_cmp(rt->name,"all")  && str_cmp(rt->name,"|all*") ) 
	    	if (!is_in(ch->name, rt->name) ) continue;
	    mobfound = TRUE;
	    if (rt->mob != 0)
	    {
		mobfound = FALSE;
	    	for ( vch = char_list; vch != NULL; vch = vch_next )
	    	{
		    vch_next	= vch->next;
		    if ( vch->in_room == NULL ) continue;
		    if ( !IS_NPC(vch)         ) continue;
		    if ( vch->in_room == ch->in_room
			&& vch->pIndexData->vnum == rt->mob )
		    {
			mobfound = TRUE;
		    	break;
		    }
	    	}
	    }
	    if (!mobfound) continue;
	    hop = FALSE;
	    switch(rt->type % RT_RETURN)
	    {
		case RT_SAY:
		    break;
		case RT_LIGHTS:
		    do_changelight(ch,"");
		    break;
		case RT_LIGHT:
		    REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
		    break;
		case RT_DARK:
		    SET_BIT(ch->in_room->room_flags, ROOM_DARK);
		    break;
		case RT_OBJECT:
		    if ( get_obj_index(rt->power) == NULL ) return;
		    obj = create_object(get_obj_index(rt->power), ch->level);
		    if (IS_SET(rt->type, RT_TIMER) ) obj->timer = 60;
		    if (ch->carry_weight + get_obj_weight(obj) > 
		    can_carry_w(ch) || ch->carry_number + 1 > can_carry_n(ch))
			extract_obj(obj);
		    else
		    {
			if (CAN_WEAR(obj,ITEM_TAKE)) obj_to_char(obj,ch);
			else obj_to_room(obj,ch->in_room);
			if (!str_cmp(rt->choutput,"copy"))
		    	    act( rt->output, ch, obj, NULL, TO_CHAR );
			else
		    	    act( rt->choutput, ch, obj, NULL, TO_CHAR );
			if (!IS_SET(rt->type, RT_PERSONAL) ) 
			    act( rt->output, ch, obj, NULL, TO_ROOM );
		    }
		    hop = TRUE;
		    break;
		case RT_MOBILE:
		    if ( get_mob_index(rt->power) == NULL ) return;
		    mob = create_mobile(get_mob_index(rt->power));
		    char_to_room(mob,ch->in_room);
		    if (!str_cmp(rt->choutput,"copy"))
		    	act( rt->output, ch, NULL, mob, TO_CHAR );
		    else
		    	act( rt->choutput, ch, NULL, mob, TO_CHAR );
		    if (!IS_SET(rt->type, RT_PERSONAL) ) 
			act( rt->output, ch, NULL, mob, TO_ROOM );
		    hop = TRUE;
		    break;
		case RT_SPELL:
		    (*skill_table[rt->power].spell_fun) ( rt->power, number_range(20,30), ch, ch );
		    break;
		case RT_PORTAL:
		    if ( get_obj_index(OBJ_VNUM_PORTAL) == NULL ) return;
		    obj = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
		    obj->timer = 5;
		    obj->value[0] = rt->power;
		    obj->value[1] = 1;
		    obj_to_room(obj,ch->in_room);
		    break;
		case RT_TELEPORT:
		    if ( get_room_index(rt->power) == NULL ) return;
		    if (!str_cmp(rt->choutput,"copy"))
		    	act( rt->output, ch, NULL, NULL, TO_CHAR );
		    else
		    	act( rt->choutput, ch, NULL, NULL, TO_CHAR );
		    if (!IS_SET(rt->type, RT_PERSONAL) ) 
			act( rt->output, ch, NULL, NULL, TO_ROOM );
		    char_from_room(ch);
		    char_to_room(ch,get_room_index(rt->power));
		    act("$n appears in the room.",ch,NULL,NULL,TO_ROOM);
		    do_look(ch,"auto");
		    hop = TRUE;
		    break;
		case RT_ACTION:
		    sprintf(arg,argument);
		    argument = one_argument( arg, arg1 );
		    argument = one_argument( arg, arg2 );
		    if ( (mob = get_char_room(ch, arg2) ) == NULL ) continue;
		    if (IS_NPC(mob)) SET_BIT(mob->act, ACT_COMMANDED);
		    interpret( mob, rt->output );
		    if (IS_NPC(mob)) REMOVE_BIT(mob->act, ACT_COMMANDED);
		    break;
		case RT_OPEN_LIFT:
		    open_lift(ch);
		    break;
		case RT_CLOSE_LIFT:
		    close_lift(ch);
		    break;
		case RT_MOVE_LIFT:
		    move_lift(ch,rt->power);
		    break;
		default:
		    break;
	    }
	    if (hop && IS_SET(rt->type, RT_RETURN) ) return;
	    else if (hop) continue;
	    if (!str_cmp(rt->choutput,"copy") && !IS_SET(rt->type, RT_ACTION))
	    	act( rt->output, ch, NULL, NULL, TO_CHAR );
	    else if (!IS_SET(rt->type, RT_ACTION))
	    	act( rt->choutput, ch, NULL, NULL, TO_CHAR );
	    if (!IS_SET(rt->type, RT_PERSONAL) && !IS_SET(rt->type, RT_ACTION) )
		act( rt->output, ch, NULL, NULL, TO_ROOM );
	    if (IS_SET(rt->type, RT_RETURN) ) return;
	}
    }
    return;
}

char *strlower(char *ip)
{
	static char buffer[MAX_STRING_LENGTH];
	int pos;

	for (pos = 0; pos < (MAX_STRING_LENGTH - 1) && ip[pos] != '\0'; pos++) {
		buffer[pos] = tolower(ip[pos]);
	}	
	buffer[pos] = '\0';
	return buffer;
}

bool is_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_STRING_LENGTH];
	int fitted;

	if (ip[0] != '|')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do {
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if (strstr(lo_arg, cmp) != NULL) {
			return TRUE;
		}
	} while (fitted > 0);
	return FALSE;
}

bool all_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_STRING_LENGTH];
	int fitted;

	if (ip[0] != '&')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do {
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if (strstr(lo_arg, cmp) == NULL) {
			return FALSE;
		}
	} while (fitted > 0);
	return TRUE;
}	

void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char poly [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;

    if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	    send_to_char( "Not in Demon Form.\n\r", ch );
	else
	    send_to_char( "Not in Zulo Form.\n\r", ch );
	return;
    }
/*
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
*/
    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (ch->in_room != NULL && IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( IS_SET(victim->deaf, CHANNEL_TELL) )
    {
	if (IS_NPC(victim) || IS_NPC(ch) || strlen(victim->pcdata->marriage) < 2
	    || str_cmp(ch->name, victim->pcdata->marriage))
	{
	    act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	    return;
	}
    }

    sprintf(poly,"You tell %s '$t'.", 
	IS_NPC(victim) ? victim->short_descr : victim->name);
    act( poly, ch, argument, victim, TO_CHAR );

    position		= victim->position;
    victim->position	= POS_STANDING;

    sprintf(poly,"%s tells you '$t'.",
	IS_NPC(ch) ? ch->short_descr : ch->name);
    ADD_COLOUR(victim,poly,WHITE);
    act( poly, ch, argument, victim, TO_VICT );

    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Not with a gag on!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Syntax: whisper <person> <message>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_AWAKE(victim) )
    {
	act( "$E cannot hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }

    act( "You whisper to $N '$t'.", ch, argument, victim, TO_CHAR );
    act( "$n whispers to you '$t'.", ch, argument, victim, TO_VICT );
    act( "$n whispers something to $N.", ch, NULL, victim, TO_NOTVICT );

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    char poly [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;

/*
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
*/
    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (ch->in_room != NULL && IS_SET(ch->in_room->added_flags, ROOM2_SILENCE))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if (IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	act( "$E is currently link dead.", ch, 0, victim, TO_CHAR );
	return;
    }

    sprintf(poly,"You reply to %s '$t'.", 
	IS_NPC(victim) ? victim->short_descr : victim->name);
    act( poly, ch, argument, victim, TO_CHAR );

    position		= victim->position;
    victim->position	= POS_STANDING;

    sprintf(poly,"%s replies '$t'.",
	IS_NPC(ch) ? ch->short_descr : ch->name);
    ADD_COLOUR(victim,poly,WHITE);
    act( poly, ch, argument, victim, TO_VICT );

    victim->position	= position;
    victim->reply	= ch;

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;

    char name   [80];
    char poly   [MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    bool is_ok;

    if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	    send_to_char( "Not in Demon Form.\n\r", ch );
	else
	    send_to_char( "Not in Zulo Form.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOPARTS) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Pose what?\n\r", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );

    act( "$n $T", ch, NULL, buf, TO_CHAR );

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
    	act( "$n $T", ch, NULL, buf, TO_ROOM );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
	else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s\n\r", name,buf);
	send_to_char(poly,to);
    }
    return;
}



void do_xemote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char oldarg[MAX_STRING_LENGTH];
    char *plast;

    char name   [80];
    char you    [80];
    char them   [80];
    char poly   [MAX_INPUT_LENGTH];
    char arg    [MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *victim;
    bool is_ok;

    argument = one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_POLYAFF(ch, POLY_ZULO))
    {
	if ( IS_DEMON(ch) )
	    send_to_char( "Not in Demon Form.\n\r", ch );
	else
	    send_to_char( "Not in Zulo Form.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOPARTS) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if ( IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if (strlen(argument) > MAX_INPUT_LENGTH)
    {
	send_to_char( "Line too long.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
	send_to_char( "Syntax: emote <person> <sentence>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(ch)) strcpy(you, ch->short_descr);
	else strcpy(you, ch->name);
    if (IS_NPC(victim)) strcpy(you, victim->short_descr);
	else strcpy(you, victim->name);
/*
oldarg = argument;
*/
    strcpy( oldarg,argument );
    strcpy( buf, argument );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    if ( isalpha(plast[-1]) )
	strcat( buf, "." );
    argument = socialc(ch, buf, you, them);

    strcpy( buf, argument );
    strcpy( buf2, "You ");
    buf[0] = LOWER(buf[0]);
    strcat( buf2, buf );
    capitalize( buf2 );
    act( buf2, ch, NULL, victim, TO_CHAR );

    if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
    {
	strcpy( buf, oldarg );
	for ( plast = argument; *plast != '\0'; plast++ )
	    ;
	if ( isalpha(plast[-1]) )
	    strcat( buf, "." );

	argument = socialn(ch, buf, you, them);

	strcpy( buf, argument );
	strcpy( buf2, "$n ");
	buf[0] = LOWER(buf[0]);
	strcat( buf2, buf );
	capitalize( buf2 );
    	act( buf2, ch, NULL, victim, TO_NOTVICT );

	strcpy( buf, oldarg );
	for ( plast = argument; *plast != '\0'; plast++ )
	    ;
	if ( isalpha(plast[-1]) )
	    strcat( buf, "." );

	argument = socialv(ch, buf, you, them);

	strcpy( buf, argument );
	strcpy( buf2, "$n ");
	buf[0] = LOWER(buf[0]);
	strcat( buf2, buf );
	capitalize( buf2 );
    	act( buf2, ch, NULL, victim, TO_VICT );
	return;
    }

    to = ch->in_room->people;
    for ( ; to != NULL; to = to->next_in_room )
    {
	is_ok = FALSE;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

	if ( ch == to )
	    continue;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_room != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_room != NULL &&
	    ch->in_room == to->in_room)
		is_ok = TRUE; else is_ok = FALSE;

	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    ch->pcdata->chobj->in_obj != NULL &&
	    !IS_NPC(to) && to->pcdata->chobj != NULL && 
	    to->pcdata->chobj->in_obj != NULL &&
	    ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		is_ok = TRUE; else is_ok = FALSE;

	if (!is_ok) continue;

	if (IS_NPC(ch))
	    sprintf(name, ch->short_descr);
	else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
	    sprintf(name, ch->morph);
	else
	    sprintf(name, ch->name);
	name[0]=UPPER(name[0]);
	sprintf(poly,"%s %s\n\r", name,buf);
	send_to_char(poly,to);
    }
    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *mount;
    int temp_num;

    smash_tilde(argument);

    if ( IS_NPC(ch) )
    {
	if (ch->wizard != NULL)
	{
	    if (IS_AFFECTED(ch, AFF_ETHEREAL)) return;
	}
	send_to_char( "\n\r                         Speak not: whisper not:\n\r",ch);
	send_to_char( "                      I know all that ye would tell,\n\r",ch);
	send_to_char( "                    But to speak might break the spell\n\r",ch);
	send_to_char( "                      Which must bend the invincible,\n\r",ch);
	send_to_char( "                          The stern of thought;\n\r",ch);
	send_to_char( "                 He yet defies the deepest power of Hell.\n\r\n\r",ch );
    	act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
	sprintf( buf, "%s has left the God Wars.", ch->short_descr );
	buf[0] = UPPER(buf[0]);
/*
	do_info(ch,buf);
*/
	extract_char( ch, TRUE );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_SLEEPING )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }
    if ( !IS_NPC(ch) && IS_MORE(ch, MORE_DARKNESS))
    {
	if (ch->in_room != NULL)
	{
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS); 
	    REMOVE_BIT(ch->more, MORE_DARKNESS);
	    ch->pcdata->darkness = 60;
	    send_to_char("The darkness in the room gradually fades away to nothing.\n\r",ch);
	    act("The darkness in the room gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
	}
    }
    if ( !IS_NPC(ch) && IS_MORE(ch, MORE_SILENCE))
    {
	if (ch->in_room != NULL)
	{
	    REMOVE_BIT(ch->in_room->added_flags, ROOM2_SILENCE);
	    REMOVE_BIT(ch->more, MORE_SILENCE);
	    ch->pcdata->silence = 60;
	    send_to_char("The room is no longer silent.\n\r",ch);
	    act("The room is no longer silent.",ch,NULL,NULL,TO_ROOM);
	}
    }
    if ( !IS_NPC(ch) && IS_MORE(ch, MORE_REINA))
    {
	if (ch->in_room != NULL)
	{
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE); 
	    REMOVE_BIT(ch->more, MORE_REINA);
	    ch->pcdata->reina = 60;
	    send_to_char("The peaceful harmony in the air gradually fades away to nothing.\n\r",ch);
	    act("The peaceful harmony in the air gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
	}
    }
    if ( !IS_NPC(ch) && IS_MORE(ch, MORE_NEUTRAL))
    {
	if (ch->in_room != NULL)
	{
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE); 
	    REMOVE_BIT(ch->more, MORE_NEUTRAL);
	    ch->pcdata->obeah = 30;
	    send_to_char("The peaceful harmony in the air gradually fades away to nothing.\n\r",ch);
	    act("The peaceful harmony in the air gradually fades away to nothing.",ch,NULL,NULL,TO_ROOM);
	}
    }

    temp_num = get_totalscore(ch);

    if ( (mount = ch->mount) != NULL ) do_dismount(ch,"");
/*
    send_to_char( "\n\r           I'm a lean dog, a keen dog, a wild dog, and lone;\n\r",ch);
    send_to_char( "           I'm a rough dog, a tough dog, hunting on my own;\n\r",ch);
    send_to_char( "           I'm a bad dog, a mad dog, teasing silly sheep;\n\r",ch);
    send_to_char( "           I love to sit and bay the moon, to keep fat souls from sleep.\n\r\n\r",ch);

    send_to_char( "\n\rIn a small section of the gardan a tiny weed spoke to the blooms that \n\r",ch);
    send_to_char( "grew there. 'Why,' he asked, 'does the gardener seek to kill me?  Do I \n\r",ch);
    send_to_char( "not have a right to life?  Are my leaves not green, as yours are?  Is it \n\r",ch);
    send_to_char( "too much to ask that I be allowed to grow and see the sun?'  The blooms \n\r",ch);
    send_to_char( "pondered on this, and decided to ask the gardener to spare the weed.  He \n\r",ch);
    send_to_char( "did so.  Day by day the weed grew, stronger and stronger, taller and \n\r",ch);
    send_to_char( "taller, its leaves covering the other plants, its roots spreading.  One \n\r",ch);
    send_to_char( "by one the flowers died, until only a rose was left.  It gazed up at the \n\r",ch);
    send_to_char( "enormous weed and asked: 'Why do you seek to kill me?  Do I not have a \n\r",ch);
    send_to_char( "right to life?  Are my leaves not green, as yours are?  Is it too much to \n\r",ch);
    send_to_char( "ask that I be allowed to grow and see the sun?'\n\r",ch);
    send_to_char( "'Yes, it is too much to ask,' said the weed.\n\r\n\r",ch);

    send_to_char( "\n\rWhen will we have peace?  That is the cry upon the lips of the multitude.\n\r",ch);
    send_to_char( "I hear it.  I understand it.  The answer is not easy to voice, and it is \n\r",ch);
    send_to_char( "harder to hear.  Peace does not come when the brigands are slain.  It is \n\r",ch);
    send_to_char( "not born with the end of a current War. It does not arrive with the \n\r",ch);
    send_to_char( "beauty of the spring.  Peace is a gift of the grave, and is found only in \n\r",ch);
    send_to_char( "the silence of the tomb.\n\r\n\r",ch);
*/

    send_to_char( "\n\r                         Speak not: whisper not:\n\r",ch);
    send_to_char( "                      I know all that ye would tell,\n\r",ch);
    send_to_char( "                    But to speak might break the spell\n\r",ch);
    send_to_char( "                      Which must bend the invincible,\n\r",ch);
    send_to_char( "                          The stern of thought;\n\r",ch);
    send_to_char( "                 He yet defies the deepest power of Hell.\n\r\n\r",ch );

    /*
     * After extract_char the ch is no longer valid!
     */
    d = ch->desc;
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_NONE) continue;
    	if ( !IS_NPC(ch) && ((obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	    obj->chobj->pcdata->obj_vnum != 0) || obj->item_type == ITEM_KEY))
	{
	    unequip_char(ch,obj);
	    extract_obj(obj);
	}
    }
    do_autosave(ch,"");
    if (ch->pcdata->obj_vnum != 0)
    	act( "$n slowly fades out of existance.", ch, NULL, NULL, TO_ROOM );
    else
    	act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );

    if ( d != NULL ) close_socket2( d );

    if (ch->in_room != NULL) char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_DISCONNECTION));

    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );

    if ( ( obj = ch->pcdata->chobj ) != NULL )
    {
	obj->chobj = NULL;
	ch->pcdata->chobj = NULL;
	extract_obj(obj);
    }
    else if (ch->pcdata->obj_vnum == 0)
    {
	sprintf( buf, "%s has left the God Wars.", ch->name );
	do_info(ch,buf);
    }
    extract_char( ch, TRUE );
    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_IMMORTAL(ch) && ch->mkill < 5)
    {
	send_to_char( "You must kill at least 5 mobs before you can save.\n\r", ch );
	return;
    }

    get_totalscore(ch);
/*
    save_char_obj_backup( ch );
*/
    save_char_obj( ch );
    send_to_char( "Saved.\n\r", ch );
    return;
}



void do_autosave( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if ( ch->level < 2 ) return;
    if ( ch->mkill < 5 && !IS_IMMORTAL(ch)) return;
    save_char_obj( ch );
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }
    
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) )
	act( "$n stops following you.", ch, NULL, ch->master, TO_VICT );
    act( "You stop following $N.", ch, NULL, ch->master, TO_CHAR    );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;

    if ( ch->master != NULL )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next;
	if (fch->leader == ch) fch->leader = fch;
	if (fch->master == ch) stop_follower( fch );
	if (!IS_NPC(ch) && IS_NPC(fch) && fch->lord != NULL)
	{
	    if (strlen(fch->lord) > 1 && !str_cmp(fch->lord,ch->name))
	    {
		act("$n slowly fades out of existance.",fch,NULL,NULL,TO_ROOM);
		extract_char(fch, TRUE);
	    }
	}
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;
    bool is_animal = FALSE;
    bool is_spirit = FALSE;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if (is_in(argument,"|quit*rem*drop*gift*claim*consent*propose*accept*vampi*ma *maj*sid *side *reca*/*ha *hav*"))
	{
	    send_to_char( "You cannot make that sort of order.\n\r", ch );
	    return;
	}

	if (IS_NPC(victim)) switch ( victim->pIndexData->vnum )
	{
	    default: is_animal = FALSE; break;
	    case 3062:
	    case 3066:
	    case 5333:
	    case 30006:
	    case 30007:
	    case 30008:
	    case 30009:
	    case 30010:
	    case 30013:
	    case 30014: is_animal = TRUE; break;
	}
	else is_animal = FALSE;
	if (IS_SPIRIT(victim)) is_spirit = TRUE; else is_spirit = FALSE;
	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    if (IS_NPC(ch) || IS_NPC(victim) || 
		strlen(victim->pcdata->love) < 2 || 
		str_cmp(victim->pcdata->love, ch->name))
	    {
		if (!is_animal || !IS_MORE(ch, MORE_ANIMAL_MASTER))
		{
		    if (!is_spirit || !IS_WEREWOLF(ch) || get_disc(ch,AUSPICE_THEURGE) < 3)
		    {
			send_to_char( "Do it yourself!\n\r", ch );
			return;
		    }
		}
	    }
	}
    }

    found = FALSE;
    is_animal = FALSE;
    is_spirit = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;
	if ( och == ch ) continue;
	if (IS_NPC(och)) switch ( och->pIndexData->vnum )
	{
	    default: is_animal = FALSE; break;
	    case 3062:
	    case 3066:
	    case 5333:
	    case 30006:
	    case 30007:
	    case 30008:
	    case 30009:
	    case 30010:
	    case 30013:
	    case 30014: is_animal = TRUE; break;
	}
	else is_animal = FALSE;
	if (IS_SPIRIT(och)) is_spirit = TRUE; else is_spirit = FALSE;
	if ((IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) ) )
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    if (IS_NPC(och)) SET_BIT(och->act, ACT_COMMANDED);
	    interpret( och, argument );
	    if (IS_NPC(och)) REMOVE_BIT(och->act, ACT_COMMANDED);
	    WAIT_STATE(ch,12);
	}
	else if (!IS_NPC(ch) && !IS_NPC(och) && strlen(och->pcdata->love) > 1 &&
	!str_cmp(och->pcdata->love,ch->name) && ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    if (IS_NPC(och)) SET_BIT(och->act, ACT_COMMANDED);
	    interpret( och, argument );
	    if (IS_NPC(och)) REMOVE_BIT(och->act, ACT_COMMANDED);
	    WAIT_STATE(ch,12);
	}
	else if (is_animal && IS_MORE(ch, MORE_ANIMAL_MASTER) && ( fAll || och == victim ))
	{
	    found = TRUE;
	    act( "$n asks you to '$t'.", ch, argument, och, TO_VICT );
	    if (IS_NPC(och)) SET_BIT(och->act, ACT_COMMANDED);
	    interpret( och, argument );
	    if (IS_NPC(och)) REMOVE_BIT(och->act, ACT_COMMANDED);
	    WAIT_STATE(ch,12);
	}
	else if (is_spirit && IS_WEREWOLF(ch) && get_auspice(ch, AUSPICE_THEURGE) > 2 && ( fAll || och == victim ))
	{
	    found = TRUE;
	    act( "$n asks you to '$t'.", ch, argument, och, TO_VICT );
	    if (IS_NPC(och)) SET_BIT(och->act, ACT_COMMANDED);
	    interpret( och, argument );
	    if (IS_NPC(och)) REMOVE_BIT(och->act, ACT_COMMANDED);
	    WAIT_STATE(ch,12);
	}
    }

    if ( found )
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}



void do_command( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char action[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch_next;
    int level;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_VAMPIRE(ch) && !IS_GHOUL(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (get_disc(ch,DISC_DOMINATE) < 1)
    {
	send_to_char( "You require level 1 Dominate to Command people.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Command whom to do what?\n\r", ch );
	return;
    }

    strcpy(action,argument);

    if (is_in(action,"|quit*rem*drop*gift*claim*consent*propose*vampi*ma *maj*sid *side *reca*/*ha *hav*"))
    {
	send_to_char( "I think not.\n\r", ch );
	return;
    }

    level = get_disc(ch,DISC_DOMINATE) * 10;

    if (!str_cmp(arg,"all") && get_disc(ch,DISC_DOMINATE) >= 5)
    {
	sprintf( buf, "I think everyone wants to %s", action );
	do_say(ch,buf);
	WAIT_STATE(ch,24);
	for ( victim = ch->in_room->people; victim != NULL; victim = vch_next )
	{
	    vch_next = victim->next_in_room;
	    WAIT_STATE(ch,24);
	    if ( victim == ch ) continue;
	    if ( IS_IMMORTAL(victim) ) continue;
	    if (is_safe(ch, victim)) continue;

	    if (!IS_NPC(victim) && strlen(victim->pcdata->conding) > 2 && 
		!str_cmp(victim->pcdata->conding,ch->name))
	    {
		act("You blink in confusion.",victim,NULL,NULL,TO_CHAR);
		act("$n blinks in confusion.",victim,NULL,NULL,TO_ROOM);
		strcpy(buf,"Yes, you're right, I do...");
		do_say(victim,buf);
		if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
		interpret( victim, action );
		if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just used Command on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }

	    if (!IS_NPC(victim) && IS_VAMPIRE(victim) && victim->vampgen < ch->vampgen)
	    {
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
		act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }

	    if ( IS_NPC(victim) && victim->level >= level )
	    {
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	    	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
		continue;
	    }
	    else if ( IS_NPC(victim) && victim->spl[BLUE_MAGIC] >= level )
	    {
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	    	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
		continue;
	    }

	    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_DOMINATE] > 0)
	    {
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }
	
	    if (!IS_NPC(victim) && ((victim->pcdata->willpower[0] >= 2 && 
		IS_MORE(victim, MORE_LOYAL)) || victim->pcdata->willpower[0] >= 5) &&
		strlen(victim->pcdata->conding) > 1)
	    {
		if (IS_MORE(victim, MORE_LOYAL)) victim->pcdata->willpower[0] -= 2;
		else victim->pcdata->willpower[0] -= 5;
		victim->pcdata->resist[WILL_DOMINATE] = 60;
		send_to_char("You burn some willpower to resist Command.\n\r",victim);
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }
	    if (!IS_NPC(victim) && IS_WILLPOWER(victim, RES_DOMINATE) && 
		victim->pcdata->willpower[0] >= 10)
	    {
		victim->pcdata->willpower[0] -= 10;
		victim->pcdata->resist[WILL_DOMINATE] = 60;
		if (!IS_WILLPOWER(victim, RES_TOGGLE))
		    REMOVE_BIT(victim->pcdata->resist[0], RES_DOMINATE);
		send_to_char("You burn some willpower to resist Command.\n\r",victim);
		act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
		act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
		act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
		if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
		{
		    guardian_message( victim );
		    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
		}
		continue;
	    }

	    act("You blink in confusion.",victim,NULL,NULL,TO_CHAR);
	    act("$n blinks in confusion.",victim,NULL,NULL,TO_ROOM);
	    strcpy(buf,"Yes, you're right, I do...");
	    do_say(victim,buf);
	    if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
	    interpret( victim, action );
	    if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
	    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	    {
		guardian_message( victim );
		act("$n just used Command on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char( "How can you Command yourself??\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) )
    {
	send_to_char( "I think not...\n\r", ch );
	return;
    }

    if (IS_ITEMAFF(victim, ITEMA_SHADES) || IS_ITEMAFF(ch, ITEMA_SHADES) || 
	!can_see(victim,ch))
    {
	if (!IS_SET(ch->act,PLR_HOLYLIGHT))
	{
	    send_to_char( "You are unable to make eye contact with them.\n\r", ch );
	    return;
	}
    }

    if (is_safe(ch, victim)) return;

    if (IS_NPC(victim))
	sprintf( buf, "I think %s wants to %s", victim->short_descr, action );
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
	sprintf( buf, "I think %s wants to %s", victim->morph, action );
    else
	sprintf( buf, "I think %s wants to %s", victim->name, action );
    do_say(ch,buf);
    WAIT_STATE(ch,24);

    if (!IS_NPC(victim) && strlen(victim->pcdata->conding) > 2 && 
	!str_cmp(victim->pcdata->conding,ch->name))
    {
	act("You blink in confusion.",victim,NULL,NULL,TO_CHAR);
	act("$n blinks in confusion.",victim,NULL,NULL,TO_ROOM);
	strcpy(buf,"Yes, you're right, I do...");
	do_say(victim,buf);
	if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
	interpret( victim, action );
	if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just used Command on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && IS_VAMPIRE(victim) && victim->vampgen < ch->vampgen)
    {
	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    if ( IS_NPC(victim) && 
	( victim->level >= level || victim->spl[BLUE_MAGIC] >= level ) )
    {
	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
    	act("$s mind is too strong to overcome.",victim,NULL,ch,TO_VICT);
	return;
    }

    if (!IS_NPC(victim) && victim->pcdata->resist[WILL_DOMINATE] > 0)
    {
	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Command.\n\r",victim);
	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
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
	send_to_char("You burn some willpower to resist Command.\n\r",victim);
	act("You shake off $N's suggestion.",victim,NULL,ch,TO_CHAR);
	act("$n shakes off $N's suggestion.",victim,NULL,ch,TO_NOTVICT);
	act("$n shakes off your suggestion.",victim,NULL,ch,TO_VICT);
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to use Command on you.",ch,NULL,victim,TO_VICT);
	}
	return;
    }

    act("You blink in confusion.",victim,NULL,NULL,TO_CHAR);
    act("$n blinks in confusion.",victim,NULL,NULL,TO_ROOM);
    strcpy(buf,"Yes, you're right, I do...");
    do_say(victim,buf);
    if (IS_NPC(victim)) SET_BIT(victim->act, ACT_COMMANDED);
    interpret( victim, action );
    if (IS_NPC(victim)) REMOVE_BIT(victim->act, ACT_COMMANDED);
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just used Command on you.",ch,NULL,victim,TO_VICT);
    }
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    bool grouped = FALSE;

    one_argument( argument, arg );

    if ( !IS_NPC(ch) && get_trust(ch) == LEVEL_BUILDER )
    {
	send_to_char( "Perhaps you should just stick to building?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%-16s] %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if (!str_cmp(arg,"all"))
    {
	for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( vch == ch ) continue;
	    if ( vch->master != ch ) continue;
	    if ( is_same_group( vch, ch ) ) continue;
	    if ( !IS_NPC(vch) && get_trust(vch) == LEVEL_BUILDER ) continue;
	    vch->leader = ch;
	    act( "$N joins $n's group.", ch, NULL, vch, TO_NOTVICT );
	    act( "You join $n's group.", ch, NULL, vch, TO_VICT    );
	    act( "$N joins your group.", ch, NULL, vch, TO_CHAR    );
	    grouped = TRUE;
	}
	if (!grouped) send_to_char("There is nobody here following you who isn't grouped already.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && get_trust(ch) == LEVEL_BUILDER )
    {
	send_to_char( "Perhaps you should just let them build?\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
	act( "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
	act( "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    victim->leader = ch;
    act( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    
    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
	    
    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"You split %d gold coins.  Your share is %d gold coins.\n\r",
	amount, share + extra );
    send_to_char( buf, ch );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    send_to_char( buf, gch );
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

void do_changelight(CHAR_DATA *ch, char *argument )
{
    if (IS_SET(ch->in_room->room_flags, ROOM_DARK))
    {
	REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
	act("The room is suddenly filled with light!",ch,NULL,NULL,TO_CHAR);
	act("The room is suddenly filled with light!",ch,NULL,NULL,TO_ROOM);
	return;
    }
    SET_BIT(ch->in_room->room_flags, ROOM_DARK);
    act("The lights in the room suddenly go out!",ch,NULL,NULL,TO_CHAR);
    act("The lights in the room suddenly go out!",ch,NULL,NULL,TO_ROOM);
    return;
}

void open_lift( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (is_open(ch)) return;

    act("The doors open.",ch,NULL,NULL,TO_CHAR);
    act("The doors open.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_ROOM);
    if (!same_floor(ch,in_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_ROOM);
    if (is_open(ch)) act("The doors open.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    open_door(ch,TRUE);
    move_door(ch);
    open_door(ch,TRUE);
    thru_door(ch,in_room);
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void close_lift( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (!is_open(ch)) return;
    act("The doors close.",ch,NULL,NULL,TO_CHAR);
    act("The doors close.",ch,NULL,NULL,TO_ROOM);
    open_door(ch,FALSE);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void move_lift( CHAR_DATA *ch, int to_room )
{
    ROOM_INDEX_DATA *location;
    int in_room;

    in_room  = ch->in_room->vnum;
    location = get_room_index(in_room);

    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_CHAR);
    if (is_open(ch)) act("The doors close.",ch,NULL,NULL,TO_ROOM);
    if (!same_floor(ch,to_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_CHAR);
    if (!same_floor(ch,to_room)) act("The lift judders suddenly.",ch,NULL,NULL,TO_ROOM);
    move_door(ch);
    open_door(ch,FALSE);
    char_from_room(ch);
    char_to_room(ch,location);
    open_door(ch,FALSE);
    thru_door(ch,to_room);
    return;
}

bool same_floor( CHAR_DATA *ch, int cmp_room )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[0] == cmp_room ) return TRUE;
	else return FALSE;
    }
    return FALSE;
}

bool is_open( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[2] == 0 ) return TRUE;
	else return FALSE;
    }
    return FALSE;
}

void move_door( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *pRoomIndex;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	pRoomIndex = get_room_index(obj->value[0]);
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	return;
    }
    return;
}

void thru_door( CHAR_DATA *ch, int doorexit )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	obj->value[0] = doorexit;
	return;
    }
    return;
}

void open_door( CHAR_DATA *ch, bool be_open )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->item_type != ITEM_PORTAL ) continue;
	if ( obj->pIndexData->vnum == 30001 ) continue;
	if ( obj->value[2] == 0 && !be_open ) obj->value[2] = 3;
	else if ( obj->value[2] == 3 && be_open ) obj->value[2] = 0;
	return;
    }
    return;
}

void do_speak( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (arg[0] == '\0')
    {
	if (IS_SPEAKING(ch,LANG_DARK)) 
	    send_to_char("You are speaking the Dark tongue.\n\r",ch);
	else if (IS_SPEAKING(ch,DIA_OLDE)) 
	    send_to_char("You are speaking Olde Worlde.\n\r",ch);
	else if (IS_SPEAKING(ch,DIA_BAD)) 
	    send_to_char("You are speaking very badly.\n\r",ch);
	else
	    send_to_char("You are speaking the common language.\n\r",ch);
	strcpy(buf,"You can speak the following languages:");
	strcat(buf," Common Olde Bad");
	if (CAN_SPEAK(ch, LANG_DARK)) strcat(buf," Dark");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
	return;
    }

    if (!str_cmp(arg,"dark"))
    {
	if (!CAN_SPEAK(ch,LANG_DARK))
	{
	    send_to_char("You cannot speak the Dark tongue.\n\r",ch);
	    return;
	}
	if (IS_SPEAKING(ch,LANG_DARK))
	{
	    send_to_char("But you are already speaking the Dark tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_DARK;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] == LANG_COMMON)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"olde"))
    {
	if (ch->pcdata->language[0] == DIA_OLDE)
	{
	    send_to_char("But you are already speaking Olde Worlde!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = DIA_OLDE;
	send_to_char("Ok.\n\r",ch);
	return;
    }
    else if (!str_cmp(arg,"bad"))
    {
	if (ch->pcdata->language[0] == DIA_BAD)
	{
	    send_to_char("But you are already speaking badly!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
/*
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] < LANG_DARK)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	if (CAN_SPEAK(ch,DIA_OLDE)) ch->pcdata->language[0] = DIA_OLDE;
	else if (CAN_SPEAK(ch,DIA_BAD)) ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
*/
    else
    {
	strcpy(buf,"You can speak the following languages:");
	strcat(buf," Common Olde Bad");
	if (CAN_SPEAK(ch, LANG_DARK)) strcat(buf," Dark");
	strcat(buf,".\n\r");
	send_to_char(buf,ch);
	return;
    }

    return;
}

char *badlang( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "my name is",	"i calls meself"},
	{ "are not",	"aint"		},
	{ "have",	"'av"		},
	{ "my",		"me"		},
	{ "hello",	"oy"		},
	{ "hi ",	"oy "		},
	{ "i am",	"im"		},
	{ "it is",	"tis"		},
	{ "the ",	"@"		},
	{ " the",	" da"		},
	{ "thank",	"fank"		},
	{ "that",	"dat"		},
	{ "with",	"wiv"		},
	{ "they",	"day"		},
	{ "this",	"dis"		},
	{ "then",	"den"		},
	{ "there",	"ver"		},
	{ "their",	"ver"		},
	{ "thing",	"fing"		},
	{ "think",	"fink"		},
	{ "was",	"woz"		},
	{ "would",	"wud"		},
	{ "what ",	"#"		},
	{ "what",	"wot"		},
	{ "where",	"weer"		},
	{ "when",	"wen"		},
	{ "are",	"is"		},
	{ "you",	"ya"		},
	{ "your ",	"&"		},
	{ "your",	"yer"		},
	{ "dead",	"ded"		},
	{ "kill",	"stomp"		},
	{ "food",	"nosh"		},
	{ "blood",	"blud"		},
	{ "vampire",	"sucker"	},
	{ "kindred",	"suckers"	},
	{ "fire",	"hot"		},
	{ "dwarf",	"stunty"	},
	{ "dwarves",	"stunties"	},
	{ "goblin",	"gobbo"		},
	{ "death",	"def"		},
	{ "immune",	"mune"		},
	{ "immunit",	"munit"		},
	{ "childer",	"nippers"	},
	{ "childe",	"nipper"	},
	{ "child",	"nipper"	},
	{ "tradition",	"wassname"	},
	{ "generation",	"batch"		},
	{ "founded",	"made"		},
	{ "sired",	"nipped"	},
	{ "sire",	"dad"		},
	{ "lineage",	"istory"	},
	{ "road",	"path"		},
	{ "recognize",	"dats"		},
	{ "recognize",	"dats"		},
	{ "decapitate",	"headchop"	},
	{ "decap",	"chop"		},
	{ "recites",	"sez"		},
	{ "recite",	"sez"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		if (!str_cmp(spk_table[iSyl].new,",") || 
		    !str_cmp(spk_table[iSyl].new,"!") || 
		    !str_cmp(spk_table[iSyl].new,"?"))
		{
		    if (strlen(buf) < MAX_INPUT_LENGTH)
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, ", ya stupid fucker" ); break;
			case 2: strcat( buf, ", ya idiot" ); break;
			case 3: strcat( buf, ", ya damn fool" ); break;
			case 4: strcat( buf, ", ya fick mug" ); break;
			case 5: strcat( buf, ", ya fuckin' jerk" ); break;
			case 6: strcat( buf, ", asshole" ); break;
			case 7: strcat( buf, ", dickwipe" ); break;
		    }
		}
		else if (!str_cmp(spk_table[iSyl].new,"@"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "da " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "da " ); break;
			case 1: strcat( buf, "da bloody " ); break;
			case 2: strcat( buf, "da fuckin' " ); break;
			case 3: strcat( buf, "da goddamn " ); break;
			case 4: strcat( buf, "da flippin' " ); break;
			case 5: strcat( buf, "da stupid " ); break;
			case 6: strcat( buf, "da stinkin' " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"#"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "wot " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "wot " ); break;
			case 1: strcat( buf, "wot da fuck " ); break;
			case 2: strcat( buf, "wot, i sez, " ); break;
			case 3: strcat( buf, "wot da hell " ); break;
			case 4: strcat( buf, "wot da flip " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"&"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "yer " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "yer " ); break;
			case 1: strcat( buf, "yer bloody " ); break;
			case 2: strcat( buf, "yer fuckin' " ); break;
			case 3: strcat( buf, "yer damn " ); break;
			case 4: strcat( buf, "yer flippin' " ); break;
			case 5: strcat( buf, "yer stupid " ); break;
			case 6: strcat( buf, "yer stinkin' " ); break;
		    }
		    break;
		}
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);
    return argument;
}

char *oldelang( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "have",	"hath"		},
	{ "hello",	"hail"		},
	{ "hi ",	"hail "		},
	{ " hi",	" hail"		},
	{ "are",	"art"		},
	{ "your",	"thy"		},
	{ "you",	"thou"		},
	{ "i think",	"methinks"	},
	{ "do ",	"doth "		},
	{ " do",	" doth"		},
	{ "it was",	"twas"		},
	{ "before",	"ere"		},
	{ "his",	"$s"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *darktongue( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };
/*
    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "a", "i" }, { "b", "t" }, { "c", "x" }, { "d", "j" },
	{ "e", "u" }, { "f", "d" }, { "g", "k" }, { "h", "z" },
	{ "i", "o" }, { "j", "s" }, { "k", "f" }, { "l", "h" },
	{ "m", "b" }, { "n", "c" }, { "o", "e" }, { "p", "r" },
	{ "q", "l" }, { "r", "v" }, { "s", "w" }, { "t", "q" },
	{ "u", "a" }, { "v", "n" }, { "w", "y" }, { "x", "g" },
	{ "y", "m" }, { "z", "p" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "", "" }
    };
*/
    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "a", "i" }, { "b", "t" }, { "c", "x" }, { "d", "j" },
	{ "e", "u" }, { "f", "d" }, { "g", "k" }, { "h", "r" },
	{ "i", "o" }, { "j", "s" }, { "k", "f" }, { "l", "r" },
	{ "m", "b" }, { "n", "z" }, { "o", "e" }, { "p", "p" },
	{ "q", "l" }, { "r", "l" }, { "s", "l" }, { "t", "s" },
	{ "u", "a" }, { "v", "n" }, { "w", "y" }, { "x", "g" },
	{ "y", "m" }, { "z", "p" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "", "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *drunktalk( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;
    int loop;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "is",		"ish"		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		if (number_range(1,5) == 1 && str_cmp(spk_table[iSyl].new," "))
		    strcat( buf, spk_table[iSyl].new );
		else if (!str_cmp(spk_table[iSyl].new," "))
		{
		    if (number_range(1,5) == 1 && strlen(buf) < MAX_INPUT_LENGTH)
			strcat( buf, "*hic* " );
		}
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);
    for (loop = 1; loop < strlen(argument); loop++ )
    {
	if (number_range(1,2) == 1)
	    argument[loop] = UPPER(argument[loop]);
    }

    return argument;
}

char *beasttalk( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "what ",	"#"		},
	{ "your ",	"&"		},
	{ "the ",	"@"		},
	{ "my ",	"$"		},
	{ "'", "'" },
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		if (!str_cmp(spk_table[iSyl].new,",") || 
		    !str_cmp(spk_table[iSyl].new,"!") || 
		    !str_cmp(spk_table[iSyl].new,"?"))
		{
		    if (strlen(buf) < MAX_INPUT_LENGTH)
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, ", you stupid fucker" ); break;
			case 2: strcat( buf, ", you idiot" ); break;
			case 3: strcat( buf, ", you damn fool" ); break;
			case 4: strcat( buf, ", you little shit" ); break;
			case 5: strcat( buf, ", you fucking jerk" ); break;
			case 6: strcat( buf, ", asshole" ); break;
			case 7: strcat( buf, ", cock sucker" ); break;
			case 8: strcat( buf, ", fuckwit" ); break;
		    }
		}
		else if (!str_cmp(spk_table[iSyl].new,"@"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "the " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "the " ); break;
			case 1: strcat( buf, "the bloody " ); break;
			case 2: strcat( buf, "the fucking " ); break;
			case 3: strcat( buf, "the goddamn " ); break;
			case 4: strcat( buf, "the stupid " ); break;
			case 5: strcat( buf, "the stinking " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"#"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "what " );
		    else switch (number_range(1,5))
		    {
			default: strcat( buf, "what " ); break;
			case 1: strcat( buf, "what the fuck " ); break;
			case 2: strcat( buf, "what the hell " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"&"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "your " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "your " ); break;
			case 1: strcat( buf, "your bloody " ); break;
			case 2: strcat( buf, "your fucking " ); break;
			case 3: strcat( buf, "your damn " ); break;
			case 4: strcat( buf, "your goddamn " ); break;
			case 5: strcat( buf, "your stupid " ); break;
			case 6: strcat( buf, "your stinking " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"$"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "my " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "my " ); break;
			case 1: strcat( buf, "my bloody " ); break;
			case 2: strcat( buf, "my fucking " ); break;
			case 3: strcat( buf, "my damn " ); break;
			case 4: strcat( buf, "my goddamn " ); break;
			case 5: strcat( buf, "my stupid " ); break;
			case 6: strcat( buf, "my stinking " ); break;
		    }
		    break;
		}
		strcat( buf, spk_table[iSyl].new );
		if (!str_cmp(spk_table[iSyl].new," ") && strlen(buf) < MAX_INPUT_LENGTH)
		{
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, "*snarl* " ); break;
			case 2: strcat( buf, "*growl* " ); break;
		    }
		}
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *beastchat( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "what ",	"#"		},
	{ "your ",	"&"		},
	{ "the ",	"@"		},
	{ "my ",	"$"		},
	{ "'", "'" },
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		if (!str_cmp(spk_table[iSyl].new,",") || 
		    !str_cmp(spk_table[iSyl].new,"!") || 
		    !str_cmp(spk_table[iSyl].new,"?"))
		{
		    if (strlen(buf) < MAX_INPUT_LENGTH)
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, ", you stupid fuckers" ); break;
			case 2: strcat( buf, ", you idiots" ); break;
			case 3: strcat( buf, ", you sad spods" ); break;
			case 4: strcat( buf, ", you bunch of shits" ); break;
			case 5: strcat( buf, ", you fucking jerks" ); break;
			case 6: strcat( buf, ", assholes" ); break;
			case 7: strcat( buf, ", cock suckers" ); break;
			case 8: strcat( buf, ", fuckwits" ); break;
		    }
		}
		else if (!str_cmp(spk_table[iSyl].new,"@"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "the " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "the " ); break;
			case 1: strcat( buf, "the bloody " ); break;
			case 2: strcat( buf, "the fucking " ); break;
			case 3: strcat( buf, "the goddamn " ); break;
			case 4: strcat( buf, "the stupid " ); break;
			case 5: strcat( buf, "the stinking " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"#"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "what " );
		    else switch (number_range(1,5))
		    {
			default: strcat( buf, "what " ); break;
			case 1: strcat( buf, "what the fuck " ); break;
			case 2: strcat( buf, "what the hell " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"&"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "your " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "your " ); break;
			case 1: strcat( buf, "your bloody " ); break;
			case 2: strcat( buf, "your fucking " ); break;
			case 3: strcat( buf, "your damn " ); break;
			case 4: strcat( buf, "your goddamn " ); break;
			case 5: strcat( buf, "your stupid " ); break;
			case 6: strcat( buf, "your stinking " ); break;
		    }
		    break;
		}
		else if (!str_cmp(spk_table[iSyl].new,"$"))
		{
		    if (strlen(buf) >= MAX_INPUT_LENGTH)
			strcat( buf, "my " );
		    else switch (number_range(1,10))
		    {
			default: strcat( buf, "my " ); break;
			case 1: strcat( buf, "my bloody " ); break;
			case 2: strcat( buf, "my fucking " ); break;
			case 3: strcat( buf, "my damn " ); break;
			case 4: strcat( buf, "my goddamn " ); break;
			case 5: strcat( buf, "my stupid " ); break;
			case 6: strcat( buf, "my stinking " ); break;
		    }
		    break;
		}
		strcat( buf, spk_table[iSyl].new );
/*
		if (!str_cmp(spk_table[iSyl].new," ") && strlen(buf) < MAX_INPUT_LENGTH)
		{
		    switch (number_range(1,20))
		    {
			default: break;
			case 1: strcat( buf, "*snarl* " ); break;
			case 2: strcat( buf, "*growl* " ); break;
		    }
		}
		break;
*/
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *socialc( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "you are",	"$E is"		},
	{ "you.",	"$M."		},
	{ "you,",	"$M,"		},
	{ "you ",	"$M "		},
	{ " you",	" $M"		},
	{ "your ",	"$S "		},
	{ " your",	" $S"		},
	{ "yours.",	"theirs."	},
	{ "yours,",	"theirs,"	},
	{ "yours ",	"theirs "	},
	{ " yours",	" theirs"	},
	{ "begins",	"begin"		},
	{ "caresses",	"caress"	},
	{ "gives",	"give"		},
	{ "glares",	"glare"		},
	{ "grins",	"grin"		},
	{ "licks",	"lick"		},
	{ "looks",	"look"		},
	{ "loves",	"love"		},
	{ "plunges",	"plunge"	},
	{ "presses",	"press"		},
	{ "pulls",	"pull"		},
	{ "runs",	"run"		},
	{ "slaps",	"slap"		},
	{ "slides",	"slide"		},
	{ "smashes",	"smash"		},
	{ "squeezes",	"squeeze"	},
	{ "stares",	"stare"		},
	{ "sticks",	"stick"		},
	{ "strokes",	"stroke"	},
	{ "tugs",	"tug"		},
	{ "thinks",	"think"		},
	{ "thrusts",	"thrust"	},
	{ "whistles",	"whistle"	},
	{ "wraps",	"wrap"		},
	{ "winks",	"wink"		},
	{ "wishes",	"wish"		},
	{ " winks",	" wink"		},
	{ " his",	" your"		},
	{ "his ",	"your "		},
	{ " her",	" your"		},
	{ "her ",	"your "		},
	{ " him",	" your"		},
	{ "him ",	"your "		},
	{ "the",	"the"		},
	{ " he",	" you"		},
	{ "he ",	"you "		},
	{ " she",	" you"		},
	{ "she ",	"you "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *socialv( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ " his",	" $s"		},
	{ "his ",	"$s "		},
	{ " her",	" $s"		},
	{ "her ",	"$s "		},
	{ " him",	" $m"		},
	{ "him ",	"$m "		},
	{ " he",	" $e"		},
	{ "he ",	"$e "		},
	{ " she",	" $e"		},
	{ "she ",	"$e "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;

    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

char *socialn( CHAR_DATA *ch, char *argument, char *you, char *them )
{
    char buf  [MAX_STRING_LENGTH];
    char *pName;
    int iSyl;
    int length;

    struct spk_type
    {
	char *	old;
	char *	new;
    };

    static const struct spk_type spk_table[] =
    {
	{ " ",		" "		},
	{ "you are",	"$N is"		},
	{ "you.",	"$N."		},
	{ "you,",	"$N,"		},
	{ "you ",	"$N "		},
	{ " you",	" $N"		},
	{ "your.",	"$N's."		},
	{ "your,",	"$N's,"		},
	{ "your ",	"$N's "		},
	{ " your",	" $N's"		},
	{ "yourself",	"$Mself"	},
	{ " his",	" $s"		},
	{ "his ",	"$s "		},
	{ " her",	" $s"		},
	{ "her ",	"$s "		},
	{ " him",	" $m"		},
	{ "him ",	"$m "		},
	{ " he",	" $e"		},
	{ "he ",	"$e "		},
	{ " she",	" $e"		},
	{ "she ",	"$e "		},
	{ "a", "a" }, { "b", "b" }, { "c", "c" }, { "d", "d" },
	{ "e", "e" }, { "f", "f" }, { "g", "g" }, { "h", "h" },
	{ "i", "i" }, { "j", "j" }, { "k", "k" }, { "l", "l" },
	{ "m", "m" }, { "n", "n" }, { "o", "o" }, { "p", "p" },
	{ "q", "q" }, { "r", "r" }, { "s", "s" }, { "t", "t" },
	{ "u", "u" }, { "v", "v" }, { "w", "w" }, { "x", "x" },
	{ "y", "y" }, { "z", "z" }, { ",", "," }, { ".", "." },
	{ ";", ";" }, { ":", ":" }, { "(", "(" }, { ")", ")" },
	{ ")", ")" }, { "-", "-" }, { "!", "!" }, { "?", "?" },
	{ "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" },
	{ "5", "5" }, { "6", "6" }, { "7", "7" }, { "8", "8" },
	{ "9", "9" }, { "0", "0" }, { "%", "%" }, {  "",  "" }
    };
    buf[0]	= '\0';

    if ( argument[0] == '\0' ) return argument;
    for ( pName = str_dup(argument); *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( spk_table[iSyl].old, pName ) )
	    {
		strcat( buf, spk_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    argument[0] = '\0';
    strcpy(argument,buf);
    argument[0] = UPPER(argument[0]);

    return argument;
}

void do_inform( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char buf2[10];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int num = 0;
    bool t_f;
    bool truth = TRUE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "What do you wish to inform?\n\r", ch );
	send_to_char( "Options: Generation Diablerie.\n\r", ch );
	return;
    }
/*
    REMOVE_BIT(ch->more, MORE_INFORM);
*/
    if (!str_cmp(arg1,"gen") || !str_cmp(arg1,"generation"))
    {
	if (arg2[0] == '\0') num = ch->vampgen;
	else if ( is_number( arg2 ) ) num = atoi( arg2 );
	else
	{
	    send_to_char( "Please select a generation in the range 1 to 13.\n\r", ch );
	    return;
	}
	if (num < 1 || num > 13)
	{
	    send_to_char( "Please select a generation in the range 1 to 13.\n\r", ch );
	    return;
	}
	switch (num)
	{
	    default: strcpy(buf2,"th"); break;
	    case 1: strcpy(buf2,"st"); break;
	    case 2: strcpy(buf2,"nd"); break;
	    case 3: strcpy(buf2,"rd"); break;
	}
	if (ch->vampgen != num) truth = FALSE;
	sprintf(buf, "I am of the %d%s generation.", num, buf2);
    }
    else if (!str_cmp(arg1,"diab") || !str_cmp(arg1,"diablerie"))
    {
	if (arg2[0] == '\0')
	{
	    if (ch->pcdata->diableries[DIAB_EVER] > 0) t_f = TRUE;
	    else t_f = FALSE;
	}
	else if ( !str_cmp(arg2,"true") || !str_cmp(arg2,"yes") ) t_f = TRUE;
	else if ( !str_cmp(arg2,"false") || !str_cmp(arg2,"no") ) t_f = FALSE;
	else
	{
	    send_to_char( "Have you ever committed diablerie? YES or NO.\n\r", ch );
	    return;
	}
	if (ch->pcdata->diableries[DIAB_EVER] > 0 && !t_f) truth = FALSE;
	else if (ch->pcdata->diableries[DIAB_EVER] < 1 && t_f) truth = FALSE;
	if (t_f)
	    strcpy(buf, "I have committed diablerie in the past.");
	else
	    strcpy(buf, "I have never committed diablerie.");
    }
    else
    {
	send_to_char( "What do you wish to inform?\n\r", ch );
	send_to_char( "Options: Generation Diablerie.\n\r", ch );
	return;
    }

    if (truth) ch->pcdata->lie = LIE_FALSE;
    else ch->pcdata->lie = LIE_TRUE;
    SET_BIT(ch->more, MORE_INFORM);
    do_say(ch,buf);
    return;
}
