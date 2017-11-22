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
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif

#if !defined(macintosh) && !defined(MSDOS)
void    system          args( ( char *string ) );
#endif

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			    FILE *fp, int iNest ) );
void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );


/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint[ MAX_STRING_LENGTH ];

    strint[0] = LOWER( str[ 0 ] );
    return strint;

}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    FILE	*fp;
    char	strsave[ MAX_INPUT_LENGTH ];

    if( IS_NPC( ch ) )
	return;

    if( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );

    /*
     * player files parsed directories by Yaz 4th Realm
     */
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );

    if( !( fp = fopen( strsave, "w" ) ) )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if( ch->carrying )
	    fwrite_obj( ch, ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    ALIAS 	*alias;
    AFFECT_DATA	*paf;
    PART_DATA	*pPart;
    int		i;
    int		sn;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"		);

    fprintf( fp, "Name         %s~\n",	ch->name		);
    fprintf( fp, "ShortDescr   %s~\n",	ch->short_descr		);
    fprintf( fp, "LongDescr    %s~\n",	ch->long_descr		);
    fprintf( fp, "Description  %s~\n",	ch->description		);
    fprintf( fp, "Prompt       %s~\n",	ch->prompt		);
    fprintf( fp, "Sex          %d\n",	ch->sex			);
    fprintf( fp, "Class        %d\n",	ch->class->number	);
    fprintf( fp, "Race         %d\n",	ch->race->number	);
    fprintf( fp, "Security     %d\n",   ch->pcdata->security	);  /* OLC */
    fprintf( fp, "Wizbit       %d\n",	ch->wizbit		);
    fprintf( fp, "Played       %d\n",
	ch->played + (int) (current_time - ch->logon)		);

    fprintf( fp, "Spool        %d ", MAX_SPOOL           );
    for( i = 0 ; i < MAX_SPOOL ; i++ )
	fprintf( fp, "%ld ",   ch->last_note[ i ]           );
    fprintf( fp, "\n" );

    fprintf( fp, "Room         %d\n",
	(  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	&& ch->was_in_room != NULL )
	    ? ch->was_in_room->vnum
	    : ch->in_room->vnum );

    fprintf( fp, "HpManaMove   %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Gold         %d\n",	ch->gold		);
    fprintf( fp, "Exp          %d\n",	ch->exp			);
    fprintf( fp, "Channels     %ld\n",  ch->chan		);
    fprintf( fp, "Penalty      %ld\n",  ch->pen			);
    fprintf( fp, "Act          %ld\n",  ch->act			);
    fprintf( fp, "AffectedBy   %ld\n",	ch->affected_by		);
    if( !IS_NPC( ch ) )
    {
	fprintf( fp, "Immortal     %ld\n",	ch->pcdata->immortal	);
    }
    fprintf( fp, "Position     %d\n",
        ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

    fprintf( fp, "Practice     %d\n",	ch->practice		);
    fprintf( fp, "SavingThrow  %d\n",	ch->saving_throw	);
    fprintf( fp, "Alignment    %d\n",	ch->alignment		);
    fprintf( fp, "Hitroll      %d\n",	ch->hitroll		);
    fprintf( fp, "Damroll      %d\n",	ch->damroll		);
    fprintf( fp, "Armour       %d\n",	ch->armour		);
    fprintf( fp, "Wimpy        %d\n",	ch->wimpy		);
    fprintf( fp, "Deaf         %d\n",	ch->deaf		);

    if( IS_NPC( ch ) )
    {
	fprintf( fp, "Vnum         %d\n",	ch->pIndexData->vnum	);
    }
    else
    {
	fprintf( fp, "Password     %s~\n",	ch->pcdata->pwd		);
	fprintf( fp, "Bamfin       %s~\n",	ch->pcdata->bamfin	);
	fprintf( fp, "Bamfout      %s~\n",	ch->pcdata->bamfout	);
	fprintf( fp, "Title        %s~\n",	ch->pcdata->title	);
	fprintf( fp, "AttrPerm     %d %d %d %d %d\n",
	    ch->pcdata->perm_str,
	    ch->pcdata->perm_int,
	    ch->pcdata->perm_wis,
	    ch->pcdata->perm_dex,
	    ch->pcdata->perm_con );

	fprintf( fp, "AttrMod      %d %d %d %d %d\n",
	    ch->pcdata->mod_str, 
	    ch->pcdata->mod_int, 
	    ch->pcdata->mod_wis,
	    ch->pcdata->mod_dex, 
	    ch->pcdata->mod_con );

	fprintf( fp, "Condition    %d %d %d\n",
	    ch->pcdata->condition[0],
	    ch->pcdata->condition[1],
	    ch->pcdata->condition[2] );

	fprintf( fp, "Pagelen      %d\n",   ch->pcdata->pagelen     );

	fprintf( fp, "Coloura     %d%d %d%d %d%d %d%d %d%d\n",   
		ch->pcdata->code->text[0],
		ch->pcdata->code->text[1],
		ch->pcdata->code->auction[0],
		ch->pcdata->code->auction[1],
		ch->pcdata->code->chat[0],
		ch->pcdata->code->chat[1],
		ch->pcdata->code->music[0],
		ch->pcdata->code->music[1],
		ch->pcdata->code->question[0],
		ch->pcdata->code->question[1] );
	fprintf( fp, "Colourb     %d%d %d%d %d%d %d%d %d%d\n",   
		ch->pcdata->code->answer[0],
		ch->pcdata->code->answer[1],
		ch->pcdata->code->class_text[0],
		ch->pcdata->code->class_text[1],
		ch->pcdata->code->class_name[0],
		ch->pcdata->code->class_name[1],
		ch->pcdata->code->immtalk_text[0],
		ch->pcdata->code->immtalk_text[1],
		ch->pcdata->code->immtalk_type[0],
		ch->pcdata->code->immtalk_type[1] );
	fprintf( fp, "Colourc     %d%d %d%d %d%d %d%d %d%d\n",   
		ch->pcdata->code->info[0],
		ch->pcdata->code->info[1],
		ch->pcdata->code->tell[0],
		ch->pcdata->code->tell[1],
		ch->pcdata->code->reply[0],
		ch->pcdata->code->reply[1],
		ch->pcdata->code->gtell_text[0],
		ch->pcdata->code->gtell_text[1],
		ch->pcdata->code->gtell_type[0],
		ch->pcdata->code->gtell_type[1] );
	fprintf( fp, "Colourd     %d%d %d%d %d%d %d%d %d%d\n",   
		ch->pcdata->code->room_title[0],
		ch->pcdata->code->room_title[1],
		ch->pcdata->code->room_text[0],
		ch->pcdata->code->room_text[1],
		ch->pcdata->code->room_exits[0],
		ch->pcdata->code->room_exits[1],
		ch->pcdata->code->room_things[0],
		ch->pcdata->code->room_things[1],
		ch->pcdata->code->prompt[0],
		ch->pcdata->code->prompt[1] );
	fprintf( fp, "Coloure     %d%d %d%d %d%d %d%d %d%d\n",   
		ch->pcdata->code->fight_death[0],
		ch->pcdata->code->fight_death[1],
		ch->pcdata->code->fight_yhit[0],
		ch->pcdata->code->fight_yhit[1],
		ch->pcdata->code->fight_ohit[0],
		ch->pcdata->code->fight_ohit[1],
		ch->pcdata->code->fight_thit[0],
		ch->pcdata->code->fight_thit[1],
		ch->pcdata->code->fight_skill[0],
		ch->pcdata->code->fight_skill[1] );
	fprintf( fp, "Colourf     %d%d\n",   
		ch->pcdata->code->bbro[0],
		ch->pcdata->code->bbro[1] );

        alias = ch->pcdata->aliases; 
        if( alias && alias->next )
	    for( i = 10 , alias = alias->next ; alias && i-- ; alias = alias->next )
		fprintf( fp, "Alias      %s %s~\n",
			alias->command,
			alias->string );

	for( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if( skill_table[sn].name && ch->pcdata->learned[sn] > 0 )
	    {
		fprintf( fp, "Skill        %d '%s'\n",
		    ch->pcdata->learned[sn], skill_table[sn].name );
	    }
	}
    }

    for( pPart = ch->parts ; pPart ; pPart = pPart->next )
    {
	fprintf( fp, "Part   %s %d %d\n",
	    ascii_flag( pPart->type ),
	    pPart->armour,
	    pPart->scars );
    }

    for( paf = ch->affected ; paf ; paf = paf->next )
    {
	fprintf( fp, "Affect %d %d %d %d %ld\n",
	    paf->type,
	    paf->duration,
	    paf->modifier,
	    paf->location,
	    paf->bitvector );
    }

    fprintf( fp, "End\n\n" );
    return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if( obj->next_content )
	fwrite_obj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
    if( IS_SET( obj->item_type, ITEM_KEY )
     || IS_SET( obj->item_type, ITEM_POTION ) )
	return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",	iNest			     );
    fprintf( fp, "Name         %s~\n",	obj->name		     );
    fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr	     );
    fprintf( fp, "Description  %s~\n",	obj->description	     );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
    fprintf( fp, "ExtraFlags   %ld\n",	obj->extra_flags	     );
    fprintf( fp, "WearFlags    %ld\n",	obj->wear_flags		     );
    fprintf( fp, "WearLoc      %d\n",	obj->wear_loc		     );
    fprintf( fp, "ItemType     %ld\n",	obj->item_type		     );
    fprintf( fp, "Weight       %d\n",	obj->weight		     );
    fprintf( fp, "Timer        %d\n",	obj->timer		     );
    fprintf( fp, "Cost         %d\n",	obj->cost		     );

    if( IS_SET( obj->item_type, ITEM_POTION ) )
    {
	fprintf( fp, "Potion       %d ", obj->potion->level	);

	if( obj->potion->spell[0] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->potion->spell[0]].name
		 ? skill_table[obj->potion->spell[0]].name : "" );
	}

	if( obj->potion->spell[1] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->potion->spell[1]].name
		 ? skill_table[obj->potion->spell[1]].name : "" );
	}

	if( obj->potion->spell[2] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->potion->spell[2]].name
		 ? skill_table[obj->potion->spell[2]].name : "" );
	}

	if( obj->potion->spell[3] > 0 )
	{
	    fprintf( fp, "'%s'\n", 
		skill_table[obj->potion->spell[3]].name
		 ? skill_table[obj->potion->spell[3]].name : "" );
	}
    }

    if( IS_SET( obj->item_type, ITEM_SCROLL ) )
    {
	fprintf( fp, "Scroll       %d ", obj->scroll->level	);

	if( obj->scroll->spell[0] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->scroll->spell[0]].name
		 ? skill_table[obj->scroll->spell[0]].name : "" );
	}

	if( obj->scroll->spell[1] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->scroll->spell[1]].name
		 ? skill_table[obj->scroll->spell[1]].name : "" );
	}

	if( obj->scroll->spell[2] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->scroll->spell[2]].name
		 ? skill_table[obj->scroll->spell[2]].name : "" );
	}

	if( obj->scroll->spell[3] > 0 )
	{
	    fprintf( fp, "'%s'\n", 
		skill_table[obj->scroll->spell[3]].name
		 ? skill_table[obj->scroll->spell[3]].name : "" );
	}
    }

    if( IS_SET( obj->item_type, ITEM_PILL ) )
    {
	fprintf( fp, "Pill         %d ", obj->pill->level	);

	if( obj->pill->spell[0] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->pill->spell[0]].name
		 ? skill_table[obj->pill->spell[0]].name : "" );
	}

	if( obj->pill->spell[1] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->pill->spell[1]].name
		 ? skill_table[obj->pill->spell[1]].name : "" );
	}

	if( obj->pill->spell[2] > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->pill->spell[2]].name
		 ? skill_table[obj->pill->spell[2]].name : "" );
	}

	if( obj->pill->spell[3] > 0 )
	{
	    fprintf( fp, "'%s'\n", 
		skill_table[obj->pill->spell[3]].name
		 ? skill_table[obj->pill->spell[3]].name : "" );
	}
    }

    if( IS_SET( obj->item_type, ITEM_STAFF ) )
    {
	fprintf( fp, "Staff        %d ", obj->staff->level	);

	if( obj->staff->spell > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->staff->spell].name
		 ? skill_table[obj->staff->spell].name : "" );
	}

	fprintf( fp, "%d ",	obj->staff->charges	);
	fprintf( fp, "%d\n",	obj->staff->max		);
    }

    if( IS_SET( obj->item_type, ITEM_WAND ) )
    {
	fprintf( fp, "Wand         %d ", obj->wand->level	);

	if( obj->wand->spell > 0 )
	{
	    fprintf( fp, "'%s' ", 
		skill_table[obj->wand->spell].name
		 ? skill_table[obj->wand->spell].name : "" );
	}

	fprintf( fp, "%d ",	obj->wand->charges	);
	fprintf( fp, "%d\n",	obj->wand->max		);
    }

    if( IS_SET( obj->item_type, ITEM_LIGHT ) )
    {
	fprintf( fp, "Light        %d %d\n",
	    obj->light->intensity,
	    obj->light->duration );
    }

    if( IS_SET( obj->item_type, ITEM_MONEY ) )
    {
	fprintf( fp, "Money        %d %d %d\n",
	    obj->money->gold,
	    obj->money->silver,
	    obj->money->copper );
    }

    if( IS_SET( obj->item_type, ITEM_FOOD ) )
    {
	fprintf( fp, "Food         %d %d\n",
	    obj->food->bite,
	    obj->food->poison ? 1 : 0 );
    }

    if( IS_SET( obj->item_type, ITEM_DRINK_CON ) )
    {
	fprintf( fp, "Drink        %d %d %d %d\n",
	    obj->drink->volume,
	    obj->drink->limit,
	    obj->drink->liquid,
	    obj->drink->poison ? 1 : 0 );
    }

    if( IS_SET( obj->item_type, ITEM_CONTAINER ) )
    {
	fprintf( fp, "Container    %ld %d %d %d\n",
	    obj->cont->closed,
	    obj->cont->key,
	    obj->cont->capacity,
	    obj->cont->single );
    }

    if( IS_SET( obj->item_type, ITEM_ARMOUR ) )
    {
	fprintf( fp, "Armour       %d %d %d %d\n",
	    obj->armour->ac[0],
	    obj->armour->ac[1],
	    obj->armour->ac[2],
	    obj->armour->ac[3] );
    }

    if( IS_SET( obj->item_type, ITEM_WEAPON ) )
    {
	fprintf( fp, "Weapon       %d %d %ld %d %dd%d+%d\n",
	    obj->weapon->type,
	    obj->weapon->message,
	    obj->weapon->flags,
	    obj->weapon->hit,
	    obj->weapon->damage[0],
	    obj->weapon->damage[1],
	    obj->weapon->damage[2] );
    }

    for( paf = obj->affected ; paf ; paf = paf->next )
    {
	fprintf( fp, "Affect       %d %d %d %d %ld\n",
	    paf->type,
	    paf->duration,
	    paf->modifier,
	    paf->location,
	    paf->bitvector );
    }

    for( ed = obj->extra_descr ; ed ; ed = ed->next )
    {
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
	    ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if( obj->contains )
	fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    static	PC_DATA		pcdata_zero;
    static	COLOUR_DATA	coldata_zero;
    		char		strsave[MAX_INPUT_LENGTH];
    		char		buf[MAX_STRING_LENGTH];
    		CHAR_DATA	*ch;
    		FILE		*fp;
    		bool		found;
    		int		i;

    if( !char_free )
    {
	ch				= alloc_perm( sizeof(*ch) );
    }
    else
    {
	ch				= char_free;
	char_free			= char_free->next;
    }
    clear_char( ch );

    if( !pcdata_free )
    {
	ch->pcdata			= alloc_perm( sizeof(*ch->pcdata) );
    }
    else
    {
	ch->pcdata			= pcdata_free;
	pcdata_free			= pcdata_free->next;
    }
    *ch->pcdata				= pcdata_zero;

    if ( !coldata_free )
    {
	ch->pcdata->code		= alloc_perm( sizeof( *ch->pcdata->code ) );
    }
    else
    {
	ch->pcdata->code		= coldata_free;
	coldata_free			= coldata_free->next;
    }
    *ch->pcdata->code			= coldata_zero;
    ch->pcdata->aliases                 = NULL;

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->prompt                          = str_dup( "<%hhp %mm %vmv> " );
    for( i = 0 ; i < MAX_SPOOL ; i++ )
	ch->last_note[ i ]              = 0;
    ch->act				= PLR_BLANK
					| PLR_COMBINE
					| PLR_PROMPT;
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->perm_str		= 13;
    ch->pcdata->perm_int		= 13; 
    ch->pcdata->perm_wis		= 13;
    ch->pcdata->perm_dex		= 13;
    ch->pcdata->perm_con		= 13;
    ch->pcdata->condition[COND_THIRST]	= 48;
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->pagelen                 = 20;
    ch->pcdata->security		= 0;	/* OLC */

    ch->pcdata->code->text[0]		= ( NORMAL );
    ch->pcdata->code->text[1]		= ( COLOUR_NONE );
    ch->pcdata->code->auction[0]	= ( NORMAL );
    ch->pcdata->code->auction[1]	= ( YELLOW );
    ch->pcdata->code->chat[0]		= ( NORMAL );
    ch->pcdata->code->chat[1]		= ( MAGENTA );
    ch->pcdata->code->music[0]		= ( NORMAL );
    ch->pcdata->code->music[1]		= ( YELLOW );
    ch->pcdata->code->question[0]	= ( NORMAL );
    ch->pcdata->code->question[1]	= ( YELLOW );
    ch->pcdata->code->answer[0]		= ( NORMAL );
    ch->pcdata->code->answer[1]		= ( YELLOW );
    ch->pcdata->code->class_text[0]	= ( NORMAL );
    ch->pcdata->code->class_text[1]	= ( YELLOW );
    ch->pcdata->code->class_name[0]	= ( NORMAL );
    ch->pcdata->code->class_name[1]	= ( CYAN );
    ch->pcdata->code->immtalk_text[0]	= ( NORMAL );
    ch->pcdata->code->immtalk_text[1]	= ( CYAN );
    ch->pcdata->code->immtalk_type[0]	= ( NORMAL );
    ch->pcdata->code->immtalk_type[1]	= ( YELLOW );
    ch->pcdata->code->info[0]		= ( NORMAL );
    ch->pcdata->code->info[1]		= ( YELLOW );
    ch->pcdata->code->say[0]		= ( NORMAL );
    ch->pcdata->code->say[1]		= ( GREEN );
    ch->pcdata->code->tell[0]		= ( NORMAL );
    ch->pcdata->code->tell[1]		= ( GREEN );
    ch->pcdata->code->reply[0]		= ( NORMAL );
    ch->pcdata->code->reply[1]		= ( GREEN );
    ch->pcdata->code->gtell_text[0]	= ( NORMAL );
    ch->pcdata->code->gtell_text[1]	= ( GREEN );
    ch->pcdata->code->gtell_type[0]	= ( NORMAL );
    ch->pcdata->code->gtell_type[1]	= ( RED );
    ch->pcdata->code->bbro[0]		= ( NORMAL );
    ch->pcdata->code->bbro[1]		= ( GREEN );
    ch->pcdata->code->room_title[0]	= ( NORMAL );
    ch->pcdata->code->room_title[1]	= ( CYAN );
    ch->pcdata->code->room_text[0]	= ( NORMAL );
    ch->pcdata->code->room_text[1]	= ( CYAN );
    ch->pcdata->code->room_exits[0]	= ( NORMAL );
    ch->pcdata->code->room_exits[1]	= ( GREEN );
    ch->pcdata->code->room_things[0]	= ( NORMAL );
    ch->pcdata->code->room_things[1]	= ( CYAN );
    ch->pcdata->code->prompt[0]		= ( NORMAL );
    ch->pcdata->code->prompt[1]		= ( CYAN );
    ch->pcdata->code->fight_death[0]	= ( NORMAL );
    ch->pcdata->code->fight_death[1]	= ( RED );
    ch->pcdata->code->fight_yhit[0]	= ( NORMAL );
    ch->pcdata->code->fight_yhit[1]	= ( GREEN );
    ch->pcdata->code->fight_ohit[0]	= ( NORMAL );
    ch->pcdata->code->fight_ohit[1]	= ( YELLOW );
    ch->pcdata->code->fight_thit[0]	= ( NORMAL );
    ch->pcdata->code->fight_thit[1]	= ( RED );
    ch->pcdata->code->fight_skill[0]	= ( NORMAL );
    ch->pcdata->code->fight_skill[1]	= ( CYAN );

    found = FALSE;
    fclose( fpReserve );

    /* parsed player file directories by Yaz of 4th Realm */
    /* decompress if .gz file exists - Thx Alander */
#if !defined(macintosh) && !defined(MSDOS)
    sprintf( strsave, "%s%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ), ".gz" );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	system( buf );
    }
#endif

#if !defined(machintosh) && !defined(MSDOS)
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
#endif
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    ALIAS 	*alias;
    char 	*p;
    char 	*p2;
    char	*word;
    char	buf[ MAX_STRING_LENGTH ];
    bool	fMatch;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",		ch->act,		fread_number( fp ) );
	    KEY( "AffectedBy",	ch->affected_by,	fread_number( fp ) );
	    KEY( "Alignment",	ch->alignment,		fread_number( fp ) );
	    KEY( "Armour",	ch->armour,		fread_number( fp ) );
	    KEY( "Armor",	ch->armour,		fread_number( fp ) );

	    if ( !str_cmp( word, "Affect" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf		= alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}

		paf->type	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= ch->affected;
		ch->affected	= paf;
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrMod"  ) )
	    {
		ch->pcdata->mod_str  = fread_number( fp );
		ch->pcdata->mod_int  = fread_number( fp );
		ch->pcdata->mod_wis  = fread_number( fp );
		ch->pcdata->mod_dex  = fread_number( fp );
		ch->pcdata->mod_con  = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrPerm" ) )
	    {
		ch->pcdata->perm_str = fread_number( fp );
		ch->pcdata->perm_int = fread_number( fp );
		ch->pcdata->perm_wis = fread_number( fp );
		ch->pcdata->perm_dex = fread_number( fp );
		ch->pcdata->perm_con = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Alias" ) )
	    {
		if( !ch->pcdata->aliases )
		{
		    ch->pcdata->aliases = (ALIAS *)alloc_mem( sizeof( ALIAS ) );
		    ch->pcdata->aliases->next = NULL;
		    ch->pcdata->aliases->command = NULL;
		    ch->pcdata->aliases->string = NULL;
		}
		for( alias = ch->pcdata->aliases ; alias->next != NULL ; 
		    alias = alias->next );
		alias->next = (ALIAS *)alloc_mem( sizeof( ALIAS ) );
		alias = alias->next;
		p = fread_string( fp );
		p2 = one_argument( p, buf );
		alias->command = str_dup( buf );
		alias->string = str_dup( p2 );
		free_string( p );
		alias->next = NULL;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'B':
	    KEY( "Bamfin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bamfout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    break;

	case 'C':
	    KEY( "Channels",	ch->chan,		fread_flag( fp ) );
	    if ( !str_cmp( word, "Class" ) )
	    {
		CLASS_DATA	*pClass;
		int		number;

		number = fread_number( fp );

		for( pClass = class_list ; pClass ; pClass = pClass->next )
		{
		    if( number == pClass->number )
		    {
			ch->class = pClass;
			fMatch = TRUE;
			break;
		    }
		}
		
		ch->class = class_list;
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Condition" ) )
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Coloura" ) )
	    {
		ch->pcdata->code->text[1]		= fread_number( fp );
		if( ch->pcdata->code->text[1] > 10 )
		{
		    ch->pcdata->code->text[1] -= 10;
		    ch->pcdata->code->text[0] = 1;
		}
		ch->pcdata->code->auction[1]		= fread_number( fp );
		if( ch->pcdata->code->auction[1] > 10 )
		{
		    ch->pcdata->code->auction[1] -= 10;
		    ch->pcdata->code->auction[0] = 1;
		}
		ch->pcdata->code->chat[1]		= fread_number( fp );
		if( ch->pcdata->code->chat[1] > 10 )
		{
		    ch->pcdata->code->chat[1] -= 10;
		    ch->pcdata->code->chat[0] = 1;
		}
		ch->pcdata->code->music[1]		= fread_number( fp );
		if( ch->pcdata->code->music[1] > 10 )
		{
		    ch->pcdata->code->music[1] -= 10;
		    ch->pcdata->code->music[0] = 1;
		}
		ch->pcdata->code->question[1]		= fread_number( fp );
		if( ch->pcdata->code->question[1] > 10 )
		{
		    ch->pcdata->code->question[1] -= 10;
		    ch->pcdata->code->question[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Colourb" ) )
	    {
		ch->pcdata->code->answer[1]		= fread_number( fp );
		if( ch->pcdata->code->answer[1] > 10 )
		{
		    ch->pcdata->code->answer[1] -= 10;
		    ch->pcdata->code->answer[0] = 1;
		}
		ch->pcdata->code->class_text[1]		= fread_number( fp );
		if( ch->pcdata->code->class_text[1] > 10 )
		{
		    ch->pcdata->code->class_text[1] -= 10;
		    ch->pcdata->code->class_text[0] = 1;
		}
		ch->pcdata->code->class_name[1]		= fread_number( fp );
		if( ch->pcdata->code->class_name[1] > 10 )
		{
		    ch->pcdata->code->class_name[1] -= 10;
		    ch->pcdata->code->class_name[0] = 1;
		}
		ch->pcdata->code->immtalk_text[1]	= fread_number( fp );
		if( ch->pcdata->code->immtalk_text[1] > 10 )
		{
		    ch->pcdata->code->immtalk_text[1] -= 10;
		    ch->pcdata->code->immtalk_text[0] = 1;
		}
		ch->pcdata->code->immtalk_type[1]	= fread_number( fp );
		if( ch->pcdata->code->immtalk_type[1] > 10 )
		{
		    ch->pcdata->code->immtalk_type[1] -= 10;
		    ch->pcdata->code->immtalk_type[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Colourc" ) )
	    {
		ch->pcdata->code->info[1]		= fread_number( fp );
		if( ch->pcdata->code->info[1] > 10 )
		{
		    ch->pcdata->code->info[1] -= 10;
		    ch->pcdata->code->info[0] = 1;
		}
		ch->pcdata->code->tell[1]		= fread_number( fp );
		if( ch->pcdata->code->tell[1] > 10 )
		{
		    ch->pcdata->code->tell[1] -= 10;
		    ch->pcdata->code->tell[0] = 1;
		}
		ch->pcdata->code->reply[1]		= fread_number( fp );
		if( ch->pcdata->code->reply[1] > 10 )
		{
		    ch->pcdata->code->reply[1] -= 10;
		    ch->pcdata->code->reply[0] = 1;
		}
		ch->pcdata->code->gtell_text[1]		= fread_number( fp );
		if( ch->pcdata->code->gtell_text[1] > 10 )
		{
		    ch->pcdata->code->gtell_text[1] -= 10;
		    ch->pcdata->code->gtell_text[0] = 1;
		}
		ch->pcdata->code->gtell_type[1]		= fread_number( fp );
		if( ch->pcdata->code->gtell_type[1] > 10 )
		{
		    ch->pcdata->code->gtell_type[1] -= 10;
		    ch->pcdata->code->gtell_type[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Colourd" ) )
	    {
		ch->pcdata->code->room_title[1]		= fread_number( fp );
		if( ch->pcdata->code->room_title[1] > 10 )
		{
		    ch->pcdata->code->room_title[1] -= 10;
		    ch->pcdata->code->room_title[0] = 1;
		}
		ch->pcdata->code->room_text[1]		= fread_number( fp );
		if( ch->pcdata->code->room_text[1] > 10 )
		{
		    ch->pcdata->code->room_text[1] -= 10;
		    ch->pcdata->code->room_text[0] = 1;
		}
		ch->pcdata->code->room_exits[1]		= fread_number( fp );
		if( ch->pcdata->code->room_exits[1] > 10 )
		{
		    ch->pcdata->code->room_exits[1] -= 10;
		    ch->pcdata->code->room_exits[0] = 1;
		}
		ch->pcdata->code->room_things[1]	= fread_number( fp );
		if( ch->pcdata->code->room_things[1] > 10 )
		{
		    ch->pcdata->code->room_things[1] -= 10;
		    ch->pcdata->code->room_things[0] = 1;
		}
		ch->pcdata->code->prompt[1]		= fread_number( fp );
		if( ch->pcdata->code->prompt[1] > 10 )
		{
		    ch->pcdata->code->prompt[1] -= 10;
		    ch->pcdata->code->prompt[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Coloure" ) )
	    {
		ch->pcdata->code->fight_death[1]	= fread_number( fp );
		if( ch->pcdata->code->fight_death[1] > 10 )
		{
		    ch->pcdata->code->fight_death[1] -= 10;
		    ch->pcdata->code->fight_death[0] = 1;
		}
		ch->pcdata->code->fight_yhit[1]		= fread_number( fp );
		if( ch->pcdata->code->fight_yhit[1] > 10 )
		{
		    ch->pcdata->code->fight_yhit[1] -= 10;
		    ch->pcdata->code->fight_yhit[0] = 1;
		}
		ch->pcdata->code->fight_ohit[1]		= fread_number( fp );
		if( ch->pcdata->code->fight_ohit[1] > 10 )
		{
		    ch->pcdata->code->fight_ohit[1] -= 10;
		    ch->pcdata->code->fight_ohit[0] = 1;
		}
		ch->pcdata->code->fight_thit[1]		= fread_number( fp );
		if( ch->pcdata->code->fight_thit[1] > 10 )
		{
		    ch->pcdata->code->fight_thit[1] -= 10;
		    ch->pcdata->code->fight_thit[0] = 1;
		}
		ch->pcdata->code->fight_skill[1]	= fread_number( fp );	    
		if( ch->pcdata->code->fight_skill[1] > 10 )
		{
		    ch->pcdata->code->fight_skill[1] -= 10;
		    ch->pcdata->code->fight_skill[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Colourf" ) )
	    {
		ch->pcdata->code->bbro[1]		= fread_number( fp );
		if( ch->pcdata->code->bbro[1] > 10 )
		{
		    ch->pcdata->code->bbro[1] -= 10;
		    ch->pcdata->code->bbro[0] = 1;
		}
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'D':
	    KEY( "Damroll",	ch->damroll,		fread_number( fp ) );
	    KEY( "Deaf",	ch->deaf,		fread_number( fp ) );
	    KEY( "Description",	ch->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return;
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Gold",	ch->gold,		fread_number( fp ) );
	    break;

	case 'H':
	    KEY( "Hitroll",	ch->hitroll,		fread_number( fp ) );

	    if ( !str_cmp( word, "HpManaMove" ) )
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'I':
	    KEY( "Immortal",	ch->pcdata->immortal,	fread_flag( fp ) );
	    break;

	case 'L':
	    KEY( "LongDescr",	ch->long_descr,		fread_string( fp ) );
	    if( !str_cmp( word, "Level" ) )
	    {
		/*
		 * Levels are obselete
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'N':
	    if ( !str_cmp( word, "Name" ) )
	    {
		/*
		 * Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Note" ) )
	    {
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'P':
	    KEY( "Pagelen",     ch->pcdata->pagelen,    fread_number( fp ) );
	    KEY( "Password",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Played",	ch->played,		fread_number( fp ) );
	    KEY( "Position",	ch->position,		fread_number( fp ) );
	    KEY( "Practice",	ch->practice,		fread_number( fp ) );
	    KEY( "Prompt",	ch->prompt,		fread_string( fp ) );
	    KEY( "Penalty",	ch->pen,		fread_flag( fp ) );

	    if( !str_cmp( word, "Part" ) )
	    {
		PART_DATA	*pPart;

		pPart		= new_part( );
		pPart->type	= fread_flag( fp );
		pPart->armour	= fread_number( fp );
		pPart->scars	= fread_number( fp );

		LINK( pPart, ch->parts );

		fMatch = TRUE;
		break;
	    }
	    break;

	case 'R':
	    if( !str_cmp( word, "Race" ) )
	    {
		RACE_DATA	*pRace;
		int		number;

		number = fread_number( fp );

		for( pRace = race_list ; pRace ; pRace = pRace->next )
		{
		    if( number == pRace->number )
		    {
			ch->race = pRace;
			fMatch = TRUE;
			break;
		    }
		}
		
		ch->race = race_list;
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( ch->in_room == NULL )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'S':
	    KEY( "SavingThrow",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEY( "ShortDescr",	ch->short_descr,	fread_string( fp ) );
	    KEY( "Security",    ch->pcdata->security,	fread_number( fp ) );	/* OLC */

	    if ( !str_cmp( word, "Skill" ) )
	    {
		int sn;
		int value;

		value = fread_number( fp );
		sn    = skill_lookup( fread_word( fp ) );
		if ( sn < 0 )
		    bug( "Fread_char: unknown skill.", 0 );
		else
		    ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Spool" ) )
	    {
		int i;
		int max;

		max = fread_number( fp );
		for( i = 0 ; i < max ; i++ )
		    ch->last_note[ i ] = fread_number( fp );

		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    if( !str_cmp( word, "Trust" ) )
	    {
		/*
		 * Trusts are obselete
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Title" ) )
	    {
		ch->pcdata->title = fread_string( fp );
		if ( isalpha(ch->pcdata->title[0])
		||   isdigit(ch->pcdata->title[0]) )
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "Wimpy",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wizbit",	ch->wizbit,		fread_number( fp ) );
	    break;
	}

	/* Make sure old chars have this field - Kahn */
	if ( !ch->pcdata->pagelen )
	    ch->pcdata->pagelen = 20;
	if ( !ch->prompt || ch->prompt == '\0' )
	    ch->prompt = "<%h %m %mv> ";

	if ( !fMatch )
	{
	    bug( "Fread_char: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
}



void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;

    if ( obj_free == NULL )
    {
	obj		= alloc_perm( sizeof(*obj) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->name		= str_dup( "" );
    obj->short_descr	= str_dup( "" );
    obj->description	= str_dup( "" );

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Affect" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf		= alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}

		paf->type	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
	    break;

	case 'C':
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':
	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) )
	    {
		EXTRA_DESCR_DATA *ed;

		if ( extra_descr_free == NULL )
		{
		    ed			= alloc_perm( sizeof(*ed) );
		}
		else
		{
		    ed			= extra_descr_free;
		    extra_descr_free	= extra_descr_free->next;
		}

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || !fVnum )
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_string( obj->name        );
		    free_string( obj->description );
		    free_string( obj->short_descr );
		    obj->next = obj_free;
		    obj_free  = obj;
		    return;
		}
		else
		{
		    obj->next	= object_list;
		    object_list	= obj;
		    obj->pIndexData->count++;
		    if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_char( obj, ch );
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    break;

	case 'L':
	    if( !str_cmp( word, "Level" ) )
	    {
		/*
		 * Levels are obselete
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Vnum" ) )	/* OLC */
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( !( obj->pIndexData = get_obj_index( vnum ) ) )
			obj->pIndexData = get_obj_index( OBJ_VNUM_DUMMY );

		fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
}

