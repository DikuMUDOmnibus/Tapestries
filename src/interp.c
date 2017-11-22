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



bool	check_social	args( ( CHAR_DATA *ch, char *command, char *argument ) );
bool    check_aliases   args( ( CHAR_DATA *, char *, char *, char *) );

bool    MP_Commands     args( ( CHAR_DATA *ch ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2

/*
 * God Levels
 */
#define L_GOD		MAX_LEVEL
#define L_SUP		L_GOD - 1
#define L_DEI		L_SUP - 1
#define L_ANG		L_DEI - 1
#define L_HER		L_ANG - 1


/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    { "areas",      do_areas,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "auto",       do_auto,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "alias",      do_alias,	POS_DEAD,       ICOM_NONE, LOG_NORMAL,  TRUE	},
    { "autoexit",   do_autoexit,POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "autoloot",   do_autoloot,POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "autosac",    do_autosac, POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "allow",      do_allow,	POS_DEAD,	ICOM_BANS, LOG_ALWAYS,	TRUE	},
    { "at",         do_at,	POS_DEAD,	ICOM_AT,   LOG_NORMAL,	TRUE	},
    { "aedit",      do_aedit,	POS_DEAD,	ICOM_OLC,  LOG_NORMAL,	FALSE	},
    { "asave",      do_asave,	POS_DEAD,	ICOM_ASAVE, LOG_NORMAL,	FALSE	},
    { "alist",      do_alist,	POS_DEAD,	ICOM_OLC,  LOG_NORMAL,	FALSE	},

    { "blank",      do_blank,   POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "brief",      do_brief,   POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "bug",        do_bug,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "buy",        do_buy,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "brandish",   do_brandish,POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "bs",         do_backstab,POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "backstab",   do_backstab,POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "ban",        do_ban,	POS_DEAD,	ICOM_BANS, LOG_ALWAYS,	TRUE	},
    { "bamfin",     do_bamfin,	POS_DEAD,	ICOM_GOTO, LOG_NORMAL,	TRUE	},
    { "bamfout",    do_bamfout,	POS_DEAD,	ICOM_GOTO, LOG_NORMAL,	TRUE	},

    { "cast",       do_cast,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "commands",   do_commands,POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "compare",    do_compare,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "consider",   do_consider,POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "credits",    do_credits,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "channels",   do_channels,POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "colour",     do_colour,  POS_DEAD,       ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "color",      do_colour,  POS_DEAD,       ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "combine",    do_combine, POS_DEAD,       ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "config",     do_config,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "chat",       do_chat,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "close",      do_close,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "down",       do_down,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "description",do_description, POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "drink",      do_drink,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "drop",       do_drop,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "disarm",     do_disarm,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "deny",       do_deny,	POS_DEAD,	ICOM_BANS, LOG_ALWAYS,	TRUE	},
    { "disconnect", do_disconnect, POS_DEAD,    ICOM_BANS, LOG_ALWAYS,	TRUE	},

    { "east",       do_east,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "exits",      do_exits,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "equipment",  do_equipment, POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "examine",    do_examine,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "emote",      do_emote,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "eat",        do_eat,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "echo",       do_echo,	POS_DEAD,	ICOM_ECHO, LOG_ALWAYS,	TRUE	},
    { "endow",      do_endow,	POS_DEAD,	ICOM_ADMIN, LOG_ALWAYS,	TRUE	},

    { "fill",       do_fill,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "flee",       do_flee,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "follow",     do_follow,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "freeze",     do_freeze,	POS_DEAD,	ICOM_PENALTY, LOG_ALWAYS, TRUE	},
    { "force",      do_force,	POS_DEAD,	ICOM_FORCE,  LOG_ALWAYS, TRUE	},

    { "get",        do_get,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "gtell",      do_gtell,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "give",       do_give,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "group",      do_group,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "goto",       do_goto,	POS_DEAD,	ICOM_GOTO, LOG_NORMAL,	TRUE	},

    { "help",       do_help,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "hold",       do_wear,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "hide",       do_hide,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "holylight",  do_holylight, POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},

    { "inventory",  do_inventory, POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "idea",       do_idea,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "invis",      do_invis,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL, TRUE	},
    { "immtalk",    do_immtalk,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL, TRUE	},

    { "kill",       do_kill,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "kick",       do_kick,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "look",       do_look,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "list",       do_list,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "lock",       do_lock,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "log",        do_log,	POS_DEAD,	ICOM_PENALTY, LOG_ALWAYS, TRUE	},

    { "mail",       do_mail,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { "murde",      do_murde,	POS_FIGHTING,	ICOM_PKILL, LOG_NORMAL,	FALSE	},
    { "murder",     do_murder,	POS_FIGHTING,	ICOM_PKILL, LOG_ALWAYS,	TRUE	},
    { "mload",      do_mload,	POS_DEAD,	ICOM_LOAD, LOG_ALWAYS,	TRUE	},
    { "mset",       do_mset,	POS_DEAD,	ICOM_SET, LOG_ALWAYS,	TRUE	},
    { "memory",     do_memory,	POS_DEAD,	ICOM_SOCKET, LOG_NORMAL, TRUE	},
    { "mfind",      do_mfind,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	TRUE	},
    { "mstat",      do_mstat,	POS_DEAD,	ICOM_STAT, LOG_NORMAL,	TRUE	},
    { "mwhere",     do_mwhere,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},
    { "mpstat",     do_mpstat,	POS_DEAD,	ICOM_STAT, LOG_NORMAL,	TRUE	},
    { "medit",      do_medit,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	FALSE	},
    /*
     * MOBprogram commands.
     */
    { "mpasound",   do_mpasound, POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpjunk",     do_mpjunk,  POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpecho",     do_mpecho,  POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpechoat",   do_mpechoat, POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpechoaround", do_mpechoaround, POS_DEAD, ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpkill",     do_mpkill,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpmload",    do_mpmload,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpoload",    do_mpoload,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mppurge",    do_mppurge,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpgoto",     do_mpgoto,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpat",       do_mpat,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mptransfer", do_mptransfer, POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },
    { "mpforce",    do_mpforce,	POS_DEAD,	ICOM_MOB, LOG_NORMAL,	FALSE },

    { "north",      do_north,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "ne",         do_northeast, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "nw",         do_northwest, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "note",       do_note,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "noemote",    do_noemote,	POS_DEAD,	ICOM_PENALTY, LOG_NORMAL, TRUE	},
    { "notell",     do_notell,	POS_DEAD,	ICOM_PENALTY, LOG_NORMAL, TRUE	},
    { "northeast",  do_northeast, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "northwest",  do_northwest,POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "order",      do_order,	POS_RESTING,	ICOM_NONE, LOG_ALWAYS,	TRUE	},
    { "open",       do_open,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "oload",      do_oload,	POS_DEAD,	ICOM_LOAD, LOG_ALWAYS,	TRUE	},
    { "oset",       do_oset,	POS_DEAD,	ICOM_SET, LOG_ALWAYS,	TRUE	},
    { "owhere",     do_owhere,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},
    { "ofind",      do_ofind,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	TRUE	},
    { "ostat",      do_ostat,	POS_DEAD,	ICOM_STAT, LOG_NORMAL,	TRUE	},
    { "oedit",      do_oedit,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	FALSE	},

    { "pagelength", do_pagelen,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "password",   do_password, POS_DEAD,	ICOM_NONE, LOG_NEVER,	TRUE	},
    { "prompt",     do_prompt,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "pose",       do_pose,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "pick",       do_pick,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "put",        do_put,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "practice",   do_practice,POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "pardon",     do_pardon,	POS_DEAD,	ICOM_PENALTY, LOG_ALWAYS, TRUE	},
    { "peace",      do_peace,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},
    { "purge",      do_purge,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},

    { "quaff",      do_quaff,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "qui",        do_qui,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { "quit",       do_quit,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "qwest",      do_qwest,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "rest",       do_rest,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "report",     do_report,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "reply",      do_reply,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "recite",     do_recite,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "remove",     do_remove,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "rescue",     do_rescue,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "recall",     do_recall,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "rent",       do_rent,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { "reboo",      do_reboo,	POS_DEAD,	ICOM_ADMIN, LOG_NORMAL,	FALSE	},
    { "reboot",     do_reboot,	POS_DEAD,	ICOM_ADMIN, LOG_ALWAYS,	TRUE	},
    { "restore",    do_restore,	POS_DEAD,	ICOM_BASIC, LOG_ALWAYS,	TRUE	},
    { "rset",       do_rset,	POS_DEAD,	ICOM_SET, LOG_ALWAYS,	TRUE	},
    { "recho",      do_recho,	POS_DEAD,	ICOM_ECHO, LOG_ALWAYS,	TRUE	},
    { "return",     do_return,	POS_DEAD,	ICOM_SWITCH, LOG_NORMAL, TRUE	},
    { "rstat",      do_rstat,	POS_DEAD,	ICOM_STAT, LOG_NORMAL,	TRUE	},
    { "redit",      do_redit,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	FALSE	},
    { "resets",     do_resets,	POS_DEAD,	ICOM_OLC, LOG_NORMAL,	FALSE	},

    { "south",      do_south,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "se",         do_southeast, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sw",         do_southwest, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sleep",      do_sleep,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "stand",      do_stand,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "score",      do_score,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "slist",      do_slist,	POS_DEAD,       ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "socials",    do_socials,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "say",        do_say,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "shout",      do_shout,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sell",       do_sell,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sacrifice",  do_sacrifice, POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "save",       do_save,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sleep",      do_sleep,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sneak",      do_sneak,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "spells",     do_spells,	POS_SLEEPING,   ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "split",      do_split,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "steal",      do_steal,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "sockets",    do_users,	POS_DEAD,	ICOM_SOCKET, LOG_NORMAL, TRUE	},
    { "shutdow",    do_shutdow,	POS_DEAD,	ICOM_ADMIN, LOG_NORMAL,	FALSE	},
    { "shutdown",   do_shutdown, POS_DEAD,	ICOM_ADMIN, LOG_ALWAYS,	TRUE	},
    { "silence",    do_silence,	POS_DEAD,	ICOM_PENALTY, LOG_NORMAL, TRUE	},
    { "sla",        do_sla,	POS_DEAD,	ICOM_SLAY, LOG_NORMAL,	FALSE	},
    { "slay",       do_slay,	POS_DEAD,	ICOM_SLAY, LOG_ALWAYS,	TRUE	},
    { "snoop",      do_snoop,	POS_DEAD,	ICOM_SNOOP, LOG_NORMAL,	TRUE	},
    { "sset",       do_sset,	POS_DEAD,	ICOM_SET, LOG_ALWAYS,	TRUE	},
    { "slookup",    do_slookup,	POS_DEAD,	ICOM_STAT, LOG_NORMAL,	TRUE	},
    { "switch",     do_switch,	POS_DEAD,	ICOM_SWITCH, LOG_ALWAYS, TRUE	},
    { "southeast",  do_southeast, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "southwest",  do_southwest, POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "tell",       do_tell,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "time",       do_time,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "typo",       do_typo,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "title",      do_title,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "take",       do_get,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "train",      do_train,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "transfer",   do_transfer, POS_DEAD,	ICOM_TRANS, LOG_ALWAYS,	TRUE	},

    { "up",         do_up,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "unread",     do_unread,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "unlock",     do_unlock,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "users",      do_users,	POS_DEAD,	ICOM_SOCKET, LOG_NORMAL, TRUE	},

    { "value",      do_value,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "visible",    do_visible,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { "west",       do_west,	POS_STANDING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wield",      do_wear,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "weather",    do_weather,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "who",        do_who,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wimpy",      do_wimpy,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wear",       do_wear,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wake",       do_wake,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "where",      do_where,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wizlist",    do_wizlist,	POS_DEAD,       ICOM_NONE, LOG_NORMAL,	TRUE	},
    { "wizhelp",    do_wizhelp,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	TRUE	},
    { "wizify",     do_wizify,	POS_DEAD,	ICOM_ADMIN, LOG_ALWAYS,	TRUE	},
    { "wizlock",    do_wizlock,	POS_DEAD,	ICOM_BASIC, LOG_ALWAYS,	TRUE	},

    { "zap",        do_zap,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	TRUE	},

    { ",",          do_emote,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { ".",          do_chat,	POS_SLEEPING,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { ";",          do_gtell,	POS_DEAD,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { "'",          do_say,	POS_RESTING,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { "/",          do_recall,	POS_FIGHTING,	ICOM_NONE, LOG_NORMAL,	FALSE	},
    { ":",          do_immtalk,	POS_DEAD,	ICOM_BASIC, LOG_NORMAL,	FALSE	},

    /*
     * End of list.
     */
    { "",           0,		POS_DEAD,	ICOM_NONE, LOG_NORMAL,	FALSE	}
};


/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 *
 * Updated for hashed commands and socials for improved
 * Performance. -Ant
 *
 * Added aliasing code, required addition of a new argument.
 *  -Ant
 */
void interpret( CHAR_DATA *ch, char *argument, bool recursing )
{
    char	alias[ MAX_INPUT_LENGTH   ];
    char	command[ MAX_INPUT_LENGTH ];
    char	logline[ MAX_INPUT_LENGTH ];
    int		cmd;
    bool	found;

    /*
     * Strip leading spaces.
     */
    while( isspace( *argument ) )
	argument++;
    if( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if( !IS_NPC( ch )
     && IS_SET( ch->pen, PEN_FREEZE ) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if( !isalpha( argument[0] )
     && !isdigit( argument[0] ) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while( isspace( *argument ) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     * Updated for hashing, Mar '96 -Ant
     * Original idea and some code by Ironhand
     * Altered for immortal command flagging, and
     * elimination of level based code. May '96 -Ant
     */
    found = FALSE;
    for( cmd = Hash_commands[ command[ 0 ]%126 ] ;
         command[ 0 ] == cmd_table[ cmd ].name[ 0 ] ; cmd++ )
    {
        if( !str_prefix( command, cmd_table[ cmd ].name )
         && ( ( ( cmd_table[ cmd ].flag == ICOM_NONE )
	     || ( !IS_NPC( ch )
		&& IS_SET( ch->pcdata->immortal, cmd_table[ cmd ].flag ) ) )
	    || MP_Commands( ch ) ) )
        {
            found = TRUE;
            break;
        }
    }

    /*
     * Log and snoop.
     */
    if( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if( ( !IS_NPC( ch )
      && IS_SET( ch->pen, PEN_LOG ) )
     || fLogAll
     || ( found && cmd_table[cmd].log == LOG_ALWAYS ) )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	log_string( log_buf );
    }

    if( ch->desc
     && ch->desc->snoop_by )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if( !found )
    {
	if( !recursing && check_aliases( ch, alias, command, argument ) )
        {
	    char *tok;

	    tok = strtok( alias, ";\0" );
	    if( !ch->wizbit )
	    {
		while( tok && !ch->wait )
		{
		    interpret( ch, tok, TRUE );
		    tok = strtok( NULL, ";\0" );
		}
		if( tok && ch->wait && ch->fighting )
		{
		    send_to_char( 
    "YoU {mc{gO{rn{Yf{Gu{MS{RE {cy{yO{gU{Br{CS{Ye{mL{rf{x, tOo mUCh iS GoINg oN!\n\r",
			ch );
		    ch->wait += 8;
 		}
	    }
	    else
	    {
		while( tok )
		{
			interpret( ch, tok, TRUE );
			tok = strtok( NULL, ";\0" );
		}
	    }
	    return;
	}
	/*
	 * Look for command in socials table.
	 */
	if( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	    case POS_DEAD:
		send_to_char( "Lie still; you are DEAD.\n\r", ch );
		break;

	    case POS_MORTAL:
	    case POS_INCAP:
		send_to_char( "You are hurt far too bad for that.\n\r", ch );
		break;

	    case POS_STUNNED:
		send_to_char( "You are too stunned to do that.\n\r", ch );
		break;

	    case POS_SLEEPING:
		send_to_char( "In your dreams, or what?\n\r", ch );
		break;

	    case POS_RESTING:
		send_to_char( "Nah... You feel too relaxed...\n\r", ch);
		break;

	    case POS_FIGHTING:
		send_to_char( "No way!  You are still fighting!\n\r", ch);
		break;
	}
	return;
    }

    /*
     * Dispatch the command.
     */
    ( *cmd_table[cmd].do_fun )( ch, argument );

    tail_chain( );
    return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA	*victim;
    char	arg[ MAX_INPUT_LENGTH ];
    SOCIAL_DATA	*pSocial;
    bool	found;
    int		iHash;

    found	= FALSE;
    iHash	= command[ 0 ] % 126;

    for( pSocial = social_first[ iHash ] ; pSocial ; pSocial = pSocial->next )
    {
	if( !str_prefix( command, pSocial->name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if( !found )
	return FALSE;

    switch( ch->position )
    {
	case POS_DEAD:
	    send_to_char( "Lie still; you are {rDEAD{x.\n\r",             ch );
	    return TRUE;

	case POS_INCAP:
	case POS_MORTAL:
	    send_to_char_bw( "You are hurt far too badly for that.\n\r", ch );
	    return TRUE;

	case POS_STUNNED:
	    send_to_char_bw( "You are too stunned to do that.\n\r",      ch );
	    return TRUE;

	case POS_SLEEPING:
	    /*
	     * I just know this is the path to a 12" 'if' statement.  :(
	     * But two players asked for it already!  -- Furey
	     */
	    if( !str_cmp( pSocial->name, "snore" ) )
		break;
	    send_to_char_bw( "In your dreams, or what?\n\r",             ch );
	    return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if( !*arg )
    {
	act( pSocial->char_no_arg,   ch, NULL, victim, TO_CHAR		);
	act( pSocial->others_no_arg, ch, NULL, victim, TO_ROOM		);
    }
    else if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char_bw( "They aren't here.\n\r",                    ch );
    }
    else if( victim == ch )
    {
	act( pSocial->char_auto,     ch, NULL, victim, TO_CHAR		);
	act( pSocial->others_auto,   ch, NULL, victim, TO_ROOM		);
    }
    else
    {
	act( pSocial->char_found,    ch, NULL, victim, TO_CHAR		);
	act( pSocial->vict_found,    ch, NULL, victim, TO_VICT		);
	act( pSocial->others_found,  ch, NULL, victim, TO_NOTVICT	);
    }

    return TRUE;
}

bool check_aliases( CHAR_DATA *ch, char *retval, char *command, char *argument )
{
    ALIAS 	*alias;
    char 	args[20][MAX_INPUT_LENGTH];
    char 	*p;
    char 	*p2;
    char 	*p3;
    int 	argcount;
    int 	argindex;

    if( IS_NPC( ch ) )
	return FALSE; 

    alias = ch->pcdata->aliases;
    if( alias == NULL || alias->next == NULL )
	return FALSE;

    for( alias = alias->next ; alias != NULL ; alias = alias->next )
    {
	if( command[0] == (alias->command)[0] && 
	    !str_prefix( command, alias->command ) )
	    break;
    } 
 
    if( alias == NULL ) 
	return FALSE;

    /* copy argument into args, for indexing purposes */
    for( argcount=0; argument[0] != '\0' && argcount <= 19; argcount++ )
	argument = one_argument( argument, args[argcount] );

    p = retval;

    /* replace %1 with first argument, and so forth */ 
    for( p2 = alias->string; *p2 != '\0' ; p2 ++ ) 
    { 
	if ( *p2 != '%' ) 
	    *p++ = *p2; 
	else 
	{
	    p2++;
	    if( *p2 == '%' ) *p++ = '%';
	    else
	    {
		argindex = *p2 - '0';
		if( argindex > argcount )
		    continue;
		for( p3 = args[argindex-1] ; *p3 != '\0' ; p3++ )
		    *p++ = *p3;
		*p++ = ' ';
	    }
	}
    }
    *p = '\0';
    return TRUE;
}  



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if( *arg == '\0' )
	return FALSE;

    if( *arg == '+' || *arg == '-' )
        arg++;

    for( ; *arg != '\0' ; arg++ )
    {
	if( !isdigit( *arg ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char	*pdot;
    int		number;
    
    for( pdot = argument ; *pdot != '\0' ; pdot++ )
    {
	if( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char	cEnd;

    while( isspace( *argument ) )
	argument++;

    cEnd = ' ';
    if( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while( *argument != '\0' )
    {
	if( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER( *argument );
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while( isspace( *argument ) )
	argument++;

    return argument;
}

bool IS_SWITCHED( CHAR_DATA *ch )
{
    if( !IS_NPC( ch ) )
	return FALSE;

    if( !ch->desc )
	return FALSE;

    return TRUE;
}

/* Can MOBProged mobs
 * use mpcommands? TRUE if yes.
 * - Kahn
 */
bool MP_Commands( CHAR_DATA *ch )
{
    if( IS_SWITCHED( ch ) )
	return FALSE;

    if( IS_NPC( ch )
     && ch->pIndexData->progtypes
     && !IS_AFFECTED( ch, AFF_CHARM ) )
	return TRUE;

    return FALSE;
}
