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

#define         SEX_NONE        4
#define         ROOM_NONE       0
#define         EX_NONE         0
#define         ITEM_NONE       0
#define         EXTRA_NONE      0
#define         ITEM_WEAR_NONE  0
#define         ACT_NONE        0
#define         AFFECT_NONE     0



/* You can define verbose if you want details printed out when
   resets are saved. */
/*
#define VERBOSE 
*/

#define SKILLSN(sn) (sn != -1 ? skill_table[sn].slot : 0)
#define IS_BUILDER(ch, Area)  ( GET_PC(ch,security,8) <= Area->security              \
                             || strstr( Area->builders, ch->name ) != NULL  \
                             || strstr( Area->builders, "All" ) != NULL     \
                             || (IS_NPC(ch) && ch->pIndexData->area == Area) )


/*
 * Globals
 */
extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          int                     top_room;

AREA_DATA		*	area_free;
EXTRA_DESCR_DATA	*	extra_descr_free;
EXIT_DATA		*	exit_free;
ROOM_INDEX_DATA		*	room_index_free;
OBJ_INDEX_DATA		*	obj_index_free;
SHOP_DATA		*	shop_free;
MOB_INDEX_DATA		*	mob_index_free;
RESET_DATA		*	reset_free;



/*
 * Memory Recycling
 */
RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;

    if ( reset_free == NULL )
    {
        pReset          =   alloc_perm( sizeof(*pReset) );
        top_reset++;
    }
    else
    {
        pReset          =   reset_free;
        reset_free      =   reset_free->next;
    }

    pReset->next        =   NULL;
    pReset->command     =   'X';
    pReset->arg1        =   0;
    pReset->arg2        =   0;
    pReset->arg3        =   0;

    return pReset;
}



void free_reset_data( RESET_DATA *pReset )
{
    pReset->next            = reset_free;
    reset_free              = pReset;
    return;
}



AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;
    char buf[MAX_INPUT_LENGTH];

    if ( area_free == NULL )
    {
        pArea   =   alloc_perm( sizeof(*pArea) );
        top_area++;
    }
    else
    {
        pArea       =   area_free;
        area_free   =   area_free->next;
    }

    pArea->next             =   NULL;
    pArea->reset_first      =   NULL;
    pArea->reset_last       =   NULL;
    pArea->name             =   str_dup( "New area" );
    pArea->area_flags       =   AREA_ADDED;
    pArea->security         =   1;
    pArea->builders         =   str_dup( "None" );
    pArea->lvnum            =   0;
    pArea->uvnum            =   0;
    pArea->age              =   0;
    pArea->nplayer          =   0;
    sprintf( buf, "area%d.are", pArea->vnum );
    pArea->filename         =   str_dup( buf );
    pArea->vnum             =   top_area-1;

    return pArea;
}



void free_area( AREA_DATA *pArea )
{
    RESET_DATA *pReset;

    free_string( pArea->name );
    free_string( pArea->filename );
    free_string( pArea->builders );

    for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
    {
        free_reset_data( pReset );
    }

    pArea->next         =   area_free->next;
    area_free           =   pArea;
    return;
}



EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

    if ( exit_free == NULL )
    {
        pExit           =   alloc_perm( sizeof(*pExit) );
        top_exit++;
    }
    else
    {
        pExit           =   exit_free;
        exit_free       =   exit_free->next;
    }

    pExit->to_room      =   NULL;
    pExit->next         =   NULL;
    pExit->vnum         =   0;
    pExit->exit_info    =   0;
    pExit->key          =   0;
/* kavirp
    pExit->keyword      =   NULL;
*/
    pExit->keyword      =   str_dup("");
    pExit->description  =   str_dup("");
    pExit->rs_flags     =   0;

    return pExit;
}



void free_exit( EXIT_DATA *pExit )
{
    free_string( pExit->keyword );
    free_string( pExit->description );

    pExit->next         =   exit_free;
    exit_free           =   pExit;
    return;
}



EXTRA_DESCR_DATA *new_extra_descr( void )
{
    EXTRA_DESCR_DATA *pExtra;

    if ( extra_descr_free == NULL )
    {
        pExtra              =   alloc_perm( sizeof(*pExtra) );
        top_ed++;
    }
    else
    {
        pExtra              =   extra_descr_free;
        extra_descr_free    =   extra_descr_free->next;
    }

    pExtra->keyword         =   NULL;
    pExtra->description     =   NULL;
    pExtra->next            =   NULL;

    return pExtra;
}



void free_extra_descr( EXTRA_DESCR_DATA *pExtra )
{
    free_string( pExtra->keyword );
    free_string( pExtra->description );

    pExtra->next        =   extra_descr_free;
    extra_descr_free    =   pExtra;
    return;
}



ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door;

    if ( room_index_free == NULL )
    {
        pRoom           =   alloc_perm( sizeof(*pRoom) );
        top_room++;
    }
    else
    {
        pRoom           =   room_index_free;
        room_index_free =   room_index_free->next;
    }

    pRoom->next             =   NULL;
    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;
    pRoom->extra_descr      =   NULL;
    pRoom->area             =   NULL;

    for ( door=0; door < MAX_DIR; door++ )
        pRoom->exit[door]   =   NULL;

    pRoom->name             =   &str_empty[0];
    pRoom->description      =   &str_empty[0];
    pRoom->vnum             =   0;
    pRoom->room_flags       =   0;
    pRoom->light            =   0;
    pRoom->sector_type      =   0;

    return pRoom;
}



void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int door;
    EXTRA_DESCR_DATA *pExtra;

    free_string( pRoom->name );
    free_string( pRoom->description );

    for ( door = 0; door < MAX_DIR; door++ )
    {
        if ( pRoom->exit[door] != NULL )
            free_exit( pRoom->exit[door] );
    }

    for ( pExtra = pRoom->extra_descr; pExtra != NULL; pExtra = pExtra->next )
    {
        free_extra_descr( pExtra );
    }

    pRoom->next     =   room_index_free;
    room_index_free =   pRoom;
    return;
}



AFFECT_DATA *new_affect( void )
{
    AFFECT_DATA *pAf;

    if ( affect_free == NULL )
    {
        pAf             =   alloc_perm( sizeof(*pAf) );
        top_affect++;
    }
    else
    {
        pAf             =   affect_free;
        affect_free     =   affect_free->next;
    }

    pAf->next       =   NULL;
    pAf->location   =   0;
    pAf->modifier   =   0;
    pAf->type       =   0;
    pAf->duration   =   0;
    pAf->bitvector  =   0;

    return pAf;
}



void free_affect( AFFECT_DATA* pAf )
{
    pAf->next           = affect_free;
    affect_free         = pAf;
    return;
}



SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy;

    if ( shop_free == NULL )
    {
        pShop           =   alloc_perm( sizeof(*pShop) );
        top_shop++;
    }
    else
    {
        pShop           =   shop_free;
        shop_free       =   shop_free->next;
    }

    pShop->next         =   NULL;
    pShop->keeper       =   0;

    for ( buy=0; buy<MAX_TRADE; buy++ )
        pShop->buy_type[buy]    =   ITEM_NONE;

    pShop->profit_buy   =   100;
    pShop->profit_sell  =   100;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;

    return pShop;
}



void free_shop( SHOP_DATA *pShop )
{
    pShop->next = shop_free;
    shop_free   = pShop;
    return;
}



OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( obj_index_free == NULL )
    {
        pObj           =   alloc_perm( sizeof(*pObj) );
        top_obj_index++;
    }
    else
    {
        pObj            =   obj_index_free;
        obj_index_free  =   obj_index_free->next;
    }

    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->name          =   str_dup( "no name" );
    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    pObj->extra_flags   =   0;
    pObj->wear_flags    =   0;
    pObj->count         =   0;
    pObj->weight        =   0;
    pObj->cost          =   0;
    for ( value=0; value<4; value++ )
        pObj->value[value]  =   0;

    return pObj;
}



void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra;
    AFFECT_DATA *pAf;

    free_string( pObj->name );
    free_string( pObj->short_descr );
    free_string( pObj->description );

    for ( pAf = pObj->affected; pAf != NULL; pAf = pAf->next )
    {
        free_affect( pAf );
    }

    for ( pExtra = pObj->extra_descr; pExtra != NULL; pExtra = pExtra->next )
    {
        free_extra_descr( pExtra );
    }
    
    pObj->next              = obj_index_free;
    obj_index_free          = pObj;
    return;
}



MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;

    if ( mob_index_free == NULL )
    {
        pMob           =   alloc_perm( sizeof(*pMob) );
        top_mob_index++;
    }
    else
    {
        pMob            =   mob_index_free;
        mob_index_free  =   mob_index_free->next;
    }

    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->spec_name     =   NULL;
    pMob->pShop         =   NULL;
    pMob->area          =   NULL;
    pMob->player_name   =   str_dup( "no name" );
    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)" );
    pMob->description   =   str_dup( "" );
    pMob->vnum          =   0;
    pMob->count         =   0;
    pMob->killed        =   0;
    pMob->sex           =   0;
    pMob->level         =   0;
    pMob->act           =   1;	/* NPC */
    pMob->affected_by   =   0;
    pMob->alignment     =   0;
    pMob->hitroll	=   0;
    pMob->ac	        =   0;
    pMob->hitnodice	=   0;
    pMob->hitsizedice	=   0;
    pMob->hitplus	=   0;
    pMob->damnodice	=   0;
    pMob->damsizedice	=   0;
    pMob->damplus	=   0;
    pMob->gold          =   0;

    return pMob;
}



void free_mob_index( MOB_INDEX_DATA *pMob )
{
    free_string( pMob->player_name );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );

    free_shop( pMob->pShop );

    pMob->next              = mob_index_free;
    mob_index_free          = pMob;
    return;
}



/*
 * End Memory Recycling
 */



AREA_DATA *get_area_data( int vnum )
{
    AREA_DATA *pArea;

    for (pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        if (pArea->vnum == vnum)
            return pArea;
    }

    return NULL;
}



AREA_DATA *get_vnum_area( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        if ( vnum >= pArea->lvnum
          && vnum <= pArea->uvnum )
            return pArea;
    }

    return NULL;
}



char *area_bit_name( int area_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( area_flags & AREA_CHANGED      )   strcat( buf, " CHANGED" );
        else                                strcat( buf, " changed" );
    if ( area_flags & AREA_ADDED        )   strcat( buf, " ADDED" );
        else                                strcat( buf, " added" );
    return buf+1;
}



int get_area_flags_number( char *argument )
{
    if ( !str_cmp( argument, "changed" ) )      return AREA_CHANGED;
    if ( !str_cmp( argument, "added" ) )        return AREA_ADDED;
    return AREA_NONE;
}



void aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char arg[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int  value;

    pArea = (AREA_DATA *)ch->desc->pEdit;
    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );


    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "AEdit:  Insufficient security to modify area.\n\r", ch );
    }


    if ( !str_cmp( arg1, "show" ) || arg_count( arg ) == 0 )
    {
        sprintf( buf, "%d", pArea->vnum );
        do_ashow( ch, buf );
        return;
    }


    if ( !str_cmp( arg1, "done" ) )
    {
        ch->desc->pEdit = NULL;
        ch->desc->connected = CON_PLAYING;
        return;
    }


    if ( !str_cmp( arg1, "?" ) )
    {
        do_help( ch, "aedit" );
        return;
    }


    if ( !IS_BUILDER( ch, pArea ) )
    {
        interpret( ch, arg );
        return;
    }


    if ( ( value = get_area_flags_number( arg1 ) ) != AREA_NONE )
    {
        TOGGLE_BIT(pArea->area_flags, value);

        send_to_char( "Flag toggled.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "create" ) )
    {
        pArea               =   new_area();
        area_last->next     =   pArea;
        ch->desc->pEdit     =   (void *)pArea;

        SET_BIT( pArea->area_flags, AREA_ADDED );
        send_to_char( "Area Created.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
               send_to_char( "Syntax:   name [$name]\n\r", ch );
               return;
        }

        free_string( pArea->name );
        pArea->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "filename" ) || !str_cmp( arg1, "file" ) )
    {
        if ( argument[0] == '\0' )
        {
            send_to_char( "Syntax:  filename [$file]\n\r", ch );
            send_to_char( "or       file     [$file]\n\r", ch );
            return;
        }

        free_string( pArea->filename );
        pArea->filename = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Filename set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "age" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  age [#age]\n\r", ch );
            return;
        }

        pArea->age = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Age set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "security" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  security [#level]\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( IS_NPC(ch) || value < GET_PC(ch,security,0) || value > 9 )
        {
            if ( GET_PC(ch,security,9) != 9 )
            {
                sprintf( buf, "Valid security range is %d-9.\n\r", PC(ch,security) );
                send_to_char( buf, ch );
            }
            else
            {
                send_to_char( "Valid security is 9 only.\n\r", ch );
            }
            return;
        }

        pArea->security = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Security set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "builder" ) )
    {
        argument = one_argument( argument, arg2 );

        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  builder [$name]\n\r", ch );
            return;
        }

        arg2[0] = UPPER( arg2[0] );

        if ( strstr( pArea->builders, arg2 ) != '\0' )
        {
            pArea->builders = string_replace( pArea->builders, arg2, "\0" );
            pArea->builders = string_unpad( pArea->builders );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Builder removed.\n\r", ch );
            return;
        }
        else
        {
            buf[0] = '\0';
            if ( strstr( pArea->builders, "None" ) != '\0' )
            {
                pArea->builders = string_replace( pArea->builders, "None", "\0" );
                pArea->builders = string_unpad( pArea->builders );
            }
            if (pArea->builders[0] != '\0' )
            {
                strcat( buf, pArea->builders );
                strcat( buf, " " );
            }
            strcat( buf, arg2 );
            free_string( pArea->builders );
            pArea->builders = string_proper( str_dup( buf ) );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Builder added.\n\r", ch );
            return;
        }
    }


    if ( !str_cmp( arg1, "lvnum" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  lvnum [#lower]\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( get_vnum_area( value ) != NULL
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "AEdit:  Vnum range already assigned.\n\r", ch );
            return;
        }

        pArea->lvnum = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Lower vnum set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "uvnum" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  uvnum [#upper]\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( get_vnum_area( value ) != NULL
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "AEdit:  Vnum range already assigned.\n\r", ch );
            return;
        }

        pArea->uvnum = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Upper vnum set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "vnum" ) )
    {
       argument = one_argument( argument, arg1 );
       strcpy( arg2, argument );

       if ( !is_number( arg1 ) || arg1[0] == '\0'
         || !is_number( arg2 ) || arg2[0] == '\0' )
       {
            send_to_char( "Syntax:  vnum [#lower] [#upper]\n\r", ch );
            return;
        }

        value = atoi( arg1 );

        if ( get_vnum_area( value ) != NULL
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
            return;
        }

        pArea->lvnum = value;
        send_to_char( "Lower vnum set.\n\r", ch );

        value = atoi( arg2 );

        if ( get_vnum_area( value ) != NULL
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "AEdit:   Upper vnum already assigned.\n\r", ch );
            return;
        }

        pArea->uvnum = value;
        send_to_char( "Upper vnum set.\n\r", ch );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }

    interpret( ch, arg );
    return;
}



int get_direction_number( char *arg )
{
    if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) return DIR_NORTH;
    if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) return DIR_EAST;
    if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) return DIR_SOUTH;
    if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) return DIR_WEST;
    if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) return DIR_UP;
    if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) return DIR_DOWN;
    return MAX_DIR;
}



int get_exit_flag_number( char *arg )
{
    if ( !str_cmp( arg, "door" ) )              return EX_ISDOOR;
    if ( !str_cmp( arg, "closed" ) )            return EX_CLOSED;
    if ( !str_cmp( arg, "locked" ) )            return EX_LOCKED;
    if ( !str_cmp( arg, "bashed" ) )            return EX_BASHED;
    if ( !str_cmp( arg, "bashproof" ) )         return EX_BASHPROOF;
    if ( !str_cmp( arg, "pickproof" ) )         return EX_PICKPROOF;
    if ( !str_cmp( arg, "passproof" ) )         return EX_PASSPROOF;
    return EX_NONE;
}



char *get_exit_flag_string( int exit )
{
    static char buf[512];

    buf[0] = '\0';
    if ( exit & EX_CLOSED    ) strcat( buf, " closed"    );
    if ( exit & EX_LOCKED    ) strcat( buf, " locked"    );
    if ( exit & EX_BASHED    ) strcat( buf, " bashed"    );
    if ( exit & EX_BASHPROOF ) strcat( buf, " bashproof" );
    if ( exit & EX_PICKPROOF ) strcat( buf, " pickproof" );
    if ( exit & EX_PASSPROOF ) strcat( buf, " passproof" );
    return ( buf[0] != '\0' ) ? buf+1 : "open";
}



void redit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    EXIT_DATA *pExit;
    EXTRA_DESCR_DATA *ed;
    char arg[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;
    int  iHash;
    int  door;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    pRoom = ch->in_room;
    pArea = pRoom->area;


    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "REdit:  Insufficient security to modify room.\n\r", ch );
    }


    if ( !str_cmp( arg1, "show" ) || arg1[0] == '\0' )
    {
        sprintf( buf, "%d", pRoom->vnum );
        do_rshow( ch, buf );
        return;
    }


    if ( !str_cmp( arg1, "done" ) )
    {
        ch->desc->pEdit = NULL;
        ch->desc->connected = CON_PLAYING;
        return;
    }


    if ( !str_cmp( arg1, "?" ) )
    {
        do_help( ch, "redit" );
        return;
    }


    if ( !IS_BUILDER( ch, pArea ) )
    {
        interpret( ch, arg );
        return;
    }


    if ( room_name_bit( arg1 ) != ROOM_NONE )
    {
        TOGGLE_BIT(pRoom->room_flags, room_name_bit( arg1 ));

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room flag toggled.\n\r", ch );
        return;
    }


    if ( sector_number( arg1 ) != SECT_MAX )
    {
        pRoom->sector_type  = sector_number( arg1 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sector type set.\n\r", ch );
        return;
    }


    if ( ( door = get_direction_number( arg1 ) ) != MAX_DIR && arg2[0] != '\0' )
    {
        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "delete" ) )
        {
            if ( pRoom->exit[door] == NULL )
            {
                send_to_char( "REdit:  Cannot delete a non-existant exit.\n\r", ch );
                return;
            }

            free_exit( pRoom->exit[door] );
            pRoom->exit[door] = NULL;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit unlinked.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg1, "link" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] link [vnum]\n\r", ch );
                return;
            }

            value = atoi( arg2 );

            if ( get_room_index( value ) == NULL )
            {
                send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
                return;
            }

            if ( !IS_BUILDER( ch, get_room_index( value )->area ) )
            {
                send_to_char( "REdit:  Remote side not created, not a builder there.\n\r", ch );
                return;
            }

            if ( get_room_index( value )->exit[rev_dir[door]] != NULL )
            {
                send_to_char( "REdit:  Remote side's exit already exists.\n\r", ch );
                return;
            }

            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            pRoom->exit[door]->to_room = get_room_index( value );
            pRoom->exit[door]->vnum = value;

            pRoom                   = get_room_index( value );
            door                    = rev_dir[door];
            pExit                   = new_exit();
            pExit->to_room          = ch->in_room;
            pExit->vnum             = ch->in_room->vnum;
            pRoom->exit[door]       = pExit;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit linked.\n\r", ch );
            return;
        }
        
        if ( !str_cmp( arg1, "dig" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
                return;
            }
            sprintf( buf, "create %s", arg2 );
            redit( ch, buf );
            sprintf( buf, "%s link %s", dir_name[door], arg2 );
            redit( ch, buf );
            return;
        }

        if ( !str_cmp( arg1, "room" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
                return;
            }

            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            value = atoi( arg2 );

            if ( get_room_index( value ) == NULL )
            {
                send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
                return;
            }

            pRoom->exit[door]->to_room = get_room_index( value );
            pRoom->exit[door]->vnum = value;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "key" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
                return;
            }

            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            value = atoi( arg2 );

            if ( get_obj_index( value ) == NULL )
            {
                send_to_char( "REdit:  Cannot use a non-existant object as a key.\n\r", ch );
                return;
            }

            pRoom->exit[door]->key = value;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit key set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "name" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
                return;
            }

            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            free_string( pRoom->exit[door]->keyword );
            pRoom->exit[door]->keyword = str_dup( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit name set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "desc" ) || !str_cmp( arg1, "description" ) )
        {
            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            if ( arg2[0] == '\0' )
            {
                string_append( ch, &pRoom->exit[door]->description );
                SET_BIT( pArea->area_flags, AREA_CHANGED );
                return;
            }

            if ( arg2[0] == '+' )
            {
                string_append( ch, &pRoom->exit[door]->description );
                SET_BIT( pArea->area_flags, AREA_CHANGED );
                return;
            }

            send_to_char( "Syntax:  [direction] desc    - line edit\n\r", ch );
            send_to_char( "         [direction] desc +  - line append\n\r",ch );
            return;
        }


        if ( get_exit_flag_number( arg1 ) != EX_NONE )
        {
            if ( pRoom->exit[door] == NULL )
            {
                pRoom->exit[door] = new_exit();
            }

            TOGGLE_BIT(pRoom->exit[door]->rs_flags,  get_exit_flag_number( arg1 ));
            TOGGLE_BIT(pRoom->exit[door]->exit_info, get_exit_flag_number( arg1 ));

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit flag toggled.\n\r", ch );
            return;
        }
    }


    if ( !str_cmp( arg1, "ed" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
            send_to_char( "         ed delete [keyword]\n\r", ch );
            send_to_char( "         ed edit [keyword]\n\r", ch );
            send_to_char( "         ed format [keyword]\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "add" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
                return;
            }

            ed                  =   new_extra_descr();
            ed->keyword         =   str_dup( arg2 );
            ed->description     =   str_dup( "" );
            ed->next            =   pRoom->extra_descr;
            pRoom->extra_descr  =   ed;

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "edit" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
                return;
            }

            for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( ed == NULL )
            {
                send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "append" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
                return;
            }

            for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( ed == NULL )
            {
                send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            EXTRA_DESCR_DATA *ped;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
                return;
            }

            ped = NULL;

            for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( ed == NULL )
            {
                send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            if ( ped == NULL )
            {
                pRoom->extra_descr = ed->next;
            }
            else
            {
                ped->next = ed->next;
            }

            free_extra_descr( ed );

            send_to_char( "Extra description deleted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "format" ) )
        {
            EXTRA_DESCR_DATA *ped;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
                return;
            }

            ped = NULL;

            for ( ed = pRoom->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( ed == NULL )
            {
                send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            ed->description = format_string( ed->description );

            send_to_char( "Extra description formatted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }
    }


    if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( arg2 );

        if ( arg2[0] == '\0'  || value == 0 )
        {
            send_to_char( "Syntax:  create [vnum]\n\r", ch );
            return;
        }

        pArea = get_vnum_area( value );

        if ( pArea == NULL )
        {
            send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }

        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
            return;
        }

        if ( get_room_index( value ) != NULL )
        {
            send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
            return;
        }

        pRoom                   = new_room_index();
        pRoom->area             = pArea;
        pRoom->vnum             = value;

        if ( value > top_vnum_room ) top_vnum_room = value;

        iHash                   = value % MAX_KEY_HASH;
        pRoom->next             = room_index_hash[iHash];
        room_index_hash[iHash]  = pRoom;
        ch->desc->pEdit         = (void *)pRoom;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room created.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name [name]\n\r", ch );
            return;
        }

        free_string( pRoom->name );
        pRoom->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "desc" ) || !str_cmp( arg1, "description" ) )
    {
        if ( arg2[0] == '\0' )
        {
            string_append( ch, &pRoom->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        if ( arg2[0] == '+' )
        {
            string_append( ch, &pRoom->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        send_to_char( "Syntax:  desc    - line edit\n\r", ch );
        send_to_char( "         desc +  - line append\n\r",ch );
        return;
    }


    if ( !str_cmp( arg1, "format" ) )
    {
        pRoom->description = format_string( pRoom->description );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "String formatted.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "light" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  light [number]\n\r", ch );
            return;
        }

        value = atoi( arg2 );
        pRoom->light = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Light set.\n\r", ch);
        return;
    }


    interpret( ch, arg );
    return;
}



void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    switch( obj->item_type )
    {
        default:
	    sprintf( buf, "This object has no values.\n\r" );
	    send_to_char( buf, ch );
            break;
            
        case ITEM_LIGHT:
            if ( obj->value[2] == -1 )
                sprintf( buf, "Hours of light(v2): Infinite(-1)\n\r" );
            else
                sprintf( buf, "Hours of light(v2): %d\n\r", obj->value[2] );
            send_to_char( buf, ch );
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf( buf, "(v3) %s\n\rLevel (v0): %d\n\rCharges (v2)/(v1): %d / %d\n\r",
                obj->value[3] != -1 ?
                      capitalize(skill_table[obj->value[3]].name)
                    : "!NO SPELL!",
                obj->value[0], obj->value[2], obj->value[1] );
            send_to_char( buf, ch );
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf( buf, "(v1) %s\n\r(v2) %s\n\r(v3) %s\n\rLevel (v0): %d\n\r",
                obj->value[1] != -1 ?
                      capitalize(skill_table[obj->value[1]].name)
                    : "!NO SPELL!",
                obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                    : "!NO SPELL!",
                obj->value[3] != -1 ? skill_table[obj->value[3]].name
                                    : "!NO SPELL!",
                obj->value[0] );
            send_to_char( buf, ch );
            break;

        case ITEM_WEAPON:
            sprintf( buf, "Type(v3): %s\n\rMin dam(v1): %d\n\rMax dam(v2): %d\n\r",
                get_type_weapon(obj->value[3]),
                obj->value[1], obj->value[2] );
            send_to_char( buf, ch );
            break;

        case ITEM_CONTAINER:
            sprintf( buf, "Weight (v0): %d \n\rFlags (v1): %s\nrKey (v2): %s (%d)\n\r",
                obj->value[0], get_flags_container(obj->value[1]),
                get_obj_index(obj->value[2])
                    ? get_obj_index(obj->value[2])->short_descr
                    : "none",
                obj->value[2]);
            send_to_char( buf, ch );
            break;

	case ITEM_DRINK_CON:
	    sprintf( buf, "Contains (v1)/(v0): %d/%d\n\rLiquid (v2): %s\n\rPoisoned? (v3): %s\n\r",
	        obj->value[1], obj->value[0], get_type_liquid(obj->value[2]),
	        obj->value[3] != 0 ? "Yes" : "No" );
            send_to_char( buf, ch );
            break;

	case ITEM_FOOD:
	    sprintf( buf, "Hours of food (v0): %d\n\rPoisoned? (v3): %s\n\r",
	        obj->value[0], obj->value[3] != 0 ? "Yes" : "No" );
            send_to_char( buf, ch );
            break;

	case ITEM_MONEY:
            sprintf( buf, "Amount of Gold (v0): %d\n\r", obj->value[0] );
            send_to_char( buf, ch );
            break;
    }

    return;
}



void set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{
    switch( pObj->item_type )
    {
        default:
            break;
            
        case ITEM_LIGHT:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_light" );
	            break;
	        case 2:
	            send_to_char( "HOURS OF LIGHT SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_staff_wand" );
	            break;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE SET.\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_scroll_potion_pill" );
	            break;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "SPELL TYPE 1 SET.\n\r\n\r", ch );
	            pObj->value[1] = skill_lookup( argument );
	            break;
	        case 2:
	            send_to_char( "SPELL TYPE 2 SET.\n\r\n\r", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE 3 SET.\n\r\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_WEAPON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_weapon" );
	            break;
	        case 1:
	            send_to_char( "MINIMUM DAMAGE SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "MAXIMUM DAMAGE SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
	            pObj->value[3] = get_weapon_type( argument );
	            break;
	    }
            break;

        case ITEM_CONTAINER:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_container" );
	            break;
	        case 0:
	            send_to_char( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "CONTAINER TYPE SET.\n\r\n\r", ch );
	            pObj->value[1] = get_container_flags( argument );
	            break;
	        case 2:
                    if ( atoi(argument) != 0 )
                    {
		    if ( !get_obj_index( atoi( argument ) ) )
		    {
		        send_to_char( "THERE IS NO SUCH KEY.\n\r\n\r", ch );
		        break;
		    }
		    if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
		    {
		        send_to_char( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
		        break;
		    }
		    }
	            send_to_char( "CONTAINER KEY SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

	case ITEM_DRINK_CON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_drink_con" );
		    do_help( ch, "liquids" );
	            break;
	        case 0:
	            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
	            pObj->value[2] = get_liquid_type( argument );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_FOOD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_food" );
	            break;
	        case 0:
	            send_to_char( "HOURS OF FOOD SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_MONEY:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "b_ref item_money" );
	            break;
	        case 0:
	            send_to_char( "GOLD AMOUNT SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	    }
            break;
    }

    show_obj_values( ch, pObj );

    return;
}



void oedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *pAf;
    char arg[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;
    int  iHash;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
    pArea = pObj->area;


    if ( !str_cmp( arg1, "show" ) || arg1[0] == '\0' )
    {
        sprintf( buf, "%d", pObj->vnum );
        do_oshow( ch, buf );
        return;
    }


    if ( !str_cmp( arg1, "done" ) )
    {
        ch->desc->pEdit = NULL;
        ch->desc->connected = CON_PLAYING;
        return;
    }


    if ( !str_cmp( arg1, "?" ) )
    {
        do_help( ch, "oedit" );
        return;
    }


    if ( !str_cmp( arg1, "addaffect" ) )
    {
        char arg3[MAX_STRING_LENGTH];

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  addaffect [location] [#mod]\n\r", ch );
            return;
        }

        pAf             =   new_affect();
        pAf->location   =   affect_name_loc( arg1 );
        pAf->modifier   =   atoi( arg2 );
        pAf->type       =   -1;
        pAf->duration   =   -1;
        pAf->bitvector  =   0;
        pAf->next       =   pObj->affected;
        pObj->affected  =   pAf;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect added.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "delaffect" ) || !str_cmp( arg1, "remaffect" ) )
    {
        pAf             =   pObj->affected;

        if ( pAf == NULL )
        {
            send_to_char( "OEdit:  No affect to remove.\n\r", ch );
            return;
        }

        pObj->affected  =   pObj->affected->next;
        free_affect( pAf );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect removed.\n\r", ch);
        return;
    }


    if ( ( value = item_name_type( arg1 ) ) != ITEM_NONE )
    {
        pObj->item_type = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Type set.\n\r", ch);

        /*
         * Clear the values.
         */
        pObj->value[0] = 0;
        pObj->value[1] = 0;
        pObj->value[2] = 0;
        pObj->value[3] = 0;

        return;
    }


    if ( ( value = extra_name_bit( arg1 ) ) != EXTRA_NONE )
    {
        TOGGLE_BIT(pObj->extra_flags, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Extra flag toggled.\n\r", ch);
        return;
    }


    if ( ( value = wear_name_bit( arg1 ) ) != ITEM_WEAR_NONE )
    {
        TOGGLE_BIT(pObj->wear_flags, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Wear flag toggled.\n\r", ch);
        return;
    }



    if ( !str_cmp( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name [string]\n\r", ch );
            return;
        }

        free_string( pObj->name );
        pObj->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "short" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  short [string]\n\r", ch );
            return;
        }

        free_string( pObj->short_descr );
        pObj->short_descr = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Short description set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "long" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  long [string]\n\r", ch );
            return;
        }
        
        strcat( arg2, "\n\r" );

        free_string( pObj->description );
        pObj->description = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Long description set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "value0") || !str_cmp( arg1, "v0" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  value0 [number]\n\r", ch );
            send_to_char( "or       v0 [number]\n\r", ch );
	    set_obj_values( ch, pObj, -1, arg2 );
            return;
        }

	set_obj_values( ch, pObj, 0, arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }

    if ( !str_cmp( arg1, "value1") || !str_cmp( arg1, "v1" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  value1 [number]\n\r", ch );
            send_to_char( "or       v1 [number]\n\r", ch );
	    set_obj_values( ch, pObj, -1, arg2 );
            return;
        }

	set_obj_values( ch, pObj, 1, arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value2") || !str_cmp( arg1, "v2" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  value2 [number]\n\r", ch );
            send_to_char( "or       v2 [number]\n\r", ch );
	    set_obj_values( ch, pObj, -1, arg2 );
            return;
        }

	set_obj_values( ch, pObj, 2, arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value3") || !str_cmp( arg1, "v3" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  value3 [number]\n\r", ch );
            send_to_char( "or       v3 [number]\n\r", ch );
	    set_obj_values( ch, pObj, -1, arg2 );
            return;
        }

	set_obj_values( ch, pObj, 3, arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }



    if ( !str_cmp( arg1, "weight" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  weight [number]\n\r", ch );
            return;
        }

        value = atoi( arg2 );
        pObj->weight = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Weight set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "cost" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  cost [number]\n\r", ch );
            return;
        }

        value = atoi( arg2 );
        pObj->cost = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Cost set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( arg2 );
        if ( arg2[0] == '\0' || value == 0 )
        {
            send_to_char( "Syntax:  oedit create [vnum]\n\r", ch );
            return;
        }

        pArea = get_vnum_area( value );

        if ( pArea == NULL )
        {
            send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }

        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
            return;
        }

        if ( get_obj_index( value ) != NULL )
        {
            send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
            return;
        }
        
        pObj                    = new_obj_index();
        pObj->vnum              = value;
        pObj->area              = pArea;
        
        if ( value > top_vnum_obj ) top_vnum_obj = value;

        iHash                   = value % MAX_KEY_HASH;
        pObj->next              = obj_index_hash[iHash];
        obj_index_hash[iHash]   = pObj;
        ch->desc->pEdit = (void *)pObj;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Object Created.\n\r", ch );
        return;
    }



    if ( !str_cmp( arg1, "ed" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
            send_to_char( "         ed delete [keyword]\n\r", ch );
            send_to_char( "         ed edit [keyword]\n\r", ch );
            send_to_char( "         ed format [keyword]\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "add" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
                return;
            }

            ed                  =   new_extra_descr();
            ed->keyword         =   str_dup( arg2 );
            ed->next            =   pObj->extra_descr;
            pObj->extra_descr   =   ed;

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "edit" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
                return;
            }

            for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( ed == NULL )
            {
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "append" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
                return;
            }

            for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( ed == NULL )
            {
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            EXTRA_DESCR_DATA *ped;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
                return;
            }

            ped = NULL;

            for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( ed == NULL )
            {
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            if ( ped == NULL )
            {
                pObj->extra_descr = ed->next;
            }
            else
            {
                ped->next = ed->next;
            }

            free_extra_descr( ed );

            send_to_char( "Extra description deleted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "format" ) )
        {
            EXTRA_DESCR_DATA *ped;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
                return;
            }

            ped = NULL;

            for ( ed = pObj->extra_descr; ed != NULL; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( ed == NULL )
            {
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return;
            }

            ed->description = format_string( ed->description );

            send_to_char( "Extra description formatted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }
    }


    interpret( ch, arg );
    return;
}



int get_mob_sex_number( char* arg )
{
    if ( !str_cmp( arg, "neuter" ) )            return SEX_NEUTRAL;
    if ( !str_cmp( arg, "male" ) )              return SEX_MALE;
    if ( !str_cmp( arg, "female" ) )            return SEX_FEMALE;
    return SEX_NONE;
}



SPEC_FUN *get_mob_spec( char* arg )
{
    char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
    strcat( buf, "spec_" );
    strcat( buf, arg );

    return spec_lookup( buf );
}



void medit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char arg[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;
    int  iHash;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
    value = atoi( arg2 );

    pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
    pArea = pMob->area;


    if ( !str_cmp( arg1, "show" ) || arg1[0] == '\0' )
    {
        sprintf( buf, "%d", pMob->vnum );
        do_mshow( ch, buf );
        return;
    }


    if ( !str_cmp( arg1, "done" ) )
    {
        ch->desc->pEdit = NULL;
        ch->desc->connected = CON_PLAYING;
        return;
    }


    if ( !str_cmp( arg1, "shop" ) )
    {
        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( arg1[0] == '\0' )
        {
            send_to_char( "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
            send_to_char( "         shop profit [#buying%] [#selling%]\n\r", ch );
            send_to_char( "         shop type [#1-5] [item type]\n\r", ch );
            send_to_char( "         shop delete\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "hours" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
                return;
            }

            if ( pMob->pShop == NULL )
            {
                pMob->pShop         = new_shop();
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->open_hour = atoi( arg1 );
            pMob->pShop->close_hour = atoi( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop hours set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "profit" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  shop profit [#buying%] [#selling%]\n\r", ch );
                return;
            }

            if ( pMob->pShop == NULL )
            {
                pMob->pShop         = new_shop();
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->profit_buy     = atoi( arg1 );
            pMob->pShop->profit_sell    = atoi( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop profit set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "type" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  shop type [#] [item type]\n\r", ch );
                return;
            }

            if ( atoi( arg1 ) > MAX_TRADE )
            {
                sprintf( buf, "%d", MAX_TRADE );
                send_to_char( "REdit:  Shop keepers may only sell ", ch );
                send_to_char( buf, ch );
                send_to_char( " items max.\n\r", ch );
                return;
            }

            if ( ( value = item_name_type( arg2 ) ) == ITEM_NONE )
            {
                send_to_char( "REdit:  That type of item is not known.\n\r", ch );
                return;
            }

            if ( pMob->pShop == NULL )
            {
                pMob->pShop         = new_shop();
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->buy_type[atoi( arg1 )] = item_name_type( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop type set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            if ( pMob->pShop == NULL )
            {
                send_to_char( "REdit:  Cannot delete a shop that is non-existant.\n\r", ch );
                return;
            }

            free_shop( pMob->pShop );
            pMob->pShop = NULL;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop deleted.\n\r", ch);
            return;
        }

        send_to_char( "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
        send_to_char( "         shop profit [#buying%] [#selling%]\n\r", ch );
        send_to_char( "         shop type [#] [item type]\n\r", ch );
        send_to_char( "         shop delete\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name [string]\n\r", ch );
            return;
        }

        free_string( pMob->player_name );
        pMob->player_name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "short" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  short [string]\n\r", ch );
            return;
        }

        free_string( pMob->short_descr );
        pMob->short_descr = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Short description set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "long" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  long [string]\n\r", ch );
            return;
        }

        free_string( pMob->long_descr );
        strcat( arg2, "\n\r" );
        pMob->long_descr = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Long description set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "desc" ) || !str_cmp( arg1, "description" ) )
    {
        if ( arg2[0] == '\0' )
        {
            string_append( ch, &pMob->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        if ( arg2[0] == '+' )
        {
            string_append( ch, &pMob->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        send_to_char( "Syntax:  desc    - line edit\n\r", ch );
        send_to_char( "         desc +  - line append\n\r",ch );
        return;
    }


    if ( get_mob_sex_number( arg1 ) != SEX_NONE )
    {
        pMob->sex = get_mob_sex_number( arg1 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sex set.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "level" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  level [number]\n\r", ch );
            return;
        }

        pMob->level = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Level set.\n\r", ch);
        return;
    }


    if ( ( value = act_name_bit( arg1 ) ) != ACT_NONE )
    {
        pMob->act ^= value;
        SET_BIT( pMob->act, ACT_IS_NPC );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Act flag toggled.\n\r", ch);
        return;
    }


    if ( ( value = affect_name_bit( arg1 ) ) != AFFECT_NONE )
    {
        pMob->affected_by ^= value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect flag toggled.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "alignment" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  alignment [number]\n\r", ch );
            return;
        }

        pMob->alignment = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Alignment set.\n\r", ch);
        return;
    }


    if ( get_mob_spec( arg ) != NULL )
    {
        pMob->spec_fun = get_mob_spec( arg );

        free_string( pMob->spec_name );
        sprintf( buf, "spec_%s", arg );
        pMob->spec_name = str_dup( buf );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Spec set.\n\r", ch);
        return;
    }

    if ( !str_cmp( arg, "none" ) )
    {
        pMob->spec_fun = NULL;

        free_string( pMob->spec_name );
        pMob->spec_name = str_dup( "" );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Spec removed.\n\r", ch);
        return;
    }


    if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( argument );
        if ( argument[0] == '\0' || value == 0 )
        {
            send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
            return;
        }

        pArea = get_vnum_area( value );

        if ( pArea == NULL )
        {
            send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }

        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
            return;
        }

        if ( get_mob_index( value ) != NULL )
        {
            send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
            return;
        }

        pMob                    = new_mob_index();
        pMob->vnum              = value;
        pMob->area              = pArea;
        
        if ( value > top_vnum_mob ) top_vnum_mob = value;        

        pMob->act               = ACT_IS_NPC;
        iHash                   = value % MAX_KEY_HASH;
        pMob->next              = mob_index_hash[iHash];
        mob_index_hash[iHash]   = pMob;
        ch->desc->pEdit = (void *)pMob;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Mobile Created.\n\r", ch );
        return;
    }


    interpret( ch, arg );
    return;
}



void do_aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    int value;

    pArea = ch->in_room->area;

    if ( is_number( argument ) )
    {
        value = atoi( argument );
        if ( ( pArea = get_area_data( value ) ) == NULL )
        {
            send_to_char( "That area vnum does not exist.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !str_cmp( argument, "reset" ) )
        {
            reset_area( pArea );
            send_to_char( "Area reset.\n\r", ch );
            return;
        }
        else
        if ( !str_cmp( argument, "create" ) )
        {
            pArea               =   new_area();
            area_last->next     =   pArea;
            SET_BIT( pArea->area_flags, AREA_ADDED );
        }
    }

    ch->desc->pEdit = (void *)pArea;
    ch->desc->connected = CON_AEDITOR;
    return;
}



void do_oedit( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int value;
    int iHash;
    char arg1[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) ) return;
    argument = one_argument( argument, arg1 );

    if ( is_number( arg1 ) )
    {
        value = atoi( arg1 );
        if ( ( pObj = get_obj_index( value ) ) == NULL )
        {
            send_to_char( "OEdit:  That vnum does not exist.\n\r", ch );
            return;
        }

        ch->desc->pEdit = (void *)pObj;
        ch->desc->connected = CON_OEDITOR;
        return;
    }
    else
    {
        if ( !str_cmp( arg1, "create" ) )
        {
            value = atoi( argument );
            if ( argument[0] == '\0' || value == 0 )
            {
                send_to_char( "Syntax:  oedit create [vnum]\n\r", ch );
                return;
            }

            pArea = get_vnum_area( value );

            if ( pArea == NULL )
            {
                send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
                return;
            }

            if ( !IS_BUILDER( ch, pArea ) )
            {
                send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
                return;
            }

            if ( get_obj_index( value ) != NULL )
            {
                send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
                return;
            }

            pObj                    = new_obj_index();
            pObj->vnum              = value;
            pObj->area              = pArea;
            
            if ( value > top_vnum_obj ) top_vnum_obj = value;
        
            iHash                   = value % MAX_KEY_HASH;
            pObj->next              = obj_index_hash[iHash];
            obj_index_hash[iHash]   = pObj;
            ch->desc->pEdit = (void *)pObj;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            ch->desc->connected = CON_OEDITOR;
            return;
        }
    }

    send_to_char( "OEdit:  There is no default object to edit.\n\r", ch );
    return;
}



void do_redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    AREA_DATA *pArea;
    int value;
    int iHash;
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    pRoom = ch->in_room;

    if ( is_number( arg1 ) )
    {
        value = atoi( arg1 );
        if ( ( pRoom = get_room_index( value ) ) == NULL )
        {
            send_to_char( "REdit:  That vnum does not exist.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !str_cmp( arg1, "create" ) )
        {
            value = atoi( argument );
            if ( argument[0] == '\0' || value == 0 )
            {
                send_to_char( "Syntax:  redit create [vnum]\n\r", ch );
                return;
            }

            pArea = get_vnum_area( value );

            if ( pArea == NULL )
            {
                send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
                return;
            }

            if ( !IS_BUILDER( ch, pArea ) )
            {
                send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
                return;
            }

            if ( get_room_index( value ) != NULL )
            {
                send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
                return;
            }

            pRoom                   = new_room_index();
            pRoom->area             = pArea;
            pRoom->vnum             = value;
            
            if ( value > top_vnum_room ) top_vnum_room = value;
        
            iHash                   = value % MAX_KEY_HASH;
            pRoom->next             = room_index_hash[iHash];
            room_index_hash[iHash]  = pRoom;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            char_from_room( ch );
            char_to_room( ch, pRoom );
        }
    }

    ch->desc->connected = CON_REDITOR;
    return;
}



void do_medit( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int value;
    int iHash;
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if ( is_number( arg1 ) )
    {
        value = atoi( arg1 );
        if ( ( pMob = get_mob_index( value ) ) == NULL )
        {
            send_to_char( "MEdit:  That vnum does not exist.\n\r", ch );
            return;
        }

        ch->desc->pEdit = (void *)pMob;
        ch->desc->connected = CON_MEDITOR;
        return;
    }
    else
    {
        if ( !str_cmp( arg1, "create" ) )
        {
            value = atoi( argument );
            if ( argument[0] == '\0' || value == 0 )
            {
                send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
                return;
            }

            pArea = get_vnum_area( value );

            if ( pArea == NULL )
            {
                send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
                return;
            }

            if ( !IS_BUILDER( ch, pArea ) )
            {
                send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
                return;
            }

            if ( get_mob_index( value ) != NULL )
            {
                send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
                return;
            }

            pMob                    = new_mob_index();
            pMob->vnum              = value;
            pMob->area              = pArea;
            pMob->act               = ACT_IS_NPC;
            
            if ( value > top_vnum_mob ) top_vnum_mob = value;

            iHash                   = value % MAX_KEY_HASH;
            pMob->next              = mob_index_hash[iHash];
            mob_index_hash[iHash]   = pMob;
            ch->desc->pEdit = (void *)pMob;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            ch->desc->connected = CON_MEDITOR;
            return;
        }
    }

    send_to_char( "MEdit:  There is no default mobile to edit.\n\r", ch );
    return;
}



void do_ashow( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf  [MAX_STRING_LENGTH];
    AREA_DATA *pArea;

    smash_tilde( argument );
    strcpy( arg1, argument );

    if ( is_number( arg1 ) )
        pArea = get_area_data( atoi( arg1 ) );
    else
        pArea = ch->in_room->area;
        
    if (!pArea)
      pArea = ch->in_room->area;

    sprintf( buf, "Name: '%s'\n\r", pArea->name );
    send_to_char( buf, ch );

    sprintf( buf, "File: '%s'\n\r", pArea->filename );
    send_to_char( buf, ch );

    sprintf( buf, "Age: %d; Players: %d\n\r", pArea->age, pArea->nplayer );
    send_to_char( buf, ch );

    sprintf( buf, "Security: %d; Builders '%s'\n\r", pArea->security, pArea->builders );
    send_to_char( buf, ch );

    sprintf( buf, "Vnums: %d-%d\n\r", pArea->lvnum, pArea->uvnum );
    send_to_char( buf, ch );

    sprintf( buf, "Flags: %s\n\r", area_bit_name( pArea->area_flags ) );
    send_to_char( buf, ch );

    return;
}



void do_rshow( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA        *obj;
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    char             buf  [ MAX_STRING_LENGTH ];
    char             buf1 [ MAX_STRING_LENGTH ];
    char             arg  [ MAX_INPUT_LENGTH  ];
    int              door;

    rch = get_char( ch );

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    sprintf( buf, "Name: '%s'\n\rArea: [%3d] '%s'\n\r",
	    location->name,
	    location->area->vnum,
	    location->area->name );
    strcat( buf1, buf );

    sprintf( buf,
	    "Vnum: %d\n\rSector: %s\n\rLight: %d\n\r",
	    location->vnum,
	    sector_name( location->sector_type ),
	    location->light );
    strcat( buf1, buf );

    sprintf( buf,
	    "Room flags: %s\n\rDescription:\n\r%s",
	    room_bit_name( location->room_flags ),
	    location->description );
    strcat( buf1, buf );

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "'\n\r" );
    }

    strcat( buf1, "Characters:" );

    /* Yes, we are reusing the variable rch.  - Kahn */
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
	strcat( buf1, " " );
	one_argument( rch->name, buf );
	strcat( buf1, buf );
    }

    strcat( buf1, "\n\rObjects:   " );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	strcat( buf1, " " );
	one_argument( STR(obj, name), buf );
	strcat( buf1, buf );
    }
    strcat( buf1, "\n\r" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) )
	{
	    sprintf( buf,
		    "Door: %5s.  To: %d.  Key: %d.  Exit flags: %d '%s'.\n\r",
		    dir_name[door],
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    pexit->exit_info,
		    (pexit->keyword != NULL) ? pexit->keyword : "" );
	    strcat( buf1, buf );
	    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	    {
		sprintf( buf, "%s", pexit->description );
		strcat( buf1, buf );
	    }
	}
    }

    send_to_char( buf1, ch );
    return;
}



void do_mshow( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "MShow whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_mob_index( atoi( arg ) ) ) == NULL )
    {
    send_to_char( "Invalid mob index VNUM.\n\r", ch );
        return;
    }

    sprintf( buf, "Name: '%s'\n\rArea %d: '%s'\n\r",
    victim->player_name,
    victim->area == NULL ? -1        : victim->area->vnum,
    victim->area == NULL ? "No Area" : victim->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "Act: %s\n\r", act_bit_name( victim->act ) );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d\n\rSex:  %s\n\r",
    victim->vnum,
    victim->sex == SEX_MALE    ? "male"   :
    victim->sex == SEX_FEMALE  ? "female" : "neutral" );
    send_to_char( buf, ch );

    sprintf( buf,
    "Level: %d\n\rAlign: %d\n\r",
    victim->level,       victim->alignment );
    send_to_char( buf, ch );

    sprintf( buf, "Affected by: %s\n\r",
                  affect_bit_name( victim->affected_by ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s\n\rLong description:\n\r%s",
    victim->short_descr,
    victim->long_descr );
    send_to_char( buf, ch );

    sprintf( buf, "Description:\n\r%s", victim->description );

    if ( victim->spec_fun != NULL )
    {
        sprintf( buf, "Spec fun: %s\n\r",  victim->spec_name );
        send_to_char( buf, ch );
    }

    if ( victim->pShop != NULL )
    {
        SHOP_DATA *pShop;
        int iTrade;

        pShop = victim->pShop;

        sprintf( buf, "Shop data (for %d):\n\r    Will buy at %d%%, and sell at %d%%.\n\r",
                      pShop->keeper, pShop->profit_buy, pShop->profit_sell );
        send_to_char( buf, ch );
        sprintf( buf, "    Opens at %d, closes at %d.\n\r",
                      pShop->open_hour, pShop->close_hour );
        send_to_char( buf, ch );

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
            if ( pShop->buy_type[iTrade] != ITEM_NONE )
            {
                sprintf( buf, "    Buys %ss (#%d)\n\r",
                         item_type_name( pShop->buy_type[iTrade] ), iTrade );
                send_to_char( buf, ch );
            }
        }
    }

    return;
}




void do_oshow( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_INDEX_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "OShow what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_index( atoi( arg ) ) ) == NULL )
    {
    send_to_char( "Invalid VNUM reference.\n\r", ch );
        return;
    }

    sprintf( buf, "Name: '%s'\n\rArea %d: '%s'\n\r",
    obj->name,
    obj->area == NULL ? -1        : obj->area->vnum,
    obj->area == NULL ? "No Area" : obj->area->name );
    send_to_char( buf, ch );


    sprintf( buf, "Vnum: %d\n\rType: %s\n\r",
    obj->vnum,
    item_type_name( obj->item_type ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s\n\rLong description: %s\n\r",
        obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear flags: %s\n\rExtra flags: %s\n\r",
    wear_bit_name( obj->wear_flags ),
    extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Weight: %d\n\rCost: %d\n\r",
    obj->weight, obj->cost );
    send_to_char( buf, ch );

    show_obj_values( ch, obj );

    if ( obj->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

    send_to_char( "Extra description keywords: ", ch );

        for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        {
        send_to_char( "'", ch );
            send_to_char( ed->keyword, ch );
        send_to_char( "'", ch );
        }

    send_to_char( "\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
       sprintf( buf, "Affects %s by %d.\n\r", affect_loc_name( paf->location ),
                                              paf->modifier );
       send_to_char( buf, ch );
    }

    return;
}



void display_resets( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char final[MAX_STRING_LENGTH];
    AREA_DATA  *pArea = ch->in_room->area;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    int iReset = 0;

    final[0]  = '\0';

    for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
    {
	OBJ_INDEX_DATA  *pObj;
	MOB_INDEX_DATA  *pMobIndex;
	OBJ_INDEX_DATA  *pObjIndex;
	OBJ_INDEX_DATA  *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf( final, "%2d- ", ++iReset );

	switch ( pReset->command )
	{
	default:
	    sprintf( buf, "Bad reset command: %c.", pReset->command );
	    strcat( final, buf );
	    break;

	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

            pMob = pMobIndex;
            sprintf( buf, "Loads %s (%d) in %s (%d) (max %d)",
                       pMob->short_descr, pReset->arg1, pRoomIndex->name,
                       pReset->arg3, pReset->arg2 );
            strcat( final, buf );

	    /*
	     * Check for pet shop.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev
		    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                    sprintf( buf, " as a pet." );
                sprintf( buf, "\n\r" );
                strcat( final, buf );
	    }

	    break;

	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

            sprintf( buf, "   Loads %s (%d) in %s (%d)\n\r",
                          pObj->short_descr, pReset->arg1,
                          pRoomIndex->name, pReset->arg3 );
            strcat( final, buf );

	    break;

	case 'P':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Put Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Put Object - Bad To Object %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

            sprintf( buf, "   Loads %s (%d) inside %s (%d)\n\r",
                          pObj->short_descr,
                          pReset->arg1,
                          pObjToIndex->short_descr,
                          pReset->arg3 );
            strcat( final, buf );

	    break;

	case 'G':
	case 'E':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Give/Equip Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !pMob )
	    {
                sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
                strcat( final, buf );
                break;
	    }

            if ( pMob->pShop )
            {
            sprintf( buf, "   Loads %s (%d) in the inventory of %s (%d) shopkeeper\n\r",
                          pObj->short_descr, pReset->arg1,
                          pMob->short_descr, pMob->vnum );
            }
            else
            sprintf( buf, "   Loads %s (%d) %s of %s (%d)\n\r",
                          pObj->short_descr, pReset->arg1,
                          (pReset->command == 'G') ? wear_loc_name( WEAR_NONE ) 
                                                   : wear_loc_name( pReset->arg3 ),
                          pMob->short_descr, pMob->vnum );
            strcat( final, buf );

	    break;

        /*
         * Doors are automatically set as what they look like when the
         * database is edited.  I would like to do this will all the
         * reset information.
         */
	case 'D':
            pRoomIndex = get_room_index( pReset->arg1 );
	    sprintf( buf, "The %s door of %s is reset to %s\n", 
                dir_name[ pReset->arg2 ],
                pRoomIndex->name,
                get_exit_flag_string( pReset->arg3 ) );
	    break;

	case 'R':
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Randomize Exits - Bad Room %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            sprintf( buf, "Exits are randomized in %s (%d)\n\r",
                          pRoomIndex->name, pReset->arg1 );
            strcat( final, buf );

	    break;
	}
	send_to_char( final, ch );
    }

    return;
}



void add_reset( AREA_DATA *area, RESET_DATA *pReset, int index )
{
    RESET_DATA *reset;
    int iReset = 0;

    if ( area->reset_first == NULL )
    {
        area->reset_first = pReset;
        area->reset_last  = pReset;
        pReset->next      = NULL;
        return;
    }

    index--;

    if ( index == 0 )
    {
        pReset->next = area->reset_first;
        area->reset_first = pReset;
        return;
    }

    for ( reset = area->reset_first; reset->next != NULL; reset = reset->next )
    {
        if ( ++iReset == index ) break;
    }

    pReset->next = reset->next;
    reset->next  = pReset;
    if ( pReset->next == NULL ) area->reset_last = pReset;
    return;
}



int get_max_dir( CHAR_DATA *ch, int arg )
{
    char buf[MAX_INPUT_LENGTH];

    if ( arg < 0 || arg >= MAX_DIR )
    {
	sprintf( buf, "Doors range from 0 to %d.\n\r", MAX_DIR-1 );
	send_to_char( buf, ch );
	return -1;
    }
    else
	return arg;
}

 


void do_resets( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    RESET_DATA *pReset = NULL;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );

    if ( arg1[0] == '\0' )
    {
        if ( ch->in_room->area->reset_first != NULL )
        {
        send_to_char( "Resets:\n\r", ch );
        display_resets( ch );
        }
        else
        send_to_char( "No resets in this area.\n\r", ch );
    }

    if ( !IS_BUILDER( ch, ch->in_room->area ) )
    {
        send_to_char( "Resets: Invalid security for editing this area.\n\r",
                      ch );
        return;
    }

    if ( is_number( arg1 ) )
    {
        AREA_DATA *pArea = ch->in_room->area;

        if ( !str_cmp( arg2, "delete" ) )
        {
        int insert_loc = atoi( arg1 );

        if ( ch->in_room->area->reset_first == NULL )
        {
            send_to_char( "No resets in this area.\n\r", ch );
            return;
        }

        if ( insert_loc-1 <= 0 )
        {
            pReset = pArea->reset_first;
            pArea->reset_first = pArea->reset_first->next;
            if ( pArea->reset_first == NULL )
                 pArea->reset_last = NULL;
        }
        else
        {
            int iReset = 0;
            RESET_DATA *prev = NULL;

            for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
            {
                if ( ++iReset == insert_loc )   break;
                prev = pReset;
            }

            if ( pReset == NULL )
            {
                send_to_char( "Reset not found.\n\r", ch );
                return;
            }

            if ( prev != NULL )  prev->next = prev->next->next;
                            else pArea->reset_first = pArea->reset_first->next;

            for ( pArea->reset_last = pArea->reset_first;
                  pArea->reset_last->next != NULL;
                  pArea->reset_last = pArea->reset_last->next );
        }

        free_reset_data( pReset );
        send_to_char( "Reset deleted.\n\r", ch );
        }
        else
        if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
          || (!str_cmp( arg2, "obj" ) && is_number( arg3 )) )
        {
            pReset = new_reset_data();
            if ( !str_cmp( arg2, "mob" ) )
            {
            pReset->command = 'M';
            pReset->arg1    = atoi( arg3 );
            pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 5; /* Max # */
            pReset->arg3    = ch->in_room->vnum;
            }
            else
            if ( !str_cmp( arg2, "obj" ) )
            {
            pReset->arg1    = atoi( arg3 );
            if ( !str_cmp( arg4, "inside" ) || !str_cmp( arg4, "in" ) )
            {
                pReset->command = 'P';
                pReset->arg2    = 0;
                pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
            }
       else if ( !str_cmp( arg4, "room" ) )
       {
           pReset->command = 'O';
           pReset->arg2     = 0;
           pReset->arg3     = ch->in_room->vnum;
       }
       else
       {
           pReset->arg2     = wear_name_loc( arg4 );
           if ( pReset->arg2 == WEAR_NONE )
               pReset->command = 'G';
           else
               pReset->command = 'E';
       }
            }

            add_reset( ch->in_room->area, pReset, atoi( arg1 ) );
            send_to_char( "Reset added.\n\r", ch );
        }
        else
        {
        send_to_char( "Syntax: RESET <number> OBJ <vnum> <location>\n\r", ch );
        send_to_char( "        RESET <number> OBJ <vnum> INside <vnum>\n\r", ch );
        send_to_char( "        RESET <number> OBJ <vnum> room\n\r", ch );
        send_to_char( "        RESET <number> MOB <vnum> [<max #>]\n\r", ch );
        send_to_char( "        RESET <number> DELETE\n\r", ch );
        }
    }

    return;
}



void save_area_list()
{
    FILE *fp;
    AREA_DATA *pArea;

    if ( ( fp = fopen( "area.lst", "w" ) ) == NULL )
    {
	bug( "Save_area_list: fopen", 0 );
	perror( "area.lst" );
    }
    else
    {
	fprintf( fp, "help.are\n" );
	fprintf( fp, "h-immref.hlp\n" );
	for( pArea = area_first; pArea != NULL; pArea = pArea->next )
	{
	    fprintf( fp, "%s\n", pArea->filename );
	}

	fprintf( fp, "$\n" );
	fclose( fp );
    }

    return;
}



char *fix_string( const char *str )
{
    static char strfix[MAX_STRING_LENGTH];
    int i;
    int o;

    if ( str == NULL )
        return '\0';

    for ( o = i = 0; str[i+o] != '\0'; i++ )
    {
        if (str[i+o] == '\r' || str[i+o] == '~')
            o++;
        strfix[i] = str[i+o];
    }
    strfix[i] = '\0';
    return strfix;
}



void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;

    fprintf( fp, "#MOBILES\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex != NULL && pMobIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",		pMobIndex->vnum );
                fprintf( fp, "%s~\n",		pMobIndex->player_name );
                fprintf( fp, "%s~\n",		pMobIndex->short_descr );
                fprintf( fp, "%s~\n",		fix_string( pMobIndex->long_descr ) );
                fprintf( fp, "%s~\n",		fix_string( pMobIndex->description ) );
                fprintf( fp, "%d ",		pMobIndex->act );
                fprintf( fp, "%d ",		pMobIndex->affected_by );
                fprintf( fp, "%d S\n",		pMobIndex->alignment );
                fprintf( fp, "%d ",		pMobIndex->level );
                fprintf( fp, "%d ",		pMobIndex->hitroll );
                fprintf( fp, "%d ",		pMobIndex->ac );
                fprintf( fp, "%dd%d+%d ",	pMobIndex->hitnodice, 
                                                pMobIndex->hitsizedice, 
                                                pMobIndex->hitplus );
                fprintf( fp, "%dd%d+%d\n",	pMobIndex->damnodice, 
                                                pMobIndex->damsizedice, 
                                                pMobIndex->damplus );
                fprintf( fp, "%d ",		pMobIndex->gold );
		fprintf( fp, "0\n0 0 " );
                fprintf( fp, "%d\n",  pMobIndex->sex );
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



void save_objects( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    OBJ_INDEX_DATA *pObjIndex;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;

    fprintf( fp, "#OBJECTS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pObjIndex = obj_index_hash[iHash]; pObjIndex != NULL; pObjIndex = pObjIndex->next )
        {
            if ( pObjIndex != NULL && pObjIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",    pObjIndex->vnum );
                fprintf( fp, "%s~\n",    pObjIndex->name );
                fprintf( fp, "%s~\n",    pObjIndex->short_descr );
                fprintf( fp, "%s~\n",    fix_string( pObjIndex->description ) );
                fprintf( fp, "~\n" );
                fprintf( fp, "%d ",      pObjIndex->item_type );
                fprintf( fp, "%d ",      pObjIndex->extra_flags );
                fprintf( fp, "%d\n",     pObjIndex->wear_flags);

		switch ( pObjIndex->item_type )
		{
		    default:
			fprintf( fp, "%d %d %d %d\n",
			    pObjIndex->value[0],
			    pObjIndex->value[1],
			    pObjIndex->value[2],
			    pObjIndex->value[3] );
			break;
		    case ITEM_PILL: case ITEM_POTION: case ITEM_SCROLL:
			fprintf( fp, "%d %d %d %d\n",
			    pObjIndex->value[0],
			    sn_lookup( pObjIndex->value[1] ),
			    sn_lookup( pObjIndex->value[2] ),
			    sn_lookup( pObjIndex->value[3] ) );
			break;
		    case ITEM_STAFF: case ITEM_WAND:
			fprintf( fp, "%d %d %d %d\n",
			    pObjIndex->value[0],
			    pObjIndex->value[1],
			    pObjIndex->value[2],
			    sn_lookup( pObjIndex->value[3] ) );
			break;
		}
                fprintf( fp, "%d ", pObjIndex->weight );
                fprintf( fp, "%d 0\n", pObjIndex->cost );

                for( pAf = pObjIndex->affected; pAf ; pAf = pAf->next )
                {
                fprintf( fp, "A\n%d %d\n",  pAf->location, pAf->modifier );
                }

                for( pEd = pObjIndex->extra_descr; pEd ; pEd = pEd->next )
                {
                fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
                                              fix_string( pEd->description ) );
                }
		if ( pObjIndex->spectype > 0 )
		{
                fprintf( fp, "Q\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%d %d\n", 
			pObjIndex->chpoweron,    pObjIndex->chpoweroff, 
			pObjIndex->chpoweruse,   pObjIndex->victpoweron, 
			pObjIndex->victpoweroff, pObjIndex->victpoweruse, 
			pObjIndex->spectype,     pObjIndex->specpower );
		}
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



void save_rooms( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    ROOMTEXT_DATA *pRt;
    EXIT_DATA *pExit;
    int door;

    fprintf( fp, "#ROOMS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",		pRoomIndex->vnum );
                fprintf( fp, "%s~\n",		pRoomIndex->name );
                fprintf( fp, "%s~\n",		fix_string( pRoomIndex->description ) );
		fprintf( fp, "0 " );
                fprintf( fp, "%d ",		pRoomIndex->room_flags );
                fprintf( fp, "%d\n",		pRoomIndex->sector_type );

                for ( pEd = pRoomIndex->extra_descr; pEd != NULL;
                      pEd = pEd->next )
                {
                    fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
                                                  fix_string( pEd->description ) );
                }
                for( door = 0; door < MAX_DIR; door++ )
                {
                    if ( ( pExit = pRoomIndex->exit[door] )
                          && pExit->to_room )
                    {
			int locks = 0;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR ) 
			&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 1;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 2;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 3;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 4;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 5;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( !IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 6;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 7;
			if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
			&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_BASHPROOF ) )
			&& ( IS_SET( pExit->rs_flags, EX_PASSPROOF ) ) )
			    locks = 8;
			
                        fprintf( fp, "D%d\n",      door );
                        fprintf( fp, "%s~\n",      fix_string( pExit->description ) );
                        fprintf( fp, "%s~\n",      pExit->keyword );
                        fprintf( fp, "%d %d %d\n", locks,
                                                   pExit->key,
                                                   pExit->vnum );
                    }
                }
                for ( pRt = pRoomIndex->roomtext; pRt != NULL;
                      pRt = pRt->next )
                {
                    fprintf( fp, "T\n%s~\n%s~\n%s~\n%s~\n%d %d %d\n", 
			pRt->input, pRt->output, pRt->choutput, pRt->name,
			pRt->type, pRt->power, pRt->mob );
                }
		fprintf( fp, "S\n" );
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



void save_specials( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;
    
    fprintf( fp, "#SPECIALS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun )
            {
                fprintf( fp, "M %d %s Load to: %s\n", pMobIndex->vnum,
                                                      pMobIndex->spec_name,
                                                      pMobIndex->short_descr );
            }
        }
    }

    fprintf( fp, "S\n\n\n\n" );
    return;
}



void save_door_resets( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pExit;
    int door;

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->area == pArea )
            {
                for( door = 0; door < MAX_DIR; door++ )
                {
                    if ( ( pExit = pRoomIndex->exit[door] )
                          && pExit->to_room 
                          && ( IS_SET( pExit->rs_flags, EX_CLOSED )
                          || IS_SET( pExit->rs_flags, EX_LOCKED ) ) )
#if defined( VERBOSE )
			fprintf( fp, "D 0 %d %d %d The %s door of %s is %s\n", 
				pRoomIndex->vnum,
				door,
				IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1,
				dir_name[ door ],
				pRoomIndex->name,
				IS_SET( pExit->rs_flags, EX_LOCKED) ? "closed and locked"
				    : "closed" );
#endif
#if !defined( VERBOSE )
			fprintf( fp, "D 0 %d %d %d\n", 
				pRoomIndex->vnum,
				door,
				IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1 );
#endif
		}
	    }
	}
    }
    return;
}




void save_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    
    fprintf( fp, "#RESETS\n" );

    save_door_resets( fp, pArea );
    for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
    {
	switch ( pReset->command )
	{
	default:
	    bug( "Save_resets: bad command %c.", pReset->command );
	    break;

#if defined( VERBOSE )
	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M 0 %d %d %d Load %s\n", 
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3,
                pLastMob->short_descr );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
	    fprintf( fp, "O 0 %d 0 %d %s loaded to %s\n", 
	        pReset->arg1,
                pReset->arg3,
                capitalize(pLastObj->short_descr),
                pRoom->name );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P 0 %d 0 %d %s put inside %s\n", 
	        pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastObj->short_descr );
            break;

	case 'G':
	    fprintf( fp, "G 0 %d 0 %s is given to %s\n",
	        pReset->arg1,
	        capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E 0 %d 0 %d %s is loaded %s of %s\n",
	        pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                wear_loc_name( pReset->arg3 ),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
            pRoom = get_room_index( pReset->arg1 );
	    fprintf( fp, "R 0 %d %d Randomize %s\n", 
	        pReset->arg1,
                pReset->arg2,
                pRoom->name );
            break;
            }
#endif
#if !defined( VERBOSE )
	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M 0 %d %d %d\n", 
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3 );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
	    fprintf( fp, "O 0 %d 0 %d\n", 
	        pReset->arg1,
                pReset->arg3 );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P 0 %d 0 %d\n", 
	        pReset->arg1,
                pReset->arg3  );
            break;

	case 'G':
	    fprintf( fp, "G 0 %d 0\n", pReset->arg1 );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E 0 %d 0 %d\n",
	        pReset->arg1,
                pReset->arg3 );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
            pRoom = get_room_index( pReset->arg1 );
	    fprintf( fp, "R 0 %d %d\n", 
	        pReset->arg1,
                pReset->arg2 );
            break;
            }
#endif
        }

    fprintf( fp, "S\n\n\n\n" );
    return;
}



void save_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int iHash;
    
    fprintf( fp, "#SHOPS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop )
            {
                pShopIndex = pMobIndex->pShop;

                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != ITEM_NONE )
                    {
                       fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
                    }
                    else
                       fprintf( fp, "0 ");
                }
                fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}



void save_area( AREA_DATA *pArea )
{
    FILE *fp;

    if ( ( fp = fopen( pArea->filename, "w" ) ) == NULL )
    {
	bug( "Open_area: fopen", 0 );
	perror( pArea->filename );
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name        %s~\n",        pArea->name );
    fprintf( fp, "Builders    %s~\n",        fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs       %d %d\n",      pArea->lvnum, pArea->uvnum );
    fprintf( fp, "Security    %d\n",         pArea->security );
    fprintf( fp, "End\n\n\n\n" );

    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_specials( fp, pArea );
    save_resets( fp, pArea );
    save_shops( fp, pArea );

    fprintf( fp, "#$\n" );

    fclose( fp );
    return;
}



void do_asave( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    AREA_DATA *pArea;
    FILE *fp;
    int value;

    fp = NULL;

    if ( ch == NULL )       /* Do an autosave */
    {
        save_area_list();
        for( pArea = area_first; pArea != NULL; pArea = pArea->next )
        {
            save_area( pArea );
            REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
        }
        return;
    }

    smash_tilde( argument );
    strcpy( arg1, argument );

    if ( arg1[0] == '\0' )
    {
    send_to_char( "Syntax:\n\r", ch );
    send_to_char( "  asave list     - saves the area.lst file\n\r",    ch );
    send_to_char( "  asave zone     - saves the zone you're in\n\r",     ch );
    send_to_char( "  asave changed  - saves all changed zones\n\r",    ch );
    send_to_char( "  asave world    - saves the world! (db dump)\n\r", ch );
    send_to_char( "\n\r", ch );
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg1 );

    if ( ( pArea = get_area_data( value ) ) == '\0' && is_number( arg1 ) )
    {
        send_to_char( "That area does not exist.\n\r", ch );
        return;
    }

    if ( is_number( arg1 ) )
    {
        save_area_list();
        save_area( pArea );
        return;
    }

    if ( !str_cmp( "world", arg1 ) || !str_cmp( "all", arg1 ) )
    {
        save_area_list();
        for( pArea = area_first; pArea != '\0'; pArea = pArea->next )
        {
            save_area( pArea );
            REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
        }
        send_to_char( "You saved the world.\n\r", ch );
	send_to_all_char( "Database saved.\n\r" );
        return;
    }

    if ( !str_cmp( "changed", arg1 ) )
    {
        char buf[MAX_INPUT_LENGTH];

        save_area_list();

        send_to_char( "Saved zones:\n\r", ch );
        sprintf( buf, "None.\n\r" );

        for( pArea = area_first; pArea != '\0'; pArea = pArea->next )
        {
            if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
            {
                save_area( pArea );
                sprintf( buf, "%24s - '%s'\n\r", pArea->name, pArea->filename );
                send_to_char( buf, ch );
                REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
            }
        }
        if ( !str_cmp( buf, "None.\n\r" ) )
             send_to_char( buf, ch );
        return;
    }

    if ( !str_cmp( arg1, "list" ) )
    {
      save_area_list();
      return;
    }

    if ( !str_cmp( arg1, "zone" ) )
    {
      save_area_list();
      save_area( ch->in_room->area );
      REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
      send_to_char( "Zone saved.\n\r", ch );
      return;
    }

    do_asave( ch, "" );
    return;
}



void do_arealist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AREA_DATA *pArea;

    sprintf( buf, "[%3s] %-32s (%-5s-%5s)   %10s %3s [%s]\n\r",
       "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );
    send_to_char( buf, ch );

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
    sprintf( buf, "[%3d] %-35s (%-5d-%5d) %13s %2d [%s]\n\r",
        pArea->vnum,
        pArea->name,
        pArea->lvnum,
        pArea->uvnum,
        pArea->filename,
        pArea->security,
        pArea->builders );
	send_to_char( buf, ch );
    }

    return;
}
