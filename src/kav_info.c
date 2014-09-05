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


void do_immune( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("                                -= Immunities =-\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    /* Display weapon resistances */
    send_to_char("Weapons:",ch);
    if (!(!IS_IMMUNE(ch,IMM_SLASH) && !IS_IMMUNE(ch,IMM_STAB) && 
	  !IS_IMMUNE(ch,IMM_SMASH) && !IS_IMMUNE(ch,IMM_ANIMAL) &&
	  !IS_IMMUNE(ch,IMM_MISC )) )
    {
    	if (IS_IMMUNE(ch,IMM_SLASH )) send_to_char(" Slash Slice",ch);
    	if (IS_IMMUNE(ch,IMM_STAB  )) send_to_char(" Stab Pierce",ch);
    	if (IS_IMMUNE(ch,IMM_SMASH )) send_to_char(" Blast Crush Pound",ch);
    	if (IS_IMMUNE(ch,IMM_ANIMAL)) send_to_char(" Claw Bite",ch);
    	if (IS_IMMUNE(ch,IMM_MISC  )) send_to_char(" Grep Whip Suck",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display spell immunities */
    send_to_char("Spells :",ch);
    if (!(!IS_IMMUNE(ch,IMM_CHARM)  && !IS_IMMUNE(ch,IMM_HEAT)      && 
	  !IS_IMMUNE(ch,IMM_COLD)   && !IS_IMMUNE(ch,IMM_LIGHTNING) &&
	  !IS_IMMUNE(ch,IMM_ACID)   && !IS_IMMUNE(ch,IMM_SUMMON)    &&
	  !IS_IMMUNE(ch,IMM_VOODOO) && !IS_IMMUNE(ch,IMM_SLEEP)    &&
	  !IS_IMMUNE(ch,IMM_DRAIN)))
    {
    	if (IS_IMMUNE(ch,IMM_CHARM     )) send_to_char(" Charm",ch);
    	if (IS_IMMUNE(ch,IMM_HEAT      )) send_to_char(" Heat",ch);
    	if (IS_IMMUNE(ch,IMM_COLD      )) send_to_char(" Cold",ch);
    	if (IS_IMMUNE(ch,IMM_LIGHTNING )) send_to_char(" Lightning",ch);
    	if (IS_IMMUNE(ch,IMM_ACID      )) send_to_char(" Acid",ch);
    	if (!IS_IMMUNE(ch,IMM_SUMMON   )) send_to_char(" Summon",ch);
    	if (IS_IMMUNE(ch,IMM_VOODOO    )) send_to_char(" Voodoo",ch);
    	if (IS_IMMUNE(ch,IMM_SLEEP     )) send_to_char(" Sleep",ch);
    	if (IS_IMMUNE(ch,IMM_DRAIN     )) send_to_char(" Drain",ch);
    	if (IS_IMMUNE(ch,IMM_TRANSPORT )) send_to_char(" Transport",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display skill immunities */
    send_to_char("Skills :",ch);
    if (!(!IS_IMMUNE(ch,IMM_HURL) && !IS_IMMUNE(ch,IMM_BACKSTAB) && 
	  !IS_IMMUNE(ch,IMM_KICK) && !IS_IMMUNE(ch,IMM_DISARM)   &&
	  !IS_IMMUNE(ch,IMM_STEAL)))
    {
    	if (IS_IMMUNE(ch,IMM_HURL      )) send_to_char(" Hurl",ch);
    	if (IS_IMMUNE(ch,IMM_BACKSTAB  )) send_to_char(" Backstab",ch);
    	if (IS_IMMUNE(ch,IMM_KICK      )) send_to_char(" Kick",ch);
    	if (IS_IMMUNE(ch,IMM_DISARM    )) send_to_char(" Disarm",ch);
    	if (IS_IMMUNE(ch,IMM_STEAL     )) send_to_char(" Steal",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display vampire immunities */
    send_to_char("Vampire:",ch);
    if (!( IS_IMMUNE(ch,IMM_VAMPIRE) && !IS_IMMUNE(ch,IMM_STAKE) && 
	  !IS_IMMUNE(ch,IMM_SUNLIGHT)))
    {
    	if (!IS_IMMUNE(ch,IMM_VAMPIRE  )) send_to_char(" Bite",ch);
    	if  (IS_IMMUNE(ch,IMM_STAKE    )) send_to_char(" Stake",ch);
    	if  (IS_IMMUNE(ch,IMM_SUNLIGHT )) send_to_char(" Sunlight",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}


void skillstance( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf        [MAX_INPUT_LENGTH];
    char bufskill   [25];
    char stancename [10];
    int stance, con_stance;

    stance = victim->stance[0];
    if (stance < 1 || stance > 15) return;
    if (IS_NPC(victim) && (stance < 1 || stance > 10)) return;

    if (!IS_NPC(victim) && IS_EXTRA(victim, EXTRA_FAKE_CON))
	con_stance = victim->pcdata->fake_stance;
    else
	con_stance = victim->stance[stance];

         if (con_stance <= 0  ) sprintf(bufskill,"completely unskilled in");
    else if (con_stance <= 25 ) sprintf(bufskill,"an apprentice of");
    else if (con_stance <= 50 ) sprintf(bufskill,"a trainee of");
    else if (con_stance <= 75 ) sprintf(bufskill,"a student of");
    else if (con_stance <= 100) sprintf(bufskill,"fairly experienced in");
    else if (con_stance <= 125) sprintf(bufskill,"well trained in");
    else if (con_stance <= 150) sprintf(bufskill,"highly skilled in");
    else if (con_stance <= 175) sprintf(bufskill,"an expert of");
    else if (con_stance <= 199) sprintf(bufskill,"a master of");
    else if (con_stance >= 200) sprintf(bufskill,"a grand master of");
    else return;
         if (stance == STANCE_VIPER   ) sprintf(stancename,"viper"   );
    else if (stance == STANCE_CRANE   ) sprintf(stancename,"crane"   );
    else if (stance == STANCE_CRAB    ) sprintf(stancename,"crab"    );
    else if (stance == STANCE_MONGOOSE) sprintf(stancename,"mongoose");
    else if (stance == STANCE_BULL    ) sprintf(stancename,"bull"    );
    else if (stance == STANCE_MANTIS  ) sprintf(stancename,"mantis"  );
    else if (stance == STANCE_DRAGON  ) sprintf(stancename,"dragon"  );
    else if (stance == STANCE_TIGER   ) sprintf(stancename,"tiger"   );
    else if (stance == STANCE_MONKEY  ) sprintf(stancename,"monkey"  );
    else if (stance == STANCE_SWALLOW ) sprintf(stancename,"swallow" );
    else if (!IS_NPC(victim) && stance >= 11)
    {
	if (strlen(victim->pcdata->stancename) < 2) return;
	strcpy(stancename,victim->pcdata->stancename );
    }
    else return;
    if (ch == victim)
    	sprintf(buf,"You are %s the %s stance.",bufskill,stancename);
    else
    	sprintf(buf,"$N is %s the %s stance.",bufskill,stancename);
    act(buf,ch,NULL,victim,TO_CHAR);
    return;
}

void do_spell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	show_spell(ch,0);
	show_spell(ch,1);
	show_spell(ch,2);
	show_spell(ch,3);
	show_spell(ch,4);
    }
    else if (!str_cmp(arg,"purple")) show_spell(ch,0);
    else if (!str_cmp(arg,"red"))    show_spell(ch,1);
    else if (!str_cmp(arg,"blue"))   show_spell(ch,2);
    else if (!str_cmp(arg,"green"))  show_spell(ch,3);
    else if (!str_cmp(arg,"yellow")) show_spell(ch,4);
    else send_to_char("You know of no such magic.\n\r",ch);
    return;
}

void show_spell( CHAR_DATA *ch, int dtype )
{
    char buf[MAX_INPUT_LENGTH];
    char bufskill[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if      (ch->spl[dtype] == 00 ) sprintf(bufskill,"untrained at");
    else if (ch->spl[dtype] <= 25 ) sprintf(bufskill,"an apprentice at");
    else if (ch->spl[dtype] <= 50 ) sprintf(bufskill,"a student at");
    else if (ch->spl[dtype] <= 75 ) sprintf(bufskill,"a scholar at");
    else if (ch->spl[dtype] <= 100) sprintf(bufskill,"a magus at");
    else if (ch->spl[dtype] <= 125) sprintf(bufskill,"an adept at");
    else if (ch->spl[dtype] <= 150) sprintf(bufskill,"a mage at");
    else if (ch->spl[dtype] <= 175) sprintf(bufskill,"a warlock at");
    else if (ch->spl[dtype] <= 199) sprintf(bufskill,"a master wizard at");
    else if (ch->spl[dtype] >= 240 && IS_MAGE(ch)) sprintf(bufskill,"the complete master of");
    else if (ch->spl[dtype] >= 200) sprintf(bufskill,"a grand sorcerer at");
    else return;
    if      (dtype == 0)
	sprintf(buf,"You are %s purple magic.\n\r",bufskill);
    else if (dtype == 1)
	sprintf(buf,"You are %s red magic.\n\r",bufskill);
    else if (dtype == 2)
	sprintf(buf,"You are %s blue magic.\n\r",bufskill);
    else if (dtype == 3)
	sprintf(buf,"You are %s green magic.\n\r",bufskill);
    else if (dtype == 4)
	sprintf(buf,"You are %s yellow magic.\n\r",bufskill);
    else return;
    send_to_char(buf,ch);
    return;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;

    location = ch->in_room;

    send_to_char("[North]\n\r",ch);
    do_spydirection(ch,"n");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[East]\n\r",ch);
    do_spydirection(ch,"e");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[South]\n\r",ch);
    do_spydirection(ch,"s");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[West]\n\r",ch);
    do_spydirection(ch,"w");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[Up]\n\r",ch);
    do_spydirection(ch,"u");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[Down]\n\r",ch);
    do_spydirection(ch,"d");
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_spy( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    char            arg1      [MAX_INPUT_LENGTH];
    int             door;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char("Which direction do you wish to spy?\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) )
	door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east" ) )
	door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) )
	door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west" ) )
	door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up" ) )
	door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down" ) )
	door = 5;
    else
    {
	send_to_char("You can only spy people north, south, east, west, up or down.\n\r", ch);
	return;
    }

    location = ch->in_room;

    send_to_char("[Short Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("[Medium Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("[Long Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);

    /* Move them back */
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_spydirection( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *vch;
    CHAR_DATA       *vch_next;
    char            arg      [MAX_INPUT_LENGTH];
    char            buf      [MAX_INPUT_LENGTH];
    int             door;
    bool            is_empty;

    argument = one_argument( argument, arg );

    is_empty = TRUE;

    if ( arg[0] == '\0' )
	return;

    if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else return;

    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	send_to_char("   No exit.\n\r",ch);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	send_to_char("   Closed door.\n\r",ch);
	return;
    }
    if (IS_SET(world_affects, WORLD_FOG))
    {
	send_to_char("   Too foggy to tell.\n\r",ch);
	return;
    }
    if (room_is_dark(to_room) && !IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
    {
	send_to_char("   Too dark to tell.\n\r",ch);
	return;
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch == ch )
	    continue;
	if ( !can_see(ch,vch) )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if (!IS_NPC(vch) && !IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"   %s (Player)\n\r",vch->name);
	    else if (!IS_NPC(vch) && IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"   %s (Player)\n\r",vch->morph);
	    else if (IS_NPC(vch))
		sprintf(buf,"   %s\n\r",capitalize(vch->short_descr));
	    send_to_char(buf,ch);
	    is_empty = FALSE;
	    continue;
	}
    }
    if (is_empty)
    {
	send_to_char("   Nobody here.\n\r",ch);
	return;
    }
    return;
}


void take_item( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if (obj == NULL) return;
    obj_from_char(obj);
    obj_to_room(obj,ch->in_room);
    act("You wince in pain and $p falls to the ground.",ch,obj,NULL,TO_CHAR);
    act("$n winces in pain and $p falls to the ground.",ch,obj,NULL,TO_ROOM);
    return;
}

void birth_date( CHAR_DATA *ch, bool is_self )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char mstatus[20];
    char dstatus[20];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int dd = 0;
    int mm = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    if (IS_NPC(ch)) return;

    if (is_self)
	strcpy(buf,ch->createtime);
    else
	strcpy(buf,ch->pcdata->conception);
    if (strlen(buf) < 24) return;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else if (total >= 30)
	{
	    total -= 30;
	    mm += 1;
	}
	else
	{
	    dd += total;
	    break;
	}
    }
    if (mm == 1) strcpy(mstatus,"month");
	else strcpy(mstatus,"months");
    if (dd == 1) strcpy(dstatus,"day");
	else strcpy(dstatus,"days");
    if (is_self)
    {
	if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17;
	sprintf(buf,"You are %d years, %d %s and %d %s old.\n\r",
	    yy,mm,mstatus,dd,dstatus);
	send_to_char(buf,ch);
    }
    else
    {
	if (yy > 0 || (dd > 0 && mm >= 9) || IS_EXTRA(ch, EXTRA_LABOUR))
	{
	    if (!IS_EXTRA(ch, EXTRA_LABOUR)) SET_BIT(ch->extra, EXTRA_LABOUR);
	    sprintf(buf,"You are ready to give birth.\n\r");
	    send_to_char(buf,ch);
	}
	else if (yy > 0 || mm > 0)
	{
	    sprintf(buf,"You are %d %s and %d %s pregnant.\n\r",
	    mm,mstatus,dd,dstatus);
	    send_to_char(buf,ch);
	}
    }
    return;
}

void other_age( CHAR_DATA *ch, int extra, bool is_preg, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char mstatus[20];
    char dstatus[20];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int dd = 0;
    int mm = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    strcpy(buf,argument);
    if (strlen(buf) < 24) return;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else if (total >= 30)
	{
	    total -= 30;
	    mm += 1;
	}
	else
	{
	    dd += total;
	    break;
	}
    }
    if (mm == 1) strcpy(mstatus,"month");
	else strcpy(mstatus,"months");
    if (dd == 1) strcpy(dstatus,"day");
	else strcpy(dstatus,"days");
    if (!is_preg)
    {
	if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17;
	sprintf(buf,"Age: %d years, %d %s and %d %s old.\n\r",
	    yy,mm,mstatus,dd,dstatus);
	send_to_char(buf,ch);
    }
    else
    {
	if (yy > 0 || (dd > 0 && mm >= 9) || IS_SET(extra, EXTRA_LABOUR))
	{
	    sprintf(buf,"She is ready to give birth.\n\r");
	    send_to_char(buf,ch);
	}
	else if (yy > 0 || mm > 0)
	{
	    sprintf(buf,"She is %d %s and %d %s pregnant.\n\r",
	    mm,mstatus,dd,dstatus);
	    send_to_char(buf,ch);
	}
    }
    return;
}

int years_old( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    if (IS_NPC(ch)) return 0;

    strcpy(buf,ch->createtime);
    if (strlen(buf) < 24) return 0;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return 0;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return 0;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else break;
    }
    if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17; /* Starting age for all players */
    return yy;
}

void do_huh( CHAR_DATA *ch, char *argument )
{
    send_to_char("Huh?\n\r",ch);
    return;
}

void do_consent( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
/*
	send_to_char("This has been temporarily disabled.\n\r",ch);
	return;
*/
/*
    if ( strlen(ch->pcdata->marriage) < 2 || !IS_EXTRA(ch, EXTRA_MARRIED) )
    {
	send_to_char("But you are not even married!\n\r",ch);
	return;
    }
*/
    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (ch == victim)
    {
	send_to_char("Not on yourself!\n\r",ch);
	return;
    }

    if (strlen(ch->pcdata->marriage) > 1 && 
	str_cmp(ch->pcdata->marriage,victim->name))
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are married to someone else!\n\r",ch);
	else
	    send_to_char("But you are engaged to someone else!\n\r",ch);
	return;
    }
    if (strlen(victim->pcdata->marriage) > 1 && 
	str_cmp(victim->pcdata->marriage,ch->name))
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are married to someone else!\n\r",ch);
	else
	    send_to_char("But they are engaged to someone else!\n\r",ch);
	return;
    }

    ch->pcdata->partner = victim;
    act("You give $N permission to use xsocials on you.",ch,NULL,victim,TO_CHAR);
    act("$n has given you permission to use xsocials on $m.",ch,NULL,victim,TO_VICT);
    return;
}

void do_totalscore( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg [MAX_INPUT_LENGTH];
    int p_score = (ch->played + (int) (current_time - ch->logon) );
    int k_score = 0;
    int t_score = 0;
    int lowest = score_1;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if (arg[0] != '\0')
    {
	if (str_cmp(arg,"all"))
	{
	    send_to_char("Try 'totalscore all' to list top 3 scores.\n\r",ch);
	    return;
	}
	send_to_char("Highest scores as follows:\n\r",ch);
	if (score_1 >= score_2 && score_1 >= score_3)
	{
	    sprintf(buf,"First place is %s with %d points.\n\r",first_place,score_1);
	    send_to_char(buf,ch);
	    if (score_2 > score_3)
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",second_place,score_2);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",third_place,score_3);
		send_to_char(buf,ch);
	    }
	    else
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",third_place,score_3);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",second_place,score_2);
		send_to_char(buf,ch);
	    }
	}
	else if (score_2 >= score_1 && score_2 >= score_3)
	{
	    sprintf(buf,"First place is %s with %d points.\n\r",second_place,score_2);
	    send_to_char(buf,ch);
	    if (score_1 > score_3)
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",first_place,score_1);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",third_place,score_3);
		send_to_char(buf,ch);
	    }
	    else
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",third_place,score_3);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",first_place,score_1);
		send_to_char(buf,ch);
	    }
	}
	else if (score_3 >= score_1 && score_3 >= score_2)
	{
	    sprintf(buf,"First place is %s with %d points.\n\r",third_place,score_3);
	    send_to_char(buf,ch);
	    if (score_1 > score_2)
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",first_place,score_1);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",second_place,score_2);
		send_to_char(buf,ch);
	    }
	    else
	    {
		sprintf(buf,"Second place is %s with %d points.\n\r",second_place,score_2);
		send_to_char(buf,ch);
		sprintf(buf,"Third place is %s with %d points.\n\r",first_place,score_1);
		send_to_char(buf,ch);
	    }
	}
	else
	{
	    /* Should never reach this, but just in case... */
	    sprintf(buf,"%s: %d.\n\r",first_place,score_1);
	    send_to_char(buf,ch);
	    sprintf(buf,"%s: %d.\n\r",second_place,score_2);
	    send_to_char(buf,ch);
	    sprintf(buf,"%s: %d.\n\r",third_place,score_3);
	    send_to_char(buf,ch);
	}
	return;
    }

    if (p_score < 1) p_score = 1;

    k_score = ch->pcdata->score[SCORE_TOTAL_XP];

    if (k_score < 1) sprintf(buf,"Exp from kills / time played: +0.\n\r");
    else 
    {
	t_score = k_score/p_score;
	if (t_score < 0) t_score = 0;
	sprintf(buf,"Exp from kills / time played: +%d.\n\r",t_score);
    }
    send_to_char(buf,ch);

    p_score = ch->stance[1];
    if (ch->stance[2]  > ch->stance[1]) p_score = ch->stance[2];
    if (ch->stance[3]  > ch->stance[2]) p_score = ch->stance[3];
    if (ch->stance[4]  > ch->stance[3]) p_score = ch->stance[4];
    if (ch->stance[5]  > ch->stance[4]) p_score = ch->stance[5];
    p_score *= 0.25;
    t_score += p_score;
    sprintf(buf,"Best stance (out of 50): +%d.\n\r",p_score);
    send_to_char(buf,ch);

    p_score = ch->wpn[0];
    if (ch->wpn[1]  > ch->wpn[0] ) p_score = ch->wpn[1];
    if (ch->wpn[2]  > ch->wpn[1] ) p_score = ch->wpn[2];
    if (ch->wpn[3]  > ch->wpn[2] ) p_score = ch->wpn[3];
    if (ch->wpn[4]  > ch->wpn[3] ) p_score = ch->wpn[4];
    if (ch->wpn[5]  > ch->wpn[4] ) p_score = ch->wpn[5];
    if (ch->wpn[6]  > ch->wpn[5] ) p_score = ch->wpn[6];
    if (ch->wpn[7]  > ch->wpn[6] ) p_score = ch->wpn[7];
    if (ch->wpn[8]  > ch->wpn[7] ) p_score = ch->wpn[8];
    if (ch->wpn[9]  > ch->wpn[8] ) p_score = ch->wpn[9];
    if (ch->wpn[10] > ch->wpn[9] ) p_score = ch->wpn[10];
    if (ch->wpn[11] > ch->wpn[10]) p_score = ch->wpn[11];
    if (ch->wpn[12] > ch->wpn[11]) p_score = ch->wpn[12];
    p_score *= 0.25;
    t_score += p_score;
    sprintf(buf,"Best weapon skill (out of 50): +%d.\n\r",p_score);
    send_to_char(buf,ch);

    p_score = ch->spl[0];
    if (ch->spl[1]  > ch->spl[0]) p_score = ch->spl[1];
    if (ch->spl[2]  > ch->spl[1]) p_score = ch->spl[2];
    if (ch->spl[3]  > ch->spl[2]) p_score = ch->spl[3];
    if (ch->spl[4]  > ch->spl[3]) p_score = ch->spl[4];
    p_score *= 0.25;
    t_score += p_score;
    sprintf(buf,"Best magic (out of 50): +%d.\n\r",p_score);
    send_to_char(buf,ch);

    p_score = ch->pcdata->score[SCORE_HIGH_LEVEL];
    sprintf(buf,"Toughest mob killed: +%d.\n\r",p_score);
    send_to_char(buf,ch);
    t_score += p_score;

    k_score = ch->pcdata->score[SCORE_NUM_QUEST];
    if (k_score > 25) k_score = 25;
    sprintf(buf,"Ten times the number of completed quests (10 times %d, 250 max): %d.\n\r", k_score, (k_score * 10));
    send_to_char(buf,ch);
    t_score += (k_score * 10);
    p_score = ch->pcdata->score[SCORE_QUEST];
    if (p_score > 0 && ch->pcdata->score[SCORE_NUM_QUEST] > 0)
    {
	p_score /= ch->pcdata->score[SCORE_NUM_QUEST];
	sprintf(buf,"Average qp earned per completed quest: +%d.\n\r", p_score);
    }
    else sprintf(buf,"Average qp earned per completed quest: +0.\n\r");
    send_to_char(buf,ch);
    t_score += p_score;

    sprintf(buf,"Total score: %d.\n\r", t_score);
    send_to_char(buf,ch);

    if (score_2 < lowest) lowest = score_2;
    if (score_3 < lowest) lowest = score_3;
    if (!str_cmp(first_place, ch->name)) lowest = score_1;
    else if (!str_cmp(second_place, ch->name)) lowest = score_2;
    else if (!str_cmp(third_place, ch->name)) lowest = score_3;
    else if (t_score < lowest) return;
    if (lowest == score_1)
    {
	strcpy(first_place,ch->name);
	score_1 = t_score;
    }
    else if (lowest == score_2)
    {
	strcpy(second_place,ch->name);
	score_2 = t_score;
    }
    else if (lowest == score_3)
    {
	strcpy(third_place,ch->name);
	score_3 = t_score;
    }
    return;
}

int get_totalscore( CHAR_DATA *ch )
{
    int p_score = (ch->played + (int) (current_time - ch->logon) );
    int k_score = 0;
    int t_score = 0;
    int lowest = score_1;

    if (IS_NPC(ch)) return 0;

    if (p_score < 1) p_score = 1;

    k_score = ch->pcdata->score[SCORE_TOTAL_XP];

    if (k_score >= 1)
    {
	t_score = k_score/p_score;
	if (t_score < 0) t_score = 0;
    }

    p_score = ch->stance[1];
    if (ch->stance[2]  > ch->stance[1]) p_score = ch->stance[2];
    if (ch->stance[3]  > ch->stance[2]) p_score = ch->stance[3];
    if (ch->stance[4]  > ch->stance[3]) p_score = ch->stance[4];
    if (ch->stance[5]  > ch->stance[4]) p_score = ch->stance[5];
    p_score *= 0.25;
    t_score += p_score;

    p_score = ch->wpn[0];
    if (ch->wpn[1]  > ch->wpn[0] ) p_score = ch->wpn[1];
    if (ch->wpn[2]  > ch->wpn[1] ) p_score = ch->wpn[2];
    if (ch->wpn[3]  > ch->wpn[2] ) p_score = ch->wpn[3];
    if (ch->wpn[4]  > ch->wpn[3] ) p_score = ch->wpn[4];
    if (ch->wpn[5]  > ch->wpn[4] ) p_score = ch->wpn[5];
    if (ch->wpn[6]  > ch->wpn[5] ) p_score = ch->wpn[6];
    if (ch->wpn[7]  > ch->wpn[6] ) p_score = ch->wpn[7];
    if (ch->wpn[8]  > ch->wpn[7] ) p_score = ch->wpn[8];
    if (ch->wpn[9]  > ch->wpn[8] ) p_score = ch->wpn[9];
    if (ch->wpn[10] > ch->wpn[9] ) p_score = ch->wpn[10];
    if (ch->wpn[11] > ch->wpn[10]) p_score = ch->wpn[11];
    if (ch->wpn[12] > ch->wpn[11]) p_score = ch->wpn[12];
    p_score *= 0.25;
    t_score += p_score;

    p_score = ch->spl[0];
    if (ch->spl[1]  > ch->spl[0]) p_score = ch->spl[1];
    if (ch->spl[2]  > ch->spl[1]) p_score = ch->spl[2];
    if (ch->spl[3]  > ch->spl[2]) p_score = ch->spl[3];
    if (ch->spl[4]  > ch->spl[3]) p_score = ch->spl[4];
    p_score *= 0.25;
    t_score += p_score;

    p_score = ch->pcdata->score[SCORE_HIGH_LEVEL];
    t_score += p_score;

    k_score = ch->pcdata->score[SCORE_NUM_QUEST];
    if (k_score > 25) k_score = 25;
    t_score += (k_score * 10);
    p_score = ch->pcdata->score[SCORE_QUEST];
    if (p_score > 0 && ch->pcdata->score[SCORE_NUM_QUEST] > 0)
    {
	p_score /= ch->pcdata->score[SCORE_NUM_QUEST];
    }
    t_score += p_score;

    if (score_2 < lowest) lowest = score_2;
    if (score_3 < lowest) lowest = score_3;
    if (!str_cmp(first_place, ch->name)) lowest = score_1;
    else if (!str_cmp(second_place, ch->name)) lowest = score_2;
    else if (!str_cmp(third_place, ch->name)) lowest = score_3;
    else if (t_score < lowest) return t_score;
    if (lowest == score_1)
    {
	strcpy(first_place,ch->name);
	score_1 = t_score;
    }
    else if (lowest == score_2)
    {
	strcpy(second_place,ch->name);
	score_2 = t_score;
    }
    else if (lowest == score_3)
    {
	strcpy(third_place,ch->name);
	score_3 = t_score;
    }
    return t_score;
}

void clear_eq( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    OBJ_DATA *obj_next;
    int max_val;

    if (IS_NPC(ch)) return;
    ch->pcdata->qstats[QS_TOTAL] = 0;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    if (in_obj->carried_by == ch)
	    {
		if (obj->item_type == ITEM_PAGE) continue;
		if (IS_SET(obj->quest, QUEST_ARTIFACT)) continue;
		if (IS_SET(obj->spectype, SITEM_DEMONIC))
		{
		    if      (obj->specpower == ARM_GREY  ) max_val = 100;
		    else if (obj->specpower == ARM_PURPLE) max_val = 250;
		    else if (obj->specpower == ARM_RED   ) max_val = 750;
		    else if (obj->specpower == ARM_BRASS ) max_val = 1500;
		    else max_val = 0;
		}
		else max_val = 0;
		if (obj->carried_by != NULL)
		{obj_from_char(obj);obj_to_char(obj,ch);}
		else if (obj->in_obj != NULL)
		{obj_from_obj(obj);obj_to_char(obj,ch);}
		else if (obj->in_room != NULL)
		{obj_from_room(obj);obj_to_char(obj,ch);}
		if (in_obj->points > max_val && in_obj->item_type != ITEM_PAGE)
		{
		    ch->pcdata->qstats[QS_TOTAL] += obj->points;
		    if (obj->pIndexData->vnum == OBJ_VNUM_PROTOPLASM)
		    {
			ch->pcdata->qstats[QS_TOTAL] += 50;
			if (obj->item_type == ITEM_WEAPON)
			    ch->pcdata->qstats[QS_TOTAL] += 100;
			else if (obj->item_type == ITEM_ARMOR)
			    ch->pcdata->qstats[QS_TOTAL] += 50;
		    }
		}
		if (IS_SET(obj->spectype, SITEM_TELEPORTER))
		    ch->pcdata->qstats[QS_TOTAL] += 50;
	    }
	}
    }
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;

	if (obj->item_type == ITEM_PAGE) continue;
	if (IS_SET(obj->quest, QUEST_ARTIFACT)) continue;
	if (IS_SET(obj->spectype, SITEM_DEMONIC))
	{
	    if      (obj->specpower == ARM_GREY  ) max_val = 100;
	    else if (obj->specpower == ARM_PURPLE) max_val = 250;
	    else if (obj->specpower == ARM_RED   ) max_val = 750;
	    else if (obj->specpower == ARM_BRASS ) max_val = 1500;
	    else max_val = 0;
	}
	else max_val = 0;
	if (obj->points > max_val || IS_SET(obj->spectype, SITEM_TELEPORTER))
	{
	    if (IS_SET(obj->spectype, SITEM_DEMONIC) && obj->item_type != ITEM_PAGE)
	    {
		ch->pcdata->power[POWER_CURRENT] += 5000;
		ch->pcdata->power[POWER_TOTAL] += 5000;
	    }
	    if (obj->item_type == ITEM_QUESTCARD)
		ch->practice += obj->level;
	    act("$p vanishes in an explosion of paradox.",ch,obj,NULL,TO_CHAR);
	    act("$p vanishes in an explosion of paradox.",ch,obj,NULL,TO_ROOM);
	    obj_from_char( obj );
	    extract_obj( obj );
	}
    }
    ch->pcdata->quest += ch->pcdata->qstats[QS_TOTAL];
    SET_BIT(ch->extra, EXTRA_CLEARED_EQ);
    char_from_room(ch);
    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
    do_autosave(ch,"");
    return;
}

void do_exp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg [MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (arg[0] != '\0' && !str_cmp(arg,"help"))
    {
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char("                                -= Experience =-\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char("[STANCES] The first 5 stances are basic stances, while the others are advanced \n\r",ch);
    	send_to_char("stances.  In order to learn an advanced stance you must get 200 exp in two of \n\r",ch);
    	send_to_char("the appropriate basic stances (see 'help advancedstance' for details).\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char("[WEAPON SKILLS] Each skill improves the chance of hitting, damage, number \n\r",ch);
    	send_to_char("of attacks and parrying ability with that particular weapon.  Unarmed \n\r",ch);
    	send_to_char("combat skill will also improve your ability to dodge incoming attacks.\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("[MAGICAL ABILITY] The greater your ability in a field of magic, the \n\r",ch);
	send_to_char("greater the power of any spells you cast from that field.  Mages are able \n\r",ch);
	send_to_char("to get up to 240 exp in each type of magic.  Others are limited to 200 exp.\n\r",ch);
	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char( "See also 'EXP HELP' and 'EXP LEVELS'.\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	return;
    }
    if (arg[0] != '\0' && (!str_cmp(arg,"level") || !str_cmp(arg,"levels")))
    {
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char("                                -= Experience =-\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	send_to_char("[EXP]        [STANCE LEVEL]         [WEAPON LEVEL]         [MAGIC LEVEL]\n\r",ch);
	send_to_char("  0          Completely unskilled.  Totally unskilled.     Untrained.\n\r",ch);
	send_to_char("  1 - 25     Apprentice.            Slightly skilled.      Apprentice.\n\r",ch);
	send_to_char(" 26 - 50     Trainee.               Reasonable.            Student.\n\r",ch);
	send_to_char(" 51 - 75     Student.               Fairly competent.      Scholar.\n\r",ch);
	send_to_char(" 76 - 100    Fairly experienced.    Highly skilled.        Magus.\n\r",ch);
	send_to_char("101 - 125    Well trained.          Very dangerous.        Adept.\n\r",ch);
	send_to_char("126 - 150    Highly experienced.    Extremely deadly.      Mage.\n\r",ch);
	send_to_char("151 - 175    Expert.                Expert.                Warlock.\n\r",ch);
	send_to_char("176 - 199    Master.                Master.                Master wizard.\n\r",ch);
	send_to_char("      200    Grand master.          Grand master.          Grand sorcerer.\n\r",ch);
    	if (ch->level >= LEVEL_APPRENTICE)
	    send_to_char("      240                                                  Complete master.\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    	send_to_char( "See also 'EXP HELP' and 'EXP LEVELS'.\n\r",ch);
    	send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
	return;
    }
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char("                                -= Experience =-\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    sprintf( buf,
	"Viper   : %3d.   Crane  : %3d.   Crab  : %3d.   Mongoose : %3d.   Bull    : %3d.\n\r",
	ch->stance[STANCE_VIPER], ch->stance[STANCE_CRANE], 
	ch->stance[STANCE_CRAB], ch->stance[STANCE_MONGOOSE], 
	ch->stance[STANCE_BULL] );
    send_to_char( buf, ch );
    sprintf( buf,
	"Mantis  : %3d.   Dragon : %3d.   Tiger : %3d.   Monkey   : %3d.   Swallow : %3d.\n\r",
	ch->stance[STANCE_MANTIS], ch->stance[STANCE_DRAGON], 
	ch->stance[STANCE_TIGER], ch->stance[STANCE_MONKEY], 
	ch->stance[STANCE_SWALLOW] );
    send_to_char( buf, ch );
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    sprintf( buf,
	"Unarmed : %3d.   Slice  : %3d.   Stab  : %3d.   Slash    : %3d.   Whip    : %3d.\n\r",
	ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4] );
    send_to_char( buf, ch );
    sprintf( buf,
	"Claw    : %3d.   Blast  : %3d.   Pound : %3d.   Crush    : %3d.   Grep    : %3d.\n\r",
	ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9] );
    send_to_char( buf, ch );
    sprintf( buf,
	"Bite    : %3d.   Pierce : %3d.   Suck  : %3d.\n\r",
	ch->wpn[10], ch->wpn[11], ch->wpn[12] );
    send_to_char( buf, ch );
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    sprintf( buf,
	"Purple  : %3d.   Red    : %3d.   Blue  : %3d.   Green    : %3d.   Yellow  : %3d.\n\r",
	ch->spl[PURPLE_MAGIC], ch->spl[RED_MAGIC], ch->spl[BLUE_MAGIC], 
	ch->spl[GREEN_MAGIC],  ch->spl[YELLOW_MAGIC] );
    send_to_char( buf, ch );
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char( "See also 'EXP HELP' and 'EXP LEVELS'.\n\r",ch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    return;
}

void do_sing( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char arg [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( ch->pcdata->memorised != NULL )
    {
	ch->pcdata->memorised = NULL;
	ch->pcdata->song = 0;
	send_to_char("You stop singing.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("What song do you wish to sing?\n\r",ch);
	return;
    }

    if ( ( obj = get_obj_here(ch, arg) ) == NULL )
    {
	send_to_char("You are not carrying that object.\n\r",ch);
	return;
    }

    if ( obj->carried_by == NULL || obj->carried_by != ch )
    {
	send_to_char("You are not carrying that object.\n\r",ch);
	return;
    }

    if ( obj->item_type != ITEM_PAGE )
    {
	send_to_char("You can only sing from a page.\n\r",ch);
	return;
    }

    if ( !IS_SET(obj->quest, QUEST_SONG) )
    {
	send_to_char("There is no song written on that page.\n\r",ch);
	return;
    }

    if (strlen(obj->chpoweruse) < 2 || strlen(obj->victpoweruse) < 2)
    {
	send_to_char("There is no song written on that page.\n\r",ch);
	return;
    }

    ch->pcdata->memorised = obj;
    ch->pcdata->song = 1;

    sprintf(buf,"The song I am about to sing is entitled '%s'.",obj->victpoweruse);
    do_say(ch,buf);
    return;
}

void sing_song( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf [MAX_STRING_LENGTH];
    char *song;
    char sline [5];
    int cline = 1;
    int length;

    if ( IS_NPC(ch) ) return;

    buf[0] = '\0';
    sline[0] = '\0';

    song = str_dup(obj->chpoweruse);

    for ( length = 0; length < strlen(song); length++ )
    {
	sline[0] = song[length];
	sline[1] = '\0';
	if (!str_cmp(sline,"\n")) {cline++;continue;}
	if (!str_cmp(sline,"\r")) continue;
	if (cline == ch->pcdata->song) strcat(buf, sline);
    }
    ch->pcdata->song++;
    if (strlen(buf) < 2)
	return;

    SET_BIT(ch->more, MORE_SINGING);
    do_say(ch,buf);

    if (ch->pcdata->song > cline)
    {
	ch->pcdata->song = 0;
	ch->pcdata->memorised = NULL;
	send_to_char("You've finished your song.\n\r",ch);
    }
    return;
}

bool play_yet( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char *strtime;
    int current_date;

    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    if (IS_NPC(ch)) return TRUE;

    current_date = date_to_int(strtime);
    if (current_date >= ch->pcdata->denydate) return TRUE;
    else
    {
	sprintf(buf,"You are denied for %d more days.\n\r",ch->pcdata->denydate - current_date);
	send_to_char(buf,ch);
    }
    return FALSE;
}

int date_to_int( char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int day_count = 0;

    if (argument[0] == '\0') return 0;

    argument = one_argument( argument, buf );

    if (buf[0] == '\0') return 0;

    argument = one_argument( argument, buf );

    if (buf[0] == '\0') return 0;

    if      (!str_cmp(buf,"Jan")) day_count = 0;
    else if (!str_cmp(buf,"Feb")) day_count = 31;
    else if (!str_cmp(buf,"Mar")) day_count = 59;
    else if (!str_cmp(buf,"Apr")) day_count = 90;
    else if (!str_cmp(buf,"May")) day_count = 120;
    else if (!str_cmp(buf,"Jun")) day_count = 151;
    else if (!str_cmp(buf,"Jul")) day_count = 181;
    else if (!str_cmp(buf,"Aug")) day_count = 212;
    else if (!str_cmp(buf,"Sep")) day_count = 242;
    else if (!str_cmp(buf,"Oct")) day_count = 273;
    else if (!str_cmp(buf,"Nov")) day_count = 304;
    else if (!str_cmp(buf,"Dec")) day_count = 334;

    argument = one_argument( argument, buf );

    if (buf[0] == '\0') return 0;
    if (!is_number(buf) || atoi(buf) < 1) return 0;

    day_count += atoi(buf);

    argument = one_argument( argument, buf );
    argument = one_argument( argument, buf );
    if (buf[0] == '\0') return 0;
    if (!is_number(buf) || atoi(buf) < 1) return 0;

    day_count += (365 * atoi(buf));

    return day_count;
}

