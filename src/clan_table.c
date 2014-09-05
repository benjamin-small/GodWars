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

int clan_table_search args( (CHAR_DATA *ch) );
void format_string_spaces args ( (char *argument,int spaces) );

/* 
 * Donate Command
 */
void do_donate(CHAR_DATA *ch, char *argument)
{
 char Object_Name[MAX_INPUT_LENGTH];
 ROOM_INDEX_DATA *xRoom;
 OBJ_DATA *xObject;
 int DONATION_ROOM;

 argument = one_argument(argument, Object_Name);
 if ( Object_Name == '\0' )
 {
   send_to_char("Donate WHAT Object?",ch);
   return;  
 }
 if ( ( xObject= get_obj_carry(ch,Object_Name)) == NULL)
 {
   send_to_char("You can't donate that item! You have not got it!",ch);
   return;
 }
 switch(xObject->item_type)
 {
   case ITEM_WEAPON :
       DONATION_ROOM=DONATION_ROOM_WEAPON;
       break;   
   case ITEM_ARMOR  :
       DONATION_ROOM=DONATION_ROOM_ARMOR;
       break;   
   default :
       DONATION_ROOM=DONATION_ROOM_REST;
       break;   
 }
 if ( ( xRoom = get_room_index(DONATION_ROOM)) == NULL)
 {
   bug("Donate Room Does Not Exist : %d",DONATION_ROOM);
   return;
 }
 act("$p vanishes from your hands in a pink mist.",ch,xObject,NULL,TO_CHAR);
 act("$p vanishes from $n's hands in a pink mist.",ch,xObject,NULL,TO_ROOM);
 obj_from_char(xObject);
 obj_to_room(xObject,xRoom);
 act("$p appears in a pink mist in the room.",ch,xObject,NULL,TO_ROOM);
 return;
}

/* 
 * Outputs Current Mud Code Version
 */ 
void do_version( CHAR_DATA *ch, char *argument)
{
   char buf[MAX_INPUT_LENGTH];

   sprintf(buf, "\n\r*************************************\n\rGodWars Version Number :- %d\n\r*************************************\n\r",VERSION_NUMBER);
   send_to_char(buf,ch);
   return;
}

void clan_table_write()
{
  int clan_pos;
  int kill_count;
  FILE *fp;

  if ( ( fp = fopen(CLAN_LIST, "w") ) !=NULL) 
  {  
     for(clan_pos=0;clan_pos<=MAX_CLAN-1;clan_pos++)
     {  
         fprintf( fp, "ClanName     %s~\n",clan_table[clan_pos].clan_name);
         fprintf( fp, "ClanLeader   %s~\n",clan_table[clan_pos].clan_leader);
         fprintf( fp, "ClanPower1   %s~\n",clan_table[clan_pos].clan_powers_1);
         fprintf( fp, "ClanPower2   %s~\n",clan_table[clan_pos].clan_powers_2);
         fprintf( fp, "ClanPower3   %s~\n",clan_table[clan_pos].clan_powers_3);
         fprintf( fp, "LowerVnum    %d\n",clan_table[clan_pos].lvnum);
         fprintf( fp, "UpperVnum    %d\n",clan_table[clan_pos].uvnum);
         fprintf( fp, "ClanRecall   %d\n",clan_table[clan_pos].clan_recall);
         fprintf( fp, "ClanKills    ");
         for(kill_count=0;kill_count<=MAX_CLAN-1;kill_count++)
         {
             fprintf(fp, "%d ", clan_table[clan_pos].clan_kills[kill_count]);
         }
         fprintf( fp, "\n"); 
     }
     fflush( fp );
     fclose( fp );
  }
  else
  {
    /*bug("Cannot Open Clan file :- WRITE CLAN TABLE", 0);*/
  }
  return;
}


void clan_table_read()
{
  int clan_pos;
  int kill_count;
  FILE *fp;
  char *dummy;

  if ( ( fp = fopen(CLAN_LIST, "r") ) !=NULL) 
  {  
     for(clan_pos=0;clan_pos<=MAX_CLAN-1;clan_pos++)
     {  
         dummy=fread_word( fp );
         clan_table[clan_pos].clan_name=fread_string( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].clan_leader=fread_string( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].clan_powers_1=fread_string( fp );
         dummy=fread_word( fp );   
         clan_table[clan_pos].clan_powers_2=fread_string( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].clan_powers_3=fread_string( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].lvnum=fread_number( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].uvnum=fread_number( fp );
         dummy=fread_word( fp );
         clan_table[clan_pos].clan_recall=fread_number( fp );
         dummy=fread_word( fp );
         for(kill_count=0;kill_count<=MAX_CLAN-1;kill_count++)
         {
             clan_table[clan_pos].clan_kills[kill_count]=fread_number( fp );
         }
     }
  }
  else
  {
    /*bug("Cannot Open Clan Table:- Loading Default Table", 0);*/
    return;
  }
  fflush( fp );
  fclose( fp );
  return;
}

void do_show_clan_info(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int clan_pos;
  int clan_pos_outer;

  if (!strcmp(argument,"more"))
  { 
    sprintf( buf, "\n\r                          Inter-Clan Kills\n\r\n\r");
    send_to_char(buf,ch);
    sprintf( buf,"Clan Name");
    format_string_spaces(buf,15);
    send_to_char(buf,ch);
    for (clan_pos=0;clan_pos<MAX_CLAN-1;clan_pos++)
    {
      sprintf( buf,"%d",clan_pos);
      format_string_spaces(buf,5);
      send_to_char(buf,ch);
    }
    sprintf( buf,"\n\r\n\r");
    send_to_char(buf,ch);
    for (clan_pos_outer=0;clan_pos_outer<MAX_CLAN-1;clan_pos_outer++)
    {
      sprintf( buf,"%d) %s",clan_pos_outer,clan_table[clan_pos_outer].clan_name);
      format_string_spaces(buf,15);
      send_to_char(buf,ch);
      for (clan_pos=0;clan_pos<MAX_CLAN-1;clan_pos++)
      {
         sprintf( buf,"%d",clan_table[clan_pos_outer].clan_kills[clan_pos]);
         format_string_spaces(buf,5);
         send_to_char(buf,ch);
      }
      sprintf( buf,"\n\r");
      send_to_char(buf,ch);
    }
  }
  else
  {
    sprintf( buf, "\n\r                          Current Vampire Clans\n\r\n\r");
    send_to_char(buf,ch);
    sprintf( buf, "Clan Name       Leader          Powers\n\r\n\r");
    send_to_char(buf, ch);
    for (clan_pos=0;clan_pos<MAX_CLAN-1;clan_pos++)
    {
       sprintf( buf,"%s",clan_table[clan_pos].clan_name);
       format_string_spaces(buf,15);
       send_to_char(buf,ch);
       sprintf( buf,"%s",clan_table[clan_pos].clan_leader);
       format_string_spaces(buf,15);
       send_to_char(buf,ch);
       sprintf( buf,"%s",clan_table[clan_pos].clan_powers_1);
       format_string_spaces(buf,12);
       send_to_char(buf,ch);
       sprintf( buf,"%s",clan_table[clan_pos].clan_powers_2);
       format_string_spaces(buf,12);
       send_to_char(buf,ch);
       sprintf( buf,"%s",clan_table[clan_pos].clan_powers_3);
       format_string_spaces(buf,12);
       send_to_char(buf,ch);
       sprintf( buf,"\n\r");
       send_to_char(buf,ch);
    }
    sprintf( buf,"Type 'claninfo more' (to see inter clan kills)\n\r");
    send_to_char(buf,ch);
  }
  return;
}

/*
 * This procedure handles what happens when a vampire is killed
 */

void clan_table_dec(CHAR_DATA *ch,CHAR_DATA *victim)
{
  int ch_pos  = clan_table_search( ch );
  int vic_pos = clan_table_search( victim );
/* Code fix by KaVir.  Was:
  if ((ch_pos!=-1)||(vic_pos!=-1))
 */
  if ( ch_pos != -1 && vic_pos != -1 )
  {
      clan_table[ch_pos].clan_kills[vic_pos]+=1;
      clan_table_write();
  }
  return;
}

/* 
 * This Procedure handles what happens when a new vampire clan is created
 * It adds the leaders name to the clan table
 */

void clan_table_bite(CHAR_DATA *ch)
{
  int clan_pos;

 if (ch->vampgen == 2)
 {  
  for (clan_pos=0;clan_pos<=MAX_CLAN-1;clan_pos++)
  {
    if (!str_cmp(clan_table[clan_pos].clan_name,"None"))
    {
      clan_table[clan_pos].clan_leader=str_dup(ch->name);
      clan_table_write();
      return;
    }  
  } 
  send_to_char("Please Inform Implementor Not Enough Free Clan Slots", ch);
 }
 return;
}

/*
 * This procedure fills in the players clan name
 */
void clan_table_namefill(CHAR_DATA *ch)
{
  int clan_pos;

  for (clan_pos=0;clan_pos<=MAX_CLAN-1;clan_pos++)
  {
    if (!str_cmp(clan_table[clan_pos].clan_leader,ch->name))
    {
      clan_table[clan_pos].clan_name=str_dup(ch->clan);
      clan_table_write();
      return;
    }
  }
  return;  
}
 
/*
 * This procedure fills in a clan power name
 */
 
void clan_table_powerselect(CHAR_DATA *ch,char *power_name)
{
   int ch_pos=clan_table_search( ch );
 
   if ((ch->vampgen>2)&&(ch->vampgen!=1))
   {
      return;
   }
   if ((ch_pos != -1) || (ch_pos > MAX_CLAN-1))
   {
      if (!strcmp(clan_table[ch_pos].clan_powers_1,"none"))
      {
        clan_table[ch_pos].clan_powers_1=str_dup(power_name);
        clan_table_write();
        return;
      }
      else 
      if (!strcmp(clan_table[ch_pos].clan_powers_2,"none"))
      {
        clan_table[ch_pos].clan_powers_2=str_dup(power_name);        
        clan_table_write();
        return;
      }
      else
      if (!strcmp(clan_table[ch_pos].clan_powers_3,"none"))
      {
        clan_table[ch_pos].clan_powers_3=str_dup(power_name);
        clan_table_write();
        return;
      }
   }
   return;
}

/*
 * Procedure finds the position of the vampire clan of a player 
 * in the clan table.  It returns a -1 if it can't find it.
 */

int clan_table_search(CHAR_DATA *ch)
{
  int clan_pos;
  
  for (clan_pos=0;clan_pos<=MAX_CLAN-1;clan_pos++)
  {
    if (!strcmp(ch->clan,clan_table[clan_pos].clan_name))
    {
      return(clan_pos);
    }  
  }
  return(-1);
}

/*
 * This procedure adds spaces to strings if they are not long enough.
 */

void format_string_spaces(char *argument,int spaces)
{
  int loop_counter;
  int string_length=strlen(argument);

  if (string_length < spaces)
  {
     for (loop_counter=string_length;loop_counter<=spaces;loop_counter++)
     {
        strcat(argument," ");
     }
  }
  return;
}

void do_fileupdate(CHAR_DATA *ch, char *argument)
{
  home_write();
  if (!strcmp(ch->name,"Rotain") || !strcmp(ch->name,"KaVir"))
  {
     artifact_table_write();
     clan_table_read();
     artifact_table_read();
  }
}

struct clan_table_type clan_table[MAX_CLAN] =
{
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
  {"None","None","none","none","none",{0,0,0,0,0,0,0,0,0,0},0,0,3001},
};


