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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down",
    "northeast", "northwest", "southeast", "southwest"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4, 9, 8, 7, 6
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );



void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA		*fch;
    CHAR_DATA		*fch_next;
    ROOM_INDEX_DATA	*in_room;
    ROOM_INDEX_DATA	*to_room;
    EXIT_DATA		*pexit;

    if( door < 0 || door > MAX_DIR )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;
    if( !( pexit   = in_room->exit[door] )
     || !( to_room = pexit->to_room      ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if( IS_SET( pexit->exit_info, EX_CLOSED )
     && !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
    {
	act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if( IS_AFFECTED( ch, AFF_CHARM )
     && ch->master
     && in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if( !IS_NPC( ch ) )
    {
	int move;

	if( in_room->sector_type == SECT_AIR
	 || to_room->sector_type == SECT_AIR )
	{
	    if( !IS_AFFECTED(  ch, AFF_FLYING ) )
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if( in_room->sector_type == SECT_WATER_NOSWIM
	 || to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    OBJ_DATA	*obj;
	    bool	found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    /*
	     * Suggestion for flying above water by Sludge
	     */
	    if( IS_AFFECTED( ch, AFF_FLYING ) )
	        found = TRUE;

	    for( obj = ch->carrying ; obj ; obj = obj->next_content )
	    {
		if( IS_SET( obj->item_type, ITEM_BOAT ) )
		{
		    found = TRUE;
		    break;
		}
	    }

	    if( !found )
	    {
		send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

	if( ch->move < move )
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, 1 );
	ch->move -= move;
    }

    if( !IS_AFFECTED( ch, AFF_SNEAK )
     && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
	act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, to_room );
    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
	act( "$n has arrived.", ch, NULL, NULL, TO_ROOM );

    do_look( ch, "auto" );

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;
	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door );
	}
    }

    mprog_entry_trigger( ch );
    mprog_greet_trigger( ch );
    return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH );
    return;
}

void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST );
    return;
}

void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH );
    return;
}

void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST );
    return;
}

void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP );
    return;
}

void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN );
    return;
}

void do_northeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NE );
    return;
}

void do_northwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NW );
    return;
}

void do_southeast( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SE );
    return;
}

void do_southwest( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SW );
    return;
}




int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for( door = 0 ; door <= MAX_DIR ; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'open object' */
	if( !IS_SET( obj->item_type, ITEM_CONTAINER ) )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSED ) )
	{
	    send_to_char( "It's already open.\n\r",      ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSEABLE ) )
	{
	    send_to_char( "You can't do that.\n\r",      ch );
	    return;
	}
	if( IS_SET( obj->cont->closed, CONT_LOCKED ) )
	{
	    send_to_char( "It's locked.\n\r",            ch );
	    return;
	}

	REMOVE_BIT( obj->cont->closed, CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'close object' */
	if( !IS_SET( obj->item_type, ITEM_CONTAINER ) )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}
	if( IS_SET( obj->cont->closed, CONT_CLOSED ) )
	{
	    send_to_char( "It's already closed.\n\r",    ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSEABLE ) )
	{
	    send_to_char( "You can't do that.\n\r",      ch );
	    return;
	}

	SET_BIT( obj->cont->closed, CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'lock object' */
	if( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSED ) )
	{
	    send_to_char( "It's not closed.\n\r",        ch );
	    return;
	}
	if( obj->cont->key < 0 )
	{
	    send_to_char( "It can't be locked.\n\r",     ch );
	    return;
	}
	if( !has_key( ch, obj->cont->key ) )
	{
	    send_to_char( "You lack the key.\n\r",       ch );
	    return;
	}
	if( IS_SET( obj->cont->closed, CONT_LOCKED ) )
	{
	    send_to_char( "It's already locked.\n\r",    ch );
	    return;
	}

	SET_BIT( obj->cont->closed, CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'unlock object' */
	if( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSED ) )
	{
	    send_to_char( "It's not closed.\n\r",        ch );
	    return;
	}
	if( obj->cont->key < 0 )
	{
	    send_to_char( "It can't be unlocked.\n\r",   ch );
	    return;
	}
	if( !has_key( ch, obj->cont->key ) )
	{
	    send_to_char( "You lack the key.\n\r",       ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_LOCKED ) )
	{
	    send_to_char( "It's already unlocked.\n\r",  ch );
	    return;
	}

	REMOVE_BIT( obj->cont->closed, CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*gch;
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		door;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for( gch = ch->in_room->people ; gch ; gch = gch->next_in_room )
    {
	if( IS_NPC( gch ) && IS_AWAKE( gch ) )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if( !IS_NPC( ch ) && number_percent( ) > ch->pcdata->learned[gsn_pick_lock] )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'pick object' */
	if( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}
	if( !IS_SET( obj->cont->closed, CONT_CLOSED ) )
	{
	    send_to_char( "It's not closed.\n\r",        ch );
	    return;
	}
	if( obj->cont->key < 0 )
	{
	    send_to_char( "It can't be unlocked.\n\r",   ch );
	    return;
	}
	if( !IS_SET(obj->cont->closed, CONT_LOCKED) )
	{
	    send_to_char( "It's already unlocked.\n\r",  ch );
	    return;
	}
	if( IS_SET( obj->cont->closed, CONT_PICKPROOF ) )
	{
	    send_to_char( "You failed.\n\r",             ch );
	    return;
	}

	REMOVE_BIT( obj->cont->closed, CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
    victim->position = POS_STANDING;
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[ gsn_sneak ] )
    {
	af.type      = gsn_sneak;
	af.duration  = ch->pcdata->learned[ gsn_sneak ] / 2;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
	SET_BIT(ch->affected_by, AFF_HIDE);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
  
    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "God has forsaken you.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    lose = (ch->desc != NULL) ? 50 : 100;
	    gain_exp( ch, 0 - lose );
	    sprintf( buf, "You failed!  You lose %d exps.\n\r", lose );
	    send_to_char( buf, ch );
	    return;
	}

	lose = (ch->desc != NULL) ? 100 : 200;
	gain_exp( ch, 0 - lose );
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*mob;
    char	buf[ MAX_STRING_LENGTH ];
    int		hp_gain = 0;
    int		mana_gain = 0;
    sh_int	*pAbility;
    char	*pOutput;
    int		cost;
 
    if( IS_NPC( ch ) )
      return;
 
    /*
     * Check for trainer.
     */
    for( mob = ch->in_room->people ; mob ; mob = mob->next_in_room )
    {
	if( IS_NPC( mob ) && IS_SET( mob->act, ACT_TRAIN ) )
	      break;
    }
 
    if( !mob )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }
 
    if( argument[0] == '\0' )
    {
	sprintf( buf, "You have %d practice sessions.\n\r", ch->practice );
	send_to_char( buf, ch );
	argument = "foo";
    }
 
    cost = 5;
 
    if( !str_cmp( argument, "str" ) )
    {
	if( ch->class->attr_prime == APPLY_STR )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
    }
    else if( !str_cmp( argument, "int" ) )
    {
	if( ch->class->attr_prime == APPLY_INT )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
    }
    else if( !str_cmp( argument, "wis" ) )
    {
	if( ch->class->attr_prime == APPLY_WIS )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
    }
    else if( !str_cmp( argument, "dex" ) )
    {
	if( ch->class->attr_prime == APPLY_DEX )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
    }
    else if( !str_cmp( argument, "con" ) )
    {
	if( ch->class->attr_prime == APPLY_CON )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
    }
    else if( !str_cmp( argument, "hp" ) )
    {
        pAbility = &ch->max_hit;
        pOutput = "number of hit points";
        cost = 1;    /* this is pracs per "train hp" */
        hp_gain = 2; /* this is hp gained per "train hp" */
    }
    else if ( !str_cmp( argument, "mana" ) )
    {
        pAbility = &ch->max_mana;
        pOutput = "amount of mana";
        cost =1;
        mana_gain = 3;
    }
    else
    {
	strcpy( buf, "You can train: hp mana" );
	if( ch->pcdata->perm_str < 18 )
	    strcat( buf, " str" );
	if( ch->pcdata->perm_int < 18 )
	    strcat( buf, " int" );
	if( ch->pcdata->perm_wis < 18 )
	    strcat( buf, " wis" );
	if( ch->pcdata->perm_dex < 18 )
	    strcat( buf, " dex" );
	if( ch->pcdata->perm_con < 18 )
	    strcat( buf, " con" );

	if( buf[ strlen( buf )-1 ] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	}
        return;
    }  
   
      
 
    if( !str_cmp( argument, "hp" ) )
    {
	if( cost > ch->practice )
	{
	    send_to_char( "You don't have enough practices.\n\r", ch );
	    return;
	}

	ch->practice        -= cost;
	*pAbility           += hp_gain;
	act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
	act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	return;
    }
 
    if( !str_cmp( argument, "mana" ) )
    {
 	if( cost > ch->practice )
	{
	    send_to_char( "You don't have enough practices.\n\r", ch );
	    return;
	}
 
	ch->practice        -= cost;
	*pAbility           += mana_gain;
	act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
	act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	return;
    }

    if( *pAbility >= 18 )
    {
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
 
    if( cost > ch->practice )
    {
	send_to_char( "You don't have enough practices.\n\r", ch );
	return;
    }
 
    ch->practice	-= cost;
    *pAbility		+= 1;

    act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );

    return;
}

void do_alias( CHAR_DATA *ch, char *argument )
{
    char	arg[ MAX_INPUT_LENGTH  ];
    char	buf[ MAX_STRING_LENGTH ];
    ALIAS	*alias;
    ALIAS	*visit;

    if( IS_NPC( ch ) )
	return;

    alias = ch->pcdata->aliases;

    if( !*argument )
    {
	if( !alias || !alias->next )
	{ 
	    send_to_char_bw( "No current aliases.\n\r", ch );
	    return;
	}
	send_to_char_bw( "Aliases set  :\n\r", ch );
	for( alias = alias->next ; alias ; alias = alias->next )
	{
	   sprintf( buf, "< %s > = < %s >\n\r", 
		alias->command, 
		alias->string );
	   send_to_char_bw( buf, ch );
	}
	return;
    }
    argument = one_argument( argument, arg );
    if( !str_prefix( arg, "alias" ) )
    {
	send_to_char_bw( "Hmmmm, alias 'alias'? I think not <chuckle>\n\r", ch ); 
    }

    if( !*argument )  /* removes alias */
    {
	if( !alias )
	{
	    send_to_char_bw( "You have no aliases to delete.\n\r", ch );
	    return;
	}

	for( visit = alias ;
	     visit->next && str_cmp( visit->next->command, arg ) ;
	     visit = visit->next );
	alias = visit->next;

	if( alias )
	{
	    free_string( alias->command );
	    free_string( alias->string );
	    visit->next = alias->next;
	    free_mem( alias, sizeof( ALIAS ) );
	    alias = NULL;
	    send_to_char_bw( "Alias removed.\n\r", ch );
	}
	else
	    send_to_char_bw( "No such alias.\n\r", ch );
	return;
    }

    if( alias )
    { 
	for( ; alias->next ; alias = alias->next )
	{ 
	    if( !str_cmp( arg, alias->next->command ) )
	    {
		send_to_char_bw( "That Alias is already set.\n\r", ch );
		return;
	    }
	}
    }
    else
    {
	ch->pcdata->aliases	= ( ALIAS * )alloc_mem( sizeof( ALIAS ) );
	alias			= ch->pcdata->aliases;
	alias->command		= NULL;
	alias->string		= NULL;
	alias->next		= NULL;
    }

    smash_tilde( argument );

    alias->next		= ( ALIAS * )alloc_mem( sizeof( ALIAS ) );
    alias		= alias->next;
    alias->command	= alloc_mem( strlen( arg ) + 1 );
    alias->string	= alloc_mem( strlen( argument ) + 1 );
    strcpy( alias->command, arg );
    strcpy( alias->string, argument );
    alias->next		= NULL;
    return;
}

