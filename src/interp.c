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
#include "imc-mercbase.h" /* Contains declarations of do_xxx functions */
#include "icec-mercbase.h"



bool	check_social	args((CHAR_DATA *ch, char *command,
                              char *argument));
bool	check_xsocial	args((CHAR_DATA *ch, char *command,
                              char *argument));
bool	check_nsocial	args((CHAR_DATA *ch, char *command,
                              char *argument));
void	make_preg	args((CHAR_DATA *mother, CHAR_DATA *father));


/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] = {
    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "buy",		do_buy,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "chant",		do_chant,	POS_FIGHTING,	 4,  LOG_NORMAL	},
    { "command",	do_command,	POS_SITTING,	 3,  LOG_ALWAYS	},
    { "crack",		do_crack,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "diagnose",	do_diagnose,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "dismount",	do_dismount,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "enter",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "exits",		do_exits,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "exp",		do_exp,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "get",		do_get,		POS_SITTING,	 0,  LOG_NORMAL	},

    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "look",		do_look,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "level",		do_exp,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "mount",		do_mount,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "order",		do_order,	POS_SITTING,	 0,  LOG_ALWAYS	},
    { "peek",		do_peek,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "pour",		do_pour,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "rest",		do_rest,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "sit",		do_sit,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "tell",		do_tell,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "whisper",	do_whisper,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "wield",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "wizhelp",	do_wizhelp,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "wizlist",	do_wizlist,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "version",	do_version,	POS_DEAD,	 0,  LOG_NORMAL	},
    /*
    { "donate",	        do_donate,	POS_SITTING,	 0,  LOG_NORMAL	},

     * Informational commands.
     */
    { "affects",	do_affects,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL },
    { "compare",	do_compare,	POS_SITTING,	 0,  LOG_NORMAL },
    { "consider",	do_consider,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "examine",	do_examine,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "skill",		do_skill,	POS_MEDITATING,	 0,  LOG_NORMAL },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL },
    { "spells",		do_spell,	POS_MEDITATING,	 0,  LOG_NORMAL },
    /*
        { "status",		do_status,	POS_STANDING,	 3,  LOG_NORMAL },
    */
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "totalscore",	do_totalscore,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "weather",	do_weather,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "introduce",	do_introduce,	POS_STANDING,	 1,  LOG_NORMAL	},
    /*
        { "xemot",		do_huh,		POS_DEAD,	 1,  LOG_NORMAL },
        { "xemote",		do_xemote,	POS_SITTING,	 1,  LOG_NORMAL },
    */
    { "nsocial",	do_huh,		POS_DEAD,	 1,  LOG_NORMAL },
    { "nsocials",	do_nsocials,	POS_DEAD,	 1,  LOG_NORMAL },
    { "xsocial",	do_huh,		POS_DEAD,	 1,  LOG_NORMAL },
    { "xsocials",	do_xsocials,	POS_DEAD,	 1,  LOG_NORMAL },

    /*
     * Configuration commands.
     */
    { "alignment",	do_alignment,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER	},
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL	},

    { "ansi",		do_ansi,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "autoexit",	do_autoexit,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "autoloot",	do_autoloot,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "autosac",	do_autosac,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "blank",		do_blank,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "brief",		do_brief,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "cprompt",	do_cprompt,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "prompt",		do_prompt,	POS_DEAD,	 0,  LOG_NORMAL	},

    /*
     * Communication commands.
     */
    { "answer",		do_answer,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "auction",	do_auction,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "chat",		do_chat,	POS_DEAD,	 0,  LOG_NORMAL	},
    { ".",		do_chat,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "emote",		do_xemote,	POS_SITTING,	 0,  LOG_NORMAL	},
    { ",",		do_xemote,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "email",		do_email,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "howl",		do_howl,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "music",		do_music,	POS_SLEEPING,	 3,  LOG_NORMAL },
    { "newbie",		do_newbie,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "note",		do_note,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "pose",		do_emote,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "quest",		do_quest,	POS_SITTING,	 2,  LOG_ALWAYS	},
    { "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "reply",		do_reply,	POS_MEDITATING,	 0,  LOG_NORMAL },
    { "say",		do_say,		POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "sing",		do_sing,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "speak",		do_speak,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "'",		do_say,		POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "shout",		do_shout,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "yell",		do_yell,	POS_SITTING,	 0,  LOG_NORMAL	},

    /*
     * Object manipulation commands.
     */
    { "activate",	do_activate,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "brandish",	do_brandish,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "close",		do_close,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "draw",		do_draw,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "drink",		do_drink,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "drop",		do_drop,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "eat",		do_eat,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "empty",		do_empty,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "fill",		do_fill,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "give",		do_give,	POS_SITTING,	 0,  LOG_ALWAYS	},
    { "hold",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "list",		do_list,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "lock",		do_lock,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "morph",		do_morph,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "open",		do_open,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "pick",		do_pick,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "press",		do_activate,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "pull",		do_activate,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "put",		do_put,		POS_SITTING,	 0,  LOG_NORMAL },
    { "quaff",		do_quaff,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "recite",		do_recite,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "remove",		do_remove,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "sell",		do_sell,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "sheath",		do_sheath,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "take",		do_get,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "reload",		do_reload,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "unload",		do_unload,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "read",		do_read,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "shoot",		do_shoot,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "throw",		do_throw,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "turn",		do_turn,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "twist",		do_activate,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "sacrifice",	do_sacrifice,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "showartifacts",	do_show_artifacts, POS_SITTING,	 1,  LOG_NORMAL	},
    { "unlock",		do_unlock,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "value",		do_value,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "wear",		do_wear,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "write",		do_write,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "voodoo",		do_voodoo,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "zap",		do_zap,		POS_SITTING,	 0,  LOG_NORMAL	},

    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "fightstyle",	do_fightstyle,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "hurl",		do_hurl,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "punch",		do_punch,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_ALWAYS	},
    { "setstance",	do_setstance,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "stance",		do_stance,	POS_FIGHTING,	 0,  LOG_NORMAL	},

    /*
     * Miscellaneous commands.
     */
    { "accep",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL	},
    { "accept",		do_accept,	POS_STANDING,	 2,  LOG_NORMAL	},
    { "artifact",	do_artifact,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "birth",		do_birth,	POS_STANDING,	 1,  LOG_NORMAL	},
    { "blindfold",	do_blindfold,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "breaku",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL	},
    { "breakup",	do_breakup,	POS_STANDING,	 2,  LOG_NORMAL	},
    { "call",		do_call,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "claim",		do_claim,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "complete",	do_complete,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "consen",		do_huh,		POS_STANDING,	 1,  LOG_NORMAL	},
    { "consent",	do_consent,	POS_STANDING,	 1,  LOG_NORMAL	},
    { "finger",		do_finger,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "firstaid",	do_firstaid,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "follow",		do_follow,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "gag",		do_gag,		POS_STANDING,	 3,  LOG_NORMAL	},
    { "gift",		do_gift,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "godless",	do_godless,	POS_DEAD,	 3,  LOG_ALWAYS	},
    { "group",		do_group,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "home",		do_home,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "hunt",		do_hunt,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "lick",		do_lick,	POS_MEDITATING,	 0,  LOG_NORMAL	},
    { "locate",		do_locate,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "propos",		do_huh,		POS_STANDING,	 2,  LOG_NORMAL	},
    { "propose",	do_propose,	POS_STANDING,	 2,  LOG_NORMAL	},
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "quit",		do_quit,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "escape",		do_escape,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "recharge",	do_recharge,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "repair",		do_repair,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "safe",		do_safe,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "slit",		do_slit,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "smother",	do_smother,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "split",		do_split,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "scan",		do_scan,	POS_SITTING,	 0,  LOG_NORMAL	},
    { "spy",		do_spy,		POS_SITTING,	 0,  LOG_NORMAL	},
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "summon",		do_summon,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "tie",		do_tie,		POS_STANDING,	 3,  LOG_NORMAL	},
    { "token",		do_token,	POS_STANDING,	 2,  LOG_NORMAL	},
    { "track",		do_track,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "train",		do_train,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "unpolymorph",	do_unpolymorph,	POS_STANDING,	 4,  LOG_NORMAL },
    { "untie",		do_untie,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "where",		do_where,	POS_SITTING,	 0,  LOG_NORMAL	},

    /*
     * Drow Commands
    { "levitation",	do_drow_levitation, POS_STANDING,	 3,  LOG_NORMAL	},
    { "darkness",	do_drow_darkness  , POS_STANDING,	 3,  LOG_NORMAL	},
    { "dancing",	do_drow_dancinglights , POS_STANDING,	 3,  LOG_NORMAL	},
    { "drowsight",	do_drow_sight	 , POS_STANDING,	 3,  LOG_NORMAL	},
    { "drowshield",	do_drow_shield 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "drowcreate",	do_drow_oload 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "drowfire",	do_drow_fire 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "spider ",	do_drow_spider 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "spray",	        do_drow_web 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "grant",	        do_grant 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "offer",	        do_offer 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "house",	        do_house 	, POS_STANDING,	 3,  LOG_NORMAL	},
    { "namehouse",      do_name_house 	, POS_STANDING,	 3,  LOG_NORMAL	},
     */


    /*
     * Vampire and werewolf commands.
     */

    { "aeolus",		do_aeolus,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "anesthetic",	do_anesthetic,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "angelarmour",	do_angelarmour,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "angelform",	do_angelform,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "anger",		do_anger,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "animalmastery",	do_animal,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "announce",	do_inform,	POS_MEDITATING,	 3,  LOG_NORMAL	},
    { "armsabyss",	do_arms,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "armsofabyss",	do_arms,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "armsoftheabyss",	do_arms,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "ashestoashes",	do_ashes,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "astral",		do_projection,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "readaura",	do_readaura,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "awe",		do_awe,		POS_STANDING,	 3,  LOG_NORMAL	},
    { "baal",		do_baal,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "beastwithin",	do_beastwithin,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "beckoning",	do_beckon,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "blast",		do_mindblast,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "bloodrage",	do_bloodrage,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "bloodagony",	do_bloodagony,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "blurofthemilkyeye", do_blur,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "blurofmilkyeye",	do_blur,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "blurmilkyeye",	do_blur,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "bond",		do_fleshbond,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "bonecraft",	do_bonecraft,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "bodywrack",	do_bodywrack,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "breath",		do_breath,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "burrow",		do_burrow,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "calllamprey",	do_lamprey,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "callthelamprey",	do_lamprey,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "calm",		do_calm,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "camouflage",	do_camouflage,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "cauldronblood",	do_cauldronblood, POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "cauldronofblood", do_cauldronblood, POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "celerity",	do_celerity,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "champions",	do_champions,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "change",		do_change,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "changeling",	do_changeling,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "cherub",		do_cherub,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "clandisc",	do_clandisc,	POS_STANDING,	 3,  LOG_ALWAYS	},
    { "clanname",	do_clanname,	POS_STANDING,	 3,  LOG_ALWAYS	},
    { "clapofthunder",	do_clap,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "clapthunder",	do_clap,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "claw",		do_claw,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "claws",		do_claws,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "cloak",		do_hide,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "cloneobject",	do_cloneobject,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "conditioning",	do_conditioning, POS_STANDING,	 3,  LOG_NORMAL	},
    { "consumingfear",	do_fear,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "controlillusion", do_control,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "controlbeast",	do_controlbeast, POS_MEDITATING,	 3,  LOG_NORMAL	},
    { "coterie",	do_coterie,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "courage",	do_courage,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "ctalk",		do_ctalk,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "curse",		do_curse,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "darkheart",	do_darkheart,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "dazzle",		do_dazzle,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "demonarmour",	do_demonarmour,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "demonform",	do_demonform,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "discipline",	do_discipline,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "disquiet",	do_disquiet,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "drainlife",	do_drainlife,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "dreadgaze",	do_dreadgaze,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "earthmeld",	do_earthmeld,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "eclipse",	do_eclipse,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "embrace",	do_embrace,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "engulf",		do_engulf,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "entrancement",	do_entrancement, POS_STANDING,	 3,  LOG_NORMAL	},
    /*
        { "evileye",	do_evileye,	POS_STANDING,	 3,  LOG_NORMAL	},
    */
    { "eyespy",		do_eyespy,	POS_SITTING,	 2,  LOG_NORMAL	},
    { "falseappearance", do_rename,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "fangs",		do_fangs,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "favour",		do_favour,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "fear",		do_fear,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "feed",		do_feed,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "fetishdoll",	do_fetishdoll,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "firewall",	do_wallflames,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "flamebolt",	do_flamebolt,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "flamesnetherworld", do_flames,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "fleshcraft",	do_fleshcraft,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "fleshbond",	do_fleshbond,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "flex",		do_flex,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "fog",		do_aeolus,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "formillusion",	do_formillusion, POS_STANDING,	 3,  LOG_NORMAL	},
    { "frenzytrigger",	do_frenzy,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "gcommand",	do_fcommand,	POS_SITTING,	 3,  LOG_ALWAYS	},
    { "gifts",		do_gifts,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "guardian",	do_guardian,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "hagswrinkles",	do_hagswrinkles, POS_STANDING,	 3,  LOG_NORMAL	},
    { "haloofthesun",	do_halosun,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "haloofsun",	do_halosun,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "halosun",	do_halosun,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "handflames",	do_hand,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "handofflames",	do_hand,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "handsflames",	do_hand,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "handsofflames",	do_hand,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "heightenedsenses", do_truesight,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "hooves",		do_hooves,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "horns",		do_horns,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "humanform",	do_humanform,	POS_SITTING,	 2,  LOG_NORMAL	},
    { "icychillofdespair", do_icy,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "icychilldespair", do_icy,		POS_STANDING,	 3,  LOG_NORMAL	},
    { "imp",		do_imp,		POS_STANDING,	 3,  LOG_NORMAL	},
    { "inferno",	do_inferno,	POS_MORTAL,	 3,  LOG_NORMAL	},
    { "infernalportal",	do_portal,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "infirmity",	do_infirmity,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "inpart",		do_inpart,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "jailwater",	do_jail,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "jailofwater",	do_jail,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "lamprey",	do_lamprey,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "leap",		do_leap,	POS_RESTING,	 2,  LOG_NORMAL	},
    { "lifespan",	do_lifespan,	POS_RESTING,	 2,  LOG_NORMAL	},
    { "lightningstrike", do_lightning,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "love",		do_love,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "loyalty",	do_loyalty,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "majesty",	do_majesty,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "mask",		do_mask,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "masssummons",	do_masssummons,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "master",		do_animal,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "mindblank",	do_shield,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "mindblast",	do_mindblast,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "mirrorimage",	do_mirrorimage,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "motherstouch",	do_mother,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "mummify",	do_mummify,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "neutralguard",	do_neutral,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "nightsight",	do_nightsight,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "obedience",	do_obedience,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "obeah",		do_obeah,	POS_MEDITATING,	 3,  LOG_NORMAL	},
    { "offersoul",	do_offersoul,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "omnipotence",	do_omnipotence,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "pact",		do_pact,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "panacea",	do_panacea,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "paralyzingstare", do_paralyzing,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "persuasion",	do_persuasion,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "pigeon",		do_pigeon,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "plasmaform",	do_plasmaform,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "porcupine",	do_porcupine,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "portal",		do_portal,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "possession",	do_possession,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "potency",	do_potency,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "powers",		do_powers,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "pray",		do_pray,	POS_MEDITATING,	 1,  LOG_NORMAL	},
    { "preservation",	do_preservation, POS_STANDING,	 3,  LOG_NORMAL	},
    { "preserve",	do_preservation, POS_STANDING,	 3,  LOG_NORMAL	},
    { "projection",	do_projection,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "rage",		do_rage,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "putrefaction",	do_putrefaction, POS_STANDING,	 3,  LOG_NORMAL	},
    { "rain",		do_rain,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "regenerate",	do_regenerate,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "reina",		do_reina,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "renew",		do_renew,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "reshapeobject",	do_reshape,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "roll",		do_roll,	POS_DEAD,	 2,  LOG_NORMAL	},
    { "scream",		do_scream,	POS_MEDITATING,	 1,  LOG_NORMAL	},
    { "scry",		do_scry,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "serenity",	do_serenity,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "serpent",	do_serpent,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "servant",	do_servant,	POS_STANDING,	 3,  LOG_NORMAL	},
    /*
        { "sect",		do_side,	POS_STANDING,	 3,  LOG_NORMAL	},
    */
    { "shadowbody",	do_shadowbody,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "shadowstep",	do_shadowstep,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "sharespirit",	do_sharespirit,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "sharingspirit",	do_sharespirit,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "shield",		do_shield,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "shift",		do_shift,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "shroudnight",	do_shroudnight,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "shroudofnight",	do_shroudnight,	POS_STANDING,	 3,  LOG_NORMAL	},
    /*
        { "side",		do_side,	POS_STANDING,	 3,  LOG_NORMAL	},
    */
    { "scentbeyond",	do_scentbeyond,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "scentfrombeyond", do_scentbeyond,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "songserenity",	do_serenity,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "songofserenity",	do_serenity,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "songofreina",	do_reina,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "soulmask",	do_soulmask,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "soultwist",	do_soultwist,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "spiritgate",	do_spiritgate,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "spiritguardian",	do_spiritguardian, POS_STANDING,	 3,  LOG_NORMAL	},
    { "spiritform",	do_spiritform,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "spiriteyes",	do_spiriteyes,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "spit",		do_spit,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "stake",		do_stake,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "staredown",	do_staredown,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "step",		do_shadowstep,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "succulence",	do_succulence,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "summoning",	do_summoning,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "tail",		do_tail,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "tasteblood",	do_tasteblood,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "tastedeath",	do_spit,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "tasteofdeath",	do_spit,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "teach",		do_teach,	POS_STANDING,	 5,  LOG_NORMAL	},
    { "teachdisc",	do_teachdisc,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "teachgift",	do_teachgift,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "thanatopsis",	do_thanatopsis,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "theftvitae",	do_theftvitae,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "theftofvitae",	do_theftvitae,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "tongue",		do_tongue,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "travel",		do_travel,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "tribe",		do_tribe,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "tribegifts",	do_tribegifts,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "truedisguise",	do_truedisguise, POS_STANDING,	 3,  LOG_NORMAL	},
    { "truefear",	do_truefear,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "twist",		do_soultwist,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "umbra",		do_shadowplane,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "umbralvision",	do_shadowsight,	POS_SITTING,	 3,  LOG_NORMAL	},
    { "unburden",	do_unburden,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "unveil",		do_unveil,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "upkeep",		do_upkeep,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "vanish",		do_vanish,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "vclan",		do_vclan,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "wallofflames",	do_wallflames,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wallflames",	do_wallflames,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wallofgranite",	do_wallgranite,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wallgranite",	do_wallgranite,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wallofwater",	do_wall,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wallwater",	do_wall,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "weakness",	do_weakness,	POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "weaponform",	do_weaponform,	POS_STANDING,	 2,  LOG_NORMAL	},
    { "web",		do_web,		POS_FIGHTING,	 3,  LOG_NORMAL	},
    { "willpower",	do_willpower,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wings",		do_wings,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "withering",	do_withering,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "witherlimb",	do_witherlimb,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "wwgifts",	do_gifts,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "wwpowers",	do_wwpowers,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "wyld",		do_wyld,	POS_DEAD,	 3,  LOG_NORMAL	},
    { "zombie",		do_zombie,	POS_STANDING,	 3,  LOG_NORMAL	},
    { "zuloform",	do_zuloform,	POS_FIGHTING,	 3,  LOG_NORMAL	},

    /*
     * IMC Mortal commands
     */
    { "rtell",          do_rtell,       POS_SLEEPING,    0,  LOG_NORMAL },
    { "rreply",         do_rreply,      POS_SLEEPING,    0,  LOG_NORMAL },
    { "rwho",           do_rwho,        POS_SLEEPING,    5,  LOG_NORMAL },
    { "rquery",         do_rquery,      POS_SLEEPING,    5,  LOG_NORMAL },
    { "imc",            do_imc,         POS_DEAD,       10,  LOG_ALWAYS },
    { "imclist",        do_imclist,     POS_DEAD,        5,  LOG_NORMAL },
    { "rbeep",          do_rbeep,       POS_SLEEPING,    0,  LOG_NORMAL },
    { "rfinger",        do_rfinger,     POS_SLEEPING,    5,  LOG_NORMAL },
    { "rwhois",         do_rwhois,      POS_SLEEPING,    5,  LOG_NORMAL },
    { "istats",         do_istats,      POS_SLEEPING,    0,  LOG_NORMAL },
    { "rchannels",      do_rchannels,   POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Immortal commands.
     */
    { "allow",		do_allow,	POS_DEAD,	11,  LOG_ALWAYS	},
    { "ban",		do_ban,		POS_DEAD,	11,  LOG_ALWAYS	},
    { "bind",		do_bind,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "clearstats",	do_clearstats,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "clearclas",	do_clearclas,	POS_STANDING,	 1,  LOG_NORMAL	},
    { "clearclass",	do_clearclass,	POS_STANDING,	11,  LOG_ALWAYS	},
    { "clist",		do_clist,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "create",		do_create,	POS_STANDING,	 8,  LOG_NORMAL	},
    { "deny",		do_deny,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "disconnect",	do_disconnect,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "divorce",	do_divorce,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "familiar",	do_familiar,	POS_STANDING,	12,  LOG_NORMAL	},
    { "fcommand",	do_fcommand,	POS_SITTING,	 3,  LOG_ALWAYS	},
    /*
        { "fileupdate",	do_fileupdate,	POS_DEAD,	12,  LOG_ALWAYS	},
    */
    { "freeze",		do_freeze,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "marry",		do_marry,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "pchat",		do_pchat,	POS_DEAD,	12,  LOG_NORMAL	},
    { "psay",		do_psay,	POS_MEDITATING,	12,  LOG_NORMAL	},
    { "ptalk",		do_ptalk,	POS_MEDITATING,	12,  LOG_NORMAL	},
    { "qset",		do_qset,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "qstat",		do_qstat,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "qtrust",		do_qtrust,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "reboo",		do_reboo,	POS_DEAD,	10,  LOG_NORMAL	},
    { "reboot",		do_reboot,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "shutdow",	do_shutdow,	POS_DEAD,	10,  LOG_NORMAL	},
    { "shutdown",	do_shutdown,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "users",		do_users,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "wizlock",	do_wizlock,	POS_DEAD,	11,  LOG_ALWAYS	},
    { "watche",		do_huh,		POS_DEAD,	 2,  LOG_NEVER	},
    { "watcher",	do_watcher,	POS_DEAD,	 2,  LOG_NEVER	},

    { "force",		do_force,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "mclear",		do_mclear,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "mload",		do_mload,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "mset",		do_mset,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "nice",		do_nice,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "noemote",	do_noemote,	POS_DEAD,	 9,  LOG_NORMAL	},
    { "notell",		do_notell,	POS_DEAD,	 9,  LOG_NORMAL	},
    { "oclone",		do_oclone,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "oload",		do_oload,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "oset",		do_oset,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "otransfer",	do_otransfer,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "pload",		do_pload,	POS_DEAD,	12,  LOG_ALWAYS	},
    { "preturn",	do_preturn,	POS_DEAD,	99,  LOG_NORMAL	},
    { "pset",		do_pset,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "purge",		do_purge,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "qmake",		do_qmake,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "release",	do_release,	POS_DEAD,	 9,  LOG_ALWAYS	},
    { "relevel",	do_relevel,	POS_DEAD,	 1,  LOG_NEVER	},
    { "restore",	do_restore,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "rset",		do_rset,	POS_DEAD,	 7,  LOG_ALWAYS	},
    { "shell",		do_shell,	POS_DEAD,	10,  LOG_NORMAL	},
    { "silence",	do_silence,	POS_DEAD,	 9,  LOG_NORMAL },
    { "sla",		do_sla,		POS_DEAD,	10,  LOG_NORMAL	},
    { "slay",		do_slay,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "special",	do_special,	POS_DEAD,	10,  LOG_ALWAYS	},
    { "decapitate",	do_decapitate,	POS_STANDING,	 3,  LOG_ALWAYS	},
    { "sset",		do_sset,	POS_DEAD,	10,  LOG_ALWAYS },
    { "tear",		do_tear,	POS_STANDING,	 3,  LOG_ALWAYS	},
    { "transfer",	do_transfer,	POS_DEAD,	 8,  LOG_ALWAYS	},

    { "transport",	do_transport,	POS_DEAD,	 0,  LOG_NORMAL	},

    { "at",		do_at,		POS_DEAD,	 8,  LOG_NORMAL	},
    { "bamfin",		do_bamfin,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "bamfout",	do_bamfout,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "echo",		do_echo,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "goto",		do_goto,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "holylight",	do_holylight,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "invis",		do_invis,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "log",		do_log,		POS_DEAD,	 9,  LOG_ALWAYS	},
    { "login",		do_login,	POS_DEAD,	 9,  LOG_NORMAL	},
    { "memory",		do_memory,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "mfind",		do_mfind,	POS_DEAD,	 7,  LOG_NORMAL },
    { "move",		do_move,	POS_STANDING,	 1,  LOG_NORMAL },
    { "mstat",		do_mstat,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "mwhere",		do_mwhere,	POS_DEAD,	 8,  LOG_NORMAL },
    { "ofind",		do_ofind,	POS_DEAD,	 7,  LOG_NORMAL },
    { "ostat",		do_ostat,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "oswitch",	do_oswitch,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "oreturn",	do_oreturn,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "peace",		do_peace,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "recho",		do_recho,	POS_DEAD,	 8,  LOG_ALWAYS	},
    { "return",		do_return,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "rstat",		do_rstat,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "slookup",	do_slookup,	POS_DEAD,	 7,  LOG_NORMAL },
    { "snoop",		do_snoop,	POS_DEAD,	 8,  LOG_NORMAL	},
    { "switch",		do_switch,	POS_DEAD,	 8,  LOG_ALWAYS	},

    { "defences",	do_vampire,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "vamptalk",	do_vamptalk,	POS_DEAD,	 1,  LOG_NORMAL	},
    { "vtalk",		do_vamptalk,	POS_DEAD,	 1,  LOG_NORMAL	},
    { ">",		do_vamptalk,	POS_DEAD,	 1,  LOG_NORMAL	},
    { "vampire",	do_vampire,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "immune",		do_immune,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "invisibility",	do_invisibility, POS_STANDING,	 3,  LOG_NORMAL	},
    { "[",		do_fcommand,	POS_SITTING,	 3,  LOG_ALWAYS	},

    { "immtalk",	do_immtalk,	POS_DEAD,	 8,  LOG_NORMAL	},
    { ":",		do_immtalk,	POS_DEAD,	 8,  LOG_NORMAL	},

    { "buildtalk",	do_buildtalk,	POS_DEAD,	 7,  LOG_NORMAL	},
    { "=",		do_buildtalk,	POS_DEAD,	 7,  LOG_NORMAL	},

    { "silencedeath",	do_silencedeath, POS_STANDING,	 3,  LOG_NORMAL	},
    { "silenceofdeath",	do_silencedeath, POS_STANDING,	 3,  LOG_NORMAL	},

    /*
     * IMC Immortal commands.
     */
    { "rinfo",		do_rinfo,	POS_DEAD,	 0,  LOG_NORMAL },
    { "rsockets",	do_rsockets,	POS_DEAD,	10,  LOG_NORMAL },
    { "rconnect",	do_rconnect,	POS_DEAD,	10,  LOG_ALWAYS },
    { "rdisconnect",	do_rdisconnect,	POS_DEAD,	10,  LOG_ALWAYS },
    { "rignore",	do_rignore,	POS_DEAD,	10,  LOG_ALWAYS },
    { "mailq",		do_mailqueue,	POS_DEAD,	10,  LOG_NORMAL },
    { "rchanset",	do_rchanset,	POS_DEAD,	10,  LOG_ALWAYS },
    { "rping",          do_rping,       POS_DEAD,	10,   LOG_NORMAL},

    { "icommand",       do_icommand,    POS_DEAD,	10,   LOG_NORMAL},
    { "isetup",         do_isetup,      POS_DEAD,	10,   LOG_ALWAYS},
    { "ilist",          do_ilist,       POS_DEAD,	10,   LOG_NORMAL},
    { "ichannels",      do_ichannels,   POS_DEAD,	10,   LOG_NORMAL},

    /*
     * Online creation commands.
     */
    { "aedit",		do_aedit,	POS_DEAD,	10,  LOG_NORMAL },
    { "redit",		do_redit,	POS_DEAD,	 7,  LOG_NORMAL },
    { "oedit",          do_oedit,	POS_DEAD,	 7,  LOG_NORMAL },
    { "medit",          do_medit,	POS_DEAD,	 7,  LOG_NORMAL },

    { "ashow",          do_ashow,	POS_DEAD,	 7,  LOG_NORMAL },
    { "rshow",          do_rshow,	POS_DEAD,	 7,  LOG_NORMAL },
    { "oshow",          do_oshow,	POS_DEAD,	 7,  LOG_NORMAL },
    { "mshow",          do_mshow,	POS_DEAD,	 7,  LOG_NORMAL },

    { "resets",         do_resets,      POS_DEAD,	 7,  LOG_NORMAL },

    { "asave",          do_asave,	POS_DEAD,	 7,  LOG_NORMAL },

    { "alist",		do_arealist,	POS_DEAD,	 7,  LOG_NORMAL },


    /*
     * Clash with "medit".
     */
    { "meditate",	do_meditate,	POS_MEDITATING,	 0,  LOG_NORMAL	},

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL	}
};




/*
 * The social table.
 * Add new socials here.
 * Alphabetical order is not required.
 */
const	struct	social_type	social_table [] = {
    {
        "accuse",
        "Accuse whom?",
        "$n is in an accusing mood.",
        "You look accusingly at $M.",
        "$n looks accusingly at $N.",
        "$n looks accusingly at you.",
        "You accuse yourself.",
        "$n seems to have a bad conscience."
    },

    {
        "ack",
        "You gasp and say 'ACK!' at your mistake.",
        "$n ACKS at $s big mistake.",
        "You ACK $M.",
        "$n ACKS $N.",
        "$n ACKS you.",
        "You ACK yourself.",
        "$n ACKS $mself.  Must be a bad day."
    },

    {
        "addict",
        "You stand and admit to all in the room, 'Hi, I'm $n, and I'm a mud addict.'",
        "$n stands and says, 'Hi, I'm $n, and I'm a mud addict.'",
        "You tell $M that you are addicted to $S love.",
        "$n tells $N that $e is addicted to $S love.",
        "$n tells you that $e is addicted to your love.",
        "You stand and admit to all in the room, 'Hi, I'm $n, and I'm a mud addict.'",
        "$n stands and says, 'Hi, I'm $n, and I'm a mud addict.'",
    },

    {
        "afk",
        "You announce that you are going away from keyboard.",
        "$n announces that $e is going away from keyboard.",
        "You announce that you are going away from keyboard.",
        "$n informs $N that $e is going away from keyboard.",
        "$n informs you that $e is going away from keyboard.",
        "You announce that you are going away from keyboard.",
        "$n announces that $e is going away from keyboard.",
    },

    {
        "agree",
        "You seem to be in an agreeable mood.",
        "$n seems to agree.",
        "You agree with $M.",
        "$n agrees with $N.",
        "$n agrees with you.",
        "Well I hope you would agree with yourself!",
        "$n agrees with $mself, of course."
    },

    {
        "airguitar",
        "You sizzle the air with your BITCHIN' guitar playing!",
        "$n air-guitars like Jimi Hendrix!",
        "You sizzle the air with your BITCHIN' guitar playing!",
        "$n air-guitars like Jimi Hendrix!",
        "$n air-guitars like Jimi Hendrix, just for you!",
        "You forget all else as you sizzle the air with your BITCHIN' guitar playing!",
        "$n forgets all else as $e air-guitars like Jimi Hendrix."
    },

    {
        "apologize",
        "You apologize for your behavior.",
        "$n apologizes for $s rude behavior.",
        "You apologize to $M.",
        "$n apologizes to $N.",
        "$n apologizes to you.",
        "You apologize to yourself.",
        "$n apologizes to $mself.  Hmmmm."
    },

    {
        "applaud",
        "Clap, clap, clap.",
        "$n gives a round of applause.",
        "You clap at $S actions.",
        "$n claps at $N's actions.",
        "$n gives you a round of applause.  You MUST'VE done something good!",
        "You applaud at yourself.  Boy, are we conceited!",
        "$n applauds at $mself.  Boy, are we conceited!"
    },

    {
        "banzai",
        "You scream 'BANZAI!!!!' and charge into the fray.",
        "$n screams 'BANZAI!!!!' and charges into the fray.",
        "You scream 'BANZAI!!!!' and drag $M into the fray with you.",
        "$n screams 'BANZAI!!!!' and drags $N into the fray with $m!",
        "$n screams 'BANZAI!!!!' and drags you into the fray with $m!",
        "You scream 'BANZAI!!!!' and brandish your weapon for battle.",
        "$n screams 'BANZAI!!!!' and brandishes $s weapon for battle.",
    },


    {
        "bark",
        "Woof!  Woof!",
        "$n barks like a dog.",
        "You bark at $M.",
        "$n barks at $N.",
        "$n barks at you.",
        "You bark at yourself.  Woof!  Woof!",
        "$n barks at $mself.  Woof!  Woof!"
    },

    {
        "bbl",
        "You announce that you will be back later.",
        "$n announces that $e'll be back later.",
        "You inform $M that you will be back later.",
        "$n informs $N that $e will be back later",
        "$n informs you that $e will be back later",
        "You mumble to yourself that you'll be back later.",
        "$n mumbles to $mself that $e'll be back later."
    },

    {
        "bearhug",
        "You hug a grizzly bear.",
        "$n hugs a flea-infested grizzly bear.",
        "You bearhug $M.",
        "$n bearhugs $N.  Some ribs break.",
        "$n bearhugs you.  Wonder what's coming next?",
        "You bearhug yourself.",
        "$n bearhugs $mself."
    },

    {
        "beef",
        "You loudly exclaim 'WHERE'S THE BEEF??????'",
        "$n loudly exclaims 'WHERE'S THE BEEF??????'",
        "You poke $N and exclaim, 'WHERE'S THE BEEF?????'",
        "$n pokes $N, and exclaims, 'WHERE'S THE BEEF????'",
        "$n pokes you and exclaims, 'WHERE'S THE BEEF????'",
        "You poke your fat rolls and exclaim, 'Oh, THERE'S THE BEEF!'",
        "$n pokes $s fat rolls, and exclaims, 'Oh, THERE'S THE BEEF!'"
    },

    {
        "beer",
        "You down a cold, frosty beer.",
        "$n downs a cold, frosty beer.",
        "You draw a cold, frosty beer for $N.",
        "$n draws a cold, frosty beer for $N.",
        "$n draws a cold, frosty beer for you.",
        "You draw yourself a beer.",
        "$n draws $mself a beer."
    },

    {
        "beg",
        "You beg the gods for mercy.",
        "The gods fall down laughing at $n's request for mercy.",
        "You desperately try to squeeze a few coins from $M.",
        "$n begs you for money.",
        "$n begs $N for a gold piece!",
        "Begging yourself for money doesn't help.",
        "$n begs $mself for money."
    },

    {
        "behead",
        "You look around for some heads to cut off.",
        "$n looks around for some heads to cut off.",
        "You grin evilly at $N and brandish your weapon.",
        "$n grins evilly at $N, while branding $s weapon!",
        "$n grins evilly at you, brandishing $s weapon.",
        "I really don't think you want to do that...",
        "$n is so desperate for exp that $e tries to decapitate $mself!"
    },

    {
        "bkiss",
        "Blow a kiss to whom?",
        "$n blows at $s hand.",
        "You blow a kiss to $M.",
        "$n blows a kiss to $N.  Touching, ain't it?",
        "$n blows a kiss to you.  Not as good as a real one, huh?",
        "You blow a kiss to yourself.",
        "$n blows a kiss to $mself.  Wierd."
    },

    {
        "bleed",
        "You bleed all over the room!",
        "$n bleeds all over the room!  Get out of $s way!",
        "You bleed all over $M!",
        "$n bleeds all over $N.  Better leave, you may be next!",
        "$n bleeds all over you!  YUCK!",
        "You bleed all over yourself!",
        "$n bleeds all over $mself."
    },

    {
        "blink",
        "You blink in utter disbelief.",
        "$n blinks in utter disbelief.",
        "You blink at $M in confusion.",
        "$n blinks at $N in confusion.",
        "$n blinks at you in confusion.",
        "You are sooooooooooooo confused",
        "$n blinks at $mself in complete confusion."
    },

    /* This social is for Stephen of ACK mud :) */
    {
        "blownose",
        "You blow your nose loudly.",
        "$n blows $s nose loudly.",
        "You blow your nose on $S shirt.",
        "$n blows $s nose on $N's shirt.",
        "$n blows $s nose on your shirt.",
        "You blow your nose on your shirt.",
        "$n blows $s nose on $s shirt."
    },

    {
        "blush",
        "Your cheeks are burning.",
        "$n blushes.",
        "You get all flustered up seeing $M.",
        "$n blushes as $e sees $N here.",
        "$n blushes as $e sees you here.  Such an effect on people!",
        "You blush at your own folly.",
        "$n blushes as $e notices $s boo-boo."
    },

    {
        "boggle",
        "You boggle at all the loonies around you.",
        "$n boggles at all the loonies around $m.",
        "You boggle at $S ludicrous idea.",
        "$n boggles at $N's ludicrous idea.",
        "$n boggles at your ludicrous idea.",
        "BOGGLE.",
        "$n wonders what BOGGLE means."
    },

    {
        "bonk",
        "BONK.",
        "$n spells 'potato' like Dan Quayle: 'B-O-N-K'.",
        "You bonk $M for being a numbskull.",
        "$n bonks $N.  What a numbskull.",
        "$n bonks you.  BONK BONK BONK!",
        "You bonk yourself.",
        "$n bonks $mself."
    },

    {
        "boogie",
        "You boogie down!",
        "$n gets down and boogies!!",
        "You grab $M and boogie down!",
        "$n grabs $N and they boogie down!",
        "$n grabs you and boogies with you!",
        "You boogie with yourself.  What a great dancer you are!",
        "$n boogies with $mself.  Guess no one will dance with $m."
    },

    {
        "bottle",
        "You open up a cold bottle of brew.",
        "$n opens a cold bottle of brew.",
        "You open up a cold bottle of brew for $M.",
        "$n opens a cold bottle of brew for $N.",
        "$n opens a cold bottle of brew for you.",
        "You open a cold bottle of brew for yourself.",
        "$n opens a cold bottle of brew for $mself."
    },

    {
        "bounce",
        "BOIINNNNNNGG!",
        "$n bounces around.",
        "You bounce onto $S lap.",
        "$n bounces onto $N's lap.",
        "$n bounces onto your lap.",
        "You bounce your head like a basketball.",
        "$n plays basketball with $s head."
    },

    {
        "bow",
        "You bow deeply.",
        "$n bows deeply.",
        "You bow before $M.",
        "$n bows before $N.",
        "$n bows before you.",
        "You kiss your toes.",
        "$n folds up like a jack knife and kisses $s own toes."
    },

    {
        "brb",
        "You announce that you will be right back.",
        "$n says in a stern voice, 'I'll be back!'",
        "You announce to $M that you will be right back.",
        "$n says to $N in a stern voice, 'I'll be back!'",
        "$n says to you in a stern voice, 'I'll be right back!'",
        "You mumble to yourself, 'I'll be right back'",
        "$n mumbles to $mself, 'I'll be right back, won't I?'"
    },

    {
        "brush",
        "Brush what? Who? Where?",
        NULL,
        "You brush out $S hair for $M.  Very thoughtful.",
        "$n brushes $N's hair for $M.  Looks better now.",
        "$n brushes out your hair.  How nice of $m.",
        "You brush out your hair.  There - much better.",
        "$n brushes out $s hair.  Looks much better now."
    },

    {
        "bully",
        "You growl and demand everyone's lunch money NOW!",
        "$n growls and demands everyone's lunch money.",
        "You growl and demand $S lunch money NOW!",
        "$n growls and demands $N's lunch money.",
        "$n growls and demands your lunch money.",
        "You bully yourself, and take your own lunch money.",
        "$n bullies $mself, and takes $s lunch money away from $mself???"
    },

    {
        "bungy",
        "You tie a bungy cord to the mud and jump into internet.",
        "$n ties a bungy cord to the mud and jumps into internet.",
        "You tie a bungy cord to $N and throw $M off the mud.",
        "$n ties a bungy cord to $N and throws $M off the mud.",
        "$n ties a bungy cord to you and throws you off the mud.",
        "You tie a bungy cord to yourself and jump off the mud.",
        "$n ties a bungy cord to $mself and jumps off the mud."
    },

    {
        "burp",
        "You burp loudly.",
        "$n burps loudly.",
        "You burp loudly to $M in response.",
        "$n burps loudly in response to $N's remark.",
        "$n burps loudly in response to your remark.",
        "You burp at yourself.",
        "$n burps at $mself.  What a sick sight."
    },

    {
        "bye",
        "You say goodbye to all in the room.",
        "$n says goodbye to everyone in the room.",
        "You say goodbye to $N.",
        "$n says goodbye to $N.",
        "$n says goodbye to you.",
        "You say goodbye to yourself.  Contemplating suicide?",
        "$n says goodbye to $mself.  Is $e contemplating suicide?"
    },

    {
        "byron",
        "You recite romantic poems to yourself.",
        "$n mutters romantic poems to himself, must be practicing for someone.",
        "You recite a romantic poem to $N.",
        "$n recites a beautiful romantic poem for $N.",
        "$n recites a heartstoppingly beautiful romantic poem for you.",
        "You read yourself romantic poems.  Falling in love with yourself?",
        "$n reads $mself a romantic poem.  Perhaps he loves $mself?",
    },

    {
        "cackle",
        "You throw back your head and cackle with insane glee!",
        "$n throws back $s head and cackles with insane glee!",
        "You cackle gleefully at $N",
        "$n cackles gleefully at $N.",
        "$n cackles gleefully at you.  Better keep your distance from $m.",
        "You cackle at yourself.  Now, THAT'S strange!",
        "$n is really crazy now!  $e cackles at $mself."
    },

    {
        "cannonball",
        "You cannonball into the pool.   *SPLOOSH*",
        "$n cannonballs into the pool.   *SPLOOSH*",
        "You cannonball into the pool and splash water over $N.",
        "$n cannonballs into the pool and splashes $N.",
        "$n cannonballs into the pool and sends water over you.  You are WET!",
        "You bounce cannonballs off your head to show your strength.",
        "$n bounces cannonballs off $s head.  Can you say 'nutcase'?"
    },

    {
        "catnap",
        "You curl into a tiny ball and go to sleep.",
        "$n curls $mself into a tiny ball and goes to sleep.",
        "You curl up in $S lap and go to sleep.",
        "$n curls up in $N's lap and goes to sleep.",
        "$n curls up in your lap and goes to sleep.",
        "You curl into a tiny ball and go to sleep.",
        "$n curls $mself into a tiny ball and goes to sleep."
    },

    {
        "challenge",
        "Challenge who?",
        NULL,
        "You challenge $N to a fight to the death.",
        "$n challenges $N to a fight to the death.",
        "$n challenges you to a fight to the death.",
        "Challenge YOURSELF to a fight to the death?  I think not...",
        NULL
    },

    {
        "cheer",
        "ZIS BOOM BAH!  BUGS BUNNY BUGS BUNNY RAH RAH RAH!",
        "$n cheers 'BUGS BUNNY BUGS BUNNY RAH RAH RAH!'",
        "You cheer loudly: 'Go $N Go!'",
        "$n cheers loudly: 'Go $N Go!'",
        "$n cheers you on!",
        "You cheer yourself up.",
        "$n cheers $mself on."
    },

    {
        "chekov",
        "You speak your best Russian in hopes of finding a nuclear wessel.",
        "$n asks sheepishly, 'Ver are the nuclear wessels?'",
        "You ask $M about the nuclear wessels.",
        "$n asks $N sheepishly, 'Ver are the nuclear wessels?'",
        "$n asks you sheepishly, 'Ver are the nuclear wessels?'",
        "You look around, muttering, 'Ver are the nuclear wessels?'",
        "$n looks around, muttering, 'Ver are the nuclear wessels?'"
    },

    {
        "chortle",
        "You chortle with glee.",
        "$n chortles with glee.",
        "You chortle loudly at $M.",
        "$n chortles loudly at $N.",
        "$n chortles loudly at you.",
        "You chortle loudly to yourself.",
        "$n chortles loudly to $mself."
    },

    {
        "chuckle",
        "You chuckle politely.",
        "$n chuckles politely.",
        "You chuckle at $S joke.",
        "$n chuckles at $N's joke.",
        "$n chuckles at your joke.",
        "You chuckle at your own joke, since no one else would.",
        "$n chuckles at $s own joke, since none of you would."
    },

    {
        "claphands",
        "You clap your hands together.",
        "$n shows $s approval by clapping $s hands together.",
        "You clap at $S performance.",
        "$n claps at $N's performance.",
        "$n claps at your performance.",
        "You clap at your own performance.",
        "$n claps at $s own performance."
    },

    {
        "clue",
        "You mumble 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "You mumble 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... $N, aha, a clue!'",
        "You mumble to yourself, 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles to $mself, 'DUM-DUM-DUM-DUM ... ah, a clue!'"
    },

    {
        "clueless",
        "You chastise everyone in the room for being clueless.",
        "$n chastises everyone for being clueless.",
        "You chastise $M for being clueless.",
        "$n chastises $N for being clueless.",
        "$n chastises you for being clueless.",
        "You chastise yourself for being clueless.  What a dweeb you are!",
        "$n chastises $mself for being clueless.  What a dweeb!"
    },

    {
        "comb",
        "You comb your hair - perfect.",
        "$n combs $s hair, how dashing!",
        "You patiently untangle $N's hair - what a mess!",
        "$n tries patiently to untangle $N's hair.",
        "$n pulls your hair in an attempt to comb it.",
        "You pull your hair, but it will not be combed.",
        "$n tries to comb $s tangled hair."
    },

    {
        "comfort",
        "Do you feel uncomfortable?",
        NULL,
        "You comfort $M.",
        "$n comforts $N.",
        "$n comforts you.",
        "You make a vain attempt to comfort yourself.",
        "$n has no one to comfort $m but $mself."
    },

    {
        "cough",
        "You cough to clear your throat and eyes and nose and....",
        "$n coughs loudly.",
        "You cough loudly.  It must be $S fault, $E gave you this cold.",
        "$n coughs loudly, and glares at $N, like it is $S fault.",
        "$n coughs loudly, and glares at you.  Did you give $m that cold?",
        "You cough loudly.  Why don't you take better care of yourself?",
        "$n coughs loudly.  $n should take better care of $mself."
    },

    {
        "cower",
        "What are you afraid of?",
        "$n cowers in the corner from claustrophobia.",
        "You cower in the corner at the sight of $M.",
        "$n cowers in the corner at the sight of $N.",
        "$n cowers in the corner at the sight of you.",
        "You cower in the corner at the thought of yourself.  You scaredy cat!",
        "$n cowers in the corner.  What is wrong with $m now?"
    },

    {
        "cringe",
        "You cringe in terror.",
        "$n cringes in terror!",
        "You cringe away from $M.",
        "$n cringes away from $N in mortal terror.",
        "$n cringes away from you.",
        "I beg your pardon?",
        NULL
    },

    {
        "crush",
        "You squint and hold two fingers up, saying 'I'm crushing your heads!'",
        "$n squints and holds two fingers up, saying 'I'm crushing your heads!'",
        "You hold two fingers up at $M and say, 'I'm crushing your head!'",
        "$n holds two fingers up at $N and says, 'I'm crushing your head!'",
        "$n holds two fingers up at you and says, 'I'm crushing your head!'",
        "You crush yourself.  YEEEEOOOUUUUCH!",
        "$n crushes $mself into the ground.  OUCH!"
    },

    {
        "cry",
        "Waaaaah ...",
        "$n bursts into tears.",
        "You cry on $S shoulder.",
        "$n cries on $N's shoulder.",
        "$n cries on your shoulder.",
        "You cry to yourself.",
        "$n sobs quietly to $mself."
    },

    {
        "cuddle",
        "Whom do you feel like cuddling today?",
        NULL,
        "You cuddle $M.",
        "$n cuddles $N.",
        "$n cuddles you.",
        "You must feel very cuddly indeed ... :)",
        "$n cuddles up to $s shadow.  What a sorry sight."
    },

    {
        "curse",
        "You swear loudly for a long time.",
        "$n swears: @*&^%@*&!",
        "You swear at $M.",
        "$n swears at $N.",
        "$n swears at you!  Where are $s manners?",
        "You swear at your own mistakes.",
        "$n starts swearing at $mself.  Why don't you help?"
    },

    {
        "curtsey",
        "You curtsey to your audience.",
        "$n curtseys gracefully.",
        "You curtsey to $M.",
        "$n curtseys gracefully to $N.",
        "$n curtseys gracefully for you.",
        "You curtsey to your audience (yourself).",
        "$n curtseys to $mself, since no one is paying attention to $m."
    },

    {
        "dance",
        "Feels silly, doesn't it?",
        "$n tries to break dance, but nearly breaks $s neck!",
        "You sweep $M into a romantic waltz.",
        "$n sweeps $N into a romantic waltz.",
        "$n sweeps you into a romantic waltz.",
        "You skip and dance around by yourself.",
        "$n dances a pas-de-une."
    },

    {
        "devour",
        "You reach out and snare a passing bovine.  You devour the cow in a single gargantuan bite!",
        "$n reaches out and snare a passing bovine.  $e devours the cow in a single gargantuan bite!",
        "You stare at $M and lick your lips hungrily.",
        "$n stares at $M and licks $s lips hungrily.",
        "$n stares at you and licks $s lips hungrily.",
        "You must be really HUNGRY!",
        "$n drools gratuitously."
    },

    {
        "dive",
        "You dive into the ocean.",
        "$n dives into the ocean.",
        "You dive behind $M and hide.",
        "$n dives behind $N and hides.",
        "$n dives behind you and hides.",
        "You take a dive.",
        "$n takes a dive."
    },

    {
        "dizzy",
        "You are so dizzy from all this chatter.",
        "$n spins twice and hits the ground, dizzy from all this chatter.",
        "You are dizzy from all of $N's chatter.",
        "$n spins twice and hits the ground, dizzy from all $N's chatter.",
        "$n spins twice and hits the ground, dizzy from all your chatter.",
        "You are dizzy from lack of air.  Don't talk so much!",
        "$n spins twice and falls to the ground from lack of air."
    },

    {
        "doc",
        "You nibble on a carrot and say 'Eh, what's up Doc?'",
        "$n nibbles on a carrot and says 'Eh, what's up Doc?'",
        "You nibble on a carrot and say to $M, 'Eh, what's up Doc?'",
        "$n nibbles on a carrot and says to $N, 'Eh, what's up Doc?'",
        "$n nibbles on a carrot and says to you, 'Eh, what's up Doc?'",
        "You nibble on a carrot and say to yourself, 'Eh, what's up $n?'",
        "$n nibbles on a carrot and says 'Eh, what's up Doc?'"
    },

    {
        "doh",
        "You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
        "$n hits $mself in the forehead and says, 'Doh!!!'",
        "You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
        "$n hits $mself in the forehead and says, 'Doh!!!'",
        "$n hits $mself in the forehead and says, 'Doh!!!'",
        "You hit yourself in the forehead and say, 'Doh!!!'",
        "$n hits $mself in the forehead and says, 'Doh!!!'"
    },

    /*
     * This one's for Baka, Penn, and Onethumb!
     */
    {
        "drool",
        "You drool on yourself.",
        "$n drools on $mself.",
        "You drool all over $N.",
        "$n drools all over $N.",
        "$n drools all over you.",
        "You drool on yourself.",
        "$n drools on $mself."
    },

    {
        "duck",
        "Whew!  That was close!",
        "$n is narrowly missed by a low-flying dragon.",
        "You duck behind $M.  Whew!  That was close!",
        "$n ducks behind $N to avoid the fray.",
        "$n ducks behind you to avoid the fray.",
        "You duck behind yourself.  Oww that hurts!",
        "$n tries to duck behind $mself.  $n needs help getting untied now."
    },

    {
        "dying",
        "You start yelling for a heal!",
        "$n yells 'Hey, how about a heal? I'm DYING here!'",
        "You start yelling at $N for a heal!",
        "$n yells 'Hey $N, how about a heal? I'm DYING here!'",
        "$n yells 'Hey $N, how about a heal? I'm DYING here!'",
        "You start yelling for a heal!",
        "$n yells 'Hey, how about a heal? I'm DYING here!'"
    },

    {
        "elt",
        "You announce proudly, 'Hi, I'm $n, and I'm an Experimental Love Torpedo!'",
        "$n stands and says, 'Hi, I'm $n, and I'm an Experimental Love Torpedo.'",
        "You tell $M that $E looks like an Experimental Love Torpedo.",
        "$n tells $N that $e looks like an Experimental Love Torpedo.",
        "$n tells you that you look like an Experimental Love Torpedo.",
        "You announce proudly, 'Hi, I'm $n, and I'm an Experimental Love Torpedo!'",
        "$n stands and says, 'Hi, I'm $n, and I'm an Experimental Love Torpedo.'",
    },

    {
        "eskimo",
        "Who do you want to eskimo kiss with?",
        "$n is looking for someone to rub noses with.  Any volunteers?",
        "You rub noses with $M.",
        "$n rubs noses with $N.  You didn't know they were eskimos!",
        "$n rubs noses with you.",
        "You ponder the difficulties involved in rubbing noses with yourself.",
        "$n ponders the difficulties involved in rubbing noses with $mself."
    },

    {
        "evilgrin",
        "You grin so evilly that everyones alignment drops to -1000.",
        "$n grins evilly that everyones alignment drops to -1000.",
        "You grin so evilly at $M that $S alignment drops to -1000.",
        "$n grins so evilly at $N that $S alignment drops to -1000.",
        "$n grins so evilly at you that your alignment drops to -1000.",
        "You grin so evilly at yourself that your alignment drops to -1000.",
        "$n grins so evilly that $s alignment drops to -1000."
    },

    {
        "eyebrow",
        "You raise an eyebrow.",
        "$n raises an eyebrow.",
        "You raise an eyebrow at $M.",
        "$n raises an eyebrow at $N.",
        "$n raises an eyebrow at you.",
        "You raise an eyebrow at yourself.  That hurt!",
        "$n raises an eyebrow at $mself.  That must have hurt!"
    },

    {
        "faint",
        "You feel dizzy and hit the ground like a board.",
        "$n's eyes roll back in $s head and $e crumples to the ground.",
        "You faint into $S arms.",
        "$n faints into $N's arms.",
        "$n faints into your arms.  How romantic.",
        "You look down at your condition and faint.",
        "$n looks down at $s condition and faints dead away."
    },

    {
        "fakerep",
        "You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
        "$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp."
    },

    {
        "fart",
        "Where are your manners?",
        "$n lets off a real rip-roarer ... a greenish cloud envelops $n!",
        "You fart at $M.  Boy, you are sick.",
        "$n farts in $N's direction.  Better flee before $e turns to you!",
        "$n farts in your direction.  You gasp for air.",
        "You fart at yourself.  You deserve it.",
        "$n farts at $mself.  Better $m than you."
    },

    {
        "flash",
        "You flash your naked body at the gawking crowd.",
        "$n flashes $s naked body at everyone.  Gasp!",
        "You flash your naked body at $M.",
        "$n flashes $s naked body at $N.  Aren't you jealous?",
        "$n flashes $s naked body at you.  Everyone else is jealous.",
        "You flash your naked body at yourself.  How strange.",
        "$n is looking down $s shirt and grinning.  Very wierd!"
    },

    {
        "flip",
        "You flip head over heels.",
        "$n flips head over heels.",
        "You flip $M over your shoulder.",
        "$n flips $N over $s shoulder.",
        "$n flips you over $s shoulder.  Hmmmm.",
        "You tumble all over the room.",
        "$n does some nice tumbling and gymnastics."
    },

    {
        "flirt",
        "Wink wink!",
        "$n flirts -- probably needs a date, huh?",
        "You flirt with $M.",
        "$n flirts with $N.",
        "$n wants you to show some interest and is flirting with you.",
        "You flirt with yourself.",
        "$n flirts with $mself.  Hoo boy."
    },

    {
        "flutter",
        "You flutter your eyelashes.",
        "$n flutters $s eyelashes.",
        "You flutter your eyelashes at $M.",
        "$n flutters $s eyelashes in $N's direction.",
        "$n looks at you and flutters $s eyelashes.",
        "You flutter your eyelashes at the thought of yourself.",
        "$n flutters $s eyelashes at no one in particular."
    },

    {
        "fondle",
        "Who needs to be fondled?",
        NULL,
        "You fondly fondle $M.",
        "$n fondly fondles $N.",
        "$n fondly fondles you.",
        "You fondly fondle yourself, feels funny doesn't it?",
        "$n fondly fondles $mself - this is going too far!!"
    },

    {
        "french",
        "Kiss whom?",
        NULL,
        "You give $N a long and passionate kiss.",
        "$n kisses $N passionately.",
        "$n gives you a long and passionate kiss.",
        "You gather yourself in your arms and try to kiss yourself.",
        "$n makes an attempt at kissing $mself."
    },

    {
        "frown",
        "What's bothering you ?",
        "$n frowns.",
        "You frown at what $E did.",
        "$n frowns at what $E did.",
        "$n frowns at what you did.",
        "You frown at yourself.  Poor baby.",
        "$n frowns at $mself.  Poor baby."
    },

    {
        "fume",
        "You grit your teeth and fume with rage.",
        "$n grits $s teeth and fumes with rage.",
        "You stare at $M, fuming.",
        "$n stares at $N, fuming with rage.",
        "$n stares at you, fuming with rage!",
        "That's right - hate yourself!",
        "$n clenches $s fists and stomps his feet, fuming with anger."
    },

    {
        "garth",
        "You will give your weapons away....NOT.",
        "$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
        "You yell 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
        "$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
        "$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
        "You yell 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
        "$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'"
    },

    {
        "gasp",
        "You gasp in astonishment.",
        "$n gasps in astonishment.",
        "You gasp as you realize what $E did.",
        "$n gasps as $e realizes what $N did.",
        "$n gasps as $e realizes what you did.",
        "You look at yourself and gasp!",
        "$n takes one look at $mself and gasps in astonisment!"
    },

    {
        "gawk",
        "You gawk at everyone around you.",
        "$n gawks at everyone in the room.",
        "You gawk at $M.",
        "$n gawks at $N.",
        "$n gawks at you.",
        "You gawk as you think what you must look like to others.",
        "$n is gawking again.  What is on $s mind?"
    },

    {
        "german",
        "You speak your best German in hopes of getting a beer.",
        "$n says 'Du bist das hundchen!'  What the hell?",
        "You speak your best German to $M in hopes of getting a beer.",
        "$n says to $N, 'Du bist das hundchen!'  What the hell?",
        "$n says to you, 'Du bist das hundchen!'  What the hell?",
        "You speak your best German in hopes of getting a beer.",
        "$n says 'Du bist das hundchen!'  What the hell?"
    },

    {
        "ghug",
        "GROUP HUG!  GROUP HUG!",
        "$n hugs you all in a big group hug.  How sweet!",
        "GROUP HUG!  GROUP HUG!",
        "$n hugs you all in a big group hug.  How sweet!",
        "$n hugs you all in a big group hug.  How sweet!",
        "GROUP HUG!  GROUP HUG!",
        "$n hugs you all in a big group hug.  How sweet!"
    },

    {
        "giggle",
        "You giggle.",
        "$n giggles.",
        "You giggle in $S's presence.",
        "$n giggles at $N's actions.",
        "$n giggles at you.  Hope it's not contagious!",
        "You giggle at yourself.  You must be nervous or something.",
        "$n giggles at $mself.  $e must be nervous or something."
    },

    {
        "glare",
        "You glare at nothing in particular.",
        "$n glares around $m.",
        "You glare icily at $M.",
        "$n glares at $N.",
        "$n glares icily at you, you feel cold to your bones.",
        "You glare icily at your feet, they are suddenly very cold.",
        "$n glares at $s feet, what is bothering $m?"
    },

    {
        "goose",
        "You honk like a goose.",
        "$n honks like a goose.",
        "You goose $S luscious bottom.",
        "$n gooses $N's soft behind.",
        "$n squeezes your tush.  Oh my!",
        "You goose yourself.",
        "$n gooses $mself.  Yuck."
    },

    {
        "grimace",
        "You contort your face in disgust.",
        "$n grimaces is disgust.",
        "You grimace in disgust at $M.",
        "$n grimaces in disgust at $N.",
        "$n grimaces in disgust at you.",
        "You grimace at yourself in disgust.",
        "$n grimaces at $mself in disgust."
    },

    {
        "grin",
        "You grin evilly.",
        "$n grins evilly.",
        "You grin evilly at $M.",
        "$n grins evilly at $N.",
        "$n grins evilly at you.  Hmmm.  Better keep your distance.",
        "You grin at yourself.  You must be getting very bad thoughts.",
        "$n grins at $mself.  You must wonder what's in $s mind."
    },

    {
        "groan",
        "You groan loudly.",
        "$n groans loudly.",
        "You groan at the sight of $M.",
        "$n groans at the sight of $N.",
        "$n groans at the sight of you.",
        "You groan as you realize what you have done.",
        "$n groans as $e realizes what $e has done."
    },

    {
        "grope",
        "Whom do you wish to grope?",
        NULL,
        "Well, what sort of noise do you expect here?",
        "$n gropes $N.",
        "$n gropes you.",
        "You grope yourself - YUCK.",
        "$n gropes $mself - YUCK."
    },

    {
        "grovel",
        "You grovel in the dirt.",
        "$n grovels in the dirt.",
        "You grovel before $M.",
        "$n grovels in the dirt before $N.",
        "$n grovels in the dirt before you.",
        "That seems a little silly to me.",
        NULL
    },

    {
        "growl",
        "Grrrrrrrrrr ...",
        "$n growls.",
        "Grrrrrrrrrr ... take that, $N!",
        "$n growls at $N.  Better leave the room before the fighting starts.",
        "$n growls at you.  Hey, two can play it that way!",
        "You growl at yourself.  Boy, do you feel bitter!",
        "$n growls at $mself.  This could get interesting..."
    },

    {
        "grumble",
        "You grumble.",
        "$n grumbles.",
        "You grumble to $M.",
        "$n grumbles to $N.",
        "$n grumbles to you.",
        "You grumble under your breath.",
        "$n grumbles under $s breath."
    },

    {
        "grunt",
        "GRNNNHTTTT.",
        "$n grunts like a pig.",
        "GRNNNHTTTT.",
        "$n grunts to $N.  What a pig!",
        "$n grunts to you.  What a pig!",
        "GRNNNHTTTT.",
        "$n grunts to nobody in particular.  What a pig!"
    },

    {
        "hand",
        "Kiss whose hand?",
        NULL,
        "You kiss $S hand.",
        "$n kisses $N's hand.  How continental!",
        "$n kisses your hand.  How continental!",
        "You kiss your own hand.",
        "$n kisses $s own hand."
    },

    {
        "hangover",
        "You pop a few aspirin and put on your sunglasses.  Ow, your head hurts!",
        "$n holds $s head and says 'Quit breathing so loud!'",
        "Won't $N be quiet?  Your head is gonna split in two if $E keeps talking!",
        "$n complains to $N 'Be quiet!  I have a hangover!",
        "$n complains to you 'Be quiet!  I have a hangover!",
        "You shoosh yourself.  Be quiet!  You have a hangover!",
        "$n shooshes $mself.  Wow, what a hangover that must be!"
    },

    {
        "happy",
        "You smile wide like Cindy Brady.",
        "$n is as happy as a maggot on a piece of rotten meat.",
        "You smile wide at $M like Cindy Brady.",
        "$n is as happy as a maggot on a piece of rotten meat.",
        "$n is as happy as a maggot on a piece of rotten meat.",
        "You smile wide like Cindy Brady.",
        "$n is as happy as a maggot on a piece of rotten meat."
    },

    {
        "hello",
        "You say hello to everyone in the room.",
        "$n says hello to everyone in the room.",
        "You tell $M how truly glad you are to see $M.",
        "$n tells $N 'Hi!'",
        "$n tells you how truly glad $e is that you are here.",
        "You greet yourself enthusiastically.",
        "$n greets $mself enthusiastically.  How odd."
    },

    {
        "highfive",
        "You jump in the air...oops, better get someone else to join you.",
        "$n jumps in the air by $mself.  Is $e a cheerleader, or just daft?",
        "You jump in the air and give $M a big highfive!",
        "$n jumps in the air and gives $N a big highfive!",
        "$n jumps in the air and gives you a big highfive!",
        "You jump in the air and congratulate yourself!",
        "$n jumps in the air and gives $mself a big highfive!  Wonder what $e did?"
    },

    {
        "hmm",
        "You Hmmmm out loud.",
        "$n thinks, 'Hmmmm.'",
        "You gaze thoughtfully at $M and say 'Hmmm.'",
        "$n gazes thoughtfully at $N and says 'Hmmm.'",
        "$n gazes thoughtfully at you and says 'Hmmm.'",
        "You Hmmmm out loud.",
        "$n thinks, 'Hmmmm.'"
    },

    {
        "hologram",
        "You snap your fingers and create an illusion.",
        "$n leaves south.",
        "You snap your fingers and create an illusion.",
        "$n leaves south.",
        "$n waves at you and leaves south.",
        "You snap your fingers and create an illusion of yourself.",
        "Suddenly, there are 2 $n's standing here!"
    },

    {
        "hop",
        "You hop around like a little kid.",
        "$n hops around like a little kid.",
        "You hop into $N's lap.",
        "$n hops into $N's lap.",
        "$n hops into your lap.",
        NULL,
        NULL
    },

    {
        "hug",
        "Hug whom?",
        NULL,
        "You hug $M.",
        "$n hugs $N.",
        "$n hugs you.",
        "You hug yourself.",
        "$n hugs $mself in a vain attempt to get friendship."
    },

    {
        "hum",
        "Hmm Hmm Hmm Hmmmmmmm.",
        "$n hums like a bee with a chest cold.",
        "You hum a little ditty for $M.  Hmm Hmm Hmm Hmmmmmm.",
        "$n hums a little ditty for $N.  Hmm Hmm Hmm Hmmmmmm.",
        "$n hums a little ditty for you.  Hmm Hmm Hmm Hmmmmmm.",
        "Hmm Hmm Hmmmmmmm.",
        "$n hums like a bee with a chest cold."
    },

    {
        "invite",
        "You speak your best French in hopes of getting lucky.",
        "$n tells you, 'Voulez-vous couche avec moi ce soir?'",
        "You speak your best French to $M in hopes of getting lucky.",
        "$n tells $N, 'Voulez-vous couche avec moi ce soir?'",
        "$n tells you, 'Voulez-vous couche avec moi ce soir?'",
        "You speak your best French in hopes of getting lucky, with yourself???",
        "$n says to $mself, 'Voulez-vous couche avec moi ce soir?'"
    },

    {
        "jsave",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces to you 'Jesus saves!  But Gretsky recovers...he scores!'",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'"
    },

    {
        "kiss",
        "Isn't there someone you want to kiss?",
        NULL,
        "You kiss $M.",
        "$n kisses $N.",
        "$n kisses you.",
        "All the lonely people :(",
        NULL
    },

    {
        "laugh",
        "You laugh.",
        "$n laughs.",
        "You laugh at $N mercilessly.",
        "$n laughs at $N mercilessly.",
        "$n laughs at you mercilessly.  Hmmmmph.",
        "You laugh at yourself.  I would, too.",
        "$n laughs at $mself.  Let's all join in!!!"
    },

    {
        "lag",
        "You complain about the terrible lag.",
        "$n starts complaining about the terrible lag.",
        "You complain to $N about the terrible lag.",
        "$n complains to $N about the terrible lag.",
        "$n complains to you about the terrible lag.",
        "You start muttering about the awful lag.",
        "$n starts muttering about the awful lag."
    },

    {
        "lick",
        "You lick your lips and smile.",
        "$n licks $s lips and smiles.",
        "You lick $M.",
        "$n licks $N.",
        "$n licks you.",
        "You lick yourself.",
        "$n licks $mself - YUCK."
    },

    {
        "love",
        "You love the whole world.",
        "$n loves everybody in the world.",
        "You tell your true feelings to $N.",
        "$n whispers softly to $N.",
        "$n whispers to you sweet words of love.",
        "Well, we already know you love yourself (lucky someone does!)",
        "$n loves $mself, can you believe it ?"
    },

    {
        "lust",
        "You are getting lusty feelings!",
        "$n looks around lustily.",
        "You stare lustily at $N.",
        "$n stares lustily at $N.",
        "$n stares lustily at you.",
        "You stare lustily at...youself?",
        "$n looks $mself up and down lustily."
    },

    {
        "maim",
        "Who do you want to maim?",
        "$n is looking for someone to maim.",
        "You maim $M with your dull fingernails.",
        "$n raises $s hand and tries to maim $N to pieces.",
        "$n raises $s hand and paws at you.  You've been maimed!",
        "You maim yourself with your dull fingernails.",
        "$n raises $s hand and maims $mself to pieces."
    },

    {
        "marvelous",
        "You say 'Ah dahling, you look MAHVELLOUS!'",
        "$n says 'Ah dahling, you look MAHVELLOUS!'",
        "You say to $M, 'Ah dahling, you look MAHVELLOUS!'",
        "$n says to $N, 'Ah dahling, you look MAHVELLOUS!'",
        "$n says to you, 'Ah dahling, you look MAHVELLOUS!'",
        "You say 'Ah dahling, I look MAHVELLOUS!'",
        "$n says 'Ah dahling, I look MAHVELLOUS!'"
    },

    {
        "massage",
        "Massage what?  Thin air?",
        NULL,
        "You gently massage $N's shoulders.",
        "$n massages $N's shoulders.",
        "$n gently massages your shoulders.  Ahhhhhhhhhh ...",
        "You practice yoga as you try to massage yourself.",
        "$n gives a show on yoga positions, trying to massage $mself."
    },

    {
        "meow",
        "MEOW.",
        "$n meows.  What's $e going to do next, wash $mself with $s tongue?",
        "You meow at $M, hoping $E will give you some milk.",
        "$n meows at $N, hoping $E will give $m some milk. ",
        "$n meows at you.  Maybe $e wants some milk.",
        "You meow like a kitty cat.",
        "$n meows like a kitty cat."
    },

    {
        "mmm",
        "You go mmMMmmMMmmMMmm.",
        "$n says 'mmMMmmMMmmMMmm.'",
        "You go mmMMmmMMmmMMmm.",
        "$n says 'mmMMmmMMmmMMmm.'",
        "$n thinks of you and says, 'mmMMmmMMmmMMmm.'",
        "You think of yourself and go mmMMmmMMmmMMmm.",
        "$n thinks of $mself and says 'mmMMmmMMmmMMmm.'",
    },

    {
        "moan",
        "You start to moan.",
        "$n starts moaning.",
        "You moan for the loss of $m.",
        "$n moans for the loss of $N.",
        "$n moans at the sight of you.  Hmmmm.",
        "You moan at yourself.",
        "$n makes $mself moan."
    },

    {
        "mooch",
        "You beg for money, weapons, coins.",
        "$n says 'Spare change?'",
        "You beg $N for money, weapons, coins.",
        "$n begs you for favors of the insidious type...",
        "$n begs you for favors of the insidious type...",
        "You beg for money, weapons, coins.",
        "$n says 'Spare change?'"
    },

    {
        "moocow",
        "You make cow noises.  Mooooooooooooooooooo!",
        "$n Mooooooooooooooooooooooooos like a cow.",
        "You make cow noises at $M.  Mooooooooooooooooooo!",
        "$n Mooooooooooooooooooooooooos like a cow at $N.",
        "$n looks at you and Mooooooooooooooooooooooooos like a cow.",
        "You make cow noises.  Mooooooooooooooooooo!",
        "$n Mooooooooooooooooooooooooos like a cow."
    },

    {
        "moon",
        "Gee your butt is big.",
        "$n moons the entire room.",
        "You show your big butt to $M.",
        "$n shows $s big butt to $N.  Find a paddle, quick!",
        "$n shows $s big ugly butt to you.  How do you like it?",
        "You moon yourself.",
        "$n moons $mself.  Ugliest butt you ever saw."
    },

    {
        "mosh",
        "You MOSH insanely about the room.",
        "$n MOSHES insanely about the room.",
        "You MOSH $M into a pile of Jello pudding.  Ewww!",
        "$n MOSHES $N into Jello pudding.  Ah, the blood!",
        "$n MOSHES you into the ground.",
        "You MOSH yourself.  Ah the blood!",
        "$n MOSHES and MOSHES and MOSHES and MOSHES..."
    },

    {
        "muhaha",
        "You laugh diabolically.  MUHAHAHAHAHAHA!",
        "$n laughs diabolically.  MUHAHAHAHAHAHA!",
        "You laugh at $M diabolically.  MUHAHAHAHAHAHA!",
        "$n laughs at $N diabolically.  MUHAHAHAHAHAHA!",
        "$n laughs at you diabolically.  MUHAHAHAHAHAHA!",
        "Muhaha at yourself??  Wierd.",
        NULL
    },

    {
        "mwalk",
        "You grab your hat and moonwalk across the room.",
        "$n grabs $s black hat and sparkly glove and moonwalks across the room.",
        "You grab your hat and moonwalk across the room.",
        "$n grabs $s black hat and sparkly glove and moonwalks across the room.",
        "$n grabs $s black hat and sparkly glove and moonwalks across the room.",
        "You grab your hat and moonwalk across the room.",
        "$n grabs $s black hat and sparkly glove and moonwalks across the room."
    },

    {
        "nail",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails.",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails.",
        "$n nibbles nervously on your fingernails.  Yuck!",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails."
    },

    {
        "nasty",
        "You do the best imitation of the nasty you can.",
        "$n does the nasty solo? -- wow.",
        "You do the nasty with $M.",
        "$n does the nasty with $N.  Find a firehose quick!",
        "$n does the nasty with you.  How do you like it?",
        "You do the nasty with yourself.",
        "$n does the nasty with $mself.  Hoo boy."
    },

    {
        "newbie",
        "You inform everyone that you're a newbie.",
        "$n says 'Hi, I'm a newbie, please help!'",
        "You ask $N to help you because you're a newbie.",
        "$n asks $N to help $m, because $e's a newbie.",
        "$n asks you to help $m, because $e's a newbie.",
        NULL,
        NULL
    },

    {
        "nibble",
        "Nibble on whom?",
        NULL,
        "You nibble on $N's ear.",
        "$n nibbles on $N's ear.",
        "$n nibbles on your ear.",
        "You nibble on your OWN ear.",
        "$n nibbles on $s OWN ear."
    },

    {
        "nod",
        "You nod affirmative.",
        "$n nods affirmative.",
        "You nod in recognition to $M.",
        "$n nods in recognition to $N.",
        "$n nods in recognition to you.  You DO know $m, right?",
        "You nod at yourself.  Are you getting senile?",
        "$n nods at $mself.  $e must be getting senile."
    },

    {
        "nose",
        "You wiggle your nose.",
        "$n wiggles $s nose.",
        "You tweek $S nose.",
        "$n tweeks $N's nose.",
        "$n tweeks your nose.",
        "You tweek your own nose!",
        "$n tweeks $s own nose!"
    },

    {
        "nudge",
        "Nudge whom?",
        NULL,
        "You nudge $M.",
        "$n nudges $N.",
        "$n nudges you.",
        "You nudge yourself, for some strange reason.",
        "$n nudges $mself, to keep $mself awake."
    },

    {
        "nuke",
        "Who do you want to nuke?",
        "$n tries to put something in the microwave.",
        "You envelop $M in a mushroom cloud.",
        "$n detonates a nuclear warhead at $N.  Uh oh!",
        "$n envelops you in a mushroom cloud.",
        "You nuke yourself.",
        "$n puts $mself in the microwave."
    },

    {
        "nuzzle",
        "Nuzzle whom?",
        NULL,
        "You nuzzle $S neck softly.",
        "$n softly nuzzles $N's neck.",
        "$n softly nuzzles your neck.",
        "I'm sorry, friend, but that's impossible.",
        NULL
    },

    {
        "ogle",
        "Whom do you want to ogle?",
        NULL,
        "You ogle $M like $E was a piece of meat.",
        "$n ogles $N.  Maybe you should leave them alone for awhile?",
        "$n ogles you.  Guess what $e is thinking about?",
        "You ogle yourself.  You may just be too wierd for this mud.",
        "$n ogles $mself.  Better hope that $e stops there."
    },

    {
        "ohno",
        "Oh no!  You did it again!",
        "Oh no!  $n did it again!",
        "You exclaim to $M, 'Oh no!  I did it again!'",
        "$n exclaims to $N, 'Oh no!  I did it again!'",
        "$n exclaims to you, 'Oh no!  I did it again!'",
        "You exclaim to yourself, 'Oh no!  I did it again!'",
        "$n exclaims to $mself, 'Oh no!  I did it again!'"
    },

    {
        "oink",
        "You make pig noises.  OINK!",
        "$n oinks 'OINK OINK OINK!'",
        "You make pig noises at $M.  OINK!",
        "$n oinks at $N: 'OINK OINK OINK!'",
        "$n oinks at you: 'OINK OINK OINK!'",
        "You make pig noises.  OINK!",
        "$n oinks 'OINK OINK OINK!'"
    },

    {
        "ooo",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'",
        "$n thinks of you and says, 'ooOOooOOooOOoo.'",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'"
    },

    {
        "pathead",
        "Pat whom?",
        NULL,
        "You pat $N on $S head.",
        "$n pats $N on $S head.",
        "$n pats you on your head.",
        "You pat yourself on your head.",
        "$n pats $mself on the head."
    },

    {
        "peck",
        "You peck for seeds on the ground.",
        "$n pecks for seeds on the ground.",
        "You give $M a little peck on the cheek.",
        "$n gives $N a small peck on the cheek.",
        "$n gives you a sweet peck on the cheek.",
        "You kiss your own pectoral muscles.",
        "$n pecks $mself on $s pectoral muscles."
    },

    {
        "peer",
        "You peer intently about your surroundings.",
        "$n peers intently about the area, looking for thieves no doubt.",
        "You peer at $M quizzically.",
        "$n peers at $N quizzically.",
        "$n peers at you quizzically.",
        "You peer intently about your surroundings.",
        "$n peers intently about the area, looking for thieves no doubt."
    },

    {
        "pinch",
        "You toss a pinch of salt over your shoulder.",
        "$n tosses a pinch of salt over $s shoulder.",
        "You pinch $S rosy cheeks.",
        "$n pinches $N's rosy cheeks.",
        "$n pinches your chubby cheeks.",
        "You need a pinch of salt.",
        "$n needs a pinch of salt."
    },

    {
        "ping",
        "Ping who?",
        NULL,
        "You ping $N.  $N is unreachable.",
        "$n pings $N.  $N is unreachable.",
        "$n pings you.  Is your server dead or alive?",
        "You ping yourself. Ooof!",
        "$n pings $mself. Ooof!"
    },

    {
        "point",
        "Point at whom?",
        NULL,
        "You point at $M accusingly.",
        "$n points at $N accusingly.",
        "$n points at you accusingly.",
        "You point proudly at yourself.",
        "$n points proudly at $mself."
    },

    {
        "poke",
        "Poke whom?",
        NULL,
        "You poke $M in the ribs.",
        "$n pokes $N in the ribs.",
        "$n pokes you in the ribs.",
        "You poke yourself in the ribs, feeling very silly.",
        "$n pokes $mself in the ribs, looking very sheepish."
    },

    {
        "ponder",
        "You ponder the question.",
        "$n sits down and thinks deeply.",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },

    {
        "pootle",
        "You pootle across the room, attracting attention to yourself.",
        "$n pootles across the room, attracting attention to $mself.",
        "You pootle up to $N and ask 'HEY!  Wanna buy a naked mole rat?'.",
        "$n pootles up to $N and asks 'HEY!  Wanna buy a naked mole rat?'.",
        "$n pootles up to you and asks 'HEY!  Wanna buy a naked mole rat?'.",
        "You pootle around in circles, getting dizzy.",
        "$n pootles around in circles, making $mself dizzy.",
    },

    {
        "possum",
        "You do your best imitation of a corpse.",
        "$n hits the ground... DEAD.",
        "You do your best imitation of a corpse.",
        "$n hits the ground... DEAD.",
        "$n hits the ground... DEAD.",
        "You do your best imitation of a corpse.",
        "$n hits the ground... DEAD."
    },

    {
        "poupon",
        "You say 'Pardon me, do you have any Grey Poupon?'.",
        "$n says 'Pardon me, do you have any Grey Poupon?'",
        "You say to $M, 'Pardon me, do you have any Grey Poupon?'.",
        "$n says to $N, 'Pardon me, do you have any Grey Poupon?'",
        "$n says to you, 'Pardon me, do you have any Grey Poupon?'",
        "You say 'Pardon me, do you have any Grey Poupon?'.",
        "$n says 'Pardon me, do you have any Grey Poupon?'"
    },

    {
        "pout",
        "Ah, don't take it so hard.",
        "$n pouts.",
        "You pout at $M.",
        "$n pouts at $N.",
        "$n pouts at you.",
        "Ah, don't take it so hard.",
        "$n pouts."
    },

    {
        "pretend",
        "You pretend you are a GOD, and slay everyone in sight!",
        "$n is pretending $e is an implementor again.  *sigh*",
        "You pretend you are a GOD, and demote $M to level 1.",
        "$n pretends $e is a GOD, and says, '$N, you're demoted to level 1!'",
        "$n pretends $e is a GOD, and says, 'You are demoted to level 1!'",
        "You pretend you are an implementor, and you demote yourself to level 1.",
        "$n pretends $e is a GOD, and demotes $mself to level 1."
    },

    {
        "prod",
        "Yeah, great, but WHO do you want to prod? Orphus?",
        NULL,
        "You pretend $N is Orphus, and prod $M firmly in the belly.",
        "$n pretend that $N is Orphus, and prods $M firmly in the belly.",
        "$n pretends that you are Orphus, and prods you firmly in the belly.",
        "You pretend you are Orphus, and prod yourself in the belly.",
        "$n pretends $e is Orphus, and prod's $mself in the belly."
    },

    {
        "puke",
        "You puke ... chunks everywhere!",
        "$n pukes.",
        "You puke on $M.",
        "$n pukes on $N.",
        "$n spews vomit and pukes all over your clothing!",
        "You puke on yourself.",
        "$n pukes on $s clothes."
    },

    {
        "purr",
        "MMMMEEEEEEEEOOOOOOOOOWWWWWWWWWWWW.",
        "$n purrs contentedly.",
        "You purr contentedly in $M lap.",
        "$n purrs contentedly in $N's lap.",
        "$n purrs contentedly in your lap.",
        "You purr at yourself.",
        "$n purrs at $mself.  Must be a cat thing."
    },

    {
        "raise",
        "You raise your hand in response.",
        "$n raises $s hand in response.",
        "You raise your hand in response.",
        "$n raises $s hand in response.",
        "$n raises $s hand in response to you.",
        "You raise your hand in response.",
        "$n raises $s hand in response."
    },

    {
        "renandstimpy",
        "You say, 'Oh Happy Happy, Joy Joy!'",
        "$n exclaims, 'Oh Happy Happy, Joy Joy!'",
        "You exclaim, 'Oh Happy Happy, Joy Joy!' at the mere thought of $M.",
        "$n exclaims, 'Oh Happy Happy, Joy Joy!' as $e sees $N enter the room.",
        "$n exclaims, 'Oh Happy Happy, Joy Joy!' when $e sees you approach.",
        "You exclaim, 'Oh Happy Happy, Joy Joy!' at the thought of yourself.",
        "$n exclaims, 'Oh Happy Happy, Joy Joy!' at the thought of $mself."
    },

    {
        "rofl",
        "You roll on the floor laughing hysterically.",
        "$n rolls on the floor laughing hysterically.",
        "You laugh your head off at $S remark.",
        "$n rolls on the floor laughing at $N's remark.",
        "$n can't stop laughing at your remark.",
        "You roll on the floor and laugh at yourself.",
        "$n laughs at $mself.  Join in the fun."
    },

    {
        "rolleyes",
        "You roll your eyes.",
        "$n rolls $s eyes.",
        "You roll your eyes at $M.",
        "$n rolls $s eyes at $N.",
        "$n rolls $s eyes at you.",
        "You roll your eyes at yourself.",
        "$n rolls $s eyes at $mself."
    },

    {
        "roll2",
        "You roll your eyes and make a repetative motion near your crotch.",
        "$n rolls $s eyes and makes a repetative motion near $s crotch.",
        "You roll your eyes at $M and make a repetative motion near your crotch.",
        "$n rolls $s eyes at $N and makes a repetative motion near $s crotch.",
        "$n rolls $s eyes at you and makes a repetative motion near $s crotch.",
        "You make a repetative motion near your crotch...you find you quite like it!",
        "$n starts making a repetative motion near $s crotch...how strange..."
    },

    {
        "rub",
        "You rub your eyes.  How long have you been at this?",
        "$n rubs $s eyes.  $n must have been playing all day.",
        "You rub your eyes.  Has $N been playing as long as you have?",
        "$n rubs $s eyes.  $n must have been playing all day.",
        "$n rubs $s eyes.  Have you been playing as long as $m?",
        "You rub your eyes.  How long have you been at this?",
        "$n rubs $s eyes.  $n must have been playing all day."
    },

    {
        "ruffle",
        "You've got to ruffle SOMEONE.",
        NULL,
        "You ruffle $N's hair playfully.",
        "$n ruffles $N's hair playfully.",
        "$n ruffles your hair playfully.",
        "You ruffle your hair.",
        "$n ruffles $s hair."
    },

    {
        "runaway",
        "You scream 'RUN AWAY! RUN AWAY!'.",
        "$n screams 'RUN AWAY! RUN AWAY!'.",
        "You scream '$N, QUICK! RUN AWAY!'.",
        "$n screams '$N, QUICK! RUN AWAY!'.",
        "$n screams '$N, QUICK! RUN AWAY!'.",
        "You desperately look for somewhere to run to!",
        "$n looks like $e's about to run away.",
    },

    {
        "russian",
        "You speak Russian.  Yeah, right.  Dream on.",
        "$n says 'Ya horosho stari malenky koshka.'  Huh?",
        "You speak Russian to $M.  Yeah, right.  Dream on.",
        "$n says to $N 'Ya horosho stari malenky koshka.'  Huh?",
        "$n says to you 'Ya horosho stari malenky koshka.'  Huh?",
        "You speak Russian.  Yeah, right.  Dream on.",
        "$n says 'Ya horosho stari malenky koshka.'  Huh?"
    },

    {
        "sad",
        "You put on a glum expression.",
        "$n looks particularly glum today.  *sniff*",
        "You give $M your best glum expression.",
        "$n looks at $N glumly.  *sniff*  Poor $n.",
        "$n looks at you glumly.  *sniff*   Poor $n.",
        "You bow your head and twist your toe in the dirt glumly.",
        "$n bows $s head and twists $s toe in the dirt glumly."
    },

    {
        "salute",
        "You salute smartly.",
        "$n salutes smartly.",
        "You salute $M.",
        "$n salutes $N.",
        "$n salutes you.",
        "Huh?",
        NULL
    },

    {
        "sausage",
        "You look around the room, wondering where that sausage jumped.",
        "$n looks around the room for the sausage he just dropped.",
        "You ask $M if $E's seen your jumping sausage.",
        "$n asks $N if $E's seen $s jumping sausage.",
        "$n asks you if you've seen $s jumping sausage.",
        "You ask yourself if you've seen your jumping sausage.",
        "$n asks $mself if $e's seen $s jumping sausage."
    },

    {
        "scowl",
        "You scowl angrilly.",
        "$n scowls angrilly.",
        "You scowl angrilly at $M.",
        "$n scowls angrilly at $N.",
        "$n scowls angrilly at you.",
        "You scowl angrilly at yourself.",
        "$n scowls angrilly at $mself."
    },

    {
        "outcry",
        "ARRRRRRRRRRGH!!!!!",
        "$n screams loudly!",
        "ARRRRRRRRRRGH!!!!!  Yes, it MUST have been $S fault!!!",
        "$n screams loudly at $N.  Better leave before $n blames you, too!!!",
        "$n screams at you!  That's not nice!  *sniff*",
        "You scream at yourself.  Yes, that's ONE way of relieving tension!",
        "$n screams loudly at $mself!  Is there a full moon up?"
    },

    {
        "shake",
        "You shake your head.",
        "$n shakes $s head.",
        "You shake $S hand.",
        "$n shakes $N's hand.",
        "$n shakes your hand.",
        "You are shaken by yourself.",
        "$n shakes and quivers like a bowl full of jelly."
    },

    {
        "shiver",
        "Brrrrrrrrr.",
        "$n shivers uncomfortably.",
        "You shiver at the thought of fighting $M.",
        "$n shivers at the thought of fighting $N.",
        "$n shivers at the suicidal thought of fighting you.",
        "You shiver to yourself?",
        "$n scares $mself to shivers."
    },

    {
        "shrug",
        "You shrug.",
        "$n shrugs helplessly.",
        "You shrug in response to $s question.",
        "$n shrugs in response to $N's question.",
        "$n shrugs in respopnse to your question.",
        "You shrug to yourself.",
        "$n shrugs to $mself.  What a strange person."
    },

    {
        "sigh",
        "You sigh.",
        "$n sighs loudly.",
        "You sigh as you think of $M.",
        "$n sighs at the sight of $N.",
        "$n sighs as $e thinks of you.  Touching, huh?",
        "You sigh at yourself.  You MUST be lonely.",
        "$n sighs at $mself.  What a sorry sight."
    },

    {
        "sing",
        "You raise your clear voice towards the sky.",
        "$n has begun to sing.",
        "You sing a ballad to $m.",
        "$n sings a ballad to $N.",
        "$n sings a ballad to you!  How sweet!",
        "You sing a little ditty to yourself.",
        "$n sings a little ditty to $mself."
    },

    {
        "slap",
        "Slap whom?",
        NULL,
        "You slap $M playfully.",
        "$n slaps $N for $S stupidity.",
        "$n slaps you for your stupidity.  OUCH!",
        "You slap yourself.  You deserve it.",
        "$n slaps $mself.  Why don't you join in?"
    },

    {
        "slobber",
        "You slobber all over the floor.",
        "$n slobbers all over the floor.",
        "You slobber all over $M.",
        "$n slobbers all over $N.",
        "$n slobbers all over you.",
        "You slobber all down your front.",
        "$n slobbers all over $mself."
    },

    {
        "slut",
        "You act like a total slut.",
        "$n lounges about looking like a total slut.",
        "You come on to $N, breathing sensuality.",
        "$n comes on to $N, perhaps he's trying to take Rosario's reputation?",
        "$n comes on to you, $n is like a living embodiment of sensuality.",
        "You sigh, and say 40 partners is average for a 18 yr old, right?",
        "$n sighs, and says 40 partners is about right for an 18 yr old right?"
    },

    {
        "smile",
        "You smile happily.",
        "$n smiles happily.",
        "You smile at $M.",
        "$n beams a smile at $N.",
        "$n smiles at you.",
        "You smile at yourself.",
        "$n smiles at $mself."
    },

    {
        "smirk",
        "You smirk.",
        "$n smirks.",
        "You smirk at $S saying.",
        "$n smirks at $N's saying.",
        "$n smirks at your saying.",
        "You smirk at yourself.  Okay ...",
        "$n smirks at $s own 'wisdom'."
    },

    {
        "smoke",
        "You calmly light a cigarette and take a puff.",
        "$n calmly lights a cigarette and take a puff.",
        "You blow smoke into $S eyes.",
        "$n blows smoke into $N's eyes.",
        "$n blows smoke rings into your eyes.",
        "You call down lightning and SMOKE yourself.",
        "$n calls down lightning and SMOKES $mself."
    },

    {
        "smooch",
        "You are searching for someone to smooch.",
        "$n is looking for someone to smooch.",
        "You give $M a nice, wet smooch.",
        "$n and $N are smooching in the corner.",
        "$n smooches you passionately on the lips.",
        "You smooch yourself.",
        "$n smooches $mself.  Yuck."
    },

    {
        "snap",
        "PRONTO ! You snap your fingers.",
        "$n snaps $s fingers.",
        "You snap back at $M.",
        "$n snaps back at $N.",
        "$n snaps back at you!",
        "You snap yourself to attention.",
        "$n snaps $mself to attention."
    },

    {
        "snarl",
        "You grizzle your teeth and look mean.",
        "$n snarls angrily.",
        "You snarl at $M.",
        "$n snarls at $N.",
        "$n snarls at you, for some reason.",
        "You snarl at yourself.",
        "$n snarls at $mself."
    },

    {
        "sneer",
        "You sneer in contempt.",
        "$n sneers in contempt.",
        "You sneer at $M in contempt.",
        "$n sneers at $N in contempt.",
        "$n sneers at you in contempt.",
        "You sneer at yourself in contempt.",
        "$n sneers at $mself in contempt."
    },

    {
        "sneeze",
        "Gesundheit!",
        "$n sneezes.",
        "You sneeze all over $M.",
        "$n sneezes all over $N.",
        "$n sneezes all over you.",
        NULL,
        NULL
    },

    {
        "snicker",
        "You snicker softly.",
        "$n snickers softly.",
        "You snicker with $M about your shared secret.",
        "$n snickers with $N about their shared secret.",
        "$n snickers with you about your shared secret.",
        "You snicker at your own evil thoughts.",
        "$n snickers at $s own evil thoughts."
    },

    {
        "sniff",
        "You sniff sadly. *SNIFF*",
        "$n sniffs sadly.",
        "You sniff sadly at the way $E is treating you.",
        "$n sniffs sadly at the way $N is treating $m.",
        "$n sniffs sadly at the way you are treating $m.",
        "You sniff sadly at your lost opportunities.",
        "$n sniffs sadly at $mself.  Something MUST be bothering $m."
    },

    {
        "snore",
        "Zzzzzzzzzzzzzzzzz.",
        "$n snores loudly.",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },

    {
        "snort",
        "You snort in disgust.",
        "$n snorts in disgust.",
        "You snort at $M in disgust.",
        "$n snorts at $N in disgust.",
        "$n snorts at you in disgust.",
        "You snort at yourself in disgust.",
        "$n snorts at $mself in disgust."
    },

    {
        "snowball",
        "Whom do you want to throw a snowball at?",
        NULL,
        "You throw a snowball in $N's face.",
        "$n throws a snowball at $N.",
        "$n throws a snowball at you.",
        "You throw a snowball at yourself.",
        "$n throws a snowball at $mself."
    },

    {
        "snuggle",
        "Who?",
        NULL,
        "You snuggle up contentedly in $S lap.",
        "$n snuggles up contentedly in $N's lap.",
        "$n snuggles up contentedly in your lap.",
        "You snuggle up, getting ready to sleep.",
        "$n snuggles up, getting ready to sleep."
    },

    {
        "spam",
        "You mutter 'SPAM' quietly to yourself.",
        "$n sings 'SPAM SPAM SPAM SPAM SPAM SPAM....'",
        "You hurl a can of SPAM at $M.",
        "$n hurls a can of SPAM at $N.",
        "$n SPAMS you viciously.",
        "You mutter 'SPAM' in the corner quietly.",
        "$n mutters 'SPAM SPAM SPAM SPAM'.  Join in!"
    },

    {
        "spanish",
        "You speak Spanish or at least you think you do.",
        "$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
        "You speak Spanish or at least you think you do.",
        "$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
        "$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
        "You speak Spanish or at least you think you do.",
        "$n says to $mself 'Naces pendejo, mueres pendejo.'  Uh oh."
    },

    {
        "spank",
        "Spank whom?",
        NULL,
        "You spank $M playfully.",
        "$n spanks $N playfully.",
        "$n spanks you playfully.  OUCH!",
        "You spank yourself.  Kinky!",
        "$n spanks $mself.  Kinky!"
    },

    {
        "spin",
        "You twirl in a graceful pirouette.",
        "$n twirls in a graceful pirouette.",
        "You spin $M on one finger.",
        "$n spins $N on $s finger.",
        "$n spins you around on $s finger.",
        "You spin yourself around and around and around....",
        "$n spins $mself around and around and around..."
    },

    {
        "squeeze",
        "Where, what, how, whom?",
        NULL,
        "You squeeze $M fondly.",
        "$n squeezes $N fondly.",
        "$n squeezes you fondly.",
        "You squeeze yourself - try to relax a little!",
        "$n squeezes $mself."
    },

    {
        "squeak",
        "You squeak like a mouse.",
        "$n squeaks like a mouse.",
        "You squeak at $M.",
        "$n squeaks at $N.  Is $e a man or a mouse?",
        "$n squeaks at you.  Is $e a man or a mouse?",
        "You squeak at yourself like a mouse.",
        "$n squeaks at $mself like a mouse."
    },

    {
        "squeal",
        "You squeal with delight.",
        "$n squeals with delight.",
        "You squeal at $M.",
        "$n squeals at $N.  Wonder why?",
        "$n squeals at you.  You must be doing something good.",
        "You squeal at yourself.",
        "$n squeals at $mself."
    },

    {
        "squirm",
        "You squirm guiltily.",
        "$n squirms guiltily.  Looks like $e did it.",
        "You squirm in front of $M.",
        "$n squirms in front of $N.",
        "$n squirms in front of you.  You make $m nervous.",
        "You squirm and squirm and squirm....",
        "$n squirms and squirms and squirm....."
    },

    {
        "squish",
        "You squish your toes into the sand.",
        "$n squishes $s toes into the sand.",
        "You squish $M between your legs.",
        "$n squishes $N between $s legs.",
        "$n squishes you between $s legs.",
        "You squish yourself.",
        "$n squishes $mself.  OUCH."
    },

    {
        "stare",
        "You stare at the sky.",
        "$n stares at the sky.",
        "You stare dreamily at $N, completely lost in $S eyes..",
        "$n stares dreamily at $N.",
        "$n stares dreamily at you, completely lost in your eyes.",
        "You stare dreamily at yourself - enough narcissism for now.",
        "$n stares dreamily at $mself - NARCISSIST!"
    },

    {
        "stickup",
        "You don a mask and ask for everyone's gold.",
        "$n says 'This is a stickup.  Gimme yer gold, NOW!'",
        "You don a mask and ask for $S gold.",
        "$n says to $N, 'This is a stickup.  Gimme yer gold, NOW!'",
        "$n says to you, 'This is a stickup.  Gimme yer gold, NOW!'",
        "You extort money from yourself.",
        "$n holds $s weapon to $s throat and says 'Ok, me, give me all my money!'"
    },

    {
        "sticktongue",
        "You stick out your tongue.",
        "$n sticks out $s tongue.",
        "You stick your tongue out at $M.",
        "$n sticks $s tongue out at $N.",
        "$n sticks $s tongue out at you.",
        "You stick out your tongue and touch your nose.",
        "$n sticks out $s tongue and touches $s nose."
    },

    {
        "stretch",
        "You stretch and relax your sore muscles.",
        "$n stretches luxuriously.  Make you want to, doesn't it.",
        "You stretch and relax your sore muscles.",
        "$n stretches luxuriously.  Make you want to, doesn't it.",
        "$n stretches luxuriously.  Make you want to, doesn't it.",
        "You stretch and relax your sore muscles.",
        "$n stretches luxuriously.  Make you want to, doesn't it."
    },

    {
        "strip",
        "You show some of your shoulder as you begin your performance.",
        "$n shows $s bare shoulder and glances seductively around the room.",
        "You show some of your shoulder as you begin your performance.",
        "$n shows $s bare shoulder and glances seductively at $N.",
        "$n shows $s bare shoulder and glances seductively at you.",
        "You show some of your shoulder as you begin your performance.",
        "$n shows $s bare shoulder and glances seductively around the room."
    },

    {
        "strut",
        "Strut your stuff.",
        "$n struts proudly.",
        "You strut to get $S attention.",
        "$n struts, hoping to get $N's attention.",
        "$n struts, hoping to get your attention.",
        "You strut to yourself, lost in your own world.",
        "$n struts to $mself, lost in $s own world."
    },

    {
        "suffer",
        "No xp again?  You suffer at the hands of fate.",
        "$n is suffering.  Looks like $e can't seem to level.",
        "You tell $M how you suffer whenever you're away from $M.",
        "$n tells $N that $e suffers whenever they're apart.",
        "$n tells you that $e suffers whenever you're apart.",
        "No xp again?  You suffer at the hands of fate.",
        "$n is suffering.  Looks like $e can't seem to level."
    },

    {
        "sulk",
        "You sulk.",
        "$n sulks in the corner.",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },

    {
        "surf",
        "You stoke your soul by catching a smooth, perfect wave.",
        "$n stokes $s soul by catching a smooth, perfect wave.",
        "You stoke your soul by catching a smooth, perfect wave.",
        "$n stokes $s soul by catching a smooth, perfect wave.",
        "$n stokes $s soul by catching a smooth, perfect wave.",
        "You stoke your soul by catching a smooth, perfect wave.",
        "$n stokes $s soul by catching a smooth, perfect wave."
    },

    {
        "swoon",
        "You swoon in ecstacy.",
        "$n swoons in ecstacy.",
        "You swoon in ecstacy at the thought of $M.",
        "$n swoons in ecstacy at the thought of $N.",
        "$n swoons in ecstacy as $e thinks of you.",
        "You swoon in ecstacy.",
        "$n swoons in ecstacy."
    },

    {
        "tackle",
        "You can't tackle the AIR!",
        NULL,
        "You run over to $M and bring $M down!",
        "$n runs over to $N and tackles $M to the ground!",
        "$n runs over to you and tackles you to the ground!",
        "You wrap your arms around yourself, and throw yourself to the ground.",
        "$n wraps $s arms around $mself and brings $mself down!?"
    },

    {
        "tap",
        "You tap your foot impatiently.",
        "$n taps $s foot impatiently.",
        "You tap your foot impatiently.  Will $E ever be ready?",
        "$n taps $s foot impatiently as $e waits for $N.",
        "$n taps $s foot impatiently as $e waits for you.",
        "You tap yourself on the head.  Ouch!",
        "$n taps $mself on the head."
    },

    {
        "tender",
        "You will enjoy it more if you choose someone to kiss.",
        NULL,
        "You give $M a soft, tender kiss.",
        "$n gives $N a soft, tender kiss.",
        "$n gives you a soft, tender kiss.",
        "You'd better not, people may start to talk!",
        NULL
    },

    {
        "thank",
        "Thank you too.",
        NULL,
        "You thank $N heartily.",
        "$n thanks $N heartily.",
        "$n thanks you heartily.",
        "You thank yourself since nobody else wants to !",
        "$n thanks $mself since you won't."
    },

    {
        "think",
        "You think about times past and friends forgotten.",
        "$n thinks deeply and is lost in thought.",
        "You think about times past and friends forgotten.",
        "$n thinks deeply and is lost in thought.",
        "$n thinks deeply and is lost in thought.  Maybe $e is thinking of you?",
        "You think about times past and friends forgotten.",
        "$n thinks deeply and is lost in thought."
    },

    {
        "throttle",
        "Whom do you want to throttle?",
        NULL,
        "You throttle $M till $E is blue in the face.",
        "$n throttles $N about the neck, until $E passes out.  THUNK!",
        "$n throttles you about the neck until you pass out.  THUNK!",
        "That might hurt!  Better not do it!",
        "$n is getting a crazy look in $s eye again."
    },

    {
        "tickle",
        "Whom do you want to tickle?",
        NULL,
        "You tickle $N.",
        "$n tickles $N.",
        "$n tickles you - hee hee hee.",
        "You tickle yourself, how funny!",
        "$n tickles $mself."
    },

    {
        "timeout",
        "You take a 'T' and drink some Gatorade (tm).",
        "$n takes a 'T' and drinks some Gatorade (tm).",
        "You take a 'T' and offer $M some Gatorade (tm).",
        "$n takes a 'T' and offers $N some Gatorade (tm).",
        "$n takes a 'T' and offers you some Gatorade (tm).",
        "You take a 'T' and drink some Gatorade (tm).",
        "$n takes a 'T' and drinks some Gatorade (tm)."
    },

    {
        "torture",
        "You have to torture someone!",
        NULL,
        "You torture $M with rusty weapons, Mwaahhhhh!!",
        "$n tortures $N with rusty weapons, $E must have been REAL bad!",
        "$n tortures you with rusty weapons!  What did you DO!?!",
        "You torture yourself with rusty weapons.  Was it good for you?",
        "$n tortures $mself with rusty weapons.  Looks like $e enjoys it!?"
    },

    {
        "tummy",
        "You rub your tummy and wish you'd bought a pie at the bakery.",
        "$n rubs $s tummy and wishes $e'd bought a pie at the bakery.",
        "You rub your tummy and ask $M for some food.",
        "$n rubs $s tummy and asks $N for some food.",
        "$n rubs $s tummy and asks you for some food.  Please?",
        "You rub your tummy and wish you'd bought a pie at the bakery.",
        "$n rubs $s tummy and wishes $e'd bought a pie at the bakery."
    },

    {
        "tweety",
        "You exclaim 'I TAWT I TAW A PUTTY TAT!!'",
        "$n exclaims 'I TAWT I TAW A PUTTY TAT!!'",
        "You exclaim to $M, 'I TAWT I TAW A PUTTY TAT!!'",
        "$n exclaims to $N, 'I TAWT I TAW A PUTTY TAT!!'",
        "$n exclaims to you, 'I TAWT I TAW A PUTTY TAT!!'",
        "You exclaim to yourself, 'I TAWT I TAW A PUTTY TAT!!'",
        "$n exclaims to $mself, 'I TAWT I TAW A PUTTY TAT!!'"
    },

    {
        "twiddle",
        "You patiently twiddle your thumbs.",
        "$n patiently twiddles $s thumbs.",
        "You twiddle $S ears.",
        "$n twiddles $N's ears.",
        "$n twiddles your ears.",
        "You twiddle your ears like Dumbo.",
        "$n twiddles $s own ears like Dumbo."
    },

    {
        "type",
        "You throw up yor handz in dizgust at yur losy typing skils.",
        "$n couldn't type a period if there was only one key on the keyboard.",
        "You throw up yor handz in dizgust at yur losy typing skils.",
        "$n couldn't type a period if there was only one key on the keyboard.",
        "$n couldn't type a period if there was only one key on the keyboard.",
        "You throw up yor handz in dizgust at yur losy typing skils.",
        "$n couldn't type a period if there was only one key on the keyboard."
    },

    {
        "wager",
        "You wager you can name that tune in ONE NOTE.",
        "$n bets $e can name that tune in ONE NOTE.",
        "You wager $M that you can name that tune in ONE NOTE.",
        "$n bets $N that $e can name that tune in ONE NOTE.",
        "$n bets you that $e can name that tune in ONE NOTE.",
        "You wager you can name that tune in ONE NOTE.",
        "$n bets $e can name that tune in ONE NOTE.",
    },

    {
        "wave",
        "You wave.",
        "$n waves happily.",
        "You wave goodbye to $N.",
        "$n waves goodbye to $N.",
        "$n waves goodbye to you.  Have a good journey.",
        "Are you going on adventures as well?",
        "$n waves goodbye to $mself."
    },

    {
        "wedgie",
        "You look around for someone to wedgie.",
        "$n is looking around for someone to wedgie!  Run!",
        "You wedgie $M.  Must be fun! ",
        "$n wedgies $N to the heavens.",
        "$n wedgies you!  Ouch!",
        "You delight in pinning your underwear to the sky.",
        "$n wedgies $mself and revels with glee."
    },

    {
        "whine",
        "You whine like the great whiners of the century.",
        "$n whines 'I want to be a god already.  I need more hitpoints..I...'",
        "You whine to $M like the great whiners of the century.",
        "$n whines to $N 'I want to be an immortal already.  I need more hp...I..'",
        "$n whines to you 'I want to be an immortal already.  I need more hp...I...'",
        "You whine like the great whiners of the century.",
        "$n whines 'I want to be a god already.  I need more hitpoints..I...'"
    },

    {
        "whistle",
        "You whistle appreciatively.",
        "$n whistles appreciatively.",
        "You whistle at the sight of $M.",
        "$n whistles at the sight of $N.",
        "$n whistles at the sight of you.",
        "You whistle a little tune to yourself.",
        "$n whistles a little tune to $mself."
    },

    {
        "wiggle",
        "Your wiggle your bottom.",
        "$n wiggles $s bottom.",
        "You wiggle your bottom toward $M.",
        "$n wiggles $s bottom toward $N.",
        "$n wiggles $s bottom toward you.",
        "You wiggle about like a fish.",
        "$n wiggles about like a fish."
    },

    {
        "wince",
        "You wince.  Ouch!",
        "$n winces.  Ouch!",
        "You wince at $M.",
        "$n winces at $N.",
        "$n winces at you.",
        "You wince at yourself.  Ouch!",
        "$n winces at $mself.  Ouch!"
    },

    {
        "wink",
        "You wink suggestively.",
        "$n winks suggestively.",
        "You wink suggestively at $N.",
        "$n winks at $N.",
        "$n winks suggestively at you.",
        "You wink at yourself ?? - what are you up to ?",
        "$n winks at $mself - something strange is going on..."
    },

    {
        "worship",
        "You worship the powers that be.",
        "$n worships the powers that be.",
        "You drop to your knees in homage of $M.",
        "$n prostrates $mself before $N.",
        "$n believes you are all powerful.",
        "You worship yourself.",
        "$n worships $mself - ah, the conceitedness of it all."
    },

    {
        "wrap",
        "You wrap a present for your love.",
        "$n wraps a present for someone special.",
        "You wrap your legs around $M.",
        "$n wraps $s legs around $N.",
        "$n wraps $s legs around you.  Wonder what's coming next?",
        "You wrap yourself with some paper.",
        "$n wraps $mself with some wrapping paper.  Is it Christmas?"
    },

    {
        "yabba",
        "YABBA-DABBA-DOO!",
        "$n hollers 'Hey Wilma -- YABBA DABBA DOO!'",
        "You holler 'Hey $N -- YABBA DABBA DOO!'",
        "$n hollers 'Hey $N -- YABBA DABBA DOO!'",
        "$n hollers 'Hey $N -- YABBA DABBA DOO!'",
        "YABBA-DABBA-DOO!",
        "$n hollers 'Hey Wilma -- YABBA DABBA DOO!'"
    },

    {
        "yahoo",
        "You do your best Han Solo impression.",
        "$n exclaims, 'YAHOO!  Great shot, kid, that was one in a million!'",
        "You do your best Han Solo impression.",
        "$n exclaims, 'YAHOO!  Great shot, $N, that was one in a million!'",
        "$n exclaims, 'YAHOO!  Great shot, $N, that was one in a million!'",
        "You do your best Han Solo impression.",
        "$n exclaims, 'YAHOO!  Great shot, $n, that was one in a million!'"
    },

    {
        "yawn",
        "You must be tired.",
        "$n yawns.",
        "You must be tired.",
        "$n yawns at $N.  Maybe you should let them go to bed?",
        "$n yawns at you.  Maybe $e wants you to go to bed with $m?",
        "You must be tired.",
        "$n yawns."
    },

    {
        "yeehaw",
        "You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
        "$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
        "You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
        "$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
        "$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
        "You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
        "$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'"
    },

    {
        "",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};


/*
 * The social table.
 * Add new socials here.
 * Alphabetical order is not required.
 */
const	struct	nsocial_type	nsocial_table [] = {
    {
        "n_furry",
        "You boast about your sister's Yak-milking fetish.",
        "$n boasts about $s sister's Yak-milking fetish.",
        "You brutally kick $N in the head, then apologise to $M.",
        "$n brutally kicks $N in the head, then apologises to $M.",
        "$n brutally kicks you in the head, then apologises to you.",
        "You look around for some curtains to tear down.",
        "$n looks around for some curtains to tear down.",
    },

    {
        "n_kavir",
        "You mutter 'Lishen, I'm NOT pished', then fall over.",
        "$n mutters 'Lishen, I'm NOT pished', then falls over.",
        "You mutter 'Lishen $N, I'm NOT pished', then fall over.",
        "$n mutters 'Lishen $N, I'm NOT pished', then falls over.",
        "$n mutters 'Lishen $N, I'm NOT pished', then falls over.",
        "You dry heave for an hour, then ask to be taken home.",
        "$n dry heaves for an hour, then asks to be taken home.",
    },

    {
        "n_orphus",
        "You rise from your coffin with the speed of the undead.",
        "$n rises from $s coffin with the speed of the undead.",
        "You prod $M in the belly.",
        "$n prods $N in the belly.",
        "$n prods you in the belly.",
        "You rise from your coffin with the speed of the undead.",
        "$n rises from $s coffin with the speed of the undead.",
    },

    {
        "n_rosario",
        "You stand up and announce that you're god's gift to women.",
        "$n stands up and announces that $e's god's gift to women.",
        "You inform $M that you're god's gift to women.",
        "$n informs $N that $e's god's gift to women.",
        "$n informs you that $e's god's gift to women.",
        "You stand up and announce that you're god's gift to women.",
        "$n stands up and announces that $e's god's gift to women.",
    },

    {
        "n_rotain",
        "You transform into an Experimental Love Torpedo.",
        "$n transforms into an Experimental Love Torpedo.",
        "You remind $N that you are the Experimental Love Torpedo.",
        "$n reminds $N that $e is the Experimental Love Torpedo.",
        "$n reminds you that $e is the Experimental Love Torpedo.",
        "You rub your hands together and grin evilly.",
        "$n rubs $s hands together and grins evilly.",
    },

    {
        "n_sectioned",
        "You waggle your huge eyebrow in a dubious manner.",
        "$n waggles $s huge eyebrow in a dubious manner.",
        "You waggle your huge eyebrow at $M.",
        "$n waggles $s huge eyebrow at $N.",
        "$n waggles $s huge eyebrow at you.",
        "You waggle your huge eyebrow in a dubious manner.",
        "$n waggles $s huge eyebrow in a dubious manner.",
    },

    {
        "n_stan",
        "You proudly display your shiny red button-nose.",
        "$n proudly display's $s shiny red button-nose.",
        "You remind $M of your shiny red button-nose.",
        "$n reminds $N of $s shiny red button-nose.",
        "$n reminds you of $s shiny red button-nose.",
        "You proudly display your shiny red button-nose.",
        "$n proudly display's $s shiny red button-nose.",
    },

    {
        "n_tijer",
        "You stagger around wildly, treading on everyone's feet.",
        "$n staggers around wildly, treading on everyone's feet.",
        "You slur, 'HEey $N *hic* dO YYOu wANnt tOo be Aa HiGh JuUDge?'.",
        "$n slurs, 'HEey $N *hic* dO YYOu wANnt tOo be Aa HiGh JuUDge?'.",
        "$n slurs, 'HEey $N *hic* dO YYOu wANnt tOo be Aa HiGh JuUDge?'.",
        "You stagger around, butting into everyone's conversations.",
        "$n staggers around, butting into everyone's conversations."
    },

    {
        "",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};


/*
 * The X-social table.
 * Add new X-socials here.
 * Alphabetical order is not required.
 */
const	struct	xsocial_type	xsocial_table [] = {

    {
        "x_earlobe",
        "On whom do you wish to do this?",
        NULL,
        "You gently tug on $M earlobe with your teeth.",
        "$n gently tugs on $N's earlobe with $s teeth.",
        "$n gently tugs on your earlobe with $s teeth.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 1, 1, FALSE
    },

    {
        "x_french",
        "On whom do you wish to do this?",
        NULL,
        "You give $M a deep throbbing kiss, rolling your tongue around $S.",
        "$n gives $N a deep throbbing kiss.",
        "$n gives you a deep throbbing kiss, rolling $s tongue around yours.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 1, 1, FALSE
    },

    {
        "x_kissneck",
        "On whom do you wish to do this?",
        NULL,
        "You slowly and softly kiss and nuzzle $M neck.",
        "$n slowly and softly kisses and nuzzles $N's neck.",
        "$n slowly and softly kisses and nuzzles your neck.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 1, 1, FALSE
    },

    {
        "x_moan",
        "On whom do you wish to do this?",
        NULL,
        "You start moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
        "$n starts moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
        "$n starts moaning 'Oh $N...oh yes...don't stop...mmMMmm...'",
        "Not on yourself!",
        NULL,
        0, 2, 0, 1, 1, FALSE
    },

    {
        "x_nuttella",
        "On whom do you wish to do this?",
        NULL,
        "You cover $N's naked flesh with a popular choc spread, before licking it off.",
        "$n smears a chocolate spread over $N's body, licking $M clean with relish.",
        "$n smears a popular choc spread on your naked flesh, licking it off you.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 5, 20, FALSE
    },

    {
        "x_stroke",
        "On whom do you wish to do this?",
        NULL,
        "You lightly run your fingers along the insides of $S thighs.",
        "$n lightly runs $s fingers along the insides of $N's thighs.",
        "$n lightly runs $s fingers along the insides of your thighs.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 5, 10, FALSE
    },

    {
        "x_tender",
        "On whom do you wish to do this?",
        NULL,
        "You run your fingers through $S hair and kiss $M tenderly on the lips.",
        "$n runs $s fingers through $N's hair and kisses $M tenderly on the lips.",
        "$n runs $s fingers through your hair and kisses you tenderly on the lips.",
        "Not on yourself!",
        NULL,
        0, 0, 0, 1, 1, FALSE
    },

    {
        "x_withdraw",
        "On whom do you wish to do this?",
        NULL,
        "You gently pull yourself free of $M.",
        "$n gently pulls $mself free of $N.",
        "$n gently pulls $mself free of you.",
        "Not on yourself!",
        NULL,
        0, 1, 0, 0, 0, FALSE
    },

    {
        "xf_blowjob",
        "On whom do you wish to do this?",
        NULL,
        "You take $N's hot member in your mouth, sucking $S shaft.",
        "$n takes $N's throbbing penis in $s mouth, sucking $N's cock.",
        "You gasp as $n takes your penis in $s mouth, licking your head.",
        "On yourself? I'm impressed!",
        NULL,
        2, 1, 250, 10, 25, FALSE
    },

    {
        "xf_breasts",
        "On whom do you wish to do this?",
        NULL,
        "You take $S hands and press them to your breasts.",
        "$n takes $N's hands and presses them to $m breasts.",
        "$n takes your hands and presses them to $m breasts.",
        "Not on yourself!",
        NULL,
        2, 0, 0, 5, 10, FALSE
    },

    {
        "xf_contract",
        "On whom do you wish to do this?",
        NULL,
        "You contract your vaginal muscles, driving $M wild.",
        "$n contracts $m vaginal muscles, driving $N wild.",
        "$n contracts $m vaginal muscles, driving you wild.",
        "Not on yourself!",
        NULL,
        2, 2, 0, 10, 15, TRUE
    },

    {
        "xf_floor",
        "On whom do you wish to do this?",
        NULL,
        "You lie on your back, and pull $M between your parted legs.",
        "$n lies on $m back, and pulls $N between $s parted legs.",
        "$n lies on $s back, and pulls you between $s parted legs.",
        "Not on yourself!",
        NULL,
        2, 1, 250, 25, 20, TRUE
    },

    {
        "xf_grind",
        "On whom do you wish to do this?",
        NULL,
        "You grind your hips up to meet $S thrusts.",
        "$n grinds $s hips up to meet $N's thrusts.",
        "$n grinds $s hips up to meet your thrusts.",
        "Not on yourself!",
        NULL,
        2, 2, 0, 15, 10, TRUE
    },

    {
        "xf_mount",
        "On whom do you wish to do this?",
        NULL,
        "You push $M onto $S back, and slowly lower yourself onto $S erection.",
        "$n pushes $N onto $S back, and slowly lowers $mself onto $S erection.",
        "$n pushes you onto your back, and slowly lowers $mself onto your erection.",
        "Not on yourself!",
        NULL,
        2, 1, 250, 25, 20, TRUE
    },

    {
        "xf_nails",
        "On whom do you wish to do this?",
        NULL,
        "You scratch your nails down $S back.",
        "$n scratches $s nails down $N's back.",
        "$n scratches $s nails down your back.",
        "Not on yourself!",
        NULL,
        2, 2, 0, 5, 1, TRUE
    },

    {
        "xf_pull",
        "On whom do you wish to do this?",
        NULL,
        "You wrap your arms and legs around $M and pull $M into you.",
        "$n wraps $s arms and legs around $N and pulls $M into $m.",
        "$n wraps $s arms and legs around you and pulls you into $m.",
        "Not on yourself!",
        NULL,
        2, 2, 0, 15, 10, TRUE
    },

    {
        "xf_squeeze",
        "On whom do you wish to do this?",
        NULL,
        "Your wrap your legs around $M and squeeze tightly.",
        "$n wraps $s legs around $N and squeezes tightly.",
        "$n wraps $s legs around you and squeezes tightly.",
        "Not on yourself!",
        NULL,
        2, 2, 0, 15, 10, TRUE
    },

    {
        "xf_titfuck",
        "Whom do you wish to treat to this?",
        NULL,
        "You take $N's cock and place it between your breasts, as $E gently thrusts.",
        "$n takes $N's penis, places it between $s breasts, and wanks $M off.",
        "$n takes your cock, places it between $s breasts, and wanks you gently with them.",
        "Not on yourself!",
        NULL,
        2, 1, 0, 10, 20, FALSE
    },

    {
        "xf_undress",
        "On whom do you wish to do this?",
        NULL,
        "You push $N onto the floor, a gleam in your eyes, and tear $S clothes off.",
        "$n pushes $N to the floor, tears $S clothes from $S body.",
        "$n pushes you to the floor, grins, and rips your clothes from your body.",
        "Not on yourself!",
        NULL,
        2, 0, 0, 1, 1, FALSE
    },

    {
        "xm_breasts",
        "On whom do you wish to do this?",
        NULL,
        "You gently run the tip of your tongue across $S naked breasts.",
        "$n gently runs the tip of $s tongue across $N's naked breasts.",
        "$n gently runs the tip of $s tongue across your naked breasts.",
        "Not on yourself!",
        NULL,
        1, 0, 0, 5, 10, FALSE
    },

    {
        "xm_cup",
        "On whom do you wish to do this?",
        NULL,
        "You cup $N's breasts in your hands, and caress $S nipples.",
        "$n cups $N's breasts in $s hands and caress' $S nipples.",
        "$n cups your breasts in $s palms and caress' your nipples.",
        "ON YOURSELF??",
        NULL,
        1, 0, 0, 1, 5, FALSE
    },

    {
        "xm_doggy",
        "On whom do you wish to do this?",
        NULL,
        "You roll $M onto all fours and penetrate $S hot flesh from behind.",
        "$n rolls $N onto all fours and penetrates $S hot flesh from behind.",
        "$n rolls you onto all fours and penetrates your hot flesh from behind.",
        "Not on yourself!",
        NULL,
        1, 1, 250, 25, 15, FALSE
    },

    {
        "xm_finger",
        "On whom do you wish to do this?",
        NULL,
        "You slide your fingers between $S legs, gently stroking $S clitoris.",
        "$n slides $s fingers between $N's legs, gently stroking $S clitoris.",
        "$n slides $s fingers between your legs, gently stroking your clitoris.",
        "Not on yourself!",
        NULL,
        1, 1, 250, 0, 10, FALSE
    },

    {
        "xm_fist",
        "On whom do you wish to do this? I hope you asked!",
        NULL,
        "You make a fist and thrust it up $N's spread wide vagina as she screams with pleasure.",
        "$N gasps as $n pushes $s hand up between $S legs into $S vagina.",
        "$n spreads your legs wide, and thrusts $s hand up your vagina making you cry out in pleasure.",
        "C'est non possible.. I hope...",
        NULL,
        1, 1, 250, 5, 15, FALSE
    },

    {
        "xm_floor",
        "On whom do you wish to do this?",
        NULL,
        "You lower $M to the floor, and slide your body between $S parted legs.",
        "$n lowers $N to the floor, and slides $s body between $S parted legs.",
        "$n lowers you to the floor, and slides $s body between your parted legs.",
        "Not on yourself!",
        NULL,
        1, 1, 250, 15, 10, TRUE
    },

    {
        "xm_nipple",
        "On whom do you wish to do this?",
        NULL,
        "You gently twist $S nipple between your thumb and forefinger.",
        "$n gently twists $N's nipple between $s thumb and forefinger.",
        "$n gently twists your nipple between $s thumb and forefinger.",
        "Not on yourself!",
        NULL,
        1, 0, 0, 5, 10, FALSE
    },

    {
        "xm_oral",
        "On whom do you wish to do this?",
        NULL,
        "$N squirms in delight as you bend down and run your tongue along and into $S vagina.",
        "$N squirms ecstatically as $n licks and kisses $S loveslit.",
        "$n wriggles $s tongue about between your legs, making you squirm in ecstacy.",
        "Biologically impossible I think you'll find!",
        NULL,
        1, 0, 0, 10, 25, FALSE
    },

    {
        "xm_press",
        "On whom do you wish to do this?",
        NULL,
        "You press $M against the wall, pulling $S legs around your hips.",
        "$n presses $N against the wall, pulling $S legs around $s hips.",
        "$n presses you against the wall, pulling your legs around $s hips.",
        "Not on yourself!",
        NULL,
        1, 1, 250, 25, 20, TRUE
    },

    {
        "xm_pull",
        "On whom do you wish to do this?",
        NULL,
        "You grab $M around the hips and pull $M firmly onto your erection.",
        "$n grabs $N around the hips and pull $M firmly onto $s erection.",
        "$n grabs you around the hips and pulls you firmly onto $s erection.",
        "Not on yourself!",
        NULL,
        1, 2, 0, 10, 10, TRUE
    },

    {
        "xm_spoon",
        "On whom do you wish to do this?",
        NULL,
        "You roll $M onto $S side and penetrate $M hot flesh from behind.",
        "$n rolls $N onto $S side and penetrates $M hot flesh from behind.",
        "$n rolls you onto your side and penetrates your hot flesh from behind.",
        "Not on yourself!",
        NULL,
        1, 1, 250, 20, 20, TRUE
    },

    {
        "xm_suck",
        "On whom do you wish to do this?",
        NULL,
        "You suck slowly on $M nipple, feeling it harden between your lips.",
        "$n suck slowly on $N's nipple.",
        "$n sucks slowly on your nipple, and you feel it harden between $s lips.",
        "Not on yourself!",
        NULL,
        1, 0, 0, 5, 10, FALSE
    },

    {
        "xm_thrust",
        "On whom do you wish to do this?",
        NULL,
        "You thrust deeply between $M warm, slippery thighs.",
        "$n thrusts deeply between $N's warm, slippery thighs.",
        "$n thrusts deeply between your warm, slippery thighs.",
        "Not on yourself!",
        NULL,
        1, 2, 0, 15, 10, TRUE
    },

    {
        "xm_tug",
        "On whom do you wish to do this?",
        NULL,
        "You gently tug $M nipple between your teeth.",
        "$n gently tugs $N's nipple between $s teeth.",
        "$n gently tugs your nipple between $s teeth.",
        "Not on yourself!",
        NULL,
        1, 0, 0, 5, 10, FALSE
    },

    {
        "xm_undress",
        "Who do you wish to undress?",
        NULL,
        "You gently tug at $N's garments with your teeth, until $E stands naked before you.",
        "$n gently tugs at $N's clothes with his teeth, until $E stands naked infront of $m.",
        "$n gently tugs at your clothing with his teeth, you stand naked before $m.",
        "Not on yourself!",
        NULL,
        1, 0, 0, 1, 1, FALSE
    },

    {
        "",
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, FALSE
    }
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret(CHAR_DATA *ch, char *argument) {
    /*  ROOMTEXT_DATA *rt;
        char kavirarg[MAX_INPUT_LENGTH]; */
    CHAR_DATA *vch;
    char arg[MAX_STRING_LENGTH];
    char argu[MAX_STRING_LENGTH];
    char command[MAX_STRING_LENGTH];
    char logline[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int trust;
    bool found;
    sprintf(argu, "%s %s", arg, one_argument(argument, arg));
    /*
     * Strip leading spaces.
     */
    while(isspace(*argument)) {
        argument++;
    }
    if(argument[0] == '\0') {
        return;
    }
    /*
     * No hiding.
    REMOVE_BIT( ch->affected_by, AFF_HIDE );
     */
    /*
     * Implement freeze command.
     */
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE)) {
        send_to_char("You're totally frozen!\n\r", ch);
        return;
    } else if(number_percent() <= 25 && IS_MORE(ch, MORE_CALM)) {
        send_to_char("You really can't be bothered.\n\r", ch);
        return;
    }
    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy(logline, argument);
    if(!isalpha(argument[0]) && !isdigit(argument[0])) {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while(isspace(*argument)) {
            argument++;
        }
    } else {
        argument = one_argument(argument, command);
    }
    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust(ch);
    for(cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++) {
        if(command[0] == cmd_table[cmd].name[0]
                &&   !str_prefix(command, cmd_table[cmd].name)
                &&   cmd_table[cmd].level <= trust) {
            if(IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)) {
                if(IS_MORE(ch, MORE_MUMMIFIED)) {
                    if(!str_cmp(cmd_table[cmd].name, "reply")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "look")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "where")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "score")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "save")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "who")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "quit")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "mummify")) {
                        found = TRUE;
                    } else {
                        send_to_char("Not while mummified!\n\r", ch);
                        return;
                    }
                } else {
                    if(!str_cmp(cmd_table[cmd].name, "say")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "'")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "immtalk")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, ":")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "vamptalk")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "ctalk")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "chat")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, ".")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "look")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "save")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "exits")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "emote")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "tell")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "reply")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "order")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "ashestoashes")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "plasmaform")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "who")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "weather")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "where")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "relevel")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "safe")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "scan")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "spy")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "score")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "save")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "inventory")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "oreturn")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "roll")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "pour")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "leap")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "lifespan")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "nightsight")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "truesight")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "horns")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "fangs")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "cast")) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "quit") &&
                              !IS_NPC(ch) && ch->pcdata->obj_vnum != 0) {
                        found = TRUE;
                    } else if(!str_cmp(cmd_table[cmd].name, "humanform") &&
                              !IS_NPC(ch) && ch->pcdata->obj_vnum != 0) {
                        found = TRUE;
                    } else {
                        if(IS_MORE(ch, MORE_STAKED) &&
                                get_disc(ch, DISC_PROTEAN) >= 5) {
                            if(!str_cmp(cmd_table[cmd].name, "stake")) {
                                found = TRUE;
                            } else {
                                send_to_char("Not while staked!\n\r", ch);
                                return;
                            }
                        } else {
                            if(IS_MORE(ch, MORE_STAKED)) {
                                send_to_char("Not while staked!\n\r", ch);
                            } else {
                                send_to_char("Not without a body!\n\r", ch);
                            }
                            return;
                        }
                    }
                }
            } else if(IS_EXTRA(ch, EXTRA_NO_ACTION)) {
                if(!str_cmp(cmd_table[cmd].name, "reply")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "fcommand")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "[")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "astral")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "projection")) {
                    found = TRUE;
                } else {
                    send_to_char("You are not in control of your body.\n\r", ch);
                    if(ch->pcdata->familiar == NULL) {
                        REMOVE_BIT(ch->extra, EXTRA_NO_ACTION);
                    }
                    return;
                }
            } else if(ch->embraced != ARE_NONE) {
                if(!str_cmp(cmd_table[cmd].name, "say")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "'")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "immtalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, ":")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "vamptalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "ctalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "chat")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, ".")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "yell")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "shout")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "look")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "save")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "kill")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "inventory")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "embrace")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "equipment")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "tell")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "reply")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "who")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "relevel")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "wake")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "stand")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "regenerate")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "vclan")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "upkeep")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "score")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "affects")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "goto")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "where")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "burn")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "flex")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "fcommand")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "[")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "group")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "gtell")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, ";")) {
                    found = TRUE;
                } else {
                    send_to_char("Not while you are in an embrace.\n\r", ch);
                    return;
                }
            } else if(IS_EXTRA(ch, TIED_UP)) {
                if(!str_cmp(cmd_table[cmd].name, "say")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "'")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "immtalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, ":")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "vamptalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "ctalk")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "chat")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, ".")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "yell")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "shout")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "look")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "save")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "exits")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "inventory")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "tell")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "reply")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "order")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "who")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "weather")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "where")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "introduce")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "relevel")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "safe")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "scan")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "spy")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "sleep")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "wake")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "stand")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "fangs")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "claws")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "nightsight")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "umbralvision")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "umbra")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "regenerate")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "shield")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "vclan")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "upkeep")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "score")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "affects")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "immune")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "report")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "goto")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "flex")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "change")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "drink")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "fcommand")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "[")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "astral")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "projection")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "plasmaform")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "earthmeld")) {
                    found = TRUE;
                } else {
                    send_to_char("Not while tied up.\n\r", ch);
                    if(ch->position > POS_STUNNED) {
                        act("$n strains against $s bonds.", ch, NULL, NULL, TO_ROOM);
                    }
                    return;
                }
            } else if(IS_VAMPAFF(ch, VAM_EARTHMELDED)) {
                if(!str_cmp(cmd_table[cmd].name, "save")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "inventory")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "who")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "relevel")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "safe")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "vclan")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "upkeep")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "score")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "affects")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "immune")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "earthmeld")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "burrow")) {
                    found = TRUE;
                } else if(!str_cmp(cmd_table[cmd].name, "regenerate")) {
                    found = TRUE;
                } else {
                    send_to_char("You cannot do that while you are burried in the earth.\n\r", ch);
                    return;
                }
            }
            found = TRUE;
            break;
        }
    }
    /*
     * Log and snoop.
     */
    if(cmd_table[cmd].log == LOG_NEVER) {
        strcpy(logline, "XXXXXXXX XXXXXXXX XXXXXXXX");
    }
    if((!IS_NPC(ch) && IS_SET(ch->act, PLR_LOG))
            ||   fLogAll
            ||   cmd_table[cmd].log == LOG_ALWAYS) {
        sprintf(log_buf, "Log %s: %s", ch->name, logline);
        log_string(log_buf);
    }
    if(ch->desc != NULL && ch->desc->snoop_by != NULL) {
        if((vch = ch->desc->snoop_by->character) == NULL) {
            ch->desc->snoop_by = NULL;
        } else if(IS_IMMORTAL(vch)) {
            write_to_buffer(ch->desc->snoop_by, "% ",    2);
            write_to_buffer(ch->desc->snoop_by, logline, 0);
            write_to_buffer(ch->desc->snoop_by, "\n\r",  2);
        } else if(ch->in_room == NULL || vch->in_room == NULL) {
            sprintf(buf, "You loose your mental link with %s.\n\r", ch->name);
            send_to_char(buf, vch);
            bug("Auspex power 4 in-room bug.", 0);
            ch->desc->snoop_by = NULL;
        } else if(ch->in_room != vch->in_room) {
            sprintf(buf, "You loose your mental link with %s.\n\r", ch->name);
            send_to_char(buf, vch);
            ch->desc->snoop_by = NULL;
        } else {
            sprintf(buf, "%s thinks: %s.\n\r", ch->name, logline);
            send_to_char(buf, vch);
        }
    }
    if(!found) {
        /*
         * Look for command in socials table.
         */
        if(!check_social(ch, command, argument)) {
            if(!check_xsocial(ch, command, argument)) {
                if(!check_nsocial(ch, command, argument)) {
                    if(!icec_command_hook(ch, command, argument)) {
                        send_to_char("Huh?\n\r", ch);
                    }
                }
            }
        }
        /*
        	else
        	{
        	    sprintf(kavirarg,">>>%s",argu);
        	    room_text( ch, kavirarg );
        	}
        */
        return;
    }
    /*
     * Character not in position for command?
     */
    if(ch->position < cmd_table[cmd].position && !IS_NPC(ch) &&
            IS_VAMPIRE(ch) && get_disc(ch, DISC_PROTEAN) >= 5) {
        bool stop_now = TRUE;
        switch(ch->position) {
        case POS_DEAD:
            send_to_char("Lie still; you are DEAD.\n\r", ch);
            break;
        case POS_MORTAL:
        case POS_INCAP:
            stop_now = FALSE;
            WAIT_STATE(ch, 12);
            break;
        case POS_STUNNED:
            stop_now = FALSE;
            WAIT_STATE(ch, 12);
            break;
        case POS_SLEEPING:
            send_to_char("In your dreams, or what?\n\r", ch);
            break;
        case POS_MEDITATING:
        case POS_SITTING:
        case POS_RESTING:
            send_to_char("Nah... You feel too relaxed...\n\r", ch);
            break;
        case POS_FIGHTING:
            send_to_char("No way!  You are still fighting!\n\r", ch);
            break;
        }
        if(stop_now) {
            return;
        }
    } else if(ch->position < cmd_table[cmd].position) {
        switch(ch->position) {
        case POS_DEAD:
            send_to_char("Lie still; you are DEAD.\n\r", ch);
            break;
        case POS_MORTAL:
        case POS_INCAP:
            send_to_char("You are hurt far too bad for that.\n\r", ch);
            break;
        case POS_STUNNED:
            send_to_char("You are too stunned to do that.\n\r", ch);
            break;
        case POS_SLEEPING:
            send_to_char("In your dreams, or what?\n\r", ch);
            break;
        case POS_MEDITATING:
        case POS_SITTING:
        case POS_RESTING:
            send_to_char("Nah... You feel too relaxed...\n\r", ch);
            break;
        case POS_FIGHTING:
            send_to_char("No way!  You are still fighting!\n\r", ch);
            break;
        }
        return;
    }
    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun)(ch, argument);
    tail_chain();
    /*
        if (!str_cmp(arg,"say"))
        {
        	sprintf(kavirarg,"%s",argu);
        	room_text( ch, strlower(kavirarg) );
        }
        else
        {
        	sprintf(kavirarg,">>>%s",argu);
        	room_text( ch, kavirarg );
        }
    */
    return;
}



bool check_social(CHAR_DATA *ch, char *command, char *argument) {
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    found  = FALSE;
    for(cmd = 0; social_table[cmd].name[0] != '\0'; cmd++) {
        if(command[0] == social_table[cmd].name[0]
                &&   !str_prefix(command, social_table[cmd].name)) {
            found = TRUE;
            break;
        }
    }
    if(!found) {
        return FALSE;
    }
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE)) {
        send_to_char("You are anti-social!\n\r", ch);
        return TRUE;
    }
    if(IS_NPC(ch) && IS_SET(ch->act, ACT_NOPARTS)) {
        send_to_char("You are unable to use socials!\n\r", ch);
        return TRUE;
    }
    /*
        if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH))
        {
    	send_to_char( "You cannot socialise in this form!\n\r", ch );
    	return TRUE;
        }
    */
    if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_EARTHMELDED)) {
        send_to_char("You cannot socialise while earthmelded!\n\r", ch);
        return TRUE;
    }
    if(!IS_NPC(ch) && get_disc(ch, DISC_PROTEAN) >= 5 &&
            ch->position <= POS_SLEEPING) {
        send_to_char("You use Movement of the Slowed body.\n\r", ch);
        WAIT_STATE(ch, 12);
    } else switch(ch->position) {
        case POS_DEAD:
            send_to_char("Lie still; you are DEAD.\n\r", ch);
            return TRUE;
        case POS_INCAP:
        case POS_MORTAL:
            send_to_char("You are hurt far too bad for that.\n\r", ch);
            return TRUE;
        case POS_STUNNED:
            send_to_char("You are too stunned to do that.\n\r", ch);
            return TRUE;
        case POS_SLEEPING:
            /*
             * I just know this is the path to a 12" 'if' statement.  :(
             * But two players asked for it already!  -- Furey
             */
            if(!str_cmp(social_table[cmd].name, "snore")) {
                break;
            }
            send_to_char("In your dreams, or what?\n\r", ch);
            return TRUE;
        }
    one_argument(argument, arg);
    victim = NULL;
    if(arg[0] == '\0') {
        act(social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);
        act(social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR);
    } else if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
    } else if(victim == ch) {
        act(social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM);
        act(social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR);
    } else {
        act(social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT);
        act(social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR);
        act(social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT);
        if(!IS_NPC(ch) && IS_NPC(victim)
                &&   !IS_AFFECTED(victim, AFF_CHARM)
                &&   !IS_EXTRA(victim, EXTRA_ILLUSION)
                &&   !IS_ITEMAFF(ch, ITEMA_PEACE)
                &&   can_see(victim, ch)
                &&   IS_AWAKE(victim)) {
            if(IS_SET(victim->act, ACT_DOG)) switch(number_range(1, 20)) {
                default:
                    break;
                case 0:
                    act("$n licks $N's hand.",  victim, NULL, ch, TO_NOTVICT);
                    act("You lick $N's hand.",  victim, NULL, ch, TO_CHAR);
                    act("$n licks you hand.", victim, NULL, ch, TO_VICT);
                    break;
                case 1:
                    act("$n growls playfully at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You growl playfully at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n growls playfully at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 2:
                    act("$n yelps loudly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You yelp loudly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n yelps loudly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 3:
                    act("$n sniffs $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs you.", victim, NULL, ch, TO_VICT);
                    break;
                case 4:
                    act("$n barks loudly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You bark loudly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n barks loudly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 5:
                    act("$n wags $s tail at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You wag your tail at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n wags $s tail at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 6:
                    act("$n sniffs $N's feet.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff $N's feet.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs your feet.", victim, NULL, ch, TO_VICT);
                    break;
                case 7:
                    act("$n whines at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You whine $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n whines at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 8:
                    act("$n gazes lovingly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You gaze lovingly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n gazes lovingly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 9:
                    act("$n sniffs at $N in search of food.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff at $N in search of food.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs at you in search of food.", victim, NULL, ch, TO_VICT);
                    break;
                case 10:
                    act("$n rolls over, hoping $N will scratch $s tummy.",  victim, NULL, ch, TO_NOTVICT);
                    act("You roll over, hoping that $N will scratch your tummy.",  victim, NULL, ch, TO_CHAR);
                    act("$n rolls over, hoping that you will scratch $s tummy.", victim, NULL, ch, TO_VICT);
                    break;
                case 11:
                    act("$n begs $N for some food.",  victim, NULL, ch, TO_NOTVICT);
                    act("You beg $N for some food.",  victim, NULL, ch, TO_CHAR);
                    act("$n begs you for some food.", victim, NULL, ch, TO_VICT);
                    break;
                case 12:
                    act("$n jumps up against $N's leg.",  victim, NULL, ch, TO_NOTVICT);
                    act("You jump up against $N's leg.",  victim, NULL, ch, TO_CHAR);
                    act("$n jumps up against your leg.", victim, NULL, ch, TO_VICT);
                    break;
                case 13:
                    act("$n tries to eat $N's shoes.",  victim, NULL, ch, TO_NOTVICT);
                    act("You try to eat $N's shoes.",  victim, NULL, ch, TO_CHAR);
                    act("$n tries to eat your shoes.", victim, NULL, ch, TO_VICT);
                    break;
                case 14:
                    act("$n runs in circles around $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You run in circles around $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n runs in circles around you.", victim, NULL, ch, TO_VICT);
                    break;
                }
            else switch(number_bits(4)) {
                default:
                case 0:
                    multi_hit(victim, ch, TYPE_UNDEFINED);
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                    act(social_table[cmd].others_found,
                        victim, NULL, ch, TO_NOTVICT);
                    act(social_table[cmd].char_found,
                        victim, NULL, ch, TO_CHAR);
                    act(social_table[cmd].vict_found,
                        victim, NULL, ch, TO_VICT);
                    break;
                case 9:
                case 10:
                case 11:
                case 12:
                    act("$n slaps $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You slap $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n slaps you.", victim, NULL, ch, TO_VICT);
                    break;
                }
        }
    }
    return TRUE;
}



bool check_nsocial(CHAR_DATA *ch, char *command, char *argument) {
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    found  = FALSE;
    for(cmd = 0; nsocial_table[cmd].name[0] != '\0'; cmd++) {
        if(command[0] == nsocial_table[cmd].name[0]
                &&   !str_prefix(command, nsocial_table[cmd].name)) {
            found = TRUE;
            break;
        }
    }
    if(!found) {
        return FALSE;
    }
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE)) {
        send_to_char("You are anti-social!\n\r", ch);
        return TRUE;
    }
    /*
        if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH))
        {
    	send_to_char( "You cannot socialise in this form!\n\r", ch );
    	return TRUE;
        }
    */
    if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_EARTHMELDED)) {
        send_to_char("You cannot socialise while earthmelded!\n\r", ch);
        return TRUE;
    }
    if(!IS_NPC(ch) && get_disc(ch, DISC_PROTEAN) > 7 &&
            ch->position <= POS_SLEEPING) {
        send_to_char("You use Movement of the Slowed body.\n\r", ch);
        WAIT_STATE(ch, 12);
    } else switch(ch->position) {
        case POS_DEAD:
            send_to_char("Lie still; you are DEAD.\n\r", ch);
            return TRUE;
        case POS_INCAP:
        case POS_MORTAL:
            send_to_char("You are hurt far too bad for that.\n\r", ch);
            return TRUE;
        case POS_STUNNED:
            send_to_char("You are too stunned to do that.\n\r", ch);
            return TRUE;
        case POS_SLEEPING:
            send_to_char("In your dreams, or what?\n\r", ch);
            return TRUE;
        }
    one_argument(argument, arg);
    victim = NULL;
    if(arg[0] == '\0') {
        act(nsocial_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);
        act(nsocial_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR);
    } else if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
    } else if(victim == ch) {
        act(nsocial_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM);
        act(nsocial_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR);
    } else {
        act(nsocial_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT);
        act(nsocial_table[cmd].char_found,    ch, NULL, victim, TO_CHAR);
        act(nsocial_table[cmd].vict_found,    ch, NULL, victim, TO_VICT);
        if(!IS_NPC(ch) && IS_NPC(victim)
                &&   !IS_AFFECTED(victim, AFF_CHARM)
                &&   !IS_EXTRA(victim, EXTRA_ILLUSION)
                &&   can_see(victim, ch)
                &&   IS_AWAKE(victim)) {
            if(IS_SET(victim->act, ACT_DOG)) switch(number_range(1, 20)) {
                default:
                    break;
                case 0:
                    act("$n licks $N's hand.",  victim, NULL, ch, TO_NOTVICT);
                    act("You lick $N's hand.",  victim, NULL, ch, TO_CHAR);
                    act("$n licks you hand.", victim, NULL, ch, TO_VICT);
                    break;
                case 1:
                    act("$n growls playfully at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You growl playfully at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n growls playfully at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 2:
                    act("$n yelps loudly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You yelp loudly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n yelps loudly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 3:
                    act("$n sniffs $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs you.", victim, NULL, ch, TO_VICT);
                    break;
                case 4:
                    act("$n barks loudly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You bark loudly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n barks loudly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 5:
                    act("$n wags $s tail at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You wag your tail at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n wags $s tail at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 6:
                    act("$n sniffs $N's feet.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff $N's feet.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs your feet.", victim, NULL, ch, TO_VICT);
                    break;
                case 7:
                    act("$n whines at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You whine $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n whines at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 8:
                    act("$n gazes lovingly at $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You gaze lovingly at $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n gazes lovingly at you.", victim, NULL, ch, TO_VICT);
                    break;
                case 9:
                    act("$n sniffs at $N in search of food.",  victim, NULL, ch, TO_NOTVICT);
                    act("You sniff at $N in search of food.",  victim, NULL, ch, TO_CHAR);
                    act("$n sniffs at you in search of food.", victim, NULL, ch, TO_VICT);
                    break;
                case 10:
                    act("$n rolls over, hoping $N will scratch $s tummy.",  victim, NULL, ch, TO_NOTVICT);
                    act("You roll over, hoping that $N will scratch your tummy.",  victim, NULL, ch, TO_CHAR);
                    act("$n rolls over, hoping that you will scratch $s tummy.", victim, NULL, ch, TO_VICT);
                    break;
                case 11:
                    act("$n begs $N for some food.",  victim, NULL, ch, TO_NOTVICT);
                    act("You beg $N for some food.",  victim, NULL, ch, TO_CHAR);
                    act("$n begs you for some food.", victim, NULL, ch, TO_VICT);
                    break;
                case 12:
                    act("$n jumps up against $N's leg.",  victim, NULL, ch, TO_NOTVICT);
                    act("You jump up against $N's leg.",  victim, NULL, ch, TO_CHAR);
                    act("$n jumps up against your leg.", victim, NULL, ch, TO_VICT);
                    break;
                case 13:
                    act("$n tries to eat $N's shoes.",  victim, NULL, ch, TO_NOTVICT);
                    act("You try to eat $N's shoes.",  victim, NULL, ch, TO_CHAR);
                    act("$n tries to eat your shoes.", victim, NULL, ch, TO_VICT);
                    break;
                case 14:
                    act("$n runs in circles around $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You run in circles around $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n runs in circles around you.", victim, NULL, ch, TO_VICT);
                    break;
                }
            else switch(number_bits(4)) {
                case 0:
                    multi_hit(victim, ch, TYPE_UNDEFINED);
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                    act(nsocial_table[cmd].others_found,
                        victim, NULL, ch, TO_NOTVICT);
                    act(nsocial_table[cmd].char_found,
                        victim, NULL, ch, TO_CHAR);
                    act(nsocial_table[cmd].vict_found,
                        victim, NULL, ch, TO_VICT);
                    break;
                case 9:
                case 10:
                case 11:
                case 12:
                    act("$n slaps $N.",  victim, NULL, ch, TO_NOTVICT);
                    act("You slap $N.",  victim, NULL, ch, TO_CHAR);
                    act("$n slaps you.", victim, NULL, ch, TO_VICT);
                    break;
                }
        }
    }
    return TRUE;
}



bool check_xsocial(CHAR_DATA *ch, char *command, char *argument) {
    char arg[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *partner = NULL;
    int cmd;
    int stage;
    int amount;
    bool is_ok = FALSE;
    bool found = FALSE;
    bool one = FALSE;
    bool two = FALSE;
    if(IS_NPC(ch)) {
        return FALSE;
    }
    for(cmd = 0; xsocial_table[cmd].name[0] != '\0'; cmd++) {
        if(command[0] == xsocial_table[cmd].name[0]
                &&   !str_prefix(command, xsocial_table[cmd].name)) {
            found = TRUE;
            break;
        }
    }
    if(!found) {
        return FALSE;
    }
    if(!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE)) {
        send_to_char("You are anti-social!\n\r", ch);
        return TRUE;
    }
    switch(ch->position) {
    case POS_DEAD:
        send_to_char("Lie still; you are DEAD.\n\r", ch);
        return TRUE;
    case POS_INCAP:
    case POS_MORTAL:
        send_to_char("You are hurt far too bad for that.\n\r", ch);
        return TRUE;
    case POS_STUNNED:
        send_to_char("You are too stunned to do that.\n\r", ch);
        return TRUE;
    case POS_SLEEPING:
        send_to_char("In your dreams, or what?\n\r", ch);
        return TRUE;
    }
    one_argument(argument, arg);
    victim = NULL;
    if(arg[0] == '\0') {
        act(xsocial_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);
        act(xsocial_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR);
    } else if((victim = get_char_room(ch, arg)) == NULL) {
        send_to_char("They aren't here.\n\r", ch);
    } else if(victim == ch) {
        act(xsocial_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM);
        act(xsocial_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR);
    } else if(IS_NPC(victim)) {
        send_to_char("You can only perform xsocials on players.\n\r", ch);
    } else if(ch->sex != SEX_MALE && ch->sex != SEX_FEMALE) {
        send_to_char("You must be either male or female to use these socials.\n\r", ch);
    } else if(victim->sex != SEX_MALE && victim->sex != SEX_FEMALE) {
        send_to_char("They must be either male or female for these socials.\n\r", ch);
    } else if(ch->sex == victim->sex) {
        send_to_char("Please stick to people of the opposite gender.\n\r", ch);
    } else {
        if(xsocial_table[cmd].gender == SEX_MALE && ch->sex != SEX_MALE) {
            send_to_char("Only men can perform this type of social.\n\r", ch);
        } else if(xsocial_table[cmd].gender == SEX_FEMALE && ch->sex != SEX_FEMALE) {
            send_to_char("Only women can perform this type of social.\n\r", ch);
        } else if(xsocial_table[cmd].gender == SEX_MALE && victim->sex != SEX_FEMALE) {
            send_to_char("You can only perform this social on a woman.\n\r", ch);
        } else if(xsocial_table[cmd].gender == SEX_FEMALE && victim->sex != SEX_MALE) {
            send_to_char("You can only perform this social on a man.\n\r", ch);
        } else if((partner = victim->pcdata->partner) == NULL || partner != ch) {
            send_to_char("You cannot perform an xsocial on someone without their CONSENT.\n\r", ch);
        } else if(xsocial_table[cmd].stage == 0 && ch->pcdata->stage[0] < 1
                  && ch->pcdata->stage[2] > 0 && ch->sex == SEX_MALE) {
            send_to_char("You have not yet recovered from last time!\n\r", ch);
        } else if(xsocial_table[cmd].stage == 0 && victim->pcdata->stage[0] < 1
                  && victim->pcdata->stage[2] > 0 && victim->sex == SEX_MALE) {
            send_to_char("They have not yet recovered from last time!\n\r", ch);
        } else if(xsocial_table[cmd].stage > 0 && ch->pcdata->stage[0] < 100) {
            send_to_char("You are not sufficiently aroused.\n\r", ch);
        } else if(xsocial_table[cmd].stage > 0 && victim->pcdata->stage[0] < 100) {
            send_to_char("They are not sufficiently aroused.\n\r", ch);
        } else if(xsocial_table[cmd].stage > 1 && ch->pcdata->stage[1] < 1) {
            send_to_char("You are not in the right position.\n\r", ch);
        } else if(xsocial_table[cmd].stage > 1 && victim->pcdata->stage[1] < 1) {
            send_to_char("They are not in the right position.\n\r", ch);
        } else {
            act(xsocial_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT);
            act(xsocial_table[cmd].char_found,    ch, NULL, victim, TO_CHAR);
            act(xsocial_table[cmd].vict_found,    ch, NULL, victim, TO_VICT);
            if(xsocial_table[cmd].chance) {
                if(ch->sex == SEX_FEMALE &&
                        !IS_EXTRA(ch, EXTRA_PREGNANT) && number_range(1, 1000) == 1) {
                    make_preg(ch, victim);
                } else if(victim->sex == SEX_FEMALE &&
                          !IS_EXTRA(victim, EXTRA_PREGNANT) &&
                          number_range(1, 1000) == 1) {
                    make_preg(victim, ch);
                }
            }
            if(xsocial_table[cmd].stage == 1) {
                ch->pcdata->stage[1] = xsocial_table[cmd].position;
                victim->pcdata->stage[1] = xsocial_table[cmd].position;
                if(!IS_SET(ch->extra, EXTRA_DONE)) {
                    SET_BIT(ch->extra, EXTRA_DONE);
                    if(ch->sex == SEX_FEMALE) {
                        act("You feel $n bleed as you enter $m.", ch, NULL, victim, TO_VICT);
                        act("You feel yourself bleed as $N enters you.", ch, NULL, victim, TO_CHAR);
                        ch->in_room->blood += 1;
                    }
                }
                if(!IS_SET(victim->extra, EXTRA_DONE)) {
                    SET_BIT(victim->extra, EXTRA_DONE);
                    if(victim->sex == SEX_FEMALE) {
                        act("You feel $N bleed as you enter $M.", ch, NULL, victim, TO_CHAR);
                        act("You feel yourself bleed as $n enters you.", ch, NULL, victim, TO_VICT);
                        ch->in_room->blood += 1;
                    }
                }
                stage = 2;
            } else {
                stage = xsocial_table[cmd].stage;
            }
            if(stage == 2) {
                amount = ch->pcdata->stage[1];
            } else {
                amount = 100;
            }
            if(xsocial_table[cmd].self > 0) {
                is_ok = FALSE;
                if(ch->pcdata->stage[stage] >= amount) {
                    is_ok = TRUE;
                }
                ch->pcdata->stage[stage] += xsocial_table[cmd].self;
                if(!is_ok && ch->pcdata->stage[stage] >= amount) {
                    stage_update(ch, victim, stage);
                    one = TRUE;
                }
            }
            if(xsocial_table[cmd].other > 0) {
                is_ok = FALSE;
                if(victim->pcdata->stage[stage] >= amount) {
                    is_ok = TRUE;
                }
                victim->pcdata->stage[stage] += xsocial_table[cmd].other;
                if(!is_ok && victim->pcdata->stage[stage] >= amount) {
                    stage_update(victim, ch, stage);
                    two = TRUE;
                }
            }
            if(one && two) {
                ch->pcdata->stage[0] = 0;
                victim->pcdata->stage[0] = 0;
                if(!IS_EXTRA(ch, EXTRA_EXP)) {
                    send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", ch);
                    SET_BIT(ch->extra, EXTRA_EXP);
                    ch->exp += 100000;
                }
                if(!IS_EXTRA(victim, EXTRA_EXP)) {
                    send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r", victim);
                    SET_BIT(victim->extra, EXTRA_EXP);
                    victim->exp += 100000;
                }
            }
        }
    }
    return TRUE;
}

void stage_update(CHAR_DATA *ch, CHAR_DATA *victim, int stage) {
    if(IS_NPC(ch) || IS_NPC(victim)) {
        return;
    }
    if(stage == 0) {
        if(ch->sex == SEX_MALE) {
            send_to_char("You feel yourself harden.\n\r", ch);
            act("You feel $n harden.", ch, NULL, victim, TO_VICT);
            return;
        } else if(ch->sex == SEX_FEMALE) {
            send_to_char("You feel moist.\n\r", ch);
            act("You feel $n dampen.", ch, NULL, victim, TO_VICT);
            return;
        }
    } else if(stage == 2) {
        if(ch->sex == SEX_MALE) {
            act("You clench your teeth as you cum in $M.", ch, NULL, victim, TO_CHAR);
            act("$n clenches $s teeth as $e cums in you.", ch, NULL, victim, TO_VICT);
            act("$n clenches $s teeth as $e cums in $N.", ch, NULL, victim, TO_NOTVICT);
            ch->pcdata->stage[0] = 0;
            ch->pcdata->stage[1] = 0;
            ch->pcdata->genes[8] += 1;
            victim->pcdata->genes[8] += 1;
            do_autosave(ch, "");
            do_autosave(victim, "");
            if(ch->pcdata->stage[0] <= 250) {
                ch->pcdata->stage[0] = 0;
            } else {
                victim->pcdata->stage[0] -= 250;
            }
            victim->pcdata->stage[1] = 0;
            if(victim->sex == SEX_FEMALE &&
                    !IS_EXTRA(victim, EXTRA_PREGNANT) && number_percent() <= 8) {
                make_preg(victim, ch);
            }
            return;
        } else if(ch->sex == SEX_FEMALE) {
            act("You wimper as you cum.", ch, NULL, victim, TO_CHAR);
            act("$n wimpers as $e cums.", ch, NULL, victim, TO_ROOM);
            if(victim->pcdata->stage[2] < 1 || victim->pcdata->stage[2] >= 250) {
                ch->pcdata->stage[2] = 0;
                if(ch->pcdata->stage[0] >= 200) {
                    ch->pcdata->stage[0] -= 100;
                }
            } else {
                ch->pcdata->stage[2] = 200;
            }
            return;
        }
    }
    return;
}

void make_preg(CHAR_DATA *mother, CHAR_DATA *father) {
    char *strtime;
    char buf [MAX_STRING_LENGTH];
    if(IS_NPC(mother) || IS_NPC(father)) {
        return;
    }
    if(IS_AFFECTED(mother, AFF_CONTRACEPTION)) {
        return;
    }
    if(IS_VAMPIRE(mother) || IS_VAMPIRE(father)) {
        return;
    }
    strtime = ctime(&current_time);
    strtime[strlen(strtime) - 1] = '\0';
    free_string(mother->pcdata->conception);
    mother->pcdata->conception = str_dup(strtime);
    sprintf(buf, "%s %s", mother->name, father->name);
    free_string(mother->pcdata->cparents);
    mother->pcdata->cparents = str_dup(buf);
    SET_BIT(mother->extra, EXTRA_PREGNANT);
    mother->pcdata->genes[0] = (mother->max_hit + father->max_hit) * 0.5;
    mother->pcdata->genes[1] = (mother->max_mana + father->max_mana) * 0.5;
    mother->pcdata->genes[2] = (mother->max_move + father->max_move) * 0.5;
    if(mother->pcdata->genes[0] > 5000) {
        mother->pcdata->genes[0] = 5000;
    }
    if(mother->pcdata->genes[1] > 5000) {
        mother->pcdata->genes[1] = 5000;
    }
    if(mother->pcdata->genes[2] > 5000) {
        mother->pcdata->genes[2] = 5000;
    }
    if(IS_IMMUNE(mother, IMM_SLASH) && IS_IMMUNE(father, IMM_SLASH)) {
        SET_BIT(mother->pcdata->genes[3], IMM_SLASH);
    }
    if(IS_IMMUNE(mother, IMM_STAB) && IS_IMMUNE(father, IMM_STAB)) {
        SET_BIT(mother->pcdata->genes[3], IMM_STAB);
    }
    if(IS_IMMUNE(mother, IMM_SMASH) && IS_IMMUNE(father, IMM_SMASH)) {
        SET_BIT(mother->pcdata->genes[3], IMM_SMASH);
    }
    if(IS_IMMUNE(mother, IMM_ANIMAL) && IS_IMMUNE(father, IMM_ANIMAL)) {
        SET_BIT(mother->pcdata->genes[3], IMM_ANIMAL);
    }
    if(IS_IMMUNE(mother, IMM_MISC) && IS_IMMUNE(father, IMM_MISC)) {
        SET_BIT(mother->pcdata->genes[3], IMM_MISC);
    }
    if(IS_IMMUNE(mother, IMM_CHARM) && IS_IMMUNE(father, IMM_CHARM)) {
        SET_BIT(mother->pcdata->genes[3], IMM_CHARM);
    }
    if(IS_IMMUNE(mother, IMM_HEAT) && IS_IMMUNE(father, IMM_HEAT)) {
        SET_BIT(mother->pcdata->genes[3], IMM_HEAT);
    }
    if(IS_IMMUNE(mother, IMM_COLD) && IS_IMMUNE(father, IMM_COLD)) {
        SET_BIT(mother->pcdata->genes[3], IMM_COLD);
    }
    if(IS_IMMUNE(mother, IMM_LIGHTNING) && IS_IMMUNE(father, IMM_LIGHTNING)) {
        SET_BIT(mother->pcdata->genes[3], IMM_LIGHTNING);
    }
    if(IS_IMMUNE(mother, IMM_ACID) && IS_IMMUNE(father, IMM_ACID)) {
        SET_BIT(mother->pcdata->genes[3], IMM_ACID);
    }
    if(IS_IMMUNE(mother, IMM_VOODOO) && IS_IMMUNE(father, IMM_VOODOO)) {
        SET_BIT(mother->pcdata->genes[3], IMM_VOODOO);
    }
    if(IS_IMMUNE(mother, IMM_HURL) && IS_IMMUNE(father, IMM_HURL)) {
        SET_BIT(mother->pcdata->genes[3], IMM_HURL);
    }
    if(IS_IMMUNE(mother, IMM_BACKSTAB) && IS_IMMUNE(father, IMM_BACKSTAB)) {
        SET_BIT(mother->pcdata->genes[3], IMM_BACKSTAB);
    }
    if(IS_IMMUNE(mother, IMM_KICK) && IS_IMMUNE(father, IMM_KICK)) {
        SET_BIT(mother->pcdata->genes[3], IMM_KICK);
    }
    if(IS_IMMUNE(mother, IMM_DISARM) && IS_IMMUNE(father, IMM_DISARM)) {
        SET_BIT(mother->pcdata->genes[3], IMM_DISARM);
    }
    if(IS_IMMUNE(mother, IMM_STEAL) && IS_IMMUNE(father, IMM_STEAL)) {
        SET_BIT(mother->pcdata->genes[3], IMM_STEAL);
    }
    if(IS_IMMUNE(mother, IMM_SLEEP) && IS_IMMUNE(father, IMM_SLEEP)) {
        SET_BIT(mother->pcdata->genes[3], IMM_SLEEP);
    }
    if(IS_IMMUNE(mother, IMM_DRAIN) && IS_IMMUNE(father, IMM_DRAIN)) {
        SET_BIT(mother->pcdata->genes[3], IMM_DRAIN);
    }
    mother->pcdata->genes[4] = number_range(1, 2);
    return;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number(char *arg) {
    if(*arg == '\0') {
        return FALSE;
    }
    for(; *arg != '\0'; arg++) {
        if(!isdigit(*arg)) {
            return FALSE;
        }
    }
    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument(char *argument, char *arg) {
    char *pdot;
    int number;
    for(pdot = argument; *pdot != '\0'; pdot++) {
        if(*pdot == '.') {
            *pdot = '\0';
            number = atoi(argument);
            *pdot = '.';
            strcpy(arg, pdot + 1);
            return number;
        }
    }
    strcpy(arg, argument);
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument(char *argument, char *arg_first) {
    char cEnd;
    while(isspace(*argument)) {
        argument++;
    }
    cEnd = ' ';
    if(*argument == '\'' || *argument == '"') {
        cEnd = *argument++;
    }
    while(*argument != '\0') {
        if(*argument == cEnd) {
            argument++;
            break;
        }
        *arg_first = LOWER(*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';
    while(isspace(*argument)) {
        argument++;
    }
    return argument;
}

