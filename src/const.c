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
#include <time.h>
#include "merc.h"

/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 140, 12 },
    {  0,  0, 140, 13 }, /* 13  */
    {  0,  1, 170, 14 },
    {  1,  1, 170, 15 }, /* 15  */
    {  1,  2, 195, 16 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 400, 30 },
    {  3,  6, 500, 35 }, /* 20  */
    {  4,  7, 600, 40 },
    {  5,  7, 700, 45 },
    {  6,  8, 800, 50 },
    {  8, 10, 900, 55 },
    { 10, 12, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  1 },	/*  0 */
    {  1 },	/*  1 */
    {  1 },
    {  1 },	/*  3 */
    {  1 },
    {  1 },	/*  5 */
    {  1 },
    {  1 },
    {  1 },
    {  1 },
    {  1 },	/* 10 */
    {  1 },
    {  1 },
    {  2 },
    {  2 },
    {  2 },	/* 15 */
    {  2 },
    {  3 },
    {  3 },	/* 18 */
    {  3 },
    {  4 },	/* 20 */
    {  4 },
    {  4 },
    {  6 },
    {  6 },
    {  5 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 5 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 7 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			"clear",	{  0, 1, 10 }	},  /*  0 */
    { "beer",			"amber",	{  3, 2,  5 }	},
    { "wine",			"rose",		{  5, 2,  5 }	},
    { "ale",			"brown",	{  2, 2,  5 }	},
    { "dark ale",		"dark",		{  1, 2,  5 }	},

    { "whisky",			"golden",	{  6, 1,  4 }	},  /*  5 */
    { "lemonade",		"pink",		{  0, 1,  8 }	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }	},
    { "local specialty",	"everclear",	{  3, 3,  3 }	},
    { "slime mold juice",	"green",	{  0, 4, -8 }	},

    { "milk",			"white",	{  0, 3,  6 }	},  /* 10 */
    { "tea",			"tan",		{  0, 1,  6 }	},
    { "coffee",			"black",	{  0, 1,  6 }	},
    { "blood",			"red",		{  0, 0,  5 }	},
    { "salt water",		"clear",	{  0, 1, -2 }	},

    { "cola",			"cherry",	{  0, 1,  5 }	}   /* 15 */
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",		99,
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			""
    },

    {
	"acid blast",		3,
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),	20,	12,
	"acid blast",		"!Acid Blast!"
    },

    {
	"armor",		1,
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 1),	 5,	12,
	"",			"You feel less protected."
    },

    {
	"bless",		1,
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 3),	 5,	12,
	"",			"You feel less righteous."
    },

    {
	"blindness",		3,
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),	 5,	12,
	"",			"You can see again."
    },

    {
	"burning hands",	99,
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!"
    },

    {
	"call lightning",	1,
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!"
    },

    {
	"cause critical",	2,
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(63),	20,	12,
	"spell",		"!Cause Critical!"
    },

    {
	"cause light",		1,
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(62),	15,	12,
	"spell",		"!Cause Light!"
    },

    {
	"cause serious",	2,
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(64),	17,	12,
	"spell",		"!Cause Serious!"
    },

    {
	"change sex",		99,
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),	15,	12,
	"",			"Your body feels familiar again."
    },

    {
	"charm person",		4,
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident."
    },

    {
	"chill touch",		99,
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold."
    },

    {
	"colour spray",		99,
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!"
    },

    {
	"continual light",	1,
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),	 7,	12,
	"",			"!Continual Light!"
    },

    {
	"control weather",	3,
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),	25,	12,
	"",			"!Control Weather!"
    },

    {
	"create food",		1,
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),	 5,	12,
	"",			"!Create Food!"
    },

    {
	"create spring",	1,
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),	20,	12,
	"",			"!Create Spring!"
    },

    {
	"create water",		1,
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),	 5,	12,
	"",			"!Create Water!"
    },

    {
	"cure blindness",	1,
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),	 5,	12,
	"",			"!Cure Blindness!"
    },

    {
	"cure critical",	2,
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(15),	20,	12,
	"",			"!Cure Critical!"
    },

    {
	"cure light",		1,
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(16),	10,	12,
	"",			"!Cure Light!"
    },

    {
	"cure poison",		1,
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(43),	 5,	12,
	"",			"!Cure Poison!"
    },

    {
	"cure serious",		2,
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),	15,	12,
	"",			"!Cure Serious!"
    },

    {
	"curse",		3,
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_curse,		SLOT(17),	20,	12,
	"curse",		"The curse wears off."
    },

    {
	"detect evil",		4,
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(18),	 5,	12,
	"",			"The red in your vision disappears."
    },

    {
	"detect hidden",	3,
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(44),	 5,	12,
	"",			"You feel less aware of your suroundings."
    },

    {
	"detect invis",		4,
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects."
    },

    {
	"detect magic",		4,
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(20),	 5,	12,
	"",			"The detect magic wears off."
    },

    {
	"detect poison",	4,
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(21),	 5,	12,
	"",			"!Detect Poison!"
    },

    {
	"dispel evil",		99,
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!"
    },

    {
	"dispel magic",		3,
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(59),	15,	12,
	"",			"!Dispel Magic!"
    },


    {
	"earthquake",		4,
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!"
    },

    {
	"enchant weapon",	3,
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),	100,	24,
	"",			"!Enchant Weapon!"
    },

    {
	"energy drain",		4,
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!"
    },

    {
	"faerie fire",		3,
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away."
    },

    {
	"faerie fog",		3,
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!"
    },

    {
	"fireball",		3,
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),	15,	12,
	"fireball",		"!Fireball!"
    },

    {
	"flamestrike",		99,
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!"
    },

    {
	"fly",			1,
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(56),	10,	18,
	"",			"You slowly float to the ground."
    },

    {
	"gate",			7,
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(83),	50,	12,
	"",			"!Gate!"
    },

    {
	"giant strength",	1,
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(39),	20,	12,
	"",			"You feel weaker."
    },

    {
	"harm",			3,
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(27),	35,	12,
	"harm spell",		"!Harm!"
    },

    {
	"heal",			3,
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(28),	50,	12,
	"",			"!Heal!"
    },

    {
	"identify",		4,
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),	12,	24,
	"",			"!Identify!"
    },

    {
	"infravision",		1,
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(77),	 5,	18,
	"",			"You no longer see in the dark."
    },

    {
	"invis",		4,
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 5,	12,
	"",			"You are no longer invisible."
    },

    {
	"know alignment",	4,
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),	 9,	12,
	"",			"!Know Alignment!"
    },

    {
	"lightning bolt",	4,
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!"
    },

    {
	"locate object",	1,
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),	20,	18,
	"",			"!Locate Object!"
    },

    {
	"magic missile",	99,
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(32),	15,	4,
	"magic missile",	"!Magic Missile!"
    },

    {
	"mass invis",		4,
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	24,
	"",			"!Mass Invis!"
    },

    {
	"pass door",		1,
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),	20,	12,
	"",			"You feel solid again."
    },

    {
	"poison",		4,
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		SLOT(33),	10,	12,
	"poison",		"You feel less sick."
    },

    {
	"protection",		3,
	spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34),	 5,	12,
	"",			"You feel less protected."
    },

    {
	"refresh",		1,
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(81),	12,	18,
	"refresh",		"!Refresh!"
    },

    {
	"remove curse",		1,
	spell_remove_curse,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(35),	 5,	12,
	"",			"!Remove Curse!"
    },

    {
	"sanctuary",		1,
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(36),	75,	12,
	"",			"The white aura around your body fades."
    },

    {
	"shield",		1,
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away."
    },

    {
	"shocking grasp",	99,
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(37),	15,	12,
	"shocking grasp",	"!Shocking Grasp!"
    },

    {
	"sleep",		4,
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired."
    },

    {
	"stone skin",		1,
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),	12,	18,
	"",			"Your skin feels soft again."
    },

    {
	"summon",		4,
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(40),	500,	12,
	"",			"!Summon!"
    },

    {
	"teleport",		3,
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SLOT( 2),	35,	12,
	"",			"!Teleport!"
    },

    {
	"ventriloquate",	4,
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),	 5,	12,
	"",			"!Ventriloquate!"
    },

    {
	"weaken",		4,
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	20,	12,
	"spell",		"You feel stronger."
    },

    {
	"word of recall",	99,
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			SLOT(42),	 5,	12,
	"",			"!Word of Recall!"
    },

/*
 * Dragon breath
 */
    {
	"acid breath",		3,
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	 50,	 12,
	"blast of acid",	"!Acid Breath!"
    },

    {
	"fire breath",		3,
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),	 50,	 12,
	"blast of flame",	"!Fire Breath!"
    },

    {
	"frost breath",		3,
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),	 50,	 12,
	"blast of frost",	"!Frost Breath!"
    },

    {
	"gas breath",		3,
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),	 50,	 12,
	"blast of gas",		"!Gas Breath!"
    },

    {
	"lightning breath",	3,
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),	 50,	 12,
	"blast of lightning",	"!Lightning Breath!"
    },

/*
 * Fighter and thief skills.
 */
    {
	"backstab",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		SLOT( 0),	 0,	24,
	"backstab",		"!Backstab!"
    },

    {
	"disarm",		1,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		SLOT( 0),	 0,	24,
	"",			"!Disarm!"
    },

    {
	"hide",			99,
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!"
    },

    {
	"hurl",			1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_hurl,		SLOT( 0),	 0,	24,
	"",			"!Hurl!"
    },

    {
	"kick",			1,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		SLOT( 0),	 0,	 8,
	"kick",			"!Kick!"
    },

    {
	"peek",			1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!"
    },

    {
	"pick lock",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	12,
	"",			"!Pick!"
    },

    {
	"rescue",		1,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		SLOT( 0),	 0,	12,
	"",			"!Rescue!"
    },

    {
	"sneak",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	12,
	"",			"Your footsteps are no longer so quiet."
    },

    {
	"steal",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	24,
	"",			"!Steal!"
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
	"general purpose",	7,
	spell_general_purpose,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(501),	0,	12,
	"general purpose ammo",	"!General Purpose Ammo!"
    },

    {
	"high explosive",	7,
	spell_high_explosive,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(502),	0,	12,
	"high explosive ammo",	"!High Explosive Ammo!"
    },

/*
 * Spells added by KaVir.
 */
    {
	"guardian",		3,
	spell_guardian,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(600),	100,	12,
	"",			"!Guardian!"
    },

    {
	"soulblade",		2,
	spell_soulblade,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(601),	100,	12,
	"",			"!Soulblade!"
    },

    {
	"mana",			4,
	spell_mana,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(602),	 0,	12,
	"",			"!Mana!"
    },

    {
	"frenzy",		2,
	spell_frenzy,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(603),	 20,	12,
	"",			"Your bloodlust subsides."
    },

    {
	"darkblessing",		3,
	spell_darkblessing,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(604),	 20,	12,
	"",			"You feel less wicked."
    },

    {
	"portal",		4,
	spell_portal,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(604),	 500,	12,
	"",			"!Portal!"
    },

    { /* For potions (super 'mana' spell).  Don't change the level. */
	"energyflux",		99,
	spell_energyflux,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(605),	 0,	12,
	"",			"!EnergyFlux!"
    },

    {
	"voodoo",		4,
	spell_voodoo,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(606),	 100,	12,
	"",			"!Voodoo!"
    },

    {
	"transport",		4,
	spell_transport,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(607),	250,	24,
	"",			"!Transport!"
    },

    {
	"regenerate",		2,
	spell_regenerate,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(608),	500,	12,
	"",			"!Regenerate!"
    },

    {
	"clot",			2,
	spell_clot,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(609),	500,	12,
	"",			"!Clot!"
    },

    {
	"mend",			2,
	spell_mend,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(610),	500,	12,
	"",			"!Mend!"
    },

    {
	"punch",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_punch,		SLOT( 0),	 0,	24,
	"punch",		"!Punch!"
    },

    {
	"elbow",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_elbow,		SLOT( 0),	 0,	24,
	"elbow",		"!Elbow!"
    },

    {
	"headbutt",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_headbutt,		SLOT( 0),	 0,	24,
	"headbutt",		"!Headbutt!"
    },

    {
	"sweep",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sweep,		SLOT( 0),	 0,	24,
	"sweep",		"!Sweep!"
    },

    {
	"knee",			99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_knee,		SLOT( 0),	 0,	24,
	"knee",			"!Knee!"
    },

    {
	"forked tongue",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_tongue,		SLOT( 0),	 0,	24,
	"forked tongue",	"!Tongue!"
    },

    {
	"lashing tail",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_tail,		SLOT( 0),	 0,	24,
	"lashing tail",		"!Tongue!"
    },

    {
	"quill-like spines",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_spines,		SLOT( 0),	 0,	24,
	"long quill-like spines","!Spines!"
    },

    {
	"quest",		2,
	spell_quest,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(611),	 500,	12,
	"",			"!Quest!"
    },

    {
	"minor creation",	4,
	spell_minor_creation,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(612),	 500,	12,
	"",			"!MinorCreation!"
    },

    {
	"brew",			4,
	spell_brew,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(613),	 100,	12,
	"",			"!Brew!"
    },

    {
	"scribe",		4,
	spell_scribe,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(614),	 100,	12,
	"",			"!Scribe!"
    },

    {
	"carve",		4,
	spell_carve,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(615),	 200,	12,
	"",			"!Carve!"
    },

    {
	"engrave",		4,
	spell_engrave,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(616),	 300,	12,
	"",			"!Engrave!"
    },

    {
	"bake",			4,
	spell_bake,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(617),	 100,	12,
	"",			"!Bake!"
    },

    {
	"mount",		2,
	spell_mount,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(618),	100,	12,
	"",			"!Mount!"
    },

    {
	"berserk",		1,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_berserk,		SLOT( 0),	 0,	24,
	"",			"!Berserk!"
    },

    {
	"fastdraw",		1,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fastdraw,		SLOT( 0),	 0,	0,
	"",			"!Fastdraw!"
    },

    {
	"scan",			4,
	spell_scan,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(619),	6,	24,
	"",			"!Scan!"
    },

    {
	"fix",			4,
	spell_fix,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(620),	100,	24,
	"",			"!Fix!"
    },

    {
	"spellproof",		4,
	spell_spellproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(621),	50,	12,
	"",			"!Spellproof!"
    },

    {
	"preserve",		4,
	spell_preserve,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(550),	12,	24,
	"",			"!Preserve!"
    },

    {
	"track",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_track,		SLOT( 0),	 0,	0,
	"",			"!Track!"
    },

    {
	"major creation",	5,
	spell_major_creation,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(551),	 100,	12,
	"",			"!MajorCreation!"
    },

    {
	"copy",			4,
	spell_copy,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(552),	 100,	12,
	"",			"!Copy!"
    },

    {
	"insert page",		4,
	spell_insert_page,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(553),	 10,	12,
	"",			"!InsertPage!"
    },

    {
	"purple sorcery",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(554),	 100,	12,
	"",			"The purple spell on you fades away."
    },

    {
	"red sorcery",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(555),	 100,	12,
	"",			"The red spell on you fades away."
    },

    {
	"blue sorcery",		99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(556),	 100,	12,
	"",			"The blue spell on you fades away."
    },

    {
	"green sorcery",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(557),	 100,	12,
	"",			"The green spell on you fades away."
    },

    {
	"yellow sorcery",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(558),	 100,	12,
	"",			"The yellow spell on you fades away."
    },

    {
	"chaos blast",		99,
	spell_chaos_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(559),	20,	12,
	"chaos blast",		"!Chaos Blast!"
    },

    {
	"resistance",		4,
	spell_resistance,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(560),	50,	12,
	"",			"!Resistance!"
    },

    {
	"web",			99,
	spell_web,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_web,		SLOT(561),	 100,	12,
	"",			"The web surrounding you breaks away."
    },

    {
	"polymorph",		4,
	spell_polymorph,	TAR_IGNORE,		POS_STANDING,
	&gsn_polymorph,		SLOT(562),	50,	12,
	"",			"You resume your normal form."
    },

    {
	"birth control",	99,
	spell_contraception,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(563),	20,	12,
	"",			""
    },

    {
	"remove page",		4,
	spell_remove_page,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(564),	 10,	12,
	"",			"!RemovePage!"
    },

    {
	"find familiar",	4,
	spell_find_familiar,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(565),	100,	12,
	"",			""
    },

    {
	"improve",		3,
	spell_improve,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(566),	1500,	12,
	"",			"!Improve!"
    },

    {
	"fear",			99,
	spell_fear,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_fear,		SLOT(567),	 5,	12,
	"",			"You no longer feel so scared."
    },

    {
	"strength",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(568),	100,	12,
	"",			"Your supernatural strength fades."
    },

    {
	"dexterity",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(569),	100,	12,
	"",			"Your supernatural dexterity fades."
    },

    {
	"stamina",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(570),	100,	12,
	"",			"Your supernatural stamina fades."
    },

    {
	"blood potency",	99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_potency,		SLOT(571),	100,	12,
	"",			"Your blood returns to its original potency."
    },

    {
	"demonic possession",	99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_demon,		SLOT(572),	100,	12,
	"",			"You feel the spirit of Baal leave your body."
    },

    {
	"weakness",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(573),	100,	12,
	"",			"You feel stronger."
    },

    {
	"slowness",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(574),	100,	12,
	"",			"You feel faster."
    },

    {
	"infirmity",		99,
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(575),	100,	12,
	"",			"You feel healthier."
    },

    {
	"tendrils",		99,
	spell_tendrils,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_tendrils,		SLOT(576),	 100,	12,
	"tendrils of darkness",	"The tendrils of darkness surrounding you fall away."
    },

    {
	"rotting flesh",	99,
	spell_null,	        TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_rot,		SLOT(577),	10,	12,
	"flesh rot",		"Your flesh stops rotting."
    },

    {
	"flame bolt",		99,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(578),	20,	12,
	"flame bolt",		"!FlameBolt!"
    },

    {
	"jail of water",	99,
	spell_jail,        	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_jail,		SLOT(579),	 5,	12,
	"spray of water",	"The bands of water entrapping you splash to the ground."
    },

    {
	"infernal flame bolt",	99,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(580),	20,	12,
	"infernal flame bolt",	"!InfernalFlameBolt!"
    },

    {
	"first aid",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_firstaid,		SLOT( 0),	 0,	24,
	"first aid",		"!First Aid!"
    },

    {
	"repair",		1,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_repair,		SLOT( 0),	 0,	24,
	"repair",		"!Repair!"
    },

    {
	"spit of blood",	99,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_spit,		SLOT( 0),	 0,	24,
	"spit of blood",	"!SpitBlood!"
    },

    {
	"darkness",		99,
	spell_null,        	TAR_CHAR_SELF,	POS_STANDING,
	&gsn_darkness,		SLOT( 91),	 5,	12,
	NULL,			"Your globe of darkness fades."
    },

    {
	"paradox",		99,
	spell_null,		TAR_CHAR_SELF,	POS_STANDING,
	&gsn_paradox,		SLOT( 90),	 5,	12,
	NULL,			"Your paradox fades."
    },

    {
	"dark poison",		99,
	spell_null,	        TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_dark_poison,	SLOT(92),	10,	12,
	"dark poison",		"You feel less sick."
    },

    {
	"levitation",		99,
	spell_null,	        TAR_CHAR_SELF,	POS_STANDING,
	NULL,			SLOT(93),	10,	12,
	"poison",		"!Levitation!"
    }

};
