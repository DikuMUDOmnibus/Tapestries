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


char *	const	where_name	[] =
{
    "{x<{cused as light{x>     ",
    "{x<{cworn on finger{x>    ",
    "{x<{cworn on finger{x>    ",
    "{x<{cworn around neck{x>  ",
    "{x<{cworn around neck{x>  ",
    "{x<{cworn on body{x>      ",
    "{x<{cworn on head{x>      ",
    "{x<{cworn on legs{x>      ",
    "{x<{cworn on feet{x>      ",
    "{x<{cworn on hands{x>     ",
    "{x<{cworn on arms{x>      ",
    "{x<{cworn as shield{x>    ",
    "{x<{cworn about body{x>   ",
    "{x<{cworn about waist{x>  ",
    "{x<{cworn around wrist{x> ",
    "{x<{cworn around wrist{x> ",
    "{x<{cwielded{x>           ",
    "{x<{cheld{x>              "
};


/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static	char	buf[ MAX_STRING_LENGTH ];
    		char	ack[ MAX_STRING_LENGTH ];

    buf[0] = '\0';
    if( IS_OBJ_STAT( obj, ITEM_INVIS )     )
	strcat( buf, "{b(Invis) "     );
    if( IS_AFFECTED( ch, AFF_DETECT_EVIL )
     && IS_OBJ_STAT( obj, ITEM_EVIL )   )
	strcat( buf, "{r(Red Aura) "  );
    if( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
     && IS_OBJ_STAT( obj, ITEM_MAGIC )  )
	strcat( buf, "{m(Magical) "   );
    if( IS_OBJ_STAT( obj, ITEM_GLOW )      )
	strcat( buf, "{y(Glowing) "   );
    if( IS_OBJ_STAT( obj, ITEM_HUM )       )
	strcat( buf, "(Humming) "   );

    strcat( buf, "{x"   );

    if( fShort )
    {
	if( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if( obj->description )
	    strcat( buf, obj->description );
    }

    if( strlen( buf ) <= 0 )
    {
	strcat( buf, "This object has no description. Please inform the IMP." );
	sprintf( ack, "Format_obj_to_char: vnum %d has no description.",
	    obj->pIndexData->vnum );
	bug( ack, 0 );
    }

    return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "     ", ch );
	    }
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char	buf[ MAX_STRING_LENGTH ];

    buf[0] = '\0';

    if( IS_AFFECTED( victim, AFF_INVISIBLE )	)
	strcat( buf, "(Invis) "      );
    if( IS_AFFECTED( victim, AFF_HIDE )		)
	strcat( buf, "(Hide) "       );
    if( IS_AFFECTED( victim, AFF_CHARM )	)
	strcat( buf, "(Charmed) "    );
    if( IS_AFFECTED( victim, AFF_PASS_DOOR )	)
	strcat( buf, "(Translucent) ");
    if( IS_AFFECTED( victim, AFF_FAERIE_FIRE )	)
	strcat( buf, "(Pink Aura) "  );
    if( IS_EVIL( victim )
     && IS_AFFECTED( ch, AFF_DETECT_EVIL )	)
	strcat( buf, "(Red Aura) "   );
    if( IS_AFFECTED( victim, AFF_SANCTUARY )	)
	strcat( buf, "(White Aura) " );
    if( !IS_NPC( victim )
     && IS_SET( victim->act, PLR_KILLER )	)
	strcat( buf, "(KILLER) "     );
    if( !IS_NPC( victim )
     && IS_SET( victim->act, PLR_THIEF  )	)
	strcat( buf, "(THIEF) "      );

    if( victim->position == POS_STANDING
     && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	return;
    }

    strcat( buf, PERS( victim, ch ) );
    if( !IS_NPC( victim )
     && !IS_SET( ch->act, PLR_BRIEF ) )
	strcat( buf, victim->pcdata->title );

    switch( victim->position )
    {
	case POS_DEAD:
	    strcat( buf, " is DEAD!!" );
	    break;
	case POS_MORTAL:
	    strcat( buf, " is mortally wounded." );
	    break;
	case POS_INCAP:
	    strcat( buf, " is incapacitated." );
	    break;
	case POS_STUNNED:
	    strcat( buf, " is lying here stunned." );
	    break;
	case POS_SLEEPING:
	    strcat( buf, " is sleeping here." );
	    break;
	case POS_RESTING:
	    strcat( buf, " is resting here." );
	    break;
	case POS_STANDING:
	    strcat( buf, " is here." );
	    break;
	case POS_FIGHTING:
	    strcat( buf, " is here, fighting " );
	    if( victim->fighting == NULL )
		strcat( buf, "thin air??" );
	    else if( victim->fighting == ch )
		strcat( buf, "YOU!" );
	    else if( victim->in_room == victim->fighting->in_room )
	    {
		strcat( buf, PERS( victim->fighting, ch ) );
		strcat( buf, "." );
	    }
	    else
		strcat( buf, "somone who left??" );
	    break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );
    return;
}

void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA	*obj;
    char	buf[ MAX_STRING_LENGTH ];
    int		iWear;
    int		just_a_hack;
    int		percent;
    bool	found;

    if( can_see( victim, ch ) )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    if( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS( victim, ch ) );

         if( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
    else if( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
    else if( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
    else if( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
    else if( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
    else if( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
    else                      strcat( buf, " is DYING.\n\r"              );

    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );

    found = FALSE;
    just_a_hack = -1;
    for( iWear = 1 ; iWear < MAX_WEAR ; iWear = iWear << 1 )
    {
	if( ( obj = get_eq_char( victim, iWear ) )
	 && can_see_obj( ch, obj ) )
	{
	    if( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[++just_a_hack], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if( victim != ch
     && !IS_NPC( ch )
     && number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}

void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for( rch = list ; rch ; rch = rch->next_in_room )
    {
	if( rch == ch )
	    continue;

	if( !IS_NPC( rch )
	 && IS_SET( rch->act, PLR_WIZINVIS )
	 && get_trust( ch ) < get_trust( rch ) )
	    continue;

	if( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if( room_is_dark( ch->in_room )
	 && IS_AFFECTED( rch, AFF_INFRARED ) )
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 

bool check_blind( CHAR_DATA *ch )
{
    if( !IS_NPC( ch )
     && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if( IS_AFFECTED( ch, AFF_BLIND ) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}

void do_look( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA	*pexit;
    CHAR_DATA	*victim;
    OBJ_DATA	*obj;
    char	buf[ MAX_STRING_LENGTH ];
    char	arg1[ MAX_INPUT_LENGTH ];
    char	arg2[ MAX_INPUT_LENGTH ];
    char	*pdesc;
    int		door;

    if( !IS_NPC( ch ) && !ch->desc ) 
	return;

    if( ch->position < POS_SLEEPING )
    {
	send_to_char_bw( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if( ch->position == POS_SLEEPING )
    {
	send_to_char_bw( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if( !check_blind( ch ) )
	return;

    if( !IS_NPC( ch )
     && !IS_SET( ch->act, PLR_HOLYLIGHT )
     && room_is_dark( ch->in_room ) )
    {
	send_to_char_bw( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	sprintf( buf, "{s%s{x\n\r", ch->in_room->name );
	send_to_char( buf, ch );

	if( arg1[0] == '\0'
	 || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
	{
	    sprintf( buf, "   {S%s{x", ch->in_room->description );
	    send_to_char( buf, ch );
	}

	if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
	    do_exits( ch, "auto" );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	/* 'look in' */
	if( arg2[0] == '\0' )
	{
	    send_to_char_bw( "Look in what?\n\r", ch );
	    return;
	}

	if( !( obj = get_obj_here( ch, arg2 ) ) )
	{
	    send_to_char_bw( "You do not see that here.\n\r", ch );
	    return;
	}

	if( IS_SET( obj->item_type, ITEM_DRINK_CON ) )
	{
	    if( obj->drink->volume <= 0 )
	    {
		send_to_char_bw( "It is empty.\n\r", ch );
	    }
	    else
	    {
		sprintf( buf, "It's %s full of a %s liquid.\n\r",
		    obj->drink->volume <     obj->drink->limit / 4
		    ? "less than" :
		    obj->drink->volume < 3 * obj->drink->limit / 4
		    ? "about"     : "more than",
		    liq_table[obj->drink->liquid].liq_color );

		send_to_char( buf, ch );
	    }
	}
	else if( IS_SET( obj->item_type, ITEM_CONTAINER ) )
	{
	    if( IS_SET( obj->cont->closed, CONT_CLOSED ) )
	    {
		send_to_char_bw( "It is closed.\n\r", ch );
	    }
	    else
	    {
		act( "$p contains:", ch, obj, NULL, TO_CHAR );
		show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    }
	}
	else if( IS_SET( obj->item_type, ITEM_CORPSE_NPC )
	 || IS_SET( obj->item_type, ITEM_CORPSE_PC ) )
	{
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	}
	else
	{
	    send_to_char_bw( "That is not a container.\n\r", ch );
	}
	
	return;
    }

    if( ( victim = get_char_room( ch, arg1 ) ) )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for( obj = ch->carrying ; obj ; obj = obj->next_content )
    {
	if( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    for( obj = ch->in_room->contents ; obj ; obj = obj->next_content )
    {
	if( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if( pdesc )
    {
	send_to_char( pdesc, ch );
	return;
    }

         if( !str_cmp( arg1, "n" )  || !str_cmp( arg1, "north" ) )      door = 0;
    else if( !str_cmp( arg1, "e" )  || !str_cmp( arg1, "east"  ) )      door = 1;
    else if( !str_cmp( arg1, "s" )  || !str_cmp( arg1, "south" ) )      door = 2;
    else if( !str_cmp( arg1, "w" )  || !str_cmp( arg1, "west"  ) )      door = 3;
    else if( !str_cmp( arg1, "u" )  || !str_cmp( arg1, "up"    ) )      door = 4;
    else if( !str_cmp( arg1, "d" )  || !str_cmp( arg1, "down"  ) )      door = 5;
    else if( !str_cmp( arg1, "ne" ) || !str_cmp( arg1, "northeast"  ) ) door = 6;
    else if( !str_cmp( arg1, "nw" ) || !str_cmp( arg1, "northwest"  ) ) door = 7;
    else if( !str_cmp( arg1, "se" ) || !str_cmp( arg1, "southeast"  ) ) door = 8;
    else if( !str_cmp( arg1, "sw" ) || !str_cmp( arg1, "southwest"  ) ) door = 9;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if( !( pexit = ch->in_room->exit[door] ) )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if( pexit->description && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if( pexit->keyword
     && pexit->keyword[0] != '\0'
     && pexit->keyword[0] != ' ' )
    {
	if( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if( IS_SET( pexit->exit_info, EX_ISDOOR ) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	buf[ MAX_STRING_LENGTH ];
    char	arg[ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if( arg[ 0 ] == '\0' )
    {
	send_to_char_bw( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if( ( obj = get_obj_here( ch, arg ) ) )
    {
	if( IS_SET( obj->item_type, ITEM_DRINK_CON )
	 || IS_SET( obj->item_type, ITEM_CONTAINER ) )
	{
	    send_to_char_bw( "\n\rWhen you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
	else if( IS_SET( obj->item_type, ITEM_CORPSE_NPC )
	 || IS_SET( obj->item_type, ITEM_CORPSE_PC ) )
	{
	    send_to_char_bw( "\n\rIn amongst the stinking remains, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    EXIT_DATA	*pexit;
    char	buf[ MAX_STRING_LENGTH ];
    bool	found;
    bool	fAuto;
    int		door;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "{o[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for( door = 0 ; door <= MAX_DIR ; door++ )
    {
	if( ( pexit = ch->in_room->exit[door] )
	 && pexit->to_room
	 && !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    found = TRUE;
	    if( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if( fAuto )
	strcat( buf, "]{x\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_score( CHAR_DATA *ch, char *argument )
{
    PART_DATA	*pPart = NULL;
    char	temp[ MAX_STRING_LENGTH ];
    char	temp2[ MAX_STRING_LENGTH ];
    char	buf[  MAX_STRING_LENGTH ];
    int		ac_head = 0;
    int		ac_chest = 0;
    int		ac_abdomin = 0;
    int		ac_l_arm = 0;
    int		ac_r_arm = 0;
    int		ac_l_leg = 0;
    int		ac_r_leg = 0;

    if( !IS_NPC( ch ) )
    {
	for( pPart = ch->parts ; pPart ; pPart = pPart->next )
	{
	    switch( pPart->type )
	    {
		case PART_HEAD:
		    ac_head	= pPart->armour;
		    break;
		case PART_CHEST:
		    ac_chest	= pPart->armour;
		    break;
		case PART_ABDOMIN:
		    ac_abdomin	= pPart->armour;
		    break;
		case PART_LEFT_ARM:
		case PART_LFRONT_LEG:
		    ac_l_arm	= pPart->armour;
		    break;
		case PART_RIGHT_ARM:
		case PART_RFRONT_LEG:
		    ac_r_arm	= pPart->armour;
		    break;
		case PART_LEFT_LEG:
		case PART_LREAR_LEG:
		    ac_l_leg	= pPart->armour;
		    break;
		case PART_RIGHT_LEG:
		case PART_RREAR_LEG:
		    ac_r_leg	= pPart->armour;
		    break;
	    }
	}
    }
    else
    {
	ac_head		= ch->armour;
	ac_chest	= ch->armour;
	ac_abdomin	= ch->armour;
	ac_l_arm	= ch->armour;
	ac_r_arm	= ch->armour;
	ac_l_leg	= ch->armour;
	ac_r_leg	= ch->armour;
    }

    sprintf( buf, "You are %s%s\n\r",
	IS_NPC( ch ) ? ch->short_descr : ch->name,
	IS_NPC( ch ) ? " the mobile" : ch->pcdata->title );
    send_to_char( buf, ch );

    sprintf( buf, 
	"@:@===============================================@=========================@:@\n\r" );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|\\| In %3d hours playing, you are %3d years old.  | Armour Points:          |/|\n\r",
	( ch->played + ( int ) ( current_time - ch->logon ) ) / 3600,
	get_age( ch ) );
    send_to_char( buf, ch );

    sprintf( buf, 
	"@:@-----------------------------------------------@          ____           @:@\n\r" );
    send_to_char_bw( buf, ch );

    if( !IS_NPC( ch  ) )
    {
	sprintf( buf,
	    "|\\| Strength:     %3d(%3d) Dexterity:    %3d(%3d) |         / %2d \\          |/|\n\r",
	    ch->pcdata->perm_str,
	    get_curr_str( ch ),
	    ch->pcdata->perm_dex,
	    get_curr_dex( ch ),
	    ac_head );
	send_to_char( buf, ch );

	sprintf( buf,
	    "|/| Intelligence: %3d(%3d) Constitution: %3d(%3d) |        _\\____/_         |\\|\n\r",
	    ch->pcdata->perm_int,
	    get_curr_int( ch ),
	    ch->pcdata->perm_con,
	    get_curr_con( ch ) );
	send_to_char( buf, ch );

	sprintf( buf,
	    "|\\| Wisdom:       %3d(%3d) Prac: %3d              |       / | %2d | \\        |/|\n\r",
	    ch->pcdata->perm_wis,
	    get_curr_wis( ch ),
	    ch->practice,
	    ac_chest );
	send_to_char( buf, ch );
    }
    else
    {
	/* TODO mobiles score */
    }

    sprintf( buf, 
	"@:@-----------------------------------------------@      /  |____|  \\       @:@\n\r" );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|\\| Class: %-10s  | Hit Points:  %5d/%-5d |     /%2d/| %2d |\\%-2d\\      |/|\n\r",
	IS_NPC( ch ) ? "mobile" : ch->class->name,
	ch->hit,  
	ch->max_hit,
	ac_r_arm,
	ac_abdomin,
	ac_l_arm );
    send_to_char_bw( buf, ch );
 
    sprintf( buf,
	"|/| Race:  %-10s  | Mana Points: %5d/%-5d |     |_| |____| |_|      |\\|\n\r",
	ch->race->name,
	ch->mana,  
	ch->max_mana );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|\\| Sex:   %-10s  | Fatigue:     %5d/%-5d |        / |  | \\         |/|\n\r",
	ch->sex == 0 ? "Sexless" : ch->sex == 1 ? "Male" : "Female",
	ch->move,  
	ch->max_move );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|/| Gold:  %-10d  |                          |       /  |  |  \\        |\\|\n\r",
	ch->gold );
    send_to_char_bw( buf, ch );

    sprintf( buf, 
	"@:@--------------------------------@--------------@      | %2d|  |%-2d |       @:@\n\r",
	ac_r_leg,
	ac_l_leg );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|\\| You carry %4d/%-4d items.   |                     |___|  |___|       |/|\n\r",
	ch->carry_number, 
	can_carry_n( ch ) );
    send_to_char_bw( buf, ch );

    sprintf( buf,
	"|/| You carry %7d/%-7d kgs. |                                        |\\|\n\r",
	ch->carry_weight,
	can_carry_w( ch ) );
    send_to_char_bw( buf, ch );

    sprintf( buf, 
	"@:@-------------------------------@@----------------------------------------@:@\n\r" );
    send_to_char_bw( buf, ch );

    switch ( ch->position )
    {
	case POS_DEAD:     
 	    sprintf( temp, "You are DEAD!!"		);
	    break;
	case POS_MORTAL:
	    sprintf( temp, "You are mortally wounded."	);
	    break;
	case POS_INCAP:
	    sprintf( temp, "You are incapacitated."	);
	    break;
	case POS_STUNNED:
	    sprintf( temp, "You are stunned."		);
	    break;
	case POS_SLEEPING:
	    sprintf( temp, "You are sleeping."		);
	    break;
	case POS_RESTING:
	    sprintf( temp, "You are resting."		);
	    break;
	case POS_STANDING:
	    sprintf( temp, "You are standing."		);
	    break;
	case POS_FIGHTING:
	    sprintf( temp, "You are fighting."		);
	    break;
	default:
	    sprintf( temp, "Bug, Contact Admin."	);
	    break;
    }

    sprintf( buf,
	"|\\| %-29s |       Hitroll: %-5d  Damroll: %-5d    |/|\n\r",
	temp,
	GET_HITROLL( ch ),
	GET_DAMROLL( ch ) );
    send_to_char_bw( buf, ch );

    sprintf( temp, "You're" );
    if( IS_NPC( ch ) )
    {
	strcat( temp, " a Mobile" );
    }
    else if( IS_IMMORTAL( ch ) )
    {
	strcat( temp, " Immortal" );
    }
    else
    {
	if( ( ch->pcdata->condition[ COND_DRUNK ]   < 10
	   && ch->pcdata->condition[ COND_THIRST ] > 0
	   && ch->pcdata->condition[ COND_FULL ]   > 0 )
	  || ( get_trust( ch ) > 50 ) )
	{
	    strcat( temp, " Fine" );
	}
	if( ch->pcdata->condition[ COND_FULL ]   <= 0 )
	{
	    strcat( temp, " hungry" );
	}
	if( ch->pcdata->condition[ COND_THIRST ] <= 0 )
	{
	    strcat( temp, " thirsty" );
	}
	if( ch->pcdata->condition[ COND_DRUNK ]  >= 10 )
	{
	    strcat( temp, " drunk" );
	}
    }

    if( ch->alignment >  900 )
	strcpy( temp2, "have an angelic aura.              "	);
    else if( ch->alignment >  700 )
	strcpy( temp2, "are quite saintly.                 "	);
    else if( ch->alignment >  350 )
	strcpy( temp2, "have goodness in your heart.       "	);
    else if( ch->alignment >  100 )
	strcpy( temp2, "are kind.                          "	);
    else if( ch->alignment > -100 )
	strcpy( temp2, "are neutral.                       "	);
    else if( ch->alignment > -350 )
	strcpy( temp2, "are pretty mean.                   "	);
    else if( ch->alignment > -700 )
	strcpy( temp2, "are bad to the bone.               "	);
    else if( ch->alignment > -900 )
	strcpy( temp2, "are totally demonic.               "	);
    else
	strcpy( temp2, "have a satanic aura.               "	);

    sprintf( buf, "|/| %-29s | You %s |\\|\n\r", temp, temp2	);
    send_to_char_bw( buf, ch );

    sprintf( buf, 
	"@:@===============================@=========================================@:@\n\r" );
    send_to_char_bw( buf, ch );

    if( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
	sprintf( buf,
 "|\\|             Holy Light: %-3s, Security: %-2d, Wizi: %-3s                    |/|\n\r",
	    IS_SET( ch->act, PLR_HOLYLIGHT ) ? "on" : "off",
	    ch->pcdata ? ch->pcdata->security : 0,
	    IS_SET( ch->act, PLR_WIZINVIS ) ? "on" : "off" );
	send_to_char_bw( buf, ch );
	sprintf( buf, 
	   "@:@=========================================================================@:@\n\r" );
	send_to_char_bw( buf, ch );
    }
    return;
}

char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "Nature", "Futility", "the Dragon", "the Sun", "the Heat", "the Battle",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[ ];
    char	buf[ MAX_STRING_LENGTH ];
    char	*suf;
    int		day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d and %d of the clock %s,\n\rDay of %s, %d%s the Month of %s.\n\r",

	( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
	time_info.minute,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month] );
    send_to_char( buf, ch );

    sprintf( buf,
	"Tapestrys started up at %s\rThe system time is %s\r",
	str_boot_time,
	(char *) ctime( &current_time ) );

    send_to_char( buf, ch );
    return;
}

void do_weather( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH ];

    static char * const sky_look[ 4 ] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA	*pHelp;
    int		iHash;

    if( argument[ 0 ] == '\0' )
	argument = "summary";

    iHash	= UPPER( argument[ 0 ] ) % 126;

    for( pHelp = help_first[ iHash ] ; pHelp ; pHelp = pHelp->next )
    {
	if( is_name( argument, pHelp->keyword ) )
	{
	    if( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
	    {
		send_to_char_bw( pHelp->keyword, ch );
		send_to_char_bw( "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    return;
	}
    }

    send_to_char_bw( "No help on that word.\n\r", ch );
    return;
}

void do_who( CHAR_DATA *ch, char *argument )
{
    send_to_char_bw( "The who function has been removed pending\n\r", ch );
    send_to_char_bw( "re-writing to whatever race/class format needed.\n\r", ch );
    return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}

void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    int		iWear;
    int		just_a_hack;
    bool	found;

    found = FALSE;

    just_a_hack = -1;
    send_to_char( "You are using:\n\r", ch );
    for( iWear = 1 ; iWear < MAX_WEAR ; iWear = iWear << 1 )
    {
	if( !( obj = get_eq_char( ch, iWear ) ) )
	    continue;

	send_to_char( where_name[++just_a_hack], ch );
	if( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char_bw( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}

void do_compare( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj1;
    OBJ_DATA	*obj2;
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    int		value1;
    int		value2;
    char	*msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if( !( obj1 = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if( arg2[0] == '\0' )
    {
	for( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
	{
	    if( obj2->wear_loc != WEAR_NONE
	     && can_see_obj( ch, obj2 )
	     && obj1->item_type == obj2->item_type
	     && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) != 0 )
		break;
	}

	if( !obj2 )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if( !( obj2 = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if( IS_SET( obj1->item_type, ITEM_ARMOUR )
     && IS_SET( obj2->item_type, ITEM_ARMOUR ) )
    {
	value1 = ( obj1->armour->ac[0] +
		 obj1->armour->ac[1] +
		 obj1->armour->ac[2] +
		 obj1->armour->ac[3] );
	value2 = ( obj2->armour->ac[0] +
		 obj2->armour->ac[1] +
		 obj2->armour->ac[2] +
		 obj2->armour->ac[3] );
    }
    else if( IS_SET( obj1->item_type, ITEM_WEAPON )
     && IS_SET( obj2->item_type, ITEM_WEAPON ) )
    {
	value1 = obj1->weapon->damage[0] * obj1->weapon->damage[1];
	value2 = obj2->weapon->damage[0] * obj2->weapon->damage[1];
    }
    else
    {
	msg = "You can't compare $p and $P.";
    }

    if( !msg )
    {
	     if( value1 == value2 ) msg = "$p and $P look about the same.";
	else if( value1  > value2 ) msg = "$p looks better than $P.";
	else                        msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}

void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}

void do_where( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA	*d;
    CHAR_DATA		*victim;
    char		buf[ MAX_STRING_LENGTH ];
    char		arg[ MAX_INPUT_LENGTH  ];
    bool		found;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for( d = descriptor_list ; d ; d = d->next )
	{
	    if( d->connected == CON_PLAYING
	     && ( victim = d->character )
	     && !IS_NPC( victim )
	     && victim->in_room
	     && victim->in_room->area == ch->in_room->area
	     && can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for( victim = char_list ; victim ; victim = victim->next )
	{
	    if( victim->in_room
	     && victim->in_room->area == ch->in_room->area
	     && !IS_AFFECTED( victim, AFF_HIDE )
	     && !IS_AFFECTED( victim, AFF_SNEAK )
	     && can_see( ch, victim )
	     && is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS( victim, ch ), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}

void do_consider( CHAR_DATA *ch, char *argument )
{
/*
 * Dunno how to do this yet...-Ant

    CHAR_DATA	*victim;
    char	arg[ MAX_INPUT_LENGTH ];
    char	*msg = '\0';
    char	*buf = '\0';
    int		diff;
    int		hpdiff;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if( !IS_NPC( victim ) )
    {
	send_to_char( "The gods do not accept this type of sacrafice.\n\r",
		     ch );
	return;
    }

    diff = victim->level - ch->level;

         if( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if( diff <=  -5 ) msg = "$N is no match for you.";
    else if( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if( diff <=   1 ) msg = "The perfect match!";
    else if( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                   msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );

    hpdiff = ( ch->hit - victim->hit );

    if( ( ( diff >= 0) && ( hpdiff <= 0 ) )
     || ( ( diff <= 0 ) && ( hpdiff >= 0 ) ) )
    {
        send_to_char( "Also,", ch );
    }
    else
    {
        send_to_char( "However,", ch );
    }

    if( hpdiff >= 101 )
        buf = " you are currently much healthier than $E.";
    if( hpdiff <= 100 )
        buf = " you are currently healthier than $E.";
    if( hpdiff <= 50 ) 
        buf = " you are currently slightly healthier than $E.";
    if( hpdiff <= 25 )
        buf = " you are a teensy bit healthier than $E.";
    if( hpdiff <= 0 )
        buf = " $E is a teensy bit healthier than you.";
    if( hpdiff <= -25 )
        buf = " $E is slightly healthier than you.";
    if( hpdiff <= -50 )
        buf = " $E is healthier than you.";
    if( hpdiff <= -100 )
        buf = " $E is much healthier than you.";

    act( buf, ch, NULL, victim, TO_CHAR );

*
*/

    return;
}

void set_title( CHAR_DATA *ch, char *title )
{
    char	buf[ MAX_STRING_LENGTH ];

    if( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if( isalpha( title[0] ) || isdigit( title[0] ) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}

void do_title( CHAR_DATA *ch, char *argument )
{
    if( IS_NPC( ch ) )
	return;

    if( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if( strlen( argument ) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}

void do_description( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH ];

    if( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if( argument[0] == '+' )
	{
	    if( ch->description )
		strcat( buf, ch->description );
	    argument++;
	    while( isspace( *argument ) )
		argument++;
	}

	if( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 100 )
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}

void do_report( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_INPUT_LENGTH ];

    sprintf( buf,
	"You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}

void do_practice( CHAR_DATA *ch, char *argument )
{
/* TODO
 * Later....-Ant

    char	buf[ MAX_STRING_LENGTH  ];
    char	buf1[ MAX_STRING_LENGTH ];
    int sn;

    if( IS_NPC( ch ) )
	return;

    buf1[0] = '\0';

    if( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if( skill_table[sn].name == NULL )
		break;
	    if( ch->level < skill_table[sn].skill_level[ch->class->number] )
		continue;

	    sprintf( buf, "%18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    strcat( buf1, buf );
	    if( ++col % 3 == 0 )
		strcat( buf1, "\n\r" );
	}

	if( col % 3 != 0 )
	    strcat( buf1, "\n\r" );

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	strcat( buf1, buf );
	send_to_char( buf1, ch );
    }
    else
    {
	CHAR_DATA	*mob;
	int		adept;

	if( !IS_AWAKE( ch ) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for( mob = ch->in_room->people ; mob ; mob = mob->next_in_room )
	{
	    if( IS_NPC( mob )
	     && IS_SET( mob->act, ACT_PRACTICE ) )
		break;
	}

	if( !mob )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if( ( sn = skill_lookup( argument ) ) < 0
	 || ( !IS_NPC( ch )
	  && ch->level < skill_table[sn].skill_level[ch->class->number] ) )
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = IS_NPC( ch ) ? 100 : ch->class->skill_adept;

	if( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    if( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
    }

*/

    return;
}

/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH ];
    char	arg[ MAX_INPUT_LENGTH  ];
    int		wimpy;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}

void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *p;
    char *pwnew;
    char cEnd;

    if( IS_NPC( ch ) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while( isspace( *argument ) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	send_to_char( "         <old> can be NULL.\n\r", ch );
	return;
    }

    if( !ch->pcdata->pwd && strcmp( arg1, "NULL" ) )
    {
	send_to_char( "Fixing Null password.\n\r", ch );
    }	
    else if( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwnew = crypt( arg2, ch->name );
    for( p = pwnew; *p != '\0'; p++ )
    {
	if( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}

/*
 * Hash table version, -Ant
 */
void do_socials( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA	*pSocial;
    char	buf[  MAX_STRING_LENGTH ];
    char	*buf1;
    int		col;
    int		cnt;

    buf1	= NULL;
    col		= 0;

    for( cnt = 0 ; cnt < 126 ; cnt++ )
    {
      for( pSocial = social_first[ cnt ] ; pSocial ; pSocial = pSocial->next )
      {
	sprintf( buf, "%-12s", pSocial->name );
	str_cat( &buf1, buf );
	if( ++col % 6 == 0 )
	    str_cat( &buf1, "\n\r" );
      }
    }
 
    if( col % 6 != 0 )
	str_cat( &buf1, "\n\r" );

    page_to_char_bw( buf1, ch );
    free( buf1 );

    return;
}

/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH  ];
    char	*buf1;
    int		cmd;
    int		col;
 
    buf1	= NULL;
    col		= 0;

    for( cmd = 0 ; cmd_table[cmd].name[0] != '\0' ; cmd++ )
    {
        if( !cmd_table[cmd].flag
	 && cmd_table[cmd].show )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    str_cat( &buf1, buf );
	    if( ++col % 6 == 0 )
		str_cat( &buf1, "\n\r" );
	}
    }
 
    if( col % 6 != 0 )
	str_cat( &buf1, "\n\r" );

    page_to_char_bw( buf1, ch );
    free( buf1 );

    return;
}

void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->pen, PEN_SILENCE) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +AUCTION"
	    : " -auction",
	    ch );

	send_to_char( IS_SET( ch->chan, CHAN_CHAT )
	    ? " +CHAT"
	    : " -chat",
	    ch );

	if ( IS_HERO(ch) )
	{
	    send_to_char( IS_SET( ch->chan, CHAN_IMM )
		? " +IMMTALK"
		: " -immtalk",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +MUSIC"
	    : " -music",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUESTION)
	    ? " +QUESTION"
	    : " -question",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +SHOUT"
	    : " -shout",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_YELL)
	    ? " +YELL"
	    : " -yell",
	    ch );

	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	int bit;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
#if 0
	else if ( !str_cmp( arg+1, "hacker"   ) ) bit = CHANNEL_HACKER;
#endif
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT (ch->deaf, bit);
	else
	    SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}

/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOEXIT)
            ? "[+AUTOEXIT ] You automatically see exits.\n\r"
	    : "[-autoexit ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOLOOT)
	    ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
	    : "[-autoloot ] You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOSAC)
	    ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] You receive a telnet GoAhead sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GoAhead sequence.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->pen, PEN_SILENCE )
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( IS_SET( ch->pen, PEN_NOEMOTE )
	    ? ""
	    : "[-emote    ] You can't emote.\n\r"
	    , ch );

	send_to_char( IS_SET( ch->pen, PEN_NOTELL )
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	bool fSet;
	int bit;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	    SET_BIT    (ch->act, bit);
	else
	    REMOVE_BIT (ch->act, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}

void do_wizlist ( CHAR_DATA *ch, char *argument )
{

    do_help ( ch, "wizlist" );
    return;

}

void do_spells ( CHAR_DATA *ch, char *argument )
{
/* TODO
 * Later....-Ant

    char	buf[ MAX_STRING_LENGTH ];
    char	*buf1;
    int		sn;
    int		col;

    if( IS_NPC ( ch ) )
    {  
       send_to_char( "You do not know how to cast spells!\n\r", ch );
       return;
    }

    buf1 = NULL;

    col = 0;
    for( sn = 0 ; sn < MAX_SKILL ; sn++ )
    {
        if( !skill_table[sn].name )
	   break;

	if( ( ch->level < skill_table[sn].skill_level[ch->class->number] )
	 || ( skill_table[sn].skill_level[ch->class->number] > LEVEL_HERO ) )
	   continue;

	sprintf( buf, "%18s %3dpts ",
           skill_table[sn].name, MANA_COST( ch, sn ) );
	str_cat( &buf1, buf );
	if( ++col % 3 == 0 )
	   str_cat( &buf1, "\n\r" );
    }

    if( col % 3 != 0 )
      str_cat( &buf1, "\n\r" );

    page_to_char( buf1, ch );
    free( buf1 );

*/
    return;
}

void do_slist( CHAR_DATA *ch, char *argument )
{
/* TODO
 * Later...-Ant

    char	buf[ MAX_STRING_LENGTH ];
    char	*buf1;
    int		sn;
    int		col;
    int		level;
    bool	pSpell;

    if( IS_NPC( ch ) )
    {  
       send_to_char( "You do not need any stinking spells!\n\r", ch );
       return;
    }

    buf1 = NULL;

    str_cat( &buf1, "ALL Spells available for your class.\n\r\n\r" );
    str_cat( &buf1, "Lv          Spells\n\r\n\r" );

    for( level = 1 ; level < LEVEL_IMMORTAL ; level++ )
    {

      col = 0;
      pSpell = TRUE;

      for( sn = 0 ; sn < MAX_SKILL ; sn++ )
      {
	if( !skill_table[sn].name )
	  break;
	if( skill_table[sn].skill_level[ch->class->number] != level )
	  continue;

	if( pSpell )
	{
	  sprintf( buf, "%2d:", level );
	  str_cat( &buf1, buf );
	  pSpell = FALSE;
	}

	if( ++col % 5 == 0 )
	  str_cat( &buf1, "   " );

	sprintf( buf, "%18s", skill_table[sn].name );
	str_cat( &buf1, buf );

	if( col % 4 == 0 )
	  str_cat ( &buf1, "\n\r" );

      }

      if( col % 4 != 0 )
	str_cat( &buf1, "\n\r" );

    }

    page_to_char( buf1, ch );
    free( buf1 );
*/

    return;

}

/* by passing the conf command - Kahn */

void do_autoexit( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_AUTOEXIT )
     ? do_config( ch, "-autoexit" )
     : do_config( ch, "+autoexit" ) );
}

void do_autoloot ( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_AUTOLOOT )
     ? do_config( ch, "-autoloot" )
     : do_config( ch, "+autoloot" ) );
}

void do_autosac ( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_AUTOSAC )
     ? do_config( ch, "-autosac" )
     : do_config( ch, "+autosac" ) );
}

void do_blank ( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_BLANK )
     ? do_config( ch, "-blank" )
     : do_config( ch, "+blank" ) );
}

void do_brief ( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_BRIEF )
     ? do_config( ch, "-brief" )
     : do_config( ch, "+brief" ) );
}

void do_combine ( CHAR_DATA *ch, char *argument )
{
    ( IS_SET ( ch->act, PLR_COMBINE )
     ? do_config( ch, "-combine" )
     : do_config( ch, "+combine" ) );
}
 
void do_pagelen ( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH ];
    char	arg[ MAX_INPUT_LENGTH  ];
    int		lines;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
	lines = 20;
    else
	lines = atoi( arg );

    if( lines < 0 )
    {
	send_to_char(
		"Negative values for a page pause is not legal.\n\r",
		     ch );
	return;
    }

    ch->pcdata->pagelen = lines;
    if( lines == 0 )
	sprintf( buf, "Pager turned off.\n\r" );
    else
	sprintf( buf, "Page pause set to %d lines.\n\r", lines );
    send_to_char( buf, ch );
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt( CHAR_DATA *ch, char *argument )
{
    char	buf[ MAX_STRING_LENGTH ];

    ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

    if( argument[0] == '\0' )
    {
	( IS_SET ( ch->act, PLR_PROMPT )
	    ? do_config( ch, "-prompt" )
	    : do_config( ch, "+prompt" ) );
	return;
    }

    if( !strcmp( argument, "all" ) )
	strcpy( buf, "<%hhp %mm %vmv> " );
    else
    {
	if( strlen(argument) > 50 )
	    argument[50] = '\0';
	strcpy( buf, argument );
	smash_tilde( buf );
    }


    ch->prompt = str_dup( buf );
    send_to_char( "Ok.\n\r", ch );
    return;
} 

void do_auto( CHAR_DATA *ch, char *argument )
{
    do_config( ch, "" );
    return;
}
