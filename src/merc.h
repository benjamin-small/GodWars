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


/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef unsigned short    int			ush_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
typedef unsigned short    int			ush_int;
typedef unsigned char			bool;
#endif



/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct  editing_data            EDITING_DATA;	/* OLC */



/*
 * Function types.
 */
typedef	void DO_FUN	args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN	args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN	args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );



/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160

/*
 * Gobal Procedures
 */
void clear_stats    args( (CHAR_DATA *ch) );
void room_is_total_darkness args( (ROOM_INDEX_DATA *pRoomIndex) );
void improve_wpn    args( (CHAR_DATA *ch,int dtype, bool right_hand) );
void improve_stance args( (CHAR_DATA *ch) );
void skillstance    args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void show_spell     args( (CHAR_DATA *ch, int dtype) );
void fightaction    args( (CHAR_DATA *ch, CHAR_DATA *victim, int actype , \
int dtype, int wpntype) ); 
void crack_head     args( (CHAR_DATA *ch, OBJ_DATA *obj, char *argument) );
void critical_hit   args( (CHAR_DATA *ch, CHAR_DATA *victim, int dt, int dam) );
void take_item      args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void raw_kill       args( (CHAR_DATA *victim, CHAR_DATA *ch) );
void trip           args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void disarm         args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void make_corpse    args( (CHAR_DATA *ch, CHAR_DATA *maker) );
void one_hit        args( (CHAR_DATA *ch, CHAR_DATA *victim, int dt, \
				int handtype) );
void make_part      args( (CHAR_DATA *ch, char *argument) );
void clan_table_read        args( ( ) );
void artifact_table_read    args( ( ) );
void clan_table_write       args( ( ) );
void artifact_table_write   args( ( ) );
void home_write             args( ( ) );
void clan_table_powerselect args( (CHAR_DATA *ch, char *power_name) );
void clan_table_namefill    args( (CHAR_DATA *ch) );
void clan_table_bite        args( (CHAR_DATA *ch) );
void clan_table_dec         args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void behead                 args( (CHAR_DATA *victim, CHAR_DATA *ch) );
void paradox                args( (CHAR_DATA *ch) );

/* 
 * Godwars Game Parameters
 * By Rotain
 */

#define SKILL_ADEPT               100
#define SKILL_THAC0_32             18
#define SKILL_THAC0_00              6
#define VERSION_NUMBER              2
#define DONATION_ROOM_WEAPON     6641
#define DONATION_ROOM_ARMOR      6643
#define DONATION_ROOM_REST       6644
#define MAX_VAMPIRE_POWER           3 
#define MAX_CLAN                   11
#define MAX_ART                    12
#define OPTIMISED_PFILES            1 /* Set to 0 for full length pfiles */

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */

#define AGE_NEWBIE		   50 /* This age = no longer a newbie */

#define PARADOX_TICK               30
#define MAX_SKILL		  156
#define MAX_SPELL		   60
#define MAX_LEVEL		   12
#define NO_WATCH		   10
#define NO_GODLESS		   (MAX_LEVEL - 2)
#define LEVEL_HERO		   (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 5)

#define LEVEL_MORTAL		   (MAX_LEVEL - 10)
#define LEVEL_AVATAR		   (MAX_LEVEL - 9)
#define LEVEL_APPRENTICE	   (MAX_LEVEL - 8)
#define LEVEL_MAGE		   (MAX_LEVEL - 7)
#define LEVEL_ARCHMAGE		   (MAX_LEVEL - 6)

#define LEVEL_BUILDER		   (MAX_LEVEL - 5)
#define LEVEL_QUESTMAKER	   (MAX_LEVEL - 4)
#define LEVEL_ENFORCER		   (MAX_LEVEL - 3)
#define LEVEL_JUDGE		   (MAX_LEVEL - 2)
#define LEVEL_HIGHJUDGE		   (MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL)

#define PULSE_PER_SECOND	    4
#define PULSE_GAIN_EXP		  ( 1 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE		  ( 1 )
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (60 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)


/*
 * Rotains Clan Table Structure
 */

struct clan_table_type
{
  char * clan_name;
  char * clan_leader;
  char * clan_powers_1;
  char * clan_powers_2;
  char * clan_powers_3;
  int    clan_kills [MAX_CLAN];
  int    lvnum;       /* Lower vnum of clan area */
  int    uvnum;       /* Upper vnum of clan area */
  int    clan_recall; /* Clan recall vnum */
};

struct artifact_type
{
  char * player_name;
  int    object_vnum;
};


/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};



/*
 * Store types of char logged in.
 */
#define LOGIN_NONE		    0
#define LOGIN_NEWBIE		    1
#define LOGIN_MORTAL		    2
#define LOGIN_GOD		    4
#define LOGIN_VAMPIRE		    8
#define LOGIN_WEREWOLF		   16
#define LOGIN_MAGE		   32
#define LOGIN_DEMON		   64
#define LOGIN_HIGHLANDER	  128



/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0 /* IN: MOTD, OUT: BEGIN PLAY */
#define CON_GET_NAME			 1 /* IN: NONE, OUT: Enter name */
#define CON_GET_OLD_PASSWORD		 2 /* IN: Name, OUT: Show MOTD */
#define CON_CONFIRM_NEW_NAME		 3 /* IN: Sure, OUT: Password */
#define CON_GET_NEW_PASSWORD		 4 /* IN: Pass, OUT: Confirm */
#define CON_CONFIRM_NEW_PASSWORD	 5 /* IN: Pass, OUT: What sex */
#define CON_GET_NEW_SEX			 6 /* IN: Sex,  OUT: Start as vamp */
#define CON_READ_MOTD			 7 /* IN: NONE, OUT: Play */

/*
 * Connected states for OLC.
 */
#define CON_AEDITOR                     -1
#define CON_REDITOR                     -2
#define CON_MEDITOR                     -3
#define CON_OEDITOR                     -4


/*
 * Online editing structure.
 */
struct  editing_data	/* OLC */
{
    void *              pIndex;
    int                 vnum;
    int                 type;
    int                 command;
    char *              text;
    char **             destination;
};

/*
 * Area flags. OLC
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1	/* Area has been modified. */
#define         AREA_ADDED      2	/* Area has been added to. */
#define         AREA_LOADING    4	/* Used for counting in db.c */


/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    sh_int		descriptor;
    sh_int		connected;
    bool		fcommand;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *		outbuf;
    int			outsize;
    int			outtop;
    EDITING_DATA        editing;	/* OLC */
    void *              pEdit;		/* OLC */
    char **             pString;	/* OLC */
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA *	next;
    sh_int	level;
    char *	keyword;
    char *	text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    sh_int	keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};



/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    sh_int		type;
    sh_int		duration;
    sh_int		location;
    sh_int		modifier;
    int			bitvector;
};



/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404


/*
 * Classes.  KaVir.
 */
#define CLASS_NONE	      	   0
#define CLASS_VAMPIRE	      	   1
#define CLASS_WEREWOLF	      	   2
#define CLASS_ABOMINATION      	   3 /* CLASS_VAMPIRE  + CLASS_WEREWOLF */
#define CLASS_MAGE	      	   4
#define CLASS_LICH	      	   5 /* CLASS_VAMPIRE  + CLASS_MAGE */
#define CLASS_SKINDANCER      	   6 /* CLASS_WEREWOLF + CLASS_MAGE */
#define CLASS_DEMON	      	   8
#define CLASS_BAALI	      	   9 /* CLASS_VAMPIRE  + CLASS_DEMON */
#define CLASS_BLACK_SPIRAL_DANCER 10 /* CLASS_WEREWOLF + CLASS_DEMON */
#define CLASS_NEPHANDI	          12 /* CLASS_MAGE     + CLASS_DEMON */
#define CLASS_HIGHLANDER          16
#define CLASS_GHOUL	          32
#define CLASS_ANGEL	          72 /* Includes the CLASS_DEMON bit */
#define CLASS_EXIT	        1024


/*
 * Immunities, for players.  KaVir.
 */
#define IMM_SLASH	      1	/* Resistance to slash, slice. 		*/
#define IMM_STAB	      2	/* Resistance to stab, pierce. 		*/
#define IMM_SMASH	      4	/* Resistance to blast, pound, crush. 	*/
#define IMM_ANIMAL	      8	/* Resistance to bite, claw. 		*/
#define IMM_MISC	     16	/* Resistance to grep, suck, whip. 	*/
#define IMM_CHARM	     32	/* Immune to charm spell. 		*/
#define IMM_HEAT	     64	/* Immune to fire/heat spells. 		*/
#define IMM_COLD	    128	/* Immune to frost/cold spells.		*/
#define IMM_LIGHTNING	    256	/* Immune to lightning spells.		*/
#define IMM_ACID	    512	/* Immune to acid spells.		*/
#define IMM_SUMMON	   1024	/* Immune to being summoned.		*/
#define IMM_VOODOO	   2048	/* Immune to voodoo magic.		*/
#define IMM_VAMPIRE	   4096	/* Allow yourself to become a vampire.	*/
#define IMM_STAKE	   8192	/* Immune to being staked (vamps only). */
#define IMM_SUNLIGHT	  16384	/* Immune to sunlight (vamps only).	*/
#define IMM_SHIELDED	  32768 /* For Obfuscate. Block scry, etc.	*/
#define IMM_HURL	  65536 /* Cannot be hurled.			*/
#define IMM_BACKSTAB	 131072 /* Cannot be backstabbed.		*/
#define IMM_KICK	 262144 /* Cannot be kicked.			*/
#define IMM_DISARM	 524288 /* Cannot be disarmed.			*/
#define IMM_STEAL	1048576 /* Cannot have stuff stolen.		*/
#define IMM_SLEEP	2097152 /* Immune to sleep spell.		*/
#define IMM_DRAIN	4194304 /* Immune to energy drain.		*/
#define IMM_DEMON	8388608 /* Allow yourself to become a demon.	*/
#define IMM_TRANSPORT  16777216 /* Objects can't be transported to you.	*/


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1		/* Auto set for mobs	*/
#define ACT_SENTINEL		      2		/* Stays in one room	*/
#define ACT_SCAVENGER		      4		/* Picks up objects	*/
#define ACT_AGGRESSIVE		     32		/* Attacks PC's		*/
#define ACT_STAY_AREA		     64		/* Won't leave area	*/
#define ACT_WIMPY		    128		/* Flees when hurt	*/
#define ACT_PET			    256		/* Auto set for pets	*/
#define ACT_TRAIN		    512		/* Can train PC's	*/
#define ACT_PRACTICE		   1024		/* Can practice PC's	*/
#define ACT_MOUNT		   2048		/* Can be mounted	*/
#define ACT_NOPARTS		   4096		/* Dead = no body parts	*/
#define ACT_NOEXP		   8192		/* No exp for killing	*/
#define ACT_DOG			  16384		/* Khang and Reina's dog :) */
#define ACT_SKILLED		  32768		/* Stances */
#define ACT_COMMANDED		  65536		/* Can't attack newbies */

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_SHADOWPLANE		     64 /* Creatures in shadow plane - KaVir */
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_FLAMING		   2048 /* For burning creatures - KaVir */
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_ETHEREAL		  16384 /* For ethereal creatures - KaVir */
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_SLEEP		 131072
#define AFF_CHARM		 262144
#define AFF_FLYING		 524288
#define AFF_PASS_DOOR		1048576
#define AFF_POLYMORPH		2097152 /* For polymorphed creatures - KaVir */
#define AFF_SHADOWSIGHT		4194304 /* Can see between planes - KaVir */
#define AFF_WEBBED		8388608 /* Cannot move - KaVir */
#define AFF_CONTRACEPTION      16777216 /* Cannot get pregnant - KaVir */
#define AFF_SHADOW             33554432 /* For Arms of the Abyss - KaVir */   
#define AFF_ROT                67108864 /* For Putrefaction - KaVir */   
#define AFF_FEAR	      134217728 /* Fear affects - KaVir */
#define AFF_JAIL	      268435456 /* For Jail of Water - KaVir */

/*
 * Bits for 'itemaffect'.
 * Used in #MOBILES.
 */
#define ITEMA_SHOCKSHIELD	      1
#define ITEMA_FIRESHIELD	      2
#define ITEMA_ICESHIELD		      4
#define ITEMA_ACIDSHIELD	      8
#define ITEMA_DBPASS                 16
#define ITEMA_CHAOSSHIELD            32
#define ITEMA_ARTIFACT               64
#define ITEMA_REGENERATE            128
#define ITEMA_SPEED                 256
#define ITEMA_VORPAL                512
#define ITEMA_PEACE                1024
#define ITEMA_RIGHT_SILVER         2048
#define ITEMA_LEFT_SILVER          4096
#define ITEMA_REFLECT              8192
#define ITEMA_RESISTANCE          16384
#define ITEMA_VISION              32768
#define ITEMA_STALKER             65536
#define ITEMA_VANISH             131072
#define ITEMA_RAGER              262144
#define ITEMA_HIGHLANDER         524288
#define ITEMA_SHADES            1048576
#define ITEMA_SUNSHIELD         2097152
#define ITEMA_RIGHT_AGG		4194304
#define ITEMA_LEFT_AGG		8388608

/*
 * Rune, Glyph and Sigil bits.
 */
#define RUNE_FIRE	      1
#define RUNE_AIR	      2
#define RUNE_EARTH	      4
#define RUNE_WATER	      8
#define RUNE_DARK	      16
#define RUNE_LIGHT	      32
#define RUNE_LIFE	      64
#define RUNE_DEATH	      128
#define RUNE_MIND	      256
#define RUNE_SPIRIT	      512
#define RUNE_MASTER	      1024
#define GLYPH_CREATION	      1
#define GLYPH_DESTRUCTION     2
#define GLYPH_SUMMONING	      4
#define GLYPH_TRANSFORMATION  8
#define GLYPH_TRANSPORTATION  16
#define GLYPH_ENHANCEMENT     32
#define GLYPH_REDUCTION	      64
#define GLYPH_CONTROL	      128
#define GLYPH_PROTECTION      256
#define GLYPH_INFORMATION     512
#define SIGIL_SELF	      1
#define SIGIL_TARGETING       2
#define SIGIL_AREA	      4
#define SIGIL_OBJECT	      8

/*
 * Spelltypes for mages.
 */
#define ST_MOB_MOB	      1
#define ST_OBJ_MOB	      2
#define ST_OBJ_OBJ	      3

/*
 * Advanced spells.
 */
#define ADV_DAMAGE	      1
#define ADV_AFFECT	      2
#define ADV_ACTION	      4
#define ADV_AREA_AFFECT       8
#define ADV_VICTIM_TARGET    16
#define ADV_OBJECT_TARGET    32
#define ADV_GLOBAL_TARGET    64
#define ADV_NEXT_PAGE       128
#define ADV_PARAMETER       256
#define ADV_SPELL_FIRST     512
#define ADV_NOT_CASTER     1024
#define ADV_NO_PLAYERS     2048
#define ADV_SECOND_VICTIM  4096
#define ADV_SECOND_OBJECT  8192
#define ADV_REVERSED      16384
#define ADV_STARTED	  32768
#define ADV_FINISHED	  65536
#define ADV_FAILED	 131072
#define ADV_MESSAGE_1	 262144
#define ADV_MESSAGE_2	 524288
#define ADV_MESSAGE_3	1048576

/*
 * Advanced spell actions.
 */
#define ACTION_NONE	      0
#define ACTION_MOVE	      1
#define ACTION_MOB	      2
#define ACTION_OBJECT	      3

/*
 * Advanced spell affects.
 */
#define ADV_STR			      1
#define ADV_DEX			      2
#define ADV_INT			      4
#define ADV_WIS			      8
#define ADV_CON			     16
#define ADV_SEX			     32
#define ADV_MANA		     64
#define ADV_HIT			    128
#define ADV_MOVE		    256
#define ADV_AC			    512
#define ADV_HITROLL		   1024
#define ADV_DAMROLL		   2048
#define ADV_SAVING_SPELL	   4096



/* Colour scale macros - added 12th Aug 1995 by Calamar */

#define NO_COLOUR	""		/* Blank */
#define GREY		"[0;1;30m"	/* Dark Grey */
#define D_RED		"[0;0;31m"	/* Dark Red */
#define L_RED		"[0;1;31m"	/* Light Red */
#define D_GREEN		"[0;0;32m"	/* Dark Green */
#define L_GREEN		"[0;1;32m"	/* Light Green */
#define BROWN		"[0;0;33m"	/* Brown */
#define YELLOW		"[0;1;33m"	/* Yellow */
#define D_BLUE		"[0;0;34m"	/* Dark Blue */
#define L_BLUE		"[0;1;34m"	/* Light Blue */
#define MAGENTA		"[0;0;35m"	/* Magenta */
#define PINK		"[0;1;35m"	/* Pink */
#define D_CYAN		"[0;0;36m"	/* Dark Cyan */
#define L_CYAN		"[0;1;36m"	/* Light Cyan */
#define NORMAL		"[0;0;37m"	/* Light Grey */
#define WHITE		"[0;0;38m"	/* White */

#define ADD_COLOUR(_player,_str,_col) { int swh_len = strlen(_str) + 20; \
					char swh_temp[swh_len]; \
if (!IS_NPC(_player) && IS_SET(_player->act, PLR_ANSI)) {swh_temp[0] = '\0'; \
strcpy(swh_temp, _col); strcat(swh_temp, _str); strcat(swh_temp, NORMAL); \
strcpy(_str, swh_temp);}} 

#define SCALE_COLS 4

#define COL_SCALE(_swh_str,_swh_ch,_swh_curr,_swh_max) \
	ADD_COLOUR(_swh_ch, _swh_str, \
	(_swh_curr < 1) ? L_RED : \
	(_swh_curr < _swh_max) ? \
	scale[(SCALE_COLS * _swh_curr) / ((_swh_max > 0) \
	? _swh_max : 1)] : L_CYAN)

extern char *scale[SCALE_COLS];

extern CHAR_DATA Score_ch;      /* Score_ch */

/*
 * File bits.
 */
#define FILE_PLAYER		      0

/*
 * Bits for 'vampire'.
 * Used for player vampires.
 */
#define VAM_FANGS		      1 /* Fangs currently extended */
#define VAM_CLAWS		      2 /* Protean level 2, claws extended */
#define VAM_NIGHTSIGHT		      4 /* Protean level 1, nightsight on */
#define VAM_FLYING		      8 /* Protean bat form */
#define VAM_SONIC		     16 /* Protean bat form */
#define VAM_CHANGED		     32 /* Protean powers level 4 and 5 */
#define VAM_SPIRITEYES		     64 /* Path of Spirit level 2 */
#define VAM_OBFUSCATE		    128 /* Concealed from sight */

#define VAM_DISGUISED		  16384 /* For the Obfuscate disguise ability */

#define VAM_EVILEYE		 131072 /* Path of Spirit level 1 */

#define VAM_EARTHMELDED		 524288 /* Protean level 3, in the ground */


/*
 * Absorbed things
 */
#define ABS_MOB_VNUM		      0 /* Vnum of Flesh Bonded mob */
#define ABS_MOB_HP		      1 /* Hp of Flesh Bonded mob */
#define ABS_MOB_MAX_HP		      2 /* Vnum of Object 1 */
#define ABS_CORPSE_HP		      3 /* Hp of Consumed corpse */
#define ABS_OBJ_1		      4 /* Vnum of Object 2 */
#define ABS_OBJ_2		      5 /* Vnum of Object 3 */


/*
 * Exercising
 */
#define EXE_TYPE		      0 /* Type of exercise */
#define EXE_POSITION		      1 /* Position - eg raised, lowered */
#define EXE_COUNT		      2 /* Counter until improvement */
#define EXE_RECOVER		      3 /* Duration until you recover */

#define EXE_NONE		      0 /* No exercise */
#define EXE_PUSHUPS		      1 /* +1 STR */
#define EXE_SKIPPING		      2 /* +1 DEX */
#define EXE_LEGRAISES		      3 /* +1 STA */

#define EXE_RAISED		      0 /* UP */
#define EXE_LOWERED		      1 /* DOWN */


/*
 * Bits for vicissitude body modifications.
 * Used for player vampires.
 */

#define MOD_TUSKS		      1 /* Large tusks */
#define MOD_HORNS		      2 /* Horns on head */
#define MOD_WINGS		      4 /* Wings */
#define MOD_UNFOLDED		      8 /* Wings are unfolded */
#define MOD_FINS		     16 /* Fins for swimming */
#define MOD_TAIL		     32 /* Tail, does nothing */
#define MOD_EXOSKELETON		     64 /* +10 STA, -10 DEX */

/*
 * Bits for disciplines.
 * Used for player vampires.
 */

#define RES_ANIMALISM		      1
#define RES_AUSPEX		      2
#define RES_DOMINATE		      4
#define RES_PRESENCE		      8
#define RES_TOGGLE		     16
#define RES_CELERITY		     32

#define WILL_ANIMALISM		      1
#define WILL_AUSPEX		      2
#define WILL_DOMINATE		      3
#define WILL_PRESENCE		      4

#define CLAN_FIRST		      0
#define CLAN_SECOND		      1
#define CLAN_THIRD		      2

#define DISC_ANIMALISM		      0
#define DISC_AUSPEX		      1
#define DISC_CELERITY		      2
#define DISC_CHIMERSTRY		      3
#define DISC_DAIMOINON		      4
#define DISC_DOMINATE		      5
#define DISC_FORTITUDE		      6
#define DISC_MELPOMINEE		      7
#define DISC_NECROMANCY		      8
#define DISC_OBEAH		      9
#define DISC_OBFUSCATE		     10
#define DISC_OBTENEBRATION	     11
#define DISC_POTENCE		     12
#define DISC_PRESENCE		     13
#define DISC_PROTEAN		     14
#define DISC_QUIETUS		     15
#define DISC_SERPENTIS		     16
#define DISC_THANATOSIS		     17
#define DISC_THAUMATURGY	     18
#define DISC_VICISSITUDE	     19

#define DISC_MAX		     19

#define POW_NONE		      0
#define POW_ANIMALISM		      1
#define POW_AUSPEX		      2
#define POW_CELERITY		      4
#define POW_CHIMERSTRY		      8
#define POW_DAIMOINON		     16
#define POW_DOMINATE		     32
#define POW_FORTITUDE		     64
#define POW_MELPOMINEE		    128
#define POW_NECROMANCY		    256
#define POW_OBEAH		    512
#define POW_OBFUSCATE		   1024
#define POW_OBTENEBRATION	   2048
#define POW_POTENCE		   4096
#define POW_PRESENCE		   8192
#define POW_PROTEAN		  16384
#define POW_QUIETUS		  32768
#define POW_SERPENTIS		  65536
#define POW_THANATOSIS		 131072
#define POW_THAUMATURGY		 262144
#define POW_VICISSITUDE		 524288

#define COT_BANNED		      0 /* Min hp required */
#define COT_BASIC		      1 /* Disciplines at level 1+ */
#define COT_INTERMEDIATE	      2 /* Disciplines at level 3+ */
#define COT_ADVANCED		      3 /* Disciplines at level 5+ */
#define COT_EXPERT		      4 /* Disciplines at level 6+ */
#define COT_HP			      5 /* Min hp required */
#define COT_GENERATION		      6 /* Highest generation allowed */
#define COT_SKILL		      7 /* Min skill required */
#define COT_STANCE		      8 /* Min stance required */
#define COT_EXTRA		      9 /* Extra requirements */

#define COT_MAX			     11 /* Variable limit */

#define COT_ADV_STANCE		      1 /* Min stance must be advanced */
#define COT_GOLCONDA		      2 /* Must have attained Golconda */
#define COT_NO_DIABLERIE	      4 /* Must not commit diablerie */
#define COT_NO_KILL_SECT	      8 /* Must not kill member of same sect */
#define COT_NO_KILL_COTERIE	     16 /* Must not kill member of coterie */

#define BREED_HOMID		      0
#define BREED_LUPUS		      1
#define BREED_METIS		      2
#define BREED_MAX		      2

#define AUSPICE_RAGABASH	      0
#define AUSPICE_THEURGE		      1
#define AUSPICE_PHILODOX	      2
#define AUSPICE_GALLIARD	      3
#define AUSPICE_AHROUN		      4
#define AUSPICE_MAX		      4

#define TRIBE_BLACK_FURIES	      0
#define TRIBE_BONE_GNAWERS	      1
#define TRIBE_CHILDREN_OF_GAIA	      2
#define TRIBE_FIANNA		      3
#define TRIBE_GET_OF_FENRIS	      4
#define TRIBE_GLASS_WALKERS	      5
#define TRIBE_RED_TALONS	      6
#define TRIBE_SHADOW_LORDS	      7
#define TRIBE_SILENT_STRIDERS	      8
#define TRIBE_SILVER_FANGS	      9
#define TRIBE_STARGAZERS	      10
#define TRIBE_UKTENA		      11
#define TRIBE_WENDIGOS		      12
#define TRIBE_MAX		      12


/*
 * Vampiric Rituals.
 */
#define R1			      0 /* Rituals level 1 */
#define R2			      1 /* Rituals level 2 */
#define R3			      2 /* Rituals level 3 */
#define R4			      3 /* Rituals level 4 */
#define R5			      4 /* Rituals level 5 */
#define R6			      5 /* Rituals level 6 */
#define R7			      6 /* Rituals level 7 */
#define R8			      7 /* Rituals level 8 */
#define R9			      8 /* Rituals level 9 */
#define R10			      9 /* Rituals level 10 */

#define R1_DEFENCE_HAVEN	      1 /* Defence of Sacred Haven */
#define R1_WAKE_FRESHNESS	      2 /* Wake with Morning's Freshness */
#define R1_DEFLECTION_DOOM	      4 /* Deflection of Wooden Doom */
#define R1_DEVILS_TOUCH		      8 /* Devil's Touch */
#define R1_ENGAGING_VESSEL	     16 /* Engaging Vessel of Transferance */
#define R1_BLOOD_RUSH		     32 /* Blood Rush */

#define R2_WARD_GHOULS		      1 /* Ward vs Ghouls */

#define R3_WARD_LUPINES		      1 /* Ward vs Lupines */

#define R4_WARD_KINDRED		      1 /* Ward vs Kindred */

#define R5_WARD_SPIRITS		      1 /* Ward vs Spirits */

#define R6_RAISE_DEAD		      1 /* Raise the Dead */



/*
 * World affect bits.
 */
#define WORLD_FOG		      1
#define WORLD_RAIN		      2
#define WORLD_ECLIPSE		      4
#define WORLD_NICE		      8 /* No swearing while frenzied */


/*
 * Bits for blood.
 * Used for player vampires.
 */
#define BLOOD_CURRENT		      0
#define BLOOD_POOL		      1
#define BLOOD_POTENCY		      2



/*
 * Bits for werewolf forms.
 */
#define FORM_HOMID		      1
#define FORM_GLABRO		      2
#define FORM_CRINOS		      3
#define FORM_HISPO		      4
#define FORM_LUPUS		      5



/*
 * Bits for 'polymorph'.
 * Used for players.
 */
#define POLY_BAT		      1
#define POLY_WOLF		      2
#define POLY_MIST		      4
#define POLY_SERPENT		      8
#define POLY_RAVEN		     16
#define POLY_FISH		     32
#define POLY_FROG		     64
#define POLY_ZULO		    128
#define POLY_SHADOW		    256
#define POLY_SPIRIT		    512
#define POLY_MASK		   1024 /* Vicissitude 3 */
#define POLY_TRUE		   2048 /* Chimerstry 6 */
#define POLY_HAGS		   4096 /* Thanatosis 1 */
#define POLY_CHANGELING		   8192 /* Vicissitude 1 */
#define POLY_FLESHCRAFT		  16384 /* Vicissitude 2 */
#define POLY_BONECRAFT		  32768 /* Vicissitude 3 */



/*
 * Languages.
 */
#define LANG_COMMON		      0
#define DIA_OLDE		      1
#define DIA_BAD			      2
#define LANG_DARK		      4



/*
 * Score.
 */
#define SCORE_TOTAL_XP		      0
#define SCORE_HIGH_XP		      1
#define SCORE_TOTAL_LEVEL	      2
#define SCORE_HIGH_LEVEL	      3
#define SCORE_QUEST		      4
#define SCORE_NUM_QUEST		      5



/*
 * Zombie Lord.
 */
#define ZOMBIE_NOTHING		      0
#define ZOMBIE_TRACKING		      1
#define ZOMBIE_ANIMATE		      2
#define ZOMBIE_CAST		      3
#define ZOMBIE_REST		      4



/*
 * Damcap values.
 */
#define DAM_CAP		      0
#define DAM_CHANGE	      1

/*
 * Bits for Demonic Champions.
 */
#define POWER_CURRENT		      0
#define POWER_TOTAL		      1

#define C_POWERS		      1
#define C_CURRENT		      2
#define C_HEAD			      3
#define C_TAIL			      4
#define O_POWERS		      5

#define DEM_UNFOLDED		      1

#define DEM_FANGS		      1
#define DEM_CLAWS		      2
#define DEM_DEMONFORM		      4
#define DEM_HOOVES		      8
#define DEM_EYES		     16
#define DEM_WINGS		     32
#define DEM_MIGHT		     64
#define DEM_TOUGH		    128
#define DEM_SPEED		    256
#define DEM_TRAVEL		    512
#define DEM_SCRY		   1024
#define DEM_SHADOWSIGHT		   2048
#define DEM_INFERNO		   4096
#define DEM_EYESPY		   8192
#define DEM_FIREWALL		  16384
#define DEM_IMP			  32768

#define HEAD_NORMAL		      0
#define HEAD_WOLF		      1
#define HEAD_EAGLE		      2
#define HEAD_LION		      4
#define HEAD_SNAKE		      8

#define TAIL_NONE		      0
#define TAIL_POISONOUS		      1
#define TAIL_SCORPION		      2

#define DEM_MOVE		      1 /* Can roll as an object */
#define DEM_LEAP		      2 /* Can leap out of someone's hands */
#define DEM_MAGIC		      4 /* Can cast spells as an object */
#define DEM_LIFESPAN		      8 /* Can change lifespan */


/*
 * Bits for Demonic Armour.
 */
#define ARM_BLACK		      0
#define ARM_GREY		      1
#define ARM_PURPLE		      2
#define ARM_RED			      3
#define ARM_BRASS		      4



/*
 * Bits for Angelic Armour.
 */
#define ARM_WHITE		      0
#define ARM_YELLOW		      1
#define ARM_BLUE		      2
#define ARM_SILVER		      3
#define ARM_GOLD		      4



/*
 * Mounts
 */
#define IS_ON_FOOT		      0
#define IS_MOUNT		      1
#define IS_RIDING		      2
#define IS_CARRIED		      4
#define IS_CARRYING		      8



/*
 * Embrace positions.
 */
#define ARE_NONE		      0
#define ARE_EMBRACING		      1
#define ARE_EMBRACED		      2



/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_BLOOD_SPRING	     23

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716

/* For KaVir's stuff */
#define OBJ_VNUM_SOULBLADE	  30000
#define OBJ_VNUM_PORTAL		  30001
#define OBJ_VNUM_EGG		  30002
#define OBJ_VNUM_EMPTY_EGG	  30003
#define OBJ_VNUM_SPILLED_ENTRAILS 30004
#define OBJ_VNUM_QUIVERING_BRAIN  30005
#define OBJ_VNUM_SQUIDGY_EYEBALL  30006
#define OBJ_VNUM_SPILT_BLOOD      30007
#define OBJ_VNUM_VOODOO_DOLL      30010
#define OBJ_VNUM_RIPPED_FACE      30012
#define OBJ_VNUM_TORN_WINDPIPE    30013
#define OBJ_VNUM_CRACKED_HEAD     30014
#define OBJ_VNUM_SLICED_EAR	  30025
#define OBJ_VNUM_SLICED_NOSE	  30026
#define OBJ_VNUM_KNOCKED_TOOTH	  30027
#define OBJ_VNUM_TORN_TONGUE	  30028
#define OBJ_VNUM_SEVERED_HAND	  30029
#define OBJ_VNUM_SEVERED_FOOT	  30030
#define OBJ_VNUM_SEVERED_THUMB	  30031
#define OBJ_VNUM_SEVERED_INDEX	  30032
#define OBJ_VNUM_SEVERED_MIDDLE	  30033
#define OBJ_VNUM_SEVERED_RING	  30034
#define OBJ_VNUM_SEVERED_LITTLE	  30035
#define OBJ_VNUM_SEVERED_TOE	  30036
#define OBJ_VNUM_PROTOPLASM	  30037
#define OBJ_VNUM_QUESTCARD	  30039
#define OBJ_VNUM_QUESTMACHINE	  30040
#define OBJ_VNUM_ASHES            30042
#define MOB_VNUM_HOUND		  30000
#define MOB_VNUM_GUARDIAN	  30001
#define MOB_VNUM_MOUNT		  30006
#define MOB_VNUM_FROG		  30007
#define MOB_VNUM_RAVEN		  30008
#define MOB_VNUM_CAT		  30009
#define MOB_VNUM_DOG		  30010
#define MOB_VNUM_EYE		  30012
#define MOB_VNUM_MOLERAT	  30013
#define MOB_VNUM_YAK		  30014
#define MOB_VNUM_ASTRAL		  30015
#define MOB_VNUM_CLONE		  30016



/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PORTAL		     27
#define ITEM_EGG		     28
#define ITEM_VOODOO		     29
#define ITEM_STAKE		     30
#define ITEM_MISSILE		     31 /* Ammo vnum, cur, max, type */
#define ITEM_AMMO		     32 /* ???, dam min, dam max, type */
#define ITEM_QUEST		     33
#define ITEM_QUESTCARD		     34
#define ITEM_QUESTMACHINE	     35
#define ITEM_SYMBOL		     36
#define ITEM_BOOK		     37
#define ITEM_PAGE		     38
#define ITEM_TOOL		     39
#define ITEM_WALL		     40 /* For wall of water */
#define ITEM_WARD		     41 /* For ritual wards */



/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_THROWN		      4
#define ITEM_KEEP		      8
#define ITEM_VANISH		     16 /* Illusion created by Chimerstry */
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_LOYAL		  16384
#define ITEM_SHADOWPLANE	  32768



/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128 
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024 
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_FACE		  32768



/*
 * Special types.
 * Used in #OBJECTS for special items - KaVir.
 */
#define SITEM_ACTIVATE		      1
#define SITEM_TWIST		      2
#define SITEM_PRESS		      4
#define SITEM_PULL		      8
#define SITEM_TARGET		      16
#define SITEM_SPELL		      32
#define SITEM_TRANSPORTER	      64
#define SITEM_TELEPORTER	      128
#define SITEM_DELAY1		      256
#define SITEM_DELAY2		      512
#define SITEM_OBJECT		     1024
#define SITEM_MOBILE		     2048
#define SITEM_ACTION		     4096
#define SITEM_MORPH		     8192
#define SITEM_SILVER		    16384
#define SITEM_WOLFWEAPON	    32768
#define SITEM_CHAMPWEAPON	   131072
#define SITEM_DEMONIC		   262144
#define SITEM_HIGHLANDER	   524288
#define SITEM_MESSAGE		  1048576


/*
 * Apply types (for quest affects).
 * Used in #OBJECTS.
 */
#define QUEST_STR		      1
#define QUEST_DEX		      2
#define QUEST_INT		      4
#define QUEST_WIS		      8
#define QUEST_CON		     16
#define QUEST_HITROLL		     32
#define QUEST_DAMROLL		     64
#define QUEST_HIT		    128
#define QUEST_MANA		    256
#define QUEST_MOVE		    512
#define QUEST_AC		   1024

#define QUEST_NAME		   2048
#define QUEST_SHORT		   4096
#define QUEST_LONG		   8192
#define QUEST_FREENAME		  16384

#define QUEST_ENCHANTED		  32768
#define QUEST_SPELLPROOF	  65536
#define QUEST_ARTIFACT		 131072
#define QUEST_IMPROVED		 262144
#define QUEST_MASTER_RUNE	 524288
#define QUEST_RELIC		1048576
#define QUEST_VANISH		2097152 /* Object that doesn't save */

#define QUEST_BLOODAGONY	4194304 /* Quietus level 4 */
#define QUEST_SONG		8388608 /* Object contains a song */
#define QUEST_CONJURED	       16777216 /* Object was conjured */
#define QUEST_FOLLOWING	       33554432 /* Object follows owner */

/*
 * Qstats.
 */
#define QS_TOTAL		      0
#define QS_HIT			      1
#define QS_DAM			      2
#define QS_AC			      3

#define QS_11			      0
#define QS_12			      1
#define QS_13			      2
#define QS_14			      3
#define QS_15			      4

#define QS_DONE			      1 /* Stance is usable */
#define QS_HIT_LESSER		      2
#define QS_HIT_GREATER		      4
#define QS_HIT_SUPREME		      8
#define QS_DAM_LESSER		     16
#define QS_DAM_GREATER		     32
#define QS_DAM_SUPREME		     64
#define QS_ARM_LESSER		    128
#define QS_ARM_GREATER		    256
#define QS_ARM_SUPREME		    512
#define QS_POW_LESSER		   1024
#define QS_POW_GREATER		   2048
#define QS_POW_SUPREME		   4096
#define QS_RES_LESSER		   8192
#define QS_RES_GREATER		  16384
#define QS_RES_SUPREME		  32768
#define QS_SUPER_PARRY		  65536
#define QS_SUPER_DODGE		 131072
#define QS_SUPER_SPEED		 262144
#define QS_SUPER_BYPASS		 524288


/*
 * Tool types.
 */
#define TOOL_PEN		      1
#define TOOL_PLIERS		      2
#define TOOL_SCALPEL		      4



/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_POLY		     25
#define APPLY_GENERATION	     26
#define APPLY_POTENCE		     27
#define APPLY_CELERITY		     28
#define APPLY_FORTITUDE		     29
#define APPLY_BLOOD_MAX		     30
#define APPLY_BLOOD_POT		     31
#define APPLY_ANIMALISM		     32
#define APPLY_AUSPEX		     33
#define APPLY_CHIMERSTRY	     34
#define APPLY_DAIMOINON		     35
#define APPLY_DOMINATE		     36
#define APPLY_MELPOMINEE	     37
#define APPLY_NECROMANCY	     38
#define APPLY_OBEAH		     39
#define APPLY_OBFUSCATE		     40
#define APPLY_OBTENEBRATION	     41
#define APPLY_PRESENCE		     42
#define APPLY_PROTEAN		     43
#define APPLY_QUIETUS		     44
#define APPLY_SERPENTIS		     45
#define APPLY_THANATOSIS	     46
#define APPLY_THAUMATURGY	     47
#define APPLY_VICISSITUDE	     48
#define APPLY_SET_ANIMALISM	     49 /* SET_ are set rather than added */
#define APPLY_SET_AUSPEX	     50
#define APPLY_SET_CELERITY	     51
#define APPLY_SET_CHIMERSTRY	     52
#define APPLY_SET_DAIMOINON	     53
#define APPLY_SET_DOMINATE	     54
#define APPLY_SET_FORTITUDE	     55
#define APPLY_SET_MELPOMINEE	     56
#define APPLY_SET_NECROMANCY	     57
#define APPLY_SET_OBEAH		     58
#define APPLY_SET_OBFUSCATE	     59
#define APPLY_SET_OBTENEBRATION      60
#define APPLY_SET_POTENCE	     61
#define APPLY_SET_PRESENCE	     62
#define APPLY_SET_PROTEAN	     63
#define APPLY_SET_QUIETUS	     64
#define APPLY_SET_SERPENTIS	     65
#define APPLY_SET_THANATOSIS	     66
#define APPLY_SET_THAUMATURGY	     67
#define APPLY_SET_VICISSITUDE	     68



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_HELL		  30000
#define ROOM_VNUM_CRYPT		  30001
#define ROOM_VNUM_DISCONNECTION	  30002
#define ROOM_VNUM_IN_OBJECT	  30008
#define ROOM_VNUM_HEAVEN	  30050



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_PRIVATE		    512
#define ROOM_SAFE		   1024
#define ROOM_SOLITARY		   2048
#define ROOM_PET_SHOP		   4096
#define ROOM_NO_RECALL		   8192
#define ROOM_NO_TELEPORT	  16384
#define ROOM_TOTAL_DARKNESS	  32768

#define ROOM2_SILENCE		      1
#define ROOM2_TIMESLOW		      2 /* Temporis */


/*
 * Room text flags (KaVir).
 * Used in #ROOMS.
 */
#define RT_LIGHTS		      1 /* Toggles lights on/off */
#define RT_SAY			      2 /* Use this if no others powers */
#define RT_ENTER		      4
#define RT_CAST			      8
#define RT_THROWOUT		     16 /* Erm...can't remember ;) */
#define RT_OBJECT		     32 /* Creates an object */
#define RT_MOBILE		     64 /* Creates a mobile */
#define RT_LIGHT		    128 /* Lights on ONLY */
#define RT_DARK			    256 /* Lights off ONLY */
#define RT_OPEN_LIFT		    512 /* Open lift */
#define RT_CLOSE_LIFT		   1024 /* Close lift */
#define RT_MOVE_LIFT		   2048 /* Move lift */
#define RT_SPELL		   4096 /* Cast a spell */
#define RT_PORTAL		   8192 /* Creates a one-way portal */
#define RT_TELEPORT		  16384 /* Teleport player to room */

#define RT_ACTION		  32768
#define RT_BLANK_1		  65536
#define RT_BLANK_2		 131072

#define RT_RETURN		1048576 /* Perform once */
#define RT_PERSONAL		2097152 /* Only shows message to char */
#define RT_TIMER		4194304 /* Sets object timer to 1 tick */



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define MAX_DIR			      6	/* OLC */



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_BASHPROOF		      8 /* KaVir */
#define EX_BASHED		     16 /* KaVir */
#define EX_PICKPROOF		     32
#define EX_PASSPROOF		     64 /* KaVir */
#define EX_WALL_WATER		    128 /* KaVir - Path of Water power 4 */
#define EX_WARD_GHOULS		    256 /* KaVir - Ward vs Ghouls */
#define EX_WARD_LUPINES		    512 /* KaVir - Ward vs Lupines */
#define EX_WARD_KINDRED		   1024 /* KaVir - Ward vs Kindred */
#define EX_WARD_SPIRITS		   2048 /* KaVir - Ward vs Spirits */
#define EX_WALL_FLAME		   4096 /* KaVir - Path of Flame power 4 */
#define EX_WALL_GRANITE		   8192 /* KaVir - Philodox power 5 */



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNDERWATER		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equipment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_FACE		     18
#define WEAR_SCABBARD_L		     19
#define WEAR_SCABBARD_R		     20
#define WEAR_STAKE		     21
#define MAX_WEAR		     22




/*
 * Locations for damage.
 */
#define LOC_HEAD		      0
#define LOC_BODY		      1
#define LOC_ARM_L		      2
#define LOC_ARM_R		      3
#define LOC_LEG_L		      4
#define LOC_LEG_R		      5

/*
 * For Head
 */
#define LOST_EYE_L		       1
#define LOST_EYE_R		       2
#define LOST_EAR_L		       4
#define LOST_EAR_R		       8
#define LOST_NOSE		      16
#define BROKEN_NOSE		      32
#define BROKEN_JAW		      64
#define BROKEN_SKULL		     128
#define LOST_HEAD		     256
#define LOST_TOOTH_1		     512 /* These should be added..... */
#define LOST_TOOTH_2		    1024 /* ...together to caculate... */
#define LOST_TOOTH_4		    2048 /* ...the total number of.... */
#define LOST_TOOTH_8		    4096 /* ...teeth lost.  Total..... */
#define LOST_TOOTH_16		    8192 /* ...possible is 31 teeth.   */
#define LOST_TONGUE		   16384

/*
 * For Body
 */
#define BROKEN_RIBS_1		       1 /* Remember there are a total */
#define BROKEN_RIBS_2		       2 /* of 12 pairs of ribs in the */
#define BROKEN_RIBS_4		       4 /* human body, so not all of  */
#define BROKEN_RIBS_8		       8 /* these bits should be set   */
#define BROKEN_RIBS_16		      16 /* at the same time.          */
#define BROKEN_SPINE		      32
#define BROKEN_NECK		      64
#define CUT_THROAT		     128
#define CUT_STOMACH		     256
#define CUT_CHEST		     512
#define LOST_ENTRAILS		    1024

/*
 * For Arms
 */
#define BROKEN_ARM		       1
#define LOST_ARM		       2
#define LOST_HAND		       4
#define LOST_FINGER_I		       8 /* Index finger */
#define LOST_FINGER_M		      16 /* Middle finger */
#define LOST_FINGER_R		      32 /* Ring finger */
#define LOST_FINGER_L		      64 /* Little finger */
#define LOST_THUMB		     128
#define BROKEN_FINGER_I		     256 /* Index finger */
#define BROKEN_FINGER_M		     512 /* Middle finger */
#define BROKEN_FINGER_R		    1024 /* Ring finger */
#define BROKEN_FINGER_L		    2048 /* Little finger */
#define BROKEN_THUMB		    4096
#define SLIT_WRIST		    8192

/*
 * For Legs
 */
#define BROKEN_LEG		       1
#define LOST_LEG		       2
#define LOST_FOOT		       4
#define LOST_TOE_A		       8
#define LOST_TOE_B		      16
#define LOST_TOE_C		      32
#define LOST_TOE_D		      64 /* Smallest toe */
#define LOST_TOE_BIG		     128
#define BROKEN_TOE_A		     256
#define BROKEN_TOE_B		     512
#define BROKEN_TOE_C		    1024
#define BROKEN_TOE_D		    2048 /* Smallest toe */
#define BROKEN_TOE_BIG		    4096

/*
 * For Bleeding
 */
#define BLEEDING_HEAD		       1
#define BLEEDING_THROAT		       2
#define BLEEDING_ARM_L		       4
#define BLEEDING_ARM_R		       8
#define BLEEDING_HAND_L		      16
#define BLEEDING_HAND_R		      32
#define BLEEDING_LEG_L		      64
#define BLEEDING_LEG_R		     128
#define BLEEDING_FOOT_L		     256
#define BLEEDING_FOOT_R		     512
#define BLEEDING_WRIST_L	    1024
#define BLEEDING_WRIST_R	    2048
#define BLEEDING_STOMACH	    4096
#define BLEEDING_CHEST		    8192


/*
 * For Spec powers on players
 */
#define EYE_SPELL		       1 /* Spell when they look at you */
#define EYE_SELFACTION		       2 /* You do action when they look */
#define EYE_ACTION		       4 /* Others do action when they look */




/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Stats - KaVir.
 */
#define STAT_STR		      0 /* Strength */
#define STAT_INT		      1 /* Intelligence */
#define STAT_WIS		      2 /* Wisdom */
#define STAT_DEX		      3 /* Dexterity */
#define STAT_CON		      4 /* Constitution */



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_MEDITATING		      5
#define POS_SITTING		      6
#define POS_RESTING		      7
#define POS_FIGHTING		      8
#define POS_STANDING		      9



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1		/* Don't EVER set.	*/

#define PLR_DEMON		      2
#define PLR_CHAMPION		      4

#define PLR_AUTOEXIT		      8
#define PLR_AUTOLOOT		     16
#define PLR_AUTOSAC                  32
#define PLR_BLANK		     64
#define PLR_BRIEF		    128
#define PLR_COMBINE		    512
#define PLR_PROMPT		   1024
#define PLR_TELNET_GA		   2048

#define PLR_HOLYLIGHT		   4096
#define PLR_WIZINVIS		   8192
#define PLR_ANSI		  16384

#define	PLR_SILENCE		  32768
#define PLR_NO_EMOTE		  65536
#define PLR_NO_TELL		 262144
#define PLR_LOG			 524288
#define PLR_RETIRE		1048576
#define PLR_FREEZE		2097152
#define PLR_WOLFMAN		4194304
#define PLR_WEREWOLF		8388608
#define PLR_GODLESS	       16777216
#define PLR_WATCHER	       33554432
#define PLR_LAST_AGG	       67108864



/*
 * EXTRA bits for players. (KaVir)
 */
#define EXTRA_SIRE		      1
#define EXTRA_PRINCE		      2
#define EXTRA_TRUSTED		      4
#define EXTRA_NEWPASS		      8
#define EXTRA_OSWITCH		     16
#define EXTRA_SWITCH		     32
#define EXTRA_FAKE_CON		     64
#define TIED_UP			    128
#define GAGGED			    256
#define BLINDFOLDED		    512
#define EXTRA_PSAY		   1024
#define EXTRA_DONE		   2048
#define EXTRA_EXP		   4096
#define EXTRA_PREGNANT		   8192
#define EXTRA_LABOUR		  16384
#define EXTRA_BORN		  32768
#define EXTRA_PROMPT		  65536
#define EXTRA_MARRIED		 131072
#define EXTRA_DYING		 262144 /* You are about to die! */
#define EXTRA_NON_NEWBIE	 524288
#define EXTRA_CALL_ALL		1048576
#define EXTRA_CAN_SEE		2097152 /* You can't see, but familiar can */
#define EXTRA_CLEARED_EQ	4194304 /* Eq has been removed from player */
#define EXTRA_AWE		8388608 /* Everyone is in awe of you */
#define EXTRA_NO_ACTION	       16777216 /* You can't do anything */
#define EXTRA_ILLUSION	       33554432 /* Mob is really just an illusion */
#define EXTRA_VICISSITUDE      67108864 /* Cannot see through disguise */
#define EXTRA_INVIS_EMAIL     134217728 /* Cannot see email via finger */
#define EXTRA_AUTHORISED      268435456 /* Player is authorised */

/*
 * MORE bits for players. (KaVir)
 */
#define MORE_NPC		      1 /* NPC - ie cannot improve */
#define MORE_ANTITRIBU		      2 /* Antitribu vampire */
#define MORE_MUMMIFIED		      4 /* Serpentis 3 */
#define MORE_DARKNESS		      8 /* Obtenebration 1 */
#define MORE_SILENCE		     16 /* Quietus 1 */
#define MORE_PLASMA		     32 /* Vicissitude 5 */
#define MORE_ASHES		     64 /* Thanatosis 3 */
#define MORE_GUARDIAN		    128 /* Necromancy 4 */
#define MORE_LOYAL		    256 /* Dominate 6 */
#define MORE_SINGING		    512 /* Singing atm */
#define MORE_LEADER		   1024 /* Coterie leader */
#define MORE_HAND_FLAME		   2048 /* Hand of Flame */
#define MORE_SPIRIT		   4096 /* You become a true spirit */
#define MORE_STAKED		   8192 /* You have been staked */
#define MORE_DEFLECTION		  16384 /* Deflection ritual in effect */
#define MORE_PROJECTION		  32768 /* Using astral projection */
#define MORE_CHAMPION		  65536 /* Coterie champion */
#define MORE_REINA		 131072 /* Song of Reina - Melpominee 2 */
#define MORE_EVILEYE		 262144 /* Evil Eye ON */
#define MORE_FLAMES		 524288 /* Flames of the nether world */
#define MORE_COURAGE		1048576 /* Courage - Presence 7 */
#define MORE_ANGER		2097152 /* Anger - Presence 8 */
#define MORE_ANIMAL_MASTER	4194304 /* Animals obey and dont attack you */
#define MORE_INFORM		8388608 /* Inform when say */
#define MORE_CALM	       16777216 /* Cancel's Anger, cannot attack */
#define MORE_BRISTLES	       33554432 /* Metis power level 5 */
#define MORE_OBEAH	       67108864 /* Obeah third eye is open */
#define MORE_NOPAIN	      134217728 /* Obeah 2, no pain penalties */
#define MORE_NEUTRAL	      268435456 /* Obeah 3, makes room safe room */

#define CRIME_C_OUTLAW		      1 /* Camarilla Outlaw */
#define CRIME_A_OUTLAW		      2 /* Anarch Outlaw */
#define CRIME_S_OUTLAW		      4 /* Sabbat Outlaw */
#define CRIME_I_OUTLAW		      8 /* Inconnu Outlaw */
#define CRIME_BLOODHUNT		     16 /* Camarilla Bloodhunted */
#define CRIME_DIABLERIE		     32 /* Committed Diablerie */
#define CRIME_MASQUERADE	     64 /* Breached the Masquerade */
#define CRIME_DOMAIN		    128 /* Disobeyed Prince */
#define CRIME_PROGENY		    256 /* Sired childe without permission */
#define CRIME_ACCOUNTING	    512 /* Childe broke tradition */
#define CRIME_HOSPITALITY	   1024 /* Not yet presented self to prince */
#define CRIME_DESTRUCTION	   2048 /* Killed a vampire in same sect */

#define DIAB_TIME		      0 /* Duration of diablerie aura */
#define DIAB_EVER		      1 /* Number of diableries committed */

#define LIE_TRUE		      0
#define LIE_FALSE		      1


/*
 * AGE Bits.
 */
#define AGE_CHILDE		      0
#define AGE_NEONATE		      1
#define AGE_ANCILLA		      2
#define AGE_ELDER		      3
#define AGE_METHUSELAH		      4
#define AGE_ANTEDILUVIAN	      5

#define AGE_PUP			      0
#define AGE_YOUTH		      1
#define AGE_ADULT		      2
#define AGE_ELDER		      3
#define AGE_ANCIENT		      4


/*
 * Status.
 */
#define STATUS_FEAR		      0
#define STATUS_PRESTIGE		      1
#define STATUS_NATURAL		      2


/*
 * Stances for combat
 */
#define STANCE_NONE		     -1
#define STANCE_NORMAL		      0
#define STANCE_VIPER		      1
#define STANCE_CRANE		      2
#define STANCE_CRAB		      3
#define STANCE_MONGOOSE		      4
#define STANCE_BULL		      5
#define STANCE_MANTIS		      6
#define STANCE_DRAGON		      7
#define STANCE_TIGER		      8
#define STANCE_MONKEY		      9
#define STANCE_SWALLOW		     10


/*
 *  Totems for werewolves.
 */
#define TOTEM_MANTIS	      0
#define TOTEM_BEAR	      1
#define TOTEM_LYNX	      2
#define TOTEM_BOAR	      3
#define TOTEM_OWL	      4
#define TOTEM_SPIDER	      5
#define TOTEM_WOLF	      6
#define TOTEM_HAWK	      7
#define TOTEM_SILVER	     10



/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete	*/
#define PLR_CHAT		    256	/* Obsolete	*/
#define PLR_NO_SHOUT		 131072	/* Obsolete	*/
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		      1
#define	CHANNEL_CHAT		      2
#define	CHANNEL_HACKER		      4
#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define	CHANNEL_VAMPTALK	    256
#define	CHANNEL_HOWL		    512
#define	CHANNEL_LOG		   1024
#define	CHANNEL_PRAY		   2048
#define	CHANNEL_INFO		   4096
#define	CHANNEL_NEWBIE		   8192
#define	CHANNEL_TELL		  16384
#define	CHANNEL_BUILD		  32768



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    char *              spec_name;	/* OLC */
    SHOP_DATA *		pShop;
    AREA_DATA *         area;		/* OLC */
    CHAR_DATA *		mount;
    CHAR_DATA *		wizard;
    CHAR_DATA *		embrace;
    CHAR_DATA *		next_fight;
    CHAR_DATA *		prev_fight;
    OBJ_DATA *		sat;
    char *		hunting;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *		lord;
    char *		clan;
    char *		side;
    char *		morph;
    char *		createtime;
    char *		lasttime;
    char *		lasthost;
    char *		powertype;
    char *		poweraction;
    char *		pload;
    char *		prompt;
    char *		cprompt;
    sh_int		spectype;
    sh_int		specpower;
    int		        loc_hp	[7];
    sh_int		vnum;
    sh_int		count;
    sh_int		killed;
    sh_int		sex;
    sh_int		mounted;
    sh_int		embraced;
    sh_int		home;
    sh_int		init;
    int		        level;
    int			immune;
    int			polyaff;
    int			vampaff;
    int			itemaffect;
    int			vamppass;
    int			form;
    int			body;
    int			act;
    int			extra;
    int			more;
    int			affected_by;
    sh_int		alignment;
    sh_int		hitroll;		/* Unused */
    sh_int		ac;			/* Unused */
    sh_int		hitnodice;		/* Unused */
    sh_int		hitsizedice;		/* Unused */
    sh_int		hitplus;		/* Unused */
    sh_int		damnodice;		/* Unused */
    sh_int		damsizedice;		/* Unused */
    sh_int		damplus;		/* Unused */
    int			gold;			/* Unused */
};



/*
 * One character (PC or NPC).
 */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		next_fight;
    CHAR_DATA *		prev_fight;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		mount;
    CHAR_DATA *		wizard;
    CHAR_DATA *		embrace;
    OBJ_DATA *		sat;
    SPEC_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    char *		hunting;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *		lord;
    char *		clan;
    char *		side;
    char *		morph;
    char *		createtime;
    char *		lasttime;
    char *		lasthost;
    char *		poweraction;
    char *		powertype;
    char *		pload;
    char *		prompt;
    char *		cprompt;
    sh_int		sex;
    sh_int		race;
    sh_int		class;
    int			immune;
    int			polyaff;
    int			vampaff;
    int			itemaffect;
    int			vamppass;
    int			form;
    int			body;
    sh_int		vampgen;
    sh_int		spectype;
    sh_int		specpower;
    sh_int		loc_hp	[7];
    sh_int		wpn	[13];
    sh_int		spl	[5];
    sh_int		cmbt	[8];
    sh_int		stance	[16];
    sh_int		blood[3];
    sh_int		beast;
    sh_int		truebeast;
    sh_int		mounted;
    sh_int		embraced;
    sh_int		home;
    sh_int		init;
    sh_int		level;
    sh_int		trust;
    int			played;
    time_t		logon;
    time_t		save_time;
    sh_int		timer;
    sh_int		wait;
    int			pkill;
    int			pdeath;
    int			mkill;
    int			mdeath;
    sh_int		agg;
    int			hit;
    int			max_hit;
    int			mana;
    int			max_mana;
    int			move;
    int			max_move;
    int			gold;
    int			exp;
    int			act;
    int			extra;
    int			more;
    int			affected_by;
    sh_int		position;
    sh_int		practice;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		saving_throw;
    sh_int		alignment;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		armor;
    sh_int		wimpy;
    sh_int		deaf;
    sh_int		paradox		[3];
    sh_int		damcap		[2];
};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    CHAR_DATA *		familiar;
    CHAR_DATA *		partner;
    CHAR_DATA *		propose;
    OBJ_DATA *		chobj;
    OBJ_DATA *		memorised;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		title;
    char *		conception;
    char *		parents;
    char *		cparents;
    char *		marriage;
    char *		email;
    char *		conding;
    char *		love;
    char *		coterie;
    char *		cot_time;
    char *		cot_clans;
    char *		cot_sides;
    char *		leader;
    char *		stancename;
    char *		stancelist;
    char *		stancework;
    int			stancemove[5];
    sh_int		perm_str;
    sh_int		perm_int;
    sh_int		perm_wis;
    sh_int		perm_dex;
    sh_int		perm_con;
    sh_int		mod_str;
    sh_int		mod_int;
    sh_int		mod_wis;
    sh_int		mod_dex;
    sh_int		mod_con;
    int			denydate;
    int			quest;
    sh_int		wolf;
    sh_int		rank;
    sh_int		regenerate;
    sh_int		demonic;
    sh_int		atm;
    sh_int		song;
    int			necromancy;
    int			obeah;
    int			darkness;
    int			temporis;
    int			reina;
    int			silence;
    int			bodymods;
    int			bpoints;
    int			dpoints;
    int			gpoints;
    int			rpoints;
    int			wpoints;
    int			spirit;
    int			conjure;
    int			celerity;
    int			firstaid;
    int			repair;
    int			teach;
    int			learn;
    int			evileye;
    sh_int		notes;
    sh_int		followers;
    int			gnosis[2];
    int			absorb[6];
    int			qstats[5];
    int			language[2];
    sh_int		stage[3];
    sh_int		status[3];
    sh_int		wolfform[2];
    int			score[6];
    sh_int		runes[4];
    sh_int		disc[11];
    int			rituals[10];
    sh_int		cland[3];
    int			diableries[2];
    sh_int		resist[5];
    sh_int		breed[3];
    sh_int		auspice[5];
    sh_int		tribes[20];
    sh_int		powers[20];
    sh_int		powers_mod[20];
    sh_int		powers_set[20];
    int			powers_set_wear;
    int			power[2];
    int			genes[10];
    int			cot_min[12];
    sh_int		willpower[2];
    sh_int		fake_skill;
    sh_int		fake_stance;
    sh_int		fake_hit;
    sh_int		fake_dam;
    int			fake_hp;
    int			fake_mana;
    int			fake_move;
    int			fake_max_hp;
    int			fake_max_mana;
    int			fake_max_move;
    int			fake_ac;
    int			fake_stats	[5];
    sh_int		obj_vnum;
    sh_int		majesty;
    sh_int		condition	[3];
    sh_int		learned		[MAX_SKILL];
    long		imc_deaf;    /* IMC channel def flags */
    long		imc_allow;   /* IMC channel allow flags */
    long		imc_deny;    /* IMC channel deny flags */
    char *		rreply;      /* IMC reply-to */
    char *		rreply_name; /* IMC reply-to shown to char */
    char *		ice_listen;
    sh_int		exhaustion;
    sh_int		exercise	[4];
    sh_int		lie;
    sh_int		class;
    int                 security;	/* OLC */
};



/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[3];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    AREA_DATA *         area;	/* OLC */
    char *		name;
    char *		short_descr;
    char *		description;
    char *		chpoweron;
    char *		chpoweroff;
    char *		chpoweruse;
    char *		victpoweron;
    char *		victpoweroff;
    char *		victpoweruse;
    char *		actcommands;
    char *		questmaker;
    char *		questowner;
    sh_int		vnum;
    sh_int		item_type;
    sh_int		extra_flags;
    sh_int		wear_flags;
    sh_int		count;
    sh_int		weight;
    int			spectype;
    int			specpower;
    sh_int		condition;
    sh_int		toughness;
    sh_int		resistance;
    int			quest;
    sh_int		points;
    int			cost;			/* Unused */
    int			value	[4];
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    CHAR_DATA *		chobj;
    CHAR_DATA *		sat;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	to_room;
    char *		name;
    char *		short_descr;
    char *		description;
    char *		chpoweron;
    char *		chpoweroff;
    char *		chpoweruse;
    char *		victpoweron;
    char *		victpoweroff;
    char *		victpoweruse;
    char *		actcommands;
    char *		questmaker;
    char *		questowner;
    sh_int		item_type;
    ush_int		extra_flags;
    sh_int		wear_flags;
    sh_int		wear_loc;
    sh_int		weight;
    int			spectype;
    int			specpower;
    sh_int		condition;
    sh_int		toughness;
    sh_int		resistance;
    int			quest;
    sh_int		points;
    int			cost;
    sh_int		level;
    sh_int		timer;
    int			value	[4];
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *         next;	/* OLC */
    sh_int		vnum;
    sh_int		exit_info;
    sh_int		key;
    char *		keyword;
    char *		description;
    int			rs_flags;	/* OLC */
};



/*
 * Room text checking data.
 */
typedef struct roomtext_data
{
    int				type;
    int				power;
    int				mob;
    char *			input;
    char *			output;
    char *			choutput;
    char *			name;
    struct roomtext_data	*next;
} ROOMTEXT_DATA;



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    sh_int		arg1;
    sh_int		arg2;
    sh_int		arg3;
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    char *		name;
    sh_int		age;
    sh_int		nplayer;
    char *		filename;	/* OLC */
    char *		builders;	/* OLC */ /* Listing of */
    int			security;	/* OLC */ /* Value 1-9  */
    int			lvnum;		/* OLC */ /* Lower vnum */
    int			uvnum;		/* OLC */ /* Upper vnum */
    int			vnum;		/* OLC */ /* Area vnum  */
    int			area_flags;	/* OLC */
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    OBJ_DATA *		to_obj;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	  [6];
    ROOMTEXT_DATA *	roomtext;
    char *		track     [5];
    char *		name;
    char *		description;
    sh_int		vnum;
    sh_int		room_flags;
    int			added_flags;
    sh_int		light;
    sh_int		blood;
    sh_int		passed;
    sh_int		track_dir [5];
    sh_int		sector_type;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000
#define TYPE_ADD_AGG                 3000
#define TYPE_AGG                     3000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4

#define PURPLE_MAGIC		    0
#define RED_MAGIC		    1
#define BLUE_MAGIC		    2
#define GREEN_MAGIC		    3
#define YELLOW_MAGIC		    4



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    sh_int	skill_level;	        /* Level needed by class	*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
    sh_int	slot;			/* Slot for #OBJECT loading	*/
    sh_int	min_mana;		/* Minimum mana used		*/
    sh_int	beats;			/* Waiting time after use	*/
    char *	noun_damage;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
};



/*
 * These are skill_lookup return values for common skills and spells.
 */
extern	sh_int	gsn_backstab;
extern	sh_int	gsn_hide;
extern	sh_int	gsn_peek;
extern	sh_int	gsn_pick_lock;
extern	sh_int	gsn_sneak;
extern	sh_int	gsn_steal;

extern	sh_int	gsn_fastdraw;
extern	sh_int	gsn_berserk;
extern	sh_int	gsn_punch;
extern	sh_int	gsn_elbow;
extern	sh_int	gsn_headbutt;
extern	sh_int	gsn_sweep;
extern	sh_int	gsn_knee;
extern	sh_int	gsn_tongue;
extern	sh_int	gsn_tail;
extern	sh_int	gsn_spines;
extern	sh_int	gsn_disarm;
extern	sh_int	gsn_hurl;
extern	sh_int	gsn_kick;
extern	sh_int	gsn_rescue;
extern	sh_int	gsn_track;
extern	sh_int	gsn_polymorph;
extern	sh_int	gsn_web;
extern	sh_int	gsn_potency;
extern	sh_int	gsn_demon;
extern	sh_int	gsn_firstaid;
extern	sh_int	gsn_repair;
extern	sh_int	gsn_spit;
extern	sh_int	gsn_tendrils;
extern	sh_int	gsn_rot;
extern	sh_int	gsn_jail;

extern	sh_int	gsn_blindness;
extern	sh_int	gsn_charm_person;
extern	sh_int	gsn_curse;
extern	sh_int	gsn_invis;
extern	sh_int	gsn_mass_invis;
extern	sh_int	gsn_poison;
extern	sh_int	gsn_sleep;
extern	sh_int	gsn_fear;

extern	sh_int	gsn_dark_poison;
extern	sh_int	gsn_paradox;
extern	sh_int	gsn_darkness;


/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))	/* OLC */
#define STR(dat, field)         (( (dat)->field != NULL                    \
                                             ? (dat)->field                \
                                             : (dat)->pIndexData->field ))	/* OLC */



/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_JUDGE(ch)		(get_trust(ch) >= LEVEL_JUDGE)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define CAN_SIT(ch)		((ch)->position >= 4 && (ch)->position <= 7)
#define IS_MAGE(ch)		((ch)->level >= 4 && (ch)->level <= 6)
#define CAN_PK(ch)		(get_trust(ch) >= 3 && get_trust(ch) <= 6)
#define IS_NEWBIE(ch)		((ch)->level <= 2)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_SPEAKING(ch, sn)	(IS_SET((ch)->pcdata->language[0], (sn)))
#define CAN_SPEAK(ch, sn)	(IS_SET((ch)->pcdata->language[1], (sn)))
#define IS_ITEMAFF(ch, sn)	(IS_SET((ch)->itemaffect, (sn)))
#define IS_IMMUNE(ch, sn)	(IS_SET((ch)->immune, (sn)))
#define IS_WILLPOWER(ch, sn)	(IS_SET((ch)->pcdata->resist[0], (sn)))
#define IS_VAMPAFF(ch, sn)	(IS_SET((ch)->vampaff, (sn)))
#define IS_VAMPPASS(ch, sn)	(IS_SET((ch)->vamppass, (sn)))
#define IS_FORM(ch, sn)		(IS_SET((ch)->body, (sn)))
#define IS_POLYAFF(ch, sn)	(IS_SET((ch)->polyaff, (sn)))
#define IS_EXTRA(ch, sn)	(IS_SET((ch)->extra, (sn)))
#define IS_MORE(ch, sn)		(IS_SET((ch)->more, (sn)))
#define IS_MOD(ch, sn)		(IS_SET((ch)->pcdata->bodymods, (sn)))
#define IS_STANCE(ch, sn)	(ch->stance[0] == sn)
#define IS_DEMPOWER(ch, sn)	(IS_SET((ch)->pcdata->disc[C_POWERS], (sn)))
#define IS_DEMAFF(ch, sn)	(IS_SET((ch)->pcdata->disc[C_CURRENT], (sn)))

#define IS_VAMPIRE(ch)		(IS_SET((ch)->class, CLASS_VAMPIRE))
#define IS_GHOUL(ch)		((ch)->class == CLASS_GHOUL)
#define IS_WEREWOLF(ch)		(IS_SET((ch)->class, CLASS_WEREWOLF))
#define IS_DEMON(ch)		(IS_SET((ch)->class, CLASS_DEMON))
#define IS_ANGEL(ch)		((ch)->class == CLASS_ANGEL)
#define IS_HIGHLANDER(ch)	((ch)->class == CLASS_HIGHLANDER)
#define IS_SPIRIT(ch)		(IS_SET((ch)->more, MORE_SPIRIT))
#define IS_ABOMINATION(ch)	((ch)->class == CLASS_ABOMINATION)
#define IS_LICH(ch)		((ch)->class == CLASS_LICH)
#define IS_SKINDANCER(ch)	((ch)->class == CLASS_SKINDANCER)
#define IS_BAALI(ch)		((ch)->class == CLASS_BAALI)
#define IS_BLACK_SPIRAL_DANCER(ch) ((ch)->class == CLASS_BLACK_SPIRAL_DANCER)
#define IS_NEPHANDI(ch)		((ch)->class == CLASS_NEPHANDI)

#define IS_HEAD(ch, sn)		(IS_SET((ch)->loc_hp[0], (sn)))
#define IS_BODY(ch, sn)		(IS_SET((ch)->loc_hp[1], (sn)))
#define IS_ARM_L(ch, sn)	(IS_SET((ch)->loc_hp[2], (sn)))
#define IS_ARM_R(ch, sn)	(IS_SET((ch)->loc_hp[3], (sn)))
#define IS_LEG_L(ch, sn)	(IS_SET((ch)->loc_hp[4], (sn)))
#define IS_LEG_R(ch, sn)	(IS_SET((ch)->loc_hp[5], (sn)))
#define IS_BLEEDING(ch, sn)	(IS_SET((ch)->loc_hp[6], (sn)))

#define IN_CLAN( ch )           (IS_NPC(ch)?0:ch->clan)
#define IS_PLAYING( d )         (d->connected==CON_PLAYING)
 
#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define PC(ch,field)            ((ch)->pcdata->field)	/* OLC */
#define GET_PC(ch,field,nopc)   ((ch->pcdata != NULL ? ch->pcdata->field    \
                                                     : (nopc)) )	/* OLC */
#define GET_AC(ch)		((ch)->armor				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))



/*
 * Object Macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: ( IS_AFFECTED( (ch), AFF_POLYMORPH) ?   \
				(ch)->morph : (ch)->name ) )		\
				: "someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
};



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    char * const	name;
    char * const	char_no_arg;
    char * const	others_no_arg;
    char * const	char_found;
    char * const	others_found;
    char * const	vict_found;
    char * const	char_auto;
    char * const	others_auto;
};


/*
 * Structure for an N-social in the nsocials table.
 */
struct	nsocial_type
{
    char * const	name;
    char * const	char_no_arg;
    char * const	others_no_arg;
    char * const	char_found;
    char * const	others_found;
    char * const	vict_found;
    char * const	char_auto;
    char * const	others_auto;
};


/*
 * Structure for an X-social in the xsocials table.
 */
struct	xsocial_type
{
    char * const	name;
    char * const	char_no_arg;
    char * const	others_no_arg;
    char * const	char_found;
    char * const	others_found;
    char * const	vict_found;
    char * const	char_auto;
    char * const	others_auto;
    sh_int		gender;
    sh_int		stage;
    sh_int		position;
    sh_int		self;
    sh_int		other;
    bool		chance;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];

extern	const	struct	cmd_type	cmd_table	[];
extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern		struct	clan_table_type	clan_table	[MAX_CLAN];
extern		struct	artifact_type	artifact_table	[MAX_ART];
extern	const	struct	social_type	social_table	[];
extern	const	struct	nsocial_type	nsocial_table	[];
extern	const	struct	xsocial_type	xsocial_table	[];
extern	char *	const	dir_name        [];	/* OLC */
extern	const	int	rev_dir         [];	/* OLC */



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		ROOMTEXT_DATA     *	roomtext_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;



/*
 * Extra global's by KaVir.
 */
extern		CHAR_DATA	  *	first_fight;
extern		CHAR_DATA	  *	last_fight;
extern		char			first_place	[];
extern		char			second_place	[];
extern		char			third_place	[];
extern		int			score_1;
extern		int			score_2;
extern		int			score_3;
extern		int			world_affects;



/*
 * online creation externals  OLC
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern  	SHOP_DATA *             shop_last;

extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern          int                     top_obj_index;
extern          int                     top_reset;
extern          int                     top_room;
extern          int                     top_shop;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

extern          char                    str_empty       [1];

extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];



/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_accept	);
DECLARE_DO_FUN(	do_activate	);
DECLARE_DO_FUN(	do_aeolus	);
DECLARE_DO_FUN(	do_affects	);
DECLARE_DO_FUN(	do_alignment	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_anesthetic	);
DECLARE_DO_FUN(	do_angelarmour	);
DECLARE_DO_FUN(	do_angelform	);
DECLARE_DO_FUN(	do_anger	);
DECLARE_DO_FUN(	do_animal	);
DECLARE_DO_FUN(	do_ansi		);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_arms		);
DECLARE_DO_FUN(	do_artifact	);
DECLARE_DO_FUN(	do_ashes	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN(	do_autoexit	);
DECLARE_DO_FUN(	do_autoloot	);
DECLARE_DO_FUN(	do_autosac	);
DECLARE_DO_FUN(	do_autosave	);
DECLARE_DO_FUN(	do_awe		);
DECLARE_DO_FUN(	do_baal		);
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_beastlike	);
DECLARE_DO_FUN(	do_beastwithin	);
DECLARE_DO_FUN(	do_beckon	);
DECLARE_DO_FUN(	do_berserk	);
DECLARE_DO_FUN(	do_bind		);
DECLARE_DO_FUN(	do_birth	);
DECLARE_DO_FUN(	do_bite		);
DECLARE_DO_FUN(	do_blank	);
DECLARE_DO_FUN(	do_blindfold	);
DECLARE_DO_FUN(	do_bloodagony	);
DECLARE_DO_FUN(	do_bloodline	);
DECLARE_DO_FUN(	do_bloodrage	);
DECLARE_DO_FUN(	do_blur		);
DECLARE_DO_FUN(	do_bodywrack	);
DECLARE_DO_FUN(	do_bonecraft	);
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN(	do_breakup	);
DECLARE_DO_FUN(	do_breath	);
DECLARE_DO_FUN(	do_brief	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buildtalk	);
DECLARE_DO_FUN(	do_burrow	);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_call		);
DECLARE_DO_FUN(	do_calm		);
DECLARE_DO_FUN(	do_camouflage	);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN(	do_cauldronblood);
DECLARE_DO_FUN(	do_celerity	);
DECLARE_DO_FUN(	do_champions	);
DECLARE_DO_FUN(	do_change	);
DECLARE_DO_FUN(	do_changelight	);
DECLARE_DO_FUN(	do_changeling	);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chant	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_cherub	);
DECLARE_DO_FUN(	do_claim	);
DECLARE_DO_FUN(	do_clandisc	);
DECLARE_DO_FUN(	do_clanname	);
DECLARE_DO_FUN(	do_clap		);
DECLARE_DO_FUN(	do_claw		);
DECLARE_DO_FUN(	do_claws	);
DECLARE_DO_FUN(	do_clearclass	);
DECLARE_DO_FUN(	do_clearclas	);
DECLARE_DO_FUN(	do_clearstats	);
DECLARE_DO_FUN(	do_clist	);
DECLARE_DO_FUN(	do_cloneobject	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN(	do_command	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_complete	);
DECLARE_DO_FUN(	do_conditioning	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consent	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_control	);
DECLARE_DO_FUN(	do_controlbeast	);
DECLARE_DO_FUN(	do_coterie	);
DECLARE_DO_FUN(	do_courage	);
DECLARE_DO_FUN(	do_cprompt	);
DECLARE_DO_FUN(	do_crack	);
DECLARE_DO_FUN(	do_create	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN(	do_ctalk	);
DECLARE_DO_FUN(	do_curse	);
DECLARE_DO_FUN(	do_darkheart	);
DECLARE_DO_FUN(	do_dazzle	);
DECLARE_DO_FUN(	do_decapitate	);
DECLARE_DO_FUN(	do_demonarmour	);
DECLARE_DO_FUN(	do_demonform	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_diagnose	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_discipline	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_dismount	);
DECLARE_DO_FUN(	do_disquiet	);
DECLARE_DO_FUN(	do_divorce	);
DECLARE_DO_FUN(	do_donate	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drainlife	);
DECLARE_DO_FUN(	do_draw		);
DECLARE_DO_FUN(	do_dreadgaze	);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_guardian	);
DECLARE_DO_FUN(	do_earthmeld	);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_eclipse	);
DECLARE_DO_FUN(	do_empty	);
DECLARE_DO_FUN(	do_email	);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_engulf	);
DECLARE_DO_FUN(	do_enter	);
DECLARE_DO_FUN(	do_embrace	);
DECLARE_DO_FUN(	do_entrancement	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_escape	);
DECLARE_DO_FUN(	do_evileye	);
DECLARE_DO_FUN(	do_eyespy	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_exp		);
DECLARE_DO_FUN(	do_familiar	);
DECLARE_DO_FUN(	do_fangs	);
DECLARE_DO_FUN(	do_fatality	);
DECLARE_DO_FUN(	do_favour	);
DECLARE_DO_FUN(	do_fcommand	);
DECLARE_DO_FUN(	do_fear		);
DECLARE_DO_FUN(	do_feed		);
DECLARE_DO_FUN(	do_fetishdoll	);
DECLARE_DO_FUN(	do_fightstyle	);
DECLARE_DO_FUN(	do_fileupdate	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN(	do_finger	);
DECLARE_DO_FUN(	do_firstaid	);
DECLARE_DO_FUN(	do_flamebolt	);
DECLARE_DO_FUN(	do_flames	);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_fleshbond	);
DECLARE_DO_FUN(	do_fleshcraft	);
DECLARE_DO_FUN(	do_flex		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_forceauto	);
DECLARE_DO_FUN(	do_formillusion	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_frenzy	);
DECLARE_DO_FUN(	do_gag		);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_gift		);
DECLARE_DO_FUN(	do_gifts	);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_godless	);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN(	do_hagswrinkles	);
DECLARE_DO_FUN(	do_hand		);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_halosun	);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_home		);
DECLARE_DO_FUN(	do_hooves	);
DECLARE_DO_FUN(	do_horns	);
DECLARE_DO_FUN(	do_howl		);
DECLARE_DO_FUN(	do_huh		);
DECLARE_DO_FUN(	do_humanform	);
DECLARE_DO_FUN(	do_humanity	);
DECLARE_DO_FUN(	do_hunt		);
DECLARE_DO_FUN(	do_hurl		);
DECLARE_DO_FUN(	do_icy		);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_immune	);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_imp		);
DECLARE_DO_FUN(	do_improve	);
DECLARE_DO_FUN(	do_inferno	);
DECLARE_DO_FUN(	do_infirmity	);
DECLARE_DO_FUN(	do_info		);
DECLARE_DO_FUN(	do_inform	);
DECLARE_DO_FUN(	do_inpart	);
DECLARE_DO_FUN(	do_introduce	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_invisibility	);
DECLARE_DO_FUN(	do_jail		);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_lamprey	);
DECLARE_DO_FUN(	do_leap		);
DECLARE_DO_FUN(	do_lick		);
DECLARE_DO_FUN(	do_lifespan	);
DECLARE_DO_FUN(	do_lightning	);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_locate	);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_login	);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_love		);
DECLARE_DO_FUN(	do_loyalty	);
DECLARE_DO_FUN(	do_majesty	);
DECLARE_DO_FUN(	do_marry	);
DECLARE_DO_FUN(	do_mask		);
DECLARE_DO_FUN(	do_masssummons	);
DECLARE_DO_FUN(	do_mclear	);
DECLARE_DO_FUN(	do_meditate	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mindblast	);
DECLARE_DO_FUN(	do_mirrorimage	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_mount	);
DECLARE_DO_FUN(	do_morph	);
DECLARE_DO_FUN(	do_mortalvamp	);
DECLARE_DO_FUN(	do_mother	);
DECLARE_DO_FUN(	do_move		);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_mummify	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_neutral	);
DECLARE_DO_FUN(	do_newbie	);
DECLARE_DO_FUN(	do_nice		);
DECLARE_DO_FUN(	do_nightsight	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_nsocials	);
DECLARE_DO_FUN(	do_obeah	);
DECLARE_DO_FUN(	do_obedience	);
DECLARE_DO_FUN(	do_oclone	);
DECLARE_DO_FUN(	do_offersoul	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_omnipotence	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oreturn	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN(	do_oswitch	);
DECLARE_DO_FUN(	do_otransfer	);
DECLARE_DO_FUN(	do_pact		);
DECLARE_DO_FUN(	do_panacea	);
DECLARE_DO_FUN(	do_paralyzing	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_pchat	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_peek		);
DECLARE_DO_FUN(	do_persuasion	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_pigeon	);
DECLARE_DO_FUN(	do_plasmaform	);
DECLARE_DO_FUN(	do_pload	);
DECLARE_DO_FUN(	do_porcupine	);
DECLARE_DO_FUN(	do_portal	);
DECLARE_DO_FUN(	do_possession	);
DECLARE_DO_FUN(	do_potency	);
DECLARE_DO_FUN(	do_pour		);
DECLARE_DO_FUN(	do_powers	);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN(	do_pray		);
DECLARE_DO_FUN(	do_preservation	);
DECLARE_DO_FUN(	do_preturn	);
DECLARE_DO_FUN(	do_projection	);
DECLARE_DO_FUN(	do_prompt	);
DECLARE_DO_FUN(	do_propose	);
DECLARE_DO_FUN(	do_psay		);
DECLARE_DO_FUN(	do_pset		);
DECLARE_DO_FUN(	do_ptalk	);
DECLARE_DO_FUN(	do_punch	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_putrefaction	);
DECLARE_DO_FUN(	do_qmake	);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_qset		);
DECLARE_DO_FUN(	do_qstat	);
DECLARE_DO_FUN(	do_qtrust	);
DECLARE_DO_FUN(	do_quest	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_rage		);
DECLARE_DO_FUN(	do_rain		);
DECLARE_DO_FUN(	do_read		);
DECLARE_DO_FUN(	do_readaura	);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recharge	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_regenerate	);
DECLARE_DO_FUN(	do_reina	);
DECLARE_DO_FUN(	do_release	);
DECLARE_DO_FUN(	do_relevel	);
DECLARE_DO_FUN(	do_reload	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rename	);
DECLARE_DO_FUN(	do_renew	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_repair	);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_reshape	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_roll		);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_safe		);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_scream	);
DECLARE_DO_FUN(	do_scry		);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN(	do_serenity	);
DECLARE_DO_FUN(	do_serpent	);
DECLARE_DO_FUN(	do_servant	);
DECLARE_DO_FUN(	do_setstance	);
DECLARE_DO_FUN(	do_shadowbody	);
DECLARE_DO_FUN(	do_shadowplane	);
DECLARE_DO_FUN(	do_shadowsight	);
DECLARE_DO_FUN(	do_shadowstep	);
DECLARE_DO_FUN(	do_sharespirit	);
DECLARE_DO_FUN(	do_sheath	);
DECLARE_DO_FUN(	do_shell	);
DECLARE_DO_FUN(	do_shield	);
DECLARE_DO_FUN(	do_shift	);
DECLARE_DO_FUN(	do_shoot	);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_show_clan_info);
DECLARE_DO_FUN(	do_shroudnight	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_show_artifacts);
DECLARE_DO_FUN(	do_side		);
DECLARE_DO_FUN(	do_scentbeyond	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_silencedeath	);
DECLARE_DO_FUN(	do_sing		);
DECLARE_DO_FUN(	do_sit		);
DECLARE_DO_FUN(	do_skill	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slit		);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_soulmask	);
DECLARE_DO_FUN(	do_soultwist	);
DECLARE_DO_FUN(	do_speak	);
DECLARE_DO_FUN(	do_special	);
DECLARE_DO_FUN(	do_spell	);
DECLARE_DO_FUN(	do_spiriteyes	);
DECLARE_DO_FUN(	do_spiritform	);
DECLARE_DO_FUN(	do_spiritgate	);
DECLARE_DO_FUN(	do_spiritguardian);
DECLARE_DO_FUN(	do_spit		);
DECLARE_DO_FUN(	do_stake	);
DECLARE_DO_FUN(	do_stance	);
DECLARE_DO_FUN(	do_staredown	);
DECLARE_DO_FUN(	do_status	);
DECLARE_DO_FUN(	do_smother	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_spy		);
DECLARE_DO_FUN(	do_spydirection	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_succulence	);
DECLARE_DO_FUN(	do_summon	);
DECLARE_DO_FUN(	do_summoning	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tail		);
DECLARE_DO_FUN(	do_tasteblood	);
DECLARE_DO_FUN(	do_teach	);
DECLARE_DO_FUN(	do_teachdisc	);
DECLARE_DO_FUN(	do_teachgift	);
DECLARE_DO_FUN(	do_tear		);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_thanatopsis	);
DECLARE_DO_FUN(	do_theftvitae	);
DECLARE_DO_FUN(	do_throw	);
DECLARE_DO_FUN(	do_tie		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_token	);
DECLARE_DO_FUN(	do_tongue	);
DECLARE_DO_FUN(	do_totalscore	);
DECLARE_DO_FUN(	do_totems	);
DECLARE_DO_FUN(	do_track	);
DECLARE_DO_FUN(	do_tradition	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN(	do_transport	);
DECLARE_DO_FUN(	do_travel	);
DECLARE_DO_FUN(	do_tribe	);
DECLARE_DO_FUN(	do_tribegifts	);
DECLARE_DO_FUN(	do_truedisguise	);
DECLARE_DO_FUN(	do_truefear	);
DECLARE_DO_FUN(	do_truesight	);
DECLARE_DO_FUN(	do_turn		);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unburden	);
DECLARE_DO_FUN(	do_unload	);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_unpolymorph	);
DECLARE_DO_FUN(	do_untie	);
DECLARE_DO_FUN(	do_unveil	);
DECLARE_DO_FUN(	do_unwerewolf	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_upkeep	);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_vampire	);
DECLARE_DO_FUN(	do_vamptalk	);
DECLARE_DO_FUN(	do_vanish	);
DECLARE_DO_FUN(	do_vclan	);
DECLARE_DO_FUN(	do_version	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_voodoo	);
DECLARE_DO_FUN(	do_weakness	);
DECLARE_DO_FUN(	do_web		);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wall		);
DECLARE_DO_FUN(	do_wallflames	);
DECLARE_DO_FUN(	do_wallgranite	);
DECLARE_DO_FUN(	do_watcher	);
DECLARE_DO_FUN(	do_watching	);
DECLARE_DO_FUN(	do_weaponform	);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_wearaffect	);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_werewolf	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_whisper	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_willpower	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wings	);
DECLARE_DO_FUN(	do_withering	);
DECLARE_DO_FUN(	do_witherlimb	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlist	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN(	do_write	);
DECLARE_DO_FUN(	do_wwpowers	);
DECLARE_DO_FUN(	do_wyld		);
DECLARE_DO_FUN(	do_xemote	);
DECLARE_DO_FUN(	do_xsocials	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN(	do_zombie	);
DECLARE_DO_FUN(	do_zuloform	);


/*
 * Online creation commands  OLC
 */
DECLARE_DO_FUN( do_ashow        );
DECLARE_DO_FUN( do_rshow        );
DECLARE_DO_FUN( do_oshow        );
DECLARE_DO_FUN( do_mshow        );

DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );

DECLARE_DO_FUN( do_resets       );

DECLARE_DO_FUN( do_asave        );

DECLARE_DO_FUN( do_arealist     );


/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_fear		);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);

DECLARE_SPELL_FUN(	spell_guardian		);
DECLARE_SPELL_FUN(	spell_soulblade		);
DECLARE_SPELL_FUN(	spell_mana		);
DECLARE_SPELL_FUN(	spell_frenzy		);
DECLARE_SPELL_FUN(	spell_darkblessing	);
DECLARE_SPELL_FUN(	spell_portal		);
DECLARE_SPELL_FUN(	spell_energyflux	);
DECLARE_SPELL_FUN(	spell_voodoo		);
DECLARE_SPELL_FUN(	spell_transport		);
DECLARE_SPELL_FUN(	spell_regenerate	);
DECLARE_SPELL_FUN(	spell_clot		);
DECLARE_SPELL_FUN(	spell_mend		);
DECLARE_SPELL_FUN(	spell_quest		);
DECLARE_SPELL_FUN(	spell_minor_creation	);
DECLARE_SPELL_FUN(	spell_brew		);
DECLARE_SPELL_FUN(	spell_scribe		);
DECLARE_SPELL_FUN(	spell_carve		);
DECLARE_SPELL_FUN(	spell_engrave		);
DECLARE_SPELL_FUN(	spell_bake		);
DECLARE_SPELL_FUN(	spell_mount		);
DECLARE_SPELL_FUN(	spell_scan		);
DECLARE_SPELL_FUN(	spell_fix		);
DECLARE_SPELL_FUN(	spell_spellproof	);
DECLARE_SPELL_FUN(	spell_preserve		);
DECLARE_SPELL_FUN(	spell_major_creation	);
DECLARE_SPELL_FUN(	spell_copy		);
DECLARE_SPELL_FUN(	spell_insert_page	);
DECLARE_SPELL_FUN(	spell_chaos_blast	);
DECLARE_SPELL_FUN(	spell_resistance	);
DECLARE_SPELL_FUN(	spell_web		);
DECLARE_SPELL_FUN(	spell_polymorph		);
DECLARE_SPELL_FUN(	spell_contraception	);
DECLARE_SPELL_FUN(	spell_remove_page	);
DECLARE_SPELL_FUN(	spell_find_familiar	);
DECLARE_SPELL_FUN(	spell_improve		);
DECLARE_SPELL_FUN(	spell_tendrils		);
DECLARE_SPELL_FUN(	spell_jail		);

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(apollo)
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(hpux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(macintosh)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
#if 	defined(SYSV)
size_t 	fread		args( ( void *ptr, size_t size, size_t n, 
				FILE *stream ) );
#else
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt		args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""		/* Player files			*/
#define NULL_FILE	"proto.are"	/* To reserve one stream	*/
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""		/* Player files                 */
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#endif

#if defined(unix)
#define PLAYER_DIR	"../player/"	/* Player files			*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#endif

#define AREA_LIST	"area.lst"	/* List of areas		*/
#define CLAN_LIST	"clan1.txt"	/* List of clans		*/
#define ART_LIST	"art1.txt"	/* List of artifacts            */
#define HOME_AREA	"../area/homes.are" /* Mages towers, etc        */

#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/



/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	room_text	args( ( CHAR_DATA *ch, char *argument ) );
char    *strlower       args( ( char * ip ) );
void	excessive_cpu	args( ( int blx ) );
bool	check_parse_name args( ( char *name ) );
bool	is_in		args( (char *, char *) );
bool	all_in		args( (char *, char *) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
int	char_hitroll	args( ( CHAR_DATA *ch ) );
int	char_damroll	args( ( CHAR_DATA *ch ) );
int	char_ac		args( ( CHAR_DATA *ch ) );
int	char_str	args( ( CHAR_DATA *ch ) );
int	char_dex	args( ( CHAR_DATA *ch ) );
int	char_sta	args( ( CHAR_DATA *ch ) );
int	char_cha	args( ( CHAR_DATA *ch ) );
int	char_int	args( ( CHAR_DATA *ch ) );
int	char_per	args( ( CHAR_DATA *ch ) );
int	stat_cost	args( ( CHAR_DATA *ch, int whichstat ) );
int	get_celerity	args( ( CHAR_DATA *ch ) );
int	get_disc	args( ( CHAR_DATA *ch, int disc ) );
int	get_truedisc	args( ( CHAR_DATA *ch, int disc ) );
int	get_breed	args( ( CHAR_DATA *ch, int disc ) );
int	get_auspice	args( ( CHAR_DATA *ch, int disc ) );
int	get_tribe	args( ( CHAR_DATA *ch, int disc ) );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );
void	open_lift	args( ( CHAR_DATA *ch ) );
void	close_lift	args( ( CHAR_DATA *ch ) );
void	move_lift	args( ( CHAR_DATA *ch, int to_room ) );
void	move_door	args( ( CHAR_DATA *ch ) );
void	thru_door	args( ( CHAR_DATA *ch, int doorexit ) );
void	open_door	args( ( CHAR_DATA *ch, bool be_open ) );
bool	is_open		args( ( CHAR_DATA *ch ) );
bool	same_floor	args( ( CHAR_DATA *ch, int cmp_room ) );
void	check_hunt	args( ( CHAR_DATA *ch ) );

/* act_obj.c */
bool	is_ok_to_wear	args( ( CHAR_DATA *ch, bool wolf_ok, char *argument ) );
void	quest_object	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
bool	fifty_objects	args( ( CHAR_DATA *ch ) );
bool	hundred_objects	args( ( CHAR_DATA *ch, OBJ_DATA *container ) );
void	set_disc	args( ( CHAR_DATA *ch ) );
void	strip_disc	args( ( CHAR_DATA *ch ) );
void	mortal_vamp	args( ( CHAR_DATA *ch ) );

/* act_wiz.c */
void	bind_char	args( ( CHAR_DATA *ch ) );
void	logchan		args( ( char *argument ) );
void	sysmes		args( ( char *argument ) );
ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg );	/* OLC */

/* comm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	close_socket2	args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
			    int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_all_char args( ( const char *text ) );
void	act		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );
void	act2		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );
void	kavitem		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );

/* db.c */
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
char *	get_roomtext	args( ( const char *name, ROOMTEXT_DATA    *rt ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
char *	str_dup		args( ( const char *str ) );
void	free_string	args( ( char *pstr ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
char *	upper_cap	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );
void	reset_area      args( ( AREA_DATA * pArea ) );	/* OLC */

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	adv_damage	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
bool	no_attack	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	auto_safe	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	hurt_person	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				bool agg_dam ) );
void	killperson	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	death_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	dambonus	args( ( CHAR_DATA *ch, CHAR_DATA *victim, 
			    int dam, int stance) );
void	demon_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_hours	args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
bool	is_abbr		args( ( const char *str, char *namelist ) );
int	loc_name	args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char	args( ( CHAR_DATA *ch ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world2	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_finger	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_in_obj	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_room	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world2	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int amount ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
void	affect_modify	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
int	str_close	args( ( char *a, char *b ) );
void	mage_format	args( ( char *arg ) );

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
void	stage_update	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int stage ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
int	slot_lookup	args( ( int slot ) );
int	sn_lookup	args( ( int sn ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );
void	enhance_stat	args( ( int sn, int level, CHAR_DATA *ch, 
				    CHAR_DATA *victim, int apply_bit,
				    int bonuses, int affect_bit ) );

/* save.c */
void	save_char_obj		args( ( CHAR_DATA *ch ) );
void	save_char_obj_backup	args( ( CHAR_DATA *ch ) );
bool	load_char_obj		args( ( DESCRIPTOR_DATA *d, char *name ) );
bool	load_char_short		args( ( DESCRIPTOR_DATA *d, char *name ) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );

/* update.c */
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

/* kav_fight.c */
void	special_move	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	highest_skill	args( ( CHAR_DATA *ch ) );
int	highest_stance	args( ( CHAR_DATA *ch, bool advanced ) );
void	beast_rage	args( ( CHAR_DATA *ch ) );
bool	is_undying	args( ( CHAR_DATA *ch ) );

/* kav_info.c */
void	birth_date	args( ( CHAR_DATA *ch, bool is_self ) );
void	other_age	args( ( CHAR_DATA *ch, int extra, bool is_preg, 
				char *argument ) );
int	years_old	args( ( CHAR_DATA *ch ) );
int	get_totalscore	args( ( CHAR_DATA *ch ) );
void	clear_eq	args( ( CHAR_DATA *ch ) );
void	sing_song	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	play_yet	args( ( CHAR_DATA *ch ) );
int	date_to_int	args( ( char *argument ) );

/* kav_wiz.c */
void oset_affect args( ( CHAR_DATA *ch, OBJ_DATA *obj, int value, int affect, bool is_quest) );

/* clan.c */
void	werewolf_regen	args( ( CHAR_DATA *ch ) );
void	part_reg	args( ( CHAR_DATA *ch ) );
void	reg_mend	args( ( CHAR_DATA *ch ) );
void	vamp_rage	args( ( CHAR_DATA *ch ) );
bool	char_exists	args( ( int file_dir, char *argument ) );
OD *	get_page	args( ( OBJ_DATA *book, int page_num ) );
void	update_gen	args( ( CHAR_DATA *ch ) );
int	true_generation	args( ( CHAR_DATA *ch, char *argument ) );
void	mummify_corpse	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	staked_corpse	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	power_ghoul	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	poison_weapon	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	ritual_ward	args( ( CHAR_DATA *ch, int direction, int ward ) );
char *	disc_name	args( ( int disc_no ) );

/* more.c */
void	guardian_message args( ( CHAR_DATA *ch ) );

/* string.c */
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );	/* OLC */
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );	/* OLC */
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );	/* OLC */
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );	/* OLC */
char *	string_replace	args( ( char * orig, char * old, char * new ) );	/* OLC */
char *	string_proper	args( ( char * argument ) );	/* OLC */
char *	string_unpad	args( ( char * argument ) );	/* OLC */
int	arg_count	args( ( char *argument ) );	/* OLC */

/* olc.c */
void    aedit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    redit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    medit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    oedit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void	add_reset	args( ( AREA_DATA *area, RESET_DATA *pReset, int index ) );	/* OLC */

/* core.c */
bool	regenerate_part	args( ( OBJ_DATA *obj ) );

/* bit.c */
char *	room_bit_name		args( ( int room_flags ) );	/* OLC */
int	room_name_bit		args( ( char *buf ) );		/* OLC */
int	sector_number		args( ( char *argument ) );	/* OLC */
char *	sector_name		args( ( int sect ) );		/* OLC */
int	item_name_type		args( ( char *name ) ); 	/* OLC */
char *	item_type_name		args( ( int item_type ) );	/* OLC */
int	extra_name_bit		args( ( char* buf ) );		/* OLC */
char *	extra_bit_name		args( ( int extra_flags ) ); 	/* OLC */
int	wear_name_bit		args( ( char* buf ) );		/* OLC */
char *	wear_bit_name		args( ( int wear ) );		/* OLC */
int	act_name_bit		args( ( char* buf ) );		/* OLC */
int	affect_name_bit		args( ( char* buf ) );		/* OLC */
int	affect_name_loc		args( ( char* name ) );		/* OLC */
int     wear_name_loc   	args( ( char *buf ) );		/* OLC */
char *	wear_loc_name		args( ( int wearloc ) );	/* OLC */
char *	act_bit_name		args( ( int act ) );		/* OLC */
int	get_weapon_type		args( ( char *arg ) );		/* OLC */
char *	get_type_weapon		args( ( int arg ) );		/* OLC */
int	get_container_flags	args( ( char *arg ) );		/* OLC */
char *	get_flags_container	args( ( int arg ) );		/* OLC */
int	get_liquid_type		args( ( char *arg ) );		/* OLC */
char *	get_type_liquid		args( ( int arg ) );		/* OLC */


#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
