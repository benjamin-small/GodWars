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

void do_inpart( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    int       inpart = 0;
    int       inpart2 = 0;
    int       cost = 0;

    smash_tilde(argument);
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON))
    {
	if (!IS_DEMON(ch))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	if ( IS_ANGEL(ch) )
	{
	    send_to_char( "Syntax: Inpart <person> <power>\n\r", ch );
	    send_to_char("Fangs (2500), Claws (2500), Hooves (1500), Nightsight (3000),\n\r",ch);
	    send_to_char("Wings (1000), Might (7500), Toughness (7500), Speed (7500), Travel (1500),\n\r",ch);
	    send_to_char("Scry (7500), Shadowsight (7500), Move (500), Leap (500), Magic (1000),\n\r",ch);
	    send_to_char("Lifespan (100), Pact (0), Prince (0), Longsword (0), Shortsword (0),\n\r",ch);
	    send_to_char("Angelform (25000), Cherub (10000), Inferno (10000), Eyespy (1000), Firewall (1000).\n\r",ch);
	    send_to_char("Vision (10000 per increase), Obeah (10000 per increase).\n\r",ch);
	}
	else
	{
	    send_to_char( "Syntax: Inpart <person> <power>\n\r", ch );
	    send_to_char("Fangs (2500), Claws (2500), Hooves (1500), Nightsight (3000),\n\r",ch);
	    send_to_char("Wings (1000), Might (7500), Toughness (7500), Speed (7500), Travel (1500),\n\r",ch);
	    send_to_char("Scry (7500), Shadowsight (7500), Move (500), Leap (500), Magic (1000),\n\r",ch);
	    send_to_char("Lifespan (100), Pact (0), Prince (0), Longsword (0), Shortsword (0),\n\r",ch);
	    send_to_char("Demonform (25000), Imp (10000), Inferno (10000), Eyespy (1000), Firewall (1000).\n\r",ch);
	    send_to_char("Vision (10000 per increase), Daimoinon (10000 per increase).\n\r",ch);
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nobody by that name.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim->level < LEVEL_AVATAR || 
	(victim != ch && !IS_SET(victim->act, PLR_CHAMPION)))
    {
	send_to_char( "Only on a champion.\n\r", ch );
	return;
    }

    if ( victim != ch && str_cmp(victim->lord, ch->name) && 
	str_cmp(victim->lord, ch->lord) && strlen(victim->lord) > 1 )
    {
	send_to_char( "They are not your champion.\n\r", ch );
	return;
    }

    if (!str_cmp(arg2,"pact"))
    {
	if (victim == ch)
	{
	    send_to_char("Not on yourself!\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim,EXTRA_SIRE))
	{
	    send_to_char("You have lost the power to make pacts!\n\r",victim);
	    send_to_char("You remove their power to make pacts.\n\r",ch);
	    REMOVE_BIT(victim->extra, EXTRA_SIRE);
	}
	else
	{
	    send_to_char("You have been granted the power to make pacts!\n\r",victim);
	    send_to_char("You grant them the power to make pacts.\n\r",ch);
	    SET_BIT(victim->extra, EXTRA_SIRE);
	}
	do_autosave(victim,"");
	return;
    }

    if (!str_cmp(arg2,"prince"))
    {
	if (victim == ch)
	{
	    send_to_char("Not on yourself!\n\r",ch);
	    return;
	}
	if (!IS_SET(ch->act, PLR_DEMON))
	{
	    if ( IS_ANGEL(ch) )
		send_to_char("Only the Archangel has the power to make princes.\n\r",ch);
	   else
		send_to_char("Only the Demon Lord has the power to make princes.\n\r",ch);
	    return;
	}
	if (IS_EXTRA(victim,EXTRA_PRINCE))
	{
	    send_to_char("You have lost your princehood!\n\r",victim);
	    send_to_char("You remove their princehood.\n\r",ch);
	    REMOVE_BIT(victim->extra, EXTRA_PRINCE);
	}
	else
	{
	    send_to_char("You have been made a prince!\n\r",victim);
	    send_to_char("You make them a prince.\n\r",ch);
	    SET_BIT(victim->extra, EXTRA_PRINCE);
	}
	do_autosave(victim,"");
	return;
    }

    if (!str_cmp(arg2,"longsword"))
    {
	if ( IS_ANGEL(ch) )
	{
	    send_to_char("You have been granted the power to transform into an angelic longsword!\n\r",victim);
	    send_to_char("You grant them the power to transform into an angelic longsword.\n\r",ch);
	    victim->pcdata->disc[10] = 29664;
	}
	else
	{
	    send_to_char("You have been granted the power to transform into a demonic longsword!\n\r",victim);
	    send_to_char("You grant them the power to transform into a demonic longsword.\n\r",ch);
	    victim->pcdata->disc[10] = 29662;
	}
	do_autosave(victim,"");
	return;
    }

    if (!str_cmp(arg2,"shortsword"))
    {
	if ( IS_ANGEL(ch) )
	{
	    send_to_char("You have been granted the power to transform into an angelic shortsword!\n\r",victim);
	    send_to_char("You grant them the power to transform into an angelic shortsword.\n\r",ch);
	    victim->pcdata->disc[10] = 29665;
	}
	else
	{
	    send_to_char("You have been granted the power to transform into a demonic shortsword!\n\r",victim);
	    send_to_char("You grant them the power to transform into a demonic shortsword.\n\r",ch);
	    victim->pcdata->disc[10] = 29663;
	}
	do_autosave(victim,"");
	return;
    }

    if (!str_cmp(arg2,"fangs")) 
	{inpart = DEM_FANGS; inpart2 = C_POWERS; cost = 2500;}
    else if (!str_cmp(arg2,"claws")) 
	{inpart = DEM_CLAWS; inpart2 = C_POWERS; cost = 2500;}
    else if (!str_cmp(arg2,"demonform") && !IS_ANGEL(ch)) 
	{inpart = DEM_DEMONFORM; inpart2 = C_POWERS; cost = 25000;}
    else if (!str_cmp(arg2,"angelform") && IS_ANGEL(ch)) 
	{inpart = DEM_DEMONFORM; inpart2 = C_POWERS; cost = 25000;}
    else if (!str_cmp(arg2,"hooves")) 
	{inpart = DEM_HOOVES; inpart2 = C_POWERS; cost = 1500;}
    else if (!str_cmp(arg2,"nightsight")) 
	{inpart = DEM_EYES; inpart2 = C_POWERS; cost = 3000;}
    else if (!str_cmp(arg2,"wings")) 
	{inpart = DEM_WINGS; inpart2 = C_POWERS; cost = 1000;}
    else if (!str_cmp(arg2,"might")) 
	{inpart = DEM_MIGHT; inpart2 = C_POWERS; cost = 7500;}
    else if (!str_cmp(arg2,"toughness")) 
	{inpart = DEM_TOUGH; inpart2 = C_POWERS; cost = 7500;}
    else if (!str_cmp(arg2,"speed")) 
	{inpart = DEM_SPEED; inpart2 = C_POWERS; cost = 7500;}
    else if (!str_cmp(arg2,"travel")) 
	{inpart = DEM_TRAVEL; inpart2 = C_POWERS; cost = 1500;}
    else if (!str_cmp(arg2,"scry")) 
	{inpart = DEM_SCRY; inpart2 = C_POWERS; cost = 7500;}
    else if (!str_cmp(arg2,"shadowsight")) 
	{inpart = DEM_SHADOWSIGHT; inpart2 = C_POWERS; cost = 3000;}
    else if (!str_cmp(arg2,"move")) 
	{inpart = DEM_MOVE; inpart2 = O_POWERS; cost = 500;}
    else if (!str_cmp(arg2,"leap")) 
	{inpart = DEM_LEAP; inpart2 = O_POWERS; cost = 500;}
    else if (!str_cmp(arg2,"magic")) 
	{inpart = DEM_MAGIC; inpart2 = O_POWERS; cost = 1000;}
    else if (!str_cmp(arg2,"lifespan")) 
	{inpart = DEM_LIFESPAN; inpart2 = O_POWERS; cost = 100;}
    else if (!str_cmp(arg2,"inferno")) 
	{inpart = DEM_INFERNO; inpart2 = C_POWERS; cost = 10000;}
    else if (!str_cmp(arg2,"eyespy")) 
	{inpart = DEM_EYESPY; inpart2 = C_POWERS; cost = 1000;}
    else if (!str_cmp(arg2,"firewall")) 
	{inpart = DEM_FIREWALL; inpart2 = C_POWERS; cost = 1000;}
    else if (!str_cmp(arg2,"imp") && !IS_ANGEL(ch)) 
	{inpart = DEM_IMP; inpart2 = C_POWERS; cost = 10000;}
    else if (!str_cmp(arg2,"cherub") && IS_ANGEL(ch)) 
	{inpart = DEM_IMP; inpart2 = C_POWERS; cost = 10000;}
    else if (!str_cmp(arg2,"vision")) 
    {
	cost = 10000;
	if ( victim->pcdata->powers[DISC_AUSPEX] >= 5 )
	{
	    send_to_char("They already have Supreme Vision.\n\r",ch);
	    return;
	}
	if (ch->pcdata->power[POWER_TOTAL] < cost || 
	    ch->pcdata->power[POWER_CURRENT] < cost)
	{
	    send_to_char("You have insufficient power to inpart that gift.\n\r",ch);
	    return;
	}
	ch->pcdata->power[POWER_TOTAL] -= cost;
	ch->pcdata->power[POWER_CURRENT] -= cost;
	if ( ++victim->pcdata->powers[DISC_AUSPEX] < 1 )
	    victim->pcdata->powers[DISC_AUSPEX] = 1;
	if (victim != ch) 
	{
	    if ( IS_ANGEL(ch) )
		send_to_char("You have been granted an angelic gift from your patron!\n\r",victim);
	    else
		send_to_char("You have been granted a demonic gift from your patron!\n\r",victim);
	}
	send_to_char("Ok.\n\r",ch);
	if (victim != ch) do_autosave(ch,"");
	do_autosave(victim,"");
	return;
    }
    else if (!str_cmp(arg2,"daimoinon") && !IS_ANGEL(ch)) 
    {
	cost = 10000;
	if ( victim->pcdata->powers[DISC_DAIMOINON] >= 5 )
	{
	    send_to_char("They already have Supreme Daimoinon.\n\r",ch);
	    return;
	}
	if (ch->pcdata->power[POWER_TOTAL] < cost || 
	    ch->pcdata->power[POWER_CURRENT] < cost)
	{
	    send_to_char("You have insufficient power to inpart that gift.\n\r",ch);
	    return;
	}
	ch->pcdata->power[POWER_TOTAL] -= cost;
	ch->pcdata->power[POWER_CURRENT] -= cost;
	if ( ++victim->pcdata->powers[DISC_DAIMOINON] < 1 )
	    victim->pcdata->powers[DISC_DAIMOINON] = 1;
	if (victim != ch) 
	    send_to_char("You have been granted a demonic gift from your patron!\n\r",victim);
	send_to_char("Ok.\n\r",ch);
	if (victim != ch) do_autosave(ch,"");
	do_autosave(victim,"");
	return;
    }
    else if (!str_cmp(arg2,"obeah") && IS_ANGEL(ch)) 
    {
	cost = 10000;
	if ( victim->pcdata->powers[DISC_OBEAH] >= 5 )
	{
	    send_to_char("They already have Supreme Obeah.\n\r",ch);
	    return;
	}
	if (ch->pcdata->power[POWER_TOTAL] < cost || 
	    ch->pcdata->power[POWER_CURRENT] < cost)
	{
	    send_to_char("You have insufficient power to inpart that gift.\n\r",ch);
	    return;
	}
	ch->pcdata->power[POWER_TOTAL] -= cost;
	ch->pcdata->power[POWER_CURRENT] -= cost;
	if ( ++victim->pcdata->powers[DISC_OBEAH] < 1 )
	    victim->pcdata->powers[DISC_OBEAH] = 1;
	if (victim != ch) 
	    send_to_char("You have been granted an angelic gift from your patron!\n\r",victim);
	send_to_char("Ok.\n\r",ch);
	if (victim != ch) do_autosave(ch,"");
	do_autosave(victim,"");
	return;
    }
    else
    {
	do_inpart(ch,"");
	return;
    }
    if (IS_SET(victim->pcdata->disc[inpart2], inpart))
    {
	send_to_char("They have already got that power.\n\r",ch);
	return;
    }
    if (ch->pcdata->power[POWER_TOTAL] < cost || 
	ch->pcdata->power[POWER_CURRENT] < cost)
    {
	send_to_char("You have insufficient power to inpart that gift.\n\r",ch);
	return;
    }
    SET_BIT(victim->pcdata->disc[inpart2], inpart);
    ch->pcdata->power[POWER_TOTAL] -= cost;
    ch->pcdata->power[POWER_CURRENT] -= cost;
    if (victim != ch) 
    {
	if ( IS_ANGEL(ch) )
	    send_to_char("You have been granted an angelic gift from your patron!\n\r",victim);
	else
	    send_to_char("You have been granted a demonic gift from your patron!\n\r",victim);
    }
    send_to_char("Ok.\n\r",ch);
    if (victim != ch) do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}

void do_demonarmour( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON) || IS_ANGEL(ch))
    {
	if (!IS_SET(ch->act, PLR_CHAMPION) || !IS_EXTRA(ch, EXTRA_PRINCE) ||
	    IS_ANGEL(ch))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: demonarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: Black Grey Purple Red Brass.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if (str_cmp(arg1,"black") && str_cmp(arg1,"red") && str_cmp(arg1,"grey") 
    && str_cmp(arg1,"purple") && str_cmp(arg1,"brass"))
    {
	send_to_char("Syntax: demonarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: Black Grey Purple Red Brass.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if      (!str_cmp(arg2,"ring"     )) vnum = 29650;
    else if (!str_cmp(arg2,"collar"   )) vnum = 29651;
    else if (!str_cmp(arg2,"plate"    )) vnum = 29652;
    else if (!str_cmp(arg2,"helmet"   )) vnum = 29653;
    else if (!str_cmp(arg2,"leggings" )) vnum = 29654;
    else if (!str_cmp(arg2,"boots"    )) vnum = 29655;
    else if (!str_cmp(arg2,"gauntlets")) vnum = 29656;
    else if (!str_cmp(arg2,"sleeves"  )) vnum = 29657;
    else if (!str_cmp(arg2,"cape"     )) vnum = 29658;
    else if (!str_cmp(arg2,"belt"     )) vnum = 29659;
    else if (!str_cmp(arg2,"bracer"   )) vnum = 29660;
    else if (!str_cmp(arg2,"visor"    )) vnum = 29661;
    else
    {
	send_to_char("Syntax: demonarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: Black Grey Purple Red Brass.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"black") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000))
    {
	send_to_char("It costs 5000 points of power to create a piece of black demon armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"grey") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 100))
    {
	send_to_char("It costs 5000 power and 100 qp to create a piece of grey demon armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"purple") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 250))
    {
	send_to_char("It costs 5000 power and 250 qp to create a piece of purple demon armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"red") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 750))
    {
	send_to_char("It costs 5000 power and 750 qp to create a piece of red demon armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"brass") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 1500))
    {
	send_to_char("It costs 5000 power and 1500 qp to create a piece of red demon armour.\n\r",ch);
	return;
    }
    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform KaVir.\n\r",ch);
	return;
    }
    obj = create_object(pObjIndex, 50);
    obj_to_char(obj, ch);
    ch->pcdata->power[0] -= 5000;
    ch->pcdata->power[1] -= 5000;
    if (!str_cmp(arg1,"grey"))
    {obj->specpower = ARM_GREY;ch->pcdata->quest -= 100;obj->points += 100;}
    if (!str_cmp(arg1,"purple"))
    {obj->specpower = ARM_PURPLE;ch->pcdata->quest -= 250;obj->points += 250;}
    if (!str_cmp(arg1,"red"))
    {obj->specpower = ARM_RED;ch->pcdata->quest -= 750;obj->points += 750;}
    if (!str_cmp(arg1,"brass"))
    {obj->specpower = ARM_BRASS;ch->pcdata->quest -= 1500;obj->points += 1500;}

    sprintf( buf, obj->name, arg1, "demon" );
    free_string( obj->name );
    obj->name = str_dup( buf );

    sprintf( buf, obj->short_descr, arg1, "demon" );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, obj->description, arg1, "demon" );
    free_string( obj->description );
    obj->description = str_dup( buf );

    free_string( obj->questmaker );
    obj->questmaker = str_dup( ch->name );

    free_string( obj->questowner );
    obj->questowner = str_dup( ch->name );

    SET_BIT( obj->spectype, SITEM_DEMONIC );
    SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );

    act("$p appears in your hands in a blast of flames.",ch,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in a blast of flames.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_angelarmour( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON) || !IS_ANGEL(ch))
    {
	if (!IS_SET(ch->act, PLR_CHAMPION) || !IS_EXTRA(ch, EXTRA_PRINCE) ||
	    !IS_ANGEL(ch))
	{
	    send_to_char("Huh?\n\r",ch);
	    return;
	}
    }

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: angelarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: White Yellow Blue Silver Gold.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if (str_cmp(arg1,"white") && str_cmp(arg1,"yellow") && str_cmp(arg1,"blue") 
    && str_cmp(arg1,"silver") && str_cmp(arg1,"gold"))
    {
	send_to_char("Syntax: angelarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: White Yellow Blue Silver Gold.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if      (!str_cmp(arg2,"ring"     )) vnum = 29650;
    else if (!str_cmp(arg2,"collar"   )) vnum = 29651;
    else if (!str_cmp(arg2,"plate"    )) vnum = 29652;
    else if (!str_cmp(arg2,"helmet"   )) vnum = 29653;
    else if (!str_cmp(arg2,"leggings" )) vnum = 29654;
    else if (!str_cmp(arg2,"boots"    )) vnum = 29655;
    else if (!str_cmp(arg2,"gauntlets")) vnum = 29656;
    else if (!str_cmp(arg2,"sleeves"  )) vnum = 29657;
    else if (!str_cmp(arg2,"cape"     )) vnum = 29658;
    else if (!str_cmp(arg2,"belt"     )) vnum = 29659;
    else if (!str_cmp(arg2,"bracer"   )) vnum = 29660;
    else if (!str_cmp(arg2,"visor"    )) vnum = 29661;
    else
    {
	send_to_char("Syntax: angelarmour <colour> <armour>\n\r",ch);
	send_to_char("Colour can be one of: White Yellow Blue Silver Gold.\n\r",ch);
	send_to_char("Armour can be one of: Ring Collar Plate Helmet Leggings Boots Gauntlets\n\r                      Sleeves Cape Belt Bracer Visor.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"white") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000))
    {
	send_to_char("It costs 5000 points of power to create a piece of white angel armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"yellow") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 100))
    {
	send_to_char("It costs 5000 power and 100 qp to create a piece of yellow angel armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"blue") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 250))
    {
	send_to_char("It costs 5000 power and 250 qp to create a piece of blue angel armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"silver") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 750))
    {
	send_to_char("It costs 5000 power and 750 qp to create a piece of silver angel armour.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1,"gold") && (ch->pcdata->power[0] < 5000 || 
	ch->pcdata->power[1] < 5000 || ch->pcdata->quest < 1500))
    {
	send_to_char("It costs 5000 power and 1500 qp to create a piece of gold angel armour.\n\r",ch);
	return;
    }
    if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
    {
	send_to_char("Missing object, please inform KaVir.\n\r",ch);
	return;
    }
    obj = create_object(pObjIndex, 50);
    obj_to_char(obj, ch);
    ch->pcdata->power[0] -= 5000;
    ch->pcdata->power[1] -= 5000;
    if (!str_cmp(arg1,"yellow"))
    {obj->specpower = ARM_YELLOW;ch->pcdata->quest -= 100;obj->points += 100;}
    if (!str_cmp(arg1,"blue"))
    {obj->specpower = ARM_BLUE;ch->pcdata->quest -= 250;obj->points += 250;}
    if (!str_cmp(arg1,"silver"))
    {obj->specpower = ARM_SILVER;ch->pcdata->quest -= 750;obj->points += 750;}
    if (!str_cmp(arg1,"gold"))
    {obj->specpower = ARM_GOLD;ch->pcdata->quest -= 1500;obj->points += 1500;}

    sprintf( buf, obj->name, arg1, "angel" );
    free_string( obj->name );
    obj->name = str_dup( buf );

    sprintf( buf, obj->short_descr, arg1, "angel" );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, obj->description, arg1, "angel" );
    free_string( obj->description );
    obj->description = str_dup( buf );

    free_string( obj->questmaker );
    obj->questmaker = str_dup( ch->name );

    free_string( obj->questowner );
    obj->questowner = str_dup( ch->name );

    SET_BIT( obj->spectype, SITEM_DEMONIC );
    SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );

    act("$p appears in your hands in a haze of blue light.",ch,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in a haze of blue light.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_travel( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char      *cdemon;
    char      *vdemon;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_DEMON(ch))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_TRAVEL))
	{
	    send_to_char("You haven't been granted the gift of travel.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( !IS_ANGEL(ch) && !str_cmp( arg, "hell" ) )
    {
	send_to_char("You sink down into the ground.\n\r",ch);
	act("$n sinks down into the ground.",ch,NULL,NULL,TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, get_room_index(ROOM_VNUM_HELL));
	do_look(ch,"");
	send_to_char("You rise up out of the ground.\n\r",ch);
	act("$n rises up out of the ground.",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if ( IS_ANGEL(ch) && !str_cmp( arg, "heaven" ) )
    {
	send_to_char("You shoot up into the sky.\n\r",ch);
	act("$n shoots up into the sky.",ch,NULL,NULL,TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, get_room_index(ROOM_VNUM_HEAVEN));
	do_look(ch,"");
	send_to_char("You float down from the sky.\n\r",ch);
	act("$n floats down from the sky.",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nobody by that name.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot travel to yourself!\n\r", ch );
	return;
    }

    if ( !IS_HERO(victim) || !IS_DEMON(victim) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( strlen( ch->lord ) > 1 )
	cdemon = ch->lord;
    else
	cdemon = ch->name;

    if ( strlen( victim->lord ) > 1 )
	vdemon = victim->lord;
    else
	vdemon = victim->name;

    /* Demons cannot travel to oppositing Demons */
    if ( str_cmp( cdemon, vdemon ) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if (victim->in_room == NULL || victim->in_room == ch->in_room)
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( victim->position != POS_STANDING )
    {
	send_to_char( "You are unable to focus on their location.\n\r", ch );
	return;
    }

    if ( IS_ANGEL(ch) )
    {
	send_to_char("You shoot up into the sky.\n\r",ch);
	act("$n shoots up into the sky.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("You sink down into the ground.\n\r",ch);
	act("$n sinks down into the ground.",ch,NULL,NULL,TO_ROOM);
    }
    char_from_room(ch);
    char_to_room(ch,victim->in_room);
    do_look(ch,"");
    if ( IS_ANGEL(ch) )
    {
	send_to_char("You float down from the sky.\n\r",ch);
	act("$n floats down from the sky.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("You rise up out of the ground.\n\r",ch);
	act("$n rises up out of the ground.",ch,NULL,NULL,TO_ROOM);
    }
    return;
}

void do_horns( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON) && !IS_SET(ch->act, PLR_CHAMPION))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_MOD(ch,MOD_HORNS) )
    {
	send_to_char("Your horns slide back into your head.\n\r",ch);
	act("$n's horns slide back into $s head.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->bodymods, MOD_HORNS);
	return;
    }
    send_to_char("Your horns extend out of your head.\n\r",ch);
    act("A pair of pointed horns extend from $n's head.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->bodymods, MOD_HORNS);
    return;
}

void do_wings( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (IS_MOD(ch, MOD_WINGS))
    {
	if (IS_MOD(ch,MOD_UNFOLDED) )
	{
	    send_to_char("Your wings fold up behind your back.\n\r",ch);
	    act("$n's wings fold up behind $s back.", ch, NULL, NULL, TO_ROOM);
	    REMOVE_BIT(ch->pcdata->bodymods, MOD_UNFOLDED);
	}
	else
	{
	    send_to_char("Your wings unfold from behind your back.\n\r",ch);
	    act("$n's wings unfold from behind $s back.", ch, NULL, NULL, TO_ROOM);
	    SET_BIT(ch->pcdata->bodymods, MOD_UNFOLDED);
	}
	return;
    }
    if (!IS_DEMON(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_WINGS))
    {
	send_to_char("You haven't been granted the gift of wings.\n\r",ch);
	return;
    }

    if (arg[0] != '\0')
    {
	if (!IS_DEMAFF(ch,DEM_WINGS) )
	{
	    send_to_char("First you better get your wings out!\n\r",ch);
	    return;
	}
	if (!str_cmp(arg,"unfold") || !str_cmp(arg,"u"))
	{
	    if (IS_DEMAFF(ch,DEM_UNFOLDED) )
	    {
		send_to_char("But your wings are already unfolded!\n\r",ch);
		return;
	    }
	    send_to_char("Your wings unfold from behind your back.\n\r",ch);
	    act("$n's wings unfold from behind $s back.", ch, NULL, NULL, TO_ROOM);
	    SET_BIT(ch->pcdata->disc[C_CURRENT], DEM_UNFOLDED);
	    return;
	}
	else if (!str_cmp(arg,"fold") || !str_cmp(arg,"f"))
	{
	    if (!IS_DEMAFF(ch,DEM_UNFOLDED) )
	    {
		send_to_char("But your wings are already folded!\n\r",ch);
		return;
	    }
	    send_to_char("Your wings fold up behind your back.\n\r",ch);
	    act("$n's wings fold up behind $s back.", ch, NULL, NULL, TO_ROOM);
	    REMOVE_BIT(ch->pcdata->disc[C_CURRENT], DEM_UNFOLDED);
	    return;
	}
	else
	{
	    send_to_char("Do you want to FOLD or UNFOLD your wings?\n\r",ch);
	    return;
	}
    }

    if (IS_DEMAFF(ch,DEM_WINGS) )
    {
	if (IS_DEMAFF(ch,DEM_UNFOLDED) )
	{
	    send_to_char("Your wings fold up behind your back.\n\r",ch);
	    act("$n's wings fold up behind $s back.", ch, NULL, NULL, TO_ROOM);
	    REMOVE_BIT(ch->pcdata->disc[C_CURRENT], DEM_UNFOLDED);
	}
	send_to_char("Your wings slide into your back.\n\r",ch);
	act("$n's wings slide into $s back.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->disc[C_CURRENT], DEM_WINGS);
	return;
    }
    send_to_char("Your wings extend from your back.\n\r",ch);
    act("A pair of wings extend from $n's back.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->disc[C_CURRENT], DEM_WINGS);
    return;
}

void do_lifespan( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act,PLR_DEMON) && !IS_SET(ch->act,PLR_CHAMPION))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_SET(ch->pcdata->disc[O_POWERS], DEM_LIFESPAN))
    {
	send_to_char("You haven't been granted the gift of lifespan.\n\r",ch);
	return;
    }

    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( obj->chobj == NULL || obj->chobj != ch )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_HEAD(ch,LOST_HEAD))
    {
	send_to_char("You cannot change your lifespan in this form.\n\r",ch);
	return;
    }

         if (!str_cmp(arg,"l") || !str_cmp(arg,"long" )) obj->timer = 0;
    else if (!str_cmp(arg,"s") || !str_cmp(arg,"short")) obj->timer = 1;
    else
    {
	send_to_char("Do you wish to have a long or short lifespan?\n\r",ch);
	return;
    }
    send_to_char("Ok.\n\r",ch);

    return;
}

void do_pact( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    bool      can_sire = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON) && !IS_SET(ch->act, PLR_CHAMPION))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (IS_SET(ch->act, PLR_DEMON)) can_sire = TRUE;
    else if (IS_EXTRA(ch,EXTRA_SIRE)) can_sire = TRUE;
    else can_sire = TRUE;

    if (!can_sire)
    {
	send_to_char("You are not able to make a pact.\n\r",ch);
	return;
    }

    if (!str_cmp(ch->lord,"") && !IS_SET(ch->act, PLR_DEMON))
    {
	send_to_char( "First you must find a demon lord.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Make a pact with whom?\n\r", ch );
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
	send_to_char( "You cannot make a pact with yourself.\n\r", ch );
	return;
    }

    if (IS_MAGE(victim))
    {
	send_to_char( "You cannot make a pact with a mage.\n\r", ch );
	return;
    }

    if ( victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim) )
    {
	send_to_char( "You can only make pacts with avatars.\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_DEMON))
    {
	send_to_char( "You cannot make a pact with an unwilling person.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_DEMON) || IS_SET(victim->act, PLR_CHAMPION))
    {
	send_to_char("They have no soul to make a pact with!\n\r",ch);
	return;
    }

    if ( IS_VAMPIRE(victim) )
    {
	send_to_char( "You cannot make a pact with the undead.\n\r", ch );
	return;
    }

    if ( IS_WEREWOLF(victim) )
    {
	send_to_char( "You cannot make a pact with the children of Gaia.\n\r", ch );
	return;
    }

    if ( IS_MAGE(victim) )
    {
	send_to_char( "You cannot make a pact with mages.\n\r", ch );
	return;
    }

    if ( IS_HIGHLANDER(victim) )
    {
	send_to_char( "You cannot make a pact with a highlander.\n\r", ch );
	return;
    }

    if ( IS_ANGEL(ch) )
    {
	if (ch->exp < 777)
	{
	    send_to_char("You cannot afford the 777 exp to make a pact.\n\r",ch);
	    return;
	}
    }
    else
    {
	if (ch->exp < 666)
	{
	    send_to_char("You cannot afford the 666 exp to make a pact.\n\r",ch);
	    return;
	}
    }

    if ( IS_ANGEL(ch) )
    {
	ch->exp = ch->exp - 777;
	act("You make $N an angelic champion!", ch, NULL, victim, TO_CHAR);
	act("$n makes $N an angelic champion!", ch, NULL, victim, TO_NOTVICT);
	act("$n makes you an angelic champion!", ch, NULL, victim, TO_VICT);
	victim->alignment = 1000;
    }
    else
    {
	ch->exp = ch->exp - 666;
	act("You make $N a demonic champion!", ch, NULL, victim, TO_CHAR);
	act("$n makes $N a demonic champion!", ch, NULL, victim, TO_NOTVICT);
	act("$n makes you a demonic champion!", ch, NULL, victim, TO_VICT);
	victim->alignment = -1000;
    }
    SET_BIT(victim->act, PLR_CHAMPION);
    REMOVE_BIT(victim->act, PLR_DEMON);
    mortal_vamp(victim);
    REMOVE_BIT(victim->act, PLR_WOLFMAN);
    REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
    REMOVE_BIT(victim->act, PLR_WIZINVIS);
    REMOVE_BIT(victim->extra, EXTRA_SIRE);
    REMOVE_BIT(victim->extra, EXTRA_PRINCE);
    victim->vampgen = 0;
    victim->vampaff = 0;
    victim->vamppass = 0;
    victim->vamppass = 0;
    victim->pcdata->wolf = 0;
    victim->pcdata->disc[0] = 0;
    victim->pcdata->disc[1] = 0;
    victim->pcdata->disc[2] = 0;
    victim->pcdata->disc[3] = 0;
    victim->pcdata->disc[4] = 0;
    victim->pcdata->disc[5] = 0;
    victim->pcdata->disc[6] = 0;
    victim->pcdata->disc[7] = 0;
    victim->pcdata->disc[8] = 0;
    victim->pcdata->disc[9] = 0;
    victim->pcdata->disc[10] = 0;
    victim->class = 0;

    free_string(victim->morph);
    victim->clan=str_dup("");
    free_string(victim->clan);
    victim->clan=str_dup("");
    free_string(victim->lord);
    if (IS_SET(ch->act,PLR_DEMON)) victim->lord=str_dup(ch->name);
    else victim->lord=str_dup(ch->lord);
    if ( IS_ANGEL(ch) )
	victim->class = CLASS_ANGEL;
    else
	victim->class = CLASS_DEMON;

    do_autosave(ch,"");
    do_autosave(victim,"");
    return;
}

void do_offersoul( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_IMMUNE(ch, IMM_DEMON))
    {
	send_to_char("You will now demons to buy your soul.\n\r",ch);
	SET_BIT(ch->immune, IMM_DEMON);
	return;
    }
    send_to_char("You will no longer allow demons to buy your soul.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_DEMON);
    return;
}

void do_weaponform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    if (IS_NPC(ch)) return;
    if (!IS_DEMON(ch))
    {
	send_to_char( "Huh?\n\r", ch);
	return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char( "You cannot do this while polymorphed.\n\r", ch);
	return;
    }
    if (ch->pcdata->disc[10] < 1)
    {
	send_to_char( "You don't have the ability to change into a weapon.\n\r", ch);
	return;
    }
    if ((obj = create_object(get_obj_index(ch->pcdata->disc[10]),60)) == NULL)
    {
	send_to_char( "You don't have the ability to change into a weapon.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_WEBBED))
    {
	send_to_char( "Not with all this sticky webbing on.\n\r", ch);
	return;
    }
    obj_to_room(obj,ch->in_room);
    SET_BIT(obj->quest, QUEST_VANISH);
    act("$n transforms into $p and falls to the ground.",ch,obj,NULL,TO_ROOM);
    act("You transform into $p and fall to the ground.",ch,obj,NULL,TO_CHAR);
    ch->pcdata->obj_vnum = ch->pcdata->disc[10];
    obj->chobj = ch;
    ch->pcdata->chobj = obj;
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->extra, EXTRA_OSWITCH);
    free_string(ch->morph);
    ch->morph = str_dup(obj->short_descr);
    return;
}

void do_humanform( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if (IS_NPC(ch)) return;
    if ( ( obj = ch->pcdata->chobj ) == NULL )
    {
	send_to_char("You are already in human form.\n\r",ch);
	return;
    }

    ch->pcdata->obj_vnum = 0;
    obj->chobj = NULL;
    ch->pcdata->chobj = NULL;
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
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

void do_champions( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char lord[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_DEMON) && !IS_SET(ch->act, PLR_CHAMPION) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (strlen(ch->lord) < 2 && !IS_SET(ch->act, PLR_DEMON) )
    {
	send_to_char("But you don't follow any demon!\n\r",ch);
	return;
    }

    if (IS_SET(ch->act, PLR_DEMON))
	strcpy(lord,ch->name);
    else 
    {
	strcpy(lord,ch->lord);
	sprintf( buf, "The champions of %s:\n\r", lord );
	send_to_char( buf, ch );
    }
    send_to_char("[      Name      ] [ Hits ] [ Mana ] [ Move ] [  Exp  ] [       Power        ]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) || !IS_DEMON(gch) ) continue;
	if ( ( strlen(gch->lord) > 1 && !str_cmp(gch->lord,lord) ) || 
	    !str_cmp(gch->name,lord) || gch == ch )
	{
	    sprintf( buf,
	    "[%-16s] [%-6d] [%-6d] [%-6d] [%7d] [ %-9d%9d ]\n\r",
		capitalize( gch->name ),
		gch->hit,gch->mana,gch->move,
		gch->exp, gch->pcdata->power[0], gch->pcdata->power[1]);
		send_to_char( buf, ch );
	}
    }
    return;
}

void do_eyespy( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *familiar;

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_DEMON) && !IS_SET(ch->act, PLR_CHAMPION))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_EYESPY))
    {
	send_to_char("You haven't been granted the gift of Eye Spy.\n\r",ch);
	return;
    }
    if (IS_HEAD(ch,LOST_EYE_L) && IS_HEAD(ch,LOST_EYE_R))
    {
	send_to_char( "But you don't have any more eyes to pluck out!\n\r", ch );
	return;
    }
    if (!IS_HEAD(ch,LOST_EYE_L) && number_range(1,2) == 1)
    {
	act( "You pluck out your left eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s left eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_L);
    }
    else if (!IS_HEAD(ch,LOST_EYE_R))
    {
	act( "You pluck out your right eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s right eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_R);
    }
    else
    {
	act( "You pluck out your left eyeball and throw it to the ground.", ch, NULL, NULL, TO_CHAR );
	act( "$n plucks out $s left eyeball and throws it to the ground.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->loc_hp[LOC_HEAD], LOST_EYE_L);
    }
    if ( ( familiar = ch->pcdata->familiar ) != NULL ) 
    {
	make_part(ch,"eyeball");
	return;
    }

    victim = create_mobile( get_mob_index( MOB_VNUM_EYE ) );
    if (victim == NULL)
    {send_to_char("Error - please inform KaVir.\n\r",ch); return;}

    SET_BIT(victim->act, ACT_NOPARTS);

    char_to_room( victim, ch->in_room );

    ch->pcdata->familiar = victim;
    victim->wizard = ch;
    return;
}

void do_inferno( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int sn;

    if (IS_NPC(ch)) return;

    if (IS_SET(ch->act, PLR_DEMON) || IS_SET(ch->act, PLR_CHAMPION))
    {
	if (!IS_SET(ch->pcdata->disc[C_POWERS], DEM_INFERNO))
	{
	    send_to_char("You haven't been granted the gift of inferno.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( ( sn = skill_lookup( "fire breath" ) ) < 0 )
    {
	send_to_char("Bug - Please inform KaVir.\n\r",ch);
	return;
    }
    if (ch->hit >= 0 || ch->position >= POS_STUNNED)
    {
	send_to_char("You are not injured enough to release an inferno.\n\r",ch);
	return;
    }
    if (ch->mana < 1000)
    {
	send_to_char("You have insufficient energy to release an inferno.\n\r",ch);
	return;
    }
    ch->mana -= 1000;
    act("$n's body errupts into an inferno of flames!",ch,NULL,NULL,TO_ROOM);
    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;
	if (ch == vch || is_safe(ch,vch)) continue;
	damage(ch,vch,ch->max_hit,sn);
    }
    WAIT_STATE(ch,24);
    return;
}

void do_wallflames( CHAR_DATA *ch, char *argument )
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

    if (!IS_DEMON(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_FIREWALL))
    {
	send_to_char("You haven't been granted the gift of Fire Wall.\n\r",ch);
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
    obj->name = str_dup("wall searing flames");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of searing flames");
    sprintf(buf,"A wall of searing flames blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_FLAME);

    if (direction == DIR_UP)
	sprintf(buf,"Flames burst from the ground and form a wall above you.");
    else if (direction == DIR_DOWN)
	sprintf(buf,"Flames burst from the ground and form a wall below you.");
    else
	sprintf(buf,"Flames burst from the ground and form a wall to the %s.",arg);
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
    obj->name = str_dup("wall searing flames");
    free_string(obj->short_descr);
    obj->short_descr = str_dup("a wall of searing flames");
    sprintf(buf,"A wall of searing flames blocks your exit %s.",arg);
    free_string(obj->description);
    obj->description = str_dup(buf);
    obj_to_room( obj, ch->in_room );

    SET_BIT(pexit->exit_info, EX_WALL_FLAME);

    if (rev == DIR_UP)
	sprintf(buf,"Flames burst from the ground and form a wall above you.");
    else if (rev == DIR_DOWN)
	sprintf(buf,"Flames burst from the ground and form a wall below you.");
    else
	sprintf(buf,"Flames burst from the ground and form a wall to the %s.",arg);
    act( buf, ch, NULL, NULL, TO_ROOM );

    char_from_room(ch);
    char_to_room(ch, location);

    return;
}

void do_hooves( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_HOOVES))
    {
	send_to_char("You haven't been granted the gift of Hooves.\n\r",ch);
	return;
    }

    if (IS_DEMAFF(ch,DEM_HOOVES) )
    {
	send_to_char("Your hooves transform into feet.\n\r",ch);
	act("$n's hooves transform back into $s feet.", ch, NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->pcdata->disc[C_CURRENT], DEM_HOOVES);
	return;
    }
    send_to_char("Your feet transform into hooves.\n\r",ch);
    act("$n's feet transform into hooves.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->disc[C_CURRENT], DEM_HOOVES);
    return;
}

void do_imp( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch) || IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_IMP))
    {
	send_to_char("You haven't been granted an imp.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	if (IS_MORE(ch, MORE_GUARDIAN) )
	{
	    guardian_message( ch );
	    send_to_char("I shall return when you call me, Master.\n\r",ch);
	    send_to_char( "The little imp takes off from your shoulder and flies away.\n\r", ch );
	    act("The little imp takes off from $n's shoulder and flies away.",ch,NULL,NULL,TO_ROOM);
	    REMOVE_BIT(ch->more, MORE_GUARDIAN);
	    return;
	}
	send_to_char( "A little imp flies down from the sky and lands on your shoulder.\n\r", ch );
	act("A little imp flies down from the sky and lands on $n's shoulder.",ch,NULL,NULL,TO_ROOM);
	guardian_message( ch );
	send_to_char("I have arrived, Master.\n\r",ch);
	SET_BIT(ch->more, MORE_GUARDIAN);
        return;
    }

    if (!IS_MORE(ch, MORE_GUARDIAN) )
    {
        send_to_char( "First you need to type 'imp' to summon your imp.\n\r", ch );
        return;
    }

    act( "$n whispers something to the little imp perched on $s shoulder.",ch,NULL,NULL,TO_ROOM);

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Transport what to whom with your imp?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (victim == ch)
    {
	send_to_char( "Your little imp looks confused!\n\r", ch );
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

    if (obj->weight > 50)
    {
	send_to_char( "That item weighs too much for your imp to carry.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_TRANSPORT))
    {
	send_to_char( "You are unable to transport anything to them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( IS_AFFECTED(ch,AFF_ETHEREAL) || IS_AFFECTED(victim,AFF_ETHEREAL) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_number + 1 > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via an imp.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    act("You give $p to your little pet imp.",ch,obj,NULL,TO_CHAR);
    act("$n gives $p to the little imp on $s shoulder.",ch,obj,NULL,TO_ROOM);
    send_to_char( "The little imp takes off from your shoulder and flies away.\n\r", ch );
    act("The little imp takes off from $n's shoulder and flies away.",ch,NULL,NULL,TO_ROOM);

    obj_from_char(obj);
    obj_to_char(obj,victim);

    send_to_char( "A little imp flies down from the sky and lands on your shoulder.\n\r", victim );
    act("A little imp flies down from the sky and lands on $n's shoulder.",victim,NULL,NULL,TO_ROOM);
    act("The little imp gives you $p.",victim,obj,NULL,TO_CHAR);
    act("The little imp gives $n $p.",victim,obj,NULL,TO_ROOM);
    send_to_char( "The little imp takes off from your shoulder and flies away.\n\r", victim );
    act("The little imp takes off from $n's shoulder and flies away.",victim,NULL,NULL,TO_ROOM);

    send_to_char( "Your little imp flies down from the sky and returns to your shoulder.\n\r", ch );
    act("The little imp flies down from the sky and returns to $n's shoulder.",ch,NULL,NULL,TO_ROOM);

    do_autosave(ch,"");
    do_autosave(victim,"");
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just sent $p to you via an imp.",ch,obj,victim,TO_VICT);
    }
    return;
}

void do_cherub( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_IMP))
    {
	send_to_char("You haven't been granted a cherub.\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	if (IS_MORE(ch, MORE_GUARDIAN) )
	{
	    guardian_message( ch );
	    send_to_char("I shall return when you call me, Master.\n\r",ch);
	    send_to_char( "The little cherub takes off from your shoulder and flies away.\n\r", ch );
	    act("The little cherub takes off from $n's shoulder and flies away.",ch,NULL,NULL,TO_ROOM);
	    REMOVE_BIT(ch->more, MORE_GUARDIAN);
	    return;
	}
	send_to_char( "A little cherub flies down from the sky and lands on your shoulder.\n\r", ch );
	act("A little cherub flies down from the sky and lands on $n's shoulder.",ch,NULL,NULL,TO_ROOM);
	guardian_message( ch );
	send_to_char("I have arrived, Master.\n\r",ch);
	SET_BIT(ch->more, MORE_GUARDIAN);
        return;
    }

    if (!IS_MORE(ch, MORE_GUARDIAN) )
    {
        send_to_char( "First you need to type 'cherub' to summon your cherub.\n\r", ch );
        return;
    }

    act( "$n whispers something to the little cherub perched on $s shoulder.",ch,NULL,NULL,TO_ROOM);

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Transport what to whom with your cherub?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (victim == ch)
    {
	send_to_char( "Your little cherub looks confused!\n\r", ch );
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

    if (obj->weight > 50)
    {
	send_to_char( "That item weighs too much for your cherub to carry.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_TRANSPORT))
    {
	send_to_char( "You are unable to transport anything to them.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( IS_AFFECTED(ch,AFF_ETHEREAL) || IS_AFFECTED(victim,AFF_ETHEREAL) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_number + 1 > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via a cherub.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    act("You give $p to your little cherub.",ch,obj,NULL,TO_CHAR);
    act("$n gives $p to the little cherub on $s shoulder.",ch,obj,NULL,TO_ROOM);
    send_to_char( "The little cherub takes off from your shoulder and flies away.\n\r", ch );
    act("The little cherub takes off from $n's shoulder and flies away.",ch,NULL,NULL,TO_ROOM);

    obj_from_char(obj);
    obj_to_char(obj,victim);

    send_to_char( "A little cherub flies down from the sky and lands on your shoulder.\n\r", victim );
    act("A little cherub flies down from the sky and lands on $n's shoulder.",victim,NULL,NULL,TO_ROOM);
    act("The little cherub gives you $p.",victim,obj,NULL,TO_CHAR);
    act("The little cherub gives $n $p.",victim,obj,NULL,TO_ROOM);
    send_to_char( "The little cherub takes off from your shoulder and flies away.\n\r", victim );
    act("The little cherub takes off from $n's shoulder and flies away.",victim,NULL,NULL,TO_ROOM);

    send_to_char( "Your little cherub flies down from the sky and returns to your shoulder.\n\r", ch );
    act("The little cherub flies down from the sky and returns to $n's shoulder.",ch,NULL,NULL,TO_ROOM);

    do_autosave(ch,"");
    do_autosave(victim,"");
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just sent $p to you via a cherub.",ch,obj,victim,TO_VICT);
    }
    return;
}

void do_demonform( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_DEMON(ch) || IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_DEMONFORM))
    {
	send_to_char("You haven't been granted the gift of Demonform.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
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
	send_to_char("Your body shrinks and distorts back to its normal form.\n\r",ch);
	if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	    act("An enormous hairy demonic beast's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	else
	    act("A great demonic beast's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
    {
	send_to_char("Your body grows and distorts into an enormous hairy demonic beast.\n\r",ch);
	act("$n's body grows and distorts into an enormous hairy demonic beast.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("Your body grows and distorts into a great demonic beast.\n\r",ch);
	act("$n's body grows and distorts into a great demonic beast.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->vampaff, VAM_CLAWS);
    SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    SET_BIT(ch->polyaff, POLY_ZULO);
    free_string( ch->morph );
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	ch->morph = str_dup( "an enormous hairy demonic beast" );
    else
	ch->morph = str_dup( "a great demonic beast" );
    return;
}

void do_angelform( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_ANGEL(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_DEMPOWER( ch, DEM_DEMONFORM))
    {
	send_to_char("You haven't been granted the gift of Angelform.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if (IS_POLYAFF(ch,POLY_ZULO))
    {
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
	send_to_char("Your body shrinks and distorts back to its normal form.\n\r",ch);
	if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	    act("An enormous golden werewolf's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	else
	    act("A beautiful golden beast's body shrinks and distorts into the form of $n.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
    {
	send_to_char("Your body grows and distorts into an enormous golden werewolf.\n\r",ch);
	act("$n's body grows and distorts into an enormous golden werewolf.",ch,NULL,NULL,TO_ROOM);
    }
    else
    {
	send_to_char("Your body grows and distorts into a beautiful golden beast.\n\r",ch);
	act("$n's body grows and distorts into a beautiful golden beast.",ch,NULL,NULL,TO_ROOM);
    }
    SET_BIT(ch->vampaff, VAM_CLAWS);
    SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    SET_BIT(ch->extra, EXTRA_VICISSITUDE);
    SET_BIT(ch->polyaff, POLY_ZULO);
    free_string( ch->morph );
    if (IS_ABOMINATION(ch) && IS_SET(ch->act, PLR_WOLFMAN))
	ch->morph = str_dup( "an enormous golden werewolf" );
    else
	ch->morph = str_dup( "a beautiful golden beast" );
    return;
}

