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
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
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
typedef struct  mob_prog_data           MPROG_DATA;         /* MOBprogram */
typedef struct  mob_prog_act_list       MPROG_ACT_LIST;     /* MOBprogram */

typedef struct	colour_data		COLOUR_DATA;
typedef struct  social_type             SOCIAL_DATA;
typedef struct  class_type		CLASS_DATA;	/* New System	*/
typedef struct  race_type		RACE_DATA;	/* New System	*/
typedef struct  part_type		PART_DATA;	/* New System	*/
typedef struct	alias			ALIAS;

/*
 * New object style structures
 */
typedef struct  weapon_data		WEAPON_DATA;
typedef struct  armour_data		ARMOUR_DATA;
typedef struct  scroll_data		SCROLL_DATA;
typedef struct  potion_data		POTION_DATA;
typedef struct  pill_data		PILL_DATA;
typedef struct  staff_data		STAFF_DATA;
typedef struct  wand_data		WAND_DATA;
typedef struct  light_data		LIGHT_DATA;
typedef struct  food_data		FOOD_DATA;
typedef struct  drink_data		DRINK_DATA;
typedef struct  cont_data		CONT_DATA;
typedef struct  money_data		MONEY_DATA;

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
#define MAX_INPUT_LENGTH	  512
#define MAGIC_NUM	      5288951



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		   90
#define MAX_CLASS		    4
#define MAX_LEVEL		  110
#define LEVEL_HERO		  ( MAX_LEVEL - 10 )
#define LEVEL_IMMORTAL		  ( MAX_LEVEL - 9  )
#define NOTE_TIMEOUT		  ( 604800 * PULSE_PER_SECOND ) /* 7 days	*/

/* TODO
 * Replace These When We go Online!
 *
#define PULSE_PER_SECOND	       4
#define PULSE_VIOLENCE		  (    3 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  (    4 * PULSE_PER_SECOND )
#define PULSE_TICK		  (   30 * PULSE_PER_SECOND )
#define PULSE_AREA		  (   60 * PULSE_PER_SECOND )
#define PULSE_DB_DUMP		  ( 3600 * PULSE_PER_SECOND ) *//* 60 minutes	*//*
#define PULSE_NOTES		  (86400 * PULSE_PER_SECOND ) *//* every day	*//*
 *
 */

/*
 * Temporary pulse times for debugging and testing. -Ant
 */
#define PULSE_PER_SECOND	       4
#define PULSE_VIOLENCE		  (    2 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  (    2 * PULSE_PER_SECOND )
#define PULSE_TICK		  (    5 * PULSE_PER_SECOND )
#define PULSE_AREA		  (   10 * PULSE_PER_SECOND )
#define PULSE_DB_DUMP		  ( 3600 * PULSE_PER_SECOND ) /* 60 minutes	*/
#define PULSE_NOTES		  (86400 * PULSE_PER_SECOND ) /* every day	*/

/*
 * Ascii flag conversion.
 * Flags cannot exceed 'ee' (1073741824) as the maximum integer value
 * on most systems is 2147483647, and setting all the flags would give
 * this value. -Ant
 */
#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864
#define bb			134217728
#define cc			268435456    
#define dd			536870912
#define ee			1073741824


/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};

struct alias
{
    char *command;
    char *string;
    ALIAS *next;
};

/*
 * New object style structures
 */
struct weapon_data
{
    WEAPON_DATA	*next;		/* for free list */
    int		type;		/* weapon type */
    int		message;	/* damage message */
    long	flags;		/* weapon flags */
    int		hit;		/* hit bounus */
    int		damage[3];	/* damage dice */
};

struct armour_data
{
    ARMOUR_DATA *next;		/* for free list */
    int		ac[4];		/* ac values */
};

struct cont_data
{
    CONT_DATA 	*next;		/* for free list */
    long	closed;		/* open, closed, etc */
    int		key;		/* to fit the lock */
    int		capacity;	/* max weight carried */
    int		single;		/* max weight for one item */
};

struct drink_data  	/* drink containers, fountains */
{
    DRINK_DATA 	*next;		/* for free list */
    int		volume;		/* liquid content */
    int		limit;		/* liquid max */
    int		liquid;		/* type of liquid */
    bool 	poison;		/* poisoned? */
};

struct food_data
{
    FOOD_DATA	*next;		/* for free list */
    int		bite;		/* how many food units per eat */
    bool	poison;		/* poisoned or safe */
};

struct potion_data
{
    POTION_DATA	*next;		/* for free list */
    int		level;		/* level of spells */
    int		spell[4];	/* spells on item */
};

struct pill_data
{
    PILL_DATA	*next;		/* for free list */
    int		level;		/* level of spells */
    int		spell[4];	/* spells on item */
};

struct scroll_data
{
    SCROLL_DATA	*next;		/* for free list */
    int		level;		/* level of spells */
    int		spell[4];	/* spells on item */
};

struct staff_data
{
    STAFF_DATA	*next;		/* for free list */
    int		level;		/* level of spell */
    int		charges;	/* number of charges */
    int		max;		/* max charges */
    int 	spell;		/* spell of item */
};

struct wand_data
{
    WAND_DATA	*next;		/* for free list */
    int		level;		/* level of spell */
    int		charges;	/* number of charges */
    int		max;		/* max charges */
    int 	spell;		/* spell of item */
};

struct light_data
{
    LIGHT_DATA	*next;		/* for free list */
    int		duration;	/* duration of light */
    int		intensity;	/* how bright the light is */
};

struct money_data
{
    MONEY_DATA	*next;		/* for free list */
    int		gold;		/* gold coins */
    int		silver;		/* silver coins */
    int		copper;		/* copper coins */
};


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
    int		minute;
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
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_COLOUR			 6
#define CON_GET_NEW_SEX			 7
#define CON_GET_NEW_RACE		 8
#define CON_GET_NEW_CLASS		 9
#define CON_READ_MOTD			10
#define CON_BREAK_CONNECT		11



/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;

    void *              pEdit;		/* OLC */
    char **             pString;	/* OLC */
    int			editor;		/* OLC */

    char *		host;
    char *		user;
    sh_int		descriptor;
    sh_int		connected;
    bool		fcommand;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *		outbuf;
    int			outsize;
    int			outtop;

    sh_int		port;
    int			timer;
    long		address;
    sh_int		host_name_state;
};

/*
 * Hostname and User resolving stuff.
 */
#define	ID_RESOLVED	0
#define	ID_IN_PROGRESS	1
#define	ID_PENDING	2


/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
};

struct	dex_app_type
{
    sh_int	defensive;
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
    HELP_DATA *	prev;
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
 * Per-class stuff.
 * Adapted for new System. -Ant
 */
struct	class_type
{
    CLASS_DATA	*next;
    CLASS_DATA	*prev;
    char 	*name;		/* Name of this class/subclass		*/
    char 	who_name[ 5 ];	/* Four-letter name for 'who'		*/
    sh_int	number;		/* Class number for identification	*/
    sh_int	type;		/* Class type 0+ 0 = primary class	*/
    sh_int	attr_prime;	/* Prime attribute			*/
    sh_int	weapon;		/* First weapon				*/
    sh_int	skill_adept;	/* Maximum skill level			*/
    sh_int	defence;	/* Base Defence rating			*/
    sh_int	attack;		/* Base Attack rating			*/
    sh_int	hp_min;		/* Min hp gained on leveling		*/
    sh_int	hp_max;		/* Max hp gained on leveling		*/
    sh_int	mana_min;	/* Min Class mana gain on level		*/
    sh_int	mana_max;	/* Max Class mana gain on level		*/
    int		guild;		/* Vnum of guild recall room		*/
    long	sub_classes;	/* Which Sub-classes are possible.	*/
};

#define CLASS_PRIMARY	0
#define CLASS_SUB	1
#define CLASS_SUBSUB	2


/*
 * Per-race stuff.
 * -Ant
 */
struct	race_type
{
    RACE_DATA	*next;			/* For Free list and stuff	*/
    RACE_DATA	*prev;			/* For Free list and stuff	*/
    char 	*name;			/* Real race name		*/
    char 	who_name[ 7 ];		/* Six-letter name for 'who'	*/
    sh_int	number;			/* race	number for ident	*/
    sh_int	base_stats[ 5 ];	/* Basic stats for this race	*/
    sh_int	max_stats[ 5 ];		/* Maximum stat including mods	*/
    sh_int	train_stats[ 5 ];	/* Maximum trained stat		*/
    long	affects;		/* Flag of perm affects		*/
    long	parts;			/* Flag of body parts		*/
    long	classes;		/* Classes available to us	*/
    long	sub_classes;		/* Sub-Classes available	*/
    bool	playable;		/* Can players use this?	*/
};

/*
 * Part stuff.
 * Parts are used for combat and a few other bits and pieces...
 * The idea is that an area builder just has to set the part flag
 * when the creature is loaded the parts will be added automagically :>
 * -Ant
 */
struct	part_type
{
    PART_DATA	*next;	/* For Linked lists		*/
    PART_DATA	*prev;	/* For Linked lists		*/
    sh_int	armour;	/* For calculation of damages	*/
    sh_int	scars;	/* For some nice touches	*/
    long	type;	/* Flag of body part type	*/
};

#define PART_HEAD		(A)	/* Most things should have a head....	*/
#define PART_CHEST		(B)	/* Humanoid and four legged things	*/
#define PART_ABDOMIN		(C)	/*     "     "    "     "      "	*/
#define PART_LEFT_ARM		(D)	/* Left arm				*/
#define PART_RIGHT_ARM		(E)	/* Right arm				*/
#define PART_LEFT_LEG		(F)	/* Yer Yer Yer....Left leg		*/
#define PART_RIGHT_LEG		(G)	/* Right Leg				*/
#define PART_LFRONT_LEG		(H)	/* Left Front Leg, for quadrupeds	*/
#define PART_RFRONT_LEG		(I)	/* Right Front Leg			*/
#define PART_LREAR_LEG		(J)	/* Left Rear Leg			*/
#define PART_RREAR_LEG		(K)	/* Right Rear Leg			*/
#define PART_LEFT_WING		(L)	/* Left Wing				*/
#define PART_RIGHT_WING		(M)	/* Right Wing				*/
#define PART_TAIL		(N)	/* Tail, how sweet, is it spiky too?	*/
#define PART_BODY		(O)	/* Body, if none of the above :>	*/
#define PART_NONE		(P)	/* Ghosts and the like			*/
#define PART_MAX		(Q)

/*
 * New data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
    time_t      date_stamp;
    int		spool;
    bool	anon;
};

/*
 *   Note types/spools
 */
#define	SPOOL_NOTE	0
#define SPOOL_IDEA	1
#define SPOOL_MAIL	2
#define SPOOL_BUG	3
#define SPOOL_QUEST	4
#define MAX_SPOOL	5

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
    long		bitvector;
};



/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};

/*
 * ColoUr stuff v2.0, by Lope.
 */
#define CLEAR		"\e[0m"		/* Resets Colour	*/
#define C_RED		"\e[0;31m"	/* Normal Colours	*/
#define C_GREEN		"\e[0;32m"
#define C_YELLOW	"\e[0;33m"
#define C_BLUE		"\e[0;34m"
#define C_MAGENTA	"\e[0;35m"
#define C_CYAN		"\e[0;36m"
#define C_WHITE		"\e[0;37m"
#define C_D_GREY	"\e[1;30m"  	/* Light Colors		*/
#define C_B_RED		"\e[1;31m"
#define C_B_GREEN	"\e[1;32m"
#define C_B_YELLOW	"\e[1;33m"
#define C_B_BLUE	"\e[1;34m"
#define C_B_MAGENTA	"\e[1;35m"
#define C_B_CYAN	"\e[1;36m"
#define C_B_WHITE	"\e[1;37m"

#define COLOUR_NONE	7		/* White, hmm...	*/
#define RED		1		/* Normal Colours	*/
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7
#define BLACK		0

#define NORMAL		0		/* Bright/Normal colours */
#define BRIGHT		1

#define ALTER_COLOUR( type )	if( !str_prefix( argument, "red" ) )		\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = RED;		\
				}						\
				else if( !str_prefix( argument, "hi-red" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = RED;		\
				}						\
				else if( !str_prefix( argument, "green" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = GREEN;		\
				}						\
				else if( !str_prefix( argument, "hi-green" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = GREEN;		\
				}						\
				else if( !str_prefix( argument, "yellow" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = YELLOW;		\
				}						\
				else if( !str_prefix( argument, "hi-yellow" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = YELLOW;		\
				}						\
				else if( !str_prefix( argument, "blue" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = BLUE;		\
				}						\
				else if( !str_prefix( argument, "hi-blue" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = BLUE;		\
				}						\
				else if( !str_prefix( argument, "magenta" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = MAGENTA;	\
				}						\
				else if( !str_prefix( argument, "hi-magenta" ) ) \
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = MAGENTA;	\
				}						\
				else if( !str_prefix( argument, "cyan" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = CYAN;		\
				}						\
				else if( !str_prefix( argument, "hi-cyan" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = CYAN;		\
				}						\
				else if( !str_prefix( argument, "white" ) )	\
				{						\
				    ch->pcdata->code->type[0] = NORMAL;		\
				    ch->pcdata->code->type[1] = WHITE;		\
				}						\
				else if( !str_prefix( argument, "hi-white" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = WHITE;		\
				}						\
				else if( !str_prefix( argument, "grey" ) )	\
				{						\
				    ch->pcdata->code->type[0] = BRIGHT;		\
				    ch->pcdata->code->type[1] = BLACK;		\
				}						\
				else						\
				{						\
		send_to_char_bw( "Unrecognised colour, unchanged.\n\r", ch );	\
				    return;					\
				}


struct	colour_data
{
    COLOUR_DATA * 	next;
    int			text[2];		/* {t */
    int			auction[2];		/* {a */
    int			chat[2];		/* {d */
    int			music[2];		/* {e */
    int			question[2];		/* {q */
    int			answer[2];		/* {f */
    int			class_text[2];		/* {h */
    int			class_name[2];		/* {H */
    int			immtalk_text[2];	/* {i */
    int			immtalk_type[2];	/* {I */
    int			info[2];		/* {j */
    int			say[2];			/* {u */
    int			tell[2];		/* {k */
    int			reply[2];		/* {l */
    int			gtell_text[2];		/* {n */
    int			gtell_type[2];		/* {N */
    int			bbro[2];		/* {B */
    int			room_title[2];		/* {s */
    int			room_text[2];		/* {S */
    int			room_exits[2];		/* {o */
    int			room_things[2];		/* {O */
    int			prompt[2];		/* {p */
    int			fight_death[2];		/* {1 */
    int			fight_yhit[2];		/* {2 */
    int			fight_ohit[2];		/* {3 */
    int			fight_thit[2];		/* {4 */
    int			fight_skill[2];		/* {5 */
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
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		(A)		/* Auto set for mobs	*/
#define ACT_SENTINEL		(B)		/* Stays in one room	*/
#define ACT_SCAVENGER		(C)		/* Picks up objects	*/
#define ACT_AGGRESSIVE		(D)		/* Attacks PC's		*/
#define ACT_STAY_AREA		(E)		/* Won't leave area	*/
#define ACT_WIMPY		(F)		/* Flees when hurt	*/
#define ACT_PET			(G)		/* Auto set for pets	*/
#define ACT_TRAIN		(H)		/* Can train PC's	*/
#define ACT_PRACTICE		(I)		/* Can practice PC's	*/



/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		(A)
#define AFF_INVISIBLE		(B)
#define AFF_DETECT_EVIL		(C)
#define AFF_DETECT_INVIS	(D)
#define AFF_DETECT_MAGIC	(E)
#define AFF_DETECT_HIDDEN	(F)
#define AFF_HOLD		(G)	/* Unused	*/
#define AFF_SANCTUARY		(H)
#define AFF_FAERIE_FIRE		(I)
#define AFF_INFRARED		(J)
#define AFF_CURSE		(K)
#define AFF_FLAMING		(L)	/* Unused	*/
#define AFF_POISON		(M)
#define AFF_PROTECT		(N)
#define AFF_PARALYSIS		(O)	/* Unused	*/
#define AFF_SNEAK		(P)
#define AFF_HIDE		(Q)
#define AFF_SLEEP		(R)
#define AFF_CHARM		(S)
#define AFF_FLYING		(T)
#define AFF_PASS_DOOR		(U)



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

#define OBJ_VNUM_NOTE		     23

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716



/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		(A)
#define ITEM_SCROLL		(B)
#define ITEM_WAND		(C)
#define ITEM_STAFF		(D)
#define ITEM_WEAPON		(E)
#define ITEM_TREASURE		(F)
#define ITEM_ARMOUR		(G)
#define ITEM_POTION		(H)
#define ITEM_FURNITURE		(I)
#define ITEM_TRASH		(J)
#define ITEM_CONTAINER		(K)
#define ITEM_DRINK_CON		(L)
#define ITEM_KEY		(M)
#define ITEM_FOOD		(N)
#define ITEM_MONEY		(O)
#define ITEM_BOAT		(P)
#define ITEM_CORPSE_NPC		(Q)
#define ITEM_CORPSE_PC		(R)
#define ITEM_FOUNTAIN		(S)
#define ITEM_PILL		(T)



/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		(A)
#define ITEM_HUM		(B)
#define ITEM_DARK		(C)
#define ITEM_LOCK		(D)
#define ITEM_EVIL		(E)
#define ITEM_INVIS		(F)
#define ITEM_MAGIC		(G)
#define ITEM_NODROP		(H)
#define ITEM_BLESS		(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)



/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD		(N)
#define ITEM_HOLD		(O)

/*
 * Weapon Classes
 */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_POLEARM		6

/*
 * Weapon Extra Flags
 */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)

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



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		(A)
#define CONT_PICKPROOF		(B)
#define CONT_CLOSED		(C)
#define CONT_LOCKED		(D)



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		(A)
#define ROOM_NO_MOB		(B)
#define ROOM_INDOORS		(C)
#define ROOM_PRIVATE		(D)
#define ROOM_SAFE		(E)
#define ROOM_SOLITARY		(F)
#define ROOM_PET_SHOP		(G)
#define ROOM_NO_RECALL		(H)



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		0
#define DIR_EAST		1
#define DIR_SOUTH		2
#define DIR_WEST		3
#define DIR_UP			4
#define DIR_DOWN		5
#define DIR_NE			6
#define DIR_NORTHEAST		6
#define DIR_NW			7
#define DIR_NORTHWEST		7
#define DIR_SE			8
#define DIR_SOUTHEAST		8
#define DIR_SW			9
#define DIR_SOUTHWEST		9



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		(A)
#define EX_CLOSED		(B)
#define EX_LOCKED		(C)
#define EX_PICKPROOF		(D)



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
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		0
#define WEAR_LIGHT		(A)
#define WEAR_FINGER_L		(B)
#define WEAR_FINGER_R		(C)
#define WEAR_NECK_1		(D)
#define WEAR_NECK_2		(E)
#define WEAR_BODY		(F)
#define WEAR_HEAD		(G)
#define WEAR_LEGS		(H)
#define WEAR_FEET		(I)
#define WEAR_HANDS		(J)
#define WEAR_ARMS		(K)
#define WEAR_SHIELD		(L)
#define WEAR_ABOUT		(M)
#define WEAR_WAIST		(N)
#define WEAR_WRIST_L		(O)
#define WEAR_WRIST_R		(P)
#define WEAR_WIELD		(Q)
#define WEAR_HOLD		(R)
#define MAX_WEAR		(S)



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
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_FIGHTING		      6
#define POS_STANDING		      7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		(A)		/* Don't EVER set.	*/
#define PLR_BOUGHT_PET		(B)
#define PLR_AUTOEXIT		(C)
#define PLR_AUTOLOOT		(D)
#define PLR_AUTOSAC             (E)
#define PLR_BLANK		(F)
#define PLR_BRIEF		(G)
#define PLR_COMBINE		(H)
#define PLR_PROMPT		(I)
#define PLR_TELNET_GA		(J)
#define PLR_HOLYLIGHT		(K)
#define PLR_WIZINVIS		(L)
#define PLR_COLOUR		(M)		/* NEW OCT 94 Ant */
#define PLR_SWITCH		(N)

#define PLR_THIEF		(T)
#define PLR_KILLER		(U)

/*
 * PENalty bits for players.
 */
#define PEN_LOG			(A)
#define PEN_DENY		(B)
#define PEN_FREEZE		(C)
#define	PEN_SILENCE		(D)
#define PEN_NOEMOTE		(E)
#define PEN_NOTELL		(F)
#define PEN_NOCHAT		(G)

/*
 * CHAN bits for players.
 */
#define	CHAN_CHAT		(A)
#define	CHAN_IMM		(B)
#define	CHAN_TELL		(C)
#define	CHAN_QUIET		(D)

/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		(A)
#define	CHANNEL_CHAT		(B)
#define	CHANNEL_HACKER		(C)
#define	CHANNEL_IMMTALK		(D)
#define	CHANNEL_MUSIC		(E)
#define	CHANNEL_QUESTION	(F)
#define	CHANNEL_SHOUT		(G)
#define	CHANNEL_YELL		(H)



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    MPROG_DATA *        mobprogs;               /* Used by MOBprogram */
    SPEC_FUN *		spec_fun;
    SHOP_DATA *		pShop;
    AREA_DATA *		area;			/* OLC */
    RACE_DATA *		race;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    sh_int		vnum;
    sh_int		count;
    sh_int		killed;
    sh_int		sex;
    sh_int		alignment;
    sh_int		hitroll;		
    sh_int		ac;			
    sh_int		hitnodice;		
    sh_int		hitsizedice;	
    sh_int		hitplus;
    sh_int		damnodice;
    sh_int		damsizedice;
    sh_int		damplus;
    int			gold;
    int			exp;			/* For new leveless system	*/
    int                 progtypes;              /* Used by MOBprogram		*/
    long		act;
    long		affected_by;
};



/*
 * One character (PC or NPC).
 */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		prev;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    SPEC_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    MPROG_ACT_LIST *    mpact;                  /* Used by MOBprogram */
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    CLASS_DATA *	class;
    RACE_DATA *		race;
    PART_DATA *		parts;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *              prompt;
    time_t		logon;
    time_t		save_time;
    time_t              last_note[ MAX_SPOOL ];
    bool                wizbit;
    bool                herobit;
    sh_int		sex;
    sh_int		timer;
    sh_int		wait;
    sh_int		hit;
    sh_int		max_hit;
    sh_int		mana;
    sh_int		max_mana;
    sh_int		move;
    sh_int		max_move;
    sh_int		position;
    sh_int		practice;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		saving_throw;
    sh_int		alignment;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		damage[ 3 ];
    sh_int		armour;
    sh_int		wimpy;
    sh_int		deaf;
    int			played;
    int			gold;
    int			exp;		/* Total for pc's, worth for mobs.	*/
    int                 mpactnum;	/* Used by MOBprogram			*/
    long		chan;		/* For toggling of channel flags	*/
    long		pen;		/* For toggling of Penalty flags	*/
    long		act;
    long		affected_by;
};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    COLOUR_DATA *	code;		/* Data for coloUr configuration	*/
    ALIAS *		aliases;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		title;
    sh_int 		security;	/* OLC - Builder security		*/
    sh_int              pagelen;
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
    sh_int		condition	[3];
    sh_int		learned		[MAX_SKILL];

    long		immortal;	/* Flags for commands relating to	*/
					/* either immortals or admin stuff.	*/
};

/*
 * MOBprogram block
*/

struct  mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    char *           buf;
    void *           vo;
};

struct  mob_prog_data
{
    MPROG_DATA *next;
    int         type;
    char *      arglist;
    char *      comlist;
};

bool    MOBtrigger;

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           1
#define SPEECH_PROG        2
#define RAND_PROG          4
#define FIGHT_PROG         8
#define DEATH_PROG        16
#define HITPRCNT_PROG     32
#define ENTRY_PROG        64
#define GREET_PROG       128
#define ALL_GREET_PROG   256
#define GIVE_PROG        512
#define BRIBE_PROG      1024


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
    OBJ_INDEX_DATA *	prev;

    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    AREA_DATA *		area;		/* OLC */

    WEAPON_DATA *	weapon;
    ARMOUR_DATA *	armour;
    SCROLL_DATA *	scroll;
    POTION_DATA *	potion;
    PILL_DATA *		pill;
    STAFF_DATA *	staff;
    WAND_DATA *		wand;
    LIGHT_DATA *	light;
    FOOD_DATA *		food;
    DRINK_DATA *	drink;
    CONT_DATA *		cont;
    MONEY_DATA *	money;

    char *		name;
    char *		short_descr;
    char *		description;

    sh_int		vnum;
    sh_int		count;
    sh_int		condition;
    sh_int		weight;
    sh_int		requires[ 5 ];

    long		item_type;
    long		extra_flags;
    long		wear_flags;

    int			cost;
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA		*	next;
    OBJ_DATA		*	prev;

    OBJ_DATA		*	next_content;
    OBJ_DATA		*	prev_content;
    OBJ_DATA		*	contains;
    OBJ_DATA		*	in_obj;
    CHAR_DATA		*	carried_by;
    EXTRA_DESCR_DATA	*	extra_descr;
    AFFECT_DATA		*	affected;
    OBJ_INDEX_DATA	*	pIndexData;
    ROOM_INDEX_DATA	*	in_room;

    WEAPON_DATA	*	weapon;
    ARMOUR_DATA	*	armour;
    SCROLL_DATA	*	scroll;
    POTION_DATA	*	potion;
    PILL_DATA	*	pill;
    STAFF_DATA	*	staff;
    WAND_DATA	*	wand;
    LIGHT_DATA	*	light;
    FOOD_DATA	*	food;
    DRINK_DATA	*	drink;
    CONT_DATA	*	cont;
    MONEY_DATA	*	money;

    char	*	name;
    char	*	short_descr;
    char	*	description;

    sh_int		condition;
    sh_int		weight;
    sh_int		timer;
    sh_int		requires[ 5 ];

    long		item_type;
    long		extra_flags;
    long		wear_flags;

    int			wear_loc;
    int			cost;
};



/*
 * Exit data.
 */
struct	exit_data
{
    EXIT_DATA *		next;		/* OLC */
    ROOM_INDEX_DATA *	to_room;
    sh_int		vnum;
    sh_int		exit_info;
    sh_int		key;
    long		rs_flags;	/* OLC */
    long		orig_door;	/* OLC */
    char *		keyword;
    char *		description;
};



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
    char *		name;
    int                 recall;
    int 		age;
    int 		nplayer;
    char *		filename;	/* OLC */
    char *		builders;	/* OLC - Listing of builders */
    int			security;	/* OLC - Value 0-infinity  */
    int			lvnum;		/* OLC - Lower vnum */
    int			uvnum;		/* OLC - Upper vnum */
    int			vnum;		/* OLC - Area vnum  */
    long		area_flags;	/* OLC */
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    RESET_DATA *	reset_first;	/* OLC */
    RESET_DATA *	reset_last;	/* OLC */
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[10];
    char *		name;
    char *		description;
    sh_int		vnum;
    long		room_flags;
    sh_int		light;
    sh_int		sector_type;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    sh_int	skill_level[MAX_CLASS];	/* Level needed by class	*/
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
extern	sh_int	gsn_dodge;
extern	sh_int	gsn_hide;
extern	sh_int	gsn_peek;
extern	sh_int	gsn_pick_lock;
extern	sh_int	gsn_sneak;
extern	sh_int	gsn_steal;

extern	sh_int	gsn_disarm;
extern	sh_int	gsn_enhanced_damage;
extern	sh_int	gsn_kick;
extern	sh_int	gsn_parry;
extern	sh_int	gsn_rescue;
extern	sh_int	gsn_second_attack;
extern	sh_int	gsn_third_attack;

extern	sh_int	gsn_blindness;
extern	sh_int	gsn_charm_person;
extern	sh_int	gsn_curse;
extern	sh_int	gsn_invis;
extern	sh_int	gsn_mass_invis;
extern	sh_int	gsn_poison;
extern	sh_int	gsn_sleep;



/*
 * Utility Macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))

/*
 * Linked List Macros.
 */
#define LINK( obj, list )	if( !( list ) )					\
				{						\
				    ( list )		= ( obj );		\
				    ( list )->next	= NULL;			\
				    ( list )->prev	= NULL;			\
				}						\
				else						\
				{						\
				    ( obj )->next	= ( list );		\
				    ( obj )->prev	= NULL;			\
				    ( list )->prev	= ( obj );		\
				    ( list )		= ( obj );		\
				}
#define UNLINK( obj, list )	if( ( obj ) == ( list ) )			\
				{						\
				    ( list )		= ( obj )->next;	\
				    if( ( obj )->next )				\
					( obj )->next->prev = NULL;		\
				}						\
				else if( !( obj )->next )			\
				{						\
				    ( obj )->prev->next	= NULL;			\
				}						\
				else						\
				{						\
				    ( obj )->next->prev	= ( obj )->prev;	\
				    ( obj )->prev->next	= ( obj )->next;	\
				}

/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(ch->wizbit)
#define IS_HERO(ch)		(ch->wizbit || ch->herobit)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armour				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))

#define MANA_COST( ch, sn )       ( IS_NPC( ch ) ? 0 : skill_table[sn].min_mana )

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	( IS_SET( ( obj )->wear_flags,  ( part ) ) )
#define IS_OBJ_STAT(obj, stat)	( IS_SET( ( obj )->extra_flags, ( stat ) ) )



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, ( ch ) ) ?		\
				( IS_NPC( ch ) ? ( ch )->short_descr	\
				: ( ch )->name ) : "someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    long		flag;
    sh_int		log;
    bool		show;
};

/*
 * Immortal Command Flags.
 * -Ant
 */
#define ICOM_NONE	(0)
#define ICOM_BANS	(A)
#define ICOM_OLC	(B)
#define ICOM_ASAVE	(C)
#define ICOM_SET	(D)
#define ICOM_STAT	(E)
#define ICOM_LOAD	(F)
#define ICOM_PENALTY	(G)
#define ICOM_SLAY	(H)
#define ICOM_SNOOP	(I)
#define ICOM_GOTO	(J)
#define ICOM_AT		(K)
#define ICOM_ECHO	(L)
#define ICOM_SWITCH	(M)
#define ICOM_SOCKET	(N)
#define ICOM_BASIC	(O)
#define ICOM_TRANS	(P)
#define ICOM_FORCE	(Q)
#define ICOM_ADMIN	(R)

/*
 * Other command related flags.
 */
#define ICOM_MOB	(Y)
#define ICOM_PKILL	(Z)



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    SOCIAL_DATA	*next;
    SOCIAL_DATA *prev;

    char *	name;
    char *	char_no_arg;
    char *	others_no_arg;
    char *	char_found;
    char *	others_found;
    char *	vict_found;
    char *	char_auto;
    char *	others_auto;

    int         social_num;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[101];
extern	const	struct	dex_app_type	dex_app		[101];

extern	const	struct	cmd_type	cmd_table	[];
extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];
extern	char *	const			title_table	[MAX_CLASS]
							[MAX_LEVEL+1]
							[2];



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first[ 126 ];
extern		HELP_DATA	  *	help_last[ 126 ];

extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		CLASS_DATA	  *	class_list;
extern		RACE_DATA	  *	race_list;

extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;

extern		CLASS_DATA	  *	class_free;
extern		RACE_DATA	  *	race_free;
extern		COLOUR_DATA	  *	coldata_free;
extern          SOCIAL_DATA       *     social_first[ 126 ];
extern          SOCIAL_DATA       *     social_last[ 126 ];

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;

extern		int			Hash_commands	[ 126 ];

/*
 * New object style structures
 */
extern	WEAPON_DATA	*weapon_free;
extern	ARMOUR_DATA	*armour_free;
extern	SCROLL_DATA	*scroll_free;
extern	POTION_DATA	*potion_free;
extern	PILL_DATA	*pill_free;
extern	STAFF_DATA	*staff_free;
extern	WAND_DATA	*wand_free;
extern	LIGHT_DATA	*light_free;
extern	FOOD_DATA	*food_free;
extern	DRINK_DATA	*drink_free;
extern	CONT_DATA	*cont_free;
extern	MONEY_DATA	*money_free;

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * This structure is used in special.c to lookup spec funcs and
 * also in olc_act.c to display listings of spec funcs.
 */
struct spec_type
{
    char *	spec_name;
    SPEC_FUN *	spec_fun;
};

/*
 * This structure is used in bit.c to lookup flags and stats.
 */
struct flag_type
{
    char * name;
    int  bit;
    bool settable;
};

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY	1

/*
 * Area flags.
 */
#define         AREA_NONE	0
#define         AREA_CHANGED	(A)	/* Area has been modified. */
#define         AREA_ADDED	(B)	/* Area has been added to. */
#define         AREA_LOADING	(C)	/* Used for counting in db.c */
#define         AREA_VERBOSE	(D)	/* Used for saving format */

#define MAX_DIR	9
#define NO_FLAG -99	/* Must not be used in flags or stats. */

/*
 * Interp.c
 */
DECLARE_DO_FUN( do_aedit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_redit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_oedit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_medit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN( do_resets	);

/*
 * Global Constants
 */
extern	char *	const	dir_name        [];
extern	const	sh_int	rev_dir         [];
extern	const	struct	spec_type	spec_table	[];

/*
 * Global variables
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
extern          int                     top_social;
extern          int                     top_part;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

/*
 * New Object Code Memory Stuff.
 */
extern	int		top_weapon;
extern	int		top_armour;
extern	int		top_cont;
extern	int		top_drink;
extern	int		top_food;
extern	int		top_potion;
extern	int		top_pill;
extern	int		top_scroll;
extern	int		top_staff;
extern	int		top_wand;
extern	int		top_light;
extern	int		top_money;

extern          char                    str_empty       [1];

extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_alias        );
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_colour       );
DECLARE_DO_FUN( do_color        );
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_endow	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_mail		);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_northeast	);
DECLARE_DO_FUN(	do_northwest	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN(	do_owhere	);
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_qwest	);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN(	do_southeast	);
DECLARE_DO_FUN(	do_southwest	);
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_unread	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);



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
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
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
#define NULL_FILE	"/dev/null"	/* To reserve one stream		*/
#define PLAYER_DIR	"../player/"	/* Player files				*/
#define MOB_DIR         "MOBProgs/"     /* MOBProg files                	*/
#define CURRENT_DIR	"current/"	/* Current areafiles in use		*/
#define AREA_TMP_DIR	"tmp/"		/* buffer directory to prevent trashing
					   of areas by OLC crashes :>		*/
#define HELP_DIR	"helps/"	/* Current helpfiles in use		*/
#define AREA_LIST	"area.lst"	/* List of areas		*/
#define CLASS_FILE	"classes"       /* File for the classes		*/
#define RACE_FILE	"races"         /* File for the races		*/
#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/
#define AUTH_PROGRAM	"getnameauth"	/* For Identification Protocol	*/



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
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_process	args( ( CHAR_DATA *ch, char *argument, int spool ) );
void    note_delete     args( ( NOTE_DATA *pnote ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );

/* act_obj.c */

/* act_wiz.c */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );

/* bit.c */
extern const struct flag_type 	area_flags[];
extern const struct flag_type	sex_flags[];
extern const struct flag_type	exit_flags[];
extern const struct flag_type	door_resets[];
extern const struct flag_type	room_flags[];
extern const struct flag_type	sector_flags[];
extern const struct flag_type	type_flags[];
extern const struct flag_type	extra_flags[];
extern const struct flag_type	wear_flags[];
extern const struct flag_type	act_flags[];
extern const struct flag_type	affect_flags[];
extern const struct flag_type	apply_flags[];
extern const struct flag_type	wear_loc_strings[];
extern const struct flag_type	wear_loc_flags[];
extern const struct flag_type	weapon_flags[];
extern const struct flag_type	weapon_extra[];
extern const struct flag_type	container_flags[];
extern const struct flag_type	liquid_flags[];
extern const struct flag_type	immortal_flags[];
extern const struct flag_type	part_flags[];
char	*ascii_flag	args( ( long vector ) );

/* comm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
			    int length ) );
void    send_to_all_char args( ( const char *text ) );
void	send_to_char_bw	 args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	page_to_char_bw	 args( ( const char *txt, CHAR_DATA *ch ) );
void	page_to_char	 args( ( const char *txt, CHAR_DATA *ch ) );
void    show_string     args( ( DESCRIPTOR_DATA *d, char *input ) );
void	act	        args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );
void	act_new	        args( ( const char *format, CHAR_DATA *ch, const void *arg1,
			    const void *arg2, int type, int min_pos ) );
char	*colour		args( ( char type, CHAR_DATA *ch ) );
char	*colourconv	args( ( const char *txt, CHAR_DATA *ch ) );

/* db.c */
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
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
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );
void	str_cat		args( ( char **dest, const char *src ) );
void	hash_commands	args( ( void ) );
void	reset_area      args( ( AREA_DATA * pArea ) );
void	reset_room	args( ( ROOM_INDEX_DATA *pRoom ) );
void   	mprog_write_programs     args( ( FILE* fp,
					MOB_INDEX_DATA *pMobIndex ) );
int	fread_flag	args( ( FILE *fp ) );
int	flag_convert	args( ( char letter ) );
void	make_parts	args( ( CHAR_DATA *ch ) );
void    delete_aliases  args( ( ALIAS *head ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( char *str, char *namelist ) );
bool	is_name_old	args( ( char *str, char *namelist ) );
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
OD *	get_eq_char	args( ( CHAR_DATA *ch, long WearFlag ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, long WearFlag ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int amount ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
void	default_colour	args( ( CHAR_DATA *ch ) );
void	all_colour	args( ( CHAR_DATA *ch, char *argument ) );
CLASS_DATA	*get_class	args( ( int number ) );
RACE_DATA	*get_race	args( ( int number ) );
RACE_DATA	*race_lookup	args( ( char *race ) );
RACE_DATA	*race_lookup_olc	args( ( char *race ) );
char *	strip_last_line	args( ( char *buf ) );

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument, bool recursing ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
int	slot_lookup	args( ( int slot ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );
/* mem.c */
PART_DATA	*new_part	args( ( void ) );
void		free_part	args( ( PART_DATA *pPart ) );

/* mob_prog.c */
#ifdef DUNNO_STRSTR
char *  strstr                  args( (const char *s1, const char *s2 ) );
#endif

void    mprog_wordlist_check    args( ( char * arg, CHAR_DATA *mob,
                CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_percent_check     args( ( CHAR_DATA *mob,
                CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void    mprog_act_trigger       args( ( char* buf, CHAR_DATA* mob,
                        CHAR_DATA* ch, OBJ_DATA* obj, void* vo ) );
void    mprog_bribe_trigger     args( ( CHAR_DATA* mob, CHAR_DATA* ch,
                        int amount ) );
void    mprog_entry_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_give_trigger      args( ( CHAR_DATA* mob, CHAR_DATA* ch,
                        OBJ_DATA* obj ) );
void    mprog_greet_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args( ( CHAR_DATA* mob ) );
void    mprog_random_trigger    args( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args( ( char* txt, CHAR_DATA* mob ) );
void	mprog_driver		args( ( char* com_list, CHAR_DATA* mob,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       void* vo ) );
/* obj.c */
WEAPON_DATA	*new_weapon	args( ( void ) );
ARMOUR_DATA	*new_armour	args( ( void ) );
CONT_DATA	*new_cont	args( ( void ) );
DRINK_DATA	*new_drink	args( ( void ) );
FOOD_DATA	*new_food	args( ( void ) );
SCROLL_DATA	*new_scroll	args( ( void ) );
PILL_DATA	*new_pill	args( ( void ) );
POTION_DATA	*new_potion	args( ( void ) );
WAND_DATA	*new_wand	args( ( void ) );
STAFF_DATA	*new_staff	args( ( void ) );
LIGHT_DATA	*new_light	args( ( void ) );
MONEY_DATA	*new_money	args( ( void ) );

void	free_weapon	args( ( OBJ_DATA *obj ) );
void	free_armour	args( ( OBJ_DATA *obj ) );
void	free_cont	args( ( OBJ_DATA *obj ) );
void	free_drink	args( ( OBJ_DATA *obj ) );
void	free_food	args( ( OBJ_DATA *obj ) );
void	free_scroll	args( ( OBJ_DATA *obj ) );
void	free_pill	args( ( OBJ_DATA *obj ) );
void	free_potion	args( ( OBJ_DATA *obj ) );
void	free_wand	args( ( OBJ_DATA *obj ) );
void	free_staff	args( ( OBJ_DATA *obj ) );
void	free_light	args( ( OBJ_DATA *obj ) );
void	free_money	args( ( OBJ_DATA *obj ) );

void	free_weapon_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_armour_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_cont_index		args( ( OBJ_INDEX_DATA *obj ) );
void	free_drink_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_food_index		args( ( OBJ_INDEX_DATA *obj ) );
void	free_scroll_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_pill_index		args( ( OBJ_INDEX_DATA *obj ) );
void	free_potion_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_wand_index		args( ( OBJ_INDEX_DATA *obj ) );
void	free_staff_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_light_index	args( ( OBJ_INDEX_DATA *obj ) );
void	free_money_index	args( ( OBJ_INDEX_DATA *obj ) );

/* olc.c */
bool	run_olc_editor	args( ( DESCRIPTOR_DATA *d ) );
char	*olc_ed_name	args( ( CHAR_DATA *ch ) );
char	*olc_ed_vnum	args( ( CHAR_DATA *ch ) );
int flag_value			args ( ( const struct flag_type *flag_table,
				         char *argument) );
char *flag_string		args ( ( const struct flag_type *flag_table,
				         int bits ) );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );
char *	spec_string	args( ( SPEC_FUN *fun ) );	/* OLC */

/* string.c */
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *	string_replace	args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *	string_unpad	args( ( char * argument ) );
char *	string_proper	args( ( char * argument ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
