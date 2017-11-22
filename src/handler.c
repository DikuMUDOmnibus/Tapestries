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



AFFECT_DATA *		affect_free;



/*
 * Local functions.
 */
void	affect_modify	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );



/*
 * Retrieve a character's trusted level for permission checking.
 * Obselete, now returns 0. -Ant
 */
int get_trust( CHAR_DATA *ch )
{
    return 0;
}



/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) (current_time - ch->logon) ) / 14400;

    /* 12240 assumes 30 second hours, 24 hours a day, 20 day - Kahn */
}



/*
 * Retrieve character's current strength.
 */
int get_curr_str( CHAR_DATA *ch )
{
    int max;

    if( IS_NPC( ch ) )
	return 50;

    if( ch->class->attr_prime == APPLY_STR || IS_IMMORTAL( ch ) )
	max = 100;
    else
	max = ch->race->max_stats[ APPLY_STR ];

    return URANGE( 1, ch->pcdata->perm_str + ch->pcdata->mod_str, max );
}



/*
 * Retrieve character's current intelligence.
 */
int get_curr_int( CHAR_DATA *ch )
{
    int max;

    if( IS_NPC( ch ) )
	return 50;

    if( ch->class->attr_prime == APPLY_INT || IS_IMMORTAL( ch ) )
	max = 100;
    else
	max = ch->race->max_stats[ APPLY_INT ];

    return URANGE( 1, ch->pcdata->perm_int + ch->pcdata->mod_int, max );
}



/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis( CHAR_DATA *ch )
{
    int max;

    if( IS_NPC( ch ) )
	return 50;

    if( ch->class->attr_prime == APPLY_WIS || IS_IMMORTAL( ch ) )
	max = 100;
    else
	max = ch->race->max_stats[ APPLY_WIS ];

    return URANGE( 1, ch->pcdata->perm_wis + ch->pcdata->mod_wis, max );
}



/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex( CHAR_DATA *ch )
{
    int max;

    if( IS_NPC( ch ) )
	return 13;

    if( ch->class->attr_prime == APPLY_DEX || IS_IMMORTAL( ch ) )
	max = 100;
    else
	max = ch->race->max_stats[ APPLY_DEX ];

    return URANGE( 1, ch->pcdata->perm_dex + ch->pcdata->mod_dex, max );
}



/*
 * Retrieve character's current constitution.
 */
int get_curr_con( CHAR_DATA *ch )
{
    int max;

    if( IS_NPC( ch ) )
	return 13;

    if( ch->class->attr_prime == APPLY_CON || IS_IMMORTAL( ch ) )
	max = 100;
    else
	max = ch->race->max_stats[ APPLY_CON ];

    return URANGE( 1, ch->pcdata->perm_con + ch->pcdata->mod_con, max );
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
	return 0;

    return MAX_WEAR + 2 * get_curr_dex( ch ) / 2;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
	return 0;

    return str_app[get_curr_str(ch)].carry;
}



/*
 * See if a string is one of the names of an object.
 * Completly new version to handle the new naming system.
 * -Ant May '96
 * Returned to an older method cos it was going to take too
 * long to do compared to a small change in performance.
 * -Ant May '96
 */
bool is_name( char *str, char *namelist )
{
    char	name[ MAX_INPUT_LENGTH ];
    char	part[ MAX_INPUT_LENGTH ];
    char	*list;
    char	*string;

    if( !namelist || namelist[ 0 ] == '\0' )
    	return FALSE;

    if( str[ 0 ] == '\0' )
	return FALSE;

    string = str;
    for( ; ; )
    {
	str	= one_argument( str, part );

	if( part[ 0 ] == '\0' )
	    return TRUE;

	list = namelist;
	for( ; ; )
	{
	    list	= one_argument( list, name );
	    if( name[ 0 ] == '\0' )
		return FALSE;

	    if( !str_prefix( string, name ) )
		return TRUE;

	    if( !str_prefix( part, name ) )
		break;
	}
    }
}

/*
 * To allow searches that require that we check for a complete
 * word match.
 */
bool is_name_old( char *str, char *namelist )
{
    char	name[ MAX_INPUT_LENGTH ];
    char	part[ MAX_INPUT_LENGTH ];
    char	*list;
    char	*string;

    if( !namelist || namelist[ 0 ] == '\0' )
    	return FALSE;

    if( str[ 0 ] == '\0' )
	return FALSE;

    string = str;
    for( ; ; )
    {
	str	= one_argument( str, part );

	if( part[ 0 ] == '\0' )
	    return TRUE;

	list = namelist;
	for( ; ; )
	{
	    list	= one_argument( list, name );
	    if( name[ 0 ] == '\0' )
		return FALSE;

	    if( !strcmp( string, name ) )
		return TRUE;

	    if( !strcmp( part, name ) )
		break;
	}
    }
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    int mod;

    mod = paf->modifier;

    if ( fAdd )
    {
	SET_BIT( ch->affected_by, paf->bitvector );
    }
    else
    {
	REMOVE_BIT( ch->affected_by, paf->bitvector );
	mod = 0 - mod;
    }

    if ( IS_NPC(ch) )
	return;

    switch ( paf->location )
    {
    default:
	bug( "Affect_modify: unknown location %d.", paf->location );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:           ch->pcdata->mod_str	+= mod;	break;
    case APPLY_DEX:           ch->pcdata->mod_dex	+= mod;	break;
    case APPLY_INT:           ch->pcdata->mod_int	+= mod;	break;
    case APPLY_WIS:           ch->pcdata->mod_wis	+= mod;	break;
    case APPLY_CON:           ch->pcdata->mod_con	+= mod;	break;
    case APPLY_SEX:           ch->sex			+= mod;	break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->max_mana		+= mod;	break;
    case APPLY_HIT:           ch->max_hit		+= mod;	break;
    case APPLY_MOVE:          ch->max_move		+= mod;	break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armour		+= mod;	break;
    case APPLY_HITROLL:       ch->hitroll		+= mod;	break;
    case APPLY_DAMROLL:       ch->damroll		+= mod;	break;
    case APPLY_SAVING_PARA:   ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_ROD:    ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_PETRI:  ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_BREATH: ch->saving_throw		+= mod;	break;
    case APPLY_SAVING_SPELL:  ch->saving_throw		+= mod;	break;
    }

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if( ( wield = get_eq_char( ch, WEAR_WIELD ) )
     && get_obj_weight( wield ) > ( get_curr_str( ch ) / 2 ) )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act( "You drop $p.", ch, wield, NULL, TO_CHAR );
	    act( "$n drops $p.", ch, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, ch->in_room );
	    depth--;
	}
    }

    return;
}



/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    if ( affect_free == NULL )
    {
	paf_new		= alloc_perm( sizeof(*paf_new) );
    }
    else
    {
	paf_new		= affect_free;
	affect_free	= affect_free->next;
    }

    *paf_new		= *paf;
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    if ( ch->affected == NULL )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    affect_modify( ch, paf, FALSE );

    if ( paf == ch->affected )
    {
	ch->affected	= paf->next;
    }
    else
    {
	AFFECT_DATA *prev;

	for ( prev = ch->affected; prev != NULL; prev = prev->next )
	{
	    if ( prev->next == paf )
	    {
		prev->next = paf->next;
		break;
	    }
	}

	if ( prev == NULL )
	{
	    bug( "Affect_remove: cannot find paf.", 0 );
	    return;
	}
    }

    paf->next	= affect_free;
    affect_free	= paf->next;
    return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
	paf_next = paf->next;
	if ( paf->type == sn )
	    affect_remove( ch, paf );
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->type == sn )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool found;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
    {
	if ( paf_old->type == paf->type )
	{
	    paf->duration += paf_old->duration;
	    paf->modifier += paf_old->modifier;
	    affect_remove( ch, paf_old );
	    break;
	}
    }

    affect_to_char( ch, paf );
    return;
}



/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA	*obj;

    if( !ch->in_room )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if( !IS_NPC( ch ) )
	--ch->in_room->area->nplayer;

    if( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
     && obj->item_type == ITEM_LIGHT
     && obj->light->duration != 0
     && ch->in_room->light > 0 )
	ch->in_room->light -= obj->light->intensity;

    if( ch == ch->in_room->people )
    {
	ch->in_room->people = ch->next_in_room;
    }
    else
    {
	CHAR_DATA *prev;

	for( prev = ch->in_room->people ; prev ; prev = prev->next_in_room )
	{
	    if( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }
	}

	if( !prev )
	    bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    return;
}



/*
 * Move a char into a room.
 * Return NULL if a null room target...not.
 *                                    -Lope
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    if( pRoomIndex == NULL )
    {
	bug( "Char_to_room: NULL.", 0 );
	bug( "Char_to_room: Sent to Limbo.", 0 );
	pRoomIndex = get_room_index( ROOM_VNUM_LIMBO );
    }

    ch->in_room		= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    if( !IS_NPC( ch ) )
	++ch->in_room->area->nplayer;

    if( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
     && obj->item_type == ITEM_LIGHT
     && obj->light->duration != 0 )
	ch->in_room->light += obj->light->intensity;

    return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    ch->carry_number	+= get_obj_number( obj );
    ch->carry_weight	+= get_obj_weight( obj );
}



/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
	ch->carrying = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( prev == NULL )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by	 = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= get_obj_number( obj );
    ch->carry_weight	-= get_obj_weight( obj );
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 * Needs further work, -Ant
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    if( !IS_SET( obj->item_type, ITEM_ARMOUR ) )
	return 0;

    switch( iWear )
    {
    case WEAR_BODY:	return 3 * obj->armour->ac[0];
    case WEAR_HEAD:	return 2 * obj->armour->ac[0];
    case WEAR_LEGS:	return 2 * obj->armour->ac[0];
    case WEAR_FEET:	return     obj->armour->ac[0];
    case WEAR_HANDS:	return     obj->armour->ac[0];
    case WEAR_ARMS:	return     obj->armour->ac[0];
    case WEAR_SHIELD:	return     obj->armour->ac[0];
    case WEAR_FINGER_L:	return     obj->armour->ac[0];
    case WEAR_FINGER_R: return     obj->armour->ac[0];
    case WEAR_NECK_1:	return     obj->armour->ac[0];
    case WEAR_NECK_2:	return     obj->armour->ac[0];
    case WEAR_ABOUT:	return 2 * obj->armour->ac[0];
    case WEAR_WAIST:	return     obj->armour->ac[0];
    case WEAR_WRIST_L:	return     obj->armour->ac[0];
    case WEAR_WRIST_R:	return     obj->armour->ac[0];
    case WEAR_HOLD:	return     obj->armour->ac[0];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 * Flag version. -Lope
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, long WearFlag )
{
    OBJ_DATA	*obj;

    for( obj = ch->carrying ; obj ; obj = obj->next_content )
    {
	if( WearFlag == 0 )
	    return NULL;

	if( obj->wear_loc == WearFlag )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 * Flag version. -Lope
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, long WearFlag )
{
    AFFECT_DATA *paf;

    if( get_eq_char( ch, WearFlag ) )
    {
	bug( "Equip_char: already equipped (%ld).", WearFlag );
	return;
    }

    if( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL )	&& IS_EVIL( ch )    )
     || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD )	&& IS_GOOD( ch )    )
     || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL )	&& IS_NEUTRAL( ch ) ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	return;
    }

    ch->armour		-= apply_ac( obj, WearFlag );
    obj->wear_loc	= WearFlag;

    for( paf = obj->pIndexData->affected ; paf ; paf = paf->next )
	affect_modify( ch, paf, TRUE );
    for( paf = obj->affected ; paf ; paf = paf->next )
	affect_modify( ch, paf, TRUE );

    if( obj->item_type == ITEM_LIGHT
     && obj->light->duration != 0
     && ch->in_room )
	ch->in_room->light += obj->light->intensity;

    return;
}



/*
 * Unequip a char with an obj.
 * Flag version. -Lope
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;

    if( obj->wear_loc == WEAR_NONE )
    {
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }

    ch->armour		+= apply_ac( obj, obj->wear_loc );
    obj->wear_loc	= WEAR_NONE;

    for( paf = obj->pIndexData->affected ; paf ; paf = paf->next )
	affect_modify( ch, paf, FALSE );
    for( paf = obj->affected ; paf ; paf = paf->next )
	affect_modify( ch, paf, FALSE );

    if( IS_SET( obj->item_type, ITEM_LIGHT )
     && obj->light->duration != 0
     && ch->in_room
     && ch->in_room->light > 0 )
	ch->in_room->light -= obj->light->intensity;

    return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA	*obj;
    int		nMatch;

    nMatch = 0;
    for( obj = list ; obj ; obj = obj->next_content )
    {
	if( obj->pIndexData == pObjIndex )
	    nMatch++;
    }

    return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA	*in_room;

    if( !obj )
    {
	bug( "Obj_from_room: NULL obj.", 0 );
	return;
    }

    if( !( in_room = obj->in_room ) )
    {
	bug( "obj_from_room: NULL.", 0 );
	return;
    }

    if( obj == in_room->contents )
    {
	in_room->contents		= obj->next_content;
	if( in_room->contents )
	    in_room->contents->prev_content	= NULL;
    }
    else if( !obj->next_content )
    {
	obj->prev_content->next_content	= NULL;
    }
    else
    {
	obj->next_content->prev_content	= obj->prev_content;
	obj->prev_content->next_content	= obj->next_content;
    }

    obj->in_room      = NULL;
    obj->next_content = NULL;
    obj->prev_content = NULL;

    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    obj->prev_content				= NULL;
    obj->next_content				= pRoomIndex->contents;
    if( pRoomIndex->contents )
	pRoomIndex->contents->prev_content	= obj;
    pRoomIndex->contents			= obj;
    obj->in_room				= pRoomIndex;
    obj->carried_by				= NULL;
    obj->in_obj					= NULL;
    return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    obj->prev_content			= NULL;
    obj->next_content			= obj_to->contains;
    if( obj->next_content )
	obj->next_content->prev_content	= obj;
    obj_to->contains			= obj;
    obj->in_obj				= obj_to;
    obj->in_room			= NULL;
    obj->carried_by			= NULL;

    for( ; obj_to ; obj_to = obj_to->in_obj )
    {
	if( obj_to->carried_by )
	{
	    obj_to->carried_by->carry_weight += get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if( !( obj_from = obj->in_obj ) )
    {
	bug( "Obj_from_obj: NULL obj_from.", 0 );
	return;
    }

    if( obj == obj_from->contains )
    {
	obj_from->contains = obj->next_content;
	if( obj_from->contains )
	    obj_from->contains->prev_content	= NULL;
    }
    else if( !obj->next_content )
    {
	obj->prev_content->next_content	= NULL;
    }
    else
    {
	obj->next_content->prev_content	= obj->prev_content;
	obj->prev_content->next_content	= obj->next_content;
    }

    obj->next_content = NULL;
    obj->prev_content = NULL;
    obj->in_obj       = NULL;

    for( ; obj_from ; obj_from = obj_from->in_obj )
    {
	if( obj_from->carried_by )
	{
	    obj_from->carried_by->carry_weight -= get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;

    if( obj->in_room )
	obj_from_room( obj );
    else if( obj->carried_by )
	obj_from_char( obj );
    else if( obj->in_obj )
	obj_from_obj( obj );

    for( obj_content = obj->contains ; obj_content ; obj_content = obj_next )
    {
	obj_next = obj_content->next_content;
	extract_obj( obj->contains );
    }

    UNLINK( obj, object_list );

    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	for( paf = obj->affected ; paf ; paf = paf_next )
	{
	    paf_next    = paf->next;
	    paf->next   = affect_free;
	    affect_free = paf;
	}
    }

    {
	EXTRA_DESCR_DATA *ed;
	EXTRA_DESCR_DATA *ed_next;

	for( ed = obj->extra_descr ; ed ; ed = ed_next )
	{
	    ed_next		= ed->next;
	    free_string( ed->description );
	    free_string( ed->keyword     );
	    extra_descr_free	= ed;
	}
    }

    free_string( obj->name        );
    free_string( obj->description );
    free_string( obj->short_descr );
    --obj->pIndexData->count;

    LINK( obj, obj_free );

    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( ch->in_room == NULL )
    {
	bug( "Extract_char: NULL.", 0 );
	return;
    }

    if ( fPull )
	die_follower( ch );

    stop_fighting( ch, TRUE );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	extract_obj( obj );
    }
    
    char_from_room( ch );

    if ( !fPull )
    {
	char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
	return;
    }

    if ( IS_NPC(ch) )
	--ch->pIndexData->count;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	do_return( ch, "" );

    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch->reply == ch )
	    wch->reply = NULL;
    }

    UNLINK( ch, char_list );

    if ( ch->desc )
	ch->desc->character = NULL;
    free_char( ch );
    return;
}

/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*rch;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    number = number_argument( argument, arg );
    count  = 0;

    if( !str_cmp( arg, "self" ) )
	return ch;

    for( rch = ch->in_room->people ; rch ; rch = rch->next_in_room )
    {
	if( !can_see( ch, rch )
	 || !is_name( arg, rch->name ) )
	    continue;
	if( ++count == number )
	    return rch;
    }

    return NULL;
}




/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*wch;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    if( ( wch = get_char_room( ch, argument ) ) )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;

    for( wch = char_list ; wch ; wch = wch->next )
    {
	if( !can_see( ch, wch )
	 || !is_name_old( arg, wch->name ) )
	    continue;
	if( ++count == number )
	    return wch;
    }

    return NULL;
}



/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA	*obj;

    for( obj = object_list ; obj ; obj = obj->next )
    {
	if( obj->pIndexData == pObjIndex )
	    return obj;
    }

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    OBJ_DATA	*obj;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    number = number_argument( argument, arg );
    count  = 0;

    for( obj = list ; obj ; obj = obj->next_content )
    {
	if( can_see_obj( ch, obj )
	 && is_name( arg, obj->name ) )
	{
	    if( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    number = number_argument( argument, arg );
    count  = 0;

    for( obj = ch->carrying ; obj ; obj = obj->next_content )
    {
	if( obj->wear_loc == WEAR_NONE
	 && can_see_obj( ch, obj )
	 && is_name( arg, obj->name ) )
	{
	    if( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    number = number_argument( argument, arg );
    count  = 0;

    for( obj = ch->carrying ; obj ; obj = obj->next_content )
    {
	if( obj->wear_loc != WEAR_NONE
	 && can_see_obj( ch, obj )
	 && is_name( arg, obj->name ) )
	{
	    if( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );

    if( obj )
	return obj;

    if( ( obj = get_obj_carry( ch, argument ) ) )
	return obj;

    if( ( obj = get_obj_wear( ch, argument ) ) )
	return obj;

    return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[ MAX_INPUT_LENGTH ];
    int		number;
    int		count;

    if( ( obj = get_obj_here( ch, argument ) ) )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;

    for( obj = object_list ; obj ; obj = obj->next )
    {
	if( can_see_obj( ch, obj )
	 && is_name_old( arg, obj->name ) )
	{
	    if( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int amount )
{
    OBJ_DATA	*obj;
    char	buf[MAX_STRING_LENGTH];

    if( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }

    if( amount == 1 )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE ) );
    }
    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ) );
	sprintf( buf, obj->short_descr, amount );
	free_string( obj->short_descr );
	obj->short_descr	= str_dup( buf );
	obj->money->gold	= amount;
    }

    return obj;
}



/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
/*
    int number;

    number = 0;
    if( IS_SET( obj->item_type, ITEM_CONTAINER ) )
      for( obj = obj->contains ; obj ; obj = obj->next_content )
	number += get_obj_number( obj );
    else
	number = 1;
*/
    return 1;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->weight;
    for( obj = obj->contains ; obj ; obj = obj->next_content )
	weight += get_obj_weight( obj );

    return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if( pRoomIndex->light > 0 )
	return FALSE;

    if( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
	return TRUE;

    if( pRoomIndex->sector_type == SECT_INSIDE
     || pRoomIndex->sector_type == SECT_CITY )
	return FALSE;

    if( weather_info.sunlight == SUN_SET
     || weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}



/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA	*rch;
    int		count;

    count = 0;
    for( rch = pRoomIndex->people ; rch ; rch = rch->next_in_room )
	count++;

    if( IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE )
     && count >= 2 )
	return TRUE;

    if( IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY )
     && count >= 1 )
	return TRUE;

    return FALSE;
}



/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if( ch == victim )
	return TRUE;
    
    if( !IS_NPC( victim )
     && IS_SET( victim->act, PLR_WIZINVIS )
     && get_trust( ch ) < get_trust( victim ) )
	return FALSE;

    if( !IS_NPC( ch )
     && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if( IS_AFFECTED( ch, AFF_BLIND ) )
	return FALSE;

    if( room_is_dark( ch->in_room )
     && !IS_AFFECTED( ch, AFF_INFRARED ) )
	return FALSE;

    if( IS_AFFECTED( victim, AFF_INVISIBLE )
     && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    if( IS_AFFECTED( victim, AFF_HIDE )
     && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN )
     && victim->fighting == NULL
     && ( IS_NPC( ch ) ? !IS_NPC( victim ) : IS_NPC( victim ) ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if( IS_SET( obj->item_type, ITEM_POTION ) )
	return TRUE;

    if( IS_AFFECTED( ch, AFF_BLIND ) )
	return FALSE;

    if( IS_SET( obj->item_type, ITEM_LIGHT ) && obj->light->duration != 0 )
	return TRUE;

    if( room_is_dark( ch->in_room ) && !IS_AFFECTED( ch, AFF_INFRARED ) )
	return FALSE;

    if( IS_SET( obj->extra_flags, ITEM_INVIS )
     && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if( !IS_SET( obj->extra_flags, ITEM_NODROP ) )
	return TRUE;

    return FALSE;
}



/*
 * Return ascii name of an item type.
 * bitvector version -Ant
 */
char *item_type_name( OBJ_DATA *obj )
{
    static	char	buf[ 512 ];
    		int	bit;

    bit = obj->item_type;
    buf[0] = '\0';

    if( bit & ITEM_LIGHT	) strcat( buf, " light"			);
    if( bit & ITEM_SCROLL	) strcat( buf, " scroll"		);
    if( bit & ITEM_WAND		) strcat( buf, " wand"			);
    if( bit & ITEM_STAFF	) strcat( buf, " staff"			);
    if( bit & ITEM_WEAPON	) strcat( buf, " weapon"		);
    if( bit & ITEM_TREASURE	) strcat( buf, " treasure"		);
    if( bit & ITEM_ARMOUR	) strcat( buf, " armour"		);
    if( bit & ITEM_POTION	) strcat( buf, " potion"		);
    if( bit & ITEM_FURNITURE	) strcat( buf, " furniture"		);
    if( bit & ITEM_TRASH	) strcat( buf, " trash"			);
    if( bit & ITEM_CONTAINER	) strcat( buf, " container"		);
    if( bit & ITEM_DRINK_CON	) strcat( buf, " drink-container"	);
    if( bit & ITEM_KEY		) strcat( buf, " key"			);
    if( bit & ITEM_FOOD		) strcat( buf, " food"			);
    if( bit & ITEM_MONEY	) strcat( buf, " money"			);
    if( bit & ITEM_BOAT		) strcat( buf, " boat"			);
    if( bit & ITEM_CORPSE_NPC	) strcat( buf, " npc-corpse"		);
    if( bit & ITEM_CORPSE_PC	) strcat( buf, " pc-corpse"		);
    if( bit & ITEM_FOUNTAIN	) strcat( buf, " fountain"		);
    if( bit & ITEM_PILL		) strcat( buf, " pill"			);

    return( buf[0] != '\0' ) ? buf+1 : "none";
}



/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:		return "none";
    case APPLY_STR:		return "strength";
    case APPLY_DEX:		return "dexterity";
    case APPLY_INT:		return "intelligence";
    case APPLY_WIS:		return "wisdom";
    case APPLY_CON:		return "constitution";
    case APPLY_SEX:		return "sex";
    case APPLY_CLASS:		return "class";
    case APPLY_LEVEL:		return "level";
    case APPLY_AGE:		return "age";
    case APPLY_MANA:		return "mana";
    case APPLY_HIT:		return "hp";
    case APPLY_MOVE:		return "moves";
    case APPLY_GOLD:		return "gold";
    case APPLY_EXP:		return "experience";
    case APPLY_AC:		return "armour class";
    case APPLY_HITROLL:		return "hit roll";
    case APPLY_DAMROLL:		return "damage roll";
    case APPLY_SAVING_PARA:	return "save vs paralysis";
    case APPLY_SAVING_ROD:	return "save vs rod";
    case APPLY_SAVING_PETRI:	return "save vs petrification";
    case APPLY_SAVING_BREATH:	return "save vs breath";
    case APPLY_SAVING_SPELL:	return "save vs spell";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
    if ( vector & AFF_HOLD          ) strcat( buf, " hold"          );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector & AFF_FLAMING       ) strcat( buf, " flaming"       );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT       ) strcat( buf, " protect"       );
    if ( vector & AFF_PARALYSIS     ) strcat( buf, " paralysis"     );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

void default_colour( CHAR_DATA *ch )
{
    if( IS_NPC( ch ) )
	return;

    if( !ch->pcdata )
	return;

    ch->pcdata->code->text[1]		= ( WHITE );
    ch->pcdata->code->auction[1]	= ( YELLOW );
    ch->pcdata->code->chat[1]		= ( MAGENTA );
    ch->pcdata->code->music[1]		= ( YELLOW );
    ch->pcdata->code->question[1]	= ( YELLOW );
    ch->pcdata->code->answer[1]		= ( YELLOW );
    ch->pcdata->code->class_text[1]	= ( YELLOW );
    ch->pcdata->code->class_name[1]	= ( GREEN );
    ch->pcdata->code->immtalk_text[1]	= ( CYAN );
    ch->pcdata->code->immtalk_type[1]	= ( YELLOW );
    ch->pcdata->code->info[1]		= ( YELLOW );
    ch->pcdata->code->tell[1]		= ( GREEN );
    ch->pcdata->code->reply[1]		= ( GREEN );
    ch->pcdata->code->gtell_text[1]	= ( GREEN );
    ch->pcdata->code->gtell_type[1]	= ( RED );
    ch->pcdata->code->bbro[1]		= ( GREEN );
    ch->pcdata->code->room_title[1]	= ( CYAN );
    ch->pcdata->code->room_text[1]	= ( CYAN );
    ch->pcdata->code->room_exits[1]	= ( GREEN );
    ch->pcdata->code->room_things[1]	= ( CYAN );
    ch->pcdata->code->prompt[1]		= ( CYAN );
    ch->pcdata->code->fight_death[1]	= ( RED );
    ch->pcdata->code->fight_yhit[1]	= ( GREEN );
    ch->pcdata->code->fight_ohit[1]	= ( YELLOW );
    ch->pcdata->code->fight_thit[1]	= ( RED );
    ch->pcdata->code->fight_skill[1]	= ( WHITE );
    ch->pcdata->code->text[0]		= ( NORMAL );
    ch->pcdata->code->auction[0]	= ( NORMAL );
    ch->pcdata->code->chat[0]		= ( NORMAL );
    ch->pcdata->code->music[0]		= ( NORMAL );
    ch->pcdata->code->question[0]	= ( NORMAL );
    ch->pcdata->code->answer[0]		= ( NORMAL );
    ch->pcdata->code->class_text[0]	= ( NORMAL );
    ch->pcdata->code->class_name[0]	= ( NORMAL );
    ch->pcdata->code->immtalk_text[0]	= ( NORMAL );
    ch->pcdata->code->immtalk_type[0]	= ( NORMAL );
    ch->pcdata->code->info[0]		= ( NORMAL );
    ch->pcdata->code->tell[0]		= ( NORMAL );
    ch->pcdata->code->reply[0]		= ( NORMAL );
    ch->pcdata->code->gtell_text[0]	= ( NORMAL );
    ch->pcdata->code->gtell_type[0]	= ( NORMAL );
    ch->pcdata->code->bbro[0]		= ( NORMAL );
    ch->pcdata->code->room_title[0]	= ( NORMAL );
    ch->pcdata->code->room_text[0]	= ( NORMAL );
    ch->pcdata->code->room_exits[0]	= ( NORMAL );
    ch->pcdata->code->room_things[0]	= ( NORMAL );
    ch->pcdata->code->prompt[0]		= ( NORMAL );
    ch->pcdata->code->fight_death[0]	= ( NORMAL );
    ch->pcdata->code->fight_yhit[0]	= ( NORMAL );
    ch->pcdata->code->fight_ohit[0]	= ( NORMAL );
    ch->pcdata->code->fight_thit[0]	= ( NORMAL );
    ch->pcdata->code->fight_skill[0]	= ( NORMAL );

    return;
}

void all_colour( CHAR_DATA *ch, char *argument )
{
    char	buf[  100 ];
    char	buf2[ 100 ];
    int		colour;
    int		bright;

    if( IS_NPC( ch ) || !ch->pcdata )
	return;

    if( !*argument )
	return;

    if( !str_prefix( argument, "red" ) )
    {
	colour = ( RED );
	bright = NORMAL;
	sprintf( buf2, "Red" );
    }
    if( !str_prefix( argument, "hi-red" ) )
    {
	colour = ( RED );
	bright = BRIGHT;
	sprintf( buf2, "Red" );
    }
    else if( !str_prefix( argument, "green" ) )
    {
	colour = ( GREEN );
	bright = NORMAL;
	sprintf( buf2, "Green" );
    }
    else if( !str_prefix( argument, "hi-green" ) )
    {
	colour = ( GREEN );
	bright = BRIGHT;
	sprintf( buf2, "Green" );
    }
    else if( !str_prefix( argument, "yellow" ) )
    {
	colour = ( YELLOW );
	bright = NORMAL;
	sprintf( buf2, "Yellow" );
    }
    else if( !str_prefix( argument, "hi-yellow" ) )
    {
	colour = ( YELLOW );
	bright = BRIGHT;
	sprintf( buf2, "Yellow" );
    }
    else if( !str_prefix( argument, "blue" ) )
    {
	colour = ( BLUE );
	bright = NORMAL;
	sprintf( buf2, "Blue" );
    }
    else if( !str_prefix( argument, "hi-blue" ) )
    {
	colour = ( BLUE );
	bright = BRIGHT;
	sprintf( buf2, "Blue" );
    }
    else if( !str_prefix( argument, "magenta" ) )
    {
	colour = ( MAGENTA );
	bright = NORMAL;
	sprintf( buf2, "Magenta" );
    }
    else if( !str_prefix( argument, "hi-magenta" ) )
    {
	colour = ( MAGENTA );
	bright = BRIGHT;
	sprintf( buf2, "Magenta" );
    }
    else if( !str_prefix( argument, "cyan" ) )
    {
	colour = ( CYAN );
	bright = NORMAL;
	sprintf( buf2, "Cyan" );
    }
    else if( !str_prefix( argument, "hi-cyan" ) )
    {
	colour = ( CYAN );
	bright = BRIGHT;
	sprintf( buf2, "Cyan" );
    }
    else if( !str_prefix( argument, "white" ) )
    {
	colour = ( WHITE );
	bright = NORMAL;
	sprintf( buf2, "White" );
    }
    else if( !str_prefix( argument, "hi-white" ) )
    {
	colour = ( WHITE );
	bright = BRIGHT;
	sprintf( buf2, "White" );
    }
    else if( !str_prefix( argument, "grey" ) )
    {
	colour = ( BLACK );
	bright = BRIGHT;
	sprintf( buf2, "White" );
    }
    else
    {
	send_to_char_bw( "Unrecognised colour, unchanged.\n\r", ch );
	return;
    }

    ch->pcdata->code->text[1]		= colour;
    ch->pcdata->code->auction[1]	= colour;
    ch->pcdata->code->chat[1]		= colour;
    ch->pcdata->code->music[1]		= colour;
    ch->pcdata->code->question[1]	= colour;
    ch->pcdata->code->answer[1]		= colour;
    ch->pcdata->code->class_text[1]	= colour;
    ch->pcdata->code->class_name[1]	= colour;
    ch->pcdata->code->immtalk_text[1]	= colour;
    ch->pcdata->code->immtalk_type[1]	= colour;
    ch->pcdata->code->info[1]		= colour;
    ch->pcdata->code->tell[1]		= colour;
    ch->pcdata->code->reply[1]		= colour;
    ch->pcdata->code->gtell_text[1]	= colour;
    ch->pcdata->code->gtell_type[1]	= colour;
    ch->pcdata->code->bbro[1]		= colour;
    ch->pcdata->code->room_title[1]	= colour;
    ch->pcdata->code->room_text[1]	= colour;
    ch->pcdata->code->room_exits[1]	= colour;
    ch->pcdata->code->room_things[1]	= colour;
    ch->pcdata->code->prompt[1]		= colour;
    ch->pcdata->code->fight_death[1]	= colour;
    ch->pcdata->code->fight_yhit[1]	= colour;
    ch->pcdata->code->fight_ohit[1]	= colour;
    ch->pcdata->code->fight_thit[1]	= colour;
    ch->pcdata->code->fight_skill[1]	= colour;
    ch->pcdata->code->text[0]		= bright;
    ch->pcdata->code->auction[0]	= bright;
    ch->pcdata->code->chat[0]		= bright;
    ch->pcdata->code->music[0]		= bright;
    ch->pcdata->code->question[0]	= bright;
    ch->pcdata->code->answer[0]		= bright;
    ch->pcdata->code->class_text[0]	= bright;
    ch->pcdata->code->class_name[0]	= bright;
    ch->pcdata->code->immtalk_text[0]	= bright;
    ch->pcdata->code->immtalk_type[0]	= bright;
    ch->pcdata->code->info[0]		= bright;
    ch->pcdata->code->tell[0]		= bright;
    ch->pcdata->code->reply[0]		= bright;
    ch->pcdata->code->gtell_text[0]	= bright;
    ch->pcdata->code->gtell_type[0]	= bright;
    ch->pcdata->code->bbro[0]		= bright;
    ch->pcdata->code->room_title[0]	= bright;
    ch->pcdata->code->room_text[0]	= bright;
    ch->pcdata->code->room_exits[0]	= bright;
    ch->pcdata->code->room_things[0]	= bright;
    ch->pcdata->code->prompt[0]		= bright;
    ch->pcdata->code->fight_death[0]	= bright;
    ch->pcdata->code->fight_yhit[0]	= bright;
    ch->pcdata->code->fight_ohit[0]	= bright;
    ch->pcdata->code->fight_thit[0]	= bright;
    ch->pcdata->code->fight_skill[0]	= bright;

    sprintf( buf, "All Colour settings set to %s.\n\r", buf2 );
    send_to_char_bw( buf, ch );

    return;
}

CLASS_DATA *get_class( int number )
{
    CLASS_DATA	*pClass;

    for( pClass = class_list ; pClass ; pClass = pClass->next )
    {
	if( pClass->number == number )
	{
	    return pClass;
	}
    }

    return NULL;
}

RACE_DATA *get_race( int number )
{
    RACE_DATA	*pRace;

    for( pRace = race_list ; pRace ; pRace = pRace->next )
    {
	if( pRace->number == number )
	{
	    return pRace;
	}
    }

    return NULL;
}

RACE_DATA *race_lookup( char *race )
{
    RACE_DATA	*pRace;

    for( pRace = race_list ; pRace ; pRace = pRace->next )
    {
	if( !strcmp( pRace->name, race ) )
	{
	    return pRace;
	}
    }

    return race_list;
}

RACE_DATA *race_lookup_olc( char *race )
{
    RACE_DATA	*pRace;

    for( pRace = race_list ; pRace ; pRace = pRace->next )
    {
	if( !strcmp( pRace->name, race ) )
	{
	    return pRace;
	}
    }

    return NULL;
}

/*
 * Used to strip the last line from a multi-line string...I hope,
 * Mostly for note function 'note -'  -Ant, Nov '94
 * Updated and Bullet-proofed -Ant, Jan '96
 */
char *strip_last_line( char *buf ) 
{
    char	nbuff[ MAX_STRING_LENGTH ];
    int		i;

    if( !buf )
	return str_dup( "" );

    strcat( buf, "\0" );
    i = strlen( buf );
    

    while( i >= 0 )
    {
        if( buf[ i ] == '\0' )
	    i--;

	if( buf[ i - 2 ] == '\n' )
	    break;

	if( i - 2 <= 0 )
	    return str_dup( "" );

	i--;
    }

    strncpy( nbuff, buf, i );
    nbuff[i] = '\0';

    return str_dup( nbuff );
}