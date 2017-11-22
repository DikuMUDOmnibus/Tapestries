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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );



/*
 * Advancement stuff.
 * Obselete -Ant
 */
void advance_level( CHAR_DATA *ch )
{
    return;
}   



void gain_exp( CHAR_DATA *ch, int gain )
{
    if( IS_NPC( ch ) )
	return;

    ch->exp += gain;

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;

    if( IS_NPC( ch ) )
    {
	gain = ch->max_hit * 2 / 10; /* TODO */
    }
    else
    {
	gain = UMIN( 5, ( ch->max_hit * 2 / 10 ) );

	switch ( ch->position )
	{
	    case POS_SLEEPING: gain += get_curr_con( ch );	break;
	    case POS_RESTING:  gain += get_curr_con( ch ) / 2;	break;
	}

	if( ch->pcdata->condition[ COND_FULL ]   <= 0 )
	    gain /= 3;

	if( ch->pcdata->condition[ COND_THIRST ] <= 0 )
	    gain /= 4;

    }

    if( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 8;

    return UMIN( gain, ch->max_hit - ch->hit );
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = ch->max_mana * 2 / 10;
    }
    else
    {
	gain = UMIN( 5, ( ch->max_mana * 2 / 10 ) /* TODO */ );

	switch( ch->position )
	{
	    case POS_SLEEPING: gain += get_curr_int( ch ) * 2;	break;
	    case POS_RESTING:  gain += get_curr_int( ch );	break;
	}

	if( ch->pcdata->condition[ COND_FULL ]   <= 0 )
	    gain /= 2;

	if( ch->pcdata->condition[ COND_THIRST ] <= 0 )
	    gain /= 3;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 10;

    return UMIN( gain, ch->max_mana - ch->mana );
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC(ch) )
    {
	gain = 50;
    }
    else
    {
	gain = UMAX( 15, ( ch->max_move / 5 ) /* TODO */ );

	switch( ch->position )
	{
	    case POS_SLEEPING: gain += get_curr_dex( ch );	break;
	    case POS_RESTING:  gain += get_curr_dex( ch ) / 2;	break;
	}

	if( ch->pcdata->condition[ COND_FULL ]  <= 0 )
	    gain /= 2;

	if( ch->pcdata->condition[ COND_THIRST ] <= 0 )
	    gain /= 4;
    }

    if( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 8;

    return UMIN( gain, ch->max_move - ch->move );
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if( value == 0 || IS_NPC( ch ) || ch->wizbit )
	return;

    condition				= ch->pcdata->condition[iCond];
    ch->pcdata->condition[iCond]	= URANGE( -48, condition + value, 48 );

    if( ch->pcdata->condition[iCond] <= 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    send_to_char( "You are hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    send_to_char( "You are thirsty.\n\r", ch );
	    break;

	case COND_DRUNK:
	    if( condition == 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 * Or more.... -Ant
 */
void mobile_update( void )
{
    CHAR_DATA	*ch;
    CHAR_DATA	*ch_next;
    EXIT_DATA	*pexit;
    int		door;

    /*
     * Examine all mobs.
     */
    for( ch = char_list ; ch ; ch = ch_next )
    {
	ch_next = ch->next;

	if( !IS_NPC( ch )
	 || !ch->in_room
	 || ch->in_room->area->nplayer == 0
	 || IS_AFFECTED( ch, AFF_CHARM ) )
	    continue;

	/* Examine call for special procedure */
	if( ch->spec_fun != 0 )
	{
	    if( ( *ch->spec_fun ) ( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if( ch->position < POS_STANDING )
	    continue;

	/*
	 * Mob_prog Random Trigger
	 */
	if( ch->in_room->area->nplayer > 0 )
	{
	    MPROG_DATA	*mprg;

	    if( IS_SET( ch->pIndexData->progtypes, RAND_PROG )
	     && ch->pIndexData->mobprogs )
	    {
		for( mprg = ch->pIndexData->mobprogs ; mprg ; mprg = mprg->next )
		{
		    if( ( mprg->type & RAND_PROG )
		     && ( number_percent( ) < atoi( mprg->arglist ) ) )
		    {
			mprog_driver( mprg->comlist, ch, NULL, NULL, NULL );
			break;
		    }
		}
	    }

	    if( ch->position < POS_STANDING )
	        continue;
	}

	/*
	 * Scavenge
	 */
	if( IS_SET( ch->act, ACT_SCAVENGER )
	 && ch->in_room->contents
	 && number_bits( 2 ) == 0 )
	{
	    OBJ_DATA	*obj;
	    OBJ_DATA	*obj_best;
	    int		max;

	    max         = 1;
	    obj_best    = 0;
	    for( obj = ch->in_room->contents ; obj ; obj = obj->next_content )
	    {
		if( CAN_WEAR( obj, ITEM_TAKE ) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/*
	 * Wander
	 */
	if( !IS_SET( ch->act, ACT_SENTINEL )
	 && ( door = number_bits( 5 ) ) <= 5
	 && ( pexit = ch->in_room->exit[door] )
	 && pexit->to_room
	 && !IS_SET(pexit->exit_info, EX_CLOSED)
	 && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB )
	 && ( !IS_SET( ch->act, ACT_STAY_AREA )
	  || pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door );
	                                       /* If ch changes position due
						  to it's or someother mob's
						  movement via MOBProgs,
						  continue - Kahn */
	    if( ch->position < POS_STANDING )
	        continue;
	}

	/*
	 * Flee
	 */
	if( ch->hit < ( ch->max_hit / 2 )
	 && ( door = number_bits( 3 ) ) <= 5
	 && ( pexit = ch->in_room->exit[door] )
	 && pexit->to_room
	 && !IS_SET( pexit->exit_info, EX_CLOSED )
	 && !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) )
	{
	    CHAR_DATA	*rch;
	    bool	found;

	    found = FALSE;
	    for( rch = pexit->to_room->people ; rch ; rch = rch->next_in_room )
	    {
		if( !IS_NPC( rch ) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if( !found )
		move_char( ch, door );
	}
    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    DESCRIPTOR_DATA	*d;
    char		buf[ MAX_STRING_LENGTH ];
    int			diff;

    buf[0] = '\0';

    if( ( time_info.minute += 15 ) == 60 )
    {
	time_info.hour++;
	time_info.minute = 0;

	switch( time_info.hour )
	{
	    case  5:
		weather_info.sunlight = SUN_LIGHT;
		strcat( buf, "The day has begun.\n\r" );
		break;

	    case  6:
		weather_info.sunlight = SUN_RISE;
		strcat( buf, "The sun rises in the east.\n\r" );
		break;

	    case 19:
		weather_info.sunlight = SUN_SET;
		strcat( buf, "The sun slowly disappears in the west.\n\r" );
		break;

	    case 20:
		weather_info.sunlight = SUN_DARK;
		strcat( buf, "The night has begun.\n\r" );
		break;

	    case 24:
		time_info.hour = 0;
		time_info.day++;
		break;
	}
    }

    if( time_info.day >= 31 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if( time_info.month >= 12 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if( time_info.month >= 3 && time_info.month <= 9 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice( 1, 4 ) + dice( 2, 6 ) - dice( 2, 6 );
    weather_info.change    = UMAX( weather_info.change, -12 );
    weather_info.change    = UMIN( weather_info.change,  12 );

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX( weather_info.mmhg,  960 );
    weather_info.mmhg  = UMIN( weather_info.mmhg, 1040 );

    switch( weather_info.sky )
    {
	default: 
	    bug( "Weather_update: bad sky %d.", weather_info.sky );
	    weather_info.sky = SKY_CLOUDLESS;
	    break;

	case SKY_CLOUDLESS:
	    if( weather_info.mmhg <  990
	     || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	    {
		strcat( buf, "The sky is getting cloudy.\n\r" );
		weather_info.sky = SKY_CLOUDY;
	    }
	    break;

	case SKY_CLOUDY:
	    if( weather_info.mmhg <  970
	     || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	    {
		strcat( buf, "It starts to rain.\n\r" );
		weather_info.sky = SKY_RAINING;
	    }

	    if( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	    {
		strcat( buf, "The clouds disappear.\n\r" );
		weather_info.sky = SKY_CLOUDLESS;
	    }
	    break;

	case SKY_RAINING:
	    if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	    {
		strcat( buf, "Lightning flashes in the sky.\n\r" );
		weather_info.sky = SKY_LIGHTNING;
	    }

	    if( weather_info.mmhg > 1030
	     || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	    {
		strcat( buf, "The rain stopped.\n\r" );
		weather_info.sky = SKY_CLOUDY;
	    }
	    break;

	case SKY_LIGHTNING:
	    if( weather_info.mmhg > 1010
	     || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	    {
		strcat( buf, "The lightning has stopped.\n\r" );
		weather_info.sky = SKY_RAINING;
		break;
	    }
	    break;
    }

    if( buf[0] != '\0' )
    {
	for( d = descriptor_list ; d ; d = d->next )
	{
	    if( d->connected == CON_PLAYING
	     && IS_OUTSIDE( d->character )
	     && IS_AWAKE( d->character ) )
		send_to_char( buf, d->character );
	}
    }
    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA	*ch;
    CHAR_DATA	*ch_next;
    CHAR_DATA	*ch_save;
    CHAR_DATA	*ch_quit;
    time_t	save_time;

    save_time	= current_time;
    ch_save	= NULL;
    ch_quit	= NULL;

    for( ch = char_list ; ch ; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

	/*
	 * Find dude with oldest save time.
	 */
	if( !IS_NPC( ch )
	 && ( !ch->desc || ch->desc->connected <= CON_PLAYING )
	 && ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if( ch->position >= POS_STUNNED )
	{
	    if( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain( ch );

	    if( ch->mana < ch->max_mana )
		ch->mana += mana_gain( ch );

	    if( ch->move < ch->max_move )
		ch->move += move_gain( ch );
	}

	if( ch->position == POS_STUNNED )
	    update_pos( ch );

	if( !IS_NPC( ch ) )
	{
	    OBJ_DATA	*obj;
	    char	buf[ MAX_STRING_LENGTH ];

	    if( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	     && IS_SET( obj->item_type, ITEM_LIGHT )
	     && obj->light->duration > 0 )
	    {
		if( --obj->light->duration == 0 && ch->in_room )
		{
		    ch->in_room->light -= obj->light->intensity;
		    if( ch->in_room->light < 0 )
			ch->in_room->light = 0;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );

		    REMOVE_BIT( obj->item_type, ITEM_LIGHT );
		    unequip_char( ch, obj );

		    if( obj->item_type == 0 )
		    {
			obj->extra_flags = 0;
		        SET_BIT( obj->item_type, ITEM_TRASH );
			sprintf( buf, "%s (dead)", obj->short_descr );
			free_string( obj->short_descr );
			obj->short_descr = str_dup( buf ); 
		    }
		}
	    }

	    if( ++ch->timer >= 12 && !ch->wizbit )
	    {
		if( !ch->was_in_room && ch->in_room )
		{
		    ch->was_in_room = ch->in_room;
		    if( ch->fighting )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }
	    if( ch->timer > 30 && !ch->wizbit )
		ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -1 );
	    gain_condition( ch, COND_FULL,   -1 );
	    gain_condition( ch, COND_THIRST, -1 );
	}

	for( paf = ch->affected ; paf ; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if( paf->duration > 0 )
		paf->duration--;
	    else if( paf->duration < 0 )
		;
	    else
	    {
		if( !paf_next
		 || paf_next->type != paf->type
		 || paf_next->duration > 0 )
		{
		    if( paf->type > 0 && skill_table[paf->type].msg_off )
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
		affect_remove( ch, paf );
	    }
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if( IS_AFFECTED( ch, AFF_POISON ) )
	{
	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    damage( ch, ch, 2, gsn_poison );
	}
	else if( ch->position == POS_INCAP )
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED );
	}
	else if( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 2, TYPE_UNDEFINED );
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if( ch_save || ch_quit )
    {
	for( ch = char_list ; ch ; ch = ch_next )
	{
	    ch_next = ch->next;
	    if( ch == ch_save )
		save_char_obj( ch );
	    if( ch == ch_quit )
		do_quit( ch, "" );
	}
    }
    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA	*obj;
    OBJ_DATA	*obj_next;

    for( obj = object_list ; obj ; obj = obj_next )
    {
	CHAR_DATA	*rch;
	char		*message;

	obj_next = obj->next;

	if( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	if( IS_SET( obj->item_type, ITEM_FOUNTAIN ) )
	{
	    message = "$p dries up.";
	}
	else if( IS_SET( obj->item_type, ITEM_CORPSE_NPC ) )
	{
	    if( obj->condition == 100 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some decayed remains" );
		free_string( obj->description );
		obj->description = str_dup( "A decaying corpse rots here." );
		obj->condition = 50;
		obj->timer = number_range( 2, 4 );
		continue;
	    }
	    else if( obj->condition == 50 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some old dusty bones" );
		free_string( obj->description );
		obj->description = str_dup( "A pile of bones lies here." );
		obj->condition = 10;
		obj->timer = number_range( 4, 8 );
		continue;
	    }
	    else if( obj->condition == 10 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some dusty remains" );
		free_string( obj->description );
		obj->description = str_dup( "Some dusty remains lie here." );
		obj->condition = 0;
		obj->timer = number_range( 6, 12 );
		continue;
	    }
	    message = "$p decay into dust.";
	}
	else if( IS_SET( obj->item_type, ITEM_CORPSE_PC ) )
	{
	    if( obj->condition == 100 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some decayed remains" );
		free_string( obj->description );
		obj->description = str_dup( "A decaying corpse rots here." );
		obj->condition = 50;
		obj->timer = number_range( 10, 20 );
		continue;
	    }
	    else if( obj->condition == 50 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some old dusty bones" );
		free_string( obj->description );
		obj->description = str_dup( "A pile of bones lies here." );
		obj->condition = 10;
		obj->timer = number_range( 10, 20 );
		continue;
	    }
	    else if( obj->condition == 10 )
	    {
		free_string( obj->short_descr );
		obj->short_descr = str_dup( "some dusty remains" );
		free_string( obj->description );
		obj->description = str_dup( "Some dusty remains lie here." );
		obj->condition = 0;
		obj->timer = number_range( 10, 20 );
		continue;
	    }
	    message = "$p decay into dust.";
	}
	else if( IS_SET( obj->item_type, ITEM_FOOD ) )
	{
	    message = "$p decomposes.";
	}
	else
	{
	    message = "$p vanishes.";
	}

	if( obj->carried_by )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if( obj->in_room
	 && ( rch = obj->in_room->people ) )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

	if( IS_SET( obj->item_type, ITEM_CORPSE_PC )
	 && obj->contains ) /* save the contents */
	{
	    OBJ_DATA *t_obj;
	    OBJ_DATA *next_obj;
 
	    for( t_obj = obj->contains ; t_obj ; t_obj = next_obj )
	    {
		next_obj = t_obj->next_content;
		obj_from_obj( t_obj );
 
		if( obj->in_obj ) /* in another object */
		    obj_to_obj( t_obj, obj->in_obj );
 
		else if( obj->carried_by )  /* carried */
		    obj_to_char( t_obj, obj->carried_by );
 
		else if( !obj->in_room )  /* destroy it */
		    extract_obj( t_obj );
 
		else /* to a room */
		    obj_to_room( t_obj, obj->in_room );
	    }
	}
	extract_obj( obj );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 * because we don't want the mob to just attack the first PC
 * who leads the party into the room.
 *
 * -- Furey
 *
 * This really used to suck, examining ALL mobiles? I don't think so
 * hows about we just examine active players instead?
 *
 * -- Ant
 */
void aggr_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *ch;
    CHAR_DATA       *mch;
    CHAR_DATA       *vch;
    CHAR_DATA       *victim;

    for( d = descriptor_list ; d ; d = d->next )
    {
	ch = d->character;

	if( d->connected != CON_PLAYING /* TODO ignore immortals */
	 || IS_SET( ch->in_room->room_flags, ROOM_SAFE )
	 || !ch->in_room )
	    continue;

	for( mch = ch->in_room->people ; mch ; mch = mch->next_in_room )
	{
	    int	count;

	    if( IS_NPC( mch )
	     && mch->mpactnum > 0
	     && mch->in_room->area->nplayer > 0 )
	    {
		MPROG_ACT_LIST	*tmp_act;
		MPROG_ACT_LIST	*tmp2_act;

		for( tmp_act = mch->mpact ; tmp_act ; tmp_act = tmp_act->next )
		{
		    mprog_wordlist_check( 
			tmp_act->buf,
			mch, 
			tmp_act->ch,
			tmp_act->obj, 
			tmp_act->vo, 
			ACT_PROG );
		    free_string( tmp_act->buf );
		}
		for( tmp_act = mch->mpact ; tmp_act ; tmp_act = tmp2_act )
		{
		    tmp2_act = tmp_act->next;
		    free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
		}
		mch->mpactnum = 0;
		mch->mpact    = NULL;
	    }

	    if( !IS_NPC( mch )
	     || !IS_SET( mch->act, ACT_AGGRESSIVE )
	     || mch->fighting
	     || IS_AFFECTED( mch, AFF_CHARM )
	     || !IS_AWAKE( mch )
	     || ( IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )
	     || !can_see( mch, ch )
	     || ( number_bits( 1 ) == 0 ) )
		continue;

	    count  = 0;
	    victim = NULL;
	    for( vch = mch->in_room->people ; vch ; vch = vch->next_in_room )
	    {
	        if( IS_NPC( vch ) ) /* TODO ignore immortals and check for
                                          power of player */
		    continue;

		if( ( !IS_SET( mch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) )
		 && can_see( mch, vch ) )
		{
		    if( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if( !victim )
	        continue;

	    multi_hit( mch, victim, TYPE_UNDEFINED );

	}
    }
    return;
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static	int	pulse_area;
    static	int	pulse_mobile;
    static	int	pulse_violence;
    static	int	pulse_point;
    static	int	pulse_db_dump;	/* OLC 1.1b	*/
    static	int	pulse_notes;	/* -Ant		*/

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
	obj_update	( );
    }

    /*
     * OLC 1.1b
     */
    if( --pulse_db_dump  <= 0 )
    {
	pulse_db_dump	= PULSE_DB_DUMP;
	do_asave( NULL, "" );
    }

    if( --pulse_notes  <= 0 )
    {
	NOTE_DATA	*pnote = NULL;
	NOTE_DATA	*pnote_next = NULL;

	pulse_notes	= PULSE_NOTES;
	for( pnote = note_list ; pnote ; pnote = pnote_next )
	{
	    pnote_next = pnote->next;

	    if( pnote->date_stamp < ( current_time - NOTE_TIMEOUT ) )
	    {
		note_delete( pnote );
	    }
	}	
    }

    aggr_update( );
    return;
}


