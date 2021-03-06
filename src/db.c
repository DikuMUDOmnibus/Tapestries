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
#include <memory.h>
#include "merc.h"

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif

/*
 * Globals.
 */
HELP_DATA *		help_first[ 126 ];
HELP_DATA *		help_last[ 126 ];

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

CHAR_DATA *		char_free;
EXTRA_DESCR_DATA *	extra_descr_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
PC_DATA *		pcdata_free;

COLOUR_DATA *		coldata_free;
CLASS_DATA *		class_free;
RACE_DATA *		race_free;

CLASS_DATA *		class_list;
RACE_DATA *		race_list;

SOCIAL_DATA *           social_first[ 126 ];
SOCIAL_DATA *           social_last[ 126 ];

char			bug_buf[ 2 * MAX_INPUT_LENGTH ];
CHAR_DATA *		char_list;
char *			help_greeting;
char			log_buf[ 2 * MAX_INPUT_LENGTH ];
KILL_DATA		kill_table[ MAX_LEVEL ];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;
sh_int			social_count;

sh_int			gsn_backstab;
sh_int			gsn_dodge;
sh_int			gsn_hide;
sh_int			gsn_peek;
sh_int			gsn_pick_lock;
sh_int			gsn_sneak;
sh_int			gsn_steal;

sh_int			gsn_disarm;
sh_int			gsn_enhanced_damage;
sh_int			gsn_kick;
sh_int			gsn_parry;
sh_int			gsn_rescue;
sh_int			gsn_second_attack;
sh_int			gsn_third_attack;

sh_int			gsn_blindness;
sh_int			gsn_charm_person;
sh_int			gsn_curse;
sh_int			gsn_invis;
sh_int			gsn_mass_invis;
sh_int			gsn_poison;
sh_int			gsn_sleep;





/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
char *			string_hash		[MAX_KEY_HASH];
int			Hash_commands		[ 126 ];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

char *			string_space;
char *			top_string;
char			str_empty	[1];

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;
int			top_social;
int			top_vnum_room;	/* OLC */
int			top_vnum_mob;	/* OLC */
int			top_vnum_obj;   /* OLC */

/*
 * New Object Code Memory Stuff.
 */
int			top_weapon;
int			top_armour;
int			top_cont;
int			top_drink;
int			top_food;
int			top_potion;
int			top_pill;
int			top_scroll;
int			top_staff;
int			top_wand;
int			top_light;
int			top_money;

/*
 * MOBprogram locals
*/

int 		mprog_name_to_type	args( ( char* name ) );
MPROG_DATA *	mprog_file_read	args( ( char* f, MPROG_DATA* mprg, 
                                        MOB_INDEX_DATA *pMobIndex ) );
void		load_mobprogs           args( ( FILE* fp ) );
void   		mprog_read_programs     args( ( FILE* fp,
					MOB_INDEX_DATA *pMobIndex ) );


/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING	1000000
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	11

void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];
char			strAreabuf[ MAX_INPUT_LENGTH   ];



/*
 * Local booting procedures.
 */
void	init_mm		args( ( void ) );

void	load_area	args( ( FILE *fp ) );
void	load_helps	args( ( FILE *fp ) );
void	load_mobiles	args( ( FILE *fp ) );
void	load_objects	args( ( FILE *fp ) );
void	load_resets	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	args( ( void ) );

void	fix_exits	args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea ) );

/*
 * Non-Merc Loading procedures.
 * Put any new loading function in this section.
 */
void	new_load_area	args( ( FILE *fp ) );	/* OLC */
void	new_load_rooms	args( ( FILE *fp ) );	/* OLC 1.1b */
void    load_socials    args( ( FILE *fp ) );	/* -Ant */


/*
 * Big mama top level function.
 */
void boot_db( void )
{
    /*
     * Init some data space stuff.
     */
    {
	if( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
	{
	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
	    exit( 1 );
	}
	top_string	= string_space;
	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
	init_mm( );
    }

    /*
     * Set time and weather.
     */
    {
	long lhour;
	long lday;
	long lmonth;

	lhour		= ( current_time - 650336715 )
			/ ( PULSE_TICK / PULSE_PER_SECOND );
	time_info.hour	= lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 31;
	lmonth		= lday   / 31;
	time_info.month	= lmonth % 12;
	time_info.year	= lmonth / 12;

	     if( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                           weather_info.sunlight = SUN_DARK;

	time_info.minute	= 0;
	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if( time_info.month >= 3 && time_info.month <= 9 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                 weather_info.sky = SKY_CLOUDLESS;

    }

    /*
     * Assign gsn's for skills which have them.
     */
    {
	int sn;

	for( sn = 0 ; sn < MAX_SKILL ; sn++ )
	{
	    if( skill_table[sn].pgsn )
		*skill_table[sn].pgsn = sn;
	}
    }

    /*
     * Read in class and race files.
     * -Ant
     */
    {
	CLASS_DATA	*pClass;
	RACE_DATA	*pRace;
	FILE		*fpNew;
	int		i;

	if( !( fpNew = fopen( CLASS_FILE, "r" ) ) )
	{
	    perror( CLASS_FILE );
	    exit( 1 );
	}

	for( ; ; )
	{
	    sh_int	vnum;
	    char	letter;

	    letter	= fread_letter( fpNew );

	    if( letter == '*' )
	    {
		fread_to_eol( fpNew );
		continue;
	    }
	    else if( letter == '$' )
	    {
		fread_to_eol( fpNew );
		continue;
	    }
	    else if( letter != '#' )
	    {
		bug( "BootDB (Load_Classes): # not found!!!", 0 );
		exit( 1 );
	    }

	    vnum	= fread_number( fpNew );
	    if( vnum == 0 )
	    {
		fclose( fpNew );
		break;
	    }

	    if( get_class( vnum ) )
	    {
		bug( "Load_Classes: number %d duplicated.", vnum );
		exit( 1 );
	    }

	    pClass		= alloc_perm( sizeof( *pClass ) );
	    pClass->number	= vnum;
	    pClass->name	= fread_string( fpNew );
	    strcpy( pClass->who_name, fread_word( fpNew ) );
	    pClass->type	= fread_number( fpNew );
	    pClass->attr_prime	= flag_value( apply_flags, fread_word( fpNew ) );
	    pClass->weapon	= fread_number( fpNew );
	    pClass->guild	= fread_number( fpNew );
	    pClass->skill_adept	= fread_number( fpNew );
	    pClass->defence	= fread_number( fpNew );
	    pClass->attack	= fread_number( fpNew );
 	    pClass->hp_min	= fread_number( fpNew );
	    pClass->hp_max	= fread_number( fpNew );
	    pClass->mana_min	= fread_number( fpNew );
	    pClass->mana_max	= fread_number( fpNew );
	    pClass->sub_classes	= fread_flag( fpNew );

	    LINK( pClass, class_list );
	}

	if( !( fpNew = fopen( RACE_FILE, "r" ) ) )
	{
	    perror( RACE_FILE );
	    exit( 1 );
	}

	for( ; ; )
	{
	    sh_int	vnum;
	    char	letter;

	    letter	= fread_letter( fpNew );

	    if( letter == '*' )
	    {
		fread_to_eol( fpNew );
		continue;
	    }
	    else if( letter != '#' )
	    {
		bug( "Load_Races: # not found!!!", 0 );
		exit( 1 );
	    }

	    vnum	= fread_number( fpNew );
	    if( vnum == 0 )
	    {
		fclose( fpNew );
		break;
	    }

	    if( get_race( vnum ) )
	    {
		bug( "Load_Races: number %d duplicated.", vnum );
		exit( 1 );
	    }


	    pRace		= alloc_perm( sizeof( *pRace ) );
	    pRace->number	= vnum;
	    pRace->name		= fread_string( fpNew );
	    strcpy( pRace->who_name, fread_word( fpNew ) );
	    pRace->playable	= fread_number( fpNew );
	    pRace->classes	= fread_flag( fpNew );
	    pRace->sub_classes	= fread_flag( fpNew );

	    for( i = 0 ; i < 5 ; i++ )
		pRace->base_stats[ i ] = fread_number( fpNew );

	    for( i = 0 ; i < 5 ; i++ )
		pRace->max_stats[ i ] = fread_number( fpNew );

	    for( i = 0 ; i < 5 ; i++ )
		pRace->train_stats[ i ] = fread_number( fpNew );

	    pRace->parts	= fread_flag( fpNew );
	    pRace->affects	= fread_flag( fpNew );

	    LINK( pRace, race_list );
	}
    }

    /*
     * Read in all the area files.
     */
    {
	FILE	*fpList;

	if( !( fpList = fopen( AREA_LIST, "r" ) ) )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for( ; ; )
	{
	    strcpy( strAreabuf, fread_word( fpList ) );
	    if( strAreabuf[0] == '$' )
		break;

	    if( strAreabuf[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
		if( ( !str_suffix( ".are", strAreabuf ) )
		 || ( !str_suffix( ".soc", strAreabuf ) ) )
		    sprintf( strArea, "%s%s", CURRENT_DIR, strAreabuf );
		else
		    sprintf( strArea, "%s%s", HELP_DIR, strAreabuf );

		if( !( fpArea = fopen( strArea, "r" ) ) )
		{
		    perror( strArea );
		    exit( 1 );
		}
	    }

	    for( ; ; )
	    {
		char *word;

		if( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    exit( 1 );
		}

		word = fread_word( fpArea );

		if( word[0] == '$'			)
		    break;
		else if( !str_cmp( word, "AREA"		) )
		    load_area( fpArea );
		else if( !str_cmp( word, "HELPS"	) )
		    load_helps( fpArea );
		else if( !str_cmp( word, "MOBILES"	) )
		    load_mobiles( fpArea );
		else if( !str_cmp( word, "MOBPROGS"	) )
		    load_mobprogs( fpArea );
		else if( !str_cmp( word, "OBJECTS"	) )
		    load_objects( fpArea );
		else if( !str_cmp( word, "RESETS"	) )
		    load_resets( fpArea );
		else if( !str_cmp( word, "ROOMS"	) )
		    load_rooms( fpArea );
		else if( !str_cmp( word, "SHOPS"	) )
		    load_shops( fpArea );
		else if( !str_cmp( word, "SPECIALS"	) )
		    load_specials( fpArea );
		else if( !str_cmp( word,  "SOCIALS"	) )	/* OLC */
		{
		    load_socials ( fpArea );
                    break;
		}
		else if( !str_cmp( word, "AREADATA"	) )	/* OLC */
		    new_load_area( fpArea );
		else if( !str_cmp( word, "ROOMDATA"	) )	/* OLC 1.1b */
		    new_load_rooms( fpArea );
		else
		{
		    bug( "Boot_db: bad section name.", 0 );
		    exit( 1 );
		}
	    }

	    if( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
	}
	fclose( fpList );
    }

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     * Set the MOBtrigger.
     */
    {
	fix_exits( );
	fBootDb	= FALSE;
	area_update( );
	load_notes( );
	MOBtrigger = TRUE;
    }

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= alloc_perm( sizeof( *pArea ) );
    pArea->name		= fread_string( fp );
    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;		/* OLC */
    pArea->security     = 1;			/* OLC */
    pArea->builders     = str_dup( "None" );	/* OLC */
    pArea->lvnum        = 0;			/* OLC */
    pArea->uvnum        = 0;			/* OLC */
    pArea->vnum		= top_area;		/* OLC */
    pArea->filename	= str_dup( strAreabuf );
    pArea->age		= 15;
    pArea->nplayer	= 0;

    if( !area_first )
	area_first = pArea;
    if(  area_last  )
    {
	area_last->next = pArea;
	REMOVE_BIT( area_last->area_flags, AREA_LOADING );	/* OLC */
    }
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}

/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined( KEY )
#undef KEY
#endif

#define KEY( literal, field, value )			\
		if( !str_cmp( word, literal ) )		\
                {					\
                    field  = value;			\
                    fMatch = TRUE;			\
                    break;				\
		}

#define SKEY( string, field )				\
                if( !str_cmp( word, string ) )		\
                {					\
                    free_string( field );		\
                    field = fread_string( fp );		\
                    fMatch = TRUE;			\
                    break;				\
		}



/* OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char      *word;
    bool      fMatch;

    pArea               = alloc_perm( sizeof( *pArea ) );
    pArea->age          = 15;
    pArea->nplayer      = 0;
    pArea->filename     = str_dup( strAreabuf );
    pArea->vnum         = top_area;
    pArea->name         = str_dup( "New Area" );
    pArea->builders     = str_dup( "" );
    pArea->security     = 1;
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->area_flags   = 0;
    pArea->recall       = ROOM_VNUM_TEMPLE;

    for( ; ; )
    {
       word   = feof( fp ) ? "End" : fread_word( fp );
       fMatch = FALSE;

       switch( UPPER( word[0] ) )
       {
           case 'N':
            SKEY( "Name", pArea->name );
            break;
           case 'S':
             KEY( "Security", pArea->security, fread_number( fp ) );
            break;
           case 'V':
            if( !str_cmp( word, "VNUMs" ) )
            {
                pArea->lvnum = fread_number( fp );
                pArea->uvnum = fread_number( fp );
            }
            break;
           case 'E':
             if( !str_cmp( word, "End" ) )
             {
                 fMatch = TRUE;
                 if( !area_first )
                    area_first = pArea;
                 if( area_last )
                    area_last->next = pArea;
                 area_last   = pArea;
                 pArea->next = NULL;
                 top_area++;
                 return;
            }
            break;
           case 'B':
            SKEY( "Builders", pArea->builders );
            break;
           case 'R':
             KEY( "Recall", pArea->recall, fread_number( fp ) );
            break;
	}
    }
}



/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum( int vnum )
{
    if( area_last->lvnum == 0 || area_last->uvnum == 0 )
	area_last->lvnum = area_last->uvnum = vnum;
    if( vnum != URANGE( area_last->lvnum, vnum, area_last->uvnum ) )
	if( vnum < area_last->lvnum )
	    area_last->lvnum = vnum;
	else
	    area_last->uvnum = vnum;
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;
    int		cnt;
    int		iHash = 0;

    for( cnt = 0 ; cnt < 126 ; cnt++ )
    {
	help_first[ cnt ]	= NULL;
	help_last[ cnt ]	= NULL;
    }

    for( ; ; )
    {
	pHelp		= alloc_perm( sizeof( *pHelp ) );
	pHelp->level	= fread_number( fp );
	pHelp->keyword	= fread_string( fp );
	if( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );

	if( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;

	iHash	= pHelp->keyword[ 0 ] % 126;

	if( !help_first[ iHash ] )
	    help_first[ iHash ] = pHelp;
	if( help_last[ iHash ] )
	    help_last[ iHash ]->next = pHelp;

	pHelp->prev		= help_last[ iHash ];
	help_last[ iHash ]	= pHelp;
	pHelp->next		= NULL;
	top_help++;
    }

    return;
}

/*
 * Snarf a mob section.
 * Updated, Ant
 */
void load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA	*pMobIndex;

    if( !area_last )	/* OLC */
    {
	bug( "Load_mobiles: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for( ; ; )
    {
	sh_int	vnum;
	char	letter;
	int	iHash;

	letter				= fread_letter( fp );
	if( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if( get_mob_index( vnum ) )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= alloc_perm( sizeof( *pMobIndex ) );
	pMobIndex->vnum			= vnum;
	pMobIndex->area			= area_last;		/* OLC */
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );

	pMobIndex->long_descr[0]	= UPPER( pMobIndex->long_descr[0] );
	pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

	pMobIndex->race			= race_lookup( fread_word( fp ) );
	pMobIndex->act			= fread_flag( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_flag( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );

	pMobIndex->hitroll		= fread_number( fp );
	pMobIndex->ac			= fread_number( fp );
	pMobIndex->hitnodice		= fread_number( fp );
	/* 'd'		*/		  fread_letter( fp );
	pMobIndex->hitsizedice		= fread_number( fp );
	/* '+'		*/		  fread_letter( fp );
	pMobIndex->hitplus		= fread_number( fp );
	pMobIndex->damnodice		= fread_number( fp );
	/* 'd'		*/		  fread_letter( fp );
	pMobIndex->damsizedice		= fread_number( fp );
	/* '+'		*/		  fread_letter( fp );
	pMobIndex->damplus		= fread_number( fp );
	pMobIndex->gold			= fread_number( fp );
	pMobIndex->exp			= fread_number( fp );
	/* position	*/		  fread_number( fp );
	/* start pos	*/		  fread_number( fp );

	/*
	 * Back to meaningful values.
	 */
	pMobIndex->sex			= fread_number( fp );

	if( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}

	letter = fread_letter( fp );

	if( letter == '>' )
	{
	    ungetc( letter, fp );
	    mprog_read_programs( fp, pMobIndex );
	}
	else
	    ungetc( letter, fp );
	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
	top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;  /* OLC */
	assign_area_vnum( vnum );				   /* OLC */
	kill_table[URANGE( 0, 50, MAX_LEVEL-1 )].number++; /* TODO */
    }

    return;
}



/*
 * Snarf an obj section.
 * Updated, Ant
 */
void load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    if( !area_last )	/* OLC */
    {
	bug( "Load_objects: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for( ; ; )
    {
	sh_int	vnum;
	char	letter;
	int	iHash;

	letter				= fread_letter( fp );
	if( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if( get_obj_index( vnum ) )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof( *pObjIndex ) );
	pObjIndex->vnum			= vnum;
        pObjIndex->area			= area_last;		/* OLC */
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
	/* Action description */	  fread_string( fp );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= fread_flag( fp );
	pObjIndex->extra_flags		= fread_flag( fp );
	pObjIndex->wear_flags		= fread_flag( fp );
	pObjIndex->weight		= fread_number( fp );
	pObjIndex->cost			= fread_number( fp );

	pObjIndex->requires[ 0 ]	= fread_number( fp );
	pObjIndex->requires[ 1 ]	= fread_number( fp );
	pObjIndex->requires[ 2 ]	= fread_number( fp );
	pObjIndex->requires[ 3 ]	= fread_number( fp );
	pObjIndex->requires[ 4 ]	= fread_number( fp );

	for( ; ; )
	{
	    char	letter;

	    letter = fread_letter( fp );

	    if( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof( *paf ) );
		paf->type		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }
	    else if( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else if( letter == 'O' )
	    {
		char	type;

		type = fread_letter( fp );

		if( type == 'A' )
		{
		    WEAPON_DATA	*weapon;

		    weapon		= alloc_perm( sizeof( *weapon ) );
		    weapon->type	= fread_number( fp );	    
		    weapon->message	= flag_value( weapon_flags, fread_word( fp ) );
		    weapon->flags	= fread_flag( fp );	    
		    weapon->hit		= fread_number( fp );	    
		    weapon->damage[0]	= fread_number( fp );
					  fread_letter( fp );	    
		    weapon->damage[1]	= fread_number( fp );	    
					  fread_letter( fp );	    
		    weapon->damage[2]	= fread_number( fp );

		    if( !IS_SET( pObjIndex->item_type, ITEM_WEAPON ) )
			SET_BIT( pObjIndex->item_type, ITEM_WEAPON );

		    pObjIndex->weapon = weapon;
		}
		else if( type == 'B' )
		{
		    ARMOUR_DATA	*armour;

		    armour		= alloc_perm( sizeof( *armour ) );
		    armour->ac[0]	= fread_number( fp );
		    armour->ac[1]	= fread_number( fp );
		    armour->ac[2]	= fread_number( fp );
		    armour->ac[3]	= fread_number( fp );

		    if( !IS_SET( pObjIndex->item_type, ITEM_ARMOUR ) )
			SET_BIT( pObjIndex->item_type, ITEM_ARMOUR );

		    pObjIndex->armour = armour;
		}	    
		else if( type == 'C' )
		{
		    CONT_DATA	*cont;

		    cont		= alloc_perm( sizeof( *cont ) );
		    cont->closed	= fread_flag( fp );
		    cont->key		= fread_number( fp );
		    cont->capacity	= fread_number( fp );
		    cont->single	= fread_number( fp );

		    if( !IS_SET( pObjIndex->item_type, ITEM_CONTAINER ) )
			SET_BIT( pObjIndex->item_type, ITEM_CONTAINER );

		    pObjIndex->cont = cont;
		}	    
		else if( type == 'D' )
		{
		    DRINK_DATA	*drink;

		    drink		= alloc_perm( sizeof( *drink ) );
		    drink->volume	= fread_number( fp );
		    drink->limit	= fread_number( fp );
		    drink->liquid	= fread_number( fp );
		    drink->poison	= fread_number( fp );

		    if( !IS_SET( pObjIndex->item_type, ITEM_DRINK_CON ) )
			SET_BIT( pObjIndex->item_type, ITEM_DRINK_CON );

		    pObjIndex->drink = drink;
		}	    
		else if( type == 'E' )
		{
		    FOOD_DATA	*food;

		    food		= alloc_perm( sizeof( *food ) );
		    food->bite		= fread_number( fp );
		    food->poison	= fread_number( fp );

		    if( !IS_SET( pObjIndex->item_type, ITEM_FOOD ) )
			SET_BIT( pObjIndex->item_type, ITEM_FOOD );

		    pObjIndex->food = food;
		}	    
		else if( type == 'F' )
		{
		    POTION_DATA	*potion;

		    potion		= alloc_perm( sizeof( *potion ) );
		    potion->level	= fread_number( fp );
		    potion->spell[0]	= skill_lookup( fread_word( fp ) );
		    potion->spell[1]	= skill_lookup( fread_word( fp ) );
		    potion->spell[2]	= skill_lookup( fread_word( fp ) );
		    potion->spell[3]	= skill_lookup( fread_word( fp ) );

		    if( !IS_SET( pObjIndex->item_type, ITEM_POTION ) )
			SET_BIT( pObjIndex->item_type, ITEM_POTION );

		    pObjIndex->potion = potion;
		}	    
		else if( type == 'G' )
		{
		    PILL_DATA	*pill;

		    pill		= alloc_perm( sizeof( *pill ) );
		    pill->level		= fread_number( fp );
		    pill->spell[0]	= skill_lookup( fread_word( fp ) );
		    pill->spell[1]	= skill_lookup( fread_word( fp ) );
		    pill->spell[2]	= skill_lookup( fread_word( fp ) );
		    pill->spell[3]	= skill_lookup( fread_word( fp ) );

		    if( !IS_SET( pObjIndex->item_type, ITEM_PILL ) )
			SET_BIT( pObjIndex->item_type, ITEM_PILL );

		    pObjIndex->pill = pill;
		}	    
		else if( type == 'H' )
		{
		    SCROLL_DATA	*scroll;

		    scroll		= alloc_perm( sizeof( *scroll ) );
		    scroll->level	= fread_number( fp );
		    scroll->spell[0]	= skill_lookup( fread_word( fp ) );
		    scroll->spell[1]	= skill_lookup( fread_word( fp ) );
		    scroll->spell[2]	= skill_lookup( fread_word( fp ) );
		    scroll->spell[3]	= skill_lookup( fread_word( fp ) );

		    if( !IS_SET( pObjIndex->item_type, ITEM_SCROLL ) )
			SET_BIT( pObjIndex->item_type, ITEM_SCROLL );

		    pObjIndex->scroll = scroll;
		}
		else if( type == 'I' )
		{
		    STAFF_DATA	*staff;

		    staff		= alloc_perm( sizeof( *staff ) );
		    staff->level	= fread_number( fp );
		    staff->charges	= fread_number( fp );
		    staff->max		= fread_number( fp );
		    staff->spell	= skill_lookup( fread_word( fp ) );

		    if( !IS_SET( pObjIndex->item_type, ITEM_STAFF ) )
			SET_BIT( pObjIndex->item_type, ITEM_STAFF );

		    pObjIndex->staff = staff;
		}
		else if( type == 'J' )
		{
		    WAND_DATA	*wand;

		    wand		= alloc_perm( sizeof( *wand ) );
		    wand->level		= fread_number( fp );
		    wand->charges	= fread_number( fp );
		    wand->max		= fread_number( fp );
		    wand->spell		= skill_lookup( fread_word( fp ) );

		    if( !IS_SET( pObjIndex->item_type, ITEM_WAND ) )
			SET_BIT( pObjIndex->item_type, ITEM_WAND );

		    pObjIndex->wand = wand;
		}
		else if( type == 'K' )
		{
		    LIGHT_DATA	*light;

		    light		= alloc_perm( sizeof( *light ) );
		    light->duration	= fread_number( fp );
		    light->intensity	= fread_number( fp );
	
		    if( !IS_SET( pObjIndex->item_type, ITEM_LIGHT ) )
			SET_BIT( pObjIndex->item_type, ITEM_LIGHT );

		    pObjIndex->light = light;
		}
		else if( type == 'L' )
		{
		    MONEY_DATA	*cash;

		    cash		= alloc_perm( sizeof( *cash ) );
		    cash->gold		= fread_number( fp );
		    cash->silver	= fread_number( fp );
		    cash->copper	= fread_number( fp );
	
		    if( !IS_SET( pObjIndex->item_type, ITEM_MONEY ) )
			SET_BIT( pObjIndex->item_type, ITEM_MONEY );

		    pObjIndex->money = cash;
		}
		else
		{
		    fread_to_eol( fp );
		    bug( "Bad sub-type for object %d", pObjIndex->vnum );
		}
	    }
	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
	top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;  /* OLC */
	assign_area_vnum( vnum );				   /* OLC */
    }

    return;
}

/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;

    if ( !pR )
       return;

    pr = pR->reset_last;

    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }

    top_reset++;
    return;
}


/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA	*pReset;
    int 	iLastRoom = 0;
    int 	iLastObj  = 0;

    if( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for( ; ; )
    {
	EXIT_DATA       *pexit;
	ROOM_INDEX_DATA *pRoomIndex;
	char             letter;

	if( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= alloc_perm( sizeof( *pReset ) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= ( letter == 'G' || letter == 'R' )
			    ? 0 : fread_number( fp );
			  fread_to_eol( fp );

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

	case 'M':
	    get_mob_index( pReset->arg1 );
	    if( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastRoom = pReset->arg3;
	    }
	    break;

	case 'O':
	    get_obj_index  ( pReset->arg1 );
	    if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastObj = pReset->arg3;
	    }
	    break;

	case 'P':
	    get_obj_index  ( pReset->arg1 );
	    if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
	    {
		new_reset( pRoomIndex, pReset );
	    }
	    break;

	case 'G':
	case 'E':
	    get_obj_index  ( pReset->arg1 );
	    if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
	    {
		new_reset( pRoomIndex, pReset );
		iLastObj = iLastRoom;
	    }
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if (   pReset->arg2 < 0
		|| pReset->arg2 > 5
		|| !pRoomIndex
		|| !( pexit = pRoomIndex->exit[pReset->arg2] )
		|| !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    switch ( pReset->arg3 )	/* OLC 1.1b */
	    {
		default:
		    bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
		case 0:
		    break;
		case 1: SET_BIT( pexit->rs_flags, EX_CLOSED );
		    break;
		case 2: SET_BIT( pexit->rs_flags, EX_CLOSED | EX_LOCKED );
		    break;
	    }
	    break;

	case 'R':
	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )	/* Last Door. */
	    {
		bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		exit( 1 );
	    }

	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) )
		new_reset( pRoomIndex, pReset );

	    break;
	}
    }

    return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for( ; ; )
    {
	sh_int vnum;
	char letter;
	int door;
	int iHash;

	letter				= fread_letter( fp );
	if( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if( get_room_index( vnum ) )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= alloc_perm( sizeof(*pRoomIndex) );
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );
	pRoomIndex->room_flags		= fread_number( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	for( door = 0 ; door <= MAX_DIR ; door++ )
	    pRoomIndex->exit[door] = NULL;

	for( ; ; )
	{
	    letter = fread_letter( fp );

	    if( letter == 'S' )
		break;

	    if( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int locks;

		door = fread_number( fp );
		if ( door < 0 || door > 9 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof(*pexit) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		pexit->exit_info	= 0;
		pexit->rs_flags		= 0;			/* OLC */
		locks			= fread_number( fp );
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );

		switch ( locks )
		{
		case 1: pexit->rs_flags = EX_ISDOOR;                break;
		case 2: pexit->rs_flags = EX_ISDOOR | EX_PICKPROOF; break;
		}

		pRoomIndex->exit[door]	= pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
	top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; /* OLC */
	assign_area_vnum( vnum );				     /* OLC */
    }

    return;
}

/* OLC 1.1b */
void new_load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA	*pRoomIndex;

    if( !area_last )
    {
	bug( "Load_rooms: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for( ; ; )
    {
	char	letter;
	int	vnum;
	int	door;
	int	iHash;

	letter				= fread_letter( fp );
	if( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if( get_room_index( vnum ) )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= alloc_perm( sizeof( *pRoomIndex ) );
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	pRoomIndex->room_flags		= fread_flag( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= fread_number( fp );
	for( door = 0 ; door <= MAX_DIR ; door++ )
	    pRoomIndex->exit[door] = NULL;

	for( ; ; )
	{
	    letter = fread_letter( fp );

	    if( letter == 'S' || letter == 's' )
	    {
		if( letter == 's' )
		    bug( "New_load_rooms: vnum %d has lowercase 's'", vnum );
		break;
	    }

	    if( letter == 'D' )
	    {
		EXIT_DATA	*pexit;
		int		locks;

		door = fread_number( fp );
		if( door < 0 || door > MAX_DIR )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof( *pexit ) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		locks			= fread_flag( fp );
		pexit->exit_info	= locks;
		pexit->rs_flags		= locks;
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );


		pRoomIndex->exit[door]		= pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
	top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
	assign_area_vnum( vnum );
    }

    return;
}


/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char letter;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE *fp;
    NOTE_DATA *pnotelast;

    if ( ( fp = fopen( NOTE_FILE, "r" ) ) == NULL )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char letter;

	do
	{
	    letter = getc( fp );
	    if ( feof(fp) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace(letter) );
	ungetc( letter, fp );

	pnote		  = alloc_perm( sizeof(*pnote) );

	if ( str_cmp( fread_word( fp ), "Sender" ) )
	    break;
	pnote->sender	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "Spool" ) )
	    break;
	pnote->spool	  = fread_number( fp );

	if ( str_cmp( fread_word( fp ), "Anon" ) )
	    break;
	pnote->anon	  = fread_number( fp );

	if ( str_cmp( fread_word( fp ), "Date" ) )
	    break;
	pnote->date	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "Stamp" ) )
	    break;
	pnote->date_stamp = fread_number( fp );

	if ( str_cmp( fread_word( fp ), "To" ) )
	    break;
	pnote->to_list	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "Subject" ) )
	    break;
	pnote->subject	  = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "Text" ) )
	    break;
	pnote->text	  = fread_string( fp );

	if ( note_list == NULL )
	    note_list		= pnote;
	else
	    pnotelast->next	= pnote;

	pnotelast	  = pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits( void )
{
    extern const sh_int rev_dir [];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    int iHash;
    int door;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for( door = 0 ; door <= MAX_DIR ; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) != NULL )
		{
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
	    }

	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex != NULL;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for( door = 0 ; door <= MAX_DIR ; door++ )
	    {
		if ( ( pexit     = pRoomIndex->exit[door]       ) != NULL
		&&   ( to_room   = pexit->to_room               ) != NULL
		&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
		&&   pexit_rev->to_room != pRoomIndex )
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			pRoomIndex->vnum, door,
			to_room->vnum,    rev_dir[door],
			(pexit_rev->to_room == NULL)
			    ? 0 : pexit_rev->to_room->vnum );
		    bug( buf, 0 );
		}
	    }
	}
    }

    return;
}



/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
	if ( ++pArea->age < 3 )
	    continue;

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( pArea->nplayer == 0 || pArea->age >= 15 )
	{
	    ROOM_INDEX_DATA *pRoomIndex;

	    reset_area( pArea );
	    pArea->age = number_range( 0, 3 );
	    pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
	    if ( pRoomIndex != NULL && pArea == pRoomIndex->area )
		pArea->age = 15 - 3;
	}
    }

    return;
}

/* OLC
 * Reset one room.  Called by reset_area and olc.
 */
void reset_room( ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA	*pReset;
    CHAR_DATA	*pMob;
    OBJ_DATA	*pObj;
    CHAR_DATA	*LastMob = NULL;
    OBJ_DATA	*LastObj = NULL;
    int		iExit;
    int		level = 0;
    bool	last;

    if( !pRoom )
	return;

    pMob	= NULL;
    last	= FALSE;
    
    for( iExit = 0;  iExit < MAX_DIR;  iExit++ )
    {
	EXIT_DATA *pExit;
	if( ( pExit = pRoom->exit[iExit] ) )
	{
	    pExit->exit_info = pExit->rs_flags;
	    if( ( pExit->to_room )
	     && ( ( pExit = pExit->to_room->exit[rev_dir[iExit]] ) ) )
	    {
		/* nail the other side */
		pExit->exit_info = pExit->rs_flags;
	    }
	}
    }

    for( pReset = pRoom->reset_first ; pReset ; pReset = pReset->next )
    {
	MOB_INDEX_DATA	*pMobIndex;
	OBJ_INDEX_DATA	*pObjIndex;
	OBJ_INDEX_DATA	*pObjToIndex;
	ROOM_INDEX_DATA	*pRoomIndex;

	switch( pReset->command )
	{
	default:
		bug( "Reset_room: bad command %c.", pReset->command );
		break;

	case 'M':
	    if( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( pMobIndex->count >= pReset->arg2 )
	    {
		last = FALSE;
		break;
	    }

	    pMob = create_mobile( pMobIndex );

	    /*
	     * Some hard coding.
	     */
	    if( room_is_dark( pRoom ) )
		SET_BIT( pMob->affected_by, AFF_INFRARED );

	    /*
	     * Pet shop mobiles get ACT_PET set.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoom->vnum - 1 );
		if( pRoomIndexPrev
		 && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
		    SET_BIT( pMob->act, ACT_PET);
	    }

	    char_to_room( pMob, pRoom );

	    LastMob = pMob;
	    level  = 0;
	    last = TRUE;
	    break;

	case 'O':
	    if( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_room: 'O': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
		bug( "Reset_room: 'O': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if( pRoom->area->nplayer > 0
	      || count_obj_list( pObjIndex, pRoom->contents ) > 0 )
		break;

	    pObj = create_object( pObjIndex );
	    pObj->cost = 0;
	    obj_to_room( pObj, pRoom );
	    break;

	case 'P':
	    if( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_room: 'P': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
		bug( "Reset_room: 'P': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if( pRoom->area->nplayer > 0
	      || !( LastObj = get_obj_type( pObjToIndex ) )
	      || count_obj_list( pObjIndex, LastObj->contains ) > 0 )
		break;

	    pObj = create_object( pObjIndex );
	    obj_to_obj( pObj, LastObj );

	    /*
	     * Ensure that the container gets reset.	OLC 1.1b
	     */
	    if( IS_SET( LastObj->item_type, ITEM_CONTAINER ) )
	    {
		LastObj->cont->closed = LastObj->pIndexData->cont->closed;
	    }
	    else
	    {
	    	    /* THIS SPACE INTENTIONALLY LEFT BLANK */
	    }
	    break;

	case 'G':
	case 'E':
	    if( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_room: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if( !last )
		break;

	    if( !LastMob )
	    {
		bug( "Reset_room: 'E' or 'G': null mob for vnum %d.",
		    pReset->arg1 );
		last = FALSE;
		break;
	    }

	    if( LastMob->pIndexData->pShop )	/* Shop-keeper? */
	    {
		int olevel;

		if( IS_SET( pObjIndex->item_type, ITEM_PILL )
		 || IS_SET( pObjIndex->item_type, ITEM_POTION ) )
		{
		    olevel = number_range( 0, 10 );
		}
		else if( IS_SET( pObjIndex->item_type, ITEM_SCROLL )
		 || IS_SET( pObjIndex->item_type, ITEM_ARMOUR )
		 || ( IS_SET( pObjIndex->item_type, ITEM_WEAPON )
		  && pReset->command == 'G' ) )
		{
		    olevel = number_range( 5, 15 );
		}
		else if( IS_SET( pObjIndex->item_type, ITEM_WAND ) )
		{
		    olevel = number_range( 10, 20 );
		}
		else if( IS_SET( pObjIndex->item_type, ITEM_STAFF ) )
		{
		    olevel = number_range( 15, 25 );
		}
		else if( IS_SET( pObjIndex->item_type, ITEM_WEAPON ) )
		{
		    olevel = number_fuzzy( level );
		}
		else
		{
		    olevel = 0;
		}

		pObj = create_object( pObjIndex );

		if( pReset->command == 'G' )
		    SET_BIT( pObj->extra_flags, ITEM_INVENTORY );
	    }
	    else
	    {
		pObj = create_object( pObjIndex );
	    }
            obj_to_char( pObj, LastMob );
	    if ( pReset->command == 'E' )
		equip_char( LastMob, pObj, pReset->arg3 );
	    last = TRUE;
	    break;

	case 'D':
	    break;

	case 'R':
/* OLC 1.1b
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_room: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		EXIT_DATA *pExit;
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2-1 );
		    pExit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pExit;
		}
	    }
*/
	    break;
	}
    }

    return;
}


/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pRoom = get_room_index(vnum) ) )
	    reset_room(pRoom);
    }

    return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;

    if( !pMobIndex )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    if( !char_free )
    {
	mob		= alloc_perm( sizeof( *mob ) );
    }
    else
    {
	mob		= char_free;
	char_free	= char_free->next;
    }

    clear_char( mob );
    mob->pIndexData	= pMobIndex;

    mob->name		= str_dup( pMobIndex->player_name );
    mob->short_descr	= str_dup( pMobIndex->short_descr );
    mob->long_descr	= str_dup( pMobIndex->long_descr  );
    mob->description	= str_dup( pMobIndex->description );
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->prompt         = "<%h %m %v>";

    mob->act		= pMobIndex->act;
    mob->affected_by	= pMobIndex->affected_by;
    mob->alignment	= pMobIndex->alignment;
    mob->race		= pMobIndex->race;
    mob->sex		= pMobIndex->sex;

    mob->exp		= pMobIndex->exp;
    mob->gold           = pMobIndex->gold;

    mob->armour		= pMobIndex->ac;

    mob->damage[ 0 ]	= number_fuzzy( pMobIndex->damnodice );
    mob->damage[ 1 ]	= number_fuzzy( pMobIndex->damsizedice );
    mob->damage[ 2 ]	= number_fuzzy( pMobIndex->damplus );

    mob->max_hit	= ( dice( pMobIndex->hitnodice, pMobIndex->hitsizedice )
				+ pMobIndex->hitplus );
    mob->hit		= mob->max_hit;
	    
    /*
     * Insert in list.
     */
    LINK( mob, char_list );

    /*
     * Increase existance count.
     */
    pMobIndex->count++;

    return mob;
}



/*
 * Create an instance of an object.
 * Crashes when a NULL object index is used? I think not...
 *                                                    -Lope
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex )
{
    static	OBJ_DATA	obj_zero;
		OBJ_DATA	*obj;
		int		i;

    if( !pObjIndex )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	bug( "Create_object: Substituted for Note.", 0 );
	pObjIndex = get_obj_index( OBJ_VNUM_NOTE );
    }

    if( !obj_free )
    {
	obj		= alloc_perm( sizeof( *obj ) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->wear_loc	= WEAR_NONE;

    obj->name		= str_dup( pObjIndex->name );
    obj->short_descr	= str_dup( pObjIndex->short_descr );
    obj->description	= str_dup( pObjIndex->description );
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->weight		= pObjIndex->weight;
    obj->cost		= pObjIndex->cost;

    for( i = 0 ; i < 5 ; i++ )
	obj->requires[ i ]	= pObjIndex->requires[ i ];


    /*
     * Mess with object properties.
     */
    if( IS_SET( obj->item_type, ITEM_SCROLL ) )
    {
	obj->scroll->level	= number_fuzzy( obj->scroll->level );
    }

    if( IS_SET( obj->item_type, ITEM_STAFF ) )
    {
	obj->staff->level	= number_fuzzy( obj->staff->level );
	obj->staff->max		= number_fuzzy( obj->staff->max );
	obj->staff->charges	= obj->staff->max;
    }

    if( IS_SET( obj->item_type, ITEM_WAND ) )
    {
	obj->wand->level	= number_fuzzy( obj->wand->level );
	obj->wand->max		= number_fuzzy( obj->wand->max );
	obj->wand->charges	= obj->wand->max;
    }

    if( IS_SET( obj->item_type, ITEM_WEAPON ) )
    {
	obj->weapon->damage[ 1 ] = number_fuzzy( obj->weapon->damage[ 1 ] );
	obj->weapon->damage[ 2 ] = number_fuzzy( obj->weapon->damage[ 2 ] );
    }

    if( IS_SET( obj->item_type, ITEM_ARMOUR ) )
    {
	obj->armour->ac[ 0 ]	= number_fuzzy( obj->armour->ac[ 0 ] );
	obj->armour->ac[ 1 ]	= number_fuzzy( obj->armour->ac[ 1 ] );
	obj->armour->ac[ 2 ]	= number_fuzzy( obj->armour->ac[ 2 ] );
	obj->armour->ac[ 3 ]	= number_fuzzy( obj->armour->ac[ 3 ] );
    }

    if( IS_SET( obj->item_type, ITEM_POTION ) )
    {
	obj->potion->level	= number_fuzzy( number_fuzzy( obj->potion->level ) );
    }

    if( IS_SET( obj->item_type, ITEM_PILL ) )
    {
	obj->pill->level	= number_fuzzy( number_fuzzy( obj->pill->level ) );
    }

    LINK( obj, object_list );

    pObjIndex->count++;

    return obj;
}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA	ch_zero;
    int			i;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    for( i = 0 ; i < MAX_SPOOL ; i++ )
	ch->last_note[ i ]      = 0;
    ch->logon			= current_time;
    ch->armour			= 100;
    ch->wizbit			= FALSE;
    ch->herobit			= FALSE;
    ch->position		= POS_STANDING;
    ch->practice		= 21;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->move			= 100;
    ch->max_move		= 100;
    return;
}



/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    OBJ_DATA	*obj;
    OBJ_DATA	*obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for( obj = ch->carrying ; obj ; obj = obj_next )
    {
	obj_next = obj->next_content;
	extract_obj( obj );
    }

    for( paf = ch->affected ; paf ; paf = paf_next )
    {
	paf_next = paf->next;
	affect_remove( ch, paf );
    }

    free_string( ch->name		);
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);
    ch->wizbit	= FALSE;
    ch->herobit	= FALSE;

    if( ch->pcdata )
    {
	if ( ch->pcdata->code )
	{
	    ch->pcdata->code->next	= coldata_free;
	    coldata_free		= ch->pcdata->code;
	}
	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
	free_string( ch->pcdata->title		);
        delete_aliases( ch->pcdata->aliases );
        ch->pcdata->aliases	= NULL;
	ch->pcdata->next 	= pcdata_free;
	pcdata_free      	= ch->pcdata;
    }

    LINK( ch, char_free );

    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for( ; ed ; ed = ed->next )
    {
	if( is_name_old( ( char * )name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex != NULL;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex != NULL;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates room virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for( pRoomIndex  = room_index_hash[ vnum % MAX_KEY_HASH ] ;
	 pRoomIndex ;
	 pRoomIndex  = pRoomIndex->next )
    {
	if( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
	exit( 1 );
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
    char *plast;
    char c;

    plast = top_string + sizeof(char *);
    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '~' )
	return &str_empty[0];

    for ( ;; )
    {
	/*
	 * Back off the char type lookup,
	 *   it was too dirty for portability.
	 *   -- Furey
	 */
	switch ( *plast = getc( fp ) )
	{
	default:
	    plast++;
	    break;

	case EOF:
	    bug( "Fread_string: EOF", 0 );
	    exit( 1 );
	    break;

	case '\n':
	    plast++;
	    *plast++ = '\r';
	    break;

	case '\r':
	    break;

	case '~':
	    plast++;
	    {
		union
		{
		    char *	pc;
		    char	rgc[sizeof(char *)];
		} u1;
		int ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof(char *)] == pHash[0]
		    &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
		    pString		= top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < sizeof(char *); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]	= pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof(char *);
		}
		else
		{
		    return str_dup( top_string + sizeof(char *) );
		}
	    }
	}
    }
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}

int fread_flag( FILE *fp )
{
    int		number;
    char	c;

    do
    {
	c = getc( fp );
    }
    while( isspace( c ) );

    number = 0;

    if( !isdigit( c ) )
    {
	while( ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' ) )
	{
	    number += flag_convert( c );
	    if( 'a' <= c && c <= 'z' ) c = getc( fp );
	    c = getc( fp );
	}
    }

    while( isdigit( c ) )
    {
	number = number * 10 + c - '0';
	c = getc( fp );
    }

    if( c == '|' )
	number += fread_flag( fp );

    else if( c != ' ')
	ungetc( c, fp );

    return number;
}

int flag_convert( char letter )
{
    long	bitsum = 0;
    char	i;

    if( 'A' <= letter && letter <= 'Z' ) 
    {
	bitsum = 1;
	for( i = letter ; i > 'A' ; i-- )
	    bitsum *= 2;
    }
    else if('a' <= letter && letter <= 'z' )
    {
	bitsum = 67108864; /* 2^26 */
	for( i = letter ; i > 'a' ; i -- )
	    bitsum *= 2;
    }

    return bitsum;
}

/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void	*pMem;
    int		*magic;
    int		iList;

    sMem += sizeof( *magic );

    for( iList = 0 ; iList < MAX_MEM_LIST ; iList++ )
    {
        if( sMem <= rgSizeList[ iList ] )
            break;
    }

    if( iList == MAX_MEM_LIST )
    {
        bug( "Alloc_mem: size %d too large.", sMem );
        exit( 1 );
    }


    if( !rgFreeList[ iList ] )
    {
        pMem              = alloc_perm( rgSizeList[ iList ] );
    }
    else
    {
        pMem              = rgFreeList[ iList ];
        rgFreeList[iList] = *( ( void ** ) rgFreeList[ iList ] );
    }

    magic	= ( int * ) pMem;
    *magic	= MAGIC_NUM;
    pMem	+= sizeof( *magic );

    return pMem;
}

/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    int		iList;
    int		*magic;

    pMem	-= sizeof( *magic );
    magic	= ( int * ) pMem;


    if( *magic != MAGIC_NUM )
    {
        bug( "Attempt to recyle invalid memory of size %d.", sMem );
        bug( ( char* ) pMem + sizeof( *magic ), 0 );
        return;
    }

    *magic	= 0;
    sMem	+= sizeof( *magic );

    for( iList = 0 ; iList < MAX_MEM_LIST ; iList++ )
    {
        if( sMem <= rgSizeList[ iList ] )
            break;
    }

    if( iList == MAX_MEM_LIST )
    {
        bug( "Free_mem: size %d too large.", sMem );
        exit( 1 );
    }

    *( ( void ** ) pMem )	= rgFreeList[ iList ];
    rgFreeList[ iList ]		= pMem;

    return;
}

/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
           void	*pMem;
    static char	*pMemPerm;
    static int	iMemPerm;

    while( sMem % sizeof( long ) != 0 )
	sMem++;

    if( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem	= pMemPerm + iMemPerm;
    iMemPerm	+= sMem;
    nAllocPerm	+= 1;
    sAllocPerm	+= sMem;

    return pMem;
}



/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
    char *str_new;

    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
	return (char *) str;

    str_new = alloc_mem( strlen(str) + 1 );
    strcpy( str_new, str );
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
    if( !pstr
     || pstr == &str_empty[0]
     || ( pstr >= string_space && pstr < top_string ) )
	return;

    free_mem( pstr, strlen( pstr ) + 1 );
    return;
}



void do_areas( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;

    iAreaHalf = (top_area + 1) / 2;
    pArea1    = area_first;
    pArea2    = area_first;
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	sprintf( buf, "%-39s%-39s\n\r",
	    pArea1->name, (pArea2 != NULL) ? pArea2->name : "" );
	send_to_char( buf, ch );
	pArea1 = pArea1->next;
	if ( pArea2 != NULL )
	    pArea2 = pArea2->next;
    }

    return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Affects %5d\n\r", top_affect    ); send_to_char( buf, ch );
    sprintf( buf, "Areas   %5d\n\r", top_area      ); send_to_char( buf, ch );
    sprintf( buf, "ExDes   %5d\n\r", top_ed        ); send_to_char( buf, ch );
    sprintf( buf, "Exits   %5d\n\r", top_exit      ); send_to_char( buf, ch );
    sprintf( buf, "Helps   %5d\n\r", top_help      ); send_to_char( buf, ch );
    sprintf( buf, "Mobs    %5d\n\r", top_mob_index ); send_to_char( buf, ch );
    sprintf( buf, "Objs    %5d\n\r", top_obj_index ); send_to_char( buf, ch );
    sprintf( buf, "Resets  %5d\n\r", top_reset     ); send_to_char( buf, ch );
    sprintf( buf, "Rooms   %5d\n\r", top_room      ); send_to_char( buf, ch );
    sprintf( buf, "Shops   %5d\n\r", top_shop      ); send_to_char( buf, ch );

    sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	nAllocString, sAllocString, MAX_STRING );
    send_to_char( buf, ch );

    sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
	nAllocPerm, sAllocPerm );
    send_to_char( buf, ch );

    return;
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch( number_bits( 2 ) )
    {
	case 0:
	    number -= 1;
	    break;
	case 3:
	    number += 1;
	    break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if( ( to = to - from + 1 ) <= 1 )
	return from;

    for( power = 2 ; power < to ; power <<= 1 )
	;

    while( ( number = number_mm( ) & (power - 1) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & (128-1) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm( ) & (8-1) ) > 5 )
	;

    return door;
}



int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int iState;

    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ((int) current_time) & ((1 << 30) - 1);
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = (piState[iState-1] + piState[iState-2])
			& ((1 << 30) - 1);
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= (piState[iState1] + piState[iState2])
			& ((1 << 30) - 1);
    piState[iState1]	= iRand;
    if ( ++iState1 == 55 )
	iState1 = 0;
    if ( ++iState2 == 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int	idice;
    int	sum;

    switch ( size )
    {
	case 0:
	    return 0;
	case 1:
	    return number;
    }

    for( idice = 0, sum = 0 ; idice < number ; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for( ; *str != '\0' ; str++ )
    {
	if( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if( !astr )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if( !bstr )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for( ; *astr || *bstr ; astr++, bstr++ )
    {
	if( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if( !astr )
    {
	bug( "Str_prefix: null astr.", 0 );
	return TRUE;
    }

    if( !bstr )
    {
	bug( "Str_prefix: null bstr.", 0 );
	return TRUE;
    }

    for( ; *astr ; astr++, bstr++ )
    {
	if( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int		sstr1;
    int		sstr2;
    int		ichar;
    char	c0;

    if( ( c0 = LOWER( astr[0] ) ) == '\0' )
	return FALSE;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );

    for( ichar = 0 ; ichar <= sstr2 - sstr1 ; ichar++ )
    {
	if( c0 == LOWER( bstr[ichar] ) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}



/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC(ch) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
	    ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];
    FILE *fp;

    if ( fpArea != NULL )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );

	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );

    fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) != NULL )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}



/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}

/*
 * MOBprogram code block
*/
/* the functions */

/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */
int mprog_name_to_type (name) char* name;
{
   if (!str_cmp(name,"in_file_prog"))	return IN_FILE_PROG;
   if (!str_cmp(name,"act_prog"))	return ACT_PROG;
   if (!str_cmp(name,"speech_prog"))	return SPEECH_PROG;
   if (!str_cmp(name,"rand_prog"))	return RAND_PROG;
   if (!str_cmp(name,"fight_prog"))	return FIGHT_PROG;
   if (!str_cmp(name,"hitprcnt_prog"))	return HITPRCNT_PROG;
   if (!str_cmp(name,"death_prog"))	return DEATH_PROG;
   if (!str_cmp(name,"entry_prog"))	return ENTRY_PROG;
   if (!str_cmp(name,"greet_prog"))	return GREET_PROG;
   if (!str_cmp(name,"all_greet_prog"))	return ALL_GREET_PROG;
   if (!str_cmp(name,"give_prog"))	return GIVE_PROG;
   if (!str_cmp(name,"bribe_prog"))	return BRIBE_PROG;
   return(ERROR_PROG);
}

/* This routine reads in scripts of MOBprograms from a file */
MPROG_DATA* mprog_file_read(f, mprg, pMobIndex)
char* f;
MPROG_DATA* mprg;
MOB_INDEX_DATA* pMobIndex;
{
  MPROG_DATA* mprg2;
  FILE * progfile;
  char letter;
  bool done=FALSE;
  char MOBProgfile[MAX_INPUT_LENGTH];

  sprintf( MOBProgfile, "%s%s", MOB_DIR, f );
  progfile=fopen(MOBProgfile,"r");
  if (!progfile)
  {
     bug("Mob:%d couldnt open mobprog file",pMobIndex->vnum);
     exit( 1 );
  }
  mprg2=mprg;
  switch (letter=fread_letter(progfile))
  {
    case '>':
     break;
    case '|':
       bug("empty mobprog file.", 0);
       exit( 1 );
     break;
    default:
       bug("in mobprog file syntax error.", 0);
       exit( 1 );
     break;
  }
  while (!done)
  {
  mprg2->type=mprog_name_to_type(fread_word(progfile));
    switch (mprg2->type)
    {
     case ERROR_PROG:
        bug( "mobprog file type error", 0 );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        bug( "mprog file contains a call to file.", 0 );
        exit( 1 );
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
        mprg2->arglist=fread_string(progfile);
        mprg2->comlist=fread_string(progfile);
        switch (letter=fread_letter(progfile))
        {
          case '>':
             mprg2->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg2=mprg2->next;
             mprg2->next=NULL;
           break;
          case '|':
             done=TRUE;
           break;
          default:
             bug( "in mobprog file syntax error.",0);
             exit( 1 );
           break;
        }
      break;
    }
  }
  fclose(progfile);
  return mprg2;
}


/* Snarf a MOBprogram section from the area file.
 */
void load_mobprogs(fp) FILE * fp;
{
char letter;
MOB_INDEX_DATA * iMob;
int value;
MPROG_DATA* original;
MPROG_DATA* working;

for (;;) switch (letter=fread_letter( fp))
  {
  default:
     bug( "Load_mobprogs: bad command '%c'.",letter);
     exit(1);
   break;
  case 'S':
  case 's':
     fread_to_eol(fp); 
     return;
  case '*':
     fread_to_eol(fp); 
     break;
  case 'M':
  case 'm':
   value=fread_number(fp);
   if ((iMob=get_mob_index(value))==NULL)
     {
      bug( "Load_mobprogs: vnum %d doesnt exist", value );
      exit( 1 );
     }
    
     if ((original=(iMob->mobprogs)))
       for (;original->next!=NULL; original=original->next);
     working=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
     if (original) original->next=working;
     else iMob->mobprogs=working;
     working=mprog_file_read(fread_word(fp),working,iMob);
     working->next=NULL;
     fread_to_eol(fp);
     break;
   }
} 

/* This procedure is responsible for writing any in_file MOBprograms.
 */
void mprog_write_programs( FILE *fp,MOB_INDEX_DATA *pMobIndex )
{
    return;
}

/* This procedure is responsible for reading any in_file MOBprograms.
 */
void mprog_read_programs(fp,pMobIndex) FILE* fp; MOB_INDEX_DATA * pMobIndex;
{
MPROG_DATA * mprg;
bool done=FALSE;
char letter;
  if ( (letter=fread_letter(fp)) != '>' )
  {
      bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
      exit( 1 );
  }
  pMobIndex->mobprogs=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
  mprg=pMobIndex->mobprogs;
  while (!done)
  {
    mprg->type=mprog_name_to_type(fread_word(fp));
    switch (mprg->type)
    {
     case ERROR_PROG:
        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        mprg=mprog_file_read(fread_string(fp),mprg,pMobIndex);
        fread_to_eol(fp);
        switch (letter=fread_letter(fp))
        {
          case '>':
             mprg->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg=mprg->next;
             mprg->next=NULL;
           break;
          case '|':
             mprg->next=NULL;
             fread_to_eol(fp);
             done=TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
        mprg->arglist=fread_string(fp);
        fread_to_eol(fp);
        mprg->comlist=fread_string(fp);
        fread_to_eol(fp);
        switch (letter=fread_letter(fp))
        {
          case '>':
             mprg->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
             mprg=mprg->next;
             mprg->next=NULL;
           break;
          case '|':
             mprg->next=NULL;
             fread_to_eol(fp);
             done=TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
    }
  }
}


/*======================================================================= 
 | dynamic strcat function, code fix by Zavod                           |
 |=======================================================================
 | DO NOT pass something that's str_dup ed here into dest, or allocated |
 | by using mud functions.  To initialize do "buf1 = NULL;"             |
 | I intentionally did not use mud memory routines to avoid             |
 | 'too large' errors -- Zavod                                          |
 =======================================================================*/
void str_cat( char **dest, const char *add )
{           
    static      int	dest_size;
                int	add_size;
                char	*tmp;
 
    if( !*dest || !**dest )
        dest_size = 0;
 
    add_size = strlen( add );
 
    /*
     * Enlarge chunk if possible, otherwise get new chunk and copy old stuff in
     */
    tmp = ( char * ) realloc( *dest, add_size + dest_size + 1 );

    /*
     * This is needed in case there is not enough realloc'able memory available
     * - Lope
     */
    if( !tmp )
    {
	tmp = ( char * ) malloc( add_size + dest_size + 1 );
    }

    *dest = tmp;

    /*
     * Go straight to where we know the beginning is
     */
    sprintf( *dest + dest_size, "%s", add );
    dest_size += add_size;
}

/*
 * Hash command lookups original code by Ironhand
 * Revised Jan '96 -Ant
 */
void hash_commands( void )
{
   sh_int         cmd;
   sh_int         cnt;
   sh_int         num_cmds;
   sh_int         iHash;

   for( num_cmds = 0 ; cmd_table[ num_cmds ].name[ 0 ] != '\0' ; num_cmds++ )
	;

   for( cnt = 0 ; cnt < 126 ; cnt++ )
	Hash_commands[ cnt ] = num_cmds;

   for( cmd = 0 ; cmd_table[ cmd ].name[ 0 ] != '\0' ; cmd++ )
   {
	iHash = cmd_table[ cmd ].name[ 0 ] % 126;
	if( Hash_commands[ iHash ] == num_cmds )
	    Hash_commands[ iHash ] = cmd;
   }

   return;
}

/*
 * New social type to make socials into linked lists
 * and hash them by their first letter, can speed searches up
 * by over 100 times.
 * Mar '96 -Ant
 */
void load_socials( FILE *fp )
{ 
    SOCIAL_DATA	*pSocial;
    int		cnt;
    int		iHash = 0;

    for( cnt = 0 ; cnt < 126 ; cnt++ )
    {
	social_first[ cnt ]	= NULL;
	social_last[ cnt ]	= NULL;
    }

    for( ; ; )
    {
	pSocial			= alloc_perm( sizeof( *pSocial ) );
	pSocial->social_num 	= fread_number( fp );
	pSocial->name 		= fread_string( fp );
	if( pSocial->name[ 0 ] == '$' )
	    break;

	pSocial->char_no_arg 	= fread_string( fp );
	pSocial->others_no_arg 	= fread_string( fp );
	pSocial->char_found 	= fread_string( fp );
	pSocial->others_found 	= fread_string( fp );
	pSocial->vict_found 	= fread_string( fp );
	pSocial->char_auto 	= fread_string( fp );
	pSocial->others_auto 	= fread_string( fp );

	iHash	= pSocial->name[ 0 ] % 126;

	if( !social_first[ iHash ] )
	    social_first[ iHash ] 	= pSocial;
	if( social_last[ iHash ] )
	    social_last[ iHash ]->next 	= pSocial;

	pSocial->prev			= social_last[ iHash ];
	social_last[ iHash ] 		= pSocial;
	pSocial->next 			= NULL;
	top_social++;
    }

    return;
}

/*
 * Make_parts takes the race structure and applies the parts
 * found there to the ch->parts structures. -Lope
 */
void make_parts( CHAR_DATA *ch )
{
    PART_DATA	*pPart		= NULL;
    PART_DATA	*part_next	= NULL;
    long	part;

    if( !ch->race )
    {
	bug( "Make_parts: NULL ch->race field!!!!", 0 );
	return;
    }

    if( ch->parts )
    {
	for( pPart = ch->parts ; pPart ; pPart = part_next )
	{
	    part_next = pPart->next;
	    UNLINK( pPart, ch->parts );
	    free_part( pPart );
	}
    }

    for( part = 1 ; part < PART_MAX ; part = part << 1 )
    {
	if( IS_SET( ch->race->parts, part ) )
	{
	    pPart		= new_part( );
	    pPart->type		= part;
	    pPart->armour	= 0;
	    pPart->scars	= 0;

	    LINK( pPart, ch->parts );
	}
    }

    return;
}

void delete_aliases( ALIAS *head )
{
    if( !head ) return;
    if( head->next ) 
	delete_aliases( head->next );
    if( head->command ) free_string( head->command );
    if( head->string ) free_string( head->string );
    free_mem( head, sizeof( ALIAS ) );
}
