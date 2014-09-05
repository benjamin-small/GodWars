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

void home_write()
{
    FILE *fp;

    fflush( fpReserve );
    fclose( fpReserve );
    if ( ( fp = fopen(HOME_AREA, "w") ) == NULL) 
	bug("Cannot Open HOME.ARE file.", 0);
    else
    {  
	fprintf( fp, "#AREA	KaVir   Various Players homes.~\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#HELPS\n");
	fprintf( fp, "0 $~\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#MOBILES\n");
	fprintf( fp, "#0\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#OBJECTS\n");
	fprintf( fp, "#0\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#ROOMS\n");
	fprintf( fp, "#0\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#RESETS\n");
	fprintf( fp, "S\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#SHOPS\n");
	fprintf( fp, "0\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#SPECIALS\n");
	fprintf( fp, "S\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "#$\n");
    }
    fflush( fp );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}
